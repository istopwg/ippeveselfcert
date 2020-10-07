/*
 * IPP Everywhere Printer Self-Certification submission tool
 *
 * Copyright © 2019-2020 by the IEEE-ISTO Printer Working Group.
 * Copyright © 2019 by Apple Inc.
 *
 * Licensed under Apache License v2.0.	See the file "LICENSE" for more
 * information.
 *
 * Usage:
 *
 *   ippevesubmit [options] "Printer Name"
 *
 * Options:
 *
 *    --help		       Show help.
 *    --override               Override test results for granted exception.
 *    -f standard              The standard firmware includes IPP Everywhere
 *                             support.
 *    -f update                A firmware update may be needed.
 *    -m models.txt	       Specify list of models, one per line.
 *    -o filename.json	       Specify the JSON output file, otherwise JSON is
 *			       sent to 'printer name.json'.
 *    -p "product family"      Specify the product family.
 *    -r {dnssd|document|ipp}  Replay the results of the specified tests.
 *    -t {printer|server}      Submit for a printer or print server.
 *    -u URL		       Specify the product family web page.
 *    -y		       Answer yes to the checklist questions.
 */


/*
 * Include necessary headers...
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifndef _WIN32
#  include <unistd.h>
#endif /* !_WIN32 */
#include <cups/cups.h>


/*
 * Local types...
 */

typedef enum media_format_e		/**** Media Format ****/
{
  MEDIA_FORMAT_SMALL,			/* Small media (<A3/Tabloid) */
  MEDIA_FORMAT_MEDIUM,			/* Medium media (<A1/D) */
  MEDIA_FORMAT_LARGE			/* Large media (A1/D and larger) */
} media_format_t;

typedef enum plist_type_e		/**** plist Data Type */
{
  PLIST_TYPE_PLIST,			/* <plist> ... </plist> */
  PLIST_TYPE_ARRAY,			/* <array> ... </array> */
  PLIST_TYPE_DICT,			/* <dict> ... </dict> */
  PLIST_TYPE_KEY,			/* <key>value</key> */
  PLIST_TYPE_DATA,			/* <data>value</data> */
  PLIST_TYPE_DATE,			/* <date>value</date> */
  PLIST_TYPE_FALSE,			/* <false /> */
  PLIST_TYPE_INTEGER,			/* <integer>value</integer> */
  PLIST_TYPE_STRING,			/* <string>value</string> */
  PLIST_TYPE_TRUE			/* <true /> */
} plist_type_t;

typedef struct plist_s			/**** plist Data Node ****/
{
  plist_type_t		type;		/* Node type */
  struct plist_s	*parent,	/* Parent node, if any */
			*first_child,	/* First child node, if any */
			*last_child,	/* Last child node, if any */
			*prev_sibling,	/* Previous sibling node, if any */
			*next_sibling;	/* Next sibling node, if any */
  char			*value;		/* Value (as a string), if any */
} plist_t;


/*
 * Local functions...
 */

static void	json_puts(FILE *fp, const char *s);
static void	json_write_plist(FILE *fp, plist_t *plist);
static plist_t	*plist_add(plist_t *parent, plist_type_t type, const char *value);
static int	plist_array_count(plist_t *plist);
static void	plist_delete(plist_t *plist);
static plist_t	*plist_find(plist_t *parent, const char *path);
static plist_t	*plist_read(const char *filename);
static int	read_boolean(const char *prompt);
static char	*read_string(const char *prompt, FILE *fp, char *buffer, size_t bufsize);
static void	replay_results(const char *filename, plist_t *results);
static void	usage(void);
static int	validate_dnssd_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);
static int	validate_document_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);
static int	validate_ipp_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);
static char	*xml_gets(FILE *fp, char *buffer, size_t bufsize, int *linenum);
static void	xml_unescape(char *buffer);


