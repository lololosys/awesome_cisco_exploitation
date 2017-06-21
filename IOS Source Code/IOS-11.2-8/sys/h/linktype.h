/* $Id: linktype.h,v 3.3.26.5 1996/05/10 01:37:32 wmay Exp $
 * $Source: /release/112/cvs/Xsys/h/linktype.h,v $
 *------------------------------------------------------------------
 * Link type definitions
 *
 * Dave Katz, December 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions of LINK_ types.  Broken out here so that it could be
 * included in "interface.h" to fix compilation warnings.
 *------------------------------------------------------------------
 * $Log: linktype.h,v $
 * Revision 3.3.26.5  1996/05/10  01:37:32  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/05/09  14:13:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.26.3  1996/05/04  00:42:24  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.3.26.2.10.1  1996/04/27  06:35:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.26.2  1996/04/03  23:18:51  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.3.26.1.6.1  1996/04/08  01:45:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.26.1  1996/03/23  01:30:13  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.32.1  1996/03/22  09:35:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.18.1  1996/03/05  06:10:33  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.12.1  1996/02/27  20:41:22  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/18  03:22:26  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.2.14.1  1996/01/11  01:44:59  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/23  03:58:50  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.2  1995/11/17  09:12:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:58:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/20  22:35:13  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.3  1995/09/11  10:22:54  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.2  1995/07/09  21:04:35  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB.
 *
 * Revision 2.1  1995/06/07  20:35:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef _LINKTYPES_
#define _LINKTYPES_

struct paktype_;
typedef long (*linktype_t)(struct paktype_ *);

/*
 * Datagram link types
 */

typedef enum LINK {
    LINK_ILLEGAL	= 0,		/* invalid link type */
    LINK_ARP 		= 1,		/* IP ARP */
    LINK_RARP		= 2,		/* Reverse ARP */
    LINK_XARP		= 3,		/* Xerox ARP */
    LINK_PROBE		= 4,		/* HP's 802 version of ARP */
    LINK_LOOP		= 5,		/* Ethernet loopback */
    LINK_ADDRESS	= 6,		/* cisco Systems Serial ARP */
    LINK_IP		= 7,	 	/* IP */
    LINK_PUP		= 8,	 	/* PUP */
    LINK_XNS		= 10,		/* XNS protocol (Xerox original) */
    LINK_NOVELL		= 11,		/* Novell IPX */
    LINK_APOLLO		= 12,    	/* Apollo Domain */
    LINK_VINES		= 13,	    	/* Banyan Vines IP */
    LINK_PAD		= 14,		/* X.2[89] PAD protocol */
    LINK_X25		= 15,		/* X.25 for purists */
    LINK_APPLETALK	= 16,		/* AppleTalk - Extended DDP */
    LINK_AARP		= 17,		/* Appletalk ARP  */
    LINK_DEC_SPANNING	= 18,		/* DEC spanning tree */
    LINK_IEEE_SPANNING	= 19,		/* IEEE spanning tree BPDU */
    LINK_NBF	        = 20,		/* NBF NetBios Frames */
    UNUSED_LINK_2       = 21,		/* Unused link types */
    LINK_DECNET		= 22,		/* DECnet Phase IV node unicast */
    LINK_DECNET_ROUTER_L1 = 23,		/* (Old) DECnet PhaseIV router multicast */
    LINK_DECNET_NODE	= 24,   	/* DECnet Phase IV node multicast */
    LINK_CLNS		= 25,		/* OSI 8473, ES-IS 9542, etc. */
    LINK_CLNS_ALL_ES	= 26,		/* ES-IS Multicast - all end systems*/
    LINK_CLNS_ALL_IS	= 27,		/* ES-IS Multicast - all intermed. */
    LINK_CLNS_BCAST	= 28,		/* CLNS & ES-IS Broadcasts */
    LINK_X25TCP		= 29,		/* X.25 over TCP */
    LINK_PPP		= 30,           /* Protocol PPP */
    LINK_LAT		= 31,		/* DEC LAT terminal protocol */
    LINK_VINES_ECHO	= 32,    	/* Banyan Vines Echo Protocol */
    LINK_VINES_LOOP	= 33,    	/* Banyan Vines Loopback Protocol */
    LINK_ATALK_SHORT	= 34,		/* AppleTalk short DDP */
    LINK_MOP_BOOT	= 35,		/* DEC MOP booting protocol */
    LINK_MOP_CONSOLE	= 36,		/* DEC MOP console protocol */
    LINK_RSRB		= 37,		/* RSRB raw */
    LINK_BRIDGE		= 38,		/* transparent bridge traffic */
    LINK_STUN		= 39,		/* Stunnel */
    LINK_FR_ARP		= 40,           /* Frame Relay */
    LINK_SMDS_ARP	= 41,           /* SMDS own ARP */
    LINK_MAC		= 42,		/* MAC packets */
    LINK_IBMNM		= 43, 		/* IBM Network Management packets */
    LINK_UARP		= 44,		/* Ultranet */
    LINK_ULTRA_HELLO	= 45,		/* Ultranet */
    LINK_X25SERVICE	= 46,		/* X.25 services */
    LINK_UNCOMPRESSED_TCP = 47,		/* Uncompressed TCP/IP */
    LINK_COMPRESSED_TCP	= 48,		/* Compressed TCP/IP */
    LINK_LLC2		= 49,		/* LLC type II */
    LINK_CMNS		= 50,		/* CMNS--X.25 over non-serial media */
    LINK_ISIS_ALL_L1_IS = 51,           /* ISIS - All Level 1 ISes */
    LINK_ISIS_ALL_L2_IS = 52,           /* ISIS - All Level 2 ISes */
    LINK_FR_SWITCH      = 53,           /* FR switching over IP tunnel */
    LINK_DECNET_PRIME_ROUTER = 54,	/* Ph-IV Prime all routers */
    LINK_SRB		= 55,		/* source route bridge traffic */
    LINK_QLLC		= 56,		/* qllc (SNA over X.25) */
    LINK_X25_MULTI_ENC	= 57,		/* X.25 multiprotocol encapsulation */
    LINK_LEX		= 58,		/* Lan Extension Protocol */
    LINK_LEX_RCMD	= 59,		/* Lex-NCP + LEX Remote commands */
    LINK_DECNET_ROUTER_L2 = 60,	        /* DECnet PhaseIV L2 router multcast */
    LINK_CLS            = 61,	        /* Frames from RSRB vring->CLS vring */
    LINK_SNAPSHOT	= 62,		/* Start dialing, don't send frame */
    LINK_DLSW           = 63,           /* Data Link Switching traffic */
    LINK_SDE            = 64,           /* 802.10 Secure Data Exchange */
    LINK_CDP            = 65,           /* Cisco Discovery Protocol */
    LINK_PPP_COMPRESSION = 66,		/* Compression Control Protocol PPP */
    LINK_NMP            = 67,           /* Local LNM packet traffic */
    LINK_BSTUN          = 68,           /* block serial tunnel */
    LINK_IPC            = 69,           /* Out-of-band IPC */
    LINK_LOVE           = 70,           /* CIP statistics */
    LINK_CFGACK         = 71,           /* CIP asynchronous cfg acks */
    LINK_APPN_ANR       = 72,		/* APPN HPR ANR traffic */
    LINK_MULTILINK      = 73,		/* Multilink protocol */	
    LINK_NHRP		= 74,		/* NHRP */
    LINK_MAC_EXP	= 75,		/* MAC frames are express buffered */
    LINK_CGMP		= 76,		/* Cisco Group Management Protocol */
    LINK_VTP            = 77,           /* Cisco VLAN Transport Protocol */
    LINK_CPP		= 78,		/* Combinet proprietary encaps */
    LINK_ISL		= 79,		/* Cisco Inter Switch Link */
    LINK_FR_SWITCH_SERIAL = 80,         /* FR switching over serial line -
					  internal packet identification only */
    
    /*
     * When adding a new linktype, update this value and the name array
     * in network.c, and the code in print_linktype in network.c.
     */
    LINK_MAXLINKTYPE    = 81     	/* total number of linktypes */

} link_t;
				        /* we count from 0 - LINK_MAXTYPE,
					   inclusive */

