CC=gcc

all: threads mysh
threads: threads.o
	$(CC) -o threads threads.o -lpthread
threads.o: threads.c
	$(CC) -c threads.c
mysh: mysh.o
	$(CC) -o mysh mysh.o
mysh.o: mysh.c
	$(CC) -c mysh.c
clean:
	rm threads mysh threads.o mysh.o
