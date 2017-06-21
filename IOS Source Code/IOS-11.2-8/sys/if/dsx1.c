/* $Id: dsx1.c,v 3.11.12.19 1996/09/03 22:42:30 wmay Exp $
 * $Source: /release/112/cvs/Xsys/if/dsx1.c,v $
 *------------------------------------------------------------------
 * dsx1.c - support for Channelized T1/E1, Primary rate ISDN
 *
 * 29-August, 1994, Bill May (from if_mip.c)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dsx1.c,v $
 * Revision 3.11.12.19  1996/09/03  22:42:30  wmay
 * CSCdi65789:  crash when config. pri if run out of idb - delete pri-group
 * if no more memory.
 * Branch: California_branch
 *
 * Revision 3.11.12.18  1996/08/28  12:47:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.12.17  1996/08/27  01:22:10  vdukki
 * CSCdi52688:  After clearing pri counters, sho controller shows loss of
 * signal
 * Branch: California_branch
 * Created dsx1_clear_mib_data function to clear the dsx1 MIB table. It
 * will be called when "clear counters" command is run, instead of
 * dsx1_init_mib_data().
 *
 * Revision 3.11.12.16  1996/08/22  21:47:46  vdukki
 * CSCdi65777:  Channelized E1 interface doesnt respond to ds1 MIB table
 * requests
 * Branch: California_branch
 * Modified the code to look for E1 interfaces, in addition to T1
 * interfaces, while accessing the ds1 table. Also did some code
 * cleanups.
 *
 * Revision 3.11.12.15  1996/08/07  19:04:59  wmay
 * CSCdi62173:  Setting pri-group timeslots 1-24 causes all pri calls to
 * drop - check if timeslots change before making the change
 * Branch: California_branch
 *
 * Revision 3.11.12.14  1996/07/16  18:08:04  wmay
 * CSCdi61965:  sh dialer on ISDN interfaces doesnt display all B Channels
 * Let the dialer code set up the dialer-groups in the correct order
 * Branch: California_branch
 *
 * Revision 3.11.12.13  1996/07/15  20:43:04  towu
 * CSCdi61208:  show cont t1 for cas channel does not display transmit RBS
 * status
 * Branch: California_branch
 *         o Added feature to show rbs xmit status when serial debug turn
 *         on.
 *         o Added new field to collect rbs xmit status in dsx1info
 *         structure.
 *
 * Revision 3.11.12.12  1996/07/11  22:39:21  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *            o Added new fdl command for t1 controller in order to support
 *              CSU function
 *
 * Revision 3.11.12.11  1996/07/08  22:30:59  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.11.12.10  1996/06/26  03:03:56  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.11.12.9  1996/06/16  21:12:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.11.12.8  1996/06/05  20:56:17  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * More protection on when a remote loop can be executed.  Use
 * far_end_looped field to actually indicate if a remote loop is
 * present, and display more state info in show int while remote
 * loop request is being processed
 * Branch: California_branch
 *
 * Revision 3.11.12.7  1996/05/19  03:08:48  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.11.12.6  1996/05/07  04:11:46  hrobison
 * CSCdi56472:  CSCdi46413 broke modularity if ISDN not included
 * Branch: California_branch
 * Quick fix: Change external reference to structure in ../isdn to
 * a registry call.  Code is scheduled for clean-up in CSCdi56813.
 *
 * Revision 3.11.12.5  1996/05/01  14:50:05  hrobison
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
 * Revision 3.11.12.4  1996/04/29  03:19:15  mwu
 * CSCdi55232:  channel mapping may be messed when controller reset
 * Branch: California_branch
 *
 * Revision 3.11.12.3  1996/04/27  05:49:36  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.11.12.2  1996/04/20  17:59:04  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/04/26  01:05:28  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.12.3  1996/04/23  00:32:08  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.4.12.2  1996/04/17  01:58:21  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.12.1  1996/02/23  20:22:44  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.11.12.1  1996/03/18  20:13:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.4  1996/03/13  01:35:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.16.3  1996/03/07  09:43:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.2  1996/02/20  00:56:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/13  08:12:44  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.10  1996/02/08  23:13:02  wmay
 * CSCdi48169:  pri channel nulled by channel-group command - don't allow
 * no channel-group command on ISDN interfaces.
 *
 * Revision 3.9  1996/02/07  16:12:29  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  06:03:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/31  23:16:20  wmay
 * CSCdi47928:  Pri-group will overwrite channel-group - check both
 * channel-group number and timeslots for overlaps.
 *
 * Revision 3.6  1996/01/26  01:30:04  hrobison
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
 * Revision 3.5  1996/01/25  11:18:51  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.4.16.1  1996/01/24  22:17:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/07  22:41:42  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  09:26:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/19  18:44:27  rbeach
 * CSCdi35737:  ISDN: No dialer-group defined - dialing can not
 * occur, for B channel. This is caused by CDP. CDP is disabled
 * on PRI MIP channels now.
 *
 * Revision 2.6  1995/10/04  19:37:40  wmay
 * CSCdi40145:  DS1 MIB returns bad values - initialize to good values
 *
 * Revision 2.5  1995/07/20  05:22:06  dbath
 * CSCdi37370:  when out of IDBs, no error provided, config corrupted
 * Don't ignore NULL returns from reg_invoke_dsx1_create_idb().
 * Doing so results in null pointer dereferences, and unusable
 * timeslots being configured.  Instead, give the user positive
 * feedback that they're SOL, and return immediately.  While the
 * bonnet is up, add some %'s to other printfs in this module.
 *
 * Revision 2.4  1995/06/19  21:50:57  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.3  1995/06/19  01:05:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  00:03:46  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:51:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "if_controller.h"
#include "logger.h"
#include "network.h"
#include "../les/isdn_bri.h"
#include "../wan/isdn.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"
#include "dsx1.h"
#include "dsx1_registry.h"
#include "../if/dsx1_registry.regc"
#include "if_msg_controller.c"		/* Not a typo, see logger.h */
#include "../snmp/ifmib_registry.h"

/* includes for parsing */
#include "config.h"
#include "parser.h"
#include "parser_defs_t1.h"
#include "parser_defs_e1.h"

dsx1info **dsx1_list;
long dsx1_regcode;		/* for dynamically allocated registry */
static queuetype dsx1_preQ;	/* preallocated dsx1 entries for hot-swap */
int dsx1_max_dsx1s;
static int dsx1_force_a_law = FALSE;
boolean dsx1_cas_supp = FALSE;  /* cas support flag, the default is 
						not support */

static void dsx1_turn_off_channel(dsx1info *dsx1, int channel);


/*************************************************************************
 * And now, for your enjoyment, a little lecture on numbering and
 * some definitions of terms for T1/E1 and ISDN
 *
 * Timeslots are the actual timeslots for the T1/E1.  T1 timeslots run
 * from 1 to 24.  E1 timeslots run from 0 to 31, but timeslot 0 is used
 * for framing and is unavailable.  These definitions let us map the
 * timeslots into a ulong, with timeslot 1 being the leftmost bit of
 * the ulong. (1 << (32 - timeslot)) is the mapping
 *
 * Channel-groups are number as the user wants for channelized T1/E1.
 * The user can map a channel-group number to any number/combinations
 * of timeslots using the channel-group command.  For T1, channel-group
 * run from 0 to 23, for E1, channel-groups run from 0 to 29.
 *
 * For isdn, channel-groups have a direct relationship to timeslots.
 * The channel-group for an isdn timeslot is equal to the timeslot
 * number - 1... So timeslot 1 is channel-group 0, ...
 *
 * This relationship is expressed in the ISDN_TS_TO_CHAN_GROUP() macro

 * This gives us slot/appl:15 for the E1 d channel, and slot/appl:23
 * for the T1 d channel.
 *
 * B channel numbers are how the isdn code deals with things.  These
 * are defined in the enum Chan in les/isdn_bri.h.  What happens
 * here is that the D channel has a definition of -1, then B1 to B31
 * has definitions of 0 to 30.
 */

/*
 * t1_bchan_bitfield_to_dsx1_bitfield
 * This routine will convert from what the isdn b channels to the
 * actual dsx1 bitfield required.  As you can see, for T1, there
 * is no conversion
 */
static ulong t1_bchan_bitfield_to_dsx1_bitfield (ulong bchan_bitfield)
{
    return(bchan_bitfield);
}

/*
 * t1_timeslot_to_bchan
 * This will convert from the real timeslot number to the bchannel
 * number definitions that the isdn code needs.  For T1, the
 * relationship is b-channel number is timeslot - 1
 */
static enum Chan t1_timeslot_to_bchan (ushort timeslot)
{
    if (timeslot == T1_ISDN_D_TIMESLOT)
	return(ISDN_Dchan);
    return((enum Chan)(timeslot - 1));
}

/*
 * e1_bchan_bitfield_to_dsx1_bitfield
 * This routine will convert from what the isdn b channels to the
 * actual dsx1 bitfield required.  For E1, timeslots 17-31 are
 * b channels B17-B31.  This requires that the bitfield be shifted
 * over 1 place for those timeslots.
 */
