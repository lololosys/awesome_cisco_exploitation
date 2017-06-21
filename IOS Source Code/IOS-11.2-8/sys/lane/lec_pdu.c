/* $Id: lec_pdu.c,v 3.5.6.9 1996/08/28 12:55:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_pdu.c,v $
 *------------------------------------------------------------------
 * LAN Emulation Client PDU routines
 *
 * December 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_pdu.c,v $
 * Revision 3.5.6.9  1996/08/28  12:55:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.6.8  1996/08/26  18:01:09  rbadri
 * CSCdi67033:  LEC can create loopback VC and munge LE-ARP table
 * Branch: California_branch
 * In lec_send_arp_rsp(), copy source & destination MAC addresses
 * from the PDU, instead of using pointers which may have stale info.
 *
 * Revision 3.5.6.7  1996/07/23  13:25:20  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.6.6  1996/06/18  19:15:18  rbadri
 * CSCdi60067:  IPX rip TR->TR through bridged ELAN works intermittently.
 * Branch: California_branch
 * Clean up a little on the LEC side.
 *
 * Revision 3.5.6.5  1996/06/05  02:01:27  rbadri
 * CSCdi59338:  TR->TR through bridged ELAN works intermittently
 * Branch: California_branch
 * Move setting of C4_proxy to a_activate_lec. Update bridge table
 * entries more tightly. When creating le-arp entries on the fly, due
 * to an incoming le-arp req, use the src_mac as the mac in le-arp entry.
 *
 * Revision 3.5.6.4  1996/05/30  21:33:13  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.5.6.3  1996/05/09  14:32:58  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.6.2.2.1  1996/04/27  07:06:50  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.6.2  1996/04/25  23:14:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.6.1.8.5  1996/04/16  02:44:54  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.6.1.8.4  1996/04/08  21:33:48  cakyol
 * correct sync problems:
 * - more unprotected buginfs.
 * - do NOT access uninitialized index "rdIndex".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.6.1.8.3  1996/04/08  21:01:16  cakyol
 * correct sync errors:
 * - remove spurious (unprotected) buginf messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.6.1.8.2  1996/04/08  14:52:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.6.1.8.1  1996/04/08  01:57:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.6.1  1996/03/18  20:32:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.10.3  1996/04/06  02:52:33  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.5.10.2  1996/03/25  02:20:04  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.10.1  1996/03/22  22:55:12  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.22.2  1996/03/04  23:01:13  cakyol
 * pull in the dual OID fix for getting LECS addresses using
 * the ILMI (bug CSCdi50537) manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.22.1  1996/02/27  21:10:38  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.2.2  1996/02/06  22:19:26  cakyol
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
 * Revision 3.4.2.1  1996/01/11  01:46:38  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1996/01/05  04:56:18  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3.10.1  1995/12/29  01:06:55  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:50:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10.8.1.2.1  1996/02/03  07:07:16  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.10.8.1  1995/10/12  03:23:33  rbadri
 * Branch: Arkansas_branch
 * Commiting LE_Ark features to Arkansas_branch
 *
 * Revision 2.7.18.4.2.1  1996/01/19  21:56:54  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.7.18.5  1995/12/01  16:01:24  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.7.18.4  1995/10/26  02:39:35  bbenson
 * CSCdi42160:  ATM port in blocking state responds to LE-ARPs
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Don't respond to LE-ARP requests when the port state is BLOCKED.
 * - Roll in fix CSCdi39461 from the mainline to fix infinite loop of the
 *   config request.
 *
 * Revision 2.7.18.3  1995/10/16  23:45:54  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.7.18.2  1995/09/27  23:27:01  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Further buff and polish of setting ACTIVE SAR state.
 * - Moving Topology change, SNMP, proxy, and flush code into Synergy.
 *
 * Revision 2.7.18.1  1995/09/20  21:42:46  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Call lec_clear_mac instead of bridge_set_mac2vc whenever clearing the
 *   le_arp entry.
 * - Set the earl's mac entry only when the le-arp state is DIRECT and the
 *   call ready state is CALL_READY.
 *
 * Revision 2.7  1995/08/06  21:59:54  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.6  1995/07/30  01:15:55  bbenson
 * CSCdi37903:  Not necessary to invalidate caches immediately for LANE.
 * - No more calls to reg_invoke_bump_cache_version with an immediate
 *   request.
 * - Also collapsed numerous message pointers in the lec_info_t into a
 *   single union
 *
 * Revision 2.5  1995/07/13  02:10:39  bbenson
 * CSCdi37028:  lec_send_release_comp clears out the vc before
 *         constructing rel_comp.
 *
 * Revision 2.4  1995/07/12  07:28:36  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.3  1995/07/11  05:27:31  bbenson
 * CSCdi36864:  No need to display UPDOWN message periodically.
 * CSCdi36904:  LEC should retransmit JOIN requests C13_max_retry_count
 * times.
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes.
 *
 * Revision 2.2  1995/06/30  22:02:20  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.1  1995/06/07  21:20:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"

#include "sys_registry.h"
#include "subsys.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/ilmi_api.h"
#include "../atm/atmsig_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"

#include "lane_registry.h"
#include "lecmib_util.h"
#include "lecmib_if.h"

const blli_type lec_blli_control = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11,		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,                   /* SNAP ID                      */
	0x00, 0xa0, 0x3e,       /* ATM Forum OUI                */
	0x00, 0x01		/* LE Config or Control VCC	*/
    }
};

