all: teste

teste: teste.o llfifo.o
	gcc -g $^ -o $@

%.o:%.c llfifo.h
	gcc -std=c99 -g -c $< -o $@

clean:
	rm llfifo.o teste.o teste

