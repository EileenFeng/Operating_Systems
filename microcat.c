#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// global variable
char c;
size_t nbytes = sizeof(c);
ssize_t bytes_read;
ssize_t bytes_written;
int rfd = 0;
int wfd = 1;

void sig_handler(int signal) {
  printf("Help! I think I've been shot!!!\n");
  exit(0);
}

// function for cat files (read from file and write to stdout)
void cat_file(char* file) {
  if((rfd = open(file, O_RDONLY)) < 0) {
    printf("Open file %s failed. Error number %d.\n", file, errno);
    exit(0);
  }
  while((bytes_read = read(rfd, &c, nbytes)) > 0) {
    bytes_written = write(wfd, &c, bytes_read);
    if(bytes_written < 0) {
      printf("Write failed! Error number: %d\n", errno);
      exit(0);
    }
  }
  if(bytes_read < 0) {
    printf("Read from file %s failed! Error number: %d\n", file, errno);
    exit(0);
  }
  if(close(rfd) < 0){
    printf("Close file failed. Error number: %d\n", errno);
    exit(0);
  }
}

// read from stdin and write to stdout or file
void cat_stdIO(){
  while((bytes_read = read(rfd, &c, nbytes)) > 0) {
    bytes_written = write(wfd, &c, bytes_read);
    if(bytes_written < 0) {
      printf("Write failed! Error number: %d\n", errno);
      exit(0);
    }
  }
  if(bytes_read < 0) {
    printf("Read from stdin failed! Error number: %d\n", errno);
    exit(0);
  }
}

int main(int argc, char** argv) {
  for (int i = 1; i <= 31; i++) {
    signal(i, sig_handler);
  }
  signal(SIGINT, sig_handler);
  if(argc > 1) {
    int outputToFile = 0;
    int fileNum = 1;
    int fileEnd; // use to track the last file in command line input that needs to read in 
    // check if need to direct output to files
    for(int i = 0; i < argc; i++) {
      if (strcmp(argv[i], ">") == 0) {
	outputToFile = 1;
      }
    }
    if(outputToFile) { // need to redirect output to files
      wfd = open(argv[argc-1], O_CREAT|O_RDWR|O_TRUNC, 0666);
      fileEnd = argc - 2;
      if(wfd < 0) {
	printf("Open file %s failed. Error number: %d\n", argv[argc-1], errno);
      }
    }else { // output to stdio
      fileEnd = argc;
      wfd = 1;
    }
    while(fileNum < fileEnd) {
      if(strcmp(argv[fileNum], "-") == 0) { // read from stdin
	rfd = 0;
	cat_stdIO();
      } else if(strcmp(argv[fileNum], ">") == 0) { 
	fileNum++;
	continue;
      } else { // read from file
	cat_file(argv[fileNum]);
      }
      fileNum ++;
    }
    // close written file if applicable
    if(outputToFile) {
      if(close(wfd) < 0) {
	printf("Close file %s failed. Error number %d.\n", argv[argc-1], errno);
      }
    }
  } else {
    wfd = 1;
    cat_stdIO();
  }
  exit(0);
}
