/* $Id: sierra_hd64570.c,v 3.3.62.2 1996/08/28 13:11:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_hd64570.c,v $
 *------------------------------------------------------------------
 * sierra_hd64570.c - R4k C4000 HD64570 specific code
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sierra_hd64570.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:58:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:50:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:41:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:31  hampton
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
#include "../les/if_les_hd64570.h"


void *qsim_wrapper_table[C4000_NUM_SLOTS * 2];



/*
 * hd64570_set_vectors()
 *
 * Set the interrupt wrappers for this serial interface
 */
void hd64570_set_vectors (hwidbtype *idb)
{
    hd64570_instance_t *ds;

    ds = INSTANCE;

    /*
     * Set up parameters in the level 2 jumptable
     */
    set_slot_idb(idb);
    set_slot_vector(idb->slot, qsim_wrapper);

    /*
     * Setup the IACK register. On Sierra the wrapper distinguishes
     * the interrupts associated with ports 0/1 from those associated
     * with ports 2/3 from the IACK register. Ports 0/1  return 0,
     * ports 2/3 return 1.
     */
    ds->regaddr->vector_base = 0;

#ifdef DEBUG
    buginf("\tqsim_slot%d = %x\n",idb->slot, qsim_wrapper);
#endif
}

/*
 * hd64570_get_wrapper_base()
 * Get the base address for the secondary wrappers for this slot.
 */
void *hd64570_get_wrapper_base (uint slot)
{
    return ((void *)&qsim_wrapper_table[slot * 2]);
}


/*
 * hd64570_check_version:
 * Certain C4000 platforms, namely sierra, need the NIMs to be of a
 * certain rev level.  If the NIM isn't of that NIM level, whine about
 * it.
 */
void hd64570_check_version (int slot)
{
    ushort type;
    ushort version;
    type = nim_get_type(slot);
    version = nim_get_version(slot);
    
    switch (type) {
      case XX_SERIAL_QUAD:
	if (version < SIERRA_4T_MIN_VERSION) {
	    errmsg(&msgsym(BADNIMREV, NIM), "Quad Serial", slot, version);
	}
	break;
	
      default:
	/*
	 * Others are OK
	 */
	break;
    }
}
