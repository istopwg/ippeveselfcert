//
// Selfcert submission tool for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//
// Usage:
//
//   ippevesubmit [options] "Printer Name"
//
// Options:
//
//    --help		       Show help.
//    --override               Override test results for granted exception.
//    -f standard              The standard firmware includes IPP Everywhere
//                             support.
//    -f update                A firmware update may be needed.
//    -m models.txt	       Specify list of models, one per line.
//    -o filename.json	       Specify the JSON output file, otherwise JSON is
//			       sent to 'printer name.json'.
//    -p "product family"      Specify the product family.
//    -r {dnssd|document|ipp}  Replay the results of the specified tests.
//    -t {printer|server}      Submit for a printer or print server.
//    -u URL		       Specify the product family web page.
//    -y		       Answer yes to the checklist questions.
//

#include "selfcert.h"


// Local types...
typedef enum media_format_e		// Media Format
{
  MEDIA_FORMAT_SMALL,			// Small media (<A3/Tabloid)
  MEDIA_FORMAT_MEDIUM,			// Medium media (<A1/D)
  MEDIA_FORMAT_LARGE			// Large media (A1/D and larger)
} media_format_t;


// Local functions...
static void	error_cb(void *data, const char *message);
static bool	read_boolean(const char *prompt);
static char	*read_string(const char *prompt, FILE *fp, char *buffer, size_t bufsize);
static void	replay_results(const char *filename, plist_t *results);
static void	usage(void);


//
// 'main()' - Main entry for submission tool.
//

