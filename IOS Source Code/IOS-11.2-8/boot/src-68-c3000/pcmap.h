/* $Id: pcmap.h,v 3.3.4.3 1996/07/25 23:37:38 jhernand Exp $
 * $Source: /release/112/cvs/Xboot/src-68-c3000/pcmap.h,v $
 *------------------------------------------------------------------
 * pcmap.h -- memory mapping definitions
 *
 * August 1989, Chris Shaker
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcmap.h,v $
 * Revision 3.3.4.3  1996/07/25  23:37:38  jhernand
 * CSCdi57040:  SDLC test fails intermittently on Async/Sync ports OK on
 *              Sync port
 * Add the TXCRXC control bit to Low Speed Serial Device Register Hi
 * Branch: California_branch
 *
 * Revision 3.3.4.2  1996/06/17  08:44:54  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.3.4.1  1996/06/13  17:18:30  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.3  1996/03/08  23:37:18  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.2  1995/12/08  19:29:52  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.1  1995/11/09  09:09:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  23:58:17  samuell
 * CSCdi33515:  ROM monitor not reset HD64570 on C2500 platform
 *
 * Revision 2.1  1995/06/07  19:15:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.13  1995/03/21 23:45:54  gstovall
 * CSCdi31294:  Add support for new platform
 * Bring the joy of Sapphire to the 10.3 mainline.
 *
 * Revision 1.12  1995/03/07  06:36:52  vnguyen
 * CSCdi30540:  need to clean up hub repeater code
 *              - remove code that support token ring hub
 *              - move hub misc register from pcmap.h to hub.h
 *
 * Revision 1.11  1995/02/23  23:49:38  vnguyen
 * CSCdi30111:  Add ethernet hublet derivative support to 10.3
 *
 * Revision 1.10  1994/11/22  08:33:45  sgarcia
 * CSCdi27026:  New product support.
 *    Incorporate Galapagos support.
 *
 * Revision 1.9  1994/10/30  21:59:18  abaindur
 * CSCdi25240:  Need to support dfb on low end systems
 * Flash Load Helper and Dual Flash Bank port.
 *
 * Revision 1.8.2.1  1994/10/07  23:31:08  enf
 * Branch: Dfb_branch
 * flh support for ROMMON
 *
 * Revision 1.8  1994/09/27  23:20:03  jkuzma
 * CSCdi24241:  Add support for 2500 derivative (see Description)
 * Added constants for 2 interrupt levels used by Cortez.
 *
 * Revision 1.7  1994/08/16  17:13:38  jturner
 * CSCdi23449:  Changes necessary for 2500 derivative
 *                 New 2500 HW supported: 2r2t, 1r2t1e, 2e2t.
 *
 * Revision 1.6  1994/07/09  11:50:02  mdb
 * Sync MAIN to NAC1_postsync_940706
 *
 * Revision 1.1.4.3.10.3  1994/07/09  03:09:17  slin
 * Branch: NAC1_branch
 * Sync to Sierra_postsync_940706.
 *
 * Revision 1.1.4.3.10.2  1994/06/22  04:12:57  slin
 * Branch: NAC1_branch
 * Sync to Sierra_postsync_940615.
 *
 * Revision 1.1.4.3.10.1  1994/06/11  19:24:35  tkolar
 * Branch: NAC1_branch
 * Integrate commserver features.
 *
 * Revision 1.2.4.2  1994/05/21  19:03:30  tkolar
 * Branch: CS_branch
 * Make CS_branch into a Sierra twig.
 *
 * Revision 1.2.4.1  1994/02/17  00:45:48  vandys
 * Branch: CS_branch
 * Add Copan async misc. register definition
 *
 * Revision 1.1.4.5  1994/07/07  21:05:50  jhayes
 * Branch: Sierra_branch
 *         Sync Sierra_branch to Sierra_baseline_940706
 *
 * Revision 1.1.4.4  1994/06/18  01:12:15  mbeesley
 * Branch: Sierra_branch
 * Sync to mainline tag Sierra_baseline_940615.
 *
 * Revision 1.1.4.3  1994/03/30  03:10:41  mohsen
 * Branch: Sierra_branch
 * Add 4T support.
 *
 * Revision 1.1.4.2  1994/03/03  21:56:20  abaindur
 * Branch: Sierra_branch
 *    Sierra flash driver and file system.
 *
 * Revision 1.1.4.1  1993/12/29  19:15:29  mbeesley
 * Branch: Sierra_branch
 * Sync the Sierra_branch to the NAC0 mainline.
 *
 * Revision 1.5  1994/06/28  18:41:16  sjay
 * CSCdi20865:  Boot changes to support Brut II: more memory; jump to
 * diagnostics
 *
 * Support for Brut II:
 * 1.  Brut II has 8 MB DRAM.  Split it properly into IO & Processor (2/6)
 *     by using same code as for Cancun.
 * 2.  Support diagnostic code in high ROM by jumping to it if
 *     "manufacturing mode" hardware status is pulled low during
 *     initialization.
 *
 * Revision 1.4  1994/06/12  20:14:23  smackie
 * Add missing file to allow 3000 images to compile again.
 * CSCdi20767:  Add FIJI support
 *
 * Revision 1.3  1994/06/08  07:41:28  mohsen
 * CSCdi20517:  Add 4T support.
 *
 * Revision 1.2  1993/12/21  06:25:48  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:35  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.9  1993/08/19  03:51:27  hampton
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
 * Revision 1.8  1993/06/25  05:09:04  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 * CSCdi11223:  merge 9.14 low end code into aries
 *
 * Revision 1.7  1993/02/09  01:27:43  dcoli
 * CSCdi09135 - Brut fails to de-assert DTR on "shutdown"
 *
 * Revision 1.6  1993/02/03  04:21:40  gstovall
 * Removed support for the cristal low-end platform.
 *
 * Revision 1.5  1992/09/19  21:54:34  gstovall
 * Added changes to support the Cristal low-end router.  Added new processor
 * type and converted a few if-else to switch statements.
 *
 * Revision 1.4  1992/08/31  20:38:04  dcoli
 * Changes for Brut.
 *
 * Revision 1.3  1992/07/07  22:58:02  clev
 * Fix the Merlot interpretation of the magic flag bus size bits.
 *
 * Revision 1.2  1992/03/31  01:24:56  dit
 * add Flash Eprom Base
 *
 * Revision 1.1  1992/03/07  23:57:39  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Page map constants/macros:
 */
