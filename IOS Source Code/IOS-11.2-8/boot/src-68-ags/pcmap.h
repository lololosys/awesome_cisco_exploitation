/* $Id: pcmap.h,v 3.1 1995/11/09 09:08:38 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/pcmap.h,v $
 *------------------------------------------------------------------
 * pcmap.h -- memory mapping definitions
 *
 * September 1987, Greg Satz
 *
 * Copyright (c) 1986-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcmap.h,v $
 * Revision 3.1  1995/11/09  09:08:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.3  1995/03/21 03:37:41  mdb
 * CSCdi31229:  unterminated comment
 *
 * Revision 1.2  1994/10/30  21:59:01  abaindur
 * CSCdi25240:  Need to support dfb on low end systems
 * Flash Load Helper and Dual Flash Bank port.
 *
 * Revision 1.1  1993/09/24  16:33:11  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/08/19  03:51:13  hampton
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
 * Revision 1.1  1992/03/07  23:56:28  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Page map constants/macros
 */
#define RAMPAGES	0x1000		/* Up to 4096 pages for 4 MB RAM */
#define PAGESIZE	0x400		/* 1024. bytes per page */
#define PAGESHIFT	10		/* LOG2(PAGESIZE) */

/*
 * Definition of flag bits, written into the ROM monitor image by the
 * ROM image creation program, /csc/tools/bytek.c:
 */
/* Bits 0 - 3 are ROM size bits */
#define FLAGS_ROMSIZE_512K      1
#define FLAGS_ROMSIZE_1M	2
#define FLAGS_ROMSIZE_2M	3
#define FLAGS_ROMSIZE_4M	4
#define FLAGS_ROMSIZE_8M	5

/*
 * Address space allocations
 */
#define	ADRSPC_RAM	  0x0		/* Read/Write, segmented memory */
#define ADRSPC_PROM0	  0x1000000	/* ROM #0, boot space */
#define ADRSPC_ROMSIZE    0x100000f     /* ROM size code; from bytek.c */
#define ADRSPC_PROM1_1MB  0x1080000	/* ROM #1 with 1Mb EPROMS */
#define ADRSPC_PROM1_2MB  0x1100000	/* ROM #1 with 2Mb EPROMS */
#define ADRSPC_PROM1_4MB  0x1200000	/* ROM #1 with 4Mb EPROMS */
#define ADRSPC_PROM1_8MB  0x1400000	/* ROM #1 with 8Mb EPROMS */
#define	ADRSPC_MULTI	  0x2000000	/* Multibus */
#define	ADRSPC_MULTIIO	  0x2100000	/* Multibus I/O */
#define	ADRSPC_CONTROL	  0x2110000	/* System control register */
#define	ADRSPC_MBLBASE	  0x2110040	/* Multibus local base register */
#define	ADRSPC_MBMBASE	  0x2110080	/* Multibus base register */
#define	ADRSPC_MBSIZE	  0x21100C0	/* Multibus window size */
#define ADRSPC_STATUS	  0x2110100	/* System status register */
#define ADRSPC_TCONTROL	  0x2120040	/* Timer control register */

#define ADRSPC_CODE_START LOWCORE1      /* start of RAM for code */

#define	CONTROL_PARITY0		0x8000	/* Parity of ram byte 0 */
#define	CONTROL_PARITY1		0x4000	/* Parity of ram byte 1 */
#define	CONTROL_PARITY2		0x2000	/* Parity of ram byte 2 */
#define	CONTROL_PARITY3		0x1000	/* Parity of ram byte 3 */
#define	CONTROL_PARITY		0x0800	/* Parity error */
#define	CONTROL_MBCOL		0x0400	/* Multibus collision */
#define	CONTROL_MBTO		0x0200	/* Multibus timeout */
#define	CONTROL_LOCTO		0x0100	/* Local transaction timeout */
#define	CONTROL_SLAVEPAR	0x0080	/* Slave transaction parity */
#define	CONTROL_MASTER		0x0040	/* Master mode */
#define	CONTROL_RUNLAMP		0x0010	/* Run light */
#define	CONTROL_EVENPAR		0x0008	/* Even Parity */
#define	CONTROL_LAMP		0x0004	/* Lights */
#define	CONTROL_MBRESET		0x0002	/* Multibus reset */
#define	CONTROL_BOOT		0x0001	/* Boot state */

#define	TCONTROL_G2		0x80	/* Links counters 1 and 2 */
#define	TCONTROL_WE		0x40	/* Watchdog enable */
#define	TCONTROL_S1		0x38	/* source for counter 1 */
#define	TCONTROL_S0		0x07	/* source for counter 0 */

#define	TCSRC_USEC		0	/* 1 microsecond */
#define	TCSRC_CNT2OUT		1	/* nmi interrupt */
#define	TCSRC_RXCLKA		2	/* rx clock channel A */
#define	TCSRC_TXCLKA		3	/* tx clock channel A */
#define	TCSRC_RXCLKB		4	/* rx clock channel B */
#define	TCSRC_TXCLKB		5	/* tx clock channel B */
#define	TCSRC_NOCLK1		6	/* no clock generated */
#define	TCSRC_NOCLK2		7	/* no clock generated */

/*
 * Multibus memory mapping
 *
 * By convention, MultiBus memory is mapped just below 
 * Multibus I/O in the mappable address space.
 */

#define	MBMEMLOWPAGE	0x8000	/* first MultiBus memory page */
#define	MBMEMLOWADR	0x2000000 /* MBMEMLOWPAGE << PAGESHIFT */
#define	NUMMBMEMPAGES	0x400	/* number of MultiBus memory pages */

/*
 * Multibus I/O mapping
 *
 * By convention, MultiBus I/O is mapped at the top of the
 * mappable address space.
 */

#define	MBIOLOWPAGE	0x8400		/* first MultiBus page */
#define	MBIOLOWADR	0x2100000	/* IOLOWPAGE << PAGESHIFT */
#define	MBIOHIADR (MBMEMLOWADR+((NUMMBIOPAGES+NUMMBMEMPAGES)<<PAGESHIFT)-1)
#define	NUMMBIOPAGES	0x40		/* number of MultiBus pages */

/*
 * Cache control register bits for the 68020/68030:
 */
#define ENA_ICACHE	0x0001
#define CLR_ICACHE	0x0008
#define ENA_DCACHE	0x0100	/* Ignored on 68020 */
#define CLR_DCACHE	0x0800	/* Ignored on 68020 */
#define WRITE_ALLOCATE	0x2000	/* Ignored on 68020 */

#define CACHE_I		0x0001
#define CACHE_D		0x2100
#define ENABLE_CACHES	0x2101
#define CLEAR_CACHES	0x0808
#define CLR_DC_EIC      0x0801

/*
 * Cache control register bits for the 68040:
 */
#define ICACHE_040      0x00008000      /* 68040 instruction cache */
#define DCACHE_040      0x80000000      /* 68040 data cache */
#define BOTH_CACHE_040  0x80008000      /* 68040 data & inst. cache */
