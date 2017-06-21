/* $Id: lane_serve_state.c,v 3.6.52.8 1996/08/28 12:54:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_serve_state.c,v $
 *------------------------------------------------------------------
 * lane_serve_state.c -- LAN Emulation Server state machine
 *
 * December 1994, Norm Finn
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_serve_state.c,v $
 * Revision 3.6.52.8  1996/08/28  12:54:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.52.7  1996/08/12  20:05:18  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.6.52.6  1996/07/30  14:42:44  michellw
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
 * Revision 3.6.52.5  1996/06/19  18:39:07  michellw
 * CSCdi60507:  cant re-configure LES/BUS with different elan name
 * Branch: California_branch
 *
 * Revision 3.6.52.4  1996/05/31  17:52:15  michellw
 * CSCdi46369:  LEC remembers ELAN name even if it was never specified
 * Branch: California_branch
 *
 * Revision 3.6.52.3  1996/05/09  14:32:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.52.2.2.1  1996/04/27  07:06:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.52.2  1996/04/25  23:14:26  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.52.1.8.5  1996/04/16  02:44:16  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.52.1.8.4  1996/04/11  05:33:18  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.6.52.1.8.3  1996/04/09  06:58:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * lsv_init_idb was not initializing lane_info->type before referencing it.
 *
 * Revision 3.6.52.1.8.2  1996/04/08  14:51:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.52.1.8.1  1996/04/08  01:56:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.52.1  1996/03/18  20:32:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.56.2  1996/04/05  03:20:21  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.56.1  1996/03/22  22:54:19  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.42.1  1996/03/05  06:42:53  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.32.2  1996/03/01  01:46:25  cakyol
 * convert "crashdump"s to "lane_router_crash" which can print
 * a last gasp message and call "logger_flush" before dying.
 * Helps debugging a lot.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.32.1  1996/02/27  21:10:14  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.12.5  1996/02/22  09:05:27  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.6.12.4  1996/02/15  00:47:45  cakyol
 * correct misleading debugging messages giving the impression
 * that they are clearing a vc, whereas what they are in fact
 * doing is acknowledging a release received from the network.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.6.12.3  1996/02/06  22:19:15  cakyol
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
 * Revision 3.6.12.2  1996/01/26  02:02:31  cakyol
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
 *
 * $Log: lane_serve_state.c,v $
 * Revision 3.6.52.8  1996/08/28  12:54:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.52.7  1996/08/12  20:05:18  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.6.52.6  1996/07/30  14:42:44  michellw
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
 * Revision 3.6.52.5  1996/06/19  18:39:07  michellw
 * CSCdi60507:  cant re-configure LES/BUS with different elan name
 * Branch: California_branch
 *
 * Revision 3.6.52.4  1996/05/31  17:52:15  michellw
 * CSCdi46369:  LEC remembers ELAN name even if it was never specified
 * Branch: California_branch
 *
 * Revision 3.6.52.3  1996/05/09  14:32:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.52.2.2.1  1996/04/27  07:06:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.52.1.8.5  1996/04/16  02:44:16  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.52.1.8.4  1996/04/11  05:33:18  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.6.52.1.8.3  1996/04/09  06:58:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * lsv_init_idb was not initializing lane_info->type before referencing it.
 *
 * Revision 3.6.52.1.8.2  1996/04/08  14:51:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.56.1.2.1  1996/04/08  08:05:50  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.6.32.2.2.1  1996/04/06  21:19:22  ddecapit
 * Sync to LE_Cal_V111_1_0_3_branch
 * Branch: TRlane_branch_1_0_3
 *
 * Revision 3.6.12.1.2.3  1996/03/23  21:12:20  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.6.12.1.2.2  1996/03/05  04:46:44  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.6.12.1.2.1  1996/02/26  23:06:34  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.6.12.1  1996/01/11  01:46:16  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.6.8.1  1995/12/29  01:06:36  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.6  1995/11/24  19:39:40  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state.
 * Use subif_line_statechange() to control the operational state of
 * the sub-interface. In LECS/LES use interface_admin_down instead of
 * interface up to determine the adminstrative nature of the sub-interface.
 *
 * Revision 3.5  1995/11/22  09:19:29  shj
 * CSCdi44330:  LANE Server stays down after <no shut> on interface
 * The last gasp of CSCdi42765, we hope.  Replace !interface_up() with
 * interface_admin_down().
 *
 * Revision 3.4  1995/11/20  01:43:49  shj
 * CSCdi42765:  lane server remains down after interface shut/no shut
 * State change isn't always complete when statechange_complete is
 * invoked.  Use interface_admin_down() to tell what's really going
 * on instead of interface_up_simple().
 *
 * Revision 3.3  1995/11/17  17:50:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:50:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.17  1995/09/02  01:27:22  shj
 * CSCdi37789:  LANE Server dead for all interfaces when cfgd on down
 * interface
 * Skip down interfaces when looking for servers to start during
 * initialization.
 *
 * Revision 2.16  1995/08/08  01:29:14  cakyol
 * CSCdi38163:  LANE ATM address overrides are ignored
 * - fixed.
 *   The problem was: les called the lsv_kill_lesbus ONLY
 *   if it was NOT in states INITIALIZED and TERMINATING.
 *   Obviously, in this case, when the subif was shut down,
 *   it WAS in the INITIALIZED state and lsv_kill_lesbus
 *   SHOULD have been called.
 *
 * Revision 2.15  1995/07/24  04:55:51  nfinn
 * CSCdi37072:  LECS access code uses only the first LECS address
 * found via ILMI.  Also, sevel "show lane" and LANE error messages
 * need cleaning up.  A small memory leak occurred whenever LANE was
 * configured and the interface was destroyed.
 *
 * Revision 2.14  1995/07/13  21:39:34  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.13  1995/07/12  01:47:49  nfinn
 * CSCdi36959:  LECS access attempts to connect to ATM address 0
 *
 * Revision 2.12  1995/07/08  01:46:10  nfinn
 * CSCdi36260:  Router hung after assigning dup ATM addr to LES/BUS
 * State machine was fouled up worse than we thought.
 *
 * Revision 2.11  1995/07/07  18:53:37  cakyol
 * CSCdi36260:  Router hung after assigning dup ATM addr to LES/BUS
 * - lsv_stop_listening should return TRUE for an address which
 *   failed to register in the first place.  This was returning FALSE,
 *   which caused the eventual crash, becoz lsv_kill_lesbus exited
 *   prematurely
 *
 * Revision 2.10  1995/07/07  05:42:31  bbenson
 * CSCdi35012:  Server and Client should errmsg when operState goes down.
 * - Consolidated message to conform to the link and lineproto "UPDOWN"s.
 *   As Norm would say, slicker than...
 *
 * Revision 2.9  1995/06/30  00:46:11  nfinn
 * CSCdi35012:  Server and Client should errmsg when operState goes down.
 *
 * Revision 2.8  1995/06/30  00:14:41  nfinn
 * CSCdi36603:  LES/BUS should issue listens before registering addresses
 *
 * Revision 2.7  1995/06/28  22:42:51  cakyol
 * CSCdi35007:  Server should toggle state upon change on elan name.
 * - done.
 *
 * Revision 2.6  1995/06/27  02:11:12  nfinn
 * CSCdi35046:  Server should wait for ILMI prefix before registering any
 * address.
 *
 * Revision 2.5  1995/06/26  23:21:53  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 * Revision 2.3  1995/06/19  06:45:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:10:50  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:19:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * LES/BUS state machine
 ************************************************************************/
