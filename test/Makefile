INCLUDE=-I..
WARNINGS=-Wno-implicit-int -Wno-return-type
CFLAGS=-std=c11 $(WARNINGS) -g $(INCLUDE)
CC=gcc

all: bufset-test

bufset-test: bufset-test.c ../buf.h
	$(CC) $(CFLAGS) bufset-test.c -o bufset-test  

clean:
	rm bufset-test

.Phony: all clean
