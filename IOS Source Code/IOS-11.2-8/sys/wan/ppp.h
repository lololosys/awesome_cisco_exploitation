/* $Id: ppp.h,v 3.7.4.17 1996/09/10 00:52:42 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp.h,v $
 *------------------------------------------------------------------
 * ppp.h -- Definitions for PPP
 *
 * 02-January-1990, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus:
 *------------------------------------------------------------------
 * $Log: ppp.h,v $
 * Revision 3.7.4.17  1996/09/10  00:52:42  snyder
 * CSCdi68568:  more constant opportunities, save memory
 *              156 out of image,
 *              116 out of data section
 * Branch: California_branch
 *
 * Revision 3.7.4.16  1996/08/30  08:41:07  fox
 * CSCdi66686:  Shivas NBFCP protocol (0x8fec) is not properly rejected
 * Branch: California_branch
 * Protocol reject all unknown protocols and unsupported NCPs.  Reject
 * broken Shiva protocol #s using the full protocol number.  Fix
 * support for protocol rejecting packets with compressed protocol #s.
 *
 * Revision 3.7.4.15  1996/08/21  19:18:12  fox
 * CSCdi57429:  Router cannot distinguish between FCS bit set/not set in
 * PPP header
 * Branch: California_branch
 * Add support for RFC 1638 bridging packet header fields.  Bounce packets
 * to process level if any fields are set.
 *
 * Revision 3.7.4.14  1996/08/18  09:08:51  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.7.4.13  1996/08/15  04:56:53  fox
 * CSCdi63702:  ip route thrashs on MLP master bundle interface
 * Branch: California_branch
 * Do not mark multiple MLP interfaces UP.  Prevent NCP activity
 * unless the interface is UP.
 *
 * Revision 3.7.4.12  1996/08/03  23:42:17  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.4.11  1996/07/30  21:01:47  dblair
 * CSCdi57980:  debug ppp multilink is too busy
 * Branch: California_branch
 * Clean up multilink debugs
 *
 * Revision 3.7.4.10  1996/07/30  20:18:33  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * Adding support for PPP LCP endpoint discriminator
 *
 * Revision 3.7.4.9  1996/07/01  21:28:14  syiu
 * CSCdi61806:  Multichassis MLP and VPDN should coexist
 * Branch: California_branch
 *
 * Revision 3.7.4.8  1996/06/24  09:28:26  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.7.4.7  1996/05/19  05:49:06  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.7.4.6  1996/05/10  01:45:23  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.7.4.5  1996/05/06  17:03:38  fox
 * CSCdi49278:  Update PAP handling of outbound calls
 * Branch: California_branch
 * Three bug fixes in one!
 * Disable remote PAP authentication support by default.  Add 'ppp pap
 * sent-username xxx password yyy' to re-enable support if needed.  Warn
 * if sent-username/password matches entry in local database. (CSCdi49278)
 * Add 'ppp chap hostname xxx' command to allow multiple boxes to use the
 * same hostname in their CHAP Challenges.  (CSCdi44884)
 * Add 'ppp chap password yyy' command to allow a box to respond to many
 * unknown peers with the same password.  (CSCdi54088)
 *
 * Revision 3.7.4.4  1996/05/01  14:33:48  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.7.4.3  1996/04/29  09:46:27  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.7.4.2  1996/04/27  06:37:31  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.8.7  1996/04/26  00:00:04  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.8.6  1996/04/23  00:24:39  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.4.8.5  1996/04/17  00:02:00  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.8.4  1996/03/29  15:55:07  dblair
 * Branch: Dial1_branch
 * Fix crash when clearing virtual interface and add some multilink knobs
 *
 * Revision 3.4.8.3  1996/01/31  18:37:35  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 3.4.8.2  1996/01/19  02:15:37  syiu
 * Branch: Dial1_branch
 * PPP L2F support
 *
 * Revision 3.4.8.1  1996/01/16  15:45:25  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.7.4.1  1996/03/27  10:36:40  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.7  1996/03/07  19:12:17  fox
 * CSCdi50935:  LCP Identification Code is Code Rejected
 * Trace and discard the packet as it is only an advisory for debugging
 *
 * Revision 3.6  1996/02/22  05:21:10  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.5  1996/01/25  06:29:01  fox
 * CSCdi47481:  Can still pass IP packets after authen before author
 * Don't convert between internal and external types until ACLs are
 * loaded (if necessary).
 *
 * Revision 3.4  1996/01/03  20:36:03  wmay
 * CSCdi38318:  cant specify inbound only ppp authentication - add callin
 * option to ppp authentication command
 *
 * Revision 3.3  1995/11/18  17:28:03  fox
 * CSCdi43678:  Obsolete code exists in ppp.c
 * Remove unused and obsolete ifdef blocks and unreferenced variables.
 *
 * Revision 3.2  1995/11/17  18:04:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:36:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/23  21:02:41  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.4  1995/10/02  01:55:15  lol
 * CSCdi41162:  xtacacsd: username no longer included in XTA_SLIPON
 *         Fake "if-needed" authentication for vty-async.
 *
 * Revision 2.3  1995/09/20  22:39:13  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.2  1995/09/06  18:46:08  syiu
 * CSCdi37776:  PPP callback command not written to nvgen correctly
 *
 * Revision 2.1  1995/06/07  23:19:05  hampton
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

#ifndef __PPP_H__
#define __PPP_H__

#include "../wan/parser_defs_ppp.h"

/*
 * PPP address definitions 
 */
#define	PPP_STATION	0xff03	
#define	PPP_ADDRESS	0xff
#define	PPP_CONTROL	0x03
#define PPP_HOLDQ       40
#define PPP_ENCAPBYTES        4  /* total bytes for PPP encaps */
#define PPP_PROTO_HDR_ENCAPBYTES 2 /* Size of ppp protocol header field */
#define PPP_ADDRCNTL_HDR_ENCAPBYTES 2 /* Size of addr/control header field */
#define PPP_MAX_COMPRESSED_PID 0xfe
				/* Maximum compressable protocol ID */
#define PPP_ENDPROTO	0x01	/* End of protocol tyep flag bit */
#define PPP_WOFFSET     0x0001  /* PPP word offset to protocol in buffer */ 

/*
 * Worst case of added header bytes from PPP.
 * Leave space for up to 32 bytes of added header room for PPP.
 * This value is passed to lapb which uses it in estimating it's default N1
 * value. Note, that the N1 variable is number of bits not bytes, hence the
 * multiplication by 8.
 */
#define PPP_MAX_HDR_SIZE 32*8

/*
 * Max length of the endpoint discriminator
 */
#define PPP_MAX_ENDPOINT_LENGTH 20

