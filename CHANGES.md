Changes
=======

v1.1 Update 4 (TBD)
-------------------

- Added support for running the self-certification tools non-interactively
  with the "make test" command (Issue #56, Issue #62)
- The `ippevesubmit` program did not validate the correct number of tests for
  the document results (Issue #71)
- The DNS-SD TLS key test was incorrectly specified (Issue #72)
- The `ippevesubmit` program now validates and converts product web page URLs
  (Issue #75)
- The IPP and document tests now support printers that do not support US letter
  or ISO A4 sizes (Issue #78)
- The `ipptool` program can now generate PWG Raster documents dynamically for
  printers (Issue #79)
- The `ippevesubmit` program now uses the modification date of the plist files
  instead of the current date when generating the JSON submission file
  (Issue #81)
- The `ippfind` program did not correctly escape command-line arguments passed
  during the DNS-SD tests (Issue #83)
- The `ippevesubmit` program did not look for supported media sizes in the
  right place (Issue #84)
- Added the test numbers to all print job tests (Issue #85)
- Fixed the conditional requirements for "printer-supply-xxx" attributes with
  printers that do not have supplies.
- Fixed some problems with the Windows DNS-SD test script.
- Fixed the expected test counts in ippevesubmit and removed support for
  submitting IPP Everywhere v1.0 results.
- Fixed the "media-col-database" and "media-col-ready" tests to allow
  rangeOfInteger values for the "x-dimension" and "y-dimension" member
  attributes which are needed for roll feed and custom media support.


v1.1 Update 3 (May 17, 2021)
----------------------------

- The DNS-SD tests now look for a TLS key whose value contains a TLS version
  number (Issue #64)
- The document tests now wait for each job to complete before proceeding to the
  next job (Issue #66)
- Finishing options were not reported correctly by `ippevesubmit` in the JSON
  file (Issue #67)
- The `media-needed` test did not work on streaming printers (Issue #68)


v1.1 Update 2 (October 7, 2020)
-------------------------------

- Documentation updates (Issue #55)
- The "printer-alert" test did not allow an index value of -1 (Issue #58)
- The I-10 test didn't allow 'name' values for the
  "finishing-template-supported" attribute (Issue #59)
- Now allow the I-10.3 test to return the
  'successful-ok-ignored-or-substituted-attributes' status code (Issue #60)
- Now allow empty "printer-alert" values since PWG 5100.9 is ambiguous about
  how to report the absence of alerts (Issue #61)
- The "ippevesubmit" tool now supports the "-r" (replay) option for replaying
  the results of a test (Issue #65)
- The "dnssd-tests.sh" script did not correctly report errors on monochrome
  printers.


v1.1 Update 1 (June 17, 2020)
-----------------------------

- Fixed support for the `-m` option of the `ippevesubmit` command.
- Made output JSON from the `ippevesubmit` command pass as standalone JSON.
- Fixed Windows TLS client support.
- Increased the timeout for the browse tests to 5 seconds.


v1.1 (May 21, 2020)
-------------------

Initial release.
