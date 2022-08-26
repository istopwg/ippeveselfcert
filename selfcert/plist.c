//
// Selfcert plist utilities for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//

#include "selfcert.h"
#include <stdarg.h>


// Local functions...
static void	json_puts(FILE *fp, const char *s);
static FILE	*open_file(const char *filename, const char *mode, plist_error_cb_t cb, void *cb_data);
static void	report_error(plist_error_cb_t cb, void *cb_data, const char *filename, int linenum, const char *message, ...) SELFCERT_FORMAT(5,6);
static char	*xml_gets(FILE *fp, char *buffer, size_t bufsize, int *linenum);
static void	xml_puts(FILE *fp, const char *s);
static void	xml_unescape(char *buffer);


//
// 'plist_add()' - Add a plist node.
//

plist_t *				// O - New node or `NULL` on error
plist_add(plist_t      *parent,		// I - Parent node
	  plist_type_t type,		// I - Node type
	  const char   *value)		// I - Node value or `NULL`
{
  plist_t	*temp;			// New node


  if ((temp = calloc(1, sizeof(plist_t))) != NULL)
  {
    if (parent)
    {
      // Add node to the parent...
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

    // Copy the node values...
    temp->type = type;

    if (value)
      temp->value = strdup(value);
  }

  return (temp);
}


//
// 'plist_array_count()' - Return the number of array elements.
//

size_t					// O - Number of elements
plist_array_count(plist_t *plist)	// I - plist node
{
  size_t	count;			// Number of child nodes
  plist_t	*current;		// Current child node


  if (!plist || plist->type != PLIST_TYPE_ARRAY)
    return (0);

  for (count = 0, current = plist->first_child; current; current = current->next_sibling)
    count ++;

  return (count);
}


//
// 'plist_delete()' - Free the memory used by the plist (XML) file.
//

void
plist_delete(plist_t *plist)		// I - Root node of plist file
{
  plist_t	*current,		// Current node
		*next;			// Next node


  if (!plist)
    return;

  for (current = plist->first_child; current; current = next)
  {
    // Get the next node...
    if ((next = current->first_child) != NULL)
    {
      // Free parent nodes after child nodes have been freed...
      current->first_child = NULL;
      continue;
    }

    if ((next = current->next_sibling) == NULL)
    {
      // Next node is the parent, which we'll free as needed...
      if ((next = current->parent) == plist)
	next = NULL;
    }

    // Free current node...
    free(current->value);
    free(current);
  }

  free(plist);
}


//
// 'plist_find()' - Find the named/numbered node.
//
// The search string is an XPath with names and numbers in the tree separated
// by slashes, e.g., "foo/2/bar".
//

plist_t *				// O - Matching node or `NULL` if none
plist_find(plist_t    *parent,		// I - Parent node
	   const char *path)		// I - Slash-separated path
{
  plist_t	*current;		// Current node
  char		temp[8192],		// Temporary string
		*name,			// Name in path
		*next;			// Next pointer into path
  size_t	n;			// Number in path


  // Range check input...
  if (!parent || !path)
    return (NULL);

  // Copy the path and loop through it to find the various nodes...
  strncpy(temp, path, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';

  for (name = temp, current = parent; *name; name = next)
  {
    // Nul-terminate current path component as needed...
    if ((next = strchr(name, '/')) != NULL)
      *next++ = '\0';
    else
      next = name + strlen(name);

    if (isdigit(*name & 255))
    {
      // Look for a 0-indexed child node...
      n = (size_t)strtol(name, NULL, 10);

      if (current->type == PLIST_TYPE_ARRAY)
      {
        // Get the Nth child node...
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
       // Look for a <key> of the specified name...
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

      // Then point to the value node that follows it...
      current = current->next_sibling;
    }
  }

  return (current);
}


//
// 'plist_new()' - Create a new plist (XML) file with its plist root node.
//

plist_t	*				// O - Root node or `NULL` on error
plist_new(void)
{
  return (plist_add(NULL, PLIST_TYPE_PLIST, NULL));
}


//
// 'plist_read()' - Read a plist (XML) file.
//

plist_t *				// O - Root node of plist file or `NULL` on error
plist_read(FILE             *fp,	// I - Input file or `NULL` to open filename
           const char       *filename,	// I - Filename
           plist_error_cb_t cb,		// I - Error callback function
           void             *cb_data)	// I - Error callback data
{
  bool		close_fp = !fp;		// Close the input file?
  plist_t	*plist = NULL,		// Root plist node
		*parent = NULL;		// Current parent node
  char		buffer[65536];		// Element/value buffer
  int		linenum = 1;		// Current line number
  int		needval = 0;		// Just read a <key>, need a value


  // Range check input...
  if (!fp && !filename)
    return (NULL);

  // Open file as needed...
  if (!fp)
  {
    if ((fp = open_file(filename, "r", cb, cb_data)) == NULL)
      return (NULL);
  }

  // Read the file...
  while (xml_gets(fp, buffer, sizeof(buffer), &linenum))
  {
    if (!strncmp(buffer, "<?xml ", 6) || !strncmp(buffer, "<!DOCTYPE ", 10))
    {
      // Ignore XML document declarations...
      continue;
    }
    else if (!strncmp(buffer, "<plist ", 7))
    {
      // A <plist> element starts the data content, but only if we haven't
      // already seen a root node!
      if (plist)
      {
        report_error(cb, cb_data, filename, linenum, "Unexpected (second) <plist> seen.");
	break;
      }

      plist = parent = plist_add(NULL, PLIST_TYPE_PLIST, NULL);
    }
    else if (!plist)
    {
      // Cannot handle content before <plist ...>
      break;
    }
    else if (!strcmp(buffer, "</plist>"))
    {
      // End of the data content...
      if (parent != plist)
      {
	buffer[0] = 'x';		// Flag this as an error

	report_error(cb, cb_data, filename, linenum, "Unexpected '</plist>'.");
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
      // Empty array...
      plist_add(parent, PLIST_TYPE_ARRAY, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "</array>"))
    {
      if (parent->type != PLIST_TYPE_ARRAY)
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
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
      // Empty dict...
      plist_add(parent, PLIST_TYPE_DICT, NULL);
      needval = 0;
    }
    else if (!strcmp(buffer, "</dict>"))
    {
      if (parent->type != PLIST_TYPE_DICT)
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
	break;
      }

      parent = parent->parent;
    }
    else if (!strcmp(buffer, "<key>"))
    {
      if (needval)
      {
	report_error(cb, cb_data, filename, linenum, "Expected a value after a '<key>' element.");
	break;
      }

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	report_error(cb, cb_data, filename, linenum, "Missing <key> value.");
	break;
      }

      plist_add(parent, PLIST_TYPE_KEY, buffer);
      needval = 1;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</key>"))
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<data>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	report_error(cb, cb_data, filename, linenum, "Missing <data> value.");
	break;
      }

      plist_add(parent, PLIST_TYPE_DATA, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</data>"))
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<date>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	report_error(cb, cb_data, filename, linenum, "Missing <date> value.");
	break;
      }

      plist_add(parent, PLIST_TYPE_DATE, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</date>"))
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
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
	report_error(cb, cb_data, filename, linenum, "Missing <integer> value.");
	break;
      }

      plist_add(parent, PLIST_TYPE_INTEGER, buffer);
      needval = 0;

      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum) || strcmp(buffer, "</integer>"))
      {
	report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
	break;
      }
    }
    else if (!strcmp(buffer, "<string>"))
    {
      if (!xml_gets(fp, buffer, sizeof(buffer), &linenum))
      {
	report_error(cb, cb_data, filename, linenum, "Missing <string> value.");
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
	  report_error(cb, cb_data, filename, linenum, "Unexpected '%s'.", buffer);
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
      // Something else that was unexpected...
      report_error(cb, cb_data, filename, linenum, "Unkwown '%s'.", buffer);
      break;
    }
  }

  if (plist && strcmp(buffer, "</plist>"))
  {
    report_error(cb, cb_data, filename, linenum, "File appears to be truncated or corrupted.");
    plist_delete(plist);
    plist = NULL;
  }

  // Close the file as needed...
  if (close_fp)
    fclose(fp);

  return (plist);
}


