
.PHONY: check all
.SUFFIXES: .3mp .xml .1

prefix = /usr

VERSION=$(shell . ../IDMETA; echo $$VERSION)

XSLTPROC_COMPILE = xsltproc --xinclude -path "."
PDF_COMPILE = dblatex -P latex.class.options=a4paper,12pt,twoside,openright
HTML_COMPILE = $(XSLTPROC_COMPILE) common/id.htmlhelp.xsl
MAN_COMPILE = $(XSLTPROC_COMPILE) common/id.man.xsl
REFFILES = xquery.xml
MANFILES=xquery.3mp

XMLFILES=book.xml manref.xml

all: index.html $(MANFILES)

index.html: $(XMLFILES) local.ent
	$(HTML_COMPILE) book.xml

manref.xml: $(REFFILES) local.ent
	rm -f manref.xml
	for i in $(REFFILES); do \
		xsltproc stripref.xsl $$i | sed 1d >>manref.xml; \
        done

mp-xquery.pdf: $(XMLFILES) local.ent
	rm -f mp-xquery.pdf
	$(PDF_COMPILE) book.xml
	mv book.pdf mp-xquery.pdf

.xml.3mp:
	$(MAN_COMPILE) $<

clean:
	rm -f *.html *.pdf manref.xml *.hhp *.hhc $(MANFILES)

distclean: clean

local.ent: local.ent.in 
	sed "s/@VERSION@/$(VERSION)/" <local.ent.in >local.ent

check: all

install:
	mkdir -p $(DESTDIR)$(prefix)/share/man/man3
	cp *.3mp $(DESTDIR)$(prefix)/share/man/man3/

