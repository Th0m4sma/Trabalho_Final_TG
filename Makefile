all: main

main: main.o interacoes.o
	gcc -o main main.o interacoes.o

main.o: main.c interacoes.h
	gcc -c main.c

interacoes.o: interacoes.c interacoes.h
	gcc -c interacoes.c

clean:
	rm -f *.o main
