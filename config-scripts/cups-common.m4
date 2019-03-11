dnl
dnl Common configuration stuff for CUPS.
dnl
dnl Copyright 2015-2019 by the ISTO Printer Working Group.
dnl Copyright 2007-2019 by Apple Inc.
dnl Copyright 1997-2007 by Easy Software Products, all rights reserved.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more
dnl information.
dnl

dnl Set the name of the config header file...
AC_CONFIG_HEADER(config.h)

dnl Version number information...
CUPS_VERSION="AC_PACKAGE_VERSION"
CUPS_REVISION=""

AC_SUBST(CUPS_VERSION)
AC_SUBST(CUPS_REVISION)
AC_DEFINE_UNQUOTED(CUPS_SVERSION, "AC_PACKAGE_NAME v$CUPS_VERSION$CUPS_REVISION")
AC_DEFINE_UNQUOTED(CUPS_MINIMAL, "AC_PACKAGE_NAME/$CUPS_VERSION$CUPS_REVISION")

dnl Default compiler flags...
CFLAGS="${CFLAGS:=}"
CPPFLAGS="${CPPFLAGS:=}"
CXXFLAGS="${CXXFLAGS:=}"
LDFLAGS="${LDFLAGS:=}"

dnl Checks for programs...
AC_PROG_AWK
AC_PROG_CC(clang cc gcc)
AC_PROG_CPP
AC_PROG_RANLIB
AC_PATH_PROG(AR,ar)
AC_PATH_PROG(CHMOD,chmod)
AC_PATH_PROG(GZIP,gzip)
AC_PATH_PROG(LD,ld)
AC_PATH_PROG(LN,ln)
AC_PATH_PROG(MKDIR,mkdir)
AC_PATH_PROG(MV,mv)
AC_PATH_PROG(RM,rm)
AC_PATH_PROG(RMDIR,rmdir)
AC_PATH_PROG(SED,sed)

if test "x$AR" = x; then
	AC_MSG_ERROR([Unable to find required library archive command.])
fi
if test "x$CC" = x; then
	AC_MSG_ERROR([Unable to find required C compiler command.])
fi

dnl Check for pkg-config, which is used for some other tests later on...
AC_PATH_TOOL(PKGCONFIG, pkg-config)

dnl Check for libraries...
AC_SEARCH_LIBS(abs, m, AC_DEFINE(HAVE_ABS))
AC_SEARCH_LIBS(fmod, m)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER(stdlib.h,AC_DEFINE(HAVE_STDLIB_H))
AC_CHECK_HEADER(langinfo.h,AC_DEFINE(HAVE_LANGINFO_H))
AC_CHECK_HEADER(malloc.h,AC_DEFINE(HAVE_MALLOC_H))
AC_CHECK_HEADER(stdint.h,AC_DEFINE(HAVE_STDINT_H))
AC_CHECK_HEADER(string.h,AC_DEFINE(HAVE_STRING_H))
AC_CHECK_HEADER(strings.h,AC_DEFINE(HAVE_STRINGS_H))
AC_CHECK_HEADER(bstring.h,AC_DEFINE(HAVE_BSTRING_H))
AC_CHECK_HEADER(sys/ioctl.h,AC_DEFINE(HAVE_SYS_IOCTL_H))
AC_CHECK_HEADER(sys/param.h,AC_DEFINE(HAVE_SYS_PARAM_H))
AC_CHECK_HEADER(sys/ucred.h,AC_DEFINE(HAVE_SYS_UCRED_H))

dnl Checks for iconv.h and iconv_open
AC_CHECK_HEADER(iconv.h,
	SAVELIBS="$LIBS"
	LIBS=""
	AC_SEARCH_LIBS(iconv_open,iconv,
		AC_DEFINE(HAVE_ICONV_H)
		SAVELIBS="$SAVELIBS $LIBS")
	AC_SEARCH_LIBS(libiconv_open,iconv,
		AC_DEFINE(HAVE_ICONV_H)
		SAVELIBS="$SAVELIBS $LIBS")
	LIBS="$SAVELIBS")

dnl Checks for statfs and its many headers...
AC_CHECK_HEADER(sys/mount.h,AC_DEFINE(HAVE_SYS_MOUNT_H))
AC_CHECK_HEADER(sys/statfs.h,AC_DEFINE(HAVE_SYS_STATFS_H))
AC_CHECK_HEADER(sys/statvfs.h,AC_DEFINE(HAVE_SYS_STATVFS_H))
AC_CHECK_HEADER(sys/vfs.h,AC_DEFINE(HAVE_SYS_VFS_H))
AC_CHECK_FUNCS(statfs statvfs)

dnl Checks for string functions.
AC_CHECK_FUNCS(strdup strlcat strlcpy)
AC_CHECK_FUNCS(snprintf vsnprintf)

dnl Check for random number functions...
AC_CHECK_FUNCS(random lrand48 arc4random)

dnl Checks for signal functions.
case "$host_os_name" in
	linux* | gnu*)
		# Do not use sigset on Linux or GNU HURD
		;;
	*)
		# Use sigset on other platforms, if available
		AC_CHECK_FUNCS(sigset)
		;;
esac

AC_CHECK_FUNCS(sigaction)

dnl Checks for wait functions.
AC_CHECK_FUNCS(waitpid wait3)

dnl Check for posix_spawn
AC_CHECK_FUNCS(posix_spawn)

dnl See if the tm structure has the tm_gmtoff member...
AC_MSG_CHECKING(for tm_gmtoff member in tm structure)
AC_TRY_COMPILE([#include <time.h>],[struct tm t;
	int o = t.tm_gmtoff;],
	AC_MSG_RESULT(yes)
	AC_DEFINE(HAVE_TM_GMTOFF),
	AC_MSG_RESULT(no))

dnl See if the stat structure has the st_gen member...
AC_MSG_CHECKING(for st_gen member in stat structure)
AC_TRY_COMPILE([#include <sys/stat.h>],[struct stat t;
	int o = t.st_gen;],
	AC_MSG_RESULT(yes)
	AC_DEFINE(HAVE_ST_GEN),
	AC_MSG_RESULT(no))

dnl ZLIB
LIBZ=""
AC_CHECK_HEADER(zlib.h,
    AC_CHECK_LIB(z, gzgets,
	AC_DEFINE(HAVE_LIBZ)
	LIBZ="-lz"
	LIBS="$LIBS -lz"
	AC_CHECK_LIB(z, inflateCopy, AC_DEFINE(HAVE_INFLATECOPY))))
AC_SUBST(LIBZ)

dnl Flags for "ar" command...
case $host_os_name in
        darwin* | *bsd*)
                ARFLAGS="-rcv"
                ;;
        *)
                ARFLAGS="crvs"
                ;;
esac

AC_SUBST(ARFLAGS)

dnl Extra platform-specific libraries...
case $host_os_name in
        darwin*)
                LIBS="-framework SystemConfiguration -framework CoreFoundation -framework Security $LIBS"

		dnl Check for framework headers...
		AC_CHECK_HEADER(ApplicationServices/ApplicationServices.h,AC_DEFINE(HAVE_APPLICATIONSERVICES_H))
		AC_CHECK_HEADER(CoreFoundation/CoreFoundation.h,AC_DEFINE(HAVE_COREFOUNDATION_H))
		;;
esac
