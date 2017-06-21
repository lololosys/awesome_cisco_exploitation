/* $Id: ipxcp.h,v 3.2.62.1 1996/08/18 09:08:08 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ipxcp.h,v $
 *------------------------------------------------------------------
 * IPX control protocol definitions for PPP
 *
 * March, 1994, William Miskovetz
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxcp.h,v $
 * Revision 3.2.62.1  1996/08/18  09:08:08  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.2  1995/11/17  18:02:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:18:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Requires novell.h for NOVELL_SERVER_NAME_LEN */

typedef struct ipxcp_options {
    int neg_net : 1;		/* Negotiate IPX Network Number? */
    int neg_node : 1;		/* Negotiate IPX Node Number? */
    int neg_compress: 1;	/* Negotiate IPX compression? */
    int neg_rprot: 1;		/* Negotiate Routing Protocol? */
    int neg_rname: 1;		/* Negotiate Router Name? */
    int neg_conf_comp: 1;	/* Negotiate IPX configuration complete? */
    addrtype address;		/* network and node number for negotiation */
    uint routing_protocol;	/* number of routing protocol to use */
    char router_name [NOVELL_SERVER_NAME_LEN];  /* name of the router */
    uint name_length;		/* length of router name */
    uint comp_slots, comp_flags; /* Compression slots & flags */
} ipxcp_options;


/*
 * Options.
 */
#define CI_IPX_NET	1	/* IPX Network Number */
#define CI_IPX_NODE	2	/* IPX Node Number */
#define CI_IPX_COMPRESS	3	/* IPX Compression Protocol */
#define CI_IPX_ROUTING	4	/* IPX Routing Protocol */
#define CI_IPX_NAME	5	/* IPX Router Name */
#define CI_IPX_CONFCOMP	6	/* IPX Configuration Complete */

#define LENCI_COMP      4       /* Basic compression option size 4 */

#define IPXCP_COMP_CIPX  0x002  /* IPX compression: TELEBIT */
#define IPXCP_COMP_SHIVA 0x235  /* IPX compression: SHIVA */

#define LENCINET(neg)  (neg ? 6 : 0)
#define LENCINODE(neg)  (neg ? 8 : 0)
#define LENCICOMPRESS(neg)  (neg ? (LENCI_COMP + 2) : 0)
#define LENCIRPROT(neg)  (neg ? 4 : 0)   /* will need to be changed */
#define LENCINAME(neg, len)  (neg ? (2 + len) : 0)
#define LENCICONFCOMP(neg)  (neg ? 2 : 0)

