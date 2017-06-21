/* $Id: lane_server.c,v 3.3.28.14 1996/08/28 12:54:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_server.c,v $
 *------------------------------------------------------------------
 * lane_server.c -- LAN Emulation Server
 *
 * December 1994, Norm Finn
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_server.c,v $
 * Revision 3.3.28.14  1996/08/28  12:54:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.28.13  1996/08/16  14:41:19  michellw
 * CSCdi61426:  LANE: _lane_show_one_all  crash
 * Branch: California_branch
 * Null out the reg_table->next pointer when freeing this entry.
 * Use while loop instead of for loop when traversing
 * lsv_info->reg_table.
 *
 * Revision 3.3.28.12  1996/08/12  20:05:22  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.3.28.11  1996/07/30  14:42:47  michellw
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
 * Revision 3.3.28.10  1996/07/12  01:44:03  cakyol
 * CSCdi61426:  LANE: _lane_show_one_all  crash
 * missing mem_lock() and free() during printf's.
 * Branch: California_branch
 *
 * Revision 3.3.28.9  1996/07/11  17:41:04  bbenson
 * CSCdi62539:  ALC needs a registry for obtaining lsv i/f by name
 * Branch: California_branch
 * Add reg_add_lsv_find_by_name_on_interface.
 *
 * Revision 3.3.28.8  1996/07/11  17:18:04  bbenson
 * CSCdi62539:  ALC needs a registry for obtaining lsv i/f by name
 * Branch: California_branch
 *
 * Revision 3.3.28.7  1996/07/02  22:19:50  michellw
 * CSCdi61813:  LEC was not invoked when the timer expired
 * Branch: California_branch
 * Changed LEC, LES/BUS and LECS process priority to NORMAL.
 *
 * Revision 3.3.28.6  1996/05/30  07:24:38  cakyol
 * CSCdi59052:  LES gives the wrong vcd number during debugging
 * Branch: California_branch
 *
 * Revision 3.3.28.5  1996/05/23  22:31:58  cakyol
 * CSCdi58596:  break missing in token ring debug message.
 *              fixed.
 * Branch: California_branch
 *
 * Revision 3.3.28.4  1996/05/17  22:08:10  cakyol
 * CSCdi58049:  minor correction in lane display for better readability
 * Branch: California_branch
 *
 * Revision 3.3.28.3  1996/05/09  14:32:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.28.2.2.1  1996/04/27  07:06:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.28.2  1996/04/25  23:14:31  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.28.1.8.7  1996/04/19  02:02:11  shj
 * CSCdi55100:  need display of unicast pkts forwarded by LANE BUS
 * Branch: LE_Cal_V112_0_2_branch
 * Add counter to display of multicast-forward vc.
 *
 * Revision 3.3.28.1.8.6  1996/04/16  02:44:20  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.28.1.8.5  1996/04/11  18:19:55  cakyol
 * amendments to the latest sync:
 * - take some unnecessary crashdump's out of lane server.
 * - correct possible input buffer loss at LECS input processing
 *   for token ring lan type.
 * - debugging messages needed tidying up.
 * - some but not all white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.28.1.8.4  1996/04/11  05:33:29  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.3.28.1.8.3  1996/04/08  21:01:11  cakyol
 * correct sync errors:
 * - remove spurious (unprotected) buginf messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.28.1.8.2  1996/04/08  14:51:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.28.1.8.1  1996/04/08  01:56:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.28.1  1996/03/18  20:32:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.32.3  1996/04/05  03:20:25  cakyol
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
 * Revision 3.3.32.2  1996/04/02  10:09:45  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.32.1  1996/03/22  22:54:24  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.18.4  1996/03/20  01:13:13  sakumar
 * Handle LES packets that need to be reflected at process level
 * since Cat5K ATM Module does not have fast switching.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.18.3  1996/03/08  01:10:18  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix router/Synergy conflicts.
 *
 * Revision 3.3.18.2  1996/03/05  06:42:57  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.18.1  1996/03/03  21:23:28  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.12.2  1996/03/01  01:46:30  cakyol
 * convert "crashdump"s to "lane_router_crash" which can print
 * a last gasp message and call "logger_flush" before dying.
 * Helps debugging a lot.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.12.1  1996/02/27  21:10:17  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/22  06:35:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2.16.1  1996/02/29  07:48:49  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.6  1996/02/22  21:08:11  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.5  1996/02/16  22:14:59  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Scheduler interface changes for the LANE server code.
 *
 * Revision 3.2.14.4  1996/02/06  22:19:19  cakyol
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
 * Revision 3.2.14.3  1996/01/30  23:56:18  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.2  1996/01/26  02:02:35  cakyol
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
 * Revision 3.2.14.1.2.2  1996/03/23  21:12:31  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 * 
 * Revision 3.2.14.1  1996/01/11  01:46:18  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:38  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20.18.1  1996/02/03  07:07:04  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.20  1995/09/02  01:45:18  shj
 * CSCdi39757:  LANE sever rejects join packet if it is too long
 * Change test for rejection to "< size" rather than "!= size."
 * Also fix some incorrect buginf formats.
 *
 * Revision 2.19  1995/08/30  02:34:10  bbenson
 * CSCdi37992:  LANE packet count can go negative
 * - Change appropriate format chars from d to u.  AKA gratuitous commit
 *   to impress the boss.
 *
 * Revision 2.18.22.1  1995/10/16  23:45:45  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.18  1995/07/31  02:22:04  nfinn
 * CSCdi37914:  LES/BUS should show time-to-restart like LEC does
 *
 * Revision 2.17  1995/07/29  06:07:00  nfinn
 * CSCdi37717:  show lane should not display de-configured LANE
 * components.  Bug not resolved, yet.  This commit improves
 * the diagnostic messages when the bug occurs.
 *
 * Revision 2.16  1995/07/29  01:10:13  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.15  1995/07/25  22:04:20  nfinn
 * CSCdi37254:  sh lane shows server/bus when its not configured.
 *
 * Revision 2.14  1995/07/25  21:53:59  nfinn
 * CSCdi37659:  LANE server hangs waiting to get LECS addr from switch
 *
 * Revision 2.13  1995/07/24  04:55:53  nfinn
 * CSCdi37072:  LECS access code uses only the first LECS address
 * found via ILMI.  Also, sevel "show lane" and LANE error messages
 * need cleaning up.  A small memory leak occurred whenever LANE was
 * configured and the interface was destroyed.
 *
 * Revision 2.12  1995/07/12  01:47:51  nfinn
 * CSCdi36959:  LECS access attempts to connect to ATM address 0
 *
 * Revision 2.11  1995/07/11  05:27:26  bbenson
 * CSCdi36864:  No need to display UPDOWN message periodically.
 * CSCdi36904:  LEC should retransmit JOIN requests C13_max_retry_count
 * times.
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes.
 *
 * Revision 2.10  1995/07/08  02:20:16  nfinn
 * CSCdi36834:  LEC not told when LES/BUS ATM address changes
 *
 * Revision 2.9  1995/07/08  01:47:58  nfinn
 * CSCdi36823:  Missing LES/BUS entries in show lane
 *
 * Revision 2.8  1995/06/27  02:11:14  nfinn
 * CSCdi35046:  Server should wait for ILMI prefix before registering any
 * address.
 *
 * Revision 2.7  1995/06/26  23:21:56  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 * Revision 2.6  1995/06/23  22:17:51  cakyol
 * CSCdi35865:  No error or warning messages displayed for clear atm
 * server...
 * fixed.  Most of the error messages were already there but they were
 * protected buginf's.  I changed them to direct printf's for
 * immediate response.
 *
 * Revision 2.5  1995/06/23  18:48:05  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/20  18:13:00  cakyol
 * CSCdi35998:  no error while configuring LEC on subif missing LECS NSAP
 * fixed
 *
 * Revision 2.3  1995/06/19  06:45:52  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:10:52  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:19:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * LES/BUS code.  Initialization is at the end of the file.
 ************************************************************************/


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "stdarg.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lsv_private.h"
#include "../atm/ilmi_api.h"
#include "lane_ilmi.h"
#include "../ui/debug.h"
#include "parser_defs_lane.h"
#include "lane_debug.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "lane_registry.h"


/*
 * G l o b a l  S t o r a g e
 */
boolean		lsv_running;		/* LES/BUS process past NVRAM wait */
ilmiClient	lsv_ilmi_handle;	/* Client ID: ILMI requests */
ulong		lane_ilmi_transaction_id;	/* transaction ID for ILMI */

/*
 * L o c a l   S t o r a g e
 */

static	boolean		lsv_started;		/* LES/BUS process started */

static const char lsv_20_byte[] = "20-byte ATM address required";

/*
 * LAN emulation control packets come in through this queue.
 */
#define		MAX_LSV_INPUT_QLEN	40
watched_queue	*lsv_inputQ;


/********************************************************
**			helpers etc.
********************************************************/

/*
** is this a "valid" les/bus ?  Is the pointer valid ? 
*/
boolean valid_lesbus (lsv_info_t *lsv_info)
{
    lsv_info_t *scanner;

    if (lsv_info == NULL) return FALSE;
    scanner = lsv_all_lesbus;
    while (scanner)
    {
	if (scanner == lsv_info) return TRUE;
	scanner = scanner->next;
    }
    return FALSE;
}

/****************************************************************
 * LES/BUS show CLI commands
 ****************************************************************/


/*
 * lsv_pr_admin_state - convert lsv_server_admin_stat_t to character string
 */
static char *
lsv_pr_admin_state (
    lsv_server_admin_stat_t state)
{
    switch(state) {
    case LSV_LESBUS_ADMIN_UP:		return "up";
    case LSV_LESBUS_ADMIN_DOWN:		return "down";
    case LSV_LESBUS_ADMIN_DESTROY:	return "destroy";
    default:                            return "[undefined]";
    }
}


/*
 * lsv_pr_server_state - convert lsv_server_state_t to character string
 */
char *
lsv_pr_server_state (
    lsv_server_state_t state)
{
    switch(state) {
    case LSV_LESBUS_INITIALIZED:        return "down";
    case LSV_LESBUS_WAITING_ILMI:       return "waiting_ILMI";
    case LSV_LESBUS_WAITING_LISTEN:     return "waiting_listen";
    case LSV_LESBUS_PROVISIONAL:        return "up_not_registered";
    case LSV_LESBUS_OPERATIONAL:        return "operational";
    case LSV_LESBUS_TERMINATING:        return "terminating";
    default:                            return "[undefined]";
    }
}


/*
 * lsv_pr_client_state - convert lsv_client_state_t to character string
 */
static char *
lsv_pr_client_state (
    lsv_client_state_t state)
{
    switch(state) {
    case LSV_CLIENT_INITIALIZED:	return "I";
    case LSV_CLIENT_CONNECTED:		return "C";
    case LSV_CLIENT_JOIN_RECEIVED:	return "W";
    case LSV_CLIENT_ADD_SENT:		return "A";
    case LSV_CLIENT_JOINED:		return "J";
    case LSV_CLIENT_OPERATIONAL:	return "O";
    case LSV_CLIENT_REJECTED:		return "R";
    case LSV_CLIENT_TERMINATING:	return "T";
    default:				return "U";
    }
}


