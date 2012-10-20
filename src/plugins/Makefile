CC=gcc
CFLAGS=-W -Wall -I../include -ggdb 
LFLAGS=-D_GNU_SOURCE=1 

SRC= $(wildcard *.c)
OBJ= $(addprefix ../../obj/,$(SRC:.c=.o))


../../plugins/ip_blacklist.so: ../../obj/ip_blacklist.o
	gcc -shared -Wl -o $@ $^ $(LFLAGS)

../../obj/%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf ../../obj/*.o

mrproper: clean
	rm -rf $(EXEC)