#define RAMPAGES        0x4000  /* Up to 16384 pages for 16 MB of RAM */
#define IOMEM_PAGES	0x200	/* pages of IOMEM (last 1/2 MB) */
#define PAGESIZE        0x400   /* 1024. bytes per page */
#define PAGESHIFT       10      /* LOG2(PAGESIZE) */

/*
 * Definition of flag bits, written into the ROM monitor image by the
 * ROM image creation program, /csc/tools/bytek.c:
 */
/* Bits 0 - 3 are ROM size bits */
#define FLAGS_ROMSIZE_MASK	0x0007
#define FLAGS_ROMSIZE_512K	1	/* EPROM size values (# of bits) */
#define FLAGS_ROMSIZE_1M	2
#define FLAGS_ROMSIZE_2M	3
#define FLAGS_ROMSIZE_4M	4
#define FLAGS_ROMSIZE_8M	5
#define FLAGS_ROMSIZE_16M	6
#define FLAGS_ROMSIZE_32M	7
/* Bits 4 - 6 are bus size bits */
#define FLAGS_BUS_MASK          0x070
#define FLAGS_BUS_8		0x010
#define FLAGS_BUS_16		0x020
#define FLAGS_BUS_32		0x040


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
#define ADRSPC_NVRAM_END	0x02100000 /* NVRAM address space end (1MB) */
#define	ADRSPC_CONTROL		0x02110000 /* System control register */
#define ADRSPC_HW_REVISION	0x02110002 /* Hardware rev & CONTROL2 */
#define ADRSPC_CONTROL2		ADRSPC_HW_REVISION
#define ADRSPC_RESET            0x02110004 /* Reset control register */
#define ADRSPC_INTERRUPT        0x02110006 /* Interrupt status register */
#define ADRSPC_FIJI_CONTROL     0x02110008 /* Control-status register */
#define ADRSPC_ASIC_REG         0x0211000A /* ASIC register */
#define ADRSPC_BRUT_ASIC_REV    0x02110010 /* ASIC rev level (BRUT only) */
#define ADRSPC_CONTROL3		0x02110080
#define ADRSPC_CONTROL4		0x02110140
#define ADRSPC_COOKIE		0x02110040 /* Address of cookie */
#define ADRSPC_SERIAL_NVRAM     0x02110060 /* Serial NVRAM control register */
#define ADRSPC_STATUS		0x02110100 /* System status register */
#define ADRSPC_TCONTROL		0x02120040 /* Timer control register */
#define ADRSPC_BRUT_TIMER0      0x02120050 /* Timer register 0 (BRUT only) */
#define ADRSPC_BRUT_TIMER1      0x02120060 /* Timer register 1 (BRUT only) */
#define ADRSPC_BRUT_TIMER2      0x02120070 /* Timer register 2 (BRUT only) */
#define ADRSPC_NETWORK_A	0x02130000 /* Network interface A registers */
#define ADRSPC_NETWORK_B	0x02130040 /* Network interface B registers */
#define ADRSPC_NETWORK_C	0x02130100 /* Network interface C registers */
#define ADRSPC_SERIAL_STATE_B	0x02130081 /* serial status reg for chnl b */
#define ADRSPC_SERIAL_STATE_C	0x02130141 /* serial status reg for chnl c */
#define ADRSPC_CRISTAL_NET_A  	0x02131000 /* Net A (token ring) registers */
#define ADRSPC_TOKEN_CH_A_MAP_1 0x02131010 /* Token ring H/W map register 1 */
#define ADRSPC_TOKEN_CH_A_MAP_2	0x02131012 /* Token ring H/W map register 2 */
#define ADRSPC_CRISTAL_NET_C  	0x02132000 /* Net C (serial) registers */
#define ADRSPC_DEVICE_SERIAL_0	0x02132100 /* Serial 0 device register */
#define ADRSPC_DEVICE_SERIAL_1	0x02132102 /* Serial 1 device register */
#define ADRSPC_CAMPECHE_NET_C   0x02133000 /* Net C (token ring 2) registers */
#define ADRSPC_TOKEN_CH_C_MAP_1 0x02133010 /* Token ring H/W Ch C map register 1 */
#define ADRSPC_TOKEN_CH_C_MAP_2 0x02133012 /* Token ring H/W Ch C map register 2 */
#define ADRSPC_LSS_MISC		0x02134000 /* Low-speed serial miscellany */
#define ADRSPC_LSS_REG_BASE	0x02134002 /* Cobra LSS device registers base */
#define ADRSPC_LSS_RESET_ASEL	0x02134090 /* Cobra LSS reset and automatic
                                              10BaseT or AUI select register */
#define ADRSPC_GAL_HIMIB_0_CTRL 0x02134027 /* ether Galapagos motherboard
					      HIMIB control register */
#define ADRSPC_GAL_HIMIB_0_DATA 0x02134026 /* ether Galapagos motherboard
					      HIMIB data register */
#define ADRSPC_GAL_HIMIB_1_CTRL 0x02134029 /* ether Galapagos daughterboard
					      HIMIB control register */
#define ADRSPC_GAL_HIMIB_1_DATA 0x02134028 /* ether Galapagos daughterboard
				              HIMIB data register */
#define ADRSPC_GAL_HUB_MISC_REG 0x02134030 /* Galapagos Hub Misc Register */
#define ADRSPC_GAL_HUB_IVR      0x02134032 /* Galapagos Interrupt Vector Register */
#define ADRSPC_BOA_WAN0_MISC    0x02134040 /* Serial 0 Integrated CSU-DSU misc register */
#define ADRSPC_BOA_WAN0_CMD_REG \
                                0x02134051 /* Serial 0 CSU-DSU register address pointer */
#define ADRSPC_BOA_WAN0_DATA_REG \
                                0x02134050 /* Serial 0 CSU-DSU i/o data bus */
