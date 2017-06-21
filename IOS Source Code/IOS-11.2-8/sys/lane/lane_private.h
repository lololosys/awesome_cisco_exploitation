/* $Id: lane_private.h,v 3.4.10.10 1996/08/28 12:54:35 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_private.h,v $
 *------------------------------------------------------------------
 * lane_private.h: LAN Emulation private definitions
 *
 * October 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_private.h,v $
 * Revision 3.4.10.10  1996/08/28  12:54:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.9  1996/08/27  02:07:30  rbadri
 * CSCdi67231:  Topology change messages hinder LANE client debugging
 * Branch: California_branch
 * Dispose Topology change messages at fast path, if there is no
 * change or if it is from ourself.
 *
 * Revision 3.4.10.8  1996/08/26  19:45:52  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.4.10.7  1996/07/30  14:42:41  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.4.10.6  1996/06/06  04:31:14  cakyol
 * CSCdi59510:  format string in inline wastes space.
 *              offending macro obliterated.
 * Branch: California_branch
 *
 * Revision 3.4.10.5  1996/05/31  23:24:06  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              this really had nothing to do with the atm switch.
 *              it was all a coincidence.  forgot locking the elements
 *              while printing them.  this is a much better fix.
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/05/30  21:32:39  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.4.10.3  1996/05/25  22:23:44  cakyol
 * CSCdi58700:  start the lecs finder ONLY when a relevant request comes
 * in.
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/09  14:32:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.10.1.2.2  1996/05/03  18:36:19  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * Add switches to determine what vc type to pass into lane_sig_blli_match
 * instead of assuming EN.
 *
 * Revision 3.4.10.1.2.1  1996/04/27  07:06:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.10.1  1996/04/25  23:14:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.26.8  1996/04/16  02:44:11  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.26.7  1996/04/13  23:13:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * - Don't call tr_lane_start, llc2_start or start_srb_background unless
 *   tr-lane is configured.
 * - First shot at calling lec_set_maxmtu to set the tr-lane MTU.
 * - Remove lane_type from lec_muex_config.  It's not needed.
 * - Only do the tr-lane config of srb when tr-lane is configured.
 *
 * Revision 3.4.26.6  1996/04/13  01:13:16  cakyol
 * - tidy up "byteStreamCompare".
 * - better timer handling for the ticker in lecs.
 * - some more adherence to coding conventions.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.26.5  1996/04/11  05:33:09  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.4.26.4  1996/04/09  02:16:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.4.26.3  1996/04/08  23:04:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Messed up formats from TR-LANE commit.
 *
 * Revision 3.4.26.2  1996/04/08  14:51:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.26.1  1996/04/08  01:56:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.16.4  1996/04/05  21:28:58  cakyol
 * Combine all the special atm printing routines into a
 * single one (atm_related_printf), saving code space.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.16.3  1996/04/05  03:20:18  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.16.2  1996/04/02  10:09:42  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.16.1  1996/03/22  22:54:13  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/02/24  02:09:35  bbenson
 * CSCdi49790:  Flush message format follows the typo in the LANE spec.
 * Spec only defined the src_lan_dest, must add the target_dest.
 *
 * Revision 3.3.46.1  1996/03/05  06:42:49  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.36.2  1996/03/01  01:46:21  cakyol
 * convert "crashdump"s to "lane_router_crash" which can print
 * a last gasp message and call "logger_flush" before dying.
 * Helps debugging a lot.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.36.1  1996/02/27  21:10:11  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.6  1996/02/24  02:13:50  bbenson
 * CSCdi49790:  Flush message format follows the typo in the LANE spec.
 * Branch: LE_Cal_V111_0_16_branch
 * Spec only defined the src_lan_dest, must add the target_dest.
 *
 * Revision 3.3.14.5  1996/02/06  22:19:12  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.4  1996/01/26  02:02:28  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.3.2.2  1996/03/19  00:34:48  kjadams
 * Branch: TRlane_branch
 * Added #defines for the new TLV required for TR LANE
 *
 * Revision 3.3.14.3.2.1  1996/02/26  23:06:24  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.3.14.3  1996/01/23  23:54:02  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.2  1996/01/19  19:36:56  cakyol
 * - lock a config table to make it accessible by only a
 *   single session at a time (this was bug CSCdi47155
 *   in 11.0 and 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:46:13  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:06:34  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/20  18:35:54  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * When a sub-interface is added to the bridge-group, initiate a config
 * change rather than resetting the ATM interface.
 *
 * Revision 3.2  1995/11/17  17:50:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:12:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/01  00:59:29  shj
 * CSCdi39584:  set sdu sizes per connection - part 1
 * Mods for LES/BUS setting sdu size in SETUP's.
 *
 * Revision 2.4  1995/08/31  02:08:33  bbenson
 * CSCdi39584:  set sdu sizes per connection - part 1
 * - Staging of the LANE/LEC part of this fix.
 *
 * Revision 2.3  1995/06/30  22:02:11  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.2  1995/06/22  01:20:43  cakyol
 * CSCdi36146:  LECS warns of failed ILMI registration, when ILMI PVC
 * missing
 *
 * fixed.  The warnings now come out ONLY if an ILMI pvc exists.
 * Otherwise, they appear ONLY if debugging is turned on.
 *
 * As a by product of fixing this, a new function named
 * "atm_get_specific_vc" is offered.  This returns a vc info,
 * given an hwidb, a vpi and a vci.
 *
 * Revision 2.1  1995/06/07  21:19:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LANE_PRIVATE_H_
#define _LANE_PRIVATE_H_

/*
 * LAN Emulation Control Frame Constants
 */
