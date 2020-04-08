# Changes

## v1.0 Update 4 (April 8, 2020)

- Issue #41: Windows IPP Everywhere Self Cert 1.0 Update 3: ipptool fails to
  run - missing regex.dll
- Updated the Windows test scripts to look for PWG Raster files on the Desktop,
  and to write the test results to the Desktop since the installation
  directory is now write-protected on current versions of Windows.
- Updated libcups and the IPP tools to CUPS v2.2.13.


## v1.0 Update 3 (October 23, 2018)

- Issue #22: Print service names could match multiple printers.
- Issue #24: Updated IPP tools to CUPS v2.2.8.


## v1.0 Update 2 (September 26, 2017)

- Issue #15: ipp-tests.test: "document-number" member of "overrides-supported"
  should be "document-numbers"


## v1.0 Update 1 (October 11, 2016)

- Issue #3: Fix handling of service instance names starting with "_".
- PR #5: Add date to log files.
- Issue #6: Do exact matches of service instance names.
- Issue #7: Do not require a resource path of "/ipp/print".
- Issue #9: Do not require "document-numbers" member attribute.
- Issue #13: Allow 'unknown' values for "date-time-at-xxx" attributes.


## v1.0 (November 10, 2015)

Initial release.