#define ADRSPC_BOA_WAN1_MISC   0x02134060 /* Serial 1 Integrated CSU-DSU misc register */
#define ADRSPC_BOA_WAN1_CMD_REG \
                                0x02134071 /* Serial 1 CSU-DSU register address pointer */
#define ADRSPC_BOA_WAN1_DATA_REG \
                                0x02134070 /* Serial 1 CSU-DSU i/o data bus */
#define ADRSPC_BOA_IVR          0x02134080 /* CSU-DSU ALM & IRQ Interreupts Vector Register */
#define ADRSPC_BOA_ETHER_MISC   0x02134090 /* Boa Ethernet Misc Register */
#define ADRSPC_BOA_BRI_MISC     0x021340A0 /* Boa BRI module Misc Register */
#define ADRSPC_BRUT_ENET        0X02140000 /* where ether. addr is stored */ 
                                           /* on a brut box instead of cookie*/

#define ADRSPC_CODE_START	 LOWCORE1      /* start of RAM for code */

#define ADRSPC_SYNOPTICS_LEDS	0x021300C0 /* Synoptics LED register */
#define ADRSPC_FIJI_SHRAM       0x0C000000 /* Fiji Shared RAM start addr */

/*
 * Chipcom mailbox addresses - 8 bits wide
 */
#define ADRSPC_CHIPCOM_MB_DATA	0x02120500 /* Chipcom mailbox data register */
#define ADRSPC_CHIPCOM_MB_RESET	0x02120501 /* Chipcom uP reset register */
/* If bit 31 is low, the 705 is reset. If bit 31 is high, the 705 is unreset */
#define ADRSPC_CHIPCOM_MB_ENABLE 0x02120502 /* Chipcom mailbox int enable */
/* Any write to this address will enable Mailbox interrupts */
#define ADRSPC_CHIPCOM_MB_DISABLE 0x02120503 /* Chipcom mailbox int disable */
/* Any write to this address will disable Mailbox interrupts */

/*
 * Chipcom mailbox addresses - Cancun version (aka, "Cortez")
 */
#define ADRSPC_CORTEZ_MBOX_DATA  0x02110008 /* Mailbox address */
#define ADRSPC_CORTEZ_MBOX_MASK  0x02110009 /* Mailbox interrupt mask addr */

/*
 * Microcom mailbox addresses - Copan version Cisco/Microcom Terminator Mailbox
 */
#define ADRSPC_TERMINATOR_MBOX_DATA  0x02110008 /* Mailbox address */
#define ADRSPC_TERMINATOR_MBOX_STATUS  0x02134000 /* Mailbox mask register */

#define ADRSPC_FLASH_BASE	0x03000000 /* Flash EPROM start address */
#define ADRSPC_BOOT_ROM_BASE  0x01000000 /* BOOT ROM start address */

/*
 * When Merlot has 512 KB, 1 MB, or 4 MB of CPU RAM, the IO memory is
 * contiguous with the CPU RAM. When we have 16 MB of CPU RAM, the
 * secondary memory moves to a new address.  On Cristal, this address is
 * constant, regardless of the size of CPU RAM.
 */

#define ADRSPC_MERLOT_IOMEM	0x04000000
#define ADRSPC_MERLOT_IOMEM_END 0x04200000


/*
 * Definitions for System Control register for 
 * pan/merlot/moosehead products. After that is 
 * the BRUT definition
 */
#define	CONTROL_PARITY0		0x8000	/* Parity of ram byte 0 */
#define	CONTROL_PARITY1		0x4000	/* Parity of ram byte 1 */
#define	CONTROL_PARITY2		0x2000	/* Parity of IO ram byte 2 */
#define	CONTROL_PARITY3		0x1000	/* Parity of IO ram byte 3 */
#define CONTROL_PARITY4		0x0800	/* Parity of ram byte 4 */
#define CONTROL_PARITY5		0x0400	/* Parity of ram byte 5 */
#define CONTROL_PARITY_ERROR	0x0200	/* A ram parity error occured */
#define	CONTROL_LOCTO		0x0100	/* Local transaction timeout */
#define CONTROL_NETWORK_B_INT	0x0080	/* Network B interrupt active */
#define CONTROL_NETWORK_A_INT	0x0040	/* Network A interrupt active */
#define CONTROL_SERIAL_LOOP	0x0020	/* Enable Serial Loopback */
#define	CONTROL_RUNLAMP		0x0010	/* Run light */
#define	CONTROL_EVENPAR		0x0008	/* Even Parity on original IGS, */
					/* disable parity when writing  */
					/* on Merlot and later IGSs     */
#define CONTROL_RESET_NETWORK_A	0x0004	/* Reset for Network A interface */
#define CONTROL_RESET_NETWORK_B	0x0002	/* Reset for Network B interface */
#define	CONTROL_BOOT		0x0001	/* Boot state */

/* 
 * Definitions for System Control register for the BRUT box.
 * This definition only shows the differences between the above
 * pan/merlot/moosehead definition for System Control register and 
 * the BRUT defintion for this register.
 */
/* Network C interrupt active. this bit replaces the serial loopback bit
 * in the pan/merlot system control register definition. */
#define BRUT_NETWORK_C_INT  0x0020	

/*
 * Definations for Cancun System Control Register 1 (SCR1).  Cancun
 * Allows parity to be enabled and disabled and also allows setting of 
 * odd/even parity
 */
#define CANCUN_PARITY_ENABLE	0x0008
#define CANCUN_EVEN_PARITY	0x0004 /* odd parity sets this bit to zero */

/* These two bits are used to identify which serial port had a modem 
 * control state change (CTS,DCD,DSR). They replace the reset network 
 * A and B bits in the pan/merlot system control register definition. */
#define BRUT_MODEM_CHANGE_B 0X0004
#define BRUT_MODEM_CHANGE_C 0X0002

/*
 * Brut uses a the I3 input of the DUART, which is connected to a pin on
 * the backplace connector, to signal the boot code to jump to diagnostics
 * stored in high ROM space.  DEC calls this the "manufacturing mode" bit.
 * This used only on Brut's with 2 MB boot code, signified by an ASIC rev
 * level defined here.
 *
 * When the diagnostic is present in the ROM, a "magic number" and a vector
 * pointing to the entry point of the diagnostic are stored at the very
 * high end on the ROM.
 *
 */
