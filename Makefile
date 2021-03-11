CC=gcc
CFLAGS=-I.
DEPS = example.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

example: example.o mkey.o 
	$(CC) -o example example.o mkey.o 
