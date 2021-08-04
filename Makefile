CC=gcc
CFLAGS=-I. -I/usr/include -I/usr/local/include -std=c11 -Wall -L/usr/local/lib -lSDL2 -pthread -D_GNU_SOURCE -std=c11
DEPS = events.h metro.h sdl.h
OBJ = niil.o events.o metro.o sdl.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

niil: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
