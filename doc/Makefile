
.PHONY: check all
.SUFFIXES: .3mp .xml .1

prefix = /usr

XSLTPROC_COMPILE = xsltproc --xinclude -path "."
HTML_COMPILE = $(XSLTPROC_COMPILE) id.htmlhelp.xsl
MAN_COMPILE = $(XSLTPROC_COMPILE) id.man.xsl
REFFILES = xquery.xml
MANFILES=xquery.3mp

XMLFILES=book.xml manref.xml

all: $(MANFILES)

index.html: $(XMLFILES)
	$(HTML_COMPILE) book.xml

manref.xml: $(REFFILES)
	rm -f manref.xml
	for i in $(REFFILES); do \
		xsltproc stripref.xsl $$i | sed 1d >>manref.xml; \
        done

.xml.3mp:
	$(MAN_COMPILE) $<

clean:
	rm -f *.html *.pdf manref.xml *.hhp *.hhc $(MANFILES)

distclean: clean

check: all

install:
	mkdir -p $(DESTDIR)$(prefix)/share/man/man3
	cp *.3mp $(DESTDIR)$(prefix)/share/man/man3/