int					// O - Exit status
main(int  argc,				// I - Number of command-line arguments
     char *argv[])			// I - Command-line arguments
{
  int		i;			// Looping var
  const char	*opt,			// Current option
		*family = NULL,		// Product family name
		*json = NULL,		// JSON output file
		*models = NULL,		// File containing a list of models
		*printer = NULL,	// Printer being tested
		*replay = NULL,		// Replay results
		*webpage = NULL;	// Product family web page
  int		override_tests = 0,	// Test results were overridden
		print_server = -1,	// Product is a print server
		firmware_update = -1,	// Is a firmware update needed?
		yes_to_all = 0;		// Answer "yes" to all checklist questions
  char		filename[1024];		// plist filename
  bool		ok = true;		// Are test results OK?
  plist_t	*dnssd_results,		// DNS-SD test results
		*ipp_results,		// IPP test results
		*document_results,	// Document test results
		*submission = NULL;	// Submission data
  char		dnssd_errors[1024],	// DNS-SD tests that failed, if any
		ipp_errors[1024],	// IPP tests that failed, if any
		document_errors[1024];	// Document tests that failed, if any
  char		response[1024];		// Response from user
  FILE		*models_fp;		// Models file
  const char	*models_prompt;		// Prompt for models
  plist_t	*fileid,		// FileId from the first test
		*supported,		// Supported attributes
		*color_supported,	// color-supported value
		*finishings_supported,	// finishings-supported values
		*ipps_supported,	// Is IPPS supported?
		*media_supported,	// media-supported values
		*sides_supported,	// sides-supported values
		*value;			// Value from attributes
  int		finishings_fold = 0,	// Folding?
		finishings_punch = 0,	// Punching?
		finishings_staple = 0,	// Stapling?
		finishings_trim = 0;	// Trimming/cutting?
  struct stat	fileinfo;		// PLIST file information
  time_t	submission_time;	// Date/time of submission (seconds)
  struct tm	submission_tm;		// Date/time of submission (tm data)
  char		submission_date[32],	// Date/time of submission (string)
		submission_version[4];	// Version of the cert tools
  media_format_t media_format = MEDIA_FORMAT_SMALL;
					// Size class
  FILE		*fp;			// Output file
  static const char * const media_formats[] =
  {					// Size classes
    "Small",
    "Medium",
    "Large"
  };


#if _WIN32
  // On Windows, always run from the user's Desktop directory...
  const char *userprofile = getenv("USERPROFILE");
					// User home directory

  if (userprofile)
  {
    if (!_chdir(userprofile))
      _chdir("Desktop");
  }
#endif // _WIN32

  // Parse command-line...
  for (i = 1; i < argc; i ++)
  {
    if (!strcmp(argv[i], "--help"))
    {
      usage();
      return (0);
    }
    else if (!strcmp(argv[i], "--override"))
    {
      override_tests = 1;
    }
    else if (!strncmp(argv[i], "--", 2))
    {
      printf("ippevesubmit: Unknown option '%s'.\n", argv[i]);
      usage();
      return (1);
    }
    else if (argv[i][0] == '-')
    {
      for (opt = argv[i] + 1; *opt; opt ++)
      {
	switch (*opt)
	{
	  case 'f' : // -f {standard|update}
	      i ++;
	      if (i >= argc || (strcmp(argv[i], "standard") && strcmp(argv[i], "update")))
	      {
	        puts("ippevesubmit: Expected 'standard' or 'update' after '-f'.");
	        usage();
	        return (1);
	      }

	      firmware_update = !strcmp(argv[i], "update");
	      break;

	  case 'm' : // -m models.txt
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected filename after '-m'.");
		usage();
		return (1);
	      }

	      if (access(argv[i], R_OK))
	      {
		printf("ippevesubmit: Unable to access models file \"%s\": %s\n", argv[i], strerror(errno));
		return (1);
	      }

	      models = argv[i];
	      break;

	  case 'o' : // -o filename.json
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected filename after '-o'.");
		usage();
		return (1);
	      }

	      json = argv[i];
	      break;

	  case 'p' : // -p "product family"
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected family name after '-p'.");
		usage();
		return (1);
	      }

	      family = argv[i];
	      break;

          case 'r' : // -r {dnssd|ipp|document}
              i ++;
              if (i >= argc || (strcmp(argv[i], "dnssd") && strcmp(argv[i], "document") && strcmp(argv[i], "ipp")))
              {
                puts("ippevesubmit: Expected 'dnssd', 'document', or 'ipp' after '-r'.");
                usage();
                return (1);
              }

              replay = argv[i];
              break;

	  case 't' : // -t {printer|server}
	      i ++;
	      if (i >= argc || (strcmp(argv[i], "printer") && strcmp(argv[i], "server")))
	      {
		puts("ippevesubmit: Expected 'printer' or 'server' after '-t'.");
		usage();
		return (1);
	      }

	      print_server = !strcmp(argv[i], "server");
	      break;

	  case 'u' : // -u URL
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected web page URL after '-u'.");
		usage();
		return (1);
	      }

	      webpage = argv[i];
	      break;

	  case 'y' : // -y (yes to all)
	      yes_to_all = 1;
	      break;

	  default :
	      printf("ippevesubmit: Unknown option '-%c'.\n", *opt);
	      usage();
	      return (1);
	}
      }
    }
    else if (!printer)
    {
      printer = argv[i];
    }
    else
    {
      printf("ippevesubmit: Unknown argument '%s'.\n", argv[i]);
      usage();
      return (1);
    }
  }

  if (!printer)
  {
    usage();
    return (1);
  }

  // Replay results if requested...
  if (replay)
  {
    plist_t	*results;		// Results to replay

    if (!strcmp(replay, "dnssd"))
      snprintf(filename, sizeof(filename), "%s DNS-SD Results.plist", printer);
    else if (!strcmp(replay, "document"))
      snprintf(filename, sizeof(filename), "%s Document Results.plist", printer);
    else
      snprintf(filename, sizeof(filename), "%s IPP Results.plist", printer);

    results = plist_read(NULL, filename, error_cb, NULL);
    replay_results(filename, results);
    return (0);
  }

  // Load test results and validate...
  submission_time = 0;

  snprintf(filename, sizeof(filename), "%s DNS-SD Results.plist", printer);
  if (!stat(filename, &fileinfo) && fileinfo.st_mtime > submission_time)
    submission_time = fileinfo.st_mtime;
  dnssd_results = plist_read(NULL, filename, error_cb, NULL);
  if (!validate_dnssd_results(filename, dnssd_results, print_server, dnssd_errors, sizeof(dnssd_errors)))
    ok = false;

  snprintf(filename, sizeof(filename), "%s IPP Results.plist", printer);
  if (!stat(filename, &fileinfo) && fileinfo.st_mtime > submission_time)
    submission_time = fileinfo.st_mtime;
  ipp_results = plist_read(NULL, filename, error_cb, NULL);
  if (!validate_ipp_results(filename, ipp_results, print_server, ipp_errors, sizeof(ipp_errors)))
    ok = false;

  snprintf(filename, sizeof(filename), "%s Document Results.plist", printer);
  if (!stat(filename, &fileinfo) && fileinfo.st_mtime > submission_time)
    submission_time = fileinfo.st_mtime;
  document_results = plist_read(NULL, filename, error_cb, NULL);
  if (!validate_document_results(filename, document_results, print_server, document_errors, sizeof(document_errors)))
    ok = false;

  if (!ok && !override_tests)
  {
    puts("Unable to submit IPP Everywhere self-certification due to errors.\n");
    if (dnssd_errors[0])
      printf("DNS-SD errors:\n%s\n", dnssd_errors);
    if (ipp_errors[0])
      printf("IPP errors:\n%s\n", ipp_errors);
    if (document_errors[0])
      printf("Document errors:\n%s\n", document_errors);

    return (1);
  }

 /*
  * Collect all information we need for the submission...
  */

  if (!yes_to_all && !read_boolean("Did you use the PWG-supplied self-certification tools"))
    return (1);

  if (!yes_to_all && !read_boolean("Did you use production-ready code"))
    return (1);

  if (!yes_to_all && !read_boolean("Did all output print correctly"))
    return (1);

  if (print_server < 0)
    print_server = read_boolean("Is this a print server");

  if (firmware_update < 0)
    firmware_update = read_boolean("Is IPP Everywhere support part of a firmware update");

  if (!family)
  {
    while (!read_string("Product Family Name", stdin, response, sizeof(response)))
      puts("\b\n    Please enter a name for your product(s).\n");

    family = strdup(response);
  }

  if (!webpage)
  {
    while (!read_string("Product Family Web Page", stdin, response, sizeof(response)))
      puts("\b\n    Please enter a web page URL for your product(s).\n");

    webpage = strdup(response);
  }

  if (strncmp(webpage, "http://", 7) && strncmp(webpage, "https://", 8))
  {
    if (strchr(webpage, '.'))
    {
      char	temp[1024];		// Temporary URL string

      snprintf(temp, sizeof(temp), "https://%s/", webpage);
      free((char *)webpage);
      webpage = strdup(temp);
    }
    else
    {
      printf("ippevesubmit: Bad product URL '%s'.\n", webpage);
      return (1);
    }
  }

 /*
  * Look for the cert version in the IPP results...
  */

  if ((fileid = plist_find(ipp_results, "Tests/0/FileId")) != NULL && !strncmp(fileid->value, "org.pwg.ippeveselfcert", 22) && isdigit(fileid->value[22] & 255) && isdigit(fileid->value[23] & 255))
    opt = fileid->value + 22;
  else
    opt = "??";

  snprintf(submission_version, sizeof(submission_version), "%c.%c", opt[0], opt[1]);

 /*
  * Look for IPPS support in the DNS-SD results...
  */

  ipps_supported = plist_find(dnssd_results, "Tests/4/Successful");

 /*
  * Supported values from the IPP results...
  */

  supported	       = plist_find(ipp_results, "Tests/9/ResponseAttributes/1");
  color_supported      = plist_find(supported, "color-supported");
  finishings_supported = plist_find(supported, "finishings-supported");
  media_supported      = plist_find(supported, "media-supported");
  sides_supported      = plist_find(supported, "sides-supported");

  if (finishings_supported)
  {
   /*
    * Look for specific kinds of finishers...
    */

    for (value = finishings_supported->first_child; value; value = value->next_sibling)
    {
      const char *keyword;		// Keyword value
      if (isdigit(value->value[0] & 255))
        keyword = ippEnumString("finishings", atoi(value->value));
      else
        keyword = value->value;

      if (!strncmp(keyword, "fold", 4))
	finishings_fold = 1;
      else if (!strncmp(keyword, "punch", 5))
	finishings_punch = 1;
      else if (!strncmp(keyword, "staple", 6))
	finishings_staple = 1;
      else if (!strncmp(keyword, "trim", 4))
	finishings_trim = 1;
    }
  }

  if (media_supported)
  {
   /*
    * Look at the supported media sizes - large format is more than 22" wide,
    * and medium format is more than 11" and less than 22" wide...
    */

    for (value = media_supported->first_child; value; value = value->next_sibling)
    {
      pwg_media_t *pwg = pwgMediaForPWG(value->value);
					// Decoded PWG size name

      if (!pwg)
	continue;

      if (pwg->width > 43180)
	media_format = MEDIA_FORMAT_LARGE;
      else if (pwg->width > 22860 && media_format < MEDIA_FORMAT_MEDIUM)
	media_format = MEDIA_FORMAT_MEDIUM;
    }
  }

 /*
  * Build the submission profile...
  */

  submission = plist_new();

  if (!submission_time)
    time(&submission_time);
  gmtime_r(&submission_time, &submission_tm);

  snprintf(submission_date, sizeof(submission_date), "%04d-%02d-%02dT%02d:%02d:%02dZ", submission_tm.tm_year + 1900, submission_tm.tm_mon + 1, submission_tm.tm_mday, submission_tm.tm_hour, submission_tm.tm_min, submission_tm.tm_sec);

  if (models)
  {
    models_fp	  = fopen(models, "r");
    models_prompt = NULL;
  }
  else
  {
    models_fp	  = stdin;
    models_prompt = "First Model Name";
  }

  while (read_string(models_prompt, models_fp, response, sizeof(response)))
  {
    if (models_prompt)
      models_prompt = "Next Model Name (blank when done)";

    plist_t *dict = plist_add(submission, PLIST_TYPE_DICT, NULL);

    plist_add(dict, PLIST_TYPE_KEY, "family");
    plist_add(dict, PLIST_TYPE_STRING, family);

    plist_add(dict, PLIST_TYPE_KEY, "model");
    plist_add(dict, PLIST_TYPE_STRING, response);

    plist_add(dict, PLIST_TYPE_KEY, "url");
    plist_add(dict, PLIST_TYPE_STRING, webpage);

    plist_add(dict, PLIST_TYPE_KEY, "color");
    plist_add(dict, PLIST_TYPE_STRING, (color_supported && color_supported->type == PLIST_TYPE_TRUE) ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "duplex");
    plist_add(dict, PLIST_TYPE_STRING, plist_array_count(sides_supported) > 1 ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "finishings");
    plist_add(dict, PLIST_TYPE_STRING, plist_array_count(finishings_supported) > 1 ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "fin_fold");
    plist_add(dict, PLIST_TYPE_STRING, finishings_fold ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "fin_punch");
    plist_add(dict, PLIST_TYPE_STRING, finishings_punch ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "fin_staple");
    plist_add(dict, PLIST_TYPE_STRING, finishings_staple ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "fin_trim");
    plist_add(dict, PLIST_TYPE_STRING, finishings_trim ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "ipps");
    plist_add(dict, PLIST_TYPE_STRING, ipps_supported ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "firmware_update");
    plist_add(dict, PLIST_TYPE_STRING, firmware_update ? "1" : "0");

    plist_add(dict, PLIST_TYPE_KEY, "media");
    plist_add(dict, PLIST_TYPE_STRING, media_formats[media_format]);

    plist_add(dict, PLIST_TYPE_KEY, "version");
    plist_add(dict, PLIST_TYPE_STRING, submission_version);

    plist_add(dict, PLIST_TYPE_KEY, "date");
    plist_add(dict, PLIST_TYPE_STRING, submission_date);
  }

 /*
  * Write JSON file for submission...
  */

  if (!json)
  {
    snprintf(filename, sizeof(filename), "%s.json", printer);
    json = filename;
  }

  if (!strcmp(json, "-"))
    fp = stdout;
  else
    fp = fopen(json, "w");

  if (!fp)
  {
    printf("ippevesubmit: Unable to create '%s': %s\n", json, strerror(errno));
    return (1);
  }

  if (override_tests)
  {
    fputs("// Note: submitted with --override\n", fp);

    if (dnssd_errors[0])
      fprintf(fp, "/* DNS-SD errors:\n%s*/\n", dnssd_errors);
    if (ipp_errors[0])
      fprintf(fp, "/* IPP errors:\n%s*/\n", ipp_errors);
    if (document_errors[0])
      fprintf(fp, "/* Document errors:\n%s*/\n", document_errors);
  }

  plist_write_json(fp, json, submission, error_cb, NULL);

  if (fp != stdout)
  {
    fclose(fp);
    printf("\nWrote submission to '%s'.\n", json);
  }

  puts("\nNow continue with your submission at:\n\n    https://www.pwg.org/ippeveselfcert\n");

  return (0);
}


