/* $Id: xx_lance.c,v 3.3.62.2 1996/08/28 13:11:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_lance.c,v $
 *------------------------------------------------------------------
 * xx_lance.c - 68k C4000 specific Lance driver code
 *
 * October 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_lance.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:59:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  23:01:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:43:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */
#include "../les/if_les.h"
#include "../les/if_les_lance.h"


/*
 * Local Storage
 */

static uint eim_int_vector [] = {
    (uint)eim_slot0a,
    (uint)eim_slot1a,
    (uint)eim_slot2a,
    (uint)eim_slot0b,
    (uint)eim_slot1b,
    (uint)eim_slot2b,
};

static uint eim_ichain_jump [] = {
    (uint)eim_slot0a_jump_entry,
    (uint)eim_slot1a_jump_entry,
    (uint)eim_slot2a_jump_entry,
    (uint)eim_slot0b_jump_entry,
    (uint)eim_slot1b_jump_entry,
    (uint)eim_slot2b_jump_entry,
};


/*
 * Set up interrupt chaining
 */
static void
lance_set_ichain (hwidbtype *idb)
{
    int slotoffset;

    if (idb->slot > 2) {
	buginf ("lance_set_ichain: Out of range slot %d for idb 0x%x",
		idb->slot, idb);
	return;
    }

    slotoffset = idb->slot + (idb->subunit * C4000_NUM_SLOTS);

    add_idb_to_ichain(idb->slot,
		      idb->subunit,
		      idb,
		      eim_ichain_jump   [slotoffset]);
}    


/*
 * set up regaddr
 */
void
lance_get_regaddr (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    switch (idb->subunit) {
      case XX_SUBUNIT_A:
        ds->eim_regaddr =
	    (xx_eim_nim_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				  (uint)DEIM_PORT_A_OFFSET);
        ds->lance_regaddr =
	    (xx_eim_lance_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				    (uint)DEIM_PORT_A_OFFSET +
				    (uint)EIM_LANCE_REGS_OFFSET);
        break;
      case XX_SUBUNIT_B:
	ds->eim_regaddr =
	    (xx_eim_nim_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				  (uint)DEIM_PORT_B_OFFSET);
        ds->lance_regaddr =
	    (xx_eim_lance_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				    (uint)DEIM_PORT_B_OFFSET +
				    (uint)EIM_LANCE_REGS_OFFSET);
	break;
      default:
        errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
        crashdump(0);
    }
}


void
lance_set_vectors (hwidbtype *idb)
{
    lance_instance_t *ds;
    uint slotoffset;

    ds = INSTANCE;

    /*
     * Initialize ichain information for interface
     */
    lance_set_ichain(idb);

    set_slot_idb(idb);

    slotoffset = idb->slot + (idb->subunit * C4000_NUM_SLOTS);

    /*
     * Set up interrupt vector table entry, NIM vector register,
     */
    set_slot_vector(idb->slot,
		    idb->subunit,
		    eim_int_vector[slotoffset]);

    ds->eim_regaddr->xx_csr1 = get_slot_vector(idb->slot, idb->subunit);
}

/*
 * reset a Lance:
 */
void
lance_reset (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    remove_idb_from_ichain (idb);

    ds->eim_regaddr->xx_csr0 |= 1;
    ds->eim_regaddr->xx_csr0 &= ~1;
}


/*
 * lance_check_version:
 * The 1E is not supported on Sierra, check for it and whine if we find one
 */
void
lance_check_version (int slot)
{
    /*
     * the 1E is supported on XX
     */
}