//
// 'plist_write()' - Write a plist to a plist file.
//

bool					// O - `true` on success, `false` on error
plist_write(
    FILE             *fp,		// I - Output file or `NULL` to open filename
    const char       *filename,		// I - Filename
    plist_t          *plist,		// I - plist to write
    plist_error_cb_t cb,		// I - Error callback function
    void             *cb_data)		// I - Error callback data
{
  bool		close_fp = !fp;		// Close the input file?
  plist_t	*current,		// Current node
		*next;			// Next node
  int		indent = 0;		// Indentation
  static const char *elements[] =	// Elements
  {
    "plist",			// <plist> ... </plist>
    "array",			// <array> ... </array>
    "dict",			// <dict> ... </dict>
    "key",			// <key>value</key>
    "data",			// <data>value</data>
    "date",			// <date>value</date>
    "false",			// <false />
    "integer",			// <integer>value</integer>
    "string",			// <string>value</string>
    "true"			// <true />
  };


  // Range check input...
  if ((!fp && !filename) || !plist)
    return (false);

  // Create file as needed...
  if (!fp)
  {
    if ((fp = open_file(filename, "w", cb, cb_data)) == NULL)
      return (NULL);
  }

  // Write file header...
  fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", fp);
  fputs("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n", fp);

  for (current = plist->first_child; current; current = next)
  {
    switch (current->type)
    {
      case PLIST_TYPE_PLIST :
          fputs("<plist version=\"1.0\">\n", fp);
	  break;
      case PLIST_TYPE_ARRAY :
      case PLIST_TYPE_DICT :
          fprintf(fp, "%*s<%s>\n", indent, "", elements[current->type]);
	  break;
      case PLIST_TYPE_KEY :
      case PLIST_TYPE_DATA :
      case PLIST_TYPE_DATE :
      case PLIST_TYPE_INTEGER :
      case PLIST_TYPE_STRING :
          fprintf(fp, "%*s<%s>", indent, "", elements[current->type]);
          xml_puts(fp, current->value);
          fprintf(fp, "</%s\n>", elements[current->type]);
	  break;
      case PLIST_TYPE_FALSE :
      case PLIST_TYPE_TRUE :
          fprintf(fp, "%*s<%s/>\n", indent, "", elements[current->type]);
	  break;
    }

    if ((next = current->first_child) != NULL)
    {
      // Descend into child...
      indent += 4;
    }
    else if ((next = current->next_sibling) == NULL)
    {
      // Ascend parent(s)...
      next = current->parent;
      indent -= 4;

      while (next)
      {
	if (next->type == PLIST_TYPE_ARRAY || next->type == PLIST_TYPE_DICT)
          fprintf(fp, "%*s</%s>\n", indent, "", elements[next->type]);

	if (next->next_sibling)
	{
	  // Next sibling...
	  next = next->next_sibling;
	  break;
	}
	else
	{
	  // Ascend parent...
	  next = next->parent;
	  indent -= 4;
	}
      }
    }
  }

  // Close the file as needed...
  if (close_fp)
    fclose(fp);

  return (true);
}


