/* $Id: isdn.c,v 3.18.12.31 1996/08/29 22:22:10 gtaylor Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn.c,v $
 *------------------------------------------------------------------
 * isdn.c -- Routines for dealing with ISDN ports.
 *
 * 15-Jan-92 - William Miskovetz
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn.c,v $
 * Revision 3.18.12.31  1996/08/29  22:22:10  gtaylor
 * CSCdi36917: Japanese require only 2 calls to the same number in 3 mins
 * Branch: California_branch
 *
 * Revision 3.18.12.30  1996/08/28  13:21:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.18.12.29  1996/08/26  23:08:57  wmay
 * CSCdi66907:  ISDN incoming calls getting rejected - enforce call ranges
 * for modem and dialer type calls
 * Branch: California_branch
 *
 * Revision 3.18.12.28  1996/08/23  23:08:48  wmay
 * CSCdi66999:  b channel specified in leased line causes crash - do not
 * allow it.
 * Branch: California_branch
 *
 * Revision 3.18.12.27  1996/08/23  04:25:34  fox
 * CSCdi66589:  ppp_periodic() dereferences null pointer - idb->lcpfsm
 * during init
 * Branch: California_branch
 * Disable interrupts in isdn_setencap(), dialer_setencap() and
 * async_setencap() before changing encapsulations.  Make sure that
 * lcpfsm pointer in idb is set before idb vectors (such as periodic)
 * are set.
 *
 * Revision 3.18.12.26  1996/08/06  17:35:04  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.18.12.25  1996/08/06  00:37:49  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.18.12.24  1996/08/04  08:09:46  fox
 * CSCdi62182:  %SCHED-3-PAGEZERO: Low memory modified by ISDN (0x74 =
 * 0x0).
 * Branch: California_branch
 * Convert ddb->dial_reason to an array of char from a char *.  Modify all
 * necessary references appropriately.
 *
 * Revision 3.18.12.23  1996/07/28  07:33:09  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.18.12.22  1996/07/22  23:44:25  wmay
 * CSCdi63517:  show dialer is ugly - don't show dialer stats for bri's
 * that are members of a rotary group - the rotary group master does.
 * Branch: California_branch
 *
 * Revision 3.18.12.21  1996/07/19  19:50:40  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.18.12.20  1996/07/18  21:57:18  snyder
 * CSCdi63402:  get isdn table out of data space and into text space
 *              84 more bytes out of data into text
 * Branch: California_branch
 *
 * Revision 3.18.12.19  1996/07/16  18:09:03  wmay
 * CSCdi61965:  sh dialer on ISDN interfaces doesnt display all B Channels
 * Set up the hunt groups of b-channels for PRI and BRI in interface order-
 * display the entire hunt group when doing a show dialer interface for
 * a hunt group master
 * Branch: California_branch
 *
 * Revision 3.18.12.18  1996/06/18  01:39:44  suresh
 * CSCdi46872:  Dialer load balancing/multilink create to groups when 2
 * numbers dial
 * Branch: California_branch
 *
 * Revamped the way dialer groups are created to do correct
 * dialer load balancin
 *
 * Revision 3.18.12.17  1996/06/16  21:21:01  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.18.12.16  1996/06/12  16:52:24  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special code for BRI.
 * Branch: California_branch
 *
 * Revision 3.18.12.15  1996/05/19  03:14:04  hrobison
 * CSCdi57959:  Incorrect show version output
 * Branch: California_branch
 * 'show version' on routers with no ISDN hardware were
 * displaying PRI software banner.
 *
 * Revision 3.18.12.14  1996/05/19  03:10:49  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.18.12.13  1996/05/10  01:44:59  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.18.12.12  1996/05/07  04:14:58  hrobison
 * CSCdi56472:  CSCdi46413 broke modularity if ISDN not included
 * Branch: California_branch
 * Quick fix: Change external reference to structure in ../isdn to
 * a registry call.  Code is scheduled for clean-up in CSCdi56813.
 *
 * Revision 3.18.12.11  1996/05/01  14:52:48  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.18.12.10  1996/05/01  14:33:31  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.18.12.9  1996/05/01  05:08:24  dblair
 * CSCdi56308:  Redundant code in isdn.c after Dial1_branch sync and commit
 * Cleaning up.
 * Branch: California_branch
 *
 * Revision 3.18.12.8  1996/05/01  04:38:46  dblair
 * CSCdi55760:  7xxx crash during unconfig of Dialer maps testing over
 * ISDN PRI
 * Branch: California_branch
 *
 * Revision 3.18.12.7  1996/05/01  03:54:02  syiu
 * CSCdi56141:  Freeing unassigned memory through isdn_call_disconnect()
 * Branch: California_branch
 *
 * Revision 3.18.12.6  1996/05/01  01:21:57  suresh
 * CSCdi56141:  ppp dialer timeout on isdn crashes
 * Branch: California_branch
 * call_id_string->dial_reason gets freed twice. Fixed isdn_call_connect()
 *
 * Revision 3.18.12.5  1996/04/27  06:36:51  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.18.12.4  1996/04/20  17:59:23  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.12.2.5  1996/04/25  23:58:58  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.12.2.4  1996/04/23  00:24:31  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.12.2.3  1996/04/17  00:01:20  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.12.2.2  1996/02/23  21:31:41  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.12.2.1  1996/01/16  15:45:17  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.18.12.3  1996/04/13  03:55:20  rbeach
 * CSCdi50619:  Dialer tries to open third channel on same BRI
 * interface. This is caused if an orphaned call leaves the
 * wait for carrier timer running. When the timer fires num_avail_b
 * gets incremented to 3. The wait for carrier timer is now
 * stopped in this case.
 * Branch: California_branch
 *
 * Revision 3.18.12.2  1996/04/12  02:30:55  bdas
 * CSCdi53500:  SNMP output byte counts differ ISDN Primary Rate interface
 * stats
 * Branch: California_branch
 *
 * Revision 3.18.12.1  1996/03/18  22:47:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.6.4  1996/03/16  07:55:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.12.6.3  1996/03/13  02:12:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.12.6.2  1996/03/07  11:07:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.12.6.1  1996/02/20  21:51:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.18  1996/02/15  18:03:10  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.17  1996/02/07  16:17:28  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.16  1996/02/01  06:12:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.15  1996/01/29  07:31:19  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.14  1996/01/26  01:31:06  hrobison
 * CSCdi43998:  BRI COMMANDS ARE NOT RECOGNIZED IN UNITS WITH MBRI &
 * CE1/CT1
 * Move increment of nprinets so that this only happens if
 * PRI service is configured.  Previously nprinets was
 * incremented if a 4XXX CT1/PRI or CE1/PRI, or 7XXX MIP
 * was installed even if no pri-group was configured.
 * This prevented channelized service from working in
 * the same router with BRI service.  Parser checks for
 * nprinets have been changed to check for either
 * nbrinets or (nt1ports, ne1ports, nst1ports or
 * nse1ports).
 *
 * Revision 3.13  1996/01/25  11:23:30  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.12  1996/01/13  01:35:05  irfan
 * CSCdi44881:  dialer interface dead after removal and redefine
 * disallow removal of dialer interface as long as it has members in
 * the rotary group.
 *
 * Revision 3.11  1996/01/12  16:48:09  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.10  1996/01/05  10:18:36  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.9  1995/12/21  20:06:52  dblair
 * CSCdi43794:  Clearing a dialer Async interface prevents new calls from
 * being made
 *
 * Revision 3.8  1995/12/21  00:02:39  lbustini
 * CSCdi42098:  BRI inidcates no free dialer when neither channel is in use
 * When clearing the call table, update the number of available B channels
 * accordingly.
 *
 * Revision 3.7  1995/12/19  18:51:18  dclare
 * CSCdi44676:  ISDN PRI will not accept call setup, Responds with Shutting
 * down ME. The BRI and PRI need differentiation in the isdn_cstate
 * registry.
 *
 * Revision 3.6  1995/12/12  19:59:00  lbustini
 * CSCdi45296:  DDR brings up second ISDN link when call already in
 * progress
 * Now that PPP line protocol comes up after authentication, extend
 * call pending state to cover the additional delay.
 *
 * Revision 3.5  1995/11/29  01:59:06  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.4  1995/11/20  22:28:36  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:02:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/11/08  21:35:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.19  1995/10/26  13:37:50  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Make warning message stand out a little clearer by adding a crlf to
 * the end in addition to the one at the beginning.
 *
 * Revision 2.18  1995/10/25  04:31:28  rbeach
 * CSCdi42764:  dialer string cause router crash. If no dialer
 * map is used and just a dialer string is used an access to
 * map->cip may cause a crash.
 *
 * Revision 2.17  1995/10/24  23:54:57  dclare
 * CSCdi42565:  ISDN crashes with an out of CCB memory blocks fatal error.
 * Caused by Call_ID being reinited to 0, now static and global. Each call
 * id to the isdn code must be unique.
 *
 * Revision 2.16  1995/10/23  18:45:56  lbustini
 * CSCdi39833:  dialer priority can not be set on individual interfaces
 * Only ISDN interfaces copy the dialer priority from the rotary group
 * leader to the members.
 *
 * Revision 2.15  1995/10/21  06:37:45  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.14  1995/10/17  22:18:40  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.13  1995/09/30  05:06:25  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.12  1995/09/13  03:54:38  rbeach
 * CSCdi37866:  Spurious access recorded when isdn PRI deconfigured.
 * There is a race condition during PRI deconfiguration. The isdn_info
 * structure may have been freed while we cleanup after deconfig.
 * This avoids spurious accesses.
 *
 * Revision 2.11  1995/08/21  22:54:32  lbustini
 * CSCdi39089:  CLNS DDR causes router to reload
 * Ensure that dialing cause does not exceed string size.
 *
 * Revision 2.10  1995/08/12  04:21:51  bdas
 * CSCdi36875:  Need to provide a show isdn history command for call
 * history mib
 *
 * Revision 2.9  1995/08/07  20:32:35  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.8  1995/07/27  22:49:20  lbustini
 * CSCdi34784:  Trying to bring up 3rd B channel with load threshold cmd
 * Number of available B channels should not be changed for no good reason.
 *
 * Revision 2.7  1995/07/21  01:46:56  bdas
 * CSCdi36879:  Need to provide dialing reason in the call history mib.
 *
 * Revision 2.6  1995/06/26  18:03:14  dclare
 * CSCdi36415:  Addition of Calling number for Australia ISDN PRI (TS014).
 *
 * Revision 2.5  1995/06/20  23:33:08  bdas
 * CSCdi36012:  shutting down BRI doesnt bring the B-channels
 * administratively down
 *
 * Revision 2.4  1995/06/20  20:38:14  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/15  23:09:32  bdas
 * CSCdi33538:  incomplete/wrong object instances returned; ISDN MIB,
 * Call History Mib.
 *
 * Revision 2.2  1995/06/09  20:59:46  rbeach
 * CSCdi32791:  BRI/MBRI interfaces dont allow more than 64 isdn caller
 * entries. Make the isdn caller entries dynamically allocated. Dialer
 * and ISDN call screening now share the same mechanism and code.
 *
 * Revision 2.1  1995/06/07  23:18:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------ 
 * $Endlog$ 
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "sys_registry.h"
#include "dialer_registry.h"
#include "media_registry.h"
#include "../ui/debug.h"
#include "parser_defs_isdn.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../if/network.h"
#include "../if/static_map.h"
#include "serial.h"
#include "address.h"
#include "dialer.h"
#include "dialer_private.h"
#include "dialer_debug.h"
#include "../les/isdn_bri.h"
#include "isdn.h"
#include "../isdn/switch_def.h"
#include "../isdn/tune.h"
#include "../isdn/gendef.h"
#include "../isdn/gp.h"
#include "../wan/serial_debug.h"
#include "../wan/isdn_debug.h"
#include "../x25/lapb.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "mgd_timers.h"
#include "../isdn/isdn_registry.h"
#include "../isdn/isdn_registry.regc"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../isdn/ccie.h"
#include "../os/clock.h"
#include "../les/isdn_bri.h"
#include "../les/if_les_bri.h"
#include "../isdn/msg_isdn.c"
#include "cs_registry.regh"

extern int nt1ports, ne1ports, nst1ports, nse1ports;
extern forktype isdnmain(void);
extern void isdn_bri_enable_layer1_Q(void);
extern hwidbtype *isdn_idblist[MAX_DSL];

static void isdn_clear_ntt_next_call_chk_time(hwidbtype *, char *);

int isdn_fork;
int isdn_pu_establish = TRUE;
long isdn_regcode;
static ushort Call_ID;     /* Call reference for outgoing ISDN calls */