/*
 * 'main()' - Main entry for submission tool.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line arguments */
     char *argv[])			/* I - Command-line arguments */
{
  int		i;			/* Looping var */
  const char	*opt,			/* Current option */
		*family = NULL,		/* Product family name */
		*json = NULL,		/* JSON output file */
		*models = NULL,		/* File containing a list of models */
		*printer = NULL,	/* Printer being tested */
		*replay = NULL,		/* Replay results */
		*webpage = NULL;	/* Product family web page */
  int		override_tests = 0,	/* Test results were overridden */
		print_server = -1,	/* Product is a print server */
		firmware_update = -1,	/* Is a firmware update needed? */
		yes_to_all = 0;		/* Answer "yes" to all checklist questions */
  char		filename[1024];		/* plist filename */
  int		ok = 1;			/* Are test results OK? */
  plist_t	*dnssd_results,		/* DNS-SD test results */
		*ipp_results,		/* IPP test results */
		*document_results,	/* Document test results */
		*submission = NULL;	/* Submission data */
  char		dnssd_errors[1024],	/* DNS-SD tests that failed, if any */
		ipp_errors[1024],	/* IPP tests that failed, if any */
		document_errors[1024];	/* Document tests that failed, if any */
  char		response[1024];		/* Response from user */
  FILE		*models_fp;		/* Models file */
  const char	*models_prompt;		/* Prompt for models */
  plist_t	*fileid,		/* FileId from the first test */
		*supported,		/* Supported attributes */
		*color_supported,	/* color-supported value */
		*finishings_supported,	/* finishings-supported values */
		*ipps_supported,	/* Is IPPS supported? */
		*media_supported,	/* media-supported values */
		*sides_supported,	/* sides-supported values */
		*value;			/* Value from attributes */
  int		finishings_fold = 0,	/* Folding? */
		finishings_punch = 0,	/* Punching? */
		finishings_staple = 0,	/* Stapling? */
		finishings_trim = 0;	/* Trimming/cutting? */
  time_t	submission_time;	/* Date/time of submission (seconds) */
  struct tm	submission_tm;		/* Date/time of submission (tm data) */
  char		submission_date[32],	/* Date/time of submission (string) */
		submission_version[4];	/* Version of the cert tools */
  media_format_t media_format = MEDIA_FORMAT_SMALL;
					/* Size class */
  FILE		*fp;			/* Output file */
  static const char * const media_formats[] =
  {					/* Size classes */
    "Small",
    "Medium",
    "Large"
  };


#if _WIN32
 /*
  * On Windows, always run from the user's Desktop directory...
  */

  const char *userprofile = getenv("USERPROFILE");
					/* User home directory */

  if (userprofile)
  {
    if (!_chdir(userprofile))
      _chdir("Desktop");
  }
#endif /* _WIN32 */

 /*
  * Parse command-line...
  */

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
	  case 'f' : /* -f {standard|update} */
	      i ++;
	      if (i >= argc || (strcmp(argv[i], "standard") && strcmp(argv[i], "update")))
	      {
	        puts("ippevesubmit: Expected 'standard' or 'update' after '-f'.");
	        usage();
	        return (1);
	      }

	      firmware_update = !strcmp(argv[i], "update");
	      break;

	  case 'm' : /* -m models.txt */
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

	  case 'o' : /* -o filename.json */
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected filename after '-o'.");
		usage();
		return (1);
	      }

	      json = argv[i];
	      break;

	  case 'p' : /* -p "product family" */
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected family name after '-p'.");
		usage();
		return (1);
	      }

	      family = argv[i];
	      break;

          case 'r' : /* -r {dnssd|ipp|document} */
              i ++;
              if (i >= argc || (strcmp(argv[i], "dnssd") && strcmp(argv[i], "document") && strcmp(argv[i], "ipp")))
              {
                puts("ippevesubmit: Expected 'dnssd', 'document', or 'ipp' after '-r'.");
                usage();
                return (1);
              }

              replay = argv[i];
              break;

	  case 't' : /* -t {printer|server} */
	      i ++;
	      if (i >= argc || (strcmp(argv[i], "printer") && strcmp(argv[i], "server")))
	      {
		puts("ippevesubmit: Expected 'printer' or 'server' after '-t'.");
		usage();
		return (1);
	      }

	      print_server = !strcmp(argv[i], "server");
	      break;

	  case 'u' : /* -u URL */
	      i ++;
	      if (i >= argc)
	      {
		puts("ippevesubmit: Expected web page URL after '-u'.");
		usage();
		return (1);
	      }

	      webpage = argv[i];
	      break;

	  case 'y' : /* -y (yes to all) */
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

 /* 
  * Replay results if requested...
  */

  if (replay)
  {
    plist_t	*results;		/* Results to replay */

    if (!strcmp(replay, "dnssd"))
      snprintf(filename, sizeof(filename), "%s DNS-SD Results.plist", printer);
    else if (!strcmp(replay, "document"))
      snprintf(filename, sizeof(filename), "%s Document Results.plist", printer);
    else
      snprintf(filename, sizeof(filename), "%s IPP Results.plist", printer);

    results = plist_read(filename);
    replay_results(filename, results);
    return (0);
  }

 /*
  * Load test results and validate...
  */

  snprintf(filename, sizeof(filename), "%s DNS-SD Results.plist", printer);
  dnssd_results = plist_read(filename);
  if (!validate_dnssd_results(filename, dnssd_results, print_server, dnssd_errors, sizeof(dnssd_errors)))
    ok = 0;

  snprintf(filename, sizeof(filename), "%s IPP Results.plist", printer);
  ipp_results = plist_read(filename);
  if (!validate_ipp_results(filename, ipp_results, print_server, ipp_errors, sizeof(ipp_errors)))
    ok = 0;

  snprintf(filename, sizeof(filename), "%s Document Results.plist", printer);
  document_results = plist_read(filename);
  if (!validate_document_results(filename, document_results, print_server, document_errors, sizeof(document_errors)))
    ok = 0;

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

  supported	       = plist_find(ipp_results, "Tests/8/ResponseAttributes/1");
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
      if (!strncmp(value->value, "fold", 4))
	finishings_fold = 1;
      else if (!strncmp(value->value, "punch", 5))
	finishings_punch = 1;
      else if (!strncmp(value->value, "staple", 6))
	finishings_staple = 1;
      else if (!strncmp(value->value, "trim", 4))
	finishings_trim = 1;
    }
  }

  if (media_supported)
  {
   /*
    * Look at the supported media sizes - large format is more than 22" wide,
    * and medium format is more than 11" and less than 22" wide...
    */

    for (value = finishings_supported->first_child; value; value = value->next_sibling)
    {
      pwg_media_t *pwg = pwgMediaForPWG(value->value);
					/* Decoded PWG size name */

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

  submission = plist_add(NULL, PLIST_TYPE_ARRAY, NULL);

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
    fputs("/* Note: submitted with --override */\n", fp);

    if (dnssd_errors[0])
      fprintf(fp, "/* DNS-SD errors:\n%s*/\n", dnssd_errors);
    if (ipp_errors[0])
      fprintf(fp, "/* IPP errors:\n%s*/\n", ipp_errors);
    if (document_errors[0])
      fprintf(fp, "/* Document errors:\n%s*/\n", document_errors);
  }

  json_write_plist(fp, submission);

  if (fp != stdout)
  {
    fclose(fp);
    printf("\nWrote submission to '%s'.\n", json);
  }

  puts("\nNow continue with your submission at:\n\n    https://www.pwg.org/ippeveselfcert\n");

  return (0);
}


