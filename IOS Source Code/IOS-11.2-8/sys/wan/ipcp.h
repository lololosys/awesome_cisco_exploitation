/* $Id: ipcp.h,v 3.2.62.1 1996/08/18 09:07:53 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ipcp.h,v $
 *------------------------------------------------------------------
 * ipcp.h - IP Control Protocol definitions.
 *
 * 02-January-1990, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcp.h,v $
 * Revision 3.2.62.1  1996/08/18  09:07:53  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.2  1995/11/17  18:02:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/16  21:22:42  fox
 * CSCdi42205:  Microsoft IPCP Extension options are rejected
 * Add support for primary and secondary DNS options.
 *
 * Revision 2.1  1995/06/07  23:18:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

typedef struct ipcp_options {
    int neg_addrs : 1;		/* Negotiate IP Addresses? (obsolete) */
    int neg_vj : 1;		/* Van Jacobson Compression? */
    int neg_address: 1;		/* Negotiate IP Address */
    ulong address;		/* Address to use in negotiation */
    uchar vj_slots;		/* Maximum VJ slots supported */
    uchar vj_csid;		/* 1 if compress SID OK, else 0 */
} ipcp_options;


/*
 * Options.
 */
#define CI_ADDRS	1	/* IP Addresses (obsolete) */
#define CI_COMPRESSTYPE	2	/* Compression Type */
#define CI_ADDRESS	3	/* IP Address */

#define CI_PRIMARYDNS	0x81	/* 129 Primary DNS Address (Microsoft) */
#define CI_PRIMARYNBNS	0x82	/* 130 Primary NBNS Address (Microsoft) */
#define CI_SECONDARYDNS	0x83	/* 131 Secondary DNS Address (Microsoft) */
#define CI_SECONDARYNBNS 0x84	/* 132 Secondary NBNS Address (Microsoft) */

#define LEN_CI_VJHDR 6

#define LENCISHORT(neg)  (neg ? 4 : 0)
#define LENCIADDRS(neg)  (neg ? 10 : 0)
#define LENCIVJHDR(neg)  (neg ? LEN_CI_VJHDR : 0)
#define LENCIADDRESS(neg) (neg ? 6 : 0)

