CC=gcc
CFLAGS=-I. -I/usr/include -I/usr/local/include -std=c11 -Wall -L/usr/local/lib -lSDL2
DEPS = events.h metro.h
OBJ = niil.o events.o metro.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

niil: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
