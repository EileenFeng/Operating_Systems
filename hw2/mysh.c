#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define HISTSIZE 50

// global variables
int buff_size = 32;
int arg_nums = 0;
char** history; // store history inputs
int count = 0; // keep counting the number of inputs in total

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

// cd is not executable command, function for cd
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

// get the address of history input
char** get_recent_history_input(int num) {
  int index = count >= HISTSIZE ? num % HISTSIZE : num;
  index = index == 0 ? HISTSIZE - 1 : index - 1;
  return &history[index];
}

// store inputs
void store_history(char* input, int incre) { // decre = 1 when we need to increment count
  count = incre? count+1 : count;
  int index = count % HISTSIZE;
  index = index == 0 ? HISTSIZE - 1 : index - 1;
  if(count >= HISTSIZE) {
    free(history[index]);
  }
  history[index] = malloc((strlen(input)+1)*sizeof(char));
  char* temp = input;
  int tcount = 0;
  while(tcount < strlen(input)) {
    history[index][tcount] = (*temp == '\n') ? ' ' : *temp;
    tcount++; temp++;
  }
  history[index][tcount] = NULL;
}

// execute history input is requested
int execute_history_input(char* history_input) {
  int index = 0;
  int count_backwards = 0; // check if it's counting backwards for history input
  if(strcmp(history_input, "!!") == 0) {
    count_backwards = 1;
    index = 1;
  } else {
    int i = 0;
    if(history_input[1] == '-' ) {
      i = 2;
      count_backwards = 1;
    } else if(isdigit(history_input[1])) {
      i = 1;
    } else {
      printf("Invalid input for executing history input!\n");
      return 1;
    }
    for(; i < strlen(history_input); i++) {
      if(!isdigit(history_input[i])) {
	printf("History index can be positive numbers only! \n");
	return 1;
      } else {
	index *= 10;
	index += history_input[i] - '0';
      }
    }
  }
  if(index <0 || index > HISTSIZE || index > count) {
    printf("History input index too small or too big!\n");
    store_history(history_input, 0);
    return 1;
  }
  int minus = (count >= HISTSIZE) ? HISTSIZE : count;
  // get the history input
  char* temp = count_backwards ? *get_recent_history_input(count - index + 1) : *get_recent_history_input(count - (minus - index));
  // hard copy since parse_input will change the string
  char temp_input[strlen(temp) + 1];
  for(int i = 0; i < strlen(temp); i++) { temp_input[i] = temp[i];}
  temp_input[strlen(temp)] = NULL;
  store_history(temp_input, 1);
  char** args = parse_input(temp_input);
  if(arg_nums == 0) {
    printf("History input has no command inputs!\n");
    return 1;
  }
  return exec_args(args, temp_input, 1);
}

// function for print all history input
int print_history(){
  printf("History inputs are: \n");
  int ind = (count >= HISTSIZE) ? HISTSIZE : count;
  for(int i = 0; i < ind; i++) {
    printf("%d. ", i+1);
    printf("%s\n", *get_recent_history_input(count - (ind - i - 1)));
  }
  return 1;
}

// function for executing 'history' command
int exec_history(char** arg) {
  if(arg[1] != NULL) {
    printf("'history' does not take any arguments! \n");
  } else {
    print_history();
  }
}

// execute command line inputs
int exec_args(char** args, char*input, int free_args) {
  int value;
  if(args[0][0] == '!') {
    return execute_history_input(args[0]);
  } else {
    if(strcmp(args[0], "history") == 0) {
      value =  exec_history(args);
    }else if(strcmp(args[0], "cd") == 0) { // if the command is cd
      value = exec_cd(args);
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
	free(args);
	exit(0); // usage of exit referrence to: https://brennan.io/2015/01/16/write-a-shell-in-c/  
      } else if (pid > 0) {
	pid_t res = wait(&status);
	if(res < 0) {
	  printf("Child %d is not waited by the parent\n", pid);
	}
      }
      value = 1;
    }
  }
  // when exec_history_input calls exec_args, need to free the array 'args'
  if(free_args) {
    free(args);
  }
    return value;
}

int main(int argc, char** argv) {
  history = malloc(HISTSIZE * sizeof(char*));
  char *input, **args;
  int run;
  do {
    input = read_input();
    if(*input != '!') { store_history(input, 1);}
    args = parse_input(input);
    if(arg_nums == 0) {
      printf("No command line inputs!\n");
    } else {
      run = exec_args(args, input, 0);
    }
    free(args);
    free(input);
  } while (run);
  int end = count >= HISTSIZE ? HISTSIZE : count;
  for(int i = 0; i < end; i++) {
    free(history[i]);
  }
  free(history);
}
