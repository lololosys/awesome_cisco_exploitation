/* $Id: arap_smartbuf.c,v 3.2.60.2 1996/08/02 22:52:28 smackie Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap_smartbuf.c,v $
 *------------------------------------------------------------------
 * smartbuf.c - AppleTalk Remote Access SmartBuffering code
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *         "SmartBuffering" - simple tokenizing of large sections of
 *         data inside a specific packet.  Smartbuffering is only
 *         applied one packet at a time and is very specific to
 *         Appletalk. (though, it could be easily extended to other
 *         protocols)
 *
 *          Note that the structure of the ring buffer must be
 *         identical on the client and server side.  This means that
 *         the packet header used in the ring buffer must be 12 bytes
 *         on any server in order to keep the ring buffer wrapping in
 *         sync.
 *         
 *         See "AppleTalk Remote Access Protocol, External Reference
 *         Specification"
 * ENVIRONMENT:
 *         Unix
 * MANIFEST:
 *         smartbuffer_new_ring()
 *         smartbuffer_dump_ring_stats()
 *         smartbuffer_encode()
 *         smartbuffer_decode()
 *------------------------------------------------------------------
 * $Log: arap_smartbuf.c,v $
 * Revision 3.2.60.2  1996/08/02  22:52:28  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  18:50:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:27:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:31:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:38:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:54:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:05:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define PRIVATE static 
typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned long u_int32;

#include "master.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "../h/ttysrv.h"
#include "arap_smartbuf.h"
#include "arap_public.h"

static unsigned short csum(unsigned char *, int, int);
static int find_sequence_of_bytes(ring_t *, unsigned char *, int, int,
                        sb_t **, sb_t **, int *, int *, int);
static sb_t *find_sequence_number(ring_t *, int);
static void dump_sequence_numbers(ring_t *);
static void make_new_sequence(ring_t *, unsigned char *, int, int, int);


#define SB_DEBUG(s)	if (conf->debug_sb) buginf s

int arapinternal_debug = 0;

/***********************************************************************
 * NAME:
 *         smartbuffer_new_ring()
 * ABSTRACT:
 *         allocate a new smartbuffer ring for one side of the decode.
 *         does all initialization of ring contents.
 * INPUT:
 *         conf - ptr to session config struct
 *         pring - ptr to ring ptr
 *         size - size of ring in bytes
 * OUTPUT:
 *         fills in pring; returns 0 if successful
\************************************************************************/

int
smartbuffer_new_ring (
    config_t *conf,
    ring_t **pring,
    int size)
{

    *pring = malloc(sizeof(ring_t));
    if (*pring) {
	memset(*pring, 0, sizeof(ring_t));
	(*pring)->size = size;
	(*pring)->ring = malloc(size);
	if (!(*pring)->ring) {
		free(*pring);
		return(-1);
	}
	memset((*pring)->ring, 0, size);
	(*pring)->start = (*pring)->end = (sb_t *)(*pring)->ring;
	return 0;
    }

    return -1;
}

/*
 * NAME:
 *	   smartbuffer_destroy_ring()
 */

void
smartbuffer_destroy_ring(ring_t *pring)
{
	free(pring->ring);
	free(pring);
}

/***********************************************************************
 * NAME:
 *         csum(b, offset, s)
 * ABSTRACT:
 *         simple checksum use for ring comparison
 * INPUT:
 *         b - ptr to data
 *         offset - offset into data to use
 *         s - size of data in bytes
 * OUTPUT:
 *         returns 16 bit unsigned checksum
\************************************************************************/

PRIVATE unsigned short
csum (unsigned char *b, int offset, int s)
{
    unsigned short sum = 0;

    b += offset;

    while (s--)
	sum += *b++;

    return sum;
}

