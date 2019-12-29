CFLAGS=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: management
all: management

management: main.o
	cc main.o ${LDFLAGS} -o management

main.o: main.c
	cc ${CFLAGS} -c main.c ${LDFLAGS}

.PHONY: clean
clean:
	-rm management
	-rm main.o
