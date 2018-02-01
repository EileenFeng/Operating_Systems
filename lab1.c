#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <stdio.h>

void inParent(int signal) {
  printf("Receive child sent signal: %d\n", signal);
}

void inChild(int signal) {
  printf("Receive parent sent signal: %d\n", signal);
}

int main(){
  pid_t pid;
  signal(SIGUSR1, inParent);
  signal(SIGUSR2, inChild);
  pid = fork();
  if(pid == 0) {
    pid_t ppid = getppid();
    int check = kill(ppid, SIGUSR1);
    if(check != 0) {
      printf("signal failed to send in child process\n");
    }
    sleep(1);
  } else if (pid > 0) {
    int check = kill(pid, SIGUSR2);
    if(check != 0) {
      printf("signal failed to send in parent process\n");
    }
    int status;
    wait(&status);
  }else {
    printf("fork exit error code: %d\n", pid);
  }
}
