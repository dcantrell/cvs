/* getnline.c -- Implementation of getnline function, a modification of
   the GNU C library function getline to allow the caller to set a
   maximum number of characters to be retrieved.

   Copyright (C) 1993, 1996, 1997, 1998, 2000, 2003 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Adapted by Derek Price <derek@ximbiot.com>
 * from getline.c written by Jan Brittenson <bson@gnu.ai.mit.edu>.
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "getnline.h"

#include <stddef.h>
#include <stdio.h>

#if STDC_HEADERS
# include <stdlib.h>
#else
char *malloc (), *realloc ();
#endif

#include "unlocked-io.h"

/* Always add at least this many bytes when extending the buffer.  */
#define MIN_CHUNK 64

/* Read up to (and including) a delimiter DELIM1 from STREAM into *LINEPTR
   + OFFSET (and NUL-terminate it).  If DELIM2 is non-zero, then read up
   and including the first occurrence of DELIM1 or DELIM2.  *LINEPTR is
   a pointer returned from malloc (or NULL), pointing to *N characters of
   space.  It is realloc'd as necessary.  Return the number of characters
   read (not including the NUL terminator), or -1 on error or EOF.  */

static int
getdelim2( char **lineptr, size_t *n, size_t offset, int limit,
           int delim1, int delim2, FILE *stream )
{
  size_t nchars_avail;		/* Allocated but unused chars in *LINEPTR.  */
  char *read_pos;		/* Where we're reading into *LINEPTR. */
  int ret;

  if (!lineptr || !n || !stream)
    return -1;

  if (!*lineptr)
    {
      *n = MIN_CHUNK;
      *lineptr = malloc (*n);
      if (!*lineptr)
	return -1;
    }

  if (*n < offset)
    return -1;

  nchars_avail = *n - offset;
  read_pos = *lineptr + offset;

  for (;;)
    {
      register int c;

      if (limit == 0)
	break;

      c = getc (stream);

      if (limit != GETNDELIM_NO_LIMIT)
	limit--;

      /* We always want at least one char left in the buffer, since we
	 always (unless we get an error while reading the first char)
	 NUL-terminate the line buffer.  */

      if (nchars_avail < 2)
	{
	  if (*n > MIN_CHUNK)
	    *n *= 2;
	  else
	    *n += MIN_CHUNK;

	  nchars_avail = *n + *lineptr - read_pos;
	  *lineptr = realloc (*lineptr, *n);
	  if (!*lineptr)
	    return -1;
	  read_pos = *n - nchars_avail + *lineptr;
	}

      if (c == EOF || ferror (stream))
	{
	  /* Return partial line, if any.  */
	  if (read_pos == *lineptr)
	    return -1;
	  else
	    break;
	}

      *read_pos++ = c;
      nchars_avail--;

      if (c == delim1 || (delim2 && c == delim2))
	/* Return the line.  */
	break;
    }

  /* Done - NUL terminate and return the number of chars read.  */
  *read_pos = '\0';

  ret = read_pos - (*lineptr + offset);
  return ret;
}

int
getnline( char **lineptr, size_t *n, int limit, FILE *stream )
{
  return getdelim2( lineptr, n, 0, limit, '\n', 0, stream );
}

int
getndelim( char **lineptr, size_t *n, int limit, int delimiter, FILE *stream )
{
  return getdelim2( lineptr, n, 0, limit, delimiter, 0, stream );
}
