/* $Id: tcpcompress.h,v 3.2.60.1 1996/04/15 21:23:49 bcole Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpcompress.h,v $
 *------------------------------------------------------------------
 * tcpcompress.h -- TCP/IP header compression definitions, taken
 *                  from slcompress.h
 *
 * 16-January-1991, Tony Li
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpcompress.h,v $
 * Revision 3.2.60.1  1996/04/15  21:23:49  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:48:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/17  06:26:33  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 * 	Copyright (c) 1989 Regents of the University of California.
 * 	All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Van Jacobson (van@helios.ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 */

/* Number of connection states must be > 2 and <= 256 */
#define MAX_STATES 256
#define MIN_STATES 3
#define DEFAULT_THC_STATES 16
#define MAX_HDR 128

/*
 * Compressed packet format: (see RFC 1144 for a more complete explanation)
 *
 * The first octet contains the packet type (top 3 bits), TCP
 * 'push' bit, and flags that indicate which of the 4 TCP sequence
 * numbers have changed (bottom 5 bits).  The next octet is a
 * conversation number that associates a saved IP/TCP header with
 * the compressed packet.  The next two octets are the TCP checksum
 * from the original datagram.  The next 0 to 15 octets are
 * sequence number changes, one change per bit set in the header
 * (there may be no changes and there are two special cases where
 * the receiver implicitly knows what changed -- see below).
 * 
 * There are 5 numbers which can change (they are always inserted
 * in the following order): TCP urgent pointer, window,
 * acknowlegement, sequence number and IP ID.  (The urgent pointer
 * is different from the others in that its value is sent, not the
 * change in value.)  Since typical use of SLIP links is biased
 * toward small packets (see comments on MTU/MSS below), changes
 * use a variable length coding with one octet for numbers in the
 * range 1 - 255 and 3 octets (0, MSB, LSB) for numbers in the
 * range 256 - 65535 or 0.  (If the change in sequence number or
 * ack is more than 65535, an uncompressed packet is sent.)
 */

/*
 * The top nibble of the first octet is the packet type.  There are
 * three possible types: IP (not proto TCP or tcp with one of the
 * control flags set); uncompressed TCP (a normal IP/TCP packet but
 * with the 8-bit protocol field replaced by an 8-bit connection id --
 * this type of packet syncs the sender & receiver); and compressed
 * TCP (described above).
 *
 * LSB of 4-bit field is TCP "PUSH" bit (a worthless anachronism) and
 * is logically part of the 4-bit "changes" field that follows.  Top
 * three bits are actual packet type.  For backward compatibility
 * and in the interest of conserving bits, numbers are chosen so the
 * IP protocol version number (4) which normally appears in this nibble
 * means "IP packet".
 */

# define TYPE_ERROR 0x00		/* XXX Probably don't need this. */

/* 
 * Bits in first octet of compressed packet -- flag bits for what changed
 * in a packet
 */ 
# define NEW_C	0x40			/* change in connection */
# define NEW_I	0x20			/* change in IP id */
# define NEW_S	0x08			/* change in sequence number */
# define NEW_A	0x04			/* change in ack */
# define NEW_W	0x02			/* change in window */
# define NEW_U	0x01			/* change in URG */

/* reserved, special-case values of above */
# define SPECIAL_I (NEW_S|NEW_W|NEW_U)	/* echoed interactive traffic */
# define SPECIAL_D (NEW_S|NEW_A|NEW_W|NEW_U) /* unidirectional data */
# define SPECIALS_MASK (NEW_S|NEW_A|NEW_W|NEW_U)

#define TCP_PUSH_BIT 0x10		/* bit in change header */

/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate {
	struct cstate *cs_next;		/* next most recently used thcstate (xmit only) */
	ushort cs_hlen;		/* size of hdr (receive only) */
	u_char cs_id;			/* connection # associated with this state */
	u_char cs_filler;
	union {
		char csu_hdr[MAX_HDR];
		iphdrtype csu_ip;	/* ip/tcp hdr from most recent packet */
	} cs_u;
};
# define cs_ip cs_u.csu_ip
# define cs_hdr cs_u.csu_hdr

/*
 * all the state data for one serial line (we need one of these
 * per interface).
 */