#define BRUT_DUART_MFG_MODE     0x8
#define BRUT_2M_BOOT_REV        2
#define BRUT_DIAG_MAGIC_ADDR    0x011FFFFC
#define BRUT_DIAG_MAGIC_VALUE   0x4747
#define BRUT_DIAG_VECTOR_ADDR   0x011FFFF8

/*
 * Note that the MSB of the ADRSPC_HW_REVISION register is the hardware
 * revision number. This number changes as the hardware is ECOd.
 * On Merlot, the least significant byte is the System Control register 2.
 * On the IGS, least significant byte is merely a copy of the System Control
 * register.
 * Definitions for System Control register 2 (Merlot and BRUT only):
 */
#define CONTROL2_VPPEN		0x0080 /* 1=VPP on, 0=VPP off */
#define CONTROL2_4M		0x0040 /* 0=16, 1=4 Mbit - Token ring speed */
#define CONTROL2_MSIZE_MASK	0x0030 /* main memory size control bits */
#define CONTROL2_MSIZE_SHIFT	4      /* bits to shift left for MSIZE */
#define CONTROL2_MSIZE_16MB	0      /* 16 MB main memory, 32 bits wide */
#define CONTROL2_MSIZE_4MB	1      /* 4 MB main memory, 32 bits wide */
#define CONTROL2_MSIZE_1MB	2      /* 1 MB main memory, 32 bits wide */
#define CONTROL2_MSIZE_2MB	2      /* 2 MB main memory, 32 bits wide */
#define CONTROL2_MSIZE_512KB	3      /* 512 KB main memory, 16 bits wide */
#define CONTROL2_MSIZE_8MB	3      /* 8 MB main memory, 32 bits wide */
/* For the 2 boot EPROM sockets - 16 bits wide */
#define CONTROL2_BROMSIZE_MASK	0x000C /* ROM size control bits */
#define CONTROL2_BROMSIZE_SHIFT	2      /* bits to shift left for BROMSIZE */
#define CONTROL2_BROMSIZE_4MB	3      /* 16M bit boot ROMs */
#define CONTROL2_BROMSIZE_2MB	0      /* 8 M bit boot ROMs */
#define CONTROL2_BROMSIZE_1MB	1      /* 4 M bit boot ROMs */
#define CONTROL2_BROMSIZE_512KB	2      /* 2 M bit boot ROMs */
#define CONTROL2_BROMSIZE_256KB	3      /* 1 M bit boot ROMs */
#define CONTROL2_FLASHSIZE_MASK	0x0003 /* FLASH size control bits */
/* For the 8 Flash/EPROM sockets - 32 bits wide */
#define CONTROL2_FLASHSIZE_4MB	0      /* 4 M bit EPROMs */
#define CONTROL2_FLASHSIZE_2MB	1      /* 2 M bit EPROMs or FLASH ROMs */
#define CONTROL2_FLASHSIZE_1MB	2      /* 1 M bit EPROMs or FLASH ROMs */


/*
 * Definitions for System Control register 3 (BRUT only):
 * Some of these bits exist in other registers for other platforms.
 */
#define BRUT_HUB_SLOT_3          0x8000 /* hub slot id bit 3 */
#define BRUT_HUB_SLOT_2          0x4000 /* hub slot id bit 2 */
#define BRUT_HUB_SLOT_1          0x2000 /* hub slot id bit 1 */
#define BRUT_HUB_SLOT_0          0x1000 /* hub slot id bit 0 */
#define BRUT_SERIAL_TYPE_B2      0x0800 /* serial cable id chnl B bit 2 */
#define BRUT_SERIAL_TYPE_B1      0x0400 /* serial cable id chnl B bit 1 */
#define BRUT_SERIAL_TYPE_B0      0x0200 /* serial cable id chnl B bit 0 */
#define BRUT_SERIAL_TYPE_C2      0x0100 /* serial cable id chnl C bit 2 */
#define BRUT_SERIAL_TYPE_C1      0x0080 /* serial cable id chnl C bit 1 */
#define BRUT_SERIAL_TYPE_C0      0x0040 /* serial cable id chnl C bit 0 */
#define BRUT_SERIAL_LOOPBACK_C   0x0020 /* Enable Serial Loopback, chnl C*/
#define BRUT_SERIAL_LOOPBACK_B   0x0010 /* Enable Serial Loopback, chnl B */
#define BRUT_RESET_COUNTER       0x0008 /* Reset for ASIC counters */
#define BRUT_RESET_NETWORK_A     0x0004 /* Reset for Network A interface */
#define BRUT_RESET_NETWORK_B     0x0002 /* Reset for Network B interface */
#define BRUT_RESET_NETWORK_C     0x0001 /* Reset for Network C interface */

/*
 * Definitions for System Control register 4 (BRUT only):
 * These define the number of wait states for the various board devices.
 */
#define BRUT_0_WS_COOKIE         0      /* 0 wait states */
#define BRUT_1_WS_COOKIE         0x4000 /* 1 wait state */
#define BRUT_2_WS_COOKIE         0x8000 /* 2 wait states */
#define BRUT_3_WS_COOKIE         0xC000 /* 3 wait states */
/*
 * Cache control register bits for the 68020/68030:
 */
#define ENA_ICACHE	0x0001
#define CLR_ICACHE	0x0008
#define ENA_DCACHE	0x0100	/* Ignored on 68020 */
#define CLR_DCACHE	0x0800	/* Ignored on 68020 */
#define WRITE_ALLOCATE	0x2000	/* Ignored on 68020 */
#define INST_BURST	0x0010
#define DATA_BURST	0x1000