static ulong e1_bchan_bitfield_to_dsx1_bitfield (ulong bchan_bitfield)

{
    ulong temp1, temp2;

    temp1 = bchan_bitfield & 0xfffe0000;
    temp2 = bchan_bitfield & 0x0001ffff;

    return(temp1 | temp2);
}

/*
 * e1_timeslot_to_bchan
 * This will convert from the real timeslot number to the bchannel
 * number definitions that the isdn code needs.  For E1, the
 * relationship is b-channel number is timeslot - 1 for the
 * first 15 timeslots.  Timeslot 16 is the D channel, then
 * timeslots 17 to 31 are bchannels B17 to B31 (which have numerical
 * equivalents of 16 to 30).
 */
static enum Chan e1_timeslot_to_bchan (ushort timeslot)
{
    if (timeslot == E1_ISDN_D_TIMESLOT)
	return(ISDN_Dchan);
    return((enum Chan)(timeslot - 1));
}

static void e1_print_version (void)
{
    printf("Channelized E1, Version 1.0.\n");
}

/************************************************************************
 *									*
 *			Dsx1 Interface					*
 *									*
 ************************************************************************/
/*
 * Set up the data area for the driver.  This is called only once
 * for the system.  Make sure it is called before setting up any
 * of the stub registries.
 */
void init_dsx1_subsystem (int dsx1max)
{
    int ix;
    
    dsx1_list = malloc(dsx1max * sizeof(dsx1info *));
    if (dsx1_list == NULL)
	return;
    dsx1_max_dsx1s = dsx1max;
    /*
     * create the dsx1 registry.  This will set up the stubs.
     */
    dsx1_regcode = registry_create(REG_UNKNOWN, SERVICE_DSX1_MAX, "DSX1");
    if (dsx1_regcode != REG_ILLEGAL)
	create_registry_dsx1();

    reg_add_dsx1_applique_name(dsx1_applique_name, "dsx1_applique_name");
    
    for (ix = 0; ix < dsx1max; ix++) {
	dsx1_list[ix] = NULL;
    }
}

/* prepare for hot-swaps */
void dsx1_preallocate (boolean more, ushort max_dsx1)
{
    int ix;
    dsx1info *tmp;
    
    if (!more)
	queue_init(&dsx1_preQ, max_dsx1);

    for (ix = dsx1_preQ.count ; ix < max_dsx1 ; ix++) {
	tmp = malloc_named(sizeof(dsx1info), "DSX1 Info");
	if (tmp != NULL)
	    enqueue(&dsx1_preQ, tmp);
    }
}


/*
 * dsx1_create_context
 * We have found a dsx1type card - allocate a structure and fill it
 *
 * History:
 * 12/15/95   H.M. Robison   Moved increment of nprinets from
 *                           here to dsx1_setup_pri() to allow
 *                           BRI and Channelized interfaces to
 *                           work in the same router.
 */
dsx1info *dsx1_create_context (cdbtype *cdb,
			       int applique,
			       boolean ok2malloc)
{
    int ix;
    boolean init_dsx1;
    dsx1info *dsx1;

    /* Do we have a type that we recognize ? */
    switch (cdb->pif_type) {
    case PIFT_T1:
    case PIFT_E1:
	break;
    default:
	return(NULL);
    }
    if (cdb->unit >= dsx1_max_dsx1s) {
	buginf("\n%%Illegal cdb unit number %d:dsx1 context not created",
	       cdb->unit);
	return(NULL);
    }
    /*
     * Have a type we recognize - get a dsx1info structure, or recycle one
     */
    if (dsx1_list[cdb->unit] == NULL) {
	/*
	 * On the 7000 we can be called via a hot-stall interrupt, which
	 * means we cannot malloc.  So we rely on the preQ instead.
	 */
	if (ok2malloc) {
	    dsx1 = dsx1_list[cdb->unit] = malloc(sizeof(dsx1info));
	    if (dsx1 == NULL) {
		return(NULL);
	    }
	} else {
	    dsx1 = dsx1_list[cdb->unit] = dequeue(&dsx1_preQ);
	    if (dsx1 == NULL) {
		buginf("\n%%No preallocated memory: Cannot create "
		       "DSX1 context");
		return(NULL);
	    }
	    memset(dsx1, 0, sizeof(dsx1info));
	}

	for (ix = 0 ; ix <= DSX1_MAX_TS ; ix++)
	    dsx1->timeslots[ix] = UNUSED_CHAN;
	init_dsx1 = TRUE;
    } else {
	dsx1 = dsx1_list[cdb->unit];
	/*
	 * hotswap case...
	 * If the applique type has changed - re-init the card -
	 * but don't delete the hwidb's - it will retain the
	 * old ones in the various queues.
	 */
	if (dsx1->pif_type != cdb->pif_type)
	    init_dsx1 = TRUE;
	else
	    init_dsx1 = FALSE;
   } 

    dsx1->unit = cdb->unit;
    dsx1->pif_type = cdb->pif_type;
    dsx1->applique_type = (enum appl_type)applique;
    
    dsx1->cdb = cdb;
    /*
     *  If this is a board which was previously hotswapped out, then
     *  we must retain the old values.  If this is new, we must set
     *  to our defaults.
     */
    switch (dsx1->pif_type) {
    case PIFT_T1:
	if (init_dsx1) {
	    dsx1->loop = DSX1_LOOP_NONE;
	    dsx1->framing = DSX1_FRM_SF;
	    dsx1->homologation_type = DSX1_HOMOL_NORMAL;
	    dsx1->clocking = DSX1_CLK_LINE;
	    dsx1->linecode = DSX1_LC_AMI;
            dsx1->fdl = DSX1_FDL_NONE;
	    dsx1->cablelength = DSX1_LENGTH_133;
            dsx1->length = DSX1_LONG;
                if (dsx1->applique_type != APPL_T1_CSU)
                   dsx1->length = DSX1_SHORT;  /* DSX1 ver supports only short length cable */
            dsx1->gain = DSX1_GAIN36;
            dsx1->pulse = DSX1_0DB;
	    dsx1->max_channels = T1_MAXCHAN;
	    dsx1->max_timeslots = T1_MAXTS;
	    dsx1->isdn_d_chan = T1_ISDN_D_CHANNEL;
	    dsx1->isdn_d_timeslot = T1_ISDN_D_TIMESLOT;
	    dsx1->bchan_bitfield_to_dsx1_bitfield =
		t1_bchan_bitfield_to_dsx1_bitfield;
	    dsx1->timeslot_to_bchan = t1_timeslot_to_bchan;
	    dsx1->pri_timeslot_bitmap = 0;
		
	    for (ix = 0 ; ix <= DSX1_MAX_CHAN ; ix++)
		dsx1->speed[ix] = DSX1_SPEED_56;
	}
	/* Clear the MIB variable here, after the defaults are set
	 * so we catch the clocking part
	 */
	dsx1_init_mib_data(dsx1);
	add_t1_commands();	/* kick the parser while we're at it */
	/* 
	 * register this t1 controller physical interface with the
	 * ifmib 
	 */
	reg_invoke_ifmib_add_cdb_if(cdb->pif_type,cdb);
	break;
	
    case PIFT_E1:
	if (init_dsx1) {
	    dsx1->loop = DSX1_LOOP_NONE;
	    dsx1->framing = DSX1_FRM_E1_CRC;
	    dsx1->homologation_type = DSX1_HOMOL_NORMAL;
	    /*
	     * Note:  For e1, we only need line clocking.  The board
	     * will automagically handle internal clock correctly
	     */
	    dsx1->clocking = DSX1_CLK_LINE;
	    dsx1->linecode = DSX1_LC_HDB3;
	    dsx1->cablelength = DSX1_LENGTH_133;
	    dsx1->max_channels = E1_MAXCHAN;
	    dsx1->max_timeslots = E1_MAXTS;
	    dsx1->isdn_d_chan = E1_ISDN_D_CHANNEL;
	    dsx1->isdn_d_timeslot = E1_ISDN_D_TIMESLOT;
	    dsx1->bchan_bitfield_to_dsx1_bitfield =
		e1_bchan_bitfield_to_dsx1_bitfield;
	    dsx1->timeslot_to_bchan = e1_timeslot_to_bchan;
	    dsx1->pri_timeslot_bitmap = 0;

	    for (ix = 0 ; ix <= DSX1_MAX_CHAN ; ix++)
		dsx1->speed[ix] = DSX1_SPEED_64;
	    reg_add_print_copyright(e1_print_version, "e1_print_version");
	}
	/* Clear the MIB variable here, after the defaults are set
	 * so we catch the clocking part
	 */
	dsx1_init_mib_data(dsx1);
	add_e1_commands();	/* kick the parser while we're at it */
	break;
    default:
	break;	/* for compiler */
    }
    return(dsx1);
}

/*
 * dsx1_reset
 *
 * Initialize the DSX1 parameters for a particular cdb - should be
 * called from the controller reset specific routine.
 */
