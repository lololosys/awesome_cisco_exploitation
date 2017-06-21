/* $Id: if_c4000_pci.c,v 3.5.54.1 1996/08/28 12:58:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_pci.c,v $
 *------------------------------------------------------------------
 * if_c4000_pci.c - The PCI NIM subsystem specific routines.
 *
 * Aug 5 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_pci.c,v $
 * Revision 3.5.54.1  1996/08/28  12:58:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5  1995/12/01  05:33:48  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 * Incorporate code review comments.
 *
 * Revision 3.4  1995/12/01  01:59:49  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.3  1995/11/17  17:39:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:14:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/19  06:47:27  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:35:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "logger.h"
#include "subsys.h"
#include "sys_registry.h"
#include "if_les.h"
#include "if_c4000_pci.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/*
 * array of pointers to pci instance structures.
 */
pci_nim_instance_t *pci_nim_ds[C4000_NUM_SLOTS];

/*
 * extern ref.
 */

/*
 * Xilinx pointers from the firmware file.
 */
extern ulong pci_xilinx[];
extern ulong pci_end_xilinx[];

/*
 * =====================================================================
 * pci_config_read -
 *
 * Description:
 * Perform a PCI configuration read cycle. Note, the bus number
 * indicates which slot we are referring to.
 *
 * Parameters:
 * Bus no - 
 * Device no -
 * Function -
 * Register offset -
 *
 * Note: Register offset is in bytes
 *
 * Returns:
 * ulong
 *  
 * =====================================================================
 */
ulong pci_config_read (uchar bus_no, uchar device_no, uchar func, 
		       uchar offset)
{
    pci_nim_instance_t *ds;
    leveltype level;
    ulong temp;

    /*
     * Raise interrupt level and find pointers
     */
    level = raise_interrupt_level(ALL_DISABLE);
    ds = PCI_NIM_INSTANCE(bus_no);

    /*
     * Calculate value to be written into addr config register
     */
    temp = (0x80000000 |
	    (((ulong)device_no) << 11) | \
	    (((ulong)func) << 8) | \
	    ((ulong)offset));
    *(ds->config_addr_reg) = temp;

    /*
     * Read back the data
     */
    temp = *(ds->config_data_reg);

    /*
     * Restore interrupt levels and return data
     */
    reset_interrupt_level(level);
    return(temp);
}

/*
 * =====================================================================
 * pci_config_write -
 *
 * Description:
 * Perform a PCI configuration write cycle. Note, the bus number
 * indicates which slot we are referring to.
 *
 * Parameters:
 * Bus no -
 * Device no -  
 * Function -
 * Register offset -
 * Value - 
 *
 * Note: Register offset is in bytes
 *
 * =====================================================================
 */
void pci_config_write (uchar bus_no, uchar device_no, uchar func, 
		       uchar offset, ulong value)
{
    pci_nim_instance_t *ds;
    leveltype level;
    ulong temp;

    /*
     * Raise interrupt level and find pointers
     */
    level = raise_interrupt_level(ALL_DISABLE);
    ds = PCI_NIM_INSTANCE(bus_no);

    /*
     * Calculate value to be written into addr config register
     */
    temp = (0x80000000 |
	    (((ulong)device_no) << 11) | \
	    (((ulong)func) << 8) | \
	    ((ulong)offset));
    *(ds->config_addr_reg) = temp;

    /*
     * Write the data
     */
    *(ds->config_data_reg) = value;

    /*
     * Restore interrupt level
     */
    reset_interrupt_level(level);
}

/*
 * =====================================================================
 * pci_nim_analyze_interface -
 *
 * Description:
 * The general PCI startup routine which updates the PCI NIM instance,
 * sets the register pointers, and calls the generic PCI discover routine.
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * void
 *  
 * =====================================================================
 */
