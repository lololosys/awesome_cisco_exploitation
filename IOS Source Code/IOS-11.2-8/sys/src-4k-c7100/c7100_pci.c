/* $Id: c7100_pci.c,v 3.1.66.14 1996/08/28 13:11:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_pci.c,v $
 *------------------------------------------------------------------
 * c7100_pci.c - PCI support for Predator
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_pci.c,v $
 * Revision 3.1.66.14  1996/08/28  13:11:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.13  1996/08/20  00:10:29  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.1.66.12  1996/06/04  00:31:14  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.1.66.11  1996/05/31  23:44:24  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.1.66.10  1996/05/24  04:11:23  mbeesley
 * CSCdi58493:  Tweak PCI settings for new arbiters
 * Branch: California_branch
 *
 * Revision 3.1.66.9  1996/05/18  08:54:40  fsunaval
 * CSCdi57640:  1F - Software tweaks to augment new EPLD.
 * Branch: California_branch
 * 1.Remove 1F specific latency timer changes.
 *
 * Revision 3.1.66.8  1996/05/06  20:25:58  mbeesley
 * CSCdi56608:  DEC PCI/PCI Bridge busted - turn off prefetch space
 * Branch: California_branch
 *
 * Revision 3.1.66.7  1996/04/29  19:27:56  mbeesley
 * CSCdi55915:  One broken PA causes all interfaces to be ignored
 * Branch: California_branch
 *
 * Revision 3.1.66.6  1996/04/23  17:13:56  gshen
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
 * Revision 3.1.66.5  1996/04/06  02:05:42  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.4  1996/04/05  06:30:36  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.1.66.3  1996/04/02  22:55:21  mbeesley
 * CSCdi53300:  PA power-on sequence can leave bus interface off
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/04/02  02:48:31  mbeesley
 * CSCdi53087:  Need dynamic primary latency PCI configuration
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  23:48:08  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.13  1996/03/08  21:36:26  mbeesley
 * CSCdi51090:  Need to support rev 2 CPU card
 * Branch: ELC_branch
 *
 * Revision 3.1.2.12  1996/02/08  09:26:53  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.11  1996/02/02  22:57:09  fsunaval
 * Branch: ELC_branch
 * 1.Port Adapter Management interrupts functional.
 *
 * Revision 3.1.2.10  1996/01/26  22:10:37  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.1.2.9  1996/01/16  02:16:51  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.8  1996/01/09  21:27:48  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.1.2.7  1996/01/03  06:20:51  wfried
 * Branch: ELC_branch
 * Introduce delay to allow PA power to stabilize.
 *
 * Revision 3.1.2.6  1995/12/19  19:50:07  wfried
 * Branch: ELC_branch
 * Fix PCI bridges configuration, MP interrupt registers masks and Galileo
 * PCI timing initialization value to Async.
 *
 * Revision 3.1.2.5  1995/12/13  02:23:49  mbeesley
 * Branch: ELC_branch
 * Fix misdocumented register bit defintions. Add error interrupt
 * masks. Clear OIR interrupts at boot time. Ensure that PCI Signals
 * Invalid is not set at port adaptor power on.
 *
 * Revision 3.1.2.4  1995/12/12  04:50:47  mbeesley
 * Branch: ELC_branch
 * Add an enumeration for the IO card 1fe device number. Also fix some
 * DRAM PCI configuration in the GT64010.
 *
 * Revision 3.1.2.3  1995/12/05  20:00:06  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization. Add support for dynamic
 * parallel bridge config. Do not use a small piece of SRAM as
 * fast memory on Rev 2 cards.
 *
 * Revision 3.1.2.2  1995/11/30  02:14:29  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1995/11/22  21:43:21  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:46  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "sched.h"
#include "exception.h"
#include "sys_registry.h"
#include "config.h"
#include "stacks.h"

#include "../os/gdb.h"
#include "../os/free.h"
#include "../os/region.h"

#include "../if/pci.h"
#include "../src-4k/gt64010.h"
#include "../filesys/clpd6720.h"
#include "../filesys/clpd6729.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/dec_ppb.h"
#include "../pas/pas_id.h"

#include "c7100_fpga.h"
#include "msg_c7100.c"			/* Not a typo, see logger.h */

/*
 * Define primary latency times. These differ per PA.
 */
#define C7100_MB_UP_PRIMARY_LATENCY	0x0000ff00
#define C7100_MB_DOWN_PRIMARY_LATENCY	0x00000010

#define C7100_MB_UP_BURST		0x0008
#define C7100_MB_DOWN_BURST		0x0008

#define C7100_MB_UP_BRIDGE_CONTROL	0x00030000
#define C7100_MB_DOWN_BRIDGE_CONTROL	0x00030000

