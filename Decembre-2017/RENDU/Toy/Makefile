# Makefile for the toy programming language
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 18:19 (eg)
# Last file update: 18-Dec-2017 13:46 (eg)

SUBDIRS = lib src runtime tests
ALLDIRS = $(SUBDIRS) doc

.PHONY: all doc tests clean distclean archive

all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

tests trace:  all
	@($(MAKE) -C tests $@)

doc:
	@($(MAKE) -C doc doc)

archive:
	git archive --format tar.gz --prefix Toy-full/ -o /tmp/Toy-full.tar.gz master

clean:
	for i in $(ALLDIRS); do $(MAKE) -C $$i clean; done
	rm -f *~

distclean: clean
	for i in $(ALLDIRS); do $(MAKE) -C $$i distclean; done
