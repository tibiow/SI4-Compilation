#
#  Makefile 	-- Makefile for testing the Toy programming language
#
# Copyright © 2015-2017 Erick Gallesio - Polytech Nice-Sophia <eg@unice.fr>
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 19:39 (eg)
# Last file update: 13-Dec-2017 17:45 (eg)
#

SUCCESS=$(sort $(wildcard ok-*.toy))
FAIL=$(sort $(wildcard fail-*.toy))

SUCCESS_EXE=$(SUCCESS:.toy=)
FAIL_EXE   =$(FAIL:.toy=)
ALL_C      =$(SUCCESS:.toy=.c) $(FAIL:.toy=.c)

RUNTIMEDIR =../runtime
RUNTIME    =$(RUNTIMEDIR)/toy-runtime.a

# Compilers
TOY=../src/toy
CC=gcc
CFLAGS=-std=gnu99 -g -Wall -Wextra -Wno-unused-parameter -I$(RUNTIMEDIR)

# ----------------------------------------------------------------------
# Implicit rules
#
%.c:%.toy
	$(TOY) -o $*.c $*.toy 

%:%.c
	@$(CC) $(CFLAGS) -o $* $*.c $(RUNTIME)

all:	$(SUCCESS_EXE)

# c: $(SUCCESS:.toy=.c)

tests: successes failures
	@echo "All (`ls ok-*.toy fail-*.toy| wc -l`) tests passed"

successes: all
	@echo "==== Testing successes ===="
	@for i in $(SUCCESS_EXE) ;do \
	   if ! ./exec-success $$i ;then \
	     exit 1; \
           fi; \
         done

failures:
	@echo "==== Testing failures ===="
	@for i in $(FAIL_EXE); do \
	   if ! ./exec-fail $$i ;then \
	     exit 1; \
           fi; \
         done


clean:
	rm -f $(SUCCESS_EXE) $(ALL_C) *.c *.out *~

distclean: clean
