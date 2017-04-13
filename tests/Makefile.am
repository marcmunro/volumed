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
check-am: 
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	@echo $(MAKE) $(AM_MAKEFLAGS) check-TESTS
	@$(am__set_TESTS_bases); \
	redo_bases=`for i in $$bases; do \
	                am__f_ok $$i.trs && am__f_ok $$i.log || echo $$i; \
	            done`; \
	if test -n "$$redo_bases"; then \
	    redo_logs=`for i in $$redo_bases; do echo $$i.log; done`; \
	    redo_results=`for i in $$redo_bases; do echo $$i.trs; done`; \
	fi; \
	if $(MAKE) $(AM_MAKEFLAGS) check-TESTS; then \
	    echo; grep "^[0-9]*%: Checks" $$redo_logs; echo; \
	else \
	    echo; grep "[^:]*.c:[[0-9]*:" $(TEST_SUITE_LOG) | cut -d: -f1-6; \
	    echo; grep "^[0-9]*%: Checks" $(TEST_SUITE_LOG); echo; \
	    exit 1; \
	fi