void dsx1_reset (cdbtype *cdb)
{
    dsx1info *dsx1;
    int channel, bitfield, ts;
    hwidbtype *hwidb;

    dsx1 = dsx1_find_by_unit(cdb->unit);

    if (dsx1 == NULL)
	return;

    /*
     * If we're in the middle of an EOIR event,
     * we don't need to do anything in this routine.
     * Another card was removed or was reinserted.
     */
    if (cdb->cdb_status & CDB_EOIR_RESET)
	return;

    if (serial_debug)
	buginf("\n%s reset", cdb->cdb_namestring);
    
    /*
     * If T1/E1 is shut, don't put it in loop mode - the framer will
     * sync to itself.
     */
    if (cdb->cdb_state != IDBS_ADMINDOWN)
	reg_invoke_dsx1_set_loopback(dsx1);

    reg_invoke_dsx1_set_framing(dsx1);

    /*
     * Note: for E1, we don't really need the clock source command -
     * it doesn't hurt, since we initialize it to LINE
     */
    reg_invoke_dsx1_set_clock_source(dsx1);
    reg_invoke_dsx1_set_linecode(dsx1);
    reg_invoke_dsx1_set_fdl(dsx1);
    reg_invoke_dsx1_set_cablelength(dsx1);

    FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
	hwidb = dsx1->hwidb[channel];
	if (hwidb != NULL) {
	    bitfield = 0;

	    FOR_ALL_DSX1_TIMESLOTS(ts, dsx1) { 
		if (dsx1->timeslots[ts] == channel)
		    bitfield |= make_bit_field(ts, ts);
	    }
	    /* so we get re-mapped */
	    reg_invoke_dsx1_create_idb(cdb, hwidb, channel);
	    hwidb->far_end_looped = FALSE;
	    hwidb->ext_loop_type = DSX1_LOOP_NONE;
	    reg_invoke_dsx1_map_channel(dsx1, hwidb,
					channel, bitfield,
					dsx1->speed[channel]);
	}
    }
    /*
     * Maybe we want to bring all the interfaces down ? ASDF.
     */
}


/*
 * dsx1_give_e1_applique_name
 * return a pointer to the correct applique name
 * This should really be a const char *, but the stub registry
 * stuff doesn't let you declare one yet
 */
char *dsx1_applique_name (int unit)
{
    dsx1info *dsx1;

    dsx1 = dsx1_find_by_unit(unit);

    switch (dsx1->applique_type) {
    case APPL_T1:
    case APPL_T1_CSU:
        return("Channelized T1");
    case APPL_E1_BAL: return("Channelized E1 - balanced");
    case APPL_E1_UNBAL: return("Channelized E1 - unbalanced");
    default: return("Unknown");
    }
}

int dsx1_count_timeslots (dsx1info *dsx1, int channel)
{
    int result, ts;

    result = 0;
    FOR_ALL_DSX1_TIMESLOTS(ts, dsx1) {
	if (dsx1->timeslots[ts] == channel)
	    result++;
    }
    return(result);
}
/*
 * dsx1_show_timeslots
 * Given a dsx1, and a circuit number, display the timeslots
 */
boolean dsx1_show_timeslots (char *comline, dsx1info *dsx1, int channel)
{
    int ts;
    boolean first;

    first = FALSE;
    *comline = '\0';

    for (ts = 1 ; ts <= dsx1->max_timeslots ; first = TRUE) {
	while ((ts <= dsx1->max_timeslots)
	       && (dsx1->timeslots[ts] != channel))
	    ts++;

	if (ts > dsx1->max_timeslots) {
	    break;
	}

	/*
	 * If we already have 1, put a comma
	 */
	if (first)
	    strcat(comline, ",");
	
	if (ts == dsx1->max_timeslots) {
	    sprintf(comline + strlen(comline), "%d", ts);
	    ts++; /* ugly, but should work */
	} else if (dsx1->timeslots[ts + 1] == channel) {
	    /* we have a range */
	    sprintf(comline + strlen(comline), "%d-", ts);
	    while((++ts <= dsx1->max_timeslots)
		&& (dsx1->timeslots[ts] == channel));
	    sprintf(comline + strlen(comline), "%d", ts - 1);
	} else {
	    sprintf(comline + strlen(comline), "%d", ts);
	    ts += 2;	/* we know ts + 1 is not valid */
	}
    }
    return(first);
}
/*
 * dsx1_show_isdn_timeslots
 * Go thru dsx1 hwidb array and display the timeslots allocated
 * for isdn.
 * Note: might want to neaten this up so that we have just the D channel
 * left over (ie:1-5,24), we don't display the ,24.
 */
static boolean dsx1_show_isdn_timeslots (char *comline, dsx1info *dsx1)
{
    int ts;
    boolean first;

    first = FALSE;
    *comline = '\0';

    for (ts = 1 ; ts <= dsx1->max_timeslots ; first = TRUE) {
	while ((ts <= dsx1->max_timeslots)
	       && (!(is_isdn(dsx1->hwidb[ISDN_TS_TO_CHAN_GROUP(ts)]))))
	    ts++;

	if (ts > dsx1->max_timeslots) {
	    break;
	}

	/*
	 * If we already have 1, put a comma
	 */
	if (first)
	    strcat(comline, ",");
	
	if (ts == dsx1->max_timeslots) {
	    sprintf(comline + strlen(comline), "%d", ts);
	    ts++; /* ugly, but should work */
	} else if (is_isdn(dsx1->hwidb[ISDN_TS_TO_CHAN_GROUP(ts + 1)])) {
	    /* we have a range */
	    sprintf(comline + strlen(comline), "%d-", ts);
	    while((++ts <= dsx1->max_timeslots)
		&& (is_isdn(dsx1->hwidb[ISDN_TS_TO_CHAN_GROUP(ts)])));
	    sprintf(comline + strlen(comline), "%d", ts - 1);
	} else {
	    sprintf(comline + strlen(comline), "%d", ts);
	    ts += 2;	/* we know ts + 1 is not valid */
	}
    }
    return(first);
}

static const char *dsx1_display_speed (enum dsx1_speed_type speed)
{
    switch (speed) {
    case DSX1_SPEED_8:  return("8");
    case DSX1_SPEED_16: return("16");
    case DSX1_SPEED_24: return("24");
    case DSX1_SPEED_32: return("32");
    case DSX1_SPEED_40: return("40");
    case DSX1_SPEED_48_I460: return("i460-48");
    case DSX1_SPEED_48: return("48");
    case DSX1_SPEED_56: return("56");
    case DSX1_SPEED_64: return("64");
    default: return("unknown");
    }
}

static const char *dsx1_display_sigtype(enum dsx1_sig_type sigtype)
{
	switch (sigtype) {
		case DSX1_SIGTYPE_NOSIG: return("");
		case DSX1_SIGTYPE_EM: return("E&M");
		default: return("unknown");
	}
}

/************************************************************************
 *									*
 *			Common Map routines				*
 *									*
 ************************************************************************/

/*
 * dsx1_turn_off_channel
 * remove a channel-group/isdn channel from the system.  This
 * should remove the idb (at least as much as is safe) from
 * any lists that could make it active
 */
static void dsx1_turn_off_channel (dsx1info *dsx1, int channel)
{
    int ix;
    hwidbtype *hwidb;
    
    /*
     *	Go through a little effort to insure we don't send the command
     *  unnecessarily
     */
    hwidb = dsx1->hwidb[channel];
    if (hwidb == NULL)
	return;

    if (is_isdn(hwidb)) {
        hwidbtype *dchan_idb;
	/*
	 * we also need to get dialer_encapsulation out
	 * of the way now.
	 */
        dchan_idb = hwidb->isdn_info->d_chan_idb;
	reg_invoke_dialer_pri_shutdown(hwidb);
        if (dchan_idb && dchan_idb->isdn_info)
            dchan_idb->isdn_info->b_chan_idb[channel] = NULL;
	free(hwidb->isdn_info);
	hwidb->isdn_info = NULL;
        /* 
         * CDP was disabled for PRI. We need to 
         * enable it for MIP. CSCdi35737.
         */
        hwidb->firstsw->cdp_enabled = TRUE; 
	nprinets--;
    }

    reg_invoke_dsx1_map_channel(dsx1, hwidb, channel, 0, 0);
    dsx1->hwidb[channel] = NULL;
    reg_invoke_dsx1_delete_idb(dsx1->cdb, hwidb);

    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
	if (dsx1->timeslots[ix] == channel)
	    dsx1->timeslots[ix] = UNUSED_CHAN;
    }
}

/*
 * dsx1_map_channel
 * Maps a channel, or displays the NVGEN command.  Called from
 * the controller commands (both T1 and E1)
 * default_speed is the default for the type of card
 */
static void dsx1_map_channel (parseinfo *csb, dsx1info *dsx1,
			      enum dsx1_speed_type default_speed)
{
    int ix;
    cdbtype *cdb;
    hwidbtype *hwidb;
    int channel;
    
    cdb = csb->controller;

