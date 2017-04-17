

AC_INIT([volumed], [0.1.0])
AC_CONFIG_SRCDIR(src/volumed.c)
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CC
AC_PROG_LIBTOOL

# For check, the unit test framework.
# see https://crsr.net/Notes/CheckAndAutotools.html
AC_CONFIG_MACRO_DIR([m4])

AM_PATH_CHECK(,[have_check="yes"],
  AC_MSG_WARN([Check not found; cannot run unit tests!])
  [have_check="no"])
AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

# Checks for library functions.
AC_FUNC_MALLOC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h] [stdarg.h] [string.h])


AC_OUTPUT(Makefile Doxyfile
	  src/Makefile src/volumed.h
	  tests/Makefile)