const blli_type lec_blli_ether_data = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11,		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e,	/* ATM Forum OUI                */
	0x00, 0x02		/* LE 802.3 Data Direct		*/
    }
};


const blli_type lec_blli_ether_mcast = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11, 		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e, 	/* ATM Forum OUI                */
	0x00, 0x04		/* LE 802.3 Multicast VCC	*/
    }
};

const blli_type lec_blli_token_ring_data = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11,		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e,	/* ATM Forum OUI                */
	0x00, 0x03		/* LE 802.5 Data Direct		*/
    }
};

const blli_type lec_blli_token_ring_mcast = {
    LEC_BLLI_LENGTH,
    {
	0x60 + 11, 		/* Layer 3 id + ISO/EIC TR 9577 */
	0x40, 0x80,		/* 9577 IPI = SNAP              */
	0x80,			/* SNAP ID                      */
	0x00, 0xa0, 0x3e, 	/* ATM Forum OUI                */
	0x00, 0x05		/* LE 802.5 Multicast VCC	*/
    }
};


/*
 * fixed part of the oid
 */
static ulong atmf_LECS_addr_oid [] =
{
    atmfSrvcRegATMAddress,
    0	/* port number, 0 == "this" port */
};
 
static OID atmf_LECS_addr =
{
 length:     sizeof(atmf_LECS_addr_oid)/sizeof(ulong),
 oid_ptr:    atmf_LECS_addr_oid
 };
 
static VarBind lecs_var_bind =
{
 data_length:        0,
 name:               &atmf_LECS_addr,
 name_size:          0,
 value:              { type: NULL_TYPE },
 value_size:         0,
 next_var:           NULL
 };



/*
 * lec_prsignal
 *
 * Returns a string representing the signaling message.
 */
char *lec_prsignal (int opcode)
{
    switch (opcode) {
    case ATM_SETUP: 		return("SETUP");
    case ATM_CONNECT:		return("CONNECT");
    case ATM_CONNECT_ACK: 	return("CONNECT_ACK");
    case ATM_RELEASE: 		return("RELEASE");
    case ATM_RELEASE_COMPLETE:	return("RELEASE_COMPLETE");
    case ATM_ADD_PARTY: 	return("ADD_PARTY");
    case ATM_DROP_PARTY: 	return("DROP_PARTY");
    case ATM_LISTEN_NSAP:	return("LISTEN");
    case ATM_CANCEL_NSAP:	return("CANCEL");
    default:			return("UNKNOWN");
    }
}

/*
 * lec_dump_sig_msg
 *
 */