/*
 * isdn_call_connect
 * A B channel has connected, so do the appropriate work
 */
void isdn_call_connect (hwidbtype *hwidb)
{
    isdn_bri_t *isdn_info;
    isdn_bri_t *d_isdn_info;
    call_id_string_t *tmp;
    call_id_string_t *prev;
    hwidbtype *master_idb;
    dialerdbtype *ddb;
    dialerdbtype *masterddb;

    ddb = get_dialerdb(hwidb);
    isdn_info = hwidb->isdn_info;
    /*
     * If this isn't a B channel, just exit.  But leave a debug in here
     * so we can verify...
     */
    if (isdn_info->chantype == ISDN_Dchan) {
	if (dialer_debug)
            buginf("\n%s: connect called for something other than B channel", 
	            hwidb->hw_namestring);
	return;
    }

    hwidb->oldstate = hwidb->state;
    GET_TIMESTAMP(hwidb->state_time);
    hwidb->state = IDBS_UP;
    master_idb = get_netidb(hwidb);
    hwidb->snmp_trap_nolink = master_idb->snmp_trap_nolink;
    if (hwidb->state != hwidb->oldstate) {
	hwidb->reason = print_hwidbstate(hwidb);
        switch (hwidb->state) {
	  case IDBS_DOWN: 
	  case IDBS_UP: 
	    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(master_idb)) {
		errmsg(&msgsym(UPDOWN, LINK), hwidb->hw_namestring,
		       hwidb->reason);
	    }
	    break;
	  default:
	    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(master_idb)) {
		errmsg(&msgsym(CHANGED, LINK), hwidb->hw_namestring,
		       hwidb->reason);
	    }
	    break;
        }
	reg_invoke_ifmib_link_trap(hwidb->state == IDBS_UP ?
		LINK_UP_TRAP : LINK_DOWN_TRAP, hwidb);
    } else {
        return; /* we've already been called and told we are up */
    }

    d_isdn_info = isdn_info->d_chan_idb->isdn_info;

    /*
     * Ok, one of the B channels has connected.  First, look
     * through the list of calls we have pending, and see if
     * the call id of this call matches the call id of one of
     * the calls we have placed.  If so, then initialize the idb
     * with info that usually would have been set up at call time,
     * except we didn't know which B channel to use.
     */

    tmp = d_isdn_info->call_tbl;
    prev = NULL;

    /*
     * walk down the linked list looking for the call id
     */

    while (tmp) {
        if (tmp->call_id == isdn_info->call_id)
	    break;
	else {
	    prev = tmp;
	    tmp = tmp->next;
        }
    }

    /*
     * If we found an entry, it means that a call we placed is
     * connected.  If we didn't find an entry, either this is
     * an incoming call, or it is a call we placed, but the
     * wait for carrier timer expired before the call
     * completed.  If the latter, we handle just like the former,
     * but it will be hung up.  There is a race condition here...
     */
            
    if (tmp) {
        destroy_timer(tmp->carrier_timer);

	/*
	 * Take care of dialer data here.
	 */
	dialer_set_number_called(ddb, tmp->called_num);
	/*
	 * Ideally we should lookup class and *set* all parameters
	 * for now we will explicitly pullin timers only
	 */
	masterddb = get_dialerdb(master_idb);
	ddb->dialer_idle_ticks = masterddb->dialer_idle_ticks;
	ddb->dialer_fast_idle_ticks = masterddb->dialer_fast_idle_ticks;
	ddb->dialer_enable_ticks = masterddb->dialer_enable_ticks;
	ddb->dialer_carrier_ticks = masterddb->dialer_carrier_ticks;
	ddb->dialer_holdq_ticks = masterddb->dialer_holdq_ticks;
	ddb->dialer_call_group = tmp->call_group;
	if (ddb->dialer_holdQ) {
	    if (dialer_holdq_debug)
		buginf("\n%s - already had holdq - overwriting",
		       ddb->ownidb->hw_namestring);
	}
	ddb->dialer_holdQ = tmp->holdq_ptr;
	tmp->holdq_ptr = NULL;
	ddb->dialer_stats = tmp->stats;
	sstrncpy(ddb->dial_reason, tmp->dial_reason, MAXSTR);
	ddb->dialing_ddb = tmp->netddb;
       /*
        * Don't need to call dialer_increment_call() since
        * this is handled by dial_if().
        */
	dialer_out_call_connected(ddb);

	/*
	 * Take care of ISDN data here.
	 */
	if (isdn_info->remote_number)
	    free(isdn_info->remote_number);
	isdn_info->remote_number = strdup(ddb->number_called);
	isdn_info->remote_src = ISDN_SRC_DIALER;
	isdn_info->speed = tmp->speed;

        /*
         * Unlink element from the list and free it.
         */
        if (prev) {
            prev->next = tmp->next;
        } else {
            d_isdn_info->call_tbl = tmp->next;
        }
	/*
	 * start the idle timer...
	 */
	TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
	ddb->dialer_in_use = TRUE;
        GET_TIMESTAMP(isdn_info->connect_time);
	GET_TIMESTAMP(ddb->mru_callsuccess);	/* for get_free_dialer */

	sstrncpy(hwidb->dialerdb->dial_reason, tmp->dial_reason, MAXSTR);
        isdn_info->outgoing = TRUE;
        current_time_string(isdn_info->connect_tod);
        isdn_info->tx_packets = idb_outputs(hwidb);
        isdn_info->tx_bytes = idb_tx_cumbytes(hwidb);
        isdn_info->rx_packets = idb_inputs(hwidb);
        isdn_info->rx_bytes = idb_rx_cumbytes(hwidb);
        isdn_mib_update(hwidb, NULL, TRUE);

        /*
         * This timer is used for NTT switches only. Checks that a number which
         * is failing to connect isn't called more than twice in 3 minutes.
         * Homologation requirement for NTT.
         */
        isdn_clear_ntt_next_call_chk_time(hwidb, tmp->called_num);

        free(tmp);
    } else if (isdn_info->call_id & 0x8000) {
	/*
	 * This is a call with a call_id that appears to have 
	 * been assigned by us.  Just hang it up, just in case.
	 * If we hang it up here, and we hung it up because the
	 * wait for carrier timer expired, telenetworks will
	 * complain, but not in a fatal way.
	 */
        isdn_fromrouter(ISDN_HANGUP, isdn_info->call_id, NULL, 0, 
		isdn_info->ces, hwidb, isdn_info->spc);
        if (dialer_debug)
	    buginf("\n%s: Expired call id received.  id = 0x%x", 
			hwidb->hw_namestring, isdn_info->call_id);
    } else {
	/*
	 * must be an incoming call...
	 */
        isdn_decrement_bchan(d_isdn_info);   /* one less available */
	/*
	 * we don't have the slightest idea who is calling
	 * us, so stuff the remote_number
	 * if the ISDN process was able to extract it.
	 * otherwise, pretend it is the default number.
	 */
	if (isdn_info->remote_number) {
	    dialer_set_number_called(ddb, isdn_info->remote_number);
        } else {
	    if ((!(*ddb->number_called)) && ddb->dialer_string)
		dialer_set_number_called(ddb,
				      ddb->dialer_string->entry->dialer_string);
	}
	dialer_in_call_connected(ddb);
	/*
	 * Keep statistics.
	 */
        GET_TIMESTAMP(isdn_info->connect_time);
        current_time_string(isdn_info->connect_tod);
        isdn_info->outgoing = FALSE;
        isdn_info->tx_packets = idb_outputs(hwidb);
        isdn_info->tx_bytes = idb_tx_cumbytes(hwidb);
        isdn_info->rx_packets = idb_inputs(hwidb);
        isdn_info->rx_bytes = idb_rx_cumbytes(hwidb);
        isdn_mib_update(hwidb, NULL, TRUE);

	/*
	 * Pass it to autodetect
	 */
	if (reg_invoke_serial_autodetect_start(hwidb)) {
	    return;
	}
    }

    /*
     * Perform line protocol dependent actions upon
     * state change, e.g. LAPB brings up a link, Token
     * Ring adjusts ARP and RIF caches, etc.
     */
    if (hwidb->lineaction)
	(*(hwidb->lineaction))(hwidb);
}

/*
 * isdn_call_disconnect
 * Called when an ISDN B channel disconnects.
 */
void isdn_call_disconnect (hwidbtype *hwidb)
{
    isdn_bri_t *isdn_info;
    isdn_bri_t *d_isdn_info;
    hwidbtype *master_idb;
    dialerdbtype *ddb;

    isdn_info = hwidb->isdn_info;

    /*
     * If this isn't a B channel, just exit.  But leave a debug in here
     * so we can verify...
     */
    if (isdn_info->chantype == ISDN_Dchan) {
	if (dialer_debug)
            buginf("\n%s: disconnect called for something other than B channel", 
	            hwidb->hw_namestring);
	return;
    }

	/*
	 * If the B channel has been configured as a leased line it remains up
	 * indefinately.
	 */
	if(ISBRILEASED(hwidb))
		return;

    hwidb->oldstate = hwidb->state;
    GET_TIMESTAMP(hwidb->state_time);
    hwidb->state = IDBS_DOWN;
    master_idb = get_netidb(hwidb);
    hwidb->snmp_trap_nolink = master_idb->snmp_trap_nolink;
    if (hwidb->state != hwidb->oldstate) {
	hwidb->reason = print_hwidbstate(hwidb);
        switch (hwidb->state) {
	  case IDBS_DOWN: 
	  case IDBS_UP: 
	    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(master_idb)) {
		errmsg(&msgsym(UPDOWN, LINK), hwidb->hw_namestring,
		       hwidb->reason);
	    }
	    break;
	  default:
	    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(master_idb)) {
		errmsg(&msgsym(CHANGED, LINK), hwidb->hw_namestring,
		       hwidb->reason);
	    }
	    break;
        }
	reg_invoke_ifmib_link_trap(hwidb->state == IDBS_UP ?
		LINK_UP_TRAP : LINK_DOWN_TRAP, hwidb);
    } else {
        return; /* we've been called already and we are down */
    }
    d_isdn_info = isdn_info->d_chan_idb->isdn_info;
    if ((hwidb->state != hwidb->oldstate) &&
       (hwidb->oldstate == IDBS_UP))
          isdn_increment_bchan(d_isdn_info);   /* one more B available */

    ddb = get_dialerdb(hwidb);
    ddb->disconnect_reason = NO_DISCONNECT;
    dialer_call_disconnected(ddb, DIALER_READY);

    if (isdn_info->v120info != NULL)
	reg_invoke_v120_stop_interface(hwidb);

    /*
     * Perform line protocol dependent actions upon
     * state change, e.g. LAPB brings up a link, Token
     * Ring adjusts ARP and RIF caches, etc.
     */
    if (hwidb->lineaction)
	(*(hwidb->lineaction))(hwidb);
}

