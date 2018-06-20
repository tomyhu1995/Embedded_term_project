FLAGS=-Wall -ggdb -I../../lib 
LDFLAGS=-L../../lib ../../lib/libmosquitto.so.1
include ../../config.mk
.PHONY: all clean

all: my_pub my_sub

my_sub: my_sub.o
	gcc $^ -I./ -o $@ ${LDFLAGS}

my_pub: my_pub.o 
	gcc $^ -o $@ ${LDFLAGS}

my_pub.o: my_publish.c
	gcc -c $^  -o $@ ${CFLAGS}

my_sub.o: my_subscribe.c
	gcc -c $^ -I./ -o $@ ${CFLAGS}

clean:
	rm -f *.o my_pub my_sub