#define C7100_MB_UP_PRIMARY_TIMER	0x20
#define C7100_MB_UP_SECONDARY_TIMER	0x80

#define C7100_MB_DOWN_PRIMARY_TIMER	0x20
#define C7100_MB_DOWN_SECONDARY_TIMER	0x80

#define C7100_GENERAL_PRIMARY_LATENCY	0x0000ff00

/*
 * Define PA bridge settings
 */
#define C7100_PA_BURST			0x0008
#define C7100_PA_PRIMARY_TIMER		0x20
#define C7100_PA_SECONDARY_TIMER	0x90
#define C7100_PA_BRIDGE_CONTROL		0x00030000

/*
 * Define the bay management data structure
 */
typedef struct c7100_bay_cntl_t_ {

    /*
     * Addresses
     */
    ulong reg_io_base_address;		/* CPU's address for IO registers */
    ulong reg_io_base_offset;		/* PCI's address for IO registers */

    /*
     * Control registers and masks
     */
    volatile ulong *present_reg;	/* register for PA presence */
    ulong present_mask;			/* mask for PA presence */
    ulong pci_invalid_mask;		/* PCI Signal invalid */

    volatile ulong *power_reg;		/* register for PA power control */
    ulong power_mask;			/* mask for PA power control */

    volatile ulong *led_reg;		/* register for PA LED control */
    ulong led_mask;			/* mask for PA LED control */

    volatile ulong *pkt_intr_reg;	/* packet intr register */
    ulong pkt_intr_mask;		/* mask for PA pkt interrupt */

    volatile ulong *mgmt_intr_reg;	/* mgmt intr register */
    ulong mgmt_intr_mask;		/* mask for PA mgmt interrupt */

} c7100_bay_cntl_t;

/*
 * Enumerate all the control information for each bay
 */
c7100_bay_cntl_t c7100_bay_cntl[MAX_PA_BAYS] = {

/* IO slot, bay 0 */
{
    ADRSPC_BAY0,			/* CPU address, virtual */
    PHY_ADRSPC_BAY0,			/* PCI address, physical */

    NULL, 				0,
    0, 					0,
    PCI_INVALID_SIG_BAY0,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	IOLED_ENABLE,
    &C7100_MP_FPGA->pkt_int_enbl_lsw,	PKT_INTR_ENABLE_BAY0,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY0
},

/* Bay 1 */
{
    ADRSPC_BAY1,			/* CPU address, virtual */
    PHY_ADRSPC_BAY1,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY1|PCI_5VOK_BAY1),
    PCI_INVALID_SIG_BAY1,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY1,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY1,
    &C7100_MP_FPGA->pkt_int_enbl_lsw,	PKT_INTR_ENABLE_BAY1,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY1
},

/* Bay 2 */
{
    ADRSPC_BAY2,			/* CPU address, virtual */
    PHY_ADRSPC_BAY2,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY2|PCI_5VOK_BAY2),
    PCI_INVALID_SIG_BAY2,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY2,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY2,
    &C7100_MP_FPGA->pkt_int_enbl_nsw,	PKT_INTR_ENABLE_BAY2,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY2
},

/* Bay 3 */
{
    ADRSPC_BAY3,			/* CPU address, virtual */
    PHY_ADRSPC_BAY3,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY3|PCI_5VOK_BAY3),
    PCI_INVALID_SIG_BAY3,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY3,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY3,
    &C7100_MP_FPGA->pkt_int_enbl_lsw,	PKT_INTR_ENABLE_BAY3,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY3
},

/* Bay 4 */
{
    ADRSPC_BAY4,			/* CPU address, virtual */
    PHY_ADRSPC_BAY4,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY4|PCI_5VOK_BAY4),
    PCI_INVALID_SIG_BAY4,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY4,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY4,
    &C7100_MP_FPGA->pkt_int_enbl_lsw,	PKT_INTR_ENABLE_BAY4,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY4
},

/* Bay 5 */
{
    ADRSPC_BAY5,			/* CPU address, virtual */
    PHY_ADRSPC_BAY5,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY5|PCI_5VOK_BAY5),
    PCI_INVALID_SIG_BAY5,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY5,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY5,
    &C7100_MP_FPGA->pkt_int_enbl_nsw,	PKT_INTR_ENABLE_BAY5,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY5
},

