/*
 * Copyright (C) 2005 The Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Verify interface.  */
#include "sign.h"

/* Standard headers.  */
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* GNULIB headers.  */
#include "error.h"
#include "wait.h"
#include "xalloc.h"

/* CVS headers.  */
#include "classify.h"
#include "client.h"
#include "filesubr.h"
#include "ignore.h"
#include "recurse.h"
#include "root.h"
#include "run.h"
#include "server.h"	/* Get TRACE ().  */
#include "stack.h"
#include "stack.h"
#include "subr.h"
#include "vers_ts.h"



/* FIXME: Once cvs.h is pared ot the bare essentials, it may be included for
 * the following.
 */
extern int noexec;
extern int quiet, really_quiet;
void usage (const char *const *cpp);



/*
 * Globals set via the command line parser in main.c.
 */

/* If a program capable of generating OpenPGP signatures couldn't be found at
 * configure time, default the sign state to off, otherwise, depend on the
 * server support.
 */
#ifdef HAVE_OPENPGP
static sign_state sign_commits = SIGN_DEFAULT;
#else
static sign_state sign_commits = SIGN_NEVER;
#endif

static char *sign_template;
static char *sign_textmode;
static List *sign_args;



void
set_sign_commits (sign_state sign)
{
    sign_commits = sign;
}



void
set_sign_template (const char *template)
{
    assert (template);
    if (sign_template) free (sign_template);
    sign_template = xstrdup (template);
}



void
set_sign_textmode (const char *textmode)
{
    assert (textmode);
    if (sign_textmode) free (sign_textmode);
    sign_textmode = xstrdup (textmode);
}



void
add_sign_arg (const char *arg)
{
    if (!sign_args) sign_args = getlist ();
    push_string (sign_args, xstrdup (arg));
}



/* Return true if the client should attempt to sign files sent to the server
 * as part of a commit.
 *
 * INPUTS
 *   server_support	Whether the server supports signed files.
 */
bool
get_sign_commits (bool server_active, bool server_support)
{
    sign_state tmp;

    /* Only sign commits from the client (and in local mode).  */
    if (server_active) return false;

    if (sign_commits == SIGN_DEFAULT)
	tmp = current_parsed_root->sign;
    else
	tmp = sign_commits;

    return tmp == SIGN_ALWAYS || (tmp == SIGN_DEFAULT && server_support);
}



/* Return SIGN_TEMPLATE from the command line if it exists, else return the
 * SIGN_TEMPLATE from CURRENT_PARSED_ROOT.
 */
static inline const char *
get_sign_template (void)
{
    if (sign_template) return sign_template;
    return current_parsed_root->sign_template;
}



/* Return SIGN_TEXTMODE from the command line if it exists, else return the
 * SIGN_TEXTMODE from CURRENT_PARSED_ROOT.
 *
 * This function is not static because sign_textmode is reused for
 * verify_textmode.
 */
const char *
get_sign_textmode (void)
{
    if (sign_textmode) return sign_textmode;
    return current_parsed_root->sign_textmode;
}



/* Return SIGN_ARGS from the command line if it exists, else return the
 * SIGN_ARGS from CURRENT_PARSED_ROOT.
 */
static inline List *
get_sign_args (void)
{
    if (sign_args && !list_isempty (sign_args)) return sign_args;
    return current_parsed_root->sign_args;
}



/* This function is intended to be passed into walklist() with a list of args
 * to be substituted into the sign template.
 *
 * closure will be a struct format_cmdline_walklist_closure
 * where closure is undefined.
 */
