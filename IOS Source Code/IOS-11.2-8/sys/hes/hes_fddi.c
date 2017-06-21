/* $Id: hes_fddi.c,v 3.13.4.15 1996/08/28 12:44:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/hes_fddi.c,v $
 *------------------------------------------------------------------
 * High-End platforms common fddi functions.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hes_fddi.c,v $
 * Revision 3.13.4.15  1996/08/28  12:44:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.4.14  1996/08/26  15:06:37  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.4.13  1996/08/16  00:49:39  ogrady
 * CSCdi54594:  SRB on FDDI propogates broadcast storms
 *              Branch: California_branch
 *              Very small bridge packets not stripped from ring.  Make
 *              routine that inits FIP hold register common instead of
 *              platform specific.
 *
 * Revision 3.13.4.12  1996/08/07  08:58:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.4.11  1996/07/09  05:50:52  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.4.10  1996/06/27  02:41:24  hou
 * CSCdi60669:  Spanning tree dont converage on 802.10 vlan (7507 and
 * C1200)
 * Branch: California_branch
 * - When smf was updated, media interesting addresses were only populated
 *   on firstsw, but not on subinterface
 *
 * Revision 3.13.4.9  1996/06/18  04:26:47  fsunaval
 * CSCdi60307:  Reloading one router can cause FDDI on other routers to
 * transition
 * Branch: California_branch
 *
 * Revision 3.13.4.8  1996/06/10  03:09:22  fsunaval
 * CSCdi58917:  alignment problems with fddi_reset
 * Branch: California_branch
 *
 * Revision 3.13.4.7  1996/06/01  01:31:15  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.13.4.6  1996/05/22  13:50:57  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.13.4.5  1996/05/17  11:17:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.4  1996/05/05  23:06:41  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.3  1996/04/26  15:18:27  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/03  14:28:37  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.4.4  1996/05/09  14:28:29  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.4.3  1996/05/01  09:15:08  shaker
 * CSCdi54444:  Booting causes FIP FDDI to beacon
 * Branch: California_branch
 * Wait around in the process that starts CMT for it to finish
 * so that we know that no other code disables interrupts,
 * preventing us from bringing the MAC online, causing the
 * ring to beacon.
 *
 * Revision 3.13.4.2.16.1  1996/04/27  07:02:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.4.2.6.1  1996/04/08  01:46:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.4.2  1996/03/21  22:40:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.13.4.1  1996/03/18  19:40:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.8.1  1996/03/22  09:36:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/03/11  09:55:08  ogrady
 * CSCdi50668:  TRIP and FIP address filter should disable protocol bits
 *              for SRB.  Fix some AF bit masks shift values which are not
 *              the same for TRIP and FIP.  Disable IP bit only on packets
 *              with rif instead of for multiring configuration.
 *
 * Revision 3.12  1996/03/07  11:05:12  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Don't allow optimum/flowswitching on tokenring or fddi when
 * an SRB RIF is present; otherwise it screws up cache invalidation.
 *
 * Revision 3.11  1996/02/12  21:00:59  motto
 * CSCdi44521:  OIR on FIP breaks SRB on FDDI
 *
 * Revision 3.10.6.1  1996/03/05  06:11:00  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.10.2.1  1996/02/27  20:41:46  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10  1996/02/07  16:11:18  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/04  02:29:18  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.8  1996/02/01  06:02:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/23  00:56:03  ioakley
 * CSCdi46591:  Restore missing srb-related hwidb initialization to
 *              correct problem of "source-bridge spanning" interface
 *              configuration command causing illegal instruction halt.
 *
 * Revision 3.6.2.1  1996/01/24  21:45:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/18  22:47:03  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5.2.1  1996/02/15  19:12:44  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1996/01/05  04:55:45  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/11/21  23:14:23  shaker
 * CSCdi44143:  FIP doesnt EOIR in 7500 since CSCdi42010 was applied
 * If system is not yet configured or if hotstall is still active when an
 * init_cmt() is normally done in fddi_reset(), fork a 'one shot' process
 * to do it later instead of leaving the FDDI interface dead.
 *
 * Revision 3.3  1995/11/17  09:16:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:59:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/12  05:55:49  richl
 * CSCdi38547:  [no] multiring [ip|ipx] should reset fddi
 *
 * Revision 2.5  1995/08/08  16:46:40  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/06/30 20:03:23  fred
 * CSCdi36499:  remove buginf inadvertently left in code
 *
 * Revision 2.3  1995/06/30  20:00:04  fred
 * CSCdi36499:  priority-group causes fddi interface down
 *              resulted from incorrect type of variable
 *
 * Revision 2.2  1995/06/09  13:04:36  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:39:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../les/msg_fddi.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "sys_registry.h"
#include "cbus_registry.h"
#include "../srt/srt_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_fip.h"
#include "ieee.h"
#include "../if/rif.h"
#include "packet.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/fddi_debug.h"
#include "../if/network.h"

#include "parser.h"
#include "../if/parser_defs_fddi.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/smtmsg.h"
#include "../xdi/cspproto.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmproto.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"

#include "mgd_timers.h"
#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"
#include "if_cmd.h"
#include "hes_fip_inline.h"
#include "../smf/smf_public.h"
#include "../tbridge/tbridge_debug.h"
#include "../iprouting/standby.h"
#include "if_mci.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

/*
 * External variables.
 */
extern mgd_timer CSPTimer;

/* Direct_Beacon_DA */
uchar dbptr_fcit[6] = { 0x01, 0x80, 0xc2, 0x00, 0x01, 0x00 };
uchar dbptr_fddi[6] = { 0x80, 0x01, 0x43, 0x00, 0x80, 0x00 };

extern boolean MACInterruptsEnabled;

/*
 * Convert endec status bits to asci string.
 */
static char *const phy_status_string[OVUF_STATUS + 1] = {
    "LSU",
    "NLS",
    "MLS",
    "ILS",
    "HLS",
    "QLS",
    "ALS",
    "OVUF"};

queuetype smtinputQ;
int smt_process_id = 0;


/*
 * Return TRUE if we should wait more before attempting to bring up CMT
 */

static boolean
fddi_not_ready_for_cmt (hwidbtype *hwidb)
{
    if (system_configured && !hotstall_flag)
	return(FALSE);
    else
	return(TRUE);
}



static void
fddi_bring_up_cmt (hwidbtype *hwidb)
{
  int time;

  /*
   * FDDI is ready for CMT, so tell the FIP ucode to fire up CMT
   */
  init_cmt(hwidb);

  /*
   * Wait for the CMT to come up to ensure that no other process
   * disables interrupts while the FDDI ucode state machine
   * cranks up CMT.
   *
   * We want to wait in this process, because we know
   * that interrupts are enabled, so we can get the love note from the
   * FIP ucode which tells us that we are on the ring and need to enable
   * the MAC. If we don't respond to the lovenote and enable the MAC within
   * 165 ms, the ring beacons, which disrupts all FDDI stations on the ring!
   * Not a good thing to happen.
   */
  for (time = 0; time < 1500; time++) {
    if (hwidb->state == IDBS_UP) {
      if (fddi_debug)
	buginf("\n%s: CMT up in %d ms", hwidb->hw_namestring, time);
      return;
    }
    DELAY(1);				/* Wait one ms */
  }
  if (fddi_debug)
    buginf("\n%s: CMT NOT up in %d ms, state %d",
	   hwidb->hw_namestring, time, hwidb->state);
}



static void
fddi_try_init_cmt (hwidbtype *hwidb)
{
    if (hwidb->state == IDBS_ADMINDOWN)
	return;

    /*
     * If any interrupts are disabled, or if we are otherwise
     * not yet ready to start CMT, defer the CMT start to
     * fddi_periodic()
     *
     * Starting CMT with interrupts disabled WILL BEACON the ring,
     * causing it to be unusable by all other FDDI stations on the ring!
     */
    if ( (get_interrupt_level() != ALL_ENABLE) || onintstack() ||
	 fddi_not_ready_for_cmt(hwidb)) {
	fddisb *fsb;

	if (fddi_debug)
	    buginf("\n%s: Setting deferred CMT flag", hwidb->hw_namestring);

        /* 
         * Set deferred CMT flag 
         */
	fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        fsb->fddi_deferred_cmt = TRUE;
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    } else {

	/*
	 * We can do it now
	 */
	fddi_bring_up_cmt(hwidb);
    }
}

/*
 * select_fiber_status
 * Function to select the line status in a particular ENDEC.
 * CAUTION: This function may only be called with interrupts disabled 
 *          (also assumes that the interface has been previously selected).
 */
void select_fiber_status (
    hwidbtype	*hwidb,
    ulong	endec)
{
    ushort status;

    /*
     * Select the correct line status.
     */
    status = send_fip_applique_select_cmd(hwidb, (2 + (endec << 2)));
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x) select endec-%c - select_fiber_status()",
		   status, (endec ? 'B' : 'A'));
    }
}


/*
 * select_endec
 * Function to select a register in a particular ENDEC.
 * CAUTION: This function may only be called with interrupts disabled 
 *          (also assumes that the interface has been previously selected).
 */
void select_endec (
    hwidbtype	*hwidb,
    ulong	reg,
    ulong	endec)
{
    ushort status;

    /*
     * Select the command register.
     */
    status = send_fip_en_select_cmd(hwidb, (ENDEC_CMD + (endec << 2))); 
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x) select endec-%c - select_endec()",
		   status, (endec ? 'B' : 'A'));
	return;
    }

    /*
     * Write the requested data register into the command register.
     */
    status = send_fip_en_write_cmd(hwidb, reg);
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x) write endec-%c - select_endec()",
		   status, (endec ? 'B' : 'A'));
	return;
    }
    
    /*
     * Select the desired data register.
     */
    status = send_fip_en_select_cmd(hwidb, (ENDEC_DATA + (endec << 2)));
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x) select endec-%c - select_endec()",
		   status, (endec ? 'B' : 'A'));
    }
}


/*
 * fddi_periodic
 */
void fddi_periodic (hwidbtype *hwidb)
{
    fddisb *fsb;

    if (hwidb->state == IDBS_ADMINDOWN)
	return;

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    if (fsb->fddi_shutdown_reasons & FDDI_SHUTDOWN_DONE) {
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;
    }
    if (fsb->fddi_shutdown_reasons &
	(WRAP_A_FAIL | WRAP_B_FAIL | THRU_A_FAIL)){
	fsb->fddi_shutdown_reasons |= FDDI_SHUTDOWN_DONE;
	shutdown_going_down(hwidb);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;
    }

    reg_invoke_fddi_ringstate_leds(hwidb);

    /*
     * Perform a deferred FDDI CMT start
     */
    if (fsb->fddi_deferred_cmt) {
        fsb->fddi_deferred_cmt = FALSE;
        /*    
         * We can do it now
         */
        fddi_bring_up_cmt(hwidb);
    }

    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}

/* 
 * get_frinc_count
 */
