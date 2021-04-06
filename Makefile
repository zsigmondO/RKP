

CC=clang
CFLAGS=-Wall -g
BINS=main libargvhandler.so libsecondexercise.so
all: $(BINS)

libargvhandler.o: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -c libargvhandler.c

libsecondexercise.o: libsecondexercise.c secondexercise.h
	$(CC) $(CFLAGS) -c libsecondexercise.c

libargvhandler.so: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libargvhandler.c -lc

libsecondexercise.so: libsecondexercise.c secondexercise.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libsecondexercise.c -lc

main: main.c libargvhandler.o libsecondexercise.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o *.so main	
