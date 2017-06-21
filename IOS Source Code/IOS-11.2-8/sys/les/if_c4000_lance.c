/* $Id: if_c4000_lance.c,v 3.5.12.2 1996/08/28 12:58:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_lance.c,v $
 *------------------------------------------------------------------
 * C4000 lance driver routines
 *
 * 4/92 Steve Southam, Marciano Pitargue
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_lance.c,v $
 * Revision 3.5.12.2  1996/08/28  12:58:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.1  1996/03/18  20:41:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/09  05:07:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.24.2  1996/02/20  01:08:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  21:26:13  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1996/02/01  06:06:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.24.1  1996/01/24  22:23:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/09  00:21:08  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.4  1995/06/27  01:04:13  yoko
 * CSCdi31775:  "media-type ?" on Guinness NIM should not show any option
 * give error message when user selects AUI on a Guinness (which is
 * 10Base-T only)
 *
 * Revision 2.3  1995/06/19  06:47:21  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:25  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:34:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * if_c3000_lance.c  AMD Lance chip ethernet driver for C3000
 *
 *	December 1989, Chris Shaker
 *	Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "parser_defs_xx.h"
#include "logger.h"
#include "if_les.h"  
#include "if_les_lance.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../os/nv.h"
#include "msg_nim.c"			/* Not a typo, see logger.h */


static void lance_set_media_type (hwidbtype *idb)
{
    lance_instance_t *ds;
    ethersb *esb;

    ds = (lance_instance_t *)INSTANCE;
    esb = idb_get_hwsb_inline(idb, HWIDB_SB_ETHER);

    switch (esb->media_type) {
      case ETHER_MEDIA_TYPE_AUI:
	ds->eim_regaddr->xx_csr0 &= ~(EIM_MEDIA_TYPE | EIM_SQUELCH);
	break;
      case ETHER_MEDIA_TYPE_10BASET:
	ds->eim_regaddr->xx_csr0 |= EIM_MEDIA_TYPE;
	break;
    default:
	break;
    }
}


static boolean lance_get_squelch (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;
    return(ds->eim_regaddr->xx_csr0 & EIM_SQUELCH);
}


static void lance_set_squelch (hwidbtype *idb, int squelch_type)
{
    lance_instance_t *ds;

    ds = INSTANCE;
    
    switch (squelch_type) {
      case PARSER_SQUELCH_NORMAL:
	ds->eim_regaddr->xx_csr0 &= ~EIM_SQUELCH;
	break;
      case PARSER_SQUELCH_REDUCED:
	ds->eim_regaddr->xx_csr0 |= EIM_SQUELCH;
	break;
    }
}


/*
 * If we have no nvram and are sitting around without line protocol, 
 * switch to 10baseT type hoping that will help things.  We have to
 * get bootp and rarp information out this device, cant wait for user.
 */
static void update_media_attempt (void)
{
    hwidbtype *idb;
    lance_instance_t *ds;
    list_element *l_elt;

    /*
     * an AUI inteface will be line down until it sees a packet, which
     * assures that it is up.  So give time for that to happen.
     */
    process_sleep_for(ONESEC*2);
 
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_LANCE, l_elt, idb) {
	if ((idb->nim_type == XX_ETHER) ||
	    (idb->nim_type == XX_ETHER_SINGLE) ||
	    (idb->nim_type == XX_ETHER_DUAL)) {
	    if (!nv_valid && !systeminit_flag && !hw_interface_up(idb)) {
		ds = INSTANCE;
		ds->eim_regaddr->xx_csr0 |= EIM_MEDIA_TYPE;
	    }
	}
    }
}


/*
 * lance_get_default_hw_address
 * Get the default hardware address of network interface.
 * Note that the cookie only contains the hardware ethernet address
 * for Ethernet unit number 0. Increment the 24 bit host number for
 * each additional unit.
 */
void lance_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    /*
     * Make sure the old 4000 does not come up with the 6E NIM.
     */
    if (IS_FAMILY(CPU_XX))
	if ((!(cookie->new_xx)) &&
	    (idb->subunit >= OLD_XX_MAX_ETHER_SUBUNITS)) {
	    errmsg(&msgsym(NOHWADDRESS, NIM), OLD_XX_MAX_ETHER_ADDRESSES);
	    crashdump(0);
	}

    nim_get_slot_hardware_address(idb, addr);
}


