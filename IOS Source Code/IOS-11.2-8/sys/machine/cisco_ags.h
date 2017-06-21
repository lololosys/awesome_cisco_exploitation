/* $Id: cisco_ags.h,v 3.6.10.3 1996/06/04 22:55:03 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_ags.h,v $
 *------------------------------------------------------------------
 * cisco_ags.h - Definitions for the CSC2/3/4 processors
 *
 * June 1987, Kirk Lougheed
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_ags.h,v $
 * Revision 3.6.10.3  1996/06/04  22:55:03  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/05/17  11:33:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.1  1996/05/02  22:05:47  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.10.1  1996/05/09  14:34:18  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.36.1  1996/04/27  07:15:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.26.1  1996/04/08  01:59:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.16.1  1996/03/22  09:40:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/23  17:33:53  hampton
 * Remove unused define for "MULTIBUS".  [CSCdi48844]
 *
 * Revision 3.5.8.1  1996/03/03  21:24:09  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/03/02  01:20:14  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/02  04:29:13  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.4.18.1  1996/02/29  07:49:30  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/23  13:10:03  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/21  16:29:06  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:49:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.50.1  1996/02/03  07:07:46  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2.34.1  1995/09/20  03:29:37  rlowe
 * Allow, but do not require, system timer interrupt vector to be
 * different from NMI vector.  For 68k-related platforms.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.2  1995/06/28  03:06:54  rmontino
 * CSCdi36503:  AGS zipped images
 *
 * Revision 2.1  1995/06/07  21:42:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * The AGS uses a.out images
 */
#include "image_aout.h"


/*
 * Current Multibus memory space assignment by supported devices.
 *
 *  memory card            00000         10000     ( 64 Kbytes)
 *
 *    MLAN-T       0       10000         20000     (128 Kbytes)
 *    MLAN-T       1       30000         20000     (128 Kbytes)
 *    MLAN-T       2       50000         20000     (128 Kbytes)
 *    MLAN-T       3       70000         20000     (128 Kbytes)
 *    MLAN-T       4       90000         20000     (128 Kbytes)
 *
 *    NVRAM        0       B0000         10000     ( 64 Kbytes)
 *
 *    MLAN-T       5       C0000         20000     (128 Kbytes)
 *  / MLAN-T       6       E0000         20000     (128 Kbytes) \
 * {  MLAN-E       0       E0000         10000     ( 64 Kbytes)  } shared
 *  \ MLAN-E       1       F0000         10000     ( 64 Kbytes) /
 *
 * MLAN-T <--> CSC-R16.  The MLAN-2T, aka CSC-1R/2R, occupies
 * the same memory locations as the MLAN-T card.
 *
 * Older Multibus memory space assignment by supported devices.
 *
 *  memory card            00000         10000     ( 64 Kbytes)
 *
 *         STP     0       40000           100     (.25 kbytes)
 *                 1       40100
 *                 2       40200
 *                 3       40300
 *                 4       40400
 *                 5       40500
 *                 6       40600
 *
 *   Interlan      0       42000          2000     (  8 kbytes)
 *                 1       44000
 *                 2       46000
 *                 3       48000
 *                 4       4A000
 *                 5       4B000
 *                 6       4C000
 *
 *                 (unused  4D000 - 7FFFF, 204 Kbytes)
 *
 *    SBE(obsolete)0       80000         10000     ( 64 Kbytes)
 *                 1       90000
 *                 2       A0000
 *
 *    NVRAM        0       B0000         10000     ( 64 Kbytes)
 *
 *                 (unused  C0000 - EFFFF, 192 Kbytes)
 *
 *                 (Note well that the CSC1 uses the last 64 Kbytes of
 *                  Multibus memory address space for the I/O space.
 *                  Use this chunk of memory only for devices that the
 *                  CSC1 doesn't support.)
 *
 * Current Multibus I/O space assignment by supported devices.
 *
 *         EM(EC)  0       0000             2      environment monitor
 *
 *           STR   0       0010             2      CSC-1R/2R mailbox
 *      (obsolete) 1       0012             2
 *                 2       0014             2
 *                 3       0016             2
 *                 4       0018             2
 *                 5       001A             2
 *                 6       001C             2
 *                 7       001E             2
 *
 *        MLAN-E   0       0090             2      SBE Ethernet mailbox
 *                 1       0092             2
 *
 *        MLAN-T   0       0098             2      SBE Token Ring mailbox
 *                 1       009A             2
 *
 *         TRA4    0       00A0             4      token ring adapter
 *                 1       00A4             4
 *                 2       00A8             4
 *                 3       00AC             4
 *
 *         SBE16   0       00B0             2      SBE-16-TR mailbox
 *                 1       00B2             2
 *                 2       00B4             2
 *                 3       00B6             2
 *                 4       00B8             2
 *                 5       00BA             2
 *                 6       00BC             2
 *                 7       00BE             2
 *
 *         MLP     0       00C0            20
 *                 1       00E0
 *
 *         3MB     0       0100           100
 *                 1       0200
 *                 2       0300
 *                 3       0400
 *
 *   Interlan      0       0500            10
 *                 1       0510
 *                 2       0520
 *                 3       0530
 *                 4       0540
 *                 5       0550
 *                 6       0560
 *
 *                 (unused 0570 - 05FF)
 *
 *         ACC     0       0600            100
 *                 1       0700
 *                 2       0800
 *                 3       0900
 *
 *         HUB     0       0A00            100    (256 bytes)
 *                 1       0B00
 *
 *                 (unused 0C00 - 1FFF, 5120 bytes)
 *
 *         3COM    0       2000           2000
 *                 1       4000
 *                 2       6000
 *                 3       8000
 *                 4       A000
 *
 *         MCI     0       C000             40
 *                 1       C040
 *                 2       C080
 *                 3       C0C0
 *                 4       C100
 *                 5       C140
 *                 6       C180
 *                 7       C1C0            (total of 16. possible units)
 *
 *                 (unused D000 - FFFF)
 */