/* Bay 6 */
{
    ADRSPC_BAY6,			/* CPU address, virtual */
    PHY_ADRSPC_BAY6,			/* PCI address, physical */

    &C7100_MP_FPGA->pa_status_lsw, 	(PCI_3VOK_BAY6|PCI_5VOK_BAY6),
    PCI_INVALID_SIG_BAY6,
    &C7100_MP_FPGA->pa_ctrl_lsw,	POWER_ENABLE_BAY6,
    &C7100_MP_FPGA->pa_ctrl_lsw, 	LED_ENABLE_BAY6,
    &C7100_MP_FPGA->pkt_int_enbl_nsw,	PKT_INTR_ENABLE_BAY6,
    &C7100_MP_FPGA->mgmt_int_enbl_lsw,	MGMT_INTR_ENABLE_BAY6
} };

/*
 * Control data structures for IO slot and PA EEPROM
 */

/*
 * IO Slot, bay 0
 */
static pas_management_t c7100_bay0_eeprom = {
    0,						/* Bay 0 */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY0_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY0_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),   	/* 32 bit register */
    BAY0_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY0_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_lsw		/* DOUT register */
};

/*
 * Bay 1
 */
static pas_management_t c7100_bay1_eeprom = {
    1,						/* Bay 1 */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY1_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY1_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),   	/* 32 bit register */
    BAY1_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY1_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_lsw		/* DOUT register */
};

/*
 * Bay 2
 */
static pas_management_t c7100_bay2_eeprom = {
    2,						/* Bay 2 */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY2_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY2_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),   	/* 32 bit register */
    BAY2_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY2_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_nsw		/* DOUT register */
};

/*
 * Bay 3
 */
static pas_management_t c7100_bay3_eeprom = {
    3,						/* Bay 3 */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY3_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY3_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),   	/* 32 bit register */
    BAY3_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY3_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_lsw		/* DOUT register */
};

/*
 * Bay 4
 */
static pas_management_t c7100_bay4_eeprom = {
    4,						/* Bay 4 */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY4_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY4_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),   	/* 32 bit register */
    BAY4_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_lsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_lsw),	/* 32 bit register */
    BAY4_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_lsw		/* DOUT register */
};

/*
 * Bay 5
 */
static pas_management_t c7100_bay5_eeprom = {
    5,						/* Bay 5 */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY5_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY5_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),   	/* 32 bit register */
    BAY5_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY5_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_nsw		/* DOUT register */
};

/*
 * Bay 6
 */
static pas_management_t c7100_bay6_eeprom = {
    6,						/* Bay 6 */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY6_EEPROM_CLOCK_MASK,			/* Clock mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Clock control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY6_EEPROM_SELECT_MASK,			/* Select mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* Select control register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),   	/* 32 bit register */
    BAY6_EEPROM_DIN_MASK,			/* DIN mask */
    &C7100_MP_FPGA->pa_tst_port_nsw,		/* DIN register */

    sizeof(C7100_MP_FPGA->pa_tst_port_nsw),	/* 32 bit register */
    BAY6_EEPROM_DOUT_MASK,			/* DOUT mask */
    &C7100_MP_FPGA->pa_tst_port_nsw		/* DOUT register */
};

pas_management_t *pas_management_list[MAX_PA_BAYS] = {
    &c7100_bay0_eeprom,
    &c7100_bay1_eeprom,
    &c7100_bay2_eeprom,
    &c7100_bay3_eeprom,
    &c7100_bay4_eeprom,
    &c7100_bay5_eeprom,
    &c7100_bay6_eeprom
};

/*
 * Bridge 0 config data structure. Downstream MB0 to MB1
 */
static pci_bridge_config_t mb0_bridge_config = {

    "Bridge 0, Downstream MB0 to MB1", 	/* Description */

    0,			       		/* Direct access. None */
    1,				       	/* Device number */
    
    0x00000143,				/* status/command, downstream */
    C7100_MB_DOWN_PRIMARY_LATENCY,	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x1,				/* secondary bus */
    0x0,				/* subordinate bus */
    0x18,				/* secondary latency */
    0x5000,				/* IO mapped IO base */
    0x9fff,				/* IO mapped IO limit */
    0x48800000,				/* Memory mapped IO base */
    0x4affffff,				/* Memory mapped IO limit */
    0x4b000000,				/* Memory base */
    0x4bffffff,				/* Memory limit */
    C7100_MB_DOWN_BRIDGE_CONTROL,	/* Bridge control */
    C7100_MB_DOWN_BURST,	       	/* Burst limit */
    C7100_MB_DOWN_PRIMARY_TIMER,	/* Primary timer */
    C7100_MB_DOWN_SECONDARY_TIMER	/* Secondary timer */
};

/*
 * Bridge 1 config data structure. Upstream MB1 to MB0
 */
