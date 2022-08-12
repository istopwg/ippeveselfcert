//
// Selfcert validation code for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//

#include "selfcert.h"


//
// 'validate_dnssd_results()' - Validate the results from the DNS-SD tests.
//

bool					// O - `true` on success, `false` on failure
validate_dnssd_results(
    const char *filename,		// I - plist filename
    plist_t    *results,		// I - DNS-SD results
    int	       print_server,		// I - Certifying a print server?
    char       *errors,			// O - Error buffer
    size_t     errsize)			// I - Size of error buffer
{
  bool		result = true;		// Success/fail result
  plist_t	*fileid,		// FileId value
		*successful,		// Successful value
		*tests,			// Tests array
		*test;			// Current test
  int		number,			// Test number
		tests_count = 0;	// Number of tests
  char		*errptr = errors;	// Pointer into errors


  *errors = '\0';

  if ((fileid = plist_find(results, "Tests/0/FileId")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing FileId.\n");
    return (0);
  }
  else if (fileid->type != PLIST_TYPE_STRING)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "FileId is not a string value.\n");
    return (0);
  }
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert11.dnssd"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    result = false;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = false;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = false;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.dnssd") && tests_count != 10)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 10).\n", tests_count);
    result = false;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					// Test name
		*tsuccessful = plist_find(test, "Successful"),
					// Was the test successful?
		*terrors = plist_find(test, "Errors"),
					// What errors occurred?
		*terror;		// Current error message

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = false;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
        // Test failed, show error...
	result = false;

	snprintf(errptr, errsize - (size_t)(errptr - errors), "FAILED %s\n", tname->value);
	errptr += strlen(errptr);

	for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	{
	  if (terror->type != PLIST_TYPE_STRING)
	    continue;

	  snprintf(errptr, errsize - (size_t)(errptr - errors), "%s\n", terror->value);
	  errptr += strlen(errptr);
	}
      }
    }
  }

  return (result);
}


//
// 'validate_document_results()' - Validate the results from the document tests.
//

bool					// O - `true` on success, `false` on failure
validate_document_results(
    const char *filename,		// I - plist filename
    plist_t    *results,		// I - Document results
    int	       print_server,		// I - Certifying a print server?
    char       *errors,			// O - Error buffer
    size_t     errsize)			// I - Size of error buffer
{
  bool		result = true;		// Success/fail result
  plist_t	*fileid,		// FileId value
		*successful,		// Successful value
		*tests,			// Tests array
		*test;			// Current test
  int		number,			// Test number
		tests_count = 0;	// Number of tests
  char		*errptr = errors;	// Pointer into errors


  *errors = '\0';

  if ((fileid = plist_find(results, "Tests/0/FileId")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing FileId.\n");
    return (0);
  }
  else if (fileid->type != PLIST_TYPE_STRING)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "FileId is not a string value.\n");
    return (0);
  }
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert11.document"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    errptr += strlen(errptr);
    result = false;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = false;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = false;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.document") && tests_count != 53)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 53).\n", tests_count);
    errptr += strlen(errptr);
    result = false;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					// Test name
		*tsuccessful = plist_find(test, "Successful"),
					// Was the test successful?
		*terrors = plist_find(test, "Errors"),
					// What errors occurred?
		*terror;		// Current error message

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = false;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
        // Test failed, show errors...
	result = false;

	snprintf(errptr, errsize - (size_t)(errptr - errors), "FAILED %s\n", tname->value);
	errptr += strlen(errptr);

	for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	{
	  if (terror->type != PLIST_TYPE_STRING)
	    continue;

	  snprintf(errptr, errsize - (size_t)(errptr - errors), "%s\n", terror->value);
	  errptr += strlen(errptr);
	}
      }
    }
  }

  return (result);
}


//
// 'validate_ipp_results()' - Validate the results from the IPP tests.
//

bool					// O - `true` on success, `false` on failure
validate_ipp_results(
    const char *filename,		// I - plist filename
    plist_t    *results,		// I - IPP results
    int	       print_server,		// I - Certifying a print server?
    char       *errors,			// O - Error buffer
    size_t     errsize)			// I - Size of error buffer
{
  bool		result = true;		// Success/fail result
  plist_t	*fileid,		// FileId value
		*successful,		// Successful value
		*tests,			// Tests array
		*test;			// Current test
  int		number,			// Test number
		tests_count = 0;	// Number of tests
  char		*errptr = errors;	// Pointer into error buffer


  *errors = '\0';

  if ((fileid = plist_find(results, "Tests/0/FileId")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing FileId.\n");
    return (0);
  }
  else if (fileid->type != PLIST_TYPE_STRING)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "FileId is not a string value.\n");
    return (0);
  }
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    errptr += strlen(errptr);
    result = false;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = false;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = false;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = false;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp") && tests_count != 41)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 41).\n",  tests_count);
    errptr += strlen(errptr);
    result = false;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					// Test name
		*tsuccessful = plist_find(test, "Successful"),
					// Was the test successful?
		*terrors = plist_find(test, "Errors"),
					// What errors occurred?
		*terror;		// Current error message

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = false;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
        // Test failed...
        result = false;

	snprintf(errptr, errsize - (size_t)(errptr - errors), "FAILED %s\n", tname->value);
	errptr += strlen(errptr);

	for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	{
	  if (terror->type != PLIST_TYPE_STRING)
	    continue;

	  snprintf(errptr, errsize - (size_t)(errptr - errors), "%s\n", terror->value);
	  errptr += strlen(errptr);
	}
      }
    }
  }

  return (result);
}
