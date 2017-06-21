/* $Id: xx_tms380.c,v 3.3.62.5 1996/08/28 13:12:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_tms380.c,v $
 *------------------------------------------------------------------
 * xx_tms380.c - 68k C4000 specific TMS380 driver code
 *
 * October 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_tms380.c,v $
 * Revision 3.3.62.5  1996/08/28  13:12:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.4  1996/08/26  15:11:20  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/07/09  06:04:19  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.62.2  1996/05/17  11:53:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/04/26  15:23:59  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  21:41:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.62.1  1996/03/18  21:59:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:12:33  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/01/24  23:01:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:43:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../les/if_les.h"
#include "../les/if_les_tms380.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"


static uint trim_int_vector[3][2] = {
    {(uint)trim_slot1a, (uint)trim_slot1b},
    {(uint)trim_slot2a, (uint)trim_slot2b},
    {(uint)trim_slot3a, (uint)trim_slot3b},
};


static void tms380_get_regaddr (hwidbtype *idb)
{
    tms380_instance_t *ds;

    ds = INSTANCE;

    ds->xx_trim_regs = (xx_trim_regs_t *) NULL;
    switch (idb->subunit) {
      case XX_SUBUNIT_A:
	ds->xx_trim_regs = (xx_trim_regs_t *)((uint)GET_SLOT_BASE(idb->slot)
	    + (uint)TRIM_PORT_A_OFFSET);
	break;
      case XX_SUBUNIT_B:
	ds->xx_trim_regs = (xx_trim_regs_t *)((uint)GET_SLOT_BASE(idb->slot)
	    + (uint)TRIM_PORT_B_OFFSET);
	break;
      default:
        errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
        crashdump(0);
	break;
    }
}

static void tms380_set_vectors (hwidbtype *idb)
{
    tms380_instance_t *ds;

    ds = INSTANCE;

    set_slot_idb(idb);

    /* Set up interrupt vector table entry */

    set_slot_vector(idb->slot,
		    idb->subunit,
		    trim_int_vector[idb->slot][idb->subunit]);

    /* inform the nim of the vector addr */

    ds->xx_trim_regs->csr1 = get_slot_vector(idb->slot, idb->subunit);
}


static boolean init_xx380_slot (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    BEG_TIME_380("init_xx380_slot");
    
    ds = INSTANCE;
    
    /* set up interrupt wrapper.
     * done here for future hot swap?
     */
    
    tms380_get_regaddr(idb);

    if (ds->xx_trim_regs == (xx_trim_regs_t *)NULL) {
	errmsg(&msgsym(BADUNIT, TR), idb->unit);
	idb->state = IDBS_DOWN;
	GET_TIMESTAMP(idb->state_time);
	return(FALSE);
    }    

    idb->nim_type =    ds->xx_trim_regs->id_prom & 0xff;
    idb->nim_version = ds->xx_trim_regs->id_version & 0xff;

    switch(idb->nim_type) {
      case XX_TOKEN_SINGLE:
      case XX_TOKEN_DUAL:
	ds->nim_asic_rev = ds->xx_trim_regs->asic_rev & 0xff;
	break;
      case XX_TOKEN_RING:
      default:
	ds->nim_asic_rev = 0;
	break;
    }    

    DBGBEG(DBG_XX);
    buginf("ds->xx_trim_regs = %x\n",ds->xx_trim_regs);
    buginf("ds->xx_trim_regs->csr0 = %x\n",
	    &((ds->xx_trim_regs)->csr0));
    buginf("ds->xx_trim_regs->csr1 = %x\n",
	    &((ds->xx_trim_regs)->csr1));
    buginf("ds->xx_trim_regs->csr2 = %x\n",
	    &((ds->xx_trim_regs)->csr2));
    buginf("ds->xx_trim_regs->sif_regs = %x\n",
	    &((ds->xx_trim_regs)->sif_regs));
    DBGEND;

    tms380_set_vectors(idb);
    return(TRUE);
}


void init_tms380_dev (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    ushort set_csr0 = 0;

    /*
     * Select the proper assembly interrupt wrapper:
     */
    init_xx380_slot(idb);
    
    /* set up burst mode on 2R nim */
    switch (idb->nim_type) {
      case XX_TOKEN_SINGLE:                     /* 2R nim: 1 i/f */
      case XX_TOKEN_DUAL:                       /* 2R nim: 2 i/f */
	/* disable burst mode for Rev 2 ASICs to avoid bringup problems .. */
    /* This should be replace by :
     *
     *        set_csr0 |= TMS380_2RNIM_BURST_MODE;
     *
     * when the problem has been fixed.
     */
           if (ds->nim_asic_rev != 2)
               set_csr0 |= TMS380_2RNIM_BURST_MODE;
           else
               set_csr0 &= ~TMS380_2RNIM_BURST_MODE; /* On the safe side */

	break;
      case XX_TOKEN_RING:                        /* 1R nim */
      default:
	break;
    }

    /*
     * set page size and base address 
     */
    ds->xx_trim_regs->csr0 = set_csr0;
}


void tms380_vendor_init (hwidbtype *idb)
{
    boolean srb_bridging;
    srb_vector_table_t *srbV_p;

    /*
     * Update various vendor bells and whistles
     */
    if (cookie->vendor == VENDOR_SYNOPTICS) {
        uchar   *leds;

	leds = (uchar *)GET_SLOT_BASE(idb->slot) +
                        SYNOPTICS_TRIM_LED_REGISTER;

	/*
	 * Set Source Routing LED state
	 */
        srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            srb_bridging = srbV_p->srb_sb_get_bridging_enabled(idb->firstsw);
        } else {
            srb_bridging = FALSE;
        }
	if (!srb_bridging) {
	    CLR_SYNOPTICS_TRIM_LEDS(leds, SYNOPTICS_TRIM_LED_SRB);
	} else {
	    SET_SYNOPTICS_TRIM_LEDS(leds, SYNOPTICS_TRIM_LED_SRB);
	}
	
	/*
	 * Set Transparent LED state
	 */
	if (RUNNING_IF(LINK_BRIDGE,idb->firstsw)) {
	    SET_SYNOPTICS_TRIM_LEDS(leds, SYNOPTICS_TRIM_LED_TB);
	} else {
	    CLR_SYNOPTICS_TRIM_LEDS(leds, SYNOPTICS_TRIM_LED_TB);
	}
    }
}

/*
 * tms380_check_version:
 * Certain C4000 platforms, namely sierra, need the NIMs to be of a
 * certain rev level.  If the NIM isn't of that NIM level, whine about 
 * it.
 */

void tms380_check_version (int slot)
{
    /*
     * The XX can handle all the NIMs with no NIM version restriction
     */
}
