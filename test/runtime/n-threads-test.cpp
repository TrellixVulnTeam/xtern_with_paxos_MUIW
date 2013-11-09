// RUN: %srcroot/test/runtime/run-scheduler-test.py %s -gxx "%gxx" -llvmgcc "%llvmgcc" -projbindir "%projbindir" -ternruntime "%ternruntime" -ternannotlib "%ternannotlib"  -ternbcruntime "%ternbcruntime"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

#define N (1000)

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void *arg) {
  char buf[64];
  int tid = (intptr_t)arg;

  for(unsigned i=0;i<100;++i)
    sched_yield();

  //sprintf(buf, "%03d RUNS\n", tid);
  sprintf(buf, "RUNS\n");
  pthread_mutex_lock(&m);
  printf("%s", buf);
  pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[], char *env[]) {
  int ret;
  pthread_t th[N];

  for(unsigned i=0; i<N; ++i) {
    ret  = pthread_create(&th[i], NULL, thread_func, (void*)i);
    assert(!ret && "pthread_create() failed!");
  }

  for(unsigned i=0; i<N; ++i)
    pthread_join(th[i], NULL);
}

// CHECK indicates expected output checked by FileCheck; auto-generated by appending -gen to the RUN command above.
// CHECK:      RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
// CHECK-NEXT: RUNS
