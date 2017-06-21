/* $Id: signal_4k.h,v 3.2.62.1 1996/05/10 23:54:01 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/signal_4k.h,v $
 *------------------------------------------------------------------
 * signal_4k.h - Support for signals on R4K
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: signal_4k.h,v $
 * Revision 3.2.62.1  1996/05/10  23:54:01  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.2  1995/11/17  18:40:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:21:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SIGNAL_4K_H__
#define __SIGNAL_4K_H__

#include "../os/signal.h"

/*
 * Need some special purpose signals for dis-tastefull kernel type
 * errors
 */
#define SIGCACHE  20	/* cache error */
#define SIGWBERR  21	/* Write bus error interrupt */
#define SIGERROR  22    /* Fatal errors (shared memory parity etc) */
#define SIGRELOAD 23	/* Software forced reload */

/*
 * Define a literal to help in getting the cpu registers
 */
#define REG_INDEX(index)	((index*2)+1)

/*
 * Define the address of our virtual vector table
 */
#define R4K_EXCEPTION_TABLE	0x800001C0

#ifndef ASMINCLUDE

/*
 * Externs
 */
extern ulong r4k_cause_sig_map[];
extern char parity_error_buffer[];

/*
 * CPU context
 */

typedef struct r4k_context_t_ {
    int regs[64];		/* cpu registers 	*/
    int sreg;			/* status register 	*/
    int pad1;			/* padding for alignment*/
    int mdlo_hi, mdlo;		/* LO register		*/
    int mdhi_hi, mdhi;		/* HI register		*/
    int badvaddr_hi, badvaddr;	/* Bad Virtual address  */
    int cause;			/* cause register	*/
    int pad2;			/* padding for alignment*/
    int epc_hi, epc;		/* epc register  	*/
    int err_epc_hi, err_epc;	/* error epc		*/
} r4k_context_t;

/*
 * Define some common register numbers
 */
#define SP_REGNUM	59	
#define RA_REGNUM	63
#define V0_REGNUM	5
#define A0_REGNUM	9
#define A1_REGNUM	11
#define A2_REGNUM	13
#define A3_REGNUM	15

#endif  /* ASMINCLUDE */

/*
 * Definitions for the reset reason in the Monitor. We can reset due
 * to a power-on, the reset command or a watch dog timeout
 */
#define RESET_COLD   1
#define RESET_WARM   2
#define RESET_WDOG   3
#define RESET_NMI    4

#ifndef ASMINCLUDE

/*
 * Defines for the signal system
 */
typedef int (*SIG_PF)(int, ...);

#define MAX_SIGNAL_NUMBER	24

/*
 * Prototypes
 */
extern SIG_PF signal(int, SIG_PF handler);
extern void r4k_signal_handler(int, int, r4k_context_t *);
extern void r4k_init_signals(void);

#else /* ASMINCLUDE */
/*
 * Definitions for exception processing
 * Note : EXCPT_FRAME is just sizeof(r4k_context_t) rounded to 
 * quadword boundary
 */
#define EXCPT_FRAME        0x138     /* Size of exception stack frame */

/*
 * A macro to allow asm code to easily read/write
 * a context data structure
 */
#define SP_OFS(a)	(a*8)

/*
 * Index to the above macro for each register
 */
#define REG_ZERO	0
#define REG_AT		1
#define REG_V0		2
#define REG_V1		3
#define REG_A0		4
#define REG_A1		5
#define REG_A2		6
#define REG_A3		7
#define REG_T0		8
#define REG_T1		9
#define REG_T2		10
#define REG_T3		11
#define REG_T4		12
#define REG_T5		13
#define REG_T6		14
#define REG_T7		15
#define REG_S0		16
#define REG_S1		17
#define REG_S2		18
#define REG_S3		19
#define REG_S4		20
#define REG_S5		21
#define REG_S6		22
#define REG_S7		23
#define REG_T8		24
#define REG_T9		25
#define REG_K0		26
#define REG_K1		27
#define REG_GP		28
#define REG_SP		29
#define REG_S8		30
#define REG_RA		31

#define REG_SREG     	32
#define REG_LO  	33
#define REG_HI          34
#define REG_BADVADDR	35
#define REG_CAUSE       36
#define REG_EPC		37
#define REG_ERR_EPC     38

/*
 * The amount of space in bytes, needed on the stack for a 
 * context structure
 */
#define R4K_CONTEXT_FRAME	312

#endif /* ASMINCLUDE */


#endif /* !__SIGNAL_4K_H__ */
