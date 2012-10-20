
CC=gcc
CFLAGS=-W -Wall -ansi -pedantic -g 
LDFLAGS=
EXEC=

test: test.o
	$(CC) $^ -o ../../bin/$@ $(LDFLAGS)

genDB: genDB.o
	$(CC) $^ -o ../../bin/$@ $(LDFLAGS) -lkyotocabinet -lmysqlclient 

getVal: getVal.o
	$(CC) $^ -o ../../bin/$@ $(LDFLAGS) -lkyotocabinet

setVal: setVal.o
	$(CC) $^ -o ../../bin/$@ $(LDFLAGS) -lkyotocabinet

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm *.o

mrproper: clean
	rm $(EXEC)

