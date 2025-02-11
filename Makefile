CC = gcc
CFLAGS = -I. -ggdb3 -Wall -Wpedantic -fsanitize=address -std=c11
LIBS = -lSDL2 -lSDL2_ttf -lm
DEPS = defs.h render.h board.h 
OBJ = tetris.o tetromino.o render.o board.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tetris: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f tetris *.o

