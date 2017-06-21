/* $Id: sierra_mk5025.c,v 3.4.44.2 1996/08/28 13:11:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_mk5025.c,v $
 *------------------------------------------------------------------
 * sierra_mk5025.c - R4k C4000 MK5025 specific code
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_mk5025.c,v $
 * Revision 3.4.44.2  1996/08/28  13:11:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.1  1996/03/18  21:58:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.1  1996/01/24  22:50:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:22:57  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:41:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:36  hampton
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
#include "../les/if_les_mk5025.h"


/*
 * mk5025_get_regaddr:
 * Return the base address of the serial controller block
 */
void mk5025_get_regaddr (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    ds->dsimaddr = (xx_dsim_regs_t *)GET_SLOT_BASE(idb->slot);

    switch (idb->subunit) {
    case SIERRA_SUBUNIT_A:
        ds->regaddr = &ds->dsimaddr->mk5025_port_A;
        break;
    case SIERRA_SUBUNIT_B:
        ds->regaddr = &ds->dsimaddr->mk5025_port_B;
        break;
    default:
	errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
	crashdump(0);
    }
}

/*
 * mk5025_set_vectors:
 * Set the interrupt wrapprs for this serial interface
 */
void mk5025_set_vectors (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    /*
     * Setup the parameters in the Level 2 jumptable
     */
    set_slot_idb(idb);
    set_slot_vector(idb->slot, dsim_wrapper);

    /*
     * Setup the IACK register. We only care about the low order 
     * bits in the interrupt acknowledge register.
     */
    ds->dsimaddr->vector = (DSIM_PORT_A_ENABLE | DSIM_PORT_B_ENABLE);
}

/*
 * mk5025_reset:
 * Reset the MK5025.
 */
void mk5025_reset (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    if (C4000_EVEN_UNIT)
        ds->dsimaddr->vector = DSIM_PORT_B_ENABLE;
    else
        ds->dsimaddr->vector = DSIM_PORT_A_ENABLE;

    /*
     * Wait a bit
     */
    usecdelay(5);

    ds->dsimaddr->vector = (DSIM_PORT_A_ENABLE | DSIM_PORT_B_ENABLE);
}

/* End of file */