/*
 * These are the route/bridge indicators (RBI's) for each of the eight
 * routeable protocols:
 *
 *    IP, XNS, NOVELL, APOLLO, VINES, APPLETALK, DECNET, and CLNS.
 *
 * They are encoded in tbridge_route_linktypes on the software idb to
 * indicate whether a given protocol is routed (SET) or bridged (CLEAR).
 * Currently they are only used in the context of Concurrent Routing
 * and Bridging (CRB).  Their use will be extended with the introduction
 * of Integrated Routing and Bridging (IRB).
 *
 * DO NOT CREATE THESE MASKS FOR ANY OTHER LINKTYPES!
 * DON'T USE tbridge_route_linktypes FOR ANY OTHER LINKTYPES!
 *
 * For IRB, since it is no longer true that a given protocol is
 * either routed or bridged, we need tbridge_bridge_linktypes on
 * software idb to indicate the protocol is bridged or not.
 *
 * The same indicators below are used for both tbridge_route_linktypes
 * and tbridge_bridge_linktypes.
 * idb->tbridge_route_linktypes => routed(SET); Not routed(CLEAR)
 * This indicator doesn't imply the given protocol is bridged or not
 * idb->tbridge_bridge_linktypes => bridged(CLEAR); Not bridged(SET)
 * This indicator doesn't imply the given protocol is routed or not
 *
 * Note that the default for IRB is to bridge everything.
 */

#define LINK_IP_RBI		(1 << LINK_IP)
#define LINK_XNS_RBI		(1 << LINK_XNS)
#define LINK_NOVELL_RBI		(1 << LINK_NOVELL)
#define LINK_APOLLO_RBI		(1 << LINK_APOLLO)
#define LINK_VINES_RBI		(1 << LINK_VINES)
#define LINK_APPLETALK_RBI	(1 << LINK_APPLETALK)
#define LINK_DECNET_RBI		(1 << LINK_DECNET)
#define LINK_CLNS_RBI		(1 << LINK_CLNS)

#define LINK_ALL_RBI		(LINK_IP_RBI | LINK_XNS_RBI | \
				 LINK_NOVELL_RBI | LINK_APOLLO_RBI | \
				 LINK_VINES_RBI | LINK_APPLETALK_RBI | \
				 LINK_DECNET_RBI | LINK_CLNS_RBI)

extern char *link2name(int);

#endif /* _LINKTYPES_ */





