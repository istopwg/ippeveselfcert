#!/bin/sh
#
# IPP Everywhere Printer Self-Certification Manual 1.1: Section 7: Document Data Tests.
#
# Copyright © 2014-2022 by the IEEE-ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
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

PLIST="${TARGET} Document Results.plist"

$IPPFIND --literal-name "${TARGET}" -x $IPPTOOL -P "$PLIST" -I '{}' document-tests.test \;

# confirm that the PLIST is well formed, if plutil is available (e.g. running on Darwin / OS X / macOS)
test `which plutil` && plutil -lint -s "${PLIST}"
