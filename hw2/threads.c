#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef _REENTRANT
   #define _REENTRANT
#endif

#define N 5

//global variables
int A[N][N];
int B[N][N];
int C[N][N];

typedef struct {int row; int col; int a_val; int b_val;} write_val;
typedef struct {int ar; int bc;} calc_val;
void print_matrix(int matrix[N][N]) {
  for (int row = 0; row < N ; row++) {
    for (int col = 0; col < N; col++) {
      printf("%d ", matrix[row][col]);
    }
    printf("\n");
  }
}

void* write_m(void* arg) {
  write_val* wv = arg;
  A[wv->row][wv->col] = wv->a_val;
  B[wv->row][wv->col] = wv->b_val;
}

void* compute(void* arg) {
  calc_val* cv = arg;
  int result = 0; 
  for(int i = 0; i < N; i++) {
    result += A[cv->ar][i] * B[i][cv->bc];
  }
  C[cv->ar][cv->bc] = result;
} 

//void write_matrices(unsigned int seed, pthread_t write_threads[N][N], write_val args[N][N]){
void write_matrices() {
  pthread_t write_threads[N][N];
  unsigned int seed = time(NULL);
  write_val args[N][N];
  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      srand(seed);
      int a = rand_r(&seed) % 9 + 2;
      srand(seed);
      int b = rand_r(&seed) % 9 + 2;
      write_val* temp = &args[row][col];
      temp->a_val = a;
      temp->b_val = b;
      temp->row = row;
      temp->col = col;
      pthread_create(&write_threads[row][col], 0, write_m, &args[row][col]);
    }
  }

  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      pthread_join(write_threads[row][col], 0);
    }
  }
}

void compute_C() {
  pthread_t calc_threads[N][N];
  calc_val args[N][N];
  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      calc_val* cv = &args[row][col];
      cv->ar = row;
      cv->bc = col;
      pthread_create(&calc_threads[row][col], 0, compute, &args[row][col]);
    }
  }

  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      pthread_join(calc_threads[row][col], 0);
    }
  }
}

int main(int argc, char** argv) {
  /* pthread_t write_threads[N][N]; 
   write_val args[N][N]; 
   unsigned int seed = time(NULL); 
   write_matrices(seed, write_threads, args);*/
  write_matrices();
  compute_C();
  print_matrix(A);
  printf("\n");
  print_matrix(B);
  printf("\n");
  print_matrix(C);
  
}