#define ADDCIADDRS(opt, neg, value1, value2) \
    if (neg) { \
	ulong l; \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + 2 * sizeof(long), ucp); \
	l = n2hl(value1); \
	PPP_PUTLONG(l, ucp); \
	l = n2hl(value2); \
	PPP_PUTLONG(l, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: sending CONFREQ, type = %d (%s), value = %i/%i", \
		 opt, #opt, \
		 value1, value2); \
    }


#define ACKCIADDRS(opt, neg, value1, value2) \
    if (neg) { \
	ulong l; \
	if ((len -= 2 + 2 * sizeof(long)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + 2 * sizeof(long) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
        if (value1 != l) \
	  goto bad; \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	if (value2 != l) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config ACK received, type = %d (%s), value = %i/%i", \
		 opt, #opt, \
		 value1, value2); \
    }


#define NAKCIADDRS(opt, neg, code) \
    if (neg && \
	len >= 2 + 2 * sizeof(long) && \
	p[1] == 2 + 2 * sizeof(long) && \
	p[0] == opt) { \
	ulong l; \
	len -= 2 + 2 * sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	ciaddr1 = h2nl(l); \
	PPP_GETLONG(l, p); \
	ciaddr2 = h2nl(l); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config NAK received, type = %d (%s), value = %i/%i", \
		 opt, #opt, \
		 ciaddr1, ciaddr2); \
	code \
    }

#define REJCIADDRS(opt, neg, value1, value2) \
    if (neg && \
	len >= 2 + 2 * sizeof(long) && \
	p[1] == 2 + 2 * sizeof(long) && \
	p[0] == opt) { \
	ulong l; \
	len -= 2 + 2 * sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	/* Check rejected value. */ \
	if (cilong != value1) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	/* Check rejected value. */ \
	if (cilong != value2) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config REJ received, type = %d (%s), value = %i/%i", \
		 opt, #opt, \
		 value1, value2); \
	neg = 0; \
    }

#define ADDCIVJCOMP(opt, neg, slots, csid) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LEN_CI_VJHDR, ucp); \
	PPP_PUTSHORT(TYPE_PPP_VJCOMP, ucp); \
	PUTCHAR(slots, ucp); \
	PUTCHAR(csid, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: sending CONFREQ, type = %d (%s), slots = %d, csid = %d", \
		 opt, #opt, \
		 slots, csid); \
    }


#define ACKCIVJCOMP(opt, neg, slots, csid) \
    if (neg) { \
	ushort s; \
	uchar c; \
	if ((len -= LEN_CI_VJHDR) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LEN_CI_VJHDR || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != TYPE_PPP_VJCOMP) \
          goto bad; \
	GETCHAR(c, p); \
	if (c != slots) \
	  goto bad; \
	GETCHAR(c, p); \
	if (c != csid) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config ACK received, type = %d (%s), slots = %d, csid = %d", \
		 opt, #opt, \
		 slots, csid); \
    }


#define NAKCIVJCOMP(opt, neg, code) \
    if (neg && \
	len >= LEN_CI_VJHDR && \
	p[1] == LEN_CI_VJHDR && \
	p[0] == opt) { \
	ushort s; \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	if (s == TYPE_PPP_VJCOMP)  { \
            uchar slots; \
            uchar csid; \
            len -= LEN_CI_VJHDR; \
	    GETCHAR(slots, p); \
            GETCHAR(csid, p); \
	    if (ppp_negotiation_debug) \
	      buginf("\nipcp: config NAK received, type = %d (%s), slots = %d, csid = %d", \
		     opt, #opt, \
		     slots, csid); \
            code \
	  } \
    }

#define REJCIVJCOMP(opt, neg, slots, csid) \
    if (neg && \
	len >= LEN_CI_VJHDR && \
	p[1] == LEN_CI_VJHDR && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LEN_CI_VJHDR; \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	if (s != TYPE_PPP_VJCOMP) \
	  goto bad; \
	GETCHAR(c, p); \
	if (c != slots) \
          goto bad; \
	GETCHAR(c, p); \
	if (c != csid) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config REJ received, type = %d (%s), slots = %d, csid = %d", \
		 opt, #opt, \
		 slots, csid); \
	neg = 0; \
    }

#define ADDCIADDRESS(opt, neg, value1) \
    if (neg) { \
	ulong l; \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + sizeof(long), ucp); \
	l = n2hl(value1); \
	PPP_PUTLONG(l, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: sending CONFREQ, type = %d (%s), Address = %i", \
		 opt, #opt, \
		 value1); \
    }


#define ACKCIADDRESS(opt, neg, value1) \
    if (neg) { \
	ulong l; \
	if ((len -= 2 + sizeof(long)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + sizeof(long) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	if (value1 != l) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config ACK received, type = %d (%s), Address = %i", \
		 opt, #opt, \
		 value1); \
    }


#define NAKCIADDRESS(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(long) && \
	p[1] == 2 + sizeof(long) && \
	p[0] == opt) { \
	ulong l; \
	len -= 2 + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	ciaddr1 = h2nl(l); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config NAK received, type = %d (%s), Address = %i", \
		 opt, #opt, \
		 ciaddr1); \
	code \
    }

#define REJCIADDRESS(opt, neg, value1) \
    if (neg && \
	len >= 2 + sizeof(long) && \
	p[1] == 2 + sizeof(long) && \
	p[0] == opt) { \
	ulong l; \
	len -= 2 + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	/* Check rejected value. */ \
	if (cilong != value1) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config REJ received, type = %d (%s), Address = %i", \
		 opt, #opt, \
		 value1); \
	neg = 0; \
    }

/*
 * The following macros were added to support the Microsoft IPCP Extensions.
 * They are identical to the xxxCIADDRESS set above but were made unique
 * to ease long-term maintenance and reduce the possibility of a bug.
 */

#define NAKCISERVER(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(long) && \
	p[1] == 2 + sizeof(long) && \
	p[0] == opt) { \
	ulong l; \
	len -= 2 + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	ciaddr1 = h2nl(l); \
	if (ppp_negotiation_debug) \
	  buginf("\nipcp: config NAK received, type = %d (%s), Address = %i", \
		 opt, #opt, \
		 ciaddr1); \
	code \
    }

extern void ipcp_init(fsm *);
