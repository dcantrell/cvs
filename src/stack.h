/*
 * Copyright (c) 2004, Free Software Foundation,
 *                     Derek Price,
 *                     & Ximbiot <http://ximbiot.com>.
 * 
 * You may distribute under the terms of the GNU General Public License as
 * specified in the README file that comes with the CVS source distribution.
 */

void push (List *_stack, void *_elem);
void *pop (List *_stack);
void unshift (List *_stack, void *_elem);
void *shift (List *_stack);
int isempty (List *_stack);
