/* $Id: ucode_compress.c,v 3.2 1995/11/17 09:21:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/ucode_compress.c,v $
 *------------------------------------------------------------------
 * ucode_compress.c ucode bundling decompress
 *
 * September 1993, Stig Thormodsrud
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: ucode_compress.c,v $
 * Revision 3.2  1995/11/17  09:21:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/compress_lzw.h"
#include "ucode_compress.h"

/*  mbuf     start of compressed data
 *  mlen     length of compressed data
 *  pbuf     start of plain text buffer
 *  plen     length of plain data
 */
long ucode_decompress (const uchar *mbuf, long mlen, uchar *pbuf, long plen)
{
    /*  since this can be called during a hotswap interrupt, don't
     *  malloc the priv structure.  Must use variable name 'priv' since the
     *  compress uses macros with priv.
     */

    static compress_private priv_data;
    compress_private *priv = &priv_data; 
    long new_size;
    
    init_compress(priv);
    inputbuf = (uchar *)mbuf;
    inputbufend = (uchar *)mbuf + mlen;
    outputbuf = pbuf;
    outputbufend = pbuf + plen;
    
    if ((getchar_decompress(priv) != MAGIC_1)
	|| (getchar_decompress(priv) != MAGIC_2)) {
	return(-1);
    }
    maxbits = getchar_decompress(priv);	/* set -b from file */
    block_compress = maxbits & BLOCK_MASK;
    maxbits &= BIT_MASK;
    maxmaxcode = 1L << maxbits;
    if(maxbits > BITS) {
	return(-1);
    }
    {
	register int code;
	/*
	 * As above, initialize the first 256 entries in the table.
	 */
	maxcode = MAXCODE(n_bits = INIT_BITS);
	for ( code = 255; code >= 0; code-- ) {
	    tab_prefixof(code) = 0;
	    tab_suffixof(code) = (uchar)code;
	}
	free_ent = ((block_compress) ? FIRST : 256 );
    }
    
    outputcount = 0;
    if (!decompress(priv)) {
	return(-1);
    }
    
    new_size = outputbuf - pbuf;
    return(new_size);
}