/*
 * Data Link Layer header = Address, Control, Protocol.
 */

typedef struct ppp_dll_hdr_ {
    uchar  address;
    uchar  control;
    ushort type;
    uchar  data[0];
} ppp_dll_hdr;

#define ALLSTATIONS	0xff	/* All-Stations Address */
#define UI		0x0003	/* Unnumbered Information */
#define DLLHEADERLEN	sizeof(ppp_dll_hdr)
#define MTU		1500	/* Default MTU */

/*
 * Packet header = Code, id, length.
 */
#define HEADERLEN	(sizeof(uchar) + sizeof(uchar) + sizeof(ushort))
#define PACKET_DATA(p)	(p + HEADERLEN + DLLHEADERLEN);


/*
 * Bridge Header (from RFC 1638)
 */
#define PPP_BH_FCS	0x8000	/* LAN FCS present in datagram */
#define PPP_BH_LAN_ID	0x4000	/* LAN IDentification present in datagram */
#define PPP_BH_ZEROFILL	0x2000	/* Packet must be zero filled- Tinygram Comp */
#define PPP_BH_PADS	0x0f00	/* # of pad chars at the end of the datagram */
#define PPP_BH_MAC_TYPE	0x00ff	/* MAC type of source packet */

#define PPP_BH_802_3	0x0001	/* Ethernet/802.3 bridged packet MAC type */

/*
 * Supported (default) value of F I Z, Count and MAC type
 */
#define PPP_FIZC_MACTYPE    PPP_BH_802_3

#define PPP_BRIDGE_ENCAPBYTES 6  /* 6 byte header for bridged pkts. */
#define PPP_BRIDGE_EXTRABYTES 2  /* 2 byte extra header for bridged pkts. */

#define PPP_LAN_FCS_SIZE   4	/* Size of FCS field (if present) */
#define PPP_LAN_ID_SIZE    4	/* Size of LAN ID (if present) */
#define PPP_MIN_PDU_SIZE  60	/* used for Tinygram Compression - RFC 1638 */


/*
 * PPP type codes - defined by the protocol
 */
#define TYPE_PPP_BROKEN1	0x000f	/* Shiva NetBIOS 0x0FEC */
#define TYPE_PPP_IP		0x0021	/* PPP IP */
#define TYPE_PPP_CLNS		0x0023	/* OSI  */
#define TYPE_PPP_XNS		0x0025	/* XNS IDP */
#define TYPE_PPP_DECNET		0x0027	/* DECnet phase IV */
#define TYPE_PPP_ETHERTALK	0x0029	/* Appletalk */
#define TYPE_PPP_NOVELL1	0x002b	/* Novell IPX */
#define TYPE_PPP_VJCOMP		0x002d	/* Van Jacobson Compressed TCP/IP */
#define TYPE_PPP_VJUNCOMP	0x002f	/* Van Jacobson Uncompressed TCP/IP */
#define TYPE_PPP_BRIDGE		0x0031	/* Bridging PDU */
#define TYPE_PPP_STREAM		0x0033	/* Stream Protocol ST-II */
#define TYPE_PPP_VINES		0x0035	/* Banyan Vines */
#define TYPE_PPP_MULTILINK	0x003d	/* Multilink PPP */
#define TYPE_PPP_NBF		0x003f	/* NBF Protocol */
#define TYPE_PPP_LEX		0x0041	/* Lan Extension Protocol */
#define TYPE_PPP_LLC2		0x004b	/* LLC2 */
#define TYPE_PPP_APPNANR	0x004d	/* APPN HPR non-ERP */
#define TYPE_PPP_TAG 		0x0081	/* Unicast tagswitching */
#define TYPE_PPP_MTAG 		0x0083	/* Multicast tagswitching */
#define TYPE_PPP_BROKEN3	0x008f	/* Shiva NetBIOS CP 0x8FEC */
#define TYPE_PPP_BROKEN4	0x00c1	/* (old) Shiva PAP 0xC123 */
#define TYPE_PPP_COMPRESS	0x00fd	/* Compressed Packet */ 

#define TYPE_PPP_SPANNING	0x0201	/* spanning tree bpdu ieee or dec */
#define TYPE_PPP_CDP		0x0207	/* Cisco Discovery Protocol */

#define TYPE_PPP_IPCP		0x8021	/* PPP IP Control Protocol */
#define TYPE_PPP_OSICP		0x8023	/* OSI Control Protocol */
#define TYPE_PPP_XNSCP		0x8025	/* XNS IDP Control Protocol */
#define TYPE_PPP_DECCP		0x8027	/* DECnet phase IV Control Protocol */
#define TYPE_PPP_ETHERTALKCP	0x8029	/* Appletalk Control Protocol */
#define TYPE_PPP_IPXCP		0x802b	/* Novell IPX Control Protocol */
#define TYPE_PPP_RESERVE1	0x802d 
#define TYPE_PPP_RESERVE2	0x802f
#define TYPE_PPP_BRIDGECP	0x8031	/* Bridging NCP */
#define TYPE_PPP_STREAMCP	0x8033	/* Stream Protocol CP */
#define TYPE_PPP_VINESCP	0x8035	/* Banyan Vines Control Protocol */
#define TYPE_PPP_NBFCP		0x803f	/* Netbios Frame Control Protocol */
#define TYPE_PPP_LEXCP		0x8041	/* Lan Extension Control Protocol */
#define TYPE_PPP_LLC2CP		0x804b	/* LLC2 CP */
#define TYPE_PPP_APPNANRCP	0x804d	/* APPN HPR non-ERP CP */
#define TYPE_PPP_TAGCP          0x8081  /* Unicast tagswitching CP */
#define TYPE_PPP_MTAGCP         0x8083  /* Multicast tagswitching CP */
#define TYPE_PPP_CCP		0x80fd	/* Compression Control Protocol */ 

#define TYPE_PPP_CDPCP		0x8207	/* Cisco Discovery Protocol */

#define TYPE_PPP_LCP		0xc021	/* PPP Link Control Protocol */
#define TYPE_PPP_UPAP		0xc023	/* PPP User Passwd. Authen. Protocol */
#define TYPE_PPP_LQR		0xc025	/* PPP Link Quality Reports */
#define TYPE_PPP_SPAP		0xc027	/* PPP Shiva Passwd Auth Protocol */
#define TYPE_PPP_CHAP		0xc223	/* Challenge Handshake Authen */

/*
 * NOTE:  Early in the implementation of PPP, IANA assigned four PPP
 *        protocol numbers to Shiva that violated the fundamental
 *        rules of PPP Protocol numbers, ie the first byte must be
 *        even and the second odd.  Shiva shipped product using these
 *        numbers and continues to support them in current products.
 *        IANA is supposed to reserve the four protocol numbers that
 *        the broken protocols resemble (0x0F, 0x81, 0x8f and 0xc1).
 *        CSCdi66686
 */
