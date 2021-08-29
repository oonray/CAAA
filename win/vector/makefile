COMMAND=cmd.exe /c "F:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_x64 \&\&
PT=F:\\lib

CC=$(COMMAND) cl.exe
NAME=vector
CFLAGS=/Wall /O2 /TC
INCLUDE= /I $(INCDIR)

LIBNAME=build/lib$(NAME).lib
HFILES=$(wildcard src/*.h)$(wildcard src/*/*.h)

CFILES=$(wildcard src/*.c)$(wildcard src/*/*.c)
CBINS=$(wildcard bin/*.c)$(wildcard bin/*/*.c)
CTESTS=$(wildcard tests/*.c)

T=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%.exe,$(T))
FILES=$(patsubst %.c,%,$(CFILES))
BINS=$(patsubst %.c,%.exe,$(CBINS))
SO=$(patsubst %.lib,%.dll,$(LIBNAME))

LIBDIR=$(PT)\\lib
INCDIR=$(PT)\\include\\c
BINDIR=$(PT)\\bin

OBJECTS=$(patsubst %.c,%.obj,$(CFILES))
TOBJECTS=$(patsubst %.c,%.obj,$(CTESTS))

all: build_dirs $(OBJECTS) $(LIBNAME) $(SO) tests

%.obj: %.c 
	$(CC) /c $(CFLAGS) $(INCLUDE) $^ /Fo:$@

%.lib: $(OBJECTS)
	$(COMMAND) lib.exe $^ /OUT:$@

%.dll:CFLAGS+= /link /DLL
%.dll: $(CFILES)
	$(CC) /D_USRDLL /D_WINDLL $^ $(CFLAGS) /OUT:$@


.PHONY: tests 
tests: $(TOBJECTS)
	$(CC) $^ /link $(LIBNAME) /out:$(TESTS)
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
	rm $(OBJECTS)
	rm $(LIBNAME)
	rm $(SO)
