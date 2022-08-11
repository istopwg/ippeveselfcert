//
// Selfcert header file for the IPP Everywhere Printer Self-Certification
// application.
//
// Copyright © 2019-2022 by the IEEE-ISTO Printer Working Group.
//
// Licensed under Apache License v2.0.	See the file "LICENSE" for more
// information.
//

#ifndef SELFCERT_H
#  define SELFCERT_H
#  include <config.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdbool.h>
#  include <string.h>
#  include <ctype.h>
#  include <errno.h>
#  ifndef _WIN32
#    include <unistd.h>
#  endif /* !_WIN32 */
#  include <sys/stat.h>
#  include <cups/cups.h>
#  ifdef __cplusplus
extern "C" {
#  endif // __cplusplus


// Annotations...
#  if _WIN32
#    define SELFCERT_FORMAT(a,b)
#    define SELFCERT_NONNULL(...)
#    define SELFCERT_NORETURN
#  elif defined(__has_extension) || defined(__GNUC__)
#    define SELFCERT_FORMAT(a,b)  __attribute__ ((__format__(__printf__, a,b)))
#    define SELFCERT_NONNULL(...) __attribute__ ((nonnull(__VA_ARGS__)))
#    define SELFCERT_NORETURN     __attribute__ ((noreturn))
#  else
#    define SELFCERT_FORMAT(a,b)
#    define SELFCERT_NONNULL(...)
#    define SELFCERT_NORETURN
#  endif // __has_extension || __GNUC__

// Types...
typedef void (*plist_error_cb_t)(void *cb_data, const char *message);

typedef enum plist_type_e		// plist Data Type
{
  PLIST_TYPE_PLIST,			// <plist> ... </plist>
  PLIST_TYPE_ARRAY,			// <array> ... </array>
  PLIST_TYPE_DICT,			// <dict> ... </dict>
  PLIST_TYPE_KEY,			// <key>value</key>
  PLIST_TYPE_DATA,			// <data>value</data>
  PLIST_TYPE_DATE,			// <date>value</date>
  PLIST_TYPE_FALSE,			// <false />
  PLIST_TYPE_INTEGER,			// <integer>value</integer>
  PLIST_TYPE_STRING,			// <string>value</string>
  PLIST_TYPE_TRUE			// <true />
} plist_type_t;

typedef struct plist_s			// plist Data Node
{
  plist_type_t	type;			// Node type
  struct plist_s *parent,		// Parent node, if any
		*first_child,		// First child node, if any
		*last_child,		// Last child node, if any
		*prev_sibling,		// Previous sibling node, if any
		*next_sibling;		// Next sibling node, if any
  char		*value;			// Value (as a string), if any
} plist_t;


// Functions...
extern plist_t	*plist_add(plist_t *parent, plist_type_t type, const char *value);
extern size_t	plist_array_count(plist_t *plist);
extern void	plist_delete(plist_t *plist);
extern plist_t	*plist_find(plist_t *parent, const char *path);
extern plist_t	*plist_new(void);
extern plist_t	*plist_read(FILE *fp, const char *filename, plist_error_cb_t cb, void *cb_data);
extern bool	plist_write(FILE *fp, const char *filename, plist_t *plist, plist_error_cb_t cb, void *cb_data);
extern bool	plist_write_json(FILE *fp, const char *filename, plist_t *plist, plist_error_cb_t cb, void *cb_data);

extern bool	validate_dnssd_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);
extern bool	validate_document_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);
extern bool	validate_ipp_results(const char *filename, plist_t *results, int print_server, char *errors, size_t errsize);


#  ifdef __cplusplus
}
#  endif // __cplusplus
#endif // !SELFCERT_H
