README.txt - 2015-10-09
-----------------------

INTRODUCTION

    This directory contains the IPP Everywhere Printer Self-Certification
    tools.

    In addition to the files in this directory, you must also download and
    extract one or more PWG Raster Format file archives from:

      http://ftp.pwg.org/pub/pwg/ipp/examples/

    These archives are used for the Document Data tests.


CONTENTS

    Scripts for running the self-certification tests:

      bonjour-tests.bat   Bonjour Tests for Windows
      bonjour-tests.sh    Bonjour Tests for Linux and OS X

      ipp-tests.bat       IPP Tests for Windows
      ipp-tests.sh        IPP Tests for Linux and OS X

      document-tests.bat  Document Data Tests for Windows
      document-tests.sh   Document Data Tests for Linux and OS X

    Tools:

      ippfind             Tool for finding printers with Bonjour/DNS-SD
      ippserver           Sample IPP server, useful for testing
      ipptool             IPP test tool

    Documentation:

      LICENSE.txt         CUPS software license
      man-*.html          HTML documentation for the tools
      README.txt          This README file



RUNNING THE SUITE

  Linux and OS X

    In the top level directory of the distribution, you can execute the
    following command to find your target printer's DNS-SD Instance Name:

      ./ippfind -s

    If your printer implements the "_print" DNS-SD subtype for the "_ipp._tcp"
    service type that is required by IPP Everywhere, you can filter the
    results using this command, if you have many printers:

      ./ippfind "_print._ipp._tcp" -s

    Once you have acquired the target printer's DNS-SD Instance Name, you can
    execute the following commands to run the 3 sets of tests ("Example Test Printer"
    is the example DNS-SD Instance Name in the examples below):

      ./runtests.sh bonjour-tests.sh "Example Test Printer"
      ./runtests.sh ipp-tests.sh "Example Test Printer"
      ./runtests.sh document-tests.sh "Example Test Printer"

    The corresponding PWG Raster sample files (see link in the introduction) MUST
    be placed in a subdirectory of the "tests" directory. For example:

      cd tests
      curl http://ftp.pwg.org/pub/pwg/ipp/examples/pwg-raster-samples-300dpi-20150616.zip >temp.zip
      unzip temp.zip
      rm temp.zip
      cd ..

  Windows
    You'll need to manually copy the DLL and EXE files from the "vcnet" directory
    to the "tests" directory.

    In the top level directory of the distribution, you can execute the
    following command to find your target printer's DNS-SD Instance Name:

      .\ippfind -s

    If your printer implements the "_print" DNS-SD subtype for the "_ipp._tcp"
    service type that is required by IPP Everywhere, you can filter the
    results using this command, if you have many printers:

      .\ippfind "_print._ipp._tcp" -s

    Once you have acquired the target printer's DNS-SD Instance Name, you can
    execute the following commands to run the 3 sets of tests ("Example Test Printer"
    is the example DNS-SD Instance Name in the examples below):

      cd tests
      .\bonjour-tests.bat "Example Test Printer"
      .\ipp-tests.bat "Example Test Printer"
      .\document-tests.bat "Example Test Printer"

    The corresponding PWG Raster sample files (see link in the introduction) MUST
    be placed in a subdirectory of the "tests" directory. After downloading the
    files just extract them using Windows Explorer.



GETTING SUPPORT AND OTHER RESOURCES

    The IPP Everywhere home page provides access to all information relevant to
    IPP Eveywhere:

      http://www.pwg.org/ipp/everywhere.html

    The "ippeveselfcert@pwg.org" mailing list is used to discuss IPP Everywhere
    Printer Self-Certification.  You can subscribe from the following page:

      https://www.pwg.org/mailman/listinfo/ippeveselfcert


LEGAL STUFF

    These tools are Copyright 2014-2015 by The Printer Working Group and
    Copyright 2007-2015 by Apple Inc.  CUPS and the CUPS logo are trademarks of
    Apple Inc.  PWG and IPP Everywhere are trademarks of the IEEE-ISTO.

    CUPS is provided under the terms of version 2 of the GNU General Public
    License and GNU Library General Public License. This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the file "LICENSE.txt" for more information.
