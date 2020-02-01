IPP Everywhere™ v1.0 Printer Self-Certification Tools
=====================================================

This directory contains the IPP Everywhere™ v1.0 Printer Self-Certification
tools.

In addition to the files in this directory, you must also download and
extract one or more PWG Raster Format file archives from
<https://ftp.pwg.org/pub/pwg/ipp/examples/>

These archives are used for the Document Data tests.

> Note: Tests are intended to be run on an isolated network, or at least when no
> other users are printing using the target printer.  Otherwise the test scripts
> will fail randomly.


Contents
--------

Scripts for running the self-certification tests:

- "bonjour-tests.bat": DNS-SD Tests for Windows
- "bonjour-tests.sh": DNS-SD Tests for Linux and macOS

- "ipp-tests.bat": IPP Tests for Windows
- "ipp-tests.sh": IPP Tests for Linux and macOS

- "document-tests.bat": Document Data Tests for Windows
- "document-tests.sh": Document Data Tests for Linux and OS X

Tools:

- "ippevesubmit": IPP Everywhere Printer Self-Certification submission tool
- "ippfind": Tool for finding printers with DNS-SD (Bonjour)
- "ippserver": Sample IPP Everywhere printer, useful for testing
- "ipptool": IPP test tool

Documentation:

- "LICENSE.md": Software license
- "man-*.html": HTML documentation for the tools
- "README.md": This README file


Getting Support and Other Resources
-----------------------------------

The IPP Everywhere home page provides access to all information relevant to
IPP Eveywhere: <https://www.pwg.org/ipp/everywhere.html>

The "ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere
Printer Self-Certification.  You can subscribe from the following page:
<https://www.pwg.org/mailman/listinfo/ippeveselfcert>


Legal Stuff
-----------

These tools are Copyright © 2014-2020 by The Printer Working Group and
Copyright © 2007-2019 by Apple Inc.  CUPS and the CUPS logo are trademarks of
Apple Inc.  PWG and IPP Everywhere are trademarks of the IEEE-ISTO.

The tools are provided under the terms of version 2 of the GNU General Public
License and GNU Library General Public License.  This program is distributed in
the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the file "LICENSE.md" for more information.