/*
 * One state machine per LES/BUS:
 *
 * Events:
 *	set UP		lsv_info_t.admin_state = LSV_LESBUS_ADMIN_UP
 *	set DOWN	lsv_info_t.admin_state = any other LSV_LESBUS_ADMIN_xxx
 *	proc ready	lsv_input process gets past initialization block
 *	ATMSAP avail	ATMSAP hardcoded or ilmi reports prefix available
 *	ATMSAP reg	ilmi reports ATMSAP is registered
 *	setup received	lsv_sigback_listen_direct got LANE_CALL_SETUP_RECEIVED
 *	term event	Any of: listen cancel complete, last client killed,
 *				LES/BUS connection downs completed, LES/BUS
 *				unregistrations complete, last multicast group
 *				deleted.
 *
 * States/transitions:
 *
 *  LSV_LESBUS_INITIALIZED
 *	proc ready:
 *		Ask for LES and BUS ATMSAPs
 *		-> LSV_LESBUS_WAITING_ILMI
 *	set DOWN:
 *		-> LSV_LESBUS_INITIALIZED
 *	other:
 *		ignore or crashdump
 *  LSV_LESBUS_WAITING_ILMI
 *	ATMSAP avail:
 *		register LES and BUS ATMSAPs
 *		issue listens for LES and BUS
 *		create multicast groups
 *		-> LSV_LESBUS_PROVISIONAL
 *	set DOWN:
 *		-> LSV_LESBUS_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_LESBUS_PROVISIONAL
 *	ATMSAP reg or SVC setup received:
 *		-> LSV_LESBUS_OPERATIONAL
 *	set DOWN:
 *		-> LSV_LESBUS_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_LESBUS_OPERATIONAL
 *	set DOWN:
 *		-> LSV_LESBUS_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_LESBUS_TERMINATING
 *	set DOWN or term event:
 *		if last term event,
 *			-> LSV_LESBUS_INITIALIZED
 *		else
 *			initiate next event in sequence, if needed:
 *				cancel listens for LES and BUS
 *				unregister LES and BUS ATMSAPs
 *				issue kill_client on all clients
 *				start all connections going down
 *				delete multicast groups
 *			-> LSV_LESBUS_TERMINATING
 *	other:
 *		ignore or crashdump
 */


/*******************************************************************
 * LES/BUS
 *******************************************************************/

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "address.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lsv_private.h"
#include "lec_private.h"
#include "lane_ilmi.h"
#include "../ui/debug.h"
#include "lane_debug.h"

/*
 * G l o b a l  S t o r a g e 
 */
lsv_info_t	*lsv_all_lesbus = NULL;	/* chain of all LESs */
mgd_timer	 lsv_root_timer;	/* root of managed timers */
static MaskedAddress lsv_default_masked_addr = {
    type:STATION_ATMNSAP, length:STATIONLEN_ATMNSAP,
    dataBytes:{0}, maskBytes:{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};


/*******************************************************************
 * LES/BUS miscellaneous utilities
 *******************************************************************/

/*
** callback routine for the finder to call us, when it 
** manages to find the master LECS for us.
*/
static void lsv_handleMasterLecsAddress (idbtype *swidb,
    uint transactionId, void *userContext,
    int count, hwaddrtype *lecs_addresses);

/*
** start the ball rolling to connect to the master LECS
*/
void lsv_start_lecs_connection_procedures (lsv_info_t *lsv_info)
{
    idbtype *swidb;

    if (! lsv_info) return;
    swidb = lsv_info->lane_info->swidb;

    /* since we are here, we can stop the timer */
    mgd_timer_stop(&lsv_info->lecsAccess.lecsReconnectTimer);

    /* this starts the state machine */
    lecsFinderRequest(swidb, GET_MASTER_LECS_ADDRESS, lsv_info,
	lsv_handleMasterLecsAddress, 1);
}

/*
 * lsv_server_new_state - Make LES/BUS state change and report it
 */
void
lsv_server_new_state (lsv_info_t		*lsv_info,
		      lsv_server_state_t	 state)
{
    if (lsv_info->server_state != state) {
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s LES/BUS state change %s -> %s",
	    lsv_info->lane_info->name,
	    lsv_pr_server_state(lsv_info->server_state),
	    lsv_pr_server_state(state));

	/* coming up */
	if (state == LSV_LESBUS_OPERATIONAL) {
	    errmsg(&msgsym(UPDOWN, LANE),
		   lsv_info->lane_info->swidb->namestring,
		   "elan",
		   lsv_info->lane_info->name,
		   "LE Server/BUS", "up");
	    lsv_start_lecs_connection_procedures(lsv_info);
	/* going down */
	} else if ((lsv_info->server_state == LSV_LESBUS_OPERATIONAL) ||
		   (lsv_info->server_state == LSV_LESBUS_PROVISIONAL)) {
	    errmsg(&msgsym(UPDOWN, LANE),
		   lsv_info->lane_info->swidb->namestring,
		   "elan",
		   lsv_info->lane_info->name,
		   "LE Server/BUS", "down");
	}
    }
    lsv_info->server_state = state;
}


