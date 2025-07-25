all: run

run: py main
	./main

py:
	python3 gerador_arquivos_dot.py
	python3 gerador_grafo.py

main: main.o interacoes.o interface.o agendador.o
	gcc -o main main.o interacoes.o interface.o agendador.o

main.o: main.c interacoes.h interface.h agendador.h
	gcc -c main.c

interacoes.o: interacoes.c interacoes.h 
	gcc -c interacoes.c

interface.o: interface.c interface.h
	gcc -c interface.c

agendador.o: agendador.c agendador.h
	gcc -c agendador.c

clean:
	rm -f *.o main
