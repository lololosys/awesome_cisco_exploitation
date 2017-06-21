/* $Id: if_mip.c,v 3.10.4.21 1996/08/28 12:45:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_mip.c,v $
 *------------------------------------------------------------------
 * if_mip.c -- Support for the MIP card
 *
 * 29-June-1993, Ronnie B. Kon
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_mip.c,v $
 * Revision 3.10.4.21  1996/08/28  12:45:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.20  1996/08/27  07:11:04  mwu
 * CSCdi67143:  MIP interfaces dont come up on 7000 platform
 * Branch: California_branch
 *
 * Revision 3.10.4.19  1996/08/19  23:12:25  mwu
 * CSCdi66057:  mip interface outhang during booting - don't change
 * interface state for eoir mip; also optimize code for interface
 * creation/reset.
 * Branch: California_branch
 *
 * Revision 3.10.4.18  1996/08/13  02:17:50  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.4.17  1996/08/11  20:32:04  mwu
 * CSCdi65038:  admin-down interfaces upped on EOIR - don't change
 * interface state for eoir mip; also optimize code for interface
 * creation/reset.
 * Branch: California_branch
 *
 * Revision 3.10.4.16  1996/08/09  17:20:10  mwu
 * CSCdi59746:  MIP EOIR recovery time is exceedingly long.
 * Branch: California_branch
 *
 * Revision 3.10.4.15  1996/08/02  00:38:20  mwu
 * CSCdi64153:  mips txlimit set to zero in a loaded 75xx
 * Branch: California_branch
 *
 * Revision 3.10.4.14  1996/07/26  21:29:12  drowell
 * CSCdi53263:  Flow switching (MIP-VIP2 ) will take wrong switching path
 * (mip side)
 * Branch: California_branch
 * Improved the love letter interpretation to take into account packets in
 * TX/RX queue.
 *
 * Revision 3.10.4.13  1996/07/08  22:25:36  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.10.4.12  1996/06/26  03:00:46  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.10.4.11  1996/06/19  21:52:57  wmay
 * CSCdi57946:  agent returns incorrect value for dsx1LineType mib
 * attribute - use mib setting from configuration.
 * Branch: California_branch
 *
 * Revision 3.10.4.10  1996/06/05  20:53:53  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * rework mip_interface_reset to send only 1 loopback command, and use
 * the far_end_looped field to indicate that a remote loop is actually
 * present
 * Branch: California_branch
 *
 * Revision 3.10.4.9  1996/05/21  09:47:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.4.8  1996/05/21  06:29:24  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.10.4.7  1996/05/13  14:46:52  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.10.4.6  1996/05/09  14:28:58  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.4.5  1996/05/06  23:22:47  sdurham
 * CSCdi54843:  RFC1406 MIB implementation has table indices starting at 0
 * Branch: California_branch
 *      initialize seed value for 4000 as is done for mip.
 *
 * Revision 3.10.4.4  1996/05/05  16:49:32  mwu
 * CSCdi56290:  channel-group command crashes 75xx routers caused
 * by CSCdi54721.
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/04/27  05:48:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.10.4.2  1996/04/22  18:11:23  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.10.4.1  1996/03/18  19:42:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 *
 * Revision 3.10  1996/03/07  22:41:23  wmay
 * CSCdi50940:  %SCHED-2-WATCH in mip_love_letter_handler - init watched
 * queue earlier (before cfork).
 *
 * Revision 3.9  1996/02/16  06:36:49  mwu
 * CSCdi49185:  swidb pointer in hwidb was cleared by idb_clear_dangerous.
 *
 * Revision 3.8  1996/02/13  08:12:21  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1996/02/13  02:53:47  hampton
 * Stop the carrier timer before clearing the IDB.  [CSCdi46506]
 *
 * Revision 3.6.6.1  1996/03/05  06:11:29  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.2.1  1996/02/27  20:42:11  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/02/08  22:21:58  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.5.14.1  1996/01/24  21:47:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5.4.1  1996/02/15  19:13:12  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1995/12/15  08:01:35  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.4  1995/12/06  18:28:49  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  09:18:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/06/29  23:30:33  jchlin
 * Currently, c4000 and c7000 are reusing hwidbs and swidbs because
 * of dynamic changes of interface configuration. Due to the lack of
 * way to clean up the idbs when they are freed, both platforms will
 * try to release as many known buffer pointers of idb as possible
 * before they are put in the private queue for reuse.
 * When reusing idbs, idb init routines called inside idb_create()
 * are called to reinitialized idbs. In 11.0, there is a new
 * function call added in idb_create() which is not picked up by
 * both platform init_idb(). And therefore, it caused a crash
 * because a null pointer is not initialized. This fix is trying to
 * separate the malloc of idbs and initialization of idbs into
 * two different functions so that the initialization of
 * both hwidb and swidb can be done in that new rourtine as well
 * as new addition function calls in the future. Then we can avoid
 * another addition of function calls in the idb_create() to crash
 * both platform again.
 * CSCdi36076:  Configuring PRI on E1 crashes box w/ALIGN-1-FATAL
 *
 * Revision 2.4  1995/06/19  21:50:45  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.3  1995/06/19  07:16:16  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/18  23:17:17  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:42:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "../hes/if_mci.h"
#include "../if/if_controller.h"
#include "hes_controller.h"
#include "../if/dsx1.h"
#include "ieee.h"
#include "if_mip.h"
#include "logger.h"
#include "../if/network.h"
#include "packet.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../wan/serial_private.h"
#include "../wan/dialer_registry.h"
#include "../if/dsx1_registry.h"
#include "config.h"
#include "../wan/serial_parser.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "msg_oir.c"			/* Not a typo, see logger.h */
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */
#include "../iprouting/igrp.h"
#include "../hes/dgbus.h"
#include "../hes/dbus.h"
#include "../hes/ucode.h"
#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../snmp/ifmib_registry.h"


mipinfo *mip_list[MAX_CBUS_INTERFACES];
static mipinfo null_mip_entry;
static queuetype mip_idbQ;	/* list of reusable hwidbs */
watched_queue *mip_loveletterQ;
static boolean loveletters_throttled;

queuetype mip_preQ;	/* preallocated mip entries for hot-swap */

/* Forward references */
static void mip_dsx1_reset (cdbtype *cdb);

static unsigned int snmp_dsx1LineIndex_seed; /* dump when we support ifIndex */

static void mip_idb_init(cdbtype *, hwidbtype *);
static void mip_love_note_handler(cdbtype *cdb, ushort status);
static void mip_show_serial_state(hwidbtype *, char *);
static void mip_clear_counts(cdbtype *cdb);
static void mip_dsx1_carrier(mipinfo *);
static void mip_shut_down_card(int, ushort);
static void mip_interface_reset(hwidbtype *);
static void mip_delete_idb (cdbtype *, hwidbtype *);
static inline int convert_slot_appl_to_unit (int slot, int appl_no)
{
    return(slot * MAX_CTRLR_INTERFACES + appl_no);
}

inline mipinfo *mip_find_by_idb (hwidbtype *idb)
{
    return(mip_list[idb->mci_index]);
}

static inline mipinfo *mip_find_by_slot_appl (int slot, int appl_no)
{
    return(mip_list[convert_slot_appl_to_unit(slot, appl_no)]);
}

static inline mipinfo *mip_find_by_unit (int unit)
{
    return mip_list[unit];
}

/*
 * Utility routine to select an plughole, and then
 * select a VC within that plughole.  This routine assumes
 * that interrupts have already been disabled.  It returns
 * the status from the command(s).
 */
inline ushort mip_select_vc (mipinfo *mip, int channel)
{
    ushort sresult;

    sresult = mip_select_if(mip);
    if (sresult != FCI_RSP_OKAY) {
	return(sresult);
    }

    sresult = mip_command_arg(mip, FCI_CMD_VC_SELECT, channel);
    return(sresult);
}


/*
 * mip_idbtype
 * Return an IDBTYPE for a unit
 */
static enum IDBTYPE mip_idbtype (int unit)
{
    switch (mip_list[unit]->applique_type) {
    case APPL_T1:
	return(IDBTYPE_T1);
    case APPL_E1_BAL:
    case APPL_E1_UNBAL:
	return(IDBTYPE_E1);
    case APPL_6B:
    case APPL_NONE:
    case APPL_T1_CSU:
	return(0);	/* can never happen, but shut the compiler up */
    }
    return(0);
}

/************************************************************************
 *									*
 *	Controller commands - initialization and reseting		*
 *									*
 ************************************************************************/
/*
 * mip_create_context
 * We have found a MIP card.
 *
 * Note, that the first parameter is a transparent pointer type to
 * some suitable hardware handle for whatever platform this code is
 * running on.  mip_create_context() does not interpret this parameter,
 * but merely passes it on down to controller_create() where it gets
 * stashed away for later use by platform specific code.
 */
