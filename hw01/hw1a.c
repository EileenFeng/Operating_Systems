#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int last_total = 0;
int current_total;
int read_failed = 0;

void sig_handler(int num) {
  FILE *fp;
  fp = fopen("/proc/stat", "r");
  if(!fp) {
    read_failed = 1;
  } else {
    char c;
    while((c = fgetc(fp)) != 'r') { continue; }
    c = fgetc(fp);
    current_total = 0;
    while((c = fgetc(fp)) != ' '){
      current_total *= 10;
      current_total += c - '0';
    }
    fclose(fp);
  }
}

int main() {
  signal(SIGALRM, sig_handler);
  while(1) {
    alarm(1);
    if(read_failed) {
      printf("Read file failed! \n");
      exit(0);
    }
    pause();
    if(!read_failed) {
 
      if(last_total == 0) {
	last_total = current_total;
      }else{
	printf("The interrupt rate at the past second is: %d\n", current_total - last_total);
      }
      printf("Total number of interrupts is: %d\n", current_total);
    }
  }
  exit(0);
}
