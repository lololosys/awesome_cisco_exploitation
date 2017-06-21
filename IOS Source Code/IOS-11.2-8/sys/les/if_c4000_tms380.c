/* $Id: if_c4000_tms380.c,v 3.3.62.1 1996/03/18 20:41:58 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/if_c4000_tms380.c,v $
 *------------------------------------------------------------------
 * if_c4000_tms380.c - C4000 specific routines for les tms380 driver
 *
 * May 1993 - Dit Morse
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_tms380.c,v $
 * Revision 3.3.62.1  1996/03/18  20:41:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:49:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  14:41:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:39:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:47:30  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:35:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "logger.h"
#include "interface_private.h"
#include "../if/tring.h"
#include "../if/tring_common.h"

#include "if_les.h"
#include "if_les_tms380.h"


static void init_tms380_idb (uint slot,     /* slot number */
			     uint unit,	    /* unit number */
			     uint subunit)  /* subunit (of nim) number */
{
    hwidbtype *idb;
    tms380_instance_t *ds;

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

    init_tms380_idb_common(idb);

    /*
     * Initialize the hardware:
     */
    tms380_initialize(idb);
    idb->counters.resets = 0;		/* clear reset count */

     /*
      * Save our NIM type code, and NIM version:
      */
    ds = INSTANCE;
    idb->nim_type = ds->xx_trim_regs->id_prom & 0xff;
    idb->nim_version = ds->xx_trim_regs->id_version & 0xff;
}


/*
 * tms380_analyze_interface
 *
 * Initialize the TMS380 NIM for the given slot.
 */
static void tms380_analyze_interface (int slot)
{
    ushort type;
    uint subunit = 0;
    
    tms380_check_version(slot);

    type = nim_get_type(slot);

    switch (type) {
      case XX_TOKEN_RING:                       /* 1R nim */
      case XX_TOKEN_SINGLE:                     /* 2R nim: 1 i/f */
	init_tms380_idb(slot, nTRs++, subunit++);
	break;
	
      case XX_TOKEN_DUAL:                       /* 2R nim: 2 i/f */
	init_tms380_idb(slot, nTRs++, subunit++);
	init_tms380_idb(slot, nTRs++, subunit++);
	break; 
	
      default:
	break;
    } 
}


/*
 * TMS380 subsystem initialisation entry point
 */
void tms380_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM IDs for the various TMS380 NIMs
     */
    reg_add_analyze_interface(XX_TOKEN_RING,
			      tms380_analyze_interface,
			      "tms380_analyze_interface");
    reg_add_analyze_interface(XX_TOKEN_SINGLE,
			      tms380_analyze_interface,
			      "tms380_analyze_interface");
    reg_add_analyze_interface(XX_TOKEN_DUAL,
			      tms380_analyze_interface,
			      "tms380_analyze_interface");
}

/*
 * tms380_get_default_hw_address
 * Get the default hardware address of network interface.
 * Note that the cookie only contains the hardware ethernet address
 * for Ethernet unit number 0. Increment the 24 bit host number for
 * each additional unit.
 */
void tms380_get_default_hw_address (hwidbtype *idb,
				    uchar addr[IEEEBYTES])
{
    nim_get_slot_hardware_address(idb, addr);

    /*
     * Bit swap each byte for tokenring:
     */
    ieee_swap(addr, addr);
}

void tms380_set_ringspeed (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    ushort control2;

    control2 = ds->xx_trim_regs->csr0;
    if (idb->tr_ring_speed == 4) {
        control2 &= ~CONTROL2_4M;
    } else {
        control2 |= CONTROL2_4M;
    }
    DBG1(DBG_MADGE,"%#x\n", control2);
    ds->xx_trim_regs->csr0 = control2;
    DELAY(1);	/* Let things settle down */
}


/*
 * Reset a tms380:
 * May be called from interrupt path.
 */
void tms380_reset (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    leveltype level;
    
    BEG_TIME_380("tms380_reset")
	
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /* Issue a hard reset command to stop the TMS380 */
    level = raise_interrupt_level(ETHER_DISABLE);
    sif_regs->actl = TRACTL_HARDRESET;
    DELAY(1);			/* Hopefully, DMA will complete cleanly */
    
    if (ds) {
	ds->xx_trim_regs->csr0 |= TMS380_RESET; /* Reset the TMS380 */
	DELAY(5);
	ds->xx_trim_regs->csr0 &= ~TMS380_RESET; /* Unreset the TMS380 */
    }
    
    reset_interrupt_level(level);
    END_TIME_380("tms380_reset");
}

void tms380_print_nim_id (hwidbtype *idb)
{
    tms380_instance_t *ds;

    ds = INSTANCE;

    printf("NIM slot %d, NIM type code 0x%x, NIM version %d",
	   idb->slot, idb->nim_type, idb->nim_version);
    switch(idb->nim_type) {
      case XX_TOKEN_SINGLE:
      case XX_TOKEN_DUAL:
	printf(" ASIC revision %d", ds->nim_asic_rev);
	break;
      case XX_TOKEN_RING:
      default:
	break;
    }	
    printf("\n  ");
}
