/* $Id: compress_lzw.h,v 3.4 1996/02/09 00:01:47 tkam Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/compress_lzw.h,v $
 *------------------------------------------------------------------
 * compress_lzw.h
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: compress_lzw.h,v $
 * Revision 3.4  1996/02/09  00:01:47  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.3  1996/01/17  23:29:48  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:13  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COMPRESS_LZW_H__
#define __COMPRESS_LZW_H__

#include "dbus.h"
#include "queue.h"

/* Defines for third byte of header */
#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80

#if 1
/* Defines for 16 bit has table.  This requires 69k for hash entries. */
#define BITS            16 
#define HSIZE	        69001
 		
#else
/* Use the following definitions for a 12 bit hash encoding.  This
   would require only 5k of hash entries, not 69k. */
#define BITS            12
#define HSIZE           5003
#endif 

#define STACK_SIZE 8000
#define COMPQ_SIZE  0x10000  /* define 64K of work space for Queue */
#define FLOW_BUFFER 20000 /* define flow control buffer */
#define BAD_RC       -1
#define END_OF_HASH_ADDR 0x80800000


#define INIT_BITS       9		/* initial number of bits/code */

/* data structure which will be overlaid ontop of bss */
typedef struct compress_private_def {
    int	initialized;
    uchar *cp_inputbuf;
    uchar *cp_inputbufend;
    uchar *cp_outputbuf;
    uchar *cp_outputbufend;
    int cp_outputcount;
    int cp_n_bits;			/* number of bits/code */
    int cp_maxbits ;			/* user settable max # bits/code */
    int cp_last_output_nbits;		/* compress: last known bits/code */
    int cp_maxcode;		        /* maximum code, given n_bits */
    int cp_maxmaxcode;		        /* should NEVER generate this code */
    int cp_htab [1<<BITS];
    uchar stack[STACK_SIZE];
    ushort cp_codetab [HSIZE];
    int cp_free_ent;     		/* first unused entry */
    int cp_save_finchar;		/* decompress: previous finchar */
    int cp_save_oldcode;		/* decompress: previous oldcode */
    int cp_exit_stat;
    int cp_block_compress;
    int cp_clear_flg;
    int cp_need_output_clear;		/* compress: output a CLEAR next */
    uchar cp_rmask[9];
    uchar cp_lmask[9];
    int cp_offset, cp_size;
    uchar cp_buf[BITS];
    long int cp_in_count;		/* length of input */
    long int cp_bytes_out;		/* length of compressed output */
    long int cp_out_count;		/* # of codes output (for debugging) */
    long int cp_ratio;
    long int cp_checkpoint;
    long int cp_hsize;			/* for dynamic table sizing */
    uchar priv_queue[COMPQ_SIZE];
} compress_private;


/* magic number in header */
#define MAGIC_1  037
#define MAGIC_2  0235
#define MAGIC_NUMBERS  0x1025


#define CHECK_GAP 500	/* ratio check interval */
#define DUMP_ALL -2     /* flag to dump all codes */

/* macro access to every field of the private structure */

#define MAXCODE(n_bits)	  ((1L << (n_bits)) - 1)

#define EOF               (-1)


/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */ 
#define FIRST	257	/* first free entry */
#define	CLEAR	256	/* table clear output code */
#define NO_ABSOLUTE_CODES  256

/* Macro-ize bzero, since the bzero code might be compressed */
#define bzero(p,l) { \
	char * P = (char *)(p); \
	int L=(l); \
	while(L--) *P++='\0'; \
}

/* external variable declares */
extern uint comp_w_offset;
extern uint comp_r_offset;


/* inlines */


/*
 * I/O routine emulation
 */

/*
 * getchar_decom press - Get a byte for decompress
 */

static inline uint getchar_decompress (compress_private *priv)
{
    
    
    extern uint get_byte_comp(void);

    return(get_byte_comp());
}

/* 
 * Reset cicular queue offsets to zero.
 */
static inline void reset_buffer (void)
{
    comp_w_offset = comp_r_offset = 0;
}


/* Prototypes for functions used in other files */

extern int check_for_data(void);

#endif __COMPRESS_LZW_H__