/***********************************************************************
 * NAME:
 *         find_sequence_of_bytes(ring, buffer, size, offset,
 *		       phit1, phit2, prange_start, prange_len)
 * ABSTRACT:
 *         Search the entire ring, noting the following matches:
 *         
 *         - entire packet
 *         - ddp header
 *         - data + offset (2 bytes (nbp) or 8 bytes (atp))
 *         
 *         Return a bitmap + a range
 *         	bitmap
 *         		entire packet
 *         		ddp header
 *         		offset
 *         	start range
 *         	len range
 *         
 *         Use checksums to speedup for 
 *         	entire packet - [0]
 *         	offset packet (data + 8 bytes) - [1]
 *         
 *         	if offset checksum matches,
 *         	check data+2..data+7 for nbp special case (6 bytes, 3 words)
 *
 *         Note that since there is virtually no spec for this, some of
 *         this is empiricle/devined/thats-how-I-think-it-works.
 *
 *         The look code tries to hang on to the "best" matches, i.e. those
 *         that generate the fewest output bytes, namely full token matches.
 *
 *         It also goes to great lengths not to pick tokens which are
 *         near the edges of the ring.  It makes sense that if the ring
 *         is full (i.e. it has wrapped), the picking a entry at the "end"
 *         of the ring is wrong, as any newly created entries will overwrite
 *         this entry (so we avoid this).  It's not so obvious why picking
 *         entries created with the last packet (i.e. the last 3 entries) in
 *         the ring causes failure, but it does (so we avoid this also).
 *
 *         I would really like to get a look at Apple's code... (or a more
 *         complete specification)
 *
 * INPUT:
 *         ring_t *ring			- ptr to ring to search
 *         unsigned char *buffer 	- buffer to search for
 *         int size                     - size of buffer in bytes
 *         int offset                   - offset of non-protocol data
 *         sb_t **phit1;                - ptr to full/pkt or hdr hit sb entry
 *         sb_t **phit2;                - ptr to data hit sb entry
 *         int *prange_start;           - ptr to offset start (see below)
 *         int *prange_len;             - ptr to offset length
 * OUTPUT:
 *         returns bitmap of "flags" indicating what hits where made;
 *         if hits found, phit1, phit2, prange_start, prange_end will
 *         all be filled in, depending on the flags set.
 *         
 *         HIT_PACKET	- full packet hit, phit1 -> sb entry of packet
 *         HIT_DDP_HDR	- ddp hdr hit, phit1 -> sb entry of hdr
 *         HIT_OFFSET	- data offset hit, phit2 -> sb entry of data
 *         S1_RANGE     - phit1 is a "range" (not entire token)
 *         S2_RANGE	- phit2 is a "range" (not entire token)
 *
 *         note that S1_RANGE & S2_RANGE are exclusive
\************************************************************************/

