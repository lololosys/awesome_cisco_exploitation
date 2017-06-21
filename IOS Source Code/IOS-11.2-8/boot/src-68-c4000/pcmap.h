/* $Id: pcmap.h,v 3.2 1995/12/08 19:29:56 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/src-68-c4000/pcmap.h,v $
 *------------------------------------------------------------------
 * pcmap.h -- XX memory mapping and register defines
 *
 * December 1991, Rob Clevenger
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pcmap.h,v $
 * Revision 3.2  1995/12/08  19:29:56  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.1  1995/11/09  09:09:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/01  23:11:05  pilee
 * CSCdi39557:  Additional NIM ID is required.
 *
 * Revision 2.1  1995/06/07  19:16:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.12  1995/06/07 09:04:57  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.11.32.2  1995/05/30  19:11:19  yoko
 * Branch: Arizona_branch
 * Add 4500 6E NIM support
 *
 * Revision 1.11.32.1  1995/05/15  18:38:58  ppearce
 * Merge Newappn_branch into Arizona
 *
 * Revision 1.1.4.5.10.1.8.3  1995/03/04  06:42:27  ppearce
 * CSCdi30463:  Merge BSC/BSTUN pass-through code into Newappn_branch
 * Branch: Newappn_branch
 *
 * Revision 1.1.4.5.10.1.8.2  1995/01/12  05:07:18  dmccowan
 * Branch: Newappn_branch
 * Sync Newappn_branch to alaska (Newappn_baseline_950105)
 *
 * Revision 1.1.4.5.10.1.8.1  1994/10/22  00:42:35  dcalia
 * Sync Newappn_branch to CLS_branch (Newappn_baseline_941007)
 * Branch: Newappn_branch
 *
 * Revision 1.1.4.5.10.1.2.1  1994/09/30  00:16:33  kchiu
 * Sync CLS_branch to NAC1(V102_DUMMY_001)
 * Revision 1.11  1994/11/17  19:38:35  wfried
 * CSCdi26890:  Reserve ID for future NIMs
 * Added IDs for ATM NIMs.
 *
 * Revision 1.10  1994/10/30  21:59:37  abaindur
 * CSCdi25240:  Need to support dfb on low end systems
 * Flash Load Helper and Dual Flash Bank port.
 *
 * Revision 1.9.2.1  1994/10/07  23:31:27  enf
 * Branch: Dfb_branch
 * flh support for ROMMON
 *
 * Revision 1.9  1994/09/14  17:14:41  mleelani
 * CSCdi24398:  New Nim id for 6E NIM
 * Add the 6E NIM id the the XX map.
 *
 * Revision 1.8  1994/07/09  12:14:17  mdb
 * Sync MAIN to NAC1_postsync_940706
 *
 * Revision 1.1.4.5.10.1  1994/06/22  04:13:08  slin
 * Branch: NAC1_branch
 * Sync to Sierra_postsync_940615.
 *
 * Revision 1.1.4.6  1994/06/18  01:11:57  mbeesley
 * Branch: Sierra_branch
 * Sync to mainline tag Sierra_baseline_940615.
 *
 * Revision 1.1.4.5  1994/03/30  03:11:08  mohsen
 * Branch: Sierra_branch
 * Add 4T support.
 *
 * Revision 1.1.4.4  1994/03/25  19:59:32  mbeesley
 * Branch: Sierra_branch
 * Sync to 10.0(11)
 *
 * Revision 1.1.4.3  1994/03/13  10:39:57  jhayes
 * Branch: Sierra_branch - temporarily restore a timer definition until
 *         68k/r4k timer modularization is completed.
 *
 * Revision 1.1.4.2  1994/03/04  18:35:52  gstovall
 * Branch: Sierra_branch
 * Missed this one in last nights commit.
 *
 * Revision 1.1.4.1  1993/12/29  19:15:33  mbeesley
 * Branch: Sierra_branch
 * Sync the Sierra_branch to the NAC0 mainline.
 * Revision 1.7  1994/06/08  07:41:40  mohsen
 * CSCdi20517:  Add 4T support.
 *
 * Revision 1.6  1994/03/22  00:16:33  smackie
 * Tidy up *_MEG defines and add one for 16M. (CSCdi18309)
 *
 * Revision 1.5  1994/03/19  02:08:46  clev
 * CSCdi17574:  Handle the split main memory on cost reduced C4000
 * Add code to handle the 8m and 32m options on the cost reduced
 * c4000
 *
 * Revision 1.4  1994/03/03  20:29:30  enf
 * CSCdi17638:  configure ntp on a 2500 crashed the box
 * make compile for xx too.
 *
 * Revision 1.3  1993/12/21  06:25:54  enf
 * Cancun boot support
 *
 * Revision 1.2  1993/12/21  02:25:33  mikehub
 * CSCdi14058:  nim ID needed for 4T and Multi Channel Nim
 *
 * Revision 1.1  1993/09/24  16:34:04  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.8  1993/08/19  03:51:44  hampton
 * Bring modular system to Aries 9.21(0.18) patch level.
 *
 * CSCdi09946:  Protect stack_whyreload() from bad boot_reason values
 * 
 * Doing ROM monitor changes required to support EMT_SET_CACR on all
 * 68020/68030/68040 platforms. Share setcacr() and getcacr() on all of
 * these platforms. Move all defines for bits in CACR registers to the
 * pcmap.h files for each processor so that process level code does not
 * have to have its own definition of the CACR bits.
 * 
 * Add better comments to explain memory sizing code in Merlot (IGS/TR)
 * ROM monitor.
 * 
 * Add new EMT call, EMT_WRITE_BAD_MAIN_MEMORY_PARITY, used by the new
 * 'test crash' menu in the system image. This EMT call writes a 32 bit
 * word of information into main memory with bad parity.
 * 
 * Fix EMT number collision between Clean and 9.1 for new EMT call to make
 * life easier for the 9.17 release.  Also, the numbers for all new EMT
 * calls currently defined in the 9.21 release are now also defined in the
 * 9.1 release as placeholders to prevent such EMT collision problems in
 * the future.
 *
 * Revision 1.7  1993/06/25  05:09:11  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 * CSCdi11223:  merge 9.14 low end code into aries
 *
 * Revision 1.6  1993/03/22  09:15:20  smackie
 * Bring include file up-to-date with other repositories.
 *
 * Revision 1.5  1992/10/17  06:03:42  smackie
 * Added in new NIM IDs for Dual Ethernet and Dual Token Ring NIMs
 *
 * Revision 1.4  1992/08/31  20:38:12  dcoli
 * Changes for Brut.
 *
 * Revision 1.3  1992/08/21  22:05:56  smackie
 * Changed the IO base address to 0x6000000 and the acu registers to the
 * correct cache mappings. Also changed the cache default to 0x2111
 *
 * Revision 1.2  1992/04/15  02:11:52  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/04/07  19:49:37  clev
 * Initial CVS checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:10  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * 4000 Memory Map
 *
 * Unused - undecoded and accesses will cause bus error
 *
 * [c]    - address space is cached
 * [u]    - address space is uncached
 *
 * [d]    - address space is in DRAM
 * [s]    - address space is in SRAM
 * [i]    - address space is memory mapped I/O 
 *
 *
 *  +------------------+
 *  |                  |
 *    NIM         [ui]        
 *  |                  |
 *  +------------------+    0x08000000
 *  |                  |
 *    Unused       [c]
 *  |                  |
 *  +------------------+    0x07000000
 *  |                  |
 *    Packet RAM  [ud]
 *  |                  |
 *  +------------------+    0x06000000
 *  |                  |
 *    Unused       [c]
 *  |                  |
 *  +------------------+    0x05000000
 *  |                  |
 *    Unused       [c]
 *  |                  |
 *  +------------------+    0x04000000
 *  |                  |
 *    Flash       [ui]
 *  |                  |
 *  +------------------+    0x03000000
 *  |                  |
 *    System IO   [ui]
 *  |                  |
 *  +------------------+    0x02000000
 *  |                  |
 *    EPROM       [ci]            
 *  |                  |
 *  +------------------+    0x01000000
 *  |                  |
 *    Code        [cd]
 *  |                  |
 *  +- - - - - - - - - +    0x00040000
 *  |                  |
 *  | Code        [cs] |
 *  |                  |
 *  +------------------+    0x00012000
 *  | scratch     [cs] |
 *  +------------------+    0x00011000
 *  |                  |
 *  | smalloc     [cs] |
 *  |                  |
 *  +------------------+    0x00001000
 *  | Stack       [cs] |
 *  +------------------+    0x00000000
 *
 *
 * The scratch space currently looks like
 *
 *  +------------------+    0x00011000
 *  | ichain jmp table |
 *  +------------------+    0x00011F00
 *  | idb ptr table    |
 *  +------------------+    0x00011E00
 *  | nim scratch area | 
 *  +------------------+    0x00011D00
 *  | Currently Free   |    
 *  |                  |
 *  +- - - - - - - - - +    0x00011000
 *
 *
 * The NIM space is mapped as
 *
 *  +------------------+
 *  | NIM Slot 2       |
 *  +------------------+    0x08200000
 *  | NIM Slot 1       |
 *  +------------------+    0x08100000
 *  | NIM Slot 0       |
 *  +------------------+    0x08000000
 *
 */


