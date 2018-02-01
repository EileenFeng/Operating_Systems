#include <sys/time.h>
#include <signal.h>


#define INTERVAL_SECS 1
#define INTERVAL_MSECS 0
// pause() -- could be used to kill a process with the signal SIGINT
void sigCount(int sig) {
  
}

int main() {
  struct itimerval v;
  v.it_value.tv_sec = INTERVAL_SECS;
  v.it_value.tv_usec = INTERVAL_MSECS;
  v.it_interval.tv_sec = INTERVAL_SECS;
  v.it_interval.tv_usec = INTERVAL_MSECS;
  
}
