# The compiler 
CC = gcc

# CFLAGS for compilation 
CFLAGS = -pedantic -Wall -std=gnu99
DEBUG = -g 

# Libraries
LDFLAGS = -L/local/courses/csse2310/lib -lcsse2310a1
CFLAGS += -I/local/courses/csse2310/include

.DEFAULT_GOAL := uqwordladder

.PHONY: debug clean 

debug: CFLAGS += $(DEBUG)
debug: uqwordladder

uqwordladder.o: uqwordladder.c
	$(CC) $(CFLAGS) -c $^ -o $@

uqwordladder: uqwordladder.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f uqwordladder *.o
