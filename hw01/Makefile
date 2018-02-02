CC=gcc

all: hw1a hw1b microcat
hw1a: hw1a.o
	$(CC) -o hw1a hw1a.o
hw1b: hw1b.o
	$(CC) -o hw1b hw1b.
microcat: microcat.o
	$(CC) -o microcat microcat.o
hw1a.o: hw1a.c
	$(CC) -c hw1a.c
hw1b.o: hw1b.c
	$(CC) -c hw1b.c
microcat.o: microcat.c
	$(CC) -c microcat.c
clean:
	rm hw1a hw1b microcat hw1a.o hw1b.o microcat.o
