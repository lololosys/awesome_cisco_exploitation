/* $Id: sierra_tms380.c,v 3.3.62.2 1996/08/28 13:11:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_tms380.c,v $
 *------------------------------------------------------------------
 * sierra_tms380.c - R4k based C4000 specific TMS380 driver code
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_tms380.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:58:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:50:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:41:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:23:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../les/if_les.h"
#include "../les/if_les_tms380.h"


static void tms380_get_regaddr (hwidbtype *idb)
{
    tms380_instance_t *ds;

    ds = INSTANCE;

    ds->xx_trim_regs = (xx_trim_regs_t *) NULL;
    switch (idb->subunit) {
      case SIERRA_SUBUNIT_A:
	ds->xx_trim_regs = (xx_trim_regs_t *)((uint)GET_SLOT_BASE(idb->slot)
	    + (uint)TRIM_PORT_A_OFFSET);
	break;
      case SIERRA_SUBUNIT_B:
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
   
    /*
     * Setup the parameters in the level 2 jumptable
     */
    set_slot_idb(idb);
    set_slot_vector(idb->slot, trim_wrapper);

    /*
     * Setup the IACK register. On Sierra the wrapper distinguishes
     * subunit 0 and 1 from the IACK register. Subunit 0 will return 0,
     * Subunit 1 will return 1.
     */
    ds->xx_trim_regs->csr1 = idb->subunit;

}

/*
 * init_sierra380_slot:
 * Initialize a slot with a TMS380 based TR interface
 */
static boolean init_sierra380_slot (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    BEG_TIME_380("init_sierra380_slot");
    
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
    idb->nim_type = ds->xx_trim_regs->id_prom & 0xff;
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


/*
 * init_tms380_dev:
 * Initialize a TMS380 based token ring interface
 */
void init_tms380_dev (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    ushort set_csr0 = 0;

    /*
     * Select the proper assembly interrupt wrapper:
     */
    init_sierra380_slot(idb);
    
    /* set up burst mode on 2R nim */
    switch (idb->nim_type) {
      case XX_TOKEN_SINGLE:                     /* 2R nim: 1 i/f */
      case XX_TOKEN_DUAL:                       /* 2R nim: 2 i/f */

      /* Note:  Channel Hang doesn't occur if Burst mode
       *        is not used. But further investigation
       *        by H/W is required to determine the cause.
       *        Enable the next line once BM problem is fixed.
       */
/*	set_csr0 |= TMS380_2RNIM_BURST_MODE; */
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
    /*
     * Update various vendor bells and whistles.
     * For the time being, Sierra does not support any
     * OEM vendor, so this is a null function.
     *
     */
}


/*
 * tms380_check_version:
 * Certain C4000 platforms, namely sierra, need the NIMs to be of a
 * certain rev level.  If the NIM isn't of that NIM level, whine about 
 * it.
 */
void tms380_check_version (int slot)
{
    ushort type;
    ushort version;
    xx_trim_regs_t *trim_regs;

    type = nim_get_type(slot);
    version = nim_get_version(slot);

    trim_regs = (xx_trim_regs_t *)GET_SLOT_BASE(slot);

    switch (type) {
      case XX_TOKEN_SINGLE:      /* 2R nim; 1 i/f */
	if ((version < SIERRA_2R_1_MIN_VERSION) || (trim_regs->asic_rev < 2))
	    errmsg(&msgsym(BADNIMREV, NIM), "Token Ring", slot, version);
	break;

      case XX_TOKEN_DUAL:        /* 2R nim; 2 i/f */
	if ((version < SIERRA_2R_2_MIN_VERSION) || (trim_regs->asic_rev < 2))
	    errmsg(&msgsym(BADNIMREV, NIM), "Token Ring", slot, version);
	break;

      default:
	/*
	 * Others are OK
	 */
	break;
    }
}
