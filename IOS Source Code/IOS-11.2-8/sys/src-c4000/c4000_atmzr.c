/* $Id: c4000_atmzr.c,v 3.19.4.28 1996/09/04 23:59:49 hxu Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_atmzr.c,v $
 *------------------------------------------------------------------
 * C4000 router family ATMizer driver.
 *
 * October, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_atmzr.c,v $
 * Revision 3.19.4.28  1996/09/04  23:59:49  hxu
 * CSCdi58134:  ATM: 4500 hangs after atmzr_hi_irq_hand(ATM0): Secondary
 * port error
 * Branch: California_branch
 * Reset the interface when the unexpected interrupt takes place
 *
 * Revision 3.19.4.27  1996/08/28  13:12:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.19.4.26  1996/08/18  19:12:10  hxu
 * CSCdi65263:  Incorrect high mark value for ATM private buffer pool
 * Branch: California_branch
 * Used fixed high mark value for supporting single active private buffer
 * poo
 *
 * Revision 3.19.4.25  1996/08/14  20:51:05  nrvankay
 * CSCdi54374:  Broken fast switching over ATM when AAL3/4 config and
 * ATM-RX
 * Branch: California_branch
 * Add the code to decode the SMDS encapsulation
 *
 * Revision 3.19.4.24  1996/08/13  02:23:41  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.19.4.23  1996/08/03  23:36:32  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.19.4.22  1996/07/29  21:23:15  hxu
 * CSCdi63953:  Randomzied value for CPCS-UU and CPI
 * Branch: California_branch
 * put 00 into cpcs-uu and cpi fields
 *
 * Revision 3.19.4.21  1996/07/20  03:20:24  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.19.4.20  1996/07/04  00:14:20  rbadri
 * CSCdi61232:  IP & IPX ping packets > 1484 would fail on RSP (1483-LANE
 * Bridging)
 * Branch: California_branch
 * For ATM, check the incoming datagramsize against sub_mtu + encaps
 * overhead.
 *
 * Revision 3.19.4.19  1996/07/03  00:01:55  rzagst
 * CSCdi55228:  spurious messages when PLIM is oversubscribed
 * Branch: California_branch
 *
 * Revision 3.19.4.18  1996/06/29  22:04:46  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.19.4.17  1996/06/27  23:02:55  hxu
 * CSCdi57984:  show interface atm shows incorrect value for current VCCs
 * Branch: California_branch
 * Let upper-layer cleanup routines finish before firmware reset.
 *
 * Revision 3.19.4.16  1996/06/26  20:35:12  rbadri
 * CSCdi61312:  Broadcast packets (1483 PVC) are not properly filtered
 * Branch: California_branch
 * Move setting up of atm_vcid & atm_vc.
 *
 * Revision 3.19.4.15  1996/05/31  15:31:01  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.19.4.14  1996/05/27  05:52:42  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Branch: California_branch
 *
 * Revision 3.19.4.13  1996/05/22  22:34:21  hxu
 * CSCdi56054:  4500 uses 18K buffer pool even for a 9180 MTU
 * Branch: California_branch
 * Adding a private 9180-byte buffer pool to accommodate 9180 MTU request.
 *
 * Revision 3.19.4.12  1996/05/22  17:28:35  hxu
 * CSCdi56497:  Memory allocation failures with light traffic
 * Branch: California_branch
 * Avoid excessive buffer allocation when the real value of "quota"
 * in atmzr_refill_freeq_inline() becomes negative
 *
 * Revision 3.19.4.11  1996/05/21  21:40:47  hxu
 * CSCdi57776:  4500 router crashes when 2nd ATM DS3 NIM is pinged
 * Branch: California_branch
 * Correct subunit value to accommodate the fact for one ATM interface
 * per slot.
 *
 * Revision 3.19.4.10  1996/05/17  11:53:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.17.2.4  1996/05/05  23:52:55  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.17.2.3  1996/04/25  18:14:35  pleung
 * CSCdi55222:  appn/atm broken on C4500
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.2  1996/04/03  21:41:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.17.2.1  1996/03/17  18:30:32  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.19.4.9  1996/05/13  22:54:20  hxu
 * CSCdi50852:  Buffer pool allocation for VCs does not follow VCs MTU
 * Branch: California_branch
 * Allowing LANE use 1524-byte buffer pool for efficient I/O memory usage.
 *
 * Revision 3.19.4.8  1996/05/13  18:41:20  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.19.4.7  1996/05/10  21:03:17  hxu
 * CSCdi53038:  ATM fails to create VC on 4700
 * Branch: California_branch
 * Add a return in atmzr_proc_rsp() to accommodate the fact
 * that async commands (between system and firmware) are currently
 * unsupported.
 *
 * Revision 3.19.4.6  1996/05/09  14:45:50  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.19.4.4.2.1  1996/04/27  07:25:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.19.4.5  1996/05/05  17:08:40  rzagst
 * CSCdi54165:  Cant create pvcs during subinterface shutdown
 * Branch: California_branch
 *
 * Revision 3.19.4.4  1996/04/25  23:22:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.19.4.3  1996/04/17  13:47:41  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.19.4.2.8.3  1996/04/12  07:54:52  kjadams
 * Branch: LE_Cal_V112_0_2_branch
 * Added cases for TR LANE - and necessary includes.
 *
 * Revision 3.19.4.2.8.2  1996/04/08  23:05:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Missing include for if/tring.h from TR_LANE commit.
 *
 * Revision 3.19.4.2.8.1  1996/04/08  02:09:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.19.4.2  1996/03/18  22:00:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.19.4.1  1996/03/17  19:23:27  hxu
 * CSCdi49809:  Closing SVC errors after major atm interface shutdown
 * Branch: California_branch
 * atmzr_tear_down() should delay VC closing command and RQ closing command
 * if ATMzr is already disabled.
 *
 * Revision 3.19.6.1  1996/03/28  17:36:00  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.19  1996/03/08  23:30:34  shj
 * CSCdi50945:  LANE client does not handle data arriving on multicast-send
 * Treat packets coming in on mcast-send same as mcast-forward.
 *
 * Revision 3.18  1996/02/27  19:28:01  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.17  1996/02/23  00:55:43  rzagst
 * CSCdi43692:  ATM subinterfaces ignore subinterface shutdowns
 *
 * Revision 3.16  1996/02/07  02:52:49  shj
 * CSCdi48298:  pkts fastswitched to LANE get extra bytes on 4500/7500
 * Set pak->encsize correctly for LANE packets in
 * atmzr_decode_encaps_inline.
 *
 * Revision 3.15  1996/02/01  06:09:15  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.14  1996/01/29  21:38:52  mohsen
 * CSCdi44192:  Add Adaptation layer 3/4 support for 4500 ATM NIM
 * Implement AAL3/4 (ITU-T I.363)
 *
 * Revision 3.13  1996/01/29  21:18:28  mohsen
 * CSCdi44191:  Enable the support for E3 and DS3 PLIMs on 4500 ATM NIM
 * Initialize E3 and DS3 PLIMS on startup.
 *
 * Revision 3.12  1996/01/29  07:30:02  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.11  1996/01/26  03:11:19  awu
 * CSCdi43779:  Declaring or removing a PVC can cause loss of all SVCs.
 *  -change the way of setting up a rateq so that resetting the interf card
 *   is being bypassed.
 *
 * Revision 3.10  1996/01/25  01:03:18  hxu
 * CSCdi46323:  atmzr shows wrong VCCs associated w/ a rateq
 * An active VC is expected to be associated with a non-zero value of
 * vcmode.
 * A new constant ATMZR_AIP_HACK is created to guarantee this.
 *
 * Revision 3.9  1996/01/24  21:42:27  mohsen
 * CSCdi46503:  atm nim transmit hung sometimes
 * Have the driver clear the command queue entries after they are
 * processed by the firmware. Leftover entries put the firmware in
 * an infinite command processing loop which results in driver
 * reseting the interface.
 *
 * Revision 3.8  1996/01/06  03:23:09  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.7  1995/12/12  00:01:16  mohsen
 * CSCdi44687:  4500 ATM NIM can not receive any incoming traffic
 * Modify the 4500 ATM driver to work properly with the recent
 * changes made in CSCdi31527.
 *
 * Revision 3.6  1995/12/11  01:48:25  mohsen
 * CSCdi38494:  traffic exceeds peak rate on the ATM NMP/4500
 * When openeing rate queues explicitly via atm rate queue command,
 * select the clock source properly. Also fix the arithmetic.
 *
 * Revision 3.5  1995/12/02  03:22:55  mohsen
 * CSCdi44189:  SMDS encapsulation is broken on 4500 ATM environment
 * Build SMDS header and trailer for process/fast switched packets.
 *
 * Revision 3.4  1995/11/23  19:58:14  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.3  1995/11/17  18:44:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:27:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/08/30  00:59:35  wfried
 * CSCdi39530:  Need to limit the number of ATM interfaces per box
 * Limit number of ATM interfaces per type (only 1 ATM OC-3)
 *
 * Revision 2.13  1995/08/30  00:33:05  wfried
 * CSCdi39526:  Need framing driver version number
 * Include framing driver version number in output of *show controller atm*
 *
 * Revision 2.12  1995/08/28  15:47:53  wfried
 * CSCdi39408:  ATM interface up decision should be based on more than
 * just LOS
 * Declare interface up only if PLCP framing or Cell delineation is
 * recognized for E3/DS3 interfaces.
 *
 * Revision 2.11  1995/08/25  11:45:07  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.10  1995/08/14  23:23:11  wfried
 * CSCdi38140:  ATM NIM cant ping itself when a 2E NIM is in the box
 * Move setting of IDB_HEARSELF to platform dependent code.
 *
 * Revision 2.9  1995/08/10  23:31:29  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.8  1995/07/14  19:33:20  wfried
 * CSCdi37118:  Wrong SONET OC-3 bandwidth being reported on ATM interface
 * Remove extra interface bandwidth setting which was overwriting the
 * correct value.
 *
 * Revision 2.7  1995/07/06  01:14:42  wfried
 * CSCdi36736:  Interface reset causes infinite recursion
 * Prevent transmission of packets on closed VCs and update interface
 * control data structures before calling media registry functions.
 *
 * Revision 2.6  1995/06/27  00:30:54  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.5  1995/06/21  03:15:16  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.4  1995/06/20  07:18:29  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/19  07:17:04  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/10  17:50:57  pitargue
 * CSCdi35638:  packets going to process level from the 4500 atm nim
 * crashes router
 * make sure to initialize fast_vc_info, before calling
 * hwidb->parse_packet
 *
 * Revision 2.1  1995/06/07  22:55:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*******************************************************************
			Include Files
 *******************************************************************/

#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../atm/parser_defs_atm.h"
#include "logger.h"
#include "../os/region.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/msg_system.c"  
#include "../if/network.h"
#include "../wan/smds.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../os/timer_generic.h"
#include "c4000_atmzr.h"
#include "../tbridge/tbridge_public.h"
#include "../atm/atm_oam.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include "../if/ether_inline.h"
#include "../srt/trsrb.h" 
#include "../if/rif_inline.h" 
#include "../tbridge/tbridge_sde.h"
#include "../if/tring_inline.h"
#include "../lane/lane.h"
#include "../atm/atmsig_api.h"	
#include "../lane/lane_private.h"	
#include "../lane/lanefast_les.h"
#include "../lane/lane_registry.h"
#include "../if/traffic_shape.h"
#include "../parser/parser_defs_parser.h"


/*******************************************************************
			Definitions
 *******************************************************************/

#define	DEBUG_COUNTERS(expr)	expr
#define	DEBUG_EVENTS		if (atm_event_debug) buginf
#define	DEBUG_ERRORS		if (atm_errors_debug) buginf
#define	DEBUG_OAM		if (atm_oam_debug) buginf


/*******************************************************************
			Variables
 *******************************************************************/

extern unsigned short atmizer_firmware[];
extern int atmizer_firmware_words;
extern char atmizer_version[];
extern ulong atm_xilinx[];
extern ulong atm_endxilinx[];
extern char atm_xilinx_version[];

static atmzr_hw_t		atmzr_hardware[C4000_NUM_SLOTS];
static ushort			natmoc3;	/* max # ATM OC3 interf/box */



/*******************************************************************
		ATMizer ATM sybsystem header
 *******************************************************************/
 
#define ATMZR_MAJVERSION 1
#define ATMZR_MINVERSION 0
#define ATMZR_EDITVERSION  1
 
SUBSYS_HEADER(atmzr, ATMZR_MAJVERSION, ATMZR_MINVERSION, ATMZR_EDITVERSION,
              atmzr_subsys_init, SUBSYS_CLASS_KERNEL,
              NULL,
              NULL);

 
/*******************************************************************
			Functions
 *******************************************************************/


/*
 * a t m z r _ s u b s y s _ i n i t  ( )
 *
 * ATMizer subsystem initialization.
 */
void
atmzr_subsys_init (subsystype *subsys)		/* Subsystem struc ptr */
{
    /*
     * Register the NIM IDs for the various BSI FDDI NIMs
     */
    natmoc3 = 0;
    reg_add_analyze_interface (XX_ATM_BX50,
                               atmzr_analyze_interface,
                               "atmzr_analyze_interface");

    /*
     * Register hardware function registry
     */
    reg_add_media_hw_setup (atmzr_setup, "atmzr_setup");

    /*
     * Add final startup routine
     */
    reg_add_if_final_init (atmzr_if_final_init, "atmzr_if_final_init");

}


/*
 * a t m z r _ a n a l y z e _ i n t e r f a c e  ( )
 *
 * Analyze ATMizer NIM in the given slot.
 */
void
atmzr_analyze_interface (int slot)		/* Slot number */
{
    hwidbtype                   *idb;           /* HW IDB pointer */
    int				iosize;		/* Shared mem size */

    /*
     * Check available shared memory
     */
    iosize = region_get_size_by_class(REGION_CLASS_IOMEM);

    /*
     * Allocate and initialize hardware IDB.
     * Don't recognize interface if not enough
     * system memory to build control data structures.
     */
    if (!(idb = atmzr_init_idb (slot, natmnets)))
	return;

    /*
     * Make sure that the public SRAM size is big
     * enough to hold all the interface data structures.
     * If not, reject this NIM.
     */
    if (((atmzr_instance_t *)INSTANCE)->nim_pusram_size <
	(sizeof (pusram_t) + sizeof (intr_t))) {
        errmsg(&msgsym(NOPUSRAM, ATM), idb->unit);
	return;
    }

    /*
     * Register interrupt handlers
     */
    init_atmzr_dev (idb);
    atmzr_hardware[slot].hwidb = idb;
    nim_register_hi_irq_handler (idb->slot, atmzr_hi_irq_hand,
				 IDBTYPE_ATMZR, idb->slot);

    /*
     * Add this NIM to the number of ATM interfaces
     */
    natmnets++;
}


/*
 * a t m z r _ i n i t _ i d b  ( )
 *
 * ATMizer hardware IDB allocation and initialization.
 * Only the device specific IDB parameters are filled in here,
 * and the ATM platform independent code atm_setup() will fill
 * in the rest.
 *
 * NOTE: The Xilinx has to be downloaded before NIM registers
 * in general can be read. The only two registers that can be
 * read without downloading the Xilinx are the PC_ID and PC_REV
 * registers.
 *
 * Returns idb structure pointer.
 */
hwidbtype *
atmzr_init_idb (int slot,			/* Box slot number */
		int unit)			/* ATM unit number */
{
    hwidbtype			*idb;		/* HW IDB pointer */

    /*
     * ALlocate hardware IDB memory block
     */
    idb = idb_create();
    if (!idb) {
        return (NULL);
    }

    /*
     * Initialize hardware IDB
     */
    /* Global entries */
    idb->unit = unit;
    idb->subunit = 0; /* interface density on each NIM is only 1 */
    idb->slot = slot;
    idb->type = IDBTYPE_ATMZR;
    idb->name = IFNAME_ATM;
    idb->status = IDB_ATM | IDB_SYNC | IDB_BRIDGE | IDB_BCAST | IDB_HEARSELF;
    idb->counters.resets = 0;
    idb->nim_type = nim_get_type(slot);
    idb->nim_version = nim_get_version(slot);
    idb->typestring  = atmzr_nim_type_string(idb->nim_type);
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;

    /* Encapsulation parameters */
    idb->maxmtu_def = idb->maxmtu = ATM_DEFAULT_MTU;
    idb->firstsw->sub_mtu = idb->maxmtu_def;
    idb->span_encapsize = idb->encsize;
    idb->bridge_transit = TRUE;
    idb->firstsw->tbridge_media = TBR_ATM_TRANSIT;

    /* IDB functions */
    idb->soutput = atmzr_output;
    idb->reset = atmzr_reset;
    idb->shutdown = atmzr_shutdown;
    idb->lineaction = atmzr_line_action;
    idb->linestate = atmzr_line_state;
    idb->enable = atmzr_enable;
    idb->clear_support = atmzr_clear_counters;
    idb->system_configured = atmzr_system_configured;
    idb->show_controller = atmzr_show_controller;
    idb->fastsend = atmzr_fastsend;
    idb->set_maxdgram = atmzr_set_maxdgram;

    /* Time stamps */
    TIMER_STOP (idb->lastinput);
    TIMER_STOP (idb->lastoutput);
    TIMER_STOP (idb->lastreset);

    /*
     * Initialize device control structure hardware & software parameters
     */
    if (!atmzr_init_hw_ds (idb)) {
	if (INSTANCE)
	    free (INSTANCE);
	free (idb);
	return (NULL);
    }
    atmzr_init_sw_ds (idb);

    /* Setup ATM global control structures */
    if (reg_invoke_media_atm_setup (idb)) {
	reg_invoke_media_hw_setup (idb);
    } else {
	free (idb);
	return (NULL);
    }

    /*
     * Get a MAC address
     */
    nim_get_slot_hardware_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware))
        nim_get_slot_hardware_address(idb, idb->hardware);

    /*
     * Enqueue newly created IDB
     */
    idb_enqueue(idb);

    return (idb);
}


