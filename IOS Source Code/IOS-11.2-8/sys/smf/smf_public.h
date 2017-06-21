/* $Id: smf_public.h,v 3.5.4.2 1996/05/09 14:40:44 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/smf/smf_public.h,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) public definitions.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf_public.h,v $
 * Revision 3.5.4.2  1996/05/09  14:40:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.4.1  1996/05/04  01:45:06  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 *
 * Revision 3.5.30.1  1996/04/27  07:24:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.20.1  1996/04/08  02:08:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.10.2  1996/03/23  04:54:39  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.10.1  1996/03/22  22:56:39  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5  1996/03/07  23:31:40  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.4.22.1  1996/03/05  05:50:27  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.16.1  1996/02/27  21:19:24  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/10  06:48:07  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.3.14.1  1996/01/11  01:47:41  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/23  03:59:42  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.3  1995/11/17  18:52:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:12:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:24:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/26  00:29:00  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert SMF to use a chunk instead of malloc() and free().
 *
 * Revision 2.3  1995/10/03  14:41:43  speakman
 * CSCdi41428:  SMF debug no longer compiles
 * CSCdi41340 broke SMF debug.  Make smf_mactyp_string external.
 *
 * Revision 2.2  1995/06/21  02:06:39  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Insert IEEE spanning tree multicast address into SMF when AST over
 * SRB/FDDI is enabled.
 *
 * Revision 2.1  1995/06/07  22:44:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__SMF_PUBLIC_H__
#define	__SMF_PUBLIC_H__

/*
 * The SMF result is a value recorded in each SMF entry that
 * encodes the type of the MAC address and the disposition of
 * packets bearing that address as a destination.  The type
 * is significant only for insertion and deletion operations.
 * It just lets us know who put the MAC address in the SMF and
 * keeps the MAC address in the SMF until all interested parties
 * have removed their instance. The disposition determines how
 * the bridge code treats packets pointed at a given MAC address.
 *
 */

#define	SMF_RESULT_NOT_FOUND		(0x0)

#define SMF_MACTYP_MASK			(0xFFFFFF00)
#define SMF_ACTION_MASK			(~SMF_MACTYP_MASK)

enum SMF_ACTION_BIT {

    SMF_ACTION_RECEIVE_BIT		=  0,
    SMF_ACTION_FORWARD_BIT		=  1,
    SMF_ACTION_DISCARD_BIT		=  2,

    /* DO NOT DEFINE MORE THAN 8 SMF_ACTION BITS */

    SMF_ACTION_MAXIMUM_BIT		=  7
};

#define SMF_ACTION_RECEIVE		(1 << SMF_ACTION_RECEIVE_BIT)
#define SMF_ACTION_FORWARD		(1 << SMF_ACTION_FORWARD_BIT)
#define SMF_ACTION_DISCARD		(1 << SMF_ACTION_DISCARD_BIT)

enum SMF_MACTYP_BIT {

    SMF_MACTYP_OWN_UNICAST_BIT		=  8,
    SMF_MACTYP_PHYS_BCAST_BIT		=  9,
    SMF_MACTYP_FDDI_TRANSIT_BIT		= 10,
    SMF_MACTYP_STANDBY_UNICAST_BIT	= 11,

    SMF_MACTYP_IEEE_SPAN_BIT		= 12,
    SMF_MACTYP_DEC_SPAN_BIT		= 13,
    SMF_MACTYP_FUNC_SPAN_BIT		= 14,
    SMF_MACTYP_IEEE_IBM_SPAN_BIT	= 15,
    SMF_MACTYP_CDP_MCAST_BIT		= 16,

    SMF_MACTYP_AT_BCAST_BIT		= 17,
    SMF_MACTYP_AT_ZONE_BIT		= 18,
    SMF_MACTYP_CLNS_MCAST_BIT		= 19,
    SMF_MACTYP_DECNET_MCAST_BIT		= 20,

    SMF_MACTYP_HP_MCAST_BIT		= 21,
    SMF_MACTYP_IP_MCAST_BIT		= 22,
    SMF_MACTYP_LAT_MCAST_BIT		= 23,
    SMF_MACTYP_LAT_UNICAST_BIT          = 24,

