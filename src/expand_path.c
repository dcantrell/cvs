/* expand_path.c -- expand environmental variables in passed in string
 *
 * The main routine is expand_path(), it is the routine that handles
 * the '~' character in four forms: 
 *     ~name
 *     ~name/
 *     ~/
 *     ~
 * and handles environment variables contained within the pathname
 * which are defined by:
 *     ${var_name}   (var_name is the name of the environ variable)
 *     $var_name     (var_name ends w/ non-alphanumeric char other than '_')
 */

#include "cvs.h"
#include <sys/types.h>

static char *expand_variable PROTO((char *env));
extern char *xmalloc ();
extern void  free ();


/* User variables.  */

List *variable_list = NULL;

static void variable_delproc PROTO ((Node *));

static void
variable_delproc (node)
    Node *node;
{
    free (node->data);
}

/* Currently used by -s option; we might want a way to set user
   variables in a file in the $CVSROOT/CVSROOT directory too.  */

void
variable_set (nameval)
    char *nameval;
{
    char *p;
    char *name;
    Node *node;

    p = nameval;
    while (isalnum (*p) || *p == '_')
	++p;
    if (*p != '=')
	error (1, 0, "illegal character in user variable name in %s", nameval);
    if (p == nameval)
	error (1, 0, "empty user variable name in %s", nameval);
    name = xmalloc (p - nameval + 1);
    strncpy (name, nameval, p - nameval);
    name[p - nameval] = '\0';
    /* Make p point to the value.  */
    ++p;
    if (strchr (p, '\012') != NULL)
	error (1, 0, "linefeed in user variable value in %s", nameval);

    if (variable_list == NULL)
	variable_list = getlist ();

    node = findnode (variable_list, name);
    if (node == NULL)
    {
	node = getnode ();
	node->type = VARIABLE;
	node->delproc = variable_delproc;
	node->key = name;
	node->data = xstrdup (p);
	(void) addnode (variable_list, node);
    }
    else
    {
	/* Replace the old value.  For example, this means that -s
	   options on the command line override ones from .cvsrc.  */
	free (node->data);
	node->data = xstrdup (p);
	free (name);
    }
}

/*  This routine will expand the pathname to account for ~
    and $ characters as described above.  If an error occurs, NULL
    is returned (FIXME: we should be printing an error message, so that it
    is specific, but instead the caller prints a generic message).  */
char *
expand_path (name)
    char *name;
{
    char *s;
    char *d;
    /* FIXME: arbitrary limit.  */
    char  mybuf[PATH_MAX];
    char  buf[PATH_MAX];
    char *result;
    s = name;
    d = mybuf;
    while ((*d++ = *s))
	if (*s++ == '$')
	{
	    char *p = d;
	    char *e;
	    int flag = (*s == '{');

	    for (; (*d++ = *s); s++)
		if (flag
		    ? *s =='}'
		    : isalnum (*s) == 0 && *s != '_')
		    break;
	    *--d = 0;
	    e = expand_variable (&p[flag]);

	    if (e)
	    {
		for (d = &p[-1]; (*d++ = *e++);)
		    ;
		--d;
		if (flag && *s)
		    s++;
	    }
	    else
		return NULL;	/* no env variable */
	}
    *d = 0;
    s = mybuf;
    d = buf;
    /* If you don't want ~username ~/ to be expanded simply remove
     * This entire if statement including the else portion
     */
    if (*s++ == '~')
    {
	char *t;
	char *p=s;
	if (*s=='/' || *s==0)
	    t = getenv ("HOME");
	else
	{
	    struct passwd *ps;
	    for (; *p!='/' && *p; p++)
		;
	    *p = 0;
	    ps = getpwnam (s);
	    if (ps == 0)
		return NULL;   /* no such user */
	    t = ps->pw_dir;
	}
	while ((*d++ = *t++))
	    ;
	--d;
	if (*p == 0)
	    *p = '/';	       /* always add / */
	s=p;
    }
    else
	--s;
	/* Kill up to here */
    while ((*d++ = *s++))
	;
    *d=0;
    result = xmalloc (sizeof(char) * strlen(buf)+1);
    strcpy (result, buf);
    return result;
}

static char *
expand_variable (name)
	char *name;
{
    if (strcmp (name, CVSROOT_ENV) == 0)
	return CVSroot;
    else if (strcmp (name, RCSBIN_ENV)  == 0)
	return Rcsbin;
    else if (strcmp (name, EDITOR1_ENV) == 0)
	return Editor;
    else if (strcmp (name, EDITOR2_ENV) == 0)
	return Editor;
    else if (strcmp (name, EDITOR3_ENV) == 0)
	return Editor;
    else if (isalpha (name[0]))
	/* It is a CVS internal variable which is not recognized.
	   Return an error; we want to reserve these names for
	   future versions of CVS.  */
	return NULL;
    else if (name[0] == '=')
    {
	Node *node;
	/* Crazy syntax for a user variable.  But we want
	   *something* that lets the user name a user variable
	   anything he wants, without interference from
	   (existing or future) internal variables.  */
	node = findnode (variable_list, name + 1);
	if (node == NULL)
	    /* No such user variable.  */
	    return NULL;
	return node->data;
    }
    else
	/* It is unrecognized character.  We return an
	   error to reserve these for future versions of CVS;
	   it is plausible that various crazy syntaxes might be
	   invented for inserting information about revisions,
	   branches, etc.  */
	return NULL;
}
