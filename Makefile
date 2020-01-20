CC?=gcc
DESTDIR?=./_install
override CFLAGS+=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
override LDFLAGS+=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: wez-mnie-gtk
all: wez-mnie-gtk test

COMMON = Greeter.o RecentList.o
COMMON += Editor.o EditorDialogs.o Datepicker.o
COMMON += EditorSemester.o EditorEquipment.o
COMMON += EditorPeriodicReservation.o
COMMON += EditorReports.o
COMMON += Repo.o RepoData.o RepoString.o
COMMON += RepoLogic.o
COMMON += About.o Utils.o dialog/Dialogs.o

wez-mnie-gtk: wez-mnie-gtk.o ${COMMON} demo.db
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@

test: test.o ${COMMON}
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@

gen-demo: gen-demo.o ${COMMON}
	$(CC) $< ${COMMON} ${LDFLAGS} -o $@
demo.db: gen-demo
	./gen-demo

%.o: %.c
	$(CC) ${CFLAGS} -c -MMD $< -o $@

%.pdf: %.md
	pandoc --pdf-engine=xelatex $< -o $@ \
		-V geometry:"top=1.5cm, bottom=1.5cm, left=1.5cm, right=1.5cm"

.PHONY: all fmt clean docs install
fmt:
	clang-format -i *.c *.h
clean:
	-rm -f wez-mnie-gtk test gen-demo *.o *.d *.pdf demo.db
docs: DESIGN.pdf README.pdf
install: wez-mnie-gtk demo.db
	install -d $(DESTDIR)/
	install -m 755 wez-mnie-gtk $(DESTDIR)/
	install -m 644 demo.db $(DESTDIR)/
	cp -a icons $(DESTDIR)/icons
	cp *.glade $(DESTDIR)/
	install -d $(DESTDIR)/dialog/
	cp dialog/*.glade $(DESTDIR)/dialog/

-include *.d
