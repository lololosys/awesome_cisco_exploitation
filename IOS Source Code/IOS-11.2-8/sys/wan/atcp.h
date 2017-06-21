/* $Id: atcp.h,v 3.2.62.2 1996/08/18 09:07:30 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/atcp.h,v $
 *------------------------------------------------------------------
 * Appletalk control protocol definitions for PPP
 *
 * January 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atcp.h,v $
 * Revision 3.2.62.2  1996/08/18  09:07:30  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.2.62.1  1996/04/29  09:45:06  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.2  1995/11/17  18:00:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MAX_CIARRAY_LENGTH 128

struct atcp_options_ {
    int neg_address : 1;	/* Negotiate Appletalk Address? */
    int neg_routeprot: 1;	/* Negotiate Routing Protocol? */
    int neg_supbroad: 1;	/* Negotiate Broadcast Supression? */
    int neg_compprot: 1;	/* Negotiate Compression Protocol? */
    int neg_server: 1;		/* Negotiate Server Information? */
    int neg_zone: 1;		/* Negotiate Zone Information? */
    int neg_defrouter: 1;	/* Negotiate Default Router? */
    ushort her_address_net;	/* network number for other side */
    uchar her_address_node;	/* node number for other side */
    ushort my_address_net;	/* network number for my side */
    uchar my_address_node;	/* node number for my side */
    ushort routing_protocol;	/* number of routing protocol to use */
    uchar suppressed_types[MAX_CIARRAY_LENGTH];  /* list of DDP types for which 
					broadcasts are supressed */
    uchar num_suppressed_types;	/* number of types supressed */
    ushort compression_protocol; /* type of compression to use */
    ushort server_class;	/* type of server */
    ulong implementation_id;	/* Unique ID for our implementation */
    uchar server_name[MAX_CIARRAY_LENGTH];  /* Server name */
    uchar server_name_len;
    uchar zone_name[MAX_CIARRAY_LENGTH];	/* Server's zone name */
    uchar zone_name_len;
    ushort default_router_net;	/* Network number for default router */
    ushort default_router_node;	/* node number for default router */

};

typedef struct atcp_options_ atcp_options;


/*
 * Options.
 */
#define CI_AT_ADDRESS		1	/* Appletalk Address */
#define CI_AT_ROUTING_PROTOCOL	2	/* Protocol to use?  */
#define CI_AT_SUPRESSBROADCAST	3	/* Suppress Broadcasts? */
#define CI_AT_COMPRESSION	4	/* Compression Protocol? */
#define CI_AT_RESERVED		5	/* RESERVED  */
#define CI_AT_SERVERINFO	6	/* Server Information */
#define CI_AT_ZONEINFO		7	/* Zone Information */
#define CI_AT_DEFAULT_ROUTER	8	/* Address of Default Router */

#define LENCIATADDR(neg)  (neg ? 6 : 0)
#define LENCIATRPROT(neg)  (neg ? 4 : 0)
#define LENCIATSUPR(neg,len)  (neg ? (2+len) : 0)	
#define LENCIATCOMP(neg)  (neg ? 4 : 0) 
#define LENCIATSERV(neg,len)  (neg ? (8+len) : 0)
#define LENCIATZONE(neg,len)  (neg ? (2+len) : 0)
#define LENCIATDEFROUT(neg)  (neg ? 6 : 0)

