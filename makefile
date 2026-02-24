CC=gcc
CFLAGS=-Wall -g

all: stage1exe

stage1exe: driver.o lexer.o parser.o
	$(CC) $(CFLAGS) -o stage1exe driver.o lexer.o parser.o

driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c driver.c

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h parserDef.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f *.o stage1exe
