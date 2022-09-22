#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([wanhive], [9.0.0], [info@wanhive.com], , [www.wanhive.com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/wanhive-reactor.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])
PKG_INSTALLDIR

# Release year
INCEPTION_YEAR='2022'
AC_SUBST([INCEPTION_YEAR])
AC_DEFINE_UNQUOTED([INCEPTION_YEAR], ["${INCEPTION_YEAR}"], [Release year])

# Library version
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
WH_LIB_VERSION='9:0:0'
AC_SUBST([WH_LIB_VERSION])
AC_DEFINE_UNQUOTED([WH_LIB_VERSION], ["${WH_LIB_VERSION}"], [Version number of library])

# Checks for programs.
AC_PROG_CXX([g++])
AC_PROG_CC([gcc])
## https://www.gnu.org/software/autoconf-archive/ax_cxx_compile_stdcxx.html
AX_CXX_COMPILE_STDCXX(17, [noext], [mandatory])

# Checks for libraries.
LT_INIT
AC_CHECK_LIB([crypto], [BN_clear_free], , [AC_MSG_ERROR([libcrypto not found.])])
AC_CHECK_LIB([pthread], [pthread_create], , [AC_MSG_ERROR([libpthread not found.])])
AC_CHECK_LIB([sqlite3], [sqlite3_open_v2], , [AC_MSG_ERROR([libsqlite3 not found.])])
AC_CHECK_LIB([ssl], [OPENSSL_init_ssl], , [AC_MSG_ERROR([libssl not found.])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netdb.h sys/file.h sys/time.h syslog.h termios.h unistd.h], , [AC_MSG_ERROR([Header files missing.])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_HEADER_MAJOR
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([alarm atexit clock_gettime fchdir ftruncate getcwd gethostname lchown localtime_r memmove memset mkdir pathconf putenv realpath rmdir select setenv socket strcasecmp strchr strdup strrchr uname])

# Conditionally build the additional packages
AC_ARG_WITH([executables], [AS_HELP_STRING([--without-executables], [Do not build executable programs.])])
AS_IF([test "x$with_executables" != "xno"], [AC_CHECK_LIB([pq], [PQfinish], , [AC_MSG_ERROR([libpq not found.])])])
AS_IF([test "x$with_executables" != "xno"], [AC_CHECK_HEADERS([postgresql/libpq-fe.h], , [AC_MSG_ERROR([Additional header file missing.])])])
AM_CONDITIONAL([WH_WITH_EXECUTABLES], [test "x$with_executables" != "xno"])

AC_CONFIG_FILES([Makefile data/Makefile docs/Makefile src/Makefile src/wanhive.pc])
AC_OUTPUT
