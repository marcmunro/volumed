#
# Basic build stuff
#

ACLOCAL_AMFLAGS = -I m4
bin_PROGRAMS = volumed
volumed_SOURCES = src/volumed.c src/config.c src/params.c

AM_CFLAGS = -g -O2 -Wall

#
# Unit test stuff
#

TESTS = tests/check_volumed
check_PROGRAMS = $(TESTS)

tests_check_volumed_SOURCES = tests/check_volumed.c
tests_check_volumed_LDADD = $(top_builddir)/src/params.o \
	$(top_builddir)/src/config.o @CHECK_LIBS@ #-lm -lrt

# Redefine rules for check-am target so that we can check the output and
# provide a summary.
# NOTES:
#    - The recursive make for check-TESTS has its output sanitised:
#      we do not want to see recipe failed errors as they just clutter
#      the output and make next-error functionality (in eg, emacs)
#      less useful.
#    - The awk script is for printing any debug output from the program
#      being tested.
#
check-am: 
	$(MAKE) $(AM_MAKEFLAGS) --no-print-directory $(check_PROGRAMS)
	@echo $(MAKE) $(AM_MAKEFLAGS) check-TESTS
	@am__f_ok () { test -f "$$1" && test -r "$$1"; }; \
	$(am__set_TESTS_bases); \
	redo_logs=`for i in $$bases; do \
	                am__f_ok $$i.log && echo $$i.log; \
	            done`; \
	$(MAKE) $(AM_MAKEFLAGS)  --no-print-directory check-TESTS | \
	    grep -v '^Makefile:[0-9]*: recipe'; \
	echo; \
	exitcode=0; \
	for i in $$redo_logs; do \
	    awk '/^[0-9]*%: Checks/{exit(0)}; \
		(do_print); \
		 /^Running suite/ {do_print=1} ' $$i; \
	    if ! grep "^100%: Checks" $$i >/dev/null; then \
	        grep "[^:]*.c:[[0-9]*:" $$i | cut -d: -f1-6; \
	        echo; \
		exitcode=1; \
	    fi; \
	    grep "^[0-9]*%: Checks" $$i; \
	    echo; \
	done; \
	exit $$exitcode

coverage:
	@echo REBUILDING FOR COVERAGE TESTS
	@$(MAKE) --no-print-directory clean
	@$(MAKE) $(AM_MAKEFLAGS) --no-print-directory \
	    CFLAGS="$(CFLAGS) -fprofile-arcs -ftest-coverage" \
	    check

#
# Doxygen stuff
#

DX_CLEAN = docs/html docs/latex
DX_DB = docs/doxygen_sqlite3.db docs/doxygen_objdb*tmp

mostlyclean-local:
	-rm -rf $(DX_CLEAN)

clean-local:
	-rm -f $(DX_DB)
	-for d in $(SUBDIRS); do \
	    echo XXXrm -f $$d/*.gcov $$d/*.gcno $$d/*.gcda; \
	    rm -f $$d/*.gcov $$d/*.gcno $$d/*.gcda; \
	done

distclean-local:
	rm -f test-driver

docs:	docs/html

docs/html: Doxyfile $(SOURCES)
	doxygen Doxyfile


.PHONY: clean-local mostlyclean-local docs

