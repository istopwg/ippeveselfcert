dnl
dnl Configuration script for the IPP Everywhere Printer Self-Certification
dnl tools.
dnl
dnl Copyright 2015-2022 by the ISTO Printer Working Group.
dnl Copyright 2007-2019 by Apple Inc.
dnl Copyright 1997-2007 by Easy Software Products, all rights reserved.
dnl
dnl Licensed under Apache License v2.0.  See the file "LICENSE" for more
dnl information.
dnl

dnl We need at least autoconf 2.60...
AC_PREREQ(2.60)

dnl Package name and version...
AC_INIT([IPPEVESELFCERT11], [20220526], [https://github.com/istopwg/ippeveselfcert/issues], [ippeveselfcert], [http://www.pwg.org/ipp/everywhere.html])

dnl IPP Everywhere Printer Self-Certification Manual Version; this is the
dnl filename prefix...
IPPEVESELFCERT_VERSION="ippeveselfcert11"
AC_SUBST(IPPEVESELFCERT_VERSION)

dnl Include common configuration scripts...
sinclude(config-scripts/cups-opsys.m4)
sinclude(config-scripts/cups-common.m4)
sinclude(config-scripts/cups-compiler.m4)
sinclude(config-scripts/cups-network.m4)
sinclude(config-scripts/cups-poll.m4)
sinclude(config-scripts/cups-threads.m4)
sinclude(config-scripts/cups-tls.m4)
sinclude(config-scripts/cups-largefile.m4)
sinclude(config-scripts/cups-dnssd.m4)
sinclude(config-scripts/cups-directories.m4)

dnl Generate a bunch of files...
AC_CONFIG_FILES([
    Makedefs
    tests/dnssd-access-tests.test
    tests/dnssd-tests.bat
    tests/dnssd-tests.sh
    tests/dnssd-value-tests.test
    tests/document-tests.bat
    tests/document-tests.sh
    tests/document-tests.test
    tests/ipp-tests.bat
    tests/ipp-tests.sh
    tests/ipp-tests.test
])
AC_OUTPUT

chmod +x tests/*.sh
