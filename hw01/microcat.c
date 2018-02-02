#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <unistd.h>

#define RW_STDOUT 1

// global variable
char c;
size_t nbytes = sizeof(c);
ssize_t bytes_read;
ssize_t bytes_written;
int fd;


void sig_handler(int signal) {
  printf("Help! I think I've been shot!!!\n");
  exit(0);
}

void catFile(char* file) {
  fd = open(file, O_RDONLY);
  while((bytes_read = read(fd, &c, nbytes)) > 0) {
    bytes_written = write(1, &c, bytes_read);
    if(bytes_written < 0) {
      printf("Write to stdin failed! Error number: %d\n", errno);
      exit(0);
    }
  }
  if(bytes_read < 0) {
    printf("Read from file %s failed! Error number: %d\n", file, errno);
    exit(0);
  }
  if(close(fd) < 0){
    printf("Close file failed. Error number: %d\n", errno);
    exit(0);
  }
}

void cat_stdin(){
  while((bytes_read = read(0, &c, nbytes)) > 0) {
    bytes_written = write(1, &c, bytes_read);
    if(bytes_written < 0) {
      printf("Write to stdin failed! Error number: %d\n", errno);
      exit(0);
    }
  }
  if(bytes_read < 0) {
    printf("Read from stdin failed! Error number: %d\n", errno);
    exit(0);
  }
}

int main(int argc, char** argv) {
  if(argc > 1) {
    int fileNum = 1;
    while(fileNum < argc) {
      catFile(argv[fileNum]);
      fileNum++;
    }
  } else {
    cat_stdin();
  }
}