static pci_bridge_config_t mb1_bridge_config = {

    "Bridge 1, Upstream MB1 to MB0", 	/* Description */

    0,			       		/* Direct access. None */
    2,				       	/* Device number */
    
    0x00000144,				/* status/command, MB0 master */
    C7100_MB_UP_PRIMARY_LATENCY,   	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x4,				/* secondary bus */
    0x9,				/* subordinate bus */
    0x18,				/* secondary latency */
    0x5000,				/* IO mapped IO base */
    0x9fff,				/* IO mapped IO limit */
    0x48800000,				/* Memory mapped IO base */
    0x4affffff,				/* Memory mapped IO limit */
    0x4b000000,				/* Memory base */
    0x4cffffff,				/* Memory limit */
    C7100_MB_UP_BRIDGE_CONTROL,		/* Bridge control */
    C7100_MB_UP_BURST,	       		/* Burst limit */
    C7100_MB_UP_PRIMARY_TIMER,		/* Primary timer */
    C7100_MB_UP_SECONDARY_TIMER		/* Secondary timer */
};

/*
 * Bridge 2 config data structure. Downstream MB0 to MB2
 */
static pci_bridge_config_t mb2_bridge_config = {

    "Bridge 2, Downstream MB0 to MB2", 	/* Description */

    0,			       		/* Direct access. None */
    3,				       	/* Device number */
    
    0x00000143,				/* status/command, downstream */
    C7100_MB_DOWN_PRIMARY_LATENCY,   	/* primary latency, cache line */
    0x0,				/* primary bus */
    0x2,				/* secondary bus */
    0x0,				/* subordinate bus */
    0x18,				/* secondary latency */
    0xb000,				/* IO mapped IO base */
    0xffff,				/* IO mapped IO limit */
    0x4d000000,				/* Memory mapped IO base */
    0x4f7fffff,				/* Memory mapped IO limit */
    0x4c000000,				/* Memory base */
    0x4cffffff,				/* Memory limit */
    C7100_MB_DOWN_BRIDGE_CONTROL,	/* Bridge control */
    C7100_MB_DOWN_BURST,	       	/* Burst limit */
    C7100_MB_DOWN_PRIMARY_TIMER,	/* Primary timer */
    C7100_MB_DOWN_SECONDARY_TIMER	/* Secondary timer */
};

/*
 * Bridge 3 config data structure. Upstream MB2 to MB0
 */
static pci_bridge_config_t mb3_bridge_config = {

    "Bridge 3, Upstream MB2 to MB0", 	/* Description */

    0,			       		/* Direct access. None */
    4,				       	/* Device number */
    
    0x00000144,				/* status/command, MB0 master */
    C7100_MB_UP_PRIMARY_LATENCY,     	/* primary latency, cache line */
    0x0,				/* primary bus */
    0xa,				/* secondary bus */
    0xf,				/* subordinate bus */
    0x18,				/* secondary latency */
    0xb000,				/* IO mapped IO base */
    0xffff,				/* IO mapped IO limit */
    0x4d000000,				/* Memory mapped IO base */
    0x4f7fffff,				/* Memory mapped IO limit */
    0x4b000000,				/* Memory base */
    0x4cffffff,				/* Memory limit */
    C7100_MB_UP_BRIDGE_CONTROL,		/* Bridge control */
    C7100_MB_UP_BURST,	       		/* Burst limit */
    C7100_MB_UP_PRIMARY_TIMER,		/* Primary timer */
    C7100_MB_UP_SECONDARY_TIMER		/* Secondary timer */
};

/*
 * Array of pointers to the system bridge config
 */
static pci_bridge_config_t *(system_bridge_config[]) = {
    &mb0_bridge_config, &mb1_bridge_config, 
    &mb2_bridge_config, &mb3_bridge_config
};

/*
 * Bridge 4 config data structure. Bay 1 Bridge
 */
