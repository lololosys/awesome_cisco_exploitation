/* $Id: ether.h,v 3.6.10.5 1996/05/17 20:53:18 wmay Exp $
 * $Source: /release/112/cvs/Xsys/if/ether.h,v $
 *------------------------------------------------------------------
 * ether.h  -- Ethernet/IEEE802.3 definitions
 *
 * 1-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether.h,v $
 * Revision 3.6.10.5  1996/05/17  20:53:18  wmay
 * CSCdi57354:  Extra prototype in if/ether.h - remove it.
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/05/10  01:38:21  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/09  14:29:50  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.10.2.4.1  1996/04/27  07:03:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.10.2  1996/04/22  18:12:38  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.6.10.1.8.1  1996/04/08  01:54:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.10.1  1996/03/18  20:13:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.1  1996/03/22  09:37:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/25  03:51:54  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.5.8.1  1996/03/05  06:13:45  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.4.1  1996/02/27  21:08:15  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/30  17:29:46  bcole
 * CSCdi47620:  IP ARP does not support full SMDS addresses
 *
 * Fix ARP code to handle 8 byte SMDS addresses in SOME cases.
 * (Code still assumes 6 byte layer 2 addresses in other less critical
 * cases.)
 *
 * Revision 3.4.16.1  1996/01/24  22:18:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4.6.1  1996/02/13  00:18:33  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.4  1995/12/06  18:29:43  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  09:26:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:02:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/09  00:52:22  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.1  1995/06/07  20:51:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ETHER_H__
#define __ETHER_H__
 
#include "ieee.h"

# define MINETHERSIZE 60		/* smallest 10MB packet, w/ encaps */
# define MAXETHERSIZE 1500		/* largest 10MB packet, w/o encaps */

# define ETHER_ARPA_ENCAPBYTES 14	/* total bytes of Arpa encapsulation*/
# define ETHER_ARPA_ENCAPWORDS (ETHER_ARPA_ENCAPBYTES / 2)
# define ETHER_8023_ENCAPBYTES 14	/* total bytes of 802.3 encapsulation*/
# define ETHER_8023_ENCAPWORDS (ETHER_8023_ENCAPBYTES / 2)
# define ETHER_SAP_ENCAPBYTES  17	/* total bytes of ISO1 encapsulation*/
# define ETHER_SNAP_ENCAPBYTES 22	/* total bytes of ISO2 encapsulation*/
# define ETHER_ISO3_ENCAPBYTES 16	/* total bytes of ISO3 encapsulation*/
# define NOVELL_ETHER_ENCAPBYTES 14	/* total bytes of Novell Ether encap */
# define ETALK_ENCAPBYTES 17		/* total bytes of Ethertalk encaps.*/
# define APOLLO_ENCAPBYTES 44		/* total bytes for apollo encaps */
# define APOLLO_EXTRABYTES 30		/* apollo private overhead */
#define ETHER_BAGGAGE         20        /* baggage, in bytes, of ethernet */
#define ETHER_OVERHEAD_BYTES  24        /* overhead, in bytes, of ethernet */

#define FCS8023_SIZE            4       /* Size of 802.3 FCS */

/*
 * Ethernet header format
 */
typedef struct ether_hdr_ {
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    ushort type_or_len;
    uchar  data[0];
} ether_hdr;

/*
 * Definitions for encapsulation fields
 */
#define	ARPADST		0
#define	ARPASRC 	6
#define ARPATYPE 	12

#define	ISO1DST		0
#define	ISO1SRC		6
#define	ISO1LEN		12
#define	ISO1DSAP	14
#define	ISO1LSAP	15
#define	ISO1CTL		16

#define	ISO2DST		0
#define	ISO2SRC		6
#define	ISO2LEN		12
#define	ISO2DSAP	14
#define	ISO2LSAP	15
#define	ISO2CTL		16
#define	ISO2SNAP	17
#define	ISO2TYPE	20

#define	ETALKETHERDST		0
#define	ETALKETHERSRC		6
#define	ETALKETHERTYPE		12
#define	ETALKETHERDSTADDR	14
#define	ETALKETHERSRCADDR	15
#define	ETALKLAPTYPE		16

/*
 * Novell uses a weird encapsulation on ethernet that uses an 802.3 length
 * field, but does not have a dsap or lsap. Instead, an XNS datagram
 * immediately follows.  Fortunately, Novell uses FFFF as the XNS checksum,
 * and we can check for this to recognize Novell packets on an Ethernet.
 */

typedef struct ether_novell_ {
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    ushort length;
    ushort data[0];		/* 1st word is checksum */
} ether_novell;

#define ETHER_NOVELL_ENCAPBYTES 14

/*
 * Apollo domain uses a weird encapsulation on ethernet that has 30 bytes
 * of "private" data.  Other than that it looks like an ARPA packet.
 */
#define APOLLOPRIVATELEN 30

typedef struct ether_apollo_ {
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    ushort type;
    uchar  private[APOLLOPRIVATELEN];
} ether_apollo;

#define ETHER_APOLLO_ENCAPBYTES 44

/* 
 * Type 80d5 Ethernet V2 headers (for translational bridging)
 */
