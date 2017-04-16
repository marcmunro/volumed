## Process this file with automake to produce Makefile.in

TESTS = check_volumed 
check_PROGRAMS = check_volumed
check_volumed_SOURCES = check_volumed.c 
#	$(top_builddir)/src/config.c \
#	$(wildcard $(top_builddir)/src/*.h)
check_volumed_CFLAGS = @CHECK_CFLAGS@ 
check_volumed_LDFLAGS = -pthread 
check_volumed_LDADD = $(top_builddir)/src/params.o @CHECK_LIBS@ -lm -lrt

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
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	@echo $(MAKE) $(AM_MAKEFLAGS) check-TESTS
	@am__f_ok () { test -f "$$1" && test -r "$$1"; }; \
	$(am__set_TESTS_bases); \
	redo_logs=`for i in $$bases; do \
	                am__f_ok $$i.log && echo $$i.log; \
	            done`; \
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS | \
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

# Redefine rules for compilation of objects since by default they do
# not properly handle objects in other directories.
# This would probably not be an issue except that I want to be able to
# invoke make check from the tests directory when there may be out of date
# dependencies in the src (or other directories).  This hack simply fixes
# the path for the dependencies.  I suspect this whole issue is another
# example of why recursive make may be considered harmful.
.c.o:
	$(AM_V_CC)$(COMPILE) -MT $@ -MD -MP -MF \
	    `dirname $*`/$(DEPDIR)/`basename $*`.Tpo -c -o $@ $<
	$(AM_V_at)$(am__mv) `dirname $*`/$(DEPDIR)/`basename $*`.Tpo \
	    `dirname $*`/$(DEPDIR)/`basename $*`.Po
