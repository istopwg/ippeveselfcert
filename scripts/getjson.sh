#!/bin/sh
#
# Get attributes as JSON.
#
# Usage:
#
#   ./getjson.sh PRINTER-URI FILENAME.json
#

if test $# != 2; then
	echo "Usage: ./getjson.sh PRINTER-URI FILENAME.json"
	exit 1
fi

ipptool -j "$1" get-printer-attributes.test >"$2"
