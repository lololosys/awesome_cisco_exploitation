/* $Id: packet.h,v 3.1 1995/11/09 09:06:31 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/packet.h,v $
 *------------------------------------------------------------------
 * packet.h  -- protocol independent packet header definitions
 *
 * 1-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: packet.h,v $
 * Revision 3.1  1995/11/09  09:06:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:49  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * PAKMACRO defines those fields common to all packet buffers.
 */

#define ENCAPBYTES 24
#define ENCAPWORDS (ENCAPBYTES/2)

#define PAKMACRO \
	idbtype	*if_input;		/* pointer to input interface */ \
	idbtype	*if_output;		/* pointer to output interface */ \
	short sharecount;		/* output queue share count */ \
	ulong flags;			/* flags */ \
   	ipaddrtype desthost;		/* next hop destination */ \
	ulong length;			/* used by TCP */ \
	char *dataptr;			/* used by TCP */ \
	mempointer cb;			/* control block (used by X.25) */ \
	mempointer cacheptr;		/* used by routing cache support */ \
	ulong unspecified[2];		/* miscellaneous (used by IP/TCP) */ \
	ulong inputtime;		/* time when packet entered system */\
	ushort datagramsize;		/* bytes of raw data */ \
	ushort enctype;			/* encapsulation type */ \
	uchar *datagramstart;		/* pointer to start of raw data */ \
        ushort linktype;		/* type of protocol packet holds. */ \
	union { \
	    ushort encaps[ENCAPWORDS]; \
	    uchar encapc[ENCAPBYTES]; \
	} e;

/*
 * Now we define the generic packet type.
 */
paktype {
	paktype *next;
	PAKMACRO
};					  

# define OVERHEADBYTES  (sizeof(paktype))

/*
 * Packet size definitions
 */

# define MAXDATAGRAMSIZE LARGEDATA	/* largest datagram we can allocate */

# define LARGEDATA 8192			/* large buffer size */
# define BIGDATA 1500			/* big buffer size */
# define MEDDATA 576			/* medium size buffers */
# define SMALLDATA 80			/* minimum buffer size */

/*
 * Queueing element definitions
 */

#define elementtype struct elementtype_

elementtype {
    elementtype *next;
    paktype *pakptr;
};

#define MINELEMENTS 100

#ifdef c_plusplus
extern paktype *pak_enqueue(queuetype *, paktype *);
extern paktype *pak_dequeue(queuetype *);
extern paktype *pak_requeue(queuetype *, paktype *);
extern paktype *pak_unqueue(queuetype *, paktype *);
extern paktype *pak_duplicate(paktype *);
#else
extern paktype *pak_enqueue();
extern paktype *pak_dequeue();
extern paktype *pak_requeue();
extern paktype *pak_unqueue();
extern paktype *pak_duplicate();
#endif
extern int elements;
extern boolean elementBLOCK();

/*
 * Datagram link types
 */

# define LINK_ILLEGAL	0		/* invalid link type */
# define LINK_PUP 	1		/* PUP */
# define LINK_IP 	2		/* IP */
# define LINK_ARP	3		/* IP ARP */
# define LINK_XARP	4		/* Xerox ARP */
# define LINK_RARP	5		/* Reverse ARP */
# define LINK_UNUSED	6		/* NOT IN USE */
# define LINK_ADDRESS	7		/* cisco Systems Serial ARP */
# define LINK_PROBE	8		/* HP's 802 version of ARP */
# define LINK_CHAOS	9		/* CHAOS protocols */
# define LINK_DECNET	10		/* DECNET Phase IV */
# define LINK_ISOIP	11		/* ISO IP */
# define LINK_LOOP	12		/* Ethernet loopback */
# define LINK_XNS	13		/* XNS protocols */
# define LINK_PAD	14		/* X.2[89] PAD protocol */
# define LINK_X25	15		/* X.25 for purists */
# define LINK_TCP2PAD	16		/* TCP to PAD translation */
# define LINK_PAD2TCP	17		/* PAD to TCP translation */
# define LINK_SPANNING  18		/* DEC spanning tree */
# define LINK_ETHERTALK 19	        /* Apple Ethertalk */
# define LINK_AARP      20	        /* Appletalk ARP  */

