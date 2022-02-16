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
tools/ippeveprinter -vvv -f image/jpeg,image/pwg-raster,application/pdf -s 10,5 "Test" >test.log 2>&1 &
pid=$!

# Run the tests for a print server...
IPP_EVERYWHERE_SERVER=1; export IPP_EVERYWHERE_SERVER

./runtests.sh dnssd-tests.sh "Test"
./runtests.sh ipp-tests.sh "Test"
./runtests.sh document-tests.sh "Test"

# Stop ippeveprinter...
kill $pid

# See if we passed or failed...
(tail -3 tests/Test*.plist | grep -q false) && exit 1
