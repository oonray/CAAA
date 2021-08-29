CC=gcc
AR=ar
RANLIB=ranlib
LIBS=-lvector
CFLAGS=-g -Wall -Wextra -O2 -Isrc -rdynamic
NAME=stack
INCLUDE= -I/usr/local/include

LIBNAME=build/lib$(NAME).a

HFILES=$(wildcard src/*.h)$(wildcard src/*/*.h)

CFILES=$(wildcard src/*.c)$(wildcard src/*/*.c)
CBINS=$(wildcard bin/*.c)$(wildcard bin/*/*.c)
CTESTS=$(wildcard tests/*_tests.c)

FILES=$(patsubst %.c,%,$(CFILES))
TESTS=$(patsubst %.c,%,$(CTESTS))
BINS=$(patsubst %.c,%.bin,$(CBINS))
SO=$(patsubst %.a,%.so,$(LIBNAME))

PT?=/usr/local
LIBDIR=$(PT)/lib
INCDIR=$(PT)/include
BINDIR=$(PT)/bin

OBJECTS=$(patsubst %.c,%.o,$(CFILES))

all:build $(OBJECTS) $(LIBNAME) $(SO) $(BINS) tests

%.o: %.c 
	$(CC) $(CFLAGS) -fPIC $(LININCLUDE) -o $@ -c $^

%.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%.so: %.a $(OBJECTS)
	$(CC) -shared $(CFLAGS) -o $@ $^

%_tests: $(LIBNAME)
	$(CC) -I tests -o $@ $(CFLAGS) $(INCLUDE) tests/munit.c $@.c $^ $(LIBS)

%.bin: $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS) 

.PHONY: tests 
tests: $(TESTS)
	@bash -c $(TESTS)

build:
	@mkdir -p bin
	@mkdir -p build

install:
	install -t $(INCDIR) $(HFILES)	
	#install -t $(BINDIR) $(patsubst %.bin, %, $(BINS))

clean:
	rm $(OBJECTS)
	rm $(LIBNAME)
	rm $(SO)
