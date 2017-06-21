/* $Id: x25_switch.h,v 3.3 1995/11/21 08:27:39 ahh Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_switch.h,v $
 *------------------------------------------------------------------
 * x25_switch.h -- X25 Packet Switch definitions
 *
 * November 1988, Greg Satz
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_switch.h,v $
 * Revision 3.3  1995/11/21  08:27:39  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:08:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  19:19:09  gglick
 * CSCdi35754:  Incoming x25 call is accepted as PAD call i.s.o being
 * routed
 *
 * Revision 2.1  1995/06/07  23:23:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __X25SWITCH_H__
#define __X25SWITCH_H__

#define	X25_SWITCH_VERSION	0

#define	X25_TCP_WINDOWSIZE	8192
#define X25_MAX_ALTERNATE 6     /*max # of ip alternate routes*/

/*
 * Transaction header
 */

typedef struct trans_ {
    ushort version;			/* version of this protocol */
    ushort length;			/* length of remainder of data */
} transtype;

/*
 * X.25 routing table structure
 */

#define	X25_ROUTE_IF		1
#define	X25_ROUTE_IPADDR	2
#define	X25_ROUTE_IPPT		3
#define	X25_ROUTE_CLNPPT	4
#define	X25_ROUTE_ALIAS		5
#define	X25_ROUTE_FUNC          6
#define	X25_ROUTE_CMNS          7

struct x25routeentrytype_ {
    struct x25routeentrytype_ *next;	/* next route in the linked list */

    char *x121;				/* X.121 address text */
    regexp *x121pattern;		/* parsed X.121 regular expression */
    char *cud;				/* Call User Data text */
    regexp *cudpattern;			/* parsed CUD regular expression */
    char *repsrc;			/* source re-write string */
    char *repdst;			/* destination re-write string */

    char *x121match;			/* last addr. matched to this route */
    idbtype *idb;			/* interface pointer */
    idbtype *source_idb;		/* source interface pointer for TCP */
    ushort type;			/* type of route */

    short pvc_dest_vc;			/* routed PVC's destination VC */
    char pvc_remote_if[16];		/* ASCII name of remote PVC's interf.*/
    uchar pvc_remote_if_type_len;
    uchar pvc_remote_if_len;

    short ipaddr_max;			/* max range + 1 of k in ipaddr[k]*/
    ipaddrtype ipaddr[X25_MAX_ALTERNATE]; /* IP address */
    void *data;				/* other data (PT or X.25 Function) */
    ulong usage;			/* usage counter */
    ulong tcp_keepalive_period;
    int tcp_keepalive_tries;
};


/***********************************************************************
 *
 *			Externs and Prototypes
 *
 ***********************************************************************/

/*
 * x25_switch.c
 */
extern boolean x25_switching(ulong sport, ulong dport, ipaddrtype fhost);
extern void x25_switch_packet(tcp_encaps *t, paktype *pak, lcitype *lci);
extern uint x25_forward_call(lcitype *lci, paktype *pak,
			     uchar *cud, uint cud_len, uchar *diag);
extern void x25_pvc_retry(lcitype *lci);
extern void x25_forward_call_connected(lcitype *lci, paktype *pak);
extern void x25_flush_call(lcitype *lci,
			   uchar cause, uchar diag, boolean time_pvc);
extern void x25_forward_clear(lcitype *lci, paktype *pak,
			      uchar cause, uchar diag);
extern void x25_forward_reset(lcitype *lci, paktype *pak,
			      uchar cause, uchar diag);
extern void x25_forward_interrupt(lcitype *lci, paktype *pak);
extern void x25_forward_interrupt_confirm(lcitype *, paktype *);
extern void x25_forward_data(lcitype *lci, paktype *pak, int sn,
			     uint len, int pr);
extern void x25_forward_ack(lcitype *lci, paktype *pak, int sn, int pr);
extern uint cons_forward_call(lcitype *lci, llctype *llc2,
			      uchar *cud, uint cud_len, boolean fwd);
extern uint x25_cons_forward_call(lcitype *lci, paktype *pak,
				  uchar *cud, uint cud_len, uchar *diag);
#endif /* __X25SWITCH_H__ */
