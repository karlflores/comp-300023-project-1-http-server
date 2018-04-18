CC = gcc
CFLAGS = -Wall -std=c99 -pthread

SRC1 = server.c main.c http_library.c
OBJ1 = server.o main.o http_library.o

EXE1 = myserver

all: $(EXE1)

$(EXE1): $(OBJ1) Makefile
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJ1) -g

clean:
	rm -f $(OBJ1) $(EXE1)
