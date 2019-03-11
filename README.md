IPP Everywhere™ v1.1 Printer Self-Certification Tools
=====================================================

The IPP Everywhere™ Printer self-certification tools are used to test the
conformance of printers to PWG Candidate Standard 5100.14-201x: IPP Everywhere™
v1.1. The testing and submission procedures are defined in PWG Candidate
Standard 5100.20-201x: IPP Everywhere™ v1.1 Printer Self-Certification Manual.

The [IPP Everywhere™ home page](http://www.pwg.org/ipp/everywhere.html) provides
access to all information relevant to IPP Everywhere™. Sample PWG Raster files
needed for the document tests canbe downloaded from the
[PWG FTP server IPP Examples folder](https://ftp.pwg.org/pub/pwg/ipp/examples).

The "ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere™
Printer Self-Certification. You can subscribe at
[https://www.pwg.org/mailman/listinfo/ippeveselfcert](https://www.pwg.org/mailman/listinfo/ippeveselfcert).

Issues found in the tools should be reported using the
[Github issues page](https://github.com/istopwg/ippeveselfcert).

> Note: Tests are intended to be run on an isolated network, or at least when no
> other users are printing using the target printer.  Otherwise the test scripts
> will fail randomly.


Compiling
---------

Below are instructions for building the tools on Linux, macOS, and Linux.

> Note: Self-certification results submitted to the PWG IPP Everywhere™ portal
> MUST be generated using the tools provides on the PWG web site.  The following
> instructions are provided for developers to build and test using unofficial
> builds.


### Linux

You'll need the Avahi and GNU TLS developer packages to provide DNS-SD and TLS
support, clang or GCC, and GNU make. Packages are targeted for Red Hat
Enterprise Linux and Ubuntu. Run the following to compile the tools:

    ./configure
    make


### macOS

You'll need the current Xcode software and command-line tools to build things.
Run the following to compile the tools:

    ./configure
    make


### Windows

You'll need the current Visual Studio C++ as well as the code signing tools and
the PWG code signing certificate (available from the PWG officers for official
use only) - without the certificate the build will fail unless you disable the
post-build events that add the code signatures.

Open the "ippeveselfcert.sln" file in the "vcnet" subdirectory and build the
installer project.


Running/Testing with Local Builds
---------------------------------

### Linux and macOS

The "runtests.sh" script can be used to run any of the test scripts using the
locally-built tools.  For example:

    ./runtests.sh dns-sd-tests.sh "Example Test Printer"
    ./runtests.sh ipp-tests.sh "Example Test Printer"
    ./runtests.sh document-tests.sh "Example Test Printer"

The corresponding PWG Raster sample files (see link in the introduction) MUST
be placed in a subdirectory of the "tests" directory. For example:

    cd tests
    curl http://ftp.pwg.org/pub/pwg/ipp/examples/pwg-raster-samples-300dpi-20150616.zip >temp.zip
    unzip temp.zip
    rm temp.zip
    cd ..


### Windows

You'll need to manually copy the DLL and EXE files from the "vcnet" directory to
the "tests" directory. Then run the corresponding test from that directory, for
example:

    cd tests
    dns-sd-tests.bat "Example Test Printer"
    ipp-tests.bat "Example Test Printer"
    document-tests.bat "Example Test Printer"

The corresponding PWG Raster sample files (see link in the introduction) MUST be
placed in a subdirectory of the "tests" directory. After downloading the files
just extract them using Windows Explorer.


Packaging
---------

### Linux

Run this:

    make dist

A GZipped tar ball (.tar.gz) file will be placed in the current directory.


### macOS

You'll need the PWG code signing certificate (available from the PWG officers
for official use only) or your own certificate loaded into your login keychain.
Then run:

    CODESIGN_IDENTITY="common name or SHA-1 hash of certificate" make dist


### Windows

Building the installer target successfully will produce an MSI installer package
file in the "vcnet" directory.


Legal Stuff
-----------

Copyright © 2014-2019 by the IEEE-ISTO Printer Working Group.
Copyright © 2007-2019 by Apple Inc.
Copyright © 1997-2007 by Easy Software Products.

This software is provided under the terms of the Apache License, Version 2.0.
A copy of this license can be found in the file `LICENSE`.  Additional legal
information is provided in the file `NOTICE`.

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.
