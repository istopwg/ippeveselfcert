/*
 * IPP Everywhere Printer Self-Certification submission tool
 *
 * Copyright © 2019 by the ISTO Printer Working Group.
 * Copyright © 2019 by Apple Inc.
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 *
 * Usage:
 *
 *   ippevesubmit [options] "Printer Name"
 *
 * Options:
 *
 *    -m models.txt        Specify list of models, one per line.
 */


/*
 * Include necessary headers...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef _WIN32
#  include <io.h>
#  include <direct.h>
#  define access	_access
#  define R_OK          04
#else
#  include <unistd.h>
#endif /* _WIN32 */
#include <cups/cups.h>


/*
 * Local types...
 */

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

static plist_t	*plist_add(plist_t *parent, plist_type_t type, const char *value);
static int	plist_array_count(plist_t *plist);
static void	plist_delete(plist_t *plist);
static plist_t	*plist_find(plist_t *parent, const char *path);
static plist_t	*plist_read(const char *filename);
static void	usage(void);
static int	validate_dnssd_results(const char *filename, plist_t *results);
static int	validate_document_results(const char *filename, plist_t *results);
static int	validate_ipp_results(const char *filename, plist_t *results);
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
		*models = NULL,		/* File containing a list of models */
		*printer = NULL;	/* Printer being tested */
  char		filename[1024];		/* plist filename */
  int		ok = 1;			/* Are test results OK? */
  plist_t	*dnssd_results,		/* DNS-SD test results */
		*ipp_results,		/* IPP test results */
		*document_results;	/* Document test results */


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
  * Load test results and validate...
  */

  snprintf(filename, sizeof(filename), "%s DNS-SD Results.plist", printer);
  dnssd_results = plist_read(filename);
  if (!validate_dnssd_results(filename, dnssd_results))
    ok = 0;

  snprintf(filename, sizeof(filename), "%s IPP Results.plist", printer);
  ipp_results = plist_read(filename);
  if (!validate_ipp_results(filename, ipp_results))
    ok = 0;

  snprintf(filename, sizeof(filename), "%s Document Results.plist", printer);
  document_results = plist_read(filename);
  if (!validate_document_results(filename, document_results))
    ok = 0;

 /*
  * Write JSON file for submission...
  */

  return (!ok);
}


/*
 * 'plist_add()' - Add a plist node.
 */