/*
 * 'json_puts()' - Write a string with JSON encoding to a file.
 */

static void
json_puts(FILE	     *fp,		/* I - File to write to */
	  const char *s)		/* I - String to write */
{
  putc('\"', fp);

  while (*s)
  {
    if (*s == '\b')
      fputs("\\b", fp);
    else if (*s == '\f')
      fputs("\\f", fp);
    else if (*s == '\n')
      fputs("\\n", fp);
    else if (*s == '\r')
      fputs("\\r", fp);
    else if (*s == '\t')
      fputs("\\t", fp);
    else if (*s == '\\')
      fputs("\\\\", fp);
    else if (*s == '\"')
      fputs("\\\"", fp);
    else if (*s == '\'')
      fputs("\\'", fp);
    else if ((*s & 255) >= ' ')
      putc(*s, fp);

    s ++;
  }

  putc('\"', fp);
}


/*
 * 'json_write_plist()' - Write a plist as a JSON file.
 */

static void
json_write_plist(FILE	 *fp,		/* I - File to write to */
		 plist_t *plist)	/* I - plist to write */
{
  plist_t	*current,		/* Current node */
		*next;			/* Next node */


  putc('[', fp);

  if (plist->type == PLIST_TYPE_DICT)
    putc('{', fp);

  for (current = plist->first_child; current; current = next)
  {
    if (current->prev_sibling && current->parent->type == PLIST_TYPE_ARRAY)
      fputs(",\n", fp);

    switch (current->type)
    {
      case PLIST_TYPE_PLIST :
	  break;
      case PLIST_TYPE_ARRAY :
	  putc('[', fp);
	  break;
      case PLIST_TYPE_DICT :
	  putc('{', fp);
	  break;
      case PLIST_TYPE_KEY :
	  if (current->prev_sibling)
	    putc(',', fp);

	  json_puts(fp, current->value);
	  putc(':', fp);
	  break;
      case PLIST_TYPE_DATA :
      case PLIST_TYPE_DATE :
      case PLIST_TYPE_STRING :
	  json_puts(fp, current->value);
	  break;
      case PLIST_TYPE_FALSE :
	  fputs("false", fp);
	  break;
      case PLIST_TYPE_TRUE :
	  fputs("true", fp);
	  break;
      case PLIST_TYPE_INTEGER :
	  fputs(current->value, fp);
	  break;
    }

    next = current->first_child;
    if (!next)
      next = current->next_sibling;
    if (!next)
    {
      next = current->parent;
      while (next)
      {
	if (next->type == PLIST_TYPE_ARRAY)
	  putc(']', fp);
	else if (next->type == PLIST_TYPE_DICT)
	  putc('}', fp);

	if (next->next_sibling)
	{
	  next = next->next_sibling;
	  break;
	}
	else
	  next = next->parent;
      }
    }
  }

  if (plist->type == PLIST_TYPE_DICT)
    putc(']', fp);

  putc('\n', fp);
}


/*
 * 'plist_add()' - Add a plist node.
 */