    if (csb->nvgen) {
	char *comline;

	comline = string_getnext();
	FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
	    hwidb = dsx1->hwidb[channel];
            if ((hwidb != NULL) && (!(is_isdn(hwidb))) && (is_t1_digit_channel(dsx1, channel))) {
		sprintf(comline, "channel-group %d timeslots ", channel );
		dsx1_show_timeslots(comline + strlen(comline), dsx1, channel);
		if (dsx1->speed[channel] != default_speed) {
		    sprintf(comline + strlen(comline), " speed %s",
			    dsx1_display_speed(dsx1->speed[channel]));
		}
		nv_write(TRUE, "%s", comline);
	    }
	}
    } else {
	int speed;

	channel = GETOBJ(int,1);
	if (csb->sense) {
	    ulong bitfield = GETOBJ(int,2);
	    idbtype *swidb;

	    /* Check if we are already overlapping timeslots */
	    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
		if (bitfield & (1 << (32 - ix))) {
                    if (((dsx1->timeslots[ix] != channel) &&
                        (dsx1->timeslots[ix] != UNUSED_CHAN)) ||
                        ((dsx1->timeslots[ix] == channel) &&
                        (is_t1_cas(dsx1, channel)))) {
			if (is_isdn(dsx1->hwidb[dsx1->timeslots[ix]]))
			    printf("\n%%Timeslot %d already used by pri-group",
				   ix);
                        else if (is_t1_cas(dsx1, dsx1->timeslots[ix]))
                            printf("\n%%Timeslot %d already used by cas-group", ix);
			else
			    printf("\n%%Timeslot %d already used by "
				   "channel-group %d",
				   ix, dsx1->timeslots[ix]);
			return;
		    }
		}
	    }

	    hwidb = dsx1->hwidb[channel];
	    if (hwidb == NULL) {
		hwidb = dsx1->hwidb[channel] =
		    reg_invoke_dsx1_create_idb(cdb, NULL, channel);
		if (hwidb == NULL) {
		    printf("\n%%Insufficient resources to create channel group");
		    return;
		}
	    } else if (is_isdn(hwidb)) {
		printf("\n%%Channel-group %d is already an isdn channel",
		       channel);
		return;
	    }
	    /*
	     * Set the timeslots in the timeslot array to this channel
	     */
	    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
		if (bitfield & (1 << (32 - ix))) {
		    dsx1->timeslots[ix] = channel;
		} else if (dsx1->timeslots[ix] == channel) {
		    dsx1->timeslots[ix] = UNUSED_CHAN;
		}
	    }
	    /*
	     * Set channel type 
	     */
	    dsx1->signal_type[channel] = DSX1_SIGTYPE_NOSIG;

	    dsx1->speed[channel] = speed = GETOBJ(int,3);
	    reg_invoke_dsx1_map_channel(dsx1, hwidb, channel,
					bitfield, speed);
	    /*
	     * Advantage DSX1: we know our true bandwidth.
	     *
	     * Anytime we map the channel here,
	     * we will set the bandwidth default.
	     *
	     * The speed is 8kb/s per bit set in the speed mask,
	     * times the number of channels (the number of bits set
	     * in the bit field).
	     */
	    hwidb = dsx1->hwidb[channel];
	    swidb = hwidb->firstsw;
	    if (swidb != NULL) {
		ulong visible_bw;
                ix = num_bits(bitfield);
                visible_bw = ((num_bits(speed) * 8) * ix);
		set_default_interface_bandwidth(swidb, visible_bw);
	    }
	} else {
	    /* No channel-group command */
	    hwidb = dsx1->hwidb[channel];
            if (hwidb && (!(is_isdn(hwidb))) && (is_t1_digit_channel(dsx1, channel)))
		dsx1_turn_off_channel(dsx1, channel);
	    else {
		printf("\n%%Please use pri-group command or cas-group command");
		return;
	    }
	}
    }
}


/*
 * dsx1_init_pri
 * initialize the primary rate group idbs and dialer fields
 */
static void dsx1_init_pri (dsx1info *dsx1,
			   ulong bchanbitf,
			   boolean d_init_flag,
			   int dsl)
{
    hwidbtype *hwidb;
    hwidbtype *tmphwidb;
    idbtype *swidb;
    hwidbtype *dchan_idb;
    isdn_bri_t *isdn_info;
    dialerdbtype *d_ddb, *b_ddb, *masterddb, *netddb;
    int channel, ts, ix;
    int configedints;

    dchan_idb = dsx1->hwidb[dsx1->isdn_d_chan];
    dchan_idb->isdn_info->chantype = ISDN_Dchan;
    if (dchan_idb == NULL) {
    	printf("\n%%PRI: No D-Channel IDB in dsx1_init_pri");
        return;
    } else {
	/*
	 * This PRI may be a member of a rotary group, so before reconfiguring
	 * it, save the pointer to the hunt group leader.
	 * Set netidb to point to either D channel, or hunt group leader.
	 */
	d_ddb = get_dialerdb(dchan_idb);
	if (d_ddb == NULL) {
	    /*
	     * Initialize D channel first, because the B channels reference
	     * its dialerdb structure.
	     */
            if (!reg_invoke_init_pri_idb(dchan_idb)) {
		printf("\n%%PRI:no memory for dialer");
		return;
	    }
	    d_ddb = get_dialerdb(dchan_idb);
	}
	d_ddb->hunt_group_first = NULL;
	masterddb = d_ddb->hunt_group_master;
	netddb = d_ddb->netcfg_ddb;
    }

    dsx1->isdn_d_chan_hwidb = dchan_idb;
    hwidb = NULL;
    swidb = NULL;
    configedints = 0;

    FOR_ALL_DSX1_TIMESLOTS(ts, dsx1) {
	channel = ISDN_TS_TO_CHAN_GROUP(ts);
        hwidb = dsx1->hwidb[channel];
	/* 
	 * This idb entry could be null if all timeslots
	 * are not configured, or we could have channel-groups still defined
	 */
	if (!(is_isdn(hwidb)))
	    continue;

	/* 
	 * Increment the number of configured timeslots on
    	 * this board so we have a valid count for number
	 * of available B's.
	 */
	configedints++;
    	swidb = hwidb->firstsw;

	/*
	 * We've already allocated the isdn_info fields when we
	 * created the idbs
	 */
	isdn_info = hwidb->isdn_info;
        if (channel != dsx1->isdn_d_chan) {
	    isdn_info->chantype = (*dsx1->timeslot_to_bchan)(ts);
	}
	isdn_info->dsl = dsl;
	isdn_info->d_chan_idb = dchan_idb;
	isdn_info->primary_rate = TRUE;
	isdn_info->activate_line = NULL;
	isdn_info->disable_bri_channel = NULL;
	isdn_info->enable_bri_channel = NULL;
	isdn_info->set_bri_bandwidth = NULL;


        hwidb->status |= IDB_SERIAL | IDB_BCAST | IDB_HEARSELF | IDB_BRIDGE |
            IDB_ISDN;
        GET_TIMESTAMP(hwidb->state_time);
        hwidb->error_threshold = 5000;
        hwidb->error_count = 0;
        hwidb->fast_switch_flag = 0;

        swidb->span_disabled = FALSE;
        hwidb->state = IDBS_DOWN;

        /*
         * Set up arping (actually, turn off all arping).
         */

        swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
        swidb->arp_probe = swidb->arp_probe_def = FALSE;
        swidb->arp_snap = swidb->arp_snap_def = FALSE;

	set_default_interface_bandwidth(swidb, VISIBLE_T1_ISDN_BANDWIDTH);
	set_default_interface_delay(swidb, T1_ISDN_DELAY);
        hwidb->reliability = 255;
        hwidb->load = 1;
        hwidb->rxload = 1;

        /*
         * NOTE: This code emulates the bri isdn_setup call.
         * The idb pointers have already been initialized by the driver,
         * so initialize the calling and called number, and setup the idb
         * so it looks like dialer.
         */
        isdn_info->remote_number = NULL;
	isdn_info->remote_src = ISDN_SRC_UNKNOWN;
        isdn_info->call_tbl = NULL;

        if (!reg_invoke_init_pri_idb(hwidb)) {
	    printf("\n%%PRI:no memory for dialer");
	    return;
	}

        if (channel != dsx1->isdn_d_chan) {
	    b_ddb = get_dialerdb(hwidb);
            b_ddb->netcfg_ddb = netddb;
            b_ddb->netcfg_idb = netddb->ownidb;
	    /*
	     * The IDB_NO_NVGEN flag will hide all the B-channels
	     * from the nvram. Nothing can be done to them anyway.
	     * This idb will be put back on the idb queue when
	     * the "no pri-group" command is executed. The complete
	     * idb will be zeroed when the idb is taken back off the
	     * queue in the create_idb registry.
	     */
             hwidb->status |= IDB_NO_NVGEN;
        }
    }

    /* 
     * I subtract one off the configedints count 
     * because this includes the D channel. This
     * will have to change when we support NFAS.
     * This is here because d channel isn't always at the end
     * Also, we put all the b channel's in the D channel spots
     *
     * Do the hunt_group_lead here - it must be after the
     * reg_invoke_dialer_pri_idb above
     */
    d_ddb->hunt_group_lead = TRUE;
    d_ddb->hunt_group_master = masterddb; /* restore rotary group idb */
    d_ddb->netcfg_ddb = netddb;		/* restore top rotary group ddb */
    d_ddb->netcfg_idb = netddb->ownidb;	/* restore top rotary group idb */
    isdn_info = dchan_idb->isdn_info;
    isdn_info->num_avail_b = configedints - 1;
    d_ddb->dialer_num_link = configedints - 1;
    FOR_ALL_DSX1_CHANNELS(ix, dsx1) {
	isdn_info->b_chan_idb[ix] = NULL;
    }
    /*
     * Put the hwidbs in the b_chan_idb array for the d-channel
     * They are indexed by the chantype array
     */
    FOR_ALL_DSX1_CHANNELS(ix, dsx1) {
	tmphwidb = dsx1->hwidb[ix];
	if ((is_isdn(tmphwidb)) &&
	    (tmphwidb->isdn_info->chantype != ISDN_Dchan))
	    isdn_info->b_chan_idb[ix] = tmphwidb;
    }
    /*
     * Tell ISDN code about D chan idb.
     * Assign reserved dsl to interface.
     *
     * Note:  the dsl must first have been
     * reserved by reg_invoke_isdn_verify_dsl()
     * in dsx1.c
     */
    if (!d_init_flag)
	reg_invoke_isdn_pri_init_core(dchan_idb, dsl);

    /*
     * isdn_set_chan_state will allocate the appropriate B channels
     * in the Telenetworks code. If we are running T1 than we want
     * to reserve channel 24 for the D channel. (except for NFAS)
     * E1 uses channel 16 for the D channel and there is also a
     * B16 (timeslot 17). That is why I don't want to reserve
     * channel 16 in the E1 case.
     */
    if (dchan_idb->type == IDBTYPE_T1)
	reg_invoke_isdn_set_chan_state(bchanbitf, dsl, TRUE,
				       T1_ISDN_D_TIMESLOT);
    else
	reg_invoke_isdn_set_chan_state(bchanbitf, dsl, TRUE,
				       E1_ISDN_D_TIMESLOT);

    /*
     * Let's spoof on the D-channel.
     */

    idb_init_names(dchan_idb->firstsw, FALSE);
}

