#!/bin/sh
#
# Make an IPP Everywhere Printer self-certification package.
#
# Copyright 2014-2020 by the ISTO Printer Working Group.
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
	echo "Usage: scripts/make-ippeveselfcert.sh ippeveselfcertNN [yyyymmdd] [platform]"
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
cp LICENSE NOTICE $pkgdir
cp man/*.html $pkgdir
cp tests/README.md $pkgdir
cp tests/*.jpg $pkgdir
cp tests/*.pdf $pkgdir
cp tests/*.sh $pkgdir
chmod +x $pkgdir/*.sh
cp tests/*.test $pkgdir

TOOLS="ippeveprinter ippevesubmit ippfind ipptool"
for tool in $TOOLS; do
	cp tools/$tool $pkgdir;
done

if test x$platform = xmacos; then
	# Sign executables...
	if test "x$CODESIGN_IDENTITY" = x; then
#		CODESIGN_IDENTITY="IEEE INDUSTRY STANDARDS AND TECHNOLOGY ORGANIZATION"
		CODESIGN_IDENTITY="Developer ID Application"
	fi

	for tool in $TOOLS; do
		xcrun codesign -s "$CODESIGN_IDENTITY" -fv \
			--prefix org.pwg.$pkgname. \
			-o runtime --timestamp \
			$pkgdir/$tool;
	done

	# Make ZIP archive...
	pkgfile="$pkgdir-macos.zip"
	echo "Creating notarization ZIP file $pkgfile..."
	test -f $pkgfile && rm $pkgfile
	zip -r9 $pkgfile $pkgdir || exit 1

	# Notarize package...
	if test "x$TEAMID" = x; then
		echo "Set the TEAMID environment variable to your Apple Developer Team ID (from your"
		echo "Developer ID Application certificate)"
		exit 1
	fi

	if test "x$APPLEID" = x; then
		echo "Set the APPLEID environment variable to your Apple ID."
		exit 1
	fi

	echo "Notarizing ZIP file $pkgfile..."
	xcrun altool --notarize-app -f $pkgfile --primary-bundle-id org.pwg.$pkgname --username $APPLEID --password '@keychain:AC_PASSWORD' --asc-provider $TEAMID | tee .notarize
	uuid=`grep RequestUUID .notarize | awk '{print $3}'`

	echo Waiting for notarization to complete...
	approved="in progress"
	while test "$status" = "in progress"; do
		sleep 10
		xcrun altool --notarization-info $uuid --username $APPLEID --password "@keychain:AC_PASSWORD" --asc-provider $TEAMID | tee .notarize
		status="`grep Status: .notarize | cut -b14-`"
	done

	rm -f .notarize
else
	# Make archive...
	pkgfile="$pkgdir-$platform.tar.gz"
	echo Creating archive $pkgfile...
	tar czf $pkgfile $pkgdir || exit 1
fi

echo Removing temporary files...
rm -r $pkgdir

echo Done.
