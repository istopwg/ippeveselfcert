dnl
dnl Threading stuff for CUPS.
dnl
dnl Copyright 2007-2017 by Apple Inc.
dnl Copyright 1997-2005 by Easy Software Products, all rights reserved.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more information.
dnl

AC_ARG_ENABLE(threads, [  --disable-threads       disable multi-threading support])

have_pthread=no
PTHREAD_FLAGS=""

if test "x$enable_threads" != xno; then
	AC_CHECK_HEADER(pthread.h, AC_DEFINE(HAVE_PTHREAD_H))

	if test x$ac_cv_header_pthread_h = xyes; then
		dnl Check various threading options for the platforms we support
		for flag in -lpthreads -lpthread -pthread; do
        		AC_MSG_CHECKING([for pthread_create using $flag])
			SAVELIBS="$LIBS"
			LIBS="$flag $LIBS"
        		AC_TRY_LINK([#include <pthread.h>],
				[pthread_create(0, 0, 0, 0);],
        			have_pthread=yes,
				LIBS="$SAVELIBS")
        		AC_MSG_RESULT([$have_pthread])

			if test $have_pthread = yes; then
				PTHREAD_FLAGS="-D_THREAD_SAFE -D_REENTRANT"

				# Solaris requires -D_POSIX_PTHREAD_SEMANTICS to
				# be POSIX-compliant... :(
				if test $host_os_name = sunos; then
					PTHREAD_FLAGS="$PTHREAD_FLAGS -D_POSIX_PTHREAD_SEMANTICS"
				fi
				break
			fi
		done
	fi
fi

AC_SUBST(PTHREAD_FLAGS)
