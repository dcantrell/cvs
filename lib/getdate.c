/* A Bison parser, made from /home/oberon/work/ccvs-gnulib/lib/getdate.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	tAGO	257
# define	tDST	258
# define	tDAY	259
# define	tDAY_UNIT	260
# define	tDAYZONE	261
# define	tHOUR_UNIT	262
# define	tLOCAL_ZONE	263
# define	tMERIDIAN	264
# define	tMINUTE_UNIT	265
# define	tMONTH	266
# define	tMONTH_UNIT	267
# define	tSEC_UNIT	268
# define	tYEAR_UNIT	269
# define	tZONE	270
# define	tSNUMBER	271
# define	tUNUMBER	272
# define	tSDECIMAL_NUMBER	273
# define	tUDECIMAL_NUMBER	274

#line 1 "getdate.y"

/* Parse a string into an internal time stamp.
   Copyright (C) 1999, 2000, 2002, 2003, 2004 Free Software Foundation, Inc.

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

/* Originally written by Steven M. Bellovin <smb@research.att.com> while
   at the University of North Carolina at Chapel Hill.  Later tweaked by
   a couple of people on Usenet.  Completely overhauled by Rich $alz
   <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990.

   Modified by Paul Eggert <eggert@twinsun.com> in August 1999 to do
   the right thing about local DST, and in February 2004 to support
   nanosecond-resolution time stamps.  Unlike previous versions, this
   version is reentrant.  */

/* FIXME: Check for arithmetic overflow in all cases, not just
   some of them.

   FIXME: The current code uses 'int' to count seconds; it should use
   something like 'intmax_t' to support time stamps that don't fit in
   32 bits.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if TEST
# include <stdio.h>
#endif /* TEST */

#include "getdate.h"

#include <alloca.h>

/* Since the code of getdate.y is not included in the Emacs executable
   itself, there is no need to #define static in this file.  Even if
   the code were included in the Emacs executable, it probably
   wouldn't do any harm to #undef it here; this will only cause
   problems if we try to write to a static variable, which I don't
   think this code needs to do.  */
#ifdef emacs
# undef static
#endif

#include <ctype.h>
#include <limits.h>
#include <stdlib.h> /* for `free'; used by Bison 1.27 */

#if STDC_HEADERS || (! defined isascii && ! HAVE_ISASCII)
# define IN_CTYPE_DOMAIN(c) 1
#else
# define IN_CTYPE_DOMAIN(c) isascii (c)
#endif

#define ISSPACE(c) (IN_CTYPE_DOMAIN (c) && isspace (c))
#define ISALPHA(c) (IN_CTYPE_DOMAIN (c) && isalpha (c))
#define ISLOWER(c) (IN_CTYPE_DOMAIN (c) && islower (c))
#define ISDIGIT_LOCALE(c) (IN_CTYPE_DOMAIN (c) && isdigit (c))

/* ISDIGIT differs from ISDIGIT_LOCALE, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
   - It's typically faster.
   POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
   ISDIGIT_LOCALE unless it's important to use the locale's definition
   of `digit' even when the host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

#include <string.h>

#include "unlocked-io.h"

#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8) || __STRICT_ANSI__
# define __attribute__(x)
#endif

#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#endif

#define EPOCH_YEAR 1970
#define TM_YEAR_BASE 1900

#define HOUR(x) ((x) * 60)

/* An integer value, and the number of digits in its textual
   representation.  */
typedef struct
{
  long int value;
  size_t digits;
} textint;

/* An entry in the lexical lookup table.  */
typedef struct
{
  char const *name;
  int type;
  int value;
} table;

/* Meridian: am, pm, or 24-hour style.  */
enum { MERam, MERpm, MER24 };

enum { BILLION = 1000000000, LOG10_BILLION = 9 };

/* Information passed to and from the parser.  */
typedef struct
{
  /* The input string remaining to be parsed. */
  const char *input;

  /* N, if this is the Nth Tuesday.  */
  long int day_ordinal;

  /* Day of week; Sunday is 0.  */
  int day_number;

  /* tm_isdst flag for the local zone.  */
  int local_isdst;

  /* Time zone, in minutes east of UTC.  */
  long int time_zone;

  /* Style used for time.  */
  int meridian;

  /* Gregorian year, month, day, hour, minutes, seconds, and nanoseconds.  */
  textint year;
  long int month;
  long int day;
  long int hour;
  long int minutes;
  struct timespec seconds; /* includes nanoseconds */

  /* Relative year, month, day, hour, minutes, seconds, and nanoseconds.  */
  long int rel_year;
  long int rel_month;
  long int rel_day;
  long int rel_hour;
  long int rel_minutes;
  long int rel_seconds;
  long int rel_ns;

  /* Counts of nonterminals of various flavors parsed so far.  */
  bool timespec_seen;
  size_t dates_seen;
  size_t days_seen;
  size_t local_zones_seen;
  size_t rels_seen;
  size_t times_seen;
  size_t zones_seen;

  /* Table of local time zone abbrevations, terminated by a null entry.  */
  table local_time_zone_table[3];
} parser_control;

#define PC (* (parser_control *) parm)
#define YYLEX_PARAM parm
#define YYPARSE_PARAM parm

static int yyerror ();
static int yylex ();


