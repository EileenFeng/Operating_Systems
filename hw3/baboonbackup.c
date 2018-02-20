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
sem_t* west_go; // controls whether west waiting baboons have access to the rope
sem_t* east_go; // controls whether east waiting baboons have access to the rope

void climb_rope(struct data *bdata, int num, int side){
  int time = 0;
  printf("active climbing\n");
  while(time < CLIMBTIME) {
    sem_wait(eastw);
    int now_eastw = bdata->east_waiting;
    sem_post(eastw);
    sem_wait(westw);
    int now_westw = bdata->west_waiting;
    sem_post(westw);
    for(int i = 0; i < now_westw; i++) {
      printf(" WW ");
    }
    printf(" | ");
    if(side == west) {
      for(int i = 0; i < num; i++) {
	printf(" WB ");
      }
    } else if (side == east) {
      for(int i = 0; i < num; i++) {
	printf(" EB ");
      }
    }
    printf(" | ");
    for(int i = 0; i < now_eastw; i++) {
      printf(" EW ");
    }
    printf("\n");
    sleep(1);
    time++;
  }
}

void west_on_rope(struct data* bdata, pid_t pid) {
  sem_wait(west_go);
  printf("============ West %d start waiting\n", pid);
  sem_wait(rope_data);
  int num = 0;
  int side = 0;
  if(bdata->on_rope_num != 0) {
    if(bdata->side == east) {
      sem_post(rope_data);
      sem_wait(rope);
      sem_wait(rope_data);
    }
  } else {
    sem_wait(rope);
  }
  printf("++++++++WWWest: %d is in CRITICAL REGION\n", pid);
  sem_wait(westw);
  bdata->west_waiting -= 1;
  sem_post(westw);
  bdata->on_rope_num += 1;
  num = bdata->on_rope_num;
  bdata->side = west;
  side = bdata->side;
  num = bdata->on_rope_num;
  bdata->side = west;
  side = bdata->side;
  sem_post(rope_data);
  climb_rope(bdata, num, side);
  sem_wait(rope_data);
  bdata->on_rope_num -= 1;
  if(bdata->on_rope_num == 0) {
    printf("-W_W_W_W_W_W__W no west baboon is on the rope\n");
    sem_post(rope);
  }
  
  printf("--------West: %d left the rope\n", pid);
  sem_post(rope_data);
}

void east_on_rope(struct data* bdata, pid_t pid) {
  sem_wait(east_go);
  printf("=========== East %d start waiting \n", pid);
  sem_wait(rope_data);
  int num = 0;
  int side = 0;
  if(bdata->on_rope_num != 0) {
    if (bdata->side == west) {
      sem_post(rope_data);
      sem_wait(rope);
      sem_wait(rope_data);
    }
  } else {
    sem_wait(rope);
  }
  printf("++++++++EEEest: %d is in CRITICAL REGION\n", pid);
  sem_wait(eastw);
  bdata->east_waiting -= 1;
  sem_post(eastw);
  bdata->on_rope_num += 1;
  num = bdata->on_rope_num;
  bdata->side = east;
  side = bdata->side;
  sem_post(rope_data);
  climb_rope(bdata, num, side);
  sem_wait(rope_data);
  bdata->on_rope_num -= 1;
  if(bdata->on_rope_num == 0) {
    printf("E_E_E_E_E_E_E_E__E_E_E no east baboon is on the rope\n");
    sem_post(rope);
  }
  printf("--------East: %d left the rope\n", pid);
  sem_post(rope_data);
}

void update(struct data* bdata, int cur_side) {
  int valw;
  int vale;
  sem_getvalue(east_go, &vale);
  sem_getvalue(west_go, &valw);
  sem_wait(eastw);
  int cur_eastw = bdata->east_waiting;
  sem_post(eastw);
  sem_wait(westw);
  int cur_westw = bdata->west_waiting;
  sem_post(westw);
  if(cur_side == west) {
    if(cur_eastw > 0) {
      if(valw > 0) { sem_wait(west_go);}
      if(vale == 0) { sem_post(east_go); }
    } else {
      if(valw == 0) { sem_post(west_go); }
    }
  } else if (cur_side == east) {
    if(cur_westw > 0) {
      if(vale > 0) { sem_wait(east_go);}
      if(valw == 0) { sem_post(west_go);}
    } else {
      if(vale == 0) { sem_post(east_go);}
    }
  }
}

void childBaboon(pid_t pid, struct data* bdata) {
  srand(pid);
  int get_side = rand() % 20;
  int curr_side;
  if(get_side <= west) {
    sem_wait(rope_data);
    if(bdata->side == -1) {
      bdata->side = west;
    }
    curr_side = bdata->side;
    sem_post(rope_data);
    sem_wait(westw);
    bdata->west_waiting += 1;
    sem_post(westw);
    update(bdata, curr_side);
    west_on_rope(bdata, pid);
  } else if (get_side <= east) {
    sem_wait(rope_data);
    if(bdata->side == -1) {
      bdata->side = east;
    }
    curr_side = bdata->side;
    sem_post(rope_data);
    sem_wait(eastw);
    bdata->east_waiting += 1;
    sem_post(eastw);
    update(bdata, curr_side);
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
  west_go = sem_open("semawestgo", O_CREAT, 0600, 1);
  east_go = sem_open("semaeastgo", O_CREAT, 0600, 1);
  
    int value = 11;
    sem_getvalue(rope, &value);
    printf("in main rope value  %d \n", value);
    sem_getvalue(westw, &value);
    printf("in main westw value  %d \n", value);
    sem_getvalue(eastw, &value);
    printf("in main eastw value  %d \n", value);
    sem_getvalue(rope_data, &value);
    printf("in main rope_data value  %d \n", value);
    sem_getvalue(west_go, &value);
    printf("in main west_go value  %d \n", value);
    sem_getvalue(east_go, &value);
    printf("in main east_go value  %d \n", value);

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
  sem_unlink("semawestgo");
  sem_close(west_go);
  sem_unlink("semaeastgo");
  sem_close(east_go);
  
  int stat;
  wait(&stat);
  // while(waitpid(-1, &stat,WNOHANG) != 0);
}