/*
 * Page map constants/macros:
 */
#define RAMPAGES	0x4000	/* Up to 16384 pages for 16 MB RAM */
#define IOMEM_PAGES	0x4000	/* pages of IOMEM (1, 4, or 16 MB) */
#define PAGESIZE	0x400	/* 1024. bytes per page */
#define PAGESHIFT	10	/* LOG2(PAGESIZE) */

/*
 * Address space allocations:
 */
#define ADRSPC_RAM		0x00000000 /* Start of RAM */
#define ADRSPC_PROM		0x01000000 /* Start of ROM */
#define ADRSPC_PROM_END		0x02000000 /* End of ROM (up to 16 MB) */
#define ADRSPC_NVRAM		0x02000000 /* Start of NV RAM */
#define ADRSPC_NVRAM_STATUSMAG	0x02000000 /* NVRAM status reg Magic number */
#define ADRSPC_NVRAM_STATUS	0x02000002 /* Virtual 16 bit status register */
#define ADRSPC_NVRAM_MAGIC	0x02000004 /* Magic number for NVRAM config */
#define ADRSPC_NVRAM_START	0x02000006 /* Start of NVRAM config data */
#define ADRSPC_NVRAM_END	0x02020000 /* NVRAM address space end (1MB) */
#define ADRSPC_CONTROL		0x02110000 /* System control register */
#define ADRSPC_HW_REVISION	0x02110002 /* ?  Hardware rev & CONTROL2 */