static int
sign_args_list_to_args_proc (Node *p, void *closure)
{
    struct format_cmdline_walklist_closure *c = closure;
    char *arg = NULL;
    const char *f;
    char *d;
    size_t doff;

    if (p->data == NULL) return 1;

    f = c->format;
    d = *c->d;
    /* foreach requested attribute */
    while (*f)
    {
	switch (*f++)
	{
	    case 'a':
		arg = p->key;
		break;
	    default:
		error (1, 0,
		       "Unknown format character or not a list attribute: %c",
		       f[-1]);
		/* NOTREACHED */
		break;
	}
	/* copy the attribute into an argument */
	if (c->quotes)
	{
	    arg = cmdlineescape (c->quotes, arg);
	}
	else
	{
	    arg = cmdlinequote ('"', arg);
	}

	doff = d - *c->buf;
	expand_string (c->buf, c->length, doff + strlen (arg));
	d = *c->buf + doff;
	strncpy (d, arg, strlen (arg));
	d += strlen (arg);
	free (arg);

	/* Always put the extra space on.  we'll have to back up a char
	 * when we're done, but that seems most efficient.
	 */
	doff = d - *c->buf;
	expand_string (c->buf, c->length, doff + 1);
	d = *c->buf + doff;
	*d++ = ' ';
    }
    /* correct our original pointer into the buff */
    *c->d = d;
    return 0;
}



char *
get_sigfile_name (const char *fn)
{
    return Xasprintf ("%s%s%s", BAKPREFIX, fn, ".sig");
}



bool
have_sigfile (bool server_active, const char *fn)
{
    char *sfn;
    bool retval;

    /* Sig files are only created on the server.  Optimize.  */
    if (!server_active) return false;

    sfn = get_sigfile_name (fn);
    if (isreadable (sfn)) retval = true;
    else retval = false;

    free (sfn);
    return retval;
}



/* Generate a signature and return it in allocated memory.  */
char *
gen_signature (const char *srepos, const char *filename, bool bin, size_t *len)
{
    char *cmdline;
    FILE *pipefp;
    bool save_noexec = noexec;
    char *sigbuf = NULL;
    size_t sigoff = 0, sigsize = 64;
    int pipestatus;

    /*
     * %p = shortrepos
     * %r = repository
     * %{a} = user defined sign args
     * %t = textmode flag
     * %s = file name
     */
    /*
     * Cast any NULL arguments as appropriate pointers as this is an
     * stdarg function and we need to be certain the caller gets what
     * is expected.
     */
    cmdline = format_cmdline (
#ifdef SUPPORT_OLD_INFO_FMT_STRINGS
	                      false, srepos,
#endif /* SUPPORT_OLD_INFO_FMT_STRINGS */
	                      get_sign_template (),
	                      "a", ",", get_sign_args (),
			      sign_args_list_to_args_proc, (void *) NULL,
	                      "r", "s", current_parsed_root->directory,
	                      "p", "s", srepos,
	                      "t", "s", bin ? NULL : get_sign_textmode (),
	                      "s", "s", filename,
	                      (char *) NULL);

    if (!cmdline || !strlen (cmdline))
	error (1, 0, "sign template resolved to the empty string!");

    noexec = false;
    if (!(pipefp = run_popen (cmdline, "r")))
	error (1, errno, "failed to execute signature generator");
    noexec = save_noexec;

    do
    {
	size_t len;

	sigsize *= 2;
	sigbuf = xrealloc (sigbuf, sigsize);
	len = fread (sigbuf + sigoff, sizeof *sigbuf, sigsize - sigoff,
		     pipefp);
	sigoff += len;
	/* Fewer bytes than requested means EOF or error.  */
    } while (sigsize == sigoff);

    if (ferror (pipefp))
	error (1, ferror (pipefp), "Error reading from sign program.");

    pipestatus = pclose (pipefp);
    if (pipestatus == -1)
	error (1, errno,
	       "failed to obtain exit status from signature program");
    else if (pipestatus)
    {
	if (WIFEXITED (pipestatus))
	    error (1, 0, "sign program exited with error code %d",
		   WEXITSTATUS (pipestatus));
	else
	    error (1, 0, "sign program exited via signal %d",
		   WTERMSIG (pipestatus));
    }

    *len = sigoff;
    return sigbuf;
}



/* Read a signature from a file and return it in allocated memory.  */
static char *
read_signature (const char *fn, size_t *len)
{
    char *sfn = get_sigfile_name (fn);
    char *data = NULL;
    size_t datasize;

    get_file (sfn, sfn, "rb", &data, &datasize, len);

    free (sfn);
    return data;
}



