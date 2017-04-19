

AC_INIT([volumed], [0.1.0])
AC_CONFIG_SRCDIR(src/volumed.c)
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CC
AC_PROG_LIBTOOL

# For check, the unit test framework.
# see https://crsr.net/Notes/CheckAndAutotools.html
AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for library functions.
AC_FUNC_MALLOC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h] [stdarg.h] [string.h])


AC_OUTPUT(Makefile Doxyfile src/volumed.h)