#define ADRSPC_COOKIE           0x02110040 /* Address of cookie */

#define ADRSPC_COOKIE0		0x02110040 /* Address of cookie */
#define ADRSPC_COOKIE1		0x02110048 /* Address of cookie */
#define ADRSPC_COOKIE2		0x02110050 /* Address of cookie */
#define ADRSPC_SHMEM_CONTROL    0x02110100 /* Shared memory control register */
#define ADRSPC_STATUS		0x02110100 /* System status register */
#define ADRSPC_TCONTROL		0x02120040 /* Timer control register */
#define ADRSPC_FLASH_BASE	0x03000000 /* Flash EPROM start address */
#define ADRSPC_EXP_RAM          0x05000000 /* Expansion RAM on rev 2 hardware */
#define ADRSPC_SHAREDMEM        0x06000000 /* start of shared memory */
#define SCOPE_TRIG_REG          0x025a0000 /* scope trigger register */

#define ADRSPC_SERIAL_STATE	0x02130081 /* ?  High speed serial status reg */

#define ADRSPC_NETWORK_A        0x02130000 /* Network interface A registers */
#define ADRSPC_NETWORK_B        0x02130040 /* Network interface B registers */

#define ADRSPC_SLOT_1           0x08000000 /* Interface card slot 1 */
#define ADRSPC_SLOT_2           0x08100000 /* Interface card slot 2 */
#define ADRSPC_SLOT_3           0x08200000 /* Interface card slot 3 */