/* lsv_nsap_or_template - Print an ATM address template or the full
 *			  ATM address derived from it, according to
 *			  whether the state of the LES/BUS makes the
 *			  full ATM address meaningful.
 */
static void
lsv_nsap_or_template (
    lsv_server_state_t	 server_state,			/* state of owner */
    hwaddrtype		*atm_addr,			/* derived address */
    MaskedAddress	*template,			/* template for addr */
    char		 buf[ATM_MAX_NSAP_STRING])	/* output buffer */
{
    switch (server_state) {
    case LSV_LESBUS_INITIALIZED:
    case LSV_LESBUS_WAITING_ILMI:
    case LSV_LESBUS_TERMINATING:
	sprintf(buf, "%Cw", template);
	break;
    default:
	atm_printnsap(atm_addr->addr, buf);
	break;
    }
}


/*
 * lsv_lesbus_alive - return TRUE if LES/BUS is not quiescent.  That is,
 *		      if it is going to do something on its own, sometime.
 *		      Return FALSE if it's just going to sit there.
 */
boolean
lsv_lesbus_alive (
    lsv_info_t *lsv_info)
{
    return (lsv_info != NULL					  &&
	    (lsv_info->server_state != LSV_LESBUS_INITIALIZED  ||
	     mgd_timer_running(&lsv_info->recycle_timer)	 )   );
}


/*
 * lsv_show_sleep_time - Display time until next restart of LES/BUS or null
 */
static void
lsv_show_sleep_time (
    lsv_info_t	*lsv_info)
{
    if (lsv_info->admin_state == LSV_LESBUS_ADMIN_UP		&&
	(lsv_info->server_state == LSV_LESBUS_INITIALIZED  ||
	 lsv_info->server_state == LSV_LESBUS_TERMINATING     )	&&
	mgd_timer_running(&lsv_info->recycle_timer)		   )
    {
	printf("Next restart attempt in %d seconds\n",
	       (mgd_timer_left_sleeping(&lsv_info->recycle_timer) 
			+ ONESEC/2) / ONESEC);
    }
}


/*
 * lane_show_one_bus
 *
 * Show BUS information associated with specified lane_info.
 *
 * Always shows "summary" information, includes per client info
 * if brief == FALSE.
 */
void
lane_show_one_bus (
    lane_info_t	*lane_info,
    boolean	 brief)
{
    lsv_info_t          *lsv_info;
    hwidbtype           *hwidb;
    lsv_client_t        *client;
    int                  idx;
    unsigned             ipkts, opkts;
    char                 buf[ATM_MAX_NSAP_STRING];

    /* Don't show it if it's not there.
     */
    if (lane_info == NULL)
        return;
    if (! lane_info->type_from_les)
		return;
    lsv_info = lane_info->lsv_info;
    if (lsv_info == NULL)
		return;

    /* Show "sumamry" information for this BUS.
     * The BUS doesn't have it's own "state", so we use the state
     * of the bus-forward vcc to decide if we are "up" or "down".
     * If it's CONNECTED, we consider the BUS to be up.
     */
    mem_lock(lsv_info);
    mem_lock(lane_info);
    hwidb = lane_info->swidb->hwptr;
    printf("\nLE BUS %s  ELAN name: %s  Admin: %s  State: %s\n",
           lane_info->swidb->namestring, lane_info->name,
           lsv_pr_admin_state(lsv_info->admin_state),
           (lsv_info->forward_vcc.mcgroup_state == LANE_VCC_CONNECTED) ?
           "operational" : "down");
    lsv_show_sleep_time(lsv_info);
    if (lsv_info->admin_state == LSV_LESBUS_ADMIN_DESTROY && lane_lsv_debug) {
	printf("\nWaiting for");
	if (lsv_info->bus_listen_vcc.vcc_state != LANE_VCC_INITIALIZED)
	    printf(" BUS_listen");
	if (lsv_info->lsv_listen_vcc.vcc_state != LANE_VCC_INITIALIZED)
	    printf(" LES_listen");
	if (lsv_info->client_table != NULL)
	    printf(" client_kill");
	if (lsv_info->distribute_vcc.mcgroup_state != LANE_VCC_INITIALIZED)
	    printf(" dist_destroy");
	if (lsv_info->forward_vcc.mcgroup_state != LANE_VCC_INITIALIZED)
	    printf(" fwd_destroy");
	if (lsv_info->lsv_reg_state != LANE_ADREG_INITIALIZED)
	    printf(" LES_unreg");
	if (lsv_info->bus_reg_state != LANE_ADREG_INITIALIZED)
	    printf(" BUS_unreg");
    }
    printf("type: %s         Max Frame Size: %s",
           lane_pr_lan_type(lane_info->type),
           lane_pr_max_frame_size(lane_info->max_data_frame));
    if (lane_info->type == LANE_TYPE_802_5)
       {
       printf("      Segment ID: ");
       if (lsv_info->seg_id == 0)
          printf("not set");
       else printf ("%d", lsv_info->seg_id);
       }
    printf("\n");

    lsv_nsap_or_template(lsv_info->server_state, &lsv_info->bus_nsap,
			 &lane_info->bus_nsap, buf);
    printf("ATM address: %s\n", buf);
    if (lsv_info->forward_vcc.vcd != 0) {
        atm_getvc_counts(hwidb, lsv_info->forward_vcc.vcd,
                              &ipkts, &opkts);
        printf("data forward: vcd %u, %u members, %u packets, %u unicasts\n",
               lsv_info->forward_vcc.vcd, lsv_info->forward_vcc.n_members,
               opkts, lsv_info->buss.busUnicastForwards);
    }

    /* If "brief", or no clients on this server/bus, done.
     */
    if (brief || (lsv_info->client_table == NULL)) {
        free(lane_info);
        free(lsv_info);
        return;
    }

    /* Show info for data send vcc for each client
     */
    automore_header("lecid  vcd     pkts   ATM Address\n");
    idx = 1;
    for (client = lsv_info->client_table; client;
        client = queuepick(&lsv_info->client_table, idx++))
    {
	if (client->send_vcc.vcc_state == LANE_VCC_INITIALIZED)
	    continue;
        atm_getvc_counts(hwidb, client->send_vcc.vcd, &ipkts, &opkts);
        atm_printnsap(client->send_vcc.calling_addr.addr, buf);
        printf("%-5u %-4u %-8u %s\n",
               client->lecid, client->send_vcc.vcd, ipkts, buf);
    }
    free(lane_info);
    free(lsv_info);
    automore_header(NULL);
}


/*
 * lane_show_one_server
 *
 * Show Server information associated with the specified lane_info.
 *
 * Always show "summary" information, include per client info if
 * brief == FALSE.
 */
void
lane_show_one_server (
    lane_info_t	*lane_info,
    boolean	 brief)
{
    lsv_info_t          *lsv_info;
    lsv_client_t        *client;
    lsv_reg_table_t     *reg, *temp;
    hwidbtype           *hwidb;
    boolean              first_reg;
    int                  idx;
    unsigned             ipkts, opkts;
    char                 buf[ATM_MAX_NSAP_STRING];

    /* Don't show it if it's not there.
     */
    if (lane_info == NULL)
        return;
    if (! lane_info->type_from_les)
		return;
    lsv_info = lane_info->lsv_info;
    if (lsv_info == NULL)
		return;

    /* Show "summary" information for this server
     */
    mem_lock(lane_info);
    mem_lock(lsv_info);
    hwidb = lane_info->swidb->hwptr;
    printf("\nLE Server %s  ELAN name: %s  Admin: %s  State: %s\n",
           lane_info->swidb->namestring, lane_info->name,
           lsv_pr_admin_state(lsv_info->admin_state),
           lsv_pr_server_state(lsv_info->server_state));
    lsv_show_sleep_time(lsv_info);
    printf("type: %s         Max Frame Size: %s",
           lane_pr_lan_type(lane_info->type),
           lane_pr_max_frame_size(lane_info->max_data_frame));
 
    if (lane_info->type == LANE_TYPE_802_5)
       {
       printf("      Segment ID: ");
       if (lsv_info->seg_id == 0)
          printf("not set");
       else printf("%d", lsv_info->seg_id);
       }
    printf("\n");

    lsv_nsap_or_template(lsv_info->server_state, &lsv_info->lsv_nsap,
			 &lane_info->lsv_nsap, buf);
    printf("ATM address: %s\n", buf);

    /* Show config server information */
    printf("LECS used: %Ch", &lsv_info->lsv_lecs_nsap);
    if (lsv_info->lecsAccess.connected)
    {
	printf(" connected, vcd %u",
	    atmSig_api_callId2vcd(lsv_info->lecsAccess.callId));
    }
    else
    {
	printf(" NOT yet connected");
    }
    printf("\n");

    if (lsv_info->distribute_vcc.vcd != 0) {
        atm_getvc_counts(hwidb, lsv_info->distribute_vcc.vcd,
                              &ipkts, &opkts);
        printf("control distribute: vcd %u, %u members, %u packets\n",
               lsv_info->distribute_vcc.vcd,
               lsv_info->distribute_vcc.n_members, opkts);
    }

    /* if "brief", or no clients on this server, done.
     */
    if (brief || (lsv_info->client_table == NULL)) {
        free(lsv_info);
        free(lane_info);
        return;
    }

    /*
     * Show info for each client.  We want the display to be in lecid
     * order, and to include all registered mac/nsap's for each client.
     * The registrations are in a separate chain, not ordered by lecid,
     * so we need a nested loop structure, with an outer loop over the
     * clients, and an inner loop searching for all registrations for
     * each client.
     *
     * If there are no registrations for a client, we display the
     * client's control direct calling party addr as its nsap, with
     * no mac information.
     */
    automore_header("\
proxy/ (ST: Init, Conn, Waiting, Adding, Joined, Operational, Reject, Term)\n\
lecid ST vcd    pkts Hardware Addr  ATM Address\n");
    idx = 1;
    for (client = lsv_info->client_table; client;
         client = queuepick(&lsv_info->client_table, idx++)) {
	 mem_lock(client);
        /* 
         * get current packet count for control direct vcc, display
         * lecid, control direct vcd, state, & packet count.  No newline
         * here so first mac/nsap registration is shown on same line.
         */
        atm_getvc_counts(hwidb, client->direct_vcc.vcd, &ipkts, &opkts);
        printf("%-4u%s %s %-4u%-8u", client->lecid,
               client->is_proxy ? "P" : " ",
	       lsv_pr_client_state(client->client_state),
	       client->direct_vcc.vcd, ipkts);

        /* look for registrations for this client.
	 */
        first_reg = TRUE;
        reg = lsv_info->reg_table;
        while (reg) {
	    mem_lock(reg);
            if (reg->client == client) {
                /*
                 * if not 1st registration for this client, need blanks
                 * so mac/nsap line up with 1st line.
                 */                  
                if (!first_reg)
                    printf("                    ");
                first_reg = FALSE;

                /* show mac (if it's valid) & nsap for this registration
		 */
                switch(reg->lan_dest.tag) {
                  case(LANE_TAG_MAC_ADDRESS):
                     printf(" %e", reg->lan_dest.addr.mac);
                     break;
                  case(LANE_TAG_ROUTE_DESC):
                     printf(" %d.%d        ", reg->lan_dest.addr.rd.val >> 4,
                             reg->lan_dest.addr.rd.val & 0x0f);
                     break;
                  default:
                    printf("               ");
                  }
                atm_printnsap(reg->nsap.addr, buf);
                printf(" %s\n", buf);
            }  /* end if (reg->client == client) */
            temp = reg->next;
            free(reg);
            reg = temp;
        }  /* end inner loop over registrations */

        /* If we found no registrations for this client, show
         * blank mac & control direct calling party nsap
         */
        if (first_reg) {
            atm_printnsap(client->direct_vcc.calling_addr.addr, buf);
            printf("                %s\n", buf);
        }
	free(client);
    } /* end outer loop over clients */
    
    free(lsv_info);
    free(lane_info);
    automore_header(NULL);
}


