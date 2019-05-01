#
# IPP documentation makefile.
#
# Copyright © 2014-2019 by the IEEE-ISTO Printer Working Group.
# Copyright © 2015-2019 by Apple Inc.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include ../Makedefs


OBJS		=	\
			mantohtml.o
TARGETS		=	\
			mantohtml
MAN1		=	\
			ippeveprinter.1 \
			ippevesubmit.1 \
			ippfind.1 \
			ipptool.1
MAN5		=	\
			ipptoolfile.5

HTML		=	\
			ippeveprinter.html \
			ippevesubmit.html \
			ippfind.html \
			ipptool.html \
			ipptoolfile.html


#
# Rules for converting man pages to HTML...
#

.SUFFIXES:	.1 .5 .8 .html
.1.html .5.html .8.html:	mantohtml
	echo Generating $@...
	./mantohtml $< >$@


#
# Make all targets...
#

all:		$(TARGETS) $(HTML)


#
# Clean all object files...
#

clean:
	$(RM) $(TARGETS) $(OBJS)


#
# mantohtml
#

mantohtml:	mantohtml.o ../cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ mantohtml.o $(LIBS)