cdbtype *mip_create_context (platform_hware_handle_type *hware_handle,
			     int unit,
			     int interfacetype,
			     boolean ok2malloc,
			     int applique_type)
{
    int ix;
    mipinfo *mip;
    cdbtype *cdb;
    int slot, appl_no;
    boolean this_is_ssip_smip_pslot = FALSE;

    cbus_unit2slotunit(unit, &slot, &appl_no);
    this_is_ssip_smip_pslot = ssip_smip_pslot(slot);

    /* Do we have a type that we recognize ? */
    switch (applique_type) {
    case APPL_T1:
    case APPL_E1_BAL:
    case APPL_E1_UNBAL:
	break;
    default:
	return(NULL);
    }
    /*
     * Have a type we recognize - get a mipinfo structure, or recycle one
     */
    if (mip_list[unit]==NULL || mip_list[unit]==&null_mip_entry) {
	/*
	 * On the 7000 we can be called via a hot-stall interrupt, which
	 * means we cannot malloc.  So we rely on the preQ instead.
	 */
	if (ok2malloc) {
	    if ((mip_list[unit] = malloc(sizeof(mipinfo))) == NULL) {
		mip_list[unit] = &null_mip_entry;
		return(NULL);
	    }
	} else {
	    if ((mip_list[unit] = dequeue(&mip_preQ)) == NULL) {
		buginf("\n%%No preallocated memory: Cannot create "
		       "MIP card context");
		mip_list[unit] = &null_mip_entry;
		return(NULL);
	    }
	    memset(mip_list[unit], 0, sizeof(mipinfo));	/* mimic malloc() */
	}

	mip = mip_list[unit];
	mip->unit = unit;
	for (ix = 0 ; ix <= MIP_MAX_VC ; ix++)
	    mip->hwidb[ix] = NULL;
    } else {
	mip = mip_list[unit];
    }
    
    mip->slot = slot;
    mip->appl_no = appl_no;
    mip->applique_type = applique_type;
    mip->interfacetype = interfacetype;
    
    mip->cdb = cdb = controller_create(FCI_MIP_CONTROLLER,
				       applique_type == APPL_T1 ?
				       PIFT_T1 : PIFT_E1,
				       mip->unit, hware_handle, ok2malloc);
    if (cdb == NULL) {
	free_mip(mip);
	mip_list[unit] = &null_mip_entry;
	return(NULL);
    }
    /*
     *  If this is a board which was previously hotswapped out, then
     *  we must retain the old values.  If this is new, we must set
     *  to our defaults.
     *
     *  We can tell which of these is the case by testing the CONFIGURE
     *  bit.  The default is guaranteed to be off, we always set it
     *  to on.
     */
    if (!(cdb->cdb_status & CDB_CONFIGURE)) {
	cdb->cdb_status |= CDB_CONFIGURE;
	cdb->reset = mip_dsx1_reset;
	cdb->love_note = mip_love_note_handler;
	cdb->idb_init = mip_idb_init;
	cdb->encsize = MIP_ENCAPSIZE_WORDS;
	cdb->idbtype = mip_idbtype(mip->unit);
	cdb->clear_counts = mip_clear_counts;
	    
	/* this can be dumped when we support an ifIndex for the T1 card */
	cdb->hw_if_index = snmp_dsx1LineIndex_seed;
	snmp_dsx1LineIndex_seed+=2; /* advance to the next valid index */

	/* now init the buffer related stuff */
	cdb->max_buffers = 0;
	cdb->mci_txcount = 0;
	cdb->buffer_pool = 0;
	cdb->mci_txcount_default = 0;
	cdb->mci_txqloc = 0;
	cdb->cbus_burst_count = 0;
        /*
         * Initialise the MTU stuff as if love letters were
	 * irrelevant.  Then, depending on whether or not 
         * a seperate love letter pool is used on this platform,
	 * adjust to ensure it's large enough.
         */
	cdb->maxmtu = MAXSERIALSIZE;
	cdb->buffersize = MCI_BUFFERSIZE_BYTES;
	cdb->max_buffer_size = MCI_MAXDGRAM_DEF;
	platform_mip_adjust_mtu_for_love(mip, &cdb->maxmtu, &cdb->buffersize,
					 &cdb->max_buffer_size);
    }

    switch (mip->applique_type) {
    case APPL_T1:
	cdb->bandwidth_def = SCALED_BANDWIDTH(VISIBLE_T1_BANDWIDTH);
	dsx1_create_context(cdb, applique_type, ok2malloc);
	mip->carrier = mip_dsx1_carrier;
	mip_dsx1_reset(cdb);
	if (this_is_ssip_smip_pslot)
	    nst1ports++;
	else
	    nt1ports++; 
	return(cdb);
	
    case APPL_E1_BAL:
    case APPL_E1_UNBAL:
	cdb->bandwidth_def = SCALED_BANDWIDTH(VISIBLE_E1_BANDWIDTH);
	dsx1_create_context(cdb, applique_type, ok2malloc);
	mip->carrier = mip_dsx1_carrier;
	mip_dsx1_reset(cdb);
	if (this_is_ssip_smip_pslot)
	    nse1ports++;
	else
	    ne1ports++; 
	return(cdb);
    case APPL_6B:
	buginf("\n%%MIP %d/%d: Unsupported applique type %d.",
	       mip->slot, mip->appl_no, mip->applique_type);
	/* fall through... */
    case APPL_NONE:
    case APPL_T1_CSU:
	/* Blow away the mip_info structure */
	free_mip(mip);
	mip_list[unit] = &null_mip_entry;
	return(NULL);
    }
    return(NULL);
}

/*
 * mip_eoir_capable_hw_version
 * Return true if this MIP hardware is EOIR capable
 */
static boolean mip_eoir_capable_hw_version (cdbtype *cdb)
{
    ushort hw_version = dbus_hw_version(pslot_to_dbus(cdb->slot));

    if (hw_version == MIP_NO_EOIR_VERSION)
	return(FALSE);

    return(TRUE);
}

/*
 * mip_dsx1_reset
 *
 * Reset an entire T1/E1 controller on the MIP
 */
static void mip_dsx1_reset (cdbtype *cdb)
{
    mipinfo	*mip;
    boolean	eoir;
    ushort	sresult;
    leveltype	status;

    /*
     * We don't want to reset the controller and framer before the 
     * configration is done during the system initialization.
     */
    if (!mip_platform_configured_check())
        return;

    eoir = (cdb->cdb_status & CDB_EOIR_RESET);

    if (oir_debug) {
	buginf("\ncontroller %s reset, state=%d, eoir=%d, cdb=%#x",
	       cdb->cdb_namestring, cdb->cdb_state, eoir, cdb);
    }

    /*
     *	During on-line insertion, we get called before the system knows
     *  we are here.  Wait until they are really ready for us.
     */
    if (platform_mip_carve_in_progress(cdb))
	return;

    /*
     *  The NOHARDWARE probably can never happen.  The CRASHED part will
     *	happen any time you do a clear controller on a crashed card.
     */
    if (cdb->cdb_status & (CDB_NOHARDWARE | CDB_CRASHED))
	return;

    /*
     * Don't allow EOIR on MIP hardware version that will not support it
     */
    if (eoir) {
	if (!mip_eoir_capable_hw_version(cdb)) {
	    ushort hw_version;

	    eoir = FALSE;
	    hw_version = dbus_hw_version(pslot_to_dbus(cdb->slot));
	    errmsg(&msgsym(NOEOIR, OIR), cdb->cdb_namestring, "hardware",
		   (hw_version >> 8), (hw_version & 0xFF));
	} else {
	    if (oir_debug)
		buginf("\nPerforming eoir reset of %s", cdb->cdb_namestring);
	}
    }

    /*
     *  This one is just a sanity check.  It should never happen.
     */
    if ((mip = mip_find_by_unit(cdb->unit)) == NULL || mip == &null_mip_entry)
	return;

    if (serial_debug)
	buginf("\n%s reset", cdb->cdb_namestring);
    
    status = raise_interrupt_level(NETS_DISABLE);

    sresult = mip_select_if(mip);
    if (sresult != FCI_RSP_OKAY) {
	mip_its_dead_jim(mip, sresult);
	reset_interrupt_level(status);
	return;
    }

    /*
     * Don't disable the MIP port during EOIR. Don't flap interface
     */
    if (!eoir) {
	mip_controller_status(mip, FCI_STATUS_DISABLE);
	/* should not change t1 loop status during eoir */
	mip_controller_status(mip, FCI_STATUS_NOLOOP);
    }

    /*
     * Call a platform specific routine to take care of the platform
     * specific parts of re-initialising a plughole.  For example,
     * on the c7000 we'll assign pool indexs, set up buffer parking etc.
     */
    platform_mip_restart_init(mip);

    /*
     * If we are in EOIR event, we need to tell the MIP that it
     * can use MEMD again. We've already told it about its queues
     * and buffers.
     */
    if (eoir) {
	/* 
	 * Reset channel interfaces before we unquiesce the mip
	 * controller so the mip can start working on packet
	 * once it receives an unquiesce command.
	 */
	hwidbtype *vc_hwidb;
	int vc;

	for (vc = 0; vc <= MIP_MAX_VC; vc++) {
	    vc_hwidb = mip->hwidb[vc];
	    if (vc_hwidb) {
		/*
		 * We have to use the MIP interface reset routine directly,
		 * because hwidb->reset may point to dialer_disconnect()!
		 */
		vc_hwidb->oir_flags |= EOIR_RESET;
		mip_interface_reset(vc_hwidb);
		vc_hwidb->oir_flags &= ~EOIR_RESET;
	    }
	}
	mip_controller_status(mip, FCI_STATUS_UNQUIESCE);
    } else {
	if (cdb->cdb_state == IDBS_ADMINDOWN) {
	    if (oir_debug)
		buginf("\nDisabling MIP %s because cdb shutdown",
		       mip->cdb->cdb_namestring);
	    mip_controller_status(mip, FCI_STATUS_DISABLE);
	} else {
	    if (oir_debug)
		buginf("\nEnabling MIP %s",
		       mip->cdb->cdb_namestring);
	    mip_controller_status(mip, FCI_STATUS_ENABLE);
	}
    }
    mip_simple_command(mip, 
		       loveletters_throttled ? FCI_CMD_NO_LOVELETTERS
		                             : FCI_CMD_OK_LOVELETTERS);
    reset_interrupt_level(status);

    /*
     * Now reset all dsx1 information if not an eoir reset
     */
    if (!eoir) {
	dsx1_reset(cdb);
    }   
}