#define ADRSPC_CODE_START	0x12000	   /* start of RAM for code */

/*
 * Defines for ichain table, idb table and nim srcatch regions 
 * in Scratch space.
 */
#define ICHAIN_ENTRY_SIZE      	4
#define HWIDB_ENTRY_SIZE     	4
#define SCRATCH_ENTRY_SIZE     	4

#define ICHAIN_TABLE_BASE   	0x00011F00  		/* ichain jump table */
#define ICHAIN_SLOT_SIZE       	XX_VECTORS_PER_SLOT
#define ICHAIN_TABLE_SIZE      	ICHAIN_SLOT_SIZE * C4000_NUM_SLOTS
#define ICHAIN_MAX_ENTRIES     	16
#define ICHAIN_BYTES_PER_SLOT  	ICHAIN_MAX_ENTRIES * ICHAIN_ENTRY_SIZE

#define HWIDB_TABLE_BASE        0x00011E00  /* table of hwidb pointers */
#define HWIDB_ENTRIES_PER_SLOT 	16
#define HWIDB_BYTES_PER_SLOT   	(HWIDB_ENTRIES_PER_SLOT * HWIDB_ENTRY_SIZE)
#define HWIDB_TABLE_ENTRIES    	(HWIDB_ENTRIES_PER_SLOT * C4000_NUM_SLOTS)

#define NIM_SCRATCH_BASE               0x00011D00  /* nims scratch area */
#define NIM_SCRATCH_ENTRIES_PER_SLOT   16
#define NIM_SCRATCH_BYTES_PER_SLOT     (NIM_SCRATCH_ENTRIES_PER_SLOT * \
                                        SCRATCH_ENTRY_SIZE)
#define NIM_SCRATCH_ENTRIES            (NIM_SCRATCH_ENTRIES_PER_SLOT * \
                                        C4000_NUM_SLOTS)


/*
 * DEIM Addresses
 */

#define SLOT0_DEIM_A_CSR  0x8000400
#define SLOT0_DEIM_A_RAP  0x8000402
#define SLOT0_DEIM_B_CSR  0x8000C00
#define SLOT0_DEIM_B_RAP  0x8000C02
#define SLOT1_DEIM_A_CSR  0x8100400
#define SLOT1_DEIM_A_RAP  0x8100402
#define SLOT1_DEIM_B_CSR  0x8100C00
#define SLOT1_DEIM_B_RAP  0x8100C02
#define SLOT2_DEIM_A_CSR  0x8200400
#define SLOT2_DEIM_A_RAP  0x8200402
#define SLOT2_DEIM_B_CSR  0x8200C00
#define SLOT2_DEIM_B_RAP  0x8200C02

/*
 * DSIM Addresses
 */

#define SLOT0_DSIM_VECTOR 0x8000202
#define SLOT1_DSIM_VECTOR 0x8100202
#define SLOT2_DSIM_VECTOR 0x8200202

#define SLOT0_DSIM_A_CSR  0x8000380
#define SLOT0_DSIM_A_RAP  0x8000382
#define SLOT0_DSIM_B_CSR  0x8000480
#define SLOT0_DSIM_B_RAP  0x8000482
#define SLOT1_DSIM_A_CSR  0x8100380
#define SLOT1_DSIM_A_RAP  0x8100382
#define SLOT1_DSIM_B_CSR  0x8100480
#define SLOT1_DSIM_B_RAP  0x8100482
#define SLOT2_DSIM_A_CSR  0x8200380
#define SLOT2_DSIM_A_RAP  0x8200382
#define SLOT2_DSIM_B_CSR  0x8200480
#define SLOT2_DSIM_B_RAP  0x8200482

#define DSIM_A_INT        0
#define DSIM_MA_INT       1
#define DSIM_B_INT        2
#define DSIM_MB_INT       3

/*
 * QSIM (4T) Addresses
 */