/*
 * isdn_sw_cstate
 * Perform actions specific to ISDN links when they transition state.
 */
static void isdn_sw_cstate (hwidbtype *hwidb)
{
    call_id_string_t *tmp, *tmp2;
    isdn_bri_t *isdn_info;
    hwidbtype *bchan_idb = NULL;
    dialerdbtype *ddb, *bchan_ddb;
    int i;

    isdn_info = hwidb->isdn_info;

    /*
     * If there is any existing calls connected and the
     * controller is deconfigured we have a race condition
     * where the isdn_info structure might have been
     * pulled out from underneath us. This check here
     * avoids a spurious access. CSCdi37866.
     */
    if (isdn_info == NULL) {
	return;
    }
    /*
     * If this isn't a D channel, just exit.  But leave a debug in here
     * so we can verify...
     */
    if ((isdn_info->chantype != ISDN_Dchan) || (ISBRILEASED(hwidb))) {
	if (dialer_debug)
            buginf("\n%s: net_cstate called for something other than D channel", 
	            hwidb->hw_namestring);
	return;
    }

    /*
     * if the D channel goes down, or is taken down, then we need
     * to hang up any calls that may have been active or in progress
     *
     * Make sure we have a dialer, may not be true if a leased line was
     * configured and un-configured.
     */
    ddb = get_dialerdb(hwidb);
    if (ddb == NULL)
        return;

    if (hwidb->state != IDBS_UP) {
	if (hwidb->isdn_info->primary_rate == FALSE) {
       	    isdn_call_disconnect(isdn_info->b1_chan_idb);
       	    isdn_call_disconnect(isdn_info->b2_chan_idb);
            /*
             * Clean up call table, and flush out hold queues.
             */
            tmp = isdn_info->call_tbl;
            while (tmp != NULL) {
                destroy_timer(tmp->carrier_timer);
                dialer_holdq_discard_queue(&tmp->holdq_ptr,
					   "Interface shutdown");
                tmp2 = tmp;
                tmp = tmp->next;
                if (tmp2->dial_reason)
                    free(tmp2->dial_reason);
                free(tmp2);
            }
            isdn_info->call_tbl = NULL;
	    if (isdn_info->num_avail_b != SERIALS_PER_BRI) {
		/*
		 * This should not happen.  But lets clean up
		 * anyway.
		 */
		isdn_info->num_avail_b = SERIALS_PER_BRI;
	    }

	    /*
	     * For BRI interfaces that are not shutdown, we want to spoof
	     * the interface.  That way we deal with deactivated interfaces.
	     */
	    if (hwidb->state != IDBS_ADMINDOWN) {
	        ddb->dialer_spoofing_int = TRUE;
	        ddb->dialer_spoofing_prot = TRUE;
	        hwidb->state = IDBS_UP;
	    } else {
	        ddb->dialer_spoofing_int = FALSE;
	        ddb->dialer_spoofing_prot = FALSE;
	    }
        } else {
	    for (i = 0; i < ISDN_MAX_CHAN; i++) {
	        bchan_idb = hwidb->isdn_info->b_chan_idb[i];
	        if (bchan_idb)
               	    isdn_call_disconnect(bchan_idb);
	    }
	    /*
	     * For PRI, we don't spoof the interface.  If the interface is
	     * down, we are down and can't dial.
	     */
            ddb->dialer_spoofing_int = FALSE;
            ddb->dialer_spoofing_prot = FALSE;
	    set_dialer_state(ddb, DIALER_SHUTDOWN);
        }
        if ((hwidb->state == IDBS_ADMINDOWN)
			|| (hwidb->state == IDBS_RESET)){
            FOR_ALL_HUNTDDBS_ON_MASTER(ddb, bchan_ddb) {
		bchan_ddb->ownidb->state = IDBS_ADMINDOWN;
		set_dialer_state(bchan_ddb, DIALER_SHUTDOWN);
	    }
        }
    } else {
	if (hwidb->isdn_info->primary_rate == FALSE) {
	    /*
	     * We used to be real paranoid here.  We may get an activate 
	     * without there having been a deactivate.  We could
	     * drop all the calls that we may think we have up, but
	     * that is dangerous.  So just ignore activation messages and
	     * change from spoofing to not spoofing and hope for the best.
	     */

	    ddb->dialer_spoofing_int = FALSE;
	    /*
	     * Since this interface looks like a dialer, we must turn on
	     * spoofing so that the line looks "up"
	     */
	    ddb->dialer_spoofing_prot = TRUE;
        } else {
	    for (i = 0; i < ISDN_MAX_CHAN; i++) {
	        bchan_idb = hwidb->isdn_info->b_chan_idb[i];
	        if (bchan_idb)
	            /*
		     * With PRI, if the physical link was down, and now it is
		     * up, we know that we should disconnect any calls that
		     * were outstanding.  This should never happen, but better
		     * to be safe...
	             */
               	    isdn_call_disconnect(bchan_idb);
	    }
            ddb->dialer_spoofing_prot = TRUE;
            ddb->dialer_spoofing_int = FALSE;
        }
	set_dialer_state(ddb, DIALER_READY);
    }
}


/*
 * isdn_cstate
 *     registry for isdn_cstate, should be done for BRI only
 */
static void isdn_cstate (hwidbtype *hwidb)
{
    /*
     * this should only be done for BRI, it causes 4xxx PRI
     * to incorrectly reset the interface.
     */
    if (hwidb->type == IDBTYPE_BRI)
        isdn_sw_cstate(hwidb);
}

/*
 * isdn_set_chan_state
 *      registry for isdn_set_chan_state
 */
void isdn_set_chan_state (
    int bitfield,
    int dsl,
    boolean dchan,
    int dchan_val)
{

    /* if the pri-group is being deleted, deassign dsl id */
    if (!bitfield)
       isdn_deassign_dsl(dsl);

    CC_CHAN_Set_Chan_State(bitfield, dsl, dchan, dchan_val);
}
/*
 * isdn_pri_getswitch
 *      registry for isdn_getswitchtype
 */
int
isdn_pri_getswitch (void)
{
    return (isdn_getswitchtype());
}

/*
 * q921_q931_init
 * start up the telenetworks code.
 */
static void q921_q931_init (void)
{
    if ((nbrinets || nt1ports || ne1ports || nst1ports || nse1ports) && (isdn_getswitchtype() != NO_ISDN_SWITCH)) {
        if (!isdn_fork) {
            isdn_fork =  cfork (isdnmain, 0L, 1000, "ISDN", CONSOLE_LINENUM);
			if (nbrinets)
				isdn_bri_enable_layer1_Q();
	}
    } else if (nbrinets || nt1ports || ne1ports || nst1ports || nse1ports)
        printf("\nWarning: No ISDN switch-type defined.  "
	       "No calls possible.\n");
}

/*
 * isdn_parse_packet
 *
 * Basically a stub function for forwarding D-channel frames.
 * 
 */
static iqueue_t isdn_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    return(isdn_iqueue);
}

/*
 * isdn_setencap - Set the encapsulation on a isdn interface
 *
 * Set the encapsulation on a isdn interface
 */

static void isdn_setencap (parseinfo *csb)
{
    hwidbtype *d_idb;
    dialerdbtype *ddb, *tempddb;
    encapstype enctype;
    int call_flag;
    leveltype level;

    d_idb = csb->interface->hwptr;      /* Get interface pointer */
    enctype = GETOBJ(int, 1);
    call_flag = GETOBJ(int, 10);
    ddb = get_dialerdb(d_idb);

    /*
     * If we're a member of a hunt group, do not allow encapsulation changes
     */
    if (HUNTGROUP_MEMBER(ddb) && !call_flag) {
	 printf("\n%% Cannot change encapsulation of hunt group member");
	return;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    if (!HUNTGROUP_MEMBER(ddb)) {
	dialer_set_leader_encap(csb, d_idb, enctype);
    } else {
	dialer_set_member_encap(csb, d_idb, enctype);
    }
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
        dialer_set_member_encap(csb, tempddb->ownidb, enctype);
    }
    d_idb->iqueue = isdn_iqueue;
    set_default_queueing(d_idb);
    d_idb->periodic =  NULL;
    d_idb->parse_packet = isdn_parse_packet;
    d_idb->hdlc_lineup = TRUE;
    d_idb->ppp_lineup = TRUE;
    d_idb->lineaction = isdn_sw_cstate;

    reset_interrupt_level(level);
}

/*
 * isdn_pri_init - Set isdn specific fields in this pri idb. 
 *
 * init the pri idb
 */

void isdn_pri_init (hwidbtype *idb)
{
    hwidbtype * tempidb;
    dialerdbtype *ddb, *tempddb;

    /*
     * If the encapsulation is not the default, ET_HDLC
     * then let's propagate the fields to the whole
     * hunt group. We are passed the d-channel idb
     * so we can call copy hunt fields with the d-chan
     * and the hunt_group_first field.
     */
    ddb = get_dialerdb(idb);
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
        copy_dialer_fields(ddb, tempddb);
        /*
         * I need to force dialer_set_member_encap to
         * execute in this case only.
         * This is the case where we are growing our
         * pri-group.
         */
	tempidb = tempddb->ownidb;
        tempidb->enctype = ET_NULL;
        copy_hunt_fields(ddb, tempddb);
        copy_priority_fields(ddb, tempddb);

        /*
         * We need to propagate the keep period here.
         */

        tempidb->keep_period = idb->keep_period;
        tempidb->nokeepalive = idb->nokeepalive;
        tempidb->inloopback = idb->inloopback;
        
    }

    /* 
     * This is for the D-channel only.
     */

    idb->iqueue = isdn_iqueue;
    idb->periodic = NULL;
    idb->lineaction = isdn_sw_cstate;
    idb->hdlc_lineup = TRUE;
    idb->ppp_lineup = TRUE;
    idb->parse_packet = isdn_parse_packet;
}

/*
 * =====================================================================
 * pri_enq_prim -
 *
 * Description:
 * This routine is common to the C4XXX as well
 * as the low end. This routine grabs an ISDN primitive element from
 * the IsdnL1PrimPoolQ, assigns the primitive value, and enqs into the
 * IsdnL1Q using p_enqueue. The ISDN process level should use a
 * p_dequeue to deq the element. Since it is called in a level4/level3
 * interrupt, it is made inline to prevent ftion call overhead.
 *
 * Paramters: idb_unit_no -> the unit # which identifies each idb uniquely.
 *            prim        -> the layer 1 primitive passed to isdn.
 *
 * Returns: nothing
 *
 * Notes: none.
 * =====================================================================
 */
