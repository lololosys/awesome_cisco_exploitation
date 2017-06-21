/* $Id: lsv_client.c,v 3.3.56.10 1996/08/28 12:55:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lsv_client.c,v $
 *------------------------------------------------------------------
 * lsv_client.c -- LAN Emulation Server's clients' state machine
 *
 * April 1995, Norm Finn
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lsv_client.c,v $
 * Revision 3.3.56.10  1996/08/28  12:55:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.56.9  1996/08/16  14:41:38  michellw
 * CSCdi61426:  LANE: _lane_show_one_all  crash
 * Branch: California_branch
 * Null out the reg_table->next pointer when freeing this entry.
 * Use while loop instead of for loop when traversing
 * lsv_info->reg_table.
 *
 * Revision 3.3.56.8  1996/08/15  02:01:45  rbadri
 * CSCdi63405:  LANE LES: lec gets stuck in state T
 * Branch: California_branch
 * Back out Gee's changes as it causes crash, Memory leak
 *
 * Revision 3.3.56.7  1996/07/23  13:25:23  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.56.6  1996/07/21  00:56:41  cakyol
 * CSCdi63405:  LANE LES: lec gets stuck in state T
 * Branch: California_branch
 *
 * Revision 3.3.56.5  1996/06/08  01:01:52  cakyol
 * CSCdi56897:  LANE: buffers remain in incoming queue.
 *              missing datagram_done when receiving a join request
 *              while in the middle of processing a previous one.
 * Branch: California_branch
 *
 * Revision 3.3.56.4  1996/05/23  22:29:30  cakyol
 * CSCdi58551:  improve debug message to hilite misconfigurations.
 *              fixed.
 * Branch: California_branch
 *
 * Revision 3.3.56.3  1996/05/09  14:33:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.56.2.2.1  1996/04/27  07:07:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.56.2  1996/04/25  23:15:13  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.56.1.8.5  1996/04/17  01:25:31  cakyol
 * eliminate potential NULL dereference in "lsv_lecsAccessSigCB"
 * when subinterface gets deleted.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.56.1.8.4  1996/04/16  02:45:18  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.56.1.8.3  1996/04/11  22:00:35  cakyol
 * looks awful now, but the boss is happy.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.56.1.8.2  1996/04/08  14:52:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.56.1.8.1  1996/04/08  01:57:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.56.1  1996/03/18  20:33:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/07  10:03:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  14:38:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3.60.6  1996/04/05  03:20:53  cakyol
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
 * Revision 3.3.60.5  1996/04/03  02:50:22  cakyol
 * plug some unlikely but possible holes:
 * - Do not allow a control direct to be established to an LES
 *   if the calling entity has the same atm address as the called
 *   LES (this may happen if the atm network goes wacko).
 * - Do not setup a back end LECS vc if the said LECS has the same
 *   at address as the calling LES (this is possible due to an
 *   operator configuration error).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.60.4  1996/04/02  22:02:57  cakyol
 * - If the LES is not yet connected to the LECS, at the time
 *   the join request is received, do not just deny it, queue
 *   it up to be processed later, if and when the connection
 *   to the LECS succeeds.
 * - Use LES_DBG throughout.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.60.3  1996/04/02  10:10:06  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.60.2  1996/03/25  02:20:13  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.60.1  1996/03/22  22:55:40  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.34.3  1996/03/16  07:59:51  cakyol
 * missed a very important datagram_done.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.34.2  1996/03/01  01:46:38  cakyol
 * convert "crashdump"s to "lane_router_crash" which can print
 * a last gasp message and call "logger_flush" before dying.
 * Helps debugging a lot.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.34.1  1996/02/27  21:11:01  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.1  1996/02/29  07:49:04  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.14  1996/02/24  01:36:13  cakyol
 * - much better ordering of debug messages.
 * - use the new "atmSig_printOpcode" api function.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.13  1996/02/23  00:51:43  cakyol
 * current statement ordering frees a block twice, change it.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.12  1996/02/22  21:08:32  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.11  1996/02/22  09:05:44  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.10  1996/02/08  08:29:18  cakyol
 * lecs.c:
 *     - Call "chunk_create" when needed and not at init time.
 *       This was bugid CSCdi48400 in previous releases.
 *     - add logger_flush before calling crashdump.
 * lsv_client.c:
 *     - correct misleading comment.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.9  1996/02/06  22:19:45  cakyol
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
 * Revision 3.3.12.8  1996/02/02  02:16:36  cakyol
 * - much more robust procesing of calling party address on
 *   the LECS now.  If a call from an already existing caller
 *   is received, the old one is maintained, the new one is
 *   rejected.  Before, the new one bumped the old one off.
 * - More efficient lecs finder:
 *         - if noone wants to know the master LECS address,
 *           this step is completely eliminated saving cycles.
 *         - as soon as the global LECS addresses are determined,
 *           the users awaiting only for those are responded to
 *           immediately.  Before, they had to wait until the
 *           master LECS was determined as well.  This could take
 *           a long time if signalling was slow.
 *         - safety timer to protect against ILMI hangups is
 *           added for debugging only.
 *         - the "#ifdef TESTING" section is now taken out.
 * - long printf string cleanup done, concatenation used now.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.7  1996/02/01  02:44:34  cakyol
 * - an LES should not send the LECS error code straight thru
 *   to an LEC trying to join.  It should map the NO_CONFIG
 *   error from the LECS to an ACCESS_DENIED before sending it
 *   across to the client.
 *   (This is bug id CSCdi47794 in arkansas 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.6  1996/01/30  23:56:37  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.5  1996/01/26  02:02:56  cakyol
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
 * Revision 3.3.12.4.2.3  1996/03/23  21:14:09  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.3.12.4.2.2  1996/03/05  04:47:45  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.3.12.4.2.1  1996/02/26  23:09:09  ddecapit
 * Branch: TRlane_branch
 * 
 * Revision 3.3.12.4  1996/01/20  02:13:15  cakyol
 * - debug message in "sendRelease" was erroneous.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.3  1996/01/17  20:26:33  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.2  1996/01/11  22:03:38  cakyol
 * - warn against duplicate LECS addresses from the switch
 * - update from the switch once a minute
 * - correct more out of date comments
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.1  1996/01/11  01:46:58  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.3  1996/01/08  20:47:04  cakyol
 * During client re-validation, give up context after
 * PAK_SWITCHCOUNT validations.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3.10.2  1996/01/03  22:53:04  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:12  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6.40.1  1996/02/03  07:07:20  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.6.22.1  1995/10/16  23:45:55  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.6  1995/07/31  22:45:24  nfinn
 * CSCdi37969:  LES wont allow a PVC-connected client
 *
 * Revision 2.5  1995/07/30  06:21:53  nfinn
 * CSCdi37638:  router crashed at _lane_cfgacc_send+3c0 after Create VC
 * failure
 *
 * Revision 2.4  1995/07/05  21:13:03  nfinn
 * CSCdi36529:  AIP input queue 75/75 with drops and throttled
 * Review of code revealed one more case that needed the datagram_done.
 *
 * Revision 2.3  1995/06/29  19:15:58  shj
 * CSCdi36529:  AIP input queue 75/75 with drops and throttled
 *
 * Put in missing datagram_done on config-response packet.
 *
 * Revision 2.2  1995/06/19  06:46:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:20:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 
/**************************************************************
 * LES/BUS clients' state machine
 **************************************************************/
/*
 * One state machine per client of a LES/BUS:
 *
 * Events:
 *
 *	direct received	lsv_sigback_listen_direct got 
 *			    LANE_CALL_SETUP_RECEIVED
 *	good join	Valid Join request received
 *	bad join	Invalid Join request received
 *	other request	Any other request received
 *	BUS event	MC Send or MC Forward connection/release event
 *	timer expired	Client's join timer expired
 *	kill_client	lsv_kill_client issued
 *	good cfgresp	Good config response received from LECS
 *	bad cfgresp	Reject config response received from LECS
 *	timeout cfgresp	No config response received from LECS
 *	add complete	Add party to control distribute succeeded
 *	control lost	Control direct or control distribute leaf 
 *			    released
 *	term events	last control lost event
 *
 * States/transitions:
 *
 *  LSV_CLIENT_INITIALIZED
 *	(created in this state when direct received)
 *	direct received:
 *		start timer
 *		-> LSV_CLIENT_CONNECTED
 *	kill_client
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_CONNECTED
 *	good join:
 *		issue config request
 *		-> LSV_CLIENT_JOIN_RECEIVED
 *	bad join:
 *		Send fail join response
 *		restart timer
 *		-> LSV_CLIENT_REJECTED
 *	timer expired, kill_client, or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_JOIN_RECEIVED
 *	good cfgresp:
 *		Add client to control direct
 *		-> LSV_CLIENT_ADD_SENT
 *	bad cfgresp:
 *		Send fail join response
 *		restart timer
 *		-> LSV_CLIENT_REJECTED
 *	timeout cfgresp:
 *		Send fail join response
 *		restart timer
 *		-> LSV_CLIENT_REJECTED
 *	timer expired, kill_client, or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_ADD_SENT
 *	add complete:
 *		kill timer
 *		Send successful join response
 *		-> LSV_CLIENT_JOINED
 *	timer expired, kill_client, or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_JOINED
 *	other request:
 *		respond to request appropriately
 *		-> LSV_CLIENT_JOINED
 *	BUS event:
 *		if MC send setup request
 *			initiate BUS Forward add leaf
 *			-> LSV_CLIENT_JOINED
 *		else if MC forward complete
 *			-> LSV_CLIENT_OPERATIONAL
 *		else if any BUS connection lost,
 *			release the other BUS connection
 *			-> LSV_CLIENT_JOINED
 *	kill_client or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_OPERATIONAL
 *	other request:
 *		respond to request appropriately
 *		-> LSV_CLIENT_JOINED
 *	BUS event:
 *		if any BUS connection lost,
 *			release the other BUS connection
 *			-> LSV_CLIENT_JOINED
 *		else
 *			refuse new BUS connections
 *	kill_client or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_REJECTED
 *	kill_client or control lost:
 *		-> LSV_CLIENT_TERMINATING
 *	other:
 *		ignore or crashdump
 *  LSV_CLIENT_TERMINATING
 *	kill_client or term event:
 *		if last term event,
 *			-> LSV_CLIENT_INITIALIZED
 *		else
 *			initiate next event in sequence, if needed:
 *				unregister all MACs and ATMSAPs
 *				start all connections going down
 *			-> LSV_CLIENT_TERMINATING
 *	kill_client:
 *		ignored
 *	other:
 *		ignore or crashdump
 */

