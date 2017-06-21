/* $Id: serial_compress.c,v 3.2.62.1 1996/06/28 23:34:11 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xremote/serial_compress.c,v $
 *------------------------------------------------------------------
 * serial_compress.c - Asynchronous line compression/decompression
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: serial_compress.c,v $
 * Revision 3.2.62.1  1996/06/28  23:34:11  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:26:39  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:31:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/compress_lzw.h"
#include "serial_compress.h"

/*
 * Initialize compression context
 */

char *InitCompression(void)
{
    return(malloc(sizeof(serialctx)));
}

/*
 * Free compression conterxt
 */

void FreeCompression(serialctx *ctx)
{
    free(ctx);
}

/*
 * reset the output compressor to a state that will cause it to emit a clear
 * code in the correct code size the next time the compressor is invoked.
 */
void ResetCompressionOutput(serialctx *ctx)
{
    compress_private *priv = &ctx->compress_state;
    need_output_clear = TRUE;
}

/*
 * Restart the compressor.  Both the input and output sides are
 * set to their initial values.
 */
void RestartCompression(serialctx *ctx)
{
    compress_private *priv;

    priv = &ctx->compress_state;
    priv->initialized = 0;
    priv = &ctx->decompress_state;
    priv->initialized = 0;
}

/* 
 *  ctx    Pointer to serial line context block
 *  pbuf   start of plain text buffer
 *  plen   length of plain data
 *  mbuf   start of compressed data
 *  mlen   max length of compressed data
 */
int SerialCompress(serialctx *ctx, uchar *pbuf, int plen, uchar *mbuf, int mlen)
{
    compress_private * priv = &ctx->compress_state;
    outputbuf = mbuf;
    outputbufend = mbuf + mlen;
    inputbuf = pbuf;
    inputbufend = pbuf + plen;
    if (!priv->initialized) {
	init_compress(priv);

	offset = 0;
	clear_flg = 0;
	maxcode = MAXCODE(n_bits = INIT_BITS);
	free_ent = ((block_compress) ? FIRST : 256 );

	/* emit the magic header */
	
	putchar_compress(priv, MAGIC_1);
	putchar_compress(priv, MAGIC_2);
	putchar_compress(priv, (char)(maxbits | block_compress));

	bytes_out += 3;		/* includes 3-byte header mojo */
    }
    if (need_output_clear) {
	n_bits = last_output_nbits;	/* restore last known bitsize */
	offset = 0;			/* Ensure no residual offset */
	if (!clear_output(priv)) {	/* reset output codes */
	    need_output_clear = TRUE;
	    return(-1);
	}
    } else {
	last_output_nbits = n_bits;		/* save last known bitsize */
    }
    if (!compress(priv)) {
	need_output_clear = TRUE;
	return(-1);
    }

    need_output_clear = FALSE;
    return(outputbuf - mbuf);
}

int SerialDecompress(serialctx *ctx, uchar *mbuf, int mlen, uchar *pbuf, int plen)
{
    compress_private * priv = &ctx->decompress_state;
    inputbuf = mbuf;
    inputbufend = mbuf + mlen;
    outputbuf = pbuf;
    outputbufend = pbuf + plen;
    if (!priv->initialized) {
	init_compress(priv);

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
    }
    outputcount = 0;
    if (!decompress(priv))
      return(-1);

    return(outputbuf - pbuf);
}
