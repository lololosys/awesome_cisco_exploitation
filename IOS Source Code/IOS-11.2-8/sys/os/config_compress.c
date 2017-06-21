/* $Id: config_compress.c,v 3.2.62.1 1996/06/28 23:24:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/config_compress.c,v $
 *------------------------------------------------------------------
 * config_compress.c nvram configuration compress/decompress 
 *
 * September 1993, Stig Thormodsrud
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config_compress.c,v $
 * Revision 3.2.62.1  1996/06/28  23:24:37  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:46:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:48:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:54:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "compress_lzw.h"
#include "config_compress.h"

/*  pbuf     start of plain text buffer
 *  plen     length of plain data
 *  mbuf     start of compressed data
 *  mlen     length of compressed data
 */
long config_compress (uchar *pbuf, long plen, uchar *mbuf, long mlen)
{
    compress_private *priv;
    long new_size;

    if ((priv=malloc(sizeof(compress_private))) == NULL) {
	printf(nomemory);
	return -1;
    }

    init_compress(priv);
    outputbuf = mbuf;
    outputbufend = mbuf + mlen;
    inputbuf = pbuf;
    inputbufend = pbuf + plen;
    
    offset = 0;
    clear_flg = 0;
    maxcode = MAXCODE(n_bits = INIT_BITS);
    free_ent = ((block_compress) ? FIRST : 256 );
    
    /* emit the magic header */
    
    putchar_compress(priv, MAGIC_1);
    putchar_compress(priv, MAGIC_2);
    putchar_compress(priv, (char)(maxbits | block_compress));
    bytes_out += 3;		/* includes 3-byte header mojo */
    
    last_output_nbits = n_bits;		/* save last known bitsize */
    
    if (!compress(priv)) {
	need_output_clear = TRUE;
	free(priv);
	return(-1);
    }
    
    need_output_clear = FALSE;
    
    new_size  = outputbuf - mbuf;
    free(priv);
    return(new_size);
}

/*  mbuf     start of plain text buffer
 *  mlen     length of plain data
 *  pbuf     start of compressed data
 *  plen     length of compressed data
 */
long config_decompress (uchar *mbuf, long mlen, uchar *pbuf, long plen)
{
    compress_private *priv;
    long new_size;
    
    if ((priv = malloc(sizeof(compress_private))) == NULL) {
	printf(nomemory);
	return -1;
    }
    
    init_compress(priv);
    inputbuf = mbuf;
    inputbufend = mbuf + mlen;
    outputbuf = pbuf;
    outputbufend = pbuf + plen;
    
    if ((getchar_decompress(priv) != MAGIC_1)
	|| (getchar_decompress(priv) != MAGIC_2)) {
	free(priv);
	return(-1);
    }
    maxbits = getchar_decompress(priv);	/* set -b from file */
    block_compress = maxbits & BLOCK_MASK;
    maxbits &= BIT_MASK;
    maxmaxcode = 1L << maxbits;
    if(maxbits > BITS) {
	free(priv);
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
	free(priv);
	return(-1);
    }
    
    new_size = outputbuf - pbuf;
    free(priv);
    return(new_size);
}
