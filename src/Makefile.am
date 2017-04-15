bin_PROGRAMS = volumed

AM_CXXFLAGS = $(INTI_CFLAGS)

volumed_SOURCES = volumed.c params.c config.c
volumed_LDADD = $(INTI_LIBS)


# Enable make check from this directory.
check-am:
	cd ../tests; $(MAKE) $@
