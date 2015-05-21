SUBDIRS = src doc
.PHONY: $(SUBDIRS) all

all: $(SUBDIRS)

CDEP=doc/common/common.ent

$(SUBDIRS): $(CDEP)
	$(MAKE) -C $@

clean install: $(CDEP)
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

$(CDEP):
	git submodule init
	git submodule update