void lec_dump_sig_msg (lec_sr_t send_or_recv,
		       sig_api_msg *sig_msg)
{
    idbtype	*swidb;
    lec_info_t	*lec_info;
    char	nsapstr[ATM_MAX_NSAP_STRING];

    swidb = sig_msg->idb;
    lec_info = lec_info_from_swidb(sig_msg->idb);

    switch (send_or_recv) {
    case SEND:
	buginf("\nLEC %s: sending %s",
	       swidb->namestring,
               lec_prsignal(sig_msg->opcode));
	break;
    case RECV:
	buginf("\nLEC %s: received %s",
	       swidb->namestring,
               lec_prsignal(sig_msg->opcode));
	break;
    }

    switch(sig_msg->opcode) {
    case ATM_LISTEN_NSAP:
	atm_printnsap(sig_msg->u.listen.nsap.addr, 
		      nsapstr);
	if (send_or_recv == SEND) {
	    buginf("\nLEC %s:   listen on\t%s",
		   lec_info->swidb->namestring,
		   nsapstr);
	}
	break;
    case ATM_CANCEL_NSAP:
	atm_printnsap(sig_msg->u.cancel.nsap.addr, 
		      nsapstr);
	if (send_or_recv == SEND) {
	    buginf("\nLEC %s:   ATM address\t%s",
		   lec_info->swidb->namestring,
		   nsapstr);
	}
	break;
    case ATM_SETUP:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.setup.call_id);
	atm_printnsap(sig_msg->u.setup.called_party.addr, 
		      nsapstr);
	buginf("\nLEC %s:   called party\t%s",
	       lec_info->swidb->namestring,
               nsapstr);
	atm_printnsap(sig_msg->u.setup.calling_party.addr,
		      nsapstr);
        buginf("\nLEC %s:   calling_party\t%s",
               lec_info->swidb->namestring,
               nsapstr);
	break;
    case ATM_CONNECT:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.connect.call_id);
        buginf("\nLEC %s:   vcd\t\t%d",
               lec_info->swidb->namestring,
               sig_msg->u.connect.vcd);
	break;
    case ATM_RELEASE:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.release.call_id);
        buginf("\nLEC %s:   cause code\t%d",
               lec_info->swidb->namestring,
               sig_msg->u.release.cause);
	break;
    case ATM_RELEASE_COMPLETE:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.release_comp.call_id);
        buginf("\nLEC %s:   cause code\t%d",
               lec_info->swidb->namestring,
               sig_msg->u.release_comp.cause);
	break;
    case ATM_ADD_PARTY:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.add_party.call_id);
        buginf("\nLEC %s:   endpoint reference\t0x%x",
               lec_info->swidb->namestring,
               sig_msg->u.add_party.endpoint_id);
	break;
    case ATM_DROP_PARTY:
	buginf("\nLEC %s:   callid\t\t0x%x",
	       lec_info->swidb->namestring,
	       sig_msg->u.drop_party.call_id);
        buginf("\nLEC %s:   endpoint reference\t0x%x",
               lec_info->swidb->namestring,
               sig_msg->u.drop_party.endpoint_id);
	break;
    case ATM_CONNECT_ACK:
    default:
    }
}


/*
 * lec_dump_control
 */
void lec_dump_control (lec_sr_t send_or_recv,
		       idbtype *swidb, 
		       paktype *pak, 
		       vcd_t vcd)
{
    lane_ctl_msg_u *msg;
    char nsapstr[ATM_MAX_NSAP_STRING];

    msg	= (lane_ctl_msg_u *)pak->datagramstart;

    switch (send_or_recv) {
    case SEND:
	buginf("\nLEC %s: sending %s on VCD %d",
	       swidb->namestring,
	       lane_pr_opcode(&msg->hdr),
	       vcd);
	break;
    case RECV:
	buginf("\nLEC %s: received %s on VCD %d",
	       swidb->namestring,
	       lane_pr_opcode(&msg->hdr),
	       vcd);
	break;
    }

    switch (msg->hdr.opcode) {
    case LANE_CONFIG_REQ:
    case LANE_CONFIG_RSP:	
	buginf("\nLEC %s:   SRC MAC address\t%e",
	       swidb->namestring,
	       msg->cfg.src_lan_dest.addr.mac);
	atm_printnsap(msg->cfg.src_atm_addr, nsapstr);
	buginf("\nLEC %s:   SRC ATM address\t%s",
	       swidb->namestring,
	       nsapstr);
	buginf("\nLEC %s:   LAN Type\t%d",
	       swidb->namestring,
	       msg->cfg.lan_type);
	buginf("\nLEC %s:   Frame size\t%d",
	       swidb->namestring,
	       msg->cfg.max_frame_size);
	buginf("\nLEC %s:   LAN Name\t%32s",
	       swidb->namestring, 
	       msg->cfg.elan_name);
	buginf("\nLEC %s:   LAN Name size\t%d",
	       swidb->namestring,
	       msg->cfg.name_size);
	break;
    case LANE_JOIN_REQ:
    case LANE_JOIN_RSP:
	buginf("\nLEC %s:   Status\t\t%d",
	       swidb->namestring,
	       msg->hdr.status);
	buginf("\nLEC %s:   LECID\t\t%d",
	       swidb->namestring,
	       msg->hdr.requestor_lecid);
	buginf("\nLEC %s:   SRC MAC address\t%e",
	       swidb->namestring,
	       msg->join.src_lan_dest.addr.mac);
	atm_printnsap(msg->join.src_atm_addr, nsapstr);
	buginf("\nLEC %s:   SRC ATM address\t%s",
	       swidb->namestring,
	       nsapstr);
	buginf("\nLEC %s:   LAN Type\t%d", 
	       swidb->namestring,
	       msg->join.lan_type);
	buginf("\nLEC %s:   Frame size\t%d",
	       swidb->namestring,
	       msg->join.max_frame_size);
	buginf("\nLEC %s:   LAN Name\t%32s",
	       swidb->namestring, 
	       msg->join.elan_name);
	buginf("\nLEC %s:   LAN Name size\t%d",
	       swidb->namestring, 
	       msg->join.name_size);
	break;
    case LANE_REG_REQ:
    case LANE_REG_RSP:
    case LANE_UNREG_REQ:
    case LANE_UNREG_RSP:
	break;
    case LANE_ARP_REQ:
    case LANE_ARP_RSP:
	buginf("\nLEC %s:   SRC MAC address     %e",
	       swidb->namestring,
	       msg->arp.src_lan_dest.addr.mac);
	atm_printnsap(msg->arp.src_atm_addr, nsapstr);
	buginf("\nLEC %s:   SRC ATM address     %s",
	       swidb->namestring,
	       nsapstr);
	buginf("\nLEC %s:   TARGET MAC address  %e",
	       swidb->namestring,
	       msg->arp.target_dest.addr.mac);
	atm_printnsap(msg->arp.target_atm_addr, nsapstr);
	buginf("\nLEC %s:   TARGET ATM address  %s",
	       swidb->namestring,
	       nsapstr);
	break;
    case LANE_FLUSH_REQ:
    case LANE_FLUSH_RSP:
    case LANE_TOPO_CHANGE:
    case LANE_READY_QUERY:
    case LANE_READY_IND:
    default:
	break;
    }
    
}


