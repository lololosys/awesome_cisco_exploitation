/* $Id: asm_4k_rsp.h,v 3.2.56.1 1996/03/21 22:27:11 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_rsp.h,v $
 *------------------------------------------------------------------
 * Assembly definitions for RSP
 *
 * June 1994, Chris Shaker
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_rsp.h,v $
 * Revision 3.2.56.1  1996/03/21  22:27:11  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/11/22  20:25:03  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef ASMINCLUDE
#include "target_cisco.h"
#endif
#include "../src-4k/signal_4k.h"

/*
 * Define NMI stack, counter, subroutine and exception level
 */
#define NMI_STACK	l7stk
#define NMI_COUNTER	l7cnt
#define NMI_SUBR	lev7subr
#define NMI_SREG_MASK	0xffff83fd

/*
 * Define level for signals, exceptions etc
 */
#define SIG_LEVEL	LEVEL7

/*
 * Define external interrupt controller cause bit
 */
#define CAUSE_EXT_BIT	CAUSE_IBIT2

/*
 * Some overall system defines
 */
#define MSEC_SETTING	1000			/* timer setting for 1ms */
#define	REFRFREQUENCY	(REFRESHTIME*MSEC_SETTING) /* Refresh freq. */

#define IO_IMASK_REG	ADRSPC_INTMASK
#define IO_WATCHDOG_REG	ADRSPC_COUNTER0		/* counter/timer 0 */

#define IO_MSDOG_REG    ADRSPC_MASTER_TIMER     /* master/slave watchdog */
#define IO_MSDOG_10MSEC MASTER_TIMER_10MSEC     /* largest is 10 msecs */
/*
 * Level 1/2 status reg interrupt mask
 * for masking interrupts in network interrupt handlers
 * (a 1 value enables the interrupt)
 */
#define LEVEL1_SREG_MASK    	0xffffc7fd	/* 1100 0111 - mask QA low */
#define LEVEL2_SREG_MASK        0xffff87fd	/* 1000 0111 - mask all QA */
