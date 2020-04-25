CC = gcc
CFLAGS = -std=c11 -g

DIRS = bld bin

OBJ = bld/main.o

bin/chip8: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o bin/chip8

bld/main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bld/main.o -c

clean:
	rm bld/*.o && rm bin/*

$(shell mkdir -p $(DIRS))
