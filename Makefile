

CC=clang
CFLAGS=-Wall -g -fopenmp
BINS=main libbmpencoder.so
all: $(BINS)

libbmpencoder.o: libbmpencoder.c bmpencoder.h
	$(CC) $(CFLAGS) -c libbmpencoder.c

libbmpencoder.so: libbmpencoder.c bmpencoder.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libbmpencoder.c -lc

main: main.c libbmpencoder.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o *.so main	