static pci_bridge_config_t bay1_bridge_config = {

    "Bridge 4, Port Adaptor 1", 	/* Description */

    0,			       		/* Direct access. None */
    1,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0x4,				/* primary bus */
    0x5,				/* secondary bus */
    0x5,				/* subordinate bus */
    0x18,				/* secondary latency */
    0x5000,				/* IO mapped IO base */
    0x5fff,				/* IO mapped IO limit */
    0x48800000,				/* Memory mapped IO base */
    0x48ffffff,				/* Memory mapped IO limit */
    0x48800000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Bridge 5 config data structure. Bay 2 Bridge
 */
static pci_bridge_config_t bay2_bridge_config = {

    "Bridge 5, Port Adaptor 2", 	/* Description */

    0,			       		/* Direct access. None */
    1,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0xa,				/* primary bus */
    0xb,				/* secondary bus */
    0xb,				/* subordinate bus */
    0x18,				/* secondary latency */
    0xb000,				/* IO mapped IO base */
    0xbfff,				/* IO mapped IO limit */
    0x4d000000,				/* Memory mapped IO base */
    0x4d7fffff,				/* Memory mapped IO limit */
    0x4d000000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Bridge 6 config data structure. Bay 3 Bridge
 */
static pci_bridge_config_t bay3_bridge_config = {

    "Bridge 6, Port Adaptor 3", 	/* Description */

    0,			       		/* Direct access. None */
    2,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0x4,				/* primary bus */
    0x6,				/* secondary bus */
    0x6,				/* subordinate bus */
    0x18,				/* secondary latency */
    0x6000,				/* IO mapped IO base */
    0x6fff,				/* IO mapped IO limit */
    0x49000000,				/* Memory mapped IO base */
    0x497fffff,				/* Memory mapped IO limit */
    0x49000000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Bridge 7 config data structure. Bay 4 Bridge
 */
static pci_bridge_config_t bay4_bridge_config = {

    "Bridge 7, Port Adaptor 4", 	/* Description */

    0,			       		/* Direct access. None */
    2,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0xa,				/* primary bus */
    0xc,				/* secondary bus */
    0xc,				/* subordinate bus */
    0x18,				/* secondary latency */
    0xc000,				/* IO mapped IO base */
    0xcfff,				/* IO mapped IO limit */
    0x4d800000,				/* Memory mapped IO base */
    0x4dffffff,				/* Memory mapped IO limit */
    0x4d800000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Bridge 8 config data structure. Bay 5 Bridge
 */
static pci_bridge_config_t bay5_bridge_config = {

    "Bridge 8, Port Adaptor 5", 	/* Description */

    0,			       		/* Direct access. None */
    3,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0x4,				/* primary bus */
    0x7,				/* secondary bus */
    0x7,				/* subordinate bus */
    0x18,				/* secondary latency */
    0x7000,				/* IO mapped IO base */
    0x7fff,				/* IO mapped IO limit */
    0x49800000,				/* Memory mapped IO base */
    0x49ffffff,				/* Memory mapped IO limit */
    0x49800000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Bridge 9 config data structure. Bay 6 Bridge
 */
static pci_bridge_config_t bay6_bridge_config = {

    "Bridge 9, Port Adaptor 6", 	/* Description */

    0,			       		/* Direct access. None */
    3,				       	/* Device number */
    
    0x00000147,				/* status/command */
    C7100_GENERAL_PRIMARY_LATENCY,	/* primary latency, cache line */
    0xa,				/* primary bus */
    0xd,				/* secondary bus */
    0xd,				/* subordinate bus */
    0x18,				/* secondary latency */
    0xd000,				/* IO mapped IO base */
    0xdfff,				/* IO mapped IO limit */
    0x4e000000,				/* Memory mapped IO base */
    0x4e7fffff,				/* Memory mapped IO limit */
    0x4e000000,				/* Memory base */
    0x00000000,				/* Memory limit */
    C7100_PA_BRIDGE_CONTROL,		/* Bridge control */
    C7100_PA_BURST,			/* Burst limit */
    C7100_PA_PRIMARY_TIMER,		/* Primary timer */
    C7100_PA_SECONDARY_TIMER	     	/* Secondary timer */
};

/*
 * Array of pointers to the port adaptor bridge configuration
 */
static pci_bridge_config_t *(pabay_bridge_config[]) = {
    &bay1_bridge_config, &bay2_bridge_config, &bay3_bridge_config,
    &bay4_bridge_config, &bay5_bridge_config, &bay6_bridge_config
};

/*
 * pas_pa_present :
 * See if a Port Adaptor is present in the selected bay
 */
boolean pas_pa_present (uint pa_bay)
{
    c7100_bay_cntl_t *ptr;
    ulong temp;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(FALSE);
    }
    
    /*
     * If it is the IO slot, it is always present
     */
    if (pa_bay == 0) {
	return(TRUE);
    }
    ptr = &c7100_bay_cntl[pa_bay];
    temp = *ptr->present_reg;
    if (temp & ptr->present_mask) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * pas_control_power:
 * Control power on a PA.
 */
void pas_control_power (uint pa_bay, boolean onoff)
{
    c7100_bay_cntl_t *ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return;
    }
    ptr = &c7100_bay_cntl[pa_bay];

    /*
     * Control the power and OIR interrupt
     */
    if (pa_bay != 0) {
	if (onoff) {
	    /*
	     * Acknowledge any OIR event. This should cause
	     * the PCI Signals to go valid
	     */
	    (*ptr->power_reg) |= ptr->power_mask;
	    flush_io_wbuffer();
	    pas_delay(1000); /* Time for PA Power to stabilize (1s for now) */
	    C7100_MP_FPGA->err_int_stat_lsw = (1 << pa_bay);
	    flush_io_wbuffer();
	    if (*ptr->present_reg & ptr->pci_invalid_mask) {
		errmsg(&msgsym(PCIVALID, C7200), pa_bay, *ptr->present_reg);
	    }
	} else {
	    (*ptr->power_reg) &= ~ptr->power_mask;
	}
    }

    /*
     * Control the LED
     */
    if (onoff) {
	(*ptr->led_reg) |= ptr->led_mask;
    } else {
	(*ptr->led_reg) &= ~ptr->led_mask;
    }

    /*
     * Map the slot in/out of the virtual address space
     */
    c7100_bay2vaddr(pa_bay, onoff, onoff);
}

/*
 * pas_netio_interrupts:
 * Control the network IO interrupt enables for a pa bay. Raises the
 * interrupt level to lock out OIR events.
 */
void pas_netio_interrupts (uint pa_bay, boolean onoff)
{
    c7100_bay_cntl_t *ptr;
    leveltype level;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return;
    }
    level = raise_interrupt_level(LEVEL_ERROR);
    ptr = &c7100_bay_cntl[pa_bay];
    if (onoff) {
	(*ptr->pkt_intr_reg) |= ptr->pkt_intr_mask;
    } else {
	(*ptr->pkt_intr_reg) &= ~ptr->pkt_intr_mask;
    }
    reset_interrupt_level(level);
}

/*
 * pas_mngmt_interrupts:
 * Control the PA management interrupt enables for a bay. Must be called with
 * interrupts masked.
 */
void pas_mngmt_interrupts (uint pa_bay, boolean onoff)
{
    c7100_bay_cntl_t *ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return;
    }
    ptr = &c7100_bay_cntl[pa_bay];
    if (onoff) {
	(*ptr->mgmt_intr_reg) |= ptr->mgmt_intr_mask;
    } else {
	(*ptr->mgmt_intr_reg) &= ~ptr->mgmt_intr_mask;
    }
}

/*
 * pas_control_reset:
 * Control reset signal to PA's
 */
void pas_control_reset (uint pa_bay, boolean onoff)
{
    /* 
     * There does not seem to be a way to do this on Predator.
     */
}

/*
 * get_bridge_config:
 * Return pointer to a bridge config structure. The parameter being passed in
 * is the bay number.
 */
pci_bridge_config_t *get_bridge_config (uint pa_bay)
{
    if ((pa_bay >= MAX_PA_BAYS) || (pa_bay == 0)) {
	return(NULL);
    } else {
	return(pabay_bridge_config[pa_bay-1]);
    }
}

/*
 * pas_bridge_config:
 * Configure the PA upstream bridge
 */
boolean pas_bridge_config (uint pa_bay)
{
    pci_bridge_config_t *ptr;
    ulong vendor_id;

    /*
     * If this is the IO slot, there is nothing to do
     */
    if (pa_bay == 0) {
	return(TRUE);
    }
    ptr = get_bridge_config(pa_bay);
    if (!ptr) {
	return(FALSE);
    }

    /*
     * Verify the box configuration
     */
    if (!c7100_verify_sysconfig(pa_bay)) {
	return(FALSE);
    }

    /*
     * Read the Vendor/ID and make sure it is a DEC21050
     */
    vendor_id = pci_config_read(ptr->primary_bus, ptr->device_num,
				0, PCI_VENDOR_ID_OFFSET);
    if (vendor_id != DECPPB_VENDOR_DEVICE_ID) {
	errmsg(&msgsym(UNDEFPABRIDGE, PA), vendor_id, pa_bay);
	return(FALSE);
    } 
    ptr->primary_latency = C7100_GENERAL_PRIMARY_LATENCY;
    return(dec21050_bridge_config(ptr, FALSE));
}

/*
 * get_pci_bus_no:
 * Return PCI bus number for a bay.
 */
uchar get_pci_bus_no (uint pa_bay)
{
    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(0xff);
    } else if (pa_bay == 0) {
	return(0);		/* IO slot is on bus 0 */
    }

    /*
     * Return the secondary bus from the bridge config data structure
     */
    return(pabay_bridge_config[pa_bay-1]->secondary_bus);
}

/*
 * get_io_base_address:
 * Return the IO base address for a port adaptor
 */
void *get_io_base_address (uint pa_bay)
{
    ulong temp;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(NULL);
    }
    temp = c7100_bay_cntl[pa_bay].reg_io_base_address;
    if (pa_bay == 0) {
	/*
	 * If this is the IO card, we need to provide an address
	 * that is 6 PCI_DEV_MEM_MAPPED_IO_SEPARATION's from the start
	 * of the port adaptor, as the 1fe is really device
	 * 6 on the PCI bus, but we are forcing it to be
	 * device 0 from the systems perspective IE int fastethernet 0/0
	 */
	return((void *)(temp + (PCI_IO1FE_DEV_NUM * PCI_DEV_MEM_MAPPED_IO_SEPARATION)));
    }
    return((void *)(temp));
}

