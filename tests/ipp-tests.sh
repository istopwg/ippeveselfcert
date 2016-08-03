#!/bin/sh
#
#  "$Id: ipp-tests.sh 12897 2015-10-09 19:18:39Z msweet $"
#
# IPP Everywhere Printer Self-Certification Manual 1.0: Section 6: IPP Tests.
#
# Copyright 2014-2015 by The Printer Working Group.
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

if test $# -ne 1; then
    echo "Usage: ${0} \"Printer DNS-SD Service Instance Name\""
    echo "Usage: ${0} \"Printer IPP URI\""
    echo ""
    exit 1
else
    TARGET="${1}"
    # check to see if the argument $1 is a URI or a service name
    echo "${1}" | grep -q 'ipp://'
    if test $? -eq 0; then
        TARGET_IS_URI=1
        TARGETNAME=`echo -n "${TARGET}" | cut -d '/' -f 3`
    else
        TARGET_IS_URI=0
        TARGETNAME="${TARGET}"
    fi
fi


if test -x ../test/ippfind-static; then
	IPPFIND="../test/ippfind-static"
elif test -x ./ippfind; then
	IPPFIND="./ippfind"
else
	IPPFIND="ippfind"
fi

if test -x ../test/ipptool-static; then
	IPPTOOL="../test/ipptool-static"
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


PLIST="${TARGETNAME} IPP Results $(date +'%Y%m%d%H%M').plist"


if test ${TARGET_IS_URI} -eq 1; then
    "${IPPTOOL}" -P "${PLIST}" -I "${TARGET}" ipp-tests.test
else
    "${IPPFIND}" --name "^${TARGET}\$" "_ipp._tcp.local." -x "${IPPTOOL}" -P "${PLIST}" -I '{}' ipp-tests.test \;
fi

# confirm that the PLIST is well formed, if plutil is available (e.g. running on Darwin / OS X / macOS)
test `which plutil` && plutil -lint -s "${PLIST}"


#
# End of "$Id: ipp-tests.sh 12897 2015-10-09 19:18:39Z msweet $".
#
