dnl
dnl Large file support stuff for CUPS.
dnl
dnl Copyright 2007-2011 by Apple Inc.
dnl Copyright 1997-2005 by Easy Software Products, all rights reserved.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more
dnl information.
dnl

dnl Check for largefile support...
AC_SYS_LARGEFILE

dnl Define largefile options as needed...
LARGEFILE=""
if test x$enable_largefile != xno; then
	LARGEFILE="-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE"

	if test x$ac_cv_sys_large_files = x1; then
		LARGEFILE="$LARGEFILE -D_LARGE_FILES"
	fi

	if test x$ac_cv_sys_file_offset_bits = x64; then
		LARGEFILE="$LARGEFILE -D_FILE_OFFSET_BITS=64"
	fi
fi
AC_SUBST(LARGEFILE)

dnl Check for "long long" support...
AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
	[if test "$GCC" = yes; then
		ac_cv_c_long_long=yes
	else
		AC_TRY_COMPILE(,[long long int i;],
			ac_cv_c_long_long=yes,
			ac_cv_c_long_long=no)
	fi])

if test $ac_cv_c_long_long = yes; then
	AC_DEFINE(HAVE_LONG_LONG)
fi

AC_CHECK_FUNC(strtoll, AC_DEFINE(HAVE_STRTOLL))
