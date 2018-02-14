### Operating Systems HW2
### Yazhe(Eileen) Feng

- How to compile
  - Type in 'make all' to compile all programs

- How to run
  - Type in './threads' to run the first program (first part of homework)
  - Type in './mysh' to run the simple shell
  - 'mysh' has history fully implemented
  
- Limitations & Notations
  - Do not allow special symbols such as '&'
  - Spaces need to be inserted between each two arguments
  - Does not support 'cat' with redirection
  - Does not support multiple commands input in one line, for instance 'pwd ls' does not work, a system error will be given but the program ./mysh mostly does not crash
  - For history:
    - No spaces are allowed between '!' or '!-'and the number
    - No negative number for history index are allowed, only positive number are accepted
    - '^C' (Control-C) does not stop the shell; it simply print out '^C' to the console and keeps running
     - '^D' (Control-D) will cause read from stdin failed, but the program does not exit (which is not the same as shell on powerpuff, which exit when typed in Control-D)
     - If trying to execute history inputs when there is no history inputs available yet, for instance when '!!' is the first input to the shell, then a warning will be given by the program
  - When typed in '^Z' (Control-Z) while running program in valgrind, valgrind will show there is an error, but it does not influence program running  
   