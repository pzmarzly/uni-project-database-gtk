CFLAGS=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: management
all: management

MANAGEMENT_OBJS = main.o Repo.o Editor.o Recent.o Welcome.o Utils.o
management: ${MANAGEMENT_OBJS}
	cc ${MANAGEMENT_OBJS} ${LDFLAGS} -o management

%.o: %.c
	cc ${CFLAGS} -c -MMD $< ${LDFLAGS}

.PHONY: clean
clean:
	-rm management
	-rm *.o
	-rm *.d

-include *.d
