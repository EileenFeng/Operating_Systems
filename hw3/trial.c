#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define BABOONNUM 100
#define CLIMBTIME 4
// global variables
enum baboon_direction{west = 10, east = 20};
struct data {
  int west_baboon;
  int east_baboon;
  int west_waiting;
  int east_waiting;
  int baboons;
  int value;
};
sem_t* srope; // controls access to the rope
sem_t* wbc; // controls access to the counter for westward moving baboons
sem_t* ebc; // controls access to the counter for eastward moving baboons
sem_t* w_wait;  // controls access to the counter for the number of  going west waiting baboons
sem_t* e_wait; // controls access to the counter for the number of going east waiting baboons
unsigned int value = 1;

void get_on_rope() {
  sleep(1);
}

void print_west(int west_waiting, int west_baboon) {
  for(int i = 0; i < west_waiting; i++) {
    printf(" Ww ");
  }
  printf(" | ");
  for(int i = 0; i < west_baboon; i++) {
    printf(" WG ");
  }
}

void print_east(int east_waiting, int east_baboon) {
  for(int i = 0; i < east_baboon; i++) {
    printf(" EG ");
  }
  printf(" | ");
  for(int i = 0; i < east_waiting; i++) {
    printf(" Ew ");
  }
}

void climb_rope(struct data* bdata) {
  int time_spent = 0;
  while(time_spent < CLIMBTIME) {
    print_west(bdata->west_waiting, bdata->west_baboon);
    print_east(bdata->east_waiting, bdata->east_baboon);
    printf("\n");
    //printf("climbing go east are: %d, go west are: %d, wait west are %d, wait east are %d\n", bdata->east_baboon, bdata->west_baboon, bdata->west_waiting, bdata->east_waiting);
    sleep(1);
    time_spent ++;
  }
}

void go_west(struct data* bdata) {
  pid_t pid = getpid();
  sem_wait(wbc);
  bdata->west_baboon += 1;
  if(bdata->west_baboon == 1) {
    get_on_rope();
    printf("Process %d in west is ready to wait \n", pid);
    sem_wait(srope);
    printf("####Process %d in west is in the critical zone\n", pid);
    sem_post(wbc);
    sem_wait(w_wait);
    bdata->west_waiting -= 1;
    sem_post(w_wait);
    
  }
  climb_rope(bdata);
  sem_wait(wbc);
  bdata->west_baboon -= 1;
  if(bdata->west_baboon == 0) {
    sem_post(srope);
    printf("Process %d in west is left ROPE \n", pid);
  
  }
  sem_post(wbc); 
}

void go_east(struct data* bdata) {
  pid_t pid = getpid();
  sem_wait(ebc);
  bdata->east_baboon += 1;
  printf("in go_east\n");
  if(bdata->east_baboon == 1) {
    get_on_rope();
    printf("Process %d in east is ready to wait\n", pid);
    sem_wait(srope);   
    printf("$$$$$$$$ Process %d in east is in the critical zone\n", pid); 
    sem_post(ebc);
    sem_wait(e_wait);
    bdata->east_waiting -= 1;
    sem_post(e_wait);
  }
  climb_rope(bdata);
  sem_wait(ebc);
  bdata->east_baboon -= 1;
  if(bdata->east_baboon == 0) {
    sem_post(srope);
    printf("Process %d in east is left ROPE \n", pid);
  }
  sem_post(ebc);
  
}

//void child_baboon(pid_t pid, int* wstwait, int* wbaboon, int* estwait, int* ebaboon) {
void child_baboon(pid_t pid, struct data* bdata) {
  printf("%d starts\n", pid);
  srand(pid);
  int randNum = rand() % 20;
  if(randNum < west) {
    sem_wait(w_wait);
    bdata->west_waiting += 1;
    sem_post(w_wait);
    go_west(bdata);
  } else if (randNum < east) {
    sem_wait(e_wait);
    bdata->east_waiting += 1;
    sem_post(e_wait);
    go_east(bdata);
  }
}

int main (int argc, char** argv) {
  struct data baboon_data = {.west_baboon = 0, .east_baboon = 0, .west_waiting = 0, .east_waiting = 0, .baboons = 0, .value = 1};
  struct data* shm = mmap(NULL, sizeof(baboon_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , -1, 0);
  unsigned int svalue = 1;
  srope = sem_open("s_rope", O_CREAT, 0600, 1);
  wbc = sem_open("west_b", O_CREAT, 0600, 1);
  ebc = sem_open("eastb", O_CREAT, 0600, 1);
  w_wait = sem_open("west_wait", O_CREAT, 0600, 1);
  e_wait = sem_open("east_wait", O_CREAT, 0600, 1);

  int value = 11;
  sem_getvalue(srope, &value);
  printf("in main srop value  %d \n", value);
  sem_getvalue(wbc, &value);
  printf("in main wbc value  %d \n", value);
  sem_getvalue(ebc, &value);
  printf("in main ebc value  %d \n", value);
  sem_getvalue(w_wait, &value);
  printf("in main w_wait value  %d \n", value);
  sem_getvalue(e_wait, &value);
  printf("in main e_wait value  %d \n", value);
  
  //  printf("in main values; %d    %d \n", sem_getvalue(srope, &value), value);
  
  //pid_t pid;
  while(shm->baboons < 5) {
    pid_t pid;
    shm->baboons += 1;
    srand(time(NULL));
    int randTime = rand() % 5 + 1;
    sleep(randTime);
    pid = fork();
    if(pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    else if(pid == 0) {
      pid_t childPid = getpid();
      //      printf("here forked %d\n", childPid);
      child_baboon(childPid, shm);
      //printf("child %d finished\n", childPid);
      exit(0);
    
    }//else if (pid > 0) {  
  }
  int stat;
      wait(&stat);
      sem_unlink("s_rope");
      sem_close(srope);
      sem_unlink("westbb");
      sem_close(wbc);
      sem_unlink("eastb");
      sem_close(ebc);
      sem_unlink("west_waiting");
      sem_close(w_wait);
      sem_unlink("east_waiting");
      sem_close(e_wait);
    
    
  
}
