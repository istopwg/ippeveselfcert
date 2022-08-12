#!/bin/sh
#
# Build test script for the self-certification suite.
#
# Copyright © 2022 by the ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

# Run ippeveprinter in the background...
libcups/tools/ippeveprinter -vvv -f image/jpeg,image/pwg-raster,application/pdf -s 10,5 "Test" >test.log 2>&1 &
pid=$!

# Run the tests for a print server...
IPP_EVERYWHERE_SERVER=1; export IPP_EVERYWHERE_SERVER

echo "DNS-SD Tests:"
./runtests.sh dnssd-tests.sh "Test"

echo "IPP Tests:"
./runtests.sh ipp-tests.sh "Test"

echo "Document Tests:"
./runtests.sh document-tests.sh "Test"

echo "Test Printer 1000" >/tmp/test-models.txt
echo "Test Printer 2000" >>/tmp/test-models.txt
echo "Test Printer 3000" >>/tmp/test-models.txt

echo "ippevesubmit Test: \c"
cd tests
if ../selfcert/ippevesubmit -f standard -m /tmp/test-models.txt -p "Test Product Family" -t server -u "https://www.pwg.org/" -y "Test" >/tmp/test.log 2>&1; then
	if test $(wc -l <Test.json) = 3; then
		echo "PASS"
		cat /tmp/test.log
	else
		echo "FAIL (got $(wc -l <Test.json) lines in JSON output, expected 3)"
		cat /tmp/test.log
		echo "Test.json:"
		cat Test.json
		kill $pid
		exit 1
	fi
else
	echo "FAIL (ippevesubmit failed)"
	cat /tmp/test.log
	kill $pid
	exit 1
fi


# Stop ippeveprinter...
kill $pid

# See if we passed or failed...
(tail -3 Test*.plist | grep -q false) && exit 1

echo "All tests PASSED."
exit 0
