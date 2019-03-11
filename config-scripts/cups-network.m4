dnl
dnl Networking stuff for CUPS.
dnl
dnl Copyright 2007-2016 by Apple Inc.
dnl Copyright 1997-2005 by Easy Software Products, all rights reserved.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more
dnl information.
dnl

AC_CHECK_HEADER(resolv.h,AC_DEFINE(HAVE_RESOLV_H),,[
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>])
AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(gethostbyaddr, nsl)
AC_SEARCH_LIBS(getifaddrs, nsl, AC_DEFINE(HAVE_GETIFADDRS))
AC_SEARCH_LIBS(hstrerror, nsl socket resolv, AC_DEFINE(HAVE_HSTRERROR))
AC_SEARCH_LIBS(rresvport_af, nsl, AC_DEFINE(HAVE_RRESVPORT_AF))
AC_SEARCH_LIBS(__res_init, resolv bind, AC_DEFINE(HAVE_RES_INIT),
	AC_SEARCH_LIBS(res_9_init, resolv bind, AC_DEFINE(HAVE_RES_INIT),
	AC_SEARCH_LIBS(res_init, resolv bind, AC_DEFINE(HAVE_RES_INIT))))

# Tru64 5.1b leaks file descriptors with these functions; disable until
# we can come up with a test for this...
if test "$host_os_name" != "osf1"; then
	AC_SEARCH_LIBS(getaddrinfo, nsl, AC_DEFINE(HAVE_GETADDRINFO))
	AC_SEARCH_LIBS(getnameinfo, nsl, AC_DEFINE(HAVE_GETNAMEINFO))
fi

AC_CHECK_MEMBER(struct sockaddr.sa_len,,, [#include <sys/socket.h>])
AC_CHECK_HEADER(sys/sockio.h, AC_DEFINE(HAVE_SYS_SOCKIO_H))

CUPS_DEFAULT_DOMAINSOCKET=""

dnl Domain socket support...
AC_ARG_WITH(domainsocket, [  --with-domainsocket     set unix domain socket name],
	default_domainsocket="$withval",
	default_domainsocket="")

if test x$enable_domainsocket != xno -a x$default_domainsocket != xno; then
	if test "x$default_domainsocket" = x; then
		case "$host_os_name" in
			darwin*)
				# Darwin and macOS do their own thing...
				CUPS_DEFAULT_DOMAINSOCKET="$localstatedir/run/cupsd"
				;;
			*)
				# All others use FHS standard...
				CUPS_DEFAULT_DOMAINSOCKET="$CUPS_STATEDIR/cups.sock"
				;;
		esac
	else
		CUPS_DEFAULT_DOMAINSOCKET="$default_domainsocket"
	fi

	CUPS_LISTEN_DOMAINSOCKET="Listen $CUPS_DEFAULT_DOMAINSOCKET"

	AC_DEFINE_UNQUOTED(CUPS_DEFAULT_DOMAINSOCKET, "$CUPS_DEFAULT_DOMAINSOCKET")
else
	CUPS_LISTEN_DOMAINSOCKET=""
fi

AC_SUBST(CUPS_DEFAULT_DOMAINSOCKET)
AC_SUBST(CUPS_LISTEN_DOMAINSOCKET)
