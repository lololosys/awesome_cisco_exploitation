/* $Id: if_c4000_pci.h,v 3.4 1995/12/01 01:59:50 mbeesley Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c4000_pci.h,v $
 *------------------------------------------------------------------
 * if_c4000_pci.h
 *
 * Aug 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_pci.h,v $
 * Revision 3.4  1995/12/01  01:59:50  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.3  1995/11/17  17:39:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:35:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_C4000_PCI_H__
#define __IF_C4000_PCI_H__

#include "../if/pci.h"

/*
 * PCI NIM specific defines.
 */
#define PCI_NIM_MAX_DEVICES            6     /* 
                                              * # of pci devices possible;
                                              * 6E NIM hardware requires
					      * this value to be 6
                                              */
/*
 * Actual separation between I/O spaces of 2 consecutive PCI devices.
 * NOTE: All PCI designs for C4XXX NIMs should conform to this.
 * This assumes no controller will need more than 255 registers.
 */
#define PCI_NIM_DEV_IO_SEPARATION     	0x0100

#define PCI_NIM_HIGH_ORDER_REG_OFFSET 	0x0240
#define PCI_NIM_CONF_ADDR_REG_OFFSET  	0x0300
#define PCI_NIM_CONF_DATA_REG_OFFSET  	0x20000
#define PCI_NIM_DEVICE_BASE_OFFSET    	0x10000

#define XILINX_DLOAD_DONE 0x3   /* down load Xilinx file done */

/* PCI NIM specific registers. */
typedef struct pci_nim_block pci_nim_block_t;
struct pci_nim_block {
    volatile ushort nim_type;
    volatile ushort nim_rev;
    volatile ushort adapter_id;
    volatile ushort xilinx_base;
    volatile ushort pad[252];
    volatile ushort reset_control;
    volatile ushort int_vector;	  /* the lower 3 bits decide subunit 0-7 */
};

/* PCI NIM instance */
typedef struct pci_nim_instance pci_nim_instance_t;
struct pci_nim_instance {
    pci_nim_block_t *nim_regs;
    ulong	slot_no;
    ushort      *high_order_reg;
    ulong       *config_addr_reg;
    ulong       *config_data_reg;
    void        *device_ds[PCI_NIM_MAX_DEVICES];
};
 
/*
 * Function prototypes
 */
uint pci_nim_configure_device(pci_nim_instance_t *, uchar, uint);

/*
 * external references and declarations.
 */
extern pci_nim_instance_t *pci_nim_ds[];

/*
 * Macro for accessing instance.
 */
#define PCI_NIM_INSTANCE(slot)  (pci_nim_ds[slot])
#endif    




