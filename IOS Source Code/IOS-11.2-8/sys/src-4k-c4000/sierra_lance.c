/* $Id: sierra_lance.c,v 3.3.60.2 1996/08/28 13:11:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_lance.c,v $
 *------------------------------------------------------------------
 * sierra_lance.c - R4k C4000 specific Lance driver code
 *
 * October 1993, Scott Mackie
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_lance.c,v $
 * Revision 3.3.60.2  1996/08/28  13:11:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.1  1996/03/18  21:58:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.1  1996/01/24  22:50:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:41:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:33  hampton
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
 * set up regaddr
 */
void
lance_get_regaddr (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    switch (idb->subunit) {
      case SIERRA_SUBUNIT_A:
        ds->eim_regaddr =
	    (xx_eim_nim_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				  (uint)DEIM_PORT_A_OFFSET);
	ds->lance_regaddr =
	    (xx_eim_lance_regs_t *)((uint)GET_SLOT_BASE(idb->slot) +
				    (uint)DEIM_PORT_A_OFFSET +
				    (uint)EIM_LANCE_REGS_OFFSET);
        break;
      case SIERRA_SUBUNIT_B:
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

    ds = INSTANCE;

    /*
     * Setup the parameters in the level 2 jumptable
     */
    set_slot_idb(idb);
    set_slot_vector(idb->slot, eim_wrapper);

    /*
     * Setup the IACK register. On Sierra the wrapper distinguishes
     * lance 0 and 1 from the IACK register. Lance 0 will return 0,
     * Lance 1 will return 1.
     */
    ds->eim_regaddr->xx_csr1 = idb->subunit;
}

/*
 * Reset a Lance:
 */
void
lance_reset (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    ds->eim_regaddr->xx_csr0 |= 1;
    ds->eim_regaddr->xx_csr0 &= ~1;
    flush_shmem_wbuffer();
}


/*
 * lance_check_version:
 * The 1E is not supported on Sierra, check for it and whine if we find one
 */
void
lance_check_version (int slot)
{
    ushort type;
    
    type = nim_get_type(slot);

    if (type == XX_ETHER)
	errmsg(&msgsym(UNSUPNIM, NIM), "Ethernet", slot);

}