/*
 * get_pci_device_base:
 * Get the base IO address for an interfaces registers from the
 * CPU perspective (IE for ds->reg_ptr).
 */
void *get_pci_device_base (uint pa_bay, uint pa_device)
{
    ulong temp;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(NULL);
    }
    temp = c7100_bay_cntl[pa_bay].reg_io_base_address;
    return((void *)(temp + (pa_device * PCI_DEV_MEM_MAPPED_IO_SEPARATION)));
}

/*
 * get_pci_device_base_offset:
 * Get the offset from the start of PCI IO space for a device
 * on a port adaptor (IE for programming into the IO base address
 * register of a chip).
 */
ulong get_pci_device_base_offset (uint pa_bay, uint pa_device)
{
    ulong temp;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return(0);
    }
    temp = c7100_bay_cntl[pa_bay].reg_io_base_offset;
    return(temp + (pa_device * PCI_DEV_MEM_MAPPED_IO_SEPARATION));
}

/*
 * pas_nmi_dispatcher :
 * Call all the interrupt handlers associated with all Ports requesting
 * a management interrupt (in a sequence based on their priorities).
 */
void pas_nmi_dispatcher (void)
{
    pa_nmi_handler_t *ptr;
    ulong int_stat, status=0;

    /*
     * First thing to do is see if the PCMCIA card interrupted us
     */
    int_stat = C7100_MP_FPGA->mgmt_int_stat_lsw;
    if (int_stat & PCMCIA_INTR_STATUS) {
	clpd6720_mgmt_intr_handler();
    }

    /*
     * Now loop thru all the other management interrupt sources
     * and call their dispatch routines if they are active.
     * First, create a bit field value, each bit corresponding
     * to each of the seven PA bays (zero out bay 0).
     */
    int_stat &= ~(MGMT_INTR_STATUS_BAY0|PCMCIA_INTR_STATUS);

    if (int_stat & MGMT_INTR_ENABLE_BAY1)
        status += 2;
    if (int_stat & MGMT_INTR_ENABLE_BAY2)
        status += 4;
    if (int_stat & MGMT_INTR_ENABLE_BAY3)
        status += 8;
    if (int_stat & MGMT_INTR_ENABLE_BAY4)
        status += 16;
    if (int_stat & MGMT_INTR_ENABLE_BAY5)
        status += 32;
    if (int_stat & MGMT_INTR_ENABLE_BAY6)
        status += 64;

    /*
     * Now dispatch each handler
     */
    while (status) {
	ptr = pa_nmi_dispatch_table[status];
	(*ptr->func)(ptr->arg);
	status &= ptr->mask;
    }
}