/*
 * lsv_big_error - Recycle a LES/BUS, slowly
 */
void
lsv_big_error (lsv_info_t *lsv_info, int seconds)
{
    /*
     * Do nothing if big-error timer already running
     */
    if (mgd_timer_running(&lsv_info->recycle_timer)) {
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s repeat big error ignored",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name);
	lsv_kill_lesbus(lsv_info);
	return;
    }
    if (lane_lsv_debug)
	buginf("\nLES %s elan %s recycling, will retry in %d seconds",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name, seconds);
    mgd_timer_start_jittered(&lsv_info->recycle_timer, seconds*ONESEC, 10);
    lsv_info->dont_recurse = FALSE;
    lsv_kill_lesbus(lsv_info);
}


/*
 * lsv_crash - Print error message and take down LES/BUS
 */
void
lsv_crash (lsv_info_t	*lsv_info,
	   char		*msg)
{
    errmsg(&msgsym(LSV_CRASH, LANE), lsv_info->lane_info->swidb->namestring,
	   lsv_info->lane_info->name, msg);
    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
}


/*
 * lsv_kill_mcgroup - Kill a multicast group
 *	returns TRUE if gone, FALSE if kill in progress
 */
static boolean
lsv_kill_mcgroup (lane_signal_mcgroup_t *mcgroup)
{
    if (mcgroup->mcgroup_state == LANE_VCC_INITIALIZED)
	return(TRUE);
    return (LANE_SIG_SUCCESS == lane_delete_mcgroup(mcgroup));
}


/*
 * lsv_stop_listening - Stop listening on an ATMSAP
 *	returns TRUE if no longer listening, FALSE if stop in progress
 */
static boolean
lsv_stop_listening (lane_vcc_info_t *vcc_info)
{
    if (vcc_info->vcc_state == LANE_VCC_INITIALIZED)
	return(TRUE);
    if (vcc_info->vcc_state == LANE_VCC_LISTENING)
	return(LANE_SIG_CALLBACK != lane_no_listen(vcc_info));
    return(FALSE);
}


/************************************************************************
 * LES/BUS state machine
 ************************************************************************/


/*
 * lsv_process_recycle - Serve the LES/BUS recycle timer
 */
void
lsv_process_recycle (lsv_info_t	*lsv_info)
{
    mgd_timer_stop(&lsv_info->recycle_timer);
    /*
     * try again
     */
    if (lane_lsv_debug)
	buginf("\nLES %s elan %s restarting after recycle delay",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name);
    lsv_info->dont_recurse = FALSE;
    lsv_start_lesbus(lsv_info);
}


/*
 * lsv_start_listen - Initiate a listen, if needed, on the LES or the BUS
 *		ATM address.
 */
static lane_signal_return_t
lsv_start_listen (lsv_info_t		*lsv_info,
		  lane_vcc_info_t	*vcc_info)
{
    switch(vcc_info->vcc_state) {
    case LANE_VCC_INITIALIZED:
	switch(lane_listen(vcc_info)) {
	case LANE_SIG_SUCCESS:
	    return(LANE_SIG_SUCCESS);
	case LANE_SIG_FAILURE:
	    return(LANE_SIG_FAILURE);
	case LANE_SIG_CALLBACK:
	    return(LANE_SIG_CALLBACK);
	}
	lsv_crash(lsv_info, "lsv_start_listen listen");
	return(LANE_SIG_FAILURE);
    case LANE_VCC_LISTEN_SENT:
	return(LANE_SIG_CALLBACK);
    case LANE_VCC_CANCEL_SENT:
	return(LANE_SIG_CALLBACK);
    case LANE_VCC_LISTENING:
	return(LANE_SIG_SUCCESS);
    default:
	lsv_crash(lsv_info, "lsv_start_listen state");
	return(LANE_SIG_FAILURE);
    }
}


/*
 * lsv_release_lesbus_structures - release all belonging to a LES/BUS
 */
static void
lsv_release_lesbus_structures (lsv_info_t  *lsv_info)
{
    lane_info_t	*lane_info;
    lsv_info_t	*last_lsv;
    lsv_info_t	*this_lsv;

    /* clean up EVERYTHING in LECS access module */
    cleanupLecsAccess(lsv_info, TRUE, TRUE, TRUE);

    /* Remove from chain of all lsv_info_t structures
     */
    lane_info = lsv_info->lane_info;
    last_lsv  = NULL;
    this_lsv  = lsv_all_lesbus;
    while (this_lsv) {
	if (this_lsv == lsv_info)
	    break;
	last_lsv = this_lsv;
	this_lsv = this_lsv->next;
    }
    if (this_lsv == NULL)
	lane_router_crash("lsv_release_lesbus_structures");
    if (last_lsv == NULL)
	lsv_all_lesbus = lsv_info->next;
    else
	last_lsv->next = lsv_info->next;

    /*
     * Kill managed timers and free memory used by structure.  Kill
     *	    LANE info structure if appropriate.
     */
    mgd_timer_stop(&lsv_info->timer_node);
    lane_info->lsv_info = NULL;
    if (lane_lsv_debug)
	buginf("\nLES %s elan %s destroyed",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name);
    Index_destroy(lsv_info->clientList);
    free(lsv_info);
    lane_info_destroy(lane_info);
}

/*
** see if another server-bus serving the same elan is already
** running on an subinterface of this interface ?
*/
static idbtype *
duplicate_serverbus (idbtype *swidb, uchar *elan_name)
{
    idbtype *idb;
    lane_info_t *lane_info;
    lsv_info_t *lsv_info;

    FOR_ALL_SWIDBS_ON_HW(swidb->hwptr, idb) {
	if (idb != swidb) {
	    lane_info = idb->lane_info;
	    if (lane_info && lane_info->name_from_les) {
		lsv_info = lane_info->lsv_info;
		if (lsv_info) {
		    if (strncmp(elan_name, lane_info->name, 
			MAX_ELAN_NAME_LENGTH) == 0)
			    return idb;
		}
	    }
	}
    }
    return NULL;
}