/*
 * Flags defined for the flags word of a datagram header
 */

# define PAK_PERMANENT      0x1		/* permanent buffer */
# define PAK_SMALL	    0x2		/* small buffer */
# define PAK_MIDDLE	    0x4		/* middle sized buffer */
# define PAK_BIG	    0x8		/* big buffer */ 
# define PAK_LARGE	    0x10	/* large buffer */
# define PAK_BROADCAST 	    0x20	/* input datagram is broadcast */
# define PAK_NOREDIRECT	    0x40	/* don't send redirects for this */
# define PAK_ALIAS	    0x80	/* sent to one of our IP aliases */
# define PAK_FAIRQ	    0x100	/* packet on fair queue */
# define PAK_SLIP	    0x200	/* packet originated from SLIP */
# define PAK_CONTROL	    0x400	/* packet is a PAD control packet */
# define PAK_HEARSELF	    0x800	/* turn on MCI hearself when TX'ing */
# define PAK_MCICHECKED	    0x1000	/* packet has been checked by MCI */
# define PAK_ABORT	    0x2000	/* packet has been aborted */

# define PAK_MASK (PAK_SMALL+PAK_MIDDLE+PAK_BIG+PAK_LARGE+PAK_PERMANENT)

/*
 * Flags that denote options that may be in the IP packet.
 * ipopt_inproc is responsible for setting them.
 */
# define PAKOPT_HASSEC	    0x10000	/* basic or extended SECurity */
# define PAKOPT_HASLSR	    0x20000	/* Loose Source Route */
# define PAKOPT_HASSSR	    0x40000	/* Strict Source Route */
# define PAKOPT_HASRRT	    0x80000	/* Record RouTe */
# define PAKOPT_HASSID	    0x100000	/* Stream ID */
# define PAKOPT_HASTSTMP    0x200000	/* Time STaMP */
# define PAKOPT_SRNOTEND    0x400000	/* route pntr not at end of list */


# define PAKOPT_HASROUTE  (PAKOPT_HASSSR | PAKOPT_HASLSR | PAKOPT_HASRRT)

# define PAKOPT_MASK	(PAKOPT_HASROUTE | PAKOPT_HASSEC | PAKOPT_HASSID | PAKOPT_HASTSTMP | PAKOPT_SRNOTEND)


/*
 * Traffic analysis array offsets
 */

extern ulong traffic[];

#define PACK_ILLEGAL		0

#define PROBE_REQRCV		(PACK_ILLEGAL+1)
#define PROBE_REPLYRCV		(PROBE_REQRCV+1)
#define PROBE_OTHERRCV		(PROBE_REPLYRCV+1)
#define PROBE_REPLYSENT		(PROBE_OTHERRCV+1)
#define PROBE_REPLYPROXY	(PROBE_REPLYSENT+1)
#define PROBE_REQSENT		(PROBE_REPLYPROXY+1)

#define ARP_REQRCV		(PROBE_REQSENT+1)
#define ARP_REPRCV		(ARP_REQRCV+1)
#define ARP_REVRCV		(ARP_REPRCV+1)
#define ARP_OTHERRCV		(ARP_REVRCV+1)
#define ARP_REQSENT		(ARP_OTHERRCV+1)
#define ARP_REPSENT		(ARP_REQSENT+1)
#define ARP_PROXYREP		(ARP_REPSENT+1)
#define ARP_REVSENT		(ARP_PROXYREP+1)

