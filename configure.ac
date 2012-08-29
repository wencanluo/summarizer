#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([Summarizer], [0.1])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE([enable])

AC_CONFIG_SRCDIR([src/distribution.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES(SUM, libxml-2.0 protobuf)
AC_SUBST(SUM_CFLAGS)
AC_SUBST(SUM_LIBS)
ACX_PTHREAD
AC_CXX_STL_HASH

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_STRTOD

AC_CONFIG_SUBDIRS([gtest])
AC_CONFIG_FILES([Makefile src/Makefile summarizer.pc])
AC_OUTPUT