/*
 * The next few routines are stub routines to set loopback, framing,
 * clock source, line code, and cablelength from the dsx1 code
 */
static void mip_set_loopback (dsx1info *dsx1)
{
    ushort tosend;

    switch (dsx1->loop) {
    case DSX1_LOOP_LOCAL:
	tosend = FCI_STATUS_LOOPLOCAL;
	break;
    case DSX1_LOOP_REMOTE:
	tosend = FCI_STATUS_LOOPREMOTE;
	break;
    default:
	tosend = FCI_STATUS_NOLOOP;
	break;
    }
    if (dsx1->cdb->cdb_state != IDBS_ADMINDOWN) {
	leveltype level;
	mipinfo*  mip = mip_find_by_unit(dsx1->unit);

	level = raise_interrupt_level(NETS_DISABLE);
	mip_select_if(mip);
	mip_controller_status(mip, tosend);
	reset_interrupt_level(level);
    }
}

static void mip_set_framing (dsx1info *dsx1)
{
    mipinfo  *mip;
    int level;

    mip = mip_find_by_unit(dsx1->unit);
    level = raise_interrupt_level(NETS_DISABLE);
    mip_select_if(mip);
    mip_command_arg(mip, FCI_CMD_FRAME_TYPE, dsx1->framing);
    mip_command_arg(mip, FCI_CMD_HOMOL_TYPE, dsx1->homologation_type);
    reset_interrupt_level(level);
}

static void mip_set_clock_source (dsx1info *dsx1)
{
    mipinfo  *mip;
    int level;

    mip = mip_find_by_unit(dsx1->unit);
    level = raise_interrupt_level(NETS_DISABLE);
    mip_select_if(mip);
    mip_command_arg(mip, FCI_CMD_CLOCKTYPE, dsx1->clocking);
    reset_interrupt_level(level);
}

static void mip_set_linecode (dsx1info *dsx1)
{
    mipinfo  *mip;
    int level;

    mip = mip_find_by_unit(dsx1->unit);
    level = raise_interrupt_level(NETS_DISABLE);
    mip_select_if(mip);
    mip_command_arg(mip, FCI_CMD_LINECODE_TYPE, dsx1->linecode);
    reset_interrupt_level(level);
}

static void mip_set_cablelength (dsx1info *dsx1)
{
    mipinfo  *mip;
    int level;

    mip = mip_find_by_unit(dsx1->unit);
    level = raise_interrupt_level(NETS_DISABLE);
    mip_select_if(mip);
    mip_command_arg(mip, FCI_CMD_CABLE_LENGTH, dsx1->cablelength);
    reset_interrupt_level(level);
}


/*
 * mip_map_channel
 * This routine will actually map the mip channel-group command.  It
 * is a stub routine from the dsx1 code.
 * It should be called with an idb already set, then have an idb->reset
 * follow it.
 */
static void mip_map_channel (dsx1info	*dsx1,
			     hwidbtype 	*hwidb,
			     int	channel,
			     ulong	bitfield,
			     int	subrate)
{
    int status, translated;
    ushort sresult;
    mipinfo *mip;
    ulong old_bitfield = bitfield;

    if (hwidb == NULL)
	return;
    mip = mip_find_by_unit(dsx1->unit);
    mip->hwidb[channel] = hwidb;
    hwidb->vc = channel;

    /* do the math before disabling interrupts */
    /* Subrate gets added for T1 only.  E1 will need seperate command */
    if (mip->applique_type == APPL_T1) {
	bitfield |= subrate;		/* set the speed for T1 links */
    } else {
	/*
	 * This is sorta gross, but needed.  The bitfield calculated
	 * will give positions 0 to 30.  We need 1 to 31 for the MIP
	 */
	bitfield >>= 1;
    }
    translated = ((bitfield & 0xffff) << 16) | ((bitfield & 0xffff0000) >> 16);
    
    status = raise_interrupt_level(NETS_DISABLE);
    
    /*
     * No need to enable vc before the controller is
     * configured. After the controller is configured,
     * all channel will be remapped and vc will be enabled.
     */
    if (mip_platform_configured_check()) {
	/* send the command to the MIP */
	sresult = mip_select_vc(mip, channel);
	if (sresult != FCI_RSP_OKAY) {
	    mip_its_dead_jim(mip, sresult);
	    reset_interrupt_level(status);
	    return;
	}

	if (mip->applique_type != APPL_T1) {
	    mip_command_arg(mip, FCI_CMD_VC_SUBRATE, subrate);
	}
	mip_command_larg(mip, FCI_CMD_VC_MAPENABLE, translated);
    }

    mip_platform_map_channel(mip, hwidb, old_bitfield);

    if (!old_bitfield) {
	/*
	 * Channel is being removed; we are done as far as vc concern.
	 */
	reset_interrupt_level(status);
	return;
    }

    /*
     * channel may get remapped during a B channel
     * setup. If is is the case, just let isdn call
     * path to take care interface state.
     *
     * During an oir, the interface will be reset
     * per platform as an insertion operation.
     *
     * Don't need to reset interface if it is in
     * nvram configuration operation during the system init.
     */
    if ((!hotstall_flag) && 
	(mip_platform_configured_check())) {    
        if ((!is_isdn(hwidb) && (hwidb->state != IDBS_ADMINDOWN))) {
	    hwidb->state = IDBS_DOWN;

	    /*
	     * We are ready to bring up the interface if the controller
	     * is up and channel is not being removed.
	     */
	    if (mip->cdb->cdb_state == IDBS_UP)
	        dsx1_change_idb_state(hwidb, IDBS_UP);
	}
	else {
	    /*
	     * In the RSP platform, need to inform the mip of channel's tx acc
	     * and set tx acc with tql limit after the B channel remap.
	     */
	    mip_platform_if_reset(mip, hwidb);
	}
    }

    reset_interrupt_level(status);

}

static void mip_set_transmit_delay (hwidbtype *idb, int value)
{
    leveltype level;
    mipinfo *mip;
    
    mip = mip_find_by_idb(idb);

    /* send the command to the MIP */
    if (value > MAX_MIP_TX_DELAY)
	value = MAX_MIP_TX_DELAY;
    
    idb->xmitdelay = value;
    if (value > 0) value--;	/* adjust for mip */
    level = raise_interrupt_level(NETS_DISABLE);
    
    mip_select_vc(mip, idb->vc);
    mip_command_larg(mip, FCI_CMD_DELAY, value);

    reset_interrupt_level(level);
}

/*
 * mip_set_hw_encapsulation
 *
 * Tell the card about the encapsulation type in use.  This is used by SSE
 * switching to parse the packet.
 */
static void mip_set_hw_encapsulation (hwidbtype *hwidb)
{
    mipinfo *mip;
    ushort sresult;
    int status;

    mip = mip_find_by_idb(hwidb);
    status = raise_interrupt_level(NETS_DISABLE);

    sresult = mip_select_vc(mip, hwidb->vc);
    if (sresult != FCI_RSP_OKAY) {
	mip_its_dead_jim(mip, sresult);
	reset_interrupt_level(status);
	return;
    }

    if (hwidb->enctype == ET_FRAME_RELAY)
	mip_command_arg(mip, FCI_CMD_VC_ENCAP, ET_FR_IETF);
    else
	mip_command_arg(mip, FCI_CMD_VC_ENCAP, hwidb->enctype);

    reset_interrupt_level(status);
}


/*
 *  Reset a single interface.  This is a stripped down version of
 *  cbus_mci_serial_reset().  Use return_nothing() as a
 *  dummy routine to replace the regular reset vector in the idb
 *  when we are changing encapsulations
 */

