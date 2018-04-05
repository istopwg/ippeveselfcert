#!/bin/sh
#
# IPP Everywhere Printer Self-Certification Manual 1.0: Section 6: IPP Tests.
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
#   ./ipp-tests.sh "Printer DNS-SD Service Instance Name"
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

for file in color.jpg; do
	if test ! -f $file -a -f ../test/$file; then
		ln -s ../test/$file .
	fi
done


PLIST="${TARGET} IPP Results.plist"

"${IPPFIND}" --literal-name "${TARGET}" -x "${IPPTOOL}" -P "${PLIST}" -I '{}' ipp-tests.test \;

# confirm that the PLIST is well formed, if plutil is available (e.g. running on Darwin / OS X / macOS)
test `which plutil` && plutil -lint -s "${PLIST}"
