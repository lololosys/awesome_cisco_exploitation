/* $Id: dess2k.c,v 3.2 1995/11/17 17:46:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/dess2k.c,v $
 *------------------------------------------------------------------
 * dess2k.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dess2k.c,v $
 * Revision 3.2  1995/11/17  17:46:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original string2key.c 3.2 5/11/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990, 1991  by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csf/header.h>


/*
converts the string pointed to by "data" into an encryption key
of type "keytype".  *keyblock is filled in with the key info;
in particular, keyblock->contents is to be set to allocated storage.
It is the responsibility of the caller to release this storage
when the generated key no longer needed.

The routine may use "salt" to seed or alter the conversion
algorithm.

If the particular function called does not know how to make a
key of type "keytype", an error may be returned.

returns: errors
*/

csf_sts csfDESstringTo_kBlk (
	const void *data,
	csf_size_t datalen,
	const void *salt,
	csf_size_t saltlen,
	csfDES_kBlk *okey
){
	char *str, *copystr;
	csf_o *key;

	unsigned temp, i;
	int j;
	csf_size_t length;
	csf_o *k_p;
	int forward;
	csf_o *p_char;
	csf_o k_char[64];
	csfDES_kSched key_sked;


	key = (csf_o *) okey;

	length = datalen;
	if (salt) {
		length += saltlen;
	}

	copystr = csfCmalloc( length );
	if (!copystr) {
		return CSF_STS_ENOMEM;
	}

	csfCmemcpy( copystr, data, datalen );
	if (salt) {
		csfCmemcpy( (char *) copystr + datalen, salt, saltlen );
	}

	/* convert to des key */
	forward = 1;
	p_char = k_char;

	/* init key array for bits */
	csfCmemzero( k_char, sizeof(k_char) );

	_csfDBGprint(( "\n\ninput str length = %d	string = %*s\nstring = 0x ",
		length,length,str ));

	str = copystr;

	/* get next 8 bytes, strip parity, xor */
	for (i = 1; i <= length; i++) {
		/* get next input key byte */
		temp = (unsigned int) *str++;
		_csfDBGprint(( "%02x ",temp & 0xff ));
		/* loop through bits within byte, ignore parity */
		for (j = 0; j <= 6; j++) {
			if (forward) {
				*p_char++ ^= (int) temp & 01;
			}
			else {
				*--p_char ^= (int) temp & 01;
			}
			temp = temp >> 1;
		}

		/* check and flip direction */
		if ((i % 8) == 0) {
			forward = !forward;
		}
	}

	/* now stuff into the key csfDES_kBlk, and force odd parity */
	p_char = k_char;
	k_p = (csf_o *) key;

	for (i = 0; i <= 7; i++) {
		temp = 0;
		for (j = 0; j <= 6; j++) {
			temp |= *p_char++ << (1 + j);
		}
		*k_p++ = (csf_o) temp;
	}

	/* fix key parity */
	csfDESfixParity_kBlk( key );

	/* Now one-way encrypt it with the folded key */
	(void) csfDESmake_kSched( key, &key_sked );
	(void) csfDESchecksumCBC( copystr, key, length, &key_sked, key );
	/* erase key_sked */
	csfCmemzero( &key_sked, sizeof(key_sked) );

	/* clean & free the input string */
	csfCmemzero( copystr, length );
	csfCfree( copystr );

	/* now fix up key parity again */
	csfDESfixParity_kBlk( key );

	_csfDBGprint(( "\nResulting string_to_key = 0x%x 0x%x\n",
		*((csf_ui32 *) key),
		*((csf_ui32 *) key+1) ));

	return CSF_STS_OK;
}
