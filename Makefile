all: serveur

serveur: main.o fonctions.o
	gcc main.o fonctions.o -o serveur -w

main.o: main.c fonctions.h
	gcc -c -Wall main.c

fonctions.o: fonctions.c definitions.h fonctions.h
	gcc -c -Wall fonctions.c -w

run: 
	./serveur 20000 20

