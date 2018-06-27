# Makefile for the toy programming language
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 18:19 (eg)
# Last file update: 18-Sep-2015 09:33 (eg)

SUBDIRS = lib src runtime tests
ALLDIRS = $(SUBDIRS) doc

.PHONY: all doc tests clean distclean

all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

tests:  all
	@($(MAKE) -C tests tests)

doc:
	@($(MAKE) -C doc doc)

clean:
	for i in $(ALLDIRS); do $(MAKE) -C $$i clean; done
	rm -f *~

distclean: clean
	for i in $(ALLDIRS); do $(MAKE) -C $$i distclean; done
