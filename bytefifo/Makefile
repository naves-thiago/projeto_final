all: main.o bytefifo.o
	gcc -Wall -g $^ -o bytefifo

%.o: %.c bytefifo.h
	gcc -Wall -c -g $< -o $@
