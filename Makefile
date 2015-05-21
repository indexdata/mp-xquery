SUBDIRS = src doc
.PHONY: $(SUBDIRS) all

all: $(SUBDIRS)

$(SUBDIRS): doc/common
	$(MAKE) -C $@

clean install: doc/common
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@; \
	done

doc/common:
	git submodule init
	git submodule update
