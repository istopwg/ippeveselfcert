dnl
dnl Compiler stuff for CUPS.
dnl
dnl Copyright 2007-2014 by Apple Inc.
dnl Copyright 1997-2007 by Easy Software Products, all rights reserved.
dnl
dnl These coded instructions, statements, and computer programs are the
dnl property of Apple Inc. and are protected by Federal copyright
dnl law.  Distribution and use rights are outlined in the file "LICENSE.txt"
dnl which should have been included with this file.  If this file is
dnl missing or damaged, see the license at "http://www.cups.org/".
dnl

dnl Clear the debugging and non-shared library options unless the user asks
dnl for them...
INSTALL_STRIP=""
OPTIM=""
AC_SUBST(INSTALL_STRIP)
AC_SUBST(OPTIM)

AC_ARG_WITH(optim, [  --with-optim            set optimization flags ])
AC_ARG_ENABLE(debug, [  --enable-debug          build with debugging symbols])
AC_ARG_ENABLE(debug_guards, [  --enable-debug-guards   build with memory allocation guards])
AC_ARG_ENABLE(debug_printfs, [  --enable-debug-printfs  build with CUPS_DEBUG_LOG support])
AC_ARG_ENABLE(unit_tests, [  --enable-unit-tests     build and run unit tests])

dnl For debugging, keep symbols, otherwise strip them...
if test x$enable_debug = xyes; then
	OPTIM="-g"
else
	INSTALL_STRIP="-s"
fi

dnl Debug printfs can slow things down, so provide a separate option for that
if test x$enable_debug_printfs = xyes; then
	CFLAGS="$CFLAGS -DDEBUG"
	CXXFLAGS="$CXXFLAGS -DDEBUG"
fi

dnl Debug guards use an extra 4 bytes for some structures like strings in the
dnl string pool, so provide a separate option for that
if test x$enable_debug_guards = xyes; then
	CFLAGS="$CFLAGS -DDEBUG_GUARDS"
	CXXFLAGS="$CXXFLAGS -DDEBUG_GUARDS"
fi

dnl Unit tests take up time during a compile...
if test x$enable_unit_tests = xyes; then
	UNITTESTS="unittests"
else
	UNITTESTS=""
fi
AC_SUBST(UNITTESTS)

dnl Setup general architecture flags...
AC_ARG_WITH(archflags, [  --with-archflags        set default architecture flags ])
AC_ARG_WITH(ldarchflags, [  --with-ldarchflags      set program architecture flags ])

if test -z "$with_archflags"; then
	ARCHFLAGS=""
else
	ARCHFLAGS="$with_archflags"
fi

if test -z "$with_ldarchflags"; then
	if test "$uname" = Darwin; then
		# Only create Intel programs by default
		LDARCHFLAGS="`echo $ARCHFLAGS | sed -e '1,$s/-arch ppc64//'`"
	else
		LDARCHFLAGS="$ARCHFLAGS"
	fi
else
	LDARCHFLAGS="$with_ldarchflags"
fi

AC_SUBST(ARCHFLAGS)
AC_SUBST(LDARCHFLAGS)

dnl Read-only data/program support on Linux...
AC_ARG_ENABLE(relro, [  --enable-relro          build with the GCC relro option])

dnl Update compiler options...
CXXLIBS="${CXXLIBS:=}"
AC_SUBST(CXXLIBS)

PIEFLAGS=""
AC_SUBST(PIEFLAGS)

RELROFLAGS=""
AC_SUBST(RELROFLAGS)