/*
 * lsv_create_lesbus - Create a LES/BUS.
 *	Returns lsv_info_t * if successful, else NULL.
 *
 * lan_type is either LANE_TYPE_802_3 or LANE_TYPE_802_5
 *
 * Feel free to call this routine at any time.  If the LES/BUS is already
 *    up, it will make sure new creation is a no-op, or will return FALSE.
 *    If LES/BUS is not up, it will create or re-create it.
 */
lsv_info_t *
lsv_create_lesbus (idbtype		*swidb,
		   int			lan_type,
		   int			name_size,
		   uchar		*elan_name,
		   lane_caller_t	caller_is_cli,
		   ushort		segment_id,
		   ulong                lesIndex)
{
    lsv_info_t	*lsv_info, *lsinfo;
    lane_info_t	*lane_info;
    idbtype *running;
    boolean nameBeingChanged;
    boolean typeBeingChanged;
    int	sub_mtu;

    /* create LE server structure, if needed.
     */
    lsv_info  = lsv_init_idb(swidb, lan_type);
    if (lsv_info == NULL)
		return(NULL);
    lane_info = lsv_info->lane_info;
    /*
     * check input validity
     */
    if ((lan_type != LANE_TYPE_802_3) && (lan_type != LANE_TYPE_802_5)) {
	if (caller_is_cli)
	    printf("%% %s\n", msgsym(BADTYPE, LANE).format);
	else
	    errmsg(&msgsym(BADTYPE, LANE));
	return(NULL);
    }
    if (name_size == 0  ||  name_size > MAX_ELAN_NAME_LENGTH) {
	if (caller_is_cli)
	    printf("%% %s\n", msgsym(BADNAME, LANE).format);
	else
	    errmsg(&msgsym(BADNAME, LANE));
	return(NULL);
    }
    /* check input validity of segment id  - if type is Token Ring */
    if ((lan_type == LANE_TYPE_802_5) && ((segment_id < 1) ||
					  (segment_id > SRB_MAX_RING_NUM))) {
	if (caller_is_cli)
	    printf("%% %s\n", msgsym(LSV_BADSEG, LANE).format);
	else
	    errmsg(&msgsym(LSV_BADSEG, LANE));
	return(NULL);
    }

    running = duplicate_serverbus(swidb, elan_name);
    if (running != NULL) {
	printf("%% A LANE server-bus for this elan is already configured on %s\n",
	       running->namestring);
	return NULL;
    }

    /* Ignore old LES type/name if going down, right now.
     */
    if (lsv_info->admin_state != LSV_LESBUS_ADMIN_UP) {
	/*
	 * If coming down, forget the old type/name information
	 */
	lane_info->type_from_les = FALSE;
	lane_info->name_from_les = FALSE;
    }

    /* if actual_name_size is not zero, it means that 
     * the lec is running; otherwise check the configuration
     * information in lane_info->name.
     */
    if (lane_info->lec_info && 
	lane_info->lec_info->actual_name_size != 0) {
	nameBeingChanged =
	    ((lane_info->lec_info->actual_name_size != name_size) ||
	     bcmp(lane_info->lec_info->actual_elan_name, 
		  elan_name, name_size));

    } else if (lane_info->name_size != 0) {	
	nameBeingChanged =
	    ((lane_info->name_size != name_size) ||
	     bcmp(lane_info->name, elan_name, name_size));
    } else
	nameBeingChanged = FALSE;

    typeBeingChanged = lane_info->type != lan_type;

    /*
    ** if an LEC is already running and either the elan name 
    ** or the elan type is being changed, its a no no.
    */
    if (lane_info->name_from_lec && nameBeingChanged) {
	if (caller_is_cli)
	    printf("%% %s\n", msgsym(NAMECHG, LANE).format);
	else
	    errmsg(&msgsym(NAMECHG, LANE));
	return(NULL);
    }
    if (lane_info->type_from_lec && typeBeingChanged) {
	if (caller_is_cli)
	    printf("%% %s\n", msgsym(TYPECHG, LANE).format);
	else
	    errmsg(&msgsym(TYPECHG, LANE));
	return(NULL);
    }

    /* if name is being changed, bring the lsv down first */
    if (nameBeingChanged || 
        ((lan_type == LANE_TYPE_802_5) && (lsv_info->seg_id != segment_id)))
       lsv_admin_down(swidb, LSV_DOWN_SHUTDOWN);

    /*
     * Set new type and name.
     */
    lane_info->type      = lan_type;
    lane_info->name_size = name_size;
    memset(lane_info->name, 0, MAX_ELAN_NAME_LENGTH+1);
    bcopy(elan_name, lane_info->name, name_size);

    /* 
     * If other lane module is running on the interface, 
     * it means that the mtu validation is done and 
     * max_data_frame is set.
     */
    if (!if_lane_exist(swidb)) {
	/* 
	 * if mtu is configured on this sub-interface, 
	 * check whether it is a valid value;
	 * otherwise use the default values.
	 */
	if (swidb->user_configured_mtu) {
	    if (is_valid_lane_mtu(swidb, swidb->sub_mtu)) {
		lane_info->max_data_frame = lane_mtu_to_frame(swidb);
	    } else {
		if (caller_is_cli) {
		    printf("%% Invalid LANE MTU %d.  ", swidb->sub_mtu);
		    printf("\n  Valid sizes are %d and %d for Ethernet and "
			   "%d and %d for TR",
			   LANE_MTU_ETHER_1516, LANE_MTU_ETHER_4544,
			   LANE_MTU_TR_4544, LANE_MTU_TR_9234);
		}
		return(NULL);
	    }
	} else {
	    lane_info->max_data_frame = lane_default_frame_size(swidb);
	    sub_mtu = lane_default_mtu(swidb);

	    if (swidb->sub_mtu != sub_mtu)
		if (!lane_set_maxmtu(swidb, sub_mtu))
		    return(NULL);
	}
    }

    lane_info->type_from_les = TRUE;
    lane_info->name_from_les = TRUE;

    /* if the type is changed to 802.3, reset segid to invalid value */
    if ((typeBeingChanged) && (lan_type == LANE_TYPE_802_3)) {
	lsv_info->seg_id = 0;
    }
    
    /* input segment_id is ignored if not token ring lan type */
    if (lan_type == LANE_TYPE_802_5) {
	lsv_info->seg_id = segment_id;
    }

    if (lesIndex)
	lsv_info->lesIndex = lesIndex;
    else {
	lsv_info->lesIndex = 1;
	for (lsinfo = lsv_all_lesbus; lsinfo; lsinfo = lsinfo->next) 
	    if ((lsinfo->lane_info->name_size == lane_info->name_size)
		&& (!memcmp(lsinfo->lane_info->name, lane_info->name,
			    lane_info->name_size))) 
		lsv_info->lesIndex = lsv_info->lesIndex >
		    lsinfo->lesIndex ? lsv_info->lesIndex :
			lsinfo->lesIndex++; 
    }

    return(lsv_info);
}


