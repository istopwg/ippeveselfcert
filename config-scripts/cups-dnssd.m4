dnl
dnl DNS Service Discovery (aka Bonjour) stuff for CUPS.
dnl
dnl Copyright 2007-2017 by Apple Inc.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more
dnl information.
dnl

AC_ARG_ENABLE(avahi, [  --disable-avahi         disable DNS Service Discovery support using Avahi])
AC_ARG_ENABLE(dnssd, [  --disable-dnssd         disable DNS Service Discovery support using mDNSResponder])
AC_ARG_WITH(dnssd-libs, [  --with-dnssd-libs       set directory for DNS Service Discovery library],
	LDFLAGS="-L$withval $LDFLAGS"
	DSOFLAGS="-L$withval $DSOFLAGS",)
AC_ARG_WITH(dnssd-includes, [  --with-dnssd-includes   set directory for DNS Service Discovery includes],
	CFLAGS="-I$withval $CFLAGS"
	CPPFLAGS="-I$withval $CPPFLAGS",)

DNSSDLIBS=""
IPPFIND_BIN=""
IPPFIND_MAN=""

if test "x$PKGCONFIG" != x -a x$enable_avahi != xno -a x$host_os_name != xdarwin; then
	AC_MSG_CHECKING(for Avahi)
	if $PKGCONFIG --exists avahi-client; then
		AC_MSG_RESULT(yes)
		CFLAGS="$CFLAGS `$PKGCONFIG --cflags avahi-client`"
		DNSSDLIBS="`$PKGCONFIG --libs avahi-client`"
		IPPFIND_BIN="ippfind"
		IPPFIND_MAN="ippfind.\$(MAN1EXT)"
		AC_DEFINE(HAVE_AVAHI)
	else
		AC_MSG_RESULT(no)
	fi
fi

if test x$enable_dnssd != xno; then
	AC_CHECK_HEADER(dns_sd.h, [
		case "$host_os_name" in
			darwin*)
				# Darwin and macOS...
				AC_DEFINE(HAVE_DNSSD)
				DNSSDLIBS="-framework CoreFoundation -framework SystemConfiguration"
				IPPFIND_BIN="ippfind"
				IPPFIND_MAN="ippfind.\$(MAN1EXT)"
				;;
			*)
				# All others...
				AC_MSG_CHECKING(for current version of dns_sd library)
				SAVELIBS="$LIBS"
				LIBS="$LIBS -ldns_sd"
				AC_TRY_COMPILE([#include <dns_sd.h>],
					[int constant = kDNSServiceFlagsShareConnection;
					unsigned char txtRecord[100];
					uint8_t valueLen;
					TXTRecordGetValuePtr(sizeof(txtRecord),
					    txtRecord, "value", &valueLen);],
					AC_MSG_RESULT(yes)
					AC_DEFINE(HAVE_DNSSD)
					DNSSDLIBS="-ldns_sd"
					IPPFIND_BIN="ippfind"
					IPPFIND_MAN="ippfind.\$(MAN1EXT)"
					AC_MSG_RESULT(no))
				LIBS="$SAVELIBS"
				;;
		esac
	])
fi

AC_SUBST(DNSSDLIBS)
AC_SUBST(IPPFIND_BIN)
AC_SUBST(IPPFIND_MAN)