void pri_enq_prim (uint idb_unit_no, uint prim)
{
    isdn_l1_primitive_t *ptr;   /* ptr to L1 primitive struct */
 
    /*
     * Deq a prim element from the global pool
     */  
    ptr = p_dequeue(IsdnL1PrimPoolQ);
    if (!ptr) {
        errmsg(&msgsym(NOMEMORY, ISDN), idb_unit_no,
               "PRI ISDN L1 Q elements. L1 message not enqueued to L2.");
        return;
    }
 
    /*
     * Assign values and enq into the isdn Q to inform
     * either L2 or ME.
     */  
    ptr->dchan_num = idb_unit_no;                                        
    ptr->prim = prim;
    p_enqueue(IsdnL1Q, ptr);
}


/*
 * =====================================================================
 * pri_init_isdn_queue -
 *
 * Description:
 * Common to XX/low end. Initialise the L1 queue
 * and primitives pool on startup. Should be called only one time.
 *
 * Parameters: nothing
 *
 * Returns:   nothing
 *
 * Notes:     Called only one time when the box is initialised from
 *            the analyze routine.
 * =====================================================================
 */
static void pri_init_isdn_queue (void)
{
    isdn_l1_primitive_t *ptr, *ptr_next, *pool;
    int i = 0;

      /*
       * Allocate and init the L1 primitive Pool .
       */
      IsdnL1PrimPoolQ = malloc(sizeof(queuetype));
      if (!IsdnL1PrimPoolQ) {
        crashdump(0);
      }

      queue_init(IsdnL1PrimPoolQ, PRI_MAX_L1_PRIM);
       
      /*                                            
       * Allocate memory for PRI_MAX_L1_PRIM entries in the IsdnL1PrimPoolQ.
       * Malloc a single chunk to reduce overhead from multiple mallocs.
       * Then enq each Q element into the IsdnL1PrimPoolQ.
       */
      pool = malloc(sizeof(isdn_l1_primitive_t) * PRI_MAX_L1_PRIM);
      if (!pool) {
        crashdump(0);
      }
      for (i = 0, ptr = pool; i < PRI_MAX_L1_PRIM; i++, ptr = ptr_next) {
        ptr_next = (isdn_l1_primitive_t *) ((char *)ptr +
                                            sizeof(isdn_l1_primitive_t));
        /*
         * Initialise values and enqueue into the Pool
         */
        ptr->source = ISDN_BRI_L1;
        enqueue(IsdnL1PrimPoolQ, ptr);
      }

      /*
       * Now initialise the IsdnL1Q for passing primitives up to ISDN.
       * 
       */
      IsdnL1Q = malloc(sizeof(queuetype));
         
      if (!IsdnL1Q) {                      
        crashdump(0);
      }
      queue_init(IsdnL1Q, 0);
}


/*
 * isdn_pri_init_core
 *      registry for isdn_pri_init_core.
 */
void isdn_pri_init_core (
    hwidbtype *idb,
    int dsl)
{
    if (!IsdnL1PrimPoolQ) 
       pri_init_isdn_queue();

    isdn_assign_dsl(idb, dsl);
    isdn_init_core(idb, dsl);

}


#define ISDN_CARRIER_DELAY 50

/*
 * isdn_add_bchan_to_hunt_group
 * Add an isdn b channel to the hunt group of a d-channel
 * This adds it in order, based on the criteria for adding an
 * interface to the idb queues.
 * This also sets the hunt_group_master field
 */
static void isdn_add_bchan_to_hunt_group (dialerdbtype *dchan_ddb,
					  dialerdbtype *ddb)
{
    dialerdbtype *p, *q;

    ddb->hunt_group_master = dchan_ddb;
    
    if (dchan_ddb->hunt_group_first == NULL) {
	dchan_ddb->hunt_group_first = ddb;
	return;
    }

    p = dchan_ddb->hunt_group_first;
    q = NULL;

    while ((p != NULL) &&
	   (p != ddb) &&
	   (platform_interface_ordering(ddb->ownidb, p->ownidb) == FALSE)) {
	q = p;
	p = p->hunt_group_next;
    }

    /*
     * If this member is already in the list, don't add it
     */
    if (p == ddb)
	return;
    
    if (q == NULL) {
	ddb->hunt_group_next = dchan_ddb->hunt_group_first;
	dchan_ddb->hunt_group_first = ddb;
    } else {
	q->hunt_group_next = ddb;
	ddb->hunt_group_next = p;
    }
}

/*
 * isdn_setup
 * initialize the ISDN specific fields of the idb.
 */
void isdn_setup (hwidbtype *idb)
{
    isdn_bri_t *isdn_info;
    dialerdbtype *ddb, *dchan_ddb;


    /*
     * The idb pointers should already have been initialized by the driver,
     * so initialize the calling and called number, and setup the idb so it
     * looks like a dialer.
     */
    isdn_info = idb->isdn_info;
    isdn_info->remote_number = NULL;
    isdn_info->remote_src = ISDN_SRC_UNKNOWN;
    isdn_info->call_tbl = NULL;
    TIMER_STOP(isdn_info->connect_time);
    isdn_info->call_timers = NULL;

    ddb = init_dialer_idb(idb);
    if (ddb == NULL) {
	printf(nomemory);
	return;
    }
    ddb->dialer = DIALER_ISDN;
    idb->setencap = isdn_setencap;
    idb->carrierdelay = ISDN_CARRIER_DELAY;
    idb->carrierdelay_def = ISDN_CARRIER_DELAY;

    /*
     * Fixup idb vectors for dialer use
     */
    dialer_member_adjust_encap_vectors(ddb, TRUE);

    if (isdn_info->chantype == ISDN_Dchan) {
        idb->periodic = NULL;
        isdn_info->num_avail_b = SERIALS_PER_BRI;
        idb->parse_packet = isdn_parse_packet;
        ddb->hunt_group_lead = TRUE;
	ddb->dialer_num_link = SERIALS_PER_BRI;
        idb->lineaction = NULL;
    } else {
        idb->reset = dialer_disconnect;
	dchan_ddb = get_dialerdb(isdn_info->d_chan_idb);
	isdn_add_bchan_to_hunt_group(dchan_ddb, ddb);
	ddb->netcfg_ddb = dchan_ddb;
	ddb->netcfg_idb = dchan_ddb->ownidb;
    }
}


char *const switch_names[NISDNSWITCHES] = {
    "none",
    "basic-net3",
    "primary-4ess",
    "primary-5ess",
    "basic-5ess",
    "primary-dms100",
    "basic-dms100",
    "vn2",
    "vn3",
    "kdd",
    "ntt",
    "primary-1tr6",
    "basic-1tr6",
    "basic-ni1",
    "basic-ts013",
    "basic-nznet3",
    "basic-nwnet3",
    "primary-ntt",
    "primary-net5",
    "primary-ts014",
    "ccitt"
};

/*
 * print_isdn_switch
 * display the ascii version of the given isdn switch type
 */
char *print_isdn_switch (
    SWITCH_TYPE isdnsw)
{
    return(switch_names[isdnsw]);
}


/*
 * select_isdn_switch
 * routine to parse the command line switch and store the switch type.
 * We need to know the switch type in order to interoperate with the
 * switch correctly.
 */

static void select_isdn_switch (parseinfo *csb)
{
    int isdnsw;
    SWITCH_TYPE old_switch;

    isdnsw = GETOBJ(int,1);
    old_switch = isdn_getswitchtype();
    isdn_switchtype((SWITCH_TYPE) isdnsw);

    if (old_switch == NO_ISDN_SWITCH) {
	q921_q931_init();
    } else {
	if (old_switch != (SWITCH_TYPE) isdnsw) {
	    printf("\nNew switch-type will not take effect until reload");
	}
    }
}

/*
 * isdn_command
 * Used to configure the ISDN parameters
 *
 * History:
 * 01/22/96  H.M. Robison     Change check for nprinets to check for
 *                            any channelized ports (which may later be
 *                            configured for PRI if no BRI interfaces are present.
 */