//
// 'plist_write_json()' - Write a plist to a JSON file.
//

bool					// O - `true` on success, `false` on error
plist_write_json(
    FILE             *fp,		// I - Output file or `NULL` to open filename
    const char       *filename,		// I - Filename
    plist_t          *plist,		// I - plist to write
    plist_error_cb_t cb,		// I - Error callback function
    void             *cb_data)		// I - Error callback data
{
  bool		close_fp = !fp;		// Close the input file?
  plist_t	*current,		// Current node
		*next;			// Next node


  // Range check input...
  if ((!fp && !filename) || !plist)
    return (false);

  // Create file as needed...
  if (!fp)
  {
    if ((fp = open_file(filename, "w", cb, cb_data)) == NULL)
      return (NULL);
  }

  // Write the plist as a JSON array...
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

  // Close the file as needed...
  if (close_fp)
    fclose(fp);

  return (true);
}


//
// 'json_puts()' - Write a string with JSON encoding to a file.
//

static void
json_puts(FILE	     *fp,		// I - File to write to
	  const char *s)		// I - String to write
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


//
// 'open_file()' - Open a file.
//

static FILE *				// O - File pointer or `NULL` on error
open_file(const char       *filename,	// I - Filename to open
          const char       *mode,	// I - "r" to read or "w" to write
          plist_error_cb_t cb,		// I - Error callback function
          void             *cb_data)	// I - Error callback data
{
  FILE	*fp;				// File pointer


  if ((fp = fopen(filename, mode)) == NULL)
    report_error(cb, cb_data, filename, 0, "%s", strerror(errno));

  return (fp);
}


//
// 'report_error()' - Report an error when loading a plist file.
//

static void
report_error(plist_error_cb_t cb,	// I - Callback function
             void             *cb_data,	// I - Callback data
             const char       *filename,// I - Filename
             int              linenum,	// I - Line number
             const char       *message,	// I - Printf-style message
             ...)			// I - Additional arguments
{
  va_list	ap;			// Pointer to arguments
  char		buffer[8192],		// Message buffer
		*bufptr;		// Pointer into buffer


  // Range check input...
  if (!cb)
    return;

  // Prefix the message with the filename and line number...
  if (linenum)
    snprintf(buffer, sizeof(buffer), "%s:%d  ", filename, linenum);
  else
    snprintf(buffer, sizeof(buffer), "%s  ", filename);

  bufptr = buffer + strlen(buffer);

  // Append the message...
  if (strchr(message, '%'))
  {
    // Format the message...
    va_start(ap, message);
    vsnprintf(bufptr, sizeof(buffer) - (size_t)(bufptr - buffer), message, ap);
    va_end(ap);
  }
  else
  {
    // Copy the literal message...
    cupsCopyString(bufptr, message, sizeof(buffer) - (size_t)(bufptr - buffer));
  }

  // Call the error callback...
  (cb)(cb_data, buffer);
}


