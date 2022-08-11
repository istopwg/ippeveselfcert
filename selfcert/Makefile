#
# Selfcert makefile for the IPP Everywhere Printer Self-Certification tools.
#
# Copyright © 2015-2022 by the IEEE-ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include ../Makedefs


COBJS		=	\
			plist.o
CXXOBJS		=	\
			main.o \
			SelfCertApp.o
OBJS		=	$(COBJS) $(CXXOBJS)
TARGETS         =       \
                        ippeveselfcert


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
	$(CC) -MM $(CPPFLAGS) $(COBJS:.o=.c) | sed -e '1,$$s/ \/usr\/include\/[^ ]*//g' -e '1,$$s/ \/usr\/local\/include\/[^ ]*//g' >Dependencies
	$(CXX) -MM $(CPPFLAGS) $(CXXOBJS:.o=.cxx) | sed -e '1,$$s/ \/usr\/include\/[^ ]*//g' -e '1,$$s/ \/usr\/local\/include\/[^ ]*//g' >>Dependencies


#
# Install all targets...
#

install:	all
	echo "Installing selfcert to $(BUILDROOT)$(bindir)..."
	$(INSTALL_DIR) $(BUILDROOT)$(bindir)
	$(INSTALL_BIN) ippeveselfcert $(BUILDROOT)$(bindir)


#
# Test all tools.
#

test:


#
# selfcert
#

ippeveselfcert:	$(OBJS) ../libcups/cups/libcups.a
	echo Linking $@...
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)


#
# Dependencies...
#

include Dependencies