    SMF_MACTYP_APOLLO_MCAST_BIT		= 25,

    SMF_MACTYP_MCAST_RANGE_BIT		= 26,
    SMF_MACTYP_ISL_VLAN_MCAST_BIT       = 27,
    SMF_MACTYP_VTP_MCAST_BIT            = 28,

    SMF_MACTYP_BVI_BIT  		= 29,

    /* DO NOT DEFINE MORE THAN 24 SMF_MACTYP BITS */

    SMF_MACTYP_MAXIMUM_BIT		= 31
};

#define SMF_MACTYP_OWN_UNICAST		(1 << SMF_MACTYP_OWN_UNICAST_BIT)
#define SMF_MACTYP_PHYS_BCAST		(1 << SMF_MACTYP_PHYS_BCAST_BIT)
#define SMF_MACTYP_FDDI_TRANSIT		(1 << SMF_MACTYP_FDDI_TRANSIT_BIT)
#define SMF_MACTYP_STANDBY_UNICAST	(1 << SMF_MACTYP_STANDBY_UNICAST_BIT)

#define SMF_MACTYP_IEEE_SPAN		(1 << SMF_MACTYP_IEEE_SPAN_BIT)
#define SMF_MACTYP_DEC_SPAN		(1 << SMF_MACTYP_DEC_SPAN_BIT)
#define SMF_MACTYP_FUNC_SPAN		(1 << SMF_MACTYP_FUNC_SPAN_BIT)
#define SMF_MACTYP_IEEE_IBM_SPAN	(1 << SMF_MACTYP_IEEE_IBM_SPAN_BIT)
#define SMF_MACTYP_CDP_MCAST		(1 << SMF_MACTYP_CDP_MCAST_BIT)

#define SMF_MACTYP_AT_BCAST		(1 << SMF_MACTYP_AT_BCAST_BIT)
#define SMF_MACTYP_AT_ZONE		(1 << SMF_MACTYP_AT_ZONE_BIT)
#define SMF_MACTYP_CLNS_MCAST		(1 << SMF_MACTYP_CLNS_MCAST_BIT)
#define SMF_MACTYP_DECNET_MCAST		(1 << SMF_MACTYP_DECNET_MCAST_BIT)

#define SMF_MACTYP_HP_MCAST		(1 << SMF_MACTYP_HP_MCAST_BIT)
#define SMF_MACTYP_IP_MCAST		(1 << SMF_MACTYP_IP_MCAST_BIT)
#define SMF_MACTYP_LAT_MCAST		(1 << SMF_MACTYP_LAT_MCAST_BIT)
#define SMF_MACTYP_LAT_UNICAST          (1 << SMF_MACTYP_LAT_UNICAST_BIT)

#define SMF_MACTYP_APOLLO_MCAST		(1 << SMF_MACTYP_APOLLO_MCAST_BIT)

#define SMF_MACTYP_ISL_VLAN_MCAST	(1 << SMF_MACTYP_ISL_VLAN_MCAST_BIT)

#define SMF_MACTYP_MCAST_RANGE		(1 << SMF_MACTYP_MCAST_RANGE_BIT)

#define SMF_MACTYP_BVI  		(1 << SMF_MACTYP_BVI_BIT)

#define SMF_MACTYP_VTP_MCAST            (1 << SMF_MACTYP_VTP_MCAST_BIT)

extern boolean
smf_create_filter(idbtype *swidb);

extern boolean
smf_insert_address(uchar *mac_addr, idbtype *swidb, ulong result);

extern boolean
smf_delete_address(uchar *mac_addr, idbtype *swidb, ulong result);

extern boolean
smf_delete_by_result(idbtype *swidb, ulong result);

extern void
smf_show_filter(idbtype *swidb);

extern ulong
smf_match_address(uchar *mac_addr, idbtype *swidb);

extern void
smf_show_command(parseinfo *csb);

extern void
smf_parser_init(void);

extern char *
smf_mactyp_string(ulong);

#endif