#define ADDCINET(opt, neg, value1) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCINET(TRUE), ucp); \
	PPP_PUTLONG(value1, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: sending CONFREQ, type = %d (%s), value = %x", \
		 opt, #opt, value1); \
    }


#define ACKCINET(opt, neg, value1) \
    if (neg) { \
	ulong l; \
	if ((len -= LENCINET(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCINET(TRUE) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
        if (value1 != l) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config ACK received, type = %d (%s), value = %x", \
		 opt, #opt, value1); \
    }


#define NAKCINET(opt, neg, code) \
    if (neg && \
	len >= LENCINET(TRUE) && \
	p[1] == LENCINET(TRUE) && \
	p[0] == opt) { \
	ulong l; \
	len -= LENCINET(TRUE); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	ciaddr1 = h2nl(l); \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config NAK received, type = %d (%s), value = %x", \
		 opt, #opt, ciaddr1); \
	code \
    }

#define REJCINET(opt, neg, value1) \
    if (neg && \
	len >= LENCINET(TRUE) && \
	p[1] == LENCINET(TRUE) && \
	p[0] == opt) { \
	ulong l; \
	len -= LENCINET(TRUE); \
	INCPTR(2, p); \
	PPP_GETLONG(l, p); \
	cilong = h2nl(l); \
	/* Check rejected value. */ \
	if (cilong != value1) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config REJ received, type = %d (%s), value = %x", \
		 opt, #opt, value1); \
	neg = 0; \
    }

#define ADDCINODE(opt, neg, value1) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCINODE(TRUE), ucp); \
	ieee_copy(value1, ucp); \
	INCPTR(IEEEBYTES, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: sending CONFREQ, type = %d (%s), value = %e", \
		 opt, #opt, value1); \
    }


#define ACKCINODE(opt, neg, value1) \
    if (neg) { \
	uchar n[IEEEBYTES]; \
	if ((len -= LENCINODE(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCINODE(TRUE) || \
	    citype != opt) \
	    goto bad; \
	ieee_copy(p, n); \
	INCPTR(IEEEBYTES, p); \
        if (bcmp(n, value1, IEEEBYTES) != 0) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config ACK received, type = %d (%s), value = %e", \
		 opt, #opt, value1); \
    }


#define NAKCINODE(opt, neg, code) \
    if (neg && \
	len >= LENCINODE(TRUE) && \
	p[1] == LENCINODE(TRUE) && \
	p[0] == opt) { \
	len -= LENCINODE(TRUE); \
	INCPTR(2, p); \
	ieee_copy(p, cinode); \
	INCPTR(IEEEBYTES, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config NAK received, type = %d (%s), value = %e", \
		 opt, #opt, cinode); \
	code \
    }

#define REJCINODE(opt, neg, value1) \
    if (neg && \
	len >= LENCINODE(TRUE) && \
	p[1] == LENCINODE(TRUE) && \
	p[0] == opt) { \
	uchar n[IEEEBYTES]; \
	len -= LENCINODE(TRUE); \
	INCPTR(2, p); \
	ieee_copy(p, n); \
	INCPTR(IEEEBYTES, p); \
	/* Check rejected value. */ \
        if (bcmp(n, value1, IEEEBYTES) != 0) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config REJ received, type = %d (%s), value = %e", \
		 opt, #opt, value1); \
	neg = 0; \
    }



#define ADDCICOMPRESS(opt, neg, slots, value1) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCICOMPRESS(TRUE), ucp); \
	PPP_PUTSHORT(IPXCP_COMP_CIPX, ucp); \
	PUTCHAR(slots, ucp); \
        PUTCHAR(value1, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: sending CONFREQ, type = %d (%s), comp = %x, slots = %d, flags = %x", \
		 opt, #opt, IPXCP_COMP_CIPX, slots, value1); \
    }


#define ACKCICOMPRESS(opt, neg, slots, value1) \
    if (neg) { \
	ulong l; \
	uchar c; \
	if ((len -= LENCICOMPRESS(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCICOMPRESS(TRUE) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(l, p); \
	cilong = h2nl(l); \
        if (l != IPXCP_COMP_CIPX) \
	  goto bad; \
	GETCHAR(c, p); \
        if (c != slots) \
	  goto bad; \
	GETCHAR(c, p); \
	if (value1 != c) \
          goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config ACK received, type = %d (%s), comp = %x, slots = %d, value = %x", \
          opt, #opt, IPXCP_COMP_CIPX, slots, value1); \
    }

#define NAKCICOMPRESS(opt, neg, code) \
    if (neg && \
	len >= LENCI_COMP && \
	p[1] >= LENCI_COMP && \
	p[0] == opt) { \
        uchar c; \
	ushort s; \
	GETCHAR(c, p); \
	INCPTR(1, p); \
	PPP_GETSHORT(s, p); \
	if ((s == IPXCP_COMP_CIPX) && (c == LENCICOMPRESS(TRUE)))  { \
            uchar slots; \
            uchar flags; \
            len -= LENCICOMPRESS(TRUE); \
	    GETCHAR(slots, p); \
            GETCHAR(flags, p); \
	    if (ppp_negotiation_debug) \
	      buginf("\nipxcp: config NAK received, type = %d (%s), comp = %x, slots = %d, flags = %x", \
		     opt, #opt, IPXCP_COMP_CIPX, slots, flags); \
            code \
	  } else { \
	    neg = 0; \
	  }\
    }


#define REJCICOMPRESS(opt, neg, slots, flags) \
    if (neg && \
	len >= LENCICOMPRESS(TRUE) && \
	p[1] == LENCICOMPRESS(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LENCICOMPRESS(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	if (s != IPXCP_COMP_CIPX) \
	  goto bad; \
	GETCHAR(c, p); \
	if (c != slots) \
          goto bad; \
	GETCHAR(c, p); \
	if (c != flags) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\nipxcp: config REJ received, type = %d (%s), comp = %x, slots = %d, flags = %x", \
		 opt, #opt, IPXCP_COMP_CIPX, slots, flags); \
	neg = 0; \
	ao->neg_compress = ho->neg_compress = 0; \
    }

extern void ipxcp_init(fsm *);
