#
# Makefile for our C library of utilities
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date:  4-Jan-2015 10:25 (eg)
# Last file update: 11-Jul-2015 20:27 (eg)

CC=gcc
CFLAGS=-Wall -std=gnu99
#CC= clang
#CFLAGS=


SRC = $(wildcard *.c)
OBJC= $(SRC:.c=.o)
LIB=  libutil.a

all: depend $(LIB)

$(LIB): $(OBJC)
	rm -f $(LIB)
	ar cq $(LIB) $(OBJC)
	ranlib $(LIB)

clean:
	rm -f $(OBJC) $(LIB) *~

distclean: clean
	rm -f Makefile.depend

depend:
	gcc -MM $(SRC) > Makefile.depend

sinclude Makefile.depend