void get_frinc_count (
    hwidbtype *hwidb)
{
    leveltype	level;
    fddisb *fsb;

    /*
     * Disable interrupts and select the FDDI controller.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    
    /*
     * Read (also clears) error frame counter:
     */
    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    fsb->fddi_frinc += send_errors_cmd(hwidb, FCI_ERR_FRAMES_SEEN);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    reset_interrupt_level(level);
}
   

/*
 * start_smt_input_proc
 * Fire up fddi smt input process, or schedule it to happen when we are
 * no longer in the middle of hot swap processing where it is impossible
 * to fork a new process.
 */
void start_smt_input_proc (void)
{
    if (onintstack()) {
	/* signal that SMT process needs to be started */
	hotstall_smt_flag = TRUE;

	/* signal that hotstall process needs to run */
	hotstall_flag = TRUE;
    } else {
	smt_process_id = process_create(fddi_smt_input, "SMT input",
					NORMAL_STACK, PRIO_NORMAL); 
    }
}

/*
 * read_fiber_status
 * Function to read the line status of a particular ENDEC.
 */
static int read_fiber_status (
    hwidbtype	*hwidb,
    ulong	endec)
{
    leveltype	level;
    ushort	sword;
    ushort	status;

    level = raise_interrupt_level(NETS_DISABLE);

    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x), select, interface %s - read_fiber_status()",
		   status, hwidb->hw_namestring);
	reset_interrupt_level(level);
	return(LSU_STATUS);
    }
    select_fiber_status(hwidb, endec);
    status = send_fip_applique_read_cmd(hwidb, &sword);
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x), read endec-%c, interface %s - read_fiber_status()",
		   status, (endec ? 'B' : 'A'), hwidb->hw_namestring);
	reset_interrupt_level(level);
	return(LSU_STATUS);
    }
    reset_interrupt_level(level);

    /*
     * Return the value read from the endec.
     */
    return((int) sword);
}


/*
 * get_phy_status
 */
static const char *get_phy_status (hwidbtype *hwidb, ushort endec)
{
    int value;
    
    value = read_fiber_status(hwidb, endec);
    return(phy_status_string[value & 0x7]);
}


void smt_threshold_command (parseinfo *csb)
{
    leveltype level;

    if (csb->nvgen) {
	nv_write(smtinputQ.maximum != nfddinets,
		 "%s %u", csb->nv_command, smtinputQ.maximum);
	return;
    }

    /*
     * Get the required command parameter value (the threshold)
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (csb->sense && GETOBJ(int,2)) {
	smtinputQ.maximum = GETOBJ(int,1);
    } else {
	smtinputQ.maximum = nfddinets;
    }
    reset_interrupt_level(level);
}

/*
 * get_phy_pc_status
 * Return an ASCII string descibing the PCM state.
 */

#define PHY_ERROR	(PC_MAINT + 1)

static const char *const phy_pc_status_string[PHY_ERROR + 1] = {
    "off",
    "break",
    "trace",
    "connect",
    "next",
    "signal",
    "join",
    "verify",
    "active",
    "maint",
    "error!"
};

const char *get_phy_pc_status (hwidbtype *hwidb, char b)
{
    phytype	*phy;
    boolean	status;
    ushort	which_phy;
    ushort	phy_state;
    leveltype	level;
    
    if (b) 
	phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_B]);
    else 
	phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_A]);
    if (hwidb->fddi_options & FDDI_FAST_CMT) { 
	level = raise_interrupt_level(NETS_DISABLE);

	status = select_if(hwidb);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
		   status, "select");
	    reset_interrupt_level(level);
	    return(phy_pc_status_string[PHY_ERROR]);
	}

	which_phy = FCI_PHYA_STATE;
	if (b)
	    which_phy = FCI_PHYB_STATE;
	status = send_fip_get_state_cmd(hwidb, which_phy, &phy_state);
	if (status == CMD_RSP_OKAY)
	    phy->pc_state = (phy_state & 0xf);
	else
	    phy->pc_state = PHY_ERROR;

	reset_interrupt_level(level);
    }
    return(phy_pc_status_string[phy->pc_state]);
}

/*
 * get_phy_neighbor 
 * Function to return a ASCII string describing a PHY neigbor.
 */
static const char *const phy_neighbor_string[5] = {
    "A",
    "S",
    "B",
    "M",
    "unk"};

static const char *get_phy_neighbor (hwidbtype *hwidb, char b)
{
    phytype *phy;
    int indx;
    
    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_A]);
    if (b)
	phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_B]);
    if (phy->pc_state == PC_ACTIVE)
	indx = ((phy->rval >> 1) & 0x3);
    else
	indx = 4;
    return(phy_neighbor_string[indx]);
}

/*
 * get_pc_state
 *
 * Returns the XDI encoded PCM state as read from
 * the microcode.
 */
ushort get_pc_state (hwidbtype *hwidb, ushort entity)
{
    ushort	pc_state;
    ushort	which_phy_state;
    ushort	status;
    leveltype	level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    if (entity)
	which_phy_state = FCI_PHYB_STATE;
    else
	which_phy_state = FCI_PHYA_STATE;
    status = send_fip_get_state_cmd(hwidb, which_phy_state, &pc_state);

    reset_interrupt_level(level);

    pc_state &= 0xf;
    return (pc_state);
}

/*
 * get_pc_neighbor
 *
 * Returns the XDI encoded PCM neighbor type as read from
 * the microcode.
 */
ushort get_pc_neighbor (hwidbtype *hwidb, ushort entity)
{
    ushort	pc_neighbor;
    ushort	pc_rval;
    ushort	which_phy;
    ushort	pcm_state;
    leveltype	level;
    
    pcm_state = get_pc_state (hwidb, entity);
    if ((pcm_state == PC_OFF) || (pcm_state == PC_BREAK) ||
        (pcm_state == PC_CONNECT))
        return (PT_none);

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
    if (entity)
	which_phy = FCI_PHYB_RVAL;
    else
	which_phy = FCI_PHYA_RVAL;
    send_fip_get_rval_cmd(hwidb, which_phy, &pc_rval);
    if (pc_rval & 0x002)
	pc_neighbor = (pc_rval & 0x004) ? PT_m : PT_s;
    else
	pc_neighbor = (pc_rval & 0x004) ? PT_b : PT_a;
    reset_interrupt_level(level);
    return(pc_neighbor);
}

/*
 * get_pc_withhold
 *
 * Returns the XDI encoded PC_Withhold reason code as read from
 * the microcode.
 */
ushort get_pc_withhold (hwidbtype *hwidb, ushort entity)
{
    ushort	pc_withhold;
    ushort	pc_rval;
    ushort	which_phy;
    leveltype	level;
    
    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
    if (entity)
	which_phy = FCI_PHYB_RVAL;
    else
	which_phy = FCI_PHYA_RVAL;
    send_fip_get_rval_cmd(hwidb, which_phy, &pc_rval);
    if (pc_rval & 0x008)
	pc_withhold = PCW_otherincompatible;
    else
	pc_withhold = PCW_none;
    reset_interrupt_level(level);
    return(pc_withhold);
}

/*
 * get_pc_conn_state
 *
 * Returns the XDI encoded PCM connect state as read from
 * the microcode.
 */
ushort get_pc_conn_state (hwidbtype *hwidb, ushort entity)
{
    ushort conn_state = CS_disabled;

    switch (get_pc_state (hwidb, entity)) {
    case PC_OFF:
    case PC_MAINT:
	break;

    case PC_BREAK:
    case PC_CONNECT:
    case PC_JOIN:
    case PC_VERIFY:
	conn_state = CS_connecting;
	break;

    case PC_NEXT:
    case PC_SIGNAL:
	conn_state = CS_connecting;
	if (entity == CSPDATA (hwidb->fddi_stn_id, stationData.A_Index)) {
	    /* Check for Standby condition (Phy A only) */
	    if ((get_pc_neighbor (hwidb, 0) == PT_m) ||
		(get_pc_neighbor (hwidb, 1) == PT_m))
	        conn_state = CS_standby;
	}
	break;

    case PC_ACTIVE:
    case PC_TRACE:
	conn_state = CS_active;
	break;
    }

    return (conn_state);
}

/*
 * get_pc_rval
 *
 * Returns the Pseudo Code bits as read from the microcode.
 */
static ushort get_pc_rval (hwidbtype *hwidb, ushort entity)
{
    ushort	pc_rval;
    ushort	which_phy;
    leveltype	level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
    if (entity)
	which_phy = FCI_PHYB_RVAL;
    else
	which_phy = FCI_PHYA_RVAL;
    send_fip_get_rval_cmd(hwidb, which_phy, &pc_rval);
    reset_interrupt_level(level);
    return (pc_rval);
}

/*
 * fddi_interface
 * A function to display some unique fddi type information.
 */
void fddi_interface (hwidbtype *hwidb)
{
    int indx;
    phytype *phy;
    uchar una[6],dna[6];  /* bit swapped upstream & downstream neighbors */
    fddisb  *fsb;
    srb_vector_table_t *srbV_p;

    fsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    /*
     * Read the status of the current FDDI error counters.
     */
    (void) get_cbus_errors(hwidb);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        idbtype *swidb;
        srb_triplet_t srb_triplet;
    
        swidb = firstsw_or_null(hwidb);
        srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
        if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
	    printf("\n  Source bridging enabled, srn %d bn %d trn %d %s",
	        srb_triplet.localRing,
                srb_triplet.bridgeNum,
	        srb_triplet.remoteRing,
	        (hwidb->tr_vring_blk ? "(ring group)" : ""));
	    printf("\n    spanning explorer %sabled",
	        (srbV_p->srb_sb_get_spanning_explorer(swidb) ? "en" : "dis"));
	    automore_conditional(0);
        }
    }

    for (indx = PHY_A; indx < (PHY_B + 1); indx++) {
	phy = (phytype *)STADATA (hwidb->fddi_stn_id, portData[indx]);

	/* 
	 * Display the status of the PHYs.
	 */

    get_phy_pc_status(hwidb,indx);
	printf("\n  Phy-%c state is %-7s, neighbor is %-3s, cmt signal bits %-03x/%-03x, status %3s",
	       phy->type, get_phy_pc_status(hwidb, indx), 
	       get_phy_neighbor(hwidb, indx),
	       phy->tval, phy->rval, get_phy_status(hwidb, indx));
    }
    printf("\n  ECM is %s, CFM is %s, RMT is %s",
	   get_smt_string(&SMTMsg.ec_state,
			  MIBDATA (hwidb->fddi_stn_id, mib->smtData.ECMState)),
	   get_smt_string(&SMTMsg.cf_state,
			  MIBDATA (hwidb->fddi_stn_id, mib->smtData.CF_State)),
	   get_smt_string(&SMTMsg.rm_state,
			  MIBDATA (hwidb->fddi_stn_id,
				   mib->macBase[0].hwData.RMTState)));
    printf("\n  Requested token rotation %u usec, negotiated %u usec",
            fsb->fddi_trt, fsb->fddi_trt_neg);
    printf("\n  Configured tvx is %u usec", fsb->fddi_tvx);
    if (!TIMER_RUNNING(fsb->fddi_operational))
	printf(" ring not operational");
    else {
	printf(" ring operational ");
	print_dhms(fsb->fddi_operational);
    }
    ieee_swap(MIBDATA (hwidb->fddi_stn_id, mib->macBase[0].hwData.UpstreamNbr),
	      una);
    ieee_swap(MIBDATA (hwidb->fddi_stn_id,
		       mib->macBase[0].hwData.DownstreamNbr), dna);
    printf("\n  Upstream neighbor %e, downstream neighbor %e", una, dna);
}