//
// 'error_cb()' - Display an error message.
//

static void
error_cb(void       *data,		// I - Callback data (unused)
         const char *message)		// I - Message string
{
  (void)data;

  fprintf(stderr, "ippevesubmit: %s\n", message);
}


//
// 'read_boolean()' - Ask a yes/no question.
//

static bool				// O - `true` if yes, `false` if no
read_boolean(const char *prompt)	// I - Question to ask
{
  char	buffer[256];			// Response buffer


  printf("%s (y/N)? ", prompt);
  fflush(stdout);

  if (fgets(buffer, sizeof(buffer), stdin) && toupper(buffer[0] & 255) == 'Y')
    return (true);
  else
    return (false);
}


//
// 'read_string()' - Read a string response from the console or a file.
//

static char *				// O - String or `NULL` if none
read_string(const char *prompt,		// I - Prompt (if interactive)
	    FILE       *fp,		// I - File to read from
	    char       *buffer,		// I - Response buffer
	    size_t     bufsize)		// I - Size of response buffer
{
  char	*bufptr;			// Pointer into buffer


  if (prompt)
  {
    // Show prompt...
    printf("%s? ", prompt);
    fflush(stdout);
  }

  if (fgets(buffer, bufsize, fp))
  {
    // Got a line from the user, strip trailing whitespace...
    for (bufptr = buffer + strlen(buffer) - 1; bufptr >= buffer; bufptr --)
    {
      if (isspace(*bufptr & 255))
	*bufptr = '\0';
      else
        break;
    }

    // If there is anything left, return it...
    if (buffer[0])
      return (buffer);
  }

  return (NULL);
}


