CFLAGS=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: management
all: management

management: main.o RepoEditor.o RepoRecent.o RepoSelect.o Utils.o
	cc main.o RepoEditor.o RepoRecent.o RepoSelect.o Utils.o ${LDFLAGS} -o management

%.o: %.c %.h
	cc ${CFLAGS} -c $< ${LDFLAGS}

.PHONY: clean
clean:
	-rm management
	-rm *.o