#define IP_INPUT		(ARP_REVSENT+1)
#define IP_FORMATERR		(IP_INPUT+1)
#define IP_CHECKSUMERR		(IP_FORMATERR+1)
#define IP_HOPCOUNT		(IP_CHECKSUMERR+1)
#define IP_NOPROTOCOL		(IP_HOPCOUNT+1)
#define	IP_RAINPUT		(IP_NOPROTOCOL+1)
#define IP_REASSEMBLED		(IP_RAINPUT+1)
#define IP_RATIMEOUT		(IP_REASSEMBLED+1)
#define IP_RAFAILED		(IP_RATIMEOUT+1)
#define IP_BCASTIN		(IP_RAFAILED+1)
#define IP_OUTPUT 		(IP_BCASTIN+1)
#define IP_NOROUTE		(IP_OUTPUT +1)
#define IP_FORWARDED		(IP_NOROUTE+1)
#define IP_LOCALDEST		(IP_FORWARDED+1)
#define IP_NOTGATEWAY		(IP_LOCALDEST+1)
#define IP_BCASTOUT		(IP_NOTGATEWAY+1)
#define	IP_FRAGGED		(IP_BCASTOUT+1)
#define IP_FRAGCOUNT		(IP_FRAGGED+1)
#define IP_FRAGFAILED		(IP_FRAGCOUNT+1)
#define IP_ENCAPSFAILED		(IP_FRAGFAILED+1)
#define IP_BADSECURE		(IP_ENCAPSFAILED+1)
#define IP_BADOPTION		(IP_BADSECURE+1)

#define TCP_INPUT		(IP_BADOPTION+1)
#define TCP_OUTPUT		(TCP_INPUT+1)
#define	TCP_RETRANS		(TCP_OUTPUT+1)
#define TCP_CHECKSUMERR		(TCP_RETRANS+1)
#define TCP_NOPORT		(TCP_CHECKSUMERR+1)
#define	TCP_ACTOPEN		(TCP_NOPORT+1)
#define	TCP_PASOPEN		(TCP_ACTOPEN+1)
#define	TCP_ATTFAIL		(TCP_PASOPEN+1)
#define	TCP_ESTREST		(TCP_ATTFAIL+1)

#define UDP_INPUT		(TCP_ESTREST+1)
#define UDP_OUTPUT		(UDP_INPUT+1)
#define UDP_CHECKSUMERR		(UDP_OUTPUT+1)
#define UDP_NOPORT		(UDP_CHECKSUMERR+1)
#define UDP_BCASTFORWARD	(UDP_NOPORT+1)

#define ICMP_INPUT		(UDP_BCASTFORWARD+1)
#define ICMP_OUTPUT		(ICMP_INPUT+1)
#define ICMP_CHECKSUMERR	(ICMP_OUTPUT+1)
#define	ICMP_ECHOREPRCV		(ICMP_CHECKSUMERR+1)
#define	ICMP_UNREACHRCV		(ICMP_ECHOREPRCV+1)
#define	ICMP_QUENCHRCV		(ICMP_UNREACHRCV+1)
#define ICMP_REDIRECTRCV	(ICMP_QUENCHRCV+1)
#define ICMP_ECHORCV		(ICMP_REDIRECTRCV+1)
#define	ICMP_TTLRCV		(ICMP_ECHORCV+1)
#define	ICMP_PARAMRCV		(ICMP_TTLRCV+1)
#define	ICMP_TIMESTRCV		(ICMP_PARAMRCV+1)
#define	ICMP_TIMESTREPRCV	(ICMP_TIMESTRCV+1)
#define	ICMP_INFOREQRCV		(ICMP_TIMESTREPRCV+1)
#define	ICMP_INFOREPRCV		(ICMP_INFOREQRCV+1)
#define ICMP_MASKREQRCV		(ICMP_INFOREPRCV+1)
#define ICMP_MASKREPRCV		(ICMP_MASKREQRCV+1)
#define	ICMP_OTHERRCV		(ICMP_MASKREPRCV+1)
#define ICMP_ECHOREPSENT	(ICMP_OTHERRCV+1)
#define ICMP_UNREACHSENT	(ICMP_ECHOREPSENT+1)
#define ICMP_QUENCHSENT		(ICMP_UNREACHSENT+1)
#define ICMP_REDIRECTSENT	(ICMP_QUENCHSENT+1)
#define ICMP_ECHOSENT		(ICMP_REDIRECTSENT+1)
#define ICMP_TTLSENT		(ICMP_ECHOSENT+1)
#define ICMP_PARAMSENT		(ICMP_TTLSENT+1)
#define ICMP_TIMESTSENT		(ICMP_PARAMSENT+1)
#define	ICMP_TIMESTREPSENT	(ICMP_TIMESTSENT+1)
#define	ICMP_INFOREQSENT	(ICMP_TIMESTREPSENT+1)
#define ICMP_INFOREPSENT	(ICMP_INFOREQSENT+1)
#define ICMP_MASKREQSENT	(ICMP_INFOREPSENT+1)
#define ICMP_MASKREPSENT	(ICMP_MASKREQSENT+1)

