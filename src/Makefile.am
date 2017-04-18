bin_PROGRAMS = volumed

AM_CFLAGS = -g -O2 -Wall

volumed_SOURCES = volumed.c params.c config.c
volumed_LDADD = $(INTI_LIBS)


# Enable make check from this directory.
check-am:
	cd ../tests; $(MAKE) $@
