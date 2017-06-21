/* $Id: comp_sys.h,v 3.3 1996/01/15 19:17:45 turadek Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/wan/comp_sys.h,v $
 *------------------------------------------------------------------
 * comp_sys.h - head for system compress routines
 *
 * December 12, 1994, Steve Turadek
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * 
$Endlog$
 */

#ifndef __COMP_SYS_H__
#define __COMP_SYS_H__

extern ushort comp_stat_clks[];

extern boolean no_compression (hwidbtype *); 
extern void compress_restart (hwidbtype *);
extern void comp_bf_to_a (ulong , uchar *);
extern void compress_parser_init (void);
extern uchar compress_rand (uchar *, ushort *, uchar *source, ulong len, 
				uchar *dest, ulong *dest_len);
extern void compress_rand_fixup (uchar *, ushort *, uchar *, ulong );
extern void comp_timer (void);
extern void decompress_rand_fixup (uchar *guess_table, ushort *hash_p, uchar *source, ulong len, uchar *dest);
extern void decompress_rand (uchar *guess_table, ushort *hash_p, uchar *source, ulong len, uchar *dest, ulong *dest_len);
extern void compress_clearcounters(hwidbtype *);
extern void ppp_compression_config(hwidbtype *hwidb, boolean sense, int type);

/*
extern paktype *rand_compress (hwidbtype *, paktype *);
extern paktype *stac_compress (hwidbtype *, paktype *);
extern paktype *default_compress (hwidbtype *, paktype *);
extern paktype *rand_decompress (hwidbtype *, paktype *);
extern paktype *stac_decompress (hwidbtype *, paktype *);
extern paktype *default_decompress (hwidbtype *, paktype *);
extern void compress_initialize_rand (hwidbtype *);
extern void compress_initialize_stac (hwidbtype *);
extern void compress_initialize_default (hwidbtype *);
extern void compress_init (void);
*/

/* negative non-standard offset into "encapc" area REQUIRED by 
 * CODE REVIEW of 11/24/93.  required offset is 9 but datagramstart
 * MUST remain on an even word boundary (see CSCdi17129).
 */
#define COMP_PRED_ENCAP_OFFSET 10 /*- offset of datagramstart into encapc*/
#define COMP_STAC_ENCAP_OFFSET (LZS_DEST_MIN + 1)

#define COMP_RATIO_F 3    /*number fractional digits in ratio*/
#define COMP_RATIO_M 1000 /*fractional scale factor 10**3    */

#define HASH_MASK 0xffff
#define GUESS_TABLE_SIZE (HASH_MASK + 1)
#if HASH_MASK == 0xffff 
#define RAND_HASH(x, h) ((h)=(((h)<<4)^(x)))/* hasher for rand compressor */
#else
#define RAND_HASH(x, h) ((h)=((((h)<<4)^(x))&HASH_MASK))
#endif

/* length of compression destination buffer for packet-by-packet */
#define SCRATCH_LEN	4096

#define INLINE_RAND_COMP(b) \
	(guess_hash_p) = (&(guess_table)[(hash)]); \
	if(*(guess_hash_p) == (*(source))) \
		(flags) |= (b); \
	else { \
		(*(guess_hash_p) = (*(source))); \
		(*(dest)++) = (*(source)); \
	} \
	RAND_HASH(*(source)++, (hash));

#define INLINE_RAND_DECOMP(b) if((flags)&(b)) \
		(*(dest) = (guess_table)[(hash)]); \
	else{ \
		((guess_table)[(hash)]) = (*(source)); \
		(*(dest) = (*(source)++)); \
		((len)--); \
	} \
	RAND_HASH(*(dest)++,(hash));

#define INLINE_RAND_DECOMP_END(b) if((flags)&(b)) \
		(*(dest) = (guess_table)[(hash)]); \
	else{ \
		if(len <= 0) \
		break; \
		((guess_table)[(hash)]) = (*(source)); \
		(*(dest) = (*(source)++)); \
		((len)--); \
	} \
	RAND_HASH(*(dest)++,(hash));

typedef struct {
    uchar version;	/* version + anti expansion bit         */
/* if bit set, this pak is uncompressed but in the compression dictionary */
#define COMP_ANTI_EXPANSION 0x80 
#define COMP_NO_ANTI_EXPANSION 0x0
#define COMP_UNCOMPRESSED   0x40 /* this packet was not compressed */
#define COMP_VERSION_MASK 0x3f
/*************************************************
 *defined in "parser_defs_lapb.h"
 *#define COMP_NOT_ENABLED 0 
 *#define COMP_RAND 1
 */
    uchar xmt_paks;	/* sequence number   */
    ushort org_size;	/* uncompressed size */ 
#define COMP_SYNC_MASK  0xff
} comp_hdr_t;