/****************************************************************
 * Miscallaneous utilities
 ****************************************************************/

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../if/network.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lsv_private.h"
#include "../ui/debug.h"
#include "lane_debug.h"
#include "generic_objects.h"

#include "sys_registry.h"
#include "media_registry.h"
#include "lane_registry.h"

/* forward decl */

static void validate_all_clients (lsv_info_t *lsv_info);

/*
 * L o c a l   S t o r a g e
 */
static char *lsv_client_state_names[] = LSV_CLIENT_STATE_NAMES;

/*
 * lsv_getbuffer - Sets up most of header for LANE control frame, but
 *		   does NOT fill opcode.
 */
paktype *
lsv_getbuffer (lsv_info_t *lsv_info, int size)
{
    paktype		*pak;
    lane_ctl_hdr_t	*hdr;

    pak = getbuffer(size);
    if (!pak) {
	return(NULL);
    }
    hdr = (lane_ctl_hdr_t *) pak->datagramstart;
    memset(hdr, 0, size);
    hdr->marker		= LANE_MARKER;
    hdr->protocol	= LANE_PROTOCOL_ID;
    hdr->version	= LANE_PROTOCOL_VERSION;
    pak->if_output	= lsv_info->lane_info->swidb;
    pak->datagramsize	= size;
    return(pak);
}

/*
 * lsv_client_new_state - Make client state change and report it
 */
static void
lsv_client_new_state (
    lsv_client_t	*client,
    lsv_client_state_t	 state)
{
    if (client->client_state == state) return;
    LES_DBG(client->lsv_info->lane_info->swidb->namestring,
	"elan %s client %d state change %s -> %s",
	client->lsv_info->lane_info->name,
	client->lecid, 
	lsv_client_state_names [client->client_state],
	lsv_client_state_names [state]);
    client->client_state = state;
}


/*
 * lsv_find_client_primary - find a client corresponding to 
 * a primary NSAP
 */
lsv_client_t *
lsv_find_client_primary (
    lsv_info_t	*lsv_info,
    hwaddrtype	*atmsap)
{
    lsv_client_t	*client;

    client = lsv_info->client_table;
    while(client != NULL) {
	if (lane_addr_equal(atmsap, 
		&(client->direct_vcc.calling_addr)))
	    return(client);
	client = client->next;
    }
    return(NULL);
}

/*
 * lsv_vcd_to_client - Find client based on vcd
 */
static lsv_client_t *
lsv_vcd_to_client (
    lsv_info_t	*lsv_info,
    vcd_t	 vcd)
{
    lsv_client_t	*client;

    client = lsv_info->client_table;
    while (client != NULL) {
	if (client->direct_vcc.vcd == vcd)
	    return(client);
	client = client->next;
    }
    return(NULL);
}

/*********************************************************
**********************************************************
** 		LES to LECS validation subsystem
**********************************************************
*********************************************************/

/* initialization flag */
static boolean validationSubsystemInited = FALSE;

/*
** initialize the LES client validation subsystem
*/
void lsv_initValidationSubsystem (void)
{
    if (validationSubsystemInited) return;
    validationSubsystemInited = TRUE;
    LES_DBG(NULL, "validation system initialized");
}

/*
** destroy a validation object.
*/
static void validation_destroy (ClientValidation_t *validation)
{
    lsv_info_t *lsv_info;
    lsv_client_t *client;
    ListPtr list;

    if (validation == NULL) return;
    lsv_info = validation->lsv_info;
    if (lsv_info) {
	list = lsv_info->lecsAccess.validationRequests;
	if (list) List_remove(list, validation);
    }

    /* clean the cross pointers */
    client = validation->client;
    if (client) client->validation = NULL;

    mgd_timer_stop(&validation->timer);
    free(validation);
}

/*
** allocate a validation object.  This also
** adds it to the list of all validation objects.
*/
static ClientValidation_t *validation_alloc (lsv_info_t *lsv_info,
    lsv_client_t *client)
{
    ClientValidation_t *cv;
    ListPtr list;

    if (lsv_info == NULL) return NULL;
    if (client == NULL) return NULL;
    if (client->validation != NULL) {
	LES_ERR("client already has a validation record");
	return NULL;
    }
    list = lsv_info->lecsAccess.validationRequests;
    if (list == NULL) return NULL;
    cv = malloc_named(sizeof(ClientValidation_t), 
	"ClientValidation_t");
    if (cv != NULL) {
	cv->lsv_info = lsv_info;
	cv->client = client;
	client->validation = cv;
	mgd_timer_init_leaf(&cv->timer, &lsv_root_timer,
	    LSV_LEC_VALIDATION, cv, FALSE);
	List_clearLinks(cv);
	List_addToEnd(list, cv);
    }
    return cv;
}

/*
** destroy all pending validation requests that
** belong to a specific LES.
*/
static void validation_destroyAll (lsv_info_t *lsv_info)
{
    ListPtr list;
    ClientValidation_t *cv, *deleter;

    if (lsv_info == NULL) return;
    list = lsv_info->lecsAccess.validationRequests;
    if (list == NULL) return;
    cv = List_head(list);
    while (cv) {
	deleter = cv;
	cv = List_next(cv);
	validation_destroy(deleter);
    }
}

/*
** release with a debug message
*/
static void lsvRelease (boolean releaseComplete, void *callId,
    sig_api_cause_code cause, idbtype *swidb)
{
    LES_DBG(swidb->namestring,
	"sending RELEASE%s for call 0x%x cause %d",
	releaseComplete ? "_COMPLETE" : "",
	callId, cause);
    sendRelease(releaseComplete, callId, cause, swidb);
}

/*
** cleanup/delete/destroy individual sections of
** the LECS access subsystem, depending on the 
** flags provided.
*/
void cleanupLecsAccess (lsv_info_t *lsv_info,

    boolean disconnect,		/* disconnect from the LECS */
    boolean discard,		/* discard validation requests */
    boolean destroyList)	/* destroy the list object */
{
    char *ifname;
    ListPtr list;
    LecsAccess *lac;

    if (lsv_info == NULL) return;
    ifname = lsv_info->lane_info->swidb->namestring;
    lac = &lsv_info->lecsAccess;

    /* 
    ** logically, if list destruction is requested,
    ** then discard should automatically be turned on to
    ** flush the list to make it ready for destruction.
    */
    if (destroyList) discard = TRUE;

    /* disconnect from the LECS if required */
    if (disconnect) {
	lac->shouldBeConnected = FALSE;
	if (lac->connected && lac->callId) {
	    LES_DBG(ifname, 
		"cleanupLecsAccess: disconnecting "
		"from LECS callId 0x%x",
		lac->callId);
	    lsvRelease(FALSE, lac->callId,
		ATM_NORMAL_UNSPECIFIED, lsv_info->lane_info->swidb);
	}
	lac->connected = FALSE;
	lac->callId = NULL;
    }

    /* clean all pending verification requests if required */
    if (discard) {
	LES_DBG(ifname, 
	    "cleanupLecsAccess: discarding all validation requests");
	validation_destroyAll(lsv_info);
    }

    /* destroy the list as well ? */
    if (destroyList) {
	LES_DBG(ifname, 
	    "cleanupLecsAccess: destroying the validation list");
	list = lsv_info->lecsAccess.validationRequests;
	if (list) List_destroy(list);
	lac->validationRequests = NULL;
	mgd_timer_stop(&lac->lecsReconnectTimer);
    }
}

