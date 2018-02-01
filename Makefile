CC=gcc

all: hw1a hw1b
hw1a: hw1a.o
	$(CC) -o hw1a hw1a.o
hw1b: hw1b.o
	$(CC) -o hw1b hw1b.o
hw1a.o: hw1a.c
	$(CC) -c hw1a.c
hw1b.o: hw1b.c
	$(CC) -c hw1b.c
clean:
	rm hw1a hw1b hw1a.o hw1b.o
