CC=gcc

all: baboon
baboon: baboon_hw3.o
	$(CC) -o baboon baboon_hw3.o -lpthread -lrt
baboon.o: baboon_hw3.c
	$(CC) -c -Wall baboon_hw3.c
clean:
	rm baboon baboon_hw3.o