/*
** the callback routine used to handle the responses
** from the connect to LECS call to the signalling.
**
** The important point in this routine is that:
**
**    - as soon as a break/disconnect is detected,
**	set timer to try to reconnect soon.
**    - as soon as connection succeeds, reverify
**	all connected clients.
*/
static void lsv_lecsAccessSigCB (void *arg)
{
    sig_api_msg *msg = (sig_api_msg*) arg;
    lsv_info_t *lsv_info;
    void *callId = NULL;
    boolean connected = FALSE;
    char *ifname = NULL;
    lane_info_t *lane_info;
    LecsAccess *lac;

    /* sanity */
    if (msg == NULL) return;
    if (msg->idb == NULL) return;
    ifname = msg->idb->namestring;

    /* do these first */
    switch (msg->opcode) {
	case ATM_CONNECT:
	    callId = msg->u.connect.call_id;
	    connected = TRUE;
	    break;
	case ATM_CONNECT_ACK:
	    callId = msg->u.connect_ack.call_id;
	    connected = TRUE;
	    break;
	case ATM_RELEASE:
	    callId = msg->u.release.call_id;
	    break;
	case ATM_RELEASE_COMPLETE:
	    callId = msg->u.release_comp.call_id;
	    break;
	default:
	    LES_ERR("lsv_lecsAccessSigCB called with opcode %u",
		msg->opcode);
	    return;
    }
    LES_DBG(ifname,
	"lsv_lecsAccessSigCB called with callId 0x%x, opcode %s",
	callId, atmSig_printOpcode(msg->opcode));

    /* always acknowledge a release immediately no matter what */
    if (msg->opcode == ATM_RELEASE)
	lsvRelease(TRUE, callId, ATM_NORMAL_UNSPECIFIED, 
	    msg->idb);

    /* more sanity */
    lane_info = msg->idb->lane_info;
    if (lane_info == NULL) return;
    lsv_info = lane_info->lsv_info;
    if (lsv_info == NULL) return;
    lac = &lsv_info->lecsAccess;
    if (lac == NULL)
    {
	LES_ERR("interface %s: lsv_lecsAccessSigCB: NULL lac",
	    ifname);
	return;
    }

    /* 
    ** This is very important here, it makes sure that
    ** the LES discards previous transactions that
    ** do not matter any more.  Be very careful when
    ** you change the few statements below.
    */
    if (callId != lac->callId) {
	LES_DBG(ifname,
	    "previous sig transaction result being ignored");

	/*
	** although this transaction is not relevant to us
	** now, it has set up a vc.  So, tear it down so that
	** "lost" and "dangling" vcs are not left lying around.
	*/
	if (connected)
	    lsvRelease(FALSE, callId, ATM_NORMAL_UNSPECIFIED, 
		msg->idb);
	return;
    }

    /*
    ** we are successfully connected to the LECS.
    ** Therefore analyze the situation.
    */
    if (connected) {
	LES_DBG(ifname, "connected to the master LECS");

	/* are we already connected ? */
	if (lac->connected)
	    LES_ERR("but was ALREADY connected anyway");

	/* were we expecting to be connected ? */
	if (! lac->shouldBeConnected)
	    LES_ERR("but should NOT have been connected");

	lac->connected = TRUE;
	LES_DBG(ifname, "now validating already connected clients");
	validate_all_clients(lsv_info);
    } else {
	/* disconnected */
	LES_DBG(ifname, "disconnected from the master LECS");
	if (! lac->connected)
	    LES_DBG(ifname, "and was NOT connected anyway");
	lac->callId = NULL;
	lac->connected = FALSE;

	/* but, are we supposed to be connected ? */
	if (lac->shouldBeConnected) {
	    LES_DBG(ifname, 
		"should have been connected, will "
		"reconnect in %d seconds",
		LECS_RECONNECT_TIME);

	    mgd_timer_start_jittered(&lac->lecsReconnectTimer,
		 LECS_RECONNECT_TIME*ONESEC, 20);
	}
    }
}

/*
** connect to the master LECS from the LES.
*/
void lsv_connectToMasterLecs (lsv_info_t *lsv_info)
{
    lane_info_t *lane_info;
    idbtype *swidb;
    char *ifname;
    char *ename;
    sig_api_msg msg;
    void *callId;
    int n;
    LecsAccess *lac;

    if (lsv_info == NULL) return;
    lac = &lsv_info->lecsAccess;
    lane_info = lsv_info->lane_info;
    swidb = lane_info->swidb;
    ifname = swidb->namestring;
    ename = lane_info->name;

    /* set what is "desired" */
    lac->shouldBeConnected = TRUE;

    /* if already connected however, return */
    if (lac->connected) {
	LES_DBG(ifname,
	    "elan %s; LES already connected to the LECS", ename);
	return;
    }

    /* if already DURING the process of connecting, return */
    if (lac->callId != NULL) {
	LES_DBG(ifname,
	    "elan %s; LES in the process of connecting to the LECS",
	    ename);
	return;
    }

    /* if LES is on the way down, dont continue */
    if (lsv_info->server_state == LSV_LESBUS_TERMINATING)
	return;

    /* is the LES address fully determined yet ? */
    if (! lane_addr_valid(&lsv_info->lsv_nsap)) {
	LES_DBG(ifname,
	    "elan %s; LES nsap address not yet determined fully",
	    ename);
	return;
    }

    /*
    ** Ensure that the LECS is not actually me,
    ** this is a possible configuration error
    */
    if (lane_addr_equal(&lsv_info->lsv_nsap, 
	&lsv_info->lsv_lecs_nsap)) {
	LES_ERR("LES %s: master LECS address is the same as mine", 
	    ifname);
	
	/* retry again later */
	mgd_timer_start_jittered(&lac->lecsReconnectTimer,
	    10*LECS_RECONNECT_TIME*ONESEC, 20);

	return;
    }

    /* ok connect */
    LES_DBG(ifname, 
	"LES trying to connect to the master LECS %Ch",
	&lsv_info->lsv_lecs_nsap);
    callId = atmSig_api_allocate_call_id();
    lac->callId = callId;
    if (callId == NULL) {
	LES_DBG(ifname,
	    "lsv_connectToMasterLecs: allocate call_id failed");
	
	/* retry again later */
	mgd_timer_start_jittered(&lac->lecsReconnectTimer,
	    LECS_RECONNECT_TIME*ONESEC, 20);

	return;
    }
    msg.link = NULL;
    msg.opcode = ATM_SETUP;
    msg.idb = swidb;
    msg.u.setup.multipoint = FALSE;
    msg.u.setup.endpoint_id = NULL;
    msg.u.setup.call_id = callId;
    msg.u.setup.sig_user_callback = lsv_lecsAccessSigCB;
    atm_params_init(&msg.u.setup.params);
    msg.u.setup.called_party = lsv_info->lsv_lecs_nsap;
    msg.u.setup.flags = (ATM_VC_AAL5LANE | VC_FLAG_NOIDLETIME);
    msg.u.setup.user_vc_type = LANE_LES_DIRECT;
    msg.u.setup.fwd_sdu_size = LANE_SDU_SIZE_1516;
    msg.u.setup.bwd_sdu_size = LANE_SDU_SIZE_1516;
    lane_sig_make_blli(&msg.u.setup.blli [0], LANE_CONFIGURE);
    for (n=1; n<ATM_MAX_BLLI_NUM; n++)
	msg.u.setup.blli [n].length = 0;
    msg.u.setup.calling_party = lsv_info->lsv_nsap;
    atmSig_handleUserRequest(&msg);
}

/****************************************************************
 * Client state machine
 ****************************************************************/

/*
 * lsv_release_client_structures - release all belonging to a client
 */
static void
lsv_release_client_structures (
    lsv_info_t	 *lsv_info,
    lsv_client_t *client)
{
    lsv_client_t *this_client;
    lsv_client_t *last_client;

    /* delete registrations and timeouts
     */
    lsv_unregister_all_dest_nsap(lsv_info, client);
    mgd_timer_stop(&client->S4_timer);

    /* Remove client from LES/BUS's chain of clients
     */
    last_client = NULL;
    this_client = lsv_info->client_table;
    while (this_client!= NULL) {
	if (this_client == client) {
	    validation_destroy(client->validation);
	    
	    /* adjust this if being deleted */
	    if (lsv_info->beingValidated == client)
		lsv_info->beingValidated = client->next;

	    if (last_client == NULL)
		lsv_info->client_table = client->next;
	    else
		last_client->next      = client->next;

	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s client %d destroyed",
		lsv_info->lane_info->name, client->lecid);
	    Index_remove(lsv_info->clientList, client);
	    client->next = NULL;  /* protect 'show' command */
	    free(client);

	    /* Advance LES/BUS termination if LES/BUS is terminating
	     */
	    if (lsv_info->client_table == NULL  &&
		lsv_info->server_state == LSV_LESBUS_TERMINATING) {
		lsv_kill_lesbus(lsv_info);
	    }
	    return;
	}
	last_client = this_client;
	this_client = this_client->next;
    }

    /* client not on LES/BUS chain!
     */
    lsv_crash(client->lsv_info, "lsv_release_client_structures");
}

/*
** returns TRUE if the request has successfully finished.
** Otherwise returns FALSE if either still working on it
** or some failure has occured.
*/
static boolean terminate_vcc (lane_signal_mcgroup_t *mcg,
    lane_vcc_info_t *lv)
{
    if (mcg)
	return (lane_drop(mcg, lv) == LANE_SIG_SUCCESS);
    else
	return (lane_release(lv) == LANE_SIG_SUCCESS);
}

/*
 * lsv_kill_client - terminate a client for cause
 */
void
lsv_kill_client (
    lsv_info_t	 *lsv_info,
    lsv_client_t *client)
{
    boolean all_done = TRUE;

    if (client->dont_recurse) return;
    client->dont_recurse = TRUE;
    
    /* lock out all other activities */
    lsv_client_new_state(client, LSV_CLIENT_TERMINATING);

    /* kill all relevant circuits */
    all_done &= terminate_vcc(&lsv_info->distribute_vcc, 
	&client->distribute_vcc);
    all_done &= terminate_vcc(&lsv_info->forward_vcc,
	&client->forward_vcc);
    all_done &= terminate_vcc(NULL, &client->send_vcc);
    all_done &= terminate_vcc(NULL, &client->direct_vcc);

    /* If all done, release structures, else wait for sig */
    if (all_done) {
	lsv_release_client_structures(lsv_info, client);
    } else {
	client->dont_recurse = FALSE;
    }
}

/*
 * lsv_client_gained_forward - MC Forward leaf added to a client.
 */