PRIVATE int
find_sequence_of_bytes (
    ring_t *ring,
    unsigned char *buffer,
    int size,
    int offset,
    sb_t **phit1,
    sb_t **phit2,
    int *prange_start,
    int *prange_len,
    int ttynum)
{
    sb_t *s, *start;
    sb_t *last_full = 0, *last_hdr = 0, *last_offset = 0;
    int flag = 0;
#define HIT_PACKET	1
#define HIT_DDP_HDR	2
#define HIT_OFFSET	4
#define S1_RANGE	0x10
#define S2_RANGE	0x20
    int skip;
    unsigned short checksum;

    /* calc checksum of entire buffer */
    checksum = csum(buffer, 0, size);

    /*
     * skip the initial "n" entries as they might be deleted when we add
     * matching sequences...
     */
    if (ring->wrapped) {
	/* this makes sense (a bit conservative, but safe) */
	skip = (604 + sizeof(sb_t)) * 6;
	for (start = ring->start; skip > 0 && start;
	     start = GET_PTR(start->right))
	{
	    skip -= sizeof(sb_t) + ring_size_pad(start->data_len);
	}
    } else
	start = ring->start;

    ring->scans++;
    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	buginf("ARAP SMARTBUF TTY %t: search...\n", ttynum);

    *phit1 = *phit2 = 0;

    /* scan the ring... */
    for (s = start; s; s = GET_PTR(s->right))
    {
	sb_t *sn;

	/* Don't use either the last three (3) entries in the ring */

	/*
	 * (I don't know why; Apparently this is some constraint in
	 * Apple's code.  It took me 2 weeks to figure this out)
	 */
	if ((sn = GET_PTR(s->right))) {
	    if ((sn = GET_PTR(sn->right)))
		if (GET_PTR(sn->right) == 0)
		    break;
	}

	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	    buginf("%d ", s->seq_num);

	ring->full_compares++;

	/* exact matches; any chance of matching? */
	if (checksum == s->left[0] &&
	    bcmp((char *)(s + 1), buffer, size) == 0) 
	{
	    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
		buginf("full; ");
	    last_full = s;
	    flag |= HIT_PACKET;
	    continue;
	}

	ring->hdr_compares++;

	/* match 1st 16 bytes? (ddp header) */
	if (bcmp((char *)(s + 1), buffer, 16) == 0)
	{
	    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
		buginf("ddp; ");

	    /* try to hang onto full token ddp hdr hits */
	    /* (i.e. only replace if nothing there, or it's a range
	       or it's a ddp hdr entry) */
	    if (last_hdr == 0 || (flag & S1_RANGE) || s->data_len == 16)
	    {
		last_hdr = s;
		flag |= HIT_DDP_HDR;

		/* hdr only? (i.e. entire token or just hdr?) */
		if (s->data_len != 16)
		    flag |= S1_RANGE;
		else
		    flag &= ~S1_RANGE;
	    }
	}

	/* move on if packet has no data */
	if (offset == 0 || size <= offset)
	    continue;

	/* match offset as entire token? (data) */
	if (bcmp((char *)(s + 1), buffer + offset, size - offset) == 0)
	{
	    if (arapinternal_debug)
		buginf("offset; ");

	    /* try to hang onto full token hits */
	    if (last_offset == 0 || (flag & S2_RANGE) ||
		s->data_len == *prange_len)
	    {
		last_offset = s;
		flag |= HIT_OFFSET;

		*prange_start = 0;
		*prange_len = size - offset;

		/* covers entire token? (or just a range) */
		if (*prange_len != s->data_len)
		    flag |= S2_RANGE;
		else
		    flag &= ~S2_RANGE;
	    }
	}

	/* match offset as offset? (data) */
	if (bcmp((char *)(s + 1) + offset, buffer + offset,
		   size - offset) == 0)
	{
	    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
		buginf("trailing; ");

	    /* try to hang onto full token hits */
	    if (last_offset == 0 || (flag & S2_RANGE) ||
		s->data_len == *prange_len)
	    {
		last_offset = s;
		flag |= HIT_OFFSET;

		*prange_start = offset;
		*prange_len = size - offset;

		/* covers entire token? (or just a range) */
		if (*prange_len != s->data_len)
		    flag |= S2_RANGE;
		else
		    flag &= ~S2_RANGE;
	    }
	}
    }

    /* check for hits, preparing output vars... */

    if (flag == 0) {
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	    buginf("miss\n");
    } else {
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	    buginf("\n");
#define DO_FULL_PKT
#ifdef DO_FULL_PKT
	if (flag & HIT_PACKET)
	    *phit1 = last_full;
	else
#else
	flag &= ~HIT_PACKET;
#endif
	{
	    /* no full packet, see if we hit the ddp hdr */
	    if (flag & HIT_DDP_HDR) {
		*phit1 = last_hdr;

		/* got ddp hdr, see if we got data also */
		if (flag & HIT_OFFSET)
		    *phit2 = last_offset;
	    } else
		flag = 0;
	}
    }

    return flag;
}


/***********************************************************************
 * NAME:
 *         find_sequence_number()
 * ABSTRACT:
 *         search the ring, finding a specifc entry by seq #
 * INPUT:
 *         ring - ptr to ring header
 *         seq - sequence # to find
 * OUTPUT:
 *         returns ptr to sb entry with matching seq # or null
\************************************************************************/

PRIVATE sb_t *
find_sequence_number (ring_t *ring, int seq)
{
    sb_t *s;

    for (s = ring->start; s; s = GET_PTR(s->right))
	if (s->seq_num == seq)
	    return s;

    return 0;
}

/***********************************************************************
 * NAME:
 *         dump_sequence_number()
 * ABSTRACT:
 *         output seq #'s in a ring; useful when debugging
 * INPUT:
 *         ring - ptr to ring header
 * OUTPUT:
 *         none
\************************************************************************/

