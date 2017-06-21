/* $Id: rvip_pci.c,v 3.5.30.6 1996/08/28 13:14:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_pci.c,v $
 *------------------------------------------------------------------
 * rvip_pci.c - RVIP specific pci routines
 *
 * March 14 1995, Paulina Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rvip_pci.c,v $
 * Revision 3.5.30.6  1996/08/28  13:14:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.30.5  1996/06/04  00:31:17  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.5.30.4  1996/04/23  17:14:04  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.5.30.3  1996/04/22  18:32:45  ptran
 * CSCdi55273:  PA should be set to memory map by default
 * Branch: California_branch
 *
 * Revision 3.5.30.2  1996/04/19  15:31:46  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.5  1996/04/09  21:59:13  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.4  1996/02/14  23:53:34  ptran
 * Change the default to use I/O mapped I/O.  Note: This change only
 * effect the IosSec_branch.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.3  1996/01/23  06:51:13  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.2  1995/12/17  02:22:22  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  18:18:13  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.5.30.1  1996/04/02  22:55:25  mbeesley
 * CSCdi53300:  PA power-on sequence can leave bus interface off
 * Branch: California_branch
 *
 * Revision 3.5  1996/01/13  02:51:21  rharagan
 * CSCdi46869:  Enable memory mapping for VIP
 * Change the default OCI access mode.
 *
 * Revision 3.4  1995/12/06  20:48:25  hwang
 * CSCdi45104:  pci io base address for bay 0 need to skip first 8K space
 *
 * Revision 3.3  1995/12/01  02:00:16  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:50:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "logger.h"

#include "../if/pci.h"
#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/dec_ppb.h"
#include "../pas/pas_id.h"
 
/*
 * FIXME:  These defines belong in an rvip-specific header file.
 * Figure out which one!
 */
#define	   PCI_IO_OFFSET_START_PCIBAY_0			0x2000
#define	   PCI_IO_OFFSET_END_PCIBAY_0			0x7FFF
#define	   PCI_IO_OFFSET_START_PCIBAY_1			0xA000
#define	   PCI_IO_OFFSET_END_PCIBAY_1			0xFFFF

/*
 * Bridge 0 config data structure
 */
static pci_bridge_config_t bay0_bridge_config = {
    "PA Bay 0 Upstream PCI-PCI Bridge", /* Description */

    ADRSPC_PCI_IDSEL0,		       	/* Direct access */
    0,				       	/* Device number */
    
    0x00000147,				/* status/command */
    0x00000008,			  	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x1,				/* secondary bus */
    0x1,				/* subordinate bus */
    0x0,				/* secondary latency */
    PCI_IO_OFFSET_START_PCIBAY_0,	/* IO mapped IO base */
    PCI_IO_OFFSET_END_PCIBAY_0,		/* IO mapped IO limit */
    PCI_MEMMAPPED_IO_BAY0,		/* Memory mapped IO base */
    PCI_MEMMAPPED_IO_BAY0+ONE_MEG,	/* Memory mapped IO limit */
    0xfff00000,				/* Memory base */
    0x0,				/* Memory limit */
    0x00000000,				/* Bridge control */
    0x0008,				/* Burst limit */
    0x00,				/* Primary timer */
    0x00				/* Secondary timer */
};

/*
 * Bridge 1 config data structure
 */
static pci_bridge_config_t bay1_bridge_config = {
    "PA Bay 1 Upstream PCI-PCI Bridge", /* Description */

    ADRSPC_PCI_IDSEL1,		       	/* Direct access */
    0,				       	/* Device number */
    
    0x00000147,				/* status/command */
    0x00000008,			  	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x2,				/* secondary bus */
    0x2,				/* subordinate bus */
    0x0,				/* secondary latency */
    PCI_IO_OFFSET_START_PCIBAY_1,	/* IO mapped IO base */
    PCI_IO_OFFSET_END_PCIBAY_1,		/* IO mapped IO limit */
    PCI_MEMMAPPED_IO_BAY1,		/* Memory mapped IO base */
    PCI_MEMMAPPED_IO_BAY1+ONE_MEG,	/* Memory mapped IO limit */
    0xfff00000,				/* Memory base */
    0x0,				/* Memory limit */
    0x00000000,				/* Bridge control */
    0x0008,				/* Burst limit */
    0x00,				/* Primary timer */
    0x00				/* Secondary timer */
};

/*
 * get_bridge_config:
 * Return pointer to a bridge config structure
 */
pci_bridge_config_t *get_bridge_config (uint pa_bay)
{
    if (pa_bay == PA0) {
	return(&bay0_bridge_config);
    } else if (pa_bay == PA1) {
	return(&bay1_bridge_config);
    } else {
	return(NULL);
    }
}

/*
 * get_pci_bus_no:
 * Return PCI bus number
 */
uchar get_pci_bus_no (uint pa_bay)
{
    return(pa_bay+1);
}

/*
 * rvip_iomapped_pa:
 * Return TRUE if this PA should be mapped in the PCI IO space
 */
static boolean rvip_iomapped_pa (uint pa_bay)
{
    return(FALSE);		/* map all others in IO space */
}

/*
 * get_io_base_address:
 * Return the IO base address for a port adaptor
 */
