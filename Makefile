CFLAGS=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: wez-mnie-gtk
all: wez-mnie-gtk

WEZ_MNIE_GTK = main.o Repo.o Editor.o Recent.o Welcome.o Utils.o
wez-mnie-gtk: ${WEZ_MNIE_GTK}
	cc ${WEZ_MNIE_GTK} ${LDFLAGS} -o wez-mnie-gtk

%.o: %.c
	cc ${CFLAGS} -c -MMD $< ${LDFLAGS}

.PHONY: clean
clean:
	-rm wez-mnie-gtk
	-rm *.o
	-rm *.d

-include *.d
