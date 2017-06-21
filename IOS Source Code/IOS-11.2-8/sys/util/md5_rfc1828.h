/* $Id: md5_rfc1828.h,v 3.1.22.1 1996/07/16 08:01:34 raj Exp $
 * $Source: /release/112/cvs/Xsys/util/md5_rfc1828.h,v $
 *------------------------------------------------------------------
 * All-in-one keyed MD5 routines (supporting RFC-1321 / RFC-1828)
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md5_rfc1828.h,v $
 * Revision 3.1.22.1  1996/07/16  08:01:34  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/17  01:26:29  pst
 * CSCdi49257:  need user-friendly support for RFC-1828 keyed MD5
 * authentication (add a .h file)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

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

extern boolean md5_rfc1828(void *data,    int datalen,
			   char *key,     int keylen,
			   uchar *digest, int digestlen);

/*
 * md5_hmac is the proposed replacement for RFC1828,  there are proofs
 * that it is as cryptographicly secure as MD5.
 *
 * see draft-krawczyk-keyed-md5-01.txt for more info
 */
extern boolean md5_hmac(void *data,    int datalen,
			char *key,     int keylen,
			uchar *digest, int digestlen);