static plist_t *			/* O - New node or `NULL` on error */
plist_add(plist_t      *parent,		/* I - Parent node */
	  plist_type_t type,		/* I - Node type */
	  const char   *value)		/* I - Node value or `NULL` */
{
  plist_t	  *temp;		  /* New node */


  if ((temp = calloc(1, sizeof(plist_t))) != NULL)
  {
    if (parent)
    {
     /*
      * Add node to the parent...
      */

      temp->parent = parent;

      if (parent->last_child)
      {
	parent->last_child->next_sibling = temp;
	temp->prev_sibling		 = parent->last_child;
	parent->last_child		 = temp;
      }
      else
      {
	parent->first_child = parent->last_child = temp;
      }
    }

   /*
    * Copy the node values...
    */

    temp->type = type;

    if (value)
      temp->value = strdup(value);
  }

  return (temp);
}


/*
 * 'plist_array_count()' - Return the number of array elements.
 */

static int				/* O - Number of elements */
plist_array_count(plist_t *plist)	/* I - plist node */
{
  int		count;			/* Number of child nodes */
  plist_t	*current;		/* Current child node */


  if (!plist || plist->type != PLIST_TYPE_ARRAY)
    return (0);

  for (count = 0, current = plist->first_child; current; current = current->next_sibling)
    count ++;

  return (count);
}


/*
 * 'plist_delete()' - Free the memory used by the plist (XML) file.
 */

static void
plist_delete(plist_t *plist)		/* I - Root node of plist file */
{
  plist_t	*current,		/* Current node */
		*next;			/* Next node */


  if (!plist)
    return;

  for (current = plist->first_child; current; current = next)
  {
   /*
    * Get the next node...
    */

    if ((next = current->first_child) != NULL)
    {
     /*
      * Free parent nodes after child nodes have been freed...
      */

      current->first_child = NULL;
      continue;
    }

    if ((next = current->next_sibling) == NULL)
    {
     /*
      * Next node is the parent, which we'll free as needed...
      */

      if ((next = current->parent) == plist)
	next = NULL;
    }

   /*
    * Free current node...
    */

    free(current->value);
    free(current);
  }

  free(plist);
}


/*
 * 'plist_find()' - Find the named/numbered node.
 *
 * The search string is an XPath with names and numbers in the tree separated
 * by slashes, e.g., "foo/2/bar".
 */

