# _inttypes_h.m4 serial 2
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Derek Price.

AC_DEFUN([gl_INTTYPES_H],
[AC_REQUIRE([gl_INCLUDE_NEXT])dnl
AC_LIBSOURCES([inttypes.h])
if test $gl_cv_have_include_next = no; then
  gl_FULL_HEADER_PATH([inttypes.h])
fi
AC_CHECK_DECLS_ONCE([strtoimax strtoumax])dnl
])