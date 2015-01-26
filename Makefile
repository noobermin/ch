INCLUDE=-Ich
WARNINGS=-Wall -Wno-return-type -Wno-implicit-int 
CFLAGS=-std=c11 $(WARNINGS) -g $(INCLUDE)
LIBS=-lreadline -lhistory -lm


all: aifaleene

aifaleene: main.o type.o
	$(CC) main.o type.o -o aifaleene $(LIBS)

type.o: type.c ch/hash.h ch/list.h ch/ibuf.h ch/buf.h ch/misc.h aifaleene.h
	$(CC) $(CFLAGS) -c type.c -o type.o
main.o: main.c ch/hash.h ch/list.h ch/ibuf.h ch/buf.h ch/misc.h aifaleene.h 
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm aifaleene *.o

.Phony: all clean
