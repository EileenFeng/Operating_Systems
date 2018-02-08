#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef _REENTRANT
  #define _REENTRANT
#endif

unsigned int seed;

typedef struct {char* c; int times;} passin;

void* func(void* p) {
  char* arg = p;
  int times = rand_r(&seed)%9 + 2;
  printf("%s to be print %d times\n", arg, times);
  for(int i = 0; i < times; i++) {
    printf("%s ", arg);
  }
  printf("\n");
} 

int main(int argc, char** argv) {
  int num_threads = argc-1;
  pthread_t threads[num_threads];
  for(int i = 0; i < num_threads; i++) {
    seed = time(NULL);
    printf("create: %d\n", pthread_create(&threads[i], 0, func, argv[i+1]));
  }
  for(int i = 0; i < num_threads; i++) {
    printf("Threads %d returns %d\n", &threads[i], pthread_join(threads[i], 0));
  } 
}