#define CACHE_I		0x0001
#define CACHE_D		0x2100
#define ENABLE_CACHES	0x2101
#define CLEAR_CACHES	0x0808
#define CLR_DC_EIC      0x0801
#define BRUT_0_WS_FLASH          0      /* 0 wait reads, 1 wait writes */
#define BRUT_1_WS_FLASH          0x1000 /* 1 wait reads, 2 wait writes */
#define BRUT_2_WS_FLASH          0x2000 /* 2 wait reads, 3 wait writes */
#define BRUT_3_WS_FLASH          0x3000 /* 3 wait reads, 4 wait writes */
#define BRUT_1_WS_NVRAM          0      /* 1 wait state */
#define BRUT_2_WS_NVRAM          0x0400 /* 2 wait states */
#define BRUT_3_WS_NVRAM          0x0800 /* 3 wait states */
#define BRUT_4_WS_NVRAM          0x0C00 /* 4 wait states */
#define BRUT_1_WS_PROM           0      /* 1 wait state  (120 nS) */
#define BRUT_2_WS_PROM           0x0200 /* 2 wait states (150 nS) */
#define BRUT_0_WS_DRAM           0      /* 0 wait states (20MHz) */
#define BRUT_1_WS_DRAM           0x0100 /* 1 wait state  (25MHz) */
#define BRUT_5_WS_UART           0      /* 5 wait states */
#define BRUT_6_WS_UART           0x0080 /* 6 wait states */
#define BRUT_20MHZ_CPU           0      /* CPU speed is 20MHz */
#define BRUT_25MHZ_CPU           0x0002 /* CPU speed is 25MHz */
#define BRUT_IO_PRIORITY         0      /* I/O devices have DRAM priority */
#define BRUT_CPU_PRIORITY        0x0001 /* CPU has priority accessing DRAM */

/*
 * Defintions for Reset Control register (cristal only)
 * All the reset bits have been pulled from other registers and put here.
 * Add reset bits as needed.  Bits 14 and 15 are used for token ring speed.
 */
#define RESET_RING_SPEED_1      0x8000 /* Ring speed for token ring dev 1 */
#define RESET_RING_SPEED_2      0x4000 /* Ring speed for token ring dev 2 */
#define RESET_GLOBAL            0x0100 /* Global reset (except cpu) */
#define RESET_NETWORK_H         0x0080 /* Reset for network chan H */
#define RESET_NETWORK_G         0x0040 /* Reset for network chan G */
#define RESET_NETWORK_F         0x0020 /* Reset for network chan F */
#define RESET_NETWORK_E         0x0010 /* Reset for network chan E */
#define RESET_NETWORK_D         0x0008 /* Reset for network chan D */
#define RESET_NETWORK_C         0x0004 /* Reset for network chan C */
#define RESET_NETWORK_B         0x0002 /* Reset for network chan B */
#define RESET_NETWORK_A         0x0001 /* Reset for network chan A */

#define ALL_NETWORK_RESET       (RESET_NETWORK_A | RESET_NETWORK_B | \
                                RESET_NETWORK_C | RESET_NETWORK_D  | \
				RESET_NETWORK_E | RESET_NETWORK_F)

/*
 * Defintions for Interrupt Status register (cristal only)
 * All the interface interrupt bits have been moved to here.
 */
#define INTERRUPT_MODEM_H       0x8000 /* Modem control int on chan H */
#define INTERRUPT_MODEM_G       0x4000 /* Modem control int on chan G */
#define INTERRUPT_MODEM_F       0x2000 /* Modem control int on chan F */
#define INTERRUPT_MODEM_E       0x1000 /* Modem control int on chan E */
#define INTERRUPT_MODEM_D       0x0800 /* Modem control int on chan D */
#define INTERRUPT_MODEM_C       0x0400 /* Modem control int on chan C */
#define INTERRUPT_MODEM_B       0x0200 /* Modem control int on chan B */
#define INTERRUPT_MODEM_A       0x0100 /* Modem control int on chan A */
#define INTERRUPT_NETWORK_H     0x0080 /* Network chan H interrupt */
#define INTERRUPT_NETWORK_G     0x0040 /* Network chan G interrupt */
#define INTERRUPT_NETWORK_F     0x0020 /* Network chan F interrupt */
#define INTERRUPT_NETWORK_E     0x0010 /* Network chan E interrupt */
#define INTERRUPT_NETWORK_D     0x0008 /* Network chan D interrupt */
#define INTERRUPT_NETWORK_C     0x0004 /* Network chan C interrupt */
#define INTERRUPT_NETWORK_B     0x0002 /* Network chan B interrupt */
#define INTERRUPT_NETWORK_A     0x0001 /* Network chan A interrupt */

/*
 * Definitions for Serial Device registers (cristal/cancun only)
 * These registers are used for serial modem control and cable type
 */
#define DEVICE_SERIAL_INT_CLR   0x8000 /* Clear the interrupt */
#define DEVICE_SERIAL_RS232     0x0400 /* RS232 bit */
#define DEVICE_SERIAL_RS449     0x0200 /* RS449 or X.21 bit */
#define DEVICE_SERIAL_DTE       0x0100 /* DTE/DCE bit */
#define DEVICE_SERIAL_TXC_INV   0x0080 /* DTE TXC invert */
#define DEVICE_SERIAL_INT_ENB   0x0040 /* Modem control interrupt enable */
#define DEVICE_SERIAL_LTST      0x0020 /* LTST bit */
#define DEVICE_SERIAL_CTS       0x0010 /* CTS bit */
#define DEVICE_SERIAL_RTS       0x0008 /* RTS bit */
#define DEVICE_SERIAL_DSR       0x0004 /* DSR bit */
#define DEVICE_SERIAL_DTR       0x0002 /* DTR bit */
#define DEVICE_SERIAL_DCD       0x0001 /* DCD bit */

#define HD64570_ALL_INT_CLEAR  ~(DEVICE_SERIAL_INT_CLR | \
                                 DEVICE_SERIAL_INT_ENB | \
                                 DEVICE_SERIAL_LTST    | \
                                 DEVICE_SERIAL_CTS     | \
                                 DEVICE_SERIAL_RTS     | \
                                 DEVICE_SERIAL_DSR     | \
                                 DEVICE_SERIAL_DTR     | \
                                 DEVICE_SERIAL_DCD)

/*
 * Definitions for the Low Speed Serial device registers on the Cobra.
 */
        /* Lo */