void *get_io_base_address (uint pa_bay)
{
    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(NULL);
    }

    if (rvip_iomapped_pa(pa_bay)) {
	/* 
	 * IO Mapped PA's 
	 */
	if (pa_bay == PA0) {
	    return((void *)ADRSPC_PCIBAY_0);
	} else {
	    return((void *)ADRSPC_PCIBAY_1);
	}
    } else {
	/*
	 * Memory mapped PA's
	 */
	if (pa_bay == PA0) {
	    return((void *)ADRSPC_PCIBAY_0_MM);
	} else {
	    return((void *)ADRSPC_PCIBAY_1_MM);
	}
    }
}

/*
 * get_pci_device_base:
 * Get the base IO address for an interfaces registers from the
 * CPU perspective (IE for ds->reg_ptr).
 */
void *get_pci_device_base (uint pa_bay, uint pa_device)
{
    ulong base;

    if (pa_bay >= MAX_PA_BAYS) {
        errmsg(&msgsym(BADPABAY, PA), pa_bay);
        return(NULL);
    }

    if (rvip_iomapped_pa(pa_bay)) {
        if (pa_bay == PA0) {
            base = ADRSPC_PCIBAY_0;
        } else {
            base = ADRSPC_PCIBAY_1;
        }
        base += pa_device * PCI_DEV_IO_MAPPED_IO_SEPARATION;
    } else {
        if (pa_bay == PA0) {
            base = ADRSPC_PCIBAY_0_MM;
        } else {
            base = ADRSPC_PCIBAY_1_MM;
        }
        base += pa_device * PCI_DEV_MEM_MAPPED_IO_SEPARATION;
    }
    return((void *)base);
}

/*
 * get_pci_device_base_offset:
 * Get the offset from the start of PCI IO space for a device
 * on a port adaptor (IE for programming into the IO base address
 * register of a chip).
 */
ulong get_pci_device_base_offset (uint pa_bay, uint pa_device)
{
    ulong base;

    if (pa_bay >= MAX_PA_BAYS) {
        errmsg(&msgsym(BADPABAY, PA), pa_bay);
        return(0);
    }
    if (rvip_iomapped_pa(pa_bay)) {
        if (pa_bay == PA0) {
            base = PCI_IO_OFFSET_START_PCIBAY_0;
        } else {
            base = PCI_IO_OFFSET_START_PCIBAY_1;
        }
        base += pa_device * PCI_DEV_IO_MAPPED_IO_SEPARATION;
    } else {
        if (pa_bay == PA0) {
            base = PCI_MEMMAPPED_IO_BAY0;
        } else {
            base = PCI_MEMMAPPED_IO_BAY1;
        }
        base += pa_device * PCI_DEV_MEM_MAPPED_IO_SEPARATION;
    }
    return(base);
}

/*
 * pci_config_write :
 * Perform a PCI config write cycle.
 */
void pci_config_write (uchar bus_no, uchar device_no, uchar func, 
		       uchar offset, ulong value)
{
    leveltype level;
    ulong temp;

    level = raise_interrupt_level(ALL_DISABLE);
    temp = (0x80000000 |
	    (((ulong)bus_no) << 16) | 
	    (((ulong)device_no) << 11) | \
	    (((ulong)func) << 8) | \
	    ((ulong)offset) |
	    ((ulong)PCI_TYPE_ONE));
    *(volatile ulong *)(ADRSPC_PMAC) = temp;
    *(volatile ulong *)(ADRSPC_PCI_TYPE1) = value;
    reset_interrupt_level(level);
}

/*
 * pci_config_read :
 * Perform a PCI config read cycle.
 */
ulong pci_config_read (uchar bus_no, uchar device_no, uchar func, 
		       uchar offset)
{
    leveltype level;
    ulong temp;

    level = raise_interrupt_level(ALL_DISABLE);
    temp = (0x80000000 |
	    (((ulong)bus_no) << 16) | 
	    (((ulong)device_no) << 11) | \
	    (((ulong)func) << 8) | \
	    ((ulong)offset) |
	    ((ulong)PCI_TYPE_ONE));
    *(volatile ulong *)(ADRSPC_PMAC) = temp;
    temp = *(volatile ulong *)(ADRSPC_PCI_TYPE1);
    reset_interrupt_level(level);
    return(temp);
}

/*
 * pas_bridge_config:
 * Configure the PA upstream bridge
 */
boolean pas_bridge_config (uint pa_bay)
{
    pci_bridge_config_t *ptr;
    volatile ulong *reg_ptr;
    ulong vendor_id;

    /*
     * Get the bridge config data structure
     */
    ptr = get_bridge_config(pa_bay);
    if (!ptr) {
	return(FALSE);
    }

    /*
     * Read the Vendor/ID and make sure it is a DEC21050
     */
    reg_ptr = (volatile ulong *)(ptr->config_ptr);
    vendor_id = *reg_ptr;
    if (vendor_id != DECPPB_VENDOR_DEVICE_ID) {
	return(FALSE);
    } else {
	return(dec21050_bridge_config(ptr, TRUE));
    }
}

/*
 * pas_pci_controller_summary:
 * Print out a summary of the PCI controller ASIC
 */
void pas_pci_controller_summary (void)
{
    leveltype level;
    ulong status;

    level = raise_interrupt_level(ALL_DISABLE);
    status = *(volatile ulong *)(&PCI_ASIC->pmac_intrpt_status_reg);
    reset_interrupt_level(level);

    printf("PCI Controller Summary : \n");
    printf(" config register = 0x%08x, interrupt mask register = 0x%08x\n",
	   *(ulong *)(&PCI_ASIC->pmac_config_reg),
	   *(ulong *)(&PCI_ASIC->pmac_intrpt_mask_reg));
    printf(" interrupt status register = 0x%08x\n", status);
}

/* end of file */