#define QSIM_SLOT0A_BASE        0x08000000
#define QSIM_SLOT0B_BASE        0x08010000
#define QSIM_SLOT1A_BASE        0x08100000
#define QSIM_SLOT1B_BASE        0x08110000
#define QSIM_SLOT2A_BASE        0x08200000
#define QSIM_SLOT2B_BASE        0x08210000

/* 4T interrupt source registers. */
#define QSIM_SLOT0_INT_SRC      QSIM_SLOT0B_BASE
#define QSIM_SLOT1_INT_SRC      QSIM_SLOT1B_BASE
#define QSIM_SLOT2_INT_SRC      QSIM_SLOT2B_BASE

/* 4T's register offsets for SCA and MSCI registers. */
#define HD64570_ISR1_ISR0     0x110
#define HD64570_NONE_ISR2     0x112
#define HD64570_C0_ST1_ST0    0x122
#define HD64570_C1_ST1_ST0    0x142
#define HD64570_C0_ST3_ST2    0x124
#define HD64570_C1_ST3_ST2    0x144

/* 4T interrupt control registers. */
#define QSIM_SLOT0_INT_0     0x08000006
#define QSIM_SLOT0_INT_1     0x0800000A
#define QSIM_SLOT0_INT_2     0x08010006
#define QSIM_SLOT0_INT_3     0x0801000A
#define QSIM_SLOT1_INT_0     0x08100006
#define QSIM_SLOT1_INT_1     0x0810000A
#define QSIM_SLOT1_INT_2     0x08110006
#define QSIM_SLOT1_INT_3     0x0811000A
#define QSIM_SLOT2_INT_0     0x08200006
#define QSIM_SLOT2_INT_1     0x0820000A
#define QSIM_SLOT2_INT_2     0x08210006
#define QSIM_SLOT2_INT_3     0x0821000A

/* 4T's secondary interrupt wrapper functions. */
#define QSIM_SLOT0A_WRAPPER    NIM1_SCRATCH_AREA
#define QSIM_SLOT0B_WRAPPER    NIM1_SCRATCH_AREA + SCRATCH_ENTRY_SIZE
#define QSIM_SLOT1A_WRAPPER    NIM2_SCRATCH_AREA
#define QSIM_SLOT1B_WRAPPER    NIM2_SCRATCH_AREA + SCRATCH_ENTRY_SIZE
#define QSIM_SLOT2A_WRAPPER    NIM3_SCRATCH_AREA
#define QSIM_SLOT2B_WRAPPER    NIM3_SCRATCH_AREA + SCRATCH_ENTRY_SIZE

/*
 * SynOptics NIM specific defines
 */

/*
 * Ethernet NIM
 */
#define SYNOPTICS_EIM_LED_REGISTER 0x0200 /* This will be bogus if SynOptics ever
                                           * ships a 2 port EIM...
                                           */

#define SYNOPTICS_EIM_LED_BRIDGE   0x4000
#define SYNOPTICS_EIM_LED_STANDBY  0x8000
#define SYNOPTICS_EIM_LED_ALL      (SYNOPTICS_EIM_LED_BRIDGE | SYNOPTICS_EIM_LED_STANDBY)

#define SET_SYNOPTICS_EIM_LEDS(address, mask) *(volatile ushort *)(address) &= ~(mask)
#define CLR_SYNOPTICS_EIM_LEDS(address, mask) *(volatile ushort *)(address) |= (mask)

/*
 * Token Ring NIM
 */
#define SYNOPTICS_TRIM_LED_REGISTER 0x0200 /* This will be bogus if SynOptics ever
                                            * ships a 2 port TRIM...
                                            */

#define SYNOPTICS_TRIM_LED_SRB   0x0100
#define SYNOPTICS_TRIM_LED_TB    0x0200
#define SYNOPTICS_TRIM_LED_ALL   (SYNOPTICS_TRIM_LED_SRB | SYNOPTICS_TRIM_LED_TB)

#define SET_SYNOPTICS_TRIM_LEDS(address, mask) *(volatile ushort *)(address) &=~(mask)
#define CLR_SYNOPTICS_TRIM_LEDS(address, mask) *(volatile ushort *)(address) |= (mask)

