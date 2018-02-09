#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// global variables 
int buff_size = 64;
int arg_nums = 0;

// read in the input
char* read_input() {
  size_t readn;
  char* input = NULL;
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
  arg_nums = count;
  return tokens;
}

int exec_args(char** args) {
  
}

int main(int argc, char** argv) {
  char* input = read_input();
  char** args = parse_input(input);
 
}
