Running/Testing from a Local Build
==================================

This file describes how to run tests locally after you have compiled the
software on your system.

> Note: See the file "BUILD.md" for instructions on how to compile the software.

> Note: Tests are intended to be run on an isolated network, or at least when no
> other users are printing using the target printer.  Otherwise the test scripts
> will fail randomly.

> Note: Certification results posted to the IPP Everywhere portal MUST be
> generated using the posted binaries.  You cannot build and use your own copy
> of the tools to generate the results for the portal.


Getting PWG Raster Sample Files
-------------------------------

The [IPP Everywhere™ home page](http://www.pwg.org/ipp/everywhere.html) provides
access to all information relevant to IPP Everywhere™. Sample PWG Raster files
(needed for the document tests) can be downloaded from the PWG FTP server at
<https://ftp.pwg.org/pub/pwg/ipp/examples> - only the files dated on or after
June 7th, 2018 can be used.

Extract the ZIP files under the "tests" directory so that the tools can find the
PWG Raster files.


Testing on Linux and macOS
--------------------------

The "runtests.sh" script can be used to run any of the test scripts using the locally-built tools. For example:

    ./runtests.sh bonjour-tests.sh "Example Test Printer"
    ./runtests.sh ipp-tests.sh "Example Test Printer"
    ./runtests.sh document-tests.sh "Example Test Printer"


Testing on Windows
------------------

You'll need to manually copy the DLL and EXE files from the "vcnet" directory to the "tests" directory. Then run the corresponding test from that directory, for example:

    cd tests
    bonjour-tests.bat "Example Test Printer"
    ipp-tests.bat "Example Test Printer"
    document-tests.bat "Example Test Printer"


Getting Debug Logging
---------------------

The following environment variables are used to enable and control debug
logging:

- `CUPS_DEBUG_FILTER`: Specifies a POSIX regular expression to control which
  messages are logged.
- `CUPS_DEBUG_LEVEL`: Specifies a number from 0 to 9 to control the verbosity of
  the logging. The default level is 1.
- `CUPS_DEBUG_LOG`: Specifies a log file to use.  Specify the name "-" to send
  the messages to stderr.  Prefix a filename with "+" to append to an existing
  file.  You can include a single "%d" in the filename to embed the current
  process ID.
