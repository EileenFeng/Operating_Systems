CC=gcc

all: threads
threads: threads.o
	$(CC) -o threads threads.o -lpthread
threads.o: threads.c
	$(CC) -c threads.c
clean:
	rm threads threads.o
