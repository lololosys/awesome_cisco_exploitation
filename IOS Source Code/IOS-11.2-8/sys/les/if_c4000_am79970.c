/* $Id: if_c4000_am79970.c,v 3.6.20.3 1996/05/21 18:01:54 kew Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_am79970.c,v $
 *------------------------------------------------------------------
 * if_c4000_am79970.c - The Am79970 specific functions.
 *
 * Aug 5 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_am79970.c,v $
 * Revision 3.6.20.3  1996/05/21  18:01:54  kew
 * CSCdi51927:  sh contr e gives wrong Link State for 6-port eth (AM79970)
 * Branch: California_branch
 * Link State was read from wrong AM79970-register.
 *
 * Revision 3.6.20.2  1996/04/06  00:50:10  vdukki
 * CSCdi48472:  Sustained process switched throughput degraded 28%
 * Branch: California_branch
 * Modified the lance_stop() function to add 100 microseconds delay
 * only for 6E NIM.
 *
 * Revision 3.6.20.1  1996/03/21  22:52:42  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.2.5  1996/02/08  08:12:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.4  1996/01/15  23:11:29  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.3  1995/12/11  18:56:10  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.2  1995/11/30  01:24:02  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/11/22  20:31:02  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.6  1996/02/01  06:06:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/23  00:01:13  smackie
 * Make subsystem name lowercase. (CSCdi46085)
 *
 * Revision 3.4  1995/12/01  01:59:48  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.3  1995/11/17  17:38:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/19  06:47:08  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  21:37:56  yoko
 * CSCdi35809:  Program Guinness LEDs
 * Enable Link status, RX, and TX LEDs
 *
 * Revision 2.1  1995/06/07  21:33:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "subsys.h"
#include "config.h"
#include "logger.h"
#include "if_les.h"  
#include "if_les_lance.h"
#include "if_les_lance_inline.h"
#include "parser.h"
#include "parser_defs_xx.h"
#include "if_c4000_pci.h"
#include "if_c4000_am79970.h"


/*
 * =====================================================================
 * am79970_pci_init -
 *
 * Description:
 * Perform PCI configuration specific to the AM79970.
 * 
 * Parameters:
 * slot #, pci_nim_instance ptr, controller #.
 *
 * Returns:
 * void
 * =====================================================================
 */
static uint am79970_pci_init (pci_nim_instance_t *ds,
			      uchar device_no,
			      uint slot_base)
{
    ulong  value;
    uint   device_base;

    /* Do PCI configuration generic to the NIM */
    device_base = pci_nim_configure_device(ds, device_no, slot_base);
    device_base &= 0xFFFFFFF0;

    /* Program I/O enable and bus master mode */
    value = PCI_COMMAND_BMEN | PCI_COMMAND_IOEN;
    pci_config_write(ds->slot_no, device_no, 0, PCI_COMMAND_REG_OFFSET, 
		     value); 
    pci_config_write(ds->slot_no, device_no, 0, PCI_LATENCY_OFFSET,
		     PCI_LATENCY_TIMER_DEFAULT);
    return(device_base);
}


/*
 * =====================================================================
 * am79970_post_init -
 *
 * Description:
 * Post init for the controller to enable the burst mode.
 * called only after interrupts are disabled.
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void
 * =====================================================================
 */
static void am79970_post_init (hwidbtype *idb)
{
   am79970_instance_t *ds;

   ds = AM79970_INSTANCE(idb->slot, idb->subunit);
   ds->am79970_regaddr->rap = AM79970_BCR18;
   ds->am79970_regaddr->bdp = AM79970_BCR18_BREADE |
       AM79970_BCR18_BWRITE;
   ds->am79970_regaddr->rap = LANCE_CSR0;	

}