static plist_t *			/* O - Matching node or `NULL` if none */
plist_find(plist_t    *parent,		/* I - Parent node */
	   const char *path)		/* I - Slash-separated path */
{
  plist_t	*current;		/* Current node */
  char		temp[8192],		/* Temporary string */
		*name,			/* Name in path */
		*next;			/* Next pointer into path */
  int		n;			/* Number in path */


 /*
  * Range check input...
  */

  if (!parent || !path)
    return (NULL);

 /*
  * Copy the path and loop through it to find the various nodes...
  */

  strncpy(temp, path, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';

  for (name = temp, current = parent; *name; name = next)
  {
   /*
    * Nul-terminate current path component as needed...
    */

    if ((next = strchr(name, '/')) != NULL)
      *next++ = '\0';
    else
      next = name + strlen(name);

    if (isdigit(*name & 255))
    {
     /*
      * Look for a 0-indexed child node...
      */

      n = atoi(name);

      if (current->type == PLIST_TYPE_ARRAY)
      {
       /*
	* Get the Nth child node...
	*/

	current = current->first_child;

	while (n > 0 && current)
	{
	  current = current->next_sibling;
	  n --;
	}

	if (!current)
	  return (NULL);
      }
      else
	return (NULL);
    }
    else
    {
     /*
      * Look for a <key> of the specified name...
      */

      if (current->type == PLIST_TYPE_PLIST && current->first_child && current->first_child->type == PLIST_TYPE_DICT)
	current = current->first_child;

      if (current->type != PLIST_TYPE_DICT)
	return (NULL);

      for (current = current->first_child; current; current = current->next_sibling)
      {
	if (current->type == PLIST_TYPE_KEY && !strcmp(current->value, name))
	  break;
      }

      if (!current || !current->next_sibling)
	return (NULL);

     /*
      * Then point to the value node that follows it...
      */

      current = current->next_sibling;
    }
  }

  return (current);
}


/*
 * 'plist_read()' - Read a plist (XML) file.
 */

static plist_t *			/* O - Root node of plist file or `NULL` on error */
plist_read(const char *filename)	/* I - File to read */
{
  FILE		*fp;			/* File pointer */
  plist_t	*plist = NULL,		/* Root plist node */
		*parent = NULL;		/* Current parent node */
  char		buffer[65536];		/* Element/value buffer */
  int		linenum = 1;		/* Current line number */
  int		needval = 0;		/* Just read a <key>, need a value */


  if ((fp = fopen(filename, "r")) == NULL)
  {
    printf("ippevesubmit: Unable to open \"%s\": %s\n", filename, strerror(errno));
    return (NULL);
  }

  while (xml_gets(fp, buffer, sizeof(buffer), &linenum))
  {
    if (!strncmp(buffer, "<?xml ", 6) || !strncmp(buffer, "<!DOCTYPE ", 10))
    {
     /*
      * Ignore XML document declarations...
      */

      continue;
    }
    else if (!strncmp(buffer, "<plist ", 7))
    {
     /*
      * A <plist> element starts the data content, but only if we haven't
      * already seen a root node!
      */

      if (plist)
      {
	printf("%s:%d: Unexpected (second) <plist> seen.\n", filename, linenum);
	break;
      }

      plist = parent = plist_add(NULL, PLIST_TYPE_PLIST, NULL);
    }
    else if (!plist)
    {
     /*
      * Cannot handle content before <plist ...>
      */

      break;
    }
    else if (!strcmp(buffer, "</plist>"))
    {
     /*
      * End of the data content...
      */

      if (parent != plist)
      {
	buffer[0] = 'x';		/* Flag this as an error */

	printf("%s:%d: Unexpected '</plist>'\n", filename, linenum);
      }

      break;
    }
    else if (!strcmp(buffer, "<array>"))
    {
      parent  = plist_add(parent, PLIST_TYPE_ARRAY, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "<array />"))
    {
     /*
      * Empty array...
      */

      plist_add(parent, PLIST_TYPE_ARRAY, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "</array>"))
    {
      if (parent->type != PLIST_TYPE_ARRAY)
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }

      parent = parent->parent;
    }
    else if (!strcmp(buffer, "<dict>"))
    {
      parent  = plist_add(parent, PLIST_TYPE_DICT, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "<dict />"))
    {
     /*
      * Empty dict...
      */

      plist_add(parent, PLIST_TYPE_DICT, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "</dict>"))
    {
      if (parent->type != PLIST_TYPE_DICT)
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }

      parent = parent->parent;
    }
    else if (!strcmp(buffer, "<key>"))
    {
      if (needval)
      {
	printf("%s:%d: Expected a value after a '<key>' element.\n", filename, linenum);
	break;
      }

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	printf("%s:%d: Missing <key> value.\n", filename, linenum);
	break;
      }

      plist_add(parent, PLIST_TYPE_KEY, buffer);
      needval = 1;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</key>"))
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<data>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	printf("%s:%d: Missing <data> value.\n", filename, linenum);
	break;
      }

      plist_add(parent, PLIST_TYPE_DATA, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</data>"))
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<date>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	printf("%s:%d: Missing <date> value.\n", filename, linenum);
	break;
      }

      plist_add(parent, PLIST_TYPE_DATE, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</date>"))
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<false />"))
    {
      plist_add(parent, PLIST_TYPE_FALSE, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "<integer>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	printf("%s:%d: Missing <integer> value.\n", filename, linenum);
	break;
      }

      plist_add(parent, PLIST_TYPE_INTEGER, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</integer>"))
      {
	printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<string>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	printf("%s:%d: Missing <string> value.\n", filename, linenum);
	break;
      }

      if (!strcmp(buffer, "</string>"))
      {
	plist_add(parent, PLIST_TYPE_STRING, "");
      }
      else
      {
	plist_add(parent, PLIST_TYPE_STRING, buffer);

	if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</string>"))
	{
	  printf("%s:%d: Unexpected '%s'\n", filename, linenum, buffer);
	  break;
	}
      }

      needval = 0;
    }
    else if (!strcmp(buffer, "<true />"))
    {
      plist_add(parent, PLIST_TYPE_TRUE, NULL);
      needval = 0;
    }
    else
    {
     /*
      * Something else that was unexpected...
      */

      printf("%s:%d: Unkwown '%s'\n", filename, linenum, buffer);
      break;
    }
  }

  if (plist && strcmp(buffer, "</plist>"))
  {
    printf("%s:%d: File appears to be truncated or corrupted.\n", filename, linenum);
    plist_delete(plist);
    plist = NULL;
  }

  return (plist);
}


/*
 * 'read_boolean()' - Ask a yes/no question.
 */

static int				/* O - 1 if yes, 0 if no */
read_boolean(const char *prompt)	/* I - Question to ask */
{
  char	buffer[256];			/* Response buffer */


  printf("%s (y/N)? ", prompt);
  fflush(stdout);

  if (fgets(buffer, sizeof(buffer), stdin) && toupper(buffer[0] & 255) == 'Y')
    return (1);
  else
    return (0);
}


/*
 * 'read_string()' - Read a string response from the console or a file.
 */

static char *				/* O - String or `NULL` if none */
read_string(const char *prompt,		/* I - Prompt (if interactive) */
	    FILE       *fp,		/* I - File to read from */
	    char       *buffer,		/* I - Response buffer */
	    size_t     bufsize)		/* I - Size of response buffer */
{
  char	*bufptr;			/* Pointer into buffer */


  if (prompt)
  {
   /*
    * Show prompt...
    */

    printf("%s? ", prompt);
    fflush(stdout);
  }

  if (fgets(buffer, bufsize, fp))
  {
   /*
    * Got a line from the user, strip trailing whitespace...
    */

    for (bufptr = buffer + strlen(buffer) - 1; bufptr >= buffer; bufptr --)
    {
      if (isspace(*bufptr & 255))
	*bufptr = '\0';
      else
        break;
    }

   /*
    * If there is anything left, return it...
    */

    if (buffer[0])
      return (buffer);
  }

  return (NULL);
}