static void pci_nim_analyze_interface (int slot)
{
    pci_nim_instance_t 	*ds;
    uint 		slot_base;
    uchar 		*xilinx_ptr, *xilinx_end;
    ushort              xilinx_done;
    volatile ushort     reset_val, val;
    int                 i;
    
    /*
     * Allocate the pci nim instance. pci_nim_ds is a global array.
     */
    ds = malloc(sizeof(pci_nim_instance_t));
    if (!ds) {
	crashdump(0);
    }
    pci_nim_ds[slot] = ds;
    slot_base = (uint) GET_SLOT_BASE(slot);
    
        
    /*
     * Set the register addresses necessary for PCI configs.
     */
    ds->nim_regs = (pci_nim_block_t *) (slot_base);
    ds->high_order_reg = (ushort *) (slot_base + 
					 (uint) PCI_NIM_HIGH_ORDER_REG_OFFSET);
    ds->config_addr_reg = (ulong *) (slot_base +
					 (uint) PCI_NIM_CONF_ADDR_REG_OFFSET);
    ds->config_data_reg = (ulong *) (slot_base +
					 (uint) PCI_NIM_CONF_DATA_REG_OFFSET);
    ds->slot_no = slot;

    /*
     * Initialise the Xilinx chip.
     */
    xilinx_ptr   = (uchar *) pci_xilinx;
    xilinx_end   = (uchar *) pci_end_xilinx;
    while (xilinx_ptr < xilinx_end) {
	ds->nim_regs->xilinx_base = (ushort) *xilinx_ptr++;
    }

    /* Extra clock cycle needed by hw */
    ds->nim_regs->xilinx_base = 0x00ff;
    xilinx_done = ds->nim_regs->xilinx_base;
    if (xilinx_done != XILINX_DLOAD_DONE){
	errmsg(&msgsym(NOMEMORY,SYS), slot,
	       "Download Xilinx is not successful.\n");
	crashdump(0);
    }
        
    /*
     * Take the NIM out of reset
     */
    ds->nim_regs->reset_control = 0;
    reset_val = ds->nim_regs->reset_control;

    /* long delay necessary to prevent bus errors */
    for (i = 0; i < 200; i++)
         usecdelay(5000);

    ds->nim_regs->reset_control = 0x3f;
    reset_val = ds->nim_regs->reset_control;
    if ((reset_val  & 0x3f) != 0x3f){
	errmsg(&msgsym(NOMEMORY,SYS), slot, "NIM reset not successful. \n");
	crashdump(0);
    }

    val = (volatile ushort) (slot_base >> 16);
    *ds->high_order_reg = val;

    DELAY(30);

    /*
     * Find the devices and configure 'em. We set the bus_no field
     * to the slot so that the config read/write routines know
     * which set of pointers to use.
     */
    pci_discover_devices(slot, PCI_NIM_MAX_DEVICES, slot);
}

/*
 * =====================================================================
 * pci_nim_subsys_init -
 *
 * Description:	PCI NIM subsystem initialisation entry point
 *
 * Parameters:
 * subsys:	subsystype ptr.
 *
 * Returns:
 * Nothing.
 * 
 * =====================================================================
 */
static void pci_nim_subsys_init (subsystype *subsys)
{
    /*
     * Register the PCI NIM. 
     */
     reg_add_analyze_interface(XX_PCI,
			      pci_nim_analyze_interface,
			      "pci_nim_analyze_interface");
}

/*
 * PCI subsystem header
 */

#define PCI_NIM_MAJVERSION   1
#define PCI_NIM_MINVERSION   0
#define PCI_NIM_EDITVERSION  1

SUBSYS_HEADER(pci_nim, PCI_NIM_MAJVERSION, PCI_NIM_MINVERSION,
	      PCI_NIM_EDITVERSION,
	      pci_nim_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);



/*
 * =====================================================================
 * pci_nim_configure_device -
 *
 * Description:
 * The PCI NIM configurations, specific to 4X00 platforms,
 * but generic to any PCI NIM.
 *
 * Parameters:
 * slot #, pci_nim_instance ptr, device #(0 to max devices).
 *
 * Returns:
 * void.
 *  
 * =====================================================================
 */
uint pci_nim_configure_device (pci_nim_instance_t *ds,
			       uchar device_no,
			       uint slot_base)
{
    ulong  device_base;

    /*
     * Program I/O base. Note that consecutive
     * device I/O spaces are separated by
     * PCI_NIM_DEV_IO_SEPARATION.
     */
    device_base = slot_base + PCI_NIM_DEVICE_BASE_OFFSET +
	(device_no * PCI_NIM_DEV_IO_SEPARATION);
    pci_config_write(ds->slot_no, device_no, 0, PCI_IO_BASE_OFFSET, 
		     device_base);
    device_base = pci_config_read(ds->slot_no, device_no, 0, 
				  PCI_IO_BASE_OFFSET);
    return(device_base);
}