static void
lsv_client_gained_forward (
    lsv_client_t *client)
{
    switch(client->client_state) {
    case LSV_CLIENT_INITIALIZED:
    case LSV_CLIENT_CONNECTED:
    case LSV_CLIENT_JOIN_RECEIVED:
    case LSV_CLIENT_ADD_SENT:
    case LSV_CLIENT_OPERATIONAL:
    case LSV_CLIENT_REJECTED:
	lsv_crash(client->lsv_info, "gained_forward");
	break;
    case LSV_CLIENT_JOINED:
	if (client->send_vcc.vcc_state == LANE_VCC_CONNECTED) {
            lsv_client_new_state(client, LSV_CLIENT_OPERATIONAL);

	    /* for Catalyst 5000 ATM Module */
            reg_invoke_bus_map_bus_vcds(client);
	}
	break;
    case LSV_CLIENT_TERMINATING:
	lsv_kill_client(client->lsv_info, client);
	break;
    }
}


/*
 * lsv_join_complete - Come here when we're ready to tell 
 * client he's OK
 */
static void
lsv_join_complete (
    lsv_info_t		*lsv_info,
    lsv_client_t	*client)
{
    lane_info_t		*lane_info;
    paktype		*pak;
    lane_join_pdu_t	*join;

    mgd_timer_stop(&client->S4_timer);
    /*
     * Answer client only if client sent a Join.  
     * PVC clients don't have to.
     */
    if (client->first_join.hdr.marker == LANE_MARKER) {
	client->is_proxy = (client->first_join.hdr.flags &
					LANE_PROT_FLAG_PROXY) != 0;
	pak = lsv_getbuffer(lsv_info, sizeof(lane_join_pdu_t));
	if (pak == NULL) {
	    lsv_kill_client(lsv_info, client);
	    return;
	}
	lane_info = lsv_info->lane_info;
	join = (lane_join_pdu_t *)pak->datagramstart;
	bcopy(&client->first_join, join, sizeof(lane_join_pdu_t));
	join->lan_type		  = lane_info->type;
	join->max_frame_size      = lane_info->max_data_frame;
	join->num_tlvs		  = 0;
	join->name_size		  = lane_info->name_size;
	join->hdr.requestor_lecid = client->lecid;
	bcopy(lane_info->name, join->elan_name, 
	    MAX_ELAN_NAME_LENGTH);
	lsv_answer_request(client, pak, LANE_STATUS_SUCCESS);
    }
    lsv_client_new_state(client, LSV_CLIENT_JOINED);
}


/*
 * lsv_signal_distribute_callback - Come here for any callback 
 * from the signalling ifc on a control distribute connection.
 */
static lane_vcc_info_t *
lsv_signal_distribute_callback (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_client_t	*client;
    lsv_info_t		*lsv_info;

    client   = (lsv_client_t *)vcc_info->parm;
    lsv_info = client->lsv_info;
    switch(code) {
    case LANE_CALL_LISTEN_COMPLETE:
    case LANE_CALL_LISTEN_FAILED:
    case LANE_CALL_CANCEL_COMPLETE:
    case LANE_CALL_SETUP_RECEIVED:
	break;
    case LANE_CALL_CALL_COMPLETE:
    case LANE_CALL_ADD_COMPLETE:
	lsv_join_complete(lsv_info, client);
	return(NULL);

    case LANE_CALL_CALL_FAILED:
    case LANE_CALL_ADD_FAILED:
    case LANE_CALL_RELEASE_COMPLETE:
    case LANE_CALL_RELEASE_RECEIVED:
    case LANE_CALL_DROP_COMPLETE:
    case LANE_CALL_DROP_FAILED:
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d lost control distribute",
	   lsv_info->lane_info->name, client->lecid);
	lsv_kill_client(lsv_info, client);

	/* and/or the server is dying */
	if (lsv_info->server_state == LSV_LESBUS_TERMINATING)
	    lsv_kill_lesbus(lsv_info);

	return(NULL);
    }
    lsv_crash(lsv_info, "lsv_signal_distribute_callback");
    return(NULL);
}

/*
 * lsv_signal_direct_callback - Come here for any callback from the
 *  signalling ifc on a control direct connection.
 */
static lane_vcc_info_t *
lsv_signal_direct_callback (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_info_t		*lsv_info;
    lsv_client_t	*client;

    /* If call complete, turn on timer, advance client's state
     */
    client   = (lsv_client_t *)vcc_info->parm;
    lsv_info = client->lsv_info;
    switch(code) {
    case LANE_CALL_CALL_COMPLETE:
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d control direct complete",
	    lsv_info->lane_info->name, client->lecid);
	break;

    case LANE_CALL_RELEASE_COMPLETE:
    case LANE_CALL_RELEASE_RECEIVED:
	/*
	 * if call released, drop the client
	 */
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d released control direct",
	    lsv_info->lane_info->name, client->lecid);
	lsv_kill_client(client->lsv_info, client);

	/* and/or server is dying */
	if (lsv_info->server_state == LSV_LESBUS_TERMINATING)
	    lsv_kill_lesbus(lsv_info);

	break;

    default:
	/*
	 * complain if any other state
	 */
	lsv_crash(lsv_info, "lsv_signal_direct_callback");
	break;
    }
    return(NULL);
}

/*
 * lsv_create_lsv_client
 *
 * Create a new lsv_client_t with a specified lecid (or 0, to 
 * assign the lecid automatically).  Return NULL if unable to 
 * allocate an lecid or if the specified lecid already exists.  
 * Bring the client all the way up if the connection is a PVC.
 */
static lsv_client_t *
lsv_create_lsv_client (
    lsv_info_t *lsv_info, /* Which server gets the client */
    lane_vcc_info_t *direct_vcc) /* client's control direct vcc */
{
    lsv_client_t	*new_client;
    lsv_client_t	*client;
    lsv_client_t	*last_client;
    lane_info_t		*lane_info;
    ushort		 lecid;

    /* Scan existing clients to find where this new LECID goes
     */
    lane_info	= lsv_info->lane_info;
    client	= lsv_info->client_table;
    last_client	= NULL;
    new_client	= NULL;

    /* allocate a new lecid for SVC-connected client
     */
    lecid = 1;
    while (client != NULL) {
	if (client->lecid == lecid) {
	    if (++lecid == LANE_MARKER) {
		errmsg(&msgsym(LSV_LECID, LANE),
		   lane_info->swidb->namestring, lane_info->name);
		return(NULL);	/* No more LECIDs left!! */
	    }
	    last_client = client;
	    client	    = client->next;
	} else if (client->lecid < lecid) {
	    lsv_crash(lsv_info, "lsv_create_lsv_client svc");
	    return(NULL);
	} else {
	    break;
	}
    }

    /* Create a new client if required.  Abort if we can't */

    if (new_client == NULL) {
	new_client = malloc(sizeof(lsv_client_t));
	if (new_client == NULL) {
	    return(NULL);
	}
	LES_DBG(lane_info->swidb->namestring,
	    "elan %s new client %d",
	    lane_info->name, lecid);
	if (Index_add(lsv_info->clientList, 
		new_client, new_client) != new_client) {
	    LES_ERR("could NOT Index_add client %d for elan %s",
		lecid, lane_info->name);
	    free(new_client);
	    return NULL;
	}
	*new_client = LSV_CLIENT_EMPTY;
	mgd_timer_init_leaf(&new_client->S4_timer, 
	    &lsv_info->timer_node, LSV_TIMER_CLIENT, 
	    new_client, FALSE);
	new_client->next = client;
	if (last_client == NULL)
	    lsv_info->client_table = new_client;
	else
	    last_client->next = new_client;
	client			= new_client;
	client->lecid		= lecid;
	client->lsv_info	= lsv_info;
	LES_DBG(lane_info->swidb->namestring,
	    "elan %s initialized client %d",
	    lane_info->name, lecid);
    } else {
	LES_DBG(lane_info->swidb->namestring,
	    "elan %s re-initialized client %d",
	    lane_info->name, lecid);
    }

    /* (Re-)initialze client with new connection information
     */
    if (client->direct_vcc.vcc_state != LANE_VCC_INITIALIZED)
	return(NULL);
    client->direct_vcc			= *direct_vcc;
    client->direct_vcc.parm		= client;
    client->direct_vcc.callback		= lsv_signal_direct_callback;

    /* Accept the call
     */
    return(new_client);
}


/*
 * lsv_setup_distribute_vcc - Setup client's 
 * distribute_vcc for an add.
 */
static void
lsv_setup_distribute_vcc (
    lsv_client_t	*client,
    hwaddrtype		*nsap)
{
    client->distribute_vcc = LANE_VCC_INFO_EMPTY;
    client->distribute_vcc.no_svc_timeout = TRUE;
    client->distribute_vcc.swidb = client->lsv_info->lane_info->swidb;
    client->distribute_vcc.called_addr = *nsap;
    client->distribute_vcc.calling_addr = client->lsv_info->lsv_nsap;
    client->distribute_vcc.parm = client;
    client->distribute_vcc.vc_type = LANE_LES_DISTRIBUTE;
    client->distribute_vcc.callback = lsv_signal_distribute_callback;
}


/*
 * lsv_sigback_listen_direct - Come here for any callback from the
 *  call listen for control direct vccs.
 */
