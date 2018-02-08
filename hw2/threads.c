#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef _REENTRANT
   #define _REENTRANT
#endif

#define N 3

//global variables
int A[N][N];
int B[N][N];

typedef struct {int row; int col; int a_val; int b_val;} write_val;

void print_matrix(int matrix[N][N]) {
  for (int row = 0; row < N ; row++) {
    for (int col = 0; col < N; col++) {
      printf("%d ", matrix[row][col]);
    }
    printf("\n");
  }
}

void* write_matrix(void* arg) {
  write_val* wv = arg;
  A[wv->row][wv->col] = wv->a_val;
  B[wv->row][wv->col] = wv->b_val;
}

int main(int argc, char** argv) {
  pthread_t write_threads[N][N];
  write_val args[N][N];
  unsigned int seed = time(NULL);
  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      srand(seed);
      int a = rand_r(&seed) % 9 + 2;
      srand(seed);
      int b = rand_r(&seed) % 9 + 2;
      write_val temp = args[row][col];
      temp.a_val = a;
      temp.b_val = b;
      temp.row = row;
      temp.col = col;
      printf("create %d\n", pthread_create(&write_threads[row][col], 0, write_matrix, &temp));
    }
  }

  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      printf("thread returns %d\n", pthread_join(write_threads[row][col], 0));
    }
  }

  print_matrix(A);
  print_matrix(B);
  
}
