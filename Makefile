

CC=clang
CFLAGS=-Wall -g
BINS=main libargvhandler.so libsecondexercise.so libnoargumentbroswerhandler.so
all: $(BINS)

libargvhandler.o: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -c libargvhandler.c

libsecondexercise.o: libsecondexercise.c secondexercise.h
	$(CC) $(CFLAGS) -c libsecondexercise.c

libnoargumentbroswerhandler.o: libnoargumentbroswerhandler.c noargumentbroswerhandler.h
	$(CC) $(CFLAGS) -c libnoargumentbroswerhandler.c 	

libargvhandler.so: libargvhandler.c argvhandler.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libargvhandler.c -lc

libsecondexercise.so: libsecondexercise.c secondexercise.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libsecondexercise.c -lc

libnoargumentbroswerhandler.so: libnoargumentbroswerhandler.c noargumentbroswerhandler.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libnoargumentbroswerhandler.c -lc	

main: main.c libargvhandler.o libsecondexercise.o libnoargumentbroswerhandler.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o *.so main	