/*
 * show_fddi
 * A function to display the formac and FDDI registers.
 */

static boolean show_fddi (hwidbtype *hwidb, parseinfo *csb)
{
    phytype	*phy_a, *phy_b;
    int		indx;
    ushort	status;
    ushort	endec_state;
    ushort	regs[64];
    leveltype	level;
    
    if (hwidb->status & IDB_NOHARDWARE)
	return(TRUE);
    printf("\n%s - ", hwidb->hw_namestring);
	
    /*
     * Print out the hardware and micro-code versions.
     */
    fddi_print_versions(hwidb);
	
    /*
     * Select the interface.
     */	
    level = raise_interrupt_level(NETS_DISABLE);

    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	reset_interrupt_level(level);
	return(TRUE);
    }

    /*
     * Read ENDEC A registers.
     */
    for (indx = 0; indx < 5; indx++) {
	select_endec(hwidb, indx, ENDECA);
	send_fip_en_read_cmd(hwidb, &endec_state);
	regs[indx] = (endec_state & 0x7);
    }
    reset_interrupt_level(level);
    printf("\n  Phy-A registers:\n    cr0 %-01x, cr1 %-01x, cr2 %-01x, status %-01x, cr3 %-01x", 
	   regs[0], regs[1], regs[2], regs[3], regs[4]);
	
    /*
     * Read ENDEC B register.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	reset_interrupt_level(level);
	return(TRUE);
    }

    for (indx = 0; indx < 5; indx++) {
	select_endec(hwidb, indx, ENDECB);
	send_fip_en_read_cmd(hwidb, &endec_state);
	regs[indx] = (endec_state & 0x7);
    }
    reset_interrupt_level(level);	    
    printf("\n  Phy-B registers:\n    cr0 %-01x, cr1 %-01x, cr2 %-01x, status %-01x, cr3 %-01x",
	   regs[0], regs[1], regs[2], regs[3], regs[4]);

    /*
     * Read the FORMAC registers.
     */	
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	reset_interrupt_level(level);
	return(TRUE);
    }
    for (indx = 0; indx < 16; indx++) {
	regs[indx] = 0xFFFF;
	send_fip_fm_select_cmd(hwidb, indx);

	status = send_fip_fm_read_cmd(hwidb, &regs[indx]);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
		   status, "fm read");
	    reset_interrupt_level(level);
	    return(TRUE);
	}
    }
    reset_interrupt_level(level);
    printf("\n  FORMAC registers:");
    printf("\n    irdtlb  %-04x, irdtneg %-04x, irdthtt %-04x, irdmir  %-04x%04x",	
	   regs[0], regs[1], regs[4], regs[2], regs[3]);
    printf("\n    irdtrth %-04x, irdtmax %-04x, irdtvxt %-04x, irdstmc %-04x", 
	   regs[5], regs[6], regs[8], regs[9]);
    printf("\n    irdmode %-04x, irdimsk %-04x, irdstat %-04x, irdtpri %-04x",
	   regs[10], regs[11], regs[12], regs[15]);
	
    for (indx = 0; indx < 64; indx++) {
	regs[indx] = 0xFFFF;
	level = raise_interrupt_level(NETS_DISABLE);
	status = select_if(hwidb);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
		   status, "select");
	    reset_interrupt_level(level);
	    return(TRUE);
	}
	send_reg_select_cmd(hwidb, indx);
	send_reg_read_cmd(hwidb, &regs[indx]);
	reset_interrupt_level(level);
    }

    phy_a = (phytype *)STADATA (hwidb->fddi_stn_id, portData[PHY_A]);
    phy_b = (phytype *)STADATA (hwidb->fddi_stn_id, portData[PHY_B]);

    fddi_print_ip_regs(hwidb, regs);

    printf("\n Total LEM: phy-a %lu, phy-b %lu",
	   MIBDATA (hwidb->fddi_stn_id, mib->portBase[PHY_A].hwData.Lem_Ct),
	   MIBDATA (hwidb->fddi_stn_id, mib->portBase[PHY_B].hwData.Lem_Ct));
    return(FALSE);
}

/*
 * fddi_config_commands
 * Parse the commands following the keyword fddi.
 * Current supported interface specific commands:
 *
 * 	fddi token-rotation-time <microsecond value>
 *	fddi cmt-signal-bits <value> [<phy-a | phy-b>]
 *      fddi valid-transmission-time <microsecond value>
 *      fddi tl-min-time <microsecond value>
 * 	fddi c-min <microsecond value>
 *	fddi tb-min <microsecond value>
 * 	fddi duplicate-address-check
 * 	fddi smt-frames
 * 	fddi if-cmt
 */
void fddi_config_commands (parseinfo *csb)
{
    hwidbtype	*hwidb;
    phytype	*phy;
    paktype	*pak;
    leveltype	level;
    fddisb *fsb;

    hwidb = hwidb_or_null(csb->interface);
    if (!hwidb)
	return;

    if (!(hwidb->status & IDB_FDDI)) {
	if (!csb->nvgen)
	    printf("\n%%Only allowed on Fddi interfaces");
	return;
    }

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    if (csb->nvgen) {
	switch (csb->which) {
	  case FDDI_INTFC_TRT:
	    nv_write((fsb->fddi_trt != DEFAULT_TRT_TIME),
		     "%s %u", csb->nv_command, fsb->fddi_trt);
	    break;
	  case FDDI_INTFC_CMT_S_BITS:
	    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_A]);
	    nv_write((phy->tval != DEFAULT_PHY_A_TVAL),
		     "%s 0x%-02x phy-a", csb->nv_command,
		     phy->tval);
	    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_B]);
	    nv_write((phy->tval != DEFAULT_PHY_B_TVAL),
		     "%s 0x%-02x phy-b", csb->nv_command,
		     phy->tval);
	    break;
	  case FDDI_INTFC_TVX:
	    nv_write((fsb->fddi_tvx != DEFAULT_T_TVX_TIME),
		     "%s %u", csb->nv_command, fsb->fddi_tvx);
	    break;
	  case FDDI_INTFC_TL_MIN:
	    nv_write((STADATA (hwidb->fddi_stn_id, TLMin) != TL_MIN),
		     "%s %u", csb->nv_command,
		     STADATA (hwidb->fddi_stn_id, TLMin));
	    break;
	  case FDDI_INTFC_C_MIN:
	    nv_write((STADATA (hwidb->fddi_stn_id, CMin) != 1600),
		     "%s %u", csb->nv_command,
		     STADATA (hwidb->fddi_stn_id, CMin));
	    break;
	  case FDDI_INTFC_TB_MIN:
	    nv_write((STADATA (hwidb->fddi_stn_id, TBMin) != (TB_MIN * 1000)),
		     "%s %u", csb->nv_command,
		     STADATA (hwidb->fddi_stn_id, TBMin));
	    break;
	  case FDDI_INTFC_T_OUT:
	    nv_write((STADATA (hwidb->fddi_stn_id, TOut) != T_OUT),
		     "%s %u", csb->nv_command,
		     STADATA (hwidb->fddi_stn_id, TOut));
	    break;
	  case FDDI_INTFC_DUP_CHECK:
            if ((hwidb->fddi_options & FDDI_DUPLICATE_ADDRESS_CHK) &&
                (hwidb->fddi_options & FDDI_FAST_CMT_ENB))
	         nv_write((hwidb->fddi_options & FDDI_DUPLICATE_ADDRESS_CHK),
		     "%s", csb->nv_command);
	    break;
	  case FDDI_INTFC_SMT_FRAMES:
	    nv_write((!(hwidb->fddi_options & FDDI_SMT_FRAMES)),
		     "no %s", csb->nv_command);
	    break;
	  case FDDI_INTFC_BURST_COUNT:
	    nv_write((fsb->fddi_burst_count != FDDI_DEFAULT_BURST_COUNT),
		     "%s %u", csb->nv_command,
		     fsb->fddi_burst_count);
	    break;
	  case FDDI_INTFC_ENCAP:
	    if (hwidb->type == IDBTYPE_FDDIT) {
		if (hwidb->fddi_options & FDDI_ENCAPSULATE)
		    nv_write(TRUE, csb->nv_command);
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;
    }


    switch (csb->which) {

      case FDDI_INTFC_TRT:
	if (csb->sense) {
	    fsb->fddi_trt = GETOBJ(int,1);
	} else {
	    fsb->fddi_trt = DEFAULT_TRT_TIME;
	}
	break;
	
      case FDDI_INTFC_CMT_S_BITS:
	if (GETOBJ(int,3) & FDDI_PHY_A) {
	    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_A]);
	    if (csb->sense) {
		phy->tval = GETOBJ(int,1);
	    } else {
		phy->tval = DEFAULT_PHY_A_TVAL;
	    }
	}
	if (GETOBJ(int,3) & FDDI_PHY_B) {
	    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[PHY_B]);
	    if (csb->sense) {
		phy->tval = GETOBJ(int,1);
	    } else {
		phy->tval = DEFAULT_PHY_B_TVAL;
	    }
	}
	break;
      case FDDI_INTFC_TVX:
	if (csb->sense) {
	    fsb->fddi_tvx = GETOBJ(int,1);
	} else {
	    fsb->fddi_tvx = DEFAULT_T_TVX_TIME;
	}
	break;
      case FDDI_INTFC_TL_MIN:
	if (csb->sense) {
	    STADATA (hwidb->fddi_stn_id, TLMin) = GETOBJ(int,1);
	} else {
	    STADATA (hwidb->fddi_stn_id, TLMin) = TL_MIN;
	}
        if (hwidb->fddi_options & FDDI_FAST_CMT) {
	    cbus_daughter_cmd(hwidb, FCI_FDDI_SET_TL_MIN, 
			      (STADATA (hwidb->fddi_stn_id, TLMin) ?
			       STADATA (hwidb->fddi_stn_id, TLMin)/20 : 0));
	}
	return;
      case FDDI_INTFC_C_MIN:
	if (csb->sense) { /* microseconds */
	    STADATA (hwidb->fddi_stn_id, CMin) = GETOBJ(int,1);
	} else {
	    STADATA (hwidb->fddi_stn_id, CMin) = 1600;
	}
	return;
	
      case FDDI_INTFC_TB_MIN:
	if (csb->sense)  /* microseconds */
	    STADATA (hwidb->fddi_stn_id, TBMin) = GETOBJ(int,1);
	else
	    STADATA (hwidb->fddi_stn_id, TBMin) = TB_MIN * 1000; 
        return;
	
      case FDDI_INTFC_T_OUT:
	if (csb->sense) 
	    STADATA (hwidb->fddi_stn_id, TOut) = GETOBJ(int,1);
	else
	    STADATA (hwidb->fddi_stn_id, TOut) = T_OUT;	/* milliseconds */
	return;
    case FDDI_INTFC_DUP_CHECK:
	if (hwidb->fddi_options & FDDI_FAST_CMT_ENB) {
	  if (!csb->sense)
	    hwidb->fddi_options &= ~FDDI_DUPLICATE_ADDRESS_CHK;
	  else 
	    hwidb->fddi_options |= FDDI_DUPLICATE_ADDRESS_CHK;
        } else {
             printf("\nDuplicate-address-check is not supported for %s.",
		    hwidb->hw_namestring);
             return;}
	if (hwidb->fddi_options & FDDI_VERSION_128) {
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	    return;
	}
	break;
      case FDDI_INTFC_SMT_FRAMES:
	level = raise_interrupt_level(NETS_DISABLE);
	if (!csb->sense) {
	    hwidb->fddi_options &= ~FDDI_SMT_FRAMES;

	    /*
	     * If all the interfaces are disabled for SMT frames -- kill
	     * the process that handles SMT frames and flush the queue.
	     */
	    if (smt_process_id) {
		if (fddi_smt_enabled_any_intfc(hwidb)) {
		    reset_interrupt_level(level);
		    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
		    return;
		}

		/*
		 * Kill the running SMT frame process.
		 */
		reset_interrupt_level(level);
		process_kill(smt_process_id);
		level = raise_interrupt_level(NETS_DISABLE);
		smt_process_id = 0;
		while ((pak = dequeue(&smtinputQ))) {
		    retbuffer(pak);
		}
	    }
	} else {
	    hwidb->fddi_options |= FDDI_SMT_FRAMES;
	    if (!smt_process_id) {
		queue_init(&smtinputQ, nfddinets);
		reset_interrupt_level(level);
		smt_process_id = process_create(fddi_smt_input, "SMT input",
						NORMAL_STACK,  PRIO_NORMAL);
		idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
		return;
	    }
	}
	reset_interrupt_level(level);
	return;
      case FDDI_INTFC_BURST_COUNT:
	if (csb->sense) 
	    fsb->fddi_burst_count = GETOBJ(int,1);
	else
	    fsb->fddi_burst_count = FDDI_DEFAULT_BURST_COUNT;
	if (hwidb->fddi_options & FDDI_VERSION_128) {
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	    return;
	}
	break;
      case FDDI_INTFC_ENCAP:
	/*
	 * Succeeding in changing the encapsulation mode flushes
	 * this ports bridging entries from the bridge cache if
	 * any.  We also reset after all of this to clear the CAM
	 * and reset the AF to be appropriate for the current
	 * bridging state.
	 */
	level = raise_interrupt_level(NETS_DISABLE);
	if (!csb->sense) {
	    hwidb->fddi_options &= ~FDDI_ENCAPSULATE;
	    hwidb->span_macoffset = FDDI_RECEIVE_OFFSET + 1;
	    hwidb->span_encapsize = FDDI_FC;
	    hwidb->bridge_transit = FALSE;
	    hwidb->firstsw->tbridge_media = TBR_FDDI;
	    hwidb->tbridge_on_hwidb = TRUE;
	} else {
	    hwidb->fddi_options |= FDDI_ENCAPSULATE;
	    hwidb->span_macoffset = FDDI_RECEIVE_OFFSET + 
		((FDDI_SNAP_ENCAPBYTES >> 1) + 1);
	    hwidb->span_encapsize = FDDI_SNAP_ENCAPBYTES;	/* transit */
	    if (RUNNING_IF(LINK_BRIDGE, hwidb->firstsw))
		hwidb->bridge_transit = TRUE;
	    hwidb->firstsw->tbridge_media = TBR_FDDI_TRANSIT;
	    hwidb->tbridge_on_hwidb = TRUE;
	}
	/*
	 * If in a bridging group, clear the bridge cache of any entries.
	 */
	reg_invoke_bridge_clear_interface(hwidb->firstsw);
	(*hwidb->reset)(hwidb);
	if (hwidb->counters.resets > 0)
	    hwidb->counters.resets--;
	reset_interrupt_level(level);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;

      default:
	bad_parser_subcommand(csb, csb->which);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;
    }
    (*hwidb->reset)(hwidb);
    if (hwidb->counters.resets > 0)
	hwidb->counters.resets--;

    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}

