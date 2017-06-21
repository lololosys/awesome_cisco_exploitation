/* $Id: compress_lzw.h,v 3.3.62.1 1996/03/18 21:29:20 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/compress_lzw.h,v $
 *------------------------------------------------------------------
 * compress_lzw.h
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
 * $Log: compress_lzw.h,v $
 * Revision 3.3.62.1  1996/03/18  21:29:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:18:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  16:38:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:46:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Defines for third byte of header */
#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80

#define BITS            12 
#define HSIZE	        5003  		/* 80% occupancy */ 

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
    int cp_maxbits;			/* user settable max # bits/code */
    int cp_last_output_nbits;		/* compress: last known bits/code */
    int cp_maxcode;		        /* maximum code, given n_bits */
    int cp_maxmaxcode;		        /* should NEVER generate this code */
    int cp_htab [HSIZE];
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
} compress_private;


/* magic number in header */
#define MAGIC_1  037
#define MAGIC_2  0235

#define CHECK_GAP 500	/* ratio check interval */
#define DUMP_ALL -2     /* flag to dump all codes */

/* macro access to every field of the private structure */
#define n_bits            (priv->cp_n_bits)
#define last_output_nbits (priv->cp_last_output_nbits)
#define need_output_clear (priv->cp_need_output_clear)
#define maxbits           (priv->cp_maxbits)
#define maxcode           (priv->cp_maxcode)
#define maxmaxcode        (priv->cp_maxmaxcode)
#define htab              (priv->cp_htab)
#define codetab           (priv->cp_codetab)
#define free_ent          (priv->cp_free_ent)
#define save_finchar      (priv->cp_save_finchar)
#define save_oldcode      (priv->cp_save_oldcode)
#define exit_stat         (priv->cp_exit_stat)
#define block_compress    (priv->cp_block_compress)
#define clear_flg         (priv->cp_clear_flg)
#define rmask             (priv->cp_rmask)
#define lmask             (priv->cp_lmask)
#define offset            (priv->cp_offset)
#define size              (priv->cp_size)
#define buf               (priv->cp_buf)
#define inputbuf          (priv->cp_inputbuf)
#define inputbufend       (priv->cp_inputbufend)
#define outputbuf         (priv->cp_outputbuf)
#define outputbufend      (priv->cp_outputbufend)
#define outputcount       (priv->cp_outputcount)
#define in_count          (priv->cp_in_count)
#define bytes_out         (priv->cp_bytes_out)
#define out_count         (priv->cp_out_count)
#define ratio             (priv->cp_ratio)
#define checkpoint        (priv->cp_checkpoint)
#define hsize             (priv->cp_hsize)

#define MAXCODE(n_bits)	  ((1L << (n_bits)) - 1)

#define EOF               (-1)

/*
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define htabof(i)	htab[i]
#define codetabof(i)	codetab[i]
#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((uchar *)(htab))[i]
#define de_stack	((uchar *)&tab_suffixof(1<<BITS))

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */ 
#define FIRST	257	/* first free entry */
#define	CLEAR	256	/* table clear output code */

/* Prototypes */

int getchar_compress(compress_private *);
boolean putchar_compress(compress_private *, uchar);
boolean putstring_compress(compress_private *, uchar *, int);
int getchar_decompress(compress_private *);
boolean putchar_decompress(compress_private *, char);
boolean compress(compress_private *);
boolean clear_output(compress_private *);
void init_compress(compress_private *);
boolean decompress(compress_private *);