#define BAD_TYPE_PPP_SNBF	0x0FEC	/* Shiva NetBIOS 0x0FEC */
#define BAD_TYPE_PPP_SIPXCP	0x812B	/* Shiva IPXCP 0x812B */
#define BAD_TYPE_PPP_SNBFCP	0x8FEC	/* Shiva NetBIOS CP 0x8FEC */
#define BAD_TYPE_PPP_OLDSPAP	0xC123	/* (old) Shiva PAP 0xC123 */


#define PPP_NOVCODE   (PPP_STATION << 16) | TYPE_PPP_NOVELL1

#define DIGEST_MD5	5
#define DIGEST_DES	0x80	/* Microsoft extension to CHAP */
#define NUM_AUTH_INSISTS (5+1)	/* # of times to try to negotiate auth */
				/* before quitting. */

/*
 * PPP's local authentication mechanisms
 */
#define PPP_LAUTHEN_TACACS	1	    /* TACACS support */
#define PPP_LAUTHEN_IFNEED	2	    /* Authenticate once only */
#define PPP_LAUTHEN_SINGLELINE	4	    /* username & pass in username */
#define PPP_PT_LAUTHEN_IFNEED   8           /* PT-specific if-needed flag */

/*
 * Frame Checking Defines (CRC16)
 */
typedef unsigned short u16;
#define PPPINITFCS 0xffff  /* Initial FCS value */
#define PPPGOODFCS 0xf0b8  /* Good final FCS value */

/*
 * Options.
 */
#define CI_MRU		1	/* Maximum Receive Unit */
#define CI_ASYNCMAP	2	/* Async Control Character Map */
#define CI_AUTHTYPE	3	/* Authentication Type */
#define CI_QUALITYTYPE	4	/* Quality Protocol Type */
#define CI_MAGICNUMBER	5	/* Magic Number */
#define CI_RESERVED	6	/* formerly Link Quality Monitoring */
#define CI_PCOMPRESSION	7	/* Protocol Field Compression */
#define CI_ACCOMPRESSION 8	/* Address/Control Field Compression */
#define CI_32FCS	9	/* 32 bit FCS */
#define CI_NUMBEREDMODE 11      /* Number-Mode reliable transmission */
#define CI_CALLBACK     13      /* Callback */
#define CI_MULTILINK_MRRU  17   /* Multilink Max Receive Reconstrunted Unit */
                                /*  also Multilink request */
#define CI_ENDPOINT_DISC   19   /* Endpoint discriminator */

/*
 * Endpoint discriminator classes
 */
#define ENDPOINT_NULL        0
#define ENDPOINT_LOCAL       1
#define ENDPOINT_IP_ADDR     2
#define ENDPOINT_IEEE_802    3
#define ENDPOINT_MAGIC       4
#define ENDPOINT_PHONE_NUMBER  5   

/*
 * Callback option types
 */
#define CALLBACK_PPP_NOLOCATION 0   /* Callback Location by User Authen */
#define CALLBACK_PPP_DIALSTRING 1   /* Callback Dialstring */
#define CALLBACK_PPP_LOCATION   2   /* Callback location */   
#define CALLBACK_PPP_E164       3   /* Callback E.164 number */
#define CALLBACK_PPP_DN         4   /* Callback Distinguished name */
#define CALLBACK_PPP_NONEG      5   /* No Callback actually negotiated */

#define DEFMRU	1500		/* Try for this */
#define MINMRU	128		/* No MRUs below this */

/*
 *  CP (LCP, IPCP, etc.) codes.
 */
#define CONFREQ		1	/* Configuration Request */
#define CONFACK		2	/* Configuration Ack */
#define CONFNAK		3	/* Configuration Nak */
#define CONFREJ		4	/* Configuration Reject */
#define TERMREQ		5	/* Termination Request */
#define TERMACK		6	/* Termination Ack */
#define CODEREJ		7	/* Code Reject */
#define PROTREJ		8	/* Protocol Reject */
#define ECHOREQ		9	/* Echo Request */
#define ECHOREP		10	/* Echo Reply */
#define DISCREQ		11	/* Discard Request */
#define IDENTIFY	12	/* Identification */
#define RESETREQ	14	/* Reset Request */
#define RESETACK	15	/* Reset Acknowledge */

#define MAX_LCPTYPES RESETACK
/*
 * Link states.
 */
#define CLOSED		1	/* Connection closed */
#define LISTEN		2	/* Listening for a Config Request */
#define REQSENT		3	/* We've sent a Config Request */
#define ACKSENT		4	/* We've sent a Config Ack */
#define ACKRCVD		5	/* We've received a Config Ack */
#define OPEN		6	/* Connection open */
#define TERMSENT	7	/* We've sent a Terminate Request */
#define PPP_UNKNOWN     8       /* haven't negotiated */


/*
 * Flags.
 */
#define LOWERUP		1	/* The lower level is UP */
#define AOPENDING	2	/* Active Open pending timeout of request */
#define POPENDING	4	/* Passive Open pending timeout of request */
#define WASREJECTED	8	/* Peer proto rejected this proto */

/*
 * Timeouts & Number-of-Attempt constants.
 */
#define DEFTIMEOUT (2*ONESEC)   /* Timeout time in miliseconds */
#define DEFMAXTERMTRANSMITS 2	/* Maximum Terminate-Request transmissions */
#define DEFMAXCONFIGURE     10	/* Maximum CONREQ retransmissions */
#define DEFMAXNAKLOOPS	    10	/* Maximum number of nak loops received */
#define DEFMAXFAILURE	    10  /* Maximum number of naks to send */
#define DEFMAXBADAUTHS      1   /* Hang up after 1 authentication failure */

/* max count value for echo req's sent and haven't received a response */

#define MAX_ECHO_CNT         5  /* max 5 echo req's before informing NCP */
#define MAX_LQR_CNT          5  /* max 5 LQR before informing NCP */

/*
 * PPP options test
 */
#define PPP_TEST_NONE 0
#define PPP_TEST_ASYNC 1
#define PPP_TEST_MLP 2

/*
 * The LCP authentication option is variable sized (CHAP needs a digest value)_
 * These values are actually the size of the protocol value (and optional
 * digest).
 */
#define PPP_CHAP_OPT_SIZE (sizeof(short) + sizeof(char))
#define PPP_PAP_OPT_SIZE  (sizeof(short))

/*
 * LQM history size parameters
 */
#define BITSPERCHAR 8
#define CHECK_PERIOD 16
#define LQM_HISTORY_SIZE ((CHECK_PERIOD+(BITSPERCHAR-1))/BITSPERCHAR)