/*
 * pas_pci_controller_summary:
 * Print out a summary of our PCI infrastructure
 */
void pas_pci_controller_summary (void)
{
    int i;

    /*
     * Dump the PCI registers from the gt64010
     */
    gt64010_show_pci_registers();

    /*
     * Turn on automore and print out the bridge chips
     */
    automore_enable(NULL);
    for (i=0; i<4; i++) {
	dec21050_print_regs(system_bridge_config[i], i);
    }
 
    /*
     * Print PCMCIA Controller registers
     */
    clpd6729_print_regs();
    automore_disable();
}

/*
 * c7100_pci_init:
 * Initialize Predators PCI subsystems
 */
void c7100_pci_init (void)
{
    pci_bridge_config_t *b;
    ulong vid;
    int i;

    /*
     * First thing to do is init the GT64010
     */
    gt64010_init();

    /*
     * The next thing to do is init the four system bridges
     */
    for (i=0; i<4; i++) {
	b = system_bridge_config[i];
	vid = pci_config_read(0, b->device_num, 0, PCI_VENDOR_ID_OFFSET);
	if (vid != PCI_BRIDGE_ID) {
	    ttyprintf(CONTTY,
		      "\nError, bridge %d not DEC21050, returned %#x\n",
		      i, vid);
	    crashdump(0);
	}
	if (!dec21050_bridge_config(b, FALSE)) {
	    ttyprintf(CONTTY, "\nError, failed to configure bridge %d\n", i);
	    crashdump(0);
	}
    }

    /*
     * And finally setup the PCMCIA PCI controller. Read the Vendor/ID,
     * and setup the device. The PCMCIA controller has two operational
     * registers that need to reside in the PCI IO space. These registers
     * reside PCMCIA_REGS_OFFSET from the start of PCI IO mapped IO space.
     */
    vid = pci_config_read(0, PCI_PCMCIA_DEV_NUM, 0, PCI_VENDOR_ID_OFFSET);
    if (vid != CLPD6729_VENDOR_ID) {
	ttyprintf(CONTTY, "\nError, PCMCIA vendor/id incorrect, %#x\n",
		vid);
	return;
    }
    pci_config_write(0, PCI_PCMCIA_DEV_NUM, 0, PCI_COMMAND_REG_OFFSET,
		     CLPD6279_COMMAND_SETTINGS);
    pci_config_write(0, PCI_PCMCIA_DEV_NUM, 0, PCI_IO_BASE_OFFSET,
		     PCMCIA_REGS_OFFSET);
}

