/* $Id: svip_compress_lzw.c,v 1.1.6.1 1996/07/08 22:43:01 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_compress_lzw.c,v $
 *------------------------------------------------------------------
 * svip_compress_lzw.c
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_compress_lzw.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:01  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:56  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "compress_lzw.h"
#include "dbus.h"
#include "dbstatus.h"
#include "command.h"

/* local variables */
static uchar * circ_queue;      /* ptr to circular queue.  Characters
				   are copied here from the dbus until
				   their is time to process them for 
				   decompression. */
uint comp_w_offset = 0;         /* Offset of next writable location
				   in circular queue. */
uint comp_r_offset = 0;          /* Offset of next readable char in 
				    circular queue */
static uint bytes_received = 0;  /* bytes received from rp */
static uint comp_bytes_used = 0; /* # comp bytes used */
static uint total_length = 0;    /* total # of compressed bytes */
static compress_private *comp_priv;   /* pointer to compression structure */


/* local prototypes */
static void cl_hash(compress_private *, register long);
static void init_compress(compress_private *);
static int getcode(compress_private *);
static boolean decompress(compress_private *);
static void config_decompress (void);
static void put_byte_comp(uchar);
static inline boolean putchar_decompress(compress_private *, uchar,
					 dbus_globals *);
static void local_bzero (uchar *, ulong);

static uint check_dbus_ints(boolean flag)
{
    return(KSEG1_IO_ASIC->nev_isr & NEV_ISR_DBUS_INTR);
}

/*
 * Routine called from image_transfer() after the 
 * header for the image transfer has been received.
 * Starts decompression.
 * Returns when image data transfer is complete
 * or when an error occurs.
 *  retval:  =  0 ok
 *           = -1  error
 */
void decomp_image_mode (void) 
{
    if(total_length != 0) {
	wcs_parity_error();
	return;
    }
	
    total_length = dbuscore_p->db.wh.length;
    comp_bytes_used = 0;

    config_decompress();
    dbuscore_p->db.wh.length = dbuscore_p->db.wcs_data_count = total_length;
    total_length = 0;

    return;
}


/*  
 *  Initializes compression tables and checks the first two 
 *  characters.  If all is OK, it calls decompress() to do the
 *  the real work.
 *
 */
static void config_decompress (void)
{

    comp_priv = (compress_private *) (END_OF_HASH_ADDR - sizeof(*comp_priv) - 0x100);

    /* Set up queue for input data (leave a little slack) */
    circ_queue = (uchar *)comp_priv - total_length - 0x100;

    init_compress(comp_priv); 
    
    /*
     * Check the first two chars to be sure this is
     * the type of image we expect.
     */
    if ((get_byte_comp() !=  MAGIC_1) 
	|| (get_byte_comp() !=  MAGIC_2)) {
	wcs_parity_error();
	return;
    }

    comp_priv->cp_maxbits = get_byte_comp();	/* set -b from file */

    comp_priv->cp_block_compress = comp_priv->cp_maxbits & BLOCK_MASK;
    comp_priv->cp_maxbits &= BIT_MASK;
    comp_priv->cp_maxmaxcode = 1L << comp_priv->cp_maxbits;
    if(comp_priv->cp_maxbits > BITS) {
	return;
    }
    {
	register int code;
	/*
	 * Initialize the first 256 entries in the table.
	 */
	comp_priv->cp_maxcode = MAXCODE(comp_priv->cp_n_bits = INIT_BITS);
	for ( code = 255; code >= 0; code-- ) {
	    comp_priv->cp_codetab[code] = 0;
	    ((uchar *)(comp_priv->cp_htab))[code] = (uchar)code;
	    if(code % 8 == 0) {
		check_for_data();
	    }
	}
	comp_priv->cp_free_ent = 
	    ((comp_priv->cp_block_compress) ? FIRST : NO_ABSOLUTE_CODES);
    }
    
    comp_priv->cp_outputcount = 0;
    /*
     * Call decompress to do the real work.
     */
    if (!decompress(comp_priv)) {
	return;
    }

    return;
}