#define DEVICE_LSS_DCD		0x0001 /* DCD bit */
#define DEVICE_LSS_LTST		0x0002 /* LTST bit */
#define DEVICE_LSS_DCE_TERM	0x0004 /* DCE terminal timing */
#define DEVICE_LSS_RTS_CTS	0x0008 /* RTS/CTS output bit */
#define DEVICE_LSS_AS		0x0010 /* Sync/async bit */
#define DEVICE_LSS_TXC_INV	0x0080 /* DTE Tx clock invert */
        /* Hi */
#define DEVICE_LSS_DTE		0x0001 /* DTE/DCE bit */
#define DEVICE_LSS_TXCRXC       0x0080 /* TxCout is looped back to RxCin
                                          providing a 1X receive clock */

/*
 * Definitions for the Low Speed Serial Reset/10BaseT register
 */
#define DEVICE_LSS_RESET_MASK	0xFF3F /* Mask for the reset bits */
#define DEVICE_LSS_FDE		0xFFF7 /* FDE feature */
#define DEVICE_LSS_INT_LOOP	0xFFFB /* Internal loopback */
#define DEVICE_LSS_ASEL		0xFFFD /* Automatic AUI/10BaseT select */
#define DEVICE_LSS_AUI		0xFFFE /* User select of AUI/10BaseT */

/*
 * Defintions for Serial NVRAM control register (cristal only)
 */
#define SERIAL_NVRAM_CS         0x08 /* Chip select */
#define SERIAL_NVRAM_SK         0x04 /* Serial clock input */
#define SERIAL_NVRAM_DI         0x02 /* Data input */
#define SERIAL_NVRAM_DO         0x01 /* Data output */



/*
 * Status Register bits:
 *
 * On the Chipcom IGS:
 *  Data bus bit 16 is FLAGSTAT. We read this as bit 0 in our short read.
 *    FLAGSTAT is clear when a mailbox interrupt is pending to the 705 uP.
 *  Data bus bit 17 is MAIL020. We read this as bit 1 in our short read.
 *    MAIL020 is clear when a mailbox interrupt is pending to the 68020
 *    and enabled.
 *  Data bus bit 18 is PC2. We read this as bit 2 in our short read.
 *    PC2 is high when the 705 uP is ready for mailbox data.
 *
 *  Data bus bits 19, 20, and 21 are unused. Bits 3, 4, and 5 in our short.
 *
 * On the cisco, Cabletron, and Chipcom versions of the IGS:
 *  Data bus bit 22 is V.35. We read this as bit 6 of the short.
 *  Data bus bit 23 is RS-232. We read this as bit 7 of the short.
 *  	cable		bit 23	bit 22
 *  	V.35		1	1
 *  	RS-232		0	1
 *  	RS-449 or X.21	1	0
 *	No cable	0	0
 *
 * On all versions of the IGS:
 *  Bit 24 is physical configuration register bit 0
 *  Bit 31 is physical configuration register bit 7
 */

#define STATUS_FLAGSTAT 0x0001	/* Clear when a 705 uP interrupt is pending */
#define STATUS_MAIL020	0x0002	/* Clear when a 68020 interrupt is pending */
#define STATUS_PC2	0x0004	/* High means that 705 uP mailbox TX ready */

#define STATUS_CABLE	   0x00C0  /* V.35 and RS-232. See table above */
#define STATUS_CABLE_SHIFT 6	   /* Number of bits to shift right */
#define STATUS_CABLE_V35   3	   /* V.35 cable attached */
#define STATUS_CABLE_232   1	   /* RS-232C cable attached */
#define STATUS_CABLE_449   2       /* RS-449 or X.21 cable attached */
#define STATUS_CABLE_NONE  0	   /* No cable attached */

#define BRUT_CABLE_MASK_B   0x0E00 /* mask for 3 bits of cable type channel B*/
#define BRUT_CABLE_MASK_C   0x01C0 /* mask for 3 bits of cable type channel C*/
#define BRUT_CABLE_SHIFT_B  9 /* No.bits to shift right for cable type chnl B*/
#define BRUT_CABLE_SHIFT_C  6 /* No.bits to shift right for cable type chnl C*/
#define BRUT_STATUS_CABLE_NONE  7   /* No cable attached */
#define BRUT_STATUS_CABLE_V35   6   /* V.35 cable attached */
#define BRUT_STATUS_CABLE_V24   5   /* V.24 cable attached */
#define BRUT_STATUS_CABLE_449   4   /* RS-449 or X.21 cable attached */
#define BRUT_STATUS_CABLE_422   3   /* EIA530A or EIA422 cable attached */

#define CRISTAL_CABLE_SHIFT	9   	/* Number of bits to shift right */
#define CRISTAL_CABLE_MASK	0x0003	/* Mask for cable bits */
#define CRISTAL_CABLE_NONE	3	/* No cable attached */
#define CRISTAL_CABLE_232	2	/* RS-232 cable attached */
#define CRISTAL_CABLE_449	1	/* RS-449 or X.21 cable attached */
#define CRISTAL_CABLE_V35	0	/* V.35 cable attached */

#define CANCUN_CABLE_NONE	7	/* No cable present on cancun */
#define CANCUN_CABLE_232	6	/* RS-232 cable attached */
#define CANCUN_CABLE_449	5	/* RS-449 cable attached */
#define CANCUN_CABLE_V35	4	/* V.35 cable attached */
#define CANCUN_CABLE_X21	3	/* X.21 cable attached */
#define CANCUN_CABLE_RS530	1	/* RS-530 cable attached */

#define CANCUN_CTS_MASK         0x0800  /* mask for cts bit for cancun hack */
					/* to determine presence of cable */
#define CANCUN_CID2_BIT		0x04	/* CID2 bit location in status */

#define COBRA_CABLE_SHIFT	1	/* Bits to shft rt for Cobra LSS ints */
#define COBRA_CABLE_MASK	0x000e  /* Cobra cable ID bits mask */
/* mask for selecting which hub the BRUT box will talk to */
#define BRUT_HUB_SELECT_MASK 0x80

#define ADRSPC_PCMCIA_BASE      0x08000000 /* PCMCIA Card start address */
/*
 * CANCUN ASIC register (address 0x0211000A)
 */
