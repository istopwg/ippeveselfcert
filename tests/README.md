IPP Everywhere™ v1.1 Printer Self-Certification Tools
=====================================================

This directory contains the IPP Everywhere™ v1.1 Printer Self-Certification
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

- "dnssd-tests.bat": DNS-SD Tests for Windows
- "dnssd-tests.sh": DNS-SD Tests for Linux and macOS

- "ipp-tests.bat": IPP Tests for Windows
- "ipp-tests.sh": IPP Tests for Linux and macOS

- "document-tests.bat": Document Data Tests for Windows
- "document-tests.sh": Document Data Tests for Linux and macOS

Tools:

- "ippeveprinter": Sample IPP Everywhere printer, useful for testing
- "ippevesubmit": IPP Everywhere Printer Self-Certification submission tool
- "ippfind": Tool for finding printers with DNS-SD (Bonjour)
- "ipptool": IPP test tool

Documentation:

- "LICENSE" and "NOTICE": Software license
- "man-*.html": HTML documentation for the tools
- "README.md": This README file


Running the Self-Certification Tools on Windows® 7 and Higher
-------------------------------------------------------------

In addition to this software, you also need to install the Bonjour for Windows
software from Apple: <http://support.apple.com/kb/DL999>

Extract the PWG Raster files to your Desktop directory, then open a command
prompt or power shell and run the following commands with "Printer Name"
replaced by the DNS-SD service name of your printer:

    cd "C:\Program Files\IPP Everywhere Printer Self-Certification Tools"
    .\dnssd-tests.bat "Printer Name"
    .\ipp-tests.bat "Printer Name"
    .\document-tests.bat "Printer Name"

Assuming all of the tests passed, you can then run the following command to
generate a JSON file containing your submission:

    .\ippevesubmit "Printer Name<"

You can then submit the JSON file to the "ippeveselfcert@pwg.org" mailing list
or the Github issue tracker at:
<https://github.com/istopwg/ippeveselfcert/issues/new>


Running the Self-Certification Tools on macOS® 10.13 and Higher
---------------------------------------------------------------

Extract the PWG Raster files to the tools directory, then open a Terminal
window and run the following commands with "Printer Name" replaced by the
DNS-SD service name of your printer:

    cd /path/to/tools
    ./dnssd-tests.sh "Printer Name"
    ./ipp-tests.sh "Printer Name"
    ./document-tests.sh "Printer Name"

Assuming all of the tests passed, you can then run the following command to
generate a JSON file containing your submission:

    ./ippevesubmit "Printer Name"

You can then submit the JSON file to the "ippeveselfcert@pwg.org" mailing list
or the Github issue tracker at:
<https://github.com/istopwg/ippeveselfcert/issues/new>


Running the Self-Certification Tools on Red Hat Enterprise Linux 7 and Higher
-----------------------------------------------------------------------------

Extract the PWG Raster files to the tools directory, then open a Terminal
window and run the following commands with "Printer Name" replaced by the
DNS-SD service name of your printer:

    cd /path/to/tools
    ./dnssd-tests.sh "Printer Name"
    ./ipp-tests.sh "Printer Name"
    ./document-tests.sh "Printer Name"

Assuming all of the tests passed, you can then run the following command to
generate a JSON file containing your submission:

    ./ippevesubmit "Printer Name"

You can then submit the JSON file to the "ippeveselfcert@pwg.org" mailing list
or the Github issue tracker at:
<https://github.com/istopwg/ippeveselfcert/issues/new>


Running the Self-Certification Tools on Ubuntu 18.04 and higher
---------------------------------------------------------------

Extract the PWG Raster files to the tools directory, then open a Terminal
window and run the following commands with "Printer Name" replaced by the
DNS-SD service name of your printer:

    cd /path/to/tools
    ./dnssd-tests.sh "Printer Name"
    ./ipp-tests.sh "Printer Name"
    ./document-tests.sh "Printer Name"

Assuming all of the tests passed, you can then run the following command to
generate a JSON file containing your submission:

    ./ippevesubmit "Printer Name"

You can then submit the JSON file to the "ippeveselfcert@pwg.org" mailing list
or the Github issue tracker at:
<https://github.com/istopwg/ippeveselfcert/issues/new>


Getting Support and Other Resources
-----------------------------------

The IPP Everywhere home page provides access to all information relevant to
IPP Eveywhere: <https://www.pwg.org/ipp/everywhere.html>

The "ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere
Printer Self-Certification.  You can subscribe from the following page:
<https://www.pwg.org/mailman/listinfo/ippeveselfcert>

Issues found in the tools should be reported using the Github issues page:
<https://github.com/istopwg/ippeveselfcert/issues>


Legal Stuff
-----------

Copyright © 2014-2021 by the IEEE-ISTO Printer Working Group.
Copyright © 2007-2019 by Apple Inc.
Copyright © 1997-2007 by Easy Software Products.

This software is provided under the terms of the Apache License, Version 2.0.
A copy of this license can be found in the file `LICENSE`.  Additional legal
information is provided in the file `NOTICE`.

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.
