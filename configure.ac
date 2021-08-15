m4_define([mcu_VERSION], [1.0])
AC_INIT([microcoreutils], m4_defn([mcu_VERSION]), [benni@stuerz.xyz], m4_defn([mcu_VERSION]), [https://github.com/Benni3D/microcoreutils])

AC_PREREQ([2.69])
AC_CONFIG_SRCDIR([src/echo.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.16 foreign subdir-objects -Wall])

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
AC_CHECK_FUNCS([getcwd getspnam lchown memmove mkdir putenv realpath
                rmdir sethostname strchr strdup strerror strndup strrchr
                strtol strtoul uname setresuid clearenv])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
