IPP Everywhere™ v1.0 Printer Self-Certification Tools
=====================================================

The IPP Everywhere™ Printer self-certification tools are used to test the
conformance of printers to PWG Candidate Standard 5100.14-2013: IPP Everywhere™.
The testing and submission procedures are defined in PWG Candidate Standard
5100.20-2016: IPP Everywhere™ v1.0 Printer Self-Certification Manual.

The [IPP Everywhere™ home page](http://www.pwg.org/ipp/everywhere.html) provides
access to all information relevant to IPP Everywhere™. Sample PWG Raster files
(needed for the document tests) can be downloaded from the PWG FTP server at
<https://ftp.pwg.org/pub/pwg/ipp/examples> - only the files dated on or after
June 7th, 2018 can be used.

The "ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere
Printer Self-Certification. You can subscribe at
<https://www.pwg.org/mailman/listinfo/ippeveselfcert>.

Issues found in the tools should be reported using the Github issues page at
<https://github.com/istopwg/ippeveselfcert>.

> Note: Tests are intended to be run on an isolated network, or at least when no
> other users are printing using the target printer.  Otherwise the test scripts
> will fail randomly.


Compiling
---------

Please see the file "BUILD.md" for instructions on compiling the software.

> Note: Self-certification results submitted to the PWG IPP Everywhere™ portal
> MUST be generated using the tools provides on the PWG web site.


Legal Stuff
-----------

These tools are Copyright © 2014-2020 by The Printer Working Group and
Copyright © 2007-2019 by Apple Inc.  CUPS and the CUPS logo are trademarks of
Apple Inc.  PWG and IPP Everywhere are trademarks of the IEEE-ISTO.

The tools are provided under the terms of version 2 of the GNU General Public
License and GNU Library General Public License.  This program is distributed in
the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the file "LICENSE.txt" for more information.