//
// 'replay_results()' - Replay the results from a test.
//

static void
replay_results(const char *filename,	// I - Filename
               plist_t    *results)	// I - Results
{
  plist_t	*tests,			// Tests array
		*test,			// Current test dictionary
		*name,			// Test name ("Name" string)
		*successful,		// Test status ("Successful" boolean)
		*skipped,		// Test skipped? ("Skipped" boolean)
		*errors;		// Test errors, if any ("Errors" array)
  const char	*status;		// Status to display
  int		total = 0,		// Test counts
		pass = 0,
		skip = 0,
		fail = 0;


  printf("\"%s\":\n", filename);
  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    puts("    No tests found in results.");
    return;
  }

  for (test = tests->first_child; test; test = test->next_sibling)
  {
    name       = plist_find(test, "Name");
    successful = plist_find(test, "Successful");
    skipped    = plist_find(test, "Skipped");
    errors     = plist_find(test, "Errors");

    if (!name || name->type != PLIST_TYPE_STRING || !successful)
      continue;

    total ++;

    if (skipped && skipped->type == PLIST_TYPE_TRUE)
    {
      status = "SKIP";
      skip ++;
    }
    else if (successful->type == PLIST_TYPE_TRUE)
    {
      status = "PASS";
      pass ++;
    }
    else
    {
      status = "FAIL";
      fail ++;
    }

    printf("    %-68.68s [%s]\n", name->value, status);

    if (errors && errors->type == PLIST_TYPE_ARRAY)
    {
      plist_t	*error;			// Current error

      for (error = errors->first_child; error; error = error->next_sibling)
        printf("        %s\n", error->value);
    }
  }

  printf("\nSummary: %d tests, %d passed, %d failed, %d skipped\n", total, pass, fail, skip);
  printf("Score: %d%%\n", 100 * (pass + skip) / total);
}


//
// 'usage()' - Show program usage.
//

static void
usage(void)
{
  puts("Usage: ippevesubmit [options] \"Printer Name\"");
  puts("");
  puts("Options:");
  puts("  --help	           Show help.");
  puts("  -f standard              The standard firmware supports IPP Everywhere.");
  puts("  -f update                The firmware may need to be updated.");
  puts("  -m models.txt	           Specify a list of models, one per line.");
  puts("  -o filename.json         Specify the JSON output file, otherwise JSON is");
  puts("		           sent to 'printer name.json'.");
  puts("  -p \"product family\"      Specify the product family.");
  puts("  -r {dnssd|document|ipp}  Replay the results for the specified tests");
  puts("  -t {printer|server}      Submit for a printer or print server.");
  puts("  -u URL	           Specify the product family web page.");
  puts("  -y		           Answer yes to the checklist questions.");
}
