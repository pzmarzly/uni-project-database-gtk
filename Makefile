CC?=gcc
CFLAGS=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: wez-mnie-gtk
all: wez-mnie-gtk test

WEZ_MNIE_GTK = main.o Repo.o Editor.o Recent.o Welcome.o Utils.o
wez-mnie-gtk: ${WEZ_MNIE_GTK}
	$(CC) ${WEZ_MNIE_GTK} ${LDFLAGS} -o $@

TEST = test.o Repo.o RepoString.o Utils.o
test: ${TEST}
	$(CC) ${TEST} ${LDFLAGS} -o $@

%.o: %.c
	$(CC) ${CFLAGS} -c -MMD $<

.PHONY: clean
clean:
	-rm wez-mnie-gtk
	-rm test
	-rm *.o
	-rm *.d

-include *.d
