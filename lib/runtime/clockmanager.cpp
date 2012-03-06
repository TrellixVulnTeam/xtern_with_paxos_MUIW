#include "tern/runtime/clockmanager.h"
#include "tern/options.h"
#include "tern/space.h"
#include <cstdio>
#include <cstdlib>
#include "assert.h"

namespace tern {

#define ts2ll(x) ((uint64_t) (x).tv_sec * factor + (x).tv_nsec)
//  at least wait for 1 miliseconds
#define WAITING_THRESHOLD 1000000
//  should not delay more than a second
#define WARNING_THRESHOLD 1000000000

ClockManager::ClockManager(uint64_t init_clock)
{
  epochLength = (uint64_t) options::epoch_length * 1000;
  tickCount = 0;
  clock = init_clock;

  //  not sure where I am, system space? user space?
  bool isSys = Space::isSys();
  if (!isSys)
    Space::enterSys();

  struct timespec now; 
  clock_gettime(CLOCK_REALTIME, &now);
  init_rclock = ts2ll(now);
  next_rclock = init_rclock + epochLength;

  if (!isSys)
    Space::exitSys();
}

void ClockManager::tick()
{
  ++tickCount;
  clock += epochLength;
  if (!options::epoch_mode || options::runtime_type != "RR")
    return;

  //  delay to next_rclock
  assert(Space::isSys());
  
  struct timespec now; 
  clock_gettime(CLOCK_REALTIME, &now);
  uint64_t rclock = ts2ll(now);
  if (rclock > next_rclock + WARNING_THRESHOLD)
    fprintf(stderr, "WARNING: delayed more than %d macroseconds\n", 
      WARNING_THRESHOLD / 1000);

  struct timespec next;
  getClock(next, next_rclock);
  if (rclock < next_rclock - WAITING_THRESHOLD)
    ::nanosleep(&next, NULL);
}

void ClockManager::getClock(time_t &t, uint64_t clock)
{
  t = clock / factor; 
}

void ClockManager::getClock(timespec &t, uint64_t clock)
{
  t.tv_sec = clock / factor;
  t.tv_nsec = clock % factor;
}

void ClockManager::getClock(timeval &t, uint64_t clock)
{
  t.tv_sec = clock / factor;
  t.tv_usec = (clock % factor) / 1000;
}

}
