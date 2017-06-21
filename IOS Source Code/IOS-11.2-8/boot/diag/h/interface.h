/* $Id: interface.h,v 3.1 1995/11/09 09:06:03 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/interface.h,v $
 *------------------------------------------------------------------
 * interface.h -- device independent network interface definitions
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface.h,v $
 * Revision 3.1  1995/11/09  09:06:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:44  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

# define paktype struct paktype_	/* forward declaration */

#define BANDWIDTH_SCALE 10000000	/* for scaling inverse bandwidths */
#define IFPERIOD_TIME   (10*ONESEC)	/* every 10 seconds */
#define	CARRIERTIME     (ONESEC/10)	/* every 100 milliseconds */
#define SNIFFLONGS  20			/* size of sniff buffer */
#define SNIFFBASE   10			/* zero offset of sniff buffer */


/*
 * Methods for resolving protocol addresses
 */

# define RES_HEURISTIC 1		/* Heuristics (usually 3MB PUP) */
# define RES_RARP 2			/* Reverse ARP */
# define RES_BOOTP 3			/* Boot Protocol */
# define RES_CONFIG 4			/* Configuration file */
# define RES_MANUAL 5			/* Manually set */
# define RES_NONVOL 6			/* Read from non-volatile RAM */


/*
 * Encapsulation methods
 * When adding a new code, be sure to update interface_up() function.
 */
# define ET_NULL 0			/* undefined -- error */
# define ET_ARPA 1			/* Ethernet - DDN style */
# define ET_ISO1 2			/* Ethernet - HP blew it */ 
# define ET_ISO2 3			/* Ethernet - the newest way */
# define ET_1822 4			/* DDN - 1822 */
# define ET_HDLC 5			/* Serial - raw HDLC */
# define ET_3MB 6			/* 3MB Ethernet */
# define ET_HDH 7			/* Serial - HDH */
# define ET_LAPB 8			/* Serial - LAPB */
# define ET_LAPBDCE 9			/* Serial - LAPB DCE */
# define ET_MLAPB 10			/* Serial - Multi LAPB */
# define ET_MLAPBDCE 11			/* Serial - Multi LAPB DCE */
# define ET_X25	12			/* Serial - X.25 */
# define ET_X25DCE 13			/* Serial - X.25 DCE */
# define ET_DDNX25 14			/* Serial - DDN X.25 */
# define ET_DDNX25DCE 15		/* Serial - DDN X.25 DCE */
# define ET_BFEX25 16			/* Serial - DDN BFE X.25 */
# define ET_MX25 17			/* Serial - Multi X25 */
# define ET_MX25DCE 18			/* Serial - Multi X25 DCE */
# define ET_HUB 19			/* HUB fiber optic */
# define ET_NOVELL_ETHERXNS 20		/* Novell style XNS on Ethernet */
# define ET_NOVELL_TRXNS 21		/* Novell style XNS on TR */
# define ET_UB_TRXNS 22			/* Ungermann-Bass XNS on TR */

#define	ISLAPB(idb) (idb && idb->enctype >= ET_HDH && idb->enctype <= ET_MX25DCE)
#define	ISX25(idb) (idb && idb->enctype >= ET_X25 && idb->enctype <= ET_MX25DCE)
#define	ISDDN(idb) (idb && (idb->enctype == ET_DDNX25 || idb->enctype == ET_DDNX25DCE))


/*
 * Flags in the status word of an idb
 * These are hardware related flags.  No protocol stack flags, please.
 */
# define IDB_XEROX     0x1		/* hardware is Xerox 3MB */
# define IDB_3COM      0x2		/* hardware is 3COM 3C400 */
# define IDB_ILAN      0x4		/* hardware is Interlan NI3210 */
# define IDB_SBESLOW   0x8		/* hardware is SBE M68K10 */
# define IDB_ACC       0x10		/* hardware is ACC M/1822 */
# define IDB_SBEFAST   0x20		/* hardware is SBE COM2 */
# define IDB_MCI       0x40		/* hardware is cisco MCI */
# define IDB_HUB       0x80		/* hardware is HUB fiber optic */
# define IDB_LB	       0x100		/* hardware is loopback */
# define IDB_LANCE     0x200		/* hardware is AMD Lance */

