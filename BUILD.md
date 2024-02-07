Build Instructions for the IPP Everywhere Printer Self-Certification Tools
==========================================================================

This file describes how to compile and install the IPP Everywhere Printer Self-
Certification Tools.

> Note: See the file "TESTING.md" for instructions on how to run tests with a
> local build of the software.

> Note: Certification results posted to the IPP Everywhere portal MUST be
> generated using the posted binaries.  You cannot build and use your own copy
> of the tools to generate the results for the portal.


Prerequisites
-------------

You'll need an ANSI-compliant C compiler plus a make program and POSIX-compliant
shell (/bin/sh).  The GNU compiler tools and Bash work well and we have tested
the current IPP sample code against several versions of Clang and GCC with
excellent results.

The makefiles used by the project should work with most versions of make.  BSD
users should use GNU make (gmake) since BSD make does not support "include".

Besides these tools you'll need the following libraries:

- Avahi (Linux) or mDNSResponder (all others) for Bonjour (DNS-SD) support
- GNU TLS, LibreSSL, or OpenSSL for encryption support
- ZLIB for compression support


Linux
-----

Packages are targeted for Red Hat Enterprise Linux and Ubuntu.  On a stock
Ubuntu install, the following command will install the required prerequisites:

    sudo apt-get install build-essential autoconf avahi-daemon avahi-utils \
        libavahi-client-dev libssl-dev libnss-mdns zlib1g-dev

Run the following to compile the tools:

    ./configure
    make


macOS
-----

You'll need the current Xcode software and command-line tools to build things.
Run the following to compile the tools:

    ./configure
    make


Windows
-------

You'll need the current Visual Studio C++ as well as the code signing tools and
the PWG code signing certificate (available from the PWG officers for official
use only) - without the certificate the build will fail unless you disable the
post-build events that add the code signatures or create a self-signed
certificate with the name "IEEE INDUSTRY STANDARDS AND TECHNOLOGY ORGANIZATION".

Open the "ippeveselfcert.sln" file in the "vcnet" subdirectory and build the
installer project.


Other Platforms
---------------

This project uses GNU autoconf, so you should find the usual `configure` script
in the main source directory.  To configure the code for your system, type:

    ./configure

The default installation will put the software in the `/usr/local` directory on
your system.  Use the `--prefix` option to install the software in another
location:

    ./configure --prefix=/some/directory

To see a complete list of configuration options, use the `--help` option:

    ./configure --help

If any of the dependent libraries are not installed in a system default location
(typically `/usr/include` and `/usr/lib`) you'll need to set the CPPFLAGS and
LDFLAGS environment variables prior to running configure:

    CPPFLAGS="-I/some/directory" \
    LDFLAGS="-L/some/directory" \
    ./configure ...

Once you have configured things, just type:

    make ENTER

or if you have FreeBSD, NetBSD, or OpenBSD type:

    gmake ENTER

to build the software.


Packaging
---------

On Linux run:

    make dist

A tar.gz file will be placed in the current directory.

On macOS you'll need the PWG code signing certificate (available from the PWG officers for official use only) or your own certificate loaded into your login keychain.  Then run:

    CODESIGN_IDENTITY="common name or SHA-1 hash of certificate" make dist

On Windows just build the installer target in Visual Studio - you'll find the
package in a MSI file in the "vcnet" directory.