/*
 * Interrupt level assignments for system software
 *
 *	1 - Serial Interfaces, HUB interfaces
 *	2 - 1822 card, STP serial lines
 *	3 - MCI, 3MB Ethernet, parallel printer (IGS serial delta int)
 *	4 - Ethernet, TokenRing
 *	5 - Console UART
 *	6 - Chipcom IGS mailbox interrupt
 */

#define LEVEL_CONSOLE 5		/* console UART */
#define ETHER_DISABLE 4		/* priority level to shut off Ethernet */
#define ETHER_INTLEVEL 4	/* another name */
#define LEVEL_3MB 3		/* 3MB Ethernet */
#define LEVEL_LPT 3		/* priority level to shut off printer */
#define LEVEL_ACC 2		/* ACC 1822 */
#define LEVEL_SBE 1		/* SBE serial interface */
#define	LEVEL_HUB 1		/* HUB network interface */

#define	TIMER_VECTOR	NMIVector

#define WASTETIME(usecs) wastetime(usecs)
#define DELAY(ms)        msecdelay(ms)

/*
 * Base of memory
 */

#define RAMBASE	0x00000000	/* Base of RAM */
#define TRAP_PAGE_ZERO		/* Catch writes to addresses 0x0 - 0xFF */

/*
 * Define the address of the pointer to the crash info data structure.
 * This pointer (and data structure) are used by GDB as a starting point
 * for analyzing a crash dump.  This location does not need to survive a
 * reload, it just needs to appear in the crash dump.  If you change
 * this value, you must coordinate with Cisco's GDB maintainers.
 */
#define CRASH_INFO (*((crashinfo **)(RAMBASE + 0xFFC)))

#define REFRESHTIME          4          /* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR   250        /* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR 1	        /* Denominator of NMI frequency */

#define	UARTIRQ1  0x40	/* UART Channel A Receiver ready */
#define	UARTIRQ2  0x41	/* UART Channel A Transmitter ready */
#define	UARTIRQ3  0x42	/* UART Channel A Status change */
#define	UARTIRQ4  0x43	/* UART Channel A Counter/TImer event */
#define	UARTIRQ5  0x44	/* UART Channel B Receiver ready */
#define	UARTIRQ6  0x45	/* UART Channel B Transmitter ready */
#define	UARTIRQ7  0x46	/* UART Channel B status change */
#define UARTIRQ8  0x47  /* UART Channel B Counter/Timer event */

#define MULTIMEMBASE 0x02000000L	/* base of Multibus memory space */
#define MULTIIOBASE  0x02100000L	/* base of Multibus I/O space */
#define ROMFREEBASE  0x1000000L		/* base of rom address space */

#define	UARTDEVBASEADDR	0x02120100

#define NVBASE	     (MULTIMEMBASE+0xB0000)

#define NVMAXBYTES   0x10000		/* Maximum supported NVRAM */

/*
 * Required by hes/slot.h and used to be defined in slot.h, but has
 * now been moved to platform specific .h files so that the RSP can
 * have a different value.
 */
#define MAX_SLOTS 16

#define MAX_CTRLR_INTERFACES  CBUS2_MAX_CTRLR_INTERFACES
#define MAX_CBUS_INTERFACES   CBUS2_MAX_INTERFACES


/* 
 * Max possible lines for AGS and ASM
 */
#define PLATFORM_MAXLINES 120

/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE  6	    /* disable all interrupts */
#define PLATFORM_TTY_DISABLE  5	    /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE 4	    /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE   0	    /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_CSC2

/*
 * No new network buffers if coreleft is below this threshold.
 */
#define BUFFERTHRESHOLD		(btow(256 * 1024))

#ifndef ASMINCLUDE
/*
 * support protected writes to dbus registers
 * 
 * stub definitions to allow code shared w/7000 to compile
 */
typedef uchar dbus_protect_t;

/*
 * AGS does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * AGS never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * AGS never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

#endif

#define dbus_protect(statep)	(*(statep) = 0)
#define dbus_restore(statep)