/*
 * Provide an external entry point for process-level
 * invocations of the corresponding inline function.
 */

static void hes_fddi_cam_depopulate (hwidbtype *hwidb,
				     uchar *mac_address,
				     hwidbtype *input_hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    hes_fddi_cam_depopulate_inline(hwidb, mac_address, input_hwidb);
    reset_interrupt_level(level);
}

static void hes_fddi_cam_populate (hwidbtype *hwidb, uchar *mac_address)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    hes_fddi_cam_populate_inline(hwidb, mac_address);
    reset_interrupt_level(level);
}


/*
 * Handle CSP Timer time-outs notifications.
 * This is the callback function for the CSP Timer Process.
 *
 * The Process can NOT call ServiceCSPTimer() direclty since
 * ProcessCSP() has to be called also to process any signals
 * created by ServiceCSPTimer().
 */
static void fddi_csp_timer_callback (void)
{
    leveltype level;

    /* Check for CSP time-outs  */
    ServiceCSPTimer ();

    /* Process CSP signals (if any) */
    if (!EmptySignalQueue()) {
        MDisableCSPInterrupts(&level);
        ProcessCSP(&level);
	MRestoreCSPInterrupts(&level);
    }
}

 

/*
 * Scheduler test for blocking until we have a CMT time-out
 */
static boolean fddi_cmt_timer_BLOCK (void)
{
    return (!mgd_timer_expired(&CSPTimer));
}
 

/*
 * Process input FDDI SMT frames as a normal priority process.
 */
forktype fddi_cmt_timeout (void)
{
    while (TRUE) {
 
        /* Sleep until there is a CMT timer time-out */
        edisms((blockproc *)fddi_cmt_timer_BLOCK, 0);
 
#if DEBUG_ALL
        bprintf("\cmt_timer time-out\n");
#endif
	/* Be paranoid. */

	if (mgd_timer_expired(&CSPTimer)) {
	    mgd_timer_stop(&CSPTimer);
	    fddi_csp_timer_callback();
	}
    }
}


/*
 * Process High-End microcode pseudo-code and update related MIB variables.
 */
static void fddi_process_pcode (ushort stnID, struct CSPPORTInfo *portPtr)
{
    /*
     * Update PC_Neighbor.
     */
    if (MReadBit (portPtr->R_Val, 1)) {
        if (MReadBit (portPtr->R_Val, 2))
            portPtr->PC_Neighbor = PT_m;
        else
            portPtr->PC_Neighbor = PT_s;
    } else {
        if (MReadBit (portPtr->R_Val, 2))
            portPtr->PC_Neighbor = PT_b;
        else
            portPtr->PC_Neighbor = PT_a;
    }
    SendCSPEvent (stnID, fddiPORTNeighborType, portPtr->ID);

    /*
     * Update PC_Mode and PC_Withhold.
     *
     * Removed the code where we check for the case when
     * we could act as M port.
     */

     if (portPtr->PC_Type == PT_m || portPtr->PC_Neighbor == PT_m)
         portPtr->PC_Mode = PC_Mode_Tree;
     else
         portPtr->PC_Mode = PC_Mode_Peer;

     /* reevaluate selection criteria */
     CFMPortSelectionCriteria (stnID, portPtr);

     if (portPtr->WC_Flag)
         portPtr->PC_Withhold = PCW_pathnotavailable;
     else
         portPtr->PC_Withhold = PCW_none;
     SendCSPEvent (stnID, fddiPORTPC_Withhold, portPtr->ID);
}


/*
 * Handle all FDDI SMT related love notes.
 */
void fddi_smt_love_note_handler (hwidbtype *hwidb, ushort status)
{
    leveltype level;
    ushort stnID, sigEntity;
    struct CSPPORTInfo *portPtr;

    /* Initialize local variables */
    stnID = hwidb->fddi_stn_id;

    /*
     * Check for CMT love notes.
     */
    if (status & FCI_COND_FDDI_PHYA_JOIN) {

	if (fddi_debug)
	    buginf("\n JOIN_A 0x%-04x %s", status, hwidb->hw_namestring);

	/*
	 * Send A:CF_Join signal to CMT and start LEM monitoring.
	 */
	sigEntity = CSPDATA (stnID, stationData.A_Index);
	SendSignalMulti (stnID, SIG_CF_Join, sigEntity, (uInt32) SET);
	SendSignalMulti (stnID, SIG_LEM_Start, sigEntity , (uInt32) 0);
	/*
	 * Update SMT CSP database R_Val value.
	 */
	CSPDATA (stnID, portData[sigEntity].R_Val) =
	    (hwidb->phy[PHY_A])->rval = get_pc_rval (hwidb, PHY_A);
	SendCSPEvent (stnID, fddiPORTMACIndicated, sigEntity);
	/*
	 * Process pseudo code bits.
	 */
	portPtr = &(CSPDATA (stnID, portData[sigEntity]));
	fddi_process_pcode (stnID, portPtr);
    }

    if (status & FCI_COND_FDDI_PHYB_JOIN) {

       if (fddi_debug)
           buginf("\n JOIN_B 0x%-04x %s", status, hwidb->hw_namestring);

	/*
	 * Send B:CF_Join signal to CMT and start LEM monitoring.
	 */
	sigEntity = CSPDATA (stnID, stationData.B_Index);
	SendSignalMulti (stnID, SIG_CF_Join, sigEntity, (uInt32) SET);
	SendSignalMulti (stnID, SIG_LEM_Start, sigEntity , (uInt32) 0);
	/*
	 * Update SMT CSP database R_Val value.
	 */
        CSPDATA (stnID, portData[sigEntity].R_Val) =
	    (hwidb->phy[PHY_B])->rval = get_pc_rval (hwidb, PHY_B);
        SendCSPEvent (stnID, fddiPORTMACIndicated, sigEntity);
	/*
	 * Process pseudo code bits.
	 */
        portPtr = &(CSPDATA (stnID, portData[sigEntity]));
	fddi_process_pcode (stnID, portPtr);
    }

    if ((status & FCI_COND_FDDI_DUP_ADDRESS) && MACInterruptsEnabled ) {

       if (fddi_debug)
           buginf("\n DUP_ADDRESS 0x%-04x %s", status, hwidb->hw_namestring);

        /*
         * Indicate that MAC entered the Beacon state (set BN_Flag).
         */
        SendSignalMulti (stnID, SIG_MAC_Interrupt,
            0, (uInt32) MAC_TBid_TReq_Mismatch);
    }

    if (status & FCI_COND_FDDI_PHYA_SEND_BREAK) {

       if (fddi_debug)
           buginf("\n BREAK_A 0x%-04x %s", status, hwidb->hw_namestring);

        /*
         * Send !A:CF_Join signal to CMT and stop LEM.
         */
	sigEntity = CSPDATA (stnID, stationData.A_Index);
        SendSignalMulti (stnID, SIG_CF_Join, sigEntity, (uInt32) CLEAR);
	SendSignalMulti (stnID, SIG_LEM_Stop, sigEntity , (uInt32) 0);

    /*
     * Update SMT CSP database R_Val value.
     */

    CSPDATA (stnID, portData[sigEntity].R_Val) =
	    (hwidb->phy[PHY_A])->rval = 0;
    SendCSPEvent (stnID, fddiPORTMACIndicated, sigEntity);

    }

    if (status & FCI_COND_FDDI_PHYB_SEND_BREAK) {

       if (fddi_debug)
           buginf("\n BREAK_B 0x%-04x %s", status, hwidb->hw_namestring);

        /*
         * Send !B:CF_Join signal to CMT and stop LEM.
         */
	sigEntity = CSPDATA (stnID, stationData.B_Index);
        SendSignalMulti (stnID, SIG_CF_Join, sigEntity, (uInt32) CLEAR);
	SendSignalMulti (stnID, SIG_LEM_Stop, sigEntity , (uInt32) 0);

    /*
     * Update SMT CSP database R_Val value.
     */

    CSPDATA (stnID, portData[sigEntity].R_Val) =
	    (hwidb->phy[PHY_B])->rval = 0;
    SendCSPEvent (stnID, fddiPORTMACIndicated, sigEntity);
    }

    if ((status & (FCI_COND_FDDI_PHYA_SEND_BREAK |
		   FCI_COND_FDDI_PHYB_SEND_BREAK)) &&
	(CSPDATA (stnID, stationData.ecState) == EC_TRACE)) {
	/*
	 * Emulate Path_Test = Pending signal to
	 * indicate end of tracing to ECM and allow the
	 * beginning of the station Path Test.
	 */
	SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Pending);
    }

    if (!EmptySignalQueue()) {
	/*
	 * Process CSP signals.
	 */
        level = raise_interrupt_level(NETS_DISABLE);
        ProcessCSP(&level);
        reset_interrupt_level(level);
    }
}


