COMMAND=cmd.exe /c "F:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_x64 \&\&
NAME=sockets

CC=gcc
WCC=$(COMMAND) cl.exe

AR=ar
RANLIB=ranlib
CFLAGS=-g -Wall -Wextra -O2 -Isrc -rdynamic
WCFLAGS=/Wall /O2 /TC /I src

INCLUDE= -I/usr/local/include -I /usr/include/SDL2
WINCLUDE=$(INCDIR)

LIBNAME=build/lib$(NAME).so
WIBNAME=build/lib$(NAME).lib

HFILES=$(wildcard src/*.h)$(wildcard src/*/*.h)

CFILES=$(wildcard src/*.c)$(wildcard src/*/*.c)
CBINS=$(wildcard bin/*.c)$(wildcard bin/*/*.c)
CTESTS=$(wildcard tests/munit.c) $(wildcard tests/*_tests.c)

FILES=$(patsubst %.c,%,$(CFILES))

LSO=$(patsubst %.a,%.so,$(LIBNAME))
WSO=$(patsubst %.lib,%.dll,$(LIBNAME))

TESTS=$(wildcard tests/*_tests.c)
ATESTS=$(wildcard tests/*.c)
LTESTS=$(patsubst %.c,%,$(TESTS))
WTESTS=$(patsubst %.c,%.exe,$(TESTS))

LBINS=$(patsubst %.c,%.bin,$(CBINS))
WBINS=$(patsubst %.c,%.exe,$(CBINS))

LPT?=/usr/local
WPT=F:\\lib

LLIBDIR=$(LPT)/lib
LINCDIR=$(LPT)/include/c

BINDIR=$(WPT)/bin
WLIBDIR=$(WPT)\\lib
WINCDIR=$(WPT)\\include\\c
WBINDIR=$(WPT)\\bin

LOBJECTS=$(patsubst %.c,%.o,$(CFILES))
WOBJECTS=$(patsubst %.c,%.obj,$(CFILES))

LTOBJECTS=$(patsubst %.c,%.o,$(CTESTS))
WTOBJECTS=$(patsubst %.c,%.obj,$(CTESTS))

all: build_dirs $(LOBJECTS) $(WOBJECTS) tests

#-------------------------------------

%.o: %.c 
	$(CC) $(CFLAGS) -fPIC $(INCLUDE) -o $@ -c $^ $(LIBS)

%.obj: %.c 
	$(WCC) /c $(WCFLAGS) /w $(WINCLUDE) $^ /Fo:$@

#-------------------------------------

%.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%.lib: $(OBJECTS)
	$(COMMAND) lib.exe $^ /OUT:$@

#-------------------------------------

%.so: %.a $(OBJECTS)
	$(CC) -shared $(CFLAGS) -o $@ $^

%.dll:CFLAGS+= /link /DLL
%.dll: $(CFILES)
	$(CC) /D_USRDLL /D_WINDLL $^ $(CFLAGS) /OUT:$@

#-------------------------------------

.PHONY: tests 
tests: $(WTOBJECTS)
	#$(CC) $^ $(LIBNAME) 
	$(WCC) $^ $(WCFLAGS) /link $(WLIBNAME) /out:$(TESTS)
	@tests/*.exe

build_dirs:
	@mkdir -p bin
	@mkdir -p build
	@mkdir -p /mnt/f/lib 
	@mkdir -p /mnt/f/lib/include/c

install:
	$(COMMAND) cp $(LIBNAME) $(LIBDIR)
	$(COMMAND) cp $(HFILES) $(INCDIR)

clean:
	find -name "*.o*" -exec rm -rf {} \;
	rm $(LIBNAME)
	rm $(SO)





#OBJECTS=$(patsubst %.c,%.o,$(CFILES))
#WINOBJECTS=$(patsubst %.c,%_win.o,$(CFILES))

#all:build $(OBJECTS) $(WINOBJECTS) $(LIBNAME) $(WINLIBNAME) $(SO) $(BINS) $(EXES) tests
#%_tests: $(LIBNAME)
#	$(CC) -I tests -o $@ $(CFLAGS) $(INCLUDE) tests/munit.c $@.c $^ $(LIBS)

##%.bin: $(OBJECTS) $(CBINS)
##	$(CC) -o $@ -Wall -O2 -Isrc $(INCLUDE) $^ $(LIBS) 

##%.exe: CC=x86_64-w64-mingw32-gcc-posix
##%.exe: $(WINOBJECTS) $(CBINS)
##	$(CC) -o $@ -Wall -O2 -Isrc $(INCLUDE) $^ $(WINLIBS) 

##.PHONY: tests 
##tests: $(TESTS)
##	@bash -c $(TESTS)

##build:
##	@mkdir -p bin
##	@mkdir -p build

##install:
##	install -t $(LIBDIR) $(LIBNAME)
##	install -t $(INCDIR) $(HFILES)	
##	#install -t $(BINDIR) $(patsubst %.bin, %, $(BINS))

##clean:
##	rm $(OBJECTS)
##	rm $(LIBNAME)
##	rm $(SO)