static void mip_interface_reset (hwidbtype *hwidb)
{
    mipinfo	*mip;
    boolean	eoir;
    leveltype	status;
    ushort	sresult;

    SAVE_CALLER();

     if (hwidb->vc == NO_VC) {
 	/*
 	 * We are even called before the channel is mapped.
 	 * Return now; otherwise we will be hurt again.
 	 */ 	
 	if (cbus_debug) {
  	    buginf("\nMIP %s reset before channel mapped\n",
  		   hwidb->hw_namestring);
  	    bugtrace();
  	}
 	return;
     }
 
    /*
     * Don't want to reset the interface before the system
     * is configured during the system initialization.
     */
    if (!mip_platform_configured_check())
        return;

    eoir = (hwidb->oir_flags & EOIR_RESET);
    mip = mip_find_by_idb(hwidb);

    /*
     *	This card has been declared crashed.  Don't even try to reset it.
     */
    if (mip->cdb->cdb_status & CDB_CRASHED)
	return;
    
    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    /*
     * During an eoir, it will be reset before the controller 
     * is UNQUIESCEd when the controller is being reset. 
     */
    
    if (oir_debug) {
	buginf("\ninterface %s reset, state=%d, pre_hotstall_state=%d, eoir=%d\n hwidb=%#x, cdb=%#x, mip=%#x",
	       hwidb->hw_namestring, hwidb->state, hwidb->pre_hotstall_state,
	       eoir, hwidb, mip->cdb, mip);
    } else if (serial_debug) {
	buginf("\n%s: Reset from 0x%x", hwidb->hw_namestring, caller());
    }

    hwidb->counters.resets++;
    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * Don't wipe our route cache entries or reenable a disabled
     * interface if we are EOIRing
     */
    if (!eoir) {
	hwidb->status &= ~IDB_DISABLED;
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    }

    sresult = mip_select_vc(mip, hwidb->vc);
    if (sresult != FCI_RSP_OKAY) {
	mip_its_dead_jim(mip, sresult);
	reset_interrupt_level(status);
	return;
    }
    
    /*
     * Do not disable the interface if another card is being
     * removed or is being inserted 
     */
    if (!eoir) 
	mip_command_arg(mip, FCI_CMD_VC_STATUS, MCI_IF_DISABLE);

    /*
     * We should already have been assigned to a buffer pool, datagramsize
     * and love note pool.  Set the rx offset, and stuff for bridging
     */

    platform_mip_set_offsets(hwidb);

    /*
     * Don't flush output hold queue if we are EOIRing
     * (another card is inserted or is removed)
     * The interface state will not be changed if
     * we are EOIRing.
     */
    if (!eoir) {

	/*
	 * Flush hold queue, set transmit delay
	 */
	holdq_reset(hwidb);
	mip_set_transmit_delay(hwidb, hwidb->xmitdelay);

	/*
	 * Save off our applique name
	 */
	mci_applique(hwidb);

	/*
	 * Exit if the interface is administratively down,
	 * or if the controller is not up
	 */
	if ( (hwidb->state == IDBS_ADMINDOWN) ||
	    (mip->cdb->cdb_state != IDBS_UP) ) {
	    reset_interrupt_level(status);
	    return;
	}
	hwidb->state = IDBS_UP;
    }

    /*
     * Try to bring it up, unless it was just inserted and
     * was previously not shut down, in which case, shut it down
     * so that restart_reinsertions() will treat this interface
     * like any other reinserted interface.
     */
    if ( (slots[hwidb->slot].flags & BOARD_RELOADED) &&
	(hwidb->pre_hotstall_state != IDBS_ADMINDOWN) ) {
	hwidb->state = IDBS_ADMINDOWN;
	if (oir_debug)
	    buginf("\nShutting down %s so card reinsertions can restart it",
		   hwidb->hw_namestring);
    }
    
    /*
     * don't change interface loop state during an eoir
     */
    if (!eoir) {
	/*
	 * Set loopback state if it should be looped
	 */
	if ((hwidb->ext_loop_type == DSX1_LOOP_REMOTE) &&
	    (dsx1_count_timeslots(dsx1_find_by_unit(mip->unit), hwidb->vc) != 1)) {
	    hwidb->ext_loop_type = DSX1_LOOP_NONE;
	    if (serial_debug)
	      buginf("\n%s - can't do remote loop w/ multiple timeslots",
		     hwidb->hw_namestring);
	}
	if (hwidb->loopback || (hwidb->ext_loop_type == DSX1_LOOP_REMOTE) ) {
	    if (hwidb->loopback)
	      mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPLOCAL);
	    else
	      mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPREMOTE);
	}
    }

    /*
     * Don't let interface that was just EOIRed transmit timeout
     * right away due to time we spent recovering. 
     */
    if (eoir) {
      TIMER_STOP(hwidb->xmittimer);
    }

    /*
     * Prime lastoutput to prevent a false outhang assertion.
     */
    GET_TIMESTAMP(hwidb->lastoutput);    

    /*
     * In the RSP platform, interface's tx_acc has been
     * reallocated and it's tql will be set.
     *
     * Want to set up the accumulator once the configuration is done
     * during the system initialization. The interface can be reset
     * as a result of controller reset after the configuration is done.
     * If we don't set up the accumulator ASAP, the interface
     * may be asserted as an outhang interface.
     *
     */
    mip_platform_if_reset(mip, hwidb);

    /*
     * Interface state will not be changed during an eoir
     */
    if (!eoir) {
	if (hwidb->state == IDBS_UP) {
	    /*
	     * Re-enable interface.
	     */
	    if (oir_debug)
	      buginf("\nEnabling VC for hwidb %#x on %s",
		     hwidb, mip->cdb->cdb_namestring);
	    mip_command_arg(mip, FCI_CMD_VC_STATUS, MCI_IF_ENABLE);
	}
    }

    if (!eoir) {
        if (hwidb->loopback)
	    mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPLOCAL);
	else if (hwidb->ext_loop_type == DSX1_LOOP_REMOTE)
	    mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPREMOTE);
	else
	    mip_command_arg(mip, FCI_CMD_VC_STATUS, MCI_IF_NOLOOPBACK);
    }

    reset_interrupt_level(status);

    mip_set_hw_encapsulation(hwidb);
    
}

/*
 * mip_set_maxdgram
 * set maxdgram for a VC - If this changes the maximum mtu for
 * all VC's on a controller, change for the controller also
 */
static boolean mip_set_maxdgram (hwidbtype *idb, int bufsize, int maxdgram)
{
    mipinfo *mip;
    int status, ix;
    ushort high_mtu;
    ulong high_dgram, high_bufsize;
    
    /* send the command to the MIP */
    mip = mip_find_by_idb(idb);
    status = raise_interrupt_level(NETS_DISABLE);
    
    mip_select_vc(mip, idb->vc);
    mip_command_larg(mip, FCI_CMD_VC_MTU, maxdgram);

    reset_interrupt_level(status);

    if (maxdgram > mip->cdb->max_buffer_size ||
	bufsize > mip->cdb->buffersize ||
	idb->maxmtu > mip->cdb->maxmtu) {
	mip->cdb->max_buffer_size = maxdgram;
	mip->cdb->buffersize = bufsize;
	mip->cdb->maxmtu = idb->maxmtu;
	platform_mip_update_carve_dbase(mip->cdb);
	return(cbus_mci_set_maxdgram(idb, bufsize, maxdgram));
    } else {
	/*
	 * Set these here - cbus_mci_set_maxdgram won't execute if
	 * there are no perceived changes
	 */
	if_maxdgram_adjust(idb, maxdgram);
	idb->buffersize = bufsize;

	/* did we just lower our overall requirements? */
	high_mtu = high_dgram = high_bufsize = 0;
	for (ix = 0 ; ix < MIP_MAX_VC + 1 ; ix++) {
	    if (mip->hwidb[ix] == NULL)
		continue;
	    high_mtu     = max(high_mtu, mip->hwidb[ix]->maxmtu);
	    high_dgram   = max(high_dgram, mip->hwidb[ix]->max_buffer_size);
	    high_bufsize = max(high_bufsize, mip->hwidb[ix]->buffersize);
	}
	/*
	 * Make sure we don't go any smaller than the minimum love
	 * letter size.  On individual channels we can, but not on
	 * the cdb.  This is only relevant on platforms that use
	 * data buffers to carry loveletters, so we call a platform
	 * specific adjuster routine and let it do the right thing.
	 */
	platform_mip_adjust_mtu_for_love(mip, &high_mtu, &high_bufsize, 
					 &high_dgram);

        if (high_dgram < mip->cdb->max_buffer_size ||
	    high_bufsize < mip->cdb->buffersize ||
	    high_mtu < mip->cdb->maxmtu) {
	    
            mip->cdb->max_buffer_size = high_dgram;
            mip->cdb->buffersize = high_bufsize;
            mip->cdb->maxmtu = high_mtu;

	    platform_mip_update_carve_dbase(mip->cdb);
	    platform_demand_complex_restart(idb);
            return(TRUE);
	}
    }
    
    return(TRUE);
}

/************************************************************************
 *									*
 *		      MIP encapsulation routines			*
 *									*
 ************************************************************************/
/*
 * mip_board_encapsulation
 * add the board level MIP encapsulation
 */
static int mip_cache_board_encap (hwidbtype *hwidb, void *loc, int max)
{
    if (max < MIP_ENCAPSIZE_BYTES)
	return(-1);
    PUTSHORT(loc, hwidb->vc);
    return(MIP_ENCAPSIZE_BYTES);
}

static boolean mip_board_encapsulation (paktype *pak, hwidbtype *hwidb)
{
    pak->encsize += MIP_ENCAPSIZE_BYTES;
    pak->datagramsize += MIP_ENCAPSIZE_BYTES;
    pak->datagramstart -= MIP_ENCAPSIZE_BYTES;
    if (pak->datagramstart < pak->data_area)
	return(FALSE);
    PUTSHORT(pak->datagramstart, hwidb->vc);
    return(TRUE);
}

static void mip_set_encapsulation (hwidbtype *hwidb)
{
    hwidb->board_encap = mip_board_encapsulation; /* board encap for level 2 */
    hwidb->cache_board_encap = mip_cache_board_encap;
    platform_mip_adjust_encsize(hwidb);
    mip_set_hw_encapsulation(hwidb);
}

/*
 * mip_encapsulation
 * Set the encapsulation for a MIP card.  We call the serial_setencap()
 * routine to get the hdlc/ppp/x25/etc. encapsulation.  We then call
 * a routine to save off the encapsulation pointers and replace them
 * with the above encap routines, which will call the original encap,
 * and then another to write in the added encap needed for the MIP.
 * We also do something really kludgy here - we save off the reset
 * vector of the idb for the call to serial_setencap().  This makes
 * sure that we don't call idb->reset until after we call the
 * mip_set_encasulation() routine.
 */