/*
 * The PRI code need to modify the speed of the specified
 * timeslot.
 */
static void dsx1_pri_speed (hwidbtype *idb, ulong bitfield, int speed)
{
    dsx1info *dsx1;
    cdbtype *cdb;
    int channel;

    /*
     * Find the apprpriate dsx1 instance from the CDB.
     */
    if ((cdb = controller_find_from_idb(idb)) != NULL) {
	dsx1 = dsx1_find_by_unit(cdb->unit);
	channel = idb->vc;
	if (speed == 64)
	    dsx1->speed[channel] = DSX1_SPEED_64;
	else
	    dsx1->speed[channel] = DSX1_SPEED_56;
	bitfield = (*dsx1->bchan_bitfield_to_dsx1_bitfield)(bitfield);
	reg_invoke_dsx1_map_channel(dsx1, idb, channel,
				    bitfield, dsx1->speed[channel]);
    } else {
	errmsg(&msgsym(NOTFOUND, CONTROLLER), idb->slot);
    }
}

static void dsx1_remove_pri_group (dsx1info *dsx1)
{
    hwidbtype *hwidb;
    int channel, dsl;
    
    hwidb = dsx1->isdn_d_chan_hwidb;
    if (hwidb != NULL) {
	dsl = hwidb->isdn_info->dsl;
	dsx1_change_idb_state(hwidb, IDBS_DOWN);
	reg_invoke_isdn_set_chan_state(0, dsl, TRUE, 0);
    }
    FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
	hwidb = dsx1->hwidb[channel];
	if (hwidb && is_isdn(hwidb) && (channel != dsx1->isdn_d_chan)) {
	    dsx1_turn_off_channel(dsx1, channel);
	}
    }
    dsx1_turn_off_channel(dsx1, dsx1->isdn_d_chan);
    dsx1->isdn_d_chan_hwidb = NULL;
    dsx1->pri_timeslot_bitmap = 0;
}

/*
 * dsx1_setup_pri
 * Here we set up the primary rate code.  We will do the NVGEN
 * or actually set up the primary rate channels/idbs
 * Called from both T1 and E1 controller commands.
 */