/************************************************************************
 * LES/BUS {mac address, ATM address, lecid} registration routines
 ************************************************************************/


/*
 * lsv_primary_nsap_conflict - check for nsap conflict with some other client
 *	returns TRUE if conflict
 */
boolean
lsv_primary_nsap_conflict (
    lsv_info_t		*lsv_info,	/* which LES */
    lsv_client_t	*asking_client,	/* which client wants to own NSAP */
    hwaddrtype		*nsap)		/* the NSAP */
{
    lsv_client_t	*client;

    client = lsv_info->client_table;
    while (client != NULL) {
	if (client		 != asking_client	       &&
	    client->client_state != LSV_CLIENT_INITIALIZED     &&
	    lane_addr_equal(nsap, &client->direct_vcc.calling_addr))
	{
	    return(TRUE);
	}
	client = client->next;
    }
    return(FALSE);
}


/*
 * Register an {LECID, MAC address or route descriptor, NSAP} triplet
 * Return LANE_STATUS_xxx code
 */
ushort
lsv_register_lan_dest_nsap (
    lsv_info_t	 *lsv_info,	/* Which LES to register it to */
    lsv_client_t *client,	/* which client */
    lane_dest_t	 *lan_dest,	/* LAN destination to register */
    hwaddrtype	 *nsap,		/* NSAP to register */
    boolean	  manual_entry)	/* TRUE = SNMP or CLI.  FALSE = reg request */
{
    int			 cmp;		/* results of comparison */
    lsv_reg_table_t	*reg_table;
    lsv_reg_table_t	*last_reg_table;
    lsv_reg_table_t	*insert_reg_table;
    lane_info_t		*lane_info;

    /* sanity check
     */
    if (nsap->type       != STATION_ATMNSAP    ||
	nsap->length     != STATIONLEN_ATMNSAP   )
    {
	lane_router_crash("lsv_register_mac_nsap");
	return(LANE_STATUS_INSUFF_RESOURCE);
    }

    last_reg_table = NULL;
    lane_info	   = lsv_info->lane_info;
    reg_table      = lsv_info->reg_table;

    /* 
     * scan table looking for LAN destination while 
     * checking that NSAP is not a duplicate.
     */
    while (reg_table != NULL) 
    {
       /* mac addr already registered ? */
       cmp = memcmp(lan_dest, &reg_table->lan_dest, 
  		    sizeof(lane_dest_t));
	if (cmp == 0)
	{
           /*
  	    ** If already registered, NSAP must match, too 
  	    ** Unless, this is a mac that matches an HSRP 
  	    ** mac address.  In this case, duplicates are
  	    ** allowed
  	    */
 	    if (lane_addr_equal(nsap, &reg_table->nsap))
  	    {
        	switch(lan_dest->tag) {
		  case LANE_TAG_MAC_ADDRESS:
		     LES_DBG(lane_info->swidb->namestring,
			  "elan %s client %d Register MAC address %e",
			  lane_info->name, client->lecid,
			  lan_dest->addr.mac);
		     break;
		  case LANE_TAG_ROUTE_DESC:
		     LES_DBG(lane_info->swidb->namestring,
			 "elan %s client %d Register Route Descriptor %d.%d",
			 lane_info->name, client->lecid,
			 lan_dest->addr.rd.val >> 4, 
			 lan_dest->addr.rd.val & 0x0f);
		     break;
		  default:
		     return(LANE_STATUS_INVALID_REQ_PARMS);
		}
		return(LANE_STATUS_SUCCESS);
            }
	    return(LANE_STATUS_DUP_LAN_DEST);
	} 
	else if (cmp < 0) {
	    /* Stop searching when we're past the right entry */
	    break;
	}

	/* Check for duplicated NSAP while we're here */
	if (reg_table->client != client	 &&
	    lane_addr_equal(nsap, &reg_table->nsap))
	{
	    if (lane_lsv_debug) {
			char atmstr[ATM_MAX_NSAP_STRING];
			atm_printnsap(nsap->addr, atmstr);
			buginf(
		"\nLES %s elan %s client %d Register duplicate ATM address %s",
			lane_info->swidb->namestring,
			lane_info->name, client->lecid, atmstr);
	    }
	    return(LANE_STATUS_DUP_ATM_ADDR);
	}
	/* advance to next entry */
	last_reg_table = reg_table;
	reg_table      = reg_table->next;
   }

    /* scan the rest of the registration table for duplicate NSAPs
     */
    insert_reg_table = reg_table;
    while (reg_table) {
		if (reg_table->client != client  &&
	    	lane_addr_equal(nsap, &reg_table->nsap))
		{
	    	if (lane_lsv_debug) {
				char atmstr[ATM_MAX_NSAP_STRING];
				atm_printnsap(nsap->addr, atmstr);
				buginf(
			"\nLES %s elan %s client %d Register duplicate ATM address %s",
				lane_info->swidb->namestring,
				lane_info->name, client->lecid, atmstr);
	    	}
	    	return(LANE_STATUS_DUP_ATM_ADDR);
		}
		reg_table = reg_table->next;
    }

    /* Can't have conflicts with other clients' primary nsaps, either
     */
    if (lsv_primary_nsap_conflict(lsv_info, client, nsap)) {
		if (lane_lsv_debug) {
	    char atmstr[ATM_MAX_NSAP_STRING];
	    atm_printnsap(nsap->addr, atmstr);
	    buginf(
		"\nLES %s elan %s client %d Register duplicate ATM address %s",
		lane_info->swidb->namestring,
		lane_info->name, client->lecid, atmstr);
		}
		return(LANE_STATUS_DUP_ATM_ADDR);
    }

    /* Allocate new registration entry
     */
    reg_table = malloc(sizeof(lsv_reg_table_t));
    if (reg_table == NULL) {
		return(LANE_STATUS_INSUFF_RESOURCE);
    }

    /* Insert new registration entry and report success
     */
    reg_table->client	     = client;
    reg_table->nsap	     = *nsap;
    reg_table->lan_dest	     = *lan_dest;
    reg_table->manual_entry |= manual_entry;	/* request can duplicate, but
						 *  not override, manual data*/
    if (last_reg_table == NULL)
		lsv_info->reg_table  = reg_table;
    else
		last_reg_table->next = reg_table;
    reg_table->next = insert_reg_table;
    if (lane_lsv_debug) {
		char atmstr[ATM_MAX_NSAP_STRING];
		atm_printnsap(nsap->addr, atmstr);
      switch(lan_dest->tag) {
      case(LANE_TAG_MAC_ADDRESS):
		  buginf("\nLES %s elan %s client %d Registered %e, %s",
		    lane_info->swidb->namestring, lane_info->name, client->lecid,
		    lan_dest->addr.mac, atmstr);
		break;
      case(LANE_TAG_ROUTE_DESC):
         buginf("\nLES %s elan %s client %d Registered %d, %s",
            lane_info->swidb->namestring, lane_info->name, client->lecid,
            lan_dest->addr.rd.val, atmstr);
         break;
       default:
         return(LANE_STATUS_INVALID_REQ_PARMS);
       }
    }
    return(LANE_STATUS_SUCCESS);
}


/*
 * Unregister an {LECID, LAN destinatino, NSAP} triplet
 * Return LANE_STATUS_xxx code
 */