#line 186 "getdate.y"
#ifndef YYSTYPE
typedef union
{
  long int intval;
  textint textintval;
  struct timespec timespec;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		80
#define	YYFLAG		-32768
#define	YYNTBASE	25

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 41)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    23,     2,     2,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
       2,     2,     2,     2,    21,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     4,     7,     8,    11,    13,    15,    17,
      19,    21,    23,    25,    28,    33,    38,    45,    52,    54,
      57,    59,    61,    64,    66,    69,    72,    76,    82,    86,
      90,    94,    97,   102,   105,   109,   112,   114,   117,   120,
     122,   125,   128,   130,   133,   136,   138,   141,   144,   146,
     149,   152,   154,   157,   160,   163,   166,   168,   170,   172,
     174,   176,   178,   180,   182,   183
};
static const short yyrhs[] =
{
      26,     0,    27,     0,    21,    36,     0,     0,    27,    28,
       0,    29,     0,    30,     0,    31,     0,    33,     0,    32,
       0,    34,     0,    39,     0,    18,    10,     0,    18,    22,
      18,    40,     0,    18,    22,    18,    17,     0,    18,    22,
      18,    22,    38,    40,     0,    18,    22,    18,    22,    38,
      17,     0,     9,     0,     9,     4,     0,    16,     0,     7,
       0,    16,     4,     0,     5,     0,     5,    23,     0,    18,
       5,     0,    18,    24,    18,     0,    18,    24,    18,    24,
      18,     0,    18,    17,    17,     0,    18,    12,    17,     0,
      12,    17,    17,     0,    12,    18,     0,    12,    18,    23,
      18,     0,    18,    12,     0,    18,    12,    18,     0,    35,
       3,     0,    35,     0,    18,    15,     0,    17,    15,     0,
      15,     0,    18,    13,     0,    17,    13,     0,    13,     0,
      18,     6,     0,    17,     6,     0,     6,     0,    18,     8,
       0,    17,     8,     0,     8,     0,    18,    11,     0,    17,
      11,     0,    11,     0,    18,    14,     0,    17,    14,     0,
      19,    14,     0,    20,    14,     0,    14,     0,    37,     0,
      38,     0,    19,     0,    17,     0,    20,     0,    18,     0,
      18,     0,     0,    10,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   206,   208,   211,   219,   221,   224,   227,   229,   231,
     233,   235,   237,   240,   249,   257,   267,   274,   285,   288,
     292,   295,   297,   301,   307,   312,   319,   325,   345,   352,
     360,   368,   373,   379,   384,   392,   403,   406,   409,   411,
     413,   415,   417,   419,   421,   423,   425,   427,   429,   431,
     433,   435,   437,   439,   441,   443,   445,   449,   449,   451,
     453,   457,   459,   463,   500,   503
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "tAGO", "tDST", "tDAY", "tDAY_UNIT", 
  "tDAYZONE", "tHOUR_UNIT", "tLOCAL_ZONE", "tMERIDIAN", "tMINUTE_UNIT", 
  "tMONTH", "tMONTH_UNIT", "tSEC_UNIT", "tYEAR_UNIT", "tZONE", "tSNUMBER", 
  "tUNUMBER", "tSDECIMAL_NUMBER", "tUDECIMAL_NUMBER", "'@'", "':'", "','", 
  "'/'", "spec", "timespec", "items", "item", "time", "local_zone", 
  "zone", "day", "date", "rel", "relunit", "seconds", "signed_seconds", 
  "unsigned_seconds", "number", "o_merid", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    25,    25,    26,    27,    27,    28,    28,    28,    28,
      28,    28,    28,    29,    29,    29,    29,    29,    30,    30,
      31,    31,    31,    32,    32,    32,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    34,    34,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    36,    36,    37,
      37,    38,    38,    39,    40,    40
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     1,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     4,     4,     6,     6,     1,     2,
       1,     1,     2,     1,     2,     2,     3,     5,     3,     3,
       3,     2,     4,     2,     3,     2,     1,     2,     2,     1,
       2,     2,     1,     2,     2,     1,     2,     2,     1,     2,
       2,     1,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       4,     0,     1,     2,    60,    62,    59,    61,     3,    57,
      58,    23,    45,    21,    48,    18,    51,     0,    42,    56,
      39,    20,     0,    63,     0,     0,     5,     6,     7,     8,
      10,     9,    11,    36,    12,    24,    19,     0,    31,    22,
      44,    47,    50,    41,    53,    38,    25,    43,    46,    13,
      49,    33,    40,    52,    37,     0,     0,     0,    54,    55,
      35,    30,     0,    29,    34,    28,    64,    26,    32,    65,
      15,     0,    14,     0,    64,    27,    17,    16,     0,     0,
       0
};

static const short yydefgoto[] =
{
      78,     2,     3,    26,    27,    28,    29,    30,    31,    32,
      33,     8,     9,    10,    34,    72
};

static const short yypact[] =
{
     -19,    29,-32768,    15,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    -8,-32768,-32768,-32768,    36,-32768,    -4,-32768,-32768,
  -32768,    38,    30,    -5,    39,    40,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,    49,-32768,-32768,-32768,    41,    32,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    33,-32768,-32768,-32768,    42,    43,    44,-32768,-32768,
  -32768,-32768,    45,-32768,-32768,-32768,    -6,    46,-32768,-32768,
  -32768,    19,-32768,    47,     8,-32768,-32768,-32768,    56,    57,
  -32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   -11,-32768,   -10
};


#define	YYLAST		70


static const short yytable[] =
{
      46,    47,     1,    48,    69,    49,    50,    51,    52,    53,
      54,    70,    55,    37,    38,    35,    71,    56,    69,    57,
      11,    12,    13,    14,    15,    76,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    40,     5,    41,     7,
      36,    42,    39,    43,    44,    45,     4,     5,     6,     7,
      63,    64,    60,    58,    59,    62,    79,    80,    61,    65,
      74,    66,    67,    68,    77,    75,     0,     0,     0,     0,
      73
};