static void dsx1_setup_pri (parseinfo *csb, dsx1info *dsx1)
{
    cdbtype	*cdb;
    ulong bitfield, bchanbitf, mask;
    int channel;
    int speed, ix, dsl;
    char *comline;
    hwidbtype *hwidb;
    idbtype *swidb;
    boolean d_init_flag;

    comline = '\0';
    d_init_flag = FALSE;
    cdb = dsx1->cdb;
    if (csb->nvgen) {
	comline = string_getnext();
	sprintf(comline, "pri-group timeslots ");
	if (dsx1_show_isdn_timeslots(comline + strlen(comline), dsx1)) {
	    if (dsx1->speed[dsx1->isdn_d_chan] != DSX1_SPEED_64){
		sprintf(comline + strlen(comline), " speed %s",
			dsx1_display_speed(dsx1->speed[dsx1->isdn_d_chan]));
	    }
	    nv_write(TRUE, "%s", comline);
	}
    } else {
	if (csb->sense) {
	    if (!is_pri_supp) {
		printf("\n%%ISDN is not supported in this image.");
		return;
	    }
	    /*
	     * Only allow the pri-group command if the isdn 
	     * switch type is already defined.
	     */

	    if (reg_invoke_isdn_pri_getswitch() == NO_ISDN_SWITCH) {
		printf("\n%%ISDN switch-type must be set first.");
		return;
	    }

	    /*
	     * Okay. I need to take the passed in bitfield (OBJ,1)
	     * and make each timeslot a 64kb timeslot. 
	     * For example:
	     *    Passed in bitfield 1-24	0xFFFFFF00
	     *    I need to call dsx1_map_channel 24 seperate times.
	     *    Likewise: passed in bitfield 20-24 0x1F00
	     *    I need to call dsx1_map_channel 5 seperate times.
	     * Note: the channel number will always be the 
	     *    timeslot - 1.
	     */

	    bchanbitf = GETOBJ(int,1);
	    /*
	     * Set the D channel in the bitfield - make sure we're
	     * not only setting it
	     */
	    bitfield = (1 << (32 - dsx1->isdn_d_timeslot));
	    if ((bchanbitf & ~bitfield) == 0) {
		printf("\n%%Must specify more than D channel for primary rate");
		return;
	    }

	    bitfield |= bchanbitf;
	    /*
	     * If we have no change, just return (stops us from clearing
	     * all the calls
	     */
	    if (bitfield == dsx1->pri_timeslot_bitmap) {
		return;
	    }
	    speed = GETOBJ(int,2);

	    /*
	     * Make sure we're not overwriting any channel-groups
	     */
	    FOR_ALL_DSX1_TIMESLOTS(ix,dsx1) {
		if (bitfield & (1 << (32 - ix))) {
		    /*
		     * Look at the timeslot we're using, and
		     * the channel-group # we're going to need.
		     */
		    if (dsx1->timeslots[ix] != UNUSED_CHAN) {
			hwidbtype *temp;
			temp = dsx1->hwidb[dsx1->timeslots[ix]];
			if (temp != NULL && (!(is_isdn(temp)))) {
			    printf("\n%%Timeslot %d already used by "
				   "channel-group or cas-group %d", ix,
				   dsx1->timeslots[ix]);
			    return;
			}
			else if (is_t1_cas(dsx1, dsx1->timeslots[ix])) {
                            printf("\n%%Timeslot %d already used by cas-group",ix);
                            return;
                        }
		    }
		    hwidb = dsx1->hwidb[ISDN_TS_TO_CHAN_GROUP(ix)];
		    if ((hwidb != NULL) && (!(is_isdn(hwidb)))) {
			printf("\n%%Channel-group %d is already defined",
			       ISDN_TS_TO_CHAN_GROUP(ix));
			return;
		    }
		}
	    }

	    /*
	     * If the D-channel is already running
	     * don't mess with it.
	     */
	    if (dsx1->isdn_d_chan_hwidb && dsx1->isdn_d_chan_hwidb->isdn_info) {
		d_init_flag = TRUE;
		dsl = dsx1->isdn_d_chan_hwidb->isdn_info->dsl;
	    }
            else {
            /* 
             * Before doing the work to setup a PRI interface,
             * make sure a free dsl is available for the interface
             * interface and reserve it.
             *
             * Note: if for some reason dsx1_init_pri() is not
             * called then the dsl must be deassigned by calling
             * reg_invoke_isdn_set_chan_state(0, dsl, TRUE, 0).
             */
               dsl = reg_invoke_isdn_verify_dsl();
               if (dsl == ISDN_NO_DSL) {
                  printf("\n%%ISDN Error: Maximum allowed "
                         "PRI interfaces exceeded.");
                  return;
	       }
            }

	    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
		mask = 1 << (32 - ix);
		channel = ISDN_TS_TO_CHAN_GROUP(ix);
		hwidb = dsx1->hwidb[channel];
		if (bitfield & mask) {
		    /*
		     * If a pri-group already existed and
		     * we are executing a new pri-group
		     * we need to make sure that we disconnect
		     * any existing calls.
		     */
		    if ((is_isdn(hwidb)) &&
			hwidb->isdn_info->chantype != ISDN_Dchan &&
			d_init_flag)
			(*hwidb->reset)(hwidb);
		    /*
		     * We know the bandwidth is 64kb per timeslot
		     * until we support six-packs for example.
		     */
		    dsx1->speed[channel] = speed;
		    dsx1->timeslots[ix] = channel;
		    dsx1->hwidb[channel] = hwidb =
			reg_invoke_dsx1_create_idb(cdb, hwidb, channel);
		    if (hwidb == NULL) {
			dsx1_remove_pri_group(dsx1);
			printf("\nNo memory for pri-group "
			       "- it has been removed");
			return;
		    }
		    reg_invoke_dsx1_map_channel(dsx1, hwidb, channel,
						mask, speed);
		    /*
		     * Advantage DSX1: we know our true bandwidth. 
		     *
		     * Anytime we map the channel here,
		     * we will set the bandwidth default.
		     *
		     * The speed is 8kb/s per bit set in the speed mask,
		     * times the number of channels (the number of bits set
		     * in the bit field).
		     */
		    if ((swidb = hwidb->firstsw) != NULL) {
			/*
			 * We know the bandwidth is 64kb per timeslot
			 * until we support multiple TS per B.
			 */
			set_default_interface_bandwidth(swidb,
						    VISIBLE_T1_ISDN_BANDWIDTH);
		    }
		    if (hwidb->isdn_info == NULL) {
			hwidb->isdn_info = malloc(sizeof(isdn_bri_t));
                        /* 12/15/95. HMR.
                         * Increment number of PRI interfaces.
                         */
                        nprinets++;
		    }

		    if (hwidb->isdn_info == NULL) {
			printf("\n%%PRI(%d): No memory for isdn structure",
			       channel);
			dsx1_remove_pri_group(dsx1);
                        /* 
                         * Decrement number of PRI interfaces.
                         */
                        nprinets--;
			return;
		    }
                    /* 
                     * This is a PRI interface now so turn
                     * off CDP. CSCdi35737.
                     */
                    hwidb->firstsw->cdp_enabled = FALSE; 
		} else if (is_isdn(hwidb)) {
		    /*
		     * Had a primary group at this timeslot, but
		     * no longer - turn it off
		     */
		    dsx1_turn_off_channel(dsx1, channel);
                  }
	    }

	    /*
	     * All done with the dsx1 setup now lets
	     * notify dialer and the isdn code of
	     * our existence.  Also set the pri timeslot map
	     */

	    dsx1_init_pri(dsx1, bchanbitf, d_init_flag, dsl);
	    dsx1->pri_timeslot_bitmap = bitfield;
	    /*
	     * We need to put dialer_encapsulation on top of the
	     * the mip vectors now. mip_map_channel calls
	     * mip_encapsulation but this is before we converted
	     * these idbs to isdn. We then need to propagate
	     * the encapsulation fields to all the idb's in the
	     * hunt group if the encapsulation is not the
	     * default encapsulation. This can occur if a pri-group
	     * already exists and is now modified.
	     * for example:
	     * 	pri-group timeslots 5-10
	     *	pri-group timeslots 1-23
	     */
	    
	    hwidb = dsx1->isdn_d_chan_hwidb;
	    reg_add_isdn_map_channel(hwidb->type,
				     dsx1_pri_speed,
				     "dsx1_pri_speed");
	    reg_invoke_isdn_pri_init(hwidb);

	    FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
		hwidb = dsx1->hwidb[channel];
		if (is_isdn(hwidb)) {
		    /*
		     * we also need to get dialer_encapsulation
		     * on top of board encaps now. 
		     */
		    reg_invoke_dsx1_hwidb_board_encap(hwidb);
		       
		    reg_invoke_pri_member_adjust_encap(hwidb, 0);
		    /*
		     * This is really, really ugly.  But the
		     * reset vector gets grabbed for dialer
		     * stuff, but we need to call mip_interface_reset
		     */
		    (*dsx1->isdn_d_chan_hwidb->reset)(hwidb);
		    /* and reset idb  */

		    /*
		     * This piece of magic is done because 
		     * the dsx1 code will only generate c-state
		     * transitions based on T1 carrier.
		     */
		    if (hwidb->isdn_info->chantype == ISDN_Dchan) {
			cdb = dsx1->cdb;
			if (cdb->cdb_state == IDBS_UP) {
			    hwidb->state = IDBS_DOWN;
			    dsx1_change_idb_state(hwidb, IDBS_UP);
			}
		    }
	            /* setup channel type */
		    dsx1->signal_type[channel] = DSX1_SIGTYPE_NOSIG;
		}
	    }
	} else {
	    dsx1_remove_pri_group(dsx1);
	}
    }
}

/*
 * dsx1_setup_cas
 * Here we set up the T1 robbed bit signals channels.
 */
static void dsx1_setup_cas (parseinfo *csb, dsx1info *dsx1)
{
    cdbtype	*cdb;
    ulong bitfield, chanbitf, mask;
    int channel; 
    int ix;
    enum dsx1_sig_type sig_type;

    if (dsx1_cas_supp == FALSE) {
	printf("\n%%CAS is not supported in this image.");
	return;
    }
    cdb = dsx1->cdb;
    if (csb->nvgen) {
	char *comline;

	comline = string_getnext();
	FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
	    if (is_t1_cas(dsx1, channel)) {
		sprintf(comline, "cas-group %d timeslots ", channel );
		dsx1_show_timeslots(comline + strlen(comline), dsx1, channel);
		sprintf(comline + strlen(comline), " type %s",
			    dsx1_display_sigtype(dsx1->signal_type[channel]));
	        nv_write(TRUE, "%s", comline);
	    }
	}
    } else {
	/* Get specified group number */
	channel = GETOBJ(int, 1);
	/* clear the robbed bit signals */
	bzero(dsx1->rx_rbs,  DSX1_MAX_CHAN/2);
	bzero(dsx1->tx_rbs,  DSX1_MAX_CHAN/2);
	if (csb->sense) {
	    /*
	     * Okay. I need to take the passed in bitfield (OBJ,1)
	     * and make each timeslot a 64kb timeslot. 
	     * For example:
	     *    Passed in bitfield 1-24	0xFFFFFF00
	     *    I need to call dsx1_map_channel 24 seperate times.
	     *    Likewise: passed in bitfield 20-24 0x1F00
	     *    I need to call dsx1_map_channel 5 seperate times.
	     * Note: the channel number will always be the 
	     *    timeslot - 1.
	     */
	    chanbitf = GETOBJ(int,2);
	    sig_type = GETOBJ(int,3);
	    bitfield = chanbitf;
	    /*
	     * Make sure we're not overwriting any channel-groups
	     */
	    if (!is_t1_cas(dsx1, channel) && dsx1->hwidb[channel]) {
		printf("\nThe channel has been assigned to pri or channel-group");
			return;
            }

	    FOR_ALL_DSX1_TIMESLOTS(ix,dsx1) {
		if (bitfield & (1 << (32 - ix))) {
		    /*
		     * Look at the timeslot we're using, and
		     * the channel-group # we're going to need.
		     */
		    if ((dsx1->timeslots[ix] != UNUSED_CHAN) &&
			 ( dsx1->timeslots[ix] != channel)) {	
		            printf("\n%%Timeslot %d already used by other channel", ix, dsx1->timeslots[ix]);
			    return;
			}
		}
	    }

	    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
		mask = 1 << (32 - ix);
		if (bitfield & mask)
		    /* setup channel number for each timeslots */
		    dsx1->timeslots[ix] = channel;
		else
		    if (dsx1->timeslots[ix] == channel)
			/* turn it off if it was assigned to this channel before
			   but not this time */
			dsx1->timeslots[ix] = UNUSED_CHAN;
	    }
	    dsx1->speed[channel] = DSX1_SPEED_64;
	    dsx1->signal_type[channel] = sig_type;
	    reg_invoke_dsx1_set_cas_group(dsx1); 

	} else {
	    /*
	     * This is the no cas-group section of the code.
	     * we need to turn off all the corresponding timeslots in that
	     * cas-group. 
	     */
	    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
		if (dsx1->timeslots[ix] == channel)
		    /* setup channel number for each timeslots */
		    dsx1->timeslots[ix] = UNUSED_CHAN;
	    }
	    dsx1->signal_type[channel] = DSX1_SIGTYPE_NOSIG;
	    reg_invoke_dsx1_set_cas_group(dsx1); /* disable signals active */ 
	}
    }
}

/************************************************************************
 *									*
 *			Configuration Commands				*
 *									*
 ************************************************************************/

