#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#ifndef _REENTRANT
   #define _REENTRANT
#endif

#define N 5

 //global variables
// matrices
int A[N][N];
int B[N][N];
int C[N][N];
// row sum 
int MAX_ROW_SUM = 0; 
// mutex
pthread_mutex_t lock;

// structs
typedef struct {int row; int col; int a_val; int b_val;} write_val;
typedef struct {int ar; int bc;} calc_val;

// function for print matrix
void print_matrix(int matrix[N][N]) {
  for (int row = 0; row < N ; row++) {
    for (int col = 0; col < N; col++) {
      printf("%d ", matrix[row][col]);
    }
    printf("\n");
  }
}

// function for writing one value into one spot in matrix
void* write_m(void* arg) {
  write_val* wv = arg;
  A[wv->row][wv->col] = wv->a_val;
  B[wv->row][wv->col] = wv->b_val;
}

// function for computing  matrix multiplication value for one spot in C 
void* compute(void* arg) {
  calc_val* cv = arg;
  int result = 0; 
  for(int i = 0; i < N; i++) {
    result += A[cv->ar][i] * B[i][cv->bc];
  }
  C[cv->ar][cv->bc] = result;
} 

// function for write values into matrices
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
      if(pthread_create(&write_threads[row][col], 0, write_m, &args[row][col]) != 0) {
	printf("Initialize thread %d failed!\n", write_threads[row][col]);
      }
    }
  }
  
  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      if(pthread_join(write_threads[row][col], 0) != 0) {
	printf("Thread %d failed to join! \n", write_threads[row][col]);
      }
    }
  }
}

// function for computing matrix C
void compute_C() {
  pthread_t calc_threads[N][N];
  calc_val args[N][N];
  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      calc_val* cv = &args[row][col];
      cv->ar = row;
      cv->bc = col;
      if(pthread_create(&calc_threads[row][col], 0, compute, &args[row][col]) != 0) {
	printf("Initialize thread %d failed! \n", calc_threads[row][col]);
      }
    }
  }

  for(int row = 0; row < N; row++) {
    for(int col = 0; col < N; col++) {
      if(pthread_join(calc_threads[row][col], 0) != 0) {
	printf("Thread %d failed to join!\n", calc_threads[row][col]);
      }
    }
  }
}

// matrix for computing and comparing one row sum value
void* row_sum (void* arg) {
  int* row = arg;
  int temp_sum = 0;
  for(int col = 0; col < N; col++) {
    temp_sum += C[*row][col];
  }
  pthread_mutex_lock(&lock);
  if(temp_sum > MAX_ROW_SUM) {
    MAX_ROW_SUM = temp_sum;
  }
  pthread_mutex_unlock(&lock);
}

void find_max_row() {
  pthread_t max_threads[N];
  int rows[N];
  for(int i = 0; i < N; i++) {
    rows[i] = i;
    if(pthread_create(&max_threads[i], 0, row_sum, &rows[i]) != 0) {
      printf("Initialize thread %d failed!\n", max_threads[i]);
      exit(0);
    }
  }
  for(int i = 0 ; i < N; i++) {
    if(pthread_join(max_threads[i], 0) != 0) {
      printf("Thread %d is failed to join! \n", max_threads[i]);
    }
  }
}

int main(int argc, char** argv) {
 if(pthread_mutex_init(&lock, NULL) != 0) {
    printf("initialize mutex failed!\n");
    exit(0);
  }
  write_matrices();
  compute_C();
  find_max_row();
  printf("Matrix A: \n");
  print_matrix(A);
  printf("\nMatrix B: \n");
  print_matrix(B);
  printf("\nMatrix C: \n");
  print_matrix(C);
  printf("\nMax row sum is: %d\n", MAX_ROW_SUM);
  pthread_mutex_destroy(&lock);
}
