/* $Id: vines.h,v 3.2 1995/11/17 17:57:25 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/vines/vines.h,v $
 *------------------------------------------------------------------
 * vines.h -- definitions for Banyan Vines
 *
 * February 1990, David Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines.h,v $
 * Revision 3.2  1995/11/17  17:57:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * On token ring, vines has a demux byte immediately following the SAP
 * codes.  These numbers are used only by Vines, and have no relation to
 * any of the SAP codes (except the fact that they are sequential to the
 * Vines SAP).
 */
#define VINES_TRING_SAP_ENCAPBYTES	18
#define VINES_DEMUX_IP		0xBA	/* Vines IP Protocol */
#define VINES_DEMUX_ECHO	0xBB	/* Vines ECHO Protocol */

/*
 * VINES statistic elements
 */

#define VINES_INPUT_O		0	/* total input */
#define VINES_OUTPUT_O		1	/* total output */
#define VINES_LOCALDEST_O	2
#define VINES_FORWARDED_O	3
#define VINES_BCASTIN_O		4
#define VINES_BCASTOUT_O	5
#define VINES_BCASTFWD_O	6
#define VINES_NOTLAN_O		7	/* Bcast w/ lan only to non lan */
#define VINES_NOTGT4800_O	8 
#define VINES_NOCHARGES_O	9 
#define VINES_FORMATERROR_O	10	/* input with format error */
#define VINES_CKSUMERR_O	11
#define VINES_HOPCOUNT_O	12
#define VINES_NOROUTE_O		13
#define VINES_ENCAPSFAILED_O	14
#define VINES_UNKNOWN_O		15
#define VINES_ICP_IN_O		16	/* ICP (error) protocol */
#define VINES_ICP_OUT_O		17	/* ICP (error) protocol */
#define VINES_METRIC_OUT_O	18	/* ICP Metric sent */
#define VINES_MACECHO_IN_O	19	/* MAC Echo protocol */
#define VINES_MACECHO_OUT_O	20	/* MAC Echo protocol */
#define VINES_ECHO_IN_O		21	/* IPC(cisco) Echo protocol */
#define VINES_ECHO_OUT_O	22	/* IPC(cisco) Echo protocol */
#define VINES_PROXY_O		23	/* Sent a proxy for a service */
#define VINES_PROXY_REPLY_O	24	/* Sent reply to a proxy */
#define VINES_HELPERED_O	25	/* Sent to nearest "real" banyan server */
				
#define VINES_MAXTRAFFIC	26

/*
 * Vines access list defines
 */
#define VINESMINOACC   1
#define VINESMAXOACC 100
#define VINESMINEACC 101
#define VINESMAXEACC 200
#define VINESMINSACC 201
#define VINESMAXSACC 300

#define VINESMINACC    1
#define VINESMAXACC  300

#define V_NETBYTES  4		/* four bytes in a Vines network number */

/*
 * Vines IP header structure definition.
 */
STRUCTURE_REF(vinesiptype);

struct vinesiptype_ {
    ushort cksum;		/* End to End checksum */
    ushort len;			/* total length */
    uchar  tc;			/* transport control */
    uchar  protocol;		/* next higher level protocol */
    uchar  ddstnet[V_NETBYTES]; /* Vines destination address */
    ushort dsthost;
    uchar  ssrcnet[V_NETBYTES];	/* Vines source address */
    ushort srchost;
    uchar  data[0];
};

/*
 * Vines Global Variables
 */
extern boolean vines_running;		/* if running VINES */
extern uint vines_network;		/* our Vines network number */
extern ushort vines_host;		/* our Vines host number (== 1) */
extern ushort vines_client;		/* next Vines host number to assign */
extern queuetype *vines_idbQ;

/*
 * Vines Function Declarations
 */
extern boolean parse_vinesaddress(char *, ulong *, ushort *);
extern boolean vines_accesscheck(paktype *, int);
extern boolean vines_lookup_address(char *, ulong *, ushort *);
extern boolean vines_on_idb(idbtype *);
extern char *vines_lookup_name(ulong, ushort);
extern int vines_get_default_enctype(idbtype *);
extern uchar vines_demuxbyte(long);
extern uint vines_get_interface_metric(idbtype *);
extern void vines_enqueue(paktype *);
extern void vines_fastsetup(hwidbtype *);
extern void vines_init(subsystype *subsys);
extern void vines_init_idb(idbtype *);
extern void vines_set_encaps(idbtype *, int);
extern void vines_reload_imminent(void);
extern void vines_route_adjust(idbtype *);
extern void vines_trace_cisco(char *);
extern void vines_trace_banyan(char *);