static plist_t *			/* O - New node or `NULL` on error */
plist_add(plist_t      *parent,		/* I - Parent node */
          plist_type_t type,		/* I - Node type */
          const char   *value)		/* I - Node value or `NULL` */
{
  plist_t         *temp;                  /* New node */


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
        temp->prev_sibling               = parent->last_child;
        parent->last_child               = temp;
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
        break;

      plist = parent = (plist_t *)calloc(1, sizeof(plist_t));
      plist->type = PLIST_TYPE_PLIST;
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
 * 'usage()' - Show program usage.
 */

static void
usage(void)
{
  puts("Usage: ippevesubmit [options] \"Printer Name\"");
  puts("");
  puts("Options:");
  puts("  --help           Show help.");
  puts("  -m models.txt    Specify a list of models, one per line.");
}


/*
 * 'validate_dnssd_results()' - Validate the results from the DNS-SD tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_dnssd_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results)		/* I - DNS-SD results */
{
  (void)filename;
  (void)results;

  return (1);
}


/*
 * 'validate_document_results()' - Validate the results from the document
 * tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_document_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results)		/* I - Document results */
{
  (void)filename;
  (void)results;

  return (1);
}


/*
 * 'validate__results()' - Validate the results from the  tests.
 */

static int				/* O - 1 on success, 0 on failure */
validate_ipp_results(
    const char *filename,		/* I - plist filename */
    plist_t    *results)		/* I - IPP results */
{
  int		result = 1;		/* Success/fail result */
  plist_t	*fileid,		/* FileId value */
		*successful,		/* Successful value */
		*tests,			/* Tests array */
		*test;			/* Current test */
  int		number,			/* Test number */
		tests_count = 0;	/* Number of tests */


  if ((fileid = plist_find(results, "Tests/0/FileId")) == NULL)
  {
    printf("%s: Missing FileId.\n", filename);
    return (0);
  }
  else if (fileid->type != PLIST_TYPE_STRING)
  {
    printf("%s: FileId is not a string value.\n", filename);
    return (0);
  }
  else if (strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp"))
  {
    printf("%s: Unsupported FileId '%s'.\n", filename, fileid->value);
    result = 0;
  }

  if ((successful = plist_find(results, "Successful")) == NULL)
  {
    printf("%s: Missing Successful.\n", filename);
    result = 0;
  }
  else if (successful->type != PLIST_TYPE_FALSE && successful->type != PLIST_TYPE_TRUE)
  {
    printf("%s: Successful is not a boolean value.\n", filename);
    result = 0;
  }

  if ((tests = plist_find(results, "Tests")) == NULL)
  {
    printf("%s: Missing Tests.\n", filename);
    result = 0;
  }
  else if (tests->type != PLIST_TYPE_ARRAY)
  {
    printf("%s: Tests is not an array value.\n", filename);
    result = 0;
    tests  = NULL;
  }

  tests_count = plist_array_count(tests);

  if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10") && tests_count != 27)
  {
    printf("%s: Wrong number of tests (got %d, expected 27).\n", filename, tests_count);
    result = 0;
  }
  else if (!strcmp(fileid->value, "org.pwg.ippeveselfcert11") && tests_count != 28)
  {
    printf("%s: Wrong number of tests (got %d, expected 28).\n", filename, tests_count);
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
        printf("%s: Missing/bad values for test #%d.\n", filename, number);
        result = 0;
        continue;
      }

      if (tsuccessful->type == PLIST_TYPE_FALSE)
      {
       /*
        * Test failed, check for auto-exceptions and show error otherwise...
        */

        int show_errors = 1;		/* Show error messages? */

        if ((!strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") || !strcmp(fileid->value, "org.pwg.ippeveselfcert11.ipp")) && number == 9 && terrors)
        {
         /*
          * Inspect errors...
          */

          show_errors = 0;

          for (terror = terrors->first_child; terror; terror = terror->next_sibling)
          {
            if (terror->type != PLIST_TYPE_STRING)
              continue;
            else if (strncmp(terror->value, "EXPECTED: media-col-ready", 25) &&
                     strncmp(terror->value, "EXPECTED: media-ready", 21) &&
                     strncmp(terror->value, "EXPECTED: identify-actions-", 27) &&
                     strncmp(terror->value, "EXPECTED: printer-device-id", 27) &&
                     strncmp(terror->value, "EXPECTED: printer-supply", 24) &&
                     strncmp(terror->value, "EXPECTED: operations-supported WITH-VALUE \"0x003c\"", 50) &&
                     strncmp(terror->value, "GOT: printer-supply=", 20))
            {
             /*
              * Show any errors that don't fall under the exception here...
              */

              if (!show_errors)
              {
                show_errors = 1;
		printf("%s: FAILED %s\n", filename, tname->value);
	      }

	      printf("%s:     %s\n", filename, terror->value);
	    }
          }

          if (show_errors)
          {
           /*
            * Since we already showed the errors we care about, clear the show
            * flag and say that we fail...
            */

            show_errors = 0;
            result      = 0;
          }
        }
	else if (!strcmp(fileid->value, "org.pwg.ippeveselfcert10.ipp") && number == 27)
	  show_errors = 0;

	if (show_errors)
	{
	  result = 0;

	  printf("%s: FAILED %s\n", filename, tname->value);

	  for (terror = terrors->first_child; terror; terror = terror->next_sibling)
	  {
	    if (terror->type != PLIST_TYPE_STRING)
	      continue;

	    printf("%s:     %s\n", filename, terror->value);
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
xml_gets(FILE   *fp,			/* I  - File to read from */
         char   *buffer,		/* I  - Buffer */
         size_t bufsize,		/* I  - Size of buffer */
         int    *linenum)		/* IO - Current line number */
{
  char  ch,                             /* Current character */
        *bufptr,                       /* Pointer into buffer */
        *bufend;                       /* Pointer to end of buffer */


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

	  *outptr++ = 0xc0 | (ch >> 6);
	  *outptr++ = 0x80 | (ch & 0x3f);
	}
	else if (ch < 0x10000)
	{
	 /*
	  * Three-byte UTF-8
	  */

	  *outptr++ = 0xe0 | (ch >> 12);
	  *outptr++ = 0x80 | ((ch >> 6) & 0x3f);
	  *outptr++ = 0x80 | (ch & 0x3f);
	}
	else
	{
	 /*
	  * Four-byte UTF-8
	  */

	  *outptr++ = 0xf0 | (ch >> 18);
	  *outptr++ = 0x80 | ((ch >> 12) & 0x3f);
	  *outptr++ = 0x80 | ((ch >> 6) & 0x3f);
	  *outptr++ = 0x80 | (ch & 0x3f);
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
