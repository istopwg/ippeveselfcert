#!/bin/sh
#
# Script to make the PWG sample files using the test script.
#
# Usage: ./make-pwg-samples.sh {100dpi|150dpi|180dpi|200dpi|300dpi|360dpi|600dpi|600x300dpi|720dpi}
#

if test $# != 1; then
	echo "Usage: ./make-pwg-samples.sh NNNdpi"
	exit 1
fi

if test ! -f tests/color.jpg; then
	echo "Run this script from the root source directory."
	exit 1
fi

case "$1" in
	*dpi)
		resolution="$1"
		;;
	*)
		echo "Usage: ./make-pwg-samples.sh NNNdpi"
		exit 1
		;;
esac

# The PPD to use for all of the tests...
PPD=scripts/pwg-raster.ppd; export PPD

# The output format...
FINAL_CONTENT_TYPE="image/pwg-raster"; export FINAL_CONTENT_TYPE

# Date for this version of the script...
date="20150616"

# Year of generation...
year=`date '+%Y'`

# Output directory...
dir="pwg-raster-samples-$resolution-$date"

# Files to render...
files="document-a4.pdf onepage-a4.pdf document-letter.pdf onepage-letter.pdf color.jpg gray.jpg"

# PDF to raster filter...
pdftoraster=""
for file in /usr/libexec/cups/filter/cgpdftoraster /usr/lib/cups/filter/pdftoraster; do
	if test -x $file; then
		pdftoraster="$file"
		break;
	fi
done

if test "x$pdftoraster" = x; then
	echo No PDF filter found.
	exit 1
fi

# Log file...
log="$dir.log"

# Remove any existing raster directory for this resolution and recreate it...
test -d $dir && rm -rf $dir

mkdir -p $dir/originals
for file in $files; do
	cp tests/$file $dir/originals
done

lastmedia=""
lastmodes=""

for file in $files; do
	echo Generating raster data for $file...

	case $file in
		*color*.jpg)
			file="$file-4x6.pdf"
			modes="black-1 sgray-8 srgb-8 srgb-16 cmyk-8"
			media="4x6"
			;;
		*gray*.jpg)
			file="$file-4x6.pdf"
			modes="black-1 sgray-8 cmyk-8"
			media="4x6"
			;;
		*-a4.pdf)
			modes="black-1 sgray-8 srgb-8 cmyk-8"
			media="a4"
			;;
		*-letter.pdf)
			modes="black-1 sgray-8 srgb-8 cmyk-8"
			media="letter"
			;;
	esac

	for mode in $modes; do
		test -d $dir/$mode || mkdir $dir/$mode

		base="`basename $file .pdf`"
		output="$base-$mode-$resolution.pwg"

		echo "$output: \c"
		$pdftoraster job user title 1 "ColorModel=$mode PageSize=$media Resolution=$resolution" "tests/$file" >"$dir/$mode/$output" 2>>"$log"
		ls -l "$dir/$mode/$output" | awk '{if ($5 > 1048575) printf "%.1fMiB\n", $5 / 1048576; else printf "%.0fkiB\n", $5 / 1024;}'
	done
done

cat >$dir/README.txt <<EOF
README.txt - $date
-----------------------

This directory contains sample PWG Raster files for different raster modes at
$resolution.  The original documents used to generate the raster files are
located in the "originals" directory.


TEST FILES

- color.jpg

    A 6 megapixel color photograph with a 3:2 aspect ratio.

- document-a4.pdf

    A multi-page PDF document containing a mix of text, graphics, and images
    sized for ISO A4 media.

- document-letter.pdf

    A multi-page PDF document containing a mix of text, graphics, and images
    sized for US Letter media.

- gray.jpg

    A 5 megapixel grayscale photograph with a 5:4 aspect ratio.

- onepage-a4.pdf

    A single-page PDF document containing a mix of text and graphics sized for
    ISO A4 media.

- onepage-letter.pdf

    A single-page PDF document containing a mix of text and graphics sized for
    US Letter media.


VIEWING PWG RASTER FILES

The free RasterView application can be used to view PWG Raster files and is
available at:

    http://www.msweet.org/projects.php/rasterview


DOCUMENTATION AND RESOURCES

The PWG Raster format is documented in PWG 5102.4: PWG Raster Format, available
at:

    http://ftp.pwg.org/pub/pwg/candidates/cs-ippraster10-20120420-5102.4.pdf

Please contact the PWG Webmaster (webmaster AT pwg.org) to report problems with
these sample files.  Questions about the PWG Raster format should be addressed
to the IPP working group mailing list (ipp AT pwg.org).


LEGAL STUFF

All original PDF files are Copyright 2011 Apple Inc.  All rights reserved.

All original JPEG files are Copyright 2003, 2007 Michael R Sweet.  All rights
reserved.

These sample PWG Raster files are Copyright $year by The Printer Working Group.
All rights reserved.
EOF

echo "Creating $dir.zip... \c"
zip -qr9 $dir.zip $dir
ls -l "$dir.zip" | awk '{printf "%.1fMiB\n", $5 / 1048576;}'

echo "Cleaning up temporary files..."
rm -rf data
