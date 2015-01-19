INCLUDE=-I/home/noobermin/t/include
WARNINGS=-Wall -Wno-return-type -Wno-implicit-int 
CFLAGS=-std=c11 $(WARNINGS) -g $(INCLUDE)


all: aifaleene

aifaleene: main.o type.o
	$(CC) main.o type.o -o aifaleene -lreadline -lhistory

type.o: type.c hash.h list.h ibuf.h buf.h aifaleene.h
	$(CC) $(CFLAGS) -c type.c -o type.o
main.o: main.c hash.h list.h ibuf.h buf.h aifaleene.h 
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm aifaleene *.o

.Phony: all clean
