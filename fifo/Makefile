all: teste

teste: teste.o fifo.o
	gcc -g $^ -o $@

%.o:%.c fifo.h
	gcc -std=c99 -g -c $< -o $@

clean:
	rm fifo.o teste.o teste

