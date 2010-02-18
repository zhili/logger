# logger's Makefile
CC = g++

CFLAGS = -g -Wall

logger: main.o logger.o mutex.o
	$(CC) main.o logger.o mutex.o -o logger $(CFLAGS)
mutex.o: mutex.h mutex.cc
	$(CC) mutex.cc -c $(CFLAGS)
logger.o: logger.h logger.cc
	$(CC) logger.cc -c $(CFLAGS)
main.o: main.cpp
	$(CC) main.cpp -c $(CFLAGS)
clean:
	rm -f *.o core logger