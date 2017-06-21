/* $Id: as5200_pcmap.h,v 1.1.62.1 1996/06/16 21:19:38 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_pcmap.h,v $
 *------------------------------------------------------------------
 * as5200_pcmap.h -- memory mapping definitions
 *
 * December 1995, Joe Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_pcmap.h,v $
 * Revision 1.1.62.1  1996/06/16  21:19:38  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/20  05:55:23  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Brasil specific register definitions.
 */
#define ADRSPC_BRASIL_MB_CONFIG_REG	0x02135004
#define ADRSPC_BRASIL_MB_CTRL1_REG	0x02135006
#define ADRSPC_BRASIL_MB_STATUS_REG	0x02135008
#define ADRSPC_BRASIL_ISR1_REG		0x0213500A
#define ADRSPC_BRASIL_ISR2_REG		0x0213500C
#define ADRSPC_BRASIL_MB_CTRL2_REG	0x0213500E
#define ADRSPC_BRASIL_MB_CTRL3_REG	0x02135010
#define ADRSPC_BRASIL_MB_CTRL4_REG	0x02135012
#define ADRSPC_BRASIL_MB_IRQ_VECTOR	0x02135014
#define ADRSPC_BRASIL_MB_TDM_REG	0x02135100
#define ADRSPC_BRASIL_BASE_SLOT		0x02138000
#define ADRSPC_BRASIL_TDM_PLL		0x02135182

/*
 * Slot reset definitions.
 */
#define RESET_SLOT0		0x0001 /* Reset bit for Slot 0 */
#define RESET_SLOT1		0x0002 /* Reset bit for Slot 1 */
#define RESET_SLOT2		0x0004 /* Reset bit for Slot 2 */
#define ALL_SLOTS_RESET		(RESET_SLOT0 | RESET_SLOT1 | RESET_SLOT2)

/*
 * Bad address for monitor bus error test
 */
#define ADRSPC_BAD_ADDR                 0xFFFFFFFF /* Bad addr for berrtest */
 
/*
 * Flash address definitions
 */
#define ADRSPC_BOOT_FLASH_BASE          0x03000000
#define ADRSPC_BRASIL_FLASH_BASE	0x22000000
 
/*
 * Brasil Packet memory location
 */
#define ADRSPC_BRASIL_PKT_MEM   0x20000000
 
/*
 * Brasil counter port pointers
 */
#define COUNTER0_PORT	*(unsigned short *)0x02120050 /* Timer register 0 */
#define COUNTER1_PORT	*(unsigned short *)0x02120060 /* Timer register 1 */
#define COUNTER2_PORT	*(unsigned short *)0x02120070 /* Timer register 2 */

/*
 * The Brasil monitor's stack location
 */
#define MONITOR_FREE_MEM_BASE   (ADRSPC_RAM+0x5000)
/*                                                  
 * One long word is reserved at the end of the ROM Monitor BSS segment
 * for the crash info ptr planted by the system image at startup. NOTE:
 * the ROM Monitor stack grows downward.
 *
 * Modifying the monitor stack location will have
 * serious consequences on GDB's ability to locate 
 * the CRASH_INFO pointer.  GDB_68k expects the CRASH_INFO 
 * pointer at one of the following addresses: 0x4ffc, 0xffc, and 
 * 0x400.  For backward compatibility brasil is using 0x4ffc
 * which is the top of stack.  
 */
#define MONITOR_STACK           (MONITOR_FREE_MEM_BASE - 4)

/*
 * Packet memory sizing defines.
 * Used for configuring the motherboard control reg 1
 */
#define PKT_MEM_SIXTEENMEGS		0x00
#define PKT_MEM_EIGHTMEGS		0x03
#define PKT_MEM_FOURMEGS		0x01
#define PKT_MEM_TWOMEGS			0x02
#define PKT_MEM_SHIFT			6
#define PKT_MEM_MASK			0xC0

