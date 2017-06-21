/* $Id: sierra_am79970.c,v 3.3 1995/11/17 18:40:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_am79970.c,v $
 *------------------------------------------------------------------
 * sierra_am79970.c - The Am79970 functions specific to sierra.
 *
 * Aug 5 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sierra_am79970.c,v $
 * Revision 3.3  1995/11/17  18:40:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_les_lance.h"
#include "../les/if_c4000_pci.h"


void
am79970_set_vectors (hwidbtype *idb)
{
    lance_instance_t 	*lance_ds;
    pci_nim_instance_t      *ds;

    lance_ds = (lance_instance_t *)INSTANCE;
    ds = PCI_NIM_INSTANCE(idb->slot);

    /*
     * Setup the parameters in the level 2 jumptable
     */
    set_slot_idb(idb);
    set_slot_vector(idb->slot, am79970_wrapper);

    /*
     * Setup the IACK register. On Sierra the wrapper distinguishes
     * among subunits #0-5 from the IACK register.
     */
    ds->nim_regs->int_vector = idb->subunit;
}
