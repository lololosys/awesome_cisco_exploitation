/* $Id: diagcode.h,v 1.1.6.1 1996/07/08 22:42:57 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/diagcode.h,v $
 *------------------------------------------------------------------
 * diagcode.h
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diagcode.h,v $
 * Revision 1.1.6.1  1996/07/08  22:42:57  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:53  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __DIAGCODE_H__
#define __DIAGCODE_H__


#define DIAGCODE_ADDR		0xA0001000 /* addr to write codes */

/* layout of diag code (these are offsets) */
#define DIAG_CODE		0	/* startup/diag code */
#define DIAG_ENDFLAG		1	/* logic analyzer flag */
#define DIAG_MISC		4	/* code dependent */
#define DIAG_CAUSE		8
#define DIAG_SREG		0xc
#define DIAG_EPC		0x10
#define DIAG_ERREPC		0x18
#define DIAG_SP			0x20
#define DIAG_RA			0x28
/* for memory errors */
#define DIAG_EXPECTED		DIAG_CAUSE
#define DIAG_READ		DIAG_SREG

/* the codes */
#define CODE_STARTUP		1	/* at start of program */
					/* misc = status reg */
#define CODE_INITSTAT		2	/* after set SREG and CAUSE */
					/* misc = status reg */
#define CODE_SETCR1		3	/* reset CYA, DBUS status, set CR1 */
					/* misc = CR1 */
#define CODE_FLUSHL2		4	/* after flushing L2 cache */
#define CODE_SIZEDRAM		5	/* size dram and set control reg */
					/* misc = CR1 */
#define CODE_TEST64K		6	/* after testing lower 64K */
#define CODE_LOWERRAM		7	/* more testing of lower DRAM */
#define CODE_CLEARBSS		8	/* after clearing BSS */
#define CODE_NVRAMINIT		9
#define CODE_INITEXCEPTION	0xa	/* after exception initialization */
#define CODE_INITTLB		0xb	/* after init TLB */
#define CODE_INITCACHE		0xc	/* after cache init */
#define CODE_SETUPPMA		0xd
#define CODE_SIZESRAM		0xe	/* after size and clear SRAM */
					/* misc = SRAM size */
#define CODE_CLEARDRAM		0xf	/* after clearing DRAM */
					/* misc = DRAM size */
#define CODE_DONE		0x7f	/* done init, before loading DBUS code */
#define CODE_MEMORYERROR	0x81	/* error while testing memory */
					/* misc = address, expected, read */


#ifndef ASMINCLUDE

void diagcode(uchar, long);

#endif

#endif
