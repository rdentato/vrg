#  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
#  SPDX-License-Identifier: MIT

_EXE=.exe
STATIC=-static

ifeq "$(COMSPEC)" ""
_EXE=
STATIC=
endif

DEBUG=-DDEBUG

SRC=../src
DIST=../dist

CFLAGS= $(XFLAGS) -std=c11 -O2 -Wall -I$(DIST) -I. $(ARCH) $(STATIC) $(DEBUG)
LIBS=

TESTS_SRC=$(wildcard cli_*.c)
TESTS_RAW=$(TESTS_SRC:.c=)
TESTS=$(TESTS_SRC:.c=$(_EXE))

# targets
all: $(TESTS)

MAKEFLAGS += --no-builtin-rules

%.o: %.c $(DIST)/vrg.h $(DIST)/cli.h
	$(CC) $(CFLAGS) -o $*.o -c $< 

%$(_EXE): %.o 
	$(CC) $(ARCH) -s -o $* $< $(LIBS)

%.obj: %.c $(DIST)/vrg.h
	$(CC) $(ARCH) $(CFLAGS) -o $*.obj -c $< 

.PRECIOUS: %.o %.obj

clean:
	rm -f $(TESTS_RAW) $(TESTS_RAW:=.exe) $(TESTS_RAW:=.o) $(TESTS_RAW:=.obj) test.log 

cleanall: clean
