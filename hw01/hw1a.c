#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int total_intr = 0;

void sig_handler(int num) {
	FILE *fp;
	fp = fopen("/proc/stat", "r"); 
	char c;
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
		printf("The interrupt rate at the past second is: %d\n", sum - total_intr);
	}
	printf("Total number of interrupts is: %d\n", sum);
	alarm(1);
}

int main() {
	signal(SIGALRM, sig_handler);
	alarm(1);
	while(1) {
		pause();
	}
	exit(0);
}