/*
 * 'replay_results()' - Replay the results from a test.
 */

static void
replay_results(const char *filename,	/* I - Filename */
               plist_t    *results)	/* I - Results */
{
  plist_t	*tests,			/* Tests array */
		*test,			/* Current test dictionary */
		*name,			/* Test name ("Name" string) */
		*successful,		/* Test status ("Successful" boolean) */
		*skipped,		/* Test skipped? ("Skipped" boolean) */
		*errors;		/* Test errors, if any ("Errors" array) */
  const char	*status;		/* Status to display */
  int		total = 0,		/* Test counts */
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
      plist_t	*error;			/* Current error */

      for (error = errors->first_child; error; error = error->next_sibling)
        printf("        %s\n", error->value);
    }
  }

  printf("\nSummary: %d tests, %d passed, %d failed, %d skipped\n", total, pass, fail, skip);
  printf("Score: %d%%\n", 100 * (pass + skip) / total);
}


/*
 * 'usage()' - Show program usage.
 */

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


/*
 * 'validate_dnssd_results()' - Validate the results from the DNS-SD tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_dnssd_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results,		/* I - DNS-SD results */
    int	       print_server,		/* I - Certifying a print server? */
    char       *errors,			/* O - Error buffer */
    size_t     errsize)			/* I - Size of error buffer */
{
  int		result = 1;		/* Success/fail result */
  plist_t	*fileid,		/* FileId value */
		*successful,		/* Successful value */
		*tests,			/* Tests array */
		*test;			/* Current test */
  int		number,			/* Test number */
		tests_count = 0;	/* Number of tests */
  char		*errptr = errors;	/* Pointer into errors */


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
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert10.bonjour") && strcmp(fileid->value, "org.pwg.ippeveselfcert11.dnssd"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    result = 0;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = 0;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = 0;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10.bonjour") && tests_count != 10)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 10).\n", tests_count);
    result = 0;
  }
  else if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.dnssd") && tests_count != 10)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 10).\n", tests_count);
    result = 0;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					/* Test name */
		*tsuccessful = plist_find(test, "Successful"),
					/* Was the test successful? */
		*terrors = plist_find(test, "Errors"),
					/* What errors occurred? */
		*terror;		/* Current error message */

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = 0;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
       /*
	* Test failed, check for auto-exceptions and show error otherwise...
	*/

	int show_errors = 1;		/* Show error messages? */

	if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10.bonjour") && (number == 4 || number == 10) && terrors)
	{
	 /*
	  * Inspect errors for the current auto-exceptions for all printers:
	  *
	  * - Allow rp values other than ipp/print and ipp/print/...
	  */

	  show_errors = 0;

	  for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	  {
	    if (terror->type != PLIST_TYPE_STRING)
	    {
	      continue;
	    }
	    else if (strncmp(terror->value, "rp has bad value", 16))
	    {
	     /*
	      * Show any errors that don't fall under the exception here...
	      */

	      if (!show_errors)
	      {
		show_errors = 1;
		snprintf(errptr, errsize - (size_t)(errptr - errors), "FAILED %s\n", tname->value);
		errptr += strlen(errptr);
	      }

	      snprintf(errptr, errsize - (size_t)(errptr - errors), "%s\n", terror->value);
	      errptr += strlen(errptr);
	    }
	  }

	  if (show_errors)
	  {
	   /*
	    * Since we already showed the errors we care about, clear the show
	    * flag and say that we fail...
	    */

	    show_errors = 0;
	    result	= 0;
	  }
	}

	if (show_errors)
	{
	  result = 0;

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
  }

  return (result);
}


/*
 * 'validate_document_results()' - Validate the results from the document tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_document_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results,		/* I - Document results */
    int	       print_server,		/* I - Certifying a print server? */
    char       *errors,			/* O - Error buffer */
    size_t     errsize)			/* I - Size of error buffer */
{
  int		result = 1;		/* Success/fail result */
  plist_t	*fileid,		/* FileId value */
		*successful,		/* Successful value */
		*tests,			/* Tests array */
		*test;			/* Current test */
  int		number,			/* Test number */
		tests_count = 0;	/* Number of tests */
  char		*errptr = errors;	/* Pointer into errors */


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
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert10.document") && strcmp(fileid->value, "org.pwg.ippeveselfcert11.document"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    errptr += strlen(errptr);
    result = 0;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = 0;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = 0;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10.document") && tests_count != 34)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 34).\n", tests_count);
    errptr += strlen(errptr);
    result = 0;
  }
  else if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.document") && tests_count != 34)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 34).\n", tests_count);
    errptr += strlen(errptr);
    result = 0;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					/* Test name */
		*tsuccessful = plist_find(test, "Successful"),
					/* Was the test successful? */
		*terrors = plist_find(test, "Errors"),
					/* What errors occurred? */
		*terror;		/* Current error message */

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = 0;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
       /*
	* Test failed, show errors...
	*/

	result = 0;

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