struct thcdbtype_ {
	struct cstate *last_cs;		/* most recently used thcstate */
	uchar last_recv;		/* last rcvd conn. id */
	uchar last_xmit;		/* last sent conn. id */
	ushort flags;
	uint packets;			/* outbound packets */
	uint compressed;		/* outbound compressed packets */
	uint saved;			/* bytes saved */
	uint sent;			/* bytes sent */
	uint searches;			/* searches for connection state */
	uint misses;			/* times couldn't find conn. state */
	uint intmisses;			/* misses last interval */
	uint missrate;			/* average miss rate */
	uint maxmissrate;		/* max average miss rate */
	uint uncompressedin;		/* inbound uncompressed packets */
	uint compressedin;		/* inbound compressed packets */
	uint errorin;			/* inbound unknown type packets */
	uint tossed;			/* inbound packets tossed because of error */ 
	uint bufswaps;			/* inbound buffer swaps */
	uint buffail;			/* inbound buffer failures */
	int statecount;			/* number of connection states */
	int tstatecount;		/* maximum contexts on transmit */
	int rstatecount;		/* maximum contexts on receive */
	struct cstate *tstate;		/* xmit connection states */
	struct cstate *rstate;		/* receive connection states */
};

/* flag values */
# define THC_TOSS 	0x1		/* tossing rcvd frames because of input err */
# define THC_ACTIVE	0x2		/* actively compressing outbound packets */
# define THC_PERMACTIVE 0x4		/* always compress */
# define THC_SESSION	0x8		/* compress this session */


/*
 * Macro to update other data structures every time we might drop a packet:
 */
#define MAYBE_DROPPED(ifc) {\
	if (ifc && ifc->tcp_compress) \
	    ifc->tcp_compress->flags |= THC_TOSS; \
        }


/* ENCODE encodes a number that is known to be non-zero.  ENCODEZ
 * checks for zero (since zero has to be encoded in the long, 3 byte
 * form).
 */
#define ENCODE(n) { \
	if ((ushort)(n) >= 256) { \
		*cp++ = 0; \
		cp[1] = (n); \
		cp[0] = (n) >> 8; \
		cp += 2; \
	} else { \
		*cp++ = (n); \
	} \
}

#define ENCODEZ(n) { \
	if ((ushort)(n) >= 256 || (ushort)(n) == 0) { \
		*cp++ = 0; \
		cp[1] = (n); \
		cp[0] = (n) >> 8; \
		cp += 2; \
	} else { \
		*cp++ = (n); \
	} \
}

#define DECODEL(f) { \
	if (*cp == 0) {\
		(f) = h2nl(n2hl(f) + ((cp[1] << 8) | cp[2])); \
		cp += 3; \
	} else { \
		(f) = h2nl(n2hl(f) + (ulong)*cp++); \
	} \
}

#define DECODES(f) { \
	if (*cp == 0) {\
		(f) = h2n(n2h(f) + ((cp[1] << 8) | cp[2])); \
		cp += 3; \
	} else { \
		(f) = h2n(n2h(f) + (ulong)*cp++); \
	} \
}

#define DECODEU(f) { \
	if (*cp == 0) {\
		(f) = h2n((cp[1] << 8) | cp[2]); \
		cp += 3; \
	} else { \
		(f) = h2n((ulong)*cp++); \
	} \
}

/*
 * Special bcopy routines for when the toolkit won't do.
 * OVBCOPY - standard copy loop, byte at a time.
 * RVBCOPY - reverse direction bcopy, given ending addresses, walking down
 *	     in memory
 */

#define OVBCOPY(s,d,c) { \
        int i; \
	for (i = c; i; i--) \
	    *d++ = *s++; \
        } 

#define RVBCOPY(s,d,c) { \
    	int i; \
	for (i = c; i; i--) \
	    *d-- = *s--; \
    	} 

extern boolean thc_verify_compress(paktype *);
extern void thc_compress_reset(thcdbtype *, int, int);
extern void show_compression_stats(hwidbtype *idb, thcdbtype *thcdb);
extern boolean ip_header_compression_set(idbtype *, boolean, boolean);
extern void thc_enqueue(paktype *);
extern paktype *thc_encapsulation(hwidbtype *, paktype *);
extern void tcpcompress_parser_init(void);
