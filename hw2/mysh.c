#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// global variables 
int buff_size = 32;
int arg_nums = 0;

// read in the input
char* read_input() {
  size_t readn;
  char* input = NULL;
  printf("mysh > ");
  if(getline(&input, &readn, stdin) < 0) {
    printf("Read from stdin failed! \n");
    return NULL;
  }
  return input;
}

// parse the input 
char** parse_input(char* input) {
  char** tokens = malloc(buff_size * sizeof(char*));
  char* token = strtok(input, " \t\r\n\a");
  int count = 0;
  while(token) {
    tokens[count] = token;
    count++;
    if(count >= buff_size) {
      buff_size *= 2;
      tokens = (char**) realloc(tokens, buff_size);
    }
    token = strtok(NULL, " \t\r\n\a");
  }
  tokens[count] = NULL;
  arg_nums = count;
  return tokens;
}

// function for changing directory
int exec_cd(char** path_args) {
  if(path_args[1] == NULL) { // when no path is provided
    printf("Please provide a path for changing directory! \n");
  } else if(path_args[2] != NULL) { // when too many arguments are passed into cd
    printf("cd: too many arguments! \n");
  } else {
    if(chdir(path_args[1]) < 0) {
      printf("Change to directory %s failed! \n", path_args[1]);
    }
  }
  return 1;
}

// execute command line inputs
int exec_args(char** args) {
  if(strcmp(args[0], "cd") == 0) { // if the command is cd
    return exec_cd(args);
  }else if(strcmp(args[0], "exit") == 0) { // if the command is exit
    return 0;
  } else {
    pid_t pid;
    int status;
    pid = fork();
    if(pid < 0) {
      printf("Execute command failed - fork failed! \n");
    } else if (pid == 0) { // children process
      if(execvp(args[0], args) < 0) {
	printf("An error occurred during execution, executing command %s failed!\n", args[0]);
      }
      exit(0);
    } else if (pid > 0) {
      pid_t res = wait(&status);
      if(res < 0) {
	printf("Child %d is not waited by the parent\n", pid);
      }       
    }
  }
  return 1;
}

int main(int argc, char** argv) {
  char* input = read_input();
  char** args = parse_input(input);
  int run = exec_args(args);
  while(run) {
    free(input);
    free(args);
    input = read_input();
    args = parse_input(input);
    run = exec_args(args);
  }
}