#define LANE_PROTOCOL_ID		0x01
#define LANE_PROTOCOL_VERSION		0x01

/*
 * LAN Emulation Control Opcodes
 */
#define LANE_CONFIG_REQ			0x0001
#define LANE_CONFIG_RSP			0x0101
#define LANE_JOIN_REQ			0x0002
#define LANE_JOIN_RSP			0x0102
#define LANE_READY_QUERY		0x0003
#define LANE_READY_IND			0x0103
#define LANE_REG_REQ			0x0004
#define LANE_REG_RSP			0x0104
#define LANE_UNREG_REQ			0x0005
#define LANE_UNREG_RSP			0x0105
#define LANE_ARP_REQ			0x0006
#define LANE_ARP_RSP			0x0106
#define LANE_FLUSH_REQ			0x0007
#define LANE_FLUSH_RSP			0x0107
#define LANE_NARP_REQ			0x0008
#define LANE_TOPO_CHANGE		0x0009

#define LANE_OPCODE_REQ_MASK		0xff00	/* mask for request/response */
#define LANE_OPCODE_VALID_REQ_MASK	0x0100	/* only this bit OK in hi 8 */
#define LANE_OPCODE_REQUEST		0x0000
#define LANE_OPCODE_RESPONSE		0x0100
#define LANE_OPCODE_OP_MASK		0x00ff	/* mask for op code */


/*
 * LAN Emulation Control Status
 */
#define LANE_STATUS_SUCCESS		0
#define LANE_STATUS_VERSION_NOT_SUP	1
#define LANE_STATUS_INVALID_REQ_PARMS	2
/* 3 is deliberately missing, it is NOT a typo */
#define LANE_STATUS_DUP_LAN_DEST	4
#define LANE_STATUS_DUP_ATM_ADDR	5
#define LANE_STATUS_INSUFF_RESOURCE	6
#define LANE_STATUS_ACCESS_DENIED	7
#define LANE_STATUS_INVALID_REQ_LECID	8
#define LANE_STATUS_INVALID_LAN_DEST	9
#define LANE_STATUS_INVALID_ATM_ADDR	10
#define LANE_STATUS_NO_CONFIG		20
#define LANE_STATUS_LE_CONFIG_ERROR	21
#define LANE_STATUS_INSUFF_INFO		22
#define LANE_STATUS_TIMEOUT             23

/*
 * LAN Emulation LAN type
 *
 * Used in the CFG req and JOIN req messages.  To negotiate LAN types.
 */