/*
 * PPP Header definitions 
 */

typedef struct ppp_hdr {
	uchar	code;
	uchar	id;
	ushort  plen;
	uchar   data[0];
} ppp_hdr;

struct fsm_ {
    sys_timestamp timeouttime;	/* Timeout time in milliseconds */
    hwidbtype *idb;	        /* My IDB */
    void *wantoptions;
    void *gotoptions;
    void *allowoptions;
    void *heroptions;
    void *ouroptions;
    ushort protocol;		/* Data Link Layer Protocol field value */
    uchar id;			/* Current id */
    uchar reqid;		/* Current request id */
    int state;			/* State */
    int flags;			/* Flags */
    int retransmits;		/* Number of retransmissions */
    int restarttime;		/* Time between retransmits in seconds */
    int maxconfigure;		/* Maximum Configure-Request transmissions */
    int maxtermtransmits;	/* Maximum Terminate-Request transmissions */
    int nakloops;		/* Number of nak loops since last timeout */
    int maxnakloops;		/* Maximum number of nak loops tolerated */
    int failures;		/* Number of Configure-Naks sent */
    int maxfailure;		/* Maximum number of Configure-Naks to send */
    int max_bad_auths;	        /* Allow Client, these many bad auth tries */
    boolean timerinitfl;	/* Only init timers and counters once per reload */
    struct fsm_callbacks *callbacks;	/* Callback routines */
    /* OLD_AUTH -- remove following line when old authentication goes away */
    int localauthen;		/* Local Authentication Mechanisms ie TACACS */
    char **cached_author;	/* Did authorization get done. */
    boolean ncp_did_author;	/* "Smart" authorization done by NCP. */
    boolean negotiate_callin_only; /* callin authentication only */
    uchar *confack_sent;   /* Last CONFACK sent */
    short confack_sent_len;
    uchar *confack_rcv;   /* Last CONFACK received */
    short confack_rcv_len;
};

/*
 * Each FSM is described by a fsm_callbacks and a fsm structure.
 */
typedef struct fsm_callbacks {
    void (*resetci)(fsm *);	/* Reset our Configuration Information */
    int (*cilen)(fsm *);	/* Length of our Configuration Information */
    void (*addci)(fsm *, uchar *); /* Add our Configuration Information */
    int (*ackci)(fsm *, uchar *, int);/* ACK our Configuration Information */
    void (*nakci)(fsm *, uchar *, int);	/* NAK our Configuration Information */
    void (*rejci)(fsm *, uchar *, int);/* Reject our Config Information */
    uchar (*reqci)(fsm *, uchar *, short *); /* Req peer's Config Info */
    void (*up)(fsm *);		/* Called when fsm reaches OPEN state */
    void (*down)(fsm *);	/* Called when fsm leaves OPEN state */
    void (*closed)(fsm *);	/* Called when fsm reaches CLOSED state */
    void (*protreject)(fsm *);	/* Called when Protocol-Reject received */
    void (*retransmit)(fsm *);	/* Retransmission is necessary */
    boolean (*resetrequest)(fsm *, uchar *, int); /* Reset Requested */
    void (*resetack)(fsm *, uchar, uchar *, int); /* Reset Acknowledge */
    void (*coderejected)(fsm *); /* Called when Code Reject is received */
} fsm_callbacks;



/*
 * The state of options is described by an lcp_options structure.
 * Warning !!! if adding pointers that are assigned via malloc
 *   make sure they are cleaned up in or before lcp_resetci which
 *   does a bcopy from wantoptions to gotoptions
 */
typedef struct lcp_options {
    int neg_mru : 1;		/* Negotiate the MRU? */
    ushort mru;			/* Value of MRU */
    int neg_asyncmap : 1;	/* Async map? */
    ulong asyncmap;
    int neg_auth;		/* authentication? */
    uchar authtypes[PPP_AUTH_NUM];
    int neg_quality : 1;	/* link quality reports? */
    ushort qualitytype;
    ulong reportingperiod;
    int neg_magicnumber : 1;	/* Magic number? */
    ulong magicnumber;
    int neg_pcompression : 1;	/* HDLC Protocol Field Compression? */
    int neg_accompression : 1;	/* HDLC Address/Control Field Compression? */
    int neg_numbered_mode : 1;  /* Numbered Mode negotiation ? */
    int neg_callback :1;        /* Negotiate a callback? */
    uchar callback;             /* Callback Option */
    char *callback_message;     /* Callback Message (variable) */
    uchar num_mode_window;
    uchar num_mode_address;
    int neg_mlp_mrru : 1;
    ushort mlp_mrru;			/* Value of multilink mrru */
    int neg_endpoint : 1;
    uchar endpoint_class;
    uchar endpoint_size;
    uchar *endpoint;            /* Endpoint discriminator class and name */
    uchar *endpoint_string;
} lcp_options;

/*
 * Additional lcp state and policies are described by an lcp_state structure.
 */
struct lcp_state_ {
    hwidbtype *idb;		/* my idb */
    int phase;			/* Phase */
    int passive : 1;		/* Passive vs. active open */
    int restart : 1;		/* Restart vs. exit after close */
    sys_timestamp timeouttime;	/* timer for lost inbound reports */
    sys_timestamp link_transition; /* timer for interface transition */

    /* Echo request parameters */    
    int echo_flag;              /* set to TRUE when sent, reset after rcv */
    int echo_cnt;               /* my counter for max echo req's sent */ 
    uchar echo_id;             /* echo id */

    /* Link Quality Monitoring State */
    int poorquality : 1;	/* Is the Link Quality is ok? */
    int loopedback : 1;		/* Is the link currently looped back? */
    int lqrrcvd : 1;		/* Has the first LQR been received? */
    int lqrrcvdthisperiod : 1;	/* has an LQR been received this period? */
    int validlqrrcvd : 1;	/* Has the first LQR with a valid inbound
				   link quality report been received? */
    int inlqrvalid : 1;		/* Is the inbound link quality report valid? */
    ulong packetsrx;           /* they've received for show interface */
    ulong octetsrx;            /* they've received for show interface */
    ulong packetstx;           /* we've sent for show interface */
    ulong octetstx;            /* we've sent for show interface */

