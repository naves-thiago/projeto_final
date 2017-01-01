all: uid.o main.o bytefifo.o
	gcc -Wall -g $^ -o uid

FLAGS = -Wall -g -c -std=c99 -I../bytefifo

bytefifo.o: ../bytefifo/bytefifo.c ../bytefifo/bytefifo.h
	gcc $(FLAGS) $< -o $@

%.o:%.c ../bytefifo/bytefifo.h uid.h
	gcc $(FLAGS) $< -o $@