/*
 * a t m z r _ c r e a t e _ p o o l  ( )
 *
 * Create a buffer pool to hold buffers of the given size.
 * Each hardware interface has its own private buffer pool.
 *
 * Buffers will only be allocated to this pool when the first VC using
 * this pool is setup. The number of buffers allocated to this pool
 * (while there is at least one VC using it) will depend on the
 * overall VC allocation. The total number of buffers allocated to
 * this hardware interface is limited by the number of available
 * receive packet descriptors. Each pool gets a weigthed share of
 * this total, according to the percentual number of VCs that use
 * this particular pool.
 *
 * This function is called once for every pool in the group of pools
 * assigned to this interface, which is designed to support 5 buffer
 * pools supporting the MTUs also supported by the public pools.
 * there might be numerous VCs opened on this same hardware interface,
 * each possibly using a different MTU.
 *
 * It is assumed that when this function is called, that the PLIM
 * attributes have already been set, which is the main parameters
 * that will specify the pool cache.
 */
static pooltype *
atmzr_create_pool (hwidbtype *idb,		/* Interface HW IDB ptr */
		   ushort mtu)			/* Pool MTU size */
{
    atmzr_instance_t            *ds;            /* ATMizer dev struc */
    pooltype			*pool;		/* Pool pointer */
    int				cache_size;	/* Pool cache size */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Create buffer pool group if first buffer pool
     */
    if (idb->pool_group == POOL_GROUP_INVALID)
	idb->pool_group = pool_create_group ();

    /*
     * Create pool
     */
    if (!idb->hw_namestring)
        idb_init_names(idb->firstsw, FALSE);
    pool = pak_pool_create(idb->hw_namestring, idb->pool_group, mtu,
			       POOL_SANITY, NULL);
    if (!pool) {
	crashdump (0);
    }
    idb->pool = pool;

    /*
     * Customize pool
     */
    pool->fallback = pak_pool_find_by_size(mtu);

    /*
     * Add buffer cache - the cache size will depend on the media speed.
     *
     * The buffer cache will only be populated/depopulated as VCs
     * are opened/closed. The number of buffers allocated
     * to the buffer cache will (when at least one VC uses this
     * particular MTU) hold up to twice the queue high water mark to
     * ensure the "continuous replenishment of buffers (to allow
     * for burst of small packets) and limit the maximum amount
     * of memory consumed per ATM interface.
     */
    cache_size = 2 * (ds->plim_linerate * RXPKTDLIMIT) / ATMZR_PLIMRATEMAX;
    if (!pak_pool_create_cache(pool, cache_size)) {
	crashdump(0);
    }

    return (pool);
}


/*
 * a t m z r _ i n i t _ h w _ d s  ( )
 *
 * ATMizer descriptor structure hardware parameters initialization.
 * Only the configuration independent parameters are initialized
 * in this function. The configuration dependent attributes will
 * have to wait until atmzr_system_configured() is called
 * to complete the device structure initialization.
 */
boolean
atmzr_init_hw_ds (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* ATMizer dev struc */
    atmzr_regs_t                *rp;            /* ATMizer NIM registers ptr */

    /*
     * Initialize hardware register pointer
     */
    rp = (atmzr_regs_t *)((uchar *)GET_SLOT_BASE (idb->slot));
 
    /*
     * Make sure we are dealing with the correct ATMizer NIM
     * (based on the ATMizer BX-50)
     */
    if (idb->nim_type != XX_ATM_BX50)
        return (FALSE);
 
    /*
     * Allocate control structure for this driver instance
     * and load basic NIM hardware pointers.
     */
    ds = malloc(sizeof(atmzr_instance_t));
    if (!ds) {
        return (FALSE);
    }
    INSTANCE = ds;
    ds->hwidb = idb;
    ds->nim_regs = rp;
 
    /*
     * Download Xilinx
     */
    if (!atmzr_download_xilinx (idb))
        return (FALSE);

    /*
     * Reset NIM and release PLIM from reset.
     * NOTE: In order to be able to read the PLIM_REV/VER
     * register, the PLIM has to be taken out of reset. Note
     * also that there is a delay from the time the PLIM is
     * taken out of reset until the PLIM_REV/VER register can
     * read the correct value.
     */
    rp->ctlr = ATMZR_CTLR_RSTPLIMN;

    /*
     * Initialize NIM device control hardware attributes
     */
    switch (rp->memcfgr & ATMZR_MCFGR_PUSRAM_MSK) {
    case ATMZR_MCFGR_PUSRAM_8K: ds->nim_pusram_size = 8 * 1024; break;
    case ATMZR_MCFGR_PUSRAM_16K: ds->nim_pusram_size = 16 * 1024; break;
    case ATMZR_MCFGR_PUSRAM_32K: ds->nim_pusram_size = 32 * 1024; break;
    case ATMZR_MCFGR_PUSRAM_64K: ds->nim_pusram_size = 64 * 1024; break;
    default: return (FALSE);
    }
    switch (rp->memcfgr & ATMZR_MCFGR_PRSRAM_MSK) {
    case ATMZR_MCFGR_PRSRAM_32K:  ds->nim_prsram_size = 32 * 1024; break;
    case ATMZR_MCFGR_PRSRAM_128K: ds->nim_prsram_size = 128 * 1024; break;
    case ATMZR_MCFGR_PRSRAM_512K: ds->nim_prsram_size = 512 * 1024; break;
    case ATMZR_MCFGR_PRSRAM_2M:   ds->nim_prsram_size = 2048 * 1024; break;
    default: return (FALSE);
    }
    ds->nim_proc_clock = ATMZR_CLOCK * 1000;	/* (kbps) */
    ds->nim_pru_clock = ATMZR_PRU_2U_RATE;	/* (kbps) */
    ds->intr = (intr_t *)((char *)&rp->pusram +
	ds->nim_pusram_size - sizeof (intr_t));

    /*
     * Initialize PLIM device control hardware attributes
     * Limit the number of OC3 interfaces allowed per box
     */
    ds->plim_type = ds->plim_version = rp->plimr;
    ds->plim_type &= ATMZR_PLIMR_ID_MSK;
    ds->plim_version = (ds->plim_version & ATMZR_PLIMR_REV_MSK) >> 8;
    switch (ds->plim_type) {
    case ATMZR_PLIMR_ID_STS3CSMF:
    case ATMZR_PLIMR_ID_STS3CMMF:
	if (natmoc3 >= MAX_ATMZR_OC3_NIMS)
	    return (FALSE);
	atmzr_sunilite_init (idb);
	natmoc3++;
	break;
    case ATMZR_PLIMR_ID_DS3: atmzr_sunipdh_ds3_init(idb); break;
    case ATMZR_PLIMR_ID_E3: atmzr_sunipdh_e3_init(idb); break;
    case ATMZR_PLIMR_ID_DS1: return (FALSE); break;
    case ATMZR_PLIMR_ID_E1: return (FALSE); break;
    case ATMZR_PLIMR_ID_NONE: return (FALSE); break;
    default: return (FALSE);
    }
    ds->pktid_rxlimit = (ushort)((ulong)((ds->plim_linerate * RXPKTDLIMIT) /
	ATMZR_PLIMRATEMAX));
    ds->pktid_txlimit = (PKTDESCRTSZ - 1) - ds->pktid_rxlimit;

    /* Spin wait delay */
    ds->delay_1ms = timer_calibrate (1000, FALSE);

    /*
     * Create buffer pools
     */
    ds->rxfreeQ[SMALL_FREEQ].bufpool = atmzr_create_pool (idb, SMALLDATA);
    ds->rxfreeQ[MID_FREEQ].bufpool = atmzr_create_pool (idb, MEDDATA);
    ds->rxfreeQ[BIG_FREEQ].bufpool = atmzr_create_pool (idb, BIGDATA);
    ds->rxfreeQ[LARGE_FREEQ].bufpool = atmzr_create_pool (idb, LARGEDATA);
    ds->rxfreeQ[SEMI_HUGE_FREEQ].bufpool = atmzr_create_pool(idb,
         SEMI_HUGE_FREEQ_MTU + ETHER_ARPA_ENCAPBYTES + ATM_SMDS_HDRBYTES);
    ds->rxfreeQ[HUGE_FREEQ].bufpool = atmzr_create_pool (idb, DEF_HUGEDATA);

    return (TRUE);
}


/*
 * a t m z r _ i n i t _ s w _ d s  ( )
 *
 * ATMizer descriptor structure software parameters initialization.
 */
void
atmzr_init_sw_ds (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;		/* ATMizer dev struc */
    rxfreeq_db_t		*freeq;		/* Free queue descr pointer */
    id2pak_t			*id2pakp;	/* ID <-> paktype entry ptr */
    pusram_t			*pusram;	/* Public SRAM pointer */
    ushort			indx;		/* Auxiliar counting index */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    pusram = &ds->nim_regs->pusram;

    /*
     * Initialize hardware idb variables that need to be
     * reinitialized every time the interface is reinitialized
     */
    TIMER_STOP (idb->xmittimer);

    /*
     * Initialize software data structures
     */

    /* Interface queues */
    for (indx=0; indx<NUM_FREEQ; indx++) {
	freeq = &ds->rxfreeQ[indx];
	freeq->high_mark = 0;
	freeq->level = 0;
	freeq->num_vc = 0;
	freeq->bufmiss = 0;
	freeq->cachemiss = 0;
	freeq->pktidmiss = 0;
    }
    ds->rxfreeQ[SMALL_FREEQ].name = "Small";
    ds->rxfreeQ[SMALL_FREEQ].base =
    ds->rxfreeQ[SMALL_FREEQ].wr_ptr = pusram->small_rxfreeQ;
    ds->rxfreeQ[MID_FREEQ].name = "Medium";
    ds->rxfreeQ[MID_FREEQ].base =
    ds->rxfreeQ[MID_FREEQ].wr_ptr = pusram->mid_rxfreeQ;
    ds->rxfreeQ[BIG_FREEQ].name = "Big";
    ds->rxfreeQ[BIG_FREEQ].base =
    ds->rxfreeQ[BIG_FREEQ].wr_ptr = pusram->big_rxfreeQ;
    ds->rxfreeQ[LARGE_FREEQ].name = "Large";
    ds->rxfreeQ[LARGE_FREEQ].base =
    ds->rxfreeQ[LARGE_FREEQ].wr_ptr = pusram->large_rxfreeQ;
    ds->rxfreeQ[SEMI_HUGE_FREEQ].name = "SemiHuge";
    ds->rxfreeQ[SEMI_HUGE_FREEQ].base =
    ds->rxfreeQ[SEMI_HUGE_FREEQ].wr_ptr = pusram->semi_huge_rxfreeQ;
    ds->rxfreeQ[HUGE_FREEQ].name = "Huge";
    ds->rxfreeQ[HUGE_FREEQ].base =
    ds->rxfreeQ[HUGE_FREEQ].wr_ptr = pusram->huge_rxfreeQ;
    ds->rxrdyQ.name = "Receive Ready";
    ds->rxrdyQ.base = ds->rxrdyQ.rd_ptr = pusram->rxrdyQ;
    ds->txrdyQ.name = "Transmit Ready";
    ds->txrdyQ.base = ds->txrdyQ.wr_ptr = pusram->txrdyQ;
    ds->txrdyQ.level = 0;
    ds->txdoneQ.name = "Transmit Done";
    ds->txdoneQ.base = ds->txdoneQ.rd_ptr = pusram->txdoneQ;
    ds->eventQ.name = "Event";
    ds->eventQ.base = ds->eventQ.rd_ptr = pusram->eventQ;
    ds->cmdrspQ.name = "Command/Response";
    ds->cmdrspQ.base = pusram->cmdrspQ;
    ds->cmdrspQ.limit = &pusram->cmdrspQ[CMDRSPQSZ];
    ds->cmdrspQ.wr_ndx = ds->cmdrspQ.rd_ndx = 0;
    ds->cmdrspQ.async_cnt = 0;
    ds->cmdrspQ.level = 0;
    for (indx=0; indx<CMDRSPQSZ; indx++) {
	ds->cmdrspQ.rspmon[indx].mb_ptr = &ds->cmdrspQ.base[indx];
	ds->cmdrspQ.rspmon[indx].data_buf = NULL;
	ds->cmdrspQ.rspmon[indx].rsp.rsp = 0;
	ds->cmdrspQ.rspmon[indx].rsp.stat = 0;
	ds->cmdrspQ.rspmon[indx].rsp.param = 0;
	TIMER_STOP(ds->cmdrspQ.rspmon[indx].timer);
	ds->cmdrspQ.rspmon[indx].rsp_dsize = 0;
	ds->cmdrspQ.rspmon[indx].async = FALSE;
    }
    ds->ndxq_ptrmask = (INDXQSZ * sizeof (indx_t)) - 1;
    ds->adjust_pools = TRUE;

    /* Packet descriptors - free interface allocated buffers */
    ds->pktdT = pusram->pktdT;
    ds->tx_control_flag = FALSE;
    ds->fastest_vc_rate = 0;
    for (indx=0; indx<PKTDESCRTSZ; indx++) {
	id2pakp = &ds->id2pakT[indx];
	if (id2pakp->pak_ptr)
	    datagram_done (id2pakp->pak_ptr);
	id2pakp->pak_ptr = NULL;
	id2pakp->qdb_ptr = NULL;
	id2pakp->next = indx + 1;
    }
    ds->id2pakT[PKTDESCRTSZ - 1].next = 0;
    ds->pktid_nextfree = 1; /* 1st entry is unused */
    ds->pktid_rxcount = ds->pktid_txcount = 0;

    /* Statistic counters */
    ds->fast_switched = 0;
    ds->bridged = 0;
    ds->to_process = 0;
    ds->tx_errors = 0;
    ds->tx_badid = 0;
    ds->tx_wrongq = 0;
    ds->tx_nopkt = 0;
    ds->tx_restarts = 0;
    ds->tx_pktidmiss = 0;
    ds->tx_badvc = 0;
    for (indx=0; indx<NUM_RXERR; indx++)
	ds->rx_errors[indx] = 0;
    ds->rx_badid = 0;
    ds->rx_wrongq = 0;
    ds->rx_nopkt = 0;

    /*
     * initialize reset flag
     */
    ds->hi_irq_reset = 0;

    /* Global */
    ds->nim_operational = FALSE;
    ds->last_rsp_stat = RSP_STAT_OK;
}


/*
 * a t m z r _ i f _ f i n a l _ i n i t  ( )
 *
 * ATMizer NIM final initialization.
 */
void
atmzr_if_final_init (void)
{
    /*
     * Register protocol functions
     */
    reg_add_atm_get_nlpidtype (atmzr_get_nlpidtype, "atmzr_get_nlpidtype");
}


/*
 * a t m z r _ s y s t e m _ c o n f i g u r e d  ( )
 *
 * Called when all NVRAM parsing is completed.
 * The ATMizer NIM may only be reset and configured from
 * this time on.
 */
void
atmzr_system_configured (hwidbtype *idb)	/* Interface HW IDB ptr */
{
    /*
     * Finish configuring interface
     */
    idb->buffersize = idb->atm_db->config.mtu;
    idb->max_pak_size_def = idb->atm_db->config.mtu;
    if_maxdgram_adjust(idb, idb->atm_db->config.mtu);

    /*
     * Mark this interface as configured && bring it up if not admin down
     */
    ((atmzr_instance_t *)INSTANCE)->system_configured = TRUE;
    if (idb->state != IDBS_ADMINDOWN)
	atmzr_reset (idb);
}


/*
 * a t m z r _ n i m _ t y p e _ s t r i n g  ( )
 *
 * Convert ATMizer hexadecimal NIM type to ASCII string.
 *
 * Returns: ASCII string pointer, or
 *	    NULL if unknown.
 */
char *
atmzr_nim_type_string (int type)		/* NIM hex type */
{
    switch (type)
    {
    case XX_ATM_BX50:
	return ("ATMizer BX-50");
    default:
	return (NULL);
    }
}


/*
 * a t m z r _ p l i m _ t y p e _ s t r i n g  ( )
 *
 * Convert PLIM hexadecimal type to ASCII string.
 *
 * Returns: ASCII string pointer, or
 *	    NULL if unknown.
 */
char *
atmzr_plim_type_string (ushort type)		/* NIM PLIM hex type */
{
    switch (type)
    {
    case ATMZR_PLIMR_ID_STS3CSMF:
	return ("OC-3 Single-Mode Fiber");
    case ATMZR_PLIMR_ID_STS3CMMF:
	return ("OC-3 Multi-Mode Fiber");
    case ATMZR_PLIMR_ID_DS1:
	return ("DS1");
    case ATMZR_PLIMR_ID_DS3:
	return ("DS3");
    case ATMZR_PLIMR_ID_E1:
	return ("E1");
    case ATMZR_PLIMR_ID_E3:
	return ("E3");
    default:
	return (NULL);
    }
}


/*
 * a t m z r _ p l i m _ l o o p b a c k _ t y p e _ s t r i n g  ( )
 *
 * Returns: ASCII string pointer, or
 *	    NULL if unknown.
 */
char *
atmzr_plim_loopback_type_string (ushort type)	/* NIM hex type */
{
    switch (type)
    {
    case ATM_LOOP_LINE:
	return ("External Line");
    case ATM_LOOP_PAYL:
	return ("External Payload");
    case ATM_LOOP_CELL:
	return ("External Cell");
    case ATM_LOOP_DIAG:
	return ("Internal Diagnostic");
    default:
	return (NULL);
    }
}


/*
 * a t m z r _ s h u t d o w n  ( )
 *
 * Change the interface state to the new state, and if it is
 * being administratively shut down, reset it and leave it like
 * that until it is unshut again.
 */
void
atmzr_shutdown (hwidbtype *idb,			/* Interface HW IDB ptr */
		int newstate)			/* New state Up/Down */
{
    atmzr_instance_t		*ds;		/* ATMizer dev struc */
    int				indx;		/* Auxiliar index */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Reset NIM and update interface state
     */
    if (newstate == IDBS_ADMINDOWN) {
	/* Reset NIM  & mark interface as non-operational */
	((atmzr_instance_t *)INSTANCE)->nim_regs->ctlr = 0;
	ds->nim_operational = FALSE;
	idb->atm_db->flags &= ~ATM_FLAG_UP;

	/* Stop transmission timer */
	TIMER_STOP (idb->xmittimer);

	/* Free interface allocated buffers */
	for (indx=0; indx<PKTDESCRTSZ; indx++) {
	    if (ds->id2pakT[indx].pak_ptr) {
		datagram_done (ds->id2pakT[indx].pak_ptr);
		ds->id2pakT[indx].pak_ptr = NULL;
	    }
	}

	/* Inform upper layers that some cleanup may be necessary */
        reg_invoke_media_hw_reset (idb);
    }
    net_cstate(idb, newstate);
}


/*
 * a t m z r _ l i n e _ a c t i o n  ( )
 *
 * Process a line state change.
 */