static ushort
lsv_unregister_lan_dest_nsap (
    lsv_info_t	 *lsv_info,	/* Which LES to remove it from */
    lsv_client_t *client,	/* Which client */
    lane_dest_t	 *lan_dest,	/* LAN destination to remove */
    hwaddrtype	 *nsap,		/* NSAP to remove */
    boolean	  manual_entry)	/* TRUE = SNMP or CLI.  FALSE = unreg request */
{
    int			 cmp;
    lsv_reg_table_t	*reg_table;
    lsv_reg_table_t	*last_reg_table;
    lane_info_t		*lane_info;

    /* sanity check
     */
    if (nsap->type       != STATION_ATMNSAP	    ||
	nsap->length     != STATIONLEN_ATMNSAP	       )
    {
		lsv_crash(lsv_info, "lsv_unregister_lan_dest_nsap");
		return(LANE_STATUS_INSUFF_RESOURCE);
    }

    /* scan table looking for MAC address
     */
    last_reg_table = NULL;
    lane_info      = lsv_info->lane_info;
    reg_table      = lsv_info->reg_table;
    while (reg_table != NULL) {
	/*
	 * match MAC address?
	 */
	cmp = memcmp(&lan_dest, &reg_table->lan_dest, sizeof(lane_dest_t));
	if (cmp == 0) {
	    break;
	} else if (cmp < 0) {
	    /*
	     * Fake success if MAC address isn't registered
	     */
	    if (lane_lsv_debug)
          switch(lan_dest->tag) {
            case(LANE_TAG_MAC_ADDRESS):
			     buginf("\nLES %s elan %s client %d unnecessary unregister %e",
			       lane_info->swidb->namestring, lane_info->name,
			       client->lecid, lan_dest->addr.mac);
              break;
            case(LANE_TAG_ROUTE_DESC):
              buginf("\nLES %s elan %s client %d unnecessary unregister %d",
                lane_info->swidb->namestring, lane_info->name,
                client->lecid, lan_dest->addr.rd.val);
              break;
            default:
              return(LANE_STATUS_INVALID_REQ_PARMS);
            }
	    return(LANE_STATUS_SUCCESS);
	}
	/*
	 * advance to next entry
	 */
	last_reg_table = reg_table;
	reg_table      = reg_table->next;
    }

    /* If NSAP doesn't match, fake success
     */
    if (! lane_addr_equal(nsap, &reg_table->nsap)) {
		if (lane_lsv_debug)
        switch(lan_dest->tag) {
        case(LANE_TAG_MAC_ADDRESS):
	         buginf("\nLES %s elan %s client %d incorrect unregister %e",
		        lane_info->swidb->namestring, lane_info->name, client->lecid,
		        lan_dest->addr.mac);
            break;
        case(LANE_TAG_ROUTE_DESC):
            buginf("\nLES %s elan %s client %d incorrect unregister %d",
             lane_info->swidb->namestring, lane_info->name, client->lecid,
             lan_dest->addr.rd.val);
           break;
        default:
           return(LANE_STATUS_INVALID_REQ_PARMS);
        }

	return(LANE_STATUS_SUCCESS);
    }
    /*
     * If not the right client, fake success
     */
    if (client != reg_table->client) {
	  if (lane_lsv_debug)
      switch(lan_dest->tag) {
        case(LANE_TAG_MAC_ADDRESS):
          buginf("\nLES %s elan %s client %d wrong client unregister %e",
            lane_info->swidb->namestring, lane_info->name,
            client->lecid, lan_dest->addr.mac);
          break;
        case(LANE_TAG_ROUTE_DESC):
          buginf("\nLES %s elan %s client %d wrong client unregister %d",
            lane_info->swidb->namestring, lane_info->name,
            client->lecid, lan_dest->addr.rd.val);
          break;
        default:
           return(LANE_STATUS_INVALID_REQ_PARMS);
        }

	return(LANE_STATUS_SUCCESS);
    }
    /*
     * Can't override a manual entry with a dynamic one
     */
    if (reg_table->manual_entry  &&  ! manual_entry) {
	  if (lane_lsv_debug)
       switch(lan_dest->tag) {
        case(LANE_TAG_MAC_ADDRESS):
          buginf(
            "\nLES %s elan %s client %d attempt to override manual entry %e",
            lane_info->swidb->namestring, lane_info->name, client->lecid,
            lan_dest->addr.mac);
          break;
        case(LANE_TAG_ROUTE_DESC):
          buginf(
            "\nLES %s elan %s client %d attempt to override manual entry %d",
            lane_info->swidb->namestring, lane_info->name, client->lecid,
            lan_dest->addr.rd.val);
          break;
        default:
           return(LANE_STATUS_INVALID_REQ_PARMS);
        }
	  return(LANE_STATUS_SUCCESS);
    }

    /* Remove registration entry from the list
     */
    if (last_reg_table == NULL)
	lsv_info->reg_table  = reg_table->next;
    else
	last_reg_table->next = reg_table->next;
    /*
     * de-allocate registration entry and return
     */
    reg_table->next = NULL; /* protect show command */
    free(reg_table);
    if (lane_lsv_debug)
      switch(lan_dest->tag) {
        case(LANE_TAG_MAC_ADDRESS):
           buginf("\nLES %s elan %s client %d unregistered %e",
             lane_info->swidb->namestring, lane_info->name,
             client->lecid, lan_dest->addr.mac);
          break;
        case(LANE_TAG_ROUTE_DESC):
           buginf("\nLES %s elan %s client %d unregistered %d",
             lane_info->swidb->namestring, lane_info->name,
             client->lecid, lan_dest->addr.rd.val);
          break;
        default:
           return(LANE_STATUS_INVALID_REQ_PARMS);
        }
    return(LANE_STATUS_SUCCESS);
}


/*
 * Clear all registrations for a given client.
 */
void
lsv_unregister_all_dest_nsap (
    lsv_info_t		*lsv_info,	/* Which LES to remove it from */
    lsv_client_t	*client)
{
    lsv_reg_table_t	*reg_table;
    lsv_reg_table_t    **prev_reg_table;

    /* Scan the registration table, removing all this client's registrations.
     */
    prev_reg_table = &lsv_info->reg_table;
    reg_table	   = lsv_info->reg_table;
    while (reg_table != NULL) {
	/*
	 * Remove entry if right client.
	 */
	if (reg_table->client == client) {
	    if (lane_lsv_debug)
          switch(reg_table->lan_dest.tag) {
            case(LANE_TAG_MAC_ADDRESS):
               buginf("\nLES %s elan %s client %d unregistered %e",
                 lsv_info->lane_info->swidb->namestring,
                 lsv_info->lane_info->name,
                 client->lecid, reg_table->lan_dest.addr.mac);
               break;
            case(LANE_TAG_ROUTE_DESC):
               buginf("\nLES %s elan %s client %d unregistered %d.%d",
                 lsv_info->lane_info->swidb->namestring,
                 lsv_info->lane_info->name,
                 client->lecid, reg_table->lan_dest.addr.rd.val >> 4,
                 reg_table->lan_dest.addr.rd.val & 0x0f);
               break;
            default:
               crashdump(0);
               return;
            }
  
	    *prev_reg_table = reg_table->next;
	    reg_table->next = NULL; /* protect show command */
	    free(reg_table);
	} else {
	    prev_reg_table = &reg_table->next;
	}
	/*
	 * advance to next entry
	 */
	reg_table = *prev_reg_table;
    }
}


/************************************************************************
 * LES/BUS general utilities
 ************************************************************************/


/*
 * lsv_lecid_to_client
 *
 * convert LECID into lsv_client_t* Return NULL if not found.
 */
static lsv_client_t *
lsv_lecid_to_client (
    lsv_info_t	*lsv_info,	/* Which server we're looking at */
    ushort	 lecid)		/* Which LECID we want */
{
    lsv_client_t *client;

    client = lsv_info->client_table;
    while (client != NULL) {
	if (client->lecid == lecid)
	    return(client);
	if (client->lecid > lecid)
	    return(NULL);
	client = client->next;
    }
    return(NULL);
}


/*
 * lsv_check_lan_dest - inspect a LAN destination for validity
 *		return TRUE if OK, FALSE if bad.
 */
boolean
lsv_check_lan_dest (
    lane_dest_t	*lan_dest)
{
    return(lan_dest->tag == LANE_TAG_MAC_ADDRESS  ||
	   lan_dest->tag == LANE_TAG_ROUTE_DESC      );
}


/*
 * lsv_pack_nsap - put an ATM NSAP address into a hwaddrtype
 */
void
lsv_pack_nsap (
    uchar	in[STATIONLEN_ATMNSAP],
    hwaddrtype *out)
{
    if (in[0] == 0) {
	memset(out, 0, sizeof(hwaddrtype));
	return;
    }
    out->type		= STATION_ATMNSAP;
    out->length		= STATIONLEN_ATMNSAP;
    out->target_type	= TARGET_UNICAST;
    bcopy(in, out->addr, STATIONLEN_ATMNSAP);
}


/*
 * lsv_find_lsv_by_name
 *
 * Used to find a lane server by name and interface.
 */
static idbtype* lsv_find_by_name_on_interface (
    uchar	*elan_name,
    uchar	elan_name_length,
    hwidbtype	*hwidb)
{ 
    idbtype	*swidb;
    lane_info_t	*lane_info;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	lane_info = swidb->lane_info;
	if (!lane_info) continue;

	if ((lane_info->name_size == elan_name_length) &&
	    (!memcmp(elan_name, lane_info->name,
		     elan_name_length)) && 
	    (swidb->hwptr == hwidb))
	    return swidb;
    }
    return NULL;
}

/*
 * lane_masked_addr_equal - compare masked NSAPs for equality
 */
static boolean
lane_masked_addr_equal (
    MaskedAddress	*addr1,
    MaskedAddress	*addr2)
{
    return(
	addr1->type   == addr2->type				      &&
	addr1->length == addr2->length				      &&
	0 == bcmp(addr1->dataBytes, addr2->dataBytes, addr1->length)  &&
	0 == bcmp(addr1->maskBytes, addr2->maskBytes, addr1->length)     );
}


/*
** either a mac address or a prefix change event.
** analyse and decide whether it affects the les/bus
** and if so, recycle.
*/
void lesbus_addr_recycle (hwidbtype *hwidb,
    lane_addr_recycle_reason reason)
{
    lsv_info_t *lsv_info;
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (!swidb->lane_info) continue;
	lsv_info = swidb->lane_info->lsv_info;
	if (!lsv_info) continue;

	/* if down already do not bother */
	if ((lsv_info->server_state == LSV_LESBUS_INITIALIZED) ||
	    (lsv_info->server_state == LSV_LESBUS_TERMINATING))
	{
	    LES_DBG(swidb->namestring,
		"lesbus going down; nsaps not affected");
	    continue;
	}
	
	/*
	** if this is a prefix add event .. AND we are awaiting to
	** be kicked by the first prefix event, then recycle.
	*/
	if ((lsv_info->server_state == LSV_LESBUS_WAITING_ILMI ||
	     lsv_info->server_state == LSV_LESBUS_PROVISIONAL) &&
	    (reason == PREFIX_ADD_EVENT))
	{
	    LES_DBG(swidb->namestring,
		"first prefix add event, kicked into life");
	    lsv_start_lesbus(lsv_info);
	    continue;
	}

	/* if we are here, recycle */
	lsv_kill_lesbus(lsv_info);
    }
}


/************************************************************************
 * LES/BUS operations: handle incoming control frames
 ************************************************************************/


/*
 * lsv_answer_request - return an answer (if possible) to requestor in pak,
 *    else discard pak.  Request must be valid except for OP-CODE.
 *    Response is sent on control distribute if client is joined or
 *    operational, else sent on client's control direct.
 */
