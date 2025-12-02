CC = gcc
CFLAGS = -Wall
OBJ = pesquisa.o asi.o arvoreBinaria.o arvoreB.o arvoreB_e.o

all: pesquisa

pesquisa: $(OBJ)
	$(CC) $(OBJ) -o pesquisa -lm
	@rm -f $(OBJ)

pesquisa.o: pesquisa.c pesquisa.h
	$(CC) $(CFLAGS) -c pesquisa.c

asi.o: asi.c asi.h
	$(CC) $(CFLAGS) -c asi.c

arvoreBinaria.o: arvoreBinaria.c arvoreBinaria.h
	$(CC) $(CFLAGS) -c arvoreBinaria.c

arvoreB.o: arvoreB.c arvoreB.h
	$(CC) $(CFLAGS) -c arvoreB.c

arvoreB_e.o: arvoreB_e.c arvoreB_e.h
	$(CC) $(CFLAGS) -c arvoreB_e.c

run:
	@./pesquisa

clean:
	rm -f pesquisa $(OBJ)

val:
	$(CC) -g -o exeval pesquisa.c asi.c arvoreBinaria.c arvoreB.c arvoreB_e.c -Wall -lm

valrun: val
	valgrind --leak-check=full --track-origins=yes -s ./exeval