# define IDB_3MB       0x1000		/* hdw type is 3MB Ethernet */
# define IDB_ETHER     0x2000		/* hdw type is Ethernet/IEEE 802.3 */
# define IDB_SERIAL    0x4000		/* hdw type is synchronous serial */
# define IDB_1822      0x8000		/* hdw type is BBN 1822 */
# define IDB_TR	       0x10000		/* hdw type is Token Ring/IEEE 802.5*/
# define IDB_FDDI      0x20000		/* hdw type is FDDI */

# define IDB_IEEEMASK	(IDB_ETHER|IDB_TR|IDB_FDDI)

# define IDB_HEARSELF       0x100000	/* can do hardware loopback */
# define IDB_BCAST          0x200000	/* interface can do broadcasts */
# define IDB_KEEPALIVE      0x400000	/* can do keepalive dance */
# define IDB_INTERLEAVE     0x800000	/* serial interleave in effect */
# define IDB_IN_NOTREADY   0x1000000	/* input not ready last time */

/*
 * Interface state codes
 */
# define IDBS_DOWN	0
# define IDBS_GOINGDOWN	1
# define IDBS_INIT	2
# define IDBS_UP	3
# define IDBS_RESET	4
# define IDBS_ADMINDOWN	5

/*
 * Interface Description Block
 */

extern queuetype *idbQ;		    /* head of list of IDB's */

