CC?=gcc
DESTDIR?=./_install
override CFLAGS+=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
override LDFLAGS+=-lm `pkg-config --libs gtk+-3.0`

default: wez-mnie-gtk
all: wez-mnie-gtk test

DEBUG_REPO?=0
DEBUG_ASAN?=0

ifneq ($(DEBUG_REPO),0)
    override CFLAGS+=-DDEBUG_REPO=1
endif

ifneq ($(DEBUG_ASAN),0)
    override LDFLAGS+=-fstack-protector-all -fsanitize=undefined \
        -fsanitize=address -fno-omit-frame-pointer
endif

COMMON = Greeter.o RecentList.o
COMMON += Editor.o EditorDialogs.o Datepicker.o
COMMON += EditorSemester.o EditorEquipment.o
COMMON += EditorPeriodicReservation.o
COMMON += EditorOneTimeReservation.o
COMMON += EditorReports.o
COMMON += Repo.o RepoData.o RepoString.o
COMMON += RepoLogic.o RepoGenerate.o
COMMON += About.o Utils.o LinkedList.o
COMMON += dialog/Dialogs.o

wez-mnie-gtk: wez-mnie-gtk.o ${COMMON} demo.db
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@

demo.db: gen-demo
	./gen-demo

test: test.o ${COMMON}
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@

gen-demo: gen-demo.o ${COMMON}
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@

%.o: %.c
	$(CC) ${CFLAGS} -c -MMD $< -o $@

.PHONY: default all fmt clean docs install
fmt:
	clang-format -i *.c *.h
clean:
	-rm -f wez-mnie-gtk test gen-demo *.o *.d demo.db
	$(MAKE) -C docs clean
docs:
	$(MAKE) -C docs
install: wez-mnie-gtk demo.db
	install -d $(DESTDIR)/
	install -m 755 wez-mnie-gtk $(DESTDIR)/
	install -m 644 demo.db $(DESTDIR)/
	cp -a icons $(DESTDIR)/icons
	cp *.glade $(DESTDIR)/
	install -d $(DESTDIR)/dialog/
	cp dialog/*.glade $(DESTDIR)/dialog/

-include *.d
