

CC=clang
CFLAGS=-Wall -g -Wno-unused-parameter
BINS=main libargvhandler.so
all: $(BINS)

libargvhandler.o: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -c libargvhandler.c

libargvhandler.so: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libargvhandler.c -lc

main: main.c libargvhandler.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o *.so main	
