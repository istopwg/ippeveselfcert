#!/bin/sh
#
# IPP Everywhere Printer Self-Certification Manual 1.0: Section 7: Document Data Tests.
#
# Copyright 2014-2018 by The Printer Working Group.
#
# This program may be copied and furnished to others, and derivative works
# that comment on, or otherwise explain it or assist in its implementation may
# be prepared, copied, published and distributed, in whole or in part, without
# restriction of any kind, provided that the above copyright notice and this
# paragraph are included on all such copies and derivative works.
#
# The IEEE-ISTO and the Printer Working Group DISCLAIM ANY AND ALL WARRANTIES,
# WHETHER EXPRESS OR IMPLIED INCLUDING (WITHOUT LIMITATION) ANY IMPLIED
# WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Usage:
#
#   ./document-tests.sh "Printer NamePrinter DNS-SD Service Instance Name"
#

if test $# -ne 1; then
    echo "Usage: ${0} \"Printer DNS-SD Service Instance Name\""
    echo ""
    exit 1
else
    TARGET="${1}"
fi

if test -x ../tools/ippfind; then
	IPPFIND="../tools/ippfind"
elif test -x ./ippfind; then
	IPPFIND="./ippfind"
else
	IPPFIND="ippfind"
fi

if test -x ../tools/ipptool; then
	IPPTOOL="../tools/ipptool"
elif test -x ./ipptool; then
	IPPTOOL="./ipptool"
else
	IPPTOOL="ipptool"
fi

for file in color.jpg document-a4.pdf document-letter.pdf; do
	if test ! -f $file -a -f ../test/$file; then
		ln -s ../test/$file .
	fi
done

if test "`ls -d pwg-raster-samples-*dpi-@PWGRASTER_VERSION@ 2>/dev/null`" = ""; then
	echo "You must first download and extract the PWG Raster Format sample files from:"
	echo ""
	echo "    http://ftp.pwg.org/pub/pwg/ipp/examples/"
	echo ""
	echo "Before you can run this script."
	echo ""
	echo "(Look for the @PWGRASTER_VERSION@ files...)"
	exit 1
fi


PLIST="${TARGET} Document Results.plist"

$IPPFIND --literal-name "${TARGET}" -x $IPPTOOL -P "$PLIST" -I '{}' document-tests.test \;

# confirm that the PLIST is well formed, if plutil is available (e.g. running on Darwin / OS X / macOS)
test `which plutil` && plutil -lint -s "${PLIST}"