/*
 * lsv_admin_up - Start a LES/BUS on the way up
 *	returns TRUE if successful, else FALSE
 */
boolean
lsv_admin_up (lsv_info_t	*lsv_info)
{
    lane_info_t	*lane_info;
    /*
     * Make sure required data is present.
     */
    lane_info = lsv_info->lane_info;
    if (! lane_info->type_from_les  ||  ! lane_info->name_from_les) {
	return(FALSE);
    }

    /* Kill recycle timer.  If state machine already running, we're done.
     */
    lsv_info->admin_state = LSV_LESBUS_ADMIN_UP;
    mgd_timer_stop(&lsv_info->recycle_timer);
    switch (lsv_info->server_state) {
    case LSV_LESBUS_INITIALIZED:
    case LSV_LESBUS_WAITING_ILMI:
    case LSV_LESBUS_WAITING_LISTEN:
    case LSV_LESBUS_PROVISIONAL:
	/*
	 * Restart LES/BUS.
	 */
	return(lsv_start_lesbus(lsv_info));
    case LSV_LESBUS_OPERATIONAL:
	return(TRUE);
    case LSV_LESBUS_TERMINATING:
	lsv_kill_lesbus(lsv_info);
	return(FALSE);
    }

    return(FALSE);
}


/*
 * lsv_admin_down - Shut down a LES/BUS, allowing for later restart.
 *	returns TRUE if successful (or in progress), else FALSE
 */
boolean
lsv_admin_down (idbtype		  *swidb,
		lsv_admin_down_code_t  destroy)
{
    lsv_info_t	*lsv_info;
    lane_info_t	*lane_info;

    /* NO command given to kill the LES/BUS.  See if LES/BUS present.
     */
    lane_info = swidb->lane_info;
    if (lane_info == NULL)
	return(FALSE);

    lsv_info  = lane_info->lsv_info;
    if (lsv_info == NULL)
	return(FALSE);

    /* 
    ** clean out LECS access stuff:
    ** disconnect, discard all outstanding validation request
    ** but do NOT delete the holding list
    */
    cleanupLecsAccess(lsv_info, TRUE, TRUE, FALSE);

    /* Start it on the way down.  Do nothing if already terminating.
     * Don't let recycle timer slow things down.
     */
    mgd_timer_stop(&lsv_info->recycle_timer);
    switch (destroy) {
    case LSV_DOWN_SHUTDOWN:
	lsv_info->admin_state = LSV_LESBUS_ADMIN_DOWN;
	break;
    case LSV_DOWN_NO_LANE:
	lsv_info->admin_state    = LSV_LESBUS_ADMIN_DOWN;
	lane_info->type_from_les = FALSE;
	lane_info->name_from_les = FALSE;
	break;
    case LSV_DOWN_ERASE:
	lsv_info->admin_state = LSV_LESBUS_ADMIN_DESTROY;

	/*
	** the 2 lines below are to handle subif deletion
	** when the les/bus is configured on the subif but
	** NEVER came up becoz the subif was shut down at
	** the time when the les bus was actually configured.
	*/
	lsv_kill_lesbus(lsv_info);
	return TRUE;

	/* old code */
	/* break; */
    }
    if (lsv_info->server_state != LSV_LESBUS_TERMINATING &&
	lsv_info->server_state != LSV_LESBUS_INITIALIZED) {
	if (lane_lsv_debug)
	    buginf("\nLES %s shutting down", swidb->namestring);
	lsv_kill_lesbus(lsv_info);
    }
    return(TRUE);
}


/*
 * lesbus_interface_updown - hwif or swif just finished changing state.
 */
void
lesbus_interface_updown (idbtype *swidb, hwidbtype *hwidb)
{
    lane_info_t	*lane_info;
    lsv_info_t	*lsv_info;

    if (hwidb != NULL) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    lane_info = swidb->lane_info;
	    if (lane_info == NULL)
		continue;
	    lsv_info = lane_info->lsv_info;
	    if (lsv_info != NULL) {
		if (lane_lsv_debug)
		    buginf("\nLES %s elan %s interface state change",
			   swidb->namestring, lsv_info->lane_info->name);
		if (interface_admin_down(swidb)) {
		    lsv_admin_down(swidb, LSV_DOWN_SHUTDOWN);
		} else {
		    lsv_admin_up(lsv_info);
		}
	    }
	}
	return;
    }

    if (swidb != NULL) {
	lane_info = swidb->lane_info;
	if (lane_info == NULL)
	    return;
	lsv_info = lane_info->lsv_info;
	if (lsv_info != NULL) {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s subinterface state change",
			swidb->namestring, lsv_info->lane_info->name);
	    if (interface_admin_down(swidb)) {
		lsv_admin_down(swidb, LSV_DOWN_SHUTDOWN);
	    } else {
		lsv_admin_up(lsv_info);
	    }
	}
    }
}


/*
 * lsv_swif_erase - Handle deletion of sub-interface for LES/BUS
 */
void
lsv_swif_erase (lane_info_t	*lane_info)
{
    lsv_info_t	*lsv_info;

    lsv_info = lane_info->lsv_info;
    if (lsv_info == NULL)
	return;
    lsv_admin_down(lane_info->swidb, LSV_DOWN_ERASE);
}


