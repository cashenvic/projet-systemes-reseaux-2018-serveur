all: ./bin/serveur

./bin/serveur: ./bin/main.o ./bin/fonctions.o
	gcc ./bin/main.o ./bin/fonctions.o -o ./bin/serveur -w

./bin/main.o: ./sources/main.c ./headers/fonctions.h
	gcc -c -Wall ./sources/main.c -o ./bin/main.o

./bin/fonctions.o: ./sources/fonctions.c ./headers/definitions.h ./headers/fonctions.h
	gcc -c -Wall ./sources/fonctions.c -w -o ./bin/fonctions.o

run: 
	./bin/serveur 20000 20

clean:
	rm -f ./bin/main.o ./bin/fonctions.o ./bin/serveur
