/* $Id: xx_mk5025.c,v 3.4.44.2 1996/08/28 13:12:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_mk5025.c,v $
 *------------------------------------------------------------------
 * xx_mk5025.c - 68k C4000 specific MK5025 driver code
 *
 * October 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_mk5025.c,v $
 * Revision 3.4.44.2  1996/08/28  13:12:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.1  1996/03/18  21:59:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.1  1996/01/24  23:01:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:23:04  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:43:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:15  hampton
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
 * Local Storage
 */

static uint dsim_int_vector [] = {
    (uint)dsim_slot0,
    (uint)dsim_slot1,
    (uint)dsim_slot2,
};


/*
 * mk5025_get_regaddr()
 *
 * Return the base address of the serial controller block
 * 
 */
void mk5025_get_regaddr (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    ds->dsimaddr = (xx_dsim_regs_t *)GET_SLOT_BASE(idb->slot);

    switch (idb->subunit) {
      case XX_SUBUNIT_A:
        ds->regaddr = &ds->dsimaddr->mk5025_port_A;
        break;
      case XX_SUBUNIT_B:
        ds->regaddr = &ds->dsimaddr->mk5025_port_B;
        break;
      default:
	errmsg(&msgsym(BADSUBUNIT, NIM), idb->subunit, idb->slot);
	crashdump(0);
    }
}


/*
 * mk5025_set_vectors()
 *
 * Set the interrupt wrapprs for this serial interface
 */
void mk5025_set_vectors (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    set_slot_vector_range(idb->slot,
			  0, XX_VECTORS_PER_SLOT,
			  dsim_int_vector   [idb->slot]);
    
    set_slot_idb(idb);
    
    ds->dsimaddr->vector = (DSIM_PORT_A_ENABLE | DSIM_PORT_B_ENABLE) |
	                    get_slot_vector (idb->slot, 0);
}


/*
 * Reset the MK5025:
 */
void mk5025_reset (hwidbtype *idb)
{
    ushort vector;
    mk5025_instance_t *ds;

    ds = INSTANCE;

    vector = get_slot_vector(idb->slot, 0);

    if (C4000_EVEN_UNIT)
        ds->dsimaddr->vector = vector | DSIM_PORT_B_ENABLE;
    else
        ds->dsimaddr->vector = vector | DSIM_PORT_A_ENABLE;

    /*
     * Wait a bit
     */
    usecdelay(5);

    ds->dsimaddr->vector = vector | (DSIM_PORT_A_ENABLE | DSIM_PORT_B_ENABLE);
}
