#
# IPP tools makefile.
#
# Copyright 2015-2019 by the ISTO Printer Working Group.
# Copyright 2007-2019 by Apple Inc.
# Copyright 1997-2006 by Easy Software Products, all rights reserved.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include ../Makedefs


OBJS		=	\
			ippeveprinter.o \
			ippevesubmit.o \
			ippfind.o \
			ipptool.o
TARGETS		=	\
			ippeveprinter \
			ippevesubmit \
			ippfind \
			ipptool


#
# Make all targets...
#

all:		$(TARGETS)


#
# Clean all object files...
#

clean:
	$(RM) $(TARGETS) $(OBJS)


#
# Update dependencies (without system header dependencies...)
#

depend:
	$(CC) -MM $(ALL_CFLAGS) $(OBJS:.o=.c) >Dependencies


#
# ippeveprinter
#

ippeveprinter:	ippeveprinter.o ../cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ ippeveprinter.o $(LIBS)


#
# ippevesubmit
#

ippevesubmit:	ippevesubmit.o ../cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ ippevesubmit.o $(LIBS)


#
# ippfind
#

ippfind:	ippfind.o ../cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ ippfind.o $(LIBS)


#
# ipptool
#

ipptool:	ipptool.o ../cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ ipptool.o $(LIBS)


#
# Dependencies...
#

include Dependencies
