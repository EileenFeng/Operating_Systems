#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/time.h>
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

// error messages
char writef[] = "Write failed!\n";
char openf[] = "Open file failed.\n";
char readf[] = "Read failed. \n";
char closef[] = "Close file failed.\n";

// output error message to console
void errormsg(char* msg, size_t bytes) {
  write(1, msg, bytes);
  exit(0);
}

void shotmsg(){
  char msg[] = "Help! I think I've been shot!!!\n";
  write(1, msg, sizeof(msg));
  exit(0);
}

void sig_handler(int signal) {
  shotmsg();
}

// function for cat files (read from file and write to stdout)
void cat_file(char* file) {
  if((rfd = open(file, O_RDONLY)) < 0) {
    errormsg(openf, sizeof(openf));
  }
  while((bytes_read = read(rfd, &c, nbytes)) > 0) {
    bytes_written = write(wfd, &c, bytes_read);
    if(bytes_written < 0) {
      errormsg(writef, sizeof(writef));
    }
  }
  if(bytes_read < 0) {
    errormsg(readf, sizeof(readf));
  }
  if(close(rfd) < 0){
    errormsg(closef, sizeof(closef));
  }
}

// read from stdin and write to stdout or file
void cat_stdIO(){
  while((bytes_read = read(rfd, &c, nbytes)) > 0) {
    bytes_written = write(wfd, &c, bytes_read);
    if(bytes_written < 0) {
      errormsg(writef, sizeof(writef));
    }
  }
  if(bytes_read < 0) {
    errormsg(readf, sizeof(readf));
  }
}

int main(int argc, char** argv) {
  int sameFile = 0;
  for (int i = 1; i <= 31; i++) {
    if(i != 9 && i !=17) {
      signal(i, sig_handler);
    }
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
      if(wfd < 0) {
	errormsg(openf, sizeof(openf));
      }
      fileEnd = argc - 2;
    }else { // output to stdio
      fileEnd = argc;
      wfd = 1;
    }
    while(fileNum < fileEnd) {
      if(strcmp(argv[fileNum], "-") == 0) { // read from stdin
	rfd = 0;
	cat_stdIO();
      } else if(strcmp(argv[fileNum], ">") == 0)  { 
	fileNum++;
	continue;
      } else if (strcmp(argv[fileNum], argv[argc-1]) == 0 && outputToFile) {
	fileNum ++;
	sameFile = 1;
	continue;
      } else { // read from file
	cat_file(argv[fileNum]);
      }
      fileNum ++;
    }
    // close written file if applicable
    if(outputToFile) {
      if(close(wfd) < 0) {
	errormsg(closef, sizeof(closef));
      }
    }
  } else {
    wfd = 1;
    cat_stdIO();
  }
  if(sameFile) {
    char samef[] = "Input file is the output file\n";
    write(1, &samef, sizeof(samef));
    sameFile = 0;
  }
  exit(0);
}