/*--------------------------- Begin ATADDR --------------------------- */
#define ADDCIATADDR(opt, neg, value1, value2) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATADDR(TRUE), ucp); \
	PUTCHAR(0, ucp); \
	PPP_PUTSHORT(value1, ucp); \
	PUTCHAR(value2, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIATADDR(opt, neg, value1, value2) \
    if (neg) { \
	ushort s; \
	uchar c; \
	if ((len -= LENCIATADDR(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATADDR(TRUE) || \
	    citype != opt) \
	    goto bad; \
	GETCHAR(c, p); \
	if (c != 0) \
	  goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != value1) \
	  goto bad; \
	GETCHAR(c, p); \
	if (c != value2) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIATADDR(opt, neg, code) \
    if (neg && \
	len >= LENCIATADDR(TRUE) && \
	p[1] == LENCIATADDR(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LENCIATADDR(TRUE); \
	INCPTR(3, p); \
	PPP_GETSHORT(s, p); \
	cinet = s; \
	GETCHAR(c, p); \
	cinode = c; \
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, cinet, cinode); \
	code \
    }

#define REJCIATADDR(opt, neg, value1, value2) \
    if (neg && \
	len >= LENCIATADDR(TRUE) && \
	p[1] == LENCIATADDR(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LENCIATADDR(TRUE); \
	INCPTR(3, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	/* Check rejected value. */ \
	if (cishort != value1) \
	    goto bad; \
	GETCHAR(c, p); \
	cichar = c; \
	if (c != value2) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1, value2); \
	neg = 0; \
    }

/*--------------------------- End ATADDR --------------------------- */

/*--------------------------- Begin ATRPROT --------------------------- */
#define ADDCIATRPROT(opt, neg, value1) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATRPROT(TRUE), ucp); \
	PPP_PUTSHORT(value1, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), value = %x", \
		 opt, #opt, value1); \
    }

#define ACKCIATRPROT(opt, neg, value1) \
    if (neg) { \
	ushort s; \
	if ((len -= LENCIATRPROT(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATRPROT(TRUE) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != value1) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x", \
		 opt, #opt, value1); \
    }

#define NAKCIATRPROT(opt, neg, code) \
    if (neg && \
	len >= LENCIATRPROT(TRUE) && \
	p[1] == LENCIATRPROT(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	len -= LENCIATRPROT(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x", \
		 opt, #opt, cishort); \
	code \
    }

#define REJCIATRPROT(opt, neg, value1) \
    if (neg && \
	len >= LENCIATRPROT(TRUE) && \
	p[1] == LENCIATRPROT(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	len -= LENCIATRPROT(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	/* Check rejected value. */ \
	if (cishort != value1) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1); \
	neg = 0; \
    }

/*--------------------------- End ATRPROT --------------------------- */

/*--------------------------- Begin ATSUPR --------------------------- */
#define ADDCIATSUPR(opt, neg, value1, value2) \
    if (neg) { \
	uint count; \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATSUPR(TRUE,value2), ucp); \
	for (count = 0; count < value2; count++){ \
	    PUTCHAR(value1[count], ucp); \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIATSUPR(opt, neg, value1, value2) \
    if (neg) { \
	uint count;\
	uchar c;\
	if ((len -= LENCIATSUPR(TRUE, value2)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATSUPR(TRUE, value2) || \
	    citype != opt) \
	    goto bad; \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c, p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIATSUPR(opt, neg, code) \
    if (neg && \
	len >= LENCIATSUPR(TRUE, 0) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	cishort = p[1]; \
	len -= LENCIATSUPR(TRUE, cishort); \
	INCPTR(2, p); \
	for  (count = 0; count < cishort; count++) { \
	    GETCHAR(c,p); \
	    ciarray[count] = c; \
	}\
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, ciarray, cishort); \
	code \
    }

#define REJCIATSUPR(opt, neg, value1, value2) \
    if (neg && \
	len >= LENCIATSUPR(TRUE, value2) && \
	p[1] == LENCIATSUPR(TRUE, value2) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	len -= LENCIATSUPR(TRUE, value2); \
	INCPTR(2, p); \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c,p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1, value2); \
	neg = 0; \
    }

/*--------------------------- End ATSUPR --------------------------- */

/*--------------------------- Begin ATCOMP --------------------------- */
#define ADDCIATCOMP(opt, neg, value1) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATCOMP(TRUE), ucp); \
	PPP_PUTSHORT(value1, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x", \
		 opt, #opt, value1); \
    }

#define ACKCIATCOMP(opt, neg, value1) \
    if (neg) { \
	ushort s; \
	if ((len -= LENCIATCOMP(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATCOMP(TRUE) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != value1) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x", \
		 opt, #opt, value1); \
    }

#define NAKCIATCOMP(opt, neg, code) \
    if (neg && \
	len >= LENCIATCOMP(TRUE) && \
	p[1] == LENCIATCOMP(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	len -= LENCIATCOMP(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x", \
		 opt, #opt, cishort); \
	code \
    }

#define REJCIATCOMP(opt, neg, value1) \
    if (neg && \
	len >= LENCIATCOMP(TRUE) && \
	p[1] == LENCIATCOMP(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	len -= LENCIATCOMP(TRUE); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	/* Check rejected value. */ \
	if (cishort != value1) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x", \
		 opt, #opt, value1); \
	neg = 0; \
    }
/*--------------------------- End ATCOMP --------------------------- */

/*--------------------------- Begin ATSERV --------------------------- */
#define ADDCIATSERV(opt, neg, value1, value2, value3, value4) \
    if (neg) { \
	uint count; \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATSERV(TRUE,value2), ucp); \
	PPP_PUTSHORT(value3, ucp); \
	PPP_PUTLONG(value4, ucp); \
	for (count = 0; count < value2; count++){ \
	    PUTCHAR(value1[count], ucp); \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIATSERV(opt, neg, value1, value2, value3, value4) \
    if (neg) { \
	uint count; \
	uchar c; \
	ushort s = 0; \
	ulong l = 0; \
	if ((len -= LENCIATSERV(TRUE, value2)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATSERV(TRUE, value2) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != value3) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	if (l != value4) \
	    goto bad; \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c, p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIATSERV(opt, neg, code) \
    if (neg && \
	len >= LENCIATSERV(TRUE, 0) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	uchar s; \
	uchar l; \
	cishort = p[1]; \
	len -= LENCIATSERV(TRUE, cishort); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	ciclass = s; \
	PPP_GETLONG(l, p); \
	ciimplementation = l; \
	for  (count = 0; count < cishort; count++) { \
	    GETCHAR(c,p); \
	    ciarray[count] = c; \
	}\
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, ciclass, ciimplementation); \
	code \
    }

#define REJCIATSERV(opt, neg, value1, value2, value3, value4) \
    if (neg && \
	len >= LENCIATSERV(TRUE, value2) && \
	p[1] == LENCIATSERV(TRUE, value2) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	ushort s; \
	ulong l; \
	len -= LENCIATSERV(TRUE, value2); \
	INCPTR(2, p); \
	PPP_GETSHORT(s, p); \
	if (s != value3) \
	    goto bad; \
	PPP_GETLONG(l, p); \
	if (l != value4) \
	    goto bad; \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c,p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1, value2); \
	neg = 0; \
    }

/*--------------------------- End ATSERV --------------------------- */

/*--------------------------- Begin ATZONE --------------------------- */
#define ADDCIATZONE(opt, neg, value1, value2) \
    if (neg) { \
	uint count; \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATZONE(TRUE,value2), ucp); \
	for (count = 0; count < value2; count++){ \
	    PUTCHAR(value1[count], ucp); \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIATZONE(opt, neg, value1, value2) \
    if (neg) { \
	uint count;\
	uchar c;\
	if ((len -= LENCIATZONE(TRUE, value2)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATZONE(TRUE, value2) || \
	    citype != opt) \
	    goto bad; \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c, p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIATZONE(opt, neg, code) \
    if (neg && \
	len >= LENCIATZONE(TRUE, 0) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	cishort = p[1]; \
	len -= LENCIATZONE(TRUE, cishort); \
	INCPTR(2, p); \
	for  (count = 0; count < cishort; count++) { \
	    GETCHAR(c,p); \
	    ciarray[count] = c; \
	}\
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, ciarray, cishort); \
	code \
    }

#define REJCIATZONE(opt, neg, value1, value2) \
    if (neg && \
	len >= LENCIATZONE(TRUE, value2) && \
	p[1] == LENCIATZONE(TRUE, value2) && \
	p[0] == opt) { \
	uint count; \
	uchar c; \
	len -= LENCIATZONE(TRUE, value2); \
	INCPTR(2, p); \
	for (count = 0; count < value2; count++) { \
	    GETCHAR(c,p); \
	    if (value1[count] != c) \
		goto bad; \
	} \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1, value2); \
	neg = 0; \
    }

/*--------------------------- End ATZONE --------------------------- */

/*--------------------------- Begin ATDEFROUT --------------------------- */
#define ADDCIATDEFROUT(opt, neg, value1, value2) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(LENCIATDEFROUT(TRUE), ucp); \
	PUTCHAR(0, ucp); \
	PPP_PUTSHORT(value1, ucp); \
	PUTCHAR(value2, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: sending CONFREQ, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define ACKCIATDEFROUT(opt, neg, value1, value2) \
    if (neg) { \
	ushort s; \
	uchar c; \
	if ((len -= LENCIATDEFROUT(TRUE)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != LENCIATDEFROUT(TRUE) || \
	    citype != opt) \
	    goto bad; \
	GETCHAR(c, p); \
	if (c != 0) \
	  goto bad; \
	PPP_GETSHORT(s, p); \
	if (s != value1) \
	  goto bad; \
	GETCHAR(c, p); \
	if (c != value2) \
	  goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config ACK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, value1, value2); \
    }

#define NAKCIATDEFROUT(opt, neg, code) \
    if (neg && \
	len >= LENCIATDEFROUT(TRUE) && \
	p[1] == LENCIATDEFROUT(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LENCIATDEFROUT(TRUE); \
	INCPTR(3, p); \
	PPP_GETSHORT(s, p); \
	cinet = s; \
	GETCHAR(c, p); \
	cinode = c; \
	if (ppp_negotiation_debug) \
	  buginf("\nATcp: config NAK received, type = %d (%s), values = %x, %x", \
		 opt, #opt, cinet, cinode); \
	code \
    }

#define REJCIATDEFROUT(opt, neg, value1, value2) \
    if (neg && \
	len >= LENCIATDEFROUT(TRUE) && \
	p[1] == LENCIATDEFROUT(TRUE) && \
	p[0] == opt) { \
	ushort s; \
	uchar c; \
	len -= LENCIATDEFROUT(TRUE); \
	INCPTR(3, p); \
	PPP_GETSHORT(s, p); \
	cishort = s; \
	/* Check rejected value. */ \
	if (cishort != value1) \
	    goto bad; \
	GETCHAR(c, p); \
	cichar = c; \
	if (c != value2) \
	    goto bad; \
	if (ppp_negotiation_debug) \
	  buginf("\natcp: config REJ received, type = %d (%s), value = %x, %x", \
		 opt, #opt, value1, value2); \
	neg = 0; \
    }

/*--------------------------- End ATDEFROUT --------------------------- */