void
lsv_answer_request (
    lsv_client_t        *client,        /* client being sent to */
    paktype	        *pak,		/* pak with bad request */
    ulong	         status)        /* status code to return */
{
    lane_ctl_hdr_t	*head;
    lsv_info_t		*lsv_info;
    ushort		 opcode;
    vcd_t                vcd;
    ulong		 bad_count;
    lane_info_t		*lane_info;

    lsv_info  = client->lsv_info;
    lane_info = lsv_info->lane_info;
    head      = (lane_ctl_hdr_t *)pak->datagramstart;
    /*
     * this must be a request to answer it
     */
    opcode = head->opcode;
    if ((opcode & LANE_OPCODE_REQ_MASK) != LANE_OPCODE_REQUEST) {
	   if (lane_lsv_debug)
	     buginf("\nLES %s elan %s client %d cannot answer invalid op-code",
		      lane_info->swidb->namestring, lane_info->name, client->lecid);
	   datagram_done(pak);
	   return;
    }
    opcode &= LANE_OPCODE_OP_MASK;

    /* Send on control direct if join, else send on control direct.
     */
    if (opcode == (LANE_OPCODE_OP_MASK & LANE_JOIN_REQ))
        vcd = client->direct_vcc.vcd;
    else
        vcd = lsv_info->distribute_vcc.vcd;
    /*
     * count as good or bad
     */
    if (status)
	bad_count = 1;
    else
	bad_count = 0;

    /* Encapsulate packet and send
     */
    head->opcode   = opcode | LANE_OPCODE_RESPONSE;
    head->status   = status;
    if (pak->if_output == NULL)
		pak->if_output = pak->if_input;
    if (lane_control_encap(pak, vcd)) {
		if (lane_lsv_debug)
	     buginf("\nLES %s elan %s client %d answered %s request",
		       lane_info->swidb->namestring, lane_info->name,
		       client->lecid, lane_pr_opcode(head));

	/* this must be a valid op-code to be answered
	 */
	switch(opcode) {
	case (LANE_OPCODE_OP_MASK & LANE_JOIN_REQ):
	    lsv_info->lsvs.lsvOutJoinResps += 1;
	    if (status) {
			lsv_info->lsvs.lsvOutJoinFails     += 1;
			lsv_info->lsvs.lsvLastJoinFailCause = status;
			lsv_info->lsvs.lsvLastJoinFailLec   =
				client->direct_vcc.calling_addr;
	    }
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_REG_REQ):
	    lsv_info->lsvs.lsvOutRegResps += 1;
	    if (status) {
			lsv_info->lsvs.lsvOutRegFails     += 1;
			lsv_info->lsvs.lsvLastRegFailCause = status;
			lsv_info->lsvs.lsvLastRegFailLec   =
				client->direct_vcc.calling_addr;
	    }
	    break;

	case (LANE_OPCODE_OP_MASK & LANE_UNREG_REQ):
	    lsv_info->lsvs.lsvOutUnregResps += 1;
	    lsv_info->lsvs.lsvOutUnregFails += bad_count;
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_ARP_REQ):
	    lsv_info->lsvs.lsvOutArpResps   += 1;
	    lsv_info->lsvs.lsvOutOtherFails += bad_count;
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_FLUSH_REQ):
	    lsv_info->lsvs.lsvOutFlushResps += 1;
	    lsv_info->lsvs.lsvOutOtherFails += bad_count;
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_NARP_REQ):
	    lsv_info->lsvs.lsvOutNarpReqs   += 1;
	    lsv_info->lsvs.lsvOutOtherFails += bad_count;
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_TOPO_CHANGE):
	    lsv_info->lsvs.lsvOutTopReqs    += 1;
	    lsv_info->lsvs.lsvOutOtherFails += bad_count;
	    break;

	default:
	    if (lane_lsv_debug)
		buginf(
		    "\nLES %s elan %s client %d cannot answer invalid op-code",
		    lane_info->swidb->namestring,
		    lane_info->name, client->lecid);
	    datagram_done(pak);
	    return;
	}
	datagram_out(pak);
    } else {
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s client %d encaps failure",
		lane_info->swidb->namestring, lane_info->name, client->lecid);
	datagram_done(pak);
    }
}


/*
 * lsv_client_request_check - get client info pointer from control packet.
 *	LECID must be present and be correct.  Client must be operational.
 *	Ignore packet if not.  (We could return an error if we were
 *	returning stuff on control directs.  For now, we're returning all
 *	on the control distribute, so we can't return anything if the LECID
 *	is bad.)
 */
static lsv_client_t *
lsv_client_request_check (
    lsv_info_t	*lsv_info,
    paktype	*pak)
{
    lsv_client_t	*client;
    lane_ctl_hdr_t	*head;
    lane_info_t		*lane_info;

    /*
     * Find the client.  Discard the packet if client not up or wrong LECID.
     */
    if (lsv_info == NULL) return NULL;
    if (pak == NULL) return NULL;
    lane_info = lsv_info->lane_info;
    if (lane_info == NULL) return NULL;
    head = (lane_ctl_hdr_t*) pak->datagramstart;
    client = lsv_lecid_to_client(lsv_info, head->requestor_lecid);
    if (client == NULL)
    {
	LES_DBG(lane_info->swidb->namestring,
		"elan %s client %d not joined, request ignored",
		lane_info->name,
		head->requestor_lecid);
	datagram_done(pak);
	return NULL;
    }

    if ((client->client_state != LSV_CLIENT_JOINED  &&
	 client->client_state != LSV_CLIENT_OPERATIONAL) ||
	client->direct_vcc.vcd != pak->vcd)
    {
	LES_DBG(lane_info->swidb->namestring,
		"elan %s client %d not joined, request ignored",
		lane_info->name, client->lecid);
	datagram_done(pak);
	return(NULL);
    }
    return(client);
}


/*
 * lsv_process_register
 *
 * Process register request frames for the LE Server
 */
static void
lsv_process_register (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    lane_reg_pdu_t	*regreq;
    lsv_client_t	*client;
    hwaddrtype		 nsap;
    lane_info_t		*lane_info;

    client = lsv_client_request_check(lsv_info, pak);
    if (client == NULL)
		return;
    regreq = (lane_reg_pdu_t *)pak->datagramstart;

    /* for TR LANE, if the segment number in a route descriptor is
     * the same as the emulated ring number, reject the registration
     */
    if ((regreq->src_lan_dest.tag == LANE_TAG_ROUTE_DESC) &&
        ((regreq->src_lan_dest.addr.rd.val >> 4) == lsv_info->seg_id)) {
       lsv_answer_request(client, pak, LANE_STATUS_INVALID_LAN_DEST);
       return;
    }

    /*
     * Try to register the address
     */
    if (! lsv_check_lan_dest(&regreq->src_lan_dest)) {
		lsv_answer_request(client, pak, LANE_STATUS_INVALID_LAN_DEST);
		return;
    }
    lsv_pack_nsap(regreq->src_atm_addr, &nsap);
    if (! lane_addr_valid(&nsap)) {
		lsv_answer_request(client, pak, LANE_STATUS_INVALID_ATM_ADDR);
		return;
    }
    regreq->hdr.status = lsv_register_lan_dest_nsap(lsv_info, client,
					       &regreq->src_lan_dest, &nsap,
					       FALSE);
    /*
     * convert request to response and return to sender
     */
    regreq->hdr.opcode = LANE_REG_RSP;
    regreq->hdr.flags  = 0;
    if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
		datagram_out(pak);
		return;
    }
    if (lane_lsv_debug) {
	lane_info = client->lsv_info->lane_info;
	buginf("\nLES %s elan %s client %d vcd %d encaps failure",
	    lane_info->swidb->namestring, lane_info->name, 
	    client->lecid, lsv_info->distribute_vcc.vcd);
    }
    datagram_done(pak);

}


/*
 * lsv_process_unregister
 *
 * Process unregister request frames for the LE Server
 */
static void
lsv_process_unregister (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    lane_reg_pdu_t	*regreq;
    lsv_client_t	*client;
    hwaddrtype		 nsap;
    lane_info_t		*lane_info;

    client = lsv_client_request_check(lsv_info, pak);
    if (client == NULL)
		return;
    regreq = (lane_reg_pdu_t *)pak->datagramstart;
    /*
     * Try to unregister the address
     */
    if (! lsv_check_lan_dest(&regreq->src_lan_dest)) {
		lsv_answer_request(client, pak, LANE_STATUS_INVALID_LAN_DEST);
		return;
    }
    lsv_pack_nsap(regreq->src_atm_addr, &nsap);
    if (! lane_addr_valid(&nsap)) {
		lsv_answer_request(client, pak, LANE_STATUS_INVALID_ATM_ADDR);
		return;
    }
    regreq->hdr.status = lsv_unregister_lan_dest_nsap(lsv_info, client,
						 &regreq->src_lan_dest, &nsap,
						 FALSE);

    /* convert request to response and return to sender
     */
    regreq->hdr.opcode = LANE_UNREG_RSP;
    regreq->hdr.flags  = 0;
    if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
		datagram_out(pak);
    } else {
	if (lane_lsv_debug) {
	lane_info = client->lsv_info->lane_info;
	buginf("\nLES %s elan %s client %d vcd %d encaps failure",
			lane_info->swidb->namestring,
			lane_info->name, client->lecid,
			lsv_info->distribute_vcc.vcd);
	}
	datagram_done(pak);
    }
}


/*
 * lsv_process_arp
 *
 * Process le-arp request frames for the LE Server
 */
static void
lsv_process_arp (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    lane_arp_pdu_t	*arp;
    lane_info_t		*lane_info;

    /*
     * legal packet?
     */
    if (NULL == lsv_client_request_check(lsv_info, pak))
		return;

    /* If this request is for the BUS, convert it to a reply.  Either
     * way, send it out on the control distribute VCC.
     */
    lane_info = lsv_info->lane_info;
    arp       = (lane_arp_pdu_t *)pak->datagramstart;
    if (arp->target_dest.tag == LANE_TAG_MAC_ADDRESS  &&
	0  ==  bcmp(arp->target_dest.addr.mac, "\377\377\377\377\377\377\377", 
                    IEEEBYTES))
    {
	arp->hdr.opcode = LANE_ARP_RSP;
	arp->hdr.flags  = 0;
	arp->hdr.status = LANE_STATUS_SUCCESS;
	bcopy(lsv_info->bus_nsap.addr, arp->target_atm_addr,
	      STATIONLEN_ATMNSAP);
	if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s vcd %d le-arp resp sent",
			lane_info->swidb->namestring,
			lane_info->name, 
			lsv_info->distribute_vcc.vcd);
	    datagram_out(pak);
	    return;
	} else {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s vcd %d encaps failure",
			lane_info->swidb->namestring,
			lane_info->name, 
			lsv_info->distribute_vcc.vcd);
	    datagram_done(pak);
	}
    } else {
	/* this should have been handled at fast level */
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s le-arp req bad packet",
		lane_info->swidb->namestring, 
		lane_info->name);
	datagram_done(pak);
    }
}


/*
 * lsv_process_flush
 *
 * Process flush request frames for the LE Server
 */
static void
lsv_process_flush (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    lane_flush_pdu_t	*flush;
    lane_info_t		*lane_info;
    /*
     * legal packet?
     */
    if (NULL == lsv_client_request_check(lsv_info, pak))
	return;

    /* If this Flush request is for the LES, convert it to a reply and
     * return it.  Otherwise, discard it.
     */
    lane_info = lsv_info->lane_info;
    flush     = (lane_flush_pdu_t *)pak->datagramstart;
    if (0 == bcmp(flush->target_atm_addr, lsv_info->lsv_nsap.addr,
		  STATIONLEN_ATMNSAP))
    {
	flush->hdr.opcode = LANE_FLUSH_RSP;
	flush->hdr.flags  = 0;
	flush->hdr.status = LANE_STATUS_SUCCESS;
	if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s flush resp sent on vcd %d",
			lane_info->swidb->namestring,
			lane_info->name, 
			lsv_info->distribute_vcc.vcd);
	    datagram_out(pak);
	} else {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s vcd %d encaps failure",
			lane_info->swidb->namestring,
			lane_info->name, 
			lsv_info->distribute_vcc.vcd);
	    datagram_done(pak);
	}
    } else {
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s flush req bad packet",
		lane_info->swidb->namestring, 
		lane_info->name);
	datagram_done(pak);
    }
}


/*
 * lsv_process_topo
 *
 * Process topology change request frames for the LE Server
 */