/*
 * Decompress from the circular queue to ram.  This routine adapts to
 * the codes in the file building the "string" table on-the-fly;
 * requiring no table to be stored in the compressed file. 
 *
 * This returns TRUE if the data is successfully decompressed, FALSE
 * if a buffer or stack overflow occured. 
 */
static boolean decompress (compress_private *priv)
{
    register  uchar *stackp; 
    register int finchar;
    register int code, oldcode, incode;
    register int restarting;
    int i;
    dbus_globals local_db;

    local_db.wcs_head_count = 0;
    local_db.wstate = image_mode;
    local_db.wcs_data_count = 0;
    local_db.wcs_check_count = 0;
    local_db.cksum_add = 1;      /* start with high */
				
    if (priv->cp_in_count == 0) {
	restarting = FALSE;
	finchar = oldcode = getcode(priv);
	priv->cp_in_count++;
	if(oldcode == EOF)	/* EOF already? */
	    return(FALSE);	/* Shouldn't happen */
	if (!putchar_decompress(priv, (char)finchar, &local_db)){
	    /* first code must be 8 bits = char */
	    return(FALSE);	/* Buffer overflow */
	}
    } else {
	restarting = TRUE;
	finchar = priv->cp_save_finchar;
	oldcode = priv->cp_save_oldcode;
    }

    stackp = priv->stack;
    
    /*
     * Main loop for decompression:
     * Keep getting codes until the end code is received or an
     * error orrurs.  The possible code situations are:
     * 
     *   1. The code is the clear code, then clear the tables
     *   2. The code is greater than we have seen, special case
     *   3. The code is less than 256, Just print it.
     *   4. the code is greater than 255, lookup char and next 
     *       code in the code table.  This is a recursive lookup
     *       which could produce a very long string of characters.
     *       The string will be in reverse order.
     *   5. The ending code.
     */   
    while ((code = getcode(priv)) != EOF) {
	if ((code == CLEAR) && priv->cp_block_compress) {
	    for (code = 255; code >= 0; code--) {
		priv->cp_codetab[code] = 0;
		if(code % 8 == 0) {
		    check_for_data();
		}
		    
	    } /* for */
	    priv->cp_clear_flg = 1;
	    priv->cp_free_ent = FIRST - 1;
	    restarting = FALSE;
	    if ((code = getcode(priv)) == EOF)	/* O, untimely death! */
		break;
	} /* if */
	incode = code;
	/*
	 * Special case for KwKwK string.
	 */
	if (code >= priv->cp_free_ent) {
            *stackp++ = finchar;
	    code = oldcode;
	}
	
	/*
	 * Generate output characters in reverse order
	 */

	i = 0;
	while (code >= 256) {
	    if(i++ % 8 == 0) {
		check_for_data();
	    }
	    if (stackp >= priv->stack + STACK_SIZE) {
		return(TRUE);
	    }
	    *stackp++ =	((uchar *)(priv->cp_htab))[code];
	    code = priv->cp_codetab[code];
	}
	if (stackp >= priv->stack + STACK_SIZE) {
	    return(FALSE);
	}
	*stackp++ = finchar = ((uchar *)(priv->cp_htab))[code];
	/*
	 * And put them out in forward order
	 */

	i = 0;
	do {	   
	    if(i++ % 8 == 0) {
		check_for_data();
	    }
	    if (!putchar_decompress(priv, *--stackp, &local_db)) {
		return(FALSE);	/* Buffer overflow */
	    }
	} while (stackp > priv->stack);

	/*
	 * Generate the new entry.
	 */

	if (!restarting) {
	    if ((code = priv->cp_free_ent) < priv->cp_maxmaxcode ) {
		priv->cp_codetab[code] = (ushort)oldcode;
		((uchar *)(priv->cp_htab))[code] = finchar;
		priv->cp_free_ent = code+1;
	    } 
	} else
	    restarting = FALSE;
	/*
	 * Remember previous code.
	 */
	oldcode = incode;
    }
    priv->cp_save_finchar = finchar;
    priv->cp_save_oldcode = oldcode;
    
    return(TRUE);		/* Success */
}