    ulong LastOutLQRs;         /* copy of last rcvd PeerOutLQRs    */ 
    ulong LastOutPackets;      /* copy of last rcvd PeerOutPackets */ 
    ulong LastOutOctets;       /* copy of last rcvd PeerOutOctets  */  
    ulong PeerInLQRs;          /* copy of SaveInLQRs     my rcv info */ 
    ulong PeerInPackets;       /* copy of SaveInPacket   my rcv info */ 
    ulong PeerInDiscards;      /* copy of SaveInDiscards my rcv info */
    ulong PeerInErrors;        /* copy of SaveInErrors   my rcv info */
    ulong PeerInOctets;        /* copy of SaveInOctets   my rcv info */
    ulong PeerOutLQRs;         /* copy of OutLQRs        my xmt info */ 
    ulong PeerOutPackets;      /* copy of XXXOutPacktets my xmt info */ 
    ulong PeerOutOctets;       /* copy of OutOctets      my xmt info */ 
    ulong SaveInLQRs;          /* copy of PeerInLQRs from rcv LQR     */ 
    ulong SaveInPackets;       /* copy of PeerInPackets from rcv LQR  */ 
    ulong SaveInDiscards;      /* copy of PeerInDiscards from rcv LQR */ 
    ulong SaveInErrors;        /* copy of PeerInErrors from rcv LQR   */ 
    ulong SaveInOctets;        /* copy of PeerInOctets from rcv LQR   */ 

    ulong cLastOutLQRs;         /* copy of last rcvd LastOutLQRs    */
    ulong cLastOutPackets;      /* copy of last rcvd LastOutPackets */
    ulong cLastOutOctets;       /* copy of last rcvd LastOutOctets  */
    ulong cPeerInLQRs;          /* copy of last rcvd PeerInLQRs     */
    ulong cPeerInPackets;       /* copy of last rcvd PeerInPacket   */
    ulong cPeerInDiscards;      /* copy of last rcvd PeerInDiscards */
    ulong cPeerInErrors;        /* copy of last rcvd PeerInErrors   */
    ulong cPeerInOctets;        /* copy of last rcvd PeerInOctets   */
    ulong cPeerOutLQRs;         /* copy of last rcvd PeerOutLQRs    */
    ulong cPeerOutPackets;      /* copy of last rcvd PeerOutPacktets */
    ulong cPeerOutOctets;       /* copy of last rcvd PeerOutOctets   */
    ulong cSaveInPackets;       /* copy of values at time of rcv */ 
    ulong cSaveInDiscards;      /* copy of values at time of rcv */ 
    ulong cSaveOutPackets;       /* copy of values at time of rcv */ 
    ulong cSaveOutOctets;      /* copy of values at time of rcv */ 
    ulong cSaveInErrors;        /* copy of values at time of rcv */ 
    ulong cSaveInOctets;        /* copy of values at time of rcv */ 

    ulong OutLQRs;		/* LQR xmit sequence number */
    ulong InLQRs;		/* LQR recv sequence number */
    ulong InGoodOctets;	/* error free octet count  tot - err = good */
    ulong InDiscards;	        /* Discard packets received */
    ulong InitOutPack;	        /* Initial count of output packets */
    ulong InitOutOct;	        /* Initial count of output octets  */
    ulong InitInPack;	        /* Initial count of input packets  */
    ulong InitInOct;           /* Initial count of input octets   */
    ulong rxpackets;           /* Number of packets received      */
    ulong rxoctets;            /* Number of octets received       */

    /* Link Quality Monitoring Policy State/Parameters */
    struct {
	uchar k, n;		/* Must have K successes in N periods */
	uchar history[LQM_HISTORY_SIZE]; /* Success/fail history (N periods) */
	uchar index;		/* Current index into history */
	uchar successes;	/* Number of successes in history */
    } in, out;

    /* OLD_AUTH -- remove following line when old authentication goes away */
    char *user;			/* Username authenticated by tacacs */
    char acc_list_name[32];	/* Authentication list name */
    userstruct *ustruct;	/* New AAA user info */

    /*
     * The next set of usernames/passwords are kept in the master IDB for a
     * dialer group or ISDN interface.  The passwords are always encrypted.
     */
    char *chap_name;		/* alternate name to use for CHAP */
    char *chap_pw;		/* default CHAP password */
    char *pap_sent_name;	/* name used for outbound PAP authentication */
    char *pap_sent_pw;		/* outbound PAP authentication password */
 
    /* Multilink config stuff */
    int mlp_max_links;
    int mlp_min_links;
    int mlp_max_frags;
    uchar mlp_threshold;
    uchar mlp_threshold_direction;
    tinybool mlp_threshold_confg;

    /*
     * Moved to here to minimize alignment issues...
     */
    uchar ppp_quality_threshold; /* percent we must receive */ 

    /*
     * CHAP extensions required to close CHAP security holes
     *
     */
    uchar chap_refuse;		/* set if CHAP to peer disabled */
    uchar call_direction;	/* Call in, call out, or no idea */
    uchar cfg_direction;	/* if set, overrides call_direction */
    tinybool chap_nowait;	/* set if we callee should respond ASAP */
    tinybool chap_noignoreus;	/* set if peer can use the same name as us */
    tinybool chap_split_names;	/* set if Chal & Resp names can be different */

    uchar chap_auto_secrets;	/* set if auto expansion of the secret is on */

    uchar chap_rsvd;		/* this space for rent [alignment] */

    /*
     * The PPP negotiation timeout is designed to catch connections that
     * have gotten hung up in some state in LCP, authentication or an NCP.
     * It is started when LCP is started, checked periodically, and cleared
     * when LCP closes or a significant NCP opens or it joins a pre-existing
     * Multilink bundle or it is forwarded.
     */
    int neg_timeout;		/* configured timeout period in msecs */
    sys_timestamp negtime;	/* timer for hung/failing negotiations */
};

/*
 * LCP phases.
 *
 * A VIRTUALIZED interface is never UP because the significance of UP
 * is that upper-level protocols (NCPs or Network Control Protocols) are
 * allowed to run.  Since a virtualized interface is a physical interface
 * whose NCPs are run on a virtual interface, the physical interface is marked
 * VIRTUALIZED and the virtual interface is marked UP.  The prime example
 * of a virtual interface is the multilink master, which used to be the first
 * physical interface connected and is now a virtual interface.
 */
#define DOWN		0	/* Phase 0: Down */
#define ESTABLISHING	1	/* Phase 1: Link Establishment */
#define LQD		2	/* Phase 2: Link Quality Determination */
#define AUTHENTICATING	3	/* Phase 2+: Authentication */
#define FORWARDING      4       /* Phase 2++  VPN Link Forwarding */
#define FORWARDED       5       /* Phase 2+++ VPN Link Forwarded */ 
#define VIRTUALIZED     6       /* Phase 2+++ Virtualized Interface */
#define UP		7	/* Phase 3: Network-Layer Protocols */
#define TERMINATING	8	/* Phase 4: Link Termination */
#define IS_PPP_AUTH(idb) ((idb) && 					\
			  (idb)->lcpfsm && 				\
			  (idb)->lcpfsm->wantoptions &&			\
  	       ((lcp_options *)((idb)->lcpfsm)->wantoptions)->authtypes[0]) 

