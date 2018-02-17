#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <error.h>
#include <time.h>
 
#define TRUE 1
#define FALSE 0
#define BABOONNUM 100
#define CLIMBTIME 4
// global variables
enum baboon_direction{west = 10, east = 20};
/*sem_t srope; // controls access to the rope
sem_t wbc; // controls access to the counter for westward moving baboons
sem_t ebc; // controls access to the counter for eastward moving baboons
sem_t w_wait;  // controls access to the counter for the number of  going west waiting baboons
sem_t e_wait; // controls access to the counter for the number of going east waiting baboons
unsigned int value = 1;
*/
void get_on_rope() {
  sleep(1);
}

void print_west(int* west_waiting, int* west_baboon) {
  for(int i = 0; i < *west_waiting; i++) {
    printf(" Ww ");
  }
  printf(" | ");
  for(int i = 0; i < *west_baboon; i++) {
    printf(" WG ");
  }
}

void print_east(int* east_waiting, int* east_baboon) {
  for(int i = 0; i < *east_baboon; i++) {
    printf(" EG ");
  }
  printf(" | ");
  for(int i = 0; i < *east_waiting; i++) {
    printf(" Ew ");
  }
}

void climb_rope(int* west_baboon, int* east_baboon, int* west_waiting, int* east_waiting) {
  int time_spent = 0;
  while(time_spent < CLIMBTIME) {
    print_west(west_waiting, west_baboon);
    print_east(east_waiting, east_baboon);
    printf("\n");
    printf("climbing go east are: %d, go west are: %d, wait west are %d, wait east are %d\n", *east_baboon, *west_baboon, *west_waiting, *east_waiting);
    sleep(1);
    time_spent ++;
  }
}

void go_west(int* west_waiting, int* west_baboon, int* east_waiting, int* east_baboon) {
  sem_wait(&wbc);
  *west_baboon += 1;
  if(*west_baboon == 1) {
    get_on_rope();
    sem_wait(&srope);
    printf("srope 0 still run? west %d\n", sem_getvalue(&srope, &value));
    sem_post(&wbc);
  
  sem_wait(&w_wait);
    *west_waiting -= 1;
    sem_post(&w_wait);
  }
  printf("west waiting is %d, west going is %d srope is %d\n", *west_waiting, *west_baboon, sem_getvalue(&srope, &value));
  climb_rope(west_baboon, east_baboon, west_waiting, east_waiting);
  sem_wait(&wbc);
  *west_baboon -= 1;
  if(*west_baboon == 0) {
    sem_post(&srope);
  }
  sem_post(&wbc); 
}

void go_east(int* east_waiting, int* east_baboon, int* west_waiting, int* west_baboon) {
  sem_wait(&ebc);
  *east_baboon += 1;
  if(*east_baboon == 1) {
    get_on_rope();
    sem_wait(&srope);   
  
    printf("srope 0 still run? east %d\n", sem_getvalue(&srope, &value));
    sem_wait(&e_wait);
    *east_waiting -= 1;
    sem_post(&e_wait);
  }
  sem_post(&ebc);
  printf("east waiting is %d, east going is %d   srope is %d \n", *east_waiting, *east_baboon, sem_getvalue(&srope, &value));
  climb_rope(west_baboon, east_baboon, west_waiting, east_waiting);
  sem_wait(&ebc);
  *east_baboon -= 1;
  if(*east_baboon == 0) {
    sem_post(&srope);
  }
  sem_post(&ebc);
  
}

//void child_baboon(pid_t pid, int* wstwait, int* wbaboon, int* estwait, int* ebaboon) {
void child_baboon(pid_t pid, int*wstwait, int* wbaboon, int* estwait, int* ebaboon) {
  srand(pid);
  int randNum = rand() % 20;
  if(randNum < west) {
    sem_wait(&w_wait);
    *wstwait += 1;
    sem_post(&w_wait);
    go_west(wstwait, wbaboon, estwait, ebaboon);
  } else if (randNum < east) {
    sem_wait(&e_wait);
    *estwait += 1;
    sem_post(&e_wait);
    go_east(estwait, ebaboon, wstwait, wbaboon);
  }
}

int main (int argc, char** argv) {
  int* shm = mmap(NULL, sizeof(int)*5, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , -1, 0);
  int* west_baboon = shm;
  *west_baboon = 0;
  int* east_baboon = shm + 1;
  *east_baboon = 0;
  int* west_waiting = shm + 2;
  *west_waiting = 0;
  int* east_waiting = shm + 3;
  *east_waiting = 0;
  int* baboons = shm + 4;
  *baboons = 0;
  /// initializing semaphores
 if(sem_init(&srope, 1, value) < 0) {
    perror("Failed to initialize sempahore 'srope'\n");
    return 0;
  }
  if(sem_init(&wbc, 1, 1) < 0) {
    perror("Failed to initialize semaphore 'wbc'\n");
    return 0;
  }
  if(sem_init(&ebc, 1, 1) <0) {
    perror("Failed to initialize semaphore 'ebc'\n");
    return 0;
  }
  if(sem_init(&w_wait, 1, 1) < 0) {
    perror("Failed to initialize semaphore 'w_wait'\n");
    return 0;
  }
  if(sem_init(&e_wait, 1, 1) < 0) {
    perror("Failed to initialize semaphore 'e_wait'\n");
    return 0;
  }
  while(*baboons < 3) {
    *baboons += 1;
    printf("baboons ccreated are %d\n", *baboons);
    pid_t pid = fork();
    if(pid == 0) {
      pid_t childPid = getpid();
      child_baboon(childPid, west_waiting, west_baboon, east_waiting, east_baboon);
    } 
  }
  int stat;
  if(wait(&stat) > 0) {
    printf("process terminated\n");
  }
}

