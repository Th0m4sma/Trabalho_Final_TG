all: run

run: py main_principal
	./main_principal

py:
	python3 gerador_arquivos_dot.py
	python3 gerador_grafo.py

main_principal: main_principal.o interacoes.o interface.o
	gcc -o main_principal main_principal.o interacoes.o interface.o

main_principal.o: main_principal.c interacoes.h interface.h
	gcc -c main_principal.c

interacoes.o: interacoes.c interacoes.h
	gcc -c interacoes.c

interface.o: interface.c interface.h
	gcc -c interface.c

clean:
	rm -f *.o main_principal
