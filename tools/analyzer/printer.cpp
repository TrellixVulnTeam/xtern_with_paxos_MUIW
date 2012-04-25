#include <deque>
#include <cstring>
#include <set>
#include <map>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <cstdio>

#include "tern/syncfuncs.h"
#include "analyzer.h"

/*
  topology printer
 */

#define OUF stdout

using namespace std;
using namespace tern;

extern bool po_signature;
extern bool bdb_spec;

void print_item(const char *st, int len = 20)
{
  int l = strlen(st);
  if (l > len) { st += (l - len); l = len; }
  fprintf(OUF, "%s", string((len - l) / 2, ' ').c_str());
  fprintf(OUF, "%s", st);
  fprintf(OUF, "%s", string((len - l + 1) / 2, ' ').c_str());
}

bool is_sched_op(unsigned op)
{
  switch (op) {
  case (syncfunc::pthread_create):
  case (syncfunc::pthread_join):
  case (syncfunc::pthread_cancel):
  case (syncfunc::pthread_exit):
  case (syncfunc::pthread_mutex_init):
  case (syncfunc::pthread_mutex_lock):
  case (syncfunc::pthread_mutex_unlock):
  case (syncfunc::pthread_mutex_trylock):
  case (syncfunc::pthread_mutex_timedlock):
  case (syncfunc::pthread_mutex_destroy):
  // case (syncfunc::pthread_cond_init):
  case (syncfunc::pthread_cond_wait):
  case (syncfunc::pthread_cond_timedwait):
  case (syncfunc::pthread_cond_broadcast):
  case (syncfunc::pthread_cond_signal):
  // case (syncfunc::pthread_cond_destroy):
  case (syncfunc::pthread_barrier_init):
  case (syncfunc::pthread_barrier_wait):
  case (syncfunc::pthread_barrier_destroy):
  // case (syncfunc::sem_init):
  case (syncfunc::sem_post):
  case (syncfunc::sem_wait):
  case (syncfunc::sem_trywait):
  case (syncfunc::sem_timedwait):
    return true;
  default:
    return false;
  }
}

#define hashit(x) sig = sig * 1000003 + (sig >> 40) + (x)

void print_signature(vector<op_t> &ops, vector<vector<int> > &hb_arrow)
{
  uint64_t sig = 0;

  int n = ops.size();

#if 0
  if (bdb_spec)
  {
    //  for Berkeley DB, 
    fprintf(OUF, "signature: %lx\n", sig);
  }
#endif

#if 0
  vector<op_t> *data = &ops;
  vector<op_t> nops;

  if (po_signature) 
  {
    vector<int> remap;
    remap.resize(n + 1);
    for (int i = 0; i < n; ++i)
    {
      remap[i] = -1;
      if (!hb_arrow[i].size()) continue;
      remap[i] = 1;
      for (unsigned p = 0; p < hb_arrow[i].size(); ++p)
        remap[hb_arrow[i][p]] = 1;
    }

  }
#endif

  for (int i = 0; i < n; ++i)
  {
    if (!is_sched_op(ops[i].rec.op)) continue;

    hashit(ops[i].tid * 100);
    fprintf(OUF, "%d: pid = %d, tid = %d, turn = %d\n", 
      i, ops[i].pid, ops[i].rec.tid, ops[i].rec.turn);
    continue;
    hashit(ops[i].rec.op * 100000);
    vector<int> &prefix = hb_arrow[i];
    for (vector<int>::iterator it = prefix.begin(); it != prefix.end(); ++it)
      hashit(*it);
  }
  fprintf(OUF, "signature: %lx\n", sig);
}

void print_trace(vector<op_t> &ops, vector<vector<int> > &hb_arrow)
{
  int n = ops.size();
  vector<int> clock;
  clock.resize(n);
  int m = 0;
  typedef pair<int, int> tid_pair;
  typedef map<tid_pair, deque<int> > tops_t;
  tops_t thread_ops;
  for (int i = 0; i < n; ++i)
  {
    clock[i] = 0;
    vector<int> &prefix = hb_arrow[i];
    for (vector<int>::iterator it = prefix.begin(); it != prefix.end(); ++it)
      clock[i] = max(clock[i], clock[*it] + 1); 
    m = max(m, clock[i]);
    thread_ops[make_pair(ops[i].pid, ops[i].tid)].push_back(i);
  }

  char buffer[256];
  fprintf(OUF, "   ");
  for (tops_t::iterator it = thread_ops.begin(); it != thread_ops.end(); ++it)
  {
    fprintf(OUF, " ");
    sprintf(buffer, "(%d, %d)", it->first.first, it->first.second);
    print_item(buffer);
  }
  fprintf(OUF, "\n");

  for (int i = 0; i < m; ++i)
  {
    fprintf(OUF, "%03d:", i);
    for (tops_t::iterator it = thread_ops.begin(); it != thread_ops.end(); ++it)
    {
      fprintf(OUF, " ");
      if (it->second.size() > 0 && clock[it->second.front()] == i)
      {
        print_item(syncfunc::getName(ops[it->second.front()].rec.op));
        it->second.pop_front(); 
      } else
        print_item("");
    }
    fprintf(OUF, "\n");
  }
}