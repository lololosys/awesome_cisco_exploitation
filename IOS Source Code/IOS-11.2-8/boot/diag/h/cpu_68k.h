/* $Id: cpu_68k.h,v 3.3.24.1 1996/06/11 22:42:45 pst Exp $
 * $Source: /release/112/cvs/Xboot/diag/h/cpu_68k.h,v $
 *------------------------------------------------------------------
 * cpu_68k.h -- Definitions specific to the 68K family of CPU's
 *
 * June 1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpu_68k.h,v $
 * Revision 3.3.24.1  1996/06/11  22:42:45  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/22  08:59:13  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  07:42:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CPU_68K 1
#define BIGENDIAN 1


/*
 * Type Definitions  -- note that these are really compiler specific 
 */
#define ushort unsigned short
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define u_short unsigned short
#define u_char unsigned char
#define u_int unsigned int
#define u_long unsigned long

typedef int forktype;
typedef long leveltype;
typedef ulong ipaddrtype;
typedef uchar *mempointer;
typedef long jmp_buf[13];
typedef int boolean;

#define	STACKLONG	1	/* size in ints of long word on stack */
#define	STACKSHORT	1	/* size in ints of short word on stack */

/*
 * Define the NULL pointer
 */
#define NULL ((void *) 0)

/*
 * Current Multibus memory space assignment by supported devices.
 *
 *  memory card		00000	      40000	(256. Kbytes)
 *
 *	STP 	0	40000		100	(.25 kbytes)
 *	 	1	40100
 *	 	2	40200
 *	 	3	40300
 *	 	4	40400
 *	 	5	40500
 *	 	6	40600
 *
 *   Interlan	0	42000	       2000	(8. kbytes)
 *		1	44000
 *		2	46000
 *		3	48000
 *		4	4A000
 *		5	4B000
 *		6	4C000
 *
 *		(unused  4D000 - 7FFFF, 204 Kbytes)
 *
 *    SBE	0	80000	        10000	(64. Kbytes)
 *		1	90000
 *		2	A0000
 *
 *    NVRAM	0	B0000		10000	(64. Kbytes)
 *
 *    HUB	0	C0000		1000	(4.  Kbytes)
 *		1	C1000
 *
 *		(unused  C2000 - EFFFF, 184 Kbytes)
 *
 *    		(Note well that the CSC1 uses the last 64 Kbytes of
 *		 Multibus memory address space for the I/O space.
 *		 Use this chunk of memory only for devices that the
 *		 CSC1 doesn't support.)
 *
 * Current Multibus I/O space assignment by supported devices.
 *
 *		(unused 0000 - 009F, 160. bytes)
 *
 * 	TRA	0	00A0		 4	token ring adapter
 * 		1	00A4		 4
 * 		2	00A8		 4
 * 		3	00AC		 4
 *
 * 		(unused 00B0 - 00BF, 16 bytes)
 * 
 *	MLP	0	00C0		20
 *		1	00E0
 *
 *	3MB	0	0100		100
 *		1	0200
 *		2	0300
 *		3	0400
 *
 *   Interlan	0	0500		  10
 *		1	0510
 *		2	0520
 *		3	0530
 *		4	0540
 *		5	0550
 *		6	0560
 *
 *		(unused 0570 - 05FF)
 *
 *	ACC	0	0600		 100
 *		1	0700
 *		2	0800
 *		3	0900
 *
 *		(unused 0A00 - 1FFF, 5632. bytes)
 *
 *	3COM	0	2000		2000
 *		1	4000
 *		2	6000
 *		3	8000
 *		4	A000
 *
 *	MCI	0	C000		40
 *		1	C040
 *		2	C080
 *		3	C0C0
 *		4	C100
 *		5	C140
 *		6	C180
 *		7	C1C0		(total of 16. possible units)
 *
 *		(unused D000 - FFFF)
 */

/*
 * Trap locations
 */
#define	IRQ1Vect	*(int*)0x64
#define	IRQ2Vect	*(int*)0x68
#define IRQ3Vect	*(int*)0x6C
#define IRQ4Vect	*(int*)0x70
#define IRQ5Vect	*(int*)0x74

/*
 * Interrupt level assignments for system software
 *
 *	1 - Serial Interfaces, HUB interfaces
 *	2 - 1822 card, STP serial lines
 *	3 - MCI, 3MB Ethernet, parallel printer
 *	4 - Ethernet, TokenRing
 *	5 - Console UART
 */
#define LEVEL_CONSOLE 5		/* console UART */
#define TTY_DISABLE 5		/* priority to shut off all types of TTY */
#define NETS_DISABLE 4		/* highest priority of a network device */
#define ETHER_DISABLE 4		/* priority level to shut off Ethernet */
#define ETHER_INTLEVEL 4	/* another name */
#define LEVEL_3MB 3		/* 3MB Ethernet */
#define LEVEL_LPT 3		/* priority level to shut off printer */
#define LEVEL_ACC 2		/* ACC 1822 */
#define LEVEL_SBE 1		/* SBE serial interface */
#define	LEVEL_HUB 1		/* HUB network interface */

#define ALL_ENABLE 0		/* enable all interrupts */
#define ALL_DISABLE 5		/* disable all interrupts */

#define	LEVEL1	1
#define	LEVEL2	2
#define	LEVEL3	3
#define	LEVEL4	4
#define	LEVEL5	5

/*
 * Configuration Register Bits
 */
extern ushort configregister;

#define CFG_DIAGNOSTIC  0x8000		/* diagnostic mode */
#define CFG_SUBNET	0x4000
#define CFG_LANCONFIG	0x2000
#define CFG_ZEROS	0x0400
#define CFG_FILE	0x000F

/*
 * adjust pointers for long bcopy's
 */
#define adjustpointer(a) a

/*
 * Lines start autobaud detect at this speed
 */
#define AUTOSPEED 2400

/*
 * Largest amount of NV memory
 */
#define NVMAXBYTES   0x10000
