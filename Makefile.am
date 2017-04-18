ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests

SOURCES = $(wildcard src/*.[ch])

AM_CFLAGS = -g -O2 -Wall

DX_CLEAN = docs/html
DX_DB = docs/doxygen_sqlite3.db

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

coverage:
	@echo REBUILDING FOR COVERAGE TESTS
	@$(MAKE) clean
	@$(MAKE) $(AM_MAKEFLAGS) \
	    CFLAGS="$(CFLAGS) -fprofile-arcs -ftest-coverage" \
	    check