static void mip_encapsulation (parseinfo *csb)
{
    hwidbtype *member_idb;
    idbtype *idb;
    hwidbtype *hwidb;
    mipinfo *mip;
    reset_t reset_v;
    reset_t mem_reset_v;		/* save off old vector */
    encapstype encap;
    dsx1info *dsx1;
    dialerdbtype *ddb, *member_ddb;

    idb = csb->interface;
    hwidb = idb->hwptr;
    mip = mip_find_by_idb(hwidb);
    reset_v = hwidb->reset;	        /* save off old vector */
    hwidb->reset = (reset_t) return_nothing;
    
    /* let the serial encaps routine do the hard work */
    if (!csb->sense) {
	/* this is the NO encap path - resto */
	encap = hwidb->enctype_def;
    } else {
	encap = (enum ET) GETOBJ(int,1);
    }
    
    if (!(is_isdn(hwidb))) {
	serial_setencap(csb, idb, encap, TRUE);
	mip_set_encapsulation(hwidb);
    } else {
	/*
	 * This is a PRI interface.
	 */
	dsx1 = dsx1_find_by_unit(mip->unit);
	if (hwidb == dsx1->isdn_d_chan_hwidb) {
	    reg_invoke_isdn_pri_setencap(csb);
	    mip_set_encapsulation(hwidb);
	    reg_invoke_pri_member_adjust_encap(hwidb, encap);
	} else {
	    hwidb->reset = reset_v; /* restore vector */
	    return;
	}
    	    	
	ddb = get_dialerdb(hwidb);
	for (member_ddb = ddb->hunt_group_first;
	     member_ddb;
	     member_ddb = member_ddb->hunt_group_next) {
	    member_idb = member_ddb->ownidb;
	    mem_reset_v = member_idb->reset; /* save off old vector */
	    member_idb->reset = (reset_t) return_nothing;
	    mip_set_encapsulation(member_idb);
	    reg_invoke_pri_member_adjust_encap(member_idb, encap);
	    member_idb->reset = mem_reset_v;
	    if (member_idb->reset != mip_interface_reset)
		mip_interface_reset(member_idb);
	    (*member_idb->reset)(member_idb); /* and reset idb  */
	}
    }
    /* Both cases will reset hwidb */
    hwidb->reset = reset_v;     /* restore vector */
    (*hwidb->reset)(hwidb);     /* and reset idb  */
}

/************************************************************************
 *									*
 *	       MIP idb initialization/manipulation routines		*
 *									*
 ************************************************************************/

/*
 * mip_create_idb
 * This will create an idb for a mip interface.
 * It does not map the channels
 */
static hwidbtype *mip_create_idb (cdbtype *cdb, hwidbtype *hwidb, int channel)
{
    mipinfo *mip;
    idbtype *swidb;
    boolean  put_on_Q;
    boolean  idb_init_done;
    int	     status;
    serialsb *ssb;
    
    put_on_Q = idb_init_done = FALSE;

    put_on_Q = FALSE;
    mip = mip_find_by_unit(cdb->unit);

    /* now go out and build the idb's for this guy */
    if (hwidb == NULL) {
	if ((hwidb = dequeue(&mip_idbQ)) == NULL) {
	    if ((hwidb = idb_create()) == NULL) {
		return(NULL);
	    }
	} else {
            /* 
             * Since there's no standard way to free an IDB, we need to
             * reuse the freed IDB in our private queue. And also there's
             * no standard way to clean the IDB, we need to bzero out the
             * IDB section to ensure that there's no old information left
             * in the reused IDB.
             *
             * we better zero them out, just to be safe 
             */
	    swidb = hwidb->firstsw;
	    idb_clear_dangerous(hwidb);
	    idb_init_all(hwidb, swidb);
	}
	put_on_Q = TRUE;
	/*
	 * Increment nnets.  nserialnets is already incremented in
	 * cbus_mci_idb_init(), which is called below
	 */
	nnets++;
    } else if (hwidb->status & IDB_NOHARDWARE) {
	/* card reseated after hot swap */
	hwidb->status &= ~IDB_NOHARDWARE;
	idb_init_done = TRUE;
    } else {
	/*
	 *  Chain the hwidb back into the platform specific data structure.
	 *  After hot swap of another board, or a microcode reload, this
	 *  structure is nuked if the MIP doesn't support EOIR.
	 *  We need to guarantee our entries are there.  The idb chain
	 *  is still valid from before, we just need to set the pointer to
	 *  the head of the chain.
	 *
	 *  If the entry is still there, then this must simply be a
	 *  remap of an existing channel.  This is perfectly legal (and
	 *  fairly common).  These routines are smart enough to deal with
	 *  this.
	 *
	 * Also - don't go through all the initialization below.
	 */
	platform_chain_hwidb_in(cdb, hwidb);
	idb_init_done = TRUE;
    }

    if (idb_init_done == FALSE) {

	status = raise_interrupt_level(NETS_DISABLE);

	if (platform_mip_dynamic_idb_init(cdb, mip, hwidb)) {
	    reset_interrupt_level(status);
	    enqueue(&mip_idbQ, hwidb);
	    return(NULL);
	}

	if (hwidb->loopback || (hwidb->ext_loop_type == DSX1_LOOP_REMOTE) ) {
	    if (hwidb->loopback)
	        mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPLOCAL);
	    else
	        mip_command_arg(mip, FCI_CMD_VC_STATUS, FCI_STATUS_LOOPREMOTE);
	
	}

	if (cdb->input_throttled) {
	    idb_mark_throttled(hwidb);
	} else {
	    idb_mark_unthrottled(hwidb);
	}
	hwidb->vc = channel;
	hwidb->status |= IDB_VIRTUAL_HW;
	hwidb->set_maxdgram = mip_set_maxdgram;
	hwidb->setencap = mip_encapsulation;
	mip_set_encapsulation(hwidb);
    
	idb_final_hw_init(hwidb);
	idb_final_sw_init(hwidb->firstsw);
	load_idb_buffer_info(hwidb, cdb);
	hwidb->cbus_burst_count = cdb->cbus_burst_count;
	hwidb->max_buffers = cdb->max_buffers;
	ieee_copy(default_mac_addr, hwidb->hardware);
	ieee_copy(default_mac_addr, hwidb->bia);

	hwidb->device_periodic = NULL;
	ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	ssb->serial_restartdelay = ssb->serial_restartdelay_def = 0;
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	reg_add_media_transmit_delay(hwidb->type,mip_set_transmit_delay,
				 "mip_set_transmit_delay");


	/* 
	 * Don't want to reset interface until accmulator and
	 * txlimit are set. 
	 */
	hwidb->state = IDBS_DOWN;

	if (put_on_Q) {
 	    /*
 	     * VC will be set again when the channel is
 	     * mapped. We set it to NO_VC to avoid some
 	     * unneeded interface resets triggered by
 	     * init_new_hwidb.
 	     */
 	    hwidb->vc = NO_VC;
	    init_new_hwidb(hwidb);
	    media_idb_enqueue(hwidb);
	}

	reg_invoke_ifmib_create_stacklink(hwidb->snmp_if_index,
					  cdb->snmp_if_index);

	reset_interrupt_level(status);

    } /* idb_init_done */
    /* 
     * In RSP platform, each channel will be allocated a txq accumulator
     * for flow control. The allocation is done during channel creation.
     * The accumulator will be returned when the channel is deleted.
     */
    if (mip_platform_idb_create(hwidb) == FALSE) {
	/* 
	 * The failure will only occur if we run out of
	 * accumulator when a new channel group is being
	 * added.
	 */
	mip_delete_idb(cdb, hwidb);
	return (NULL);
    }

    return(hwidb);
}

/*
 * mip_idb_init
 * MIP specific IDB initialization
 */
static void mip_idb_init (cdbtype *cdb, hwidbtype *idb)
{
    idb->type = mip_idbtype(cdb->unit);
    idb->show_serial_state = mip_show_serial_state;
    idb->reset = mip_interface_reset;
    idb->unit = idb->slotunit;
    idb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
    idb->ext_loop_type = DSX1_LOOP_NONE;
    idb->txqlength = mip_platform_get_txqlength;
}



/*
 * mip_delete_idb
 * removes the idb from the mip driver and unlinks it
 */
static void mip_delete_idb (cdbtype *cdb, hwidbtype *hwidb)
{
    mipinfo *mip;
    int status;
 
    /*
     * In RSP platform, we need to return allocated txq accumulator
     * when the channel is deleted.
     */
    mip_platform_idb_delete(hwidb);
   
    mip = mip_find_by_unit(cdb->unit);

    delete_interface(hwidb->firstsw);
    reg_invoke_ifmib_destroy_stacklink(hwidb->snmp_if_index,
				       cdb->snmp_if_index);
    (void) reg_invoke_ifmib_deregister_hwidb(hwidb);

    /*
     * subtract from the number of serial nets, and the number of total nets
     */
    nserialnets--;
    nnets--;
    mip->hwidb[hwidb->vc] = NULL;
    status = raise_interrupt_level(ALL_DISABLE);
    platform_unchain_hwidb(cdb, hwidb);
    reset_interrupt_level(status);
    serial_idb_dequeue(hwidb);
    idb_unlink(hwidb);
    enqueue(&mip_idbQ, hwidb);
    hwidb->vc = NO_VC;
}

/************************************************************************
 *									*
 *	       		MIP dead card routines				*
 *									*
 ************************************************************************/
/*
 * mip_its_dead_jim
 *
 * Called when a MIP card does not answer a MALU command.  Always called
 * with interrupts disabled.
 */
