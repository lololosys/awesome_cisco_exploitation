/* $Id: if_c3000_lance.c,v 3.3.60.2 1996/06/13 19:08:00 etrehus Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_lance.c,v $
 *------------------------------------------------------------------
 * if_c3000_lance.c  AMD Lance chip ethernet driver for C3000
 *
 * December 1989, Chris Shaker
 * Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 *------------------------------------------------------------------
 * $Log: if_c3000_lance.c,v $
 * Revision 3.3.60.2  1996/06/13  19:08:00  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.3.60.1  1996/03/18  20:41:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/09  05:07:26  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.24.1  1996/02/20  14:41:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:38:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/07  00:28:42  ssangiah
 * CSCdi38231:  Reserve cookie value for new development
 * Knowledge of this cookie value for the new platform has also been added
 * to the init code for the appropriate drivers so that the boot ROMs from
 * this release will initialize the corresponding interfaces on the
 * platform properly.
 *
 * Revision 2.3  1995/06/21  03:00:57  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:47:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:32:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "../../boot/src-68-c3000/console.h"
#include "logger.h"
#include "if_les.h"
#include "if_les_lance.h"  
#include "../if/network.h"
#include "ieee.h"


/*
 * set up regaddr for pancake
 */
void lance_get_regaddr (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    ds->lance_regaddr = NULL;

    if (idb->unit == 0) {
	switch (cookie->interfaces) {
	  case INTERFACE_1E1T:
	  case INTERFACE_2E:
	  case INTERFACE_1E2T:
	  case INTERFACE_1E1B:
	  case INTERFACE_1E1T1B:
	  case INTERFACE_1E2T8A:
	  case INTERFACE_1E1T1I:
	  case INTERFACE_1E2T1B:
	  case INTERFACE_1E2T1Bhub:
	  case INTERFACE_2E2T:
	  case INTERFACE_1E2Thub:
	  case INTERFACE_1E2T1B2AS:
	  case INTERFACE_1E1B2W:
	    ds->lance_regaddr = (lance_hw_reg_t *)ADRSPC_NETWORK_A;
	    switch (cpu_type) {
	      case CPU_CRISTAL:
	      case CPU_CANCUN:
		network_b_idb = idb; /* Save IDB ptr for level 4 int wrapper */
		ds->reset_reg = (u_short *)ADRSPC_RESET;
		ds->reset_mask = INTERRUPT_NETWORK_B;
		break;
	      default:
		network_a_idb = idb; /* Save IDB ptr for level 4 int wrapper */
		if (cookie->processor == PROCESSOR_BRUT)
		    ds->reset_reg = (u_short *)ADRSPC_CONTROL3;
		else
		    ds->reset_reg = (u_short *)ADRSPC_CONTROL;
		ds->reset_mask = CONTROL_RESET_NETWORK_A;
	    }
	    break;

	  case INTERFACE_1R1E:
	    ds->lance_regaddr = (lance_hw_reg_t *)ADRSPC_NETWORK_B;
	    network_b_idb = idb; /* Save IDB ptr for level 4 int wrapper */
	    ds->reset_reg = (u_short *)ADRSPC_CONTROL;
	    ds->reset_mask = CONTROL_RESET_NETWORK_B;
	    break;

	  case INTERFACE_1R2T1E:
            ds->lance_regaddr = (lance_hw_reg_t *)ADRSPC_NETWORK_B;
            network_a_idb = idb; /* Save IDB ptr for level 4 int wrapper */
            ds->reset_reg = (u_short *)ADRSPC_RESET;
            ds->reset_mask = INTERRUPT_NETWORK_E;

	  default:
	    break;
        }
    } else if (idb->unit == 1) {
	switch (cookie->interfaces) {
	  case INTERFACE_2E:
	    ds->lance_regaddr = (lance_hw_reg_t *)ADRSPC_NETWORK_B;
	    network_b_idb = idb; /* Save IDB ptr for level 4 int wrapper */
	    ds->reset_reg = (u_short *)ADRSPC_CONTROL;
	    ds->reset_mask = CONTROL_RESET_NETWORK_B;
	    break;

	  case INTERFACE_2E2T:
            ds->lance_regaddr = (lance_hw_reg_t *)ADRSPC_NETWORK_B;
            network_a_idb = idb; /* Save IDB ptr for level 4 int wrapper */
            ds->reset_reg = (u_short *)ADRSPC_RESET;
            ds->reset_mask = INTERRUPT_NETWORK_E;
	    break;
        }
    }
}


/*
 * lance_set_vectors()
 * Setup interrupt type stuff
 */