/*
 * =====================================================================
 * am79970_init_controller -
 *
 * Description:
 * Initialise the chip specific paramters on each init/restart.
 * Called after the STOP bit has been set.
 * Called only after interrupts are disabled.
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_init_controller (hwidbtype *idb)
{
    lance_instance_t *ds;
    volatile ushort mode;
    am79970_instance_t *am79970_ds;
 
    ds = INSTANCE;
    am79970_ds = AM79970_INSTANCE(idb->slot, idb->subunit);

    mode = lance_get_short(&(ds->ib->mode), ds->mode);
    mode = mode | AM79970_PORTSEL;
    lance_put_short(&(ds->ib->mode), mode, ds->mode); 
    				/* set in 10baset always */

    ds->lance_regaddr->rap = AM79970_CSR3;
    ds->lance_regaddr->csr |= AM79970_CSR3_DXSUFLO;
    
    
    /*ds->lance_regaddr->rap = AM79970_CSR4;
    ds->lance_regaddr->csr |= AM79970_CSR4_DMAPLUS;*/	/* set dma plus
							 to disable burst
							 control register*/
    /* delay required by hw to prevent bus errors*/
    DELAY(1);
    ds->lance_regaddr->rap = AM79970_CSR5;
    ds->lance_regaddr->csr = 0x0000;	/* We might want to set System
					  interrupt later */
    /*
     * Disable burst mode.
     */
    am79970_ds->am79970_regaddr->rap = AM79970_BCR18;
    am79970_ds->am79970_regaddr->bdp = 0x0;		/* bus control */

    /*
     * the dbus cant do more than 4 burst cycles at a stretch.
     */
    ds->lance_regaddr->rap = AM79970_CSR80;
    ds->lance_regaddr->csr = (ds->lance_regaddr->csr & 0x3f00) | 0x0004;

    ds->lance_regaddr->rap = LANCE_CSR0;      /* back to point to CSR0 */
}


/*
 * =====================================================================
 * am79970_suspend -
 *
 * Description:
 * To suspend the chip to reconfigure, without stopping it.
 *
 * Parameters:
 * lance instance *, unit #
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_suspend (lance_instance_t *ds, uint unit)
{
    leveltype SR;
    ushort status, i;
    
    /*
     * Set the suspend bit and wait for any data transfer to complete.
     */
    SR = raise_interrupt_level(ETHER_DISABLE);
    ds->lance_regaddr->rap = AM79970_CSR5;
    ds->lance_regaddr->csr |= AM79970_CSR5_SUSPEND;
    ds->lance_regaddr->rap = LANCE_CSR0;
    reset_interrupt_level(SR);		/* restore interrupt level */

    /*
     * Give a maximim wait of 5 ms for suspend
     */
    for (i = 10; i > 0; i--) {
	SR = raise_interrupt_level(ETHER_DISABLE);
	ds->lance_regaddr->rap = AM79970_CSR5;
	status = ds->lance_regaddr->csr & AM79970_CSR5_SUSPEND;
	ds->lance_regaddr->rap = LANCE_CSR0;
	reset_interrupt_level(SR);
	if (status) 
	    break;
	usecdelay(500);
    }

    if (i == 0) {
	if (ether_debug)
	    buginf("\nAM79970(%d): am79970_suspend failed", unit);
    }
}


/*
 * =====================================================================
 * am79970_resume -
 *
 * Description:
 * To bring the chip out of susepend mode.
 *
 * Parameters:
 * lance instance *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_resume (lance_instance_t *ds)
{
    leveltype SR;
    
    /*
     * Reset the suspend bit get ready to start again.
     */
    SR = raise_interrupt_level(ETHER_DISABLE);
    ds->lance_regaddr->rap = AM79970_CSR5;
    ds->lance_regaddr->csr &= ~(AM79970_CSR5_SUSPEND);
    ds->lance_regaddr->rap = LANCE_CSR0;
    reset_interrupt_level(SR);		/* restore interrupt level */
}


