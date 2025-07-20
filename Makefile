all: main

main: main.o interacoes.o interface.o
	gcc -o main main.o interacoes.o interface.o

main.o: main.c interacoes.h interface.h
	gcc -c main.c

interacoes.o: interacoes.c interacoes.h
	gcc -c interacoes.c

interface.o: interface.c interface.h
	gcc -c interface.c

clean:
	rm -f *.o main

