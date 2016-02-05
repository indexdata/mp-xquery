SUBDIRS = src doc
.PHONY: $(SUBDIRS) all
prefix = /usr
datadir = $(prefix)/share
pkgdatadir = $(datadir)/mp-xquery

CDEP=doc/common/common.ent

all: $(SUBDIRS)

$(SUBDIRS): $(CDEP)
	$(MAKE) -C $@

clean check: $(CDEP)
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

dist:
	./mkdist.sh

install:
	mkdir -p $(DESTDIR)$(pkgdatadir)/bibframe
	cp -r bibframe $(DESTDIR)$(pkgdatadir)
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

$(CDEP):
	git submodule init
	git submodule update