/*
 * Inline versions of get/put char/short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be uchar *.
 */
#define GETCHAR(c, cp) { \
	(c) = *(cp)++; \
}
#define PUTCHAR(c, cp) { \
	*(cp)++ = (c); \
}

#define PPP_GETSHORT(s, cp) { \
	s = ntohs(GETSHORT(cp)); \
	cp += sizeof(ushort); \
}
#define PPP_PUTSHORT(s, cp) { \
	PUTSHORT(cp, htons(s)); \
	cp += sizeof(ushort); \
}

#define PPP_GETLONG(l, cp) { \
	l = ntohl(GETLONG(cp)); \
	cp += sizeof(ulong); \
}
#define PPP_PUTLONG(l, cp) { \
	PUTLONG(cp, htonl(l)); \
	cp += sizeof(ulong); \
}

#define INCPTR(n, cp)	((cp) += (n))
#define DECPTR(n, cp)	((cp) -= (n))

#define LENCIVOID(neg) (neg ? 2 : 0)
#define LENCICHAR(neg)  (neg ? 3 : 0)
#define LENCISHORT(neg)  (neg ? 4 : 0)
#define LENCILONG(neg)  (neg ? 6 : 0)
#define LENCIAUTH(neg, val) (!neg ? 0 : (val == PPP_AUTH_CHAP) ? 5 : 4)
#define LENCIQUAL(neg)	(neg ? 8 : 0)
#define LENCINMODE(neg) (neg ? 4 : 0)
#define LENCIENDPOINT(neg, size)  (neg ? (2 + 1 + size) : 0)

#define ADDCIVOID(opt, neg) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: sending CONFREQ, type = %d (%s)", \
		 opt, #opt); \
    }

#define ADDCICHAR(opt, neg, val) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + sizeof(uchar), ucp); \
	PUTCHAR(val, ucp); \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x", \
		   opt, #opt, val); \
    }

#define ADDCISHORT(opt, neg, val) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + sizeof(short), ucp); \
	PPP_PUTSHORT(val, ucp); \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x", \
		   opt, #opt, val); \
    }

#define ADDCILONG(opt, neg, val) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + sizeof(long), ucp); \
	PPP_PUTLONG(val, ucp); \
        if (ppp_negotiation_debug) \
	  buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x", \
		   opt, #opt, val); \
    }

#define ADDCIAUTH(opt, neg, val) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	if (val == PPP_AUTH_CHAP) { \
 	    PUTCHAR(2 + sizeof(short) + sizeof(char), ucp); \
	    PPP_PUTSHORT(TYPE_PPP_CHAP, ucp); \
	    PUTCHAR(DIGEST_MD5,ucp); \
	    if (ppp_negotiation_debug) \
	      buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x/%d", \
		     opt, #opt, TYPE_PPP_CHAP, DIGEST_MD5); \
	} else { \
 	    PUTCHAR(2 + sizeof(short), ucp); \
	    PPP_PUTSHORT(TYPE_PPP_UPAP, ucp); \
	    if (ppp_negotiation_debug) \
	      buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x", \
		     opt, #opt, TYPE_PPP_UPAP); \
	} \
    }

#define ADDCIQUAL(opt, neg, value1, value2) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
 	PUTCHAR(2 + sizeof(short) + sizeof(long), ucp); \
	PPP_PUTSHORT(value1, ucp); \
	PPP_PUTLONG(value2,ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: sending CONFREQ, type = %d (%s), value = %#x/%#x", \
		 opt, #opt, value1, value2); \
    }

#define ADDCINUMBEREDMODE(opt, neg, value1, value2) \
    if (neg) { \
    PUTCHAR(opt, ucp); \
    PUTCHAR(4, ucp); \
    PUTCHAR(value1, ucp); \
    PUTCHAR(value2, ucp); \
    if (ppp_negotiation_debug) \
      buginf("\nppp: sending CONFREQ, type = %d (%s), value = %d/%d", \
         opt, #opt, value1, value2); \
    }

#define ADDCIENDPOINT(opt, neg, pchar, size) \
    if (neg) { \
	PUTCHAR(opt, ucp); \
	PUTCHAR(2 + 1 + size, ucp); \
	PUTCHAR(ENDPOINT_LOCAL, ucp); \
	bcopy(pchar, ucp, size); \
        INCPTR(size, ucp); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: sending CONFREQ, type = %d (%s)", \
		 opt, #opt); \
    }

#define ACKCIVOID(opt, neg) \
    if (neg) { \
	if ((len -= 2) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 || \
	    citype != opt) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s)", \
		   opt, #opt); \
    }

#define ACKCICHAR(opt, neg, val) \
    if (neg) { \
	if ((len -= 2 + sizeof(uchar)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + sizeof(uchar) || \
	    citype != opt) \
	    goto bad; \
	GETCHAR(cichar, p); \
	if (cichar != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s), value = %#x", \
		   opt, #opt, \
 		   cichar); \
    }

#define ACKCISHORT(opt, neg, val) \
    if (neg) { \
	if ((len -= 2 + sizeof(short)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + sizeof(short) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(cishort, p); \
	if (cishort != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s), value = %#x", \
		   opt, #opt, \
 		   cishort); \
    }
#define ACKCILONG(opt, neg, val) \
    if (neg) { \
	if ((len -= 2 + sizeof(long)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + sizeof(long) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETLONG(cilong, p); \
	if (cilong != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s), value = %#x", \
		   opt, #opt, \
		   cilong); \
    }
#define ACKCIAUTH(opt, neg, val) \
    if (neg) { \
	GETCHAR(citype, p); \
	if (citype != opt) \
	    goto bad; \
	GETCHAR(cilen, p); \
	if (len < cilen) \
	    goto bad; \
	len -= cilen; \
	PPP_GETSHORT(cishort, p); \
	if (val == PPP_AUTH_CHAP) { \
	    if (cilen != 2 + sizeof(short) + sizeof(char)) \
	        goto bad; \
	    if (cishort != TYPE_PPP_CHAP) \
	        goto bad; \
            GETCHAR(cichar, p); \
	    if (cichar != DIGEST_MD5) \
	        goto bad; \
	} else { \
	    if (cilen != 2 + sizeof(short)) \
	        goto bad; \
	    if (cishort != TYPE_PPP_UPAP) \
	        goto bad; \
	} \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s), value = %#x", \
		   opt, #opt, \
		   cishort); \
    }

#define ACKCIQUAL(opt, neg, value1, value2) \
    if (neg) { \
	if ((len -= 2 + sizeof(short) + sizeof(long)) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != 2 + sizeof(short) + sizeof(long) || \
	    citype != opt) \
	    goto bad; \
	PPP_GETSHORT(cishort, p); \
	if (cishort != value1) \
	    goto bad; \
        PPP_GETLONG(cilong, p); \
	if (cilong != value2) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s), value = %#x", \
		   opt, #opt, \
		   cishort); \
    }

