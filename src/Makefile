IDIR =../include
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OS := $(shell lsb_release -si)
endif
ifeq ($(UNAME_S),Linux)
	CC=g++
	CXX=g++
	CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -Wall -fopenmp -Wno-sign-compare
	LIBS= -lhdf5 -lhdf5_cpp -lm -lboost_program_options -lyaml-cpp -lboost_iostreams -lboost_filesystem -lboost_system
endif
ifeq ($(OS),Ubuntu)
	CC=g++
	CXX=h5c++
	CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -I/usr/include/hdf5/serial/  -Wall -fopenmp -Wno-sign-compare
	LIBS= -lhdf5_cpp  -lhdf5 -lm -lboost_program_options -lyaml-cpp -lboost_iostreams -lboost_filesystem -lboost_system
endif
ifeq ($(UNAME_S),Darwin)
	CC=/usr/local/opt/llvm/bin/clang++
	CXX=/usr/local/opt/llvm/bin/clang++
	CFLAGS= -std=c++11 -O3 -I$(IDIR) -I/usr/include -Wall -fopenmp -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/
	LIBS= -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib -lhdf5 -lhdf5_cpp -lm -lboost_program_options -lyaml-cpp -lboost_iostreams -lboost_filesystem
endif

#CFLAGS= -std=c++11 -O3 -DNDEBUG -I$(IDIR) -I/usr/include -Wall
ODIR=obj
LDIR =../lib
BDIR = ../build

_DEPS = LstReader.hpp Count.hpp fileio.hpp mainroutine.hpp parseoption.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_READ_LST_OBJ = read_lst.o LstReader.o big_time.o phase.o pulse.o gtwo.o select.o Count.o fileio.o mainroutine.o
READ_LST_OBJ = $(patsubst %,$(ODIR)/%,$(_READ_LST_OBJ))

_PRINT_HEADER_OBJ = print_header.o LstReader.o big_time.o phase.o pulse.o select.o Count.o
PRINT_HEADER_OBJ = $(patsubst %,$(ODIR)/%,$(_PRINT_HEADER_OBJ))

_REMOVE_ZERO_OBJ = remove_zero.o LstReader.o big_time.o phase.o pulse.o select.o Count.o
REMOVE_ZERO_OBJ = $(patsubst %,$(ODIR)/%,$(_REMOVE_ZERO_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

read_lst: $(READ_LST_OBJ)
	$(CXX) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

print_header: $(PRINT_HEADER_OBJ)
	$(CXX) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

remove_zero: $(REMOVE_ZERO_OBJ)
	$(CXX) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
