// RUN: %llvmgcc %s -g -O0 -c -o %t1.ll -S
// RUN: %projbindir/tern-instr < %t1.ll -o %t2
// RUN: llc -o %t2.s %t2-record.bc
// RUN: g++ -g -o %t2 %t2.s -L %projlibdir -lcommonruntime -lrecruntime -lpthread
// RUN: ./%t2
// XFAIL: *

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#define N (2)

void bar() {
  pthread_exit(NULL);
}

void* thread_func(void *arg) {
  bar();
}

int main() {
  int ret;
  pthread_t th[N];

  for(unsigned i=0; i<N; ++i) {
    ret  = pthread_create(&th[i], NULL, thread_func, (void*)i);
    assert(!ret && "pthread_create() failed!");
  }

  for(unsigned i=0; i<N; ++i)
    pthread_join(th[i], NULL);

  // this would deadlock
  pthread_exit(NULL);
  return 0;
}