void isdn_command (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    isdn_bri_t *isdn_info;

    if (csb->nvgen) {
	if (nbrinets || nt1ports || ne1ports || nst1ports || nse1ports ) {
	    switch (csb->which) {
	    case PARSER_ISDN_TEI:
		nv_write((isdn_pu_establish != 1) &&
			 (isdn_pu_establish == GETOBJ(int,1)),
			 csb->nv_command);
		break;
	    case PARSER_ISDN_SWITCH:
		nv_write(isdn_getswitchtype() != DEFAULT_ISDN_SWITCH, "%s %s",
			 csb->nv_command,
			 print_isdn_switch(isdn_getswitchtype()));
		break;
	    case PARSER_ISDN_LEASED_LINE: {
		swidb = GETOBJ(idb,1);
		hwidb = swidb ? swidb->hwptr : NULL;
		if((hwidb) && !IS_PRI(hwidb)){
		    isdn_info = hwidb->isdn_info;
		    if (isdn_info->chantype == ISDN_Dchan)
			nv_write(isdn_info->leased_line, csb->nv_command);
		}
	    }
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	    }
	}
	return;
    }

    switch (csb->which) {
    case PARSER_ISDN_TEI:
	if (!csb->sense) {
	    isdn_pu_establish = TRUE;
	} else {
	    isdn_pu_establish = GETOBJ(int,1);
	}
	break;
    case PARSER_ISDN_SWITCH:
	if (!csb->sense) {
	    isdn_switchtype(NO_ISDN_SWITCH);
	    printf("\nWarning: No ISDN switch-type defined.  "
		   "No calls possible.\n");
	} else {
	    select_isdn_switch(csb);
	}
	break;
    case PARSER_ISDN_LEASED_LINE: {
	swidb = GETOBJ(idb,1);
	hwidb = swidb ? swidb->hwptr : NULL;
	if (hwidb == NULL)
	    return;

	isdn_info = hwidb->isdn_info;
	if (isdn_info == NULL)
	    return;
	if (isdn_info->chantype != ISDN_Dchan) {
	    printf("\nPlease use d channel to designate leased line");
	    return;
	}
	    
	if (!csb->sense) {
	    hwidbtype *b_idb;
	    
	    isdn_info->leased_line = FALSE;
	    /*
	     * Mark the b-channels as not NV-able.  That will take
	     * them out when the user does a wr mem.
	     */
	    b_idb = hwidb->isdn_info->b1_chan_idb;
	    if (b_idb)
		b_idb->status |= IDB_NO_NVGEN;
	    b_idb = hwidb->isdn_info->b2_chan_idb;
	    if (b_idb)
		b_idb->status |= IDB_NO_NVGEN;
	    printf("\nChange will not take effect until reload");
	}
	else
	    if(!isdn_info->leased_line)
		reg_invoke_isdn_leased_line(hwidb);
    }
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static boolean isdn_bri_is_answer (dialerdbtype *ddb)
{
    hwidbtype *idb;
    isdn_bri_t *isdn_info;
    isdn_bri_t *d_isdn_info;
    call_id_string_t *tmp;
    call_id_string_t *prev;
    boolean retvalue;


    idb = ddb->ownidb;
    isdn_info = idb->isdn_info;
    d_isdn_info = isdn_info->d_chan_idb->isdn_info;

    /*
     * Ok, one of the B channels has connected.  First, look
     * through the list of calls we have pending, and see if
     * the call id of this call matches the call id of one of
     * the calls we have placed.  If so, then return false.
     */

    tmp = d_isdn_info->call_tbl;
    prev = NULL;

    /*
     * walk down the linked list looking for the call id
     */

    while (tmp) {
        if (tmp->call_id == isdn_info->call_id)
	    break;
	else {
	    prev = tmp;
	    tmp = tmp->next;
        }
    }

    /*
     * If we found an entry, it means that a call we placed is
     * connected.  If we didn't find an entry, either this is
     * an incoming call, or it is a call we placed, but the
     * wait for carrier timer expired before the call
     * completed.  If the latter, we handle just like the former,
     * but it will be hung up.  There is a race condition here...
     */
            
    if (tmp) {
        retvalue = FALSE;
    } else if (isdn_info->call_id & 0x8000) {
        retvalue = FALSE;
    } else {
        retvalue = TRUE;
    }

    return(retvalue);
}

/*
 * isdnif_command
 *
 */
void isdnif_command (parseinfo *csb)
{
    hwidbtype *idb = hwidb_or_null(csb->interface);
    dialerdbtype *ddb;
    dialer_call_tab *tmpcall;
    int x,y,dsl;
    char answer_num[MAX_ANSWER_LEN];
    int switchtype;

    x = 0;
    y = 0;
    dsl = 0;
    /*
     * ISDN Leased lines have no interface configurable parameters.
     */
    if (ISBRILEASED(idb))
	return;
    /*
     * Get the interface number from the idblist 
     */
    for (dsl = 0; dsl < MAX_DSL; dsl++)
	if (isdn_idblist[dsl] == idb)
	    break;
    if (dsl == MAX_DSL) {
	return;
    }
    ddb = get_dialerdb(idb);
    if (csb->nvgen) {
	if ((ddb == NULL) || !ISDN_DIALER(ddb)) {
	    return;
	}

	switch (csb->which) {
	case ISDN_MODEM_BUSY_CAUSE:
	    switch(idb->isdn_info->modem_busy_cause){
	        case NO_BUSY_CAUSE_SPECIFIED:
	            break;
	        case USER_BUSY:
		    nv_write(TRUE, "%s %s", 
			     csb->nv_command, "busy");
		    break;
		case REQ_CHANNEL_NOT_AVAIL:
		    nv_write(TRUE, "%s %s", 
			     csb->nv_command, "not-available");
		    break;
		default:
		    nv_write(idb->isdn_info->modem_busy_cause, "%s %d", 
			     csb->nv_command, idb->isdn_info->modem_busy_cause);
	    }
	    break;
	case PARSER_ISDN_BCHANNELS:
	    if (nbrinets) {
		nv_write(idb->isdn_info->num_avail_b != 2, 
			 "%s %d", csb->nv_command,
			 idb->isdn_info->num_avail_b);
	    }
	    break;
	case ISDN_SPID_STRING:
	    if (nbrinets) {
		nv_write(np_fit[dsl][0].spid[0] != '\0', "%s %s",
			 csb->nv_command, np_fit[dsl][0].spid);
		nv_add(np_fit[dsl][0].ldn[0] != '\0', " %s", np_fit[dsl][0].ldn);
	    }
	    break;
	case ISDN_SPID2_STRING:
	    if (nbrinets) {
		nv_write(np_fit[dsl][1].spid[0] != '\0', "%s %s",
			 csb->nv_command, np_fit[dsl][1].spid);
		nv_add(np_fit[dsl][1].ldn[0] != '\0', " %s", np_fit[dsl][1].ldn);
	    }
	    break;
	case ISDN_CALLER_STRING:
	    /* Since these are inserted in reverse order */
            tmpcall = ddb->dialer_call;
	    /* go to the end of the list first */
            while (tmpcall && tmpcall->next) {
		tmpcall = tmpcall->next;
	    }
	    /* and print them out backwards */
	    while (tmpcall) {
                nv_write(TRUE, "%s %s",
                         csb->nv_command,
                         tmpcall->dialer_call_string);
                tmpcall = tmpcall->prev;
            }
            break;
        case ISDN_FASTROLLOVER_DELAY:
	    nv_write(idb->dialerdb->fastrollover_ticks !=
		     ISDN_FASTROLLOVER_DELAY_DEF,
		     "%s %u", csb->nv_command,
		     idb->dialerdb->fastrollover_ticks);
	    break;
        case ISDN_VOICE_DATA:
                nv_write(idb->isdn_info->voice_answer_data, "%s", csb->nv_command);
                if ((isdn_sys_constants[dsl].voice_rate) &&
                    (idb->isdn_info->voice_answer_data)) {
                        nv_add((isdn_sys_constants[dsl].voice_speed ==
                                ISDN_SPEED_64), " 64");
                        nv_add((isdn_sys_constants[dsl].voice_speed ==
                                ISDN_SPEED_56), " 56");
                }
                break;
        case ISDN_VOICE_MODEM:
                nv_write(idb->isdn_info->voice_answer_modem, "%s", csb->nv_command);
                if ((isdn_sys_constants[dsl].voice_rate) &&
                    (idb->isdn_info->voice_answer_modem)) {
                        nv_add((isdn_sys_constants[dsl].voice_speed ==
                                ISDN_SPEED_64), " 64");
                        nv_add((isdn_sys_constants[dsl].voice_speed ==
                                ISDN_SPEED_56), " 56");
                }
                break;
	case ISDN_ANSWER1_STRING:
	    nv_write(isdn_answer[dsl].answer1[0] != '\0', "%s %s",
		     csb->nv_command, isdn_answer[dsl].answer1);
	    break;
	case ISDN_ANSWER2_STRING:
	    nv_write(isdn_answer[dsl].answer2[0] != '\0', "%s %s",
		     csb->nv_command, isdn_answer[dsl].answer2);
	    break;
        case ISDN_SENDING_COMPLETE:
            if (isdn_sys_constants[dsl].sending_complete)
                nv_write(isdn_sys_constants[dsl].sending_complete, "%s",
                     csb->nv_command);
            break;
	case ISDN_T200_TIMER:
	    nv_write(isdn_sys_constants[dsl].T200 != DEFAULT_T200, "%s %d",
		     csb->nv_command, isdn_sys_constants[dsl].T200);
	    break;
	case ISDN_N200_RETRIES:
	    nv_write(isdn_sys_constants[dsl].N200 != DEFAULT_N200, "%s %d",
		     csb->nv_command, isdn_sys_constants[dsl].N200);
	    break;
	case ISDN_CALLING_NUMBER_STRING:
	    nv_write(isdn_calling[dsl].number[0] != '\0', "%s %s",
		     csb->nv_command, isdn_calling[dsl].number);
	    break;
	case ISDN_NOT_END_TO_END:
	    nv_write(isdn_sys_constants[dsl].nend_to_end, csb->nv_command);
	    if (isdn_sys_constants[dsl].nend_to_end) {
		nv_add((isdn_sys_constants[dsl].nend_speed ==
			ISDN_SPEED_64), " 64");
		nv_add((isdn_sys_constants[dsl].nend_speed ==
			ISDN_SPEED_56), " 56");
	    }
	    break;
        case ISDN_TWAIT_DISABLED:
            nv_write(isdn_sys_constants[dsl].twait_disabled, "%s",
                     csb->nv_command);
            break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switchtype = isdn_getswitchtype();
    switch (csb->which) {
    case ISDN_MODEM_BUSY_CAUSE:
	if(csb->sense)
	    idb->isdn_info->modem_busy_cause = GETOBJ(int, 1); 
	else
	    idb->isdn_info->modem_busy_cause = NO_BUSY_CAUSE_SPECIFIED;
        break;
    case ISDN_SPID_STRING:
	if (csb->sense)
	    strcpy(np_fit[dsl][0].spid, GETOBJ(string,1));
	else
	    np_fit[dsl][0].spid[0] = '\0';

        np_fit[dsl][0].spid_sent = FALSE;
	/* check if local directory number present */
	if (GETOBJ(string,2)[0] != '\0') {
	    strcpy(np_fit[dsl][0].ldn, GETOBJ(string,2));
	    np_fit[dsl][0].DN_Routing = TRUE;
	} else {
	    np_fit[dsl][0].ldn[0] = '\0';
	    np_fit[dsl][0].DN_Routing = FALSE;
	}
	break;
    case ISDN_SPID2_STRING:
        /*
         * The second isdn spid is only valid for National ISDN and
         * DMS100 custom BRI switches.
         */ 
	if (csb->sense) {
            if (switchtype != BRI_5ESS_STYPE) { 
	        strcpy(np_fit[dsl][1].spid, GETOBJ(string,1));
            } else {
                np_fit[dsl][1].spid[0] = '\0';
                printf("\n%%isdn spid2 not allowed for 5ESS BRI custom");
            }
	} else {
	    np_fit[dsl][1].spid[0] = '\0'; 
        }
        np_fit[dsl][0].spid_sent = FALSE;
	
	/* check if local directory number present */
	if ((GETOBJ(string,2)[0] != '\0') && (switchtype != BRI_5ESS_STYPE)) {
	    strcpy(np_fit[dsl][1].ldn, GETOBJ(string,2));
	    np_fit[dsl][1].DN_Routing = TRUE;
	} else {
	    np_fit[dsl][1].ldn[0] = '\0';
	    np_fit[dsl][1].DN_Routing = FALSE;
	}
	break;
    case ISDN_VOICE_DATA:
        if (csb->sense) {
            idb->isdn_info->voice_answer_data = TRUE;
            idb->isdn_info->voice_answer_modem = FALSE;
            switch (GETOBJ(int,1)) {

            case ISDN_SPEED_56:
                isdn_sys_constants[dsl].voice_rate = TRUE;
                isdn_sys_constants[dsl].voice_speed = ISDN_SPEED_56;
                break;

            case ISDN_SPEED_64:
                isdn_sys_constants[dsl].voice_rate = TRUE;
                isdn_sys_constants[dsl].voice_speed = ISDN_SPEED_64;
                break;

            default:
                isdn_sys_constants[dsl].voice_rate = FALSE;
            }
        } else {
            idb->isdn_info->voice_answer_data = FALSE;
            isdn_sys_constants[dsl].voice_rate = FALSE;
        }
        break;
    case ISDN_VOICE_MODEM:
        if (csb->sense) {
            idb->isdn_info->voice_answer_modem = TRUE;
            idb->isdn_info->voice_answer_data = FALSE;
            switch (GETOBJ(int,1)) {

            case ISDN_SPEED_56:
                isdn_sys_constants[dsl].voice_rate = TRUE;
                isdn_sys_constants[dsl].voice_speed = ISDN_SPEED_56;
                break;

            case ISDN_SPEED_64:
                isdn_sys_constants[dsl].voice_rate = TRUE;
                isdn_sys_constants[dsl].voice_speed = ISDN_SPEED_64;
                break;

            default:
                isdn_sys_constants[dsl].voice_rate = FALSE;
            }
        } else {
            idb->isdn_info->voice_answer_modem = FALSE;
            isdn_sys_constants[dsl].voice_rate = FALSE;
        }
        break;
    case ISDN_CALLER_STRING:
        if (strlen(GETOBJ(string,1)) > DIALER_MAX_CALLER) {
            printf("\n%%Dialer caller string too long.");
            return;
        }

        dialer_parser_caller(csb, ddb);
        break;
    case ISDN_FASTROLLOVER_DELAY:
        if (csb->sense) {
	    idb->dialerdb->fastrollover_ticks = GETOBJ(int,1);
	} else {
	    idb->dialerdb->fastrollover_ticks =
		ISDN_FASTROLLOVER_DELAY_DEF;
	}
	break;

    case ISDN_ANSWER1_STRING:
	if (csb->sense) {
	    if (strlen(GETOBJ(string,1)) == 0) {
		printf("\n%%isdn answer1 incomplete command.");
	    } else {
		if (strlen(GETOBJ(string,1)) > MAX_ANSWER_LEN) 
		    printf("\n%%isdn answer1 string too long.");
		else {	
		    strcpy(answer_num, GETOBJ(string,1));
		    strcpy(isdn_answer[dsl].answer1, answer_num);
		}
	    }
      	} 
	else { 
	    /* it's a no answer command */
	    if (strlen(GETOBJ(string,1)) == 0) 
		isdn_answer[dsl].answer1[0] = '\0';
	    else
		if (strlen(GETOBJ(string,1)) > MAX_ANSWER_LEN) 
		    printf("\n%%isdn answer1 string too long.");
		else {
		    strcpy(answer_num, GETOBJ(string,1));
		    if (!strcmp (isdn_answer[dsl].answer1, answer_num))
			isdn_answer[dsl].answer1[0] = '\0';
		    else
			printf("\n%%No match found for answer1 number.");
		}
	}
	break;
    case ISDN_ANSWER2_STRING:
	if (csb->sense) {
	    if (strlen(GETOBJ(string,1)) == 0) {
		printf("\n%%isdn answer2 incomplete command.");
	    } else {
		if (strlen(GETOBJ(string,1)) > MAX_ANSWER_LEN) 
		    printf("\n%%isdn answer2 string too long.");
		else {	
		    strcpy(answer_num, GETOBJ(string,1));
		    strcpy(isdn_answer[dsl].answer2, answer_num);
		}
	    }
      	} 
	else { 
	    /* it's a no answer command */
	    if (strlen(GETOBJ(string,1)) == 0) 
		isdn_answer[dsl].answer2[0] = '\0';
	    else
		if (strlen(GETOBJ(string,1)) > MAX_ANSWER_LEN) 
		    printf("\n%%isdn answer2 string too long.");
		else {
		    strcpy(answer_num, GETOBJ(string,1));
		    if (!strcmp (isdn_answer[dsl].answer2, answer_num))
			isdn_answer[dsl].answer2[0] = '\0';
		    else
			printf("\n%%No match found for answer2 number.");
		}
	}
	break;
    /*
     * Taiwan homologation, Net5 and Net3 switches, have to include a
     * sending complete in the outgoing Setup message.
     */
    case ISDN_SENDING_COMPLETE:
        if (csb->sense)
            isdn_sys_constants[dsl].sending_complete = TRUE;
        else
            isdn_sys_constants[dsl].sending_complete = FALSE;
        break;
    case ISDN_T200_TIMER:
	if (csb->sense) {
	    if (GETOBJ(int,1) == 0) 
		printf("\n%%isdn T200 invalid command.");
	    else {
		isdn_sys_constants[dsl].T200 = GETOBJ(int,1);
		printf("Command will take effect after a shut\n"); 
	    }
      	} 
	else { 
	    /* it's a no T200 command */
	    isdn_sys_constants[dsl].T200 = DEFAULT_T200;
	}
	break;
    case ISDN_N200_RETRIES:
	if (csb->sense) {
	    if (GETOBJ(int,1) == 0) 
		printf("\n%%isdn N200 invalid command.");
	    else  {
		isdn_sys_constants[dsl].N200 = GETOBJ(int,1);
		printf("Command will take effect after a shut\n"); 
	    }
      	} 
	else { 
	    /* it's a no N200 command */
	    isdn_sys_constants[dsl].N200 = DEFAULT_N200;
	}
	break;
    case ISDN_CALLING_NUMBER_STRING:
    	if (csb->sense) {
	    switchtype = isdn_getswitchtype();
            if (switchtype == BRI_TS013_STYPE ||
		switchtype == PRI_TS014_STYPE || switchtype == BRI_5ESS_STYPE) {
		strcpy(isdn_calling[dsl].number, GETOBJ(string,1));
            } else {
                printf("\n%%isdn calling-number only allowed for "
		       "Australia and 5ESS BRI switches");
                isdn_calling[dsl].number[0] = '\0';
	    }
    	} else
	    isdn_calling[dsl].number[0] = '\0';
    	break;
    case ISDN_NOT_END_TO_END:
	if (csb->sense)
	    switch (GETOBJ(int,1)) {
	    case ISDN_SPEED_56:
		isdn_sys_constants[dsl].nend_to_end = TRUE;
		isdn_sys_constants[dsl].nend_speed = 
		    ISDN_SPEED_56;
		break;

	    case ISDN_SPEED_64:
	    case 0:
		isdn_sys_constants[dsl].nend_to_end = TRUE;
		isdn_sys_constants[dsl].nend_speed =
		    ISDN_SPEED_64;
		break;

	    default:
		isdn_sys_constants[dsl].nend_to_end = FALSE;
		printf("\n%% %d is not a valid speed setting for isdn",
		       GETOBJ(int,1));
	    }
	else
	    isdn_sys_constants[dsl].nend_to_end = FALSE;
	break;
    case ISDN_TWAIT_DISABLED:
        if (csb->sense)
            isdn_sys_constants[dsl].twait_disabled = TRUE;
        else
            isdn_sys_constants[dsl].twait_disabled = FALSE;
        break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * show_isdn
 * information about the pri and bri
 */
void show_isdn (parseinfo *csb)
{
    int dsl;
    hwidbtype *idb = NULL;

    if (GETOBJ(idb,1)) {
        idb = (GETOBJ(idb,1)->hwptr);
        /* BRI doesn't store dsl in isdn_info */
        for (dsl = 0; dsl < MAX_DSL; dsl++)
          if (isdn_idblist[dsl] == idb)
	     break;

        if (dsl == MAX_DSL) {
	  printf("\nISDN Error: Illegal Interface %s", idb->hw_short_namestring);
	  return;
        }
    }
    else
       dsl = GETOBJ(int,1);

    switch (csb->which) {
      case SHOW_ISDN_CHANNELS:
        show_isdn_pri(dsl); /* show state of the primary rate channels */
        break;
      case SHOW_ISDN_MEMORY:
        isdn_memstats();  /* show how the isdn memory is used */
        break;
      case SHOW_ISDN_TIMERS:
        show_isdn_L2timers();
        show_isdn_L3timers();
        break;
      case SHOW_ISDN_STATUS:
        show_isdn_status(dsl);
        break;
      case SHOW_ISDN_HISTORY:
        reg_invoke_isdn_show_history(dsl);
        break;
      case SHOW_ISDN_ACTIVE:
        reg_invoke_isdn_show_active(dsl);
        break;
      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * isdn_carrier_timeout
 * called by the timer routines to process a wait for carrier timeout.
 * Also, called by the fast failover for ISDN functionality. 
 * Passed the idb of the D channel and the call id.  Walk the list of
 * outstanding calls on the D channel, and find the call id.  When
 * found, destroy the the timer, and remove this from the list of
 * outstanding calls.  Then bump up the number of available B channels.
 * Also update the stats for this number.
 */
void isdn_carrier_timeout (void *p1, void *p2, void *p3)
{
    hwidbtype *idb = (hwidbtype *)p1;
    call_id_string_t *id = (call_id_string_t *)p2;
    call_id_string_t *prev;
    call_id_string_t *tmp;
    isdn_bri_t *isdn_info;
    dialerdbtype *ddb;
    dialerpaktype dpak;
    dialer_stat *ds;
    callback_type *call;

    bzero(&dpak, sizeof(dialerpaktype));

    if (id == NULL) {
	/* Shouldn't get here, but just in case... */
	if (dialer_debug)
	    buginf("\n%s: isdn_carrier_timeout Error: invalid call id",
		idb->hw_namestring);
	return;
    }

    if (dialer_debug)
        buginf("\n%s: wait for carrier timeout, call id=0x%x",
	       idb->hw_namestring, id->call_id);

    ddb = get_dialerdb(idb);

    if (ddb) {
	GET_TIMESTAMP(ddb->mru_lastfailure);
    }

    isdn_info = idb->isdn_info;
    tmp = isdn_info->call_tbl;
    prev = NULL;

    /*
     * walk down the linked list looking for the call id
     */

    while (tmp) {
        if (tmp == id)
	    break;
	else {
	    prev = tmp;
	    tmp = tmp->next;
        }
    }

    /*
     * We better have found it, but just in case...
     * Now cancel the call
     */

    if (tmp) {
        destroy_timer(tmp->carrier_timer);
        ds = tmp->stats;
	if (ds) {
	    ds->num_failures++;
	    ds->last_call_success = FALSE;
	}
	reg_invoke_isdn_stats_update(idb, 1, tmp->called_num);
	/* 
	 * if fast failover, no need to hangup the call.
	 */
	if (p3 == NULL)
           isdn_fromrouter(ISDN_HANGUP, tmp->call_id, NULL, 0, isdn_info->ces, idb, isdn_info->spc);

	/*
	 * Call failed: restore map pointer so we attempt to dial the
	 * next phone number in the list (see below), if any.
	 * Also restore num_avail_b and dialer_num_call.
	 * Take the holdq pointer and put it in the dpak structure, so
	 * we re-use it.
	 */
	dpak.ddb = tmp->netddb;
	dpak.map = tmp->call_group;
	dpak.header = tmp->dial_reason;
	dpak.holdQ = tmp->holdq_ptr;
	dialer_decrement_call(ddb);		/* one less B chan. in use */
        isdn_increment_bchan(isdn_info);		/* one more available */

        /*
         * Now unlink it from the list and free it.
         */
        if (prev) {
            prev->next = tmp->next;
        } else {
            isdn_info->call_tbl = tmp->next;
        }
        free(tmp);

	/*
	 * If this number is in a call group, try to dial the next number
	 * in the group and save hold queue, else free B channel and flush
	 * the hold queue.
	 */
	if (dpak.map && dpak.map->cip && dpak.map->cip->next) {
 	    /*
 	     *  We are about to to try to call again using the
 	     *    next phone number configured for the destination
 	     */
 	    if ((p3 == NULL) || !idb->dialerdb->fastrollover_ticks) {
 		/*
 		 *  This is the default backward compatible case
 		 *  This code is executing because of a carrier timeout OR
 		 *    no fast-rollover-time configured, dial immediately.  
 		 *  p3 == NULL means carrier timer expired
 		 *  OR, the fast rollover time is 0.
 		 */
		dial_if(ddb, &dpak);
	    } else {
 		/*
 		 *  This code is executing because an ISDN message indicated
 		 *    that the call could not be completed and we
 		 *    are trying fast rollover.
 		 *  Fast rollover does not work in some special cases because
 		 *    the resulting ISDN setup will be sent out before
 		 *    the RELEASED COMPLETE has been process for the failed
 		 *    call.
 		 *  So start the fast rollover timer to allow enough time
 		 *    for the RELEASE_COMP to be processed.
 		 *  Use the callback code to dial some time later.
 		 */
 		call = create_callback(dpak.holdQ,
 				       CALLBACK_SERVER,
 				       dpak.map->cip->hostname,
				       idb,
 				       dpak.map->cip->dialer_string,
 				       dpak.map);
 		if (call) {
 		    start_callback_timer(call,
 					 (idb->dialerdb->fastrollover_ticks * ONESEC));
 
 		} else {
 		    if (dialer_debug)
 			buginf("\n%s: Fastrollover timer NOT started.",
 			       idb->hw_namestring);
 		}
 	    }
	} else {
	    dialer_cannot_dial(ddb, &dpak);
	}
    } else
        if (serial_debug)
	    buginf("\nCall ID 0x%x not found after wait expire", id->call_id);
}

/*
 * isdn_dialing_num_again_too_soon
 * Added as a Japanese homologation requirement. This function checks a linked
 * list of numbers called within the last 90 secs to see if a call to this
 * number has already been made within this time period. Returns FALSE if not,
 * and adds the number to the list. Returns TRUE if it has.  Checks the switch
 * type. If not NTT always returns FALSE.
 */
static boolean isdn_dialing_num_again_too_soon (hwidbtype *hwidb, char *number)
{
    SWITCH_TYPE sw;
    isdn_bri_t *d_isdn_info;
    call_time_check_t *curr, *prev, *tmp;

    /*
     * If the switch type isn't NTT return a negative result.
     */
    sw = isdn_getswitchtype();
    if((sw != NTT_STYPE) && (sw != PRI_NTT_STYPE))
        return(FALSE);

    /*
     * Make sure it's the D channel isdn_info we use.
     */
    d_isdn_info = hwidb->isdn_info->d_chan_idb->isdn_info;

    /*
     * Search the list for an instance of this number.
     */
    curr = d_isdn_info->call_timers;
    prev = NULL;
    while(curr){
        if(!strcmp(number, curr->number)){
            /*
             * Has the number been found in our list with its timer still
             * running?
             */
            if(TIMER_RUNNING_AND_SLEEPING(curr->timer)){
                /*
                 * Found with not enough time elapsed since last attempt.
                 */
                return(TRUE);
            }
            else{
                /*
                 * Found but timer has expired or has been stopped.
                 * Restart the timer.
                 */
                TIMER_START(curr->timer, NTT_REDIAL_TIMEOUT);
                return(FALSE);
            }
        }
        prev = curr;
        curr = curr->next;
    }

    /*
     * Arriving at this point we assume that the number was not found in our
     * list. Add this number to the list and start a timer for it.
     */
    tmp = (call_time_check_t *)malloc(sizeof(call_time_check_t));
    if(!tmp){
        if(dialer_debug)
            buginf("\n%s: unable to add number called to list",
                hwidb->hw_namestring);
        return(FALSE);
    }
    strcpy(tmp->number, number);
    TIMER_START(tmp->timer, NTT_REDIAL_TIMEOUT);
    curr = d_isdn_info->call_timers;
    d_isdn_info->call_timers = tmp;
    tmp->next = curr;

    /*
     * Dialer is free to call the number this time.
     */
    return(FALSE);
}

/*
 * 
 * Clears the timer running on a number recently dialed since the call was
 * successfull. This check is only interested in failed calls not being
 * repeated within a defined time span.
 */
static void isdn_clear_ntt_next_call_chk_time(hwidbtype *hwidb, char *number)
{
    isdn_bri_t *d_isdn_info;
    call_time_check_t *curr, *prev;

    /*
     * Make sure it's the D channel isdn_info we use.
     */
    d_isdn_info = hwidb->isdn_info->d_chan_idb->isdn_info;

    /*
     * Search the list for an instance of this number.
     */
    curr = d_isdn_info->call_timers;
    prev = NULL;
    while(curr){
        if(!strcmp(number, curr->number)){
            TIMER_STOP(curr->timer);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

/*
 * isdn_dialer_dial
 * Dial an interface that is an ISDN D channel
 * Called by SERVICE_MEDIA_DIALER_DIAL, index is DIALER_ISDN
 */
static boolean isdn_dialer_dial (dialerdbtype *ddb, dialerpaktype *dpak)
{
    int success;
    isdn_bri_t *isdn_info;
    call_id_string_t *tmp;
    hwidbtype *idb;
    dialermaptype *map;
    dialerentrytype *pdialerentry;
    mapclass_type * pmapclass;
    dialer_params_type * pparams;

    idb = ddb->ownidb;

	/*
     * Required for Japanese NTT switch homologation.
     */
    if(isdn_dialing_num_again_too_soon(idb, dpak->dial_number)){
        if(dialer_debug)
            buginf("\n%s: call delayed. number redialed too soon",
                idb->hw_namestring);
        return(FALSE);
    }

    pdialerentry = NULL;
    pmapclass = NULL;
    pparams = NULL;
    isdn_info = idb->isdn_info;

    /*
     * Allocate and initialize a call table element.
     */
    tmp = malloc(sizeof(call_id_string_t));
    if (!tmp) {
	return(FALSE);
    }
    strcpy(tmp->called_num, dpak->dial_number);
    tmp->call_id = call_id_for_dialer(&Call_ID);
    tmp->next = isdn_info->call_tbl;
    tmp->speed = ISDN_SPEED_64;
    tmp->dial_reason = dpak->header;
    tmp->stats = dpak->stats;
    tmp->netddb = dpak->ddb;

    /*
     * Save current dialer map for possible subsequent calls using next
     * phone number in list.
     */
    map = dpak->map;
    tmp->call_group = map;

    /*
     * copy holdq from dpak to call structure.
     * We don't hook it to an interface yet.
     */
    tmp->holdq_ptr = dpak->holdQ;

    isdn_info->call_tbl = tmp;

    /*
     * Get ISDN call attributes from the dialer map.
     */
    isdn_info->spc = FALSE;
    isdn_info->voice_call = FALSE;
    isdn_info->do_nsf = FALSE;
    isdn_info->nsf_type = 0;
    if ((map != NULL) && (map->cip != NULL)) {
        pdialerentry = map->cip;
	tmp->speed = pdialerentry->speed;
	isdn_info->spc = pdialerentry->spc;
    }
    /*
     * If there is a class on this dialer map statement let's
     * see if it is configured for isdn voice. If so let's
     * set the voice field in the addr structure. This is
     * much like spc except it is parsed in a different place.
     * map->cip is the current dialerentry for this call.
     */
    if (pdialerentry && pdialerentry->class) {
        pmapclass = mapclass_lookup(pdialerentry->class, ET_DIALER);
        if (pmapclass) {
            if (pmapclass->params) {
                pparams = (dialer_params_type *)pmapclass->params;
                if (pparams->voice_call)
                    isdn_info->voice_call = TRUE;
                if (pparams->do_nsf) {
                    isdn_info->do_nsf = TRUE;
                    isdn_info->nsf_type = pparams->nsf_type;
                }
		if (pparams->speed)
		  tmp->speed = pparams->speed;
		if (pparams->spc)
		  isdn_info->spc = pparams->spc;

            }
        }
    }

    success = isdn_fromrouter(ISDN_CALL, Call_ID, dpak->dial_number, 
			      tmp->speed, 1, idb, isdn_info->spc);
    if (success == 0) {
	isdn_decrement_bchan(isdn_info->d_chan_idb->isdn_info);   /* one less avail. */
        tmp->carrier_timer = create_timer(isdn_carrier_timeout, idb, 
					  tmp, NULL, "Dialer", TMT_ONCE);
        set_timer(tmp->carrier_timer, ddb->dialer_carrier_ticks);
        return(TRUE);
    } else {
        isdn_info->call_tbl = tmp->next;
	free(tmp);
        return(FALSE);
    }
}

/*
 * isdn_dialer_drop
 * Hang up the line.
 * Called by SERVICE_MEDIA_DIALER_DROP with index DIALER_ISDN
 */

static void isdn_dialer_drop (hwidbtype *hwidb)
{
    isdn_bri_t *isdn_info;
    dialerdbtype *ddb;

    /*
     * For ISDN dialer dialer_disconnect is called every time that the
     * interface is reset; we thus have to make sure that there is
     * a call in progress before disconnecting it, which is done in
     * dialer_disconnect().
     */
    ddb = get_dialerdb(hwidb);
    if (!DIALER_INPROGRESS(ddb))
	return;

    isdn_info = hwidb->isdn_info;
    isdn_fromrouter(ISDN_HANGUP, isdn_info->call_id, NULL, 0,
		    isdn_info->ces, hwidb, isdn_info->spc);
	
}

/*
 * isdn_dialer_shutdown
 * Here on SERVICE_HWIF_GOINGDOWN to shutdown an ISDN interface.
 */
static void isdn_dialer_shutdown (dialerdbtype *ddb)
{
    call_id_string_t *tmp, *tmp2;
    hwidbtype *idb;
    isdn_bri_t *isdn_info;

    idb = ddb->ownidb;
    isdn_info = idb->isdn_info;
    if (isdn_info == NULL)
	return;

    /*
     * Clean up call table, and flush out hold queues.
     */
    tmp = isdn_info->call_tbl;
    while (tmp != NULL) {
	destroy_timer(tmp->carrier_timer);
	isdn_increment_bchan(isdn_info);
	dialer_decrement_call(ddb);
	dialer_holdq_discard_queue(&tmp->holdq_ptr, "Interface shutdown");
	if (tmp->call_group)
	    tmp->call_group->cip = NULL;
        if (tmp->dial_reason)
	    free(tmp->dial_reason);
	tmp2 = tmp;
	tmp = tmp->next;
	free(tmp2);
    }
    isdn_info->call_tbl = NULL;
}

/*
 * isdn_clear_group
 * Clear the "dialer_call_group" field if it matches the passed map pointer.
 * Clear it unconditionally if the pointer is NULL.
 */
static void isdn_clear_group (dialerdbtype *ddb, dialermaptype *map)
{
    call_id_string_t *tmp;

    /*
     * First clear the interface's own call group.
     */
    if ((ddb->dialer_call_group == map) || (map == NULL)) {
	ddb->dialer_call_group = NULL;
    }

    /*
     * Clear call group in each call table element.
     * Clear dialer statistics also since the element has been deleted.
     */
    tmp = ddb->ownidb->isdn_info->call_tbl;
    while (tmp != NULL) {
	if ((tmp->call_group == map) || (map == NULL)) {
	    tmp->call_group = NULL;
            tmp->stats = NULL;
	}
	tmp = tmp->next;
    }
}

/*
 * isdn_dialer_is_free
 * Returns TRUE if there is a B channel available to place a call.
 */
static boolean isdn_dialer_is_free (dialerdbtype *ddb)
{
    return(ddb->ownidb->isdn_info->num_avail_b > 0);
}

/*
 * isdn_get_free_dialer
 * Returns D channel idb if there is a B channel available to place a call.
 */
static dialerdbtype *isdn_get_free_dialer (dialerdbtype *ddb)
{
    hwidbtype *idb;

    idb = ddb->ownidb;
    if (idb->isdn_info->num_avail_b > 0) {
	return(ddb);
    }

    return(NULL);
}

/*
 * isdn_dialer_pending
 * Since the number called cannot be bound to an interface until the switch 
 * tells us what B channel to use, we keep a list of calls we have in progress.
 * If there is a call in progress for the given address/link pair, then 
 * return true.  Otherwise, return false.
 */
static dialerdbtype *isdn_dialer_pending (dialerdbtype *ddb, dialerpaktype *dpak)
{
    hwidbtype *idb;
    dialermaptype *map;
    dialerentrytype *e;
    call_id_string_t *tmp;
    dialerdbtype *outddb, *tempddb;

    idb = ddb->ownidb;
    map = dpak->map;

    if (map != NULL) {
	e = map->entry;
	if (e->dialer_string != NULL) {
	    /*
	     * for each number in the dialer table, walk down the linked list
	     * looking for the call id
	     */
	    while (e) {
		tmp = idb->isdn_info->call_tbl;
		while (tmp) {
		    if (strcmp(e->dialer_string, tmp->called_num) == 0) {
			/*
			 * We need to set the D channel idb hold queue ptr
			 * to point to the real hold queue.  This is so that
			 * the calling routine know where to queue this pak.
			 */
		        dpak->holdQ = tmp->holdq_ptr;
			return(ddb);
		    }
		    tmp = tmp->next;
		}
		e = e->next;
	    }
	}
    } else {
	if (ddb->dialer_string != NULL) {
	    tmp = idb->isdn_info->call_tbl;

	    /*
	     * walk down the linked list looking for the call id
	     */
	    while (tmp) {
		if (strcmp(ddb->dialer_string->entry->dialer_string,
			   tmp->called_num) == 0) {
		    /*
		     * We need to set the D channel idb hold queue ptr
		     * to point to the real hold queue.  This is so that
		     * the calling routine know where to queue this pak.
		     */
		    dpak->holdQ = tmp->holdq_ptr;
		    return(ddb);
		}
		tmp = tmp->next;
	    }
	}
    }

    /*
     * We didn't find an element in the call table, so check whether there
     * is a B channel connected but not fully operational.
     */
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
        if ((outddb = dialer_pending(tempddb, dpak))) {
	    return(outddb);
	}
    }
    return(NULL);
}

/*
 * show_isdn_dialer_info
 * ISDN support for 'show dialer' command.
 */
static void show_isdn_dialer_info (dialerdbtype *ddb)
{
    hwidbtype *idb;

    idb = ddb->ownidb;
    printf("\n\n%s - dialer type = ISDN", idb->hw_namestring);
    if (idb->isdn_info->chantype == ISDN_Dchan) {
	if (HUNTGROUP_MEMBER(ddb)) {
	    printf("\nRotary group %d, priority = %u",
		   ddb->hunt_group_master->ownidb->unit, ddb->dialer_priority);
	} else 
	    show_dialer_stats(ddb);
	printf("\n%d incoming call(s) have been screened.",
		idb->isdn_info->num_screens);
    } else {
	show_dialer_state(ddb);
    }

    automore_conditional(0);
}

/*
 * init_pri_idb
 * Let's initialize this idb to run pri.
 * Note - we are assuming that isdn_info->d_chan_idb is set up
 * before calling this routine for a b channel
 */
boolean init_pri_idb (hwidbtype *idb)
{
    dialerdbtype *ddb;

    /*
     * mip_encapsulation has already created this hunt group
     * for PRI. dialer_set_member_encap is a generic routine
     * and nulls out these values. Let's save them first and
     * restore them after we return.
     */

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	/* I don't know why we need this... */
	reset_dialer_fields(ddb);
    } else {
	ddb = init_dialer_idb(idb);
	if (ddb == NULL) {
	    return(FALSE);
	}
    }
    ddb->dialer = DIALER_ISDN;
    idb->span_bridge_process_force |= SPAN_BR_PROC_DIALER;
    reg_invoke_fast_setup(idb);

    if (idb->isdn_info->chantype != ISDN_Dchan) {
	dialerdbtype *d_ddb;

	d_ddb = get_dialerdb(idb->isdn_info->d_chan_idb);
	isdn_add_bchan_to_hunt_group(d_ddb, ddb);
						  
        idb->reset = dialer_disconnect;
    } 

    return(TRUE);
}

/*
 * isdn_print_version
 */
static void isdn_print_version (void)
{
    if (!nbrinets && (nt1ports || ne1ports || nst1ports || nse1ports))
	printf("Primary Rate ISDN software, Version 1.0.\n");
    else if (nbrinets)					/* @@@@ */
	printf("Basic Rate ISDN software, Version 1.0.\n");	/* @@@@ */
}

/*
 * isdn_interface_command_ok
 *
 * apply any validity tests ISDN may need for interface command
 */
static boolean isdn_interface_command_ok (parseinfo *csb)
{

    hwidbtype *hwidb;

    hwidb = hwidb_or_null(GETOBJ(idb,1));

    /*
     * the caller ensures hwidb is not NULL
     */

    /*
     * don't allow configuration of other than D channels
     */
    if (!csb->nvgen) {
	if (hwidb->isdn_info &&
	    (hwidb->status & IDB_NO_NVGEN)) {
	    printf("\n%% Cannot access B-channel interfaces");
	    return (FALSE);
	}
    }
    return (TRUE);
}

char *isdn_get_number (hwidbtype *hwidb, int port)
{
    hwidbtype *dchan_idb;
    int dsl;
    
    if (hwidb->isdn_info == NULL)
	return(NULL);

    if (hwidb->isdn_info->chantype != ISDN_Dchan)
	dchan_idb = hwidb->isdn_info->d_chan_idb;
    else
	dchan_idb = hwidb;
    /*
     * Get the interface number from the idblist 
     */
    for (dsl = 0; dsl < MAX_DSL; dsl++)
	if (isdn_idblist[dsl] == dchan_idb)
	    break;
    if (dsl == MAX_DSL) {
	return(NULL);
    }
    if (port == 0)
	return(isdn_answer[dsl].answer1);

    return(isdn_answer[dsl].answer2);
}

/*
 * =====================================================================
 * isdn_get_calling_number()
 * isdn_get_called_number()
 *
 * Description:
 * These functions are called through registry 
 * reg_invoke_modem_get_calling_number() and 
 * reg_invoke_modem_get_called_number() respectively.
 *
 * Call these two registry services to retrieve the Calling Party Number or 
 * the Called Party Number for users on ISDN interfaces.
 *
 * Parameters:
 * tty       - tty line.  Ignored by this function.
 * idb       - interface that might be isdn and might have info
 * ret_ptr   - The calling function uses ret_ptr to pass a pointer to
 *             the location where the Calling/Called Number should be
 *             put (or rather where a char pointer to the Number should
 *             be put).
 *
 * Returns:
 * True if the Number is available; False if Number is unavailable.
 * =====================================================================
 */

static boolean
isdn_get_calling_number (tt_soc *tty, hwidbtype *idb, char **ret_ptr)
{
    /* 
     * registry call; so have paranoia!
     */
    if (ret_ptr == NULL)
        return FALSE;
    
    if (IS_ISDN(idb)) {
	if (idb->isdn_info->remote_src == ISDN_SRC_ISDN &&
	    *idb->isdn_info->remote_number) {
	    *ret_ptr = idb->isdn_info->remote_number;
	    return(TRUE);
	}
    }
    return FALSE;	/* Calling party number unavailable */
}

/*
 * isdn_init
 */
static void isdn_init (subsystype *subsys)
{
    /*
     * Link in the parser chains
     */
    isdn_parser_init();
    isdn_debug_init();
    is_pri_supp = TRUE;
    Call_ID = ISDN_DIALER_CALL_ID_MIN;

    /*
     * Now register a few things
     */
    reg_add_dialer_shutdown(DIALER_ISDN, isdn_dialer_shutdown, 
			    "isdn_dialer_shutdown");
    reg_add_dialer_clear_group(DIALER_ISDN, isdn_clear_group, 
			       "isdn_clear_group");
    reg_add_dialer_pending(DIALER_ISDN, isdn_dialer_pending,
			   "isdn_dialer_pending");
    reg_add_setup_global2(isdn_setup_global2, "isdn_setup_global2");
    reg_add_dialer_is_free(DIALER_ISDN, isdn_dialer_is_free, 
			   "isdn_dialer_is_free");
    reg_add_get_free_dialer(DIALER_ISDN, isdn_get_free_dialer, 
			    "isdn_get_free_dialer");
    reg_add_dialer_dial(DIALER_ISDN, isdn_dialer_dial, "isdn_dialer_dial");
    reg_add_dialer_drop(DIALER_ISDN, isdn_dialer_drop, "isdn_dialer_drop");
    reg_add_dialer_show_info(DIALER_ISDN, show_isdn_dialer_info, 
			     "show_isdn_dialer_info");
    reg_add_isdn_pri_init(isdn_pri_init, "isdn_pri_init");
    reg_add_init_pri_idb(init_pri_idb, "init_pri_idb");
    reg_add_isdn_pri_setencap(isdn_setencap, "isdn_setencap");
    reg_add_isdn_pri_init_core(isdn_pri_init_core, "isdn_pri_init_core");
    reg_add_isdn_pri_getswitch(isdn_pri_getswitch, "isdn_pri_getswitch");
    reg_add_isdn_set_chan_state(isdn_set_chan_state, "isdn_set_chan_state");
    reg_add_dialer_is_answer(DIALER_ISDN, isdn_bri_is_answer,
			     "isdn_bri_is_answer");
    reg_add_print_copyright(isdn_print_version, "isdn_print_version");

    reg_add_net_cstate(isdn_cstate, "isdn_cstate");
    reg_add_interface_command_ok(IDBTYPE_T1, isdn_interface_command_ok,
				 "isdn_interface_command_ok");
    reg_add_interface_command_ok(IDBTYPE_E1, isdn_interface_command_ok,
				 "isdn_interface_command_ok");
    reg_add_interface_command_ok(IDBTYPE_BRI, isdn_interface_command_ok,
				 "isdn_interface_command_ok");
    reg_add_isdn_verify_dsl(isdn_verify_dsl, 
                            "isdn_verify_dsl");
    reg_add_pri_enq_prim(pri_enq_prim,"pri_enq_prim");
    reg_add_modem_get_calling_number(isdn_get_calling_number, 
				    "isdn_get_calling_number");

    /* Initialize list of dsl id's */
     isdn_init_dsl_list();
    /*
     * Set up x25 private function registry
     */
    isdn_regcode = registry_create(REG_UNKNOWN, SERVICE_ISDN_MAX, "ISDN");
    if (isdn_regcode != REG_ILLEGAL)
        create_registry_isdn();

}

    

/*
 * ISDN subsystem header
 */

#define ISDN_MAJVERSION 1
#define ISDN_MINVERSION 0
#define ISDN_EDITVERSION  1

SUBSYS_HEADER(isdn, ISDN_MAJVERSION, ISDN_MINVERSION, ISDN_EDITVERSION,
	      isdn_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
      
