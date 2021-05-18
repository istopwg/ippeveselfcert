#!/bin/sh
#
# IPP Everywhere Printer Self-Certification Manual 1.1: Section 6: IPP Tests.
#
# Copyright 2014-2020 by the IEEE-ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#
# Usage:
#
#   ./ipp-tests.sh "Printer DNS-SD Service Instance Name"
#

NON_INTERACTIVE=0

if test $# -eq 2; then
    if test $1 = "--non-interactive"; then
        NON_INTERACTIVE=1
        shift
    fi
fi

if test $# -ne 1; then
    echo "Usage: ${0} [--non-interactive] \"Printer DNS-SD Service Instance Name\""
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

if test "`ls -d pwg-raster-samples-*dpi 2>/dev/null`" = ""; then
	echo "You must first download and extract the PWG Raster Format sample files from:"
	echo ""
	echo "    https://ftp.pwg.org/pub/pwg/ipp/examples/"
	echo ""
	echo "Before you can run this script."
	exit 1
fi

PLIST="${TARGET} IPP Results.plist"

echo "TARGET=${TARGET}"
echo "IPPFIND=${IPPFIND}"
echo "IPPTOOL=${IPPTOOL}"
echo "PLIST=${PLIST}"
echo "TARGET=${TARGET}"

if test $NON_INTERACTIVE -eq 1; then
    echo "Non-interactive mode"
    "${IPPFIND}" --literal-name "${TARGET}" -x "${IPPTOOL}" -P "${PLIST}" -d NON_INTERACTIVE=1 -I '{}' ipp-tests.test \;
else
    "${IPPFIND}" --literal-name "${TARGET}" -x "${IPPTOOL}" -P "${PLIST}"                      -I '{}' ipp-tests.test \;
fi

# confirm that the PLIST is well formed, if plutil is available (e.g. running on Darwin / OS X / macOS)
test `which plutil` && plutil -lint -s "${PLIST}"
