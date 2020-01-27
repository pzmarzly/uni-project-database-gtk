default: all
all: initial-project-description.pdf user-guide.pdf

%.pdf: %.md
	pandoc --pdf-engine=xelatex $< -o $@ \
		-V geometry:"top=1.5cm, bottom=1.5cm, left=1.5cm, right=1.5cm"

.PHONY: default all clean
clean:
	-rm -f *.pdf