static void
lsv_process_topo (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    /* legal packet? */

    if (NULL == lsv_client_request_check(lsv_info, pak))
	return;
    /*
     * Relay topo requests to all
     */
    if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
	lsv_info->lsvs.lsvOutTopReqs += 1;
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s topo req distributed on vcd %d",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name, 
		lsv_info->distribute_vcc.vcd);
	datagram_out(pak);
    } else {
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s vcd %d encaps failure",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name, 
		lsv_info->distribute_vcc.vcd);
	datagram_done(pak);
    }
}


/*
 * lsv_process_narp
 *
 * Process negative ARP request frames for the LE Server
 */
static void
lsv_process_narp (
    paktype		*pak,
    lsv_info_t		*lsv_info)
{
    /* legal packet? */

    if (NULL == lsv_client_request_check(lsv_info, pak))
	return;
    /*
     * Relay narp requests to all
     */
    if (lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
	lsv_info->lsvs.lsvOutNarpReqs += 1;
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s narp req distributed on vcd %d",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name, 
		lsv_info->distribute_vcc.vcd);
	datagram_out(pak);
    } else {
	if (lane_lsv_debug)
	    buginf("\nLES %s elan %s vcd %d encaps failure",
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name, 
		lsv_info->distribute_vcc.vcd);
	datagram_done(pak);
    }
}

/*
** process all expired timers
*/
static void lsv_process_timers (void)
{
    mgd_timer *expired;
    lsv_info_t *lsv_info;
    lsv_client_t *client;
    ClientValidation_t *cv;
    int count;

    count = PAK_SWITCHCOUNT;
    expired = mgd_timer_first_expired(&lsv_root_timer);
    while (expired && (--count > 0))
    {
	/* always stop the expired timer */
	mgd_timer_stop(expired);

	/* analyze what to do */
	switch (mgd_timer_type(expired))
	{
	    /* "big" recycle timer */
	    case LSV_TIMER_RECYCLE:
		lsv_info = mgd_timer_context(expired);
		lsv_process_recycle(lsv_info);
		break;

	    /* client did not connect in time */
	    case LSV_TIMER_CLIENT:
		client = mgd_timer_context(expired);
		lsv_client_timed_out(client);
		break;

	    /* we need to reconnect to the master LECS */
	    case LSV_TIMER_LECS_ACCESS:
		lsv_info = mgd_timer_context(expired);
		lsv_start_lecs_connection_procedures(lsv_info);
		break;

	    /* LECS did not respond to a client validation */
	    case LSV_LEC_VALIDATION:
		cv = mgd_timer_context(expired);
		lsv_client_validation_timeout(cv);
		break;

	    /* oops */
	    default:
		LES_ERR("lsv_process_timers: unrecognized "
		    "timer type %d", mgd_timer_type(expired));
	}
	expired = mgd_timer_first_expired(&lsv_root_timer);
    }
}

/*
 * lsv_process_messages
 *
 * Handle incoming LAN emulation control packets destined for the LES.
 * Snarf packets off of the lsv_inputQ and stuff them through the lec
 * state machine.
 */
static void
lsv_process_messages (void)
{
    int			 count;
    paktype		*pak;
    idbtype		*swidb;
    lane_info_t		*lane_info;
    lsv_info_t		*lsv_info;
    lane_ctl_hdr_t	*head;

    count = PAK_SWITCHCOUNT;
    while (--count >= 0) 
    {
	/* Get packet from input queue */
	pak = process_dequeue(lsv_inputQ);
	if (!pak) break;

	/* code sanity checks */
	swidb          = pak->if_input;
	pak->if_output = swidb;
	lane_info      = swidb->lane_info;
	if (lane_info == NULL) 
	{
	    LES_ERR("LES %s vcd %d frame on NULL lane_info",
		swidb->namestring, pak->vcd);
	    datagram_done(pak);
	    continue;
	}
	lsv_info = lane_info->lsv_info;
	if (lsv_info == NULL) 
	{
	    LES_ERR("LES %s vcd %d frame on NULL lsv_info",
		swidb->namestring, pak->vcd);
	    datagram_done(pak);
	    continue;
	}

        /* If this platform does NOT have fast-switching, then we need to
         * process packets at process level that would otherwise have
         * been processed by fast-switching */
        if (reg_invoke_les_check_for_reflect(pak)) {
                /* If a function has been registered AND packet was handled */
                continue;
        }

	/* check validity of header */

	head = (lane_ctl_hdr_t *)pak->datagramstart;
	if (head->marker	!= LANE_MARKER             ||
	    head->protocol	!= LANE_PROTOCOL_ID        ||
	    head->version	!= LANE_PROTOCOL_VERSION   ||
	    pak->datagramsize	< sizeof(lane_join_pdu_t)   )
	{
	    if (lane_lsv_debug)
		buginf("\nLES %s vcd %d invalid control frame",
			swidb->namestring, pak->vcd);
	    datagram_done(pak);
	    continue;
	}

	/*
	** we should NOT get responses EXCEPT a config response.
	** This is coming from the LECS as a result of the 
	** client validation.
	*/
	if (head->opcode != LANE_CONFIG_RSP)
	{
	    if ((head->opcode & LANE_OPCODE_REQ_MASK) == 
		    LANE_OPCODE_RESPONSE) 
	    {
		LES_DBG(swidb->namestring,
		    "vcd %d invalid opcode word", pak->vcd);
		datagram_done(pak);
		continue;
	    }
	}

	/* Handle the packet
	 */
	switch(LANE_OPCODE_OP_MASK & head->opcode) {
	case (LANE_OPCODE_OP_MASK & LANE_JOIN_REQ):
	    lsv_info->lsvs.lsvInJoinReqs += 1;
	    lsv_process_join(pak, lsv_info);
	    break;
	/* response from LECS to our client validation request */
	case (LANE_OPCODE_OP_MASK & LANE_CONFIG_RSP):
	    lsv_info->lsvs.lsvInConfigResps += 1;
	    lsv_process_config_resp(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_REG_REQ):
	    lsv_info->lsvs.lsvInRegReqs += 1;
	    lsv_process_register(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_UNREG_REQ):
	    lsv_info->lsvs.lsvInUnregReqs += 1;
	    lsv_process_unregister(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_ARP_REQ):
	    lsv_info->lsvs.lsvInArpReqs += 1;
	    lsv_process_arp(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_FLUSH_REQ):
	    lsv_info->lsvs.lsvInFlushReqs += 1;
	    lsv_process_flush(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_TOPO_CHANGE):
	    lsv_info->lsvs.lsvInTopReqs += 1;
	    lsv_process_topo(pak, lsv_info);
	    break;
	case (LANE_OPCODE_OP_MASK & LANE_NARP_REQ):
	    lsv_info->lsvs.lsvInNarpReqs += 1;
	    lsv_process_narp(pak, lsv_info);
	    break;
	default:
	    lsv_info->lsvs.lsvUnknownFrames += 1;
	    LES_DBG(swidb->namestring,
		"vcd %d unknown op-code", pak->vcd);
	    datagram_done(pak);
	}
    }
}


/******************************************************************
 * LES/BUS ILMI interface
 ******************************************************************/

/*
 * lsv_reg_event - handle a LES/BUS ATM address registration event
 */
static void
lsv_reg_event (lsv_info_t		*lsv_info,
	       lane_addr_reg_state_t	*reg_state,
	       ilmiClientReq		*event,
	       boolean			 ok_result)	/* ilmi request worked */
{
    uchar nsap [STATIONLEN_ATMNSAP];
    int i;
    char *ifname = lsv_info->lane_info->swidb->namestring;
    char *ename = lsv_info->lane_info->name;

    /* get the nsap from the response */
    for (i=0; i<STATIONLEN_ATMNSAP; i++)
	nsap [i] = (uchar) event->vblist->name->oid_ptr 
	    [i+START_OF_ATM_ADDRESS_INDEX];

    switch (*reg_state) {

    case LANE_ADREG_INITIALIZED:
	errmsg(&msgsym(REGSTATE, LANE), *reg_state);
	return;

    case LANE_ADREG_REGISTERING:

	if (ok_result) {
	    LES_DBG(ifname, 
		"elan %s address %Cj ilmi registration complete",
		ename, nsap);

	    *reg_state = LANE_ADREG_REGISTERED;

	    if (lsv_info->server_state == LSV_LESBUS_TERMINATING) {
		lsv_kill_lesbus(lsv_info);
		return;
	    } 

	    if (lsv_info->bus_reg_state == LANE_ADREG_REGISTERED &&
		lsv_info->lsv_reg_state == LANE_ADREG_REGISTERED &&
		lsv_info->server_state == LSV_LESBUS_PROVISIONAL) {
		    lsv_server_new_state(lsv_info, LSV_LESBUS_OPERATIONAL);
	    }
	} else {
	    errmsg(&msgsym(NOREGILMI, LANE), ifname, "LES/BUS", nsap);

	    *reg_state = LANE_ADREG_INITIALIZED;

	    if (lsv_info->server_state == LSV_LESBUS_TERMINATING) {
		lsv_kill_lesbus(lsv_info);
		return;
	    } 

	    /* GEE */
	    lsv_big_error(lsv_info, 5);
	}
	return;

    case LANE_ADREG_REGISTERED:
	return;

    case LANE_ADREG_UNREGISTERING:

	*reg_state = LANE_ADREG_INITIALIZED;

	if (lsv_info->server_state == LSV_LESBUS_TERMINATING) {
	    LES_DBG(ifname,
		"elan %s address %Cj ilmi unregistration complete",
		ename, nsap);
	    lsv_kill_lesbus(lsv_info);
	}
    }
}


/*
 * lsv_ilmi_callback - Called by ILMI code when something intesting happens
 */
static void
lsv_ilmi_callback (
    ilmiClientReq *notify)
{
    lsv_info_t		*lsv_info;
    lsv_info_t  	*next_lsv_info;
    lane_info_t		*lane_info;
    idbtype		*swidb;
    boolean		error;

    error = (notify->error == ilmiResponseReceived) &&
	    (notify->response == ilmiRespNoErr);

    switch (notify->notify_type) {

    case ilmiResponseNotify:

	lsv_info = lsv_all_lesbus;
	while (lsv_info != NULL) {

            /* grab next pointer in case lsv_info gets deleted */
            next_lsv_info = lsv_info->next;

	    lane_info = lsv_info->lane_info;
	    swidb = lane_info->swidb;

	    /* response to LES */
	    if (lsv_info->lsv_reg_transid == notify->transid) {
		LES_DBG(swidb->namestring,
		    "elan %s ILMI response %d codes %d %d",
		    lane_info->name, notify->transid,
		    notify->error, notify->response);

		/* Received response to LES request */
		lsv_reg_event(lsv_info, &lsv_info->lsv_reg_state, 
		    notify, error);
		return;
	    }

	    /* response to BUS */
	    if (lsv_info->bus_reg_transid == notify->transid) {
		LES_DBG(swidb->namestring,
		    "elan %s ILMI response %d codes %d %d",
		    lane_info->name, notify->transid,
		    notify->error, notify->response);
		
		/* Received response to BUS request */
		lsv_reg_event(lsv_info, &lsv_info->bus_reg_state, 
		    notify, error);
		return;
	    }
	    lsv_info = next_lsv_info;
	}
	break;

    default:
	errmsg(&msgsym(BADILMI, LANE), notify->notify_type);
    }
}


