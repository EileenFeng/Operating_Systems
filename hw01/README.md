## CS355 Operating Systems HW01
### Name: Yazhe(Eileen) Feng

### How to compile the program
- This project contains three files. 'hw1a.c' for part 1a, 'hw1b.c' for part 1b, and 'microcat.c' for part 2 and extra creid.
- To compile the program, type 'make'; to compile specific program, type 'make <executable>' (for instance 'make hw1a')


### How to run the program
- part 1a: type in './hw1a' to run the program
- part 1b: type in './hw1b -s <seconds>' to custom the interval. The interval can only be integers; if a number that is not an integer is typed in, the program will default the interval to 1 second. If instead of the '-s' flag, another flag is typed in, the program will exit since there is an invalid input

### Know Limitations:
- hw1b.c: when time interval is not one second, the program divide the difference between number interrupts recorded last time and this time, by the number of seconds in the time interval (using integer division), which might not be that accurate 
- microcat.c:  Did not register signal handler for SIGKILL and SIGSTOP since both cannot be catched
