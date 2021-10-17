#  Copyright (C) 2021 Benjamin Stürz
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

AC_INIT([microcoreutils], 1.3, [benni@stuerz.xyz], [microcoreutils], [https://github.com/Benni3D/microcoreutils])

AC_PREREQ([2.69])
AC_CONFIG_SRCDIR([src/echo.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.16 foreign subdir-objects -Wall])

AC_SUBST([datarootdir], ['${prefix}/usr/share'])

# Setup default installation paths
AC_PREFIX_DEFAULT([/])

# Check for programs.
AC_PROG_CC

# Check for header files.
AC_CHECK_HEADERS([fcntl.h shadow.h sys/sysmacros.h sys/mount.h unistd.h stdbool.h crypt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_STRUCT_ST_BLOCKS

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_CHECK_FUNCS([getcwd getspnam lchown memmove mkdir putenv realpath
                rmdir sethostname strchr strdup strerror strndup strrchr
                strtol strtoul uname setresuid clearenv])

# Enable extra programs
AC_ARG_ENABLE([extra-progs],
              [AS_HELP_STRING([--enable-extra-progs], [enable extra programs (like halt)])],
              [AM_CONDITIONAL([ENABLE_EXTRA_PROGS], [test x$enableval = xyes])],
              [AM_CONDITIONAL([ENABLE_EXTRA_PROGS], false)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
