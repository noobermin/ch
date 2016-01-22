INCLUDE=-I..
WARNINGS=-Wno-implicit-int -Wno-return-type #-Wall
CFLAGS=-std=c11 $(WARNINGS) -g $(INCLUDE)
CC=gcc

all: buftest

buftest: buftest.c
	$(CC) $(CFLAGS) buftest.c -o buftest  

clean:
	rm buftest

.Phony: all clean
