Known Issues
============

v1.0 Update 3 (November 9, 2018)
--------------------------------

The following is a list of the known issues in v1.0 update 3:

- The Windows MSI package was missing the "regex.dll" library.
- The Windows ipptool


v1.0 Update 2 (September 26, 2017)
----------------------------------

The following is a list of the known issues in v1.0 update 2:

- Printer service names incorrectly match substrings.
- The Windows tools depend on DLLs supplied with Visual Studio.


v1.0 Update 1 (December 13, 2016)
---------------------------------

The following is a list of the known issues in v1.0 update 1:

- The repeat limit of 30 is sometimes inadequate for test I-16.
- The 10 second delay is sometimes inadequate for test I-27.
- The "overrides-supported" test incorrectly looked for the "document-number"
  member attribute instead of "document-numbers".


v1.0 (November 10, 2015)
------------------------

The following is a list of the known issues in v1.0:

- The test suite requires support for the document-number member attribute
  (only needed for multiple document jobs).
- The test suite requires that printers follow the best practice of using
  "/ipp/print" in the printer URI(s) (not required by IPP Everywhere).
- The test suite matches any substring of the service instance name.
- The test suite does not handle service instance names starting with an
  underscore.
