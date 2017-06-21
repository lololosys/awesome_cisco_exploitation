/* $Id: arap_smartbuf.h,v 3.2 1995/11/17 08:38:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_smartbuf.h,v $
 *------------------------------------------------------------------
 * smartbuf.h
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_smartbuf.h,v $
 * Revision 3.2  1995/11/17  08:38:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define OUR_ARAP_SENDBUFFERSIZE	       	11200
#define OUR_ARAP_RECVBUFFERSIZE		11200

/* datagroup flags which preceed each packet */
#define SFLAG_FIXUP			0x80	/* unused */
#define SFLAG_PKTDATA			0x40	/* packet data vs. control */
#define SFLAG_TOKENIZED			0x20	/* seq # follows */
#define SFLAG_LASTGROUP			0x10	/* find group in pkt */
#define SFLAG_RANGE			0x08	/* range follows */
#define SFLAG_WANTSEQD			0x04	/* sequence range after */
#define SFLAG_WANTPKTSEQD		0x02	/* sequence packet after */
#define SFLAG_RESERVED			0x01	/* unused */

enum {
    SMARTBUF_INPUT,
    SMARTBUF_OUTPUT
};

/* --- SmartBuffering --- */

/* describe an arbitrary sequence of bytes */
typedef struct sb_s {
#if defined(sparc) && defined(notdef)
    unsigned short right[2];	/* ptr to next */
#define GET_PTR(p)	(sb_t *)((long)(p)[0] << 16 | (p)[1])
#define PUT_PTR(p, v)	((p)[0] = (u_long)(v) >> 16, (p)[1] = (unsigned short)(v))
#else
    struct sb_s *right;		/* ptr to next */
#define GET_PTR(p)	(p)
#define PUT_PTR(p, v)	(p) = (v)
#endif
    unsigned short left[2];	/* checksum on decode; */
    unsigned short seq_num;	/* seq # of this entry */
    unsigned short data_len;
} sb_t;

typedef struct ring_s {
    int size;			/* size of ring in bytes */
    unsigned int seq;		/* next seq # to assign */
    int wrapped;		/* boolean; have we wrapped around? */
    char *ring;
    sb_t *start;		/* 1st valid entry in ring */
    sb_t *last;			/* last valid entry in ring */
    sb_t *end;			/* next "free" spot */

    u_long	searches;
    u_long	scans;
    u_long	full_compares;
    u_long	hdr_compares;

    u_long	hit_pkt;
    u_long	hit_nbp_range;
    u_long	hit_nbp_token;
    u_long	hit_nbp_hdr_only;

    u_long	hit_atp_range;
    u_long	hit_atp_token;
    u_long	hit_atp_hdr_only;
} ring_t;

#define get_compact_num(p, s, n) \
    { if ((p)[0] & 0x80) { (n) = *(p)++ & 0x7f; (s)--; } \
    else { (n) = (p)[0] << 8 | (p)[1]; (p) += 2; (s) -= 2; } }

#define put_compact_num(p, s, n) \
    { if ((n) <= 127) { *(p)++ = 0x80 | (n); (s)++; } \
    else { *(p)++ = (n) >> 8; *(p)++ = (n); (s) += 2; } }

#define put_seq_num(p, s, n) \
    { *(p)++ = (n) >> 8; *(p)++ = (n); (s) += 2; }


#ifdef never
#if defined(sparc)
#define ring_size_pad(size) \
    ( ((size) & 7) ? (size) + (8 - ((size) & 7)) : (size) )
#else
#define ring_size_pad(size) \
    ( ((size) & 1) ? (size) + 1 : (size) )
#endif
#endif

#define ring_size_pad(size) \
    ( ((size) & 1) ? (size) + 1 : (size) )

#define advance_ring_ptr(s) \
    (sb_t *)( ((char *)s) + sizeof(sb_t) + ring_size_pad((s)->data_len) )


typedef struct {
    boolean	debug_sb;
} config_t;


int smartbuffer_new_ring(config_t *, ring_t **, int);
void smartbuffer_dump_ring_stats(ring_t *);
int smartbuffer_encode(config_t *, tt_soc *, unsigned char *, int,
			unsigned char *, int *);
int smartbuffer_decode(config_t *, tt_soc *, unsigned char *, int,
			unsigned char *, int *);
void smartbuffer_destroy_ring(ring_t *);

extern ring_t *get_tty_smartbufptr(tt_soc *, int);