PRIVATE void
dump_sequence_numbers (ring_t *ring)
{
    sb_t *s;
    int i;

    for (i = 0, s = ring->start; s; s = GET_PTR(s->right), i++) {
	buginf("%d, ", s->seq_num);
	if (i++ > 20) {
	    i = 0;
	    buginf("\n");
	}
    }
}

/***********************************************************************
 * NAME:
 *         smartbuffer_dump_ring_stats(ring)
 * ABSTRACT:
 *         send ring statistics to the log; done after connection comes
 *         down.
 * INPUT:
 *         ring - ptr to ring header
 * OUTPUT:
 *         none
\************************************************************************/

void
smartbuffer_dump_ring_stats (ring_t *ring)
{
    if (ring->scans == 0 || ring->searches == 0)
	return;

#define rperc(member)	((ring->member * 100) / ring->searches)

    buginf("sb: scans %ld, full compares %ld, hdr compares %ld",
	   ring->scans, ring->full_compares, ring->hdr_compares);

    buginf("sb: avg full %ld, avg hdr %ld",
	   ring->full_compares/ring->scans, ring->hdr_compares/ring->scans);

    buginf("sb: searches %ld, full packet hits %ld (%d%%)",
	   ring->searches, ring->hit_pkt, rperc(hit_pkt));

    buginf("sb: nbp range %ld (%d%%), token %ld (%d%%), hdr only %ld (%d%%)",
	   ring->hit_nbp_range, rperc(hit_nbp_range),
	   ring->hit_nbp_token, rperc(hit_nbp_token),
	   ring->hit_nbp_hdr_only, rperc(hit_nbp_hdr_only));

    buginf("sb: atp range %ld (%d%%), token %ld (%d%%), hdr only %ld (%d%%)",
	   ring->hit_atp_range, rperc(hit_atp_range),
	   ring->hit_atp_token, rperc(hit_atp_token),
	   ring->hit_atp_hdr_only, rperc(hit_atp_hdr_only));
}

/***********************************************************************
 * NAME:
 *         make_new_sequence(ring, seq_ptr, seq_offset, len)
 * ABSTRACT:
 *         Create a new sb entry in the ring.  Note ring wrap (i.e. when
 *         the ring is full) and delete entries judiciously. 
 *
 *         Note that ring management must be identical on both "sides"
 *         or the master will send sequence numbers the slave has deleted.
 *         (the arap client's response to this is to drop the link)
 *
 * INPUT:
 *         ring - ptr to ring header
 *         seq_ptr - ptr to sequence of bytes
 *         seq_offet - offset into sequence (in byte) where data starts
 *         len -  size of sequence in bytes
 * OUTPUT:
 *         void (can't fail unless ring is smaller than the entry being
 *         added + sizeof(hdr))
\************************************************************************/

PRIVATE void
make_new_sequence (
    ring_t *ring,
    unsigned char *seq_ptr,
    int seq_offset,
    int len,
    int ttynum)
{
    int offset, entry_len;
    sb_t *s;

    /* get byte offset of end of ring */
    offset = (char *)ring->end - ring->ring;
    entry_len = sizeof(sb_t) + ring_size_pad(len);

    /* will this new entry fit? */
    if (offset + entry_len > ring->size) {
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	    buginf("ARAP SMARTBUF TTY %t: RING WRAP, need %d bytes\n",
		ttynum, entry_len);

	/* if start is beyond end, wrap start also...
	 *
	 * (we know that the begining of the ring is always a 
	 *  valid entry after we're wrapped once)
	 */
	if (ring->end <= ring->start)
	    ring->start = (void *)ring->ring;

	/* wrap around */
	ring->end = (void *)ring->ring;
	ring->wrapped = 1;
    }

    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	buginf("ARAP SMARTBUF TTY %t: ring end %lx, start %lx, need %d bytes\n",
	    ttynum, ring->end, ring->start, entry_len);

    /* are we in a wrapped state? (this is normal steady state) */
    if (ring->wrapped) {
	/* delete dead entries from the start */
	while (ring->end <= ring->start &&
	       (char *)ring->end + entry_len > (char *)ring->start)
	{
	    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
		buginf("ARAP SMARTBUF TTY %t: REMOVING #%d, %d bytes\n",
		    ttynum, ring->start->seq_num,
		    sizeof(sb_t) + ring_size_pad(ring->start->data_len));

	    /* end of ring ? */
	    if (GET_PTR(ring->start->right) == 0) {
		if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
		    buginf("no room\n");
		ring->start = ring->end;
		break;
	    }

	    ring->start = GET_PTR(ring->start->right);
	}
    }

    /* add to end, backfilling next ptr */
    s = ring->end;
    if (ring->last)
	PUT_PTR(ring->last->right, s);

    /* update backfill ptr */
    ring->last = s;

    PUT_PTR(s->right, 0);
    s->left[0] = csum(seq_ptr, seq_offset, len);
    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, ttynum))
	buginf("ARAP SMARTBUF TTY %t: new seq %d\n", ttynum, ring->seq);
    s->seq_num = ring->seq++;
    s->data_len = len;
    memcpy(s+1, seq_ptr + seq_offset, len);

    ring->end = advance_ring_ptr(ring->end);
}