/*
 * Show Lance structure info
 */
void lance_show_structure_info (hwidbtype *idb)
{
    lance_instance_t *ds = INSTANCE;
    ushort csr0;

    printf("NIM slot %d, NIM type code %d, NIM version %d\n",
           idb->slot, idb->nim_type, idb->nim_version);

    if (cookie->vendor != VENDOR_SYNOPTICS) {
        /*
         * Print up some information on the ethernet media
         */
        printf("Media Type is ");

        csr0 = ds->eim_regaddr->xx_csr0;

        if (csr0 & EIM_MEDIA_TYPE) {
            /*
             * We've got a funky 10BaseT setup
             */
            printf("10BaseT");

            printf(", Link State is %s",
                   (csr0 & EIM_10BASET_LINK_STATUS) ? "Up" : "Down");
            printf(", Squelch is %s",
                   (csr0 & EIM_SQUELCH) ? "Reduced" : "Normal");
            printf("\n");

        } else {
            /*
             * It's vanilla 'ol AUI
             */
            printf("AUI\n");
        }
    }
    printf("idb %#x, ds %#x, eim_regs = %#x\n", idb, ds, ds->eim_regaddr);
}


/*
 * lance_update_vendor_states
 *
 * Called to support vendor specific ethernet hardware. Not used by the c4000
 */
void lance_update_vendor_states (hwidbtype *idb)
{
    return;
}

/*
 * Set the function vectors for the lance chip, specific to c4000.
 */
static void lance_set_c4000_vectors (lance_instance_t *ds)
{
    ds->set_media_type = lance_set_media_type;
    ds->get_squelch = lance_get_squelch;
    ds->set_squelch = lance_set_squelch;
}


/*
 * init_lance_idb -- set up an idb for an lance chip
 */
static void init_lance_idb (uint slot, uint unit, uint subunit)
{
    hwidbtype *idb;
    lance_instance_t *ds;
    ethersb *esb;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }
    
    /*
     * Save our unit number and NIM slot number:
     */
    idb->unit = unit;    /* unit number */
    idb->slot = slot;
    idb->subunit = subunit;

    lance_init_idb_common(idb, NULL);

    /*
     * Set default values
     */
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
    esb->media_type = ETHER_MEDIA_TYPE_AUI;
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);

    /*
     * Set the c4000 specific lance vectors
     */
    ds = INSTANCE;
    lance_set_c4000_vectors(ds);
    
    /*
     * Initialize the hardware:
     */
    lance_init(idb, FALSE);

     /*
      * Save our NIM type code, and NIM version:
      */
    idb->nim_type = ((ushort) ds->eim_regaddr->id_prom[0] & 0xff);
    idb->nim_version = ((ushort) ds->eim_regaddr->id_prom[1] & 0xff);

}

/*
 * lance_analyze_interface
 *
 * Initialize the Lance Ethernet NIM in the given slot
 */
static void lance_analyze_interface (int slot)
{
    ushort type;
    uint  subunit = 0;

    lance_check_version(slot);

    type = nim_get_type(slot);

    if ((type == XX_ETHER) || (type == XX_ETHER_SINGLE)) {
	init_lance_idb(slot, nethernets++, subunit++);
    }
    if (type == XX_ETHER_DUAL) {
	init_lance_idb(slot, nethernets++, subunit++);
	init_lance_idb(slot, nethernets++, subunit++);
    }
}


/*
 * Lance subsystem initialisation entry point
 */
void lance_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM IDs for the various Ethernet NIMs
     */
    reg_add_analyze_interface(XX_ETHER,
			      lance_analyze_interface,
			      "lance_analyze_interface");
    reg_add_analyze_interface(XX_ETHER_SINGLE,
			      lance_analyze_interface,
			      "lance_analyze_interface");
    reg_add_analyze_interface(XX_ETHER_DUAL,
			      lance_analyze_interface,
			      "lance_analyze_interface");

    /*
     * Register some setup guff
     */
    reg_add_media_update_attempt(update_media_attempt, "update_media_attempt");

    /*
     * Register for the media-type command.
     */
    reg_add_media_type_cmd(IDBTYPE_LANCE, lance_media_type_command,
                                         "lance_media_type_command");
}