/* fddi_system_configured
 *
 * IDB callback executed when the NVRAM is parsed and the system is coming up
 */ 
void fddi_system_configured (hwidbtype *hwidb)
{
    /*
     * Reset the interface and reinitialize CMT if interface not shutdown
     */
    TIMER_STOP(hwidb->xmittimer);
    (*hwidb->reset)(hwidb);
    if (hwidb->counters.resets > 0)
      hwidb->counters.resets--;
}


/*
 * cbus_fddi_address
 * Set filter for a particular FDDI address.
 *
 * This functions should only be called with canonical addresses (ptr)!
 * The original FDDI card sees addresses in native FDDI order (bit_swapped)
 * and the AF must be programmed accordingly.  The new FDDI-T has h/w
 * that makes all MAC addresses (in the MAC header) look canonical to the
 * rest of the system.  So its address filter must be set accordingly.
 *
 * The original FDDI only has a 6 byte address filter, enough for DA only.
 * The FDDI-T has 32 bytes and starts with the FC byte.  The data in all
 * cases is left aligned (MSB) of the 16 bit word.  To overlap as much
 * as possible we make the first byte of DA be 0 and reference the FC byte
 * as 0xffff.
 */
static void cbus_fddi_address (hwidbtype *hwidb, int fsm, uchar *ptr)
{
    ushort data, newdata, uaddr;
    int indx, fddi_indx;
    uchar uc;

    newdata = ((1 << fsm) << 8);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (DA) MCI_CMD_AF_WRITE %04x",
	       hwidb->hw_namestring, newdata);
#endif	/* TBRIDGE_DEBUG */

    if (*ptr & 0x01)
	hwidb->multibits |= newdata;
    for (indx = 0; indx < IEEEBYTES; indx++) {
	uc = *ptr++;
	fddi_indx = indx;

	uaddr = (fddi_indx << 8) | uc;
	send_af_select_cmd(hwidb, uaddr);
	send_af_read_cmd(hwidb, &data);
	data |= newdata;
	send_af_select_cmd(hwidb, uaddr);
	send_af_write_cmd(hwidb, data);
    }
}

/*
 * cbus_record_fddi_address
 */

static void cbus_record_fddi_address (
    idbtype *swidb,
    uchar *address,
    uchar fsm)
{
    hwidbtype	*hwidb;
    ushort	status;
    leveltype	level;
    
    hwidb = swidb->hwptr;
    level = raise_interrupt_level(NETS_DISABLE);

    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x), select, interface %s - cbus_record_fddi_address()",
		   status, hwidb->hw_namestring);
	reset_interrupt_level(level);
	return;
    }
    cbus_fddi_address(hwidb, fsm, address);
    reset_interrupt_level(level);
}


/*
 * cbus_fddi_address_range
 *
 * Register multicast address ranges.
 */
static void cbus_fddi_address_range (hwidbtype *hwidb, int fsm,
				     uchar const *ptr, uchar const *mask)
{
    int		indx, fddi_indx;
    int		j;
    ushort	data, newdata, uaddr;
    uchar	uc;

    newdata = ((1 << fsm) << 8);
    if ((*ptr & ~*mask) & 0x01)
	hwidb->multibits |= newdata;
    for (indx = 0; indx < IEEEBYTES; indx++, mask++) {
	uc = *ptr++;
	fddi_indx = indx;

	if (*mask) {
	    for (j = 0; j < 255; j++) {
		if ((j & ~(*mask)) != uc)
		    continue;
		send_af_select_cmd(hwidb, ((fddi_indx << 8) | j ));
		send_af_read_cmd(hwidb, &data);
		data |= newdata;
		send_af_select_cmd(hwidb, ((fddi_indx << 8) | j ));
		send_af_write_cmd(hwidb, data);
	    }
	} else {
	    uaddr = (fddi_indx << 8) | uc;
	    send_af_select_cmd(hwidb, uaddr);
	    send_af_read_cmd(hwidb, &data);
	    data |= newdata;
	    send_af_select_cmd(hwidb, uaddr);
	    send_af_write_cmd(hwidb, data);
	}
    }
}


/*
 * cbus_record_fddi_address_range
 */
static void cbus_record_fddi_address_range (idbtype *swidb, uchar *address,
					    uchar *mask)
{

    hwidbtype	*hwidb;
    ushort	status;
    leveltype	level;
    
    hwidb = swidb->hwptr;
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	if (fddi_cmt_events_debug)
	    buginf("\nError: (%-04x), select, interface %s - cbus_record_fddi_address_range()",
		   status, hwidb->hw_namestring);
	reset_interrupt_level(level);
	return;
    }
    cbus_fddi_address_range(hwidb, FSM_BROADCAST, address, mask);
    reset_interrupt_level(level);
}


/*
 * hes_fddi_idb_init
 *
 * Initialize IDB with default values for FDDI media.
 */
void hes_fddi_idb_init (
    hwidbtype *hwidb)
{
    int indx;
    phytype *phy;
    StnDCBType *smt;
    fddisb *fsb;

    fddi_idb_init(hwidb);

    hwidb->listen = cbus_record_fddi_address;
    hwidb->listen_range = cbus_record_fddi_address_range;
    /* FDDI_BUFFERSIZE_WORDS requires if_mci.h XXX */
    hwidb->buffersize = (FDDI_BUFFERSIZE_WORDS << 1);

    /* Allow fddi to use mtu command for buffer tuning */
    hwidb->set_maxdgram = cbus_mci_set_maxdgram;

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    /* Initialize SMT data structures - only if not done yet */
    if (!STADATA (nfddinets - 1, hwPresent)) {
        hwidb->fddi_stn_id = nfddinets - 1;
        smt = &STAINST (hwidb->fddi_stn_id);
        smt->smtCfg = &DASSMTCfg;
        smt->macCfg = &SingleFORMACMACCfg;
        smt->pathCfg = &DASPATHCfg;
        smt->portCfg = &MMDASPORTCfg;
        smt->hwIDB[0] = hwidb;
        smt->hwPresent = TRUE;

        /* Startup XDI SMT code if first FDDI interface */
        if (nfddinets == 1) {
	    fddi_smtqsize = 2;		/* Number SMT bufs per interface */
	    MIBStartup ();
	    CSPStartup ();
	    FBMStartup ();
        }

        /* Initialize XDI SMT interface code */
        if (MIBMainMulti(hwidb->fddi_stn_id)) {
            errmsg(&msgsym(NOMEMORY, FDDI), hwidb->fddi_stn_id,
		   "MIBMainMulti");
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
            return;
        }
        if (FBMMainMulti(hwidb->fddi_stn_id)) {
            errmsg(&msgsym(NOMEMORY, FDDI), hwidb->fddi_stn_id,
		   "FBMMainMulti");
            STADATA (hwidb->fddi_stn_id, smtUp) = FALSE;
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
            return;
        }
        smt->smtUp = TRUE;
        smt->TLMin = TL_MIN;
        smt->CMin = 1600;
        smt->TBMin = TB_MIN * 1000;
        smt->TOut = T_OUT;
    }

    /*  
     * Default paramters.
     */
    fsb->fddi_trt = DEFAULT_TRT_TIME;
    fsb->fddi_tvx = DEFAULT_T_TVX_TIME;
    hwidb->fddi_options = FDDI_SMT_FRAMES;

    fddi_set_options(hwidb);

    fsb->fddi_burst_count = FDDI_DEFAULT_BURST_COUNT;

    /*
     * Allocate the phy memory structure -- used by CMT/PCM.
     */
    for (indx = 0; indx < 2; indx++) {
	phy = hes_get_phy(hwidb, indx);
	if (phy) {
	    phy->connect = TRUE;
 	    phy->idb = hwidb;
	    phy->regptr = hwidb->mci_regptr;
	    phy->tval = DEFAULT_PHY_A_TVAL;
	    phy->type = 'A';
	    if (ip_on_cbus2(hwidb))
		phy->status_offset = hwidb->cbus_icb_address +
		    CBUS2_ICB_SIZE + 1;
	    else
		phy->status_offset = hwidb->cbus_icb_address + 
		    CBUS1_ICB_SIZE + 1;
	    if (indx == 1) {
		phy->type = 'B';
		phy->tval = DEFAULT_PHY_B_TVAL;
		if (ip_on_cbus2(hwidb))
		    phy->status_offset = hwidb->cbus_icb_address +
			CBUS2_ICB_SIZE + 2;
		else
		    phy->status_offset = hwidb->cbus_icb_address + 
			CBUS1_ICB_SIZE + 2;
	    }
	    STADATA (hwidb->fddi_stn_id, portData[indx]) = phy;
	    hwidb->phy[indx] = phy;
	}
    }
    hwidb->show_controller = show_fddi;
    hwidb->tr_srb_fastswitch = (tr_srb_fastswitch_type) return_false;
    hwidb->srb_routecache = TRUE;
    reg_add_media_delete_hardware_address_filter(hes_fddi_cam_depopulate,
						 "hes_fddi_cam_depopulate");
    reg_add_media_add_hardware_address_filter(hes_fddi_cam_populate,
					         "hes_fddi_cam_populate");
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}