void e1ctrlr_command (parseinfo *csb)
{
    dsx1info	*dsx1;
    cdbtype	*cdb;

    cdb = csb->controller;
    dsx1 = dsx1_find_by_unit(cdb->unit);

    switch ((enum dsx1_ctrlr_commands)csb->which) {
    case E1_CTRLR_LOOP:
	if (csb->nvgen) {
	    nv_write((dsx1->loop == DSX1_LOOP_LOCAL), "loop");
	} else {
	    if (csb->sense) {
		dsx1->loop = DSX1_LOOP_LOCAL;
	    } else {
		dsx1->loop = DSX1_LOOP_NONE;
	    }
	    reg_invoke_dsx1_set_loopback(dsx1);
	}
	break;
	
    case E1_CTRLR_SHUT:
	if (csb->nvgen) {
	    nv_write((cdb->cdb_state == IDBS_ADMINDOWN), "shutdown");
	} else {
	    if (csb->sense) {
		cdb->cdb_state = IDBS_ADMINDOWN;
		dsx1_change_all_idbs_state(cdb, IDBS_DOWN);
	    } else {
		cdb->cdb_state = IDBS_DOWN;
		cdb->cdb_status &= ~CDB_CRASHED;
	    }
	    if (cdb->reset)
		(*cdb->reset)(cdb);
	}
	break;
	
    case E1_CTRLR_FRAMING:
	if (csb->nvgen) {
	    nv_write(((dsx1->framing != DSX1_FRM_E1_CRC) ||
		      (dsx1->homologation_type != DSX1_HOMOL_NORMAL)),
		     "framing %s %s",
		     get_dsx1_framing(dsx1->framing),
		     get_dsx1_homologation_type(dsx1->homologation_type));
	} else {
	    if (csb->sense) {
		dsx1->framing = (enum dsx1_framing_type)GETOBJ(int,1);
		dsx1->homologation_type =
		    (enum dsx1_homologation_type)GETOBJ(int,2);
	    } else {
		dsx1->framing = DSX1_FRM_E1_CRC;
		dsx1->homologation_type = DSX1_HOMOL_NORMAL;
	    }
	    reg_invoke_dsx1_set_framing(dsx1);
	}
	break;
	
    case E1_CTRLR_CLOCKING:
	if (csb->nvgen) {
        if (dsx1->clocking == DSX1_CLK_INTERNAL) {
        	nv_write(TRUE, "clock source internal");
        } else if (dsx1->clocking == DSX1_CLK_FREE) {
        	nv_write(TRUE, "clock source free running");
        } else if (dsx1->clocking == DSX1_CLK_THRU) {
        	nv_write(TRUE, "clock source line secondary");
        } else 
			if (dsx1->clocksource == DSX1_PRIMARY)
        		nv_write(TRUE, "clock source line primary");
	} else {
        dsx1->clocksource = (enum dsx1_clocksource_type)GETOBJ(int,2);
	    if (csb->sense) {
			dsx1->clocking = (enum dsx1_clocking_type)GETOBJ(int,1);
			if ((dsx1->clocking == DSX1_CLK_LINE) && (dsx1->clocksource > DSX1_PRIMARY)) 
				dsx1->clocking = DSX1_CLK_THRU;
	    } else {
			dsx1->clocking = DSX1_CLK_LINE;
		}
	    /* Set the snmp info, then send the command */
	    dsx1->MIB.C.dsx1TransmitClockSource = dsx1->clocking;
	    reg_invoke_dsx1_set_clock_source(dsx1);
	}
	break;
	
    case E1_CTRLR_LINECODE:
	if (csb->nvgen) {
	    nv_write((dsx1->linecode != DSX1_LC_HDB3), "linecode ami");
	} else {
	    if (csb->sense)
		dsx1->linecode = (enum dsx1_linecode_type)GETOBJ(int,1);
	    else
		dsx1->linecode = DSX1_LC_HDB3;
	    dsx1->MIB.C.dsx1LineCoding = dsx1->linecode;
	    reg_invoke_dsx1_set_linecode(dsx1);
	}
	break;
	
    case E1_CTRLR_CHANNEL:
	dsx1_map_channel(csb, dsx1, DSX1_SPEED_64);
	break;
	
    case E1_CTRLR_PRI_CHANNEL:
	dsx1_setup_pri(csb, dsx1);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void t1ctrlr_command (parseinfo *csb)
{
    dsx1info	*dsx1;
    cdbtype	*cdb;
    hwidbtype *hwidb;
    
    cdb = csb->controller;
    dsx1 = dsx1_find_by_unit(cdb->unit);
    hwidb = NULL;

    switch ((enum dsx1_ctrlr_commands)csb->which) {
    case T1_CTRLR_LOOP:
	if (csb->nvgen) {
	    switch (dsx1->loop) {
	    case DSX1_LOOP_NONE:
		break;
	    case DSX1_LOOP_LOCAL:
		nv_write(TRUE, "loop");
		break;
	    case DSX1_LOOP_REMOTE:
		nv_write(TRUE, "loop remote");
		break;
	    }
	} else {
	    enum dsx1_loop_type new = (enum dsx1_loop_type)GETOBJ(int,1);
	    
	    if (csb->sense) {
		if (dsx1->loop != new) {
		    if (dsx1->loop == DSX1_LOOP_REMOTE) {
			printf("\n%%Must remove remote loop before local loop");
			return;
		    }
		    dsx1->loop = new;
		    reg_invoke_dsx1_set_loopback(dsx1);
		}
	    } else {
		dsx1->loop = DSX1_LOOP_NONE;
		reg_invoke_dsx1_set_loopback(dsx1);
	    }
	}
	break;
	
    case T1_CTRLR_SHUT:
	if (csb->nvgen) {
	    nv_write((cdb->cdb_state == IDBS_ADMINDOWN), "shutdown");
	} else {
	    if (csb->sense) {
		/* Make them remove remote loopback */
		if (dsx1->loop == DSX1_LOOP_REMOTE) {
		    printf("\n%%Must remove remote loop before local loop");
		    return;
		}
		cdb->cdb_state = IDBS_ADMINDOWN;
		dsx1_change_all_idbs_state(cdb, IDBS_DOWN);
	    } else {
		cdb->cdb_state = IDBS_DOWN;
		cdb->cdb_status &= ~CDB_CRASHED;
	    }
	    if (cdb->reset)
		(*cdb->reset)(cdb);
	}
	break;
	
    case T1_CTRLR_FRAMING:
	if (csb->nvgen) {
	    nv_write((dsx1->framing != DSX1_FRM_SF), "framing esf");
	} else {
	    if (csb->sense)
		dsx1->framing = (enum dsx1_framing_type)GETOBJ(int,1);
	    else
		dsx1->framing = DSX1_FRM_SF;
	    reg_invoke_dsx1_set_framing(dsx1);
	}
	break;
	
    case T1_CTRLR_CLOCKING:
	if (csb->nvgen) {
            if (dsx1->clocking == DSX1_CLK_INTERNAL) {
                nv_write(TRUE, "clock source internal");
            } else if (dsx1->clocking == DSX1_CLK_FREE) {
                nv_write(TRUE, "clock source free running");
            } else if (dsx1->clocking == DSX1_CLK_THRU) {
                nv_write(TRUE, "clock source line secondary");
       	    } else 
		if (dsx1->clocksource == DSX1_PRIMARY)
               	    nv_write(TRUE, "clock source line primary");
	} else {
           dsx1->clocksource = (enum dsx1_clocksource_type)GETOBJ(int,2);
	   if (csb->sense) {
	       dsx1->clocking = (enum dsx1_clocking_type)GETOBJ(int,1);
	       if ((dsx1->clocking == DSX1_CLK_LINE) && 
                            (dsx1->clocksource > DSX1_PRIMARY)) 
		    dsx1->clocking = DSX1_CLK_THRU;
	    } else {
	       dsx1->clocking = DSX1_CLK_LINE;
	    }
	    /* Set the snmp info, then send the command */
	    dsx1->MIB.C.dsx1TransmitClockSource = dsx1->clocking;
	    reg_invoke_dsx1_set_clock_source(dsx1);
	}
	break;
	
    case T1_CTRLR_LINECODE:
	if (csb->nvgen) {
	    nv_write((dsx1->linecode != DSX1_LC_AMI), "linecode b8zs");
	} else {
	    if (csb->sense)
		dsx1->linecode = (enum dsx1_linecode_type)GETOBJ(int,1);
	    else
		dsx1->linecode = DSX1_LC_AMI;
	    dsx1->MIB.C.dsx1LineCoding = dsx1->linecode;
	    reg_invoke_dsx1_set_linecode(dsx1);
	}
	break;
	
    case T1_CTRLR_FDL:
        if (csb->nvgen) {
            if (dsx1->fdl == DSX1_FDL_ATT) {
                nv_write(TRUE, "fdl att");
            } else if (dsx1->fdl == DSX1_FDL_ANSI) {
                nv_write(TRUE, "fdl ansi");
            }
        } else {
            if (dsx1->framing == DSX1_FRM_SF) {
                printf("\n!!!Configure fdl command is not allowed in SF mode!!!");
            } else {
                if (csb->sense)
                    dsx1->fdl = (enum dsx1_fdl_type)GETOBJ(int,1);
                else
                    dsx1->fdl = DSX1_FDL_NONE;
                dsx1->MIB.C.dsx1Fdl = dsx1->fdl;
                reg_invoke_dsx1_set_fdl(dsx1);
            }
        }
        break;

    case T1_CTRLR_CABLELENGTH:
        if (csb->nvgen) {
            if (dsx1->length == DSX1_SHORT) {
                nv_write(TRUE, "cablelength short");
            } else {
                if (dsx1->gain == DSX1_GAIN26) {
                    switch (dsx1->pulse) {
                    case DSX1_0DB:
                        nv_write(TRUE, "cablelength long gain26 0db");
                        break;
                    case DSX1_75DB:
                        nv_write(TRUE, "cablelength long gain26 -7.5db");
                        break;
                    case DSX1_15DB:
                        nv_write(TRUE, "cablelength long gain26 -15db");
                        break;
                    case DSX1_225DB:
                        nv_write(TRUE, "cablelength long gain26 -22.5db");
                        break;
                    }
               } else {
                    switch (dsx1->pulse) {
                    case DSX1_0DB:
                        /* 36 dB gain with 0.0 dB pulse is the default */
                        break;
                    case DSX1_75DB:
                        nv_write(TRUE, "cablelength long gain36 -7.5db");
                        break;
                    case DSX1_15DB:
                        nv_write(TRUE, "cablelength long gain36 -15db");
                        break;
                    case DSX1_225DB:
                        nv_write(TRUE, "cablelength long gain36 -22.5db");
                        break;
                    }
               }
            }
	} else {
            if (csb->sense) {
	       /* first convert the cable length into a constant */
               dsx1->length = GETOBJ(int,1);
               if (dsx1->length == DSX1_LONG) {
                   if (dsx1->applique_type == APPL_T1) {
                       printf("\n%% Configuration of Long Cable is not permitted in DSX1 mode");
                       dsx1->length = DSX1_SHORT;
                       break;
                   }
                   dsx1->gain = GETOBJ(int,2);
                   dsx1->pulse = GETOBJ(int,3);

               } else {
                     if (dsx1->applique_type == APPL_T1_CSU) {
                         printf("\n%% Configuration of Short Cable not permitted in CSU mode");
                         dsx1->length = DSX1_LONG;
                         break;
                     }
                     dsx1->length = DSX1_SHORT;
                     dsx1->cablelength = GETOBJ(int,2);
                 }
            } else {
                  if (dsx1->applique_type != APPL_T1_CSU) {
                      dsx1->length = DSX1_SHORT;
                      dsx1->cablelength = DSX1_LENGTH_133;
                  } else {
                        dsx1->length = DSX1_LONG;
                        dsx1->gain = DSX1_GAIN36;
                        dsx1->pulse = DSX1_0DB;
                  }
            }
	    reg_invoke_dsx1_set_cablelength(dsx1);
	}
	break;
	
    case T1_CTRLR_CHANNEL:
	dsx1_map_channel(csb, dsx1, DSX1_SPEED_56);
	break;
	
    case T1_CTRLR_PRI_CHANNEL:
	dsx1_setup_pri(csb, dsx1);
	break;

    case T1_CTRLR_CAS_CHANNEL:
	dsx1_setup_cas(csb, dsx1);
	break;

    case T1_CTRLR_TONE_A_LAW:
	if (csb->nvgen) {
		nv_write(dsx1_force_a_law != FALSE, "compand a-law");
	} else {
        if (csb->sense)
			dsx1_force_a_law = TRUE;
		else
			dsx1_force_a_law = FALSE;
	    reg_invoke_dsx1_set_a_law(csb->sense, dsx1);
	}
	break;
	   
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * make_bit_field
 * create a bit field with the passed bits set.
 * 2^31 is timeslot 1. 2^30 is timeslot 2...
 */
int make_bit_field (int start, int end)
{
    int retval;
    
    for (retval = 0 ; start <= end ; start++)
	retval |= 1 << (32 - start);
    return(retval);
}

/*
 * dsx1_change_idb_state
 * Change the state of a particular IDB
 */
void dsx1_change_idb_state (hwidbtype *idb, enum IDB_STATES newstate)
{

    net_cstate(idb, newstate);
    idb->counters.transitions++;

    /* Use queue for L1 events */
    if (is_isdn(idb)){
      if (newstate == IDBS_UP)
       reg_invoke_pri_enq_prim(idb->isdn_info->dsl, ISDN_PH_ACT_IND);
      else
       reg_invoke_pri_enq_prim(idb->isdn_info->dsl, ISDN_PH_DEACT_IND);
    }  

    (*idb->reset)(idb);
	
    if (idb->counters.resets > 0)
        idb->counters.resets--;		/* that one didn't count */
}

/*
 * dsx1_change_all_idbs_state
 * Change all the idbs associated with a controller at once.  Do not change
 * idbs who are in ADMIN_DOWN state, though.
 */
void dsx1_change_all_idbs_state (cdbtype  *cdb, enum IDB_STATES newstate)
{
    dsx1info *dsx1;
    hwidbtype	*idb;
    int 	vc;

    dsx1 = dsx1_find_by_unit(cdb->unit);
    FOR_ALL_DSX1_CHANNELS(vc,dsx1) {
	idb = dsx1->hwidb[vc];
	if (idb != NULL) {
	    idb->ext_loop_type = DSX1_LOOP_NONE;
	    idb->far_end_looped = FALSE;
	    if ((idb->state != IDBS_ADMINDOWN) &&
		((!(is_isdn(idb))) ||
		 (idb->isdn_info->chantype == ISDN_Dchan))) {
		dsx1_change_idb_state(idb, newstate);
	    }
	}
    }
}

/*
 * dsx1_init_mib_data
 * Initializes the MIB data for a dsx1
 */
void dsx1_init_mib_data (dsx1info *dsx1)
{
    cdbtype *cdb;

    cdb = dsx1->cdb;
    dsx1->MIB.C.dsx1TransmitClockSource = dsx1->clocking;
    dsx1->MIB.C.dsx1LineCoding = dsx1->linecode;
    dsx1->MIB.C.dsx1LineType = dsx1->framing;
    dsx1->MIB.C.dsx1LoopbackConfig = DSX1_NO_LOOP;
    if (cdb->cdb_state == IDBS_ADMINDOWN) {
	dsx1->MIB.C.dsx1LineStatus = DSX1_TX_AIS | DSX1_RX_LOS;
    } else {
	dsx1->MIB.C.dsx1LineStatus = DSX1_RX_LOS;
    }
    /*
     * The below are for the SNMP tables.  These are the default
     * values for RFC 1406.  They will never be used, so I didn't
     * include equates for them
     */
    dsx1->MIB.C.dsx1SendCode = 1;
    dsx1->MIB.C.dsx1SignalMode = 1;
    dsx1->MIB.C.dsx1Fdl = 8;
    
    dsx1_clear_mib_data(dsx1);
}

/*
 * dsx1_clear_mib_data
 * Clears the MIB data for a dsx1
 */
void dsx1_clear_mib_data(dsx1info *dsx1)
{
    dsx1->MIB.CurrentIndex = 0;
    dsx1->MIB.C.dsx1ValidIntervals = 0;
    dsx1->MIB.C.dsx1TimeElapsed = 0;

    memset(&dsx1->MIB.CurrentData, 0, sizeof(dsx1TableType));
    memset(&dsx1->MIB.TotalData, 0, sizeof(dsx1TotalTableType));
    memset(&dsx1->MIB.Intervals, 0, sizeof(dsx1->MIB.Intervals));
}


/*
 * dsx1_find_SNMP
 * return pointer to SNMP data for dsx1
 */
dsx1MIBDataTable *dsx1_find_SNMP (long *index, boolean next)
{
    cdbtype *cdb;
    dsx1info *dsx1;

    if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_T1, next))
                == NULL) {
         if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_E1, next))
                     == NULL) {
	      return(NULL);
         }
    }

    dsx1 = dsx1_find_by_unit(cdb->unit);
    return(&dsx1->MIB);
}


/*
 * =====================================================================
 * dsx1_get_idbtype()
 *
 * Description:
 * Return a idbtype value based on the controller type.
 *
 * Parameters:
 * cdb - pointer to the controller data block for this controller 
 *
 * Returns:
 * an idbtype value
 * =====================================================================
 */
enum IDBTYPE dsx1_get_idbtype (cdbtype *cdb)
{
    enum IDBTYPE type;

    switch (dsx1_list[cdb->unit]->applique_type) {
    case APPL_T1:
    case APPL_T1_CSU:
        type = IDBTYPE_T1;
        break;
    case APPL_E1_BAL:
    case APPL_E1_UNBAL:
        type = IDBTYPE_E1;
        break;
    default:
        type = IDBTYPE_UNKNOWN;
    }

    return(type);
}