/*
 * =====================================================================
 * am79970_get_squelch -
 *
 * Description:
 * To provide compatibility to the lance squelch command.
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static boolean am79970_get_squelch (hwidbtype *idb)
{
    am79970_instance_t *ds_am79970;

    ds_am79970 = AM79970_INSTANCE(idb->slot, idb->subunit);
    return(ds_am79970->squelch_status);
}


/*
 * =====================================================================
 * am79970_set_squelch -
 *
 * Description:
 * To provide compatibility to the lance squelch command.
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_set_squelch (hwidbtype *idb, int squelch_type)
{
    lance_instance_t *ds;
    am79970_instance_t *ds_am79970;
    ushort  mode;

    ds = INSTANCE;
    ds_am79970 = AM79970_INSTANCE(idb->slot, idb->subunit);
   
    am79970_suspend(ds, idb->unit);
    switch(squelch_type) {
      case PARSER_SQUELCH_NORMAL:
	mode = lance_get_short(&(ds->ib->mode), ds->mode);
	lance_put_short(&(ds->ib->mode), (mode &~(AM79970_SQUELCH)), ds->mode);
	ds_am79970->squelch_status = FALSE;
	break;
	
      case PARSER_SQUELCH_REDUCED:
	mode = lance_get_short(&(ds->ib->mode), ds->mode);
        lance_put_short(&(ds->ib->mode), (mode | AM79970_SQUELCH), ds->mode);
	ds_am79970->squelch_status = TRUE;
	break;
    }
    am79970_resume(ds);
}


/*
 * =====================================================================
 * am79970_show_structure_info -
 *
 * Description:
 * To provide backward compatibility to the lance show structure.
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_show_structure_info (hwidbtype *idb)
{
    lance_instance_t *ds_lance;
    am79970_instance_t *ds_am79970;
    leveltype SR;

    ds_lance = INSTANCE;
    ds_am79970 = AM79970_INSTANCE(idb->slot, idb->subunit);

    /*
     * We have to disable interrupts here because RAP
     * should point back to CSR0 for any interrupt.
     */
    SR = raise_interrupt_level(ETHER_DISABLE);
    ds_am79970->am79970_regaddr->rap = AM79970_BCR4;
    if (ds_am79970->am79970_regaddr->bdp & AM79970_BCR4_LEDOUT) 
	ds_am79970->link_status = TRUE;
    else
	ds_am79970->link_status = FALSE;  
    ds_am79970->am79970_regaddr->rap = LANCE_CSR0;
    reset_interrupt_level(SR);
    
    printf("NIM slot %d, NIM type code %d, NIM version %d\n",
           idb->slot, idb->nim_type, idb->nim_version);
    
    /*
     * Print up some information on the ethernet media
     */
    printf("Media Type is 10BaseT");
    
    printf(", Link State is %s",
	   (ds_am79970->link_status) ? "Up" : "Down");
    printf(", Squelch is %s",
	   (ds_am79970->squelch_status) ? "Reduced" : "Normal");
    printf("\n");
    
    printf("idb %#x, ds %#x, eim_regs = %#x\n", idb, ds_lance,
	   ds_am79970->am79970_regaddr);
}


/*
 * =====================================================================
 * am79970_get_regaddr -
 *
 * Description:
 * Overlays the AM79970 csr/raps over the lance csr/raps.
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_get_regaddr (hwidbtype *idb)
{
    lance_instance_t 	*lance_ds;
    am79970_instance_t *am79970_ds;

    lance_ds = INSTANCE;
    am79970_ds = AM79970_INSTANCE(idb->slot, idb->subunit);

    /*
     * Map the am79970 rap/rdp to lance structure.
     */
    lance_ds->lance_regaddr =
	(xx_eim_lance_regs_t *)((volatile uint) &(am79970_ds->am79970_regaddr->rdp));
}


/*
 * =====================================================================
 * am79970_reset -
 *
 * Description:
 * Software reset of AM79970.
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_reset (hwidbtype *idb)
{
/* guinness - amd suggests avoiding software resets */
/*     am79970_instance_t *ds;
    lance_instance_t   *lance_ds;
    volatile ushort    reset;

    ds = AM79970_INSTANCE(idb->slot, idb->subunit);
    lance_ds = INSTANCE;
    
    am79970_suspend(lance_ds, idb->unit);
    reset = ds->am79970_regaddr->reset;
    am79970_resume(lance_ds);
 */
}

/*
 * Set the function vectors for the am79970 chip, specific to c4000.
 */
static void am79970_set_c4000_vectors (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = INSTANCE;

    ds->set_media_type = NULL;
    ds->get_squelch = am79970_get_squelch;
    ds->set_squelch = am79970_set_squelch;
}


/*
 * =====================================================================
 * am79970_set_function_vectors -
 *
 * Description:
 * Set the vectors in the instance to point to the approproiate
 * functions, specific to the lance driver(low-end).
 *
 * Parameters:
 * hwidbtype *
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_set_function_vectors (lance_vectors_t *vec)
{
    vec->get_regaddr = am79970_get_regaddr;
    vec->reset = am79970_reset;
    vec->set_vectors = am79970_set_vectors;
    vec->ctrlr_pre_init = am79970_init_controller;
    vec->ctrlr_post_init = am79970_post_init;
    vec->show_structure_info = am79970_show_structure_info;
    vec->delay_vector = usecdelay;
}


/*
 * =====================================================================
 * am79970_init_idb -
 *
 * Description:
 * Set up an idb for a am79970 interface.
 *
 * Parameters:
 * slot # -> which slot the interface belongs to.
 * unit # -> the global index for this ethernet interface.
 * subunit -> subunit # in the slot.
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void am79970_init_idb (uint slot, uint unit, uint subunit)
{
    hwidbtype 		*idb;
    am79970_instance_t  *am79970_ds;
    lance_vectors_t 	*vec;
    
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

    idb->nim_type = nim_get_type(slot);
    idb->nim_version = nim_get_version(slot);

    /*
     * Set the appropriate am79970 vectors
     */
    vec = malloc(sizeof(lance_vectors_t));
    if (!vec) {
	crashdump(0);
    }

    am79970_set_function_vectors(vec);
    lance_init_idb_common(idb, vec);
    idb->typestring = "Am79c970";
    
    /*
     * Set the C4000 specific am79970 vectors
     */
    am79970_set_c4000_vectors(idb);
    
    am79970_ds = AM79970_INSTANCE(slot, subunit);
    am79970_ds->lance_ds = INSTANCE;

    /*
     * Initialize the hardware:
     */
    lance_init(idb, FALSE);
}


