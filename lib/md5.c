/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

/* This code was modified in 1997 by Jim Kingdon of Cyclic Software to
   not require an integer type which is exactly 32 bits.  This work
   draws on the changes for the same purpose by Tatu Ylonen
   <ylo@cs.hut.fi> as part of SSH, but since I didn't actually use
   that code, there is no copyright issue.  I hereby disclaim
   copyright in any changes I have made; this code remains in the
   public domain.  */

/* Note regarding cvs_* namespace: this avoids potential conflicts
   with libraries such as some versions of Kerberos.  No particular
   need to worry about whether the system supplies an MD5 library, as
   this file is only about 3k of object code.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>	/* for memcpy() and memset() */