void lance_set_vectors (hwidbtype *idb)
{
    switch (cpu_type) {
      case CPU_CRISTAL:
      case CPU_CANCUN:
        switch (cookie->interfaces) {
          case INTERFACE_1R2T1E:
            pan_int_array[1].function = (ulong)level4_pan_e1;
            pan_int_array[1].idb1 = idb;
            break;
          case INTERFACE_2E2T:
            if (idb->unit == 0) {
                pan_int_array[0].function = (ulong)level4_pan_e0;
                pan_int_array[0].idb1 = idb;
            } else if (idb->unit == 1) {
                pan_int_array[1].function = (ulong)level4_pan_e1;
                pan_int_array[1].idb1 = idb;
            }
            break;
          default:
            pan_int_array[1].function = (ulong)level4_pan_e0;
            pan_int_array[1].idb1 = idb;
            break;
        }
        break;

      default:
	if (idb->unit == 0) {
	    pan_int_array[0].function = (ulong)level4_pan_e0;
	    pan_int_array[0].idb1 = idb;
	} else {
	    pan_int_array[1].function = (ulong)level4_pan_e1;
	    pan_int_array[1].idb1 = idb;
	}
	
    }
}


/*
 * Reset a Lance:
 */
void lance_reset (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    if (ds->reset_mask) {
	*(ds->reset_reg) |= ds->reset_mask;    
	asm("nop");
	asm("nop");
	*(ds->reset_reg) &= ~ds->reset_mask;
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
    charlong host;
    uchar *ptr;
    int i;

    /*
     * Copy the ethernet 0 hardware address into addr:
     */
    if (cookie->processor == PROCESSOR_BRUT)
        ptr = (uchar *)ADRSPC_BRUT_ENET;
    else
        ptr = cookie->ether0_hw_addr;

    ieee_copy(ptr, (uchar *)addr);
    
    if ((idb->unit) || (cookie->interfaces == INTERFACE_1R2T1E)) {

	host.d.byte[0] = 0;	/* Copy the 24 bit host number from cookie */
	for (i = 1; i < 4; i++)
	    host.d.byte[i] = cookie->ether0_hw_addr[i + 2];

	/* Increment the host number appropriately */
	host.d.lword++;

	for (i = 1; i < 4; i++)	/* Copy incremented host number to addr */
	    addr[i + 2] = host.d.byte[i];
    }
}


/*
 * Show Lance structure info
 */
void lance_show_structure_info (hwidbtype *idb)
{
    lance_instance_t *ds = INSTANCE;

    printf("idb %#x, ds %#x, regaddr = %#x, reset_mask %#x\n",
	   idb, ds, ds->lance_regaddr, ds->reset_mask);
}


/*
 * Provide support for Vendor hooks into the lance_periodic code
 */
void lance_update_vendor_states (hwidbtype *idb)
{
    if (cookie->processor == PROCESSOR_BRUT) {
	if (hw_interface_up(idb))
	    uartaddr->b.aux3 = 0x10; /* Turn on Ethernet OK LED */
	else
	    uartaddr->b.aux4 = 0x10; /* Turn off Ethernet OK LED */
    }
}


/*
 * init_lance_idb -- set up an idb for an lance chip
 */
static void init_lance_idb (int which)
{
    hwidbtype *idb;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    /*
     * Save our unit number
     */
    idb->unit = which;    /* unit number */

    lance_init_idb_common(idb, NULL);

    /*
     * Initialize the hardware:
     */
    lance_init(idb, FALSE);
}


/*
 * analyze_lance
 * Discover the lance chip.
 */
static int analyze_lance (void)
{
    int num_enet_int;

    /*
     * Are there any ethernets in this box?
     * If so, initialize them.
     */
    switch (cookie->interfaces) {
      case INTERFACE_1E1T:
      case INTERFACE_1E2T:
      case INTERFACE_1R1E:
      case INTERFACE_1E1B:
      case INTERFACE_1E1T1B:
      case INTERFACE_1E2T1B:
      case INTERFACE_1E2T1Bhub:
      case INTERFACE_1E2T8A:
      case INTERFACE_1E1T1I:
      case INTERFACE_1R2T1E:
      case INTERFACE_1E2Thub:
      case INTERFACE_1E2T1B2AS:
      case INTERFACE_1E1B2W:
         num_enet_int = 1;
         init_lance_idb(0);
         break;
      case INTERFACE_2E:
      case INTERFACE_2E2T:
         init_lance_idb(0);
         init_lance_idb(1);
         num_enet_int = 2;
         break;
      default:
         num_enet_int = 0;
         break;
    }
    return(num_enet_int);
}


/*
 * Lance subsystem initialisation entry point
 */
void lance_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the Lance Ethernet interfaces in system
     */
    nethernets += analyze_lance();
}