void mip_its_dead_jim (mipinfo *mip, ushort result)
{
    cdbtype	*cdb;
    boolean	ignore;			/* Ignore this error and return */
    char	*reason;		/* Reason to ignore this error  */

    cdb = mip->cdb;
    ignore = FALSE;
    reason = "";

    /*
     * Do not shoot the card in the foot if we
     * are in the middle of a hotswap event!
     */
    if (hotstall_flag) {
	ignore = TRUE;
	reason = "HOTSWAP";
    }

    /*
     * Was the card previously removed? Or, had it previously crashed?
     * In either case, ignore this error
     */
    if (cdb->cdb_status & (CDB_NOHARDWARE | CDB_CRASHED) ) {
	ignore = TRUE;
	if (cdb->cdb_status & CDB_NOHARDWARE)
	    reason = "NOHARDWARE";
	else if (cdb->cdb_status & CDB_CRASHED)
	    reason = "CRASHED";
    }

    /*
     * Was the card JUST removed?
     */
    if (slots[cdb->slot].flags & (BOARD_NONEXIST | BOARD_INVALID)) {
	ignore = TRUE;
	if (slots[cdb->slot].flags & BOARD_NONEXIST)
	    reason = "NONEXIST";
	else if (slots[cdb->slot].flags & BOARD_INVALID)
	    reason = "INVALID";
    }
    
    /*
     * If there is any reason to ignore the error, ignore it
     */
    if (ignore) {
	if (oir_debug) {
	    buginf("\n%s in slot %d did not respond, due to %s",
		   cdb->cdb_namestring, cdb->slot, reason);
	}
	return;
    } else {

	if (oir_debug) {
	    buginf("\n%s in slot %d was not responding",
		   cdb->cdb_namestring, cdb->slot);
	}
    }

    /*
     *	Has this controller flaked within the last 10 minutes?  If so, shut
     *  down the entire card.  If not, step on the complex.  If the cbus
     *  is in the process of initializing, shut down the card no matter
     *  what.
     */
#define	MIP_FAIL_ALLOW_TIME	(10 * ONEMIN)

    if (platform_mip_reset_in_progress(cdb) ||
	    CLOCK_IN_INTERVAL(mip->lastfault, MIP_FAIL_ALLOW_TIME)) {
	mip_shut_down_card(mip->slot, result);
    } else {
	errmsg(&msgsym(DAUGHTER_NO_RSP, CBUS), mip->slot, mip->appl_no,result);
	GET_TIMESTAMP(mip->lastfault);
	platform_mip_request_bus_reset(cdb);
    }
}

static void mip_shut_down_card (int slot, ushort result)
{
    int ix;
    mipinfo *mip;

    errmsg(&msgsym(CRASHED, CONTROLLER), slot, result);

    /*
     *	Find all the MIP controllers on this board and shut them down
     */
    for (ix = 0 ; ix < MIP_MAX_DAUGHTERS; ix++) {

	/*
	 * We must check the cdb for non-null also, as the MIP can well
	 *  be pointing at the null_mip_entry.
	 */
	mip = mip_find_by_slot_appl(slot, ix);
	if (mip != NULL && mip->cdb != NULL) {
	    errmsg(&msgsym(UPDOWN, CONTROLLER),
		   mip->cdb->cdb_namestring, print_cdbstate(IDBS_ADMINDOWN));
	    mip->cdb->cdb_state = IDBS_ADMINDOWN;
	    mip->cdb->cdb_status |= CDB_CRASHED;
	    dsx1_change_all_idbs_state(mip->cdb, IDBS_DOWN);
	}
    }
}

/************************************************************************
 *									*
 *	       	   MIP love note/love letter handlers			*
 *									*
 ************************************************************************/
/*
 * mip_dsx1_carrier
 * Deal with a carrier transition.
 */
static void mip_dsx1_carrier (mipinfo *mip)
{
    enum IDB_STATES newstate;
    cdbtype	*cdb;
    ushort	sresult;
    ushort      status;
    int	        level;
    
    cdb = mip->cdb;

    level = raise_interrupt_level(NETS_DISABLE);
    
    status = mip_select_if(mip);
    if (status != FCI_RSP_OKAY) {
	mip_its_dead_jim(mip, status);
	reset_interrupt_level(level);
	return;
    }

    mip_simple_command_result(mip, MCI_CMD_CARRIER, &sresult);
    reset_interrupt_level(level);
    
    newstate = sresult ? IDBS_UP : IDBS_DOWN;
    if (!system_loading)
        errmsg(&msgsym(UPDOWN, CONTROLLER),
		cdb->cdb_namestring, print_cdbstate(newstate));
    
    if (cdb->cdb_state != newstate) {
	cdb->cdb_state = newstate;
	dsx1_change_all_idbs_state(cdb, newstate);
    }
}

/*
 * mip_love_note_handler
 * Process a love note from the MIP
 */
static void mip_love_note_handler (cdbtype *cdb, ushort status)
{
    mipinfo *mip;
    
    mip = mip_list[cdb->unit];
    
    if (status & FCI_COND_DCD) {
	if (mip->cdb->cdb_state != IDBS_ADMINDOWN)
	    (*mip->carrier)(mip);
	else
	    (*cdb->reset)(cdb);
    }
}

/*
 * mip_throttle_love_letters
 *
 * If we are getting inundated with love letters (stopem == TRUE), stem the
 * flow.  If we are ready to resume receiving them (stopem == FALSE), send
 * resume commands.
 */
void mip_throttle_love_letters (boolean stopem)
{
    int ix;
    mipinfo *mip;
    int level;

    for (ix = 0 ; ix < MAX_CBUS_INTERFACES ; ix++) {
	mip = mip_list[ix];
	if (mip == &null_mip_entry)
	    continue;
	level = raise_interrupt_level(NETS_DISABLE);
	mip_select_if(mip);
        mip_simple_command(mip, stopem ? FCI_CMD_NO_LOVELETTERS
			               : FCI_CMD_OK_LOVELETTERS);
	reset_interrupt_level(level);
    }
    loveletters_throttled = stopem;
}

/*
 * mip_process_vc_remote_loop
 * In order to simplify mip code, we process all 4 conditions (up and down,
 * pass and fail) for all channels at 1 time.  This routine handles
 * all channels, 1 condition at a time.
 *
 * Remote interface loopback has 4 possible states, as reflected by
 * the hwidb variables ext_loop_type and far_end_looped.
 *
 * ext_loop_type     far_end_looped  means
 * DSX1_LOOP_NONE    FALSE           no loopback
 * DSX1_LOOP_REMOTE  TRUE            remote loop in effect
 * DSX1_LOOP_REMOTE  FALSE           remote loop up request pending
 * DSX1_LOOP_NONE    TRUE            remote loop down request pending
 *
 * The remote loop responses should only be received in the last 2
 * states.
 */
static void mip_process_vc_remote_loop(dsx1info *dsx1, ulong bitmask,
				boolean up, boolean passed)
{
    int ix;
    ulong bitfield;
    hwidbtype *hwidb;
    
    FOR_ALL_DSX1_CHANNELS(ix, dsx1) {
	bitfield = 1 << ix;
	if (bitfield & bitmask) {
	    hwidb = dsx1->hwidb[ix];
	    if (hwidb) {
		errmsg(&msgsym(REMLOOP, LINK),
		       hwidb->hw_namestring,
		       up ? "up" : "down",
		       passed ? "passed" : "failed");
		if (passed)  {
		    /*
		     * We passed the request.  Set the far_end_looped
		     * field of the idb to reflect this status
		     */
		    if (up) {
			if (hwidb->ext_loop_type == DSX1_LOOP_REMOTE)
			    hwidb->far_end_looped = TRUE;
		    } else {
			if (hwidb->ext_loop_type == DSX1_LOOP_NONE) {
			    hwidb->far_end_looped = FALSE;
			}
		    }
		} else {
		    /*
		     * we failed.  Set the ext_loop_type to reflect
		     * the previous condition.
		     */
		    if (up) {
			if (hwidb->far_end_looped == FALSE)
			    hwidb->ext_loop_type = DSX1_LOOP_NONE;
		    } else {
			if (hwidb->far_end_looped)
			    hwidb->ext_loop_type = DSX1_LOOP_REMOTE;
		    }
		}
	    }
	}
    }
}
	
/*
 * mip_process_love_letter
 * will process the love letters received from the mip
 */