#define LANE_TYPE_UNSPECIFIED		0x00
#define LANE_TYPE_802_3			0x01
#define LANE_TYPE_802_5			0x02

/*
 * LANE Emulation Destination Field Tags
 *
 * Used in the LAN destination field of control frames to identify 
 * the address type.
 */
#define LANE_TAG_NOT_PRESENT		0x00
#define LANE_TAG_MAC_ADDRESS		0x01
#define LANE_TAG_ROUTE_DESC		0x02

#define LANE_MAX_FRAME_UNSPECIFIED	0x00
#define LANE_MAX_FRAME_1516		0x01
#define LANE_MAX_FRAME_4544		0x02
#define LANE_MAX_FRAME_9234		0x03
#define LANE_MAX_FRAME_18190		0x04

/*
 * LAN Emulation Max Frame Size
 */
typedef enum {
    LANE_MAX_FRAME_SIZE_UNSPECIFIED =      0,
    LANE_MAX_FRAME_SIZE_1516 =          1516,
    LANE_MAX_FRAME_SIZE_4544 =          4544,
    LANE_MAX_FRAME_SIZE_9234 =          9234,
    LANE_MAX_FRAME_SIZE_18190 =        18190
} lane_max_frame_size_t;
    
/*
 * LANE SDU sizes for use in signaling.
 */
#define LANE_SDU_SIZE_1516		1516
#define LANE_SDU_SIZE_4544		4544
#define LANE_SDU_SIZE_9234		9234
#define LANE_SDU_SIZE_18190		18190

/*
 * LAN Emulation Protocol Flags
 *
 * The proxy flag is set in the join message for stations that wish to
 * join the elan and represent MAC addresses that are not registered
 * (bridges).
 */ 
#define LANE_PROT_FLAG_PROXY		0x0080

/*
 * The remote/local flag is set in le-arp messages.  Local addresses are
 * registered within the ELAN.  Remote addresses are proxied for by
 * bridges and are not directly attached to the ELAN.  The remote
 * addresses can be timed out of the arp cache quickly when the
 * topology change pdu is seen with the change bit set.
 */
#define	LANE_PROT_FLAG_REMOTE_ADDR	0x0001

/*
 * The topology change bit is set in the topology change message
 * whenever a bridge in the ELAN sends a configuration BPDU with the
 * configuration bit set.
 */
#define LANE_PROT_FLAG_TOPO_CHANGE	0x0100

/*
 * LANE NSAP hash table
 */
#define LANE_HASHLEN			256

/*
 * LAN Emulation Control Protocol Header
 *
 * Emulation packets arrive on two planes, one control, and the other
 * data.  The control plane passes such things as the Join,
 * Registration, LE-ARP, etc.  The data plane of course carries
 * user data, with the exception of the Flush packet.
 */
typedef struct lane_ctl_hdr_t_ {
    ushort	marker;
    uchar	protocol;
    uchar	version;
    ushort	opcode;
    ushort	status;
    ulong	transaction_id;
    ushort	requestor_lecid;
    ushort	flags;
} lane_ctl_hdr_t;

/* Offset, in bytes, to opcode. */
#define LANE_CTL_OPCODE_OFFSET \
  (sizeof(ushort) + sizeof(uchar) + sizeof(uchar))
/* 
 * Offset for flags, in bytes
 */
#define LANE_CTL_FLAGS_OFFSET  (sizeof(lane_ctl_hdr_t) - sizeof(ushort))
/* 
 * Offset for requestor lecid
 */
#define LANE_CTL_LECID_OFFSET  (sizeof(lane_ctl_hdr_t) - 2*sizeof(ushort))

/*
 * LAN Destination Field
 *
 * Make certain that shorts are short aligned and longs are long
 * aligned, so that the 4k images work.
 */
typedef struct lane_dest_t_ {
    ushort      tag;
    union {
        uchar mac[STATIONLEN_IEEE48];
        struct rd {
            ulong  PACKED(reserved);
            ushort val;
        } rd;
    } addr;
} lane_dest_t;

