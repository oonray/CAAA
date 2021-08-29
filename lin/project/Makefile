CC=gcc
AR=ar
RANLIB=ranlib
LIBS=
WINLIBS=
CFLAGS=-g -Wall -Wextra -O2 -Isrc -rdynamic
WFLAGS=-g -Wall -Wextra -O2 -Isrc
NAME=
INCLUDE= -I/usr/local/include

LIBNAME=build/lib$(NAME).a
WINLIBNAME=build/lib$(NAME)_win.a
LIBNAME=

HFILES=$(wildcard src/*.h)$(wildcard src/*/*.h)

CFILES=$(wildcard src/*.c)$(wildcard src/*/*.c)
CBINS=$(wildcard bin/*.c)$(wildcard bin/*/*.c)
CTESTS=$(wildcard tests/*_tests.c)

FILES=$(patsubst %.c,%,$(CFILES))
TESTS=$(patsubst %.c,%,$(CTESTS))
BINS=$(patsubst %.c,%.bin,$(CBINS))
EXES=$(patsubst %.c,%.exe,$(CBINS))
SO=$(patsubst %.a,%.so,$(LIBNAME))
SO=

PT?=/usr/local
LIBDIR=$(PT)/lib
INCDIR=$(PT)/include
BINDIR=$(PT)/bin

OBJECTS=$(patsubst %.c,%.o,$(CFILES))
WINOBJECTS=$(patsubst %.c,%_win.o,$(CFILES))

all:build $(OBJECTS) $(WINOBJECTS) $(LIBNAME) $(WINLIBNAME) $(SO) $(BINS) $(EXES) tests

%.o: %.c 
	$(CC) $(CFLAGS) -fPIC $(INCLUDE) -o $@ -c $^ $(LIBS)

%_win.o: CC=x86_64-w64-mingw32-gcc-posix
%_win.o: %.c 
	$(CC) $(WFLAGS) -fPIC $(INCLUDE) -o $@ -c $^ $(WINLIBS)

%.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%_win.a: AR=x86_64-w64-mingw32-gcc-ar-posix
%_win.a: RANLIB=x86_64-w64-mingw32-gcc-ranlib-posix
%_win.a: $(WINOBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%.so: %.a $(OBJECTS)
	$(CC) -shared $(CFLAGS) -o $@ $^

%_tests: $(LIBNAME)
	$(CC) -I tests -o $@ $(CFLAGS) $(INCLUDE) tests/munit.c $@.c $^ $(LIBS)

%.bin: $(OBJECTS) %.c
	$(CC) -o $@ -Wall -O2 -Isrc $(INCLUDE) $^ $(LIBS) 

%.exe: CC=x86_64-w64-mingw32-gcc-posix
%.exe: $(WINOBJECTS) %.c
	$(CC) -o $@ -Wall -O2 -Isrc $(INCLUDE) $^ $(WINLIBS) 

.PHONY: tests 
tests: $(TESTS)
	@bash -c $(TESTS)

build:
	@mkdir -p bin
	@mkdir -p build

install:
	install -t $(LIBDIR) $(LIBNAME) 
	install -t $(INCDIR) $(HFILES)	
	install -t $(BINDIR) $(patsubst %.bin, %, $(BINS))

clean:
	rm $(OBJECTS)
	rm $(LIBNAME)
	rm $(SO)