/*
 * lec_send_pak
 *
 * Dump the packet if debugging is enabled.  Do the final encaps of
 * the control packet, slapping the enctype and vcd on the front of the
 * packet.  Send it out the interface if successful, otherwise ditch the
 * packet.
 */
static boolean lec_send_pak (paktype *pak, ushort vcd)
{
    if (!lane_control_encap(pak, vcd)) {
	datagram_done(pak);
	return(FALSE);
    } else {
	datagram_out(pak);
	return(TRUE);
    }
}


/*
 * lec_getbuffer
 *
 * Allocate a control pak, and initialize the common LAN emulation 
 * header.
 */
static paktype *lec_getbuffer (lec_info_t *lec_info, int size)
{
    paktype		*pak;
    lane_ctl_hdr_t	*hdr = NULL;

    pak = getbuffer(size);
    if (!pak) {
        return(NULL);
    }

    memset(pak->datagramstart, 0, size);

    hdr			= (lane_ctl_hdr_t *)pak->datagramstart;
    hdr->marker		= LANE_MARKER;
    hdr->protocol	= LANE_PROTOCOL_ID;
    hdr->version	= LANE_PROTOCOL_VERSION;
    hdr->status		= 0;
    hdr->transaction_id	= lec_info->transaction_id = lec_transaction_id++;
    hdr->requestor_lecid = lec_info->lecid;
    hdr->flags		= 0;

    pak->if_output	= lec_info->swidb;
    pak->datagramsize	= size;
    return(pak);
}


/*
 * lec_send_arp_req
 */
boolean lec_send_arp_req (lec_info_t *lec_info, 
			  uchar const *dest_mac,
			  uchar const *src_mac)
{
    lane_arp_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_arp_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    pdu = (lane_arp_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_ARP_REQ;

    lec_make_dest(lec_info, dest_mac, &pdu->target_dest, LANE_TAG_MAC_ADDRESS);
    pdu->src_lan_dest.tag = LANE_TAG_MAC_ADDRESS;
    bcopy(src_mac, pdu->src_lan_dest.addr.mac, IEEEBYTES);
    bcopy(lec_info->nsap.addr, pdu->src_atm_addr, lec_info->nsap.length);

    LEC_PACKET_DEBUG (SEND,
		      pak->if_output,
		      pak,
		      lec_info->ctl_direct.vcd);

    lec_info->lecARPrequestsOut++;
    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->ctl_direct.vcd));

}


/*
 * lec_send_arp_rsp
 *
 * All we need to do in the arp rsp is set the target_atm_address
 * field and  turn it around.  The pak is dup'd because
 * lec_process_messages always deallocates the original pak.
 */