typedef struct {
    uchar daddr[IEEEBYTES];	/* Destination address */
    uchar saddr[IEEEBYTES];	/* Source address */
    ushort type;		/* Type field (should always be 80d5) */
    ushort len;			/* Length or type code */
    uchar pad;			/* Annoying pad character */
} ether_mac80d5_hdr;
#define ETHER_80D5_TYPE (0x80d5)
#define ETHER_80D5_LENGTH (17)
#define ETHER_80D5_EXTRA_BYTES (3)

typedef struct {
    ether_hdr canonical_mac;
    uchar bs_da[IEEEBYTES];
    uchar bs_sa[IEEEBYTES];
    boolean do_snap_snap;
} multi_media_bridge_mac_t;

/*
 * Loopback packet definitions
 */
#define	LOOP_REPLY	1
#define	LOOP_FORWARD	2

/*
 * Appletalk LAP packet types  -  Defined here to avoid requiring atalk.h
 */
#define ATALK_LAP_SDDP	1
#define ATALK_LAP_XDDP  2
#define ATALK_LAP_EXTRABYTES 3

/*
 * An ethernet MAC address is multicast if the most significant byte
 * (element 0 in the MAC array) is odd.
 */
#define	ETHER_MAC_MULTICAST	0x01

/*
 * Special typedef for ethernet and fddi multicasting.
 */
typedef union ethermultiaddrtype_ {
    uchar addr[IEEEBYTES];
    struct {
       unsigned       : 24;
       unsigned       : 1;
#define	bottom23 funky.bot23
       unsigned PACKED(bot23 : 23);
    } funky;
} ethermultiaddrtype;

#define	IPMULTICAST_EP	0x01005e00		/* MS 25 bits of MAC addr */
#define	ETHER_IP_MCAST	    {{ 0x01, 0x00, 0x5e, 0x00, 0x00, 0x00 }}
#define	ETHER_IP_MCAST_MASK {{ 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff }}

/*
 * Media types defines -- used in ethernet sub-block field "media_type"
 */
enum MEDIA_TYPE {
    ETHER_MEDIA_TYPE_AUI=1,
    ETHER_MEDIA_TYPE_10BASET=2,
    ETHER_MEDIA_TYPE_FEIP_RJ45=4,
    ETHER_MEDIA_TYPE_FEIP_MII=5
};

enum ETHER_HW {

    /*
     * List of fields in ether subblock
     */
    ETHER_HW_OUTPUT_ONE_COLLISION=IDB_TYPE_BASE_ETHER+1,
    ETHER_HW_OUTPUT_MORE_COLLISION,
    ETHER_HW_OUTPUT_LATE_COLLISION,
    ETHER_HW_OUTPUT_EXCESSIVE_COLLISION,
    ETHER_HW_OUTPUT_DEFERRED,
    ETHER_HW_OUTPUT_LOST_CARRIER,
    ETHER_HW_OUTPUT_BABBLES,

    ETHER_HW_SI_OUTPUT_ONE_COLLISION,
    ETHER_HW_SI_OUTPUT_MORE_COLLISION,
    ETHER_HW_SI_OUTPUT_LATE_COLLISION,
    ETHER_HW_SI_OUTPUT_EXCESSIVE_COLLISION,
    ETHER_HW_SI_OUTPUT_DEFERRED,
    ETHER_HW_SI_OUTPUT_LOST_CARRIER,
    ETHER_HW_SI_OUTPUT_BABBLES,

    ETHER_HW_FEIP_NO_CARRIER,
    ETHER_HW_FEIP_MULTICAST,
    ETHER_HW_FEIP_WATCHDOG,
    ETHER_HW_FEIP_FULLDUPLEX,

    ETHER_HW_SI_FEIP_NO_CARRIER,
    ETHER_HW_SI_FEIP_MULTICAST,
    ETHER_HW_SI_FEIP_WATCHDOG,

    ETHER_HW_MEDIA_TYPE,

    /*
     * Special codes
     */
    ETHER_HW_OUTPUT_ONE_COLLISION_STAT,
    ETHER_HW_OUTPUT_MORE_COLLISION_STAT,
    ETHER_HW_OUTPUT_LATE_COLLISION_STAT,
    ETHER_HW_OUTPUT_EXCESSIVE_COLLISION_STAT,
    ETHER_HW_OUTPUT_DEFERRED_STAT,
    ETHER_HW_OUTPUT_LOST_CARRIER_STAT,
    ETHER_HW_OUTPUT_BABBLES_STAT,
    ETHER_HW_FEIP_NO_CARRIER_STAT,
    ETHER_HW_FEIP_MULTICAST_STAT,
    ETHER_HW_FEIP_WATCHDOG_STAT,    

    ETHER_HW_CLEAR_STATS
};
/**********************************************************************
 *
 * Prototypes
 *
 **********************************************************************/
boolean ether_odd_byte_start_bug(hwidbtype *);
boolean ether_vencap(paktype *, long);
boolean parse_ether_address(char *ptr, uchar result[]);
char *get_hex_num(char *, ushort *);
long ether_getlink(paktype *);
long link2ether(long);
void ether_idb_enqueue(hwidbtype *hwidb);
void ether_idb_dequeue(hwidbtype *hwidb);
void ether_idb_init(hwidbtype *);
iqueue_t ether_parse_packet(hwidbtype *, paktype *, uchar *);
void print_encaps(paktype *, char *);
boolean ether_extract_addr(paktype *, hwaddrtype *, enum SRCDST);

#endif  __ETHER_H__
