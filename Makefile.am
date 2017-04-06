SUBDIRS = src

DX_CLEAN = docs/html
DX_DB = docs/doxygen_sqlite3.db

mostlyclean-local:
	-rm -rf $(DX_CLEAN)

clean-local:
	-rm -f $(DX_DB)

docs:	docs/html

docs/html: Doxyfile
	doxygen Doxyfile

.PHONY: clean-local mostlyclean-local docs
