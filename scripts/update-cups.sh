#!/bin/sh
#
# Script to merge changes from CUPS 2.3.x upstream into the CUPS directory.
#

if test ! -x scripts/update-cups.sh; then
	echo Must run this script from the top directory.
	exit 1
fi

if test ! -d ../cups; then
	echo Have CUPS 2.3.x checked out in ../cups.
	exit 1
fi

oldrev=`cat .cups-upstream`
newrev=`cd ../cups; git show | head -1 | awk '{print $2}'`

(cd ../cups; git diff $oldrev cups ':!cups/Dependencies' ':!cups/Makefile' ':!cups/libcups2.def' ':!cups/adminutil*' ':!cups/back*' ':!cups/getdevices*' ':!cups/getifaddrs*' ':!cups/interpret*' ':!cups/ppd*' ':!cups/raster-inter*' ':!cups/sidechannel*' ':!cups/test*' ':!cups/snmp*' ':!cups/tlscheck.c' ':!cups/cupspm*' ':!cups/*.shtml' ':!cups/*.header') >$newrev.patch
git apply $newrev.patch && (echo $newrev >.cups-upstream; rm $newrev.patch)

git commit -a -m "Merge changes from CUPS master@$newrev"