/*
 * =====================================================================
 * am79970_analyze_device -
 *
 * Description:
 * Init the am79970 interface and instance.
 *
 * Parameters:
 * slot # -> which slot the interface belongs to.
 * device_no -> subunit # in the slot.
 *
 * Returns:
 *  
 * =====================================================================
 */
static boolean am79970_analyze_device (uint slot, uint device_no)
{
    am79970_instance_t *ds;
    pci_nim_instance_t *ds_pci;
    uint device_base, slot_base;
    
    ds_pci = PCI_NIM_INSTANCE(slot);
    slot_base = (uint) GET_SLOT_BASE(slot);

    /*
     * Perform PCI configuration specific to the AM79970.
     */
    device_base = am79970_pci_init(ds_pci, device_no, slot_base);
    
    /*
     * Do the presido specific initialisations to set it in the lance
     * compatible mode.
     */
    ds = malloc(sizeof(am79970_instance_t));
    if (!ds) {
	crashdump(0);
    }
    ds->am79970_regaddr = (struct eim_am79970_regs *) device_base;

    /*
     * Configure BCRs. All these should be onetime initialisations
     * unaffected by the software reset/stop bit.
     */
    /*ds->am79970_regaddr->rap = AM79970_BCR2;
    ds->am79970_regaddr->bdp = AM79970_BCR2_ASEL_DISABLE; *//* auto select*/

    /*
     * Full duplex mode --- maybe later.
     */
#ifdef AM79970_FULL_DUPLEX
    ds->am79970_regaddr->rap = AM79970_BCR9;
    ds->am79970_regaddr->bdp |= AM79970_BCR9_FDEN;	 /* full duplex */
#endif
    
    ds->am79970_regaddr->rap = LANCE_CSR0;
    ds->am79970_regaddr->rdp = LANCE_CSR0_STOP;	/* stop */
    usecdelay(100);  /* minimum delay required by hw after stop */

    ds->am79970_regaddr->rap = AM79970_BCR20;
    ds->am79970_regaddr->bdp = AM79970_BCR20_LANCE_SW_STYLE;

    /*
     * Enable led output for Link status, receive, and transmit
     * With Rev 1 NIMs, BCR5 is used for the middle (RX) LED.
     * With Rev 2 NIMs, BCR6 is used instead for the middle (RX) LED. 
     */
    ds->am79970_regaddr->rap = AM79970_BCR4;
    ds->am79970_regaddr->bdp = AM79970_BCR4_LINKSTE; 

    ds->am79970_regaddr->rap = AM79970_BCR5;
    ds->am79970_regaddr->bdp = AM79970_BCR5_RX; 

    ds->am79970_regaddr->rap = AM79970_BCR6;
    ds->am79970_regaddr->bdp = AM79970_BCR6_RX;

    ds->am79970_regaddr->rap = AM79970_BCR7;
    ds->am79970_regaddr->bdp = AM79970_BCR7_TX;

    /*
     * Init instance variables.
     */
    ds->squelch_status = FALSE;
    ds->link_status = FALSE;
    
    /*
     * Set the device ptr for future access.
     */
    ds_pci->device_ds[device_no] = ds;
    am79970_init_idb(slot, nethernets++, device_no);
    return(TRUE);
}


/*
 * =====================================================================
 * am79970_subsys_init -
 *
 * Description:	am79970 subsystem initialisation entry point
 *
 * Parameters:
 * subsys:	subsystype ptr.
 *
 * Returns:
 * Nothing.
 * 
 * =====================================================================
 */
static void am79970_subsys_init (subsystype *subsys)
{
    reg_add_pci_device_init(AM79970_VENDOR_DEVICE_ID,
			    am79970_analyze_device,
			    "am79970_analyze_device");
}

/*
 * am79970 subsystem header
 */

#define AM79970_MAJVERSION   1
#define AM79970_MINVERSION   0
#define AM79970_EDITVERSION  1

SUBSYS_HEADER(am79970, AM79970_MAJVERSION, AM79970_MINVERSION,
	      AM79970_EDITVERSION,
	      am79970_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      "req: pci_nim, lance");