lane_vcc_info_t *
lsv_sigback_listen_direct (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_info_t		*lsv_info;
    lsv_client_t	*client;
    lane_info_t		*lane_info;

    /* Startup and takedown conditions
     */
    lsv_info = (lsv_info_t *)vcc_info->parm;
    switch(code) {
    case LANE_CALL_LISTEN_COMPLETE:
	lsv_start_lesbus(lsv_info);
	return(NULL);
    case LANE_CALL_LISTEN_FAILED:
	errmsg(&msgsym(LSV_ADDR, LANE),
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name);
	lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	return(NULL);
    case LANE_CALL_CANCEL_COMPLETE:
	lsv_kill_lesbus(lsv_info);
	return(NULL);

    /* Big deal is when a call setup arrives from a brand new client.
     */
    case LANE_CALL_SETUP_RECEIVED:
	/*
	 * Calling party ATM address is required 
	 * for a control direct call.
	 */
	lane_info = lsv_info->lane_info;
	if (lane_addr_valid(&vcc_info->calling_addr)) {
	    /*
	    ** ensure that the connecting entity does NOT
	    ** have the same address as myself.
	    */
	    if (lane_addr_equal(&vcc_info->calling_addr, 
		&lsv_info->lsv_nsap)) {
		LES_DBG(lane_info->swidb->namestring,
		    "elan %s, connecting entity has my address",
		    lane_info->name);
		return NULL;
	    }
	} else {
	    LES_DBG(lane_info->swidb->namestring,
		"elan %s rejecting control direct "
		"with no calling party addr",
		lane_info->name);
	    return(NULL);
	}
	if (lsv_info->admin_state == LSV_LESBUS_ADMIN_UP &&
	    (lsv_info->server_state == LSV_LESBUS_PROVISIONAL ||
	     lsv_info->server_state == LSV_LESBUS_OPERATIONAL)) {
	    /*
	     * Server state allows acceptance of client.  
	     * Receipt of SVC means we don't have to keep 
	     * trying to register an address.
	     */
	    if (vcc_info->svc)
		lsv_server_new_state(lsv_info, 
		    LSV_LESBUS_OPERATIONAL);

	    /* Is this ATM address already in use?
	     */
	    if (NULL != lsv_find_client_primary(lsv_info,
			    &vcc_info->calling_addr)) {
		LES_DBG(lsv_info->lane_info->swidb->namestring,
		    "elan %s control direct duplicate %Cj",
		    lsv_info->lane_info->name,
		    vcc_info->calling_addr.addr);
		return(NULL);
	    }
	    client = lsv_create_lsv_client(lsv_info, vcc_info);
	    if (client == NULL)
		return(NULL);

	    /*
	     * Start Join timeout.  PVC client bypasses 
	     * LECS confirmation!
	     */
	    mgd_timer_start_jittered(&client->S4_timer,
		lsv_info->S4_control_timeout*ONESEC, 10);
	    lsv_client_new_state(client, LSV_CLIENT_CONNECTED);
	    return(&client->direct_vcc);
	} else {

	    /* LES/BUS is not up.  Refuse client's request.
	     */
	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s not ready for control direct",
		lsv_info->lane_info->name);
	    return(NULL);
	}
    default:
	lsv_crash(lsv_info, "lsv_sigback_listen_direct");
	return(NULL);
    }
}

/*
 * lsv_client_lost_bus - Some BUS connection to client was lost.
 */
static void
lsv_client_lost_bus (
    lsv_client_t *client)
{

    switch(client->client_state) {
    case LSV_CLIENT_OPERATIONAL:
    case LSV_CLIENT_JOINED:
	break;
    case LSV_CLIENT_TERMINATING:
	lsv_kill_client(client->lsv_info, client);
	return;
    default:
	lsv_crash(client->lsv_info, "lsv_client_lost_bus");
	break;
    }

    /* if either BUS connections falls, drop both BUS connections
     */
    lsv_client_new_state(client, LSV_CLIENT_JOINED);
    if (client->send_vcc.vcc_state != LANE_VCC_INITIALIZED  &&
	client->send_vcc.vcc_state != LANE_VCC_RELEASING) {
	LES_DBG(client->lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d releasing MC send",
	    client->lsv_info->lane_info->name, client->lecid);
	lane_release(&client->send_vcc);
    }
    if (client->forward_vcc.vcc_state != LANE_VCC_INITIALIZED  &&
	client->forward_vcc.vcc_state != LANE_VCC_DROPPING) {
	LES_DBG(client->lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d dropping MC forward",
	    client->lsv_info->lane_info->name, client->lecid);
	lane_drop(&client->lsv_info->forward_vcc, 
	    &client->forward_vcc);
    }
}

/*
 * lsv_signal_forward_callback - Come here for any callback from the
 *  signalling ifc on a multicast forward connection.
 */
static lane_vcc_info_t *
lsv_signal_forward_callback (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_info_t		*lsv_info;
    lsv_client_t	*client;

    client = (lsv_client_t *)vcc_info->parm;
    lsv_info = client->lsv_info;

    switch(code) {
    case LANE_CALL_LISTEN_COMPLETE:
    case LANE_CALL_LISTEN_FAILED:
    case LANE_CALL_CANCEL_COMPLETE:
    case LANE_CALL_SETUP_RECEIVED:
	lsv_crash(client->lsv_info, "lsv_signal_forward_callback");
	return(NULL);			/* ERROR!  can't happen */
    case LANE_CALL_CALL_COMPLETE:
    case LANE_CALL_ADD_COMPLETE:
	lsv_client_gained_forward(client);
	break;
    case LANE_CALL_CALL_FAILED:
    case LANE_CALL_ADD_FAILED:
    case LANE_CALL_RELEASE_RECEIVED:
    case LANE_CALL_RELEASE_COMPLETE:
    case LANE_CALL_DROP_COMPLETE:
    case LANE_CALL_DROP_FAILED:
	LES_DBG(client->lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d lost MC forward",
	    client->lsv_info->lane_info->name, client->lecid);
	lsv_client_lost_bus(client);

	/* and/or server is dying */
	if (lsv_info->server_state == LSV_LESBUS_TERMINATING)
	    lsv_kill_lesbus(lsv_info);

	break;
    }
    return(NULL);
}


/*
 * lsv_signal_send_callback - Come here for any callback from the
 *  signalling ifc on a multicast send connection.
 */
static lane_vcc_info_t *
lsv_signal_send_callback (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_info_t		*lsv_info;
    lsv_client_t	*client;

    client   = vcc_info->parm;
    lsv_info = client->lsv_info;
    switch(code) {
    case LANE_CALL_CALL_COMPLETE:
	/*
	 * make sure LES/BUS and client are still up
	 */
	if (lsv_info->admin_state  != LSV_LESBUS_ADMIN_UP ||
	    (lsv_info->server_state != LSV_LESBUS_OPERATIONAL &&
	     lsv_info->server_state != LSV_LESBUS_PROVISIONAL) ||
	    (client->client_state != LSV_CLIENT_JOINED &&
	     client->client_state != LSV_CLIENT_OPERATIONAL)) {
	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s client %d ignoring call complete",
		lsv_info->lane_info->name, client->lecid);
	    lsv_kill_client(lsv_info, client);
	    return(NULL);
	}

	/* MC Send established.  Create MC Forward 
	 * leaf to this client
	 */

	client->forward_vcc		   = LANE_VCC_INFO_EMPTY;
	client->forward_vcc.no_svc_timeout = TRUE;
	client->forward_vcc.swidb	   = client->send_vcc.swidb;
	client->forward_vcc.called_addr    = 
	    client->send_vcc.calling_addr;
	client->forward_vcc.calling_addr   = lsv_info->bus_nsap;
	client->forward_vcc.parm	   = client;
	
	client->forward_vcc.callback       = 
	    lsv_signal_forward_callback;


        switch (lsv_info->lane_info->type) {
          case(LANE_TYPE_802_3):
              client->forward_vcc.vc_type	   = LANE_BUS_FORWARD;
              break;
          case(LANE_TYPE_802_5): 
              client->forward_vcc.vc_type	   = LANE_BUS_TR_FORWARD;
              break;
          default:
            crashdump(0);
            return(NULL);
        }


	/* Add MC Forward leaf to MC group.
	 */
	switch(lane_add(&lsv_info->forward_vcc, 
	    &client->forward_vcc)) {
	case LANE_SIG_SUCCESS:
	    break;
	case LANE_SIG_FAILURE:
	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s client %d add MC forward failed",
		lsv_info->lane_info->name, client->lecid);
	    lsv_client_lost_bus(client);
	    break;
	case LANE_SIG_CALLBACK:
	    break;
	}
	break;

    case LANE_CALL_RELEASE_COMPLETE:
    case LANE_CALL_RELEASE_RECEIVED:
	/*
	 * Lost multicast send VCC.  Kill the client.
	 */
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s client %d MC forward released",
	    lsv_info->lane_info->name, client->lecid);
	lsv_client_lost_bus((lsv_client_t *)vcc_info->parm);

	/* and/or server is dying */
	if (lsv_info->server_state == LSV_LESBUS_TERMINATING)
	    lsv_kill_lesbus(lsv_info);

	break;

    default:
	lsv_crash(lsv_info, "lsv_signal_send_callback");
	break;
    }
    return(NULL);
}

/*
 * lsv_sigback_listen_send - Come here for any callback from the
 *  call listen for multicast send vccs.
 */