if test -n "$GCC"; then
	# Add GCC-specific compiler options...
	if test -z "$OPTIM"; then
		if test "x$with_optim" = x; then
			# Default to optimize-for-size and debug
       			OPTIM="-Os -g"
		else
			OPTIM="$with_optim $OPTIM"
		fi
	fi

	# The -fstack-protector option is available with some versions of
	# GCC and adds "stack canaries" which detect when the return address
	# has been overwritten, preventing many types of exploit attacks.
	AC_MSG_CHECKING(whether compiler supports -fstack-protector)
	OLDCFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS -fstack-protector"
	AC_TRY_LINK(,,
		if test "x$LSB_BUILD" = xy; then
			# Can't use stack-protector with LSB binaries...
			OPTIM="$OPTIM -fno-stack-protector"
		else
			OPTIM="$OPTIM -fstack-protector"
		fi
		AC_MSG_RESULT(yes),
		AC_MSG_RESULT(no))
	CFLAGS="$OLDCFLAGS"

	if test "x$LSB_BUILD" != xy; then
		# The -fPIE option is available with some versions of GCC and
		# adds randomization of addresses, which avoids another class of
		# exploits that depend on a fixed address for common functions.
		#
		# Not available to LSB binaries...
		AC_MSG_CHECKING(whether compiler supports -fPIE)
		OLDCFLAGS="$CFLAGS"
		case "$uname" in
			Darwin*)
				CFLAGS="$CFLAGS -fPIE -Wl,-pie"
				AC_TRY_COMPILE(,,[
					PIEFLAGS="-fPIE -Wl,-pie"
					AC_MSG_RESULT(yes)],
					AC_MSG_RESULT(no))
				;;

			*)
				CFLAGS="$CFLAGS -fPIE -pie"
				AC_TRY_COMPILE(,,[
					PIEFLAGS="-fPIE -pie"
					AC_MSG_RESULT(yes)],
					AC_MSG_RESULT(no))
				;;
		esac
		CFLAGS="$OLDCFLAGS"
	fi

	if test "x$with_optim" = x; then
		# Add useful warning options for tracking down problems...
		OPTIM="-Wall -Wno-format-y2k -Wunused $OPTIM"

		AC_MSG_CHECKING(whether compiler supports -Wno-unused-result)
		OLDCFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Wno-unused-result"
		AC_TRY_COMPILE(,,
			[OPTIM="$OPTIM -Wno-unused-result"
			AC_MSG_RESULT(yes)],
			AC_MSG_RESULT(no))
		CFLAGS="$OLDCFLAGS"

		AC_MSG_CHECKING(whether compiler supports -Wsign-conversion)
		OLDCFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Wsign-conversion"
		AC_TRY_COMPILE(,,
			[OPTIM="$OPTIM -Wsign-conversion"
			AC_MSG_RESULT(yes)],
			AC_MSG_RESULT(no))
		CFLAGS="$OLDCFLAGS"

		AC_MSG_CHECKING(whether compiler supports -Wno-tautological-compare)
		OLDCFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -Werror -Wno-tautological-compare"
		AC_TRY_COMPILE(,,
			[OPTIM="$OPTIM -Wno-tautological-compare"
			AC_MSG_RESULT(yes)],
			AC_MSG_RESULT(no))
		CFLAGS="$OLDCFLAGS"

		# Additional warning options for development testing...
		if test -d .git; then
			OPTIM="-Werror $OPTIM"
		fi
	fi

	case "$uname" in
		Darwin*)
			# -D_FORTIFY_SOURCE=2 adds additional object size
			# checking, basically wrapping all string functions
			# with buffer-limited ones.  Not strictly needed for
			# CUPS since we already use buffer-limited calls, but
			# this will catch any additions that are broken.
			CFLAGS="$CFLAGS -D_FORTIFY_SOURCE=2"

			# -mmacosx-version-min=10.10 tells the compiler to
			# build for macOS 10.10 or later.
			CFLAGS="$CFLAGS -mmacosx-version-min=10.10"
			;;

		Linux*)
			# The -z relro option is provided by the Linux linker command to
			# make relocatable data read-only.
			if test x$enable_relro = xyes; then
				RELROFLAGS="-Wl,-z,relro,-z,now"
			fi
			;;
	esac
else
	# Add vendor-specific compiler options...
	case $uname in
		SunOS*)
			# Solaris
			if test -z "$OPTIM"; then
				if test "x$with_optim" = x; then
					OPTIM="-xO2"
				else
					OPTIM="$with_optim $OPTIM"
				fi
			fi

			if test $PICFLAG = 1; then
				OPTIM="-KPIC $OPTIM"
			fi
			;;
		*)
			# Running some other operating system; inform the user they
			# should contribute the necessary options to
			# cups-support@cups.org...
			echo "Building CUPS with default compiler optimizations; contact"
			echo "cups-devel@cups.org with uname and compiler options needed"
			echo "for your platform, or set the CFLAGS and LDFLAGS environment"
			echo "variables before running configure."
			;;
	esac
fi

# Add general compiler options per platform...
case $uname in
	Linux*)
		# glibc 2.8 and higher breaks peer credentials unless you
		# define _GNU_SOURCE...
		OPTIM="$OPTIM -D_GNU_SOURCE"
		;;
esac
