CC?=gcc
DESTDIR?=./_install
override CFLAGS+=-ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
override LDFLAGS+=-lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer

default: wez-mnie-gtk
all: wez-mnie-gtk test docs

WEZ_MNIE_GTK = wez-mnie-gtk.o Greeter.o RecentList.o
WEZ_MNIE_GTK += Editor.o EditorDialogs.o
WEZ_MNIE_GTK += EditorSemester.o EditorEquipment.o
WEZ_MNIE_GTK += EditorPeriodicReservation.o
WEZ_MNIE_GTK += EditorReports.o
WEZ_MNIE_GTK += Repo.o RepoData.o RepoString.o
WEZ_MNIE_GTK += About.o Utils.o dialog/Dialogs.o
wez-mnie-gtk: ${WEZ_MNIE_GTK} demo.db
	$(CC) ${WEZ_MNIE_GTK} ${LDFLAGS} -o $@

TEST = test.o Repo.o RepoString.o Utils.o
test: ${TEST}
	$(CC) ${TEST} ${LDFLAGS} -o $@

GEN_DEMO = gen-demo.o Repo.o RepoString.o Utils.o
gen-demo: ${GEN_DEMO}
	$(CC) ${GEN_DEMO} ${LDFLAGS} -o $@
demo.db: gen-demo
	./gen-demo

%.o: %.c
	$(CC) ${CFLAGS} -c -MMD $< -o $@

%.pdf: %.md
	pandoc --pdf-engine=xelatex $< -o $@ \
		-V geometry:"top=1.5cm, bottom=1.5cm, left=1.5cm, right=1.5cm"

.PHONY: fmt clean docs install
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
