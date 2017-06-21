/* $Id: asm_4k_svip.h,v 3.2.58.1 1996/03/21 22:27:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_svip.h,v $
 *------------------------------------------------------------------
 * asm_4k_svip.h : Assembly definitions for VIPER
 *
 * september 1995, David Getchell
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_svip.h,v $
 * Revision 3.2.58.1  1996/03/21  22:27:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/02/10  00:25:32  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ASM_4K_SVIP_H__
#define __ASM_4K_SVIP_H__

#include "../src-svip/svip_asic.h"
#include "../src-svip/svip_pcmap.h"
#include "../src-svip/svip.h"

#define SVIP_EXT_INT_MASK  0x04f0	/* used to isolate interrupt values */
#define SVIP_EXT_INT_SHIFT 2

/*
 * Define NMI stack, counter and subroutine
 */
#define NMI_STACK	l4stk
#define NMI_COUNTER	l4cnt
#define NMI_SUBR	lev4subr
#define NMI_SREG_MASK   0xffff40fd       /* Allow Nevada interrupts only */

/*
 * Define level for handling signals, exceptions etc
 */
#define SIG_LEVEL	LEVEL3

/*
 * Define external interrupt controller cause bit
 */
#define CAUSE_EXT_BIT	CAUSE_IBIT2

/*
 * Some overall system defines
 */
#define MSEC_SETTING	1000			/* timer setting for 1ms */
#define REFRESHTIME	4			/* refresh time (ms) */
#define	REFRFREQUENCY	(REFRESHTIME*MSEC_SETTING) /* Refresh freq. */

/*
 * Status register masks for interrupt levels
 */
#define LEVEL1_SREG_MASK   0xffffe1fd	/* 1110 0001 - PA1,PA2,DMA,SW1 off */
#define LEVEL2_SREG_MASK   0xffffc1fd	/* 1100 0001 - + PA management off */
#define LEVEL3_SREG_MASK   0xffffc0fd	/* 1100 0000 - + GDB off */
#define LEVEL5_SREG_MASK   0xffff40fd	/* 0100 0000 - + Timer off */
#define LEVEL6_SREG_MASK   0xffff40fd	/* 0100 0000 - nothing changes */
#define LEVEL7_SREG_MASK   0xffff00fd	/* 0000 0000 - + Nevada off */

#endif