/***********************************************************************
 * NAME:
 *         smartbuffer_encode()
 * ABSTRACT:
 *         Encode an appletalk packet using smartbuffering.
 *         
 *         Encode logic:
 *         
 *         check for entire packet match
 *             if match, send token
 *         
 *         ddp header
 *             check lap and ddp part of header (16 bytes)
 *         
 *             datagroup token range
 *             tell client to cache this range
 *             (this is so subsequent matches can just use the token,
 *             not the range)
 *         
 *         check packet type
 *         
 *         echo data
 *             always send unencoded
 *         
 *         nbp data
 *             "hone down"; function and nbp id will change, the rest won't
 *             so, send the data as a token
 *         
 *         atp data 
 *             "hone down"; skip header; only look at data 
 *         
 *         adsp data 
 *             skip header; only look at data 
 *         
 *         other
 *             match everything after ddp header
 *         
 * INPUT:
 *         config_t *conf 	- ptr to session config struct
 *         ring_t *ring 	- ptr to ring
 *         unsigned char *from  - packet to encode
 *         int size 		- size of packet in bytes
 *         unsigned char *to 	- buffer to encode into
 *         int *psize		- resulting size of encoded data
 * OUTPUT:
 *         return 0 on success
\************************************************************************/

int
smartbuffer_encode (
    config_t *conf,
    tt_soc *tty,
    unsigned char *from,
    int size,
    unsigned char *to,
    int *psize)
{
    unsigned char *orig_to = to;
    unsigned char ddp_type;
    int range_start, range_len, i, size_left = 0;
    sb_t *s1, *s2;
    unsigned char dg_flag = 0;
    int matches, offset;
    ring_t *ring = get_tty_smartbufptr(tty, SMARTBUF_OUTPUT);
    int ttynum = tty->ttynum;

    if (conf->debug_sb)
	buginf("smartbuffer_encode(%lx, %lx, %lx, %d, %lx, %lx)\n",
	       conf, ring, from, size, to, psize);

    if (conf->debug_sb > 1) {
	buginf("before smartbuffer encode:");
    }

    /* if no ring, pass raw data */
    if (ring->size == 0) {
	*to++ = dg_flag = SFLAG_PKTDATA | SFLAG_LASTGROUP;
	memcpy(to, from, size);
	*psize = size + 1;
	return 0;
    }

    /* calc offset of "offset matches" based on ddp type */
    switch (ddp_type = from[15]) {
      case 2: offset = 16+2; break;
      case 3: offset = 16+8; break;
      default: offset = 0;
    }
    
    /* search ring for matches */
    ring->searches++;

    matches = find_sequence_of_bytes(ring, from, size, offset,
			       &s1, &s2, &range_start, &range_len, ttynum);
    if (conf->debug_sb)
	buginf("matches 0x%x, s1 %lx, s2 %lx, start %d, len %d\n",
	       matches, s1, s2, range_start, range_len);

    /* entire packet in cache? */
    if (matches & HIT_PACKET) {
	SB_DEBUG(("hit on entire pkt, seq %d\n", s1->seq_num));
	ring->hit_pkt++;

	/* you would think that SFLAG_WANTPKTSEQD was the right flag
	 * here, but it's not what the client sends in this case
	 */
	*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED |
	    SFLAG_LASTGROUP | SFLAG_WANTSEQD;
	*psize = 1;
	put_seq_num(to, *psize, s1->seq_num);

	make_new_sequence(ring, from, 0, size, ttynum);

	if (conf->debug_sb > 1) {
	    buginf("after smartbuffer encode:");
	}

	return 0;
    }

    /* type lap + ddp header */
    if (matches & HIT_DDP_HDR) {
	if (matches & S1_RANGE) {
	    /* range of token */
	    SB_DEBUG(("hit on lap+ddp hdr, seq %d, range %d %d\n",
		      s1->seq_num, 0, 16));

	    *to++ = dg_flag = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_RANGE |
		SFLAG_WANTSEQD | SFLAG_WANTPKTSEQD;
	    *psize = 1;
	    put_seq_num(to, *psize, s1->seq_num);
	    put_compact_num(to, *psize, 0);
	    put_compact_num(to, *psize, 16);
	} else {
	    /* an entire token */
	    SB_DEBUG(("hit on lap+ddp hdr, seq %d, entire token\n",
		      s1->seq_num));
	    *to++ = dg_flag = SFLAG_PKTDATA | SFLAG_TOKENIZED |
		SFLAG_WANTSEQD | SFLAG_WANTPKTSEQD;
	    *psize = 1;
	    put_seq_num(to, *psize, s1->seq_num);
	}
	/* new local entry is create below (only if ddptype matches) */
    } else {
	/* did not find ddp header; send entire packet */
	if ((ddp_type = from[15]) != 4)
	    ddp_type = 0;
    }

    if (conf->debug_sb)
	buginf("ddp_type %d\n", ddp_type);

    /* type parts of the packet */
    switch (ddp_type)
    {
      case 2:			/* NBP */
	/* check nbp data */
	if (matches & HIT_OFFSET) {
	    make_new_sequence(ring, from, 0, 16, ttynum);

	    /* send nbp header as raw bytes */
	    *to++ = SFLAG_PKTDATA;
	    (*psize)++;
	    put_compact_num(to, *psize, 2);
	    *to++ = from[16];
	    *to++ = from[17];
	    (*psize) += 2;

	    /* match entire sequence or subset? */
	    if (matches & S2_RANGE) {
		/* a range of a token */
		SB_DEBUG(("hit on nbp data, seq %d, range %d %d\n",
			  s2->seq_num, range_start, range_start+range_len-1));

		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_LASTGROUP |
		    SFLAG_RANGE | SFLAG_WANTSEQD;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
		put_compact_num(to, *psize, range_start);
		put_compact_num(to, *psize, range_len);

		/* create a local entry to match on */
		make_new_sequence(ring, from, 18, size - 18, ttynum);

		ring->hit_nbp_range++;
	    } else {
		/* an entire token */
		SB_DEBUG(("hit on nbp data, seq %d, entire token\n",
			  s2->seq_num));

		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_LASTGROUP;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
		ring->hit_nbp_token++;
	    }

	    size_left = 0;
	} else {
	    dg_flag = 0;
	    to = orig_to;
	    size_left = size - 16;
	}
	break;

      case 3:			/* ATP */
	/* check atp data */
	if ((matches & HIT_OFFSET)/* && 0*/) {
	    make_new_sequence(ring, from, 0, 16, ttynum);

	    /* send atp header as raw bytes */
	    *to++ = SFLAG_PKTDATA;
	    (*psize)++;
	    put_compact_num(to, *psize, 8);
	    for (i = 0; i < 8; i++)
		*to++ = from[16 + i];
	    (*psize) += 8;

	    /* entire sequence or subset? */
	    if (matches & S2_RANGE) {
		/* a range of a token */
		SB_DEBUG(("hit on atp data, seq %d, range %d %d\n",
			  s2->seq_num, range_start, range_start+range_len-1));

		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_LASTGROUP |
		    SFLAG_RANGE | SFLAG_WANTSEQD;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
		put_compact_num(to, *psize, range_start);
		put_compact_num(to, *psize, range_len);

		/* create a local entry to match on */
		make_new_sequence(ring, from, 16+8, size-(16+8), ttynum);
		ring->hit_atp_range++;
	    } else {
		/* an entire token */
		SB_DEBUG(("hit on atp data, seq %d, entire token\n",
			  s2->seq_num));

		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_LASTGROUP;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
		ring->hit_atp_token++;
	    }

	    size_left = 0;
	} else {
#ifdef NO_PARTIAL
	    dg_flag = 0;
	    to = orig_to;
	    size_left = size - 16;
#else
	    make_new_sequence(ring, from, 0, 16, ttynum);

/*	    *to++ = SFLAG_PKTDATA | SFLAG_LASTGROUP | SFLAG_WANTSEQD; */
	    *to++ = SFLAG_PKTDATA | SFLAG_LASTGROUP;
	    (*psize)++;
	    memcpy(to, from+16, size-16);
	    *psize += size-16;
	    size_left = 0;

	    /* create a local entry to match on */
/*	    make_new_sequence(ring, from, 16, size-16); */

	    ring->hit_atp_hdr_only++;
#endif
	}
	break;

      case 4:			/* Echo */
	/* send data unchanged, no sequence */
	to = orig_to;
	*to++ = dg_flag = SFLAG_PKTDATA | SFLAG_LASTGROUP;
	memcpy(to, from, size);
	*psize = size + 1;
	size_left = 0;
	break;

      case 8:			/* ADSP */
      default:
	/* check ddp data */
	if (matches & HIT_OFFSET) {
	    SB_DEBUG(("hit on ddp data, seq %d\n", s2->seq_num));

	    make_new_sequence(ring, from, 0, 16, ttynum);

	    /* entire sequence or subset? */
	    if (matches & S2_RANGE) {
		/* a range of a token */
		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_RANGE |
		    SFLAG_WANTSEQD | SFLAG_LASTGROUP;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
		put_compact_num(to, *psize, range_start);
		put_compact_num(to, *psize, range_len);

		/* create a local entry to match on */
		make_new_sequence(ring, from, 16, size-16, ttynum);
	    } else {
		/* an entire token */
		*to++ = SFLAG_PKTDATA | SFLAG_TOKENIZED | SFLAG_LASTGROUP;
		(*psize)++;
		put_seq_num(to, *psize, s2->seq_num);
	    }

	    size_left = 0;
	} else {
	    dg_flag = 0;
	    to = orig_to;
	    size_left = size - 16;
	}
      case 0:
	break;
    }

    /* send entire packet */
    if (dg_flag == 0) {
	*to++ = dg_flag = SFLAG_PKTDATA | SFLAG_LASTGROUP | SFLAG_WANTPKTSEQD;
	memcpy(to, from, size);
	*psize = size + 1;
    } else
	/* tack on any left-overs */
	if (size_left) {
	    /* send left over data as raw bytes */
	    *to++ = SFLAG_PKTDATA | SFLAG_LASTGROUP;
	    (*psize)++;
	    for (i = 0; i < size_left; i++)
		*to++ = from[16 + i];
	    (*psize) += size_left;
	}

    /* create a local entry to match on */
    if (dg_flag & SFLAG_WANTPKTSEQD)
	make_new_sequence(ring, from, 0, size, ttynum);

    if (conf->debug_sb > 1) {
	buginf("after smartbuffer encode:");
    }

    return 0;
}