/*
 * Set the hold register correctly for the AGS+ and 7000 platforms:
 */

void hes_fddi_set_hold_reg (hwidbtype *hwidb, int byte_rxoffset)
{
    ushort	hold;
    ushort	status;
    leveltype	level;
    idbtype     *swidb;

    level = raise_interrupt_level(NETS_DISABLE);

    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	platform_req_interface_restart(hwidb);
	reset_interrupt_level(level);
	return;
    }
    hold = 0;
    if (hwidb->type == IDBTYPE_FDDIT) {
	/*
	 * Enable FDDI stripping on the hwidb 
         *   if any swidb has bridging configured, or HSRP.
	 */
        srb_vector_table_t *srbV_p;
        srbV_p = reg_invoke_srb_get_vector_table();
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
            boolean srb_enabled_for_fddi;
            if (srbV_p) {
              srb_enabled_for_fddi = srbV_p->srb_sb_get_enabled_for_fddi(swidb);
            } else {
              srb_enabled_for_fddi = FALSE;
            }
	    if (RUNNING_IF(LINK_BRIDGE, swidb) || 
                standby_if_mincfg(swidb) || srb_enabled_for_fddi) {
	            hold = FDDIT_FCTL_BRIDGE;
	            break;
	    }
        }
    }

    hold |= (byte_rxoffset & 0x3);
    send_fip_hold_reg_cmd(hwidb, hold);
    reset_interrupt_level(level);
}


/*
 * fddi_carrier
 * Called from interrupt level only.
 */
