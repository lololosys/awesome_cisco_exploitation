/* $Id: svip_pci.c,v 3.1.62.5 1996/08/28 13:15:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_pci.c,v $
 *------------------------------------------------------------------
 * SVIP specific pci routines
 *
 * November 1995, Oumar Ndiaye
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: svip_pci.c,v $
 * Revision 3.1.62.5  1996/08/28  13:15:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.4  1996/06/04  00:31:20  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/23  17:14:11  gshen
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
 * Revision 3.1.62.2  1996/04/02  22:55:29  mbeesley
 * CSCdi53300:  PA power-on sequence can leave bus interface off
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:35  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.2  1996/02/19  23:24:26  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:22:09  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:44  getchell
 * Placeholder files for Viper development
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

#include "../pas/if_pas.h"
#include "../if/pci.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/dec_ppb.h"

#include "svip_pma.h"
 
/* 
 * FIXME:  These defines belong in an rvip-specific header file.
 * Figure out which one!
 */
#define	   PCI_IO_OFFSET_START_PCIBAY_0	       0x2000
#define	   PCI_IO_OFFSET_END_PCIBAY_0	       0x7FFF
#define	   PCI_IO_OFFSET_START_PCIBAY_1	       0xA000
#define	   PCI_IO_OFFSET_END_PCIBAY_1	       0xFFFF
#define    PCI_CONF_ADDR_REG_OFF               0x20004   
#define    PCI_CONF_DATA_REG_OFF               0x2000c

/*
 * Bridge 0 config data structure
 */
static pci_bridge_config_t bay0_bridge_config = {
    "PA Bay 0 Upstream PCI-PCI Bridge", /* Description */

    0,		       	                /* Undirect access */
    0,				       	/* Device number */

    0x00000147,				/* status/command */
    0x00000000,			  	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x1,				/* secondary bus */
    0x1,				/* subordinate bus */
    0x8,				/* secondary latency */
    PCI_IO_OFFSET_START_PCIBAY_0,	/* IO mapped IO base */
    PCI_IO_OFFSET_END_PCIBAY_0,		/* IO mapped IO limit */
    PCI_MEMMAPPED_IO_BAY0,		/* Memory mapped IO base */
    PCI_MEMMAPPED_IO_BAY0+ONE_MEG-1,	/* Memory mapped IO limit */
    0xff000000,				/* Memory base */
    0x0,				/* Memory limit */
    0x00000000,				/* Bridge control */
    0x0010,				/* Burst limit - DO NOT CHANGE - WORKS
					   AROUND PMA BUG */
    0x00,				/* Primary timer */
    0x00				/* Secondary timer */
};

/*
 * Bridge 1 config data structure
 */
static pci_bridge_config_t bay1_bridge_config = {
    "PA Bay 1 Upstream PCI-PCI Bridge", /* Description */

    0,              		       	/* Undirect access */
    1,				       	/* Device number */

    0x00000147,				/* status/command */
    0x00000000,			  	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x2,				/* secondary bus */
    0x2,				/* subordinate bus */
    0x8,				/* secondary latency */
    PCI_IO_OFFSET_START_PCIBAY_1,	/* IO mapped IO base */
    PCI_IO_OFFSET_END_PCIBAY_1,		/* IO mapped IO limit */
    PCI_MEMMAPPED_IO_BAY1,		/* Memory mapped IO base */
    PCI_MEMMAPPED_IO_BAY1+ONE_MEG-1,	/* Memory mapped IO limit */
    0xff000000,				/* Memory base */
    0x0,				/* Memory limit */
    0x00000000,				/* Bridge control */
    0x0010,				/* Burst limit - DO NOT CHANGE - WORKS
					   AROUND PMA BUG */
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
 * svip_iomapped_pa:
 * Return TRUE if this PA should be mapped in the PCI IO space
 */
static boolean svip_iomapped_pa (uint pa_bay)
{
    return(FALSE);		/* map all PAs in mem space */
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

    if (svip_iomapped_pa(pa_bay)) {
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

    if (svip_iomapped_pa(pa_bay)) {
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

    if (svip_iomapped_pa(pa_bay)) {
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
    ulong temp;
    leveltype level;
    
    level = raise_interrupt_level(ALL_DISABLE);
    
    if (!bus_no) {    /* this is a bridge */
	if (device_no==0) { /* bridge 0 */
	    temp = ((1 << 16) | /* function and device number are 0 */
		    ((ulong)offset)|
		    ((ulong)PCI_TYPE_ZERO));
	} else {        /* bridge 1 */
	    temp = ((2 << 16) | /* function and device number are 0 */
		    ((ulong)offset)|
		    ((ulong)PCI_TYPE_ZERO));
	}
    } else {        /* type one */
	temp = ((((ulong)bus_no) << 16) | 
		(((ulong)device_no) << 11) | \
		((ulong)offset) |
		((ulong)PCI_TYPE_ONE));
    }
    *(volatile ulong *)(PMA_PCI_CONFIG_ADDR) = temp;
    *(volatile ulong *)(PMA_PCI_CONFIG_DATA) = value;
    
    reset_interrupt_level(level);
}

/*
 * pci_config_read :
 * Perform a PCI config read cycle.
 */
ulong pci_config_read (uchar bus_no, uchar device_no, uchar func,
		       uchar offset)
{
    ulong temp;
    leveltype level;
    
    level = raise_interrupt_level(ALL_DISABLE);
    
    if (!bus_no) {        		/* this is a bridge */
	if (device_no == 0) {  		/* bridge 0 */
	    temp = ((1 << 16) |   
		    ((ulong)offset) |  	/* function and device number are 0 */
		    ((ulong)PCI_TYPE_ZERO));
	} else {
	    temp = ((2 << 16) |   
		    ((ulong)offset) |	/* function and device number are 0 */
		    ((ulong)PCI_TYPE_ZERO));
	}
    } else {
	temp = ((((ulong)bus_no) << 16) | 
		(((ulong)device_no) << 11) | \
		((ulong)offset) |
		((ulong)PCI_TYPE_ONE));
    }
    *(volatile ulong *)(PMA_PCI_CONFIG_ADDR) = temp;
    temp = *(volatile ulong *)(PMA_PCI_CONFIG_DATA);
    
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
/*    ulong vendor_id; */
    uchar bus_no;

    bus_no = get_pci_bus_no(pa_bay);
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
/*
 * 
 * FIXME this causes crash
   vendor_id = pci_config_read(ptr->secondary_bus, ptr->device_num, 0, 
       PCI_VENDOR_ID_OFFSET);
   if (vendor_id != DECPPB_VENDOR_DEVICE_ID) {
       return(FALSE);
   } else { 
       return(dec21050_bridge_config(ptr, TRUE));
   }
 *
 */
    return(dec21050_bridge_config(ptr, TRUE));
}

/*
 * pas_pci_controller_summary:
 * Print out a summary of the PCI controller ASIC
 * FIXME - needs some work
 */
void pas_pci_controller_summary (void)
{
    ulong status;
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);

    status = *(volatile ulong *)(&PMA_ASIC->err_reg);
    reset_interrupt_level(level);

    printf("PCI Controller Summary : \n");
    printf(" config register = 0x%08x, interrupt mask register = 0x%08x\n",
	   *(ulong *)(&PMA_ASIC->config_reg1),
	   *(ulong *)(&PMA_ASIC->intr_mask_reg));
    printf(" interrupt status register = 0x%08x\n", status);
}

/* end of file */


