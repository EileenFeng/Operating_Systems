#include<string.h>
#include<ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

// global value for tv_usec
#define INTERVAL_USEC 0

// global variables
int last_total = 0;
int cur_total;
struct itimerval v;
int interval_sec = 1;

void sig_handler(int num) {
  FILE *fp;
  fp = fopen("/proc/stat", "r"); 
  char c;
  c = fgetc(fp);
  while((c = fgetc(fp)) != 'r') { continue; }
  cur_total = 0;
  c = fgetc(fp);
  while((c = fgetc(fp)) != ' '){
    cur_total *= 10;
    cur_total += c - '0';
  }
  fclose(fp);
}

int main(int argc, char** argv) {
  // check input
  if(argc > 1) {
    if(!strcmp(argv[1], "-s")) {
      int seconds = 0;
      char* s = argv[2];
      while(*s) {
	if(!isdigit(*s)) {
	  printf("Invalid input! Seconds must be integers  only! The interval second will be set to 1 second as default \n");
	  seconds = 1;
	  break;
	}
	seconds *= 10;
	seconds += *s -'0';
	s++;
      }
      interval_sec = seconds;
    } else {
      printf("Invalid flag! Program terminates. \n");
      exit(0);
    }
  }
  // set timer 
  v.it_value.tv_sec = interval_sec;
  v.it_value.tv_usec = INTERVAL_USEC;
  v.it_interval.tv_sec = interval_sec;
  v.it_interval.tv_usec = INTERVAL_USEC;
  signal(SIGALRM, sig_handler);
  
  while(1) {
    setitimer(ITIMER_REAL, &v, NULL);
    pause();
    if(last_total == 0) {
      last_total = cur_total;
    }else{
      printf("The interrupt rate at the past second is: %d\n", (cur_total - last_total)/interval_sec);
    }
    printf("Total number of interrupts is: %d\n", cur_total);
  }
  exit(0);
}