//
// 'xml_gets()' - Read an XML fragment from a file.
//
// An XML fragment is an element like "<element attr='value'>", "some text",
// and "</element>".
//

static char *				// O  - XML fragment or `NULL` on EOF/error
xml_gets(FILE	*fp,			// I  - File to read from
	 char	*buffer,		// I  - Buffer
	 size_t bufsize,		// I  - Size of buffer
	 int	*linenum)		// IO - Current line number
{
  int	ch;				// Current character
  char	*bufptr,			// Pointer into buffer
	*bufend;			// Pointer to end of buffer


  // Skip leading whitespace...
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

  // Read the buffer...
  bufptr = buffer;
  bufend = buffer + bufsize - 1;

  *bufptr++ = ch;

  if (ch == '<')
  {
    // Read element...
    while ((ch = getc(fp)) != EOF)
    {
      if (bufptr < bufend)
	*bufptr++ = (char)ch;

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
        // Read quoted string...
	int quote = ch;			// Quote character

	while ((ch = getc(fp)) != EOF)
	{
	  if (bufptr < bufend)
	    *bufptr++ = (char)ch;

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
    // Read text...
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

    // Trim trailing whitespace...
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


//
// 'xml_puts()'- Write a string to an XML file, escaping as needed.
//

static void
xml_puts(FILE       *fp,		// I - Output file
         const char *s)			// I - String
{
  const char	*start,			// Start of fragment
		*end;			// End of fragment


  // Loop through the string...
  for (start = s, end = s; *end; end ++)
  {
    if (strchr("&<>", *end))
    {
      // Need to escape this character...
      if (end > start)
      {
        // Write literal fragment..
	fwrite(start, end - start, 1, fp);
      }

      // Next fragment starts after this character...
      start = end + 1;

      // Write the escaped version...
      switch (*end)
      {
        case '&' :
            fputs("&amp;", fp);
            break;
        case '<' :
            fputs("&lt;", fp);
            break;
        case '>' :
            fputs("&gt;", fp);
            break;
      }
    }
  }

  if (end > start)
  {
    // Write trailing literal fragment...
    fwrite(start, end - start, 1, fp);
  }
}


//
// 'xml_unescape()' - Replace &foo; with corresponding characters.
//

static void
xml_unescape(char *buffer)		// I - Buffer
{
  char	*inptr,				// Current input pointer
	*outptr;			// Current output pointer


  // See if there are any escaped characters to work with...
  if ((inptr = strchr(buffer, '&')) == NULL)
    return;				// Nope

  for (outptr = inptr; *inptr;)
  {
    if (*inptr == '&' && strchr(inptr + 1, ';'))
    {
      // Figure out what kind of escaped character we have...
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
	// Numeric, copy character over as UTF-8...
	int ch;				// Numeric character value

	inptr ++;
	if (*inptr == 'x')
	  ch = (int)strtol(inptr, NULL, 16);
	else
	  ch = (int)strtol(inptr, NULL, 10);

	if (ch < 0x80)
	{
	  // US ASCII
	  *outptr++ = ch;
	}
	else if (ch < 0x800)
	{
	  // Two-byte UTF-8
	  *outptr++ = (char)(0xc0 | (ch >> 6));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}
	else if (ch < 0x10000)
	{
	  // Three-byte UTF-8
	  *outptr++ = (char)(0xe0 | (ch >> 12));
	  *outptr++ = (char)(0x80 | ((ch >> 6) & 0x3f));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}
	else
	{
	  // Four-byte UTF-8
	  *outptr++ = (char)(0xf0 | (ch >> 18));
	  *outptr++ = (char)(0x80 | ((ch >> 12) & 0x3f));
	  *outptr++ = (char)(0x80 | ((ch >> 6) & 0x3f));
	  *outptr++ = (char)(0x80 | (ch & 0x3f));
	}

	inptr = strchr(inptr, ';') + 1;
      }
      else
      {
	// Something else not supported by XML...
	*outptr++ = '&';
      }
    }
    else
    {
      // Copy literal...
      *outptr++ = *inptr++;
    }
  }

  *outptr = '\0';
}