lane_vcc_info_t *
lsv_sigback_listen_send (
    lane_vcc_info_t	*vcc_info,
    lane_callback_code_t code)
{
    lsv_info_t		*lsv_info;
    lsv_client_t	*client;

    lsv_info = (lsv_info_t *)vcc_info->parm;
    switch(code) {
    case LANE_CALL_LISTEN_COMPLETE:
	lsv_start_lesbus(lsv_info);
	return(NULL);
    case LANE_CALL_LISTEN_FAILED:
	errmsg(&msgsym(LSV_ADDR, LANE),
		lsv_info->lane_info->swidb->namestring,
		lsv_info->lane_info->name);
	lsv_big_error(lsv_info, LSV_RECYCLE_TIME);
	return(NULL);
    case LANE_CALL_CANCEL_COMPLETE:
	lsv_kill_lesbus(lsv_info);
	return(NULL);
    case LANE_CALL_SETUP_RECEIVED:
	break;
    default:
	lsv_crash(lsv_info, "lsv_sigback_listen_send code");
	return(NULL);			/* ERROR!  can't happen */
    }

    /* Handle Call setup received
     */
    if (! lane_addr_valid(&vcc_info->calling_addr)) {
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s no MC send calling party ATM addr",
	    lsv_info->lane_info->name);
	return(NULL);
    }
    if (lsv_info->admin_state  != LSV_LESBUS_ADMIN_UP ||
	(lsv_info->server_state != LSV_LESBUS_OPERATIONAL &&
	 lsv_info->server_state != LSV_LESBUS_PROVISIONAL)) {
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s ignoring MC send connect",
	    lsv_info->lane_info->name);
	return(NULL);
    }
    client = lsv_find_client_primary(lsv_info, 
		&vcc_info->calling_addr);
    if (client == NULL) {
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s MC send from unknown client %Cj",
	    lsv_info->lane_info->name, vcc_info->calling_addr.addr);
	return(NULL);
    }
    if (client->send_vcc.vcc_state != LANE_VCC_INITIALIZED) {
	LES_DBG(lsv_info->lane_info->swidb->namestring,
	    "elan %s client %u duplicate MC send refused",
	    lsv_info->lane_info->name, client->lecid);
	return(NULL);
    }
    if (client->forward_vcc.vcc_state != LANE_VCC_INITIALIZED) {
	return(NULL);
    }

    /* copy given vcc_info to client's send_vcc
     */
    client->send_vcc	      = *vcc_info;
    client->send_vcc.parm     = client;
    client->send_vcc.callback = lsv_signal_send_callback;

    return(&client->send_vcc);
}

/*
** deny client the join request.
*/
static void lsv_deny_client (lsv_client_t *client, paktype *pak, 
    ushort response_status)
{
    lsv_info_t *lsv_info = client->lsv_info;
    lane_join_pdu_t *join;

    if (pak == NULL)
	pak = lsv_getbuffer(lsv_info, sizeof(lane_join_pdu_t));
    if (pak != NULL) {
	join = (lane_join_pdu_t*) pak->datagramstart;
	bcopy(&client->first_join, join, sizeof(lane_join_pdu_t));
	lsv_answer_request(client, pak, response_status);
    }
    validation_destroy(client->validation);
    if (client->client_state != LSV_CLIENT_TERMINATING) {
	lsv_client_new_state(client, LSV_CLIENT_REJECTED);
	mgd_timer_start_jittered(&client->S4_timer,
	    lsv_info->S4_control_timeout*ONESEC, 10);
    }
}

/*
** process the config response which came in as a result
** of the LES trying to validate the FIRST join request.
** In this case, the LES has to respond with an answer
** to the joining client.
*/
static void processCfgRspFirstJoin (lsv_client_t *client,
    paktype *pak)
{
    lane_cfg_pdu_t *config;
    hwaddrtype nsap;
    lsv_info_t *lsv_info = client->lsv_info;
    lane_info_t *lane_info = lsv_info->lane_info;
    char *ifname = lane_info->swidb->namestring;
    char *errstr = NULL;

    LES_DBG(ifname,
	"processing LECS response to client %u join",
	client->lecid);

    config = (lane_cfg_pdu_t*) pak->datagramstart;

    /* downright rejected */
    if (config->hdr.status != LANE_STATUS_SUCCESS) {
	errstr = "rejected by LECS";

    /* mismatched LES address */
    } else if (bcmp(config->target_atm_addr, lsv_info->lsv_nsap.addr,
	     STATIONLEN_ATMNSAP)) {
	errstr = "mismatched LES address";

    /* mismatched elan name */
    } else if (config->name_size &&
	((config->name_size != lane_info->name_size) ||
	 bcmp(config->elan_name, lane_info->name,
		  lane_info->name_size))) {
	errstr = "mismatched elan name";
    }
    bcopy(&client->first_join, config, sizeof(lane_join_pdu_t));
    if (errstr)
    {
	LES_DBG(ifname, "elan %s client %u: %s",
	   lane_info->name, client->lecid, errstr);
	lsv_info->lsvs.lsvInConfigFails += 1;
	lsv_deny_client(client, pak, LANE_STATUS_ACCESS_DENIED);
	return;
    }

    /* If client included a registry pair, register the pair */
    lsv_pack_nsap(config->src_atm_addr, &nsap);
    if (! lane_addr_valid(&nsap)) {
	LES_DBG(ifname, "elan %s client %u Join denied: bad ATM addr",
	   lane_info->name, client->lecid);
	lsv_deny_client(client, pak, 
	    LANE_STATUS_INVALID_ATM_ADDR);
	return;
    }
    if (config->src_lan_dest.tag != LANE_TAG_NOT_PRESENT) {
	if (! lsv_check_lan_dest(&config->src_lan_dest)) {
	    LES_DBG(ifname, 
		"elan %s client %u Join denied: bad LAN dest",
		lane_info->name, client->lecid);
	    lsv_deny_client(client, pak, 
		LANE_STATUS_INVALID_LAN_DEST);
	    return;
	}
	if (lsv_register_lan_dest_nsap(lsv_info, client, 
		&config->src_lan_dest, &nsap, FALSE) != 
		    LANE_STATUS_SUCCESS)
	{
	    LES_DBG(ifname, 
		"elan %s client %u Join denied: duplicate LAN dest",
		lane_info->name, client->lecid);
	    lsv_deny_client(client, pak, LANE_STATUS_DUP_LAN_DEST);
	    return;
	}
    } else if (lsv_primary_nsap_conflict(lsv_info, client, &nsap)) {
	/* If no registry, make sure NSAP doesn't overlap */
	LES_DBG(ifname, 
	    "elan %s client %u Join denied: duplicate ATM addr",
	    lane_info->name, client->lecid);
	lsv_deny_client(client, pak, LANE_STATUS_DUP_ATM_ADDR);
	return;
    }

    /* create client's distribute VCC leaf */
    if (client->distribute_vcc.vcc_state != LANE_VCC_INITIALIZED) {
	LES_DBG(ifname, 
	    "elan %s client %u Join denied: duplicate ATM addr",
	    lane_info->name, client->lecid);
	lsv_deny_client(client, pak, LANE_STATUS_INSUFF_RESOURCE);
	return;
    }
    lsv_setup_distribute_vcc(client, &nsap);

    /* called address better be valid */
    if (! lane_addr_valid(&client->distribute_vcc.called_addr)) {
	LES_DBG(ifname, 
	    "elan %s client %u Join denied: bad distrib addr",
	    lane_info->name, client->lecid);
	lsv_deny_client(client, pak, LANE_STATUS_INVALID_ATM_ADDR);
	return;
    }

    /* Add client leaf to control distribute VCC */
    switch(lane_add(&lsv_info->distribute_vcc, 
	&client->distribute_vcc)) {
	case LANE_SIG_SUCCESS:
	    datagram_done(pak);
	    break;
	case LANE_SIG_FAILURE:
	    LES_DBG(ifname,
		"elan %s client %u Control distribute add failed",
		lane_info->name, client->lecid);
	    lsv_deny_client(client, pak, LANE_STATUS_INSUFF_RESOURCE);
	    break;
	case LANE_SIG_CALLBACK:
	    datagram_done(pak);
	    lsv_client_new_state(client, LSV_CLIENT_ADD_SENT);
	    break;
    }
}

/*
** process the config response which came in as a result
** of the LES trying to RE-validate a client.  This is
** performed by the LES internally without any solicitation
** from the client.  In this case, the LES does NOT have
** to respond to the client.  It will simply leave it alone
** if validation succeeds or kill it otherwise.
*/
static void processCfgRspRevalidation (lsv_client_t *client,
    paktype *pak)
{
    lane_cfg_pdu_t *config;
    lsv_info_t *lsv_info = client->lsv_info;
    lane_info_t *lane_info = lsv_info->lane_info;
    char *ifname = lane_info->swidb->namestring;

    LES_DBG(ifname,
	"processing LECS response to client %u re-validation",
	client->lecid);

    config = (lane_cfg_pdu_t*) pak->datagramstart;

    /* just downright rejection by the LECS */
    if (config->hdr.status != LANE_STATUS_SUCCESS) {
	LES_DBG(ifname,
	    "client %u failed revalidation by the LECS",
	    client->lecid);
	lsv_kill_client(lsv_info, client);
	return;
    }

    /* LECS returns an LES address which is NOT mine */
    if (bcmp(config->target_atm_addr, lsv_info->lsv_nsap.addr,
	     STATIONLEN_ATMNSAP)) {
	LES_DBG(ifname,
	    "client %u failed revalidation, invalid LES address",
	    client->lecid);
	lsv_kill_client(lsv_info, client);
	return;
    }

    /* LECS returns an elan name which is NOT mine */
    if (bcmp(config->elan_name, lane_info->name,
		  lane_info->name_size)) {
	LES_DBG(ifname,
	    "client %u failed revalidation; invalid elan name",
	   client->lecid);
	lsv_kill_client(lsv_info, client);
	return;
    }

    /* if we are here, validation went ok */
    LES_DBG(ifname,
	"client %u passed revalidation",
	client->lecid);
}



/*
** process a config response that came in from the LECS,
** to the config request we sent earlier to verify a client.
*/
static void processCfgRespPacket (lsv_client_t *client,
    paktype *pak)
{
    lsv_info_t *lsv_info = client->lsv_info;
    lane_info_t *lane_info = lsv_info->lane_info;
    char *ifname = lane_info->swidb->namestring;

    if (! pak) return;
    lsv_info->lsvs.lsvInConfigResps += 1;

    switch (client->client_state)
    {
	/* under these conditions, ignore the request */
	case LSV_CLIENT_INITIALIZED:
	case LSV_CLIENT_CONNECTED:
	case LSV_CLIENT_REJECTED:
	case LSV_CLIENT_TERMINATING:
	    LES_DBG(ifname, 
		"elan %s client %u Join request ignored",
		lane_info->name, client->lecid);
	    datagram_done(pak);
	    return;

	/* we are here becoz of the first join request */
	case LSV_CLIENT_JOIN_RECEIVED:
	    processCfgRspFirstJoin(client, pak);
	    break;

	/* if we are here, this must be a RE-validation */
	case LSV_CLIENT_ADD_SENT:
	case LSV_CLIENT_JOINED:
	case LSV_CLIENT_OPERATIONAL:
	    processCfgRspRevalidation(client, pak);
	    datagram_done(pak);
	    break;
    }
}

