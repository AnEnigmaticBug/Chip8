CC = gcc
CFLAGS = -std=c11 -g

DIRS = bld bin

OBJ = bld/chip8.o bld/main.o bld/viewer.o

bin/chip8: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lSDL2 -o bin/chip8

bld/chip8.o: src/chip8.h src/chip8.c
	$(CC) $(CFLAGS) src/chip8.c -o bld/chip8.o -c

bld/main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bld/main.o -c

bld/viewer.o: src/viewer.h src/viewer.c
	$(CC) $(CFLAGS) src/viewer.c -o bld/viewer.o -c

clean:
	rm bld/*.o && rm bin/*

$(shell mkdir -p $(DIRS))
