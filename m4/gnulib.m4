# Copyright (C) 2004 Free Software Foundation, Inc.
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# Invoked as: gnulib-tool --import gettext vasnprintf regex atexit save-cwd dirname exit exitfail extensions fnmatch-posix fnmatch mkstemp getopt stdbool getline getnline getndelim2 gethostname strcase getpass-gnu gettimeofday timespec
# Reproduce by: gnulib-tool --import --dir=. --lib=libgnu --source-base=lib --m4-base=m4   alloca alloca-opt atexit dirname exit exitfail extensions fnmatch fnmatch-posix gethostname getline getndelim2 getnline getopt getpass-gnu gettext gettimeofday mkstemp pathmax regex restrict save-cwd stdbool strcase timespec vasnprintf xalloc xgetcwd xsize

AC_DEFUN([gl_EARLY],
[
  AC_GNU_SOURCE
  gl_USE_SYSTEM_EXTENSIONS
])

AC_DEFUN([gl_INIT],
[
  gl_FUNC_ALLOCA
  gl_FUNC_ALLOCA
  gl_FUNC_ATEXIT
  gl_DIRNAME
  gl_EXITFAIL
  dnl gl_USE_SYSTEM_EXTENSIONS must be added quite early to configure.ac.
  # No macro. You should also use one of fnmatch-posix or fnmatch-gnu.
  gl_FUNC_FNMATCH_POSIX
  gl_FUNC_GETHOSTNAME
  AM_FUNC_GETLINE
  gl_GETNDELIM2
  gl_GETNLINE
  gl_GETOPT
  gl_FUNC_GETPASS_GNU
  dnl you must add AM_GNU_GETTEXT([external]) or similar to configure.ac.
  AC_FUNC_GETTIMEOFDAY_CLOBBER
  gl_FUNC_MKSTEMP
  gl_PATHMAX
  gl_REGEX
  gl_C_RESTRICT
  gl_SAVE_CWD
  AM_STDBOOL_H
  gl_STRCASE
  gl_TIMESPEC
  gl_FUNC_VASNPRINTF
  gl_XALLOC
  gl_XGETCWD
  gl_XSIZE
])

dnl Usage: gl_MODULES(module1 module2 ...)
AC_DEFUN([gl_MODULES], [])

dnl Usage: gl_SOURCE_BASE(DIR)
AC_DEFUN([gl_SOURCE_BASE], [])

dnl Usage: gl_M4_BASE(DIR)
AC_DEFUN([gl_M4_BASE], [])

dnl Usage: gl_LIB(LIBNAME)
AC_DEFUN([gl_LIB], [])

dnl Usage: gl_LGPL
AC_DEFUN([gl_LGPL], [])

# gnulib.m4 ends here
