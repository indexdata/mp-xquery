ROOT=..
ZORBA := $(ROOT)/../zorba/zorba-3.0/build/dist
prefix=/usr
libdir = ${prefix}/lib

ifeq "${MP_CONFIG}" ""
MP_CONFIG := $(shell if test -x $(ROOT)/../metaproxy/metaproxy-config; then echo $(ROOT)/../metaproxy/metaproxy-config; else echo metaproxy-config; fi)
endif

MP_CFLAGS := $(shell $(MP_CONFIG) --cflags)
MP_LIBS := $(shell $(MP_CONFIG) --libs)
MP_SO := metaproxy_filter_xquery.so

ZORBA_LIBS := -L $(ZORBA)/lib -lzorba_simplestore
ZORBA_CFLAGS := -I$(ZORBA)/include $(MP_CFLAGS)
CXXFLAGS := $(ZORBA_CFLAGS) $(MP_CFLAGS) -fPIC

all: tst $(MP_SO)

tst: tst.o
	$(CXX) -Wl,-rpath,$(ZORBA)/lib $^ -o $@ $(ZORBA_LIBS)

$(MP_SO): metaproxy_filter_xquery.o
	$(CXX) -Wl,-rpath,$(ZORBA)/lib -shared $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(MP_LIBS) $(ZORBA_LIBS)

install: $(MP_SO)
	mkdir -p $(DESTDIR)$(libdir)/mp-xquery
	cp $(MP_SO) $(DESTDIR)$(libdir)/mp-xquery

clean:
	rm -f *.o tst $(MP_SO)

run_tst: tst
	./tst

run_mp:
	../../metaproxy/src/metaproxy -c config.xml -X