static void mip_process_love_letter (mipinfo *mip, paktype *pak)
{
    LoveLtrHdrType *ll;
    hwidbtype *vcidb;
    dsx1info *dsx1;
    dsx1TableType *old, *new;
    MIBDataFromMIPType *mibdata;
    int interval, cnt;
    ulong *long_ptr;
    
    ll = (LoveLtrHdrType *)&pak->datagramstart[0];
    
    switch (ll->Command ) {
	
    case SNMP_LOVE_CMD_CURR_PREV:
	mibdata = (MIBDataFromMIPType *)(ll->Data);
	
	/* Have 15 minutes worth of data.  It could either be the
	 * next interval, or a replay of the current interval.
	 * Subtract the old data from the total, and
	 * add and copy the new data
	 */
	interval = mibdata->PrevData.dsx1Interval;
	dsx1 = dsx1_find_by_unit(mip->unit);
	if (interval != dsx1->MIB.CurrentIndex) {
	    
	    /* We don't match the current - see if we match the last */
	    int temp;
	    temp = (dsx1->MIB.CurrentIndex == 0 ?
		    QUARTER_HOURS_IN_DAY : dsx1->MIB.CurrentIndex) - 1;
	    if (temp != dsx1->MIB.CurrentIndex) {
		/*
		 * We did not receive the MIP information that we
		 * expected.  Clear out any old data, and start again
		 */
		dsx1_init_mib_data(dsx1);
		dsx1->MIB.CurrentIndex = interval;
		dsx1->MIB.C.dsx1ValidIntervals = 1;
	    }
	} else {
	    /* got the correct 1 - add to the valid intervals */
	    dsx1->MIB.C.dsx1ValidIntervals++;
	    if (dsx1->MIB.C.dsx1ValidIntervals > QUARTER_HOURS_IN_DAY)
		dsx1->MIB.C.dsx1ValidIntervals = QUARTER_HOURS_IN_DAY;
	}
	/*
	 * Calculate the Total data by subtracting out the old,
	 * and adding in the new.  Also copy the new data to the
	 * old data
	 */
	old = &dsx1->MIB.Intervals[interval];
	new = &mibdata->PrevData;
	/*
	 * save the next index
	 */
	dsx1->MIB.CurrentIndex = interval + 1; /* for next time */
	if (dsx1->MIB.CurrentIndex >= QUARTER_HOURS_IN_DAY)
	    dsx1->MIB.CurrentIndex = 0;
	
	dsx1->MIB.TotalData.dsx1ESs -= old->dsx1ESs ;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1ESs, new->dsx1ESs);
	old->dsx1ESs = new->dsx1ESs ;
	
	dsx1->MIB.TotalData.dsx1SESs -= old->dsx1SESs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1SESs, new->dsx1SESs);
	old->dsx1SESs = new->dsx1SESs ;
	
	dsx1->MIB.TotalData.dsx1SEFSs -= old->dsx1SEFSs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1SEFSs, new->dsx1SEFSs);
	old->dsx1SEFSs = new->dsx1SEFSs ;
	
	dsx1->MIB.TotalData.dsx1UASs -= old->dsx1UASs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1UASs, new->dsx1UASs);
	old->dsx1UASs = new->dsx1UASs ;
	
	dsx1->MIB.TotalData.dsx1CSSs -= old->dsx1CSSs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1CSSs, new->dsx1CSSs);
	old->dsx1CSSs = new->dsx1CSSs ;
	
	dsx1->MIB.TotalData.dsx1PCVs -= old->dsx1PCVs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1PCVs, new->dsx1PCVs);
	old->dsx1PCVs = new->dsx1PCVs ;
	
	dsx1->MIB.TotalData.dsx1LESs -= old->dsx1LESs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1LESs, new->dsx1LESs);
	old->dsx1LESs = new->dsx1LESs ;
	
	dsx1->MIB.TotalData.dsx1BESs -= old->dsx1BESs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1BESs, new->dsx1BESs);
	old->dsx1BESs = new->dsx1BESs ;
	
	dsx1->MIB.TotalData.dsx1DMs -= old->dsx1DMs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1DMs, new->dsx1DMs);
	old->dsx1DMs = new->dsx1DMs ;
	
	dsx1->MIB.TotalData.dsx1LCVs -= old->dsx1LCVs;
	ADD_TO_GAUGE(&dsx1->MIB.TotalData.dsx1LCVs, new->dsx1LCVs);
	old->dsx1LCVs = new->dsx1LCVs ;
	
	/*
	 * Go through and adjust the interval numbers to make the
	 * latest set of data interval #1 (A pain, but it must be done)
	 */
	for ( cnt = 1; cnt <= dsx1->MIB.C.dsx1ValidIntervals; cnt++ ) {
	    dsx1->MIB.Intervals[interval].dsx1Interval = cnt;
	    if ( interval == 0 ) interval = QUARTER_HOURS_IN_DAY - 1;
	    else interval--;
	}
	
	/* fall through  for rest of processing */
    case SNMP_LOVE_CMD_CURR_ONLY:
	dsx1 = dsx1_find_by_unit(mip->unit);
	mibdata = (MIBDataFromMIPType *)(ll->Data);
	/* copy the config information */
	dsx1->MIB.C.dsx1TimeElapsed = mibdata->dsx1TimeElapsed;
	dsx1->MIB.C.dsx1LineStatus = mibdata->dsx1LineStatus;
	dsx1->MIB.C.dsx1LoopbackConfig = mibdata->dsx1LoopbackConfig;
	/* Just copy the whole structure */
	dsx1->MIB.CurrentData = mibdata->CurrentData;
	break;
	
    case VC_REMOTE_LOOPBACK_RESP: {
	ulong *tmp, tmpread;
	dsx1 = dsx1_find_by_unit(mip->unit);
	tmp = (ulong *)(ll->Data);
        /*
	 * response field are Good up, Good down, bad up, bad down
	 */
	tmpread = GETLONG(tmp++);
	if (tmpread)
	    mip_process_vc_remote_loop(dsx1, tmpread, TRUE, TRUE);
	tmpread = GETLONG(tmp++);
	if (tmpread)
	    mip_process_vc_remote_loop(dsx1, tmpread, FALSE, TRUE);
	tmpread = GETLONG(tmp++);
	if (tmpread)
	    mip_process_vc_remote_loop(dsx1, tmpread, TRUE, FALSE);
	tmpread = GETLONG(tmp++);
	if (tmpread)
	    mip_process_vc_remote_loop(dsx1, tmpread, FALSE, FALSE);

        break;
    }

	
    case APPL_REMOTE_BAD_RESP:
    case APPL_REMOTE_GOOD_RESP:
	dsx1 = dsx1_find_by_unit(mip->unit);
	errmsg(&msgsym(REMLOOP, CONTROLLER),
	       mip->cdb->cdb_namestring,
	       dsx1->loop == DSX1_LOOP_REMOTE ? "up" : "down",
	       ll->Command == APPL_REMOTE_BAD_RESP ? "failed" : "passed");

	if (ll->Command == APPL_REMOTE_BAD_RESP) {
	    dsx1->loop = dsx1->loop == DSX1_LOOP_REMOTE ?
		DSX1_LOOP_NONE : DSX1_LOOP_REMOTE;
        }
	break;
	
    case VC_ERRORS_LOVE_NOTE:
    case VC_ERRORS_LOVE_NOTE_W_EXTRA:
	{
	    MIP_VCErrorsType *vcerr;
	    ulong rp_delta, total_cnts;
	    if (ll->Command == VC_ERRORS_LOVE_NOTE) 
		vcerr = (MIP_VCErrorsType *)(ll->Data);
	    else
		vcerr = (MIP_VCErrorsType *)(&ll->Data[2]);

	    while (vcerr->VCnumber != -1) {
		if (vcerr->VCnumber > MIP_MAX_VC) {
		    if (serial_debug)
			buginf("\n%Illegal VC, MIP %d/%d, VC %d\n",
			       mip->slot, mip->appl_no, vcerr->VCnumber);
		    return;
		}
		vcidb = mip->hwidb[vcerr->VCnumber];

		if (vcidb == NULL) {
	    	    /* No idb ?  How did we get here ? */
	    	    if (serial_debug)
			buginf("\n%Illegal VC, MIP %d/%d, VC %d\n",
		       		mip->slot, mip->appl_no, vcerr->VCnumber);
		    return; 
		}
		/*
		 * Since the silcon switch can't afford to keep autonomous
		 * switch counts for all the different possible MIP channels,
		 * we try to calculate the autonomous switch count as the
		 * difference between the IP RX/TX counts and the counts 
		 * switched by the RP. This is cool for a 7000, but for an
		 * RSP system the IP counts obtained from the love letter
		 * can differ from the RSP switch count simply because of the
		 * number of packets in the RX/TX queue.
		 *
		 * For the RX case, we check for autonomous switching turned
		 * on. If it is, the autonomous switched count is the 
		 * difference between the MIP received count and the idb input
		 * count. This is a little non-elegant since it relies on the
		 * 7000 love letters not being in a separate event queue.
		 *
		 * For the TX case, packets in the queue show up as more
		 * packets switched by the RSP than transmitted by the IP. All
		 * we have to do is keep track of this difference and account
		 * for the packets after the IP deals with them. This allows
		 * us to accurately account for VIP distributed switching out 
		 * the IP.
		 */
		total_cnts = idb_inputs(vcidb);
		if (vcidb->ip_routecache & (IP_CBUS_ROUTECACHE |
					    IP_SSE_ROUTECACHE)) {
		    rp_delta = total_cnts
			- mip->packet_counts[vcerr->VCnumber].in_paks;
		    if (rp_delta <= vcerr->RxPackets) {
			rp_delta = vcerr->RxPackets - rp_delta;
			vcidb->counters.inputs_auto[ACCT_PROTO_OTHER] +=
			    rp_delta;
			total_cnts += rp_delta;
		    }
		}
		mip->packet_counts[vcerr->VCnumber].in_paks = total_cnts;

		total_cnts = idb_rx_cumbytes(vcidb);
		if (vcidb->ip_routecache & (IP_CBUS_ROUTECACHE |
					    IP_SSE_ROUTECACHE)) {
		    rp_delta = total_cnts
			- mip->packet_counts[vcerr->VCnumber].in_bytes;
		    if ( rp_delta <= vcerr->RxBytes ) {
			rp_delta = vcerr->RxBytes - rp_delta;
			vcidb->counters.rx_cumbytes_auto[ACCT_PROTO_OTHER] +=
			    rp_delta;
			total_cnts += rp_delta;
		    }
		}
		mip->packet_counts[vcerr->VCnumber].in_bytes = total_cnts;

		total_cnts = idb_outputs( vcidb );
		rp_delta = total_cnts
		    - mip->packet_counts[vcerr->VCnumber].out_paks;
		if ( rp_delta <= vcerr->TxPackets ) {
		    rp_delta = vcerr->TxPackets - rp_delta;
		    vcidb->counters.outputs_auto[ACCT_PROTO_OTHER] += rp_delta;
		    total_cnts += rp_delta;
		} else
		    total_cnts -= rp_delta - vcerr->TxPackets;
		mip->packet_counts[vcerr->VCnumber].out_paks = total_cnts;

		total_cnts = idb_tx_cumbytes(vcidb);
		rp_delta = total_cnts
		    - mip->packet_counts[vcerr->VCnumber].out_bytes;
		if (rp_delta <= vcerr->TxBytes) {
		    rp_delta = vcerr->TxBytes - rp_delta;
		    vcidb->counters.tx_cumbytes_auto[ACCT_PROTO_OTHER] += rp_delta;
		    total_cnts += rp_delta;
		} else
		    total_cnts -= rp_delta - vcerr->TxBytes;
		mip->packet_counts[vcerr->VCnumber].out_bytes = total_cnts;
		
		vcidb->counters.input_crc += vcerr->RxCrcErrs;
		vcidb->counters.input_error += vcerr->RxCrcErrs;
		
		vcidb->counters.input_abort += vcerr->RxAborts;
		/* aborts don't add to input_error */
		
		vcidb->counters.input_overrun += vcerr->RxOverrun
		    + vcerr->RxNoCbusBuffers;
		vcidb->counters.input_error += vcerr->RxOverrun
		    + vcerr->RxNoCbusBuffers;
		
		vcidb->counters.giants += vcerr->RxGiants;
		vcidb->counters.input_error += vcerr->RxGiants;
		if (vcerr->TxNoBuffers > 0) {
		    int		status;
		    /* This can be changed in interrupts - disable them */
		    status = raise_interrupt_level(NETS_DISABLE);
		    vcidb->counters.output_total_drops += vcerr->TxNoBuffers;
		    reset_interrupt_level(status);
		}
		
		/* Don't do anything about RxBytes/packets, txbytes/packets */
		vcerr++;
	    }
	}
	break;
    case PRINTF_LOVE_LETTER:
	buginf("\n%%MIP %d/%d: %s", mip->slot, mip->appl_no, ll->Data);
	break;

    case BAD_VC_PACKET_LOVE_LETTER:
	long_ptr = (ulong *)(ll->Data);
	errmsg(&msgsym(BADVC, CBUS), "MIP", mip->slot, mip->appl_no,
 	       GETLONG(&long_ptr[0]), GETLONG(&long_ptr[1]), 
 	       GETLONG(&long_ptr[2]), GETLONG(&long_ptr[3]),
 	       GETLONG(&long_ptr[4]), GETLONG(&long_ptr[5]),
 	       GETLONG(&long_ptr[6]), GETLONG(&long_ptr[7]));
	break;

    case VC_REMOTE_LOVE_DUMP:
	if (serial_debug) {
	    static char dumpbuf[100];
	    int ix, samecnt;
	    ulong prevlong;
	    char *tempchar;
	    ushort datagramsize;
	    datagramsize = ll->VCnumber;
	    long_ptr = (ulong *)(ll->Data);
	    buginf("\ndgramsize %d", datagramsize);
	    if ((datagramsize & 0x1) != 0) {
		datagramsize += 1;
	    }
	    datagramsize >>= 1;
	    ix = 0;
	    tempchar = dumpbuf;
	    while (datagramsize) {
		ix++;
		prevlong = GETLONG(long_ptr++);
 		datagramsize--;
		samecnt = 0;
		tempchar += sprintf(tempchar, "%08x ", prevlong);
		while (datagramsize && prevlong == GETLONG(long_ptr)) {
		    datagramsize--;
		    long_ptr++;
		    samecnt++;
		}
		if (samecnt) {
		    ix++;
		    tempchar += sprintf(tempchar, "x%d ", samecnt + 1);
		}
		if (tempchar - dumpbuf > 80 || ix == 8) {
		    buginf("\n%s", dumpbuf);
		    tempchar = dumpbuf;
		    ix = 0;
		}
	    }
	    if (ix != 0)
		buginf("\n%s", dumpbuf);
	}
	break;
    default:
	buginf("\n%%MIP %d/%d: Illegal response, cmd %d\n",
	       mip->slot, mip->appl_no, ll->Command );
	break;
    }
}

