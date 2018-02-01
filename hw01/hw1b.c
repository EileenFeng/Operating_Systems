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
int total_intr = 0;
struct itimerval v;
int interval_sec = 1;

void sig_handler(int num) {
	FILE *fp;
	fp = fopen("/proc/stat", "r"); 
	char c;
	c = fgetc(fp);
	while((c = fgetc(fp)) != 'r') { continue; }
	int sum = 0;
	c = fgetc(fp);
	while((c = fgetc(fp)) != ' '){
		sum *= 10;
		sum += c - '0';
	}
	fclose(fp);
	if(total_intr == 0) {
	  total_intr = sum;
	}else{
	  printf("The interrupt rate at the past second is: %d\n", (sum - total_intr)/interval_sec);
	}
	printf("Total number of interrupts is: %d\n", sum);
	setitimer(ITIMER_REAL, &v, NULL);
}

int main(int argc, char** argv) {
  if(argc > 1) {
    if(!strcmp(argv[1], "-s")) {
      int seconds = 0;
      char* s = argv[2];
      while(*s) {
	if(!isdigit(*s)) {
	  printf("Invalid input! Seconds must be integers  only! The interval second will be set to 1 second as default \n");
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
  v.it_value.tv_sec = interval_sec;
  v.it_value.tv_usec = INTERVAL_USEC;
  v.it_interval.tv_sec = interval_sec;
  v.it_interval.tv_usec = INTERVAL_USEC;
  signal(SIGALRM, sig_handler);
  setitimer(ITIMER_REAL, &v, NULL);
  while(1) {
    pause();
  }
  exit(0);
}