/*
 * Definitions for System Control register:
 * (ADRSPC_CONTROL 0x02110000) a 32-bit r/w register
 */
#define CONTROL_PARITY0		0x80000000 /* Parity of cpu ram byte 0 */
#define CONTROL_PARITY1		0x40000000 /* Parity of cpu ram byte 1 */
#define CONTROL_PARITY2		0x20000000 /* Parity of cpu ram byte 2 */
#define CONTROL_PARITY3		0x10000000 /* Parity of cpu ram byte 3 */
#define CONTROL_PERR_MASK       0xf0000000 
#define CONTROL_FLASHSTAT	0x08000000 /* set if flash memory is present */
#define CONTROL_SPARE0		0x04000000 /* spare.  always 1.  */
#define CONTROL_OVER_TEMP	0x02000000 /* over temp, 0 if over temp */
#define CONTROL_LOCTO		0x01000000 /* Local transaction timeout */
#define CONTROL_HREV0         0x00800000 /* Hardware Revision Number */
#define CONTROL_HREV1         0x00400000 /* Hardware Revision Number */
#define CONTROL_HREV2         0x00200000 /* Hardware Revision Number */
#define CONTROL_HREV3         0x00100000 /* Hardware Revision Number */
#define CONTROL_HREV4         0x00080000 /* Hardware Revision Number */
#define CONTROL_HREV5         0x00040000 /* Hardware Revision Number */
#define CONTROL_HREV6         0x00020000 /* Hardware Revision Number */
#define CONTROL_HREV7         0x00010000 /* Hardware Revision Number */
#define CONTROL_HREV_MASK     0x00ff0000 /* Hardware Revision Number Mask */
#define CONTROL_HREV_SHIFT    16         /* Hardware Revision Number Shift */
#define CONTROL_LED_DS5         0x00008000 /* led ds5.  active low */
#define CONTROL_LED_DS7         0x00004000 /* led ds7.  active low */
#define CONTROL_LED_DS9         0x00002000 /* led ds9.  active low */
#define CONTROL_LED_DS10        0x00001000 /* led ds10. active low */
#define CONTROL_LEDS_ALL        0x0000f000
#define CONTROL_SPARE1          0x00000800 /* spare  */
#define CONTROL_SPARE2          0x00000400 /* spare  */
#define CONTROL_TIMER_CASCADE   0x00000200 /* cascade 8254, active high */
#define CONTROL_WATCHDOG        0x00000100 /* enable watchdog, active high */
#define CONTROL_PARITY_CLEAR    0x00000080 /* clear parity err, active low */
#define CONTROL_FLASH_VPP       0x00000040 /* enable vpp, active high */
#define CONTROL_ENABLE_BERR     0x00000020 /* enable bus err, active hi */
#define CONTROL_RUNLAMP         0x00000010 /* sys run light, active low */
#define CONTROL_PARITY_GEN      0x00000008 /* 0: even par, 1: odd par */
#define CONTROL_MEMORY1         0x00000004 /* cpu memory config bit 1 (see defines below) */
#define CONTROL_MEMORY0         0x00000002 /* cpu memory config bit 0 (see defines below) */
#define CONTROL_MEM_MASK        0x00000006 /* cpu memory config bit mask (see defines below) */
#define CONTROL_BOOT            0x00000001 /* boot flag, active low   */

/*
 * Extra SynOptics (Truffle) bit defines
 */
#define ADRSPC_SYNOPTICS_LEDS   ADRSPC_CONTROL

#define SYNOPTICS_LED_BOOT      0x00001000
#define SYNOPTICS_LED_BYPASS    0x00000400
#define SYNOPTICS_LED_ALL       (SYNOPTICS_LED_BOOT | SYNOPTICS_LED_BYPASS)

#define SYNOPTICS_LED_BOOT_MASK SYNOPTICS_LED_BOOT

/*
 * Macro to do the logic inversion for you:
 */
#define SET_SYNOPTICS_LEDS(address,mask) *(volatile ulong *)(address) &= ~(mask)
#define CLR_SYNOPTICS_LEDS(address,mask) *(volatile ulong *)(address) |=  (mask)

/*
 * CPU memory defines
 */