/*
 * putchar_decompress - Output a decompressed character.
 *
 */
static inline boolean putchar_decompress (compress_private *priv, uchar ch,
			    dbus_globals *dg_ptr)
{
    check_for_data(); 
    
    if(image_transfer(ch, dg_ptr) == 0)
	dg_ptr->wcs_head_count = 0;
    
    if(dbuscore_p->db.r.regs.status & DBUS_ERROR_MASK) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/* 
 * Get the next compressed byte from the circular queue.  
 * If there is no byte available, loop calling check_for_data()
 * until one becomes available.
 *
 * retval : >  0 character that was read
 *          = -1 Error condition, stop transfer
 */
uint get_byte_comp ()
{
    uint retval;
    if(comp_bytes_used++ >= total_length) {
	    /* Last byte has already been transmitted */
	return(EOF);
	
    }

    if(check_for_data() == BAD_RC)
	return(BAD_RC);

    while (comp_r_offset == comp_w_offset) {
	/* There are no characters to read */
	if(check_for_data() == BAD_RC)
	    return(BAD_RC);
    }
    retval = *(circ_queue + comp_r_offset);
    comp_r_offset++;
    return(retval);
}

/* 
 * Check the dbus to see if there is data available.  If it is for 
 * us, then call put_byte_comp()
 * retval:  BAD_RC -- We received a command we did not expect, leave
 *          0      -- No new data was added to the circular queue
 *        >= 1     -- # of bytes added to the circular queue.
 */
int check_for_data (void)
{
    int retval = 0;   /* Initialize to no new data added to cirq */

    while(check_dbus_ints(TRUE)) {
	*((ushort *)&dbuscore_p->db.data_port)
	    = *((ushort *)&db_hw->data_port); /* Get dbus command from
		Hardware.  Cast is necessary to keep the
		compiler from calling memcpy to copy
		contents of the data_port structure. */
					    
	if((dbuscore_p->db.data_port.ad.addrs &  PORT_DBUS_READ) &&
	   !(dbuscore_p->db.data_port.ad.addrs & PORT_CMD_FAIL)) {
	    write_data(dbuscore_p->db.r.r_array[dbus_addrs(dbuscore_p->db.data_port.ad.addrs)]);
	    continue;

	}

	
	if( dbuscore_p->db.data_port.ad.addrs & PORT_CMD_FAIL) {
	    /* The hardware already reported an internal error for us so
	       we should not respond to this request. We should clear the
	       error indication so that we have a chance to process
	       the next request */
	    read_status();    /* indicate that we have seen the error
			       */
	    internal_error();
	    return(BAD_RC);  /* Let's get out of here */
	}
	
	if((dbuscore_p->db.data_port.ad.addrs & PORT_DBUS_ADDRS) !=
	   DB_MICROCODE) {
	    /* either this was not for register 7, or it was a read */
	    internal_error();  /* signal error */
	    return (BAD_RC);   /* get out */
	} 

	dbuscore_p->db.r.regs.cmd = dbuscore_p->db.data_port.dbus_data;
	put_byte_comp(dbuscore_p->db.data_port.dbus_data);
	write_status();  	    
	retval++;  /* Data added to circular queue */
	
    }  /* repeat as long as there are chars from the DBUS */

    return(retval);   /* No new data on circular queue */
}

/*
 *    Put compressed byte into the circular queue from the RP.
 *    Takes byte from RP and places it in circular Buffer.  The byte
 *    is compressed if compression was on in the header of the image.
 *    retval:  void
 *    If the queue is full, we just drop the character
 */
static void put_byte_comp (uchar ch)
{
    
    *(circ_queue + comp_w_offset) = ch;
    comp_w_offset++;
    bytes_received++;
}

/*
 * Get the next compression code from the compressed data.
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


/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 * 	stdin
 * Outputs:
 * 	code or EOF is returned.
 */
int getcode (compress_private *priv)
{
    register int code;
    register int r_off, bits;
    register uchar *bp = priv->cp_buf;
    
    if (priv->cp_clear_flg > 0 || priv->cp_offset >= priv->cp_size ||priv->cp_free_ent > priv->cp_maxcode) {
	/*
	 * If the next entry will be too big for the current code
	 * size, then we must increase the size.  This implies reading
	 * a new buffer full, too.
	 */
	if (priv->cp_free_ent > priv->cp_maxcode) {
	   priv->cp_n_bits++;
	    if (priv->cp_n_bits == priv->cp_maxbits)
		priv->cp_maxcode =priv->cp_maxmaxcode;	/* won't get any bigger now */
	    else
		priv->cp_maxcode = MAXCODE(priv->cp_n_bits);
	}
	if (priv->cp_clear_flg > 0) {
    	    priv->cp_maxcode = MAXCODE (priv->cp_n_bits = INIT_BITS);
	    priv->cp_clear_flg = 0;
	}
	{
	    int c;
	    int count = priv->cp_n_bits;
	    register uchar *bpx = priv->cp_buf;
	    
	    priv->cp_size = 0;
	    while (count--) {
		c = get_byte_comp();
		if (c == EOF)
		    break;
		*bpx++ = c;
		priv->cp_size++;
	    }
	}
	if (priv->cp_size <= 0)
	    return EOF;			/* end of file */
	priv->cp_offset = 0;
	/* Round size down to integral number of codes */
	priv->cp_size = (priv->cp_size << 3) - (priv->cp_n_bits - 1);
    }

    r_off = priv->cp_offset;
    bits = priv->cp_n_bits;
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
    code |= (*bp & priv->cp_rmask[bits]) << r_off;
    priv->cp_offset += priv->cp_n_bits;

    return code;
}


/*
 * Initialize the compression tables.
 */
static void init_compress (compress_private *priv)
{
    /*
     * perform global data initializations.
     * Normally, this would be done at bss clear time, but this
     * code runs before bss is set up.
     */
    reset_buffer();
    bytes_received = 0;
    check_for_data();
    priv->cp_outputcount = 0;
    priv->cp_n_bits = 0;		/* number of bits/code */
    priv->cp_last_output_nbits = 0;	/* last known bits/code */
    priv->cp_need_output_clear = 0;	/* need to dump a CLEAR code */
    priv->cp_maxbits = BITS;		/* user settable max # bits/code */
    priv->cp_maxcode = 0;		/* maximum code, given n_bits */
    priv->cp_maxmaxcode = 1L << BITS;	/* should NEVER generate this code */
    priv->cp_hsize = HSIZE;
    priv->cp_free_ent = 0;		/* first unused entry */
    priv->cp_save_finchar = 0;
    priv->cp_save_oldcode = 0;
    priv->cp_exit_stat = 0;
    priv->cp_block_compress = BLOCK_MASK;
    priv->cp_clear_flg = 0;

    check_for_data();
    priv->cp_rmask[0] = 0x00;
    priv->cp_rmask[1] = 0x01;
    priv->cp_rmask[2] = 0x03;
    priv->cp_rmask[3] = 0x07;
    priv->cp_rmask[4] = 0x0f;
    priv->cp_rmask[5] = 0x1f;
    priv->cp_rmask[6] = 0x3f;
    priv->cp_rmask[7] = 0x7f;
    priv->cp_rmask[8] = 0xff;
    
    check_for_data();
    priv->cp_lmask[0] = 0xff;
    priv->cp_lmask[1] = 0xfe;
    priv->cp_lmask[2] = 0xfc;
    priv->cp_lmask[3] = 0xf8;
    priv->cp_lmask[4] = 0xf0;
    priv->cp_lmask[5] = 0xe0;
    priv->cp_lmask[6] = 0xc0;
    priv->cp_lmask[7] = 0x80;
    priv->cp_lmask[8] = 0x00;
    
    check_for_data();
    priv->cp_offset = 0;
    priv->cp_size = 0;
    priv->cp_in_count = 0;	   /* length of input */
    priv->cp_bytes_out = 0;	   /* length of compressed output */
    priv->cp_out_count = 0;	   /* # of codes output (for debugging) */
    priv->cp_ratio = 0;
    priv->cp_checkpoint = CHECK_GAP;
    check_for_data();
    local_bzero(priv->cp_buf, sizeof(priv->cp_buf));
    cl_hash(priv, (long int)priv->cp_hsize);	/* clear hash table */
    priv->initialized = 1;
}



/* 
 * Reset code table.
 */
static void cl_hash (compress_private *priv, register long Hsize)
{

    register int *htab_p = (int *)priv->cp_htab;
    register ushort *codetab_p = (ushort *)priv->cp_codetab;
    register long i;
    register int m1 = -1;
    
    /* clear the hash table, 16 words for each loop */
    for(i = 0; i < (1 << BITS); i+=16, htab_p +=16){
	check_for_data();
	*(htab_p) = m1;
	*(htab_p+1) = m1;
	*(htab_p+2) = m1;
	*(htab_p+3) = m1;
	*(htab_p+4) = m1;
	*(htab_p+5) = m1;
	*(htab_p+6) = m1;
	*(htab_p+7) = m1;
	check_for_data();
	*(htab_p+8) = m1;
	*(htab_p+9) = m1;
	*(htab_p+10) = m1;
	*(htab_p+11) = m1;
	*(htab_p+12) = m1;
	*(htab_p+13) = m1;
	*(htab_p+14) = m1;
	*(htab_p+15) = m1;
    }

    check_for_data();
    /* Clear the last 1 to 15 words of the hash table */
    for( ; i < (1 << BITS); i++){
	check_for_data();
	*htab_p++ = m1;
    }

    /* Clear the code table in groups of 16 half words */
    for(i = 0; i < HSIZE; i += 16, codetab_p += 16) {
	check_for_data();
	*(codetab_p) = 0;
	*(codetab_p+1) = 0;
	*(codetab_p+2) = 0;
	*(codetab_p+3) = 0;
	*(codetab_p+4) = 0;
	*(codetab_p+5) = 0;
	*(codetab_p+6) = 0;
	*(codetab_p+7) = 0;
	*(codetab_p+8) = 0;
	check_for_data();
	*(codetab_p+9) = 0;
	*(codetab_p+10) = 0;
	*(codetab_p+11) = 0;
	*(codetab_p+12) = 0;
	*(codetab_p+13) = 0;
	*(codetab_p+14) = 0;
	*(codetab_p+15) = 0;
    }
    check_for_data();

    /* Clear the last 1 to 15 words of the code table */
    for( ; i < HSIZE; i++){
	check_for_data();
	*codetab_p++ = 0;
    }
}

/*
 * Sets memory to zero and periodically checks for data
 * from the dbus.
 */
static void local_bzero (uchar * mem, ulong size)
{

    ulong *word_ptr;
    uchar *byte_ptr;


    for(word_ptr = (ulong *) mem;
	( (ulong) (word_ptr + 15) < (ulong) (mem + size));
	word_ptr += 16) {
	check_for_data();
	*(word_ptr) = 0;
	*(word_ptr+1) = 0;
	*(word_ptr+2) = 0;
	*(word_ptr+3) = 0;
	*(word_ptr+4) = 0;
	*(word_ptr+5) = 0;
	*(word_ptr+6) = 0;
	check_for_data();
	*(word_ptr+7) = 0;
	*(word_ptr+8) = 0;
	*(word_ptr+9) = 0;
	*(word_ptr+10) = 0;
	*(word_ptr+11) = 0;
	*(word_ptr+12) = 0;
	*(word_ptr+13) = 0;
	*(word_ptr+14) = 0;
	*(word_ptr+15) = 0;
    }
    check_for_data();

    /* Clear the last bytes */
    for(byte_ptr = (uchar *) word_ptr ;byte_ptr < mem + size; byte_ptr++){
	*byte_ptr++ = 0;
    }
    check_for_data();
}
