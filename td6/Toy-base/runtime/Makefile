#
# Makefile for toy runtime
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 19:01 (eg)
# Last file update:  2-Jul-2015 10:05 (eg)

CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 

LIBNAME=toy-runtime.a

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(LIBNAME)

$(OBJ): toy-runtime.h

$(LIBNAME): $(OBJ)
	ar -cr $(LIBNAME) $(OBJ)
	ranlib $(LIBNAME)
clean:
	rm -f *~ $(OBJ)

distclean: clean
	rm -f $(LIBNAME)