#define EGP_INPUT		(ICMP_MASKREPSENT+1)
#define EGP_OUTPUT		(EGP_INPUT+1)
#define EGP_FORMATERR		(EGP_OUTPUT+1)
#define EGP_CHECKSUMERR		(EGP_FORMATERR+1)
#define EGP_NOPORT		(EGP_CHECKSUMERR+1)

#define IGRP_INPUT		(EGP_NOPORT+1)
#define IGRP_OUTPUT		(IGRP_INPUT+1)
#define IGRP_CHECKSUMERR	(IGRP_OUTPUT+1)

#define CHAOS_INPUT		(IGRP_CHECKSUMERR+1)
#define CHAOS_FORMATERROR	(CHAOS_INPUT+1)
#define CHAOS_HOPCOUNT		(CHAOS_FORMATERROR+1)
#define CHAOS_BCASTIN		(CHAOS_HOPCOUNT+1)
#define CHAOS_OUTPUT 		(CHAOS_BCASTIN+1)
#define CHAOS_NOROUTE		(CHAOS_OUTPUT+1)
#define CHAOS_FORWARDED		(CHAOS_NOROUTE+1)
#define CHAOS_LOCALDEST		(CHAOS_FORWARDED+1)
#define CHAOS_NOTGATEWAY	(CHAOS_LOCALDEST+1)
#define CHAOS_BCASTOUT		(CHAOS_NOTGATEWAY+1)
#define CHAOS_ENCAPSFAILED	(CHAOS_BCASTOUT+1)

#define	PUP_INPUT		(CHAOS_ENCAPSFAILED+1)
#define	PUP_FORMATERROR		(PUP_INPUT+1)
#define	PUP_HOPCOUNT		(PUP_FORMATERROR+1)
#define	PUP_LOCALDEST		(PUP_HOPCOUNT+1)
#define	PUP_NOTGATEWAY		(PUP_LOCALDEST+1)
#define	PUP_BCASTIN		(PUP_NOTGATEWAY+1)
#define	PUP_BCASTOUT		(PUP_BCASTIN+1)
#define	PUP_FORWARDED		(PUP_BCASTOUT+1)
#define	PUP_OUTPUT		(PUP_FORWARDED+1)
#define	PUP_ENCAPSFAILED	(PUP_OUTPUT+1)
#define	PUP_NOROUTE		(PUP_ENCAPSFAILED+1)
#define	PUP_CHECKSUMERR		(PUP_NOROUTE+1)
#define	PUP_FORWBROAD		(PUP_CHECKSUMERR+1)

#define HELLO_INPUT		(PUP_FORWBROAD+1)
#define HELLO_OUTPUT		(HELLO_INPUT+1)
#define HELLO_CHECKSUMERR	(HELLO_OUTPUT+1)

#define SLIP_INPACKS		(HELLO_CHECKSUMERR+1)
#define SLIP_INBYTES		(SLIP_INPACKS+1)
#define SLIP_FMTERRORS		(SLIP_INBYTES+1)
#define SLIP_INESCAPES		(SLIP_FMTERRORS+1)
#define SLIP_CRCERR		(SLIP_INESCAPES+1)
#define SLIP_BADLEN		(SLIP_CRCERR+1)
#define SLIP_NOBUF		(SLIP_BADLEN+1)
#define SLIP_OUTPACKS		(SLIP_NOBUF+1)
#define SLIP_OUTBYTES		(SLIP_OUTPACKS+1)
#define SLIP_OUTESCAPES		(SLIP_OUTBYTES+1)
#define SLIP_DROPPED		(SLIP_OUTESCAPES+1)

#define MAXTRAFFIC		(SLIP_DROPPED+1)

#ifdef c_plusplus
paktype *getbuffer(int);
#else
paktype *getbuffer();
#endif