#define COMPHEADERBYTES 4  /*sizeof(comp_hdr_t)*/ 

#define COMP_STAT_PERIODS_MAX 3
#define COMP_ONEMIN 0
#define COMP_5_ONEMIN 1
#define COMP_10_ONEMIN 2

/* this struct is used by dictionary-conserving schemes is compress.c
 * so that compress/decompress can be protocol-independent.  the fields
 * in this struct get set up by clients of the compress/decomp functions
 */
typedef struct comp_encaps_info_ {
    uchar	comp_hdr_len;	/* length of compression header additions... */
    uchar       encaps_overhead; /* encapsulation w/o compression overhead */
    uchar	seq_numXmt;	/* packet sequence number */
    uchar	seq_numRcv;	/* packet sequence number */
    uchar	flags;		/* same order as in comp_hdr_t */

    /* if known prior to decompress, this gets set up to correct value
     * (ex.: lapb). if not known ahead (frame-relay), set up to hwidb->maxmtu.
     * *after* decomp, set to decompressed size
     */
    ushort	org_size;
    uchar	compVersion;	/* version number of compression scheme */
} comp_encaps_info;

/* debug stuff */

#define XMT 1
#define RCV 0
#define ALLOCD  1
#define DONE  2

typedef struct buf_trace_entry_ {
  long 			fingerPrints;
  paktype	 	*bp;
} buf_trace_entry;

typedef struct buf_trace_ {
  long			index;
  buf_trace_entry	trace[1000];
} buf_trace;
/* end debug stuff */

struct compress_db_ {
	/*RAND COMPRESSION VARIABLES */
	ushort comp_xmt_hash;		/* ptr to rand xmt hash reg         */
	ushort comp_rcv_hash;		/* ptr to rand rcv hash reg         */
	uchar *comp_xmt_guess_table;    /* ptr to rand xmt guess table      */
	uchar *comp_rcv_guess_table;	/* ptr to rand rcv guess table      */
	/*COMMON COMPRESSION VARIABLES */
	ulong comp_xmt_in_bytes;			/*xmt input's byte*/
	ulong comp_rcv_out_bytes;			/*rcv out byte*/
	/*current for delta mod2 comps in show_compress() */
	ulong comp_xmt_in_interv[COMP_STAT_PERIODS_MAX];/*last xmt in*/
	ulong comp_tx_interv[COMP_STAT_PERIODS_MAX];	/*last xmt out*/
	ulong comp_rcv_out_interv[COMP_STAT_PERIODS_MAX];/*last rcv out*/
	ulong comp_rx_interv[COMP_STAT_PERIODS_MAX];	/*last rcv in*/
	/*delayed for current - delta mod2 comps in show_compress() */
	ulong comp_xmt_in_interv_l[COMP_STAT_PERIODS_MAX];/*last xmt in*/
	ulong comp_tx_interv_l[COMP_STAT_PERIODS_MAX];	/*last xmt out*/
	ulong comp_rcv_out_interv_l[COMP_STAT_PERIODS_MAX];/*last rcv out*/
	ulong comp_rx_interv_l[COMP_STAT_PERIODS_MAX];	/*last rcv in*/

	ulong comp_rcv_no_bufs;		/* recv no buffer cnt  */
	ulong comp_xmt_no_bufs;		/* xmt no buffer cnt   */
	ulong comp_restarts;		/* # restarts          */
	ushort comp_next_clk[COMP_STAT_PERIODS_MAX];  /*clk # comp_timer()*/
	ushort comp_xmt_sync;		/* xmt sync cnt        */
	ushort comp_rcv_sync;		/* rcv sync cntg       */
	uchar comp_configed;		/* compression configured & type*/
	uchar *stac_comp_history;	/* compress history */
	uchar *stac_decomp_history;	/* decompress history */
	uchar *pak_scratch;		/* compress work space */
	ulong uncompressed_sent;
	ulong uncompressed_rcvd;
	ulong compressed_sent;
	ulong compressed_rcvd;
	long vc_contexts;		/* number of vcs using this context */
}; 

#endif __COMP_SYS_H__
