/* $Id: ether.h,v 3.1 1995/11/09 09:05:26 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/ether.h,v $
 *------------------------------------------------------------------
 * ether.h  -- Ethernet/IEEE802.3 definitions
 *
 * 1-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether.h,v $
 * Revision 3.1  1995/11/09  09:05:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:39  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern int nethernets;			/* total Ethernet interfaces */
extern char baddr[];			/* broadcast address */
extern long set_etherencap();		/* parse Ethernet encapsulations */
extern long etherencap();		/* do Ethernet encapsulation */
extern long get10mblink();		/* decode link type */
extern long bcast10mb();		/* broadcast boolean */
extern long net_cstate();		/* interface state change */
extern long datagram_enqueue();
extern long raw_enqueue();
extern uchar dn_multi_e_router[];	/* DECnet router multicast */
extern uchar dn_multi_e_node[];		/* DECnet end node multicast */
extern uchar hpcast1[];			/* HP multicast */
extern uchar *dn_etheraddr;
extern uchar *xns_etheraddr;
extern uchar *novell_etheraddr;;
extern uchar *pick_802addr();

# define TIMEOUT (3*ONESEC)		/* net output timeout */
# define ETHERBYTES 6			/* bytes in an Ethernet address */
# define ETHERWORDS 3			/* words in an Ethernet address */
# define MINETHERSIZE 60		/* smallest 10MB packet, w/ encaps */
# define MAXETHERSIZE 1500		/* largest 10MB packet, w/o encaps */
# define MIN3MB 2			/* 3MB minimum data bytes */
# define MAXBOARDS 5			/* the maximum total interfaces */
# define ETHER_HOLDQ 100		/* maximum ethernet hold queue */

# define IGRP_DELAY 100			/* for IGRP on Ethernets */
# define IGRP_BANDWIDTH 1000		/* for IGRP on Ethernets */

# define ET3_ENCAPSIZE 4		/* total bytes of 3MB encapsulation */
# define ARPA_ENCAPSIZE 14		/* total bytes of Arpa encapsulation*/
# define ISO1_ENCAPSIZE 17		/* total bytes of ISO1 encapsulation*/
# define ISO2_ENCAPSIZE 22		/* total bytes of ISO2 encapsulation*/
# define NOVELL_ETHER_ENCAPSIZE 14	/* total bytes of Novell Ether encap */
# define ETALK_ENCAPSIZE 14		/* total bytes of Ethertalk encaps.*/
# define IEEE_SPANNING_ENCAPSIZE 14	/* total bytes of IEEE 802.1 encaps */

/*
 * Assigned Ethernet type codes
 */
# define TYPE_PUP	  0x0200	/* PUP */
# define TYPE_IP3MB	  0x0201	/* IP on 3MB net */
# define TYPE_IP10MB      0x0800	/* IP on 10MB net */
# define TYPE_RFC826_ARP  0x0806	/* IP ARP */
# define TYPE_XEROX_ARP	  0x0201	/* Xerox ARP on 10MB nets */
# define TYPE_REVERSE_ARP 0x8035	/* Reverse ARP */
# define TYPE_CHAOS	  0x0804	/* Chaos on 10MB net */
# define TYPE_DECNET	  0x6003	/* DECnet phase IV on Ethernet */
# define TYPE_LOOP	  0x9000	/* Ethernet loopback packet */
# define TYPE_XNS	  0x0600	/* XNS */
# define TYPE_DEC_SPANNING    0x8038	/* DEC spanning tree */
# define TYPE_ETHERTALK   0x809b	/* Apple EtherTalk */
# define TYPE_AARP	  0x80f3	/* Appletalk ARP */
# define TYPE_NOVELL1	  0x8137	/* Novell IPX */

/*
 * cisco HDLC type codes - not assigned Ethernet codes
 */
# define TYPE_NOVELL2	  0x1A58	/* Novell IPX, standard form */
# define TYPE_CLNS	  0xFEFE	/* CLNS */
# define TYPE_ESIS	  0xEFEF	/* ES-IS */
# define TYPE_IEEE_SPANNING 0x0000   	/* IEEE spanning tree */