/*
 * lsv_send_reg_unreg - Setup a VarBind with an NSAP for reg/dereg via ILMI
 *			and give it to ILMI for processing.  Returns same
 *			code as ilmi_client_request.
 */
static int
lsv_send_reg_unreg (
    idbtype	*swidb,
    hwaddrtype	*nsap,
    ulong	 transid,
    boolean	 add_not_delete)
{
    ilmiClientReq	 req;
    ulong		*p;
    int			 i;
    /*
     * fill in some basic stuff
     */
    req.client_id = lsv_ilmi_handle;
    req.intf_id   = swidb->hwptr->hw_if_index;
    req.oper_type = ilmiSet;
    req.transid   = transid;

    /* Now copy the atm address into the oid.  Because ILMI makes a copy
     * of the block we send it, we can use one common memory area to construct
     * this request (IlmiSetAddressVarBind) and not worry about multiple users.
     */
    req.vblist = &IlmiSetAddressVarBind;
    p = IlmiSetAddressVarBind.name->oid_ptr + START_OF_ATM_ADDRESS_INDEX;
    for (i = 0; i < STATIONLEN_ATMNSAP; i++)
        *p++ = nsap->addr [i];
    /*
     * add or delete operation
     */
    if (add_not_delete)
	IlmiSetAddressVarBind.value.sl_value = ILMI_ADD_ADDR;
    else
	IlmiSetAddressVarBind.value.sl_value = ILMI_DELETE_ADDR;
    LES_DBG(swidb->namestring,
	"sending %sregistration request for %Cj to ILMI",
	add_not_delete ? "" : "DE-",
	nsap->addr);
    return(ilmi_client_request(&req));
}


/*
 * lsv_unregister_nsap -- Unregister an ATM address with ilmi for the LES/BUS
 *		Returns TRUE if complete or error, FALSE if not finished, yet.
 *		(Note that, if this routine is called while a registration
 *		is in progress, it will wait until the registration finishes
 *		before trying to start the unregister.)
 */