#define CANCUN_CLOCK_FREQ       0x8000
#define CANCUN_PCMCIA_PRESENT   0x4000
#define CANCUN_BOOT_WIDTH       0x2000
#define CANCUN_FLASH_SIMM_1     0x1000
#define CANCUN_FLASH_SIMM_2     0x0800
#define CANCUN_FLASH_SIMM_SIZE  0x0600
#define CANCUN_FLASH_SIMM_SPEED 0x0100
#define CANCUN_NVRAM_SIZE       0x0080
#define CANCUN_CHA_TYPE         0x0040
#define CANCUN_CHC_TYPE         0x0030
#define CANCUN_DRAM_SIMM        0x0008

/*
 * Definitions for Chipcom mailbox reset register
 */
#define CHIPCOM_MB_RESET	0x0
#define CHIPCOM_MB_UNRESET	0x80

/*
 * definitions for Timer control register:
 */
#define	TCONTROL_G2		0x80	/* Links counters 1 and 2 */
#define	TCONTROL_WE		0x40	/* Watchdog enable */

/*
 * Definition of the High Speed Serial Port Status register.
 * We get a level three interrupt whenever any of CTS, DCD, or DSR change.
 * To clear this interrupt, simply write to an odd address between
 * 0x02130080 and 0x02130083 (note that this will write DTR as well).
 * A read of any address between 0x02130080 and 0x02130083 will return the
 * Serial Port status register. Note that all of these bits are low true!
 */

/*
 * Reading from ADRSPC_SERIAL_STATE will return the following line status bits:
 */
#define SERIAL_CTS	0x80		/* Read state of *CTS line */
#define SERIAL_DCD	0x40		/* Read state of *DCD line */
#define SERIAL_DSR	0x20		/* Read state of *DSR line */
#define SERIAL_DTR	0x10		/* Read/Write state of *DTR line */

/*
 * Definition of ADRSPC_LSS_MISC register.  Describes options associated with
 * the Copan variant of Cancun.
 */
#define LSS_DAUGHTER	0x01		/* Daughter card installed */
#define LSS_CHIPVER	0x06		/* Which chip type installed: */
#define LSS_2402	0x00		/*  Cirrus CD2402 downloadable */
#define LSS_2400	0x02		/*  Cirrus CD2400 async DMA */
#define LSS_2430	0x04		/*  Cirrus CD2430 async+PPP DMA */
#define LSS_CLK		0x18		/* Clock applied to UARTs */
#define LSS_CLK30	0x00		/*  30 Mhz */
#define LSS_CLK27	0x08		/*  27 Mhz */
#define LSS_CLK25	0x10		/*  25 Mhz */
#define LSS_CLK20	0x18		/*  20 Mhz */
#define LSS_CBMAIN	0x20		/* Cable applied on main board */
#define LSS_CBDAUGHT	0x40		/*  ...on daughter card */

/*
 * Definition of Synoptics LED register.
 * Low true, so write a zero to turn on the LED:
 */
#define SYNOPTICS_LED_STANDBY   0x80	
#define SYNOPTICS_LED_BRIDGE	0x40
#define SYNOPTICS_LED_BOOT	0x20
#define SYNOPTICS_LED_DS1       0x10
#define SYNOPTICS_LED_DS2	0x08
#define SYNOPTICS_LED_DS3	0x04
#define SYNOPTICS_LED_DS4	0x02
#define SYNOPTICS_LED_DS5	0x01

/*
 * Due to the fact the LED number scheme basically works backwards
 * on the SynOptics IGS, this macro table makes the code a little
 * mode readable and the formatting shows the LED pattern produced
 */
#define SYNOPTICS_LED_STEP_1                                0x10
#define SYNOPTICS_LED_STEP_2                         0x08 
#define SYNOPTICS_LED_STEP_3                         0x08 | 0x10
#define SYNOPTICS_LED_STEP_4                  0x04
#define SYNOPTICS_LED_STEP_5                  0x04 |        0x10
#define SYNOPTICS_LED_STEP_6                  0x04 | 0x08 
#define SYNOPTICS_LED_STEP_7                  0x04 | 0x08 | 0x10
#define SYNOPTICS_LED_STEP_8           0x02 
#define SYNOPTICS_LED_STEP_9           0x02 |               0x10
#define SYNOPTICS_LED_STEP_10          0x02 |        0x08 
#define SYNOPTICS_LED_STEP_11          0x02 |        0x08 | 0x10
#define SYNOPTICS_LED_STEP_12          0x02 | 0x04 
#define SYNOPTICS_LED_STEP_13          0x02 | 0x04 |        0x10
#define SYNOPTICS_LED_STEP_14          0x02 | 0x04 | 0x08 
#define SYNOPTICS_LED_STEP_15          0x02 | 0x04 | 0x08 | 0x10
#define SYNOPTICS_LED_STEP_16   0x01 
#define SYNOPTICS_LED_STEP_17   0x01 |                      0x10
#define SYNOPTICS_LED_STEP_18   0x01 |               0x08 
#define SYNOPTICS_LED_STEP_19   0x01 |               0x08 | 0x10
#define SYNOPTICS_LED_STEP_20   0x01 |        0x04 
#define SYNOPTICS_LED_STEP_21   0x01 |        0x04 |        0x10
#define SYNOPTICS_LED_STEP_22   0x01 |        0x04 | 0x08 
#define SYNOPTICS_LED_STEP_23   0x01 |        0x04 | 0x08 | 0x10

#define SYNOPTICS_LED_ALL       0xFF

#define SYNOPTICS_LED_BOOT_MASK (SYNOPTICS_LED_BOOT | \
				 SYNOPTICS_LED_DS5  | \
				 SYNOPTICS_LED_DS4)

/*
 * Macro to do the led control for you
 *
 * The *^&%*&%ing register is write-only and the value is
 * completely random on a read. The easiest way to make the
 * code compatible between the 4000 and the IGS/3000 is to
 * shadow the value in syn_state and only ever do writes
 * to the led register. Makes the code cleaner.
 */
#define SET_SYNOPTICS_LEDS(address, mask)     \
{                                             \
    syn_state &= (uchar)~(mask);              \
    *(volatile uchar *)(address) = syn_state; \
}

