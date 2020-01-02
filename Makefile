CC?=gcc
override CFLAGS+=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
override LDFLAGS+=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: wez-mnie-gtk
all: wez-mnie-gtk test gen-demo

WEZ_MNIE_GTK = main.o Greeter.o RecentList.o
WEZ_MNIE_GTK += Editor.o EditorEquipment.o
WEZ_MNIE_GTK += EditorReports.o EditorSemester.o
WEZ_MNIE_GTK += Repo.o RepoData.o RepoString.o
WEZ_MNIE_GTK += Utils.o
wez-mnie-gtk: ${WEZ_MNIE_GTK}
	$(CC) ${WEZ_MNIE_GTK} ${LDFLAGS} -o $@

TEST = test.o Repo.o RepoString.o Utils.o
test: ${TEST}
	$(CC) ${TEST} ${LDFLAGS} -o $@

GEN_DEMO = gen-demo.o Repo.o RepoString.o Utils.o
gen-demo: ${GEN_DEMO}
	$(CC) ${GEN_DEMO} ${LDFLAGS} -o $@

%.o: %.c
	$(CC) ${CFLAGS} -c -MMD $<

.PHONY: clean
clean:
	-rm -f wez-mnie-gtk test gen-demo *.o *.d

-include *.d
