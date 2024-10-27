all: build link

build:
	gcc -c -o main.o main.c

link:
	gcc -o main main.o