void
atmzr_line_action (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    DEBUG_EVENTS ("\natmzr_line_action(%s): state=%d",
		  idb->hw_namestring, idb->atm_lineup);

    /*
     * Update line state
     */
    idb->atm_lineup = (idb->state == IDBS_UP) ? TRUE : FALSE;
}


/*
 * a t m z r _ l i n e _ s t a t e  ( )
 *
 * Report line state state.
 */
boolean
atmzr_line_state (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    return (idb->atm_lineup);
}


/*
 * a t m z r _ e n a b l e  ( )
 *
 * Enable ATMizer after it has  been throttled via receive
 * buffers starvation. The only thing required to do is to
 * refill the free buffer queues.
 */
void
atmzr_enable (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;		/* ATMizer dabase pointer */
    leveltype			level;		/* Interrupt level */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Mark interface as enabled
     */
    idb_mark_unthrottled(idb);

    /*
     * Refill free buffer queues (mainly targeted at
     * the starved free receive buffer queues)
     */
    level = raise_interrupt_level (NETS_DISABLE);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[SMALL_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[MID_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[BIG_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[LARGE_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[SEMI_HUGE_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[HUGE_FREEQ]);
    reset_interrupt_level (level);
}


/*
 * a t m z r _ d i s a b l e  ( )
 *
 * Disable ATMizer receive interrupt. Used to throttle the interface.
 *
 * We don't really need to do anything here since the interface
 * will be throttled via receive buffers starvation instead of
 * disabling any interrupts or something similar.
 */
void
atmzr_disable (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    /*
     * Make sure interface is not throttled yet
     */
    if (idb_is_throttled(idb))
	return;

    /*
     * Mark interface as disabled
     */
    idb_mark_throttled (idb);
}


/*
 * a t m z r _ p e r i o d i c  ( )
 *
 * Perform periodic ATMizer checks.
 */
void
atmzr_periodic (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atm_db_t *atm = idb->atm_db;
    atmzr_instance_t *ds = (atmzr_instance_t *)INSTANCE;

    if (idb->state != IDBS_ADMINDOWN) {
        switch (atm->config_status) {
        case ATM_CONFIG_CHANGED:
            if (CLOCK_OUTSIDE_INTERVAL(atm->config_time,
                ATMZR_CONFIG_DELAY*ONESEC)) {
                atmzr_reset (idb);
            }
            break;
	default:
            atm->config_status = ATM_CONFIG_OK;
            break;
        }
	if (ds->adjust_pools)
	    atmzr_adjust_pools (idb);

	if (ds->hi_irq_reset)
	    atmzr_reset(idb);
    }
}


/*
 * a t m z r _ s h o w _ m e m o r y  ( )
 *
 * Dump a block of ATMizer private memory on the screen.
 */
static void
atmzr_show_memory (hwidbtype *idb,		/* Interface HW IDB ptr */
		   ulong addr,			/* ATmizer's virtual addr */
		   ulong length)		/* Number of bytes to dump */
{
    ushort			indx, jndx;	/* Auxiliar indexes */
    uchar			buf[1024];	/* Dump buffer */

    addr &= ~0x3; /* force alignment */
    length = atmzr_dump_prsram (idb, buf, addr, length);
    indx = 0;
    while (length) {
	printf ("\n0x%08X:  ", addr);
	addr += 16;
	for (jndx=0; jndx<16 && length; jndx++) {
	    printf ("%02X ", buf[indx++]);
	    length--;
	}
    }
}


/*
 * a t m z r _ s h o w _ c o n t r o l l e r  ( )
 *
 * Show ATMizer detailed status.
 *
 * Returns: FALSE
 */
boolean
atmzr_show_controller (hwidbtype *idb,		/* Interface HW IDB ptr */
		       parseinfo *csb)		/* Parser string */
{
    char			*string;	/* ASCII string auxiliar ptr */
    atmzr_instance_t            *ds;            /* ATMizer database pointer */
    atmzr_regs_t		*rp;            /* ATM NIM registers pointer */
    ushort			indx;		/* Auxiliar index */
    ulong			addr;		/* Memory dump start addr */
    ulong			length;		/* Memory dump length */

    /*
     * Check first if a NIM private memory dump was requested
     */
    addr = GETOBJ (int, 4);
    length = GETOBJ (int, 5);
    if (length) {
	atmzr_show_memory (idb, addr, length);
	return (FALSE);
    }

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = ds->nim_regs;

    /*
     * Print NIM hardware information (slot, NIM type, PLIM type, ...)
     */
    printf ("\nATM Unit %d, ", idb->unit);
    if ((string = atmzr_nim_type_string (idb->nim_type)) == NULL)
	printf ("Slot %d, Type %d, Hardware Version %d",
	    idb->slot, idb->nim_type, idb->nim_version);
    else
	printf ("Slot %d, Type %s, Hardware Version %d",
	    idb->slot, string, idb->nim_version);
    printf ("\n  %s, %s", atm_xilinx_version, atmizer_version);
    printf ("\n  Public SRAM %d bytes, Private SRAM %d bytes, ",
	ds->nim_pusram_size, ds->nim_prsram_size);
    printf ("I/O Base Addr 0x%08X", ds->nim_regs);
    if ((string = atmzr_plim_type_string (ds->plim_type)) == NULL)
	printf ("\n  PLIM Type %d, Version %d,",
	    ds->plim_type, ds->plim_version);
    else
	printf ("\n  PLIM Type %s, Version %d",
	    atmzr_plim_type_string (ds->plim_type), ds->plim_version);
    if ((idb->atm_db->plimtype == ATM_PLIM_E3) ||
	(idb->atm_db->plimtype == ATM_PLIM_DS3))
	printf (", Framing %s Version 1.0", atm_framing_string (idb));
    printf ("\n  %s Transmit Clock",
	(idb->atm_db->txclock == ATM_CLOCK_LINE) ? "Network" : "Internal");
    if (idb->inloopback)
	printf (", %s Loopback",
	    atmzr_plim_loopback_type_string(idb->ext_loop_type));
    if (ds->nim_operational)
	printf ("\n  NIM IS Operational");
    else
	printf ("\n  NIM is NOT Operational");
    switch (idb->atm_db->config_status) {
    case ATM_CONFIG_OK:		printf (", Configuration OK"); break;
    case ATM_CONFIG_CHANGED:	printf (", Configuration Changed"); break;
    case ATM_CONFIG_ENABLE:	printf (", Configuration Enable"); break;
    case ATM_CONFIG_IN_PROGRESS: printf (", Configuration in Progress"); break;
    case ATM_VCS_NOT_UP:        printf (", Configuration VCS not up"); break;
    case ATM_VCS_UP:            printf(", Configuration VCS up"); break;
    }
    if (idb_is_throttled(idb))
	printf (", Receive IS Throttled");
    printf ("\n  DMA Read %d, DMA Write %d",
	(rp->ctlr & ATMZR_CTLR_RDBURST12) ? 12 : 4,
	(rp->ctlr & ATMZR_CTLR_WRBURST12) ? 12 : 4);

    /*
     * Print NIM hardware registers
     */
    printf ("\n\nNIM Hardware Registers");
    printf ("\n  pc_id 0x%04X, pc_rev 0x%04X, mem_config 0x%04X, plim_id/rev 0x%04X",
	rp->pcidr, rp->pcrevr, rp->memcfgr, rp->plimr);
    printf ("\n  control 0x%04X, err_stat 0x%04X, lirq_vec 0x%04X",
	rp->ctlr, rp->erstr, rp->lirqvecr);

    /*
     * Print PLIM hardware registers
     */
    printf ("\n\nPLIM Hardware Registers");
    (*ds->plim_show_regs)(idb);

    /*
     * Printf interface control data structures
     */
    printf ("\n\nSoftware data structures");
    printf ("\n  HwIdbAddr   DevCtlAddr   id2pakT");
    printf ("\n  0x%08X  0x%08X  0x%08X", idb, idb->devctl, ds->id2pakT);

    /*
     * Print interface data structures status
     */
    /* Free buffer index queues */
    printf ("\n\nFree Buffer Index Queues");
    printf ("\n  QueueName        BaseAddr   WrPointer  Level   ");
    printf ("High Cach/Buff/PktdMiss");
    for (indx=0; indx<NUM_FREEQ; indx++)
	PRTFRQSTAT (ds->rxfreeQ[indx]);

    /* Read index queues */
    printf ("\n\nRead Index Queues");
    printf ("\n  QueueName        BaseAddr   RdPointer  Size");
    PRTRDQSTAT(ds->rxrdyQ);
    PRTRDQSTAT(ds->txdoneQ);
    PRTRDQSTAT(ds->eventQ);

    /* Write index queues */
    printf ("\n\nWrite Index Queues");
    printf ("\n  QueueName        BaseAddr   WrPointer  Level");
    PRTWRQSTAT(ds->txrdyQ);

    /* Event queue */
    printf ("\n\nEvent Queue");
    printf ("\n                   BaseAddr   RdPointer  Size LastEvent");
    PRTEVQSTAT(ds);

    /* Command/response queue */
    printf ("\n\nCommand/Response Queue");
    printf ("\n                   BaseAddr   Pointer    Level   ");
    printf ("WrIndx RdIndx Pend LastRsp");
    PRTCRQSTAT(ds->cmdrspQ, ds);

    /* Packet descriptor table */
    printf ("\n\nPacket Descriptor Table");
    printf ("\n                   BaseAddr   Total  Receive Transmit");
    PRTPKTSTAT(ds);

    /*
     * Print global statistics
     */
    printf ("\n\nPacket switching");
    printf ("\n  Fastswitched\t%d\n  To-process\t%d\n  Bridged\t%d",
	ds->fast_switched, ds->to_process, ds->bridged);
    printf ("\n\nTransmit errors");
    printf ("\n  Restarts\t%d\n  Pktid misses\t%d\n  Bad pktid\t%d",
	ds->tx_restarts, ds->tx_pktidmiss, ds->tx_badid);
    printf ("\n  Wrong queue\t%d\n  No pkt\t%d\n  Tx errors\t%d\n  Bad VC\t%d",
	ds->tx_wrongq, ds->tx_nopkt, ds->tx_errors, ds->tx_badvc);
    printf ("\n\nReceive errors");
    printf ("\n  Bad pktid\t%d\n  Wrong queue\t%d\n  No pkt\t%d",
	ds->rx_badid, ds->rx_wrongq, ds->rx_nopkt);
    printf ("\n  CRC\t\t%d\n  Length\t%d\n  Giant\t\t%d\n  Reas tout\t%d",
	ds->rx_errors[RXPD_STAT(RXSTAT_CRC)],
	ds->rx_errors[RXPD_STAT(RXSTAT_LENGTH)],
	ds->rx_errors[RXPD_STAT(RXSTAT_GIANT)],
	ds->rx_errors[RXPD_STAT(RXSTAT_REASTOUT)]/*,
	ds->rx_errors[RXPD_STAT(RXSTAT_AAL34_MID)],
	ds->rx_errors[RXPD_STAT(RXSTAT_AAL34_BUFOVFL)],
	ds->rx_errors[RXPD_STAT(RXSTAT_AAL34_PEERABORT)],
	ds->rx_errors[RXPD_STAT(RXSTAT_AAL34_SEQNUM)]*/);
    printf ("\n  AAL5 format\t%d",
	ds->rx_errors[RXPD_STAT(RXSTAT_AAL5_FORMAT)]);

    return (FALSE);
}


/*
 * a t m z r _ c m d B L O C K  ( )
 *
 * Check to see if the response of an asynchronous command is
 * ready or if the monitoring timer expired.
 *
 * Returns:
 *	TRUE if process should remain blocked,
 *	FALSE if process should be scheduled to run.
 */
static boolean
atmzr_cmdBLOCK (rspmon_t *mon_ptr)		/* Rsp monitor struc pointer */
{
    if (mon_ptr->rsp.rsp)
	return (FALSE);
    if (AWAKE (mon_ptr->timer))
	return (FALSE);
    return (TRUE);
}


/*
 * atmzr_verify_qos_params()
 *  Called to allow the platform dependent code
 * to verify the qos parameters
 */
uint
atmzr_verify_qos_params(hwidbtype *hwidb,  /* atm interface hw type */
			unsigned peakrate, /* proposed peak cell rate */
			unsigned avgrate,  /* proposed sustainable cell rate*/
			unsigned burstcells) /* proposed burst cell size */
{
    atm_db_t *atm;    /* ATM database pointer */
    uint result;      /* value contains result of parameter tests */
    uint rate;        /* peakrate variable */
    uint i;           /* array index */
    
    /* should never happen */
    if (hwidb == NULL)
	return(FALSE);

    /*again should not happen */
    atm = hwidb->atm_db;
    if (atm == NULL)
	return(FALSE);
    
    /* initialize to pass */
    result = ATM_TRAFFIC_SHAPING_VERIFIED;
    
    if (peakrate == 0) {
	rate = atm->plimrate*ATM_MBPS2KBPS;
    } else {
	rate = peakrate;
    }
    
    /* first check if there is an available rate queue
     * or one we can already use
     */
    for (i = 0;i< ATMZR_RATEQ_NO;i++) {
	if (atm->rateq[i].state == RATE_QUEUE_DISABLED ||
	    atm->rateq[i].rate == rate)
	    break;
    }

    if (i == ATMZR_RATEQ_NO)
	result |= ATM_OUT_OF_RATE_QUEUES;
    
    /* Next check the burst cell size
     * Note: a burstcell value of zero indicates to use default
     */
    if ((burstcells != 0) && ((burstcells < ATMZR_MIN_BURST_SIZE)
			      || (burstcells > ATMZR_MAX_BURST_SIZE))) {
	if (atm_errors_debug) {
	    buginf ("Burst cell size value: %d is out of range: must be "
		    "between %d and %d",burstcells,
		    ATMZR_MIN_BURST_SIZE,
		    ATMZR_MAX_BURST_SIZE);
	}
	result |= ATM_BURST_ERROR;
    }

    /* now burst if ok so return if using peak,average defaults*/
    if (peakrate == 0 && avgrate == 0 &&
	(result == ATM_TRAFFIC_SHAPING_VERIFIED ||
	 result == ATM_OUT_OF_RATE_QUEUES))
	return(result);
    
    /* check peakrate for platform */
    if ((peakrate > (atm->plimrate) * ATM_MBPS2KBPS)
	|| (peakrate < ATMZR_MIN_PEAK_RATE)) {
	if (atm_errors_debug) {
	    buginf ("Peak rate value: %d is out of range: must be "
		    "between %d and %d",peakrate,
		    ATMZR_MIN_PEAK_RATE,
		    atm->plimrate * ATM_MBPS2KBPS);
	}
	result |= ATM_PEAKRATE_ERROR;
    }

    /* check average rate for platform*/
    if ((avgrate > peakrate) || (avgrate < ATMZR_MIN_AVG_RATE)) {
	if (atm_errors_debug) {
	    buginf ("Average rate value: %d is out of range: must be "
		    "between %d and %d",avgrate,
		    ATMZR_MIN_AVG_RATE,
		    atm->plimrate * ATM_MBPS2KBPS);
	}
	result |= ATM_AVGRATE_ERROR;
    }

    /* values verify - return good */
    return(result);

}

/*
 * a t m z r _ i s s u e _ c o m m a n d  ( )
 *
 * Issue an ATMizer command (synchronous or asynchronous).
 * If this function is called from interrupt-level, the
 * SYNCHRONOUS command option will be used, and this function
 * will busy-wait until the response is available or a monitoring
 * timer expires.
 * If this function is called from process-level, the
 * ASYNCHRNOUS command option will be used, and this function
 * will put the process to sleep until either the response is
 * ready of a monitoring timer expires.
 *
 * Returns: Response status code
 */
static ushort
atmzr_issue_command (hwidbtype *idb,		/* Interface HW IDB ptr */
		     ushort cmd,		/* Command code */
		     ushort param,		/* Command parameter value */
		     uchar *buf,		/* Cmd/Rsp data buffer */
		     ushort cmd_dsize,		/* Data size (cmd) */
		     ushort rsp_dsize)		/* Data size (rsp) */
{
    atmzr_instance_t            *ds;            /* Control struc ptr */
    rspmon_t			*mon_ptr;	/* Rsp monitor struc entry */
    cmdrsp_t			*mb_ptr;	/* PuSRAM cmd/rsp mailbox ptr */
    vshort			*data_ptr;	/* Auxiliar copy data ptr */
    uchar                       *buf_ptr;       /* Auxiliar copy buf ptr */
    uchar			*lim_ptr;	/* Auxiliar copy limit ptr */
    ulong			wait;		/* Delay loop counter */

    /*
     * Initialize auxiliar variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Make sure the NIM is operational
     */
    if (!ds->nim_operational) {
	DEBUG_ERRORS ("\natmzr_issue_command(%s): cmd=%d, nim not operational",
	    idb->hw_namestring, cmd, cmd_dsize);
	return (RSP_STAT_NOTRDY);
    }

    /*
     * Check for irregular conditions
     */
    if (cmd_dsize > CMDRSPDATASZ) {		/* Data too big */
	DEBUG_ERRORS ("\natmzr_issue_command(%s): cmd=%d, data too big %d",
	    idb->hw_namestring, cmd, cmd_dsize);
	return (RSP_STAT_TOOBIG);
    }
    if (!(mon_ptr = NEXTCMD (ds))) {		/* Cmd/Rsp table full */
	DEBUG_ERRORS ("\natmzr_issue_command(%s): cmd=%d, cmd/rsp table full",
	    idb->hw_namestring, cmd);
	return (RSP_STAT_FULL);
    }
    DEBUG_EVENTS ("\natmzr_issue_command(%s): cmd=%d", idb->hw_namestring, cmd);

    /*
     * Write data area first
     */
    mb_ptr = mon_ptr->mb_ptr;
    data_ptr = (vshort *)mb_ptr->data;
    buf_ptr = buf;
    lim_ptr = buf + cmd_dsize;
    while (buf_ptr < lim_ptr)
        *(((ushort *)data_ptr)++) = *(((ushort *)buf_ptr)++);

    /*
     * Clear response field (not zero when response ready)
     */
    mb_ptr->rsp.rsp = 0;

    /*
     * Update command/response queue control variables
     */
    ds->cmdrspQ.wr_ndx = (++ds->cmdrspQ.wr_ndx) % CMDRSPQSZ;
    ds->cmdrspQ.level++;

    /*
     * Check interrupt level to determine if response
     * should be synchronous or asynchronous.
     */
    if (1) {
    /* ## if (get_interrupt_level ()) { */
        /*
         * We are running at interrupt level -
         * select synchronous response.
         */
	/* Load command & generate interrupt */
	mb_ptr->cmd.param = param;
	mb_ptr->cmd.cmd = cmd | CMD_SYNC;
	ds->intr->cmd_rdy = 0;

	/* Update flag and counters for async commands */
	mon_ptr->async = FALSE;

	/* Busy-wait until response is available or time-out */
	wait = ATMZR_RSPTOUT * ds->delay_1ms;
	while (!(*(ushort *)&mon_ptr->rsp = GETSHORT(&mb_ptr->rsp)) &&
	       (wait--));

        /* Update buffer data with return data */
        if (mon_ptr->rsp.rsp) {
            data_ptr = (vshort *)mb_ptr->data;
            buf_ptr = buf;
            lim_ptr = buf + rsp_dsize;
            while (buf_ptr < lim_ptr)
                *(((ushort *)buf_ptr)++) = *(((ushort *)data_ptr)++);
        }
 
        /* Clear cmd field, update counters and return response code */
        mb_ptr->cmd.cmd = 0;
	if (!ds->cmdrspQ.async_cnt) {
	    ds->cmdrspQ.rd_ndx = (++ds->cmdrspQ.rd_ndx) % CMDRSPQSZ;
	    ds->cmdrspQ.level--;
	}
	if (mon_ptr->rsp.rsp) {
	    if (mon_ptr->rsp.stat) {
		DEBUG_ERRORS ("\natmzr_issue_command(%s): rsp=%d, error=%d",
                    idb->hw_namestring, mon_ptr->rsp.rsp, mon_ptr->rsp.stat);
	    } else {
		DEBUG_EVENTS ("\natmzr_issue_command(%s): rsp=%d",
		    idb->hw_namestring, mon_ptr->rsp.rsp);
	    }
	    return (mon_ptr->rsp.stat);
	} else {
	    DEBUG_ERRORS ("\natmzr_issue_command(%s): cmd=%d, time-out",
		idb->hw_namestring, cmd);
	    return (RSP_STAT_TOUT);
	}
    } else {
        /*
         * We are running at process level -
         * select asynchronous response.
         */
	/* Load command & generate interrupt */
	mb_ptr->cmd.param = param;
	mb_ptr->cmd.cmd = cmd | CMD_ASYNC;
	ds->intr->cmd_rdy = 0;

	/* Update flag and counters for async commands */
	mon_ptr->async = TRUE;
	mon_ptr->data_buf = buf;
	mon_ptr->rsp_dsize = rsp_dsize;
	ds->cmdrspQ.async_cnt++;

	/* Suspend process */
	TIMER_START (mon_ptr->timer, ATMZR_RSPTOUT);
        edisms ((blockproc *)atmzr_cmdBLOCK, (ulong)mon_ptr);

	/* Return response code */
	if (mon_ptr->rsp.rsp) {
	    return (mon_ptr->rsp.stat);
	} else {
	    DEBUG_ERRORS ("\natmzr_issue_command(%s): cmd=%d, time-out",
		idb->hw_namestring, cmd);
	    return (RSP_STAT_TOUT);
	}
    }
}

/*
 * a t m z r _ send_rateq_config ( )
 *
 * Called to request atmzr dirver set up a rateq.
 *
 * Returns the status of the execution of the request.
 */
static atm_setup_status_t
atmzr_send_rateq_config (hwidbtype *idb, ushort rateq_no)
{
    atm_db_t *atmzr = idb->atm_db;
    atmzr_instance_t *ds;
    
    uint			pru_speed;	/* PRU freq (kHz) */
    rqdata_t			rqdata;		/* Rate Queue command data */
    uint			speed;

    if (!(atmzr->flags & ATM_FLAG_UP)) {
	DEBUG_EVENTS ("\natmzr_send_rateq_config(%s): rateq %d setup delayed: ATMZR disabled",
            idb->hw_namestring, rateq_no);
	return(ATM_SETUP_DELAYED);
    }

    if (atmzr->config_status == ATM_CONFIG_CHANGED) {
	DEBUG_EVENTS ("\natmzr_send_rateq_config(%s): rateq %d setup delayed: ATMZR disabled",
            idb->hw_namestring, rateq_no);
	return(ATM_SETUP_DELAYED);
    }
    
    ds = (atmzr_instance_t *)INSTANCE; /* ATMizer database pointer */
    speed = atmzr->rateq[rateq_no].rate;
    if (speed == 0) {			/* disable a rateq */
	ds->last_rsp_stat = (atmzr_issue_command (idb, CMD_RATEQ_DESTROY,
						 rateq_no, NULL, 0, 0)
			     != RSP_STAT_OK);
    } else {
	/*
	 * Issue ATMizer create rate queue command
	 */
	rqdata.rqid = rateq_no;
	if (speed < ((ds->nim_proc_clock * CELL_BITS) / ATMZR_PRUMAX)) {
	    rqdata.rqclk = ATMZR_PRUCLK_EXT;
	    pru_speed = ds->nim_pru_clock * CELL_BITS;
	} else {
	    rqdata.rqclk = ATMZR_PRUCLK_INT;
	    pru_speed = ds->nim_proc_clock * CELL_BITS;
	}
	rqdata.prucnt = pru_speed / speed;
	rqdata.prucnt = rqdata.prucnt ? rqdata.prucnt : 1;
	ds->last_rsp_stat = (atmzr_issue_command (idb, CMD_RATEQ_CREATE,
						  0, (uchar *)&rqdata,
						  sizeof (rqdata_t), 0)
			     != RSP_STAT_OK);
    }
    if (ds->last_rsp_stat)
	return (ATM_SETUP_FAILED);
    else
    	return (ATM_SETUP_SUCCESSFUL);
}


/*
 * a t m z r _ d u m p _ p r s r a m  ( )
 *
 * Dump a block of memory from the ATMizer's private SRAM.
 *
 * Returns the number of bytes actually read.
 */
ulong
atmzr_dump_prsram (hwidbtype *idb,		/* Interface HW IDB ptr */
		   uchar *buf,			/* Buffer ptr */
		   ulong offset,		/* Private SRAM offset */
		   ulong size)			/* Memory block size (bytes) */
{
    atmzr_instance_t		*ds;
    prmemdata_t			prmemdata;
    ulong			block_size, rcount;

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rcount = 0;

    /*
     * Ensure dumps of multiple of 32-bits only
     */
    if (size & 0x3) {
	size += 4;
	size &= ~0x3;
    }

    /*
     * Dump ATMizer private memory in blocks of 56 bytes
     */
    while (size) {
	block_size = (size <= 56) ? size : 56;
	prmemdata.start = (ulong *)offset;
	prmemdata.end = (ulong *)(offset + block_size);
	if ((ds->last_rsp_stat =
	     atmzr_issue_command (idb, CMD_PRMEM_RDBLK, 0, (uchar *)&prmemdata,
				  sizeof(prmemdata_t), sizeof(prmemdata_t)))) {
            break;
	} else {
	    bcopy ((uchar *)&prmemdata.data, buf, block_size);
	    size -= block_size;
	    offset += block_size;
	    rcount += block_size;
	    buf += block_size;
	}
    }

    return (rcount);
}


/*
 * a t m z r _ d o w n l o a d _ x i l i n x  ( )
 *
 * Download ATMizer NIM Xilinx.
 */
boolean
atmzr_download_xilinx (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_regs_t		*rp;		/* NIM registers pointer */
    uchar			*xilinx_data_ptr;/* Download auxiliar pointer */
    uchar			*xilinx_end_ptr;/* Xilinx array end ptr */

    /*
     * Initialize local variables
     */
    rp = ((atmzr_instance_t *)INSTANCE)->nim_regs;
    xilinx_data_ptr = (char *)atm_xilinx;
    xilinx_end_ptr = (char *)atm_endxilinx;

    /*
     * Verify that hardware is ready to be downloaded
     */
    if (!(rp->erstr & ATMZR_ERSTR_XILINXDWNL))
        return (FALSE);

    /*
     * Download xilinx image
     */
    while (xilinx_data_ptr < xilinx_end_ptr)
        rp->pcidr = (ushort)(*xilinx_data_ptr++);

    /*
     * Verify that download succeeded
     */
    return ((rp->erstr & ATMZR_ERSTR_XILINXRDY) ? TRUE : FALSE);
}


/*
 * a t m z r _ d o w n l o a d _ f i r m w a r e  ( )
 *
 * Download ATMizer NIM Firmware.
 */
static void
atmzr_download_firmware (hwidbtype *idb,	/* Interface HW IDB ptr */
			 ushort *data,		/* Firmware array ptr */
		         ushort size)		/* Firmware size (in shorts) */
{
    atmzr_instance_t		*ds;		/* ATMizer database pointer */
    atmzr_regs_t                *rp;            /* Interface registers ptr */
    ushort			*pusram;	/* Public SRAM pointer */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = ds->nim_regs;
    pusram = (ushort *)&(((atmzr_instance_t *)INSTANCE)->nim_regs->pusram);

    /*
     * Download firmware
     */
    while (size--)
        *pusram++ = *data++;
 
    /*
     * Write Public & Private SRAM sizes
     */
    *pusram++ = HIHW (ds->nim_pusram_size);	/* Public SRAM size High */
    *pusram++ = LOHW (ds->nim_pusram_size);	/* Public SRAM size Low */
    *pusram++ = HIHW (ds->nim_prsram_size);	/* Private SRAM size High */
    *pusram++ = LOHW (ds->nim_prsram_size);	/* Private SRAM size Low */
}


/*
 * a t m z r _ r e s e t  ( )
 *
 * (Re)Initialize the ATMizer card if the system was already
 * configured, which involves the following:
 *     1) make sure that the system configuration is complete
 *     2) download xilinx
 *     3) download firmware
 *     4) release NIM reset
 */
void
atmzr_reset (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atmzr_regs_t                *rp;            /* Interface registers ptr */
    atmzr_instance_t		*ds;		/* NIM control data struc ptr */
    ushort			dummy;		/* Dummy auxiliar variable */
    idbtype			*swidb;
    ushort 			indx;		/* Auxiliar table index */  

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = ds->nim_regs;

    /*
     * Inform upper layers that some cleanup may be necessary
     */
    reg_invoke_media_hw_reset (idb);

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        reg_invoke_media_interesting_addresses(swidb);
    }

    /*
     * Reset NIM/PLIM
     */
    rp->ctlr = 0;

    /*
     * Make sure interface is not admin down
     */
    if (idb->state == IDBS_ADMINDOWN)
	return;
    else
	net_cstate(idb, IDBS_DOWN);

    /*
     * Make sure system is configured
     */
    if (!(ds->system_configured))
	return;

    /*
     * Release PLIM so that it is ready by the time we want
     * to access its registers. Also clear possible interrupts.
     */
    rp->ctlr = ATMZR_CTLR_RSTPLIMN;
    dummy = ds->intr->rsp_rdy;
    dummy = ds->intr->event;
    rp->erstr = 0;

    /* Free interface allocated buffers */
    for (indx=0; indx<PKTDESCRTSZ; indx++)
        if (ds->id2pakT[indx].pak_ptr) {
            datagram_done(ds->id2pakT[indx].pak_ptr);
            ds->id2pakT[indx].pak_ptr = NULL;
        }

    /*
     * Reinitialize software control data structures
     */
    idb->buffersize = idb->atm_db->config.mtu;
    idb->max_pak_size_def = idb->atm_db->config.mtu;
    if_maxdgram_adjust(idb, idb->atm_db->config.mtu);
    atmzr_init_sw_ds (idb);

    /*
     * Download Firmware
     */
    atmzr_download_firmware (idb,
			     (ushort *)atmizer_firmware,
			     (ushort)atmizer_firmware_words);

    /*
     * Release NIM reset so that it starts running;
     * use long write bursts only for SONET (STS-3c) interface
     */
    rp->ctlr = ATMZR_CTLR_RSTNIMN | ATMZR_CTLR_RSTPLIMN | ATMZR_CTLR_PRUCLK_2U |
	       ATMZR_CTLR_CLRLINT | ATMZR_CTLR_ENHIRQ1 | ATMZR_CTLR_ENHIRQ2 |
	       ATMZR_CTLR_ENHIRQ4;
    if ((ds->plim_type == ATMZR_PLIMR_ID_STS3CMMF) ||
	(ds->plim_type == ATMZR_PLIMR_ID_STS3CSMF))
	rp->ctlr |= (ATMZR_CTLR_WRBURST12 | ATMZR_CTLR_RDBURST12);

    /*
     * Initialize PLIM
     */
    ((atmzr_instance_t *)INSTANCE)->plim_reset (idb);

    idb->counters.resets++;
}


/*
 * a t m z r _ s e t u p  ( )
 *
 * Set the default configuration for a newly discovered ATMizer card.
 */
void
atmzr_setup (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atm_db_t			*atm;		/* Configuration database ptr */
    idbtype			*swidb;		/* Interface software IDB ptr */
    atmzr_instance_t		*ds;		/* NIM control data struc ptr */

    /*
     * Initialize local auxiliar variables
     */
    atm = idb->atm_db;
    swidb = idb->firstsw;
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Set output hold queue size
     */
    holdq_init (idb, ATMZR_HOLDQ);

    /*
     * Set options
     */
    atm->flags |= ATM_FLAG_PVC;

    /*
     * If system not configured yet, initialize ATM
     * database structure to default values.
     */
    if (!(((atmzr_instance_t *)INSTANCE)->system_configured)) {
        atm->config.mode = ATM_CFG_MODE_AAL5 | ATM_CFG_MODE_AAL34;
	atm->config.max_intf_vcs = MAX_ATMZR_VCIDS;
        atm->config.rx_vc = atm->config.max_intf_vcs;	/* Same as TX VCs */
	atm->config.mid_per_vc = ATM_MID_MAX;	/* We don't limit MIDs */
        atm->config.mtu = idb->maxmtu + ETHER_ARPA_ENCAPBYTES + ATM_SMDS_HDRBYTES;
	atm->config.rt_vc_per_vp = atm->def_config.rt_vc_per_vp = ATM_VC_PER_VP_DEFAULT;

	/* max_vci_bits is dependent on the vc per vp */
	atm->def_config.max_vci_bits = atm->config.max_vci_bits
	    = atm_calc_vci_bits(ATM_VC_PER_VP_DEFAULT);

	/* max_vpi_bits is dependent on total vp/vc bits and max_vci_bits */
	atm->def_config.max_vpi_bits = atm->config.max_vpi_bits
	    = atm_calc_vpi_bits(ATMZR_TOTAL_VPIVCI_BITS,
				atm->config.max_vci_bits);

	atm->def_config.max_burst = ATMZR_MAX_BURST_SIZE;
	atm->def_config.min_peak_rate = ATMZR_MIN_PEAK_RATE;
	
	atm->mtu_reject_call = FALSE;
	/* platform dependent value to use to display burst cells from
	   the configured value */
	atm->burstcells_factor = ATMZR_MIN_BURST_SIZE;
    }

    /*
     * Initialize hardware attributes
     */
    switch (ds->plim_type) {
    case ATMZR_PLIMR_ID_STS3CSMF:
	atm->plimtype = ATM_PLIM_SONET;
	atm->plimrate = PLIM_SONET_RATE;
	atm->medium = ATM_MEDIA_SINGLE_FIBER;
	break;
    case ATMZR_PLIMR_ID_STS3CMMF:
	atm->plimtype = ATM_PLIM_SONET;
	atm->plimrate = PLIM_SONET_RATE;
	atm->medium = ATM_MEDIA_MULTI_FIBER;
	break;
    case ATMZR_PLIMR_ID_DS1:
	atm->plimtype = ATM_PLIM_DS1;
	atm->plimrate = PLIM_DS1_RATE;
	atm->medium = ATM_MEDIA_COAX;
	break;
    case ATMZR_PLIMR_ID_DS3:
	atm->plimtype = ATM_PLIM_DS3;
	atm->plimrate = PLIM_DS3_RATE;
	atm->medium = ATM_MEDIA_COAX;
	break;
    case ATMZR_PLIMR_ID_E1:
	atm->plimtype = ATM_PLIM_E1;
	atm->plimrate = PLIM_E1_RATE;
	atm->medium = ATM_MEDIA_COAX;
	break;
    case ATMZR_PLIMR_ID_E3:
	atm->plimtype = ATM_PLIM_E3;
	atm->plimrate = PLIM_E3_RATE;
	atm->medium = ATM_MEDIA_COAX;
	break;
    case ATMZR_PLIMR_ID_NONE:
    default:
	atm->plimtype = ATM_PLIM_INVALID;
	atm->medium = ATM_MEDIA_UNKNOWN;
    }
    atm->ratequeue_no = ATMZR_RATEQ_NO;
    atm->config_delay = ATMZR_CONFIG_DELAY;
    atm->config.rx_buff = ds->pktid_rxlimit;
    atm->config.tx_buff = ds->pktid_txlimit;

    /*
     * Initialize interface function pointers
     */
    atm->funcs.reset = atmzr_reset;
    atm->funcs.rawinput = atmzr_raw_input;
    atm->funcs.setupvc = atmzr_setup_vc;
    atm->funcs.config_rateq = atmzr_send_rateq_config;
    atm->funcs.teardownvc = atmzr_teardown_vc;
    atm->funcs.encapsulate = atmzr_encaps;
    atm->funcs.periodic = atmzr_periodic;
    atm->funcs.getvc_stats = atmzr_get_vc_stats;
    atm->funcs.sniff = atmzr_sniff;
    atm->funcs.qos_params_verify = atmzr_verify_qos_params;
    atm->ds3_framing =
	atm->ds3_def_framing =
	    ATMZR_DS3_FRAMING_DEFAULT;
    atm->e3_framing =
	atm->e3_def_framing =
	    ATMZR_E3_FRAMING_DEFAULT;

    /*
     * Set IDB attributes
     */
    set_default_interface_delay(swidb, ATMZR_DELAY);

    /*
     * Set indication that configuration changed
     * so that NIM gets the update.
     */ 
    atm->config_status = ATM_CONFIG_CHANGED;
    GET_TIMESTAMP (atm->config_time);
}


/*
 * a t m z r _ e n c a p s  ( )
 *
 * Add any platform-specific header to the frame. The ATMizer
 * NIM will receive all its transmission parameters via the
 * NIM public SRAM packet descriptors.
 *
 * Returns: platform-specific encapsulation length (NONE for the ATMizer) 
 */
unsigned
atmzr_encaps (atm_db_t *atmzr,			/* ATM structure ptr */
	      paktype *pak,			/* Packet header ptr */
	      vc_info_t *vc,			/* Associated VC struct ptr */
	      ushort dm)			/* *AIP* descriptor mode */
{
    ushort			vcid;		/* VCD number */
    ushort			enctype;	/* Encapsulation type */

    /*
     * Adjust packet pointer to include ATM encapsulation (VcId/Encaps)
     */
    pak->datagramstart -= ATM_ENCAP_HDRBYTES;
    pak->datagramsize += ATM_ENCAP_HDRBYTES;

    /*
     * Obtain VC number and encapsulation type
     * Avoid reading this value back in the atmzr_output() and
     * atmzr_fastsend() routines to avoid Shared Memory reads
     * which will impact performance.
     */
    pak->atm_vcid = vcid = vc->vc;
    dm &= ATM_DM_PKT_MSK;
    if ((dm == ATM_DM_PKT_OAM_VC) || (dm == ATM_DM_PKT_OAM_DM))
	pak->atm_enctype = enctype = dm;
    else if (ISVCAAL34(vc))
	pak->atm_enctype = enctype = ATM_DM_PKT_AAL34;
    else
	pak->atm_enctype = enctype =  ATM_DM_PKT_AAL5;

    /*
     * Slam in the VC number  and encapsulation (debugging purposes)
     */
    PUTSHORT (&((atm_encap_t *)pak->datagramstart)->vcnum, vcid);
    PUTSHORT (&((atm_encap_t *)pak->datagramstart)->enctype, enctype);

    return (ATM_ENCAP_HDRBYTES);
}


/*
 * a t m z r _ s e t _ r a t e q  ( )
 *
 * Create a new rate queue if rate queue table not full.
 *
 * Note: AIP and ATMZR in fact could share the same _set_rateq
 * 	function w/ a little addition. We should combine the two to
 * 	one when modularizing the code.
 *
 * Returns:
 *     Rate queue index if successful,
 *     ATMZR_BAD_RATEQ if unsuccessful.
 */
static ushort
atmzr_set_rateq (atm_db_t *atm,			/* ATM database pointer */
		 uint speed)			/* Speed in kbps */
{
    hwidbtype			*idb;		/* Hardware IDB pointer */
    ushort			indx;		/* Auxiliar table index */
    atm_setup_status_t		status;
    unsigned			new_trate;  /* the new total rate allocation */
    rateq_param_t		rq_param;


    idb = atm->hwidb;
	
    /*
     * look for a free rate queue
     */
    for (indx = 0; indx < ATMZR_RATEQ_NO; indx++) {
        if (atm->rateq[indx].state == RATE_QUEUE_DISABLED) {
	    /*
	     * Found an unused rate queue -
	     * Issue ATMizer create rate queue command
	     */
	    rq_param.rq_id = indx;
	    rq_param.rq_rate = speed;
	    rq_param.rq_state = RATE_QUEUE_DYNAMIC;
	    atm_update_rateq(atm, &rq_param);
	    status = atmzr_send_rateq_config(idb, indx);
	    if (status == ATM_SETUP_SUCCESSFUL ||
		status == ATM_SETUP_DELAYED) {
		new_trate = atm_rateq_ratesum(atm); /* in Kbps */
		if (new_trate > (atm->plimrate * ATM_MBPS2KBPS)) {
		    DEBUG_ERRORS("Interface %s: Total rateq allocation %uKbps "
				 "exceeded maximum plim rate of %uMbps.",
				 atm->hwidb->hw_namestring,new_trate,
				 atm->plimrate);
		}
		return(indx);
	    } else {			/* failed to setup the rateq */
		rq_param.rq_id = indx;
		rq_param.rq_rate = 0;
		rq_param.rq_state = RATE_QUEUE_DISABLED;
		atm_update_rateq(atm, &rq_param);
	    }
	}
    }
    return (ATMZR_BAD_RATEQ);
}


/*
 * a t m z r _ g e t _ r a t e q  ( )
 *
 * Find a rate queue which rate matches the given rate.
 *
 * Returns:
 *     Rate queue index if successful,
 *     ATMZR_BAD_RATEQ if unsuccessful.
 */
static ushort
atmzr_get_rateq (atm_db_t *atm,			/* ATM database pointer */
		 uint speed)			/* Speed in kbps */
{
    ushort indx;				/* Auxiliar table index */
 
    for (indx = 0; indx < ATMZR_RATEQ_NO; indx++) {
        if (atm->rateq[indx].state != RATE_QUEUE_DISABLED &&
            atm->rateq[indx].rate == speed) {
            return (indx);
        }
    }
    return (ATMZR_BAD_RATEQ);
}


/*
 * a t m z r _ g e t _ p o o l _ i n d e x  ( )
 *
 * Find the appropriate ATM interface pool to match
 * the given MTU size.
 *
 * Returns: pool index if successful, or
 *	    ATMZR_BAD_POOL if no pool could be found
 */
static ushort
atmzr_get_pool_index (hwidbtype *idb,		/* Interface HW IDB ptr */
		      ushort mtu)		/* Pool minimum MTU */
{
    atmzr_instance_t            *ds;            /* ATMizer database pointer */
    ushort			indx;		/* Auxiliar pool index */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Look for buffer pool which buffers size is big enough
     */
    for (indx=0; indx<NUM_FREEQ; indx++)
	if ((ushort)(ds->rxfreeQ[indx].bufpool->size) >= mtu)
	    return (indx);

    return (ATMZR_BAD_POOL); 
}


/*
 * a t m z r _ a d j u s t _ p o o l s  ( )
 *
 * Adjust buffer pools according to interface's VCs distribution.
 *
 * Since a constant total number of buffers is intended to be used
 * by one ATM hardware interface, whenever one pool is to have its
 * number of buffers increased, there is another that will have its
 * number decreased. Also, since the destruction of buffers can take
 * a while, the total number of buffers in the system will be increased
 * until they can be destroyed, causing excessive use of Packet memory.
 * The method that will be used to destroy the buffers is to adjust
 * concurrently the pool's cache size and also the number of permanent
 * buffers, letting the system to get rid of the excessive number of
 * buffers.
 *
 * Another aspect of pools adjusting is the estimation of total required
 * shared memory, which becomes a major issue if MTUs larger than the
 * default are used. In order to limit the amount of packet memory
 * required per hardware interface, we will have to proportionaly limit
 * the number of buffers allocated to the pools using buffers bigger
 * than the default MTU size.
 */
void
atmzr_adjust_pools (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;		/* Device control struc ptr */
    atm_db_t			*atm;		/* ATM struc pointer */
    rxfreeq_db_t		*freeq;		/* Free buf queue descr ptr */
    ushort			rq_indx;	/* Auxiliar rate queue index */
    ushort			target_himark;	/* Max # pool buffers allowed */
    ulong			target_shmem;	/* Sh mem needed by this MTU */
    leveltype			level;		/* Interrupt level */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    atm = idb->atm_db;

    /*
     * Adjust buffer pools and high water marks
     */
    level = raise_interrupt_level (NETS_DISABLE);
    for (rq_indx=0; rq_indx<NUM_FREEQ; rq_indx++) {
	freeq = &ds->rxfreeQ[rq_indx];
	/* What is the ideal max # of buffers for this pool ? */
	/*
	 * save the following statement for multiple pool support
	 *
	target_himark = atm->num_vc ?
            ((ds->pktid_rxlimit * freeq->num_vc) / atm->num_vc) : 0;
	*/
	/*
	 * This only works with a single buffer pool (for CSCdi65263 fix)
	 */
	target_himark = freeq->num_vc ? ds->pktid_rxlimit : 0;
	/* Prevent MTUs larger than the default to grab too much memory */
	target_shmem = 2 * ds->pktid_rxlimit * freeq->bufpool->size;
	if (target_shmem > ATMZR_SHMEMMAX)
	    target_himark = (ATMZR_SHMEMMAX * target_himark) / target_shmem;
	/* Adjust permanent number of buffers and buffer cache */
	freeq->high_mark = target_himark;
	pool_adjust(freeq->bufpool, 0, 2*freeq->high_mark,
		    2*freeq->high_mark, TRUE);
	pool_adjust_cache(freeq->bufpool, 2*freeq->high_mark);
	atmzr_refill_freeq (ds, freeq);
    }
    reset_interrupt_level (level);
    ds->adjust_pools = FALSE;
}


/*
 * a t m z r _ s e t u p _ v c  ( )
 *
 * A VC is created with the specified parameters.
 * If this call is being called from interrupt level, a
 * synchronous response will be requested, otherwise it will
 * be asynchronous and the process will be put to sleep.
 * Associated to the opening of the VC is the free receive
 * buffer allocation. The number of free receive buffers
 * allocated is proportional to the VC bandwidth.
 *
 * Returns: TRUE = VC opened succefully
 * 	    FALSE = VC opening failed
 */
boolean
atmzr_setup_vc (hwidbtype *idb,			/* Interface HW IDB ptr */
		vc_info_t *vcinfo)		/* VC info ptr */
{
    atmzr_instance_t		*ds;		/* Device control struc ptr */
    atm_db_t			*atm;		/* ATM struc pointer */
    pooltype			*bufpool;	/* Buffer pool pointer */
    vcdata_t			atmvc;		/* VC parameters structure */
    ushort			mbs;		/* Max burst size */
    ushort			rateq;		/* Rate queue index */
    ushort                      burst;          /* burst variable */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    atm = idb->atm_db;

    DEBUG_EVENTS ("\natmzr_setup_vc(%s): vc:%d vpi:%d vci:%d",
        idb->hw_namestring, vcinfo->vc, vcinfo->vpi, vcinfo->vci);

    vcinfo->flags &= ~VC_FLAG_ACTIVE;    /* init the VC as inactive */
 
    /*
     * Make sure that VPI & VCI fields are within allowed ranges
     */
    if ((vcinfo->vci & ~(atm->config.rt_vc_per_vp - 1)) ||
	(vcinfo->vpi & ~((MAX_ATMZR_VPXVC / atm->config.rt_vc_per_vp) - 1))) {
        DEBUG_ERRORS ("\nUnable to open VC %d: VPI/VCI value(s) out of range",
	    vcinfo->vc);
        return(FALSE);
    }

    /*
     * Check for conditions that will delay the VC creation
     */
    if(!(atm->flags & ATM_FLAG_UP)) {
        DEBUG_EVENTS ("\natmzr_setup_vc(%s): vc%d creation delayed: ATMZR disabled",
            idb->hw_namestring, vcinfo->vc);
        return(TRUE);
    }
    if (vcinfo->swidb->subif_state != SUBIF_STATE_UP) {
	DEBUG_EVENTS("\natmzr_setup_vc(%s): vc%d creation delayed: ATMZR sub-interface shutdown",
		     vcinfo->swidb->namestring,vcinfo->vc);
      return(TRUE);
    }

    if ((atm->config_status != ATM_CONFIG_OK) &&
	(atm->config_status != ATM_CONFIG_IN_PROGRESS)) {
        DEBUG_EVENTS ("\natmzr_setup_vc(%s): vc%d creation delayed: ATMZR not cfg",
		 idb->hw_namestring, vcinfo->vc);
        return(TRUE);
    }

    /*
     * Build Open_VC command parameters
     */
    memset(&atmvc, 0, sizeof (vcdata_t));
    atmvc.vcid = vcinfo->vc;
    atmvc.encaps = vcinfo->etype;
    atmvc.vc_aal = (ISVCAAL34 (vcinfo)) ? ATM_CFG_MODE_AAL34 : ATM_CFG_MODE_AAL5;
    atmvc.vc_options = 0;
    atmvc.mtu = atm->config.mtu + vcinfo->encapsize;
    atmvc.atmhdr.vpi = vcinfo->vpi;
    atmvc.atmhdr.vci = vcinfo->vci;
    atmvc.vc_midlo = vcinfo->mid_start;
    atmvc.vc_midhi = vcinfo->mid_end;
    atmvc.vc_midnxt = vcinfo->mid_start;

    /* Associate a public buffer pool to this VC */
    atmvc.vc_bufq = atmzr_get_pool_index (idb, idb->maxmtu);
    if (atmvc.vc_bufq == ATMZR_BAD_POOL) {
        DEBUG_ERRORS ("\nUnable to assign VCD %d to a Buffer Pool", vcinfo->vc);
        return(FALSE);
    }
    bufpool = vcinfo->bufpool = ds->rxfreeQ[atmvc.vc_bufq].bufpool;

    /* Find a suitable rate queue */
    if (vcinfo->peakrate == 0)
	vcinfo->peakrate = atm->plimrate*ATM_MBPS2KBPS;
    if (vcinfo->avgrate == 0)
	vcinfo->avgrate = vcinfo->peakrate;
    rateq = atmzr_get_rateq (atm, vcinfo->peakrate);
    if (rateq == ATMZR_BAD_RATEQ) /* start new rateq if none suitable */
	rateq = atmzr_set_rateq (atm, vcinfo->peakrate);
    if (rateq == ATMZR_BAD_RATEQ) {
	DEBUG_ERRORS ("\nUnable to assign VCD %d to a Rate Queue", vcinfo->vc);
        return(FALSE);
    }
    atmvc.vc_rateq = rateq;
    /*
     * An active VC is expected to be associated with a non-zero
     * value of vcmode; 
     * ATMZR_AIP_HACK guarantees non-zero vcmode; 
     * This hacking should be removed in California code moduration 
     */
    vcinfo->vcmode = (rateq << ATM_VC_RATEQ_SHIFT) | ATMZR_AIP_HACK;

    /* Calculate Burst and Idle window sizes 
     * burst rate can't be zero unless not entered - use default
     * or default flag is already set in reset case
     */
    if (((vcinfo->flags & VC_FLAG_DEFAULT_BURST) != 0) ||
	(vcinfo->burstcells == 0)) {
	uint default_mtu;

	/* get interface mtu size to calculate burst size to burst mtu*/
	if (vcinfo->swidb != NULL) {
	    default_mtu = vcinfo->swidb->sub_mtu;
	} else {
	    default_mtu = idb->maxmtu;
	}
	default_mtu += (ETHER_ARPA_ENCAPBYTES +	ATM_SNAP_ENCAPBYTES);

	/* default is enough cells to burst mtu */
	burst = default_mtu/ATM_PAYLOAD_BYTES_IN_A_CELL;

	/* add one for integer truncation to make default > mtu*/
	if ((default_mtu % ATM_PAYLOAD_BYTES_IN_A_CELL) != 0)
	    burst++;

	vcinfo->burstcells = burst;
    }

    mbs = vcinfo->burstcells;
    atmvc.burstw = mbs;
    atmvc.idlew = ((vcinfo->peakrate-vcinfo->avgrate) * mbs) / vcinfo->avgrate;
    if (vcinfo->peakrate % vcinfo->avgrate)
	atmvc.idlew++;

    /*
     * Send VC open command to ATMizer NIM
     */
    if ((ds->last_rsp_stat = atmzr_issue_command (idb, CMD_VC_OPEN, vcinfo->vc,
	 (uchar *)&atmvc, sizeof (vcdata_t), 0))) {
	DEBUG_ERRORS ("\nUnable to open VC %d", vcinfo->vc);
	return(FALSE);
    }

    vcinfo->flags |= VC_FLAG_ACTIVE;
    atm->rateq[atmvc.vc_rateq].num_vc++;
    GET_TIMESTAMP(vcinfo->last_updown_time); /* time stamp the up time*/

    /*
     * Adjust free buffer pool
     */
    ds->rxfreeQ[atmvc.vc_bufq].num_vc++;
    ds->adjust_pools = TRUE;

    return (TRUE);
}


/*
 * a t m z r _ t e a r d o w n _ v c  ( )
 *
 * Destroy a VC.
 */
boolean
atmzr_teardown_vc (hwidbtype *idb,		/* Interface HW IDB ptr */
		   vc_info_t *vcinfo)		/* VC struct ptr */
{
    atmzr_instance_t		*ds;		/* Device control struc ptr */
    atm_db_t			*atm;		/* ATM database pointer */
    ushort			rateq;		/* VC's associated rate queue */
    ushort			pool_indx;	/* Buffer pool index */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    atm = idb->atm_db;
 
    DEBUG_EVENTS ("\natmzr_teardown_vc(%s): vc:%d vpi:%d vci:%d",
        idb->hw_namestring, vcinfo->vc, vcinfo->vpi, vcinfo->vci);

    /*
     * ATMizr is requested to teardown the VCC only if it is up
     */
    if (!(atm->flags & ATM_FLAG_UP) || (ds->nim_operational == FALSE)) {
        DEBUG_EVENTS("\natmzr_teardown_vc(%s): ATMizr is disabled; %s%d %s",
                      idb->hw_namestring, "Closing VC=", vcinfo->vc,
                      "is delayed.");
    } else {
        /*
         * Allow some time for packets to be transmitted by ATMizer
         * before sending down a close command to it.
         */
        usecdelay(20);
 
        /* Now close. */
        ds->last_rsp_stat = atmzr_issue_command (idb, CMD_VC_CLOSE,
                                                 vcinfo->vc, NULL, 0, 0);
        if (ds->last_rsp_stat != RSP_STAT_OK) {
            DEBUG_ERRORS ("\nATMzr close VC command error: %X",
                           ds->last_rsp_stat);
            return(FALSE);
        }
    }

    /*
     * Command successful - Mark this VC as INACTIVE now
     */
    vcinfo->flags &= ~VC_FLAG_ACTIVE;
    rateq = vcinfo->vcmode >> ATM_VC_RATEQ_SHIFT;
    atm->rateq[rateq].num_vc--;
    if (atm->rateq[rateq].num_vc == 0 &&
	atm->rateq[rateq].state == RATE_QUEUE_DYNAMIC) {
        /*
         * ATMizr is requested to destroy the rateq only if it is up
         */
        if (!(atm->flags & ATM_FLAG_UP) || (ds->nim_operational == FALSE)) {
            DEBUG_EVENTS("\natmzr_teardown_vc(%s): ATMizr is disabled; %s%d %s",
                        idb->hw_namestring, "Destroying RQ=", rateq, "is delayed.");
        } else if ((ds->last_rsp_stat = atmzr_issue_command (idb, CMD_RATEQ_DESTROY,
				     rateq, NULL, 0, 0)) != RSP_STAT_OK) {
            DEBUG_ERRORS ("\nUnable to remove RQ %d", rateq);
            return(TRUE);
	}
	atm->rateq[rateq].state = RATE_QUEUE_DISABLED;
    }
    vcinfo->vcmode = 0;
    GET_TIMESTAMP(vcinfo->last_updown_time);
    if (vcinfo->avgrate == ds->fastest_vc_rate) {
        ds->fastest_vc_rate = 0;
    }

    /*
     * Adjust free buffer pool
     */
    pool_indx = atmzr_get_pool_index (idb, vcinfo->bufpool->size);
    ds->rxfreeQ[pool_indx].num_vc--;
    ds->adjust_pools = TRUE;

    return(TRUE);
}


/*
 * a t m z r _ r a w _ i n p u t  ( )
 *
 * Process a RAW cell type. This may be OAM F4 or F5 message.
 */
void
atmzr_raw_input (hwidbtype *idb,		/* Interface HW IDB ptr */
		 paktype *pak)			/* Incoming pkt hdr ptr */
{
    process_enqueue_pak(atm_oam_packetQ, pak);
}


/*
 * a t m z r _ g e t _ v c _ s t a t s  ( )
 *
 * Get VC specific information from the NIM.
 * At this point all VC information requested should
 * be computed at the host. Only cell-specific VC
 * information will be kept at the NIM.
 * For AAL5, all VC statistics can be gathered
 * in the host itself with no need to access the
 * ATMizer.
 *
 * Returns: TRUE = success
 *	    FALSE = failure
 */
boolean
atmzr_get_vc_stats (hwidbtype *idb,		/* Interface HW IDB ptr */
		    vc_info_t *vc)		/* VC struct ptr */
{
    return(TRUE);
}



/*
 * a t m z r _ p r o c  _ t x d o n e _ i n l i n e  ( )
 *
 * Converts packet descriptor ID to paktype pointer, updates
 * transmit statistics and returns the buffer to the public
 * buffer pool.
 */
static inline void
atmzr_proc_txdone_inline (atmzr_instance_t *ds,	/* ATMizer database ptr */
			  ushort pktid)		/* Packet descriptor index */
{
    paktype			*pak;		/* paktype packet ptr */

    /*
     * Convert ID to paktype pointer and return buf to public pool
     */
    ID2PAK (pak, pktid, ds, FALSE);
    if (pak) {
        RETID (pktid, wrq_db_t, ds);
	ds->pktid_txcount--;
        if (ds->pktid_txcount < PKTDESC_THRESHHOLD)
            ds->tx_control_flag = FALSE;
	datagram_done (pak);
    } else {
	return;
    } 

    /*
     * Check transmit status
     */
    if (TXPD_STAT (ds->pktdT[pktid].txdone_stat_mid) != TXSTAT_OK) {
	ds->hwidb->counters.output_error++;
	DEBUG_COUNTERS (ds->tx_errors++);
    }

    /*
     * The latest output time
     */
    GET_TIMESTAMP (ds->hwidb->lastoutput);
    TIMER_STOP (ds->hwidb->xmittimer);
}


/*
 * a t m z r _ d e c o d e _ e n c a p s _ i n l i n e  ( )
 *
 * Decodes the incoming packet encapsulation.
 * This function assumes that the first two short words at the location
 * pointed by datagramstart contain the VCID and encapsulation
 * type respectively, followed by the datagram effectivelly
 * received over the ATM link.
 * For MUX VCs, the enctype field contains the ETYPE used by the
 * VC.
 *
 * This function returns the encapsulation type as encoded as an rxtype
 * variable.
 */
static inline ushort
atmzr_decode_encaps_inline (paktype *pak,	/* Paktype pointer */
			    ushort atm_enctype,	/* ATM encapsulation type */
                            vc_info_t *vc,      /* vc_info pointer */
                            boolean *lane_flush,  /* returned flag */
                            boolean *try_tbridge) /* returned flag */
{
    snap_hdr			*snap;		/* SNAP header pointer */
    uchar			*datagram;	/* Datagramstart pointer */
    ushort			rxtype;		/* encapsulation type */
    ushort			dsap;		/* Destination SAP */

    /*
     * Initialize local variables
     */
    datagram = pak->datagramstart;
    rxtype = RXTYPE_UNKNOWN;

    /*
     * Determine encapsulation based on the buffer encapsulation field
     */
    switch (atm_enctype) {

    /*
     * Check AAL encapsulation first
     */
    case ATM_ENCTYPE_SNAP:			/* LLC/SNAP */
	snap = &((atm_aal5snap_t *)datagram)->snap;
	pak->info_start = (uchar *)snap;
	pak->encsize = ATM_SNAP_ENCAPBYTES;
	dsap = GETSHORT(&snap->dsap);
	RXTYPE_SNAP (rxtype, snap, dsap);
	break;

    case ATM_ENCTYPE_SMDS:			/* SMDS */
    {
        smdstype                    *smds;

        smds = (smdstype *)((atm_encap_t *)datagram)->data;
        snap = (snap_hdr *)smds->data;
        pak->info_start = (uchar *)snap;
        pak->encsize = ATM_SMDS_ENCAPBYTES;
        dsap = GETSHORT(&snap->dsap);
        RXTYPE_SNAP (rxtype, snap, dsap);
        break;
    }
    case ATM_ENCTYPE_NLPID:
	switch (((atm_aal5nlpid_t *)datagram)->nlpid) {
	case ATM_NLPID_SNAP1PAD:		/* LLC/NLPID/SNAP1 */
	    snap = (snap_hdr *)&((atm_aal5nlpid_t *)datagram)->nlpid;
	    pak->info_start = (uchar *)snap;
            pak->encsize = ATM_NLPID_ENCAPBYTES + ATM_NLPID_SNAP_HDRBYTES;
	    dsap = GETSHORT(&snap->dsap);
	    RXTYPE_SNAP (rxtype, snap, dsap);
	    break;
	case ATM_NLPID_CLNP:			/* LLC/NLPID/CLNP */
            pak->encsize = ATM_NLPID_ENCAPBYTES;
	    pak->info_start =
	    pak->network_start = ((atm_aal5nlpid_t *)datagram)->data;
	    rxtype = RXTYPE_CLNS;
	    break;
	case ATM_NLPID_IETF_IP:			/* LLC/NLPID/IP */
            pak->encsize = ATM_NLPID_ENCAPBYTES;
	    pak->info_start =
	    pak->network_start = ((atm_aal5nlpid_t *)datagram)->data;
	    rxtype = RXTYPE_DODIP;
	    break;
	} 
	break;

    case ATM_ENCTYPE_LANE:
    case ATM_ENCTYPE_LANE_BUS:
        /*
         *                                rxtype        try_tbridge lane_flush
         * LANE control packets &
         *  pkts to LES/BUS             LANE_CONTROL       false      false
         *
         * LANE data pkts w/ my lecid   LANE_CONTROL       false      true
         *
         * LANE data pkts with dest
         *  MAC == unicast, me          from ether_decode  false      false
         *
         * LANE data pkts with dest
         *  MAC == unicast, not me      from ether_decode  true       true
         *
         * LANE data pkts with dest
         *  MAC == multicast            from ether_decode  true       false
         */
        pak->atm_vc = vc;
        switch(vc->user_vc_type) {
        case LANE_LEC_FORWARD:
        case LANE_LEC_SEND:
            if (lane_from_my_lecid_inline(pak, vc)) {
                rxtype = RXTYPE_LANE_CONTROL;
                pak->encsize = ATM_ENCAP_HDRBYTES;
                *lane_flush = TRUE;
                break;
            }
            /* fall through: treat as LEC_DATA if it's not from our lecid */
        case LANE_LEC_DATA:
	     if (lane_is_control_pak_inline(pak)) {
                rxtype = RXTYPE_LANE_CONTROL;
                pak->encsize = ATM_ENCAP_HDRBYTES;
            } else {
                rxtype = ether_decode_encapsulation_inline(pak,
                       (ether_hdr *)(pak->datagramstart +
                                     ATM_AAL5_LANE_ENCAPBYTES));
                pak->encsize += ATM_AAL5_LANE_ENCAPBYTES;
                if (!lane_is_my_mac_inline(pak, vc)) {
                    *try_tbridge = TRUE;
                    if (lane_is_unicast_mac_inline(pak)) {
                        *lane_flush = TRUE;
                    }
                }
            }
            break;

        case LANE_LEC_TR_FORWARD:
          if (lane_from_my_lecid_inline(pak, vc)) {
              rxtype = RXTYPE_LANE_CONTROL;
              pak->encsize = ATM_ENCAP_HDRBYTES;
              *lane_flush = TRUE;
              break;
          }
                /* fall through: treat as LEC_DATA if not from our lecid */
        case LANE_LEC_TR_DATA:
          if (lane_is_control_pak_inline(pak)) {
              rxtype = RXTYPE_LANE_CONTROL;
          } else {
              pak->datagramstart = pak->datagramstart +
                    ATM_ENCAP_HDRBYTES +
                    ATM_LANE_LECID_ENCAPBYTES;
              rxtype = tring_decode_encaps(pak);
              pak->enctype = ET_ATM;
              return(rxtype);
          }
          break;

        default:
            rxtype = RXTYPE_LANE_CONTROL;
            pak->encsize = ATM_ENCAP_HDRBYTES;
            break;
        }
        break;

    case ATM_ENCTYPE_FRNLPID:			/* FR NLPID - Not supported */
	break;

    case ATM_ENCTYPE_SAAL:			/* Signalling */
	break;

    case ATM_ENCTYPE_RAW:			/* Raw & OAM & ILMI cells */
        /*
         * vc's coming into the LANE BUS have enctype set to RAW
         * in order to keep the cbus microcode from messing with with
         * packets to the BUS.  If this is a LANE vc, it must be a
         * packet coming into the BUS, so set rxtype for LANE.
         */
        if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5LANE) {
            pak->atm_vc = vc;
            rxtype = RXTYPE_LANE_CONTROL;
        }
	break;

    /*
     * Check MUX VCs from here on
     */
    case TYPE_IP10MB:				/* MUX IP */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_DODIP;
	break;

    case TYPE_DECNET:				/* MUX DECnet */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_DECNET;
	break;

    case TYPE_XNS:				/* MUX XNS */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_XNS;
	break;

    case TYPE_ETHERTALK:			/* MUX Ethertalk */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_APPLE;
	break;

    case TYPE_NOVELL1:				/* MUX IPX */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_NOVELL1;
	break;
    case TYPE_NOVELL2:
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_NOVELL2;
	break;

    case TYPE_VINES2:				/* MUX Vines */
    case TYPE_VINES_ECHO2:
    case TYPE_VINES:      /* optional */
    case TYPE_VINES_ECHO: /* optional */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_SNAP_VINES;
	break;

    case TYPE_DEC_SPANNING:			/* MUX DEC Bridging */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	break;

    case TYPE_RSRB:				/* MUX Remote SRB */
	pak->network_start  =
	pak->info_start = ((atm_encap_t *)datagram)->data;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	rxtype = RXTYPE_SRB;
	break;
    }

    return (rxtype);
}


/*
 * a t m z r _ g e t _ n l p i d t y p e  ( )
 */
ushort
atmzr_get_nlpidtype (hwidbtype *idb,		/* Interface HW IDB ptr */
		     uchar *encaps)		/* NLPID encaps pointer */
{
    return (((atm_aal5nlpid_t *)encaps)->nlpid);
}

/*****************************************************************************
 * Name: atmzr_sniff
 *
 * Description: 4x00 specific routine "sniff" words from MEMD.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for this interface (unused)
 *   pak -- pak pointer, datagramstart points to packet in MEMD
 *   data -- pointer to beginning of packet in MEMD (not valid on 4500)
 *   sniff_buffer -- pointer to sniff buffer (unused)
 *   offset -- offset in 16-bit words from rxoffset to data to sniff
 *   num_shorts -- number of 16-bit words to sniff (unused)
 *
 * Note:
 *   Unlike 7000, MEMD is directly accessible on 4x00, so all we have to
 *   do is return pointer based on datagramstart & offset.
 *
 * Return Value: pointer to desired data
 *
 *****************************************************************************/
ushort *
atmzr_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
             ushort *sniff_buffer, int offset, int num_shorts)
{
    return((ushort *)pak->datagramstart + offset);
}

/*
 * a t m z r _ q u e u e _ f o r _ p r o c e s s  ( )
 *
 * Enqueue a packet that couldn't be fast-switched or bridged to
 * be handled at process-level.
 */
static void
atmzr_queue_for_process (atmzr_instance_t *ds,	/* ATMizer database ptr */
			 paktype *pak)		/* Incoming pkt hdr ptr */
{
    uchar			*datagramstart;	/* Original pak datagramstart */
    iqueue_t			iqueue;		/* Queue to process packet */
    ushort			scount;		/* Original datagram size */

    /*
     * Initialize local variables
     */ 
    scount = pak->datagramsize;
    datagramstart = pak->datagramstart;
 
    /*
     * Drop the packet and throttle the interface if the input
     * queue is full.
     */
    if (ds->hwidb->input_qcount > ds->hwidb->input_defqcount) {
        atmzr_disable (ds->hwidb);
        datagram_done (pak);
        ds->hwidb->counters.input_drops++;
        return;
    }
 
    /*
     * Do we need to change datagramstart? idb->parse_packet
     * will set datagramstart appropriately for the type of
     * data packet that was received.  If this is not the
     * same as the current location of the data packet, i.e.
     * its not an arpa style frame, then the data will need
     * to be copied into a new packet.
     */
    iqueue = (*ds->hwidb->parse_packet)(ds->hwidb, pak, NULL);
    if (NULL == iqueue) {
	datagram_done(pak);
	return;
    }

    if (pak->datagramstart != (uchar *)datagramstart) {
        paktype *tmp;
 
        /*
         * Yes, we need to copy the packet into a new buffer
         */
        tmp = pak;
        pak = input_getbuffer((uint)scount, ds->hwidb);
        if (pak) {

            /*
             * Save the packet enctype:
             */
            pak->enctype = tmp->enctype;
 
            /*
             * We don't have to worry about masking the buffer flags,
             *  since they now live in buffer_flags:
             */
            pak->flags = tmp->flags;
 
            /*
             * Compute datagramstart address in new packet from start
             * offset from old packet.  Call fddi_pakaling with the same
             * data as before, only have it adjust the new packet this
             * time.
             */
            iqueue = (*ds->hwidb->parse_packet)(ds->hwidb, pak, datagramstart);
            pak->datagramsize = scount;
            bcopy (datagramstart, pak->datagramstart, (uint)scount);
            datagram_done (tmp);
        } else {
            /*
             * Failed to get new buffer to copy frame - reclaim buffer
             */
            datagram_done (tmp);
            return;
        }
    }

    /*
     * Do packet accounting for packets going up to process level:
     */
    ds->hwidb->counters.rx_cumbytes += pak->datagramsize;
    ds->hwidb->counters.inputs++;
 
    /*
     * Pass the frame up to process level.
     */
    (*iqueue) (ds->hwidb, pak);
}


/*
 * a t m z r _  o u t p u t  ( )
 *
 * Start/resume transmission of packets enqueued for output at
 * the given hardware IDB. If a packet is to be transmitted
 * from process-level, this function has to be called after
 * the packet is enqueued for transmission via idb->oqueue().
 * This function can be called either from process or interrupt
 * level.
 */
void
atmzr_output (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* Control struc ptr */
    paktype			*pak;		/* Packet header ptr */
    pktd_t			*pktd;		/* Packet descriptor ptr */
    char			*datagramstart;	/* Real datagramstart */
    vc_info_t			*vc;		/* VC info pointer */
    ushort			datagramsize;	/* Real datagramsize */
    ushort			pktid;		/* Packet descriptor index */
    ushort			padsize;	/* Packet padding size */
    ushort			txopt;		/* Transmission options */
    leveltype			level;		/* Original interrupt level */

    /*
     * Initialize auxiliar variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    padsize = 0;
 
    /*
     * If the transmitter is hung, reset the interface before beginning
     */
    ATMZR_HUNG_CHECK (idb, ds, level);
 
    /*
     * Keep outputing as long as the interface is up,
     * we have more packets to transmit, and there is room
     * in the transmit ready queue
     */
    while (idb->state == IDBS_UP) {
        /*
         * Find a legal sized packet to send.
         * Please note that we use raise_interrupt_level and
         * reset_interrupt_level to renable network interrupts between each
         * output packet so that we don't block out processing of input
         * network traffic while draining a large output hold queue. We have
         * interrupts disabled while we dequeue each packet. This way, if we
         * are interrupted after processing a packet, and if atmzr_output() is
         * called again from interrupt level, it will get the correct next
         * packet to process. This allows us to drain the output hold queue
         * at interrupt level while we are processing a large burst of fast
         * switched traffic that is going out on this interface via the call
         * to atmzr_output in atmzr_fastsend().
         */
        level = raise_interrupt_level (NETS_DISABLE);
        while ((pak = (*idb->oqueue_dequeue)(idb))) {
            /*
             * Prevent too large packets from being sent:
             */
            if (pak->datagramsize > idb->buffersize) {
                errmsg(&msgsym(TOOBIG, LINK),
			idb->hw_namestring,
			pak->datagramsize);
                datagram_done (pak);
                continue;
            }
            break;
        }
	/*
	 * Make sure we have a packet to transmit and
	 * that the outgoing VC is open
	 */
        if ((pak == NULL) ||
	    !(vc = atm_getvc_inline(idb->atm_db, pak->atm_vcid)) ||
	    !(ISVCACTIVE(vc))) {
	    if (pak) {
		datagram_done(pak);
		DEBUG_COUNTERS (ds->tx_badvc++);
	    }
            reset_interrupt_level(level);
            return;
        }
        /*
         * Check for free packet descriptor
         */
        /* if the outstanding packet descriptor used by tx packets is
         * reached to a limit, the tx_control_flag is turned on
         */
         if (ds->pktid_txcount == ds->pktid_txlimit) {
             ds->tx_control_flag = TRUE;
         }
        if ((pktid = TXNEXTINDEX (ds))) {
            /* if tx_control_flag is already on and the outgoing packet has
             * a slower vc rate, then the packet is dropped here
             */
             if (ds->tx_control_flag == TRUE) {
                 if (vc->avgrate <  ds->fastest_vc_rate) {
                     datagram_done (pak);
                     idb->counters.output_total_drops++;
                     reset_interrupt_level(level);
                     return;
                 }
             }
            /*
             * Allocate the packet descriptor, fill it in
             * and pass it to the interace
             */
            ALLOCINDEX (pktid, pak, &ds->txrdyQ, ds, FALSE);/* Res free pktid */
            if (vc->avgrate > ds->fastest_vc_rate ) {
                ds->fastest_vc_rate = vc->avgrate;
            }
	    ds->pktid_txcount++;		/* Account allocated index */
            pktd = ds->pktdT + pktid;
	    datagramstart = pak->datagramstart + ATM_ENCAP_HDRBYTES;
	    datagramsize = pak->datagramsize - ATM_ENCAP_HDRBYTES;
	    txopt = 0;
	    switch (pak->atm_enctype & ATM_DM_PKT_MSK) {
	    case ATM_DM_PKT_AAL34:			/* AAL3/4 SMDS */
            {
                int pad;
                smdstype *header;
                uchar *ptr;
                int i;
 
                header = (smdstype *)datagramstart;
                header->smds_betag = idb->smds_begin_tag++;
 
                pad = 3 - ((datagramsize + 3) % 4);
                PUTSHORT(&header->smds_basize,
                    (datagramsize - ATM_SMDS_L3_HDRBYTES + SMDS_HEADER + pad));
                ptr = datagramstart + datagramsize;
 
                for (i = 0; i < pad; i++)
                    *ptr++ = 0;
 
                bcopy(&header->smds_rsvd, ptr, SMDS_TRAILER);
                datagramsize += pad + SMDS_TRAILER;
                pak->datagramsize = datagramsize + ATM_ENCAP_HDRBYTES;
            }
		break;
	    case ATM_DM_PKT_OAM_VC:			/* OAM cell */
	    case ATM_DM_PKT_OAM_DM:
		txopt = TXOPT_OAM;
		break;
	    case ATM_DM_PKT_AAL5:			/* AAL5 */
                if ((vc->etype == ATM_ENCTYPE_LANE) &&
                    (datagramsize <
                     (MINETHERSIZE + ATM_LANE_LECID_ENCAPBYTES)))
                {
                    datagramsize = MINETHERSIZE + ATM_LANE_LECID_ENCAPBYTES;
                }
                padsize = AAL5PADSIZ(datagramsize);
                /*
                 * put 00 into cpcs-uu and cpi fields
                 */
                PUTLONG (datagramstart + datagramsize + padsize -
                    ATM_AAL5_CSPDU_TRLBYTES, datagramsize);
	    }
            pktd->txrdy_start = datagramstart;
            pktd->txrdy_size = datagramsize + padsize;
            pktd->txrdy_opt_vc = pak->atm_vcid | txopt;
            PRODINDEX (ds->txrdyQ.wr_ptr, pktid);/* Write pktid to txrdyQ */
            ds->intr->tx_rdy = 0;		/* Generate txrdy intr */
	    /*
	     * Update counters
	     */
            idb->counters.tx_cumbytes += pak->datagramsize; /* total bytes sent */
	    idb->counters.outputs++; /* total packets sent */
	    vc->total_out_pkts++;
	    vc->out_bytes += pak->datagramsize;
            if (!TIMER_RUNNING(idb->xmittimer)) /* start output timer */
                TIMER_START(idb->xmittimer, TIMEOUT); /* three seconds */
        } else {
            /* no packet descriptor available, if vc speed is faster
             * than the fastest vc rate that we know, set a new fastest
             * vc rate 
             */
            if (vc->avgrate > ds->fastest_vc_rate) {
                ds->fastest_vc_rate = vc->avgrate;
            }
            /*
             * No packet descr available, so enqueue packet to the end
             * of the output queue to minimize out of order situations
             */
            DEBUG_COUNTERS (ds->tx_pktidmiss++);
            pak_requeue_head (idb, pak);
	    reset_interrupt_level (level);
	    return;
        }
	reset_interrupt_level (level);
    }
}


/*
 * a t m z r _ f a s t s e n d  ( )
 *
 * Try to transmit a packet if the idb output queue is empty or
 * enqueue it for later transmission otherwise.
 * This function is to be called ONLY FROM INTERRUPT LEVEL as it
 * doesn't protect itself by disabling interrupts. The atmzr_output ()
 * function should be used for transmission requests from process-level
 * after the packet to be transmitted is enqueued for transmission.
 */
void
atmzr_fastsend (hwidbtype *idb,			/* Interface HW IDB ptr */
		paktype *pak)			/* Packet header pointer */
{
    atmzr_instance_t            *ds;            /* Control struc ptr */
    atmzr_regs_t                *rp;            /* Interface registers ptr */
    wrq_db_t			*txrdyq;        /* TxRdy queue descr ptr */
    pktd_t			*pktd;		/* Packet descriptor ptr */
    char			*datagramstart;	/* Real datagramstart */
    vc_info_t			*vc;		/* VC info pointer */
    ushort			datagramsize;	/* Real datagramsize */
    ushort			pktid;		/* Packet descriptor index */
    ushort			padsize;	/* Packet padding size */
    ushort			txopt;		/* Transmission options */
    ulong			acct_proto;	/* Protocol being fast-sw */
    traffic_shape_t             *shape;	        /* shaping bucket */

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     *
     * not clear that software rate control is reasonable on an ATM VC -
     * is that a function of the VC, done in the ATMIZER?
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * Setup all processing auxiliar variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = (atmzr_regs_t *)ds->nim_regs;
    txrdyq = &ds->txrdyQ;
    acct_proto = (ulong)pak->acct_proto;
    padsize = 0;

    /*
     * Make sure that the outgoing VC is open
     */
    if (!(vc = atm_getvc_inline (idb->atm_db, pak->atm_vcid)) ||
	!(ISVCACTIVE(vc))) {
	datagram_done(pak);
	DEBUG_COUNTERS (ds->tx_badvc++);
	return;
    }

    /*
     * If no packets are queued for output on this interface,
     * and if the interface is up, immediately attempt to
     * transmit the packet.
     */
    if ((idb->state == IDBS_UP) && (!idb->output_qcount)) {
	/*
	 * Check for free packet descriptor
	 */
        /* if the outstanding packet descriptor used by tx packets is
         * reached to a limit, the tx_control_flag is turned on
         */
        if (ds->pktid_txcount == ds->pktid_txlimit) {
            ds->tx_control_flag = TRUE;
        }
	if ((pktid = TXNEXTINDEX (ds))) {
            /* if tx_control_flag is already on and the outgoing packet has
               a slower vc rate, then the packet is dropped here
             */
            if (ds->tx_control_flag == TRUE) {
                if (vc->avgrate <  ds->fastest_vc_rate) {
                    idb->counters.output_total_drops++;
                    datagram_done (pak);
                    return;
                }
            }
            /*
             * Allocate the packet descriptor, fill it in
             * and pass it to the interace
             */
            ALLOCINDEX (pktid, pak, &ds->txrdyQ, ds, FALSE);/* Res free pktid */
            if (vc->avgrate > ds->fastest_vc_rate ) {
                ds->fastest_vc_rate = vc->avgrate;
            }
	    ds->pktid_txcount++;		/* Account allocated index */
            pktd = ds->pktdT + pktid;
	    datagramstart = pak->datagramstart + ATM_ENCAP_HDRBYTES;
	    datagramsize = pak->datagramsize - ATM_ENCAP_HDRBYTES;
	    txopt = 0;
	    switch (pak->atm_enctype & ATM_DM_PKT_MSK) {
	    case ATM_DM_PKT_AAL34:			/* AAL3/4 SMDS */
            {
                int pad;
                smdstype *header;
                uchar *ptr;
                int i;
 
                header = (smdstype *)datagramstart;
                header->smds_betag = idb->smds_begin_tag++;
 
                pad = 3 - ((datagramsize + 3) % 4);
                PUTSHORT(&header->smds_basize,
                    (datagramsize - ATM_SMDS_L3_HDRBYTES + SMDS_HEADER + pad));
                ptr = datagramstart + datagramsize;
 
                for (i = 0; i < pad; i++)
                    *ptr++ = 0;
 
                bcopy(&header->smds_rsvd, ptr, SMDS_TRAILER);
                datagramsize += pad + SMDS_TRAILER;
                pak->datagramsize = datagramsize + ATM_ENCAP_HDRBYTES;
            }
		break;
	    case ATM_DM_PKT_OAM_VC:			/* OAM cell */
	    case ATM_DM_PKT_OAM_DM:
		txopt = TXOPT_OAM;
		break;
	    case ATM_DM_PKT_AAL5:			/* AAL5 */
                if ((vc->etype == ATM_ENCTYPE_LANE) &&
                    (datagramsize <
                     (MINETHERSIZE + ATM_LANE_LECID_ENCAPBYTES)))
                {
                    datagramsize = MINETHERSIZE + ATM_LANE_LECID_ENCAPBYTES;
                }
                padsize = AAL5PADSIZ(datagramsize);
                /*
                 * put 00 into cpcs-uu and cpi fields
                 */
                PUTLONG (datagramstart + datagramsize + padsize -
                    ATM_AAL5_CSPDU_TRLBYTES, datagramsize);
	    }
            pktd->txrdy_start = datagramstart;
            pktd->txrdy_size = datagramsize + padsize;
            pktd->txrdy_opt_vc = pak->atm_vcid | txopt;
            PRODINDEX (ds->txrdyQ.wr_ptr, pktid);/* Write pktid to txrdyQ */
            ds->intr->tx_rdy = 0;		/* Generate txrdy intr */
	    /*
	     * Update counters
	     */
            idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
            idb->counters.outputs_fast[acct_proto]++;
	    vc->total_out_pkts++;
	    vc->out_bytes += pak->datagramsize;
	    vc->outfast++;
	    ATM_OUTFAST(idb);
	} else {
            /* no packet descriptor available, if vc speed is faster
             * than the fastest_vc_rate that we know, set a new fastest
             * rate
             */
            if (vc->avgrate > ds->fastest_vc_rate) {
                ds->fastest_vc_rate = vc->avgrate;
            }
            else {
                   if (ds->tx_control_flag == TRUE) {
                     idb->counters.output_total_drops++;
                     datagram_done (pak);
                     return;
                   }
            }

	    /*
	     * No packet descr available, so enqueue packet to the end
	     * of the output queue to minimize out of order situations
	     */
	    DEBUG_COUNTERS (ds->tx_pktidmiss++);
            holdq_enqueue_fast(idb, pak);
	}
    } else {
             if (vc->avgrate > ds->fastest_vc_rate)  {
                 ds->fastest_vc_rate = vc->avgrate;
             }
             else {
                   if (ds->tx_control_flag == TRUE) {
                     idb->counters.output_total_drops++;
                     datagram_done (pak);
                     return;
                   }
             }

        /*
         * There are already packets on the output queue, or the
         * interface is not yet up, so add this packet at the end
         * of the queue to minimize out of order situations
         */
        holdq_enqueue_fast(idb, pak);
        atmzr_output(idb);
    }
}


/*
 * a t m z r _ s e t _ m a x d g r a m  ( )
 *
 * Set the MTU size for this interface
 */

boolean
atmzr_set_maxdgram (hwidbtype *idb,		/* Interface HW IDB ptr */
		    int buffersize,		/* Max buffer size */
		    int maxdgram)		/* Max datagram size */
{
    /*
     * Update ATM database & reset interface to start fresh
     */
    ((atm_db_t *)idb->atm_db)->config.mtu =
	idb->maxmtu + ETHER_ARPA_ENCAPBYTES + ATM_SMDS_HDRBYTES;
    if (idb->state != IDBS_ADMINDOWN)
        atmzr_reset (idb);

    return (TRUE);
}


/*
 * a t m z r _ c l e a r _ c o u n t e r s  ( )
 *
 * Clear interface counters.
 */
void
atmzr_clear_counters (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* Control struc ptr */
    ushort			indx;		/* Auxiliar index */
 
    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;

    /*
     * Clear counters
     */
    ds->fast_switched = 0;
    ds->bridged = 0;
    ds->to_process = 0;
    ds->tx_errors = 0;
    ds->tx_badid = 0;
    ds->tx_wrongq = 0;
    ds->tx_restarts = 0;
    ds->tx_pktidmiss = 0;
    ds->tx_badvc = 0;
    ds->rx_badid = 0;
    ds->rx_wrongq = 0;
    for (indx=0; indx<NUM_RXERR; indx++) {
	ds->rx_errors[indx] = 0;
    }
    for (indx=0; indx<NUM_FREEQ; indx++) {
        ds->rxfreeQ[indx].bufmiss = 0;
        ds->rxfreeQ[indx].pktidmiss = 0;
    }
}


/*
 * a t m z r _ p r o c  _ r x r d y _ i n l i n e  ( )
 *
 * Converts packet descriptor ID to paktype pointer, updates
 * receive statistics and process incoming packet.
 */

static inline void
atmzr_proc_rxrdy_inline (atmzr_instance_t *ds,	/* ATMizer database ptr */
			 ushort pktid)		/* Packet descriptor index */
{
    paktype			*pak;		/* Paktype packet ptr */
    vc_info_t			*vc;		/* VC info ptr */
    ushort			atm_vcid;	/* VC Id of recvd packet */
    ushort			atm_enctype;	/* ATM encapsulation type */
    ushort			stat;		/* Receive status */ 
    ushort			stat_vc;	/* Receive status/vcid field */ 
    boolean			oam_cell;	/* OAM cell flag */
    boolean                     lane_flush;     /* flag from decode */
    boolean                     try_tbridge;    /* flag from decode */
    /*
     * Initialize local variables
     */
    oam_cell = FALSE;

    /*
     * Convert ID to paktype pointer
     */
    ID2PAK (pak, pktid, ds, TRUE);
    if (pak) {
        RETID (pktid, rxfreeq_db_t, ds);
	ds->pktid_rxcount--;
    } else {
	return;
    } 

    /*
     * Check receive status
     */
    stat_vc = ds->pktdT[pktid].rxrdy_stat_vc;
    if ((stat = RXPD_STAT(stat_vc)) != RXSTAT_OK) {
	switch (stat) {
	case RXPD_STAT(RXSTAT_F5_SEGMENT):
	case RXPD_STAT(RXSTAT_F5_END2END):
	    oam_cell = TRUE;
	    break;
	case RXPD_STAT(RXSTAT_CRC):
	    ds->hwidb->counters.input_crc++;
	    ds->hwidb->counters.input_error++;
	    DEBUG_COUNTERS (ds->rx_errors[RXPD_STAT(RXSTAT_CRC)]++);
	    retbuffer (pak);
	    return;
	case RXPD_STAT(RXSTAT_GIANT):
	    ds->hwidb->counters.giants++;
	    ds->hwidb->counters.input_error++;
	    DEBUG_COUNTERS (ds->rx_errors[RXPD_STAT(RXSTAT_GIANT)]++);
	    retbuffer (pak);
	    return;
	default:
	    DEBUG_COUNTERS (ds->rx_errors[stat]++);
	    retbuffer (pak);
	    return;
	}
    }

    /*
     * Update packet header
     */
    atm_vcid = stat_vc & RXVCID_MSK;
    if (!(vc = atm_getvc_inline (ds->hwidb->atm_db, atm_vcid))) {
	retbuffer (pak);
	return; 
    }
        
    atm_enctype = oam_cell ? ATM_ENCTYPE_RAW : vc->etype;
    set_if_input(pak, vc->swidb);
    pak->datagramsize = ds->pktdT[pktid].rxrdy_size + ATM_ENCAP_HDRBYTES;
    ds->hwidb->atm_db->fast_vc_info =  pak->atm_vc = vc;
    pak->atm_vcid    = atm_vcid;
    pak->atm_enctype = atm_enctype;

    if (pak->datagramsize > (vc->swidb->sub_mtu + ETHER_ARPA_ENCAPBYTES
                                                + ATM_SMDS_HDRBYTES)) {
	retbuffer (pak);
	return; 
    }

    /*
     * Do VC packet accounting
     */
    vc->in_bytes += pak->datagramsize;
    vc->total_in_pkts++;

    /*
     * Decode packet encapsulation and do Interface packet accounting
     *
     * Passing in the addresses of the local booleans is ugly, but
     * seems to be the most code-efficient way to get the job done.
     */
    lane_flush  = FALSE;
    try_tbridge = FALSE;
    pak->rxtype = atmzr_decode_encaps_inline (pak, atm_enctype, vc,
                                              &lane_flush, &try_tbridge);

    /*
     * Decode will set try_tbridge if this is a LANE data packet to multicast
     * MAC or unicast MAC other than me.  If not LANE, or packet is sent
     * to our MAC, we can skip bridging & go straight to fastswitch routing.
     * This optimizes for most common routing cases.  This code is patterned
     * after rsp_eis_fs() in src-rsp/rsp_if.c, with the answer from 
     * atmzr_decode taking the place of the hardware address filter.
     * 
     * See comments in atmzr_decode for how rxtype & booleans are set.
     */

    if (try_tbridge) {
        if (!vc->swidb->tbridge_on_swidb) {
            if (lane_flush) {
                /*
                 * Is LANE with dst unicast MAC other than me.  When bridging
                 * not configured, must toss.
                 */
                datagram_done(pak);
                return;
            }
            /*
             * bridging not configured, was LANE with dst multicast MAC,
             * fall through to fastswitching.
             */
        } else {
            /*
             * Is LANE with dst multicast MAC or unicast other than me.
             * Let bridging figure out what to do with it.
             */
            if (reg_invoke_tbridge_forward(pak)) {
                DEBUG_COUNTERS (ds->bridged++);
                return;
            }
            /*
             * fall through to fast-switching if bridging didn't do
             * something with packet.
             */
        }
    }

    /*
     * Try packet fast-switching
     */
    ds->hwidb->fast_rcvidb = vc->swidb;
    if (reg_invoke_aip_fs (pak->rxtype, pak)) {
        vc->infast++;
        ATM_INFAST(ds->hwidb);
	DEBUG_COUNTERS (ds->fast_switched++);
        /* We want clear the PAK_INPUTQ flag and decrement the input_qcount
         * so that we don't cause input throttling during fast switching.  
         * But we don't call clear_if_input, so that the input interface
         * of the packet is still known if needed by the output routines.
         * The conditional is to ensure that we don't reference a NULL 
         * pointer if the packet was already sent before we execute this.
         */ 
        if (!pak->if_input) {
	    return;
        }
        pak->flags &= ~PAK_INPUTQ;
        atomic_decrement(&pak->if_input->hwptr->input_qcount);
	return;
    }

    /*
     * If LANE data packet & decode said it came from our own lecid, toss it.
     * Fastswitching attempt will have failed, because decode sets
     * RXTYPE_LANE_CONTROL when it sets lane_flush, and lanefast
     * doesn't do anything with LANE data packets.
     */
    if (lane_flush && !try_tbridge) {
        datagram_done(pak);
        return;
    }

    /*
     * Write VCID and encapsulations to the front of the
     * packet so that the packet has the same format for
     * all Cisco ATM interfaces.
     */
    ((atm_encap_t *)(pak->datagramstart))->vcnum = pak->atm_vcid = atm_vcid;
    ((atm_encap_t *)(pak->datagramstart))->enctype = pak->atm_enctype =
	atm_enctype;

    /*
     * Try packet bridging
     */
    if (vc->etype == ATM_ENCTYPE_SNAP) {
	if (vc->swidb->tbridge_on_swidb) {
	    atm_bridge_ether_hdr *bridge_hdr = (atm_bridge_ether_hdr *)
		(pak->datagramstart);

	    if (ieee_equal_oui(bridge_hdr->oui, atm_tbridge_oui)) {
		if (reg_invoke_tbridge_forward(pak)) {
		    DEBUG_COUNTERS (ds->bridged++);
		    return;
		}
                /*
                 * fall through to process-switching if bridging didn't do
                 * something with packet.
                 */
	    }
	}
    }

    /*
     * If the packet was not bridged, pass it up to process
     * level for processing.
     */
    ds->hwidb->atm_db->fast_vc_info = vc;
    atmzr_queue_for_process(ds, pak);
    DEBUG_COUNTERS (ds->to_process++);
}


/*
 * a t m z r _ g e t b u f f e r _ i n l i n e  ( )
 *
 * Get a buffer from the given ATM private buffer pool.
 * ALL ATM private pool buffers are held in the buffer cache.
 * If the buffer cache is empty, this function tries to get
 * a buffer from the queue's fallback pool.
 *
 * Buffers' datagramstart is aligned to 16 byte boundaries
 * because of performance issues with the Heidelberg NIM DMA
 * transfer implementation, which works best when packets are
 * aligned to 16 byte boundaries.
 *
 * NOTE: Surround call to this inline function in case
 * there is a chance that it might be called from process
 * level.
 */
static inline paktype *
atmzr_getbuffer_inline (rxfreeq_db_t *freeq)	/* Free queue descr ptr */
{
    pooltype			*pool;
    paktype			*pak;
    uchar			*datagramstart;

    /*
     * Initialize local variables
     */
    pool = freeq->bufpool;

    /*
     * Check first for buffers in the cache
     * and then in the fallback buffer pool
     */ 
    pak = pool_dequeue_cache(pool);
    if (!pak) {
	freeq->cachemiss++;
	pak = pool_getbuffer(pool->fallback);
    }
    if (pak) {
	datagramstart = pak_center(pak);
	pak->datagramstart = (uchar *)ALIGN16(datagramstart) -
	                     ATM_ENCAP_HDRBYTES;
    }
 
    return (pak);
}


/*
 * a t m z r _ g e t b u f f e r  ( )
 *
 * Get a buffer from the given ATM private buffer pool.
 * ALL ATM private pool buffers are held in the buffer cache.
 * If the buffer cache is empty, this function tries to get
 * a buffer from the queue's fallback pool.
 */
paktype *
atmzr_getbuffer (rxfreeq_db_t *freeq)		/* Free queue descr ptr */
{
    paktype			*pak;
    leveltype			level;

    level = raise_interrupt_level (NETS_DISABLE);
    pak = atmzr_getbuffer_inline (freeq);
    reset_interrupt_level (level);

    return (pak);
}


/*
 * a t m z r _ r e f i l l _ f r e e q _ i n l i n e  ( )
 *
 * Refill a free buffer index queue with free buffers.
 * The number of buffers allocated is limited by:
 *    1) queue high water mark,
 *    2) max number of pktid indexes allowed for receive, and
 */
static inline void
atmzr_refill_freeq_inline (atmzr_instance_t *ds,/* ATMizer control struc */
			   rxfreeq_db_t *freeq)	/* Recv free queue ctl struc */
{
    paktype			*pak;		/* System memory pkt hdr ptr */
    pooltype			*pool;		/* Buffer pool pointer */
    int   			quota;		/* Recv buffers alloc quota */
    ushort			pktid;		/* Packet descriptor index */

    /*
     * Do nothing if interface throttled or no buffers requested
     */
    if ((freeq->high_mark == 0) || idb_is_throttled(ds->hwidb))
	return;

    /*
     * Calculate the receive buffer quota
     */
    quota = min ((freeq->high_mark - freeq->level),
		 (ds->pktid_rxlimit - ds->pktid_rxcount));

    /*
     * Refill queue up to its high water mark
     */
    pool = freeq->bufpool;
    while (quota-- > 0) {
	/*
	 * Allocate a buffer
	 */
	if (!(pak = atmzr_getbuffer_inline (freeq))) {
	    ds->hwidb->counters.input_resource++;
	    DEBUG_COUNTERS (freeq->bufmiss++);
	    break;
	}
	/*
	 * Assign a packet ID to the buffer
	 */
	if ((pktid = RXNEXTINDEX (ds))) {
	    ALLOCINDEX (pktid, pak, freeq, ds,  TRUE);
	    ds->pktid_rxcount++;
	    (ds->pktdT + pktid)->free_start = pak->datagramstart +
					      ATM_ENCAP_HDRBYTES;
	    PRODINDEX (freeq->wr_ptr, pktid);
	} else {
	    DEBUG_COUNTERS (freeq->pktidmiss++);
	    retbuffer (pak);
	    break;
	}
    }
}


/*
 * a t m z r _ r e f i l l _ f r e e q  ( )
 *
 * Refill a free buffer index queue with free buffers.
 * The number of buffers allocated is limited by:
 *    1) queue high water mark,
 *    2) max number of pktid indexes allowed for receive, and
 *    3) hardware IDB input_defqcount
 *
 * NOTE: This function must be called with NET interrupts disabled
 */
void
atmzr_refill_freeq (atmzr_instance_t *ds,	/* ATMizer control struc */
		    rxfreeq_db_t *freeq)	/* Recv free queue ctl struc */
{
    atmzr_refill_freeq_inline (ds, freeq);
}


/*
 * a t m z r _ l o _ i r q _ h a n d  ( )
 *
 * Low priority interrupt handler.
 * This interrupt is generated by the ATMizer when:
 *    1) an incoming packet is ready to be processed, or
 *    2) an outgoing packet transmission is finished and its buffer
 *       can be freed.
 */
void
atmzr_lo_irq_hand (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t 		*ds;		/* Control struc ptr */
    atmzr_regs_t		*rp;		/* Interface registers ptr */
    ushort			pktid;		/* Packet descriptor index */

    /*
     * Setup all interrupt processing auxiliar variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = (atmzr_regs_t *)ds->nim_regs;

    /*
     * Process all Tx Done packets to free buffers first
     */
    while ((pktid = *(ds->txdoneQ.rd_ptr))) {
	CLRINDEX(ds->txdoneQ.rd_ptr);		/* Update queue */
	atmzr_proc_txdone_inline (ds, pktid);	/* Process descriptor */
    }

    /*
     * Process all Rx Ready packets.
     */
    while ((pktid = *(ds->rxrdyQ.rd_ptr))) {
	if (sched_run_interval && AWAKE (sched_run_time))
	    atmzr_disable (idb);
	CLRINDEX(ds->rxrdyQ.rd_ptr);		/* Update queue */
	atmzr_proc_rxrdy_inline (ds, pktid);	/* Process packet descriptor */
    }

    /*
     * Refill free buffer queues that have reached the low water mark
     * if interface not throttled (via receive buffers starvation)
     *
     * We will NOT refill the receive free
     * buffer queues if the scheduler needs to run. This is used
     * to throttle the receive side of the interface via buffer
     * starvation, which will go away as soon as the interface is
     * unthrottled via atmzr_enable().
     */
    atmzr_refill_freeq (ds, &ds->rxfreeQ[SMALL_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[MID_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[BIG_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[LARGE_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[SEMI_HUGE_FREEQ]);
    atmzr_refill_freeq (ds, &ds->rxfreeQ[HUGE_FREEQ]);
}


/*
 * a t m z r _ p r o c _ r s p  ( )
 *
 * ATMizer response ready interrupt handler.
 * This handler will just make sure that at least one asyncshronous
 * response is available and store the response in the rspmon_t
 * structure which will then wake up the sleeping process.
 * Multiple asynchronous responses are handled the same way.
 * When processing multiple responses, care should be taken to
 * overlook synchronous response slots (to avoid unnecessary
 * C-Bus reads, even though there will not be any problem in the
 * logic if we do that).
 */
void
atmzr_proc_rsp (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;		/* Device control struc ptr */
    cmdrspq_db_t		*cmdrspq_db;	/* Pointer to cmd/rsp queue */
    rspmon_t			*mon_ptr;	/* Response monitor pointer */
    vchar			*data_ptr;	/* Response data buffer ptr */
    vchar			*lim_ptr;	/* Rsp data limit buffer ptr */
    uchar			*buf_ptr;	/* User response buffer ptr */
    ushort			dummy;		/* Dummy auxiliar variable */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    cmdrspq_db = &((atmzr_instance_t *)INSTANCE)->cmdrspQ;
    mon_ptr = &cmdrspq_db->rspmon[cmdrspq_db->rd_ndx];

    /*
     * Clear interrupt request
     */
    dummy = ds->intr->rsp_rdy;

    /*
     * Force return because async commands are not supported
     * in atmzr_issue_commands() routine
     */
    return;

    /*
     * Process all available async responses - skip sync mailboxes
     */
    while (cmdrspq_db->rd_ndx != cmdrspq_db->wr_ndx) {
	/*
	 * Look for asynchronous mailboxes
	 */
	if (mon_ptr->async) {
	    if ((*(ushort *)&mon_ptr->rsp = GETSHORT(&mon_ptr->mb_ptr->rsp))) {
		/*
		 * Async mailbox with available response - read data
		 */
        	data_ptr = mon_ptr->mb_ptr->data;
        	buf_ptr = mon_ptr->data_buf;
        	lim_ptr = buf_ptr + mon_ptr->rsp_dsize;
        	while (buf_ptr < lim_ptr)
		    *(((ushort *)buf_ptr)++) = *(((ushort *)data_ptr)++);
                mon_ptr->mb_ptr->cmd.cmd = 0;
		cmdrspq_db->async_cnt--;
		if (mon_ptr->rsp.stat) {
		    DEBUG_ERRORS ("\natmzr_proc_rsp(%s): resp=%d, error=%d",
			idb->hw_namestring, mon_ptr->rsp.rsp, mon_ptr->rsp.stat);
		} else {
		    DEBUG_EVENTS ("\natmzr_proc_rsp(%s): resp=%d",
			idb->hw_namestring, mon_ptr->rsp.rsp, mon_ptr->rsp.stat);
		}
	    } else {
		/*
		 * Async mailbox with pending response - stop searching
		 */
	        break;
	    }
	}
	/*
	 * Update read index, monitor pointer and queue level
	 */
	cmdrspq_db->rd_ndx = (++cmdrspq_db->rd_ndx) % CMDRSPQSZ;
	mon_ptr = &cmdrspq_db->rspmon[cmdrspq_db->rd_ndx];
        cmdrspq_db->level--;
    }
}


/*
 * a t m z r _ c o n f i g  ( )
 *
 * Configure ATMizer.
 * This function is called after the NIM reports that the
 * initialization is complete. Besides sending down the
 * configuration command, any previously open PVCs have
 * to be reopened again.
 */
static void
atmzr_config (hwidbtype *idb)			/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;		/* Device control block ptr */
    atm_db_t			*atm;		/* ATM database ptr */
    vc_info_t			*vc;		/* VC info database ptr */
    ushort			indx;		/* Auxiliar index */
    cfgdata_t			cfgdata;	/* Config command structure */

    atm_setup_status_t		status;

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    atm = idb->atm_db;

    /*
     * Only configure the interface if the system NVRAM parsing was done
     */
    if (!ds->system_configured)
	return;

    /*
     * System configured - send NIM configuration command
     */
    atm->config_status = ATM_CONFIG_IN_PROGRESS;
    cfgdata.vcpervp = atm->config.rt_vc_per_vp;
    if ((ds->last_rsp_stat = atmzr_issue_command (idb, CMD_CFG_SET, 0,
				 (uchar *)&cfgdata, sizeof (cfgdata_t), 0))) {
	ds->nim_operational = FALSE;
	atm->flags &= ~ATM_FLAG_UP;
	return;
    }

    /*
     * NIM configured - Recreate Active Rate Queues
     */
    for (indx = 0; indx < ATMZR_RATEQ_NO; indx++) {
	if (atm->rateq[indx].state != RATE_QUEUE_DISABLED) {
	    /*
	     * Found an active rate queue
	     */
	    status = atmzr_send_rateq_config(idb, indx);
	    if (status == ATM_SETUP_FAILED)
  		return;
	}
    }

    /*
     * Reopen Active PVCs
     */
    for (indx=1; indx<atm->config.max_intf_vcs; indx++) {
	vc = atm_getvc_all(atm, indx);
	if (vc && (vc->flags & VC_FLAG_PVC) && 
	    (vc->swidb->subif_state == SUBIF_STATE_UP) && (!ISVCACTIVE(vc))) {
            atm_activate_pvc(idb, vc);
	}
	if (vc && !(vc->flags & VC_FLAG_PVC) && ISVCACTIVE(vc)) {
	    vc->flags &= ~VC_FLAG_ACTIVE;
	    atm->num_vc--; /* Take SVCs down */
	}
    }
    /*
     * Configuration and setup complete:
     * Get PLIM's initial state & enable PLIM and LO-IRQ interrupts
     */
    (*ds->plim_inthand)(idb);
    ((atmzr_regs_t *)ds->nim_regs)->ctlr |=
		(ATMZR_CTLR_ENLIRQ | ATMZR_CTLR_ENHIRQ3);
    atm->config_status = ATM_CONFIG_OK;
}


/*
 * a t m z r _ p r o c _ e v e n t s  ( )
 *
 * ATMizer events interrupt handler.
 */
static void
atmzr_proc_events (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;
    atm_db_t			*atm;
    ushort			event;
    ushort			dummy;

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    atm = idb->atm_db;

    /*
     * Clear interrupt request
     */
    dummy = ds->intr->event;

    /*
     * Process all available events
     */
    while ((event = *ds->eventQ.rd_ptr)) {
	/*
	 * Save event code for history
	 */
	ds->last_event = event;

	/*
	 * Update event queue pointer
	 */
	*ds->eventQ.rd_ptr = 0; /* clear entry to indicate it was processed */
	if (++ds->eventQ.rd_ptr >= (ds->eventQ.base + ds->eventQ.size))
	    ds->eventQ.rd_ptr = ds->eventQ.base;

	/*
	 * Process event
	 */
	switch (event) {
	case EVENTQ_INIT_DONE:			/* NIM ready to be configured */
	    DEBUG_EVENTS ("\natmzr_proc_events(%s): Init Done", idb->hw_namestring);
	    /*
	     * Mark interface operational and configure NIM
	     */
	    ds->nim_operational = TRUE;
	    atm->flags |= ATM_FLAG_UP;
	    atmzr_config (idb);
	    break;
	case EVENTQ_BADMEM_PUBSRAM:
	case EVENTQ_BADMEM_PRVSRAM:
	case EVENTQ_BADMEM_VCR:
	case EVENTQ_BADMEM_IRAM:
	case EVENTQ_RQ_OVERLOAD:
	case EVENTQ_BADCHK_PUBSRAM:
	case EVENTQ_BADCHK_PRVSRAM:
	case EVENTQ_BADCHK_IRAM:
	    DEBUG_ERRORS ("\natmzr_proc_events(%s): error=%d",
		idb->hw_namestring, event);
	    /*
	     * Mark interface down
	     */
	    ds->nim_operational = FALSE;
	    atm->flags &= ~ATM_FLAG_UP;
	    break;
	default:
	}
    }
}


/*
 * a t m z r _ h i _ i r q _ h a n d  ( )
 *
 * High priority interrupt handler.
 */
void
atmzr_hi_irq_hand (ushort slot)			/* Interface slot number */
{
    hwidbtype			*idb;		/* Interface HW IDB pointer */
    atmzr_instance_t		*ds;		/* ATMizer database pointer */
    atmzr_regs_t		*rp;		/* ATMizer NIM regs pointer */
    ushort			intr;		/* Interrupt requests */

    /*
     * Initialize local variables
     */
    idb = atmzr_hardware[slot].hwidb;
    ds = (atmzr_instance_t *)INSTANCE;
    rp = ds->nim_regs;

    /*
     * Check for all Hi-IRQ interrupt sources
     */
    intr = rp->erstr;
    if (intr & ATMZR_ERSTR_EVENT_HIRQ1)		/* Event */
	atmzr_proc_events (idb);

    if (intr & ATMZR_ERSTR_RSRDY_HIRQ2)		/* Response Ready */
	atmzr_proc_rsp (idb);

    if (intr & ATMZR_ERSTR_PLIMIRQ)		/* PLIM interrupt */
	(*ds->plim_inthand)(idb);

    if (intr & ATMZR_ERSTR_ASPADDRER)		/* ATMizer Sec port error */
        atmzr_erstr_aspaddrer_hand(idb);

    if (intr & ATMZR_ERSTR_APPDATER)		/* ATMizer Prim port error */
	DEBUG_ERRORS ("\natmzr_hi_irq_hand(%s): Primary port error",
	    idb->hw_namestring);
}

/*
 * a t m z r _ e r s t r _ a s p a d d r e r _ h a n d ( )
 * ATMizer ATMizer Sec port error interrupt handler.
 */
void
atmzr_erstr_aspaddrer_hand (hwidbtype *idb)  /* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;		/* Device control struc ptr */
    atmzr_regs_t                *rp;            /* ATMizer NIM regs pointer */
 
    DEBUG_ERRORS("\natmzr_dumb_inhand(%s): Secondary port error",
                    idb->hw_namestring);
 
    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = ds->nim_regs;
 
    /*
     * Clear the "secondary port error" interrupt
     * by writing any value into the ERR_STAT location.
     */
    rp->erstr |= ATMZR_ERSTR_ASPADDRER;
 
    /* Reset NIM  & mark interface as non-operational */
    ds->nim_regs->ctlr = 0;
    ds->nim_operational = FALSE;
    idb->atm_db->flags &= ~ATM_FLAG_UP;

    /*
     * reset is on
     * interface will be reset in either atmzr_output() or atmzr_periodic()
     */
    ds->hi_irq_reset = 1;
}