/*
** callback handler for when we get the master LECS address.
*/
static void lsv_handleMasterLecsAddress (idbtype *swidb, 
    uint transactionId, void *userContext, 
    int count, hwaddrtype *lecs_addresses)
{
    lsv_info_t *lsv_info;
    lane_info_t *lane_info;
    boolean changed = FALSE;
    uchar *old, *new;

    if (!swidb) return;
    lane_info = swidb->lane_info;
    if (!lane_info) return;
    lsv_info = lane_info->lsv_info;
    if (!lsv_info) return;
    if ((lsv_info_t*) userContext != lsv_info) {
	LES_ERR("lsv_handleMasterLecsAddress: "
	    "inconsistent context ptr");
	return;
    }

    /* LES must be up to proceed */
    if (lsv_info->server_state != LSV_LESBUS_OPERATIONAL) {
	cleanupLecsAccess(lsv_info, TRUE, TRUE, FALSE);
	return;
    }

    LES_DBG(swidb->namestring,
	"received %d master LECS addr: %Ch",
	count, lecs_addresses);

    /*
    ** we got it, so, if changed or not already 
    ** connected, then re-connect to it.
    */
    if ((count == 1) && lane_addr_valid(lecs_addresses)) {
	if (!lane_addr_valid(&lsv_info->lsv_lecs_nsap)) {
	    changed = TRUE;
	} else {
	    old = lsv_info->lsv_lecs_nsap.addr;
	    new = lecs_addresses->addr;
	    if (byteStreamCompare(old, new, STATIONLEN_ATMNSAP))
		changed = TRUE;
	}
	if (changed) {
	    cleanupLecsAccess(lsv_info, TRUE, TRUE, FALSE);
	    lsv_info->lsv_lecs_nsap = *lecs_addresses;
	}
	lsv_connectToMasterLecs(lsv_info);
    } else if (count <= 0) {
    /*
    ** an error occured while trying to get the master
    ** LECS address.  So, restart this timer so that we will
    ** try it again in a while, when the timer expires.
    */
	LES_DBG(swidb->namestring,
	    "could not obtain the master LECS address, "
	    "will retry in %d seconds",
	    LECS_RECONNECT_TIME);
	mgd_timer_start_jittered
	    (&lsv_info->lecsAccess.lecsReconnectTimer, 
	     LECS_RECONNECT_TIME*ONESEC, 20);
    } else {
    /* this should NOT happen */
	LES_ERR("lsv_handleMasterLecsAddress: invalid count %d", 
	    count);
    }
}

/*****************************************************************
 * LES/BUS state machine drivers
 *
 * These should, perhaps, be combined into one driver routine.
 ****************************************************************/


/*
 * lsv_start_lesbus - Start a LES/BUS
 *	return TRUE if all OK, FALSE if unable to start LES/BUS
 */
