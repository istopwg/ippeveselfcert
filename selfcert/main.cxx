//
// Selfcert main entry for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//

#include "SelfCertApp.h"
#include "selfcert.h"


//
// 'main()' - Main entry.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line arguments
     char *argv[])			// I - Command-line arguments
{
  SelfCertApp	*app = new SelfCertApp();
					// Self-certification application


  app->show(argc, argv);
  Fl::run();

  return (0);
}


//
// 'SelfCertApp::test()' - Run self-certification tests and display the output.
//

bool					// O - Result of tests
SelfCertApp::test(const char *name)	// I - Printer name
{
  size_t	i;			// Looping var
  static const char *lines[] =
  {
    "DNS-SD Tests:\n",
    "B-1. IPP Browse test: PASS\n",
    "B-2. IPP TXT keys test: PASS\n",
    "B-3. IPP Resolve test: PASS\n",
    "B-4. IPP TXT values test: ipp://mbp14.local:8501/ipp/print\n",
    "FAIL\n",
    "\"dnssd-value-tests.test\":\n",
    "    Validate TXT record values using Get-Printer-Attributes              [FAIL]\n",
    "        RECEIVED: 8458 bytes in response\n",
    "        status-code = successful-ok (successful-ok)\n",
    "        EXPECTED: printer-more-info WITH-VALUE \"https://mbp14.local.:8501/\"\n",
    "        GOT: printer-more-info=\"https://localhost:8501/\"\n",
    "B-5. TLS tests: PASS\n",
    "B-5.1 HTTP Upgrade test: FAIL\n",
    "    ipptool: Unable to connect to 'mbp14.local' on port 8501: Encryption is not supported.\n",
    "B-5.2 IPPS Browse test: PASS\n",
    "B-5.3 IPPS TXT keys test: PASS\n",
    "B-5.4 IPPS Resolve test: PASS\n",
    "B-5.5 IPPS TXT values test: ipp://mbp14.local:8501/ipp/print\n",
    "FAIL\n",
    "\"dnssd-value-tests.test\":\n",
    "    Validate TXT record values using Get-Printer-Attributes              [FAIL]\n",
    "        RECEIVED: 8458 bytes in response\n",
    "        status-code = successful-ok (successful-ok)\n",
    "        EXPECTED: printer-more-info WITH-VALUE \"https://mbp14.local.:8501/\"\n",
    "        GOT: printer-more-info=\"https://localhost:8501/\"\n",
    "Summary: 10 tests, 7 passed, 3 failed, 0 skipped\n",
    "Score: 70%\n",
    "IPP Tests:\n",
    "\"ipp-tests.test\":\n",
    "    I-1. RFC 8011 section 4.1.1: Bad request-id value 0                  [PASS]\n",
    "    I-2. RFC 8011 section 4.1.4: No Operation Attributes                 [PASS]\n",
    "    I-3. RFC 8011 section 4.1.4: attributes-charset                      [PASS]\n",
    "    I-4. RFC 8011 section 4.1.4: attributes-natural-language             [PASS]\n",
    "    I-5. RFC 8011 section 4.1.4: attributes-natural-language + attribute [PASS]\n",
    "    I-6. RFC 8011 section 4.1.4: attributes-charset + attributes-natural [PASS]\n",
    "    I-7. RFC 8011 section 4.1.8: Unsupported IPP version 0.0             [PASS]\n",
    "    I-8. RFC 8011 section 4.2: No printer-uri operation attribute        [PASS]\n",
    "    I-9. Identify-Printer Operation                                      [PASS]\n",
    "    I-10. Get-Printer-Attributes Operation (default)                     [FAIL]\n",
    "        RECEIVED: 8458 bytes in response\n",
    "        status-code = successful-ok (successful-ok)\n",
    "        EXPECTED: printer-icons WITH-ALL-HOSTNAMES \"mbp14.local\"\n",
    "        GOT: printer-icons=\"https://localhost:8501/icon-sm.png\"\n",
    "        GOT: printer-icons=\"https://localhost:8501/icon.png\"\n",
    "        GOT: printer-icons=\"https://localhost:8501/icon-lg.png\"\n",
    "        EXPECTED: printer-more-info WITH-HOSTNAME \"mbp14.local\"\n",
    "        GOT: printer-more-info=\"https://localhost:8501/\"\n",
    "        EXPECTED: printer-supply-info-uri WITH-HOSTNAME \"mbp14.local\"\n",
    "        GOT: printer-supply-info-uri=\"https://localhost:8501/supplies\"\n",
    "    I-10.1 Get-Printer-Attributes Operation (all)                        [PASS]\n",
    "    I-10.2. Get-Printer-Attributes Operation (all, media-col-database)   [PASS]\n",
    "    I-10.3. Get-Printer-Attributes Operation (none)                      [PASS]\n",
    "    I-10.4. Get-Printer-Attributes Operation (media-col-database)        [PASS]\n",
    "    I-10.5. Get-Printer-Attributes Operation (printer-description)       [PASS]\n",
    "    I-10.6. Get-Printer-Attributes Operation (job-template)              [PASS]\n",
    "    I-10.7 Get-Printer-Attributes Operation (media-col-database, printer [PASS]\n",
    "    I-11. Validate-Job Operation                                         [PASS]\n",
    "    I-12. Print-Job Operation (Print-Job Test onepage)                   [PASS]\n",
    "    I-13. Get-Jobs Operation (default)                                   [PASS]\n",
    "    I-13.1 Get-Jobs Operation (requested-attributes)                     [PASS]\n",
    "    I-13.2 Get-Jobs Operation (which-jobs=not-completed)                 [PASS]\n",
    "    I-14. Get-Job-Attributes Operation (until job complete)              [0001]\n",
    "        job-state (enum) = processing\n",
    "    I-14. Get-Job-Attributes Operation (until job complete)              [0002]\n",
    "        job-state (enum) = processing\n",
    "    I-14. Get-Job-Attributes Operation (until job complete)              [0003]\n",
    "        job-state (enum) = processing\n",
    "    I-14. Get-Job-Attributes Operation (until job complete)              [PASS]\n",
    "    I-15. Get-Jobs Operation (which-jobs=completed)                      [PASS]\n",
    "    I-15.1 Get-Jobs Operation (which-jobs, requested-attributes)         [PASS]\n",
    "    I-16. Cancel-Job Operation (completed job)                           [PASS]\n",
    "    I-16.1 Cancel-Job Operation (Cancel-Job Test onepage)                [PASS]\n",
    "    I-16.2. Cancel-Job Operation (pending/processing job)                [PASS]\n",
    "    I-16.3 Cancel-Job Operations (Get-Job-Attributes)                    [PASS]\n",
    "    I-17. Cancel-My-Jobs Operation (Cancel-My-Jobs Test onepage)         [0001]\n",
    "    I-17. Cancel-My-Jobs Operation (Cancel-My-Jobs Test onepage)         [0002]\n",
    "    I-17. Cancel-My-Jobs Operation (Cancel-My-Jobs Test onepage)         [PASS]\n",
    "    I-17.1 Cancel-My-Jobs Operation (Cancel-My-Jobs)                     [PASS]\n",
    "    I-17.2 Cancel-My-Jobs Operations (Get-Job-Attributes)                [PASS]\n",
    "    I-18. Create-Job + Send-Document Operations (Create-Job)             [0001]\n",
    "    I-18. Create-Job + Send-Document Operations (Create-Job)             [0002]\n",
    "    I-18. Create-Job + Send-Document Operations (Create-Job)             [PASS]\n",
    "    I-18.1 Create-Job + Send-Document Operations (Send-Document onepage) [PASS]\n",
    "    I-18.2 Create-Job + Send-Document Operations (Get-Job-Attributes unt [0001]\n",
    "        job-state (enum) = processing\n",
    "    I-18.2 Create-Job + Send-Document Operations (Get-Job-Attributes unt [0002]\n",
    "        job-state (enum) = processing\n",
    "    I-18.2 Create-Job + Send-Document Operations (Get-Job-Attributes unt [PASS]\n",
    "    I-19. Close-Job Test (Create-Job Operation)                          [PASS]\n",
    "    I-19.1 Close-Job Test (Close-Job Operation)                          [PASS]\n",
    "    I-19.2 Close-Job Test (Get-Job-Attributes Operation)                 [PASS]\n",
    "    I-19.3. Close-Job Test (Cancel-Job Operation)                        [PASS]\n",
    "    I-20. media-needed (media-needed Test onepage)                       [SKIP]\n",
    "    I-20.1 media-needed (Get-Printer-Attributes)                         [SKIP]\n",
    "\n",
    "Summary: 41 tests, 38 passed, 1 failed, 2 skipped\n",
    "Score: 97%\n",
    "Document Tests:\n",
    "\"document-tests.test\":\n",
    "    D-1. PWG Raster Format Tests (mandatory)                             [PASS]\n",
    "    D-1.1 Print PWG @ maximum resolution (black_1, no compression)       [PASS]\n",
    "    D-1.1 Wait for job to complete...                                    [0001]\n",
    "    D-1.1 Wait for job to complete...                                    [0002]\n",
    "    D-1.1 Wait for job to complete...                                    [PASS]\n",
    "    D-1.1 Print PWG @ maximum resolution, (sgray_8, no compression)      [PASS]\n",
    "    D-1.1 Wait for job to complete...                                    [0001]\n",
    "    D-1.1 Wait for job to complete...                                    [0002]\n",
    "    D-1.1 Wait for job to complete...                                    [PASS]\n",
    "    D-1.1 Print PWG @ maximum resolution (srgb_8, no compression)        [PASS]\n",
    "    D-1.1 Wait for job to complete...                                    [0001]\n",
    "    D-1.1 Wait for job to complete...                                    [0002]\n",
    "    D-1.1 Wait for job to complete...                                    [PASS]\n",
    "    D-1.1 Print PWG @ maximum resolution (cmyk_8, no compression)        [SKIP]\n",
    "    D-1.1 Wait for job to complete...                                    [SKIP]\n",
    "ipptool: Unsupported COMPRESSION value \"\" on line 191 of 'document-tests.test'.\n",
    "\n",
    "Summary: 9 tests, 7 passed, 0 failed, 2 skipped\n",
    "Score: 100%\n"
  };


  if (!name)
  {
    puts("No printer selected.");
    return (false);
  }

  puts(name);

  browser->deactivate();
  resultsSubmit->deactivate();
  buffer->text("");

  for (i = 0; i < (sizeof(lines) / sizeof(lines[0])); i ++)
  {
    output->insert(lines[i]);
    output->show_insert_position();

    if (!strncmp(lines[i], "D-", 2) || !strncmp(lines[i], "    I-", 6) || !strncmp(lines[i], "    D-", 6))
      Fl::wait(0.25);
    else
      Fl::wait(0.01);
  }

  resultsSubmit->activate();
  browser->activate();

  return (true);
}