/*
 * Config REQ/RSP Packet
 */
typedef struct lane_cfg_pdu_t_ {
    lane_ctl_hdr_t	hdr;
    lane_dest_t		src_lan_dest;
    lane_dest_t		target_dest;
    uchar		src_atm_addr[STATIONLEN_ATMNSAP];
    uchar		lan_type;
    uchar		max_frame_size;
    uchar		num_tlvs;
    uchar		name_size;
    uchar		target_atm_addr[STATIONLEN_ATMNSAP];
    uchar		elan_name[MAX_ELAN_NAME_LENGTH];
    uchar		tlvs[0];
} lane_cfg_pdu_t;

/*
 * Join REQ/RSP Packet
 *
 * Note that num_tlvs is in the pdu, but there are NO tlvs!  Yes, this
 * is in the spec.
 */
typedef struct lane_join_pdu_t_ {
    lane_ctl_hdr_t	hdr;
    lane_dest_t		src_lan_dest;
    lane_dest_t		target_dest;
    uchar		src_atm_addr[STATIONLEN_ATMNSAP];
    uchar		lan_type;
    uchar               max_frame_size;
    uchar               num_tlvs;
    uchar               name_size;
    uchar               target_atm_addr[STATIONLEN_ATMNSAP];
    uchar               elan_name[MAX_ELAN_NAME_LENGTH];
} lane_join_pdu_t;

/*
 * Registration REQ/RSP Packet
 */
typedef struct lane_reg_pdu_t_ {
    lane_ctl_hdr_t	hdr;
    lane_dest_t	        src_lan_dest;
    lane_dest_t		target_dest;
    uchar               src_atm_addr[STATIONLEN_ATMNSAP];
    uchar		reserved[56];
} lane_reg_pdu_t;

/*
 * LE-ARP REQ/RSP Packet
 */
typedef struct lane_arp_pdu_t_ {
    lane_ctl_hdr_t	hdr;
    lane_dest_t		src_lan_dest;
    lane_dest_t		target_dest;
    uchar               src_atm_addr[STATIONLEN_ATMNSAP];
    uchar		reserved1[4];
    uchar               target_atm_addr[STATIONLEN_ATMNSAP];
    uchar		reserved2[32];
} lane_arp_pdu_t;

/* Offset, in bytes, to target mac. */
#define LANE_ARP_TARGET_OFFSET (sizeof(lane_ctl_hdr_t) + sizeof(lane_dest_t))

/*
 * Topology Change Packet
 */
typedef struct lane_topo_pdu_t_ {
    lane_ctl_hdr_t      hdr;
    uchar		reserved[92];
} lane_topo_pdu_t;

/*
 * VCC Ready Packet
 */
typedef struct lane_ready_pdu_t_ {
    lane_ctl_hdr_t	hdr;
} lane_ready_pdu_t;

/*
 * Flush REQ/RSP Packet
 */
typedef struct lane_flush_pdu_t_ {
    lane_ctl_hdr_t	hdr;
    lane_dest_t		src_lan_dest;
    lane_dest_t		target_dest;
    uchar               src_atm_addr[STATIONLEN_ATMNSAP];
    uchar		reserved1[4];
    uchar               target_atm_addr[STATIONLEN_ATMNSAP];
    uchar		reserved2[32];
} lane_flush_pdu_t;

typedef union lane_ctl_msg_u_ {
    lane_ctl_hdr_t	hdr;
    lane_cfg_pdu_t	cfg;
    lane_join_pdu_t	join;
    lane_reg_pdu_t	reg;
    lane_arp_pdu_t	arp;
    lane_topo_pdu_t	topo;
    lane_ready_pdu_t	ready;
    lane_flush_pdu_t	flush;
} lane_ctl_msg_u;


/*
 * LAN Emulation Protocol Header (Data)
 *
 * LAN Emulation carries packets over ATM using the RFC1483 briged
 * encaps which amounts to the lec_id preceeding the 802.3 MAC header.
 */

typedef struct lane_data_hdr_t_ {
    ushort lec_id;
    union {
	ether_hdr ether;
	tring_hdr tring;
    } lan_type;
} lane_data_hdr_t;
  