/*
 * mip_love_letter_handler_proc
 * Process a love letter from the MIP (main routine--out of interrupt)
 */
static void mip_love_letter_handler_proc (void)
{
    mipinfo *mip;
    paktype *pak;
    int index;
    int pak_count;

    process_watch_queue(mip_loveletterQ, ENABLE, RECURRING);
    
    for(;;) {
	process_wait_for_event();
	for (pak_count = PAK_SWITCHCOUNT; pak_count > 0; pak_count--) {
	    pak = process_dequeue(mip_loveletterQ);
	    if (pak == NULL)
		break;

	    index = GETLONG(&pak->datagramstart[-4]);
	    if ((mip = mip_list[index]) == NULL)
		continue;

	    /*
	     *	If we have just drained the queue, and the MIPs are
	     *  throttled, unthrottle them.
	     */
	    if (loveletters_throttled &&
		process_queue_size(mip_loveletterQ) == 0)
		mip_throttle_love_letters(FALSE);

	    /*
	     *  Now go ahead and deal with this information
	     */
	    mip_process_love_letter(mip, pak);
	    retbuffer(pak);
	}
    }
}

/************************************************************************
 *									*
 *	       	   MIP miscellanous routines				*
 *									*
 ************************************************************************/

/*
 * cardifmib_get_card_index - registry fn to get card index.
 */
int cardifmib_get_card_index (cdbtype *cdb)
{
   return (-1);
}

/*
 * cardifmib_get_slot_number- registry fn to get slot number.
 */
long cardifmib_get_slot_number (cdbtype *cdb)
{
   return (-1);
}

/*
 * cardifmib_get_port_number- registry fn to get port number.
 */
long cardifmib_get_port_number (cdbtype *cdb)
 
{
   return (-1);
}

/*
 * cardifmib_get_connector_type- registry fn to get connector_type.
 */
long cardifmib_get_connector_type (cdbtype *cdb)
{
   return (-1);
}

/*
 * mip_show_serial_state
 * Show the state of the interface.  Probably never used, but all the
 * other serial cards have one, and I don't want mine to feel left out.
 */
static void mip_show_serial_state (hwidbtype *idb, char *leader)
{
    char buffer[100];
    mipinfo *mip;

    mip = mip_find_by_idb(idb);
    switch ( mip->cdb->cdb_state) {
    case IDBS_DOWN:
	printf("%s%s", leader, "alarm present");
	break;
    case IDBS_UP:
	printf("%s%s", leader, "no alarm present");
	break;
    case IDBS_ADMINDOWN:
	printf("%s%s", leader, "administratively down");
	break;
    default:
	printf("%s%s", leader, "unknown state");
	break;
    }
    
    dsx1_show_timeslots(buffer, dsx1_find_by_unit(mip->unit), idb->vc);
    printf("\n  Timeslot(s) Used:%s, Transmitter delay is %d flags",
	   buffer, idb->xmitdelay);
    printf(", transmit queue length %d", (*idb->txqlength)(idb));
}

/*
 * Used for SMDS - since on the MIP we have part of the smds encapsulation
 * sticking past the line, we need to adjust the buffer sizes.  We do
 * this before the mip encapsulation is added to the idb->encsize.
 */
static ushort mip_smds_encap_after_line (hwidbtype *idb)
{
    return(idb->encsize - HDLC_ENCAPBYTES);
}

/*
 * mip_clear_counts
 * will clear the local counts, and send a command to the controller
 * to clear the counts accumulated on the board
 */
static void mip_clear_counts (cdbtype *cdb)
{
    mipinfo *mip;
    int level;

    mip = mip_find_by_unit(cdb->unit);
    dsx1_init_mib_data(dsx1_find_by_unit(cdb->unit));
    /*
     * send message to mip clearing counters
     */
    level = raise_interrupt_level(NETS_DISABLE);
    mip_select_if(mip);
    mip_simple_command(mip, FCI_CMD_CLR_COUNTS);
    reset_interrupt_level(level);
}

/************************************************************************
 *									*
 *		     MIP driver initialization routine			*
 *									*
 ************************************************************************/
/*
 * Set up the data area for the MIP driver.  This is called only once
 * for all MIP cards
 */
static void init_mip_driver (int max_mips)
{
    int ix;
    
    /*
     *	We initialize the mip_list to point at an entry with all null
     *  hwidb pointers.  This saves a test/branch in the mip_icb_to_idb
     *  routine, which is on the fast path.
     */
    for (ix = 0 ; ix <= MIP_MAX_VC ; ix++)
	null_mip_entry.hwidb[ix] = NULL;
    null_mip_entry.applique_type = APPL_NONE;

    mip_loveletterQ = create_watched_queue("mip loveletter queue", 0, 0);
    init_dsx1_subsystem(max_mips);

    reg_add_media_smds_encap_after_line(IDBTYPE_T1,
					mip_smds_encap_after_line,
					"smds_mip_encap_after_line");

    reg_add_media_smds_encap_after_line(IDBTYPE_E1,
					mip_smds_encap_after_line,
					"smds_mip_encap_after_line");

    reg_add_dsx1_set_loopback(mip_set_loopback, "mip_set_loopback");
    reg_add_dsx1_set_clock_source(mip_set_clock_source,"mip_set_clock_source");
    reg_add_dsx1_set_framing(mip_set_framing, "mip_set_framing");
    reg_add_dsx1_set_linecode(mip_set_linecode, "mip_set_linecode");
    reg_add_dsx1_set_cablelength(mip_set_cablelength, "mip_set_cable");
    reg_add_dsx1_map_channel(mip_map_channel, "mip_map_channel");
    reg_add_dsx1_create_idb(mip_create_idb, "mip_create_idb");
    reg_add_dsx1_delete_idb(mip_delete_idb, "mip_delete_idb");
    reg_add_dsx1_hwidb_board_encap(mip_set_encapsulation,
				   "mip_set_encapsulation");
    for (ix = 0 ; ix < MAX_CBUS_INTERFACES ; ix++)
	mip_list[ix] = &null_mip_entry;

    queue_init(&mip_idbQ, 0);
    
    cfork(mip_love_letter_handler_proc, 0L,1000,"MIP Mailbox",CONSOLE_LINENUM);
}


static void mip_subsys_init (subsystype* subsys)
{
    init_mip_driver(MAX_CBUS_INTERFACES);
    mip_platform_init(subsys);
}

/*---------------------------------------------------------------------------
 * Subsystem Header for MIP Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(mip,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      mip_subsys_init,		/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: cbus_mci",  	/* sequence list */
	      "req: cbus_mci"   	/* prerequisite list */
	      );
