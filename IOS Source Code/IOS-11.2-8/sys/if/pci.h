/* $Id: pci.h,v 3.3.52.2 1996/06/04 00:31:02 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/if/pci.h,v $
 *------------------------------------------------------------------
 * pci.h - Generic defines/inlines for PCI configuration.
 *
 * Aug 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pci.h,v $
 * Revision 3.3.52.2  1996/06/04  00:31:02  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/04/19  15:16:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 2.1.6.3  1995/12/17  01:38:06  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.2  1995/12/06  23:53:36  rmontino
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.1  1995/11/17  16:58:32  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.3  1995/12/01  01:59:36  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  09:30:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:51:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:54:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_PCI_H__
#define __IF_PCI_H__

/*---------- PCI configuration specific definitions-----------------*/

/*
 * Types of PCI Access
 */
#define PCI_TYPE_ZERO		0x00
#define PCI_TYPE_ONE		0x01

/*
 * PCI configuration register offsets. Note that a 32 bit access
 * on a longword aligned address is possible and is used in the inlines
 * defined later. Eg:
 * A 32 bit config data read on 0x00h will yield a 32 bit value
 * containing both the vendor id  and device id. 
 */
#define PCI_VENDOR_ID_OFFSET	0x00
#define PCI_DEVICE_ID_OFFSET	0x02
#define PCI_COMMAND_REG_OFFSET  0x04
#define PCI_STATUS_REG_OFFSET   0x06
#define PCI_REVISION_ID_OFFSET  0x08
#define PCI_PROG_IF_OFFSET      0x09
#define PCI_SUB_CLASS_OFFSET    0x0A
#define PCI_LATENCY_OFFSET      0x0C
#define PCI_IO_BASE_OFFSET      0x10
#define PCI_MMIO_BASE_OFFSET	0x14
#define PCI_RESERVED_OFFSET     0x28 /* There are 5 reserved regs */
#define PCI_INTR_LINE_OFFSET    0x3C


/*
 * PCI command register MASKS.
 */
#define PCI_COMMAND_BMEN	0x0004
#define PCI_COMMAND_MEMEN       0x0002
#define PCI_COMMAND_IOEN        0x0001

/*
 * PCI status register MASKS.
 */
#define PCI_STATUS_FBTBC	0x0080
#define PCI_STATUS_DATAPERR	0x0100
#define PCI_STATUS_DEVSEL   	0x0200
#define PCI_STATUS_STABORT	0x0800
#define PCI_STATUS_RTABORT	0x1000
#define PCI_STATUS_RMBORT	0x2000
#define PCI_STATUS_SERR		0x4000
#define PCI_STATUS_PERR		0x8000

#define PCI_DEVICE_NOT_PRESENT    0x0000ffff
#define PCI_LATENCY_TIMER_DEFAULT 0xffffffff
#define DECPPB_VENDOR_DEVICE_ID   0x00011011  /* Dec bridge vendor id */
#define PCI_BRIDGE_ID 		  DECPPB_VENDOR_DEVICE_ID

/*
 * Typedef for bridge configuration data structure
 */
typedef struct pci_bridge_config_t_ {

    char *desc;			/* human consumable description */
    ulong config_ptr;		/* Pointer for direct access */
    ulong device_num;		/* device number */
    
    ulong stat_cmd;		/* status, command */
    ulong primary_latency;	/* primary latency, cache line */
    uchar primary_bus;		/* primary bus */
    uchar secondary_bus;	/* secondary bus */
    uchar subordinate_bus;	/* subordinate bus */
    ulong secondary_latency;	/* secondary latency */
    ulong io_base;		/* IO mapped IO base */
    ulong io_limit;		/* IO mapped IO limit */
    ulong memio_base;		/* Memory mapped IO base */
    ulong memio_limit;		/* Memory mapped IO limit */
    ulong mem_base;		/* Memory base */
    ulong mem_limit;		/* Memory limit */
    ulong bridge_control;	/* Bridge control */
    ushort burst_limit;		/* Burst limit */
    uchar primary_timer;	/* Primary timer */
    uchar secondary_timer;	/* Secondary timer */

} pci_bridge_config_t;

/*
 * Function prototypes
 */
extern ulong 	pci_config_read(uchar, uchar, uchar, uchar);
extern void	pci_config_write(uchar, uchar, uchar, uchar, ulong);
extern int 	pci_discover_devices(uchar, uchar, uint);

extern pci_bridge_config_t *get_bridge_config(uint);

#endif