/* Generate a signature or read one from the sigfile and return it in
 * allocated memory.
 */
char *
get_signature (bool server_active, const char *srepos, const char *filename,
	       bool bin, size_t *len)
{
    if (server_active) return read_signature (filename, len);
    /* else */ return gen_signature (srepos, filename, bin, len);
}



static int
sign_fileproc (void *callerdat, struct file_info *finfo)
{
    Vers_TS *vers;
    int err = 0;
    Ctype status;

    TRACE (TRACE_FUNCTION, "sign_fileproc (%s)", finfo->fullname);

    status = Classify_File (finfo, NULL, NULL, NULL, true,
			    false, &vers, false);

    switch (status)
    {
	case T_UNKNOWN:			/* unknown file was explicitly asked
					 * about */
	    error (0, 0, "Nothing known about `%s'", finfo->fullname);
	    err++;
	    break;
	case T_CONFLICT:		/* old punt-type errors */
	case T_NEEDS_MERGE:		/* needs merging */
	case T_MODIFIED:		/* locally modified */
	case T_ADDED:			/* added but not committed */
	case T_REMOVED:			/* removed but not committed */
	    error (0, 0, "Locally modified file `%s' may not be signed.",
		   finfo->fullname);
	    err++;
	    break;
	case T_CHECKOUT:		/* needs checkout */
	    if (!vers->ts_user)
	    {
		assert (vers->vn_user);
		error (0, 0,
"File `%s' not present locally (checkout before signing)",
		       finfo->fullname);
		err++;
		break;
	    }
	    /* else, fall through */
	case T_REMOVE_ENTRY:		/* needs to be un-registered */
	case T_PATCH:			/* needs patch */
	case T_UPTODATE:		/* file was already up-to-date */
	    if (!isfile (finfo->file))
		RCS_checkout (finfo->rcs, finfo->file, vers->vn_user, 
			      vers->tag, vers->options, NULL, NULL, NULL);
	    if (file_contains_keyword (finfo))
	    {
		/* Make this a warning, not an error, because the user may
		 * be intentionally signing a file with keywords.  Such a file
		 * may still be verified when checked out -ko.
		 */
		if (!quiet)
		    error (0, 0,
"warning: signed file `%s' contains at least one RCS keyword",
			   finfo->fullname);
	    }

	    RCS_add_openpgp_signature (finfo, vers->vn_user);
	    if (server_active)
		server_base_signatures (finfo, vers->vn_user);
	    break;
	default:			/* can't ever happen :-) */
	    error (0, 0,
		   "unknown file status %d for file `%s'",
		   status, finfo->file);
	    err++;
	    break;
    }

    return err;
}



static const char *const sign_usage[] =
{
    "Usage: %s %s [-lR] [files...]\n",
    "\t-l\tProcess this directory only (not recursive).\n",
    "\t-R\tProcess directories recursively.\n",
    "(Specify the --help global option for a list of other help options)\n",
    NULL
};

int
sign (int argc, char **argv)
{
    int c;
    int err = 0;
    bool local = false;

    if (argc == -1)
	usage (sign_usage);

    optind = 0;
    while ((c = getopt (argc, argv, "+lR")) != -1)
    {
	switch (c)
	{
	    case 'l':
		local = 1;
		break;
	    case 'R':
		local = 0;
		break;
	    case '?':
	    default:
		usage (sign_usage);
		break;
	}
    }
    argc -= optind;
    argv += optind;

#ifdef CLIENT_SUPPORT
    if (current_parsed_root->isremote)
    {
	start_server ();

	ign_setup ();

	if (local)
	    send_arg("-l");
	send_arg ("--");

	send_files (argc, argv, local, false, 0);
	send_file_names (argc, argv, SEND_EXPAND_WILD);

	send_to_server ("sign\012", 0);
	err = get_responses_and_close ();

	return err;
    }
#endif

    /* start the recursion processor */
    err = start_recursion (sign_fileproc, NULL, NULL, NULL, NULL, argc, argv,
			   local, W_LOCAL, false, CVS_LOCK_WRITE, NULL, true,
			   NULL);

    return err;
}
