INCLUDE=-I..
WARNINGS=-Wno-implicit-int -Wno-return-type
CFLAGS=-std=c11 $(WARNINGS) -g $(INCLUDE)
CC=gcc

all: buf-set-test

buf-set-test: buf-set-test.c ../buf.h
	$(CC) $(CFLAGS) buf-set-test.c -o buf-set-test  

clean:
	rm buf-set-test

.Phony: all clean
