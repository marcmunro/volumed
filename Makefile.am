ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests

SOURCES = $(wildcard src/*.[ch])

DX_CLEAN = docs/html
DX_DB = docs/doxygen_sqlite3.db

mostlyclean-local:
	-rm -rf $(DX_CLEAN)

clean-local:
	-rm -f $(DX_DB)

distclean-local:
	rm -f test-driver

docs:	docs/html

docs/html: Doxyfile $(SOURCES)
	doxygen Doxyfile


.PHONY: clean-local mostlyclean-local docs
