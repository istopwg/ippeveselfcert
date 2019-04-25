#!/bin/sh
#
# Make an IPP Everywhere Printer self-certification package.
#
# Copyright 2014-2019 by the ISTO Printer Working Group.
# Copyright 2007-2019 by Apple Inc.
# Copyright 1997-2007 by Easy Software Products, all rights reserved.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

# Make sure we are running in the right directory...
if test ! -f scripts/make-ippeveselfcert.sh; then
        echo "Run this script from the top-level source directory, e.g.:"
        echo ""
        echo "    scripts/make-ippeveselfcert.sh $*"
        echo ""
        exit 1
fi

if test $# -lt 1 -o $# -gt 3; then
	echo "Usage: everywhere/make-ippeveselfcert.sh ippeveselfcertNN [yyyymmdd] [platform]"
	exit 1
fi

pkgname="$1"
if test $# -gt 1; then
	fileversion="$2"
else
	fileversion="`date +%Y%m%d`"
fi
if test $# = 3; then
	platform="$3"
else
	case `uname` in
		Darwin)
			platform="macos"
			;;

		Linux)
			if test -x /usr/bin/dpkg; then
				platform="ubuntu"
			else
				platform="rhel"
			fi
			;;

		*)
			platform=`uname`
			;;
	esac
fi

echo Creating package directory...
pkgdir="sw-$pkgname-$fileversion"

test -d $pkgdir && rm -r $pkgdir
mkdir $pkgdir || exit 1

echo Copying package files
cp LICENSE.md $pkgdir
cp doc/*.html $pkgdir
cp tests/README.md $pkgdir
cp tests/*.jpg $pkgdir
cp tests/*.pdf $pkgdir
cp tests/*.sh $pkgdir
cp tests/*.test $pkgdir
cp tools/ippeveprinter $pkgdir
cp tools/ippevesubmit $pkgdir
cp tools/ippfind $pkgdir/ippfind
cp tools/ipptool $pkgdir/ipptool

chmod +x $pkgdir/*.sh

if test x$platform = xmacos; then
	# Sign executables...
	if test "x$CODESIGN_IDENTITY" = x; then
		CODESIGN_IDENTITY="IEEE INDUSTRY STANDARDS AND TECHNOLOGY ORGANIZATION"
	fi

	codesign -s "$CODESIGN_IDENTITY" -fv $pkgdir/ippeveprinter
	codesign -s "$CODESIGN_IDENTITY" -fv $pkgdir/ippevesubmit
	codesign -s "$CODESIGN_IDENTITY" -fv $pkgdir/ippfind
	codesign -s "$CODESIGN_IDENTITY" -fv $pkgdir/ipptool

	# Make ZIP archive...
	pkgfile="$pkgdir-macos.zip"
	echo Creating ZIP file $pkgfile...
	test -f $pkgfile && rm $pkgfile
	zip -r9 $pkgfile $pkgdir || exit 1
else
	# Make archive...
	pkgfile="$pkgdir-$platform.tar.gz"
	echo Creating archive $pkgfile...
	tar czf $pkgfile $pkgdir || exit 1
fi

echo Removing temporary files...
rm -r $pkgdir

echo Done.
