IDIR =../include
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC=g++
endif
ifeq ($(UNAME_S),Darwin)
	CC=g++-7
endif

#CFLAGS= -std=c++11 -O3 -DNDEBUG -I$(IDIR) -I/usr/include -Wall
CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -Wall

ODIR=obj
LDIR =../lib
BDIR = ../build

LIBS= -lm

_DEPS = LstReader.hpp Count.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = read_lst.o LstReader.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_lst: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