typedef struct idbtype_ {
	struct idbtype_ *next;	    /* pointer to next IDB in queue */
	int unit;		    /* interface unit number */
	int number;		    /* number of interface, unit 1 based */
	char *name;		    /* pointer to device type name */
	char *namestring;	    /* pointer to interface unit name */
	ushort hardware[3];	    /* the hardware host number */
	ulong status;		    /* hardware status bits */
	int state;		    /* current interface state */
	ulong state_time;	    /* and when it was last changed */
	int oldstate;		    /* state when carrier timer set */
	ulong carriertimer;	    /* timer for state change damping */
	int resolvemethod;	    /* RES_xxx method we used to resolve */
	ulong lastinput;	    /* time of last input */
	ulong lastoutput;	    /* time of last output */
	ulong lastreset;	    /* time of last reset for output failure*/
	char *reason;		    /* reason for last failure */
	ushort maxmtu;		    /* largest possible packet */
	ushort mtu;		    /* current maximum packet size */
	ulong delay;		    /* delay (used mainly by IGRP) */
	ulong delay_def;	    /* default delay value */
	ulong bandwidth;	    /* inverse bandwidth (IGRP) */
	ulong bandwidth_def;	    /* default inverse bandwidth */
	uchar load;		    /* load (IGRP) */
	long intlevel;		    /* priority interrupt level */
	mempointer devctl;	    /* ptr to device control registers */
	ushort loopback;	    /* interface is marked loopback */
	int enctype;		    /* native encapsulation type */
	int enctype_def;	    /* default encapsulation type */
	boolean nokeepalive;	    /* TRUE if keepalive disabled */
	uchar reliability;	    /* reliability coefficient */
	ulong xmittimer;	    /* timer for output */
	paktype	*pakout;	    /* packet currently being output */
	queuetype holdq;	    /* hold queue */
	short holddef;		    /* maximum default size */

	/*
	 * Interface function dispatches
	 */
	long (*encap)();	    /* encapsulation routine */
	long (*deencap)();	    /* undo encapsulation */
	long (*soutput)();	    /* output starter */
	long (*oqueue)();	    /* enqueue packet for output */
	long (*iqueue)();	    /* enqueue packet for input */
	long (*hardinit)();	    /* init hardware */
	long (*reset)();	    /* reset interface */
	long (*linktype)();	    /* get generic link type of input if */
	long (*broadcast)();	    /* physical broadcast predicate */
	long (*setencap)();	    /* set encapsulation method */
	long (*cstate)();	    /* interface changed state */
  	long (*periodic)();	    /* check timeouts, etc. */

	/*
	 * Generic interface statistics
	 */
	long inputs;		    /* no. packets received */
	long input_broad;	    /* no. broadcasts received */
	long input_error;	    /* no. damaged packets received */
	long input_crc;		    /* no. received with CRC errors */
	long input_frame;	    /* no. rcvd with framing/alignment error*/
	long input_overrun;	    /* no. overruns and resource errors */
	long input_abort;	    /* no. aborts received */
 	long input_resource;	    /* no. of insuff. input resources events*/
 	long input_restart;	    /* no. of resetarts by periodic checker */
	long input_unknown;	    /* no. of unknown protocol types */
	long outputs;		    /* no. packets transmitted */
	long output_broad;	    /* no. broadcasts transmitted */
	long output_error;	    /* no. packets erred on output */
	long output_collision;	    /* no. of packets collided on output */
	long nobuffers;		    /* no. dropped because no buffers */
	long outputdrops;	    /* no. dropped on output */ 	
	long resets;		    /* no. of board inits */
        long runts;		    /* no. too small packets received */
        long giants;		    /* no. too large packets received */
	long transitions;	    /* no. of carrier transitions */

	long tx_cumbytes;	    /* total bytes sent */
	long tx_intbytes;	    /* tx_cumbytes last interval */
	long tx_datarate;	    /* average data rate */

	long rx_cumbytes;	    /* total bytes received */	
	long rx_intbytes;	    /* value of rx_cumbytes last time */
	long rx_datarate;	    /* average data rate */

	long tx_intoutputs;	    /* value of outputs last interval */
	long tx_packetrate;	    /* average output packet rate */
	long rx_intinputs;	    /* value of inputs last time */
	long rx_packetrate;	    /* average input packet rate */
	long rxtx_interrors;	    /* input and output errors last interval*/

	/*
	 * Internet protocol specific fields
	 */
	boolean ip_enabled;	    /* TRUE if IP enabled on this interface */
        ipaddrtype ip_address;	    /* IP interface address */
	ipaddrtype ip_broadcast;    /* IP broadcast address */
	ipaddrtype ip_majornet;	    /* major IP network number */
	ipaddrtype ip_majormask;    /* major IP network mask */
	ipaddrtype ip_nets_mask;    /* mask of network and subnet fields */
	ipaddrtype ip_helpernet;    /* for "helper-net" */
	ipaddrtype ip_resolvehost;  /* host that provided address info */
	queuetype ip_addressQ;      /* list of secondary IP addresses */
	boolean ip_unreach;	    /* TRUE if send ICMP unreachables */
	boolean ip_redirect;	    /* TRUE if send ICMP redirects */
	boolean ip_maskreply;	    /* TRUE if send ICMP mask reply */
	boolean ip_routecache;	    /* TRUE if route cache enabled */
	boolean ip_fastokay;	    /* route cache enabled and possible */
	ulong ip_maskid;	    /* special ICMP mask code for valid. */
	ulong ip_quench_timer;	    /* msclock of last ICMP source quench */
	ulong ip_quench_interval;   /* minimum interval between quenchs */
	ulong ip_unreach_timer;	    /* msclock of last ICMP unreachable */
	ulong ip_unreach_interval;  /* minimum interval between unreach. */
	long ip_accesslist;	    /* access list for switching */
  	long ip_proxylist;	    /* access list for proxy ARP */
	boolean ip_secured;	    /* TRUE if IP security active */
	uchar ip_secure_mode;	    /* mode of security operation */
	uchar ip_secure_low;	    /* lower bound security level */
	uchar ip_secure_high;	    /* upper bound security level */
	uchar ip_secure_auth;	    /* authority matrix */

	/*
	 * 3COM 3C400 dependent fields.
	 */
	short ocrandom;		 /* used in collision detection */
	short ocload;		 /* output delay time */
	ushort icstatus;	 /* status bits */
	long n_collisions;	 /* =1 if collided on current output */
	volatile ushort *ethtxi; /* TX clear address for write */
	volatile ushort *ethobl; /* output buffer location */
	volatile ushort *ethocs; /* control and status */
	volatile ushort *ethibl; /* input buffer location */
	volatile ushort *ethics; /*   "   control and status */
	volatile ushort *ethrda; /* device address */
	volatile ushort *ethwda; /* write device address */
	volatile ushort *ethibb; /* 2nd input buffer */
	long collisions, jams;

	/*
	 * Interlan (Intel 82586) fields
	 */
	boolean ilan_lineup;	/* protocol up flag */
 	long ilan_deadman;	/* 82586 controller hung */

	/*
	 * Serial interface dependent fields.
	 */
	int serial_protocol;	/* firmware protocol type code */
	ulong serial_restarttimer; /* timer for periodically resets */

	/*
	 * MCI, routing cache, and bridging dependent fields
	 */
	int mci_index;		/* index into MCI controller descriptor */
	int mci_lineup;		/* protocol up flag used by MCI Ethernets */
	ulong mci_lineuptimer;	/* msclock since last lineup verification */
	int mci_clockindex;	/* index into clock rate table */
	short mci_rxoffset;	/* offset from zero of RX buffer */
	short mci_txcount;	/* count of available TX buffers */
	mempointer mci_regptr;	/* pointer to controller registers */
	mempointer mci_cacheptr;/* pointer to fast switching cache entry */
	boolean mci_checked;	/* TRUE if MCI_CMD_CHECK succeeded */
	charlong sniff[SNIFFLONGS]; /* buffer for sniffing from MCI */
	int span_index;		/* index into spanning tree array */
	short span_typeoffset;	/* offset of bridged Ethernet type field */
	short span_macoffset;	/* offset of MAC header in bridged datagram */
	int span_type_in;	/* input access list for Ethernet types */
	int span_type_out;	/* output access list for Ethernet types */
	mempointer span_ptr;	/* pointer to spanning tree structure */

	/*
	 * Spanning tree protocol variables for a port
	 */
	ushort port_id;
	int port_state;
	int path_cost;
	uchar designated_root[8];
	int designated_cost;
	uchar designated_bridge[8];
	ushort designated_port;
	boolean tc_acknowledgement;
	boolean hello_pending;
	ulong message_age_timer;
	ulong forward_delay_timer;
	ulong filter_timer;

	/*
 	 * 1822 protocol dependent fields
	 */
	short dropcnt;			/* count of packets to drop */

 	/*
 	 * DECnet protocol dependent fields
 	 */
	boolean dn_enabled;		 /* TRUE if DECnet enabled */
	boolean dn_designated;		 /* TRUE if we're designated router */
	boolean dn_lineup;		 /* TRUE if DECnet is up */
 	ulong dn_desig_timer;	 	 /* time last saw a designed router */
 	short dn_cost;			 /* cost for routing layer */
	int dn_accesslist;		 /* access list */
	int dn_in_filter;		 /* routing input filter */
	int dn_out_filter;		 /* routing output filter */
	int dn_route_time;
	int dn_hello_time;
	ulong dn_hello_timer;
	ulong dn_route_timer;
	boolean dn_routecache;		 /* route cache enabled */
	boolean dn_fastokay;		 /* route cache enabled and possible*/

	/*
	 * Chaos protocol dependent fields
	 */
	ushort chaosaddress;		/* CHAOS protocol address */
	ulong subnet_shift;		/* shift for address conversion */
	ipaddrtype subnet_mask;		/* mask of Internet subnet field */

	/*
	 * PUP protocol dependent fields
	 */
	uchar pupnet;			/* PUP network address */
	uchar puphost;			/* PUP host address */
	ushort pupaddr;			/* PUP address */

	/*
	 * XNS protocol dependent fields
	 */
	boolean xns_enabled;		/* TRUE if XNS enabled */
	ulong xnsnet;
	ulong xns_helpernet;
	uchar xns_helperaddr[6];
	int xns_enctype;		/* encapsulation to use for XNS */

	/*
 	 * Appletalk protocol dependent fields
 	 */
 	ushort atalknet;	/* 16-bit network number */
 	uchar atalknode;	/* 8-bit network node */
 	uchar zonename[32];	/* ZONELEN = 32, max Zone name length */

	/*
	 * HDLC protocol dependent fields
	 */
	ulong hdlc_myseq;		/* current probe sequence number */
	ulong hdlc_yoursseen;		/* what we believe of other side */
	ulong hdlc_mineseen;		/* what other side believes of us */
	boolean hdlc_lineup;		/* current state of line protocol */
	ushort hdlc_reliability;	/* current reliability estimate */

	/*
	 * HDH protocol dependent fields.
	 */
	ushort hdh_level;		/* HDH level up/down */
	ushort hdh_kind;		/* HDH protocol type */
	ushort hdh_lqp;			/* HDH line quality poor flag */
	ulong hdh_timer;		/* HDH control message timer */
	queuetype hdh_pQ;		/* HDH packet assembly queue */
	ushort hdh_pcnt;		/* HDH packet count in queue */
	ushort hdh_pbytes;		/* HDH byte count in queue */

	/*
	 * LAPB protocol dependent fields
	 */
	uchar lapb_state;		/* protocol state */
	uchar lapb_upstate;		/* protocol state code when up */
	uchar lapb_upstate1;		/* protocol state code when up */
	ushort lapb_t1;			/* T1 retransmission timer */
	ushort lapb_n1;			/* N1 number of bits for I data */
	uchar lapb_n2;			/* N2 retransmission count */
	uchar lapb_k;			/* k number of outstanding packets */
	short lapb_poll;		/* poll packet outstanding at seq. */
	uchar lapb_vs;			/* send state variable */
	uchar lapb_vr;			/* receive state variable */
	uchar lapb_rcnt;		/* receive frame count */
	uchar lapb_rvr;			/* remote receive packet count */
	uchar lapb_seqmap;		/* bitmap of legal NR seqs. */
	uchar lapb_retcnt;		/* retransmission count */
	ulong lapb_timer;		/* countdown timer */
	paktype *lapb_frmrpak;		/* frame error packet */
	paktype **lapb_paks;		/* pointer to array of packets */
	queuetype lapb_holdq;		/* hold packets sent outside window */
	char *lapb_buffer;		/* debugging buffer */
	ulong lapb_iframessent;		/* total IFRAMEs sent */
	ulong lapb_iframesrcvd;		/* total IFRAMEs received */
	ulong lapb_rnrsrcvd;		/* total RNRs received */
	ulong lapb_rejssent;		/* total REJs sent */
	ulong lapb_rejsrcvd;		/* total REJs received */
	ulong lapb_sabmssent;		/* total SABMs sent */
	ulong lapb_sabmsrcvd;		/* total SABMs received */
	ulong lapb_frmrssent;		/* total FRMRs sent */
	ulong lapb_frmrsrcvd;		/* total FRMRs received */
	ulong lapb_discssent;		/* total DICSs sent */
	ulong lapb_discsrcvd;		/* total DISCs received */

	/*
	 * These fields used by IGRP for load balancing over
	 * lossy serial lines.
	 */
	ulong lapb_lastiframes;		/* IFRAMES sent in last interval */
	ulong lapb_lastrejs;		/* REJ's received in last interval */
        ulong lapb_errorrate;		/* interface error rate */

	/*
	 * X25 protocol dependent fields
	 */
	ushort x25_win;			/* default window size */
	ushort x25_wout;		/* default window size */
	ushort x25_modulo;		/* default sequence numbering */
	ushort x25_pin;			/* default packet size */
	ushort x25_pout;		/* default packet size */
	ulong x25_timer;		/* interface timer */
	uint x25_default;		/* default linktype */
	uchar x25_address[20];		/* address of interface in x25ese */
	uchar x25_addrlen;		/* length of above */
	uchar x25_state;		/* interface state */
	uchar x25_upstate;		/* interface state code when up */
	uchar x25_cause;		/* for restart */
	uchar x25_diag;			/* also for restart */
	uchar x25_t0;			/* restart timer */
	uchar x25_t1;			/* call setup timer */
	uchar x25_t2;			/* reset indication timer */
	uchar x25_t3;			/* clear indication timer */
	uchar x25_th;			/* Ack hold timer */
	uchar x25_idle;			/* VC idle timer */
	uchar x25_nvc;			/* number of VCs */
	ushort x25_lic;			/* lowest incoming channel */
	ushort x25_hic;			/* highest incoming channel */
	ushort x25_ltc;			/* lowest two-way channel */
	ushort x25_htc;			/* highest two-way channel */
	ushort x25_loc;			/* lowest outgoing channel */
	ushort x25_hoc;			/* highest outgoing channel */
	ushort x25_holdmax;		/* size of hold queue */
	ushort x25_flags;		/* various support flags */
	ushort x25_holdtime;		/* time to ignore packets for VC */

	ushort x25_facilitylen;		/* length of global facilities */
	uchar x25_fwin;			/* global facility window size */
	uchar x25_fwout;		/* global facility window size */
	ushort x25_fpin;		/* global facility packet size */
	ushort x25_fpout;		/* global facility packet size */
	uchar x25_freverse;		/* global facility reverse */
	uchar x25_fcug;			/* global facility CUG */
	
	ulong x25_restartssent;		/* total restarts sent */
	ulong x25_restartsrcvd;		/* total restarts received */
	ulong x25_callssent;		/* total calls sent */
	ulong x25_callssentfailed;	/* total calls send failed */
	ulong x25_callsrcvd;		/* total calls received */
	ulong x25_callsrcvdfailed;	/* total calls received failed */
	ulong x25_callsfrwd;		/* total calls forwarded */
	ulong x25_callsfrwdfailed;	/* total calls forwarded failed */
	ulong x25_diagssent;		/* total DIAGs sent */
	ulong x25_diagsrcvd;		/* total DIAGs received */

} idbtype;
