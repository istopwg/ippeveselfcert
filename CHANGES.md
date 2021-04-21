Changes
=======

v1.1 Update 3 (PENDING)
-----------------------

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