/*
 * 'validate_ipp_results()' - Validate the results from the IPP tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_ipp_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results,		/* I - IPP results */
    int	       print_server,		/* I - Certifying a print server? */
    char       *errors,			/* O - Error buffer */
    size_t     errsize)			/* I - Size of error buffer */
{
  int		result = 1;		/* Success/fail result */
  plist_t	*fileid,		/* FileId value */
		*successful,		/* Successful value */
		*tests,			/* Tests array */
		*test;			/* Current test */
  int		number,			/* Test number */
		tests_count = 0;	/* Number of tests */
  char		*errptr = errors;	/* Pointer into error buffer */


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
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp"))
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Unsupported FileId '%s'.\n", fileid->value);
    errptr += strlen(errptr);
    result = 0;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Successful.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Successful is not a boolean value.\n");
    errptr += strlen(errptr);
    result = 0;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing Tests.\n");
    errptr += strlen(errptr);
    result = 0;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Tests is not an array value.\n");
    errptr += strlen(errptr);
    result = 0;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && tests_count != 27)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 27).\n", tests_count);
    errptr += strlen(errptr);
    result = 0;
  }
  else if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp") && tests_count != 51)
  {
    snprintf(errptr, errsize - (size_t)(errptr - errors), "Wrong number of tests (got %d, expected 51).\n",  tests_count);
    errptr += strlen(errptr);
    result = 0;
  }

  if (tests)
  {
    for (test = tests->first_child, number = 1; test; test = test->next_sibling, number ++)
    {
      plist_t	*tname = plist_find(test, "Name"),
					/* Test name */
		*tsuccessful = plist_find(test, "Successful"),
					/* Was the test successful? */
		*terrors = plist_find(test, "Errors"),
					/* What errors occurred? */
		*terror;		/* Current error message */

      if (!tname || tname->type != PLIST_TYPE_STRING || !tsuccessful || (tsuccessful->type != PLIST_TYPE_FALSE && tsuccessful->type != PLIST_TYPE_TRUE))
      {
	snprintf(errptr, errsize - (size_t)(errptr - errors), "Missing/bad values for test #%d.\n", number);
	errptr += strlen(errptr);
	result = 0;
	continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
       /*
	* Test failed, check for auto-exceptions and show error otherwise...
	*/

	int show_errors = 1;		/* Show error messages? */

	if (print_server && terrors && !strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && number == 9)
	{
	 /*
	  * Inspect errors for the current auto-exceptions for Print Servers:
	  *
	  * - media-col-ready, media-ready, identify-actions-xxx,
	  *   printer-device-id, and the printer-supply attribute are not
	  *   required.
	  * - The Identify-Printer operation is not required.
	  */

	  show_errors = 0;

	  for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	  {
	    if (terror->type != PLIST_TYPE_STRING)
	      continue;
	    else if (strncmp(terror->value, "EXPECTED: identify-actions-", 27) &&
		     strncmp(terror->value, "EXPECTED: media-col-ready", 25) &&
		     strncmp(terror->value, "EXPECTED: media-ready", 21) &&
		     strncmp(terror->value, "EXPECTED: operations-supported WITH-VALUE \"0x003c\"", 50) &&
		     strncmp(terror->value, "EXPECTED: printer-device-id", 27) &&
		     strncmp(terror->value, "EXPECTED: printer-supply", 24) &&
		     strncmp(terror->value, "GOT: printer-supply=", 20))
	    {
	     /*
	      * Show any errors that don't fall under the exception here...
	      */

	      if (!show_errors)
	      {
		show_errors = 1;
		snprintf(errptr, errsize - (size_t)(errptr - errors), "FAILED %s\n", tname->value);
		errptr += strlen(errptr);
	      }

	      snprintf(errptr, errsize - (size_t)(errptr - errors), "%s\n", terror->value);
	      errptr += strlen(errptr);
	    }
	  }

	  if (show_errors)
	  {
	   /*
	    * Since we already showed the errors we care about, clear the show
	    * flag and say that we fail...
	    */

	    show_errors = 0;
	    result	= 0;
	  }
	}
	else if (print_server && !strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && number == 27)
	{
	 /*
	  * Print Servers also do not need to support 'media-needed'.
	  */

	  show_errors = 0;
	}

	if (show_errors)
	{
	  result = 0;

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
  }

  return (result);
}


/*
 * 'xml_gets()' - Read an XML fragment from a file.
 *
 * An XML fragment is an element like "<element attr='value'>", "some text",
 * and "</element>".
 */

static char *				/* O  - XML fragment or `NULL` on EOF/error */
xml_gets(FILE	*fp,			/* I  - File to read from */
	 char	*buffer,		/* I  - Buffer */
	 size_t bufsize,		/* I  - Size of buffer */
	 int	*linenum)		/* IO - Current line number */
{
  char	ch,				/* Current character */
	*bufptr,			/* Pointer into buffer */
	*bufend;			/* Pointer to end of buffer */


 /*
  * Skip leading whitespace...
  */

  while ((ch = getc(fp)) != EOF)
  {
    if (ch == '\n')
      (*linenum)++;
    else if (!isspace(ch & 255))
      break;
  }

  if (ch == EOF)
  {
    *buffer = '\0';
    return (NULL);
  }

 /*
  * Read the buffer...
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  *bufptr++ = ch;

  if (ch == '<')
  {
   /*
    * Read element...
    */

    while ((ch = getc(fp)) != EOF)
    {
      if (bufptr < bufend)
	*bufptr++ = ch;

      if (ch == '\n')
      {
	(*linenum)++;
      }
      else if (ch == '>')
      {
	break;
      }
      else if (ch == '\"' || ch == '\'')
      {
       /*
	* Read quoted string...
	*/

	char quote = ch;		/* Quote character */

	while ((ch = getc(fp)) != EOF)
	{
	  if (bufptr < bufend)
	    *bufptr++ = ch;

	  if (ch == '\n')
	    (*linenum)++;
	  else if (ch == quote)
	    break;
	}

	if (ch != quote)
	{
	  *buffer = '\0';
	  return (NULL);
	}
      }
    }

    if (ch != '>')
    {
      *buffer = '\0';
      return (NULL);
    }

    *bufptr++ = '\0';
  }
  else
  {
   /*
    * Read text...
    */

    while ((ch = getc(fp)) != EOF)
    {
      if (ch == '\n')
      {
	(*linenum)++;
      }
      else if (ch == '<')
      {
	ungetc(ch, fp);
	break;
      }

      if (bufptr < bufend)
	*bufptr++ = ch;
    }

   /*
    * Trim trailing whitespace...
    */

    while (bufptr > buffer)
    {
      if (!isspace(bufptr[-1] & 255))
	break;

      bufptr --;
    }

    *bufptr = '\0';

    xml_unescape(buffer);
  }

  return (buffer);
}


/*
 * 'xml_unescape()' - Replace &foo; with corresponding characters.
 */

static void
xml_unescape(char *buffer)		/* I - Buffer */
{
  char	*inptr,				/* Current input pointer */
	*outptr;			/* Current output pointer */


 /*
  * See if there are any escaped characters to work with...
  */

  if ((inptr = strchr(buffer, '&')) == NULL)
    return;				/* Nope */

  for (outptr = inptr; *inptr;)
  {
    if (*inptr == '&' && strchr(inptr + 1, ';'))
    {
     /*
      * Figure out what kind of escaped character we have...
      */

      inptr ++;
      if (!strncmp(inptr, "amp;", 4))
      {
	inptr += 4;
	*outptr++ = '&';
      }
      else if (!strncmp(inptr, "lt;", 3))
      {
	inptr += 3;
	*outptr++ = '<';
      }
      else if (!strncmp(inptr, "gt;", 3))
      {
	inptr += 3;
	*outptr++ = '>';
      }
      else if (!strncmp(inptr, "quot;", 5))
      {
	inptr += 5;
	*outptr++ = '\"';
      }
      else if (!strncmp(inptr, "apos;", 5))
      {
	inptr += 5;
	*outptr++ = '\'';
      }
      else if (*inptr == '#')
      {
       /*
	* Numeric, copy character over as UTF-8...
	*/

	int ch;				/* Numeric character value */

	inptr ++;
	if (*inptr == 'x')
	  ch = (int)strtol(inptr, NULL, 16);
	else
	  ch = (int)strtol(inptr, NULL, 10);

	if (ch < 0x80)
	{
	 /*
	  * US ASCII
	  */

	  *outptr++ = ch;
	}
	else if (ch < 0x800)
	{
	 /*
	  * Two-byte UTF-8
	  */

	  *outptr++ = (char)(0xc0 | (ch >> 6));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}
	else if (ch < 0x10000)
	{
	 /*
	  * Three-byte UTF-8
	  */

	  *outptr++ = (char)(0xe0 | (ch >> 12));
	  *outptr++ = (char)(0x80 | ((ch >> 6) & 0x3f));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}
	else
	{
	 /*
	  * Four-byte UTF-8
	  */

	  *outptr++ = (char)(0xf0 | (ch >> 18));
	  *outptr++ = (char)(0x80 | ((ch >> 12) & 0x3f));
	  *outptr++ = (char)(0x80 | ((ch >> 6) & 0x3f));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}

	inptr = strchr(inptr, ';') + 1;
      }
      else
      {
       /*
	* Something else not supported by XML...
	*/

	*outptr++ = '&';
      }
    }
    else
    {
     /*
      * Copy literal...
      */

      *outptr++ = *inptr++;
    }
  }

  *outptr = '\0';
}
