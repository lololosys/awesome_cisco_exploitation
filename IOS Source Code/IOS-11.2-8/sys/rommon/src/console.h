/* $Id: console.h,v 3.2.58.3 1996/07/08 22:42:35 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/console.h,v $
 *------------------------------------------------------------------
 * console.h
 *
 * June 1993, Rob Clevenger and Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Console defines.
 *------------------------------------------------------------------
 * $Log: console.h,v $
 * Revision 3.2.58.3  1996/07/08  22:42:35  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/06/16  21:18:11  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:26:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:58  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "../../ts/s2681.h"

extern unsigned char UARTspeeds[];
extern char caphexnums[], digits[];
/* 
 * Note the use of pad macros to provide for different
 * amounts of padding between registers for hardware
 * alignment.
 */
#if !defined(BRASIL)
#define PAD(x)         volatile unsigned char pad_ ## x [7];
#define START_PAD(x)    volatile unsigned char pad_ ## x [4];
#define END_PAD(x)     volatile unsigned char pad_ ## x [3];
#endif

#if defined(BRASIL)
#define PAD(x)
#define START_PAD(x)
#define END_PAD(x)
#endif

struct regpair {
    START_PAD(0)
    volatile unsigned char mode;	/* MR1A/MR2A, MR1B/MR2B */
    PAD(1)
    volatile unsigned char state;	/* SRA/CSRA, SRB/CSRB */
    PAD(2)
    volatile unsigned char command;	/* CRA, CRB */
    PAD(3)
    volatile unsigned char data;	/* RHRA/THRA, RHRB/THRB */
    PAD(4)
    volatile unsigned char aux1;	/* A=IPCR/ACR, B=reserved */
    PAD(5)
    volatile unsigned char aux2;	/* A=ISR/IMR, B=IPR/OPCR */
    PAD(6)
    volatile unsigned char aux3;	/* A=CTU/CTUR, B=Start Counter/Set OPR */
    PAD(7)
    volatile unsigned char aux4;	/* A=CTL/CTLR, B=Stop Counter/Reset OPR */
    END_PAD(8)
};

#undef PAD
#undef START_PAD
#undef END_PAD


struct uartdevice {
    struct regpair a;		/* channel a */
    struct regpair b;		/* channel b */
};

#if defined(SIERRA) || defined(RSP) || defined(RSP2) || defined(PREDATOR) || defined(SVIP)
#define uartaddr	((struct uartdevice *)KSEG1_ADRSPC_DUART)
#endif

#if defined(BRASIL)
#define ADRSPC_DUART                    0x02120100 /* 2681 DUART (console) */
#define uartaddr        ((struct uartdevice *)ADRSPC_DUART)
#endif

/*
 * Basic IO primitives needed when initializing (since you can't
 * yet trust memory).
 */

#define PUTSTRING(str,arg) \
    str = arg; \
    while (*str) { \
        while ((uartaddr->a.state & TXREADY) == 0) \
	    ; \
	uartaddr->a.data = *str++; \
    }

#define PUTC(c) \
    while ((uartaddr->a.state & TXREADY) == 0) \
          ; \
    uartaddr->a.data = c;

#define PUT_DIAG_STR(str) \
    if (confreg & DIAG_MASK) { \
        PUTSTRING(cp,str); \
        }

#define PUTDIGIT(digit,shift) \
    while ((uartaddr->a.state & TXREADY) == 0) \
        ; \
    uartaddr->a.data = caphexnums[((digit >> shift) & 0xF)];

#define PUTHEXNUM(hexnum) \
    PUTDIGIT(hexnum,28); \
    PUTDIGIT(hexnum,24); \
    PUTDIGIT(hexnum,20); \
    PUTDIGIT(hexnum,16); \
    PUTDIGIT(hexnum,12); \
    PUTDIGIT(hexnum,8); \
    PUTDIGIT(hexnum,4); \
    PUTDIGIT(hexnum,0);

/* console.c */
extern void conout(char c);
extern int conin(void);
extern int constat(void);
extern char getchar(void);
extern void putchar(char c);
extern void setmore(void);
extern long conint(void);
extern void default_break(void);
extern int conbrkinttest(void);
extern void flush();
#if defined(SIERRA) || defined(RSP) || defined(RSP2)
extern long isconbrk(void);
#endif

/* auxport.c */
extern void initaux(void); 
extern void auxout(int c);
extern int auxin(void);
extern int auxstat(void);

/* auxtest.c */
extern int auxecho(void);
extern int auxloopback(void);
extern int auxinttest(void);
extern long auxint(void);

#endif  /* __CONSOLE_H__ */
