/* $Id: defs_csc2.h,v 3.1 1995/11/09 09:05:20 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/defs_csc2.h,v $
 *------------------------------------------------------------------
 * defs_csc2.h -- Definitions specific to the CSC2 processor
 *
 * June 1987, 1988, Kirk Lougheed
 *
 * Copyright (c) 1987-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs_csc2.h,v $
 * Revision 3.1  1995/11/09  09:05:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:38  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cpu_68k.h"

extern ulong *clockptr;
#define msclock *clockptr
#define fastclock msclock

#define	CYCLETIME	12000000L	/* 12mhz */
#define	WAITCYCLES	12		/* number of cycles in delay loop */

#define	PROCTYPE CPU_CSC2

#define ONESEC (1000L)			/* tics per second */
#define ONEMIN (60L*ONESEC)		/* tics per minute */
#define ONEHOUR (60L*ONEMIN)		/* tics per hour */

#define	UARTIRQVect1	*(int *)0x100	/* UART Interrupt vector address */
#define	UARTIRQVect2	*(int *)0x104	/* UART Interrupt vector address */
#define	UARTIRQVect3	*(int *)0x108	/* UART Interrupt vector address */
#define	UARTIRQVect4	*(int *)0x10C	/* UART Interrupt vector address */
#define	UARTIRQVect5	*(int *)0x110	/* UART Interrupt vector address */
#define	UARTIRQVect6	*(int *)0x114	/* UART Interrupt vector address */
#define	UARTIRQVect7	*(int *)0x118	/* UART Interrupt vector address */
#define	UARTIRQVect8	*(int *)0x11C	/* UART Interrupt vector address */

#define	UARTIRQ1  0x40	/* UART Interrupt vector number */
#define	UARTIRQ2  0x41	/* UART Interrupt vector number */
#define	UARTIRQ3  0x42	/* UART Interrupt vector number */
#define	UARTIRQ4  0x43	/* UART Interrupt vector number */
#define	UARTIRQ5  0x44	/* UART Interrupt vector number */
#define	UARTIRQ6  0x45	/* UART Interrupt vector number */
#define	UARTIRQ7  0x46	/* UART Interrupt vector number */
#define	UARTIRQ8  0x47	/* UART Interrupt vector number */

#define MULTIMEMBASE 0x02000000L	/* base of Multibus memory space */
#define MULTIIOBASE  0x02100000L	/* base of Multibus I/O space */
#define ROMFREEBASE  0x1000000L		/* base of rom address space */

#define interrupt586(idb)

#define CONSOLE_LINENUM 0
