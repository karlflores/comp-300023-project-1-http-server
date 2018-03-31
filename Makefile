CC = gcc
CFLAGS = -Wall -std=c99

SRC1 = server.c
OBJ1 = server.o

EXE1 = SimpleHTTPServer

all: $(EXE1)

$(EXE1): $(OBJ1) Makefile
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJ1) -g

clean:
	rm -f $(OBJ) $(EXE)