/* Offset, in bytes, to lec_id. */
#define LANE_DATA_ID_ETHER_OFFSET       0
#define LANE_DATA_ID_TR_OFFSET          0
 
/*
 * LANE offsets for TLVs in config request/response
 */
 
#define LANE_TLV_LEN_OFFSET      4
#define LANE_TYPE_LEN_SIZE       5
#define LANE_LOCAL_SEG_TLV_SIZE  7
#define LANE_LOCAL_SEG_LENGTH    2
#define LANE_LOCAL_SEG_TLV_ID    0x0B
  

/*
 * LANE event handling
 */
typedef enum {
    C7_TIMER_EXPIRED,
    C11_TIMER_EXPIRED,
    C12_TIMER_EXPIRED,
    C17_TIMER_EXPIRED,
    C20_TIMER_EXPIRED,
    C21_TIMER_EXPIRED,
    C28_TIMER_EXPIRED,
} lane_event_code;


/*
 * causes of address recycling in LANE entities
 */
typedef enum {
    PREFIX_ADD_EVENT,
    PREFIX_DELETE_EVENT,
    MAC_ADDRESS_CHANGE_EVENT
} lane_addr_recycle_reason;

/*******************************************************************
**
**			debugging support
**
*******************************************************************/

/* valid type values for lane_debug below */
#define _LECS_EVENT_DBG		1
#define _LECS_PACKET_DBG	2
#define _LECS_FINDER_DBG	3
#define _LES_EVENT_DBG		4
#define _BUS_EVENT_DBG		5
#define _LEC_EVENT_DBG		6

extern void lane_debug (int type, char *ifname, char *fmt, ...);

