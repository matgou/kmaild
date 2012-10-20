CC=gcc
CFLAGS=-W -Wall -I./include -ggdb  
LFLAGS=-D_GNU_SOURCE=1 -lkyotocabinet -ldl -lssl

EXEC=../bin/smtp-kmaild
SRC= $(wildcard *.c)
OBJ= $(addprefix ../obj/,$(SRC:.c=.o))

all: $(EXEC)

$(EXEC): $(OBJ)
	gcc -o $@ $^ $(LFLAGS)

../obj/%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf ../obj/*.o

mrproper: clean
	rm -rf $(EXEC)