#define CONTROL_1MEG_MEM        0x2   /* original design */
#define CONTROL_4MEG_MEM        0x4   /* both designs */
#define CONTROL_16MEG_MEM       0x0   /* both designs */
#define CONTROL_32MEG_MEM       0x2   /* new cost reduced design */
#define CONTROL_8MEG_MEM        0x6   /* new cost reduced design */

/*
 * Defines for the shared memory status and control register
 * (ADRSPC_SHMEM_CONTROL    0x02110100) a 32-bit r/w register
 */
#define SHMEM_SLOT3_PERR   0x80000000  /* Parity error caused by slot 3 access (active hi) */
#define SHMEM_SLOT2_PERR   0x40000000  /* Parity error caused by slot 2 access (active hi) */
#define SHMEM_SLOT1_PERR   0x20000000  /* Parity error caused by slot 1 access (active hi) */
#define SHMEM_MPU_PERR     0x10000000  /* Parity error caused by MPU access (active hi) */
#define SHMEM_BYTE3_PERR   0x08000000  /* Parity error in byte 3 (active hi) */
#define SHMEM_BYTE2_PERR   0x04000000  /* Parity error in byte 2 (active hi) */
#define SHMEM_BYTE1_PERR   0x02000000  /* Parity error in byte 1 (active hi) */
#define SHMEM_BYTE0_PERR   0x01000000  /* Parity error in byte 0 (active hi) */
#define SHMEM_PARITY_MASK  0xff000000  /* Mask for all of the parity information */
#define SHMEM_SPARE0       0x00800000  /* Spare (always hi) */
#define SHMEM_SLOT3_OCC    0x00400000  /* Slot 3 occupied (active lo) */
#define SHMEM_SLOT2_OCC    0x00200000  /* Slot 2 occupied (active lo) */
#define SHMEM_SLOT1_OCC    0x00100000  /* Slot 1 occupied (active lo) */
#define SHMEM_SLOT3_L3IRQ  0x00080000  /* Slot 3 level 3 interrupt request (active lo) */
#define SHMEM_SLOT2_L3IRQ  0x00040000  /* Slot 2 level 3 interrupt request (active lo) */
#define SHMEM_SLOT1_L3IRQ  0x00020000  /* Slot 1 level 3 interrupt request (active lo) */
#define SHMEM_SPARE1       0x00010000  /* Spare (always hi) */
#define SHMEM_SLOT3_L4IRQ  0x00008000  /* Slot 3 level 4 interrupt request (active lo) */
#define SHMEM_SLOT2_L4IRQ  0x00004000  /* Slot 2 level 4 interrupt request (active lo) */
#define SHMEM_SLOT1_L4IRQ  0x00002000  /* Slot 1 level 4 interrupt request (active lo) */
#define SHMEM_L4IRQ_ID     13          /* Level 4 IRQ field's 1st bit index */
#define SHMEM_SPARE2       0x00001000  /* Spare (always hi) */
#define SHMEM_SPARE3       0x00000800  /* Spare (always hi) */
#define SHMEM_SPARE4       0x00000400  /* Spare (always hi) */
#define SHMEM_SPARE5       0x00000200  /* Spare (always hi) */
#define SHMEM_SPARE6       0x00000100  /* Spare (always hi) */
#define SHMEM_SLOT_RESET   0x00000080  /* Reset the slots (active lo?) */
#define SHMEM_SLOT3_LED    0x00000040  /* Slot 3 status LED (active lo) */
#define SHMEM_SLOT2_LED    0x00000020  /* Slot 2 status LED (active lo) */
#define SHMEM_SLOT1_LED    0x00000010  /* Slot 1 status LED (active lo) */
#define SHMEM_SLOT_LEDS    0x00000070  /* All of the slot status LEDs (active lo) */
#define SHMEM_CONFIG0      0x00000008  /* Shared memory configuration bit */
#define SHMEM_CONFIG1      0x00000004  /* Shared memory configuration bit */
#define SHMEM_PARITY_ODD   0x00000002  /* Shared memory parity sex (ODD active hi) */
#define SHMEM_PERR_CLEAR   0x00000001  /* Shared memory parity error clear (write lo then hi) */