boolean
lsv_unregister_nsap (
    lsv_info_t			*lsv_info,
    hwaddrtype			*nsap,
    lane_addr_reg_state_t	*state,
    ulong			*transid)
{
    switch (*state) {
    case LANE_ADREG_INITIALIZED:
	return(TRUE);
    case LANE_ADREG_REGISTERING:
    case LANE_ADREG_UNREGISTERING:
	return(FALSE);
    case LANE_ADREG_REGISTERED:
	break;
    }

    *state   = LANE_ADREG_UNREGISTERING;
    *transid = ++lane_ilmi_transaction_id;
    if (ILMI_OK !=  lsv_send_reg_unreg(lsv_info->lane_info->swidb, nsap,
				       *transid, FALSE))
    {
	/* pretend unregistration worked, or we'll never kill the LES/BUS
	 */
	*state = LANE_ADREG_INITIALIZED;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * lsv_register_nsap -- Try to rgister an ATM address with ilmi for the LES/BUS
 */
void
lsv_register_nsap (
    lsv_info_t			*lsv_info,
    hwaddrtype			*nsap,
    lane_addr_reg_state_t	*state,
    ulong			*transid)
{
    if (*state != LANE_ADREG_INITIALIZED)
	return;
    *transid = ++lane_ilmi_transaction_id;
    if (ILMI_OK == lsv_send_reg_unreg(lsv_info->lane_info->swidb, nsap,
				      *transid, TRUE))
    {
	*state = LANE_ADREG_REGISTERING;
    } else {
	*state = LANE_ADREG_INITIALIZED;
    }
}


/*
 * lsv_setup_ilmi - Tell ILMI we want a certain type of call-back
 */
static void
lsv_setup_ilmi (
    ilmiClientNotifyType notype)
{
    ilmiClientReq ilmi_req;
 
    ilmi_req.client_id   = lsv_ilmi_handle;
    ilmi_req.notify_type = notype;
    ilmi_req.notifyCB    = (void (*)(void *)) lsv_ilmi_callback;
    if (ILMI_OK != ilmi_register_service(&ilmi_req))
	errmsg(&msgsym(NOILMI, LANE));
}


/*
 * lsv_setup_lsv_ilmi - Setup ILMI calls for LSV
 */
static void
lsv_setup_lsv_ilmi (void)
{
    /* We care only about ilmiResponseNotify */
    lane_ilmi_transaction_id = 0;
    lsv_ilmi_handle = ilmi_register_client("LES");
    lsv_setup_ilmi(ilmiResponseNotify);
}


/************************************************************************
 * LES/BUS configuration command generators
 ************************************************************************/


/*
 * Generate LANE LES-BUS command for write term, etc.
 */
void
lsv_gen_les_bus (
    parseinfo *csb)
{
    idbtype	*swidb;
    lane_info_t	*lane_info;
    lsv_info_t	*lsv_info;

    swidb = csb->interface;
    if (swidb == NULL)
		return;
    lane_info = swidb->lane_info;
    if (lane_info == NULL)
		return;
    lsv_info = lane_info->lsv_info;
    if (lsv_info != NULL) {
		if (lane_info->type_from_les  &&  lane_info->name_from_les) {
        switch(lane_info->type) {
           case(LANE_TYPE_802_3):
              nv_write(TRUE, "%s %s %s", csb->nv_command,
                 "ethernet", lane_info->name);
              break;
           case(LANE_TYPE_802_5):
              /* lane server-bus tokenring segment SEGMENT ELAN_NAME */
              nv_write(TRUE, "%s %s %s %d %s", csb->nv_command,
                 "tokenring", "segment", lsv_info->seg_id,
                 lane_info->name);
              break;
           default:
              break;
        }
		}
    }
}


/*
 * Generate LANE SERVER-ATM-ADDRESS and LANE BUS-ATM-ADDRESS commands.
 */
void
lsv_gen_atm_address(
    parseinfo *csb)
{
    idbtype     	*swidb;
    lane_info_t 	*lane_info;
    MaskedAddress	*addr;
 
    swidb = csb->interface;
    if (swidb == NULL)
        return;
    lane_info = swidb->lane_info;
    if (lane_info == NULL)
        return;

    switch(csb->which) {
    case LANE_CMD_BUS_NSAP:
	addr = &lane_info->bus_nsap;
	break;
    case LANE_CMD_LES_NSAP:
	addr = &lane_info->lsv_nsap;
	break;
    default:
	lane_router_crash("lsv_gen_atm_address");
	return;
    }
    if (addr->type == STATION_ATMNSAP) {
	nv_write(TRUE, "%s %Cw", csb->nv_command, addr);
    }
}


/*
 * Handle "lane les-bus" command from parser.
 */
void
lsv_cmd_les_bus (
    parseinfo *csb)
{
    idbtype	*swidb;
    uchar	*elan_name;
    int		 name_size;
    int		 lan_type = LANE_TYPE_802_3; 
    lsv_info_t	*lsv_info;
    ushort 	 segment_id = 0;


    swidb = csb->interface;
    if (csb->sense) {
	/*
	 * Activate/setup LES.
	 *
	 * Check input parameters
	 */
	switch(GETOBJ(int, 1)) {
	  case LANE_TYPE_ETHERNET:
	    lan_type = LANE_TYPE_802_3;
            segment_id = 0;
	    break;
	  case LANE_TYPE_TOKEN_RING:
            lan_type = LANE_TYPE_802_5;
            segment_id = GETOBJ(int,2);
            if (segment_id == 0)      /* shouldn't happen from cli */
              return;
	    break;
 	 }
	elan_name = (uchar *)GETOBJ(string, 1);
	if (elan_name == NULL)
		name_size = 0;
	else
       	  name_size = strlen((char *)elan_name);
  
  	/* Create and turn on the LES/BUS
  	 */
	lsv_info = lsv_create_lesbus(swidb, lan_type, name_size, elan_name,
			     LANE_CALLER_CLI, segment_id, 0);
	if (lsv_info == NULL)
          return;
 
	if (swidb->subif_state == SUBIF_STATE_UP) {
          lsv_admin_up(lsv_info);
	}
    } else {
	/*
 	 * Destroy the LES/BUS
 	 */
	lsv_admin_down(swidb, LSV_DOWN_NO_LANE);
     }
  }



/****************************************************************
 * LES/BUS CLI and SNMP command interface
 ****************************************************************/

/*
 * Check new ATMSAP for a LES/BUS component.  Zeros *dest_nsap
 * if clearing, copies from *new_nsap if setting.  *dest_nsap
 * unchanged and error logged if inputs are invalid.  LES/BUS
 * recycled and LEC notified if the address changes.
 * Return TRUE if OK, FALSE if error.
 */
boolean
lsv_new_nsap (
    lane_info_t		*lane_info,
    boolean		 set_not_clear,
    MaskedAddress	*new_nsap,	/* new ATM address */
    MaskedAddress	*dest_nsap,	/* where to put it */
    char		*name)		/* for error message */
{
    lsv_info_t	 *lsv_info;
    MaskedAddress zeroad;

    /* get NSAP and check for changes */
    if (! set_not_clear) 
    {
	bzero(&zeroad, sizeof(MaskedAddress));
	zeroad.type   = STATION_ILLEGAL;
	new_nsap = &zeroad;
    }
    lsv_info = lane_info->lsv_info;

    /*
     * Recycle LES-BUS and LEC, if necessary.  Kill the 
     * recycle timer on the LES/BUS; manual changes override 
     * the recycle timer.
     */
    if (! lane_masked_addr_equal(new_nsap, dest_nsap)) 
    {
	/* Address is changing and it matters */
	*dest_nsap = *new_nsap;
	if (lsv_lesbus_alive(lsv_info)) 
	{
	    LES_DBG(lane_info->swidb->namestring,
		"elan %s recycling after ATM address change",
		lane_info->name);
	    mgd_timer_stop(&lsv_info->recycle_timer);
	    lsv_kill_lesbus(lsv_info);
	}
	lec_config_change(lane_info);
    }
    return(TRUE);
}

/****************************************************************
 * LES/BUS CLI command routines
 ****************************************************************/


/*
 * lsv_cmd_les_nsap - Handle "lane les-nsap" command from parser.
 */
void
lsv_cmd_les_nsap (
    parseinfo *csb)
{
    lane_info_t	*lane_info;

    lane_info = lane_init_idb(csb->interface);
    if (lane_info == NULL)
	return;
    lsv_new_nsap(lane_info, csb->sense, GETOBJ(maskedAddr,1),
		 &lane_info->lsv_nsap, "LE Server");
}


/*
 * lsv_cmd_bus_nsap - Handle "lane les-nsap" command from parser.
 */
void
lsv_cmd_bus_nsap (
    parseinfo *csb)
{
    lane_info_t	*lane_info;

    lane_info = lane_init_idb(csb->interface);
    if (lane_info == NULL)
	return;
    lsv_new_nsap(lane_info, csb->sense, GETOBJ(maskedAddr,1),
		 &lane_info->bus_nsap, "BUS");
}


/*
 * lsv_clear_commands - Handle all clear commands from parser
 *
 *	--> Called from parser, so uses printf <--
 */
static void
lsv_clear_commands (
    parseinfo	*csb,
    lsv_info_t	*lsv_info)
{
    lsv_client_t	*client;
    lsv_reg_table_t	*reg;
    hwaddrtype		*mac;
    hwaddrtype		*nsap;
    ushort		 lecid;
    lane_info_t		*lane_info;
    char 		atmstr[ATM_MAX_NSAP_STRING];
    ushort  rd;
    ushort  seg_num;
    ushort  bridge;

    /* Make sure we have a valid lsv_info pointer
     */
    if (lsv_info == NULL				       ||
	(lsv_info->server_state != LSV_LESBUS_PROVISIONAL  &&
	 lsv_info->server_state != LSV_LESBUS_OPERATIONAL     )   )
    {
	printf("%% LES/BUS not found/active on this subinterface\n");
	return;
    }

    /* Figure out which client (or all clients) to clear
     */
    lane_info = lsv_info->lane_info;
    switch(csb->which) {
    case CLEAR_LANE_LES_MAC:
	/*
	 * search registration table for the specified MAC
	 */
	mac = GETOBJ(hwaddr, 2);
	if (mac		== NULL		   ||
	    mac->type	!= STATION_IEEE48  ||
	    mac->length	!= STATIONLEN_IEEE48)
	{
	    printf("%% 48-bit IEEE MAC address required");
	    return;
	}
	reg = lsv_info->reg_table;
	while (reg != NULL) 
	{
	    if (reg->lan_dest.tag == LANE_TAG_MAC_ADDRESS  &&
		! bcmp(reg->lan_dest.addr.mac, mac->addr, STATIONLEN_IEEE48))
	    {
		if (lane_lsv_debug)
		    buginf("\nLES %s elan %s clearing client %d",
			lane_info->swidb->namestring,
			lane_info->name, reg->client->lecid);
		lsv_kill_client(lsv_info, reg->client);
		return;
	    }
	    reg = reg->next;
	}

	/* if here, we couldnt find it */
	printf("%% No such client to clear\n");

	return;

    case CLEAR_LANE_LES_NSAP:
	/*
	 * First, check to see if this NSAP matches a primary ATM address
	 */
	nsap = GETOBJ(hwaddr, 2);
	if (nsap	 == NULL	     ||
	    nsap->type	 != STATION_ATMNSAP  ||
	    nsap->length != STATIONLEN_ATMNSAP)
	{
	    printf("%% %s\n", lsv_20_byte);
	    return;
	}
	client = lsv_find_client_primary(lsv_info, nsap);
	if (client != NULL) {
	    if (lane_lsv_debug)
		buginf("\nLES %s elan %s clearing client %d",
			lane_info->swidb->namestring,
			lane_info->name, client->lecid);
	    lsv_kill_client(lsv_info, client);
	    return;
	}

	/* Search registration table for the specified NSAP
	 */
	reg = lsv_info->reg_table;
	while (reg != NULL) 
	{
	    if (lane_addr_equal(nsap, &reg->nsap)) {
		if (lane_lsv_debug)
		    buginf("\nLES %s elan %s clearing client %d",
			lane_info->swidb->namestring,
			lane_info->name, reg->client->lecid);
		lsv_kill_client(lsv_info, reg->client);
		return;
	    }
	    reg = reg->next;
	}

	/* if here, we couldnt find it */
	atm_printnsap(nsap->addr, atmstr);
	printf("%% No such client to clear\n");

	return;

    case CLEAR_LANE_LES_LECID:
	/*
	 * Search client table for match to specified LECID
	 */
	lecid = GETOBJ(int, 2);
	client = lsv_info->client_table;
	while (client) {
	    if (lecid == client->lecid) {
		if (lane_lsv_debug)
		    buginf("\nLES %s elan %s clearing client %d",
			lane_info->swidb->namestring, lane_info->name, lecid);
		lsv_kill_client(lsv_info, client);
		return;
	    }
	    client = client->next;
	}

	/* if here, we couldnt find it */
	printf("%% No such client to clear\n");

	return;

    case CLEAR_LANE_LES_ALL:
	/*
	 * Clear all clients (by resetting the LES/BUS)
	 */
		if (lane_lsv_debug)
	    	buginf("\nLES %s elan %s restarting for clear all",
				lane_info->swidb->namestring, lane_info->name);
		lsv_kill_lesbus(lsv_info);
		return;

   /* add for tr lane */
    case CLEAR_LANE_LES_RD:
   /*
    * search registration table for the specified RD
    */
   seg_num = GETOBJ(int, 2);
   bridge  = GETOBJ(int, 3);

   /* paranoid - shouldn't get past parser with 0's */
   if ((seg_num == 0) || (bridge == 0))
      {
      printf("%% Segment and Bridge Numbers are required");
      return;
      }
   rd = (seg_num << 4) | (bridge & 0x0F);
   reg = lsv_info->reg_table;
   while (reg != NULL)
     {
     if ((reg->lan_dest.tag == LANE_TAG_ROUTE_DESC)  &&
         (reg->lan_dest.addr.rd.val == rd))
       {
       if (lane_lsv_debug)
          buginf("\nLES %s elan %s clearing client %d",
          lane_info->swidb->namestring,
          lane_info->name, reg->client->lecid);
       lsv_kill_client(lsv_info, reg->client);
       return;
       }
     reg = reg->next;
     }


    } /* end switch */
}


/*
 * lsv_clear_name_commands - Handle "clear lane les name" commands
 *			     from parser.
 */
void
lsv_clear_name_commands (
    parseinfo *csb)
{
    lsv_info_t	*lsv_info;
    char	*name;
    boolean	 found;
    /*
     * get the name specified in the command
     */
    name = GETOBJ(string, 1);
    if (name == NULL  ||  name[0] == '\0') {
	if (lane_lsv_debug)
	    buginf("\nLES Null LES/BUS name given");
	return;
    }

    /* scan all LES/BUSs to find that name
     */
    found    = FALSE;
    lsv_info = lsv_all_lesbus;
    while (lsv_info != NULL) {
	if (! strcmp(lsv_info->lane_info->name, name)) {
	    found = TRUE;
	    lsv_clear_commands(csb, lsv_info);
	}
	lsv_info = lsv_info->next;
    }
    if (! found)
	printf("%% No such LES/BUS to clear\n", name);
}


/*
 * lsv_clear_int_commands - Handle "clear lane les interface" commands
 *			    from parser.
 */
void
lsv_clear_int_commands (
    parseinfo *csb)
{
    idbtype	*swidb;
    lane_info_t	*lane_info;

    /* Find the lsv_info structure
     */
    swidb = GETOBJ(idb, 1);
    if (swidb == NULL) {
	if (lane_lsv_debug)
	    buginf("\nLES Interface not found");
	return;
    }
    lane_info = swidb->lane_info;
    if (lane_info == NULL) {
	printf("%% LANE is NOT configured on this subinterface\n");
	return;
    }
    lsv_clear_commands(csb, lane_info->lsv_info);
}

/*
** mtu change action routine.
** just recycle the whole thing.
*/ 
static void lsv_change_mtu (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    lane_info_t *lane_info;
    lsv_info_t *lsv_info;

    /* not really changed */
    if (delta == 0) return;
 
    if (! swidb) return;
    lane_info = swidb->lane_info;
    if (! lane_info) return;
    lsv_info = lane_info->lsv_info;
    if (! lsv_info) return;

    lane_info->max_data_frame = lane_mtu_to_frame(swidb); 	
 
    /* recycle */
    lsv_kill_lesbus(lsv_info);
}


/************************************************************************
 * LES/BUS packet handling process and initialization code
 ************************************************************************/


/*
 * lsv_enqueue
 *
 * Enqueue a packet for later processing by the LANE Client.
 * If LANE is not running, as determined by the lsv_running
 * variable, then throw all packets away.
 *
 * Always return TRUE so that the registry return can be checked to
 * see if LANE is linked into the system.
 */
boolean lsv_enqueue (paktype *pak)
{
    if (lsv_running) {
	process_enqueue(lsv_inputQ, pak);
    } else {
	pak->flags |= PAK_ABORT;
	netinput_enqueue(pak);
    }

    return(TRUE);
}


/*
 * lsv_input
 *
 * Handles all incoming LAN emulation control messages coming in from
 * the network.
 */
static process
lsv_input (void)
{
    lsv_running = TRUE;

    /* wait for system to initialize */
    process_wait_on_system_config();

    /* set event triggers */
    process_watch_queue(lsv_inputQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&lsv_root_timer, ENABLE);

    /* start any pended bring-ups or bring-downs */
    lsv_start_pended_actions();

    /* loop forever, processing incoming frames */
    for (;;) 
    {
        process_wait_for_event();
	LES_DBG(NULL, "LES LANE Server process awakened");

	/* process expired timers */
	lsv_process_timers();

	/* Check on the message queue */
	lsv_process_messages();
    }
}


/*
 * lsv_start
 *
 * This routine starts the LES/BUS process.  It should only be
 * started if there is a LES/BUS enabled on some interface on the system.
 */
void
lsv_start (void)
{
    int pid;

    if (! lsv_started) {
	pid = process_create(lsv_input, "LES/BUS input", 
			     LARGE_STACK, PRIO_NORMAL);
        if (pid > 0) {
	    lsv_started = TRUE;
	    if (lane_lsv_debug)
	        buginf("\nLES LANE Server process started");
        }
    }
}

/*
 * lsv_init
 *
 * Called from lane_init when the subsystem is initialized.
 */
void lsv_init (void)
{
    /* simple variables */
    lsv_started = FALSE;	/* LES/BUS process not up */
    lsv_running = FALSE;	/* process not past startup */
    lsv_all_lesbus = NULL;	/* Chain of all LES/BUSs */
    lsv_inputQ = create_watched_queue("lsv_inputQ", 0, 0);
    mgd_timer_init_parent(&lsv_root_timer, NULL);

    reg_add_lsv_find_by_name_on_interface(lsv_find_by_name_on_interface,
					  "lsv_find_by_name_on_interface");

    /* register with mtu change */
    reg_add_media_change_mtu(lsv_change_mtu, "lsv_change_mtu");    

    /* ilmi related stuff */
    lsv_setup_lsv_ilmi();

    /* back end LECS connection */
    lsv_initValidationSubsystem();

    /* enqueue packets to the LES */
    reg_add_lsv_enqueue(lsv_enqueue, "lsv_enqueue");
}

