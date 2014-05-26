
O=tst.o

ZORBA := ../../zorba/zorba-3.0/build/dist

CXXFLAGS=-I$(ZORBA)/include

LIB=-L $(ZORBA)/lib -lzorba_simplestore

tst: $(O)
	$(CXX) $^ -o $@ $(LIB)

clean:
	rm -f *.o tst

run: tst
	LD_LIBRARY_PATH=$(ZORBA)/lib ./tst