/* lecs events debugging */
#define LECS_DBG(ifname, fmt, args...)	\
    {					\
	if (lane_lecs_debug)		\
	    lane_debug(_LECS_EVENT_DBG, ifname, fmt, ## args); \
    }

/* lecs packet debugging */
#define LECSP_DBG(ifname, fmt, args...)	\
    {					\
	if (lane_lecsPacket_debug)	\
	    lane_debug(_LECS_PACKET_DBG, ifname, fmt, ## args); \
    }

/* lecs finder debugging */
#define LECSF_DBG(ifname, fmt, args...)	\
    {					\
	if (lane_lecsFinder_debug)	\
	    lane_debug(_LECS_FINDER_DBG, ifname, fmt, ## args); \
    }

/* les debugging */
#define LES_DBG(ifname, fmt, args...)	\
    {					\
	if (lane_lsv_debug)		\
	    lane_debug(_LES_EVENT_DBG, ifname, fmt, ## args); \
    }

/*
** general byte stream lexicographic comparison.  Returns
** 0, -1 and +1 for streams being equal, first being < the 
** second and first being > second respectively.
**
** Note that one can NOT directly use (u1-u2) as the return
** value since this will not yield a proper signed value,
** hence the funny looking return statement.
*/
static inline int byteStreamCompare (uchar *b1, uchar *b2, 
    int count)
{
    register ulong u1, u2;
    int diff;

    /* clearly equal strings */
    if (b1 == b2) return 0;

    /* compare the full words as much as you can */
    while (count >= sizeof(ulong)) {
	u1 = GETLONG(b1);
	u2 = GETLONG(b2);
	if (u1 != u2) 
	    return (u1 < u2 ? -1 : +1);
	b1 += sizeof(ulong);
	b2 += sizeof(ulong);
	count -= sizeof(ulong);
    }

    /* do the remaining, byte by byte */
    while (count-- > 0) {
	diff = *b1++ - *b2++;
	if (diff) return diff;
    }
    
    /* byte streams are equal */
    return 0;
}

/*
 * globals
 */

extern uchar ALL_ONES_MASK [];
extern queuetype lane_infoQ;

#define FOR_ALL_LANE_INFOS(ln) \
    for ((ln) = lane_infoQ.qhead; (ln); (ln) = (ln)->next)

/*
 * Extern function definitions.
 */

/* extra printing function */
extern int atm_related_printf (const char **fmt, char **cp, 
    tt_soc *tty, va_list *argp, boolean rjust, int width, int size,
    int hash, int precision);

extern void		lane_router_crash(char *lastGaspMessage);
extern boolean		lane_hdr_check(lane_ctl_hdr_t *header);
extern void		setToFixedLecsAddress(hwaddrtype*);
extern void		sendRelease (boolean releaseComplete,
				     void *callId, 
				     sig_api_cause_code cause, 
				     idbtype *swidb);
extern lane_info_t	*lane_init_idb(idbtype *);
extern boolean		lane_addr_valid (hwaddrtype *);
extern boolean		lane_addr_equal(hwaddrtype *, hwaddrtype *);
extern vc_info_t	*atm_get_specific_vc (hwidbtype *hwidb, 
					      unsigned vpi,
					      unsigned vci);
extern unsigned		lane_addr_hash(hwaddrtype *);
extern unsigned 	lane_mac_hash(uchar *);
extern boolean		lane_control_encap(paktype *pak, vcd_t vcd);
extern boolean		lane_debug_any(void);
extern void		lane_info_destroy(lane_info_t *);
extern void		lane_parser_init(void);
extern char		*lane_pr_lan_type(uchar type);
extern char		*lane_pr_max_frame_size(uchar max_frame_size);
extern unsigned         lane_frame_to_sdu_size(uchar max_frame_size);
extern char 		*lane_pr_opcode(lane_ctl_hdr_t *);
extern char 		*lane_pr_status(int status);
extern void		lane_swif_erase(idbtype *);
extern void		lec_swif_erase(lane_info_t *);
extern void		lsv_swif_erase(lane_info_t *);
extern void		lecs_swif_erase(lane_info_t *);
extern void		lecs_interface_function(parseinfo*);
extern boolean		prepareLecsFinderModule(lane_info_t*);
extern void		lec_config_change(void*);
extern void             lsv_gen_atm_address(parseinfo *);
extern void             lsv_gen_les_bus(parseinfo *);
extern void             lsv_cmd_les_bus(parseinfo *);
extern void             lsv_cmd_bus_nsap(parseinfo *);
extern void             lsv_cmd_les_nsap(parseinfo *);
extern void		set_default_mac_plan (lane_info_t *);
extern void		increment_macaddress_by 
			    (uchar *srcmac, uchar *dstmac, int increment);

char *lane_pr_addr_recycle_reason (lane_addr_recycle_reason);
extern void lecs_addr_recycle(hwidbtype*, lane_addr_recycle_reason);
extern void lesbus_addr_recycle(hwidbtype*, lane_addr_recycle_reason);
extern void lec_addr_recycle(hwidbtype*, lane_addr_recycle_reason);

extern boolean		lsv_new_nsap(lane_info_t *, 
				boolean set_not_clear,
				MaskedAddress *new_nsap,
				MaskedAddress *dest_nsap,
				char *nsap_name);

/* show routines */

extern void             lane_show_one_server(lane_info_t *, boolean);
extern void        	lane_show_one_bus(lane_info_t *, boolean);
extern void		lane_show_one_lec(lane_info_t *, boolean);
extern void 		lecs_interface_show(idbtype *, boolean);

/* masked address helper routines */

extern void binaryBytes2hexString(uchar *binaryBytes, 
				  int binaryCount, 
				  char *hexString);
extern void hexString2binaryBytes(char *hexString, 
				  int hexCount,
				  uchar *binaryBytes);
extern boolean allZeroMask(uchar *mask,
			   int count);
extern boolean allOnesMask(uchar *mask,
			   int count);

/* generic atm connect routine */
extern void *connect_to_target(idbtype *swidb, hwaddrtype *src,
    hwaddrtype *target, lane_vc_type_t vcType,
    void (*callback)(void *));

extern unsigned lane_mtu_to_frame(idbtype *swidb);
extern boolean lane_set_maxmtu(idbtype *swidb, int mtu_size);
#endif _LANE_PRIVATE_H_
