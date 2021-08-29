COMMAND=cmd.exe /c "F:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_x64 \&\&
PT=F:/lib

LIBDIR=$(PT)/lib
INCDIR=$(PT)/include
BINDIR=$(PT)/bin

CC=$(COMMAND) cl.exe
NAME=io
CFLAGS=/w /O2 /TC
INCLUDE= /I $(INCDIR) /I src

LIBNAME=build/lib$(NAME).lib
HFILES=$(wildcard src/*.h)$(wildcard src/*/*.h)
LIBS=$(LIBDIR)/libvector.lib

CFILES=$(wildcard src/*.c)$(wildcard src/*/*.c)
CBINS=$(wildcard bin/*.c)$(wildcard bin/*/*.c)
CTESTS=$(wildcard tests/*.c)

T=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%.exe,$(T))
FILES=$(patsubst %.c,%,$(CFILES))
BINS=$(patsubst %.c,%.exe,$(CBINS))
SO=$(patsubst %.lib,%.dll,$(LIBNAME))


OBJECTS=$(patsubst %.c,%.obj,$(CFILES))
TOBJECTS=$(patsubst %.c,%.obj,$(CTESTS))

all: build_dirs $(OBJECTS) $(LIBNAME) tests


%.obj: %.c 
	$(CC)  $(LIBS) /c $^ $(INCLUDE) /Fo:$@ 

%.lib: $(OBJECTS)
	$(COMMAND) lib.exe $^ /INCLUDE:$(LIBNAME) /INCLUDE:$(LIBS) /OUT:$@

%.dll:CFLAGS+= /link /DLL
%.dll: $(CFILES)
	$(CC) /D_USRDLL /D_WINDLL $^ $(CFLAGS) /link $(LIBNAME) $(LIBS) /OUT:$@

.PHONY: tests 
tests: $(TOBJECTS)
	$(CC) $^ /link $(LIBNAME) $(LIBS) /out:$(TESTS)
	@tests/*.exe

build_dirs:
	@mkdir -p bin
	@mkdir -p build
	@mkdir -p /mnt/f/lib 
	@mkdir -p /mnt/f/lib/include/c

install:
	$(COMMAND) cp $(LIBNAME) $(LIBDIR)
	$(COMMAND) cp $(HFILES) $(INCDIR)/c

clean:
	rm $(OBJECTS)
	rm $(LIBNAME)
	rm $(SO)
