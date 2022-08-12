#
# Selfcert makefile for the IPP Everywhere Printer Self-Certification tools.
#
# Copyright © 2015-2022 by the IEEE-ISTO Printer Working Group.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

include ../Makedefs


COMMON_COBJS	=	\
			plist.o \
			validate.o
APP_CXXOBJS	=	\
			main.o \
			SelfCertApp.o
SUBMIT_COBJS	=	\
			ippevesubmit.o
OBJS		=	$(COMMON_COBJS) $(APP_CXXOBJS) $(SUBMIT_COBJS)
TARGETS         =       \
                        ippeveselfcert \
                        ippevesubmit


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
	$(CC) -MM $(CPPFLAGS) $(COMMON_COBJS:.o=.c) $(SUBMIT_COBJS:.o=.c) | sed -e '1,$$s/ \/usr\/include\/[^ ]*//g' -e '1,$$s/ \/usr\/local\/include\/[^ ]*//g' >Dependencies
	$(CXX) -MM $(CPPFLAGS) $(APP_CXXOBJS:.o=.cxx) | sed -e '1,$$s/ \/usr\/include\/[^ ]*//g' -e '1,$$s/ \/usr\/local\/include\/[^ ]*//g' >>Dependencies


#
# Install all targets...
#

install:	all
	echo "Installing selfcert to $(BUILDROOT)$(bindir)..."
	$(INSTALL_DIR) $(BUILDROOT)$(bindir)
	for file in $(TARGETS); do \
		$(INSTALL_BIN) $$file $(BUILDROOT)$(bindir)/$$file; \
	done


#
# Test all tools.
#

test:


#
# ippeveselfcert
#

ippeveselfcert:	$(APP_CXXOBJS) $(COMMON_COBJS) ../libcups/cups/libcups.a
	echo Linking $@...
	$(CXX) $(LDFLAGS) -o $@ $(APP_CXXOBJS) $(COMMON_COBJS) $(LIBS)


#
# ippevesubmit
#

ippevesubmit:	$(SUBMIT_COBJS) $(COMMON_COBJS) ../libcups/cups/libcups.a
	echo Linking $@...
	$(CC) $(LDFLAGS) -o $@ $(SUBMIT_COBJS) $(COMMON_COBJS) $(LIBS)


#
# Dependencies...
#

include Dependencies