/*
 * Definitions for encapsulation fields
 */
#define e3dst e.encapc[ENCAPBYTES-4]	/* 3MB experimental ether */
#define e3src e.encapc[ENCAPBYTES-3]
#define e3type e.encaps[ENCAPWORDS-1]

#define arpadst e.encapc[ENCAPBYTES-14]	/* Ether - ARPA style */
#define arpasrc e.encapc[ENCAPBYTES-8]
#define arpatype e.encaps[ENCAPWORDS-1]

#define iso1dst e.encapc[ENCAPBYTES-17]	/* Ether - ISO1 style */
#define iso1src e.encapc[ENCAPBYTES-11]
#define iso1len e.encapc[ENCAPBYTES-5]
#define iso1dsap e.encapc[ENCAPBYTES-3]
#define iso1lsap e.encapc[ENCAPBYTES-2]
#define iso1ctl e.encapc[ENCAPBYTES-1]

#define iso2dst e.encapc[ENCAPBYTES-22]	/* Ether - ISO2 style */
#define iso2src e.encapc[ENCAPBYTES-16]
#define iso2len e.encapc[ENCAPBYTES-10]
#define iso2dsap e.encapc[ENCAPBYTES-8]
#define iso2lsap e.encapc[ENCAPBYTES-7]
#define iso2ctl e.encapc[ENCAPBYTES-6]
#define iso2snap e.encapc[ENCAPBYTES-5]
#define iso2type e.encaps[ENCAPWORDS-1]

#define ieee_spanning_dst e.encapc[ENCAPBYTES-14] /* Ether - IEEE STP */
#define ieee_spanning_src e.encapc[ENCAPBYTES-8]
#define ieee_spanning_len e.encaps[ENCAPWORDS-1]

/*
 * Novell uses a weird encapsulation on ethernet that uses an 802.3 length
 * field, but does not have a dsap or lsap.  instead, and XNS datagram
 * immediately follows.  Fortunately, Novell uses FFFF as the XNS checksum,
 * and we can check for this ro recognize Novell packets on an 
 */
#define novelletherdst e.encapc[ENCAPBYTES-14]	/* Ether - Novell XNS style */
#define novellethersrc e.encapc[ENCAPBYTES-8]
#define novelletherlen e.encaps[ENCAPWORDS-1]
/* #define novelletherdsap e.encapc[ENCAPBYTES-2] */
/* #define novelletherlsap e.encapc[ENCAPBYTES-1] */

/*
 * 802.2 definitions.
 *
 * Strictly speaking SAP_NOVELL_ETHER is an outrageous KLUDGE!  Seems
 * Novell used a raw, non-complient, XNS encapsulation.  They wrapped
 * an XNS packet in a "raw" 802.3 packet.  That is it has a length but
 * no 802.2 layer.
 */

#define SAP_NULL	0x00
#define SAP_SMF		0x02
#define SAP_SMF_GROUP	0x03
#define SAP_IP		0x06
#define SAP_SPAN	0x42
#define SAP_CLNS	0xFE
#define SAP_SNAP	0xAA
#define SAP_GLOBAL	0xFF
#define SAP_RESPONSE	0x01

#define SAP_NOVELL	0xE0		/* User defined - Novell */
#define SAP_3COM	0x80		/* User defined - 3Com */
#define SAP_NOVELL_ETHER 0xFF		/* totally bogus - Novell */
#define SAP_EXTENDED	0xFC		/* User defined  - HP */

/*
 * SNAPCODE and CTL_BYTE should go away
 */
#define SNAPCODE 0xAA
#define CTL_BYTE 3
#define SAP_IEEE_SPANNING 0x00		/* not really a SAP */

#define LLC1_UI		0x03
#define LLC1_XID	0xAF
#define LLC1_TEST	0xE3
#define LLC1_P		0x10
#define LLC1_MIN	3
#define LLC1_MAX	8

/*
 * Loopback packet definitions
 */
#define	LOOP_REPLY	1
#define	LOOP_FORWARD	2