/*
** lsv_config_resp_timeout - A configure response timed out
*/
static void lsv_config_resp_timeout (lsv_client_t *client)
{
    lsv_info_t *lsv_info;

    if (client == NULL) return;
    lsv_info = client->lsv_info;
    switch (client->client_state) {
	case LSV_CLIENT_INITIALIZED:
	case LSV_CLIENT_CONNECTED:
	case LSV_CLIENT_REJECTED:
	    lsv_crash(lsv_info, "lsv_config_resp_timeout");
	    break;
	case LSV_CLIENT_JOIN_RECEIVED:
	case LSV_CLIENT_ADD_SENT:
	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s client %u Join response timeout",
		lsv_info->lane_info->name, client->lecid);

	    /* an LES cannot send "LANE_STATUS_NO_CONFIG" */
	    lsv_deny_client(client, NULL, LANE_STATUS_ACCESS_DENIED);
	    break;

	case LSV_CLIENT_JOINED:
	case LSV_CLIENT_OPERATIONAL:
	case LSV_CLIENT_TERMINATING:
	    LES_DBG(lsv_info->lane_info->swidb->namestring,
		"elan %s client %u validation response timeout",
		lsv_info->lane_info->name, client->lecid);
	    lsv_kill_client(lsv_info, client);
	    break;
    }
}

/*
** find the ClientValidation_t record corresponding to
** the transaction id received from the LECS response.
** Note that initially, this was loaded with the pointer
** value of the record itself.  So, it is a very simple
** direct extraction from the "clientList".
*/
static lsv_client_t *tid2client (lsv_info_t *lsv_info, ulong tid)
{ 
    return 
	Index_searchElement
	    (lsv_info->clientList, (void*) tid, NULL); 
}

/*
** process a config response that came in from the LECS
** to the client verification request that we sent earlier.
** Check that the config response came over the LECS access
** vcd and not over something else.
*/
void lsv_process_config_resp (paktype *pak, 
    lsv_info_t *lsv_info)
{
    lsv_client_t *client;
    char *ifname;
    lane_cfg_pdu_t *cfg;

    if (pak == NULL) return;
    if (lsv_info == NULL) return;
    ifname = lsv_info->lane_info->swidb->namestring;

    /* make sure that the LECS call id is still there */
    if (! lsv_info->lecsAccess.callId) {
	LES_DBG(ifname, "config response packet on NULL vc");
	datagram_done(pak);
	return;
    }

    /* 
    ** make sure that the config response arrived
    ** from this swidb's LECS and not just arbitrarily
    ** from somewhere or something else.
    */
    if (pak->vcd != atmSig_api_callId2vcd
			(lsv_info->lecsAccess.callId)) {
	LES_DBG(ifname,
	    "config response packet on unexpected vcd %u",
	    pak->vcd);
	datagram_done(pak);
	return;
    }

    /* find the corresponding client for this transaction */
    cfg = (lane_cfg_pdu_t*) pak->datagramstart;
    client = tid2client(lsv_info, cfg->hdr.transaction_id);

    /* if none found, drop packet and return */
    if (client == NULL) {
	LES_DBG(ifname, 
	    "unmatched client 0x%x in config response packet",
	    cfg->hdr.transaction_id);
	datagram_done(pak);
	return;
    }

    /* if we are here we found a match */
    LES_DBG(ifname, "found a match for the config response");

    /* we dont need this anymore */
    validation_destroy(client->validation);

    /* process the config response packet */
    processCfgRespPacket(client, pak);
}

/*
** derives a config request lane packet, given a
** join request lane packet.  The transaction id
** is faked as the pointer to the corresponding
** client record so that the client can immediately
** be located when the response comes back.
*/
static void join_to_configrequest (lane_join_pdu_t *join,
    lane_cfg_pdu_t *cfg, lsv_client_t *client)
{
    bcopy(join, cfg, sizeof(lane_cfg_pdu_t));
    cfg->hdr.opcode = LANE_CONFIG_REQ;
    cfg->hdr.transaction_id = (ulong) client;
}

/*
** how long the LES will wait for a response (either positive 
** or negative) from the LECS, once it has sent a validation
** request.
*/
#define VALIDATION_TIMEOUT	(30 * ONESEC)

/*
** send a config request to the LECS to validate
** a client who is wishing to join the elan.
** Note that the original join request is already
** captured in the "first_join" field of the client.
*/
static void validate_client (lsv_client_t *client, 
    boolean firstJoin, boolean *bufferStarvationOccured)
{
    lsv_info_t *lsv_info;
    lane_info_t *lane_info;
    idbtype *swidb;
    paktype *pak;
    lane_cfg_pdu_t *cfg;
    char *ifname;
    ClientValidation_t *cv;
    int vcd;

    if (bufferStarvationOccured != NULL)
	*bufferStarvationOccured = FALSE;
    if (client == NULL) return;

    /* ignore terminating client */
    if (client->client_state == LSV_CLIENT_TERMINATING) return;

    lsv_info = client->lsv_info;
    lane_info = lsv_info->lane_info;
    swidb = lane_info->swidb;
    ifname = swidb->namestring;

    /* if LECS connection is down, not much we can do */
    if (! lsv_info->lecsAccess.connected) {
	LES_DBG(ifname, "oops, not connected to the LECS yet");

/*
** leave the pending join request hanging, with the hope that
** when the LECS connection is established, revalidating of
** all the clients will take care of it.
*/
#if 0
	if (firstJoin)
	    lsv_deny_client(client, NULL, 
		LANE_STATUS_INSUFF_RESOURCE);
	else
	    lsv_kill_client(lsv_info, client);
#endif

	/* 
	** do a favor and start the master LECS 
	** connection attempts going if NOT already
	** being done.
	*/
	if (! lsv_info->lecsAccess.callId)
	    lsv_start_lecs_connection_procedures(lsv_info);
	return;
    }

    /*
    ** if a validation for this client is
    ** already outstanding, ignore this request.
    */
    if (client->validation != NULL) return;

    /* prepare a config request packet to ask the LECS */
    pak = lsv_getbuffer(lsv_info, sizeof(lane_cfg_pdu_t));
    if (! pak) {
	LES_DBG(ifname, 
	    "no buffer space left, cannot validate client %u",
	    client->lecid);
	if (bufferStarvationOccured != NULL)
	    *bufferStarvationOccured = TRUE;
	if (firstJoin)
	    lsv_deny_client(client, NULL, 
		LANE_STATUS_INSUFF_RESOURCE);
#if 0
	else
	    lsv_kill_client(lsv_info, client);
#endif
	return;
    }
    cfg = (lane_cfg_pdu_t*) pak->datagramstart;

    /*
    ** create a validation record to match up when the 
    ** config response from the LECS comes back.
    */
    cv = validation_alloc(lsv_info, client);
    if (cv == NULL) {
	LES_DBG(ifname, 
	    "no buffer space left, validation_alloc failed");
	if (firstJoin)
	    lsv_deny_client(client, NULL, 
		LANE_STATUS_INSUFF_RESOURCE);
	else
	    lsv_kill_client(lsv_info, client);
	datagram_done(pak);
	return;
    }

    /*
    ** prepare a config request packet 
    ** derived from the "first_join" packet.
    */
    join_to_configrequest(&client->first_join, cfg, client);

    /*
    ** send packet out and either wait until config 
    ** response comes back or the request times out.
    */
    pak->if_output = swidb;
    vcd = atmSig_api_callId2vcd(lsv_info->lecsAccess.callId);
    if (lane_control_encap(pak, vcd)) {
	LES_DBG(ifname, 
	    "sending conf request to the LECS to validate "
	    "client %u for %s",
	    client->lecid,
	    firstJoin ? "join" : "revalidation");
	datagram_out(pak);
	lsv_info->lsvs.lsvOutConfigReqs += 1;
	mgd_timer_start(&cv->timer, VALIDATION_TIMEOUT);
    } else {
	LES_ERR("interface %s: validate_client: "
	    "lane_control_encap failed for vcd %u",
	    ifname, vcd);
	datagram_done(pak);
	if (firstJoin)
	    lsv_deny_client(client, NULL, 
		LANE_STATUS_INSUFF_RESOURCE);
	else
	    lsv_kill_client(lsv_info, client);
    }
}

