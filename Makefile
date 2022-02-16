#
# Top-level Makefile for IPP Everywhere Printer Self-Certification tools.
#
# Copyright 2015-2019 by the ISTO Printer Working Group.
# Copyright 2007-2019 by Apple Inc.
# Copyright 1997-2007 by Easy Software Products, all rights reserved.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include Makedefs


#
# Make all targets...
#

all:
	(cd cups; $(MAKE) $(MFLAGS) all)
	(cd tools; $(MAKE) $(MFLAGS) all)


#
# Remove object and target files...
#

clean:
	(cd cups; $(MAKE) $(MFLAGS) clean)
	(cd tools; $(MAKE) $(MFLAGS) clean)


#
# Remove all non-distribution files...
#

distclean:	clean
	$(RM) Makedefs config.h config.log config.status
	-$(RM) -r autom4te*.cache


#
# Make dependencies
#

depend:
	(cd cups; $(MAKE) $(MFLAGS) depend)
	(cd tools; $(MAKE) $(MFLAGS) depend)


#
# Run test suite...
#

test:		all
	./testbuild.sh


#
# Run the Clang static code analysis tool on the sources, available here:
#
#    http://clang-analyzer.llvm.org
#
# At least checker-231 is required.
#

.PHONY: clang clang-changes
clang:
	$(RM) -r clang
	scan-build -V -k -o `pwd`/clang $(MAKE) $(MFLAGS) clean all
clang-changes:
	scan-build -V -k -o `pwd`/clang $(MAKE) $(MFLAGS) all


#
# Make distribution files for the web site.
#

.PHONEY:	dist
dist:	all
	scripts/make-ippeveselfcert.sh $(IPPEVESELFCERT_VERSION) $(SELFCERTVERSION)


#
# Don't run top-level build targets in parallel...
#

.NOTPARALLEL:
