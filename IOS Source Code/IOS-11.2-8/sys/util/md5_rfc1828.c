/* $Id: md5_rfc1828.c,v 3.2.22.2 1996/07/18 22:23:06 hampton Exp $
 * $Source: /release/112/cvs/Xsys/util/md5_rfc1828.c,v $
 *------------------------------------------------------------------
 * All-in-one keyed MD5 routines (supporting RFC-1321 / RFC-1828)
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md5_rfc1828.c,v $
 * Revision 3.2.22.2  1996/07/18  22:23:06  hampton
 * Library includes should use angle braces not quotes.  [CSCdi63416]
 * Branch: California_branch
 *
 * Revision 3.2.22.1  1996/07/16  08:01:34  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.2  1996/02/17  01:26:28  pst
 * CSCdi49257:  need user-friendly support for RFC-1828 keyed MD5
 * authentication (add a .h file)
 *
 * Revision 3.1  1996/02/17  00:46:04  pst
 * CSCdi49257:  need user-friendly support for RFC-1828 keyed MD5
 * authentication
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "md5.h"
#include "md5_rfc1828.h"
#include <ciscolib.h>

#define	FILL_LEN 64	/* 512 bit boundaries for fill */
#define	MOD_LEN	 56	/* pad key to bit 448 modulo 512 */

/*
 * md5_rfc1828
 *
 * User-friendly way to compute keyed MD5 authentication information for
 * a chunk of data.
 *
 * Call it like this:
 *
 * char hash[MD5_LEN];
 * md5_rfc1828(message, msglength, key, strlen(key), hash, sizeof(hash));
 */

boolean
md5_rfc1828 (void *data,    int datalen,
	     char *key,     int keylen,
	     uchar *digest, int digestlen)
{
	static uchar padding[FILL_LEN] = { 0x80, 0 };

	MD5_CTX context;
	int padlen;

	if (digestlen < MD5_LEN)
	    return FALSE;

	padlen = keylen % FILL_LEN;
	padlen = padlen < MOD_LEN ? MOD_LEN - padlen
				  : (FILL_LEN+MOD_LEN) - padlen;

	MD5Init(&context);
	MD5Update(&context, key, keylen);
	MD5Update(&context, padding, padlen);
	MD5Update(&context, data, datalen);
	MD5Update(&context, key, keylen);
	MD5Final(digest, &context);

	return TRUE;
}

/*
 * md5_hmac
 * see draft-krawczyk-keyed-md5-01.txt for more info
 *
 * User-friendly way to compute keyed MD5 authentication information for
 * a chunk of data.
 *
 * Call it like this:
 *
 * char hash[MD5_LEN];
 * md5_hmac(message, msglength, key, strlen(key), hash, sizeof(hash));
 */

boolean
md5_hmac (void *data,    int datalen,
	  char *key,     int keylen,
	  uchar *digest, int digestlen)
{
    static uchar pad1[48] = {			/* magic numbers */
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36 };
    static uchar pad2[48] = {
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c,
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c,
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c,
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c,
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c,
	0x5c, 0x5c, 0x5c, 0x56, 0x5c, 0x5c, 0x5c, 0x5c };

    MD5_CTX context;
    uchar   temp[MD5_LEN];

    if (digestlen < MD5_LEN)
	return FALSE;

    MD5Init(&context);
    MD5Update(&context, key, keylen);
    MD5Update(&context, pad1, sizeof(pad1));
    MD5Update(&context, data, datalen);
    MD5Final(temp, &context);

    MD5Init(&context);
    MD5Update(&context, key, keylen);
    MD5Update(&context, pad2, sizeof(pad2));
    MD5Update(&context, temp, sizeof(temp));
    MD5Final(digest, &context);

    bzero(temp, sizeof(temp));		/* clear out interim results */
    return TRUE;
}