boolean
lsv_start_lesbus (lsv_info_t  *lsv_info)
{
    lane_info_t	*lane_info;
    hwaddrtype	 my_nsap;
    boolean	 more_to_do;
    hwidbtype	*hwidb;

    /* Start process, if needed
     */
    lsv_start();
    if (! lsv_running)
	return(TRUE);

    /* Turn around if going down
     */
    if (lsv_info->admin_state  != LSV_LESBUS_ADMIN_UP  ||
	lsv_info->server_state == LSV_LESBUS_TERMINATING  ) {
	lsv_kill_lesbus(lsv_info);
	return(FALSE);
    }
    /*
     * Come back later if sub-interface is down
     */
    lane_info = lsv_info->lane_info;
    hwidb     = hwidb_or_null(lane_info->swidb);
    if (hwidb == NULL)
	return(FALSE);
    if (interface_admin_down(lane_info->swidb))
	return(FALSE);
    
    /* Don't tie up the stack in knots
     */
    if (lsv_info->dont_recurse)
	return(FALSE);
    /*
     * Don't start if recycle timer is not expired
     */
    if (mgd_timer_running(&lsv_info->recycle_timer))
	return(FALSE);

    /* Take next step in bring-up
     */
    switch(lsv_info->server_state) {
    case LSV_LESBUS_INITIALIZED:
	/*
	 * check inputs and record start time
	 */
	if (! lane_info->type_from_les  ||  ! lane_info->name_from_les)
	    return(FALSE);
	GET_TIMESTAMP(lsv_info->start_time);
	lsv_server_new_state(lsv_info, LSV_LESBUS_WAITING_ILMI);

    case LSV_LESBUS_WAITING_ILMI:
	/*
	 * Try to find our own addresses.
	 */
	more_to_do		= FALSE;
	lsv_info->dont_recurse  = TRUE;
	switch(lane_ilmi_compute_atmsap(lane_info->swidb, 
					LANE_ILMI_BUS,
					&lane_info->bus_nsap,
					&my_nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    lsv_info->bus_nsap			 = my_nsap;
	    lsv_info->forward_vcc.calling_addr	 = my_nsap;
	    lsv_info->bus_listen_vcc.called_addr = my_nsap;
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	    errmsg(&msgsym(LSV_BADAD, LANE), lane_info->swidb->namestring,
		   lane_info->name, "BUS");
	    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	    return(FALSE);
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    more_to_do = TRUE;
	    break;
	}
	switch(lane_ilmi_compute_atmsap(lane_info->swidb, 
					LANE_ILMI_LE_SERVER,
					&lane_info->lsv_nsap,
					&my_nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    lsv_info->lsv_nsap			  = my_nsap;
	    lsv_info->distribute_vcc.calling_addr = my_nsap;
	    lsv_info->lsv_listen_vcc.called_addr  = my_nsap;
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	    errmsg(&msgsym(LSV_BADAD, LANE), lane_info->swidb->namestring,
		   lane_info->name, "server");
	    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	    return(FALSE);
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    more_to_do = TRUE;
	    break;
	}

	if (more_to_do) {
	    lsv_info->dont_recurse = FALSE;
	    return(TRUE);
	}

	/* Advance to next state
	 */
	lsv_server_new_state(lsv_info, LSV_LESBUS_WAITING_LISTEN);
	/*
	 * Create Control Distribute and BUS Forward Multipoint groups
	 */
	if (LANE_SIG_SUCCESS !=
	    lane_create_mcgroup(&lsv_info->distribute_vcc, lane_info)) {
	    errmsg(&msgsym(LSV_MCAST, LANE), lane_info->swidb->namestring,
			   lane_info->name, "control distribute");
	    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	    return(FALSE);
	}
	if (LANE_SIG_SUCCESS !=
	    lane_create_mcgroup(&lsv_info->forward_vcc, lane_info)) {
	    errmsg(&msgsym(LSV_MCAST, LANE), lane_info->swidb->namestring,
			   lane_info->name, "MC forward");
	    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	    return(FALSE);
	}

	/* FALL THROUGH TO NEXT CASE
	 */
    case LSV_LESBUS_WAITING_LISTEN:
	/*
	 * Start listening for incoming calls
	 */
	more_to_do		= FALSE;
	lsv_info->dont_recurse  = TRUE;

	switch(lsv_start_listen(lsv_info, &lsv_info->lsv_listen_vcc)) {
	case LANE_SIG_SUCCESS:
	    break;
	case LANE_SIG_FAILURE:
	    return(FALSE);
	case LANE_SIG_CALLBACK:
	    more_to_do = TRUE;
	    break;
	}
	switch(lsv_start_listen(lsv_info, &lsv_info->bus_listen_vcc)) {
	case LANE_SIG_SUCCESS:
	    break;
	case LANE_SIG_FAILURE:
	    return(FALSE);
	case LANE_SIG_CALLBACK:
	    more_to_do = TRUE;
	    break;
	}
	if (more_to_do) {
	    lsv_info->dont_recurse = FALSE;
	    return(TRUE);
	}

	/* Advance to next state
	 */
	lsv_server_new_state(lsv_info, LSV_LESBUS_PROVISIONAL);
	/*
	 * pick up any PVCs dropped because we weren't in the right state
	 */

    case LSV_LESBUS_PROVISIONAL:
	/*
	 * See whether we have a prefix from the switch.  Wait if we don't.
	 */
	lsv_info->dont_recurse  = TRUE;
	switch(lane_ilmi_compute_atmsap(lane_info->swidb, 
					LANE_ILMI_LE_SERVER,
					&lsv_default_masked_addr,
					&my_nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	    errmsg(&msgsym(LSV_BADAD, LANE), lane_info->swidb->namestring,
		   lane_info->name, "server");
	    lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	    return(FALSE);
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    lsv_info->dont_recurse = FALSE;
	    return(TRUE);
	}

	/* FALL THROUGH TO NEXT CASE
	 */
    case LSV_LESBUS_OPERATIONAL:

	lsv_info->dont_recurse  = TRUE;
	/*
	 * register our addresses
	 */
	lsv_register_nsap(lsv_info, &lsv_info->lsv_nsap,
			  &lsv_info->lsv_reg_state,
			  &lsv_info->lsv_reg_transid);
	lsv_register_nsap(lsv_info, &lsv_info->bus_nsap,
			  &lsv_info->bus_reg_state,
			  &lsv_info->bus_reg_transid);
	/*
	 * done
	 */
	lsv_info->dont_recurse = FALSE;
	return(TRUE);

    case LSV_LESBUS_TERMINATING:
	lsv_info->dont_recurse = FALSE;
	lsv_kill_lesbus(lsv_info);
	return(FALSE);
    }
    lane_router_crash("lsv_start_lesbus");
    return(FALSE);
}


/*
 * lsv_kill_lesbus - Stop a LES/BUS
 */
void
lsv_kill_lesbus (lsv_info_t  *lsv_info)
{
    boolean		 keep_going;
    lsv_client_t	*client;
    lsv_client_t	*next_client;
    lane_info_t		*lane_info;

    /* Don't tie the stick up in recursive knots
     */
    if (lsv_info->dont_recurse)
	return;
    lsv_info->dont_recurse = TRUE;
    lane_info = lsv_info->lane_info;
    /*
     * Start LES/BUS on the way down
     */
    lsv_server_new_state(lsv_info, LSV_LESBUS_TERMINATING);

    /* 
    ** clean up all the LECS access stuff 
    ** except the validation holding list.
    */
    cleanupLecsAccess(lsv_info, TRUE, TRUE, FALSE);

    /*
     * Stop all further connections
     */
    keep_going  = lsv_stop_listening(&lsv_info->bus_listen_vcc);
    keep_going &= lsv_stop_listening(&lsv_info->lsv_listen_vcc);
    if (! keep_going) {
	lsv_info->dont_recurse = FALSE;
	return;
    }

    /* Kill all the clients
     */
    client = lsv_info->client_table;
    while (client != NULL) {
	next_client = client->next;
	if (client->client_state != LSV_CLIENT_INITIALIZED  &&
	    client->client_state != LSV_CLIENT_TERMINATING) {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s killing client %d",
			lane_info->swidb->namestring,
			lane_info->name, client->lecid);
	    lsv_kill_client(lsv_info, client);
	}
	client = next_client;
    }
    /*
     * Let state machine finish up if not all clients are gone
     */
    if (lsv_info->client_table != NULL) {
	lsv_info->dont_recurse = FALSE;
	return;
    }

    /*
     * Kill the distribute and forward multicast groups
     */
    keep_going  = lsv_kill_mcgroup(&lsv_info->distribute_vcc);
    keep_going &= lsv_kill_mcgroup(&lsv_info->forward_vcc);
    /*
     * Unregister our addresses with the switch
     */
    keep_going &= lsv_unregister_nsap(lsv_info, &lsv_info->lsv_nsap,
				      &lsv_info->lsv_reg_state,
				      &lsv_info->lsv_reg_transid);
    keep_going &= lsv_unregister_nsap(lsv_info, &lsv_info->bus_nsap,
				      &lsv_info->bus_reg_state,
				      &lsv_info->bus_reg_transid);
    if (! keep_going) {
	lsv_info->dont_recurse = FALSE;
	return;
    }

    /* All done!
     */
    lsv_info->dont_recurse = FALSE;
    lsv_server_new_state(lsv_info, LSV_LESBUS_INITIALIZED);
    /*
     * Restart if required
     */
    switch (lsv_info->admin_state) {
    case LSV_LESBUS_ADMIN_UP:
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s restarting after kill",
		lane_info->swidb->namestring, lane_info->name);
	lsv_start_lesbus(lsv_info);
	return;
    case LSV_LESBUS_ADMIN_DOWN:
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s down",
		lane_info->swidb->namestring, lane_info->name);
	return;
    case LSV_LESBUS_ADMIN_DESTROY:
	break;
    }
    if (lane_lsv_debug)
	buginf("\nLES %s killing elan %s",
		lane_info->swidb->namestring, lane_info->name);
    lsv_release_lesbus_structures(lsv_info);
}