/***********************************************************************
 * NAME:
 *         smartbuffer_decode()
 * ABSTRACT:
 *         Decode an smartbuffered appletalk packet
 *
 *         Interpret smartbuffer "datagroup flags", finding and
 *         saving sequences of bytes.
 *         
 * INPUT:
 *         config_t *conf 	- ptr to session config struct
 *         ring_t *ring 	- ptr to ring
 *         unsigned char *from  - smarbuffer data to decode
 *         int size 		- size of data in bytes
 *         unsigned char *to 	- buffer to defcode packet into
 *         int *psize		- resulting size of decoded data
 * OUTPUT:
 *         return 0 on success
\************************************************************************/

int
smartbuffer_decode (
    config_t *conf,
    tt_soc *tty,
    unsigned char *from,
    int size,
    unsigned char *to,
    int *psize)
{
    unsigned char orig_dg_flag, dg_flag;
    unsigned char *orig_to = to;
    sb_t *entry;
    int done = 0;
    unsigned char *seq_ptr = NULL;
    ring_t *ring = get_tty_smartbufptr(tty, SMARTBUF_INPUT);
    int ttynum = tty->ttynum;

    if (conf->debug_sb)
	buginf("smartbuffer_decode(%lx, %lx, %lx, %d, %lx, %lx)\n",
	       conf, ring, from, size, to, psize);

    if (conf->debug_sb > 2) {
	buginf("smartbuffer input:");
    }

    /* get initial datagroup flag */
    dg_flag = orig_dg_flag = *from;
    *psize = 0;

    if (dg_flag == (SFLAG_PKTDATA | SFLAG_LASTGROUP | SFLAG_WANTPKTSEQD))
    {
	SB_DEBUG(("datagroup flag 0x%x\n", dg_flag));
	memcpy(to, ++from, size - 1);
	*psize = size - 1;
    } else
	while (size-- && !done) {
	    int seq = 0, range_start, range_len;

	    dg_flag = *from++;
	    SB_DEBUG(("datagroup flag 0x%x\n", dg_flag));

	    done = dg_flag & SFLAG_LASTGROUP;
	    range_start = range_len = 0;

	    /* if tokenized, grab seq # first */
	    if (dg_flag & SFLAG_TOKENIZED) {
		seq = from[0] << 8 | from[1];
		from += 2;
		size -= 2;
		SB_DEBUG(("tokenized: seq %d\n", seq));
	    }

	    /* if range, grab range next */
	    if (dg_flag & SFLAG_RANGE) {
		get_compact_num(from, size, range_start);
		get_compact_num(from, size, range_len);
		SB_DEBUG(("range: %d to %d\n",
			  range_start, range_start + range_len - 1));
	    }

	    /* if neither */
	    if ((dg_flag & (SFLAG_TOKENIZED | SFLAG_RANGE)) == 0) {
		/* magic; 0x50 means no compact num follows */
		if (done)
		    range_len = size;
		else
		    get_compact_num(from, size, range_len);
		memcpy(to, from, range_len);
		to += range_len;
		from += range_len;
		(*psize) += range_len;
		SB_DEBUG(("raw data: %d bytes\n", range_len));
	    } else {
		/* tokenized range; copy range of bytes from packet seq  */
		if ((entry = find_sequence_number(ring, seq))) {
		    if (range_len == 0)
			range_len = entry->data_len;
		    seq_ptr = (unsigned char *)(entry + 1);

		    memcpy(to, seq_ptr + range_start, range_len);
		    SB_DEBUG(("tokenized range, %d to %d, (%d bytes)\n",
			      range_start, range_start + range_len - 1,
			      range_len));

		    to += range_len;
		    (*psize) += range_len;
		} else {
		    buginf("smartbuffer: unable to find seq #%d\n", seq);
		    dump_sequence_numbers(ring);
		    return -1;
		}
	    }

	    /* should we create a new sequence for this sequence? */
	    if (dg_flag & SFLAG_WANTSEQD) {
		SB_DEBUG(("saving sequence, %d to %d, (%d bytes)\n",
			  range_start, range_start + range_len - 1,
			  range_len));

		make_new_sequence(ring, seq_ptr, range_start, range_len, ttynum);
	    }
	}

    /* see if sender wants the final output sequenced */
    if (orig_dg_flag & SFLAG_WANTPKTSEQD) {
	SB_DEBUG(("saving packet (%d bytes)\n", *psize));
	make_new_sequence(ring, orig_to, 0, *psize, ttynum);
    }

    if (conf->debug_sb > 2) {
	buginf("after smartbuffer decode:");
    }

    return 0;
}

