# Changes

## v1.1 Update 2 (???, 2020)

- Documentation updates (Issue #55)
- The "printer-alert" test did not allow an index value of -1 (Issue #58)
- The I-10 test didn't allow 'name' values for the
  "finishing-template-supported" attribute (Issue #59)
- Now allow the I-10.3 test to return the
  'successful-ok-ignored-or-substituted-attributes' status code (Issue #60)
- Now allow empty "printer-alert" values since PWG 5100.9 is ambiguous about
  how to report the absence of alerts (Issue #61)

## v1.1 Update 1 (June 17, 2020)

- Fixed support for the `-m` option of the `ippevesubmit` command.
- Made output JSON from the `ippevesubmit` command pass as standalone JSON.
- Fixed Windows TLS client support.
- Increased the timeout for the browse tests to 5 seconds.


## v1.1 (May 21, 2020)

Initial release.
