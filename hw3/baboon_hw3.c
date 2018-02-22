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
#define BABOONNUM 30
#define CLIMBTIME 4
#define GET_ON_ROPE_TIME 1
#define ARRIVAL 6
// globals
enum baboon_direction{west = 10, east = 20};
struct data {
  int on_rope_num;
  int west_waiting;
  int east_waiting;
  int baboons;
  int side; //used to keep track of sides
};
sem_t* rope; // controls access to the rope
sem_t* westw; // controls access to the number of baboons waiting at west side
sem_t* eastw; // controls access to the number of baboons waiting at east side
sem_t* rope_data; // controls access to the data of the number and direction of baboons on rope
sem_t* turn; // controls turns of baboon

void climb_rope(struct data *bdata, int num, int side){
  int time = 0;
  while(time < CLIMBTIME) {
    sem_wait(eastw);
    int now_eastw = bdata->east_waiting;
    sem_post(eastw);
    sem_wait(westw);
    int now_westw = bdata->west_waiting;
    sem_post(westw);
    for(int i = 0; i < now_westw; i++) {  printf(" WW "); }
    printf(" | ");
    if(side == west) {
      for(int i = 0; i < num; i++) { printf(" WB "); }
    } else if (side == east) {
      for(int i = 0; i < num; i++) { printf(" EB "); }
    }
    printf(" | ");
    for(int i = 0; i < now_eastw; i++) { printf(" EW "); }
    printf("\n");
    sleep(1);
    time++;
  }
}

void west_on_rope(struct data* bdata, pid_t pid) {
  sem_wait(turn);
  sem_wait(rope_data);
  int num = 0;
  if(bdata->on_rope_num != 0) {
    if(bdata->side == east) {
      sem_post(rope_data);
      sem_wait(rope);
      sem_wait(rope_data);
    }
  } else {
    sem_wait(rope);
  }
  sem_post(turn);
  sem_wait(westw);
  bdata->west_waiting -= 1;
  sem_post(westw);
  bdata->on_rope_num += 1;
  num = bdata->on_rope_num;
  bdata->side = west;
  sem_post(rope_data);
  climb_rope(bdata, num, west);
  sem_wait(rope_data);
  bdata->on_rope_num -= 1;
  if(bdata->on_rope_num == 0) {
    sem_post(rope);
  }
   sem_post(rope_data);
}

void east_on_rope(struct data* bdata, pid_t pid) {
  sem_wait(turn);
  sem_wait(rope_data);
  int num = 0;
  if(bdata->on_rope_num != 0) {
    if (bdata->side == west) {
      sem_post(rope_data);
      sem_wait(rope);
      sem_wait(rope_data);
    }
  } else {
    sem_wait(rope);
  }
  sem_post(turn);
  sem_wait(eastw);
  bdata->east_waiting -= 1;
  sem_post(eastw);
  bdata->on_rope_num += 1;
  num = bdata->on_rope_num;
  bdata->side = east;
  sem_post(rope_data);
  climb_rope(bdata, num, east);
  sem_wait(rope_data);
  bdata->on_rope_num -= 1;
  if(bdata->on_rope_num == 0) {
    sem_post(rope);
  }
  sem_post(rope_data);
}


void childBaboon(pid_t pid, struct data* bdata) {
  srand(pid);
  int get_side = rand() % 20;
  if(get_side <= west) {
    sem_wait(rope_data);
    if(bdata->side == -1) {
      bdata->side = west;
    }
    sem_post(rope_data);
    sem_wait(westw);
    bdata->west_waiting += 1;
    sem_post(westw);
    west_on_rope(bdata, pid);
  } else if (get_side <= east) {
    sem_wait(rope_data);
    if(bdata->side == -1) {
      bdata->side = east;
    }
    sem_post(rope_data);
    sem_wait(eastw);
    bdata->east_waiting += 1;
    sem_post(eastw);
    east_on_rope(bdata, pid);
  }
  
}

int main(int argc, char** argv) {
  struct data baboon_data;
  struct data* shm = mmap(NULL, sizeof(baboon_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , -1, 0);
  shm->on_rope_num = 0;
  shm->west_waiting = 0;
  shm->east_waiting = 0;
  shm->baboons = 0;
  shm->side = -1;
  rope = sem_open("sRope", O_CREAT, 0600, 1);
  westw = sem_open("semaWestw", O_CREAT, 0600, 1);
  eastw = sem_open("semaEastw", O_CREAT, 0600, 1);
  rope_data = sem_open("sRope_data", O_CREAT, 0600, 1);
  turn = sem_open("sTurn", O_CREAT, 0600, 1);
  
  while(shm->baboons < BABOONNUM) {
    shm->baboons += 1;
    srand(time(NULL));
    int randTime = rand() % (ARRIVAL - 1) + 1;
    sleep(randTime);
    pid_t pid = fork();
    if(pid < 0) {
      printf("fork failed\n");
      exit(0);
    } else if (pid == 0) {
      pid_t childpid = getpid();
      sleep(GET_ON_ROPE_TIME);
      childBaboon(childpid, shm);
      exit(0);
    }
  }
  sem_unlink("sRope");
  sem_close(rope);
  sem_unlink("semaWestw");
  sem_close(westw);
  sem_unlink("semaEastw");
  sem_close(eastw);
  sem_unlink("sRope_data");
  sem_close(rope_data);
  sem_unlink("sTurn");
  sem_close(turn);
  int stat;
  wait(&stat);
}