#define CLR_SYNOPTICS_LEDS(address, mask)     \
{                                             \
    syn_state |= (uchar)(mask);               \
    *(volatile uchar *)(address) = syn_state; \
}
/*
 * NORMAL_COOKIE_PROM_PRESENT returns true if the system 
 * actually has a COOKIE PROM.  THIS routine returns
 * false for a serial cookie prom
 */
#define NORMAL_COOKIE_PROM_PRESENT \
    (*(uchar *)cookie_boot != NO_COOKIE_PROM && \
    *(uchar *)cookie_boot != NO_COOKIE_PROM_CANCUN)

#define CANCUN_BPREG (*((ulong *)(0x2110010))) /* break point addr reg */
#define CANCUN_BPMSK (*((ulong *)(0x2110014))) /* break point mask reg */
#define CANCUN_BPFMSK (*((uchar *)(0x2110018))) /* break point fn mask reg */
#define CANCUN_BP_SUPVSR	0x80
#define CANCUN_BP_PRG		0x40
#define CANCUN_BP_DATA		0x20
#define CANCUN_BP_USER		0x00
#define CANCUN_BP_RD		0x10
#define CANCUN_BP_WR		0x00
#define CANCUN_BP_FMASK		0xf0
#define CANCUN_BP_DONTCARE_RDWR	0x01
#define CANCUN_BP_DONTCARE_USER 0x00
#define CANCUN_BP_DONTCARE_DATA 0x02
#define CANCUN_BP_DONTCARE_PRG	0x04
#define CANCUN_BP_DONTCARE_SUPUSR	0x08
#define CANCUN_BPCTL (*((uchar *)(0x2110019))) /* break point ctl reg */
#define CANCUN_BP_ENABLE	0x1
#define CANCUN_BP_DISABLE	0x0

/* used for writing CONFIG EEPROM on cancun */

#define NMI_VECTOR		31

/* User defined vectors */
#define GAL_ETHER_HUB_VECTOR       64 /* Galapagos Ether Hub Vector */
#define BOA_CSU_DSU_VECTOR         65

/* #define NVBASE          ADRSPC_NVRAM_START */

/**************************** Fiji definitions ***********************/
/*
 * Control cum status register for Fiji (Cancun derivative with PC ISA
 * bus interface).
 * One bit, set by the PC, indicates the SHRAM initialization state
 * when read. The same bit is used to interrupt the PC when written
 * to.
 * The second bit indicates whether the daughter card for 2nd serial
 * port and the BRI is actually present. It's been provided to support
 * field upgrades of the daughter board without having to change the
 * cookie value.
 * Register address is ADRSPC_FIJI_CONTROL (0x2110008)
 */
#define FIJI_SHRAM_STATUS_FLAG  0x8000  /* Shared RAM initialization state */
#define FIJI_DAUGHTER_CARD_FLAG 0x4000  /* Daughter card presence flag */
#define FIJI_PC_INTERRUPT       0x8000  /* PC interrupt generation bit (write)*/
#define FIJI_BIT_MASK           0xC000  /* Only uppermost 2 bits in use */

/*
 * The FIJI_DAUGHTER_CARD_FLAG will be 0 if card is present.
 */
#define FIJI_D_CARD_PRESENT     ((*(ushort *)ADRSPC_FIJI_CONTROL \
                                 & FIJI_DAUGHTER_CARD_FLAG) == 0)

/****************************** Boa definitions ***********************/
/*       Boa = 2524/2525 Integrated CSU/DSU in Serial ports           */

/*
 * Boa Serial Miscellanous Register definitions
 * This register is used to control various aspects of the CSU/DSU
 * including module interrupt masking and resets
 */
#define BOA_MISC_DSU_HW_RST_MASK    0x80
#define BOA_MISC_SEC_CHAN_ENABLE    0x40
#define BOA_MISC_WAN_MOD_ID3        0x20
#define BOA_MISC_CSU_DSU_RESET      0x10
#define BOA_MISC_READY_MASK         0x08
#define BOA_MISC_ALARM_MASK         0x04
#define BOA_MISC_READY_STATUS       0x02
#define BOA_MISC_ALARM_STATUS       0x01
/*
 * CSU/DSU ALARM and READY interrupt enable/disables
 */
#define BOA_ENABLE_READY_INTERRUPT(a)       (*a &= ~BOA_MISC_READY_MASK)
#define BOA_MASK_READY_INTERRUPT(a)         (*a |= BOA_MISC_READY_MASK)
#define BOA_ENABLE_ALM_INTERRUPT(a)         (*a &= ~BOA_MISC_ALARM_MASK)
#define BOA_MASK_ALM_INTERRUPT(a)           (*a |= BOA_MISC_ALARM_MASK)
#define BOA_ENABLE_ALM_RDY_INTERRUPT(a)     (*a &= ~(BOA_MISC_ALARM_MASK | BOA_MISC_READY_MASK))
#define BOA_MASK_ALM_RDY_INTERRUPT(a)       (*a |= (BOA_MISC_ALARM_MASK | BOA_MISC_READY_MASK))

#define BOA_ALM_STATUS_BIT              0
#define BOA_ALM_MASK_BIT                2
#define BOA_RDY_STATUS_BIT              1
#define BOA_RDY_MASK_BIT                3
/*
 * Boa BRI Miscellanous Register definitions
 * This register is used for determining if a BRI (NT1) module is present
 * in the slot
 */
#define BOA_BRI_MOD_ID_MASK     0x03
#define BOA_BRI_MOD_ID_BRI_NT1  0x00
#define BOA_BRI_MOD_ID_BRI_ST   0x01
#define BOA_BRI_MOD_ID_NONE     0x03

#define BOA_BRI_NT1_PRESENT     (((*(uchar *)ADRSPC_BOA_BRI_MISC) & BOA_BRI_MOD_ID_MASK) == \
                                 BOA_BRI_MOD_ID_BRI_NT1)
#define BOA_BRI_ST_PRESENT      (((*(uchar *)ADRSPC_BOA_BRI_MISC) & BOA_BRI_MOD_ID_MASK) == \
                                 BOA_BRI_MOD_ID_BRI_ST)
#define BOA_BRI_ANY_MODULES_PRESENT     (BOA_BRI_NT1_PRESENT || BOA_BRI_ST_PRESENT)

