#!/bin/sh
#
# Script to merge changes from CUPS 2.2.x upstream into the CUPS directory.
#

if test ! -x scripts/update-cups.sh; then
	echo Must run this script from the top directory.
	exit 1
fi

if test ! -d ../cups-2.2; then
	echo Have CUPS 2.2.x checked out in ../cups-2.2.
	exit 1
fi

oldrev=`cat .cups-upstream`
newrev=`cd ../cups-2.2; git show | head -1 | awk '{print $2}'`

(cd ../cups-2.2; git diff $oldrev cups ':!cups/Dependencies' ':!cups/Makefile' ':!cups/adminutil.*' ':!cups/api*' ':!cups/backchannel.*' ':!cups/libcups2.def' ':!cups/ppd*' ':!cups/sidechannel.*' ':!cups/snmp*' ':!cups/test*' ':!cups/tlscheck.c') >$newrev.patch
git apply $newrev.patch && (echo $newrev >.cups-upstream)
