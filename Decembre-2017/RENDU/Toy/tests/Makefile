#
#  Makefile 	-- Makefile for testing the Toy programming language
#
# Copyright © 2015-2017 Erick Gallesio - Polytech Nice-Sophia <eg@unice.fr>
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 19:39 (eg)
# Last file update: 18-Dec-2017 12:13 (eg)
#

SUCCESS=$(sort $(wildcard ok-*.toy))
FAIL=$(sort $(wildcard fail-*.toy))
TRACE=$(sort $(wildcard trace-*.toy))

SUCCESS_EXE=$(SUCCESS:.toy=)
FAIL_EXE   =$(FAIL:.toy=)
TRACE_EXE  =$(TRACE:.toy=)
ALL_C      =$(SUCCESS:.toy=.c) $(FAIL:.toy=.c) $(TRACE:.toy=.c)

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

trace-%.c:trace-%.toy
	$(TOY) -d -o trace-$*.c trace-$*.toy

%:%.c
	@$(CC) $(CFLAGS) -o $* $*.c $(RUNTIME)



all:	$(SUCCESS_EXE)

# c: $(SUCCESS:.toy=.c)

tests: successes failures
	@echo "All (`ls ok-*.toy fail-*.toy | wc -l`) tests passed"

successes: all
	@echo "==== Testing successes ===="
	@for i in $(SUCCESS_EXE) ;do \
	   if ! ./exec-success $$i ;then \
	     exit 1; \
           fi; \
         done

trace: $(TRACE_EXE)
	@echo "==== Testing traced programs ===="
	@for i in $(TRACE_EXE) ;do \
	   if ! ./exec-success $$i ;then \
	     exit 1; \
           fi; \
         done
	@echo "All (`ls trace-*.toy | wc -l`) tests passed"

failures:
	@echo "==== Testing failures ===="
	@for i in $(FAIL_EXE); do \
	   if ! ./exec-fail $$i ;then \
	     exit 1; \
           fi; \
         done


clean:
	rm -f $(SUCCESS_EXE) $(TRACE_EXE) $(ALL_C) *.c *.out *~

distclean: clean