/************************************************************************
 * LES/BUS control interface
 ************************************************************************/


/*
 * lsv_start_pended_actions - Restart anybody waiting for initialization delay
 *			      Called by main LES/BUS process to start up the
 *			      LES/BUS state machines after all the NV RAM
 *			      commands have been read.
 */
void
lsv_start_pended_actions (void)
{
    lsv_info_t	*lsv_info;
    boolean	 more_to_do;

    /*
     * start from beginning of chain each time, because we let other
     * processes run after each startup.
     */
    do {
	lsv_info = lsv_all_lesbus;
	more_to_do = FALSE;
	while (lsv_info) {
	    if (lsv_info->server_state == LSV_LESBUS_INITIALIZED  &&
		lsv_info->admin_state  == LSV_LESBUS_ADMIN_UP     &&
                !interface_admin_down(lsv_info->lane_info->swidb)) {
		mgd_timer_stop(&lsv_info->recycle_timer);
		lsv_start_lesbus(lsv_info);
		process_suspend();
		more_to_do = TRUE;
		break;
	    } else if (lsv_info->server_state == LSV_LESBUS_TERMINATING) {
		lsv_kill_lesbus(lsv_info);
		process_suspend();
		more_to_do = TRUE;
		break;
	    }
	    lsv_info = lsv_info->next;
	}
    } while (more_to_do);
}


/*
 * lsv_init_idb
 *
 * Init the swidb with an LE Server info block.  If the lane info block
 * hasn't been allocated yet, go do it too.  Anybody can call this who
 * needs the lsv_info_t data base to be created.  Don't create it, though,
 * unless you need to store something in it.
 */

lsv_info_t *
lsv_init_idb (idbtype *swidb, int elan_type)
{
    lane_info_t		*lane_info;
    lsv_info_t		*lsv_info;
    hwidbtype		*hwidb;

    hwidb = hwidb_or_null(swidb);
    if (hwidb == NULL)
	return(NULL);
  
    if (swidb->lane_info == NULL) {
        lane_info = lane_init_idb(swidb);
	if (lane_info == NULL) {
	    errmsg(&msgsym(NOMEMORY, SYS), "LANE info struct",
		   swidb->if_number);
	    return(NULL);
	}
	lane_info->type = elan_type; 
    } else {
	lane_info = swidb->lane_info;
	lane_info->type = elan_type;
    }
  
    lsv_info = lane_info->lsv_info;
    if (lsv_info != NULL) {
	return(lsv_info);
    } else {
	lsv_info = malloc(sizeof(lsv_info_t));
	if (lsv_info == NULL) {
	    return(NULL);
	}
    }

    *lsv_info = LSV_INFO_EMPTY;

    switch (lane_info->type) {
    case LANE_TYPE_802_3:
	lsv_info->forward_vcc.vc_type = LANE_BUS_FORWARD;
	lsv_info->bus_listen_vcc.vc_type = LANE_BUS_SEND;
	break;
    case LANE_TYPE_802_5:
	lsv_info->forward_vcc.vc_type = LANE_BUS_TR_FORWARD;
	lsv_info->bus_listen_vcc.vc_type = LANE_BUS_TR_SEND;
	break;
    default:
	crashdump(0);
	return(NULL);
    }
 
    lsv_info->distribute_vcc.vc_type	= LANE_LES_DISTRIBUTE;
    lsv_info->lane_info			= lane_info;
    lsv_info->lsv_listen_vcc.no_svc_timeout = TRUE;
    lsv_info->lsv_listen_vcc.swidb	= swidb;
    lsv_info->lsv_listen_vcc.callback	= lsv_sigback_listen_direct;
    lsv_info->lsv_listen_vcc.vc_type	= LANE_LES_DIRECT;
    lsv_info->lsv_listen_vcc.parm	= lsv_info;
    lsv_info->bus_listen_vcc.no_svc_timeout = TRUE;
    lsv_info->bus_listen_vcc.swidb	= swidb;
    lsv_info->bus_listen_vcc.callback	= lsv_sigback_listen_send;
    lsv_info->bus_listen_vcc.parm		= lsv_info;
    
    /* LECS access for client verification */
    snpa_clear(&lsv_info->lsv_lecs_nsap);
    lsv_info->lecsAccess.callId = NULL;
    lsv_info->lecsAccess.validationRequests = List_alloc();
    if (lsv_info->lecsAccess.validationRequests == NULL) {
	LES_ERR("lsv_init_idb: List_alloc failed");
	free(lsv_info);
	return NULL;
    }
    
    /* client index */
    lsv_info->clientList = Ulong_Index_alloc();
    if (lsv_info->clientList == NULL) {
	LES_ERR("lsv_init_idb: Ulong_Index_alloc failed");
	List_destroy(lsv_info->lecsAccess.validationRequests);
	free(lsv_info);
	return NULL;
    }
    
    mgd_timer_init_leaf(&lsv_info->lecsAccess.lecsReconnectTimer,
			&lsv_root_timer, LSV_TIMER_LECS_ACCESS, 
			lsv_info, FALSE);
    
    mgd_timer_init_parent(&lsv_info->timer_node, &lsv_root_timer);
    mgd_timer_init_leaf(&lsv_info->recycle_timer, &lsv_info->timer_node,
			LSV_TIMER_RECYCLE, lsv_info, FALSE);
    
    lsv_info->next	= lsv_all_lesbus;
    lsv_all_lesbus	= lsv_info;
    
    lane_info->lsv_info	= lsv_info;

    return(lsv_info);

}


