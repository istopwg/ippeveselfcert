# Known Issues

## v1.1 (Pending)

There are no known issues in this release.


## v1.0 Update 4 (Pending)

There are no known issues in this release.


## v1.0 Update 3 (October 23, 2018)

The following is a list of the known issues in the October 23, 2018 release of
the self-certification tools:

1. The Windows installer was missing a copy of the REGEX.DLL file.


## v1.0 Update 2 (September 26, 2017)

The following is a list of the known issues in the September 26, 2017 release of
the self-certification tools:

1. Printer service names incorrectly match substrings.

2. The Windows tools depend on DLLs supplied with Visual Studio.


## v1.0 Update 1 (December 13, 2016)

The following is a list of the known issues in the December 13, 2016 release of
the self-certification tools:

1. The repeat limit of 30 is sometimes inadequate for test I-16.

2. The 10 second delay is sometimes inadequate for test I-27.

3. The "overrides-supported" test incorrectly looked for the "document-number"
   member attribute instead of "document-numbers".


## v1.0 (November 10, 2015)

The following is a list of the known issues in the November 10, 2015 release of
the self-certification tools:

1. The test suite requires support for the document-number member attribute
   (only needed for multiple document jobs).

2. The test suite requires that printers follow the best practice of using
   "/ipp/print" in the printer URI(s) (not required by IPP Everywhere).

3. The test suite matches any substring of the service instance name.

4. The test suite does not handle service instance names starting with an
   underscore.