boolean lec_send_arp_rsp (lec_info_t *lec_info, lec_arp_type_t arp_type)
{
    lane_ctl_msg_u	*pdu;
    paktype		*pak;

    uchar		src_mac[IEEEBYTES];
    uchar		dest_mac[IEEEBYTES];
    hwaddrtype		src_nsap;
    
    pak = pak_duplicate(lec_info->data.control_pak);
    if (!pak) return(FALSE);

    pak->if_output = lec_info->swidb;

    pdu = (lane_ctl_msg_u*) pak->datagramstart;
    pdu->hdr.opcode = LANE_ARP_RSP;
    
    switch (arp_type) {
    case LEC_LOCAL_ADDR:
	break;
    case LEC_REMOTE_ADDR:
	pdu->hdr.flags = LANE_PROT_FLAG_REMOTE_ADDR;
	break;
    }

    bcopy(lec_info->nsap.addr, 
	  pdu->arp.target_atm_addr, 
	  lec_info->nsap.length);

    LEC_PACKET_DEBUG (SEND,
		      pak->if_output,
		      pak,
		      lec_info->ctl_direct.vcd);

    lec_info->lecARPrepliesOut++;
    lec_info->lecControlFramesOut++;

    bcopy(pdu->arp.src_lan_dest.addr.mac, src_mac, IEEEBYTES);
    bcopy(pdu->arp.target_dest.addr.mac, dest_mac, IEEEBYTES);
    lec_make_hwaddr(&src_nsap,
		    pdu->arp.src_atm_addr,
		    STATION_ATMNSAP,
		    STATIONLEN_ATMNSAP);

    if (lec_send_pak(pak, lec_info->ctl_direct.vcd)) {
	return (TRUE);
    }
    return (FALSE);
}

/*
 * lec_send_narp_req
 *
 * send a narp request out.  This is used typically as a result
 * of the HSRP takeover.  So fill in the mac address in question.
 * Do not touch the target atm address however, since we do not
 * know this, it is the atm nsap address of the client who 
 * just "quit".
 */
boolean lec_send_narp_req (lec_info_t *lec_info, uchar const *mac)
{
    lane_arp_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_arp_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    pdu = (lane_arp_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_NARP_REQ;

    lec_make_dest(lec_info, mac, &pdu->target_dest, LANE_TAG_MAC_ADDRESS);
    pdu->src_lan_dest.tag = LANE_TAG_NOT_PRESENT;
    bcopy(lec_info->nsap.addr, pdu->src_atm_addr, 
	lec_info->nsap.length);
    bzero(pdu->target_atm_addr, STATIONLEN_ATMNSAP);
    LEC_PACKET_DEBUG (SEND,
		      pak->if_output,
		      pak,
		      lec_info->ctl_direct.vcd);
    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->ctl_direct.vcd));
}


/*
 * lec_send_cfg_req
 *
 * Allocate a buffer, load in the configure request pdu and send it
 * out. 
 */
boolean lec_send_cfg_req (lec_info_t *lec_info)
{
    lane_cfg_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_cfg_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    lec_start_timer(lec_info,
		    lec_info->C7_control_timeout,
		    LEC_TIMER_CONFIG);

    pdu = (lane_cfg_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_CONFIG_REQ;

    pdu->src_lan_dest.tag = LANE_TAG_MAC_ADDRESS;
    bcopy(lec_info->normalMac, pdu->src_lan_dest.addr.mac, IEEEBYTES);

    bcopy(lec_info->nsap.addr, pdu->src_atm_addr, lec_info->nsap.length);

    pdu->lan_type	= lec_info->actual_lan_type;
    pdu->max_frame_size = lec_info->actual_max_frame_size;
    pdu->name_size	= lec_info->actual_name_size;
    pdu->num_tlvs	= 0;

    bcopy(lec_info->actual_elan_name, 
	  pdu->elan_name, 
	  lec_info->actual_name_size);

    LEC_PACKET_DEBUG (SEND, 
		      pak->if_output, 
		      pak, 
		      lec_info->configure.vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->configure.vcd));

}


/*
 * lec_send_flush_req
 */
boolean lec_send_flush_req (lec_info_t *lec_info, lec_le_arp_t *le_arp)
{
    lane_flush_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_flush_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    pdu = (lane_flush_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_FLUSH_REQ;
    pdu->hdr.transaction_id = (ulong)le_arp;

    bcopy(lec_info->nsap.addr, pdu->src_atm_addr, lec_info->nsap.length);
    bcopy(le_arp->nsap.addr, pdu->target_atm_addr, le_arp->nsap.length);

    LEC_PACKET_DEBUG (SEND,
		      pak->if_output,
		      pak,
		      lec_info->mcast_send.vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->mcast_send.vcd));

}


/*
 * lec_send_flush_rsp
 *
 * Just need to turn this packet around (after setting the opcode). 
 * The pak is dup'd because lec_process_messages always deallocates
 * the original pak.
 */
boolean lec_send_flush_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*pdu;
    paktype		*pak;
    
    pak = pak_duplicate(lec_info->data.control_pak);
    if (!pak) {
	return(FALSE);
    }

    pak->if_output = lec_info->swidb;

    pdu = (lane_ctl_msg_u *)pak->datagramstart;
    pdu->hdr.opcode = LANE_FLUSH_RSP;

    LEC_PACKET_DEBUG (SEND,
		      pak->if_output,
		      pak,
		      lec_info->ctl_direct.vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->ctl_direct.vcd));

}


/*
 * lec_send_join_req
 */
boolean lec_send_join_req (lec_info_t *lec_info)
{
    lane_join_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_join_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    lec_start_timer(lec_info,
		    lec_info->C7_control_timeout,
		    LEC_TIMER_JOIN);

    pdu = (lane_join_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_JOIN_REQ;
    if (lec_info->C4_proxy) {
	pdu->hdr.flags = LANE_PROT_FLAG_PROXY;
    }

    pdu->src_lan_dest.tag = LANE_TAG_MAC_ADDRESS;
    bcopy(lec_info->normalMac, pdu->src_lan_dest.addr.mac, IEEEBYTES);
    bcopy(lec_info->nsap.addr, pdu->src_atm_addr, lec_info->nsap.length);

    pdu->lan_type	= lec_info->actual_lan_type;
    pdu->max_frame_size = lec_info->actual_max_frame_size;
    pdu->name_size	= lec_info->actual_name_size;

    bcopy(lec_info->actual_elan_name,
	  pdu->elan_name, 
	  lec_info->actual_name_size);

    LEC_PACKET_DEBUG (SEND, 
		      pak->if_output, 
		      pak, 
		      lec_info->ctl_direct.vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->ctl_direct.vcd));

}



/*
 * lec_send_reg_req
 */
boolean lec_send_reg_req (lec_info_t *lec_info)
{
    lane_reg_pdu_t   *pdu;
    paktype    *pak;
    ushort     route_desc;

      /* sanity check
       */
    if (!lec_info->registered_w_les)  {
       pak = lec_getbuffer(lec_info, sizeof(lane_reg_pdu_t));
       if (!pak) {
          return(FALSE);
       }

       lec_start_timer(lec_info,
                       lec_info->C7_control_timeout,
                       LEC_TIMER_TYPE_REG_REQ);

       pdu = (lane_reg_pdu_t *)pak->datagramstart;

       pdu->hdr.opcode = LANE_REG_REQ;

      /* due to using the vring, I only have one RD to register */
       pdu->src_lan_dest.tag = LANE_TAG_ROUTE_DESC;
       route_desc = (lec_info->vr_rn << 4) | (lec_info->srb_bn & 0x0F);
       bcopy(&route_desc, &pdu->src_lan_dest.addr.rd.val, STATIONLEN_ROUTEDESC);
       bcopy(lec_info->nsap.addr, pdu->src_atm_addr, lec_info->nsap.length);

       LEC_PACKET_DEBUG (SEND,
            pak->if_output,
            pak,
            lec_info->ctl_direct.vcd);

       lec_info->lecControlFramesOut++;

       return(lec_send_pak(pak, lec_info->ctl_direct.vcd));
    }
    return(TRUE);
}



/*
 * lec_send_ready_ind
 *
 * Send that funky pdu which, due to an omission in the UNI3.0 spec,
 * is required by the LANE spec.  The calling party has to send 
 * the ready_ind to the called party after receiving the CONNECT, to 
 * inform the called party that the connection is really up.
 */
boolean lec_send_ready_ind (lec_info_t *lec_info, vcd_t vcd)
{
    lane_ready_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_ready_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    pak->if_output = lec_info->swidb;

    pdu = (lane_ready_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_READY_IND;

    LEC_PACKET_DEBUG (SEND, pak->if_output, pak, vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, vcd));

}


/*
 * lec_send_ready_query
 *
 * Send that other funky pdu which, due to an omission in the UNI3.0 spec,
 * is required by the LANE spec.  If the called party hasn't received
 * the ready_ind within the C28 timeout, it queries the calling party again.
 */
boolean lec_send_ready_query (lec_info_t *lec_info, vcd_t vcd)
{
    lane_ready_pdu_t	*pdu;
    paktype		*pak;

    pak = lec_getbuffer(lec_info, sizeof(lane_ready_pdu_t));
    if (!pak) {
	return(FALSE);
    }

    pak->if_output = lec_info->swidb;

    pdu = (lane_ready_pdu_t *)pak->datagramstart;

    pdu->hdr.opcode = LANE_READY_QUERY;

    LEC_PACKET_DEBUG (SEND, pak->if_output, pak, vcd);

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, vcd));

}

/*
 * lec_send_topo_change
 *
 * This is a registered function that is called by either the bridging
 * or synergy code to send out a topology change whenever a hello BPDU
 * is sent.  The TC bit (Topology Change) is a bit in the hello BPDU
 * that indicates that the spanning tree is undergoing change.
 */
boolean lec_send_topo_change (lec_info_t *lec_info, boolean TC)
{
    lane_topo_pdu_t	*pdu;
    paktype		*pak;
 
    pak = lec_getbuffer(lec_info, sizeof(lane_topo_pdu_t));
    if (!pak) {
	return(FALSE);
    }
 
    pdu = (lane_topo_pdu_t *)pak->datagramstart;
 
    pdu->hdr.opcode	= LANE_TOPO_CHANGE;

    if (TC) {
	pdu->hdr.flags	= LANE_PROT_FLAG_TOPO_CHANGE;
	LEC_TOPO_DEBUG ("\nLEC %s: sending LANE_TOPO_CHANGE on VCD %u",
			lec_info->swidb->namestring,
			lec_info->ctl_direct.vcd);
    }
 

    lec_info->lecControlFramesOut++;

    return(lec_send_pak(pak, lec_info->ctl_direct.vcd));
 
}


/*
 * lec_send_add_party_rej
 */
boolean lec_send_add_party_rej (lec_info_t *lec_info)
{
    sig_api_msg	*msg;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
        return(FALSE);
    }

    msg->opcode			= ATM_ADD_PARTY_REJ;
    msg->idb			= lec_info->swidb;
    msg->u.add_party_rej.call_id = lec_info->data.sig_msg->u.add_party.call_id;
    msg->u.add_party_rej.endpoint_id 
	= lec_info->data.sig_msg->u.add_party.endpoint_id;
    msg->u.add_party_rej.cause	= ATM_NORMAL_UNSPECIFIED;

    LEC_SIGNAL_DEBUG (SEND, msg);

    process_enqueue(atmSig_outputQ, msg);
    return(TRUE);
}


/*
 * lec_send_setup
 */
boolean lec_send_setup (lec_info_t *lec_info, lec_vc_t *dest_vc)
{
    sig_api_msg *msg;
    
    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
	return(FALSE);
    }

    dest_vc->call_state		= CALL_STATE_PROCEEDING;
    dest_vc->svc 		= TRUE;

    msg->opcode			= ATM_SETUP;
    msg->idb			= dest_vc->swidb;
    msg->u.setup.call_id	= dest_vc->call_id =
	atmSig_api_allocate_call_id();
    msg->u.setup.sig_user_callback = lec_sig_callback;
    msg->u.setup.called_party 	= dest_vc->nsap;
    msg->u.setup.calling_party 	= lec_info->nsap;
    msg->u.setup.user_vc_type	= dest_vc->lane_vc_type;
    msg->u.setup.flags		= dest_vc->flags;
    atm_params_init(&msg->u.setup.params);
    msg->u.setup.blli[0].length = dest_vc->blli->length;
    msg->u.setup.blli[1].length = 0;
    msg->u.setup.blli[2].length	= 0;
    bcopy(dest_vc->blli->string,
	  msg->u.setup.blli[0].string,
	  ATM_BBAND_LOLAYER_MAXSIZE);
    msg->u.setup.multipoint	= FALSE;
    msg->u.setup.fwd_sdu_size	= dest_vc->sdu_size;
    msg->u.setup.bwd_sdu_size	= dest_vc->sdu_size;

    LEC_SIGNAL_DEBUG (SEND, msg);

    process_enqueue(atmSig_outputQ, msg);
    return(TRUE);
}


/*
 * lec_send_connect
 */
boolean lec_send_connect (lec_vc_t *dest_vc)
{
    sig_api_msg	*msg;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
        return(FALSE);
    }

    dest_vc->call_state		= CALL_STATE_UP;
    dest_vc->svc		= TRUE;

    msg->opcode			= ATM_CONNECT;
    msg->idb			= dest_vc->swidb;
    msg->u.connect.call_id	= dest_vc->call_id;
    msg->u.connect.vcd		= dest_vc->vcd;
    msg->u.connect.user_vc_type	= dest_vc->lane_vc_type;
    msg->u.connect.flags	= dest_vc->flags;
    msg->u.connect.blli.length	= dest_vc->blli->length;
    bcopy(dest_vc->blli->string,
	  msg->u.connect.blli.string,
	  ATM_BBAND_LOLAYER_MAXSIZE);

    LEC_SIGNAL_DEBUG (SEND, msg);

    process_enqueue(atmSig_outputQ, msg);
    return(TRUE);
}



/*
 * lec_send_release
 */
boolean lec_send_release (lec_vc_t *dest_vc, sig_api_cause_code cause)
{
    sig_api_msg	*msg;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
        return(FALSE);
    }

    msg->opcode			= ATM_RELEASE;
    msg->idb			= dest_vc->swidb;
    msg->u.release.call_id	= dest_vc->call_id;
    msg->u.release.cause	= cause;

    LEC_SIGNAL_DEBUG (SEND, msg);

    dest_vc->call_state		= CALL_STATE_TERMINATING;

    process_enqueue(atmSig_outputQ, msg);
    return(TRUE);
}


/*
 * lec_send_release_comp
 */
boolean lec_send_release_comp (lec_vc_t *dest_vc, sig_api_cause_code cause)
{
    sig_api_msg	*msg;
    lec_info_t  *lec_info;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
        return(FALSE);
    }

    lec_info = lec_info_from_swidb(dest_vc->swidb);
    if (lec_info)
	reg_invoke_lecmib_count_deleted_SVC(lec_info, dest_vc);

    msg->opcode			= ATM_RELEASE_COMPLETE;
    msg->idb			= dest_vc->swidb;
    msg->u.release_comp.call_id	= dest_vc->call_id;
    msg->u.release_comp.cause	= cause;

    LEC_SIGNAL_DEBUG (SEND, msg);

    lec_clear_vc(lec_info, dest_vc);

    process_enqueue(atmSig_outputQ, msg);
    return(TRUE);
}


/*
 * lec_listen
 *
 * Open up the listen for incoming sig_api requests.
 */
boolean lec_listen (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
		return(FALSE);
    }

    msg->opcode 		= ATM_LISTEN_NSAP;
    msg->idb			= lec_info->swidb;
    msg->u.listen.nsap		= lec_info->nsap;
    msg->u.listen.sig_user_callback = lec_sig_callback;
    msg->u.listen.user_handle	= lec_info;

    LEC_SIGNAL_DEBUG (SEND, msg);

    process_enqueue(atmSig_outputQ, msg);

    return(TRUE);
}


/*
 * lec_cancel_listen
 *
 * Cancel the listen for incoming sig_api requests.
 */
boolean lec_cancel_listen (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    msg = malloc(sizeof(sig_api_msg));
    if (!msg) {
	return(FALSE);
    }

    msg->opcode 	= ATM_CANCEL_NSAP;
    msg->idb		= lec_info->swidb;
    msg->u.cancel.nsap	= lec_info->nsap;

    LEC_SIGNAL_DEBUG (SEND, msg);

    process_enqueue(atmSig_outputQ, msg);

    return(TRUE);
}

/*
 * lec_send_ilmi_reg_addr
 *
 * After having determined our ATM address register it with the switch
 * via the ILMI.
 */
int lec_send_ilmi_reg_addr (lec_info_t *lec_info)
{
    int i;
    ilmiClientReq req;
    
    req.client_id = lec_ilmi_clientid;
    req.intf_id	  = lec_info->swidb->hwptr->hw_if_index;
    req.oper_type = ilmiSet;
    req.transid   = (ulong)lec_info;

    req.vblist = &IlmiSetAddressVarBind;
    IlmiSetAddressVarBind.value.sl_value = ILMI_ADD_ADDR;

    for (i=0; i<STATIONLEN_ATMNSAP; i++) {
	IlmiSetAddressVarBind.name->oid_ptr[START_OF_ATM_ADDRESS_INDEX + i] 
	    = lec_info->nsap.addr[i];
    }

    return(ilmi_client_request(&req));
}


/*
 * lec_send_ilmi_unreg_addr
 *
 * In the event that we're deconfigured, the polite thing to do is
 * unregister our ATM address via the ILMI.
 */
int lec_send_ilmi_unreg_addr (lec_info_t *lec_info)
{
    int i;
    ilmiClientReq req;
    
    req.client_id = lec_ilmi_clientid;
    req.intf_id	  = lec_info->swidb->hwptr->hw_if_index;
    req.oper_type = ilmiSet;
    req.transid   = (ulong)lec_info;

    req.vblist = &IlmiSetAddressVarBind;
    IlmiSetAddressVarBind.value.sl_value = ILMI_DELETE_ADDR;

    for (i=0; i<STATIONLEN_ATMNSAP; i++) {
	IlmiSetAddressVarBind.name->oid_ptr[START_OF_ATM_ADDRESS_INDEX + i] 
	    = lec_info->nsap.addr[i];
    }

    return(ilmi_client_request(&req));
}


/*
 * lec_send_get_lecs_addr
 *
 * Have the ILMI send the SNMP GetNext requests to pull in all of the
 * LECS addresses off of the switch.
 */
int lec_send_get_lecs_addr (lec_info_t *lec_info)
{
    ilmiClientReq req;

    req.client_id = lec_ilmi_clientid;
    req.intf_id   = lec_info->swidb->hwptr->hw_if_index;
    req.oper_type = ilmiGetNext;
    req.transid   = (ulong)lec_info;
    req.vblist    = &lecs_var_bind;

    return(ilmi_client_request(&req));
}