/*
** Note that since we allow a context change in this
** routine to avoid buffer starvation, both the clients
** and/or the lsv_info CAN disappear from under us.
** So, code it very carefully.
**
** Note that we attempt to validate only the clients 
** who are awaiting a join response or have already
** fully joined.
*/
static void validate_all_clients (lsv_info_t *lsv_info)
{
    lsv_client_t *client;
    boolean bufferStarved;
    int count;
    boolean firstJoin;

    if (lsv_info == NULL) return;
    lsv_info->beingValidated = lsv_info->client_table;
    count = 0;
    while (TRUE) {

    NEXT_CLIENT:

	client = lsv_info->beingValidated;
	if (client == NULL) break;
	lsv_info->beingValidated = client->next;

	/* is the client doing the FIRST join attempt ? */
	firstJoin = client->client_state == LSV_CLIENT_JOIN_RECEIVED;

	/*
	** only currently joining or already fully 
	** operational clients should be re-verified.
	*/
	if ((client->client_state != LSV_CLIENT_OPERATIONAL) &&
	    !firstJoin)
		goto NEXT_CLIENT;

    VALIDATE_SAME_CLIENT:

	/* 
	** context switches when transmit buffer starvation 
	** occurs or PAK_SWITCHCOUNT validations have been 
	** performed.  Also, make sure that things have not 
	** disappeared from under us when we come back from 
	** the context change.
	*/
	validate_client(client, firstJoin, &bufferStarved);
	count++;
	if (bufferStarved || (count >= PAK_SWITCHCOUNT)) {
	    if (count >= PAK_SWITCHCOUNT) count = 0;
	    process_may_suspend();

	    /* if lsv_info is gone, do not continue */
	    if (! valid_lesbus(lsv_info)) break;
	    if (lsv_info->server_state != LSV_LESBUS_OPERATIONAL)
		break;

	    /* if client is still here, try re-validating */
	    if (Index_searchElement(lsv_info->clientList, client,
		    NULL) == client)
			goto VALIDATE_SAME_CLIENT;
	}
    }
}

/*
** handle client validation timeouts.  This means
** that the LECS did NOT respond to our validation
** config request packet in time.
*/
void lsv_client_validation_timeout (ClientValidation_t *cv)
{
    lsv_client_t *client;

    /* do NOT change the statement ordering below */
    if (cv == NULL) return;
    client = cv->client;
    validation_destroy(cv);
    if (client == NULL) return;
    lsv_config_resp_timeout(client);
}

/*
** process an incoming join request
*/
void lsv_process_join (paktype *pak, lsv_info_t *lsv_info)
{
    lane_join_pdu_t *join;
    lane_info_t *lane_info;
    lsv_client_t *client;
    char *ifname;
    boolean error;

    join = (lane_join_pdu_t*) pak->datagramstart;
    lane_info = lsv_info->lane_info;
    ifname = lane_info->swidb->namestring;
    client = lsv_vcd_to_client(lsv_info, pak->vcd);
    if (client == NULL) {
	LES_DBG(ifname, "elan %s Join request from unknown vcd %u",
	    lane_info->name, pak->vcd);
	datagram_done(pak);
	return;
    }

    switch (client->client_state) {
	/* shouldn't happen */
	case LSV_CLIENT_INITIALIZED:	/* shouldn't happen */
	    lsv_crash(lsv_info, "lsv_process_join state");
	    return;

	/* ignore join during termination */
	case LSV_CLIENT_TERMINATING:
	    LES_DBG(ifname,
		"elan %s client %d Join ignored: client terminating",
		lane_info->name, client->lecid);
	    datagram_done(pak);
	    break;

	case LSV_CLIENT_CONNECTED:
	    /* If LES/BUS going down, discard this request */
	    if (lsv_info->admin_state != LSV_LESBUS_ADMIN_UP) {
		LES_DBG(ifname, 
		    "elan %s client %d Join ignored: LES down",
		    lane_info->name, client->lecid);
		datagram_done(pak);
		break;
	    }

	    /*
	    ** Normal case for Join request.  Save this 
	    ** join request and check its validity.
	    */
	    bcopy(join, &client->first_join, sizeof(lane_join_pdu_t));

	    /* preliminary checks */
	    error = FALSE;

	    /* LECID must be 0 */
	    if (join->hdr.requestor_lecid) {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: bad LECID",
		    lane_info->name, client->lecid);
		lsv_deny_client(client, pak, 
		    LANE_STATUS_INVALID_REQ_LECID);
		break;
	    }
	    /* we cannot handle TLV's */
	    if (join->num_tlvs) 
	    {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: TLVs included",
		    lane_info->name, client->lecid);
		error = TRUE;
		goto ERROR;
	    }
	    /* bad lan type ? */
	    if (join->lan_type != LANE_TYPE_UNSPECIFIED &&
		 join->lan_type != lane_info->type) 
	    {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: bad lan type",
		    lane_info->name, client->lecid);
		error = TRUE;
		goto ERROR;
	    }
	    /* bad mtu size ? */
	    if (join->max_frame_size != LANE_MAX_FRAME_UNSPECIFIED &&
		join->max_frame_size < lane_info->max_data_frame)
	    {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: bad mtu size",
		    lane_info->name, client->lecid);
		error = TRUE;
		goto ERROR;
	    }
	    /* elan names must match if specified */
	    if (join->name_size > 0)
	    {
		if (join->name_size != lane_info->name_size ||
		    bcmp(join->elan_name, lane_info->name,
			join->name_size))
		{
		    LES_DBG(ifname, 
			"elan %s client %d Join reject: bad lan name",
			lane_info->name, client->lecid);
		    error = TRUE;
		    goto ERROR;
		}
	    }
	    /* calling party addr MUST match join src atm addr */
	    if (bcmp(join->src_atm_addr, 
		    client->direct_vcc.calling_addr.addr,
		    STATIONLEN_ATMNSAP) != 0)
	    {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: invalid addr",
		    lane_info->name, client->lecid);
		error = TRUE;
		goto ERROR;
	    }
	ERROR:
	    if (error) {
		lsv_deny_client(client, pak, 
		    LANE_STATUS_INVALID_REQ_PARMS);
		break;
	    }
	    
	    /* if we are here, preliminary checks have passed */

	    lsv_client_new_state(client, LSV_CLIENT_JOIN_RECEIVED);
	    LES_DBG(ifname, 
		"elan %s client %d sending config request",
		lane_info->name, client->lecid);

	    /* validate the client with the LECS */
	    validate_client(client, TRUE, NULL);

	    /* we are done */
	    datagram_done(pak);
	    break;

	case LSV_CLIENT_JOINED:
	case LSV_CLIENT_OPERATIONAL:
	case LSV_CLIENT_JOIN_RECEIVED:
	case LSV_CLIENT_ADD_SENT:

	    /* Join is OK as long as it matches old request */
	    if (join->hdr.requestor_lecid) {
		LES_DBG(ifname,
		    "elan %s client %d Join reject: bad LECID",
		    lane_info->name, client->lecid);
		lsv_deny_client(client, pak, 
		    LANE_STATUS_INVALID_REQ_LECID);
		break;
	    }
	    if (bcmp(join->src_atm_addr, 
		    client->first_join.src_atm_addr, 
		    STATIONLEN_ATMNSAP) || 
		join->hdr.flags != client->first_join.hdr.flags	||
		join->lan_type != client->first_join.lan_type ||
		bcmp(&join->src_lan_dest, 
		    &client->first_join.src_lan_dest,
		     sizeof(lane_dest_t)) ||
		join->max_frame_size != 
		    client->first_join.max_frame_size ||
		join->num_tlvs != client->first_join.num_tlvs ||
		join->name_size != client->first_join.name_size	||
		(join->name_size  &&
		 bcmp(join->elan_name, client->first_join.elan_name,
		    join->name_size)))
	    {
		LES_DBG(ifname, 
		    "elan %s client %d Join reject: join mismatch",
		    lane_info->name, client->lecid);
		lsv_deny_client(client, pak, 
		    LANE_STATUS_INVALID_REQ_PARMS);
		break;
	    }

	    /* Accept latest Join as the one that we'll respond */
	    client->first_join.hdr.transaction_id = 
		join->hdr.transaction_id;

	    /*
	    ** Don't respond if we're still working 
	    ** on the original join request.
	    */
	    if (client->client_state == LSV_CLIENT_JOIN_RECEIVED ||
		client->client_state == LSV_CLIENT_ADD_SENT)
	    {
		LES_DBG(ifname,
		    "elan %s client %d good re-Join ignored",
		    lane_info->name, client->lecid);
		datagram_done(pak);
		break;
	    }

	    /*
	    ** already joined, or even operational.  
	    ** Send the response.
	    */
	    join->lan_type = lane_info->type;
	    join->max_frame_size = lane_info->max_data_frame;
	    join->name_size = lane_info->name_size;
	    join->hdr.requestor_lecid = client->lecid;
	    bcopy(lane_info->name, join->elan_name, 
		lane_info->name_size);
	    LES_DBG(ifname, "elan %s client %d re-Join success",
		lane_info->name, client->lecid);
	    lsv_answer_request(client, pak, LANE_STATUS_SUCCESS);
	    break;

	case LSV_CLIENT_REJECTED:
	    LES_DBG(ifname, 
		"elan %s client %d Join reject: old reject",
		lane_info->name, client->lecid);
	    lsv_deny_client(client, pak, LANE_STATUS_ACCESS_DENIED);
	    break;

	default:
	    lane_router_crash("lsv_process_join");
    }
}

/*
** lsv_client_timed_out - Handle a client whose timer has expired
*/
void lsv_client_timed_out (lsv_client_t *client)
{
    mgd_timer_stop(&client->S4_timer);
    switch (client->client_state) 
    {
	case LSV_CLIENT_INITIALIZED:
	case LSV_CLIENT_JOINED:
	case LSV_CLIENT_OPERATIONAL:
	    lsv_crash(client->lsv_info, "lsv_client_timed_out");
	    break;
	case LSV_CLIENT_CONNECTED:
	case LSV_CLIENT_JOIN_RECEIVED:
	case LSV_CLIENT_ADD_SENT:
	case LSV_CLIENT_REJECTED:
	    LES_DBG(client->lsv_info->lane_info->swidb->namestring,
		"elan %s client %d control timer expired",
		client->lsv_info->lane_info->name, client->lecid);
	    lsv_kill_client(client->lsv_info, client);
	    break;
	case LSV_CLIENT_TERMINATING:
	    break;
    }
}

