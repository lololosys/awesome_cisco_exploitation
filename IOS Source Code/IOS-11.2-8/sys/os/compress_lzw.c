/* $Id: compress_lzw.c,v 3.3.62.2 1996/07/10 22:21:52 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/compress_lzw.c,v $
 *------------------------------------------------------------------
 * compress_lzw.c - generic compress/decompress routines
 *
 * September 1993, Stig Thormodsrud
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: compress_lzw.c,v $
 * Revision 3.3.62.2  1996/07/10  22:21:52  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/03/18  21:29:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:18:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  16:38:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:46:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "printf.h"
#include <string.h>
#include "../os/compress_lzw.h"

static boolean output(compress_private *, int);
static boolean cl_block(compress_private *);
static void cl_hash(compress_private *, register long);

/*
 * I/O routine emulation
 */

/*
 * getchar_compress - Input a byte for compress
 */
inline int getchar_compress (compress_private *priv)
{
    if (inputbuf < inputbufend)
	return(*inputbuf++);
    
    return(EOF);
}

/*
 * putchar_compress - Output a byte for compress
 */
inline boolean putchar_compress (compress_private *priv, uchar c)
{
    if (outputbuf < outputbufend) {
	*outputbuf++ = c;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * putstring_compress - Output a string for compress
 */
inline boolean putstring_compress (compress_private *priv, uchar *str, int len)
{
    while (len--)
	if (!putchar_compress(priv, *str++))
	    return(FALSE);
    
    return(TRUE);
}

/*
 * getchar_decompress - Get a byte for decompress
 */

inline int getchar_decompress (compress_private *priv)
{
    if (inputbuf < inputbufend)
	return(*inputbuf++);
    
    return(EOF);
}

/*
 * putchar_decompress - Output a character for decompress
 *
 * If there is room for the character in the buffer, stuff it and update
 * the pointer. Otherwise, return FALSE.
 */
inline boolean putchar_decompress (compress_private *priv, char c)
{
    if (outputbuf < outputbufend) {
	*outputbuf++ = c;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the 
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */
boolean compress (compress_private *priv)
{
    register long fcode;
    register int i = 0;
    register int c;
    register int ent;
    register int disp;
    register int hsize_reg;
    register int hshift;
    
    ent = getchar_compress(priv);
    in_count++;
    
    hshift = 0;
    for (fcode = (long)hsize; fcode < 65536L; fcode *= 2L)
    	hshift++;
    hshift = 8 - hshift;		/* set hash code range bound */
    
    hsize_reg = hsize;
    
    while ((c = getchar_compress(priv)) != EOF) {
	in_count++;
	
	fcode = (long)(((long) c << maxbits) + ent);
 	i = (((long)c << hshift) ^ ent);	/* xor hashing */
	
	if (htabof (i) == fcode) {
	    ent = codetabof(i);
	    continue;
	} else if ((long)htabof(i) < 0)	/* empty slot */
	    goto nomatch;
 	disp = hsize_reg - i;		/* secondary hash (after G. Knott) */
	if (i == 0)
	    disp = 1;
    probe:
	if ((i -= disp) < 0)
	    i += hsize_reg;
	
	if (htabof(i) == fcode) {
	    ent = codetabof(i);
	    continue;
	}
	if ((long)htabof(i) > 0) 
	    goto probe;
    nomatch:
	if (!output(priv,(int)ent))
	    return(FALSE);
	out_count++;
 	ent = c;
	if (free_ent < maxmaxcode) {
 	    codetabof(i) = free_ent++;	/* code -> hashtable */
	    htabof(i) = fcode;
	}
	else if ((long int)in_count >= checkpoint && block_compress)
	    if (!cl_block(priv))
		return(FALSE);
    }
    /*
     * Put out the final code.
     */
    if (!output(priv,(int)ent))
	return(FALSE);
    out_count++;
    if (!output(priv,(int)-1))
	return(FALSE);
    
    return(TRUE);		/* Success */
}

boolean clear_output (compress_private *priv)
{
    ratio = 0;
    cl_hash(priv, (int)hsize);
    free_ent = FIRST;
    clear_flg = 1;
    return (output(priv, (int)CLEAR));
}

/* table clear for block compress */
static boolean cl_block (compress_private *priv)
{
    register long int rat;
    
    checkpoint = in_count + CHECK_GAP;
    
    if (in_count > 0x007fffff) {	/* shift will overflow */
	rat = bytes_out >> 8;
	if (rat == 0) {		/* Don't divide by zero */
	    rat = 0x7fffffff;
	} else {
	    rat = in_count / rat;
	}
    } else {
	rat = (in_count << 8) / bytes_out;	/* 8 fractional bits */
    }
    if (rat > ratio) {
	ratio = rat;
    } else {
	if (!clear_output(priv))		/* reset output codes */
	    return(FALSE);
    }
    return(TRUE);
}

/* reset code table */
static void cl_hash (compress_private *priv, register long Hsize)
{
    register long *htab_p = (long *)htab+Hsize;
    register long i;
    register long m1 = -1;
    
    i = Hsize - 16;
    do {				/* might use Sys V memset(3) here */
	*(htab_p-16) = m1;
	*(htab_p-15) = m1;
	*(htab_p-14) = m1;
	*(htab_p-13) = m1;
	*(htab_p-12) = m1;
	*(htab_p-11) = m1;
	*(htab_p-10) = m1;
	*(htab_p-9) = m1;
	*(htab_p-8) = m1;
	*(htab_p-7) = m1;
	*(htab_p-6) = m1;
	*(htab_p-5) = m1;
	*(htab_p-4) = m1;
	*(htab_p-3) = m1;
	*(htab_p-2) = m1;
	*(htab_p-1) = m1;
	htab_p -= 16;
    } while ((i -= 16) >= 0);
    for (i += 16; i > 0; i--)
	*--htab_p = m1;
}

void init_compress (compress_private *priv)
{
    /*
     * perform global data initializations.
     * Normally, this would be done at bss clear time, but this
     * core runs before bss is set up.
     */
    outputcount = 0;
    n_bits = 0;				/* number of bits/code */
    last_output_nbits = 0;		/* last known bits/code */
    need_output_clear = 0;		/* need to dump a CLEAR code */
    maxbits = BITS;			/* user settable max # bits/code */
    maxcode = 0;			/* maximum code, given n_bits */
    maxmaxcode = 1L << BITS;	/* should NEVER generate this code */
    hsize = HSIZE;
    memset(htab, 0, sizeof(htab));
    memset(codetab, 0, sizeof(codetab));
    free_ent = 0;			/* first unused entry */
    save_finchar = 0;
    save_oldcode = 0;
    exit_stat = 0;
    block_compress = BLOCK_MASK;
    clear_flg = 0;
    rmask[0] = 0x00;
    rmask[1] = 0x01;
    rmask[2] = 0x03;
    rmask[3] = 0x07;
    rmask[4] = 0x0f;
    rmask[5] = 0x1f;
    rmask[6] = 0x3f;
    rmask[7] = 0x7f;
    rmask[8] = 0xff;
    
    lmask[0] = 0xff;
    lmask[1] = 0xfe;
    lmask[2] = 0xfc;
    lmask[3] = 0xf8;
    lmask[4] = 0xf0;
    lmask[5] = 0xe0;
    lmask[6] = 0xc0;
    lmask[7] = 0x80;
    lmask[8] = 0x00;
    
    offset = 0, size = 0;
    memset(buf, 0, sizeof(buf));
    in_count = 0;			/* length of input */
    bytes_out = 0;			/* length of compressed output */
    out_count = 0;			/* # of codes output (for debugging) */
    ratio = 0;
    checkpoint = CHECK_GAP;
    cl_hash(priv, (long int)hsize);		/* clear hash table */
    priv->initialized = 1;
}

/*****************************************************************
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 * 	code:	A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *		that n_bits =< (long)wordsize - 1.
 * Outputs:
 * 	Outputs code to the file.
 * Assumptions:
 *	Chars are 8 bits long.
 * Algorithm:
 * 	Maintain a BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */
static boolean output (compress_private *priv, int code)
{
    /*
     * On the VAX, it is important to have the register declarations
     * in exactly the order given, or the asm will break.
     */
    register int r_off = offset, bits= n_bits;
    register uchar *bp = buf;
    
    if (code >= 0) {
	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/*
	 * Since code is always >= 8 bits, only need to mask the first
	 * hunk on the left.
	 */
	*bp = (*bp & rmask[r_off]) | ((code << r_off) & lmask[r_off]);
	bp++;
	bits -= (8 - r_off);
	code >>= 8 - r_off;
	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if (bits >= 8) {
	    *bp++ = code;
	    code >>= 8;
	    bits -= 8;
	}
	/* Last bits. */
	if (bits)
	    *bp = code;
	offset += n_bits;
	if (offset == (n_bits << 3)) {
	    bp = buf;
	    bits = n_bits;
	    bytes_out += bits;
	    do {
		if (!putchar_compress(priv, *bp++))
		    return(FALSE);
	    } while (--bits);
	    offset = 0;
	}
	
	/*
	 * If the next entry is going to be too big for the code size,
	 * then increase it, if possible.
	 */
	if (free_ent > maxcode || (clear_flg > 0)) {
	    /*
	     * Write the whole buffer, because the input side won't
	     * discover the size increase until after it has read it.
	     */
	    if (offset > 0) {
		if (!putstring_compress(priv, buf, n_bits))
		    return(FALSE); /* Buffer overflow */
		bytes_out += n_bits;
	    }
	    offset = 0;
	    
	    if (clear_flg) {
		maxcode = MAXCODE (n_bits = INIT_BITS);
		clear_flg = 0;
	    }
	    else {
		n_bits++;
		if (n_bits == maxbits)
		    maxcode = maxmaxcode;
		else
		    maxcode = MAXCODE(n_bits);
	    }
	}
    } else {
	/*
	 * At EOF, write the rest of the buffer.
	 */
	if (offset > 0)
	    if (!putstring_compress(priv, buf, (offset + 7) / 8))
		return(FALSE);	/* Buffer overflow */
	bytes_out += (offset + 7) / 8;
	offset = 0;
    }
    return(TRUE);		/* Success */
}

/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 * 	stdin
 * Outputs:
 * 	code or EOF is returned.
 */
static int getcode (compress_private *priv)
{
    register int code;
    register int r_off, bits;
    register uchar *bp = buf;
    
    if (clear_flg > 0 || offset >= size || free_ent > maxcode) {
	/*
	 * If the next entry will be too big for the current code
	 * size, then we must increase the size.  This implies reading
	 * a new buffer full, too.
	 */
	if (free_ent > maxcode) {
	    n_bits++;
	    if (n_bits == maxbits)
		maxcode = maxmaxcode;	/* won't get any bigger now */
	    else
		maxcode = MAXCODE(n_bits);
	}
	if (clear_flg > 0) {
    	    maxcode = MAXCODE (n_bits = INIT_BITS);
	    clear_flg = 0;
	}
	{
	    int c;
	    int count = n_bits;
	    register uchar *bpx = buf;
	    
	    size = 0;
	    while (count--) {
		c = getchar_decompress(priv);
		if (c == EOF)
		    break;
		*bpx++ = c;
		size++;
	    }
	}
	if (size <= 0)
	    return EOF;			/* end of file */
	offset = 0;
	/* Round size down to integral number of codes */
	size = (size << 3) - (n_bits - 1);
    }
    r_off = offset;
    bits = n_bits;
    /*
     * Get to the first byte.
     */
    bp += (r_off >> 3);
    r_off &= 7;
    /* Get first part (low order bits) */
    code = (*bp++ >> r_off);
    bits -= (8 - r_off);
    r_off = 8 - r_off;		/* now, offset into code word */
    /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
    if (bits >= 8) {
	code |= *bp++ << r_off;
	r_off += 8;
	bits -= 8;
    }
    /* high order bits. */
    code |= (*bp & rmask[bits]) << r_off;
    offset += n_bits;
    
    return code;
}

/*
 * Decompress stdin to stdout.  This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.  The tables used herein are shared
 * with those of the compress() routine.  See the definitions above.
 *
 * This returns TRUE if the data is successfully decompressed, FALSE
 * if a buffer or stack overflow occured.
 */
boolean decompress (compress_private *priv)
{
    register uchar *stackp;
    register int finchar;
    register int code, oldcode, incode;
    register int restarting;
    
    if (in_count == 0) {
	restarting = FALSE;
	finchar = oldcode = getcode(priv);
	in_count++;
	if(oldcode == EOF)	/* EOF already? */
	    return(FALSE);	/* Shouldn't happen */
	if (!putchar_decompress(priv, (char)finchar))
	    /* first code must be 8 bits = char */
	    return(FALSE);	/* Buffer overflow */
    } else {
	restarting = TRUE;
	finchar = save_finchar;
	oldcode = save_oldcode;
    }
    stackp = de_stack;
    
    while ((code = getcode(priv)) != EOF) {
	if ((code == CLEAR) && block_compress) {
	    for (code = 255; code >= 0; code--)
		tab_prefixof(code) = 0;
	    clear_flg = 1;
	    free_ent = FIRST - 1;
	    restarting = FALSE;
	    if ((code = getcode(priv)) == EOF)	/* O, untimely death! */
		break;
	}
	incode = code;
	/*
	 * Special case for KwKwK string.
	 */
	if (code >= free_ent) {
            *stackp++ = finchar;
	    code = oldcode;
	}
	
	/*
	 * Generate output characters in reverse order
	 */
	while (code >= 256) {
	    if (stackp >= ((uchar *)&tab_suffixof(HSIZE))) {
		printf("\nincode=0x%x, n_bits=%d "
		       "- compress1 stackp overflow", 
		       incode, n_bits);
		return(FALSE);
	    }
	    *stackp++ = tab_suffixof(code);
	    code = tab_prefixof(code);
	}
	if (stackp >= ((uchar *)&tab_suffixof(HSIZE))) {
	    printf("\nincode=0x%x, n_bits=%d "
		   " - compress2 stackp overflow",
		   incode, n_bits);
	    return(FALSE);
	}
	*stackp++ = finchar = tab_suffixof(code);
	
	/*
	 * And put them out in forward order
	 */
	do {
	    if (!putchar_decompress(priv, *--stackp))
		return(FALSE);	/* Buffer overflow */
	}
	while (stackp > de_stack);
	
	/*
	 * Generate the new entry.
	 */
	if (!restarting) {
	    if ((code=free_ent) < maxmaxcode ) {
		tab_prefixof(code) = (ushort)oldcode;
		tab_suffixof(code) = finchar;
		free_ent = code+1;
	    } 
	} else
	    restarting = FALSE;
	/*
	 * Remember previous code.
	 */
	oldcode = incode;
    }
    save_finchar = finchar;
    save_oldcode = oldcode;
    
    return(TRUE);		/* Success */
}






