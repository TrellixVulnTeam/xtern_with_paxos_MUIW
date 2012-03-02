// RUN: %srcroot/test/runtime/run-scheduler-test.py %s -gxx "%gxx" -llvmgcc "%llvmgcc" -projbindir "%projbindir" -ternruntime "%ternruntime"  -ternbcruntime "%ternbcruntime" -nondet

// NOTE: theoretically fork() should not be mixed with threads.  However,
// in practice some real applications such as Apache do this, so we have
// to make it work ...

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

pthread_mutex_t mutex;

void *myfunc(void *arg)
{
  pthread_mutex_lock(&mutex);
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *fork_thread(void *arg)
{
  pthread_t t;

  pthread_create(&t, NULL, myfunc, NULL);

  if (fork() == 0) { // child process
    // reinitialize mutex since it's state may be undefined
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    exit(0);
  }
  pthread_mutex_lock(&mutex);
  pthread_mutex_unlock(&mutex);

  wait(NULL);

  return NULL;
}

int main()
{
  pthread_t t;
  pthread_mutex_init(&mutex, NULL);

  pthread_create(&t, NULL, fork_thread, NULL);

  pthread_mutex_lock(&mutex);
  pthread_mutex_unlock(&mutex);

  pthread_join(t, NULL);

  printf("test done\n");
  return 0;
}
// CHECK indicates expected output checked by FileCheck; auto-generated by appending -gen to the RUN command above.
// CHECK:      test done