#define ACKCINUMBEREDMODE(opt, neg, value1, value2) \
    if (neg) { \
    if ((len -= 4) < 0) \
        goto bad; \
    GETCHAR(citype, p); \
    GETCHAR(cilen, p); \
    if (cilen != 4 || \
        citype != opt) \
        goto bad; \
    GETCHAR(cichar, p); \
    if (cichar != value1) \
        goto bad; \
    GETCHAR(cichar, p); \
    if (cichar != value2) \
        goto bad; \
    if (ppp_negotiation_debug) \
        buginf("\nppp: config ACK received, type = %d (%s), (%d/%d)", \
           opt, #opt, value1, value2); \
    }


#define ACKCIENDPOINT(opt, neg) \
    if (neg) { \
	if ((len -= 2) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen < 4 ||  \
	    citype != opt) \
	    goto bad; \
        len -= (cilen - 2);  \
	INCPTR((cilen - 2), p); \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config ACK received, type = %d (%s)", \
		   opt, #opt); \
    }

    /*
     * any nak'd cis must be in exactly the same order that we sent.
     * check packet length and ci length at each step.
     * if we find any deviations, then this packet is bad.
     */

#define NAKCIVOID(opt, neg, code) \
    if (neg && \
	len >= 2 && \
	p[1] == 2 && \
	p[0] == opt) { \
	len -= 2; \
	INCPTR(2, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config NAK received, type = %d (%s)", \
		 opt, #opt); \
	code \
    }

#define NAKCICHAR(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(uchar) && \
	p[1] == 2 + sizeof(uchar) && \
	p[0] == opt) { \
	len -= 2 + sizeof(uchar); \
	INCPTR(2, p); \
	GETCHAR(cichar, p); \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config NAK received, type = %d (%s), value = %#x", \
		   opt, #opt, cichar); \
	code \
    }

#define NAKCISHORT(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(short) && \
	p[1] == 2 + sizeof(short) && \
	p[0] == opt) { \
	len -= 2 + sizeof(short); \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config NAK received, type = %d (%s), value = %#x", \
		   opt, #opt, cishort); \
	code \
    }
#define NAKCILONG(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(long) && \
	p[1] == 2 + sizeof(long) && \
	p[0] == opt) { \
	len -= 2 + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(cilong, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config NAK received, type = %d (%s), value = %#x", \
		 opt, #opt, cilong); \
	code \
    }
#define NAKCIAUTH(opt, neg, val, code) \
    if (neg && \
	p[0] == opt && \
	p[1] <= len && \
	p[1] >= (2 + sizeof(short))) { \
	int optlen = p[1]; \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
	if ((cishort == TYPE_PPP_CHAP) && \
	    optlen >= 2 + sizeof(short) + sizeof(char)) { \
	    GETCHAR(cichar1, p); \
	    INCPTR((optlen - (2 + sizeof(short) + sizeof(char))), p); \
	} else { \
	    cichar1 = 0; \
	    INCPTR((optlen - (2 + sizeof(short))), p); \
	} \
	len -= optlen; \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config NAK received, type = %d (%s), value = %#x/%d", \
		 opt, #opt, \
		 cishort, cichar1); \
	code \
    }
#define NAKCIQUAL(opt, neg, code) \
    if (neg && \
	len >= 2 + sizeof(short) + sizeof(long)&& \
	p[1] == 2 + sizeof(short) + sizeof(long) && \
	p[0] == opt) { \
	len -= 2 + sizeof(short) + sizeof(long); \
	INCPTR(2, p); \
	INCPTR(2, p); \
	PPP_GETLONG(cilong, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config NAK received, type = %d (%s), value = %#x", \
		 opt, #opt, cilong); \
	code \
    }

#define NAKCINUMBEREDMODE(opt, neg, code) \
    if (neg && len >= 4 && p[1] == 4 && p[0] == opt) { \
    len -= 4; \
    INCPTR(2, p); \
    GETCHAR(cichar1, p); \
    GETCHAR(cichar2, p); \
    if (ppp_negotiation_debug) \
      buginf("\nppp: config NAK received, type = %d (%s), value = %d/%d", \
         opt, #opt, cichar1, cichar2); \
    code \
    }

#define NAKCIENDPOINT(opt, neg, code) \
    if (neg && \
	len >= 2 && \
	p[1] <= len && \
	p[0] == opt) { \
	len -= p[1]; \
	INCPTR(p[1], p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config NAK received, type = %d (%s)", \
		 opt, #opt); \
	code \
    }

    /*
     * any rejected cis must be in exactly the same order that we sent.
     * check packet length and ci length at each step.
     * if we find any deviations, then this packet is bad.
     */
#define REJCIVOID(opt, neg) \
    if (neg && \
	len >= 2 && \
	p[1] == 2 && \
	p[0] == opt) { \
	len -= 2; \
	INCPTR(2, p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config REJ received, type = %d (%s)", \
		 opt, #opt); \
	neg = 0; \
    }
#define REJCICHAR(opt, neg, val) \
    if (neg && \
	len >= 2 + sizeof(uchar) && \
	p[1] == 2 + sizeof(uchar) && \
	p[0] == opt) { \
	len -= 2 + sizeof(uchar); \
	INCPTR(2, p); \
	GETCHAR(cichar, p); \
	/* check rejected value. */ \
	if (cichar != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config REJ received, type = %d (%s), value = %#x", \
		   opt, #opt, cichar); \
	neg = 0; \
    }
#define REJCISHORT(opt, neg, val) \
    if (neg && \
	len >= 2 + sizeof(short) && \
	p[1] == 2 + sizeof(short) && \
	p[0] == opt) { \
	len -= 2 + sizeof(short); \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
	/* check rejected value. */ \
	if (cishort != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config REJ received, type = %d (%s), value = %#x", \
		   opt, #opt, cishort); \
	neg = 0; \
    }
#define REJCILONG(opt, neg, val) \
    if (neg && \
	len >= 2 + sizeof(long) && \
	p[1] == 2 + sizeof(long) && \
	p[0] == opt) { \
	len -= 2 + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETLONG(cilong, p); \
	/* check rejected value. */ \
	if (cilong != val) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config REJ received, type = %d (%s), value = %#x", \
		   opt, #opt, cilong); \
	neg = 0; \
    }

#define REJCIQUAL(opt, neg, value1, value2) \
    if (neg && \
	len >= 2 + sizeof(short) + sizeof(long) && \
	p[1] == 2 + sizeof(short) + sizeof(long) && \
	p[0] == opt) { \
	len -= 2 + sizeof(short) + sizeof(long); \
	INCPTR(2, p); \
	PPP_GETSHORT(cishort, p); \
	/* check rejected value. */ \
	if (cishort != value1) \
	    goto bad; \
	PPP_GETLONG(cilong, p); \
	if (cilong != value2) \
	    goto bad; \
        if (ppp_negotiation_debug) \
	    buginf("\nppp: config REJ received, type = %d (%s), value = %#x/%#x", \
		   opt, #opt, \
		   cishort, cilong); \
	neg = 0; \
    }


#define REJCINUMBEREDMODE(opt, neg, value1, value2) \
    if (neg && len >= 4 && p[1] == 4 && p[0] == opt) { \
    len -= 4; \
    INCPTR(2, p); \
    GETCHAR(cichar, p); \
    /* check rejected value. */ \
    if (cichar != value1) \
        goto bad; \
    GETCHAR(cichar2, p); \
    if (cichar2 != value2) \
        goto bad; \
    if (ppp_negotiation_debug) \
        buginf("\nppp: config REJ received, type = %d (%s), value = %d/%d", \
                opt, #opt, cichar, cichar2); \
    neg = 0; \
    }

#define REJCIENDPOINT(opt, neg) \
    if (neg && \
	len >= 2 && \
	p[1] <= len && \
	p[0] == opt) { \
	len -= p[1]; \
	INCPTR(p[1], p); \
	if (ppp_negotiation_debug) \
	  buginf("\nppp: config REJ received, type = %d (%s)", \
		 opt, #opt); \
	neg = 0; \
    }

/*
 * ppp_get_protocol_type_ptr()
 *	Parse PPP header, return pointer to protocol type field
 *
 * The value at this location may be a byte or a word.  This routine is
 * only really useful for generating reject packets.
 *
 * NOTE: if you change this, change ppp_get_protocol_type() below.
 */
static inline uchar *
ppp_get_protocol_type_ptr (paktype *pak)
{
    switch (pak->encsize) {
    case 1:
    case 3:
	return(pak->network_start - sizeof(uchar));
    case 2:
    case 4:
	return(pak->network_start - sizeof(ushort));
    default:
	return(0);	/* Illegal */
    }
}

/*
 * ppp_get_protocol_type()
 *	Figure out format of PPP header, return protocol type value
 */
static inline uint
ppp_get_protocol_type (paktype *pak)
{
    /*
     * Dig up link type based on encapsulation size.  2 and 4 byte
     * encapsulations have a 2-byte type, 1 and 3 have a single
     * byte.  In either case, the type is the last field before the
     * network data portion of the packet.
     */
    switch (pak->encsize) {
    case 1:
    case 3:
	return(*(pak->network_start - sizeof(uchar)));
    case 2:
    case 4:
	return(GETSHORT(pak->network_start - sizeof(ushort)));
    default:
    	return(0);	/* ILLEGAL */
    }
}

/*
 * ppp_get_header_code()
 *	First byte of network data is the code field for all xxxCPs.
 */
static inline uchar
ppp_get_header_code (paktype *pak)
{
    return(*(uchar *)pak->network_start);
}

/*
 * ppp_decode_rxtype_inline.
 * This code is used in the ppp_unknown_fastswitch case to try
 * and classify packets that are ignored by the microcode (i.e
 * LEX.
 */
static inline ushort
ppp_decode_rxtype_inline (ushort summary)
{
    ushort rxtype;
    
    switch (summary) {
    case TYPE_PPP_IP:
	rxtype = RXTYPE_IPUNC;
	break;
    case TYPE_PPP_CLNS:
	rxtype = RXTYPE_CLNS;
	break;
    case TYPE_PPP_XNS:
	rxtype = RXTYPE_XNS;
	break;
    case TYPE_PPP_DECNET:
	rxtype = RXTYPE_DECNET;
	break;
    case TYPE_PPP_ETHERTALK:
	rxtype = RXTYPE_APPLE;
	break;
    case TYPE_PPP_NOVELL1:
	rxtype = RXTYPE_NOVELL1;
	break;
    case TYPE_PPP_VINES:
	rxtype = RXTYPE_VINES;
	break;
    case TYPE_PPP_LEX:
	rxtype = RXTYPE_LEX;
	break;
    default:
	rxtype = RXTYPE_UNKNOWN;
    }
    return(rxtype);
}

static inline boolean is_mlp_enabled (hwidbtype *idb)
{
  lcp_options *wo;

  if (idb->lcpfsm) {
    wo = idb->lcpfsm->wantoptions;
    return(wo->neg_mlp_mrru);
  }
  return(FALSE);
}

/*
 *  ppp_is_callback_negotiated
 *  return(TRUE) if the callback option in ppp was successfully negotiated
 */

static inline boolean ppp_is_callback_negotiated (hwidbtype *idb)
{
    lcp_options *ho, *go;

    ho = idb->lcpfsm->heroptions;
    go = idb->lcpfsm->gotoptions;
    return(go->neg_callback || 
	   ho->neg_callback);
}

extern const char *const ppp_linkstates[];
extern const char *const ppp_packettypes[];

extern boolean auth_method_supported(fsm *, uchar);
extern void fsm_activeopen(fsm *);
extern void fsm_close(fsm *);
extern void fsm_init(fsm *);
extern void fsm_input(fsm *, paktype *);
extern void fsm_lowerdown(fsm *);
extern void fsm_lowerup(fsm *);
extern void fsm_passiveopen(fsm *);
extern void fsm_sconfreq(fsm *);
extern void fsm_sprotrej(ushort, uchar *, int, fsm *);
extern void lcp_close(fsm *);
extern void lcp_lowerup(fsm *);
extern void lcp_authcomplete(hwidbtype *, usernametype *);
extern void ppp_send_packet(ushort, uchar, uchar, paktype *,
			    short, hwidbtype *);
extern void ppp_dataprint(char *, uchar *, short, const char *const *, int);
extern fsm *ppp_link2fsm(long, hwidbtype *);
extern int ppp_serial2link(long);
extern fsm *ppp_serial2fsm(long, hwidbtype *);
extern void ppp_parser_init(void);
extern iqueue_t ppp_parse_packet(hwidbtype *, paktype *, uchar *);
extern void ppp_enqueue(paktype *pak);
extern u16 pppfcs(register u16 fcs, register unsigned char *cp,
					register int len);
extern uchar *ppp_get_remote_name(hwidbtype *);

#endif /* __PPP_H__ */
