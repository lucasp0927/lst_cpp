IDIR =../include
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC=g++
	CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -Wall
	LIBS=  -lhdf5 -lhdf5_cpp -lm
endif
ifeq ($(UNAME_S),Darwin)
	CC=g++
	CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -Wall
	LIBS=  -lhdf5 -lhdf5_cpp -lm
endif

#CFLAGS= -std=c++11 -O3 -DNDEBUG -I$(IDIR) -I/usr/include -Wall
ODIR=obj
LDIR =../lib
BDIR = ../build

_DEPS = LstReader.hpp Count.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_READ_LST_OBJ = read_lst.o LstReader.o Count.o
READ_LST_OBJ = $(patsubst %,$(ODIR)/%,$(_READ_LST_OBJ))

_PRINT_HEADER_OBJ = print_header.o LstReader.o Count.o
PRINT_HEADER_OBJ = $(patsubst %,$(ODIR)/%,$(_PRINT_HEADER_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_lst: $(READ_LST_OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

print_header: $(PRINT_HEADER_OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