static const short yycheck[] =
{
       5,     6,    21,     8,    10,    10,    11,    12,    13,    14,
      15,    17,    17,    17,    18,    23,    22,    22,    10,    24,
       5,     6,     7,     8,     9,    17,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,     6,    18,     8,    20,
       4,    11,     4,    13,    14,    15,    17,    18,    19,    20,
      17,    18,     3,    14,    14,    23,     0,     0,    17,    17,
      71,    18,    18,    18,    74,    18,    -1,    -1,    -1,    -1,
      24
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 3:
#line 213 "getdate.y"
{
	PC.seconds = yyvsp[0].timespec;
	PC.timespec_seen = true;
      }
    break;
case 6:
#line 226 "getdate.y"
{ PC.times_seen++; }
    break;
case 7:
#line 228 "getdate.y"
{ PC.local_zones_seen++; }
    break;
case 8:
#line 230 "getdate.y"
{ PC.zones_seen++; }
    break;
case 9:
#line 232 "getdate.y"
{ PC.dates_seen++; }
    break;
case 10:
#line 234 "getdate.y"
{ PC.days_seen++; }
    break;
case 11:
#line 236 "getdate.y"
{ PC.rels_seen++; }
    break;
case 13:
#line 242 "getdate.y"
{
	PC.hour = yyvsp[-1].textintval.value;
	PC.minutes = 0;
	PC.seconds.tv_sec = 0;
	PC.seconds.tv_nsec = 0;
	PC.meridian = yyvsp[0].intval;
      }
    break;
case 14:
#line 250 "getdate.y"
{
	PC.hour = yyvsp[-3].textintval.value;
	PC.minutes = yyvsp[-1].textintval.value;
	PC.seconds.tv_sec = 0;
	PC.seconds.tv_nsec = 0;
	PC.meridian = yyvsp[0].intval;
      }
    break;
case 15:
#line 258 "getdate.y"
{
	PC.hour = yyvsp[-3].textintval.value;
	PC.minutes = yyvsp[-1].textintval.value;
	PC.seconds.tv_sec = 0;
	PC.seconds.tv_nsec = 0;
	PC.meridian = MER24;
	PC.zones_seen++;
	PC.time_zone = yyvsp[0].textintval.value % 100 + (yyvsp[0].textintval.value / 100) * 60;
      }
    break;
case 16:
#line 268 "getdate.y"
{
	PC.hour = yyvsp[-5].textintval.value;
	PC.minutes = yyvsp[-3].textintval.value;
	PC.seconds = yyvsp[-1].timespec;
	PC.meridian = yyvsp[0].intval;
      }
    break;
case 17:
#line 275 "getdate.y"
{
	PC.hour = yyvsp[-5].textintval.value;
	PC.minutes = yyvsp[-3].textintval.value;
	PC.seconds = yyvsp[-1].timespec;
	PC.meridian = MER24;
	PC.zones_seen++;
	PC.time_zone = yyvsp[0].textintval.value % 100 + (yyvsp[0].textintval.value / 100) * 60;
      }
    break;
case 18:
#line 287 "getdate.y"
{ PC.local_isdst = yyvsp[0].intval; }
    break;
case 19:
#line 289 "getdate.y"
{ PC.local_isdst = yyvsp[-1].intval < 0 ? 1 : yyvsp[-1].intval + 1; }
    break;
case 20:
#line 294 "getdate.y"
{ PC.time_zone = yyvsp[0].intval; }
    break;
case 21:
#line 296 "getdate.y"
{ PC.time_zone = yyvsp[0].intval + 60; }
    break;
case 22:
#line 298 "getdate.y"
{ PC.time_zone = yyvsp[-1].intval + 60; }
    break;
case 23:
#line 303 "getdate.y"
{
	PC.day_ordinal = 1;
	PC.day_number = yyvsp[0].intval;
      }
    break;
case 24:
#line 308 "getdate.y"
{
	PC.day_ordinal = 1;
	PC.day_number = yyvsp[-1].intval;
      }
    break;
case 25:
#line 313 "getdate.y"
{
	PC.day_ordinal = yyvsp[-1].textintval.value;
	PC.day_number = yyvsp[0].intval;
      }
    break;
case 26:
#line 321 "getdate.y"
{
	PC.month = yyvsp[-2].textintval.value;
	PC.day = yyvsp[0].textintval.value;
      }
    break;
case 27:
#line 326 "getdate.y"
{
	/* Interpret as YYYY/MM/DD if the first value has 4 or more digits,
	   otherwise as MM/DD/YY.
	   The goal in recognizing YYYY/MM/DD is solely to support legacy
	   machine-generated dates like those in an RCS log listing.  If
	   you want portability, use the ISO 8601 format.  */
	if (4 <= yyvsp[-4].textintval.digits)
	  {
	    PC.year = yyvsp[-4].textintval;
	    PC.month = yyvsp[-2].textintval.value;
	    PC.day = yyvsp[0].textintval.value;
	  }
	else
	  {
	    PC.month = yyvsp[-4].textintval.value;
	    PC.day = yyvsp[-2].textintval.value;
	    PC.year = yyvsp[0].textintval;
	  }
      }
    break;
case 28:
#line 346 "getdate.y"
{
	/* ISO 8601 format.  YYYY-MM-DD.  */
	PC.year = yyvsp[-2].textintval;
	PC.month = -yyvsp[-1].textintval.value;
	PC.day = -yyvsp[0].textintval.value;
      }
    break;
case 29:
#line 353 "getdate.y"
{
	/* e.g. 17-JUN-1992.  */
	PC.day = yyvsp[-2].textintval.value;
	PC.month = yyvsp[-1].intval;
	PC.year.value = -yyvsp[0].textintval.value;
	PC.year.digits = yyvsp[0].textintval.digits;
      }
    break;
case 30:
#line 361 "getdate.y"
{
	/* e.g. JUN-17-1992.  */
	PC.month = yyvsp[-2].intval;
	PC.day = -yyvsp[-1].textintval.value;
	PC.year.value = -yyvsp[0].textintval.value;
	PC.year.digits = yyvsp[0].textintval.digits;
      }
    break;
case 31:
#line 369 "getdate.y"
{
	PC.month = yyvsp[-1].intval;
	PC.day = yyvsp[0].textintval.value;
      }
    break;
case 32:
#line 374 "getdate.y"
{
	PC.month = yyvsp[-3].intval;
	PC.day = yyvsp[-2].textintval.value;
	PC.year = yyvsp[0].textintval;
      }
    break;
case 33:
#line 380 "getdate.y"
{
	PC.day = yyvsp[-1].textintval.value;
	PC.month = yyvsp[0].intval;
      }
    break;
case 34:
#line 385 "getdate.y"
{
	PC.day = yyvsp[-2].textintval.value;
	PC.month = yyvsp[-1].intval;
	PC.year = yyvsp[0].textintval;
      }
    break;
case 35:
#line 394 "getdate.y"
{
	PC.rel_ns = -PC.rel_ns;
	PC.rel_seconds = -PC.rel_seconds;
	PC.rel_minutes = -PC.rel_minutes;
	PC.rel_hour = -PC.rel_hour;
	PC.rel_day = -PC.rel_day;
	PC.rel_month = -PC.rel_month;
	PC.rel_year = -PC.rel_year;
      }
    break;
case 37:
#line 408 "getdate.y"
{ PC.rel_year += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 38:
#line 410 "getdate.y"
{ PC.rel_year += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 39:
#line 412 "getdate.y"
{ PC.rel_year += yyvsp[0].intval; }
    break;
case 40:
#line 414 "getdate.y"
{ PC.rel_month += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 41:
#line 416 "getdate.y"
{ PC.rel_month += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 42:
#line 418 "getdate.y"
{ PC.rel_month += yyvsp[0].intval; }
    break;
case 43:
#line 420 "getdate.y"
{ PC.rel_day += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 44:
#line 422 "getdate.y"
{ PC.rel_day += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 45:
#line 424 "getdate.y"
{ PC.rel_day += yyvsp[0].intval; }
    break;
case 46:
#line 426 "getdate.y"
{ PC.rel_hour += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 47:
#line 428 "getdate.y"
{ PC.rel_hour += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 48:
#line 430 "getdate.y"
{ PC.rel_hour += yyvsp[0].intval; }
    break;
case 49:
#line 432 "getdate.y"
{ PC.rel_minutes += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 50:
#line 434 "getdate.y"
{ PC.rel_minutes += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 51:
#line 436 "getdate.y"
{ PC.rel_minutes += yyvsp[0].intval; }
    break;
case 52:
#line 438 "getdate.y"
{ PC.rel_seconds += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 53:
#line 440 "getdate.y"
{ PC.rel_seconds += yyvsp[-1].textintval.value * yyvsp[0].intval; }
    break;
case 54:
#line 442 "getdate.y"
{ PC.rel_seconds += yyvsp[-1].timespec.tv_sec * yyvsp[0].intval; PC.rel_ns += yyvsp[-1].timespec.tv_nsec * yyvsp[0].intval; }
    break;
case 55:
#line 444 "getdate.y"
{ PC.rel_seconds += yyvsp[-1].timespec.tv_sec * yyvsp[0].intval; PC.rel_ns += yyvsp[-1].timespec.tv_nsec * yyvsp[0].intval; }
    break;
case 56:
#line 446 "getdate.y"
{ PC.rel_seconds += yyvsp[0].intval; }
    break;
case 60:
#line 454 "getdate.y"
{ yyval.timespec.tv_sec = yyvsp[0].textintval.value; yyval.timespec.tv_nsec = 0; }
    break;
case 62:
#line 460 "getdate.y"
{ yyval.timespec.tv_sec = yyvsp[0].textintval.value; yyval.timespec.tv_nsec = 0; }
    break;
case 63:
#line 465 "getdate.y"
{
	if (PC.dates_seen
	    && ! PC.rels_seen && (PC.times_seen || 2 < yyvsp[0].textintval.digits))
	  PC.year = yyvsp[0].textintval;
	else
	  {
	    if (4 < yyvsp[0].textintval.digits)
	      {
		PC.dates_seen++;
		PC.day = yyvsp[0].textintval.value % 100;
		PC.month = (yyvsp[0].textintval.value / 100) % 100;
		PC.year.value = yyvsp[0].textintval.value / 10000;
		PC.year.digits = yyvsp[0].textintval.digits - 4;
	      }
	    else
	      {
		PC.times_seen++;
		if (yyvsp[0].textintval.digits <= 2)
		  {
		    PC.hour = yyvsp[0].textintval.value;
		    PC.minutes = 0;
		  }
		else
		  {
		    PC.hour = yyvsp[0].textintval.value / 100;
		    PC.minutes = yyvsp[0].textintval.value % 100;
		  }
		PC.seconds.tv_sec = 0;
		PC.seconds.tv_nsec = 0;
		PC.meridian = MER24;
	      }
	  }
      }
    break;
case 64:
#line 502 "getdate.y"
{ yyval.intval = MER24; }
    break;
case 65:
#line 504 "getdate.y"
{ yyval.intval = yyvsp[0].intval; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 507 "getdate.y"


static table const meridian_table[] =
{
  { "AM",   tMERIDIAN, MERam },
  { "A.M.", tMERIDIAN, MERam },
  { "PM",   tMERIDIAN, MERpm },
  { "P.M.", tMERIDIAN, MERpm },
  { 0, 0, 0 }
};

static table const dst_table[] =
{
  { "DST", tDST, 0 }
};

static table const month_and_day_table[] =
{
  { "JANUARY",	tMONTH,	 1 },
  { "FEBRUARY",	tMONTH,	 2 },
  { "MARCH",	tMONTH,	 3 },
  { "APRIL",	tMONTH,	 4 },
  { "MAY",	tMONTH,	 5 },
  { "JUNE",	tMONTH,	 6 },
  { "JULY",	tMONTH,	 7 },
  { "AUGUST",	tMONTH,	 8 },
  { "SEPTEMBER",tMONTH,	 9 },
  { "SEPT",	tMONTH,	 9 },
  { "OCTOBER",	tMONTH,	10 },
  { "NOVEMBER",	tMONTH,	11 },
  { "DECEMBER",	tMONTH,	12 },
  { "SUNDAY",	tDAY,	 0 },
  { "MONDAY",	tDAY,	 1 },
  { "TUESDAY",	tDAY,	 2 },
  { "TUES",	tDAY,	 2 },
  { "WEDNESDAY",tDAY,	 3 },
  { "WEDNES",	tDAY,	 3 },
  { "THURSDAY",	tDAY,	 4 },
  { "THUR",	tDAY,	 4 },
  { "THURS",	tDAY,	 4 },
  { "FRIDAY",	tDAY,	 5 },
  { "SATURDAY",	tDAY,	 6 },
  { 0, 0, 0 }
};

static table const time_units_table[] =
{
  { "YEAR",	tYEAR_UNIT,	 1 },
  { "MONTH",	tMONTH_UNIT,	 1 },
  { "FORTNIGHT",tDAY_UNIT,	14 },
  { "WEEK",	tDAY_UNIT,	 7 },
  { "DAY",	tDAY_UNIT,	 1 },
  { "HOUR",	tHOUR_UNIT,	 1 },
  { "MINUTE",	tMINUTE_UNIT,	 1 },
  { "MIN",	tMINUTE_UNIT,	 1 },
  { "SECOND",	tSEC_UNIT,	 1 },
  { "SEC",	tSEC_UNIT,	 1 },
  { 0, 0, 0 }
};

/* Assorted relative-time words. */
static table const relative_time_table[] =
{
  { "TOMORROW",	tDAY_UNIT,	 1 },
  { "YESTERDAY",tDAY_UNIT,	-1 },
  { "TODAY",	tDAY_UNIT,	 0 },
  { "NOW",	tDAY_UNIT,	 0 },
  { "LAST",	tUNUMBER,	-1 },
  { "THIS",	tUNUMBER,	 0 },
  { "NEXT",	tUNUMBER,	 1 },
  { "FIRST",	tUNUMBER,	 1 },
/*{ "SECOND",	tUNUMBER,	 2 }, */
  { "THIRD",	tUNUMBER,	 3 },
  { "FOURTH",	tUNUMBER,	 4 },
  { "FIFTH",	tUNUMBER,	 5 },
  { "SIXTH",	tUNUMBER,	 6 },
  { "SEVENTH",	tUNUMBER,	 7 },
  { "EIGHTH",	tUNUMBER,	 8 },
  { "NINTH",	tUNUMBER,	 9 },
  { "TENTH",	tUNUMBER,	10 },
  { "ELEVENTH",	tUNUMBER,	11 },
  { "TWELFTH",	tUNUMBER,	12 },
  { "AGO",	tAGO,		 1 },
  { 0, 0, 0 }
};

/* The time zone table.  This table is necessarily incomplete, as time
   zone abbreviations are ambiguous; e.g. Australians interpret "EST"
   as Eastern time in Australia, not as US Eastern Standard Time.
   You cannot rely on getdate to handle arbitrary time zone
   abbreviations; use numeric abbreviations like `-0500' instead.  */
static table const time_zone_table[] =
{
  { "GMT",	tZONE,     HOUR ( 0) },	/* Greenwich Mean */
  { "UT",	tZONE,     HOUR ( 0) },	/* Universal (Coordinated) */
  { "UTC",	tZONE,     HOUR ( 0) },
  { "WET",	tZONE,     HOUR ( 0) },	/* Western European */
  { "WEST",	tDAYZONE,  HOUR ( 0) },	/* Western European Summer */
  { "BST",	tDAYZONE,  HOUR ( 0) },	/* British Summer */
  { "ART",	tZONE,	  -HOUR ( 3) },	/* Argentina */
  { "BRT",	tZONE,	  -HOUR ( 3) },	/* Brazil */
  { "BRST",	tDAYZONE, -HOUR ( 3) },	/* Brazil Summer */
  { "NST",	tZONE,	 -(HOUR ( 3) + 30) },	/* Newfoundland Standard */
  { "NDT",	tDAYZONE,-(HOUR ( 3) + 30) },	/* Newfoundland Daylight */
  { "AST",	tZONE,    -HOUR ( 4) },	/* Atlantic Standard */
  { "ADT",	tDAYZONE, -HOUR ( 4) },	/* Atlantic Daylight */
  { "CLT",	tZONE,    -HOUR ( 4) },	/* Chile */
  { "CLST",	tDAYZONE, -HOUR ( 4) },	/* Chile Summer */
  { "EST",	tZONE,    -HOUR ( 5) },	/* Eastern Standard */
  { "EDT",	tDAYZONE, -HOUR ( 5) },	/* Eastern Daylight */
  { "CST",	tZONE,    -HOUR ( 6) },	/* Central Standard */
  { "CDT",	tDAYZONE, -HOUR ( 6) },	/* Central Daylight */
  { "MST",	tZONE,    -HOUR ( 7) },	/* Mountain Standard */
  { "MDT",	tDAYZONE, -HOUR ( 7) },	/* Mountain Daylight */
  { "PST",	tZONE,    -HOUR ( 8) },	/* Pacific Standard */
  { "PDT",	tDAYZONE, -HOUR ( 8) },	/* Pacific Daylight */
  { "AKST",	tZONE,    -HOUR ( 9) },	/* Alaska Standard */
  { "AKDT",	tDAYZONE, -HOUR ( 9) },	/* Alaska Daylight */
  { "HST",	tZONE,    -HOUR (10) },	/* Hawaii Standard */
  { "HAST",	tZONE,	  -HOUR (10) },	/* Hawaii-Aleutian Standard */
  { "HADT",	tDAYZONE, -HOUR (10) },	/* Hawaii-Aleutian Daylight */
  { "SST",	tZONE,    -HOUR (12) },	/* Samoa Standard */
  { "WAT",	tZONE,     HOUR ( 1) },	/* West Africa */
  { "CET",	tZONE,     HOUR ( 1) },	/* Central European */
  { "CEST",	tDAYZONE,  HOUR ( 1) },	/* Central European Summer */
  { "MET",	tZONE,     HOUR ( 1) },	/* Middle European */
  { "MEZ",	tZONE,     HOUR ( 1) },	/* Middle European */
  { "MEST",	tDAYZONE,  HOUR ( 1) },	/* Middle European Summer */
  { "MESZ",	tDAYZONE,  HOUR ( 1) },	/* Middle European Summer */
  { "EET",	tZONE,     HOUR ( 2) },	/* Eastern European */
  { "EEST",	tDAYZONE,  HOUR ( 2) },	/* Eastern European Summer */
  { "CAT",	tZONE,	   HOUR ( 2) },	/* Central Africa */
  { "SAST",	tZONE,	   HOUR ( 2) },	/* South Africa Standard */
  { "EAT",	tZONE,	   HOUR ( 3) },	/* East Africa */
  { "MSK",	tZONE,	   HOUR ( 3) },	/* Moscow */
  { "MSD",	tDAYZONE,  HOUR ( 3) },	/* Moscow Daylight */
  { "IST",	tZONE,	  (HOUR ( 5) + 30) },	/* India Standard */
  { "SGT",	tZONE,     HOUR ( 8) },	/* Singapore */
  { "KST",	tZONE,     HOUR ( 9) },	/* Korea Standard */
  { "JST",	tZONE,     HOUR ( 9) },	/* Japan Standard */
  { "GST",	tZONE,     HOUR (10) },	/* Guam Standard */
  { "NZST",	tZONE,     HOUR (12) },	/* New Zealand Standard */
  { "NZDT",	tDAYZONE,  HOUR (12) },	/* New Zealand Daylight */
  { 0, 0, 0  }
};

/* Military time zone table. */
static table const military_table[] =
{
  { "A", tZONE,	-HOUR ( 1) },
  { "B", tZONE,	-HOUR ( 2) },
  { "C", tZONE,	-HOUR ( 3) },
  { "D", tZONE,	-HOUR ( 4) },
  { "E", tZONE,	-HOUR ( 5) },
  { "F", tZONE,	-HOUR ( 6) },
  { "G", tZONE,	-HOUR ( 7) },
  { "H", tZONE,	-HOUR ( 8) },
  { "I", tZONE,	-HOUR ( 9) },
  { "K", tZONE,	-HOUR (10) },
  { "L", tZONE,	-HOUR (11) },
  { "M", tZONE,	-HOUR (12) },
  { "N", tZONE,	 HOUR ( 1) },
  { "O", tZONE,	 HOUR ( 2) },
  { "P", tZONE,	 HOUR ( 3) },
  { "Q", tZONE,	 HOUR ( 4) },
  { "R", tZONE,	 HOUR ( 5) },
  { "S", tZONE,	 HOUR ( 6) },
  { "T", tZONE,	 HOUR ( 7) },
  { "U", tZONE,	 HOUR ( 8) },
  { "V", tZONE,	 HOUR ( 9) },
  { "W", tZONE,	 HOUR (10) },
  { "X", tZONE,	 HOUR (11) },
  { "Y", tZONE,	 HOUR (12) },
  { "Z", tZONE,	 HOUR ( 0) },
  { 0, 0, 0 }
};



static int
to_hour (long int hours, int meridian)
{
  switch (meridian)
    {
    case MER24:
      return 0 <= hours && hours < 24 ? hours : -1;
    case MERam:
      return 0 < hours && hours < 12 ? hours : hours == 12 ? 0 : -1;
    case MERpm:
      return 0 < hours && hours < 12 ? hours + 12 : hours == 12 ? 12 : -1;
    default:
      abort ();
    }
  /* NOTREACHED */
}

static long int
to_year (textint textyear)
{
  long int year = textyear.value;

  if (year < 0)
    year = -year;

  /* XPG4 suggests that years 00-68 map to 2000-2068, and
     years 69-99 map to 1969-1999.  */
  else if (textyear.digits == 2)
    year += year < 69 ? 2000 : 1900;

  return year;
}

static table const *
lookup_zone (parser_control const *pc, char const *name)
{
  table const *tp;

  /* Try local zone abbreviations first; they're more likely to be right.  */
  for (tp = pc->local_time_zone_table; tp->name; tp++)
    if (strcmp (name, tp->name) == 0)
      return tp;

  for (tp = time_zone_table; tp->name; tp++)
    if (strcmp (name, tp->name) == 0)
      return tp;

  return 0;
}

#if ! HAVE_TM_GMTOFF
/* Yield the difference between *A and *B,
   measured in seconds, ignoring leap seconds.
   The body of this function is taken directly from the GNU C Library;
   see src/strftime.c.  */
static long int
tm_diff (struct tm const *a, struct tm const *b)
{
  /* Compute intervening leap days correctly even if year is negative.
     Take care to avoid int overflow in leap day calculations.  */
  int a4 = (a->tm_year >> 2) + (TM_YEAR_BASE >> 2) - ! (a->tm_year & 3);
  int b4 = (b->tm_year >> 2) + (TM_YEAR_BASE >> 2) - ! (b->tm_year & 3);
  int a100 = a4 / 25 - (a4 % 25 < 0);
  int b100 = b4 / 25 - (b4 % 25 < 0);
  int a400 = a100 >> 2;
  int b400 = b100 >> 2;
  int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
  long int ayear = a->tm_year;
  long int years = ayear - b->tm_year;
  long int days = (365 * years + intervening_leap_days
		   + (a->tm_yday - b->tm_yday));
  return (60 * (60 * (24 * days + (a->tm_hour - b->tm_hour))
		+ (a->tm_min - b->tm_min))
	  + (a->tm_sec - b->tm_sec));
}
#endif /* ! HAVE_TM_GMTOFF */

static table const *
lookup_word (parser_control const *pc, char *word)
{
  char *p;
  char *q;
  size_t wordlen;
  table const *tp;
  bool period_found;
  bool abbrev;

  /* Make it uppercase.  */
  for (p = word; *p; p++)
    if (ISLOWER ((unsigned char) *p))
      *p = toupper ((unsigned char) *p);

  for (tp = meridian_table; tp->name; tp++)
    if (strcmp (word, tp->name) == 0)
      return tp;

  /* See if we have an abbreviation for a month. */
  wordlen = strlen (word);
  abbrev = wordlen == 3 || (wordlen == 4 && word[3] == '.');

  for (tp = month_and_day_table; tp->name; tp++)
    if ((abbrev ? strncmp (word, tp->name, 3) : strcmp (word, tp->name)) == 0)
      return tp;

  if ((tp = lookup_zone (pc, word)))
    return tp;

  if (strcmp (word, dst_table[0].name) == 0)
    return dst_table;

  for (tp = time_units_table; tp->name; tp++)
    if (strcmp (word, tp->name) == 0)
      return tp;

  /* Strip off any plural and try the units table again. */
  if (word[wordlen - 1] == 'S')
    {
      word[wordlen - 1] = '\0';
      for (tp = time_units_table; tp->name; tp++)
	if (strcmp (word, tp->name) == 0)
	  return tp;
      word[wordlen - 1] = 'S';	/* For "this" in relative_time_table.  */
    }

  for (tp = relative_time_table; tp->name; tp++)
    if (strcmp (word, tp->name) == 0)
      return tp;

  /* Military time zones. */
  if (wordlen == 1)
    for (tp = military_table; tp->name; tp++)
      if (word[0] == tp->name[0])
	return tp;

  /* Drop out any periods and try the time zone table again. */
  for (period_found = false, p = q = word; (*p = *q); q++)
    if (*q == '.')
      period_found = true;
    else
      p++;
  if (period_found && (tp = lookup_zone (pc, word)))
    return tp;

  return 0;
}

static int
yylex (YYSTYPE *lvalp, parser_control *pc)
{
  unsigned char c;
  size_t count;

  for (;;)
    {
      while (c = *pc->input, ISSPACE (c))
	pc->input++;

      if (ISDIGIT (c) || c == '-' || c == '+')
	{
	  char const *p;
	  int sign;
	  unsigned long int value;
	  if (c == '-' || c == '+')
	    {
	      sign = c == '-' ? -1 : 1;
	      c = *++pc->input;
	      if (! ISDIGIT (c))
		/* skip the '-' sign */
		continue;
	    }
	  else
	    sign = 0;
	  p = pc->input;
	  for (value = 0; ; value *= 10)
	    {
	      unsigned long int value1 = value + (c - '0');
	      if (value1 < value)
		return '?';
	      value = value1;
	      c = *++p;
	      if (! ISDIGIT (c))
		break;
	      if (ULONG_MAX / 10 < value)
		return '?';
	    }
	  if ((c == '.' || c == ',') && ISDIGIT (p[1]))
	    {
	      time_t s;
	      int ns;
	      int digits;
	      unsigned long int value1;

	      /* Check for overflow when converting value to time_t.  */
	      if (sign < 0)
		{
		  s = - value;
		  if (0 < s)
		    return '?';
		  value1 = -s;
		}
	      else
		{
		  s = value;
		  if (s < 0)
		    return '?';
		  value1 = s;
		}
	      if (value != value1)
		return '?';

	      /* Accumulate fraction, to ns precision.  */
	      p++;
	      ns = *p++ - '0';
	      for (digits = 2; digits <= LOG10_BILLION; digits++)
		{
		  ns *= 10;
		  if (ISDIGIT (*p))
		    ns += *p++ - '0';
		}

	      /* Skip excess digits, truncating toward -Infinity.  */
	      if (sign < 0)
		for (; ISDIGIT (*p); p++)
		  if (*p != '0')
		    {
		      ns++;
		      break;
		    }
	      while (ISDIGIT (*p))
		p++;

	      /* Adjust to the timespec convention, which is that
		 tv_nsec is always a positive offset even if tv_sec is
		 negative.  */
	      if (sign < 0 && ns)
		{
		  s--;
		  if (! (s < 0))
		    return '?';
		  ns = BILLION - ns;
		}

	      lvalp->timespec.tv_sec = s;
	      lvalp->timespec.tv_nsec = ns;
	      pc->input = p;
	      return sign ? tSDECIMAL_NUMBER : tUDECIMAL_NUMBER;
	    }
	  else
	    {
	      if (sign < 0)
		{
		  lvalp->textintval.value = - value;
		  if (0 < lvalp->textintval.value)
		    return '?';
		}
	      else
		{
		  lvalp->textintval.value = value;
		  if (lvalp->textintval.value < 0)
		    return '?';
		}
	      lvalp->textintval.digits = p - pc->input;
	      pc->input = p;
	      return sign ? tSNUMBER : tUNUMBER;
	    }
	}

      if (ISALPHA (c))
	{
	  char buff[20];
	  char *p = buff;
	  table const *tp;

	  do
	    {
	      if (p < buff + sizeof buff - 1)
		*p++ = c;
	      c = *++pc->input;
	    }
	  while (ISALPHA (c) || c == '.');

	  *p = '\0';
	  tp = lookup_word (pc, buff);
	  if (! tp)
	    return '?';
	  lvalp->intval = tp->value;
	  return tp->type;
	}

      if (c != '(')
	return *pc->input++;
      count = 0;
      do
	{
	  c = *pc->input++;
	  if (c == '\0')
	    return c;
	  if (c == '(')
	    count++;
	  else if (c == ')')
	    count--;
	}
      while (count != 0);
    }
}

/* Do nothing if the parser reports an error.  */
static int
yyerror (char *s ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Parse a date/time string, storing the resulting time value into *RESULT.
   The string itself is pointed to by P.  Return true if successful.
   P can be an incomplete or relative time specification; if so, use
   *NOW as the basis for the returned time.  */
bool
get_date (struct timespec *result, char const *p, struct timespec const *now)
{
  time_t Start;
  long int Start_ns;
  struct tm const *tmp;
  struct tm tm;
  struct tm tm0;
  parser_control pc;
  struct timespec gettime_buffer;

  if (! now)
    {
      if (gettime (&gettime_buffer) != 0)
	return false;
      now = &gettime_buffer;
    }

  Start = now->tv_sec;
  Start_ns = now->tv_nsec;

  tmp = localtime (&now->tv_sec);
  if (! tmp)
    return false;

  pc.input = p;
  pc.year.value = tmp->tm_year;
  pc.year.value += TM_YEAR_BASE;
  pc.year.digits = 4;
  pc.month = tmp->tm_mon + 1;
  pc.day = tmp->tm_mday;
  pc.hour = tmp->tm_hour;
  pc.minutes = tmp->tm_min;
  pc.seconds.tv_sec = tmp->tm_sec;
  pc.seconds.tv_nsec = Start_ns;
  tm.tm_isdst = tmp->tm_isdst;

  pc.meridian = MER24;
  pc.rel_ns = 0;
  pc.rel_seconds = 0;
  pc.rel_minutes = 0;
  pc.rel_hour = 0;
  pc.rel_day = 0;
  pc.rel_month = 0;
  pc.rel_year = 0;
  pc.timespec_seen = false;
  pc.dates_seen = 0;
  pc.days_seen = 0;
  pc.rels_seen = 0;
  pc.times_seen = 0;
  pc.local_zones_seen = 0;
  pc.zones_seen = 0;

#if HAVE_STRUCT_TM_TM_ZONE
  pc.local_time_zone_table[0].name = tmp->tm_zone;
  pc.local_time_zone_table[0].type = tLOCAL_ZONE;
  pc.local_time_zone_table[0].value = tmp->tm_isdst;
  pc.local_time_zone_table[1].name = 0;

  /* Probe the names used in the next three calendar quarters, looking
     for a tm_isdst different from the one we already have.  */
  {
    int quarter;
    for (quarter = 1; quarter <= 3; quarter++)
      {
	time_t probe = Start + quarter * (90 * 24 * 60 * 60);
	struct tm const *probe_tm = localtime (&probe);
	if (probe_tm && probe_tm->tm_zone
	    && probe_tm->tm_isdst != pc.local_time_zone_table[0].value)
	  {
	      {
		pc.local_time_zone_table[1].name = probe_tm->tm_zone;
		pc.local_time_zone_table[1].type = tLOCAL_ZONE;
		pc.local_time_zone_table[1].value = probe_tm->tm_isdst;
		pc.local_time_zone_table[2].name = 0;
	      }
	    break;
	  }
      }
  }
#else
#if HAVE_TZNAME
  {
# ifndef tzname
    extern char *tzname[];
# endif
    int i;
    for (i = 0; i < 2; i++)
      {
	pc.local_time_zone_table[i].name = tzname[i];
	pc.local_time_zone_table[i].type = tLOCAL_ZONE;
	pc.local_time_zone_table[i].value = i;
      }
    pc.local_time_zone_table[i].name = 0;
  }
#else
  pc.local_time_zone_table[0].name = 0;
#endif
#endif

  if (pc.local_time_zone_table[0].name && pc.local_time_zone_table[1].name
      && ! strcmp (pc.local_time_zone_table[0].name,
		   pc.local_time_zone_table[1].name))
    {
      /* This locale uses the same abbrevation for standard and
	 daylight times.  So if we see that abbreviation, we don't
	 know whether it's daylight time.  */
      pc.local_time_zone_table[0].value = -1;
      pc.local_time_zone_table[1].name = 0;
    }

  if (yyparse (&pc) != 0)
    return false;

  if (pc.timespec_seen)
    {
      *result = pc.seconds;
      return true;
    }

  if (1 < pc.times_seen || 1 < pc.dates_seen || 1 < pc.days_seen
      || 1 < (pc.local_zones_seen + pc.zones_seen)
      || (pc.local_zones_seen && 1 < pc.local_isdst))
    return false;

  tm.tm_year = to_year (pc.year) - TM_YEAR_BASE + pc.rel_year;
  tm.tm_mon = pc.month - 1 + pc.rel_month;
  tm.tm_mday = pc.day + pc.rel_day;
  if (pc.times_seen || (pc.rels_seen && ! pc.dates_seen && ! pc.days_seen))
    {
      tm.tm_hour = to_hour (pc.hour, pc.meridian);
      if (tm.tm_hour < 0)
	return false;
      tm.tm_min = pc.minutes;
      tm.tm_sec = pc.seconds.tv_sec;
    }
  else
    {
      tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
      pc.seconds.tv_nsec = 0;
    }

  /* Let mktime deduce tm_isdst if we have an absolute time stamp,
     or if the relative time stamp mentions days, months, or years.  */
  if (pc.dates_seen | pc.days_seen | pc.times_seen | pc.rel_day
      | pc.rel_month | pc.rel_year)
    tm.tm_isdst = -1;

  /* But if the input explicitly specifies local time with or without
     DST, give mktime that information.  */
  if (pc.local_zones_seen)
    tm.tm_isdst = pc.local_isdst;

  tm0 = tm;

  Start = mktime (&tm);

  if (Start == (time_t) -1)
    {

      /* Guard against falsely reporting errors near the time_t boundaries
	 when parsing times in other time zones.  For example, if the min
	 time_t value is 1970-01-01 00:00:00 UTC and we are 8 hours ahead
	 of UTC, then the min localtime value is 1970-01-01 08:00:00; if
	 we apply mktime to 1970-01-01 00:00:00 we will get an error, so
	 we apply mktime to 1970-01-02 08:00:00 instead and adjust the time
	 zone by 24 hours to compensate.  This algorithm assumes that
	 there is no DST transition within a day of the time_t boundaries.  */
      if (pc.zones_seen)
	{
	  tm = tm0;
	  if (tm.tm_year <= EPOCH_YEAR - TM_YEAR_BASE)
	    {
	      tm.tm_mday++;
	      pc.time_zone += 24 * 60;
	    }
	  else
	    {
	      tm.tm_mday--;
	      pc.time_zone -= 24 * 60;
	    }
	  Start = mktime (&tm);
	}

      if (Start == (time_t) -1)
	return false;
    }

  if (pc.days_seen && ! pc.dates_seen)
    {
      tm.tm_mday += ((pc.day_number - tm.tm_wday + 7) % 7
		     + 7 * (pc.day_ordinal - (0 < pc.day_ordinal)));
      tm.tm_isdst = -1;
      Start = mktime (&tm);
      if (Start == (time_t) -1)
	return false;
    }

  if (pc.zones_seen)
    {
      long int delta = pc.time_zone * 60;
      time_t t1;
#ifdef HAVE_TM_GMTOFF
      delta -= tm.tm_gmtoff;
#else
      time_t t = Start;
      struct tm const *gmt = gmtime (&t);
      if (! gmt)
	return false;
      delta -= tm_diff (&tm, gmt);
#endif
      t1 = Start - delta;
      if ((Start < t1) != (delta < 0))
	return false;	/* time_t overflow */
      Start = t1;
    }

  /* Add relative hours, minutes, and seconds.  Ignore leap seconds;
     i.e. "+ 10 minutes" means 600 seconds, even if one of them is a
     leap second.  Typically this is not what the user wants, but it's
     too hard to do it the other way, because the time zone indicator
     must be applied before relative times, and if mktime is applied
     again the time zone will be lost.  */
  {
    long int sum_ns = pc.seconds.tv_nsec + pc.rel_ns;
    long int normalized_ns = (sum_ns % BILLION + BILLION) % BILLION;
    time_t t0 = Start;
    long int d1 = 60 * 60 * pc.rel_hour;
    time_t t1 = t0 + d1;
    long int d2 = 60 * pc.rel_minutes;
    time_t t2 = t1 + d2;
    long int d3 = pc.rel_seconds;
    time_t t3 = t2 + d3;
    long int d4 = (sum_ns - normalized_ns) / BILLION;
    time_t t4 = t3 + d4;

    if ((d1 / (60 * 60) ^ pc.rel_hour)
	| (d2 / 60 ^ pc.rel_minutes)
	| ((t1 < t0) ^ (d1 < 0))
	| ((t2 < t1) ^ (d2 < 0))
	| ((t3 < t2) ^ (d3 < 0))
	| ((t4 < t3) ^ (d4 < 0)))
      return false;

    result->tv_sec = t4;
    result->tv_nsec = normalized_ns;
    return true;
  }
}

#if TEST

int
main (int ac, char **av)
{
  char buff[BUFSIZ];

  printf ("Enter date, or blank line to exit.\n\t> ");
  fflush (stdout);

  buff[BUFSIZ - 1] = 0;
  while (fgets (buff, BUFSIZ - 1, stdin) && buff[0])
    {
      struct timespec d;
      struct tm const *tm;
      if (! get_date (&d, buff, NULL))
	printf ("Bad format - couldn't convert.\n");
      else if (! (tm = localtime (&d.tv_sec)))
	{
	  long int sec = d.tv_sec;
	  printf ("localtime (%ld) failed\n", sec);
	}
      else
	{
	  int ns = d.tv_nsec;
	  printf ("%04ld-%02d-%02d %02d:%02d:%02d.%09d\n",
		  tm->tm_year + 1900L, tm->tm_mon + 1, tm->tm_mday,
		  tm->tm_hour, tm->tm_min, tm->tm_sec, ns);
	}
      printf ("\t> ");
      fflush (stdout);
    }
  return 0;
}
#endif /* defined TEST */