#define SHMEM_1MEG         0x8
#define SHMEM_4MEG         0x4
#define SHMEM_16MEG        0x0

/*
 * Definition of ID Prom on DosXX
 */
/*
 ***************************************************************
 *							       *
 * If you add a NIM ID, you must concurrently update this      *
 * table in all current release trees to prevent the           *
 * possibility of multiple assignment.                         *
 * Also, please update sys/src-4k-c4000/pcmap.h	               *
 ***************************************************************
 */

#define XX_FDDI_SINGLE   0x00       /* Single Attach, no bridge hardware */
#define XX_FDDI_DUAL     0x01       /* Dual Attach, no bridge hardware */
#define XX_FDDI_SBRIDGE  0x02       /* Single Attach, bridge hardware */
#define XX_FDDI_DBRIDGE  0x03       /* Dual Attach, bridge hardware */
#define XX_ETHER         0x04       /* Ethernet, Lance Type */
#define XX_TOKEN_RING    0x05       /* Token Ring */
#define XX_SERIAL_SINGLE 0x06       /* Single Serial I/O */
#define XX_SERIAL_DUAL   0x07       /* Dual Serial I/O */
#define XX_ETHER_SINGLE  0x08       /* Dual Ethernet, Single Port, Lance */
#define XX_ETHER_DUAL    0x09       /* Dual Ethernet, Dual Port, Lance */
#define XX_TOKEN_SINGLE  0x0A       /* Dual Token Ring, Single Port, C16 */
#define XX_TOKEN_DUAL    0x0B       /* Dual Token Ring, Dual Port, C16 */
#define XX_SERIAL_QUAD   0x0C       /* Quad Serial, Hatachi chip */
#define XX_MCN           0x0D       /* Multi Channel Serial Nim, Munich32 */
#define XX_PCI           0x0E       /* PCI-based NIMs */
#define XX_ATM_BX50      0x0F       /* ATMizer BX-50 */
#define XX_ATM_RES1      0x10       /* ATM NIM reserved ID */
#define XX_ATM_RES2      0x11       /* ATM NIM reserved ID */
#define XX_ATM_RES3      0x12       /* ATM NIM reserved ID */
#define XX_SERIAL_2T16S  0x13       /* Serial I/O for 2T(Hitachi 64570), 16S
                                       (Cirrus Logic CD2402). */

/*
 * definitions for Timer control register:
 */
#define TCONTROL_G2		0x80	/* Links counters 1 and 2 */
#define TCONTROL_WE		0x40	/* Watchdog enable */


/*
 * Reading from ADRSPC_SERIAL_STATE will return the following line status bits:
 */
#define SERIAL_CTS	0x0080		/* Read state of *CTS line */
#define SERIAL_DCD	0x0040		/* Read state of *DCD line */
#define SERIAL_DSR	0x0020		/* Read state of *DSR line */
#define SERIAL_DTR	0x0010		/* Read/Write state of *DTR line */

/*
 * Cache control register bits for the 68020/68030:
 */
#define ENA_ICACHE	0x0001
#define CLR_ICACHE	0x0008
#define ENA_DCACHE	0x0100	/* Ignored on 68020 */
#define CLR_DCACHE	0x0800	/* Ignored on 68020 */
#define INST_BURST	0x0010	/* Ignored on 68020 */
#define DATA_BURST	0x1000	/* Ignored on 68020 */
#define WRITE_ALLOCATE	0x2000	/* Ignored on 68020 */

#define CACHE_I		0x0011
#define CACHE_D		0x3100
#define ENABLE_CACHES	0x2111
#define CLEAR_CACHES	0x0808
#define CLR_DC_EIC      0x0811

#define ADRSPC_BRUT_TIMER0      0x02120050 /* Timer reg 0 (BRUT only) */
#define ADRSPC_BRUT_TIMER1      0x02120060 /* Timer reg 1 (BRUT only) */
#define ADRSPC_BRUT_TIMER2      0x02120070 /* Timer reg 2 (BRUT only) */
