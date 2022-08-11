#
# Top-level makefile for the IPP Everywhere Printer Self-Certification tools.
#
# Copyright © 2015-2022 by the ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include Makedefs


#
# Make all targets...
#

all:
	(cd libcups; $(MAKE) $(MFLAGS) all)
	(cd selfcert; $(MAKE) $(MFLAGS) all)


#
# Remove object and target files...
#

clean:
	(cd libcups; $(MAKE) $(MFLAGS) clean)
	(cd selfcert; $(MAKE) $(MFLAGS) clean)


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
	(cd libcups; $(MAKE) $(MFLAGS) depend)
	(cd selfcert; $(MAKE) $(MFLAGS) depend)


#
# Run test suite...
#

test:		all
	./testbuild.sh


#
# Make distribution files for the web site.
#

.PHONEY:	dist
dist:	all
	scripts/make-ippeveselfcert.sh $(IPPEVESELFCERT_DOCVERSION) $(IPPEVESELFCERT_SWVERSION)


#
# Don't run top-level build targets in parallel...
#

.NOTPARALLEL:
