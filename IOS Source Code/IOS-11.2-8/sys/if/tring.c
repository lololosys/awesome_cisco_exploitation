/* $Id: tring.c,v 3.15.12.17 1996/08/28 12:48:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/tring.c,v $
 *------------------------------------------------------------------
 * tring.c -- generic routines for dealing with Token Ring Media
 *
 * 23-Jan-1989, Eric B. Decker & David Hampton
 *
 * Copyright (c) 1988-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tring.c,v $
 * Revision 3.15.12.17  1996/08/28  12:48:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.12.16  1996/08/26  15:09:21  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.15.12.15  1996/08/16  20:20:10  gstovall
 * CSCdi66228:  Token Ring Driver failure
 * Branch: California_branch
 * When setting up the max_pak_size, take into account the fact that the
 * low end platforms default to a ring speed of 0.
 *
 * Revision 3.15.12.14  1996/08/14  17:51:16  vdukki
 * CSCdi35905:  crash - %ALIGN-1-FATAL: Illegal access to a low address -
 * tr_vencap
 * Branch: California_branch
 * add missing check for NULL idb pointer
 *
 * Revision 3.15.12.13  1996/08/13  02:19:09  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.15.12.12  1996/08/07  09:00:01  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.15.12.11  1996/07/23  18:46:12  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.15.12.10  1996/07/23  13:24:01  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.15.12.9  1996/07/22  01:32:50  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.15.12.8  1996/07/09  05:59:03  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.15.12.7  1996/06/28  23:06:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.15.12.6  1996/06/27  09:34:16  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.15.12.5  1996/05/17  11:23:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.15.2.7  1996/05/05  23:10:26  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.15.2.6  1996/05/03  20:46:34  ppearce
 * CSCdi56660:  connIn to CLS/RSRB fails when remote SAP addr not enabled
 * Branch: IbuMod_Calif_branch
 *   Use local SAP address rather than remote SAP address on SAP lookup
 *
 * Revision 3.15.2.5  1996/05/02  22:04:43  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.4  1996/04/26  15:20:40  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.3  1996/04/03  14:39:19  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.15.2.2  1996/03/20  17:54:25  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.15.2.1  1996/03/16  12:09:33  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.15.12.4  1996/05/09  14:30:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.15.12.3.4.1  1996/04/27  07:04:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.15.12.3  1996/04/22  18:13:11  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.15.12.2.6.1  1996/04/08  14:47:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.15.12.2  1996/03/23  01:32:40  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.15.12.1  1996/03/18  20:14:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.4  1996/03/13  01:35:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.10.2.3  1996/03/07  09:44:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.2  1996/02/20  00:57:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/02/13  08:13:16  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.14  1996/02/06  00:01:27  sberl
 * CSCdi47471:  CLS LLC2 sessions may use wrong RIF
 * Check address+RIF_DTE explicitly for 0x01 instead of depending on
 * RIF_LENGTH() macro.
 *
 * Revision 3.13  1996/01/31  18:55:09  rhubbard
 * CSCdi44171:  test-p to virt. mac addr. cause rsrb peer to drop
 *
 * Revision 3.12  1996/01/25  23:11:25  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.11  1996/01/23  08:49:02  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.10.2.1  1996/01/24  22:19:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.10  1996/01/18  03:22:45  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.9.4.1  1996/02/26  20:19:27  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.9  1995/12/30  00:27:58  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.8  1995/12/19  00:40:36  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.7  1995/12/06  18:30:13  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.6  1995/11/30  22:57:47  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.5  1995/11/30  02:59:17  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.4  1995/11/20  22:25:32  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:31:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:03:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/09  00:52:56  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.3  1995/08/07  05:36:54  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.2  1995/07/05  20:49:09  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.1  1995/06/07 20:55:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------------------------------------------------------------------------
 * This module contains low level interface code that is specific to
 * token ring type of media.  It currently supports 802.5 and IBM type
 * token rings running at the 4 Megabit rate of transfere.
 *
 * This module contains idb vector routines generic to token ring type
 * media
 *
 * These routines are organized in the following order:
 *
 * 	idb vector functions and their support routines
 * 	misc token ring functions
 * 	configuration routines
 *
 * Arp and associated code is located with the tr_vencap function.
 *------------------------------------------------------------------
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../decnet/decnet.h"
#include "../ui/debug.h"
#include "ether.h"
#include "logger.h"
#include "../tbridge/tbridge_sde.h"
#include "rif.h"
#include "rif_inline.h"
#include "tring.h"
#include "tring_private.h"
#include "tring_common.h"
#include "../srt/trsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_multiring.h"
#include "tring_inline.h"
#include "tring_debug.h"
#include "network.h"
#include "arp.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../llc2/llc2.h"
#include "../srt/srt_registry.h"
#include "linkdown_event.h"
#include "../snmp/sr_trmib.h"
#include "../if/rif_util.h"
#include "../ibm/netbios_sb.h"
#include "../util/random.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"
#include "../vlan/vlan.h"
#include "../srt/rsrb.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../parser/parser_defs_parser.h"

/*
 * Misc.
 */
#define trebuginf if (tokenevent_debug || tokenring_debug) buginf 

/*
 * The following is a temporary scratch area for token ring drivers to
 * build a text version of a ring status in preparation to printing it.
 * It is highly unlikely that two people connected to the router would
 * actaully need this scratch spot at the same time.  It is only used
 * from the "show interface token" cand "show controller token" commands.
 */
char ring_status_text[TR_RSTAT_BUF_SIZE];

/*
 * Token Ring Broadcast addresses.  There are two!  Can you say bozo.
 * I knew you could.
 * 
 * The following is defined in network.c since it is used by many
 * of the xxx_media.c files.
 *
 * uchar trbaddr1[IEEEBYTES] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
 */

uchar trbaddr2[IEEEBYTES] = {0xC0,0x00,0xFF,0xFF,0xFF,0xFF};
uchar tr_amp[IEEEBYTES]   = {0xC0,0x00,0x00,0x00,0x00,0x01};
uchar tr_crs[IEEEBYTES]   = {0xC0,0x00,0x00,0x00,0x00,0x10};

/*
 * Token Ring Functionals:  (the 0000.8000.0000 bit must be off)
 *
 * tr_dn_multi_t_router		0xC000.4000.0000	TRFUNC_DN_ROUTE
 * tr_dn_multi_t_node		0xC000.2000.0000	TRFUNC_DN_NODE
 * tr_novell_multi		0xC000.0080.0000	TRFUNC_NOVELL
 * tr_esis_8025_all_is_multi	0xC000.0010.8000	TRFUNC_ESIS_IS
 * tr_esis_8025_all_es_multi	0xC000.0008.4000	TRFUNC_ESIS_ES
 * tr_standby_multi		0xC000.0007.0000	TRFUNC_STANDBY
 * tr_isis_8025_all_is_multi	0xC000.0000.8000	TRFUNC_ISIS_IS
 * tr_lan_manager		0xC000.0000.2000	TRFUNC_LANMGR
 * tr_bridge_multi		0xC000.0000.0100	TRFUNC_BRIDGE
 * tr_netbios_multi		0xC000.0000.0080	TRFUNC_NETBIOS
 * tr_report_server		0xC000.0000.0010	TRFUNC_CRS
 * tr_error_mon			0XC000.0000.0008	TRFUNC_REM
 * tr_param_server		0xC000.0000.0002	TRFUNC_RPS
 * tr_active_monitor		0xC000.0000.0001	TRFUNC_MON
 */


/************************************************************************
 *
 * IDB vector functions.
 *
 ************************************************************************/

/*
 * TokenRing state/config vectors.
 */
static boolean tring_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				    tokensb *trsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	  case TOKEN_HW_GROUP:
	    trsb->tr_group = item->u.l_value;
	    break;

	  case TOKEN_HW_FUNCTIONAL:
	    trsb->tr_functional = item->u.l_value;
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean tring_hw_read_config (hwidbtype *hwidb, itemlist *ilist,
				     tokensb *trsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	  case TOKEN_HW_GROUP:
	    item->u.l_value = trsb->tr_group;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_FUNCTIONAL:
	    item->u.l_value = trsb->tr_functional;
	    item->length = sizeof(ulong);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean tring_hw_set_state (hwidbtype *hwidb, itemlist *ilist,
				   tokensb *trsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	  case TOKEN_HW_STATUS_TIME:
	    trsb->tr_status_time = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_LINEUP:
	    trsb->tr_lineup = item->u.l_value;
	    break;

	  case TOKEN_HW_LINEUPTIMER:
	    trsb->tr_lineuptimer = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_RING_STATUS:
	    trsb->tr_ring_status = item->u.s_value[0];
	    break;

	  case TOKEN_HW_MBUS_TIMER:
	    trsb->mbus_tr_timer = item->u.buffer;
	    break;

	  case TOKEN_HW_TEST_TIMER:
	    trsb->tr_test_timer = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_LAST_PANIC:
	    trsb->tr_last_panic = item->u.l_value;
	    break;

	  case TOKEN_HW_LAST_PANIC_TIME:
	    trsb->tr_last_panic_time = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_FAIL_COUNT:
	    trsb->tr_fail_count = item->u.s_value[0];
	    break;

	  case TOKEN_HW_LAST_FAIL:
	    trsb->tr_last_fail = item->u.s_value[0];
	    break;

	  case TOKEN_HW_LAST_FAIL_CODE:
	    trsb->tr_last_fail_code = item->u.l_value;
	    break;

	  case TOKEN_HW_LAST_FAIL_TIME:
	    trsb->tr_last_fail_time = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_NEXT_BUGINF:
	    trsb->tr_next_buginf_time = item_get_timestamp(ilist, item);
	    break;

	  case TOKEN_HW_POLLING_COUNT:
	    trsb->tr_polling_count = item->u.s_value[0];
	    break;

	  case TOKEN_HW_POLLING_FAILURE:
	    trsb->tr_polling_failure = item->u.s_value[0];
	    break;

	  case TOKEN_HW_POLLING_STATE:
	    trsb->tr_polling_state = item->u.l_value;
	    break;

	  case TOKEN_HW_SIGNAL_LOSS:
	    trsb->signal_loss = item->u.l_value;
	    break;

	  case TOKEN_HW_HARD_ERRS:
	    trsb->hard_errs = item->u.l_value;
	    break;

	  case TOKEN_HW_SOFT_ERRS:
	    trsb->soft_errs = item->u.l_value;
	    break;

	  case TOKEN_HW_TX_BEACON:
	    trsb->tx_beacon = item->u.l_value;
	    break;

	  case TOKEN_HW_WIRE_FAULT:
	    trsb->wire_fault = item->u.l_value;
	    break;

	  case TOKEN_HW_RMT_REMOVAL:
	    trsb->rmt_removal = item->u.l_value;
	    break;

	  case TOKEN_HW_SINGLE_STATION:
	    trsb->single_station = item->u.l_value;
	    break;

	  case TOKEN_HW_RING_RECOVERY:
	    trsb->ring_recovery = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_SIGNAL_LOSS:
	    trsb->prev_signal_loss = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_HARD_ERRS:
	    trsb->prev_hard_errs = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_SOFT_ERRS:
	    trsb->prev_soft_errs = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_TX_BEACON:
	    trsb->prev_tx_beacon = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_WIRE_FAULT:
	    trsb->prev_wire_fault = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_RMT_REMOVAL:
	    trsb->prev_rmt_removal = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_SINGLE_STATION:
	    trsb->prev_single_station = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_RING_RECOVERY:
	    trsb->prev_ring_recovery = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_BAD_ACBITS:
	    trsb->prev_bad_acbits = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_LINE:
	    trsb->prev_err_line = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_INTERNAL:
	    trsb->prev_err_internal = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_BURST:
	    trsb->prev_err_burst = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_ARIFCI:
	    trsb->prev_err_arifci = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_ABORT:
	    trsb->prev_err_abort = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_LOSTFRAME:
	    trsb->prev_err_lostframe = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_RCVCONGESTION:
	    trsb->prev_err_rcvcongestion = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_COPY:
	    trsb->prev_err_copy = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_FREQ:
	    trsb->prev_err_freq = item->u.l_value;
	    break;

	  case TOKEN_HW_PREV_ERR_TOKEN:
	    trsb->prev_err_token = item->u.l_value;
	    break;

	  case TOKEN_HW_LAST_BEACON_TYPE:
	    trsb->last_beacon_type = item->u.s_value[0];
	    break;

	  case TOKEN_HW_RINGOPENSTATUS:
	    trsb->ringOpenStatus = item->u.l_value;
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean tring_hw_read_state (hwidbtype *hwidb, itemlist *ilist,
				    tokensb *trsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	  case TOKEN_HW_STATUS_TIME:
	    item_put_timestamp(ilist, item, trsb->tr_status_time);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_LINEUP:
	    item->u.l_value = trsb->tr_lineup;
	    item->length = sizeof(int);
	    break;

	  case TOKEN_HW_LINEUPTIMER:
	    item_put_timestamp(ilist, item, trsb->tr_lineuptimer);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_RING_STATUS:
	    item->u.s_value[0] = trsb->tr_ring_status;
	    item->length = sizeof(ushort);
	    break;

	  case TOKEN_HW_MBUS_TIMER:
	    item->u.buffer = trsb->mbus_tr_timer;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_TEST_TIMER:
	    item_put_timestamp(ilist, item, trsb->tr_test_timer);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_LAST_PANIC:
	    item->u.l_value = trsb->tr_last_panic;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_LAST_PANIC_TIME:
	    item_put_timestamp(ilist, item, trsb->tr_last_panic_time);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_FAIL_COUNT:
	    item->u.s_value[0] = trsb->tr_fail_count;
	    item->length = sizeof(ushort);
	    break;

	  case TOKEN_HW_LAST_FAIL:
	    item->u.s_value[0] = trsb->tr_last_fail;
	    item->length = sizeof(ushort);
	    break;

	  case TOKEN_HW_LAST_FAIL_CODE:
	    item->u.l_value = trsb->tr_last_fail_code;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_LAST_FAIL_TIME:
	    item_put_timestamp(ilist, item, trsb->tr_last_fail_time);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_NEXT_BUGINF:
	    item_put_timestamp(ilist, item, trsb->tr_next_buginf_time);
	    item->length = sizeof(sys_timestamp);
	    break;

	  case TOKEN_HW_POLLING_COUNT:
	    item->u.s_value[0] = trsb->tr_polling_count;
	    item->length = sizeof(short);
	    break;

	  case TOKEN_HW_POLLING_FAILURE:
	    item->u.s_value[0] = trsb->tr_polling_failure;
	    item->length = sizeof(short);
	    break;

	  case TOKEN_HW_POLLING_STATE:
	    item->u.l_value = trsb->tr_polling_state;
	    item->length = sizeof(int);
	    break;

	  case TOKEN_HW_SIGNAL_LOSS:
	    item->u.l_value = trsb->signal_loss;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_HARD_ERRS:
	    item->u.l_value = trsb->hard_errs;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_SOFT_ERRS:
	    item->u.l_value = trsb->soft_errs;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_TX_BEACON:
	    item->u.l_value = trsb->tx_beacon;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_WIRE_FAULT:
	    item->u.l_value = trsb->wire_fault;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_RMT_REMOVAL:
	    item->u.l_value = trsb->rmt_removal;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_SINGLE_STATION:
	    item->u.l_value = trsb->single_station;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_RING_RECOVERY:
	    item->u.l_value = trsb->ring_recovery;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_SIGNAL_LOSS:
	    item->u.l_value = trsb->prev_signal_loss;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_HARD_ERRS:
	    item->u.l_value = trsb->prev_hard_errs;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_SOFT_ERRS:
	    item->u.l_value = trsb->prev_soft_errs;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_TX_BEACON:
	    item->u.l_value = trsb->prev_tx_beacon;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_WIRE_FAULT:
	    item->u.l_value = trsb->prev_wire_fault;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_RMT_REMOVAL:
	    item->u.l_value = trsb->prev_rmt_removal;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_SINGLE_STATION:
	    item->u.l_value = trsb->prev_single_station;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_RING_RECOVERY:
	    item->u.l_value = trsb->prev_ring_recovery;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_BAD_ACBITS:
	    item->u.l_value = trsb->prev_bad_acbits;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_LINE:
	    item->u.l_value = trsb->prev_err_line;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_INTERNAL:
	    item->u.l_value = trsb->prev_err_internal;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_BURST:
	    item->u.l_value = trsb->prev_err_burst;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_ARIFCI:
	    item->u.l_value = trsb->prev_err_arifci;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_ABORT:
	    item->u.l_value = trsb->prev_err_abort;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_LOSTFRAME:
	    item->u.l_value = trsb->prev_err_lostframe;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_RCVCONGESTION:
	    item->u.l_value = trsb->prev_err_rcvcongestion;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_COPY:
	    item->u.l_value = trsb->prev_err_copy;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_FREQ:
	    item->u.l_value = trsb->prev_err_freq;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_PREV_ERR_TOKEN:
	    item->u.l_value = trsb->prev_err_token;
	    item->length = sizeof(ulong);
	    break;

	  case TOKEN_HW_NAUN:
	    item->u.buffer = &trsb->naun;
	    item->length = IEEEBYTES;
	    break;

	  case TOKEN_HW_BEACON_NAUN:
	    item->u.buffer = &trsb->beacon_naun;
	    item->length = IEEEBYTES;
	    break;

	  case TOKEN_HW_LAST_BEACON_TYPE:
	    item->u.s_value[0] = trsb->last_beacon_type;
	    item->length = sizeof(ushort);
	    break;

	  case TOKEN_HW_RINGOPENSTATUS:
	    item->u.l_value = trsb->ringOpenStatus;
	    item->length = sizeof(ulong);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}


static boolean tring_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
				  itemlist *ilist)
{
    boolean	ret;
    tokensb	*trsb;

    if (!(hwidb && ilist))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    trsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    if (trsb == NULL)
	return (FALSE);

    switch (opcode) {
    case IDB_CONTROL_SET_CONFIG:
	ret = tring_hw_set_config(hwidb, ilist, trsb);
	break;

    case IDB_CONTROL_READ_CONFIG:
	ret = tring_hw_read_config(hwidb, ilist, trsb);
	break;

    case IDB_CONTROL_SET_STATE:
	ret = tring_hw_set_state(hwidb, ilist, trsb);
	break;

    case IDB_CONTROL_READ_STATE:
	ret = tring_hw_read_state(hwidb, ilist, trsb);
	break;

    default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*trsb->super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}

/*
 * token_et_mac
 *
 * Do the MAC layer only encapsulation for a Token Ring.
 * Called by SERVICE_MACWRITE_TOKEN, indexed by ET_MAC.
 */
static boolean token_et_mac (paktype *pak, int linktype, char *daddr,
			     char *saddr, char const *other_info,
			     riftype *rifptr)
{
    tring_hdr *trh;
    int riflen;

    riflen = rifptr ? rifptr->length : 0;
    pak->datagramstart -= (TRING_8025_ENCAPBYTES + riflen);
    pak->datagramsize += (TRING_8025_ENCAPBYTES + riflen);
    trh = (tring_hdr *)pak->datagramstart;
    trh->ac = AC_PRI0;		/* could set priority here */
    if (pak->linktype == LINK_MAC)
        trh->fc = FC_TYPE_MAC;
    else
        trh->fc = FC_TYPE_MAC_EXP;
    ieee_copy(daddr, trh->daddr);
    ieee_copy(saddr, trh->saddr);
    if (riflen) {
	trh->saddr[0] |= TR_RII;	/* indicate there is a RIF */
	bcopy(rifptr->rif, trh->data, riflen);
    }
    return(TRUE);
}

/*
 * tr_vencap
 * Perform 802.5/802.2 encapsulation
 *
 * Group addresses of various persuasions are sent out with explorer rifs
 * if srb_multiring is enabled for the protocol of the frame being
 * transmitted. Normally these use the spanning version.  ARP and RARP
 * could be different but aren't.  You might want them to be different
 * (use all routes) to maximize the chances of finding something.  But it
 * is better to not work and call attention to a network problem rather
 * than working for some things.
 *
 * Note that the above is changed significantly by the presence of STBs.
 *
 * If the output interface has multi-ring on, non-group/multicast packets
 * in a multiring environment are sent after successfully looking up
 * their rif component in the rif-cache.  If multi-ring is not on no RIF
 * is ever used.
 *
 * We use trbaddr1 (the all ones address) as our default broadcast address.
 */

boolean tr_vencap (
    paktype *pak,
    long address)
{
    uchar *daddr = NULL;
    uchar const *other_info = zerosnap;
    int enctype = ET_SNAP;
    riftype *rifptr, rif_local_copy;
    tring_hdr *trh;		/* pointer to TR MAC header */
    sap_hdr *sap;	/* pointer to SAP header */
    snap_hdr *snap;		/* pointer to SNAP header */
    int riflen;			/* length of RIF in TR header */
    uchar source[IEEEBYTES];
    idbtype *interface;
    hwidbtype *hwidb;
    leveltype level;

    ulong rif_flags = RIF_NOT_PRESENT;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);

    /*
     * Find out the encapsulation type, destination address, and token
     * ring specific information.
     */

    if (!reg_invoke_media_token_macaddr(pak->linktype, pak, address, &enctype,
				    &daddr, &other_info, &rif_flags)) {
	if (!reg_used_media_token_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), interface->namestring,
		   pak->linktype);
	return(FALSE);
    }

    pak->enctype = enctype;

    /*
     * Set up correct source addresses for later use.
     */
    if (pak->flags & PAK_LACK_OUT) {
	ieee_copy((uchar *)(&(daddr[LMAC_DTE])), source);
	/* Don't change source address, but build FCS */
	pak->flags &= ~PAK_SRB_OUT;
	pak->flags |= PAK_SRB_EXPLORER;
    } else {
	if (hwidb == NULL)
	    return(FALSE);
	ieee_copy(hwidb->hardware, source);
    }

    /*
     * Look the daddr up in the rif cache to see how we should handle it.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (rif_flags & RIF_IN_LLC_DTE) {
	if (*((uchar*)address + RIF_DTE) == 0x01) {
	    /* 
	     * Illegal rif control byte signals that riflength is
	     * really zero! Send no RIF.
	     */
	    riflen = 0;
	} else {
	    riflen = RIF_LENGTH(*((uchar*)address + RIF_DTE));
	    bcopy((uchar *)(address + RIF_DTE), 
		  &rif_local_copy.rif[0], 
		  riflen);
	}
	rif_local_copy.length = riflen;
	goto rif_found;

    } else  if (pak->flags & PAK_LACK_OUT) {
	rifptr = rif_lookup((uchar *) address, hwidb, (uchar*)(daddr + 8),
			    rif_flags);
    } else if (rif_flags & RIF_REQUIRED) {
	rifptr = rif_lookup((uchar *) address, hwidb, NULL, rif_flags);
    } else if (rif_flags & RIF_PRESENT) {
	/*
	 * If not found then dump.  This may get us into trouble but to
	 * hang onto the packet now while doing route exploration has
	 * significant structural ramifications.  That is our caller
	 * expects for things to either work or not work.  We now have
	 * another possibility that of delayed.  Gag.  Because we are an
	 * idb function this potentially effects the entire driver layer.
	 *
	 * This routine will return the proper RIFs for broadcast,
	 * group, and functional addresses.
	 */
	rifptr = rif_lookup_callback(daddr, hwidb, NULL, rif_flags, 
				    pak->linktype, interface, address);
	if (!rifptr) {
	    reset_interrupt_level(level);
	    return(FALSE);
	}
    } else {
	rifptr = NULL;
    }
    if (rifptr) {
	bcopy(rifptr, &rif_local_copy, sizeof(riftype));
    } else {
	memset(&rif_local_copy, 0, sizeof(riftype));
    }

 rif_found:
    rifptr = &rif_local_copy;
    riflen = rifptr->length;
    reset_interrupt_level(level);

    /*
     * We know the destination Token Ring address, now encapsulate the
     * datagram according to method.  Quit if no method has emerged by
     * now.
     */
    if (!enctype || !daddr)
	return(FALSE);

    switch (enctype) {

      case ET_SAP:
	pak->datagramstart -= (TRING_SAP_ENCAPBYTES + riflen);
	pak->datagramsize += (TRING_SAP_ENCAPBYTES + riflen);
	trh = (tring_hdr *)pak->datagramstart;
	trh->ac = AC_PRI0;		/* could set priority here */
	trh->fc = FC_TYPE_LLC;
	ieee_copy(daddr, trh->daddr);
	ieee_copy(source, trh->saddr);
	if (riflen > 0) {
	    trh->saddr[0] |= TR_RII;	/* indicate there is a RIF */
	    bcopy(rifptr->rif, trh->data, riflen);
	}
	sap = (sap_hdr *)(&trh->data[riflen]);
	sap->dsap    = other_info[0];
	sap->ssap    = other_info[1];
	sap->control = other_info[2];

	pak->mac_start  = (uchar *)trh;
	pak->addr_start = trh->daddr;
	pak->rif_start  = (riflen ? trh->data : NULL);
	pak->info_start = (uchar *)sap;
	pak->encsize    = TRING_SAP_ENCAPBYTES + riflen;
	break;

      case ET_SNAP:
      case ET_SDE:

        /*
         * For traffic we generate (e.g. Routing Protocol updates) through
         * dot10 vLAN configured FDDI interfaces (i.e. "ET_SDE"), we assume
         * them to start out as SNAP encapsulated and will then rewrite the
	 * header as appropriate below ...
         */

	pak->datagramstart -= (TRING_SNAP_ENCAPBYTES + riflen);
	pak->datagramsize += (TRING_SNAP_ENCAPBYTES + riflen);
	trh = (tring_hdr *)pak->datagramstart;
	trh->ac = AC_PRI0;		/* could set priority here */
	trh->fc = FC_TYPE_LLC;
	ieee_copy(daddr, trh->daddr);
	ieee_copy(source, trh->saddr);
	if (riflen > 0) {
	    trh->saddr[0] |= TR_RII;	/* indicate we are here */
	    bcopy(rifptr->rif, trh->data, riflen);
	}
  	snap = (snap_hdr *) (&trh->data[riflen]);
	snap->dsap    = SAP_SNAP;
	snap->ssap    = SAP_SNAP;
	snap->control = LLC1_UI;
	snap->oui[0]  = other_info[0];
	snap->oui[1]  = other_info[1];
	snap->oui[2]  = other_info[2];
	PUTSHORT(&snap->type, link2snaptype(pak->linktype));

	pak->mac_start  = (uchar *)trh;
	pak->addr_start = trh->daddr;
	pak->rif_start  = (riflen ? trh->data : NULL);
	pak->info_start = (uchar *)snap;
	pak->encsize    = TRING_SNAP_ENCAPBYTES + riflen;
	break;

      default:
	if (!reg_invoke_media_token_macwrite(enctype, pak,
					     pak->linktype, daddr, source,
					     other_info, rifptr)) {
	    errmsg(&msgsym(BADENCAP, LINK), interface->namestring,
		   enctype);
	    return(FALSE);
	}
	break;
    }

    /*
     * Now check to see if the outgoing interface is actually a VLAN interface
     * If so then build a dot10 packet, (currently ISL not supported on FDDI).
     * Note the encapsulation at this point will be ARPA (Ethernet V2) which
     * becomes the interior packet's encapsulation, this is as expected.
     *
     * Increase the encapsulation size as used by the cache update routines
     * to figure out how much MAC to prepend.
     *
     * Note this is a generic routine, so need to use a fast registry call :-(
     */

    if (IS_DOT10_VLAN_SUBINTERFACE(interface)) {

        pak->addr_start = pak->datagramstart + TR_ACFC_SIZE;
        pak->datagramstart -= SDE_ENCAPBYTES;

        trh = (tring_hdr *)pak->datagramstart;
        trh->ac = AC_PRI0;
        trh->fc = FC_TYPE_LLC;

        if (!reg_invoke_write_dot10_encapsulation(pak, interface))
            return(FALSE);

        reg_invoke_vlan_packet_counters(pak, ET_SDE);
        pak->datagramsize += SDE_ENCAPBYTES;
        pak->encsize += SDE_ENCAPBYTES;
    }

    /*
     * Make sure routing cache is populated by transit traffic.  Don't
     * do it for LAN emulation.  It will get populated out of atm_vencap. 
     */

    if (!is_atm_lane(interface)) {
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    }

    return(TRUE);
}


/*
 * tring_getlink
 *
 * Return a generic link code for a token ring frame
 */ 
long tring_getlink (paktype *pak)
{
    sap_hdr *sap;
    snap_hdr *snap;

    /*
     * First try differentiating based solely upon the encapsulation type.
     */
    switch (pak->enctype) {
      case ET_SNAP:			/* SNAP */
	snap = (snap_hdr *)pak->info_start;
	return(type2link(GETSHORT(&snap->type)));
	
      case ET_SAP:
	sap = (sap_hdr *)pak->info_start;
	pak->llc2_cb = NULL;	/* indicate UI for LLC2 */
	return(sap2link(sap->dsap));
	
      case ET_LLC2:
	return(sap2link(pak->llc2_sap));
	
      case ET_3COM_TR:        return(LINK_XNS);
      case ET_APOLLO:	      return(LINK_APOLLO);
      case ET_MAC:	      return(LINK_MAC);

      case ET_SDE:
        sap = (sap_hdr *)pak->info_start;
        return(sap2link(sap->dsap));

      default:
	return(reg_invoke_media_getlink(pak->enctype, pak->if_input, pak));
    }
    return(LINK_ILLEGAL);
}

/*
 * tr_bcast
 * Return TRUE if the destination address matches either of the two (!)
 * broadcast addresses -- 0xFFFF FFFF FFFF or 0xC000 FFFF FFFF
 */
fuzzy tr_bcast (hwidbtype *hwidb, paktype *pak)
{
    tring_hdr *trh;		/* pointer to TR MAC header */
 
    trh = (tring_hdr *)pak->datagramstart;
    if ((GETLONG(&trh->daddr[2]) == 0xFFFFFFFF) &&
	((GETSHORT(&trh->daddr[0]) == 0xFFFF) ||
	 (GETSHORT(&trh->daddr[0]) == 0xC000)))
	return(FUZ_TRUE);
    else
	return(FUZ_FALSE);
}

/*
 * tr_periodic
 * 
 * Period processing for the token ring interface.  Sends a keepalive
 * message every idb->keep_period, and checks whether one has been
 * received in the last 2*idb->keep_period.  If not, decrement
 * idb->lineup counter (which starts out at TR_KEEP_COUNT, 5), and if
 * this hits 0 declare the interface down.
 */

static void tr_periodic (hwidbtype *idb)
{
    boolean newstate, oldstate;
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    if (idb->state == IDBS_ADMINDOWN || idb->state == IDBS_RESET) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return;
    }
    if (idb->nokeepalive) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return;
    }

    /* If initializing give the interface 60 seconds to respond else shut
     * it down.  The user will have to do a "no shut" to get the system to
     * try again.  */

    if (idb->state == IDBS_INIT) {
        if (!TIMER_RUNNING(tsb->tr_lineuptimer)) {
            TIMER_START(tsb->tr_lineuptimer, 30*ONESEC);
        } else if (AWAKE(tsb->tr_lineuptimer)) {
            buginf("\n%s: Failed to initialize, shutting down ",
		   idb->hw_short_namestring);
            TIMER_STOP(tsb->tr_lineuptimer);
            (*idb->shutdown)(idb, IDBS_RESET);
        }
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
        return;
    }

    /* If there is a measurable packet rate then assume the ring is OK.
     * Adjust the timer so the keepalive doesn't timeout even if the
     * packet is dropped.  */

    if(((ELAPSED_TIME(idb->lastinput)/1000) < idb->keep_period) &&
              ((ELAPSED_TIME(idb->lastoutput)/1000) < idb->keep_period)) {
        GET_TIMESTAMP(tsb->tr_lineuptimer);
        tsb->tr_lineup = TR_KEEP_COUNT;
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return;
    }
 
    if (idb->counters.rx_packetrate > 100 || idb->counters.tx_packetrate > 100)
	GET_TIMESTAMP(tsb->tr_lineuptimer);
 
    tr_send_loop(idb, idb->hardware);
    if (TIMER_RUNNING(tsb->tr_lineuptimer)) {
	newstate = CLOCK_IN_INTERVAL(tsb->tr_lineuptimer,
				     2*ONESEC*idb->keep_period);
	if (!newstate)
	    TIMER_STOP(tsb->tr_lineuptimer);
    } else {
	newstate = FALSE;
    }
    oldstate = tsb->tr_lineup ? TRUE : FALSE;
    if (newstate != oldstate) {
	if (newstate) {
	    /*
	     * we've heard a keepalive recently, and the protocol was not
	     * up previously, so make it be up.
	     */
	    tsb->tr_lineup = TR_KEEP_COUNT;
	    idb->reason = "Keepalive OK";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	    if (tokenring_debug)
	      buginf("\n%s: Keepalive OK", idb->hw_short_namestring);
	} else if (--tsb->tr_lineup <= 0) {
	    tsb->tr_lineup = 0;		/* the bugger goes down */
	    /*
	     * we haven't heard a keepalive recently, and furthermore,
	     * we've exhausted the keepalive counter.  Bring down the 
	     * interface
	     */
            if (idb->failCause == 0)       /* Dont overwrite failCause */
	        idb->failCause = TR_KeepAliveFailure; /* NmEvent Hook 9 */

	    if (tokenevent_debug || tokenring_debug)
	      buginf("\n%s: Keepalive failed, resetting",
		     idb->hw_short_namestring);
	    /*
	     * Adjust routing to reflect that this interface went down.
	     * reset will do this too but possibly after a slight delay.
	     */
	    (*idb->reset)(idb);
	    route_adjust(idb);
	}
    } else if (newstate)
      /*
       * we received a loopback recently, and the protocol is up.  Just
       * refresh the keepalive counter
       */
      tsb->tr_lineup = TR_KEEP_COUNT;

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/*
 * mac_macaddr_token
 *
 * Turn an Token Ring loopback address into an encapsulation type and
 * a MAC address. Called by SERVICE_TOKEN_MACADDR, indexed by LINK_MAC.
 */
static boolean mac_macaddr_token (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **snapaddr,
				  ulong *rif_flags)
{
    *enctype = ET_MAC;
    *daddr = (uchar *)address;
    *snapaddr = NULL;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * loop_macaddr_token
 *
 * Turn an Token Ring loopback address into an encapsulation type and
 * a MAC address. Called by SERVICE_TOKEN_MACADDR, indexed by LINK_LOOP.
 */
static boolean loop_macaddr_token (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **snapaddr,
				   ulong *rif_flags)
{
    *enctype = ET_SNAP;
    *daddr = (uchar *) address;
    *snapaddr = zerosnap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * tr_send_loop
 * Generate and send a loopback packet to ourselves.  
 */

void tr_send_loop (hwidbtype *idb, uchar *dest)
{
    ushort *ptr;
    paktype *pak;
    int len;

    pak = getbuffer(SMALLDATA);
    if (!pak)
	return;
    ptr = (ushort *)pak->network_start;
    *ptr++ = h2lit(0);			/* skip */
    *ptr++ = h2lit(LOOP_REPLY);		/* function */
    *ptr++ = random_gen() & 0xFFFF;
    len = (int)ptr - (int)pak->network_start;
    pak->if_output = idb->firstsw;
    pak->flags |= (PAK_HEARSELF | PAK_PRIORITY);
    pak->linktype = LINK_LOOP;
    if (pak_encap(idb, pak, len, (long)dest))
	datagram_out(pak);
    else
	retbuffer(pak);
}

/* 
 * tr_rcvloop
 * Process a received loopback packet.
 */

static void tr_rcvloop (paktype *pak)
{
    int skip, len;
    uchar *dataptr = pak->network_start;
    uchar addr[6];
    ushort *ptr;
    hwidbtype *idb;
    tring_hdr *trh;
    tokensb *tsb;

    /*
     * Get the skip count, make sure it is even, and that the
     * function word pointed to is within the packet.
     */
    idb = pak->if_input->hwptr;
    idb->counters.rx_frames[ACCT_PROTO_OTHER]++;
    idb->counters.rx_bytes[ACCT_PROTO_OTHER] += pak->datagramsize;
    skip = lit2h(*(ushort *)dataptr);
    ptr = (ushort *)(dataptr + skip + 2);
    if ((skip & 1) || 
	((uchar *)ptr + 2 > pak->datagramstart + pak->datagramsize)) {
	retbuffer(pak);
	return;
    }

    switch (lit2h(*ptr++)) {		/* process the function word */
    case LOOP_FORWARD:
	/*
	 * Make sure the forward address is within the packet.
	 */
	if ((uchar *)ptr + IEEEBYTES > pak->datagramstart + pak->datagramsize)
	    break;
	*(ushort *)dataptr = h2lit(skip + IEEEBYTES + 2); /* update skip */
	pak->if_output = pak->if_input;
	pak->linktype = LINK_LOOP;
	len = pak->datagramstart + pak->datagramsize - dataptr;
	/*
	 * Assume that the MAC address is in canonical form and that
	 * we need to bitswap it.
	 */
	ieee_swap((uchar *)ptr, addr);
	if (pak_encap(idb, pak, len, (long)addr)) {
	    datagram_out(pak);
	    return;
	}
	break;
       
    case LOOP_REPLY:
	/*
	 * Check that that the source and destination are both the 
	 * address of the interface it came from, and if so, set 
	 * the idb->lineuptimer to the clock.
	 */
	tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
	trh = (tring_hdr *)pak->datagramstart;
	if (ieee_equal(&trh->daddr[0], idb->hardware) &&
	    ieee_equal(&trh->saddr[0], idb->hardware)) {
	    GET_TIMESTAMP(tsb->tr_lineuptimer); /* for old TRs */
	    idb->mci_lineuptimer = tsb->tr_lineuptimer; /* for CTRs */
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	break;
    }
    retbuffer(pak);
}

/*
 * tr_functional_init
 * Setup the idb->tr_function cell
 *
 * Various protocols use hardcoded functional addresses (multicasts)
 * instead of broadcasts.  Turn the appropriate functional address bits
 * on or off here.
 */

void tr_functional_init (hwidbtype *hwidb)
{
    idbtype *swidb;
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    tsb->tr_functional = 0L;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (RUNNING_IF(LINK_DECNET, swidb)) {
 	    tsb->tr_functional |= 
		GETLONG(&hwidb->dn_func->dn_tr_iv_routers.lower);
 	    tsb->tr_functional |= 
		GETLONG(&hwidb->dn_func->dn_tr_iv_prime_routers.lower);
	}
	if (RUNNING_IF(LINK_NOVELL, swidb))
	    tsb->tr_functional |= TRFUNC_NOVELL;
	if (RUNNING_IF(LINK_CLNS, swidb))
	    tsb->tr_functional |= (TRFUNC_ESIS_ES | TRFUNC_ESIS_IS);
	if (RUNNING_IF(LINK_LAT, swidb))
	    tsb->tr_functional |= TRFUNC_LAT;
	if (RUNNING_IF(LINK_CDP, swidb))
	    tsb->tr_functional |= TRFUNC_CDP;
        if (RUNNING_IF(LINK_NBF, swidb))
 	    tsb->tr_functional |= TRFUNC_NETBIOS;

	reg_invoke_media_tring_add_functional(hwidb, swidb);
    }
    if (hwidb->tr_ring_mode & RINGMODE_RPS)
	tsb->tr_functional |= TRFUNC_RPS;
    if (hwidb->tr_ring_mode & RINGMODE_REM)
	tsb->tr_functional |= TRFUNC_REM;
    if (hwidb->tr_ring_mode & RINGMODE_NETMGR)
	tsb->tr_functional |= TRFUNC_CRS;

    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}

/*
 * Get the max MTU for a particular protocol.
 *
 * The default is the max MTU of the interface.
 */
static int tring_max_proto_mtu (
    idbtype *swidb,
    enum LINK linktype,
    boolean get_default)
{
    int value;
    value = reg_invoke_media_tring_mtu(linktype, swidb, get_default);
    return(value ? value : swidb->hwptr->maxmtu);
}

/************************************************************************
 *
 * Misc. Token Ring functions
 *
 ************************************************************************/
  
/*
 * tr_print_packet
 * Checks access list for tokenring_debug
 * Return TRUE if packet is printed.
 * 
 */
char trprint_rifstr[SRB_MAXRIF*2+SRB_MAXRIF/2];

boolean tr_print_packet (
    hwidbtype *idb,
    char      *label,			/* label to mark it with */
    tring_hdr *trh,			/* pointer to TR MAC header */
    ulong     *llc,			/* pointer to llc data */
    ushort    buf_flags,		/* type of buffer */
    int size)
{
    char *type;
    int unit;
    int llc_offset = IEEEBYTES * 2 + 2;
    debug_list_t *list;

    SAVE_CALLER();

    list = debug_listq_find(&tokenring_debug);
    if (idb && list && (list->idb != NULL) && (list->idb != idb->firstsw)) {
	return (FALSE);
    }
   if (list && list->access_list &&
       !(reg_invoke_xmacaccesscheck_gen(
					list->access_list,
					(mac_address_type *)trh->daddr,
					(mac_address_type *)trh->saddr,
					(uchar *)llc,
					llc_offset - 2,
					/* Let's count from daddr*/
					size))) {
       return (FALSE);
   }

    unit = idb ? idb->unit : 99;
    buginf("\n%s: %s MAC: acfc: 0x%-04x Dst: %e Src: %e  bf: 0x%-02x 0x%x",
	   idb->hw_short_namestring, label, ((ushort) trh->ac << 8) | trh->fc,
	   trh->daddr, trh->saddr, buf_flags, caller());
    if (trh->saddr[0] & TR_RII)
	buginf("\n%s: %s RIF: %s", idb->hw_short_namestring, label,
	     get_rif_str((uchar *) trh->data, trprint_rifstr));
    type = ((trh->fc & FC_TYPE_MASK) == FC_TYPE_MAC) ? "MAC" : "LLC";
    buginf("\n%s: %s %s: %-08x %-08x %-08x %-08x %-08x %-08x ln: %d",
	   idb->hw_short_namestring, label, type,
	   GETLONG(&llc[0]), GETLONG(&llc[1]), GETLONG(&llc[2]),
	   GETLONG(&llc[3]), GETLONG(&llc[4]), GETLONG(&llc[5]),  size);

       return(TRUE);
}


/*
 * tring_cstate
 * change state and perform various housekeeping type functions.
 * This is NOT an interface vector function.  It is intended to be
 * called from a periodic invokator such as cstate_check.  This
 * implements state hysteresis.
 */

static void tring_cstate (hwidbtype *idb)
{
    idbtype *swidb;

    if (idb->state == IDBS_UP) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    reg_invoke_arp_reload(swidb);
	}
    } else if (idb->state == IDBS_DOWN ||
	       idb->state == IDBS_ADMINDOWN ||
	       idb->state == IDBS_RESET) {
	/*
	 * must be hard down.  Clean out the hold queues.  idb->pakout
	 * is handled by the interrupt handler.
	 */
	holdq_reset(idb);
	rif_ager(TRUE, FALSE);	/* unlearn rifs (flush all) */
    }
}


/*
 * tr_setup_test
 * Setup a response to a test command packet.
 */


static paktype *tr_setup_test (
    uchar *dst,
    uchar *src,
    sap_hdr *llc,
    riftype *rifraf,
    int llcsize,
    hwidbtype *idb)
{
    paktype *pak;
    tring_hdr *trframe;
    int riflen;
    uchar localSap;

    pak = getbuffer(llcsize);
    if (!pak) {
	return(NULL);
    }
    riflen = (rifraf ? rifraf->length : 0);
    trframe = (tring_hdr *)(pak->network_start - TRING_ENCAPBYTES - riflen);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy_sans_rii(dst, trframe->daddr);
    ieee_copy(src, trframe->saddr);
    if (riflen) {
      bcopy(rifraf->rif, trframe->data, riflen);
      trframe->saddr[0] |= TR_RII;
      trframe->data[0] &= ~RC_RTMASK;

      /*
       * for SDLLC limit max frame size to the value given for the interface
       */
      if (idb->sdllc_active)
	  trframe->data[1] = (trframe->data[1] & ~(RC_LFMASK)) |
	      idb->sdllc_largest_token_frame;
    }
    bcopy((uchar *)llc, pak->network_start, llcsize);
    llc = (sap_hdr *)pak->network_start;
    localSap = llc->dsap;
    llc->dsap = llc->ssap & ~SAP_RESPONSE;
    llc->ssap = localSap | SAP_RESPONSE;
    pak->if_output     = idb->firstsw;
    pak->enctype       = idb->enctype;
    pak->datagramstart = (uchar *) trframe;
    pak->datagramsize  = llcsize + TRING_ENCAPBYTES + riflen;
    /*
     * Because idb->oqueue is overloaded for cls->rsrb and 
     * rsrb->cls, we need to specify direction here...
     * 
     * Only do this if the dest Sap is a CLS SAP
     * [pmorton 3/2/95]
     */
    if (idb->status & IDB_VIRTUAL)
    {
    sap_entry_t                 *sap_entry;

      sap_entry = sap_get_entry(idb->firstsw, localSap);
      /* 
       * verify this is LINK_CLS type. 
       * addded inline macro  Cls_Is_Idb_Virtual  to determine
       * between CLS and legacy link type .
       */
      if ( (Cls_Is_Idb_Virtual(idb)) && (sap_entry != NULL) &&
	 ((sap_entry->sap_flags) & SAP_CLS_ACT_SAP) )
	  pak->linktype = LINK_CLS;
      else
	  pak->linktype = LINK_LLC2;
    }    
    return(pak);
}

/*
 * Tables to convert error values to strings.  These are used by
 * various printing routines.
 */
char *const tr_panic_types[LAST_SCA_FAIL] = {
    "unknown",
    "bad debugger exit",
    "stack overflow",
    "stack underflow",
    "stack guard",
    "unexpect interrupt",
    "parity error",
    "timeout",
    "multibus access",
    "protocol error",
    "bad alignment",
    "segment wrapped",
    "address out of range",
    "debugger protocol",
    "memory trash",
    "t2m protocol",
    "t2m protocol sequence",
    "t2m protocol clogged",
    "t2m protocol busy",
    "bad receive completion",
    "rlist",
    "tlist",
    "TMS380 initialization",
    "TMS380 command failed",
    "TMS380 adapter check",
    "TMS380 receive",
    "TMS380 transmit",
    "TMS380 close",
    "TMS380 transmit interrupt",
    "TMS380 receive interrupt",
    "receive pointer",
    "transmit pointer",
    "multibus memory test",
    "recovery mode",
    "info",
    "checksum failed",
    "MADGE bad ARB command",
    "MADGE bad SRB command",
    "MADGE bad SSB command",
    "MADGE open timeout",
    "MADGE change mode timeout",
    "MADGE configure bridge timeout",
    "MADGE set address timeout",
    "MADGE read error log timeout"
};

/*
 * Convert a ring status value to a string.
 */
static const tr_rstattab_t tr_rstattab[] = {
    { RNG_SIGNAL_LOSS, "Signal Loss" },
    { RNG_HARD_ERROR, "Hard Error" },
    { RNG_SOFT_ERROR, "Soft Error" },
    { RNG_BEACON, "Ring Beacon" },
    { RNG_WIRE_FAULT, "Wire Fault" },
    { RNG_HW_REMOVAL, "HW Removal" },
    { RNG_RMT_REMOVAL, "Remote Removal" },
    { RNG_CNT_OVRFLW, "Counter Overflow" },
    { RNG_SINGLE, "Only station" },
    { RNG_RECOVERY, "Ring Recovery" },
    { 0, NULL },
};

static char rstat_int_text[TR_RSTAT_BUF_SIZE];

/*
 * decode_ring_status
 * Decode the ring status bits into a string
 */

void decode_ring_status (char *buf, ushort ringstat)
{
    char delim;
    int i;

    delim = '<';
    for (i=0; tr_rstattab[i].bit; i++) {
	if (ringstat & tr_rstattab[i].bit) {
	    buf += sprintf(buf, "%c%s", delim, tr_rstattab[i].msg);
	    delim = ',';
	}
    };
    if (ringstat & RNG_UNDEFINED)
	buf += sprintf(buf, "%cundefined(%04x)", delim,
		       ringstat&RNG_UNDEFINED);
    *buf++ = '>';
    *buf = '\0';
}

boolean tr_ringstat_common (hwidbtype *idb, ushort ringstat, boolean display)
{
    char *p;
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /*
     * For CTR, the call the lanmgr_ringstat() is handled by
     * ctr_read_ringstat() under different conditions.
     */
    if (idb->state == IDBS_UP && idb->type != IDBTYPE_CTR)
	reg_invoke_media_tring_ring_status(idb, ringstat);
    if (ringstat & RNG_SIGNAL_LOSS)
      tsb->signal_loss++;
    if (ringstat & RNG_HARD_ERROR)
      tsb->hard_errs++;
    if (ringstat & RNG_SOFT_ERROR)
      tsb->soft_errs++;
    if (ringstat & RNG_BEACON){
      tsb->tx_beacon++;
      idb->failCause = TR_beacon;/* NmEvent Hook 10 */
    }
    if (ringstat & RNG_WIRE_FAULT){
      tsb->wire_fault++;
      idb->failCause = TR_wireFault; /* NmEvent Hook 7 */
    }
    if (ringstat & RNG_RMT_REMOVAL){
      tsb->rmt_removal++;
      idb->failCause = TR_remoteRemoval;/* NmEvent Hook 8 */
    }
    if (ringstat & RNG_SINGLE)
      tsb->single_station++;
    if (ringstat & RNG_RECOVERY)
      tsb->ring_recovery++;

    if (ringstat & RNG_FATAL) {
	if ((idb->state == IDBS_INIT) || (idb->state == IDBS_UP)) {
	    net_cstate(idb, IDBS_DOWN);
	    idb->counters.transitions++;
	}
    } else {
	if (idb->state == IDBS_DOWN) {
	    net_cstate(idb, IDBS_UP);
	    tsb->tr_lineup = TR_KEEP_COUNT; /* avoid spurious */
	    GET_TIMESTAMP(tsb->tr_lineuptimer);
	    idb->counters.transitions++;
	    /* restart output */
	    (*idb->soutput)(idb);
	}
    }

    /*
     * Should we display this error?
     */
    if (ringstat & (RNG_FATAL & ~RNG_WIRE_FAULT))
	display = TRUE;

    if ((ringstat & ~(RNG_SOFT_ERROR | RNG_CNT_OVRFLW)) == 0
	&& !tokenring_debug) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }

    if (tokenevent_debug || tokenring_debug || display) {
	/*
	 * put out a nice message describing the status.  We create the
	 * message in a static buffer to save stack space.  This is OK
	 * since this routine runs at interrupt level, and there can
	 * therefore be only one such routine running at a time.
	 */
	if (!ringstat) {
	    errmsg(&msgsym(STATE, TR), idb->unit,
		   ((idb->state == IDBS_UP) ? "Up" : "Down"));
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return(FALSE);
	}

	p = &rstat_int_text[0];
	decode_ring_status(rstat_int_text, ringstat);
	errmsg(&msgsym(STATRING, TR), idb->unit,
	       ((idb->state == IDBS_UP) ? "Up" : "Down"), rstat_int_text);
    }

    if (ringstat & RNG_WIRE_FAULT) {
	errmsg(&msgsym(WIREFAULT, TR), idb->unit);
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    return(display);
}


/* One common routine to display Open errors and alerts. 
 * Used by lower level routines.
 */
void TR_OpenError (hwidbtype *idb,               
		   ushort  error, 
		   ushort  phase, 
		   boolean display)
{
/*
 * Open Phases and Open Errors.  These are used to decode
 * the reason the open failed.
 */

static char *const tr_open_phases[] = {
    "reserved (0)",
    "Lobe Test",
    "Phys. Insertion",
    "Addr. Verify",
    "Ring Poll",
    "Init Request"
  };

static char *const tr_open_errors[] = {
    "reserved (0)",
    "failed",
    "signal loss",
    "wire fault",
    "ring speed mismatch",
    "timeout",
    "ring failure",
    "ring beaconing",
    "duplicate address",
    "request params not answered",
    "removed"
  };
enum openPhasesType {
    TrLobeTest = 1,
};

enum openErrorType {
    TrFailed = 1,
    TrSignalLoss,
    TrRingFail = 6,
    TrRingBeacon,
    TrDupAddr,
    TrReqParmsNotAnswer,
    TrRemoved
};

    if (tokenring_debug || tokenevent_debug || display) 
       	if (phase == 0 && error == 0)
       	    errmsg(&msgsym(OPENFAIL2, TR), idb->unit);
       	else
       	    errmsg(&msgsym(OPENFAIL, TR),
       		   idb->unit, 
                   tr_open_phases[phase],
       		   tr_open_errors[error]);

    if (display && error != 0 && phase != 0){
        if (phase == TrLobeTest)
            /* NmEvent Hook 2 */
            idb->failCause = TR_openFailLobeTest;
        else {
          switch (error) {
             case TrRingBeacon: 
                /* NmEvent Hook 3 */
                idb->failCause = TR_openFailRingBeacon;
                break;
             case TrDupAddr:
                /* NmEvent Hook 4 */
                idb->failCause = TR_openFailDupAddr;
                break;
             case TrRemoved:
                /* NmEvent Hook 5 */
                idb->failCause = TR_openFailRemove;
                break;
             default: /* unexpected alert */
               /* NmEvent Hook 6 */
               idb->failCause = TR_openFailUnexpected;
          }
       }
    } 
}


/* 
 * tr_respond_xid
 *
 * Common routine used to respond to both IEEE and SNA XIDs
 *
 * First it tests for an XID that's so short it's a null XID
 *	and replies with an XID Format 0 Type 2
 *
 * Then, it tests for IEEE XID (XID_FMT_STD) and replies to that.
 *
 * Then, it doesn't reply if the XID has an Error Control Vector
 * or if the originating station sent an XID Type 2 or lower.
 * It also doesn't respond if the "usesa" flag is NOT set - which
 * is only the case for QLLC media conversion, where the source address
 * is a virtual mac address defined in the router, rather than a real
 * interface MAC address.
 *
 * After building the response packet, it completes the llc header
 * manipuulation and sends it.
 * 
 */
static void tr_respond_xid (
    hwidbtype	*idb,
    paktype	*xid_pak,    
    boolean	usesa,    
    boolean 	secondary)
{
    tring_hdr 	*trh;
    llc1_hdr_t 	*llc1;
    int		llclen;
    int 	llcoffset;
    llc1_xid_t 	*xid;
    riftype 	*rifptr;
    uchar 	sa[IEEEBYTES];
    leveltype 	level;
    paktype	*pak;

    trh = (tring_hdr *) xid_pak->datagramstart;
    llc1 = (llc1_hdr_t *) xid_pak->info_start;
    llcoffset = (int)(xid_pak->info_start) - (int)(xid_pak->datagramstart);
    llclen = xid_pak->datagramsize - llcoffset;


    /*
     * Build and send the response
     */
    ieee_copy_sans_rii(trh->saddr, sa);
    level = raise_interrupt_level(NETS_DISABLE);
    rifptr = rif_lookup(sa, idb, usesa ? trh->daddr : NULL, FALSE);
    if ( (idb->enctype == ET_SDLCP) && (llclen <= SAP_HDRBYTES) ) {
	/*
	 * NULL XID, reply with 0T2 if the llc (network) side is the secondary.
	 * Other wise use a null xid since we are a primary side.
	 */
	pak = tr_setup_xid(sa, trh->daddr, idb,
			   ((rifptr && rifptr->length) ? rifptr->rif : NULL),
			   usesa, trh->daddr[5], 0, 0, secondary);
    } else if (llc1->oui[0] == XID_FMT_STD) {
	/*
	 * STD XID, reply in kind.  
	 */
	if (rif_debug)
	    buginf("\nResponding to IEEE XID");
	pak = tr_setup_xid(sa, trh->daddr, idb,
			   ((rifptr && rifptr->length) ? rifptr->rif : NULL),
			   FALSE, trh->daddr[5], 0, 0, TRUE);
    } else if (llc1->oui[0] == XID_ERROR_CV ) {
	  pak = NULL;
    } else if ((llc1->oui[0] & XID_FMT_MASK) < XID_FMT_8 &&
	       (llc1->oui[0] & XID_TYPE_MASK) > XID_TYPE_2) {
	/*
	 * Only respond to XID commands that aren't from type 1 or type 2
	 * nodes.  We respond with a 0T2 and that would confuse big time a
	 * 2.0 node.
	 */
	if (usesa)
	    pak = tr_setup_xid(sa, idb->hardware, idb,
			       ((rifptr && rifptr->length) ?
				rifptr->rif : NULL),
			       TRUE, trh->daddr[5], 0, 0, TRUE);
	else
	    pak = NULL;
    } else
	pak = NULL;      
    reset_interrupt_level(level);
    if (!pak)
	return;

    xid = (llc1_xid_t *)pak->network_start;
    xid->ssap = llc1->dsap | SAP_RESPONSE;
    xid->dsap = llc1->ssap;
    xid->control = llc1->control;
    datagram_out(pak);

    if (rif_debug || tokenring_debug)
	buginf("\n%s: sent XID response to %e",
	       idb->hw_short_namestring, trh->saddr);
}

/*
 * Process an XID packet and return a response to the sender.
 * These functions are required by the ISO 88022 (IEEE 802.2) LLC
 * specification.  They are normally used in determining the source
 * routing path of a host, and the up/down status of a host.
 *
 * Ignore broadcasts.
 * If there's no handler we can ignore the packet. This eliminates the
 * need for the check_sap variable; all idbs are registered,
 * and when an idb is registered all known SAPs are also registered
 *
 * If it should be forwarded to the higher layer do so by calling
 *	the suitable handler.
 *
 * If it's a Command AND autoresponse is set
 *	autorespond (using tr_respond_sna_xid).
 */

void tr_process_xid (
    hwidbtype	*hwidb,
    paktype	*pak,
    boolean usesa,
    boolean secondary)
{
    tring_hdr 			*trh;
    sap_entry_t			*sap_entry;
    sap_hdr			*llc1;
    boolean			isresponse;
    idbtype                     *idb;

    sap_handle_sna_xid_t	handle_sna_xid;
    sap_handle_ieee_xid_t	handle_ieee_xid;

/* common code for both kinds of XID */

    idb = firstsw_or_null(hwidb);
    trh = (tring_hdr *) pak->datagramstart;
    llc1 = (sap_hdr *) pak->info_start;

    if (trh->daddr[0] & TR_GROUP)
	return;
    
    sap_entry = sap_get_entry(idb, llc1->dsap);
    if (sap_entry == NULL)
	return;

    isresponse = (llc1->ssap & SAP_RESPONSE);

    /* decide whether it's SNA XID or IEEE XID */

    /* length test for NULL XID (bf is last byte)
     * if it's longer than that, see if next byte is
     * IEEE XID (0x81)
     */
    if ( (pak->datagramstart + pak->datagramsize > 
	  ((uchar *)llc1 + SAP_HDRBYTES) ) &&
	llc1->data[0] == XID_FMT_STD) {
	
			/* YUP - IEEE XID */
	if (rif_debug)
	    buginf("\nReceived IEEE XID %s",
		   isresponse ? "Response": "Command");

	if ( !isresponse &&	
	    (sap_entry->sap_flags & SAP_IEEE_XID_AUTORESPONSE))
	   tr_respond_xid(hwidb, pak, usesa, secondary);

	if ( (sap_entry->sap_flags & SAP_IEEE_XID_FORWARD) &&
	    (handle_ieee_xid = sap_entry->sap_handle_ieee_xid) ) {
	    (*handle_ieee_xid)(idb, pak, usesa );
	}
    }
	
    else {		/* HANDLE SNA XID */
	if ( !isresponse &&	
	    (sap_entry->sap_flags & SAP_SNA_XID_AUTORESPONSE))
	   tr_respond_xid(hwidb, pak, usesa, secondary);

	if ((sap_entry->sap_flags & SAP_SNA_XID_FORWARD) &&
	    (handle_sna_xid = sap_entry->sap_handle_sna_xid) ) {
	    (*handle_sna_xid)(idb, pak, sap_entry->usapid, isresponse );
	}
    }
}

/* tr_respond_test
 *
 * handle test packets for SAP when there's no SAP registry
 * build a response, and send it on its way
 */

static void tr_respond_test (hwidbtype *idb,
			     paktype const *testpak,
			     boolean usesa)
{
    tring_hdr *trh;		/* pointer to TR MAC header */
    sap_hdr *llc1;		/* pointer to llc-1 header */
    paktype *pak;
    riftype tmprif;
    uchar sa[IEEEBYTES];   /* src addr holding pen */
    uchar da[IEEEBYTES];   /* dest addr holding pen */
    leveltype level;

    trh = (tring_hdr *)testpak->datagramstart;
    llc1 = (sap_hdr *) testpak->info_start;

    ieee_copy_sans_rii(trh->saddr, sa);
    ieee_copy(trh->daddr, da);

    /* CSCdi21556 - keep the rif_update for its side-effect; 
     * but use rif from test-req for test-rsp, not the rif from cache
     */
    level = raise_interrupt_level(NETS_DISABLE);
    rif_lookup(sa, idb, (usesa ? trh->daddr : NULL), FALSE);
    reset_interrupt_level(level);

  	tmprif.length = trh->data[0] & RC_LTHMASK;
	bcopy( trh->data, tmprif.rif, tmprif.length);
  	tmprif.rif[0] &= ~RC_BROADBIT;
  	tmprif.rif[1] ^= RC_D;

    pak = tr_setup_test(sa, da, llc1, &tmprif,
			testpak->datagramsize-((uchar *) llc1 - (uchar *) trh),
			idb);
    if (!pak)
  	return;
  
    if (tokenring_debug)
	buginf("\n%s: responded to TEST from %e",
	       idb->hw_short_namestring, trh->saddr);
    datagram_out(pak);
}
 
/*
 * tr_process_test
 *
 * handling incoming TEST packets
 *
 * If the interface receives a test Command that means a remote
 * station is trying to contact us, and we have to reply on behalf of
 * SAPs that we handle (and for the null SAP)
 *
 * IBM SAPs (typically SAP_IBM_SNA, but sometimes other ones)
 * should be handled through the SAP registry. We accommodate
 * SDLLC by allowing it even if there isn't a SAP registry.
 *
 * If there's no registered SAP entry, ignore the packet.
 *
 * If it's a TEST Command, and sap is configured to respond automatically
 *	if TEST is all-routes explorer (ARE), or
 *		spanning tree explorer (STE) and we're configured to respond
 *				to STE
 *	respond
 *
 * There's always a handler for SAP=NULL
 *
 * If there's a handler for TEST packets, and the FORWARD bit is set,
 * 	forward to higher layer.
 *
 * As the years progress EVERYTHING should go through the SAP registry
 * but for now we'll just preregister the other SAPs
 */

void tr_process_test (
    hwidbtype *hwidb,
    paktype *testpak,
    boolean usesa)
{
    trrif_t *trh;		/* pointer to TR MAC header */
    sap_hdr *llc1;		/* pointer to llc-1 header */
    sap_entry_t		*sap_entry;
    sap_handle_test_t	handle_test;
    sap_registry_t      *sap_registry;
    idbtype             *idb;
    
    idb = firstsw_or_null(hwidb);
    
    trh = (trrif_t *)testpak->datagramstart;
    llc1 = (sap_hdr *) testpak->info_start;

    /* eliminate the obvious: don't reply to, or process broadcast */
  
      if (trh->daddr[0] & TR_GROUP)
  	return;
  
    /*
     * If there is no sap registry, go ahead and respond to any TEST
     * request that we receive. Probably want to revisit this someday.
     */
    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL) {
	if (((llc1->ssap & SAP_RESPONSE) == 0) &&
	    is_well_known_sap(llc1->dsap)) {
	    tr_respond_test(hwidb, testpak, usesa);
	}
	return;
    }
	
    sap_entry = sap_get_entry(idb, llc1->dsap);
    if (sap_entry == NULL)
  	return;
    
    if ( !(llc1->ssap & SAP_RESPONSE) &&
	(sap_entry->sap_flags & SAP_TEST_AUTORESPONSE)) {
	    tr_respond_test(hwidb, testpak, usesa);
      }
  
    handle_test = sap_entry->sap_handle_test;
    if (handle_test == NULL)
	return;

    if (sap_entry->sap_flags & SAP_TEST_FORWARD)
	(*handle_test)(idb, testpak, usesa);    
}
  

/*
 * tr_interface
 *
 * Print some 16/4 token ring specific information for the show interface
 * command.
 */
static void tr_interface (hwidbtype *idb)
{
    char *buf;
    tokensb  *tsb;
    idbtype  *swidb = firstsw_or_null(idb);
    srb_triplet_t srb_triplet;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);

    printf("\n  Ring speed: %d Mbps", idb->tr_ring_speed);
    if ((idb->tr_ring_speed == 16) &&
	(idb->tr_ring_mode & RINGMODE_ETR))
	printf(", early token release");
    printf("\n  %s node", 
           (srb_multiring_any_enabled(swidb) ? "Multiring" : "Single ring"));
    if (idb->status & IDB_BRIDGE)
	printf(", %s Bridge capable",
	       (subsys_find_by_name("srb") ? "Source Route Transparent" : 
		"Transparent"));
    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
    if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
	printf("\n  Source bridging enabled, srn %d bn %d trn %d %s",
	       srb_triplet.localRing,
               srb_triplet.bridgeNum,
               srb_triplet.remoteRing,
	       (idb->tr_vring_blk ? "(ring group)" : ""));
	printf("\n    proxy explorers %sabled, ",
	       (srbV.srb_sb_get_proxy_explorer(swidb)) ? "en" : "dis");
	printf("spanning explorer %sabled, ",
	       (srbV.srb_sb_get_spanning_explorer(swidb) ? "en" : "dis"));
	printf("NetBIOS cache %sabled",
            (netbios_sb_name_caching_is_enabled(swidb) ? "en" : "dis"));
	automore_conditional(0);
    }
    if ((tsb->tr_group & ~0x80000000) || tsb->tr_functional)
	printf("\n  Group Address: 0x%08x, Functional Address: 0x%08x",
	       tsb->tr_group  & ~0x80000000, tsb->tr_functional);
    if (idb->tr_transit_oui) {
	printf("\n  Ethernet Transit OUI: 0x%06x",
	       (idb->tr_transit_oui & 0xffffff));
    }
    if (!tsb->tr_ring_status)
	return;
    buf = malloc(TR_RSTAT_BUF_SIZE);
    if (!buf)
        return;
    decode_ring_status(buf, tsb->tr_ring_status);
    printf("\n  Last Ring Status ");
    print_dhms(tsb->tr_status_time);
    printf(" %s (%#x)", buf, tsb->tr_ring_status);
    free(buf);
}

/*
 * new_tring_speed
 *
 * Set a new token ring speed (4 or 16 Mbps).  Updates IDB fields for
 * bandwidth, delay, and MTU.
 */
void new_tring_speed (hwidbtype* idb, int speed, boolean initial)
{
    idbtype *swidb;
    idb->tr_ring_speed = speed;
    
    /*
     * Set up packet size
     */
    if (speed == 4) {
	idb->max_pak_size = idb->max_pak_size_def = TR_MAXPAK;
    } else if (speed == 16) {
	idb->max_pak_size = idb->max_pak_size_def = TR16_MAXPAK;
    } else {
	/*
	 * default to 16
	 */
	idb->max_pak_size = idb->max_pak_size_def = TR16_MAXPAK;
    }

    /*
     * TMS380 - don't associate MTU and ring speed
     * CTR - always use 4k mtu (doesn't support 8k)
     * OTHER - MTU based on ring speed 
     */
    switch (idb->type) {
    case IDBTYPE_TMS380:
        break;
    case IDBTYPE_CTR:
	new_physical_mtu(idb, TR_MTU, initial); 
	break;
    default:
	if (speed == 4) 
	    new_physical_mtu(idb, TR_MTU, initial); 
	else
	    new_physical_mtu(idb, TR16_MTU, initial); 
    }

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        set_default_interface_delay(swidb, (speed == 4) ? TR_DELAY
				    : TR16_DELAY);
	set_default_interface_bandwidth(swidb, (speed == 4) ?
					VISIBLE_TR_BANDWIDTH : VISIBLE_TR16_BANDWIDTH);
    }

    if ( !initial ) {
	if (idb->state != IDBS_ADMINDOWN) {
	    (*idb->reset)(idb); /* Make new speed active */
	    if (idb->counters.resets > 0)
		idb->counters.resets--;
	    idb->counters.transitions += 2;
	}
    }
}

/*
 * tring_linestate
 * Return current line protocol status
 */

static boolean tring_linestate (hwidbtype *hwidb)
{
    tokensb *tsb;

    tsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    if (hwidb->status & IDB_CBUS)
	return(hwidb->mci_lineup); /* CTR uses mci_lineup, not tr_lineup */
    else
	return(tsb->tr_lineup);
}


/*
 * idbupBLOCK
 * Wait for an interface to come up or for a timer to expire.
 */

boolean tr_test_idbupBLOCK (hwidbtype *idb)
{
    tokensb *tsb;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    if (idb->state == IDBS_UP)
	return(FALSE);
    if (AWAKE(tsb->tr_test_timer))
	return(FALSE);
    return(TRUE);
}


/*
 * Set up for token ring when testing its hardware.
 */
static char tr_test_interface_setup (hwidbtype *idb)
{
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    TIMER_START(tsb->tr_test_timer, ONEMIN);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    edisms((blockproc *)tr_test_idbupBLOCK, (long)idb);
    transition_adjust(idb); /* see if anything really happened */
    route_adjust(idb);
    backup_adjust(idb); /* why bother? who's gonna backup TRs? */
    if (idb->state != IDBS_UP) {
	printf(" Failed - initialization problem");
	return(-1);
    }
    return(TRUE);
}

/*
 * tr_extract_addr
 *
 * Given an Token Ring packet, extract the requested MAC level address
 * into a hwaddrtype structure.  Assumes pak->mac_start is set.
 */
boolean tr_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    tring_hdr *addrs;

    addrs = (tring_hdr *)pak->mac_start;
    address->type = STATION_IEEE48;
    address->length = STATIONLEN_IEEE48;
    address->target_type = TARGET_UNICAST;
    switch (which) {
      case SOURCE:
	ieee_copy_sans_rii(addrs->saddr, address->addr);
	break;
      case DESTINATION:
	ieee_copy(addrs->daddr, address->addr);
	break;
    }

    if (is_ieee_bcast(address->addr))
	address->target_type = TARGET_BROADCAST;
    else if (ieee_equal(esis_8025_all_is_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_IS;
    else if (ieee_equal(esis_8025_all_es_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_ES;

    return(TRUE);
}

/*
 * tring_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are correctly aligned in the buffer.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * WARNING: It it up to the calling routine to ensure that the
 * (potential) snap header is in a location that it *byte* accessible.
 * For MCI/Cbus devices, this means reading this information off of the
 * controller and into local memory.  The same rules apply for all other
 * drivers.
 */

iqueue_t tring_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    int info_size, riflen, enctype;
    sap_hdr *sap;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	sap = (sap_hdr *)data;
	riflen = idb->rxrifwordlen * 2;
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	
	pak->riflen = riflen = get_tr_riflen_pak(pak);
	pak->rif_start = pak->datagramstart + TRING_ENCAPBYTES;
	sap = (sap_hdr *)(pak->rif_start + riflen);
    }

    token_get_enctype_infosize(sap, &enctype, &info_size, pak->if_input);
    pak->enctype = enctype;

    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) -
	    TRING_8025_ENCAPBYTES - riflen - info_size;
    }
    token_set_data_pointers(pak, riflen, info_size);
    return(raw_enqueue);
}


/*
 * tring_idb_enqueue()
 * Add an idb to the tring idb list
 */
void tring_idb_enqueue (hwidbtype *hwidb)
{
    ulong idblist_type;

    if (!(hwidb->status & IDB_TR)) return;

    /*
     * Check for token ring hwidb list if not there, create it.
     */
    if (idb_get_listhead(HWIDBLIST_TOKEN) == NULL) {
	if (!idb_create_list(HWIDBLIST_TOKEN, &idblist_type) ||
	    idblist_type != HWIDBLIST_TOKEN) {
	    if (tokenring_debug) {
		idb_report_create_error("token ring");
	    }
	    return;
	}
    }

    /*
     * Add the idb to the token ring idb list
     */
    if (!idb_add_hwidb_to_list(hwidb, HWIDBLIST_TOKEN)) {
	if (tokenring_debug) {
	    idb_report_add_error("token ring");
	}
    }
}


/*
 * tring_idb_dequeue()
 * Dequeue an idb from the tring idb list
 */
void tring_idb_dequeue (hwidbtype *hwidb)
{
    if (!idb_remove_from_list(hwidb, HWIDBLIST_TOKEN)) {
	if (tokenring_debug) {
	    idb_report_removal_error("token ring");
	}
    }
}


/*
 * tring_idb_init
 *
 * Do Token Ring specific interface initializations.
 * Protocol specific routines (such as adding ARP entries) are done elsewhere.
 *
 * This routine sets up some fields in the first (master) soft IDB;  later
 * subinterfaces will clone this information.
 */
void tring_idb_init (hwidbtype *idb, int speed)
{
    idbtype *swidb = idb->firstsw;
    tokensb *tsb;
    hwidb_sb_t type = HWIDB_SB_TOKEN;

    /*
     * Create the subblock, if needed
     */
    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    if (tsb == NULL) {
	tsb = malloc(sizeof(tokensb));
	if (tsb == NULL) {
	    /*
	     * This is way bad, we have no memory, crash and burn
	     */
	    crashdump(0);
	}
	
	if (!idb_add_hwsb(idb, &type, tsb))
	    return;
    }

    idb->name = IFNAME_TOKENRING;
    idb->load = 1;
    new_tring_speed(idb, speed, TRUE);

    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_TOKEN;

    idb->loopback = FALSE;
    idb->tr_transit_oui = DEFAULT_OUI;
    idb->enctype = idb->enctype_def = ET_SNAP;
    idb->enctype_cfg = swidb->ip_enctype = ET_SNAP;
    reg_invoke_media_enctype_setup(swidb, ET_SNAP);
    idb->nokeepalive = FALSE;
    idb->reliability = 255;
    tsb->tr_lineup = 0;
    GET_TIMESTAMP(tsb->tr_lineuptimer);
    idb->tr_3com_hack = FALSE;

    /*
     * If the state config isn't set up yet, do so now
     */
    if (idb->state_config != tring_hwidb_state_config) {
	tsb->super_state_config = idb->state_config;
	idb->state_config = tring_hwidb_state_config;
    }

    /*
     * set up how arping is done.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = TRUE;
    swidb->arp_timeout = ARP_MAXAGE;
    swidb->arp_gra_time = TR_ARP_GRATUITOUS_HOLD;

    /*
     * special ring services running on this interface.
     */
    idb->tr_ring_mode = 0;
    tsb->tr_functional = 0;
    tsb->tr_group = 0x80000000;

    /*
     * Set up all common function assignments
     */
    idb->vencap = tr_vencap;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->deencap = NULL;
    set_default_queueing(idb);
    idb->iqueue = raw_enqueue;
    idb->getlink = tring_getlink;
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->extract_snpa = tr_extract_addr;
    idb->broadcast = tr_bcast;
    idb->periodic = tr_periodic;
    idb->listen = (listen_t)return_nothing;
    idb->show_support = tr_interface;
    idb->tr_srb_fastswitch =(tr_srb_fastswitch_type) return_false;
    idb->max_proto_mtu = tring_max_proto_mtu;
    idb->lineaction = tring_cstate;
    idb->linestate = tring_linestate;
    idb->parse_packet = tring_parse_packet;

    holdq_init(idb, TR_HOLDQ);
}

/*
 *
 */
void set_new_ringmode (hwidbtype *idb, int functional)
{
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    tsb->tr_functional &= ~(TRFUNC_REM | TRFUNC_RPS | TRFUNC_CRS);
    idb->tr_ring_mode  &= ~(RINGMODE_REM | RINGMODE_RPS | RINGMODE_NETMGR);

    if (functional & TRFUNC_REM) {
	tsb->tr_functional |= TRFUNC_REM;
	idb->tr_ring_mode  |= RINGMODE_REM;
    }
    if (functional & TRFUNC_RPS) {
	tsb->tr_functional |= TRFUNC_RPS;
	idb->tr_ring_mode  |= RINGMODE_RPS;
    }
    if (functional & TRFUNC_CRS) {
	tsb->tr_functional |= TRFUNC_CRS;
	idb->tr_ring_mode  |= RINGMODE_NETMGR;
    }
    
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/*
 * using_multiring
 *
 * Is multiring turned on for a particular protocol.  The multiring bits
 * array is private to this file, so this is an access function.
 */
boolean using_multiring (idbtype *idb, int linktype)
{
    switch (linktype) {
      case LINK_ARP:
      case LINK_RARP:
      case LINK_IP:
	return(srb_multiring_ip_enabled(idb));
      case LINK_APPLETALK:
      case LINK_AARP:
	return(srb_multiring_apple_enabled(idb));
      case LINK_XNS:
	return(srb_multiring_xns_enabled(idb));
      case LINK_NOVELL:
	return(srb_multiring_novell_enabled(idb));
      case LINK_CLNS:
      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
      case LINK_CLNS_BCAST:
	return(srb_multiring_clns_enabled(idb));
      case LINK_DECNET:
      case LINK_DECNET_ROUTER_L1:
      case LINK_DECNET_ROUTER_L2:
      case LINK_DECNET_PRIME_ROUTER:
      case LINK_DECNET_NODE:
	return(srb_multiring_decnet_enabled(idb));
      case LINK_VINES:
	return(srb_multiring_vines_enabled(idb));
    }
    return(srb_multiring_other_enabled(idb));
}

/*
 * multicast_explorer_can_be_bridged
 *
 * This is used to determine whether it is OK to send a multicast
 * explorer directly through the srb path without first presenting
 * it to the protocol stack.
 *
 * This approach addresses the following problems:
 *    1) router geting blasted with packets when bridging multicasts.
 *    2) packets with invalid snap-type overflowing network-input-queue.
 */
boolean multicast_explorer_can_be_bridged (
    idbtype *swidb,
    ushort rxtype,
    sap_hdr *llc1)
{
  switch (rxtype) {
    case RXTYPE_SAP_NOVELL:
    case RXTYPE_NOVELL1:
    case RXTYPE_NOVELL2:
    case RXTYPE_SNAP_NOV1:
         return BRIDGING_RXTYPE(swidb, rxtype);

    case RXTYPE_SAP_UNKN:
        /* test and xid frames to multicast address */
        if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
            ((llc1->control & ~LLC1_P) == LLC1_TEST))
            return TRUE;

        return FALSE;

    case RXTYPE_UNKNOWN:
         /* Unknown rxtype is not necessarily an illegal linktype. */
         if(type2link(((snap_hdr *)llc1)->type) == LINK_ILLEGAL)
             return TRUE;
         break;

    default:
         break;
    }
    return FALSE;
}



/*
 * tr_stay_shutdown
 * Returns TRUE if the TR interface should remain in a shutdown state.
 * This should be only if the ring speed has not been set.
 * Called by SERVICE_STAY_SHUTDOWN
 */

static boolean tr_stay_shutdown (idbtype *swidb)
{
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    if (hwidb && (hwidb->status & IDB_TR) && !hwidb->tr_ring_speed) {
	printf("\n%% %s: no ring speed has been set",  hwidb->hw_namestring);
	return(TRUE);
    }
    return(FALSE);
}

int getRifLfTrh(trrif_t *trh)
{
    return((trh->saddr[0] & TR_RII) ? trh->rc_dlf & RC_LFMASK : 0);
}

boolean setRifLfTrh(trrif_t *trh, int lfValue)
{
    if (!(trh->saddr[0] & TR_RII))
	return(FALSE);
    
    trh->rc_dlf = (trh->rc_dlf & ~RC_LFMASK) |
	(lfValue & RC_LFMASK);
    
    return(TRUE);
}


/*
 * tring_update_counters()
 * Update the token ring counters in the subblock
 */
void tring_update_counters (tokensb *tsb)
{
    tsb->prev_signal_loss += tsb->signal_loss;
    tsb->prev_soft_errs += tsb->soft_errs;
    tsb->prev_hard_errs += tsb->hard_errs;
    tsb->prev_tx_beacon += tsb->tx_beacon;
    tsb->prev_wire_fault += tsb->wire_fault;
    tsb->prev_rmt_removal += tsb->rmt_removal;
    tsb->prev_single_station += tsb->single_station;
    tsb->prev_ring_recovery += tsb->ring_recovery;

    tsb->soft_errs = tsb->signal_loss = tsb->hard_errs =
	tsb->tx_beacon = tsb->wire_fault = tsb->rmt_removal =
	tsb->single_station = tsb->ring_recovery = 0;
}


/*
 * print_ring_status()
 * Display the ring status fields for this interface
 */
void print_ring_status (hwidbtype *hwidb)
{
    tokensb  *tsb;
    idbtype  *swidb = firstsw_or_null(hwidb);
    srb_triplet_t srb_triplet;

    tsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    printf("\n\n  Last Ring Status: ");

    if (tsb->tr_ring_status) {
	decode_ring_status(ring_status_text, tsb->tr_ring_status);
	print_dhms(tsb->tr_status_time);
	printf(" %s (%#x)", ring_status_text, tsb->tr_ring_status);
    } else printf("none");

    printf("\n    Stats: soft: %d/%d, hard: %d/%d, sig loss: %d/%d",
	   tsb->soft_errs, tsb->prev_soft_errs,
	   tsb->hard_errs, tsb->prev_hard_errs,
	   tsb->signal_loss, tsb->prev_signal_loss);

    printf("\n           tx beacon: %d/%d, wire fault %d/%d, recovery: %d/%d",
	   tsb->tx_beacon, tsb->prev_tx_beacon,
	   tsb->wire_fault, tsb->prev_wire_fault,
	   tsb->ring_recovery, tsb->prev_ring_recovery);

    printf("\n           only station: %d/%d, remote removal: %d/%d",
	   tsb->single_station, tsb->prev_single_station,
	   tsb->rmt_removal, tsb->prev_rmt_removal);

    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
    if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
	printf("\n  Bridge: local %d, bnum %d, target %d",
	       srb_triplet.localRing,
               srb_triplet.bridgeNum,
	       srb_triplet.remoteRing);
	printf("\n    max_hops %d, target idb: 0x%x, %s",
	       srbV.srb_sb_get_max_hops(swidb), hwidb->tr_bridge_idb,
	       (hwidb->tr_bridge_idb ? hwidb->tr_bridge_idb->hw_namestring
		: "not local"));
    }

    printf("\n  Interface failures: %d ", tsb->tr_fail_count);
    if (tsb->tr_fail_count) {
	printf("last %#x %#x, ", tsb->tr_last_fail, tsb->tr_last_fail_code);
	print_dhms(tsb->tr_last_fail_time);
    }
}


/*
 * tring_init
 *
 * Initialize global Token Ring related information.
 */
static void tring_init (subsystype* subsys)
{
    rif_init();

    /*
     * Set up debugging flags
     */
    tring_debug_init();

    /*
     * Register some functions
     */
    reg_add_stay_shutdown(tr_stay_shutdown, "tr_stay_shutdown");
    reg_add_loop_enqueue(IDB_TR, tr_rcvloop, "tr_rcvloop");
    reg_add_media_token_macaddr(LINK_LOOP, loop_macaddr_token,
				"loop_macaddr_token");
    reg_add_media_token_macaddr(LINK_MAC, mac_macaddr_token,
				"mac_macaddr_token");
    reg_add_media_hwidb_enqueue_private_list(tring_idb_enqueue,
					     "tring_idb_enqueue");
    reg_add_media_token_macwrite(ET_MAC, token_et_mac, "token_et_mac");
    reg_add_test_interfaces_setup(IDB_TR, tr_test_interface_setup,
		       "tr_test_interface_setup");
    reg_add_media_rif_lookup(rif_lookup, "rif_lookup");
    reg_add_setup_int(tring_setup_int, "tring_setup_int");
 
    /*
     * Initialize token ring parser support
     */
    tring_parser_init();
}


/*
 * Token ring subsystem header
 */

#define TRING_MAJVERSION 1
#define TRING_MINVERSION 0
#define TRING_EDITVERSION 1

SUBSYS_HEADER(tring, TRING_MAJVERSION, TRING_MINVERSION, TRING_EDITVERSION,
	      tring_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);
      