void fddi_carrier (hwidbtype *hwidb)
{
    ulong	trt_msb, trt_lsb;
    int		trt_neg, newstate;
    ushort	response;
    ushort	sword;
    fddisb      *fsb;
    uInt16  stnID = hwidb->fddi_stn_id;

    /*
     * Count number of times we are being interrupted.
     */
    if (hwidb->state == IDBS_ADMINDOWN)
	return;

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    /*
     *To ensure that we catch fast transitions, clear values unconditionally.
     */
    TIMER_STOP(fsb->fddi_operational);

    /*
     * Find out whether the interface went up or down.
     */
    (void)select_if(hwidb);
    (void)send_carrier_cmd(hwidb, &response);
    SendSignalMulti (hwidb->fddi_stn_id, SIG_Ring_OP, 0, response);
    newstate = (response ? IDBS_UP : IDBS_DOWN);

    if (fddi_debug){
        buginf("\n %s: interface is %s", hwidb->hw_namestring,
               ((newstate == IDBS_UP) ? "UP" : "DOWN"));
    }

    /*
     * Reset the transmit byte offset when the ring changes status.
     */
    hes_fddi_set_hold_reg(hwidb, get_fip_byte_rxoffset(hwidb));

    /* set up fddi hack */
    (void)select_if(hwidb);
    send_fip_hack_cmd(hwidb);

    if (newstate == IDBS_UP) {
	GET_TIMESTAMP(fsb->fddi_operational);
	hwidb->fddi_rops++;
	hwidb->mci_lineup = MCI_KEEP_COUNT;

	/*
	 * Ring just went operational -- get the negotiated TRT.
	 * The FORMAC holds this value in two (2) registers --
	 * the MSB is contained in FDDI_FM_R_TNEG and the LSB is
	 * contained in bits 14 - 10 of FDDI_FM_R_LSBS.
	 */
	(void)send_fip_fm_select_cmd(hwidb, FDDI_FM_R_TNEG);
	(void)send_fip_fm_read_cmd(hwidb, &sword);
	trt_msb = sword;
	(void)send_fip_fm_select_cmd(hwidb, FDDI_FM_R_LSBS);
	(void)send_fip_fm_read_cmd(hwidb, &sword);
	trt_lsb = sword;

	/*
	 * Combine the values and save in the hwidb for display and SMT.
	 */
	trt_lsb >>= 10;
	trt_lsb &= 0x1F;
	trt_msb <<= 5;
	trt_neg = (trt_msb | trt_lsb);
	trt_neg = -trt_neg;
	trt_neg &= 0x1fffff;
    fsb->fddi_trt_neg = ((trt_neg * 10) / ((FDDI_BYTES_SECOND) / 100000));
    } else {
	/*
	 * The interface went down -- clear some hwidb variables.
	 */
	hwidb->mci_lineup = 0;
	fsb->fddi_trt_neg = 0;
    }
    if ((hwidb->state != newstate) && (hwidb->state != IDBS_ADMINDOWN)
            &&  (MIBDATA (stnID, mib->smtData.CF_State) != CF_isolated)) {
        net_cstate(hwidb, newstate);
        hwidb->counters.transitions++;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}

static void cbus_fddi_af_or (hwidbtype *hwidb, ushort index, ushort value)
{
    ushort data;

    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_read_cmd(hwidb, &data);
    data |= value;
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_write_cmd(hwidb, data);
}

/*
 * fddi_reset
 * Reset the FDDI daughter board.
 */

void fddi_reset (hwidbtype *hwidb)
{
    uchar	*ptr;
    ushort	*sptr;
    idbtype	*swidb;
    int		indx, ltemp, cnt, rif_bit;
    leveltype	level;
    ushort	mask, smask, filter, data;
    ushort      rif_mask, norif_mask;
    ushort	status, af_size;
    ushort	swords[IEEEBYTES];
    uchar	uc;
    boolean     eoir_reset, srbridge_on_hwidb;
    fddisb     *fsb;
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;


    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }
    if (hwidb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    if (fddi_debug | fddi_cmt_events_debug) {
	buginf("\nfddi_reset(%s)", hwidb->hw_namestring);
    }

    if (eoir_reset) {
	level = raise_interrupt_level(NETS_DISABLE);
	status = select_if(hwidb);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status,
		   "select");
	    platform_req_card_restart(hwidb);
	    reset_interrupt_level(level);
	    return;
	}
	TIMER_STOP(hwidb->xmittimer);
	(void)send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	reset_interrupt_level(level);
    }

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    fddi_disconnect(hwidb->fddi_stn_id);

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;

    srbV_p = reg_invoke_srb_get_vector_table();

    level = raise_interrupt_level(NETS_DISABLE);
    reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    reg_invoke_media_rif_reset(hwidb);
    if (registry_populated(REG_SRT))
        reg_invoke_srt_setup_auto_sw_srb(NULL);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	platform_req_card_restart(hwidb);
	reset_interrupt_level(level);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	return;
    }

    if (!eoir_reset) {
	/*
	 * Disable the interface and then give the firmware a chance
	 * to finish the disable function.
	 */
	status = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
		   status, "disable");
	    platform_req_card_restart(hwidb);
	    reset_interrupt_level(level);
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	    return;
	}
    }
    fip_restart_init(hwidb);

    reset_interrupt_level(level);

    if (hwidb->fddi_options & FDDI_FAST_CMT)
	cbus_daughter_cmd(hwidb, FCI_FDDI_SET_CMT_MODE, CMT_MODE_CMT);
    else
	cbus_daughter_cmd(hwidb, FCI_FDDI_SET_CMT_MODE, CMT_MODE_IMMED);
    if ((hwidb->fddi_options & FDDI_DUPLICATE_ADDRESS_CHK) &&
        (hwidb->fddi_options & FDDI_FAST_CMT_ENB))
	cbus_daughter_cmd(hwidb, FCI_FDDI_DUP_ADDRESS, 1);
    else
	cbus_daughter_cmd(hwidb, FCI_FDDI_DUP_ADDRESS, 0);

    /*
     * Flush hold queue -- ring down -- no Upstream or Downstream neighbors.
     */
    holdq_reset(hwidb);
    TIMER_STOP(fsb->fddi_operational);

    /*
     * Disable interrupts and re-select the interface.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
	       status, "select");
	platform_req_card_restart(hwidb);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	reset_interrupt_level(level);
	return;
    }

    /*
     * Get the cookie address.  If hwidb->hardware is set-up, use that
     * address.
     */
    if (is_ieee_zero(hwidb->bia)) {

	if (!reg_invoke_assign_mac_addr(hwidb)) {
	    for (indx = 0; indx < IEEEBYTES; indx++) {
		send_station_cmd(hwidb, indx, &uc);
		swords[indx] = uc;
	    }
	    ptr = (uchar *)hwidb->bia;
	    for (indx = 0; indx < IEEEBYTES; indx++) {
		uc = (uchar)(swords[indx] & 0xFF);
		*ptr++ = uc;
	    }
	}

	if (is_ieee_zero(hwidb->hardware))
	    ieee_copy(hwidb->bia, hwidb->hardware);
    }

    /*
     * Bit swap our canonical address to form the actual address.
     */
    ieee_swap(hwidb->hardware, hwidb->bit_swapped_hardware);

    /*
     * Adjust number of SMT buffers to respond to possible card insertion
     */
    adjust_smt_bufs();

    /*
     * Report new address to the FDDI SMT MIB
     */
    {
    SMTMessage msg;

    msg.destination = MIB_MSG_ID;
    msg.source = MAP_MSG_ID;
    msg.type = MIB_ACTION_CHANGE_MACADDR;
    msg.stnID = hwidb->fddi_stn_id;
    msg.entity = 0;
    MCopyAddress (msg.data.addr, &hwidb->bit_swapped_hardware);

    SendMAPMessage(&msg);
    }

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
		BypassRequestMulti (hwidb->fddi_stn_id, DEINSERTED);
		idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
		reset_interrupt_level(level);
        return;
    }


    /*
     * The following code will attempt to clear up a deadlock situation in
     * the ENDECs. When the box is first powered up, AND the fiber is
     * connected to the ENDECs, we are unable to perform any CMT because
     * the ENDEC status is always OVUF.
     */
    for (indx = ENDECA; indx < ENDECB + 1; indx++) {
	select_endec(hwidb, FDDI_EN_RW_CR1, indx);
	send_fip_en_write_cmd(hwidb, ENDEC_LOOPBACK);

	select_endec(hwidb, FDDI_EN_RW_CR0, indx);
	send_fip_en_write_cmd(hwidb, ENDEC_FORCE_MASTER);
	DELAY(10);
    }
    BypassRequestMulti (hwidb->fddi_stn_id, INSERTED);
    /*
     * Step on the ENDEC's -- reset them, also forces Quiets on the fiber.
     */
    select_endec(hwidb, FDDI_EN_RW_CR2, ENDECA);
    send_fip_en_write_cmd(hwidb, ENDEC_RESET);

    select_endec(hwidb, FDDI_EN_RW_CR2, ENDECB);
    send_fip_en_write_cmd(hwidb, ENDEC_RESET);

    /*
     * Place interface in the CF_isolated state.
     */
    ConfigureAttachMulti(hwidb->fddi_stn_id, CF_isolated);

    hwidb->multibits = 0;
    mask = smask = filter = 0;
    srbridge_on_hwidb = FALSE;

    /*
     * Determine whether SRB is configured on the interface.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
	if (RUNNING_IF(LINK_SRB, swidb)) {
	    srbridge_on_hwidb = TRUE;
	    break;
	}

    /*
     * Turn on bridging on hwidb if any swidb has tbridging 
     * or SRB configured.
     */
    if (bridge_enable && (hwidb->tbridge_on_hwidb || srbridge_on_hwidb)) {
	if (!(hwidb->fddi_options & FDDI_ENCAPSULATE)) {
	    /*
	     * If encapsulating don't set up a non-zero bridge
	     * filter and don't execute the FCI_CMD_BRIDGE command
	     * because this will fuck up other interfaces.
	     */
	    filter = FCI_BRIDGING_MASK << 8;
	    status = send_bridge_cmd(hwidb, filter);
	    if (status != FCI_RSP_OKAY)
		errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, 
		       status, "bridge cmd");
	}
    }

    if (!eoir_reset) {
	/*
	 * Get size of the address filter.
	 */
	send_af_size_cmd(hwidb, &af_size);

	if (hwidb->type == IDBTYPE_FDDIT) {
	    mask = (filter | ((FCI_STATION_MASK | FCI_HSRP_STATION_MASK |
			       FCI_MULTICAST_MASK) << 8));
	    smask = mask;

	    /*
	     * Set up the protocol portion of the address mask.
	     * Do NOT program the AF in this way if CRB is enabled
	     * and the interface is configured for transparent
	     * bridging.
	     */
	    if (!(global_crb_enable && hwidb->tbridge_on_hwidb)) {
		FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		    if ((ROUTING_IF(swidb, LINK_IP)))
			mask |= FCI_DOD_IP_PROTOCOL_MASK << 8;
		    if (ROUTING_IF(swidb, LINK_DECNET))
			mask |= FCI_DECNET_PROTOCOL_MASK << 8;
		    if (ROUTING_IF(swidb, LINK_CLNS))
			mask |= FCI_CLNS_PROTOCOL_MASK << 8;
		    if (ROUTING_IF(swidb, LINK_XNS))
			mask |= FCI_XNS_PROTOCOL_MASK << 8;
		}
	    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
	    if (tbridge_debug_crb)
		buginf("\nTB-CRB %s MCI_CMD_AF_WRITE %04x",
		       hwidb->hw_namestring, mask);
#endif  /* TBRIDGE_DEBUG */

	    /*
	     * Initialize the first page special.  This is the FC
	     * page and should never have the bridge bit set on any
	     * other than the LLC shit.
	     */
	    send_af_select_cmd(hwidb, FDDIT_AF_FC_PAGE);
	    for (indx = 0; indx < 0x100; indx++) {
		
		/*
		 * Zero the first page.  FC byte.
		 */
		send_af_write_cmd(hwidb, 0);
	    }

	    /*
	     * Set the Fddi-T's address filter to accept SMT and LLC frames
	     * sent to this interface.  We accept 0x41 - 0x4f for SMT,
	     * 0x50-0x5f for Async LLC, and 0xd0-0xdf for Sync LLC.  The
	     * later we can receive but not transmit.
	     */
	    send_af_select_cmd(hwidb, (FDDIT_AF_FC_PAGE | 0x41));
	    for (indx = 1; indx < 0x10; indx++) {
		
		/*
		 * SMT frames should be accepted.
		 */
		send_af_write_cmd(hwidb, (mask & ~(FCI_BRIDGING_MASK << 8)));
	    }
	    send_af_select_cmd(hwidb, (FDDIT_AF_FC_PAGE | 0x50));
	    for (indx = 0; indx < 0x10; indx++) {
		
		/*
		 * Write the Async LLC stuff.
		 */
		send_af_write_cmd(hwidb, mask);
	    }
	    send_af_select_cmd(hwidb, (FDDIT_AF_FC_PAGE | 0xd0));
	    for (indx = 0; indx < 0x10; indx++) {

		/*
		 * Write the Sync LLC stuff.
		 */
		send_af_write_cmd(hwidb, mask);
	    }
	    /*
	     * Do part of the DA, more later
	     */
	    send_af_select_cmd(hwidb, 0x0);
	    for (indx = 0x0; indx < 0x600; indx++) {
		send_af_write_cmd(hwidb, filter);
	    }

	    /*
	     * Set SA with mask
	     *
	     * if we are doing srb accept frames with RII bit set in src addr
	     * if we are doing tbridge accept frames without RII bit set 
	     */
            if (srbridge_on_hwidb) {
                rif_mask = mask;
            } else {
                rif_mask = (mask & ~(FCI_BRIDGING_MASK<<8));
            }
	    /*
	     * Don't set IP bit if there's a RIF, since we want
	     * to use the IP bit to know if we can optimum switch.
	     */
	    rif_mask &= ~(FCI_DOD_IP_PROTOCOL_MASK<<8);

            if (hwidb->tbridge_on_hwidb) {
                norif_mask = mask;
            } else {
                norif_mask = (mask & ~(FCI_BRIDGING_MASK<<8));
            }

            rif_bit = (hwidb->type == IDBTYPE_FDDIT) ? FDDIT_RII : FDDI_RII;

	    for (indx = 0x600; indx < 0x700; indx++) {
	        if (indx & rif_bit) {
	    	    send_af_write_cmd(hwidb, rif_mask);
                } else {
	    	    send_af_write_cmd(hwidb, norif_mask);
		}
	    }
	    for (indx = 0x700; indx < 0xC00; indx++) {
		send_af_write_cmd(hwidb, mask);
	    }

	    for (indx = 0xC00; indx < 0xE00; indx += 2) {
		data = smask;
		if ((indx == 0xC00 + SAP_CLNS) || (indx == 0xD00 + SAP_CLNS))
		    data |= (mask & (FCI_CLNS_PROTOCOL_MASK << 8));
		if ((indx == 0xC00 + SAP_SNAP) || (indx == 0xD00 + SAP_SNAP)) {
		    data |= (mask & (FCI_DOD_IP_PROTOCOL_MASK << 8));
		    data |= (mask & (FCI_DECNET_PROTOCOL_MASK << 8));
		    data |= (mask & (FCI_XNS_PROTOCOL_MASK << 8));
		}
		send_af_write_cmd(hwidb, data);
		send_af_write_cmd(hwidb, data);
	    }
	    for (indx = 0xE00; indx < 0xF00; indx++) {
		data = smask;
		if (indx == 0xE00 + 0x03) {        /* UI frame */
		    data |= (mask & (FCI_CLNS_PROTOCOL_MASK << 8));
		    data |= (mask & (FCI_DOD_IP_PROTOCOL_MASK << 8));
		    data |= (mask & (FCI_DECNET_PROTOCOL_MASK << 8));
		    data |= (mask & (FCI_XNS_PROTOCOL_MASK << 8));
		}
		send_af_write_cmd(hwidb, data);
	    }

	    for (indx = 0xF00; indx < 0x1200; indx++) {
		send_af_write_cmd(hwidb, mask);
	    }
	    for (indx = 0x1200; indx < 0x1400; indx++) {
		send_af_write_cmd(hwidb, smask);
	    }
	    /*
	     * fill the rest of our address filter with appropriate
	     * value so its a don't care.  The current pointer is at
	     * 0x1400.  Start at 0x1500 to compensate for the FC page
	     * being at 0xff00 (-1).
	     */
	    for (indx = 0x1500; indx < af_size; indx++) {
		send_af_write_cmd(hwidb, mask);
	    }

	    /*
	     * The af has been set up -- except for the SNAP type field.
	     */
	    if (mask & (FCI_DOD_IP_PROTOCOL_MASK << 8)) {
		cbus_fddi_af_or(hwidb, (0x1200 + 0x08),
				(FCI_DOD_IP_PROTOCOL_MASK << 8));
		cbus_fddi_af_or(hwidb, (0x1300 + 0x00),
				(FCI_DOD_IP_PROTOCOL_MASK << 8));

		/*
		 * Clear IP protocol bit except for 45.
		 */
		for (indx = 0x1400; indx < 0x1500; indx++) {
		    if (indx == 0x1400 + 0x45)
			continue;
		    cbus_af_and(hwidb, indx, ~(FCI_DOD_IP_PROTOCOL_MASK << 8));
		}

		/*
		 * Clear IP protocol bit if TTL is 0 or 1.
		 * TTL is the 9th byte in the IP header (30th in packet).
		 */
		cbus_af_and(hwidb, 0x1C00, ~(FCI_DOD_IP_PROTOCOL_MASK << 8));
		cbus_af_and(hwidb, 0x1C01, ~(FCI_DOD_IP_PROTOCOL_MASK << 8));
	    }
	    if (mask & (FCI_DECNET_PROTOCOL_MASK << 8)) {
		cbus_fddi_af_or(hwidb, (0x1200 + 0x60),
				(FCI_DECNET_PROTOCOL_MASK << 8));
		cbus_fddi_af_or(hwidb, (0x1300 + 0x03),
				(FCI_DECNET_PROTOCOL_MASK << 8));

		/*
		 * Clear DECnet protocol bit -- if padding with not one 1 byte.
		 */
		for (indx = 0x1600 + 0x80; indx < 0x1700; indx++) {
		    if (indx == (0x1600 + 0x81))
			continue;
		    cbus_af_and(hwidb, indx, ~(FCI_DECNET_PROTOCOL_MASK << 8));
		}
	    }

	    /*
	     * If routing XNS set type 0600 in octets 20 and 21.
	     */
	    if (mask & (FCI_XNS_PROTOCOL_MASK << 8)) {
		cbus_fddi_af_or(hwidb, (0x1200 + 0x06),
				(FCI_XNS_PROTOCOL_MASK << 8));
		cbus_fddi_af_or(hwidb, (0x1300 + 0x00),
				(FCI_XNS_PROTOCOL_MASK << 8));

		/*
		 * In case of XNS 26th octet contains hop count
		 * Exclude packets with hope count 15 and greater.
		 */
		for (indx = 0x1800 + 0x0F; indx < 0x1900; indx++) {
		    cbus_af_and(hwidb, indx, ~(FCI_XNS_PROTOCOL_MASK << 8));
		}
	    }

	    /*
	     * Initialize the RIF Scanner page to all zeros.
	     */
	    send_af_select_cmd(hwidb, FDDIT_AF_RIF_PAGE);
	    for (indx = 0; indx < 0x100; indx++) {
		send_af_write_cmd(hwidb, 0);
	    }
            if (srbV_p) {
                srbV_p->srb_sb_get_triplet(hwidb->firstsw, &srb_triplet);
            } else {
                srb_triplet.localRing  = SRB_INVALID_RING_NUM;
                srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
                srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
            }
	    if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
		/*
		 * Source Route Bridging is enabled on this interface.
		 * Program the address filter so the RIF Scanner does
		 * its thing.
		 *
		 * Using the upper 8 bits of the local ring number we
		 * want to set both forward and reverse bit positions
		 * for LRN in the AF.
		 */
		indx = (srb_triplet.localRing >> 4) & 0xff;
		cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx, 
				(AF_RIF_FLRN | AF_RIF_RLRN));

		/*
		 * Now take the lower 4 bits of the local ring number (LRN).
		 * All of those slots get set with RLRNE (Reverse ending 
		 * Local Ring Num).  The lower 4 bits of LRN combined with 
		 * the bridge number also is used to set FLRBN (Fwd LRN and 
		 * Bridge Num).
		 */
		indx = (srb_triplet.localRing & 0xf) << 4;
		for (cnt = 0; cnt < 0x10; cnt++) {
		    cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx, 
				    AF_RIF_RLRNE);
		    indx++;
		}
		indx = ((srb_triplet.localRing & 0xf) << 4) | (srb_triplet.bridgeNum & 0xf);
		cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx, AF_RIF_FLRBN);


		/*
		 * Now do the same thing for target ring number
		 */
		indx = (srb_triplet.remoteRing >> 4) & 0xff;
		cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx, 
				(AF_RIF_RTRN | AF_RIF_FTRN));
		indx = (srb_triplet.remoteRing & 0xf) << 4;
		for (cnt = 0; cnt < 0x10; cnt++) {
		    cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx,
				    AF_RIF_FTRNE);
		    indx++;
		}
		indx = ((srb_triplet.remoteRing & 0xf) << 4) | (srb_triplet.bridgeNum & 0xf);
		cbus_fddi_af_or(hwidb, FDDIT_AF_RIF_PAGE | indx, AF_RIF_RTRBN);
	    }
	} 

	cbus_fddi_address(hwidb, FSM_STATION, hwidb->hardware);
	cbus_fddi_address(hwidb, FSM_BROADCAST, (uchar *) baddr);

	/*
	 * Do NOT program the AF in this way if CRB is enabled
	 * and the interface is configured for transparent bridging.
	 */
	if (hwidb->type == IDBTYPE_FDDIT &&
	    (!(global_crb_enable && hwidb->tbridge_on_hwidb))) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		/*
		 * Set IP bit for DA
		 */
		if (ROUTING_IF(swidb, LINK_IP))
		    cbus_fddi_address(hwidb, FCI_DOD_IP_PROTOCOL,
				      hwidb->hardware);
		/*
		 * Set DEC bit for DA
		 */
		if (ROUTING_IF(swidb, LINK_DECNET))
		    cbus_fddi_address(hwidb, FCI_DECNET_PROTOCOL,
				      hwidb->hardware);
		/*
		 * Set CLNS bit for DA
		 */
		if (ROUTING_IF(swidb, LINK_CLNS))
		    cbus_fddi_address(hwidb, FCI_CLNS_PROTOCOL,
				      hwidb->hardware);
		/*
		 * Set XNS bit for DA
		 */
		if (ROUTING_IF(swidb, LINK_XNS))
		    cbus_fddi_address(hwidb, FCI_XNS_PROTOCOL,
				      hwidb->hardware);
	    }
	}
    }


    /*
     * Turn on the LEM monitor -- if running correct microcode.
     */
    if (hwidb->fddi_options & FDDI_LEM_SUPPORTED) {
	send_fip_phy_a_lem_cmd(hwidb, FDDI_LEM_ON);
	send_fip_phy_b_lem_cmd(hwidb, FDDI_LEM_ON);
    }

    /*
     * The formac TVX value is only 1/255 of the actual time.
     */
    ltemp =  -((FDDI_BYTES_SECOND / 10000) * (fsb->fddi_tvx / 100));
    ltemp = ltemp / 255;
    ltemp++;				/* round up */
    send_fip_fm_select_cmd(hwidb, FDDI_FM_RW_TVX);
    /* 2's complement of requested value */
    send_fip_fm_write_cmd(hwidb, -ltemp);

    /*
     * Write the requested TRT into the formac register.
     */
    send_fip_fm_select_cmd(hwidb, FDDI_FM_RW_TRT);
    /* 2's complement of requested value */
    send_fip_fm_write_cmd(hwidb, 
        -((FDDI_BYTES_SECOND / 10000) * (fsb->fddi_trt / 100)));

    /*
     * Write the default T_MAX value into the formac register.
     */
    send_fip_fm_select_cmd(hwidb, FDDI_FM_RW_TMAX);
    send_fip_fm_write_cmd(hwidb, (CISCO_DEFAULT_TMAX >> 5));

    /*
     * Reset the address register pointer.
     */
    send_fip_fm_select_cmd(hwidb, FDDI_FM_W_RESET_ADDRPTR);
    send_fip_fm_write_cmd(hwidb, 0);

    /*
     * Select the address register and write our station address and TRT
     * into the FORMAC address registers -- both used for claim processing.
     */
    send_fip_fm_select_cmd(hwidb, FDDI_FM_RW_ADDR_REG);
    sptr = (ushort *) hwidb->bit_swapped_hardware;
    ltemp = -(((FDDI_BYTES_SECOND / 100) * (fsb->fddi_trt / 100)) / 100);  
    for (indx = 0; indx < 8; indx++) {
	ushort	arg;

	if (indx == 1)
	    arg = GETSHORT(sptr++);
	else if (indx == 2)
	    arg = GETSHORT(sptr++);
	else if (indx == 3)
	    arg = GETSHORT(sptr);
	else if (indx == 6)
	    arg = (ltemp >> 16);
	else if  (indx == 7)
	    arg = ltemp;
	else {
	    /*
	     * If we are skipping over SAID (indx == 0),
	     * SAGP (indx == 4), or the reserved position (indx == 5)
	     */
	    arg = 0;
	}
	send_fip_fm_write_cmd(hwidb, arg);
    }

    /*
     * Build the claim/beacon/directed_beacon buffer and
     * stuff our station address and TRT into it.
     */
    status = build_claim_beacon_buffer(hwidb, ltemp);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status,
	       "claim/beacon");
	platform_req_card_restart(hwidb);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	reset_interrupt_level(level);
	return;
    }

    /*
     * Teach the CAM uCode what our nodal mac address is.  The CAM
     * uCode prevents our address or any multicast from being inserted
     * into the CAM.  The CAM insertion system code must take into account
     * the RII bit when manipulating the CAM using source addresses.
     */
    if (hwidb->type == IDBTYPE_FDDIT) {
	select_if(hwidb);
        send_fddi_cam_wr_arg_cmd(hwidb, GETSHORT(&hwidb->hardware[0]));
        send_fddi_cam_wr_arg_cmd(hwidb, GETSHORT(&hwidb->hardware[2]));
        send_fddi_cam_wr_arg_cmd(hwidb, GETSHORT(&hwidb->hardware[4]));
	send_fddi_cam_cmd(hwidb, FDDI_CAM_LEARN_MA);

	fddi_check_for_old_cam_bug(hwidb);
    }

    /*
     * Enable reception.  It is possible for this command to fail if there
     * insufficient receive buffers allocated.
     */
    cbus_mci_txlimit(hwidb);
    status = send_if_enable_cmd(hwidb);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSETU, CBUS), hwidb->mci_index, status, "enable");
	platform_req_card_restart(hwidb);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
	reset_interrupt_level(level);
	return;
    }

    reset_interrupt_level(level);

    /*
     * Enable Reception for our hardware address and the broadcast address.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
        reg_invoke_tbridge_smf_update(hwidb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);

        reg_invoke_media_interesting_addresses(swidb);
    }
    /*
     * If we entered fddi_reset() from the initial MEMD carve,
     * before we parsed the configuration and gathered any fddi
     * specific interface configuration commands, we don't want
     * to even try to join the ring. We will join the ring later,
     * when hwidb->fddi_system_configured() is called.
     */
    if (system_configured)
	fddi_try_init_cmt(hwidb);

    /*
     * recalc srb params
     */
    if (srbV_p) {
        srbV_p->srb_sb_init_rng_br_rng_mask(hwidb->firstsw);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}