/*
 * c7100_pci_enable_packet_sram:
 * Configure the packet SRAM to respond to PCI slave cycles
 */
boolean c7100_pci_enable_packet_sram (void)
{
    boolean ret_val = TRUE;
    ulong vid;

    /*
     * First PCI bus (bus 4)
     */
    vid = pci_config_read(PCI_SRAM_BUS0, SRAM_DEV_NUM, 0, 
			  PCI_VENDOR_ID_OFFSET);
    if (vid == PCI_SRAM_VENDOR_ID) {
	pci_config_write(PCI_SRAM_BUS0, SRAM_DEV_NUM, 0, 
			 PCI_COMMAND_REG_OFFSET, PCI_SRAM_COMMAND);
    } else {
	ttyprintf(CONTTY, "\nError, SRAM(0) ven/id incorrect, %#x\n", vid);
	ret_val = FALSE;
    }
    
    /*
     * Second PCI bus (bus 10)
     */
    vid = pci_config_read(PCI_SRAM_BUS1, SRAM_DEV_NUM, 0, 
			  PCI_VENDOR_ID_OFFSET);
    if (vid == PCI_SRAM_VENDOR_ID) {
	pci_config_write(PCI_SRAM_BUS1, SRAM_DEV_NUM, 0, 
			 PCI_COMMAND_REG_OFFSET, PCI_SRAM_COMMAND);
    } else {
	ttyprintf(CONTTY, "\nError, SRAM(1) ven/id incorrect, %#x\n", vid);
	ret_val = FALSE;
    }
    return(ret_val);
}
 
/*
 * gt64010_setup_parallel_bridge:
 * When the system wants to do a configuration cycle to a target
 * PCI bus, the "parallel" bridge (Bridge 0 or Bridge 2) must be
 * dynamically reconfigured to have a primary bus equal to the
 * bus number of the target cycle to stop a configuration race condition.
 *
 * IE  Target cycle to bus 4, 5, 6, 7      : MB0 must be re-configured
 *     Target cycle to bus 10, 11, 12, 13  : MB2 must be re-configured
 */
void gt64010_setup_parallel_bridge (uchar bus)
{
    pci_bridge_config_t *b = NULL;
    ulong temp;

    if (bus >= 4 && bus <= 7) {
	/*
	 * Need to dynamically re-configure MB0
	 */
	b = &mb0_bridge_config;
    } else if (bus >= 10 && bus <= 13) {
	/*
	 * Need to dynamically re-configure MB2
	 */
	b = &mb2_bridge_config;
    }
    if (b) {
	temp = ((bus) | 
		(b->secondary_bus << 8) |
		(b->subordinate_bus << 16) |
		(b->secondary_latency << 24));
	pci_config_write(b->primary_bus, b->device_num, 0, REG6_OFFSET, temp);
    }
}

/*
 * gt64010_restore_parallel_bridge:
 * When the system has done a configuration cycle to a target
 * PCI bus, the "parallel" bridge (Bridge 0 or Bridge 2) will have
 * been dynamically reconfigured to have a primary bus equal to the
 * bus number of the target cycle to stop a configuration race condition.
 * We must restore this re-configuration.
 */
void gt64010_restore_parallel_bridge (uchar bus)
{
    pci_bridge_config_t *b = NULL;
    ulong temp;

    if (bus >= 4 && bus <= 7) {
	/*
	 * Need to restore MB0 back to its original state
	 */
	b = &mb0_bridge_config;
    } else if (bus >= 10 && bus <= 13) {
	/*
	 * Need to restore MB2 back to its original state
	 */
	b = &mb2_bridge_config;
    }
    if (b) {
	temp = ((b->primary_bus) | 
		(b->secondary_bus << 8) |
		(b->subordinate_bus << 16) |
		(b->secondary_latency << 24));
	pci_config_write(b->primary_bus, b->device_num, 0, REG6_OFFSET, temp);
    }
}

/* end of file */