/*****************************************************************************
 * Name: get_fddi_cbus_errors
 *
 * Description: Accumulates cBus errors from fddi interfaces
 *   Called by get_cbus_errors
 *
 * Assumptions:
 *   1. interrupts are disabled
 *   2. interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *****************************************************************************/
static void get_fddi_cbus_errors (hwidbtype *hwidb)
{
    long	counts;
    fddisb      *fsb;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_CRC);

    if (hwidb->status & IDB_VIP)   
    return;

    hwidb->counters.input_crc += counts;
    hwidb->counters.input_error += counts;

    counts = send_errors_cmd(hwidb, FCI_ERR_NOBUFFERS);
    hwidb->counters.input_resource += counts;

    counts = send_errors_cmd(hwidb, FCI_ERR_FRAME);
    hwidb->counters.input_frame += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_GIANTS);
    hwidb->counters.giants += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_OVERRUN);
    hwidb->counters.input_overrun += counts;
    hwidb->counters.input_error += counts;
    
    if (hwidb->type == IDBTYPE_FDDIT) {
	fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

	counts = send_errors_cmd(hwidb, FCI_ERR_TX_UNDERFLOW);
	hwidb->counters.output_underrun += counts;
	
	counts = send_errors_cmd(hwidb, FCI_ERR_CLAIMS);
	fsb->fddi_claims += counts;

	if (MACInterruptsEnabled) {
	    ulong	beacons_cnt;
	    leveltype	cmt_level;

	    beacons_cnt = send_errors_cmd(hwidb, FCI_ERR_BEACONS);
	    if (beacons_cnt) {
		fsb->fddi_beacons += beacons_cnt;
		SendSignalMulti(hwidb->fddi_stn_id, SIG_MAC_Interrupt,
				0, (uInt32) MAC_TRT_In_T4T5);
		/* Process CSP signal */
		cmt_level = raise_interrupt_level (NETS_DISABLE);
		ProcessCSP(&cmt_level);
		reset_interrupt_level(cmt_level);
	    }
	}
	idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    }
}

/*
 * fip_init:	Called to init. the registry states to support the FDDIT
 *	specific driver code.
 */
static void fip_init (subsystype* subsys) 
{
    reg_add_cbus_get_ip_errors(FCI_TYPE_FDDIT, 
			       get_fddi_cbus_errors, "get_fddi_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_FDDI, 
			       get_fddi_cbus_errors, "get_fddi_cbus_errors");
    fip_platform_init(subsys);
}

/*
 * FDDIT subsystem header
 */

#define FIP_MAJVERSION	1
#define FIP_MINVERSION	0
#define FIP_EDITVERSION	1

SUBSYS_HEADER(fip, FIP_MAJVERSION, FIP_MINVERSION, FIP_EDITVERSION,
	      fip_init, SUBSYS_CLASS_DRIVER,
	      "seq: cbus_mci",
	      "req: cbus_mci");
