/* $Id: mips.h,v 1.1.4.1 1996/05/09 15:00:09 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/mips.h,v $
 *-----------------------------------------------------------------
 * mips.h --  machine dependent definitions for the MIPS architecture
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mips.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:09  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:15  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* mips.h - machine dependent definitions for the MIPS architecture
 *	The contents of this file are not copyrighted in any
 *	way, and may therefore be used without restriction.
 */

/*
 * This file supports both the LR3000 and the LR33000. By default
 * the LR3000 is assumed. For the LR33000 invoke the compiler with
 * the option -DLR33000.
 *
 * This file also supports development under SABLE, (the simulator
 * supplied with SPP). To compile for the SABLE environment, invoke
 * the compiler with the option -DSABLE.
 */

#ifndef MIPSEB
#ifndef MIPSEL
neither MIPSEB nor MIPSEL specified
#endif
#endif

#include <defines.h>

#if defined(LR33120) && !defined(LR33020)
#define LR33020
#endif

#if defined(LR33020) && !defined(LR33000)
#define LR33000
#endif

#if defined(LR33300) && !defined(LR33000)
#define LR33000
#endif

#if defined(ATMIZER) && !defined(LR33300)
#define LR33300
#endif

#ifndef _MIPS_
#define _MIPS_

#define K0BASE 		0x80000000
#define K0SIZE 		0x20000000
#define K1BASE 		0xa0000000
#define K1SIZE 		0x20000000
#define K2BASE 		0xc0000000
#define	IS_K0SEG(x)	((unsigned)(x)>=K0BASE && (unsigned)(x)<K1BASE)
#define	IS_K1SEG(x)	((unsigned)(x)>=K1BASE && (unsigned)(x)<K2BASE)
#define GEN_VECT 	0x80000080
#define UTLB_VECT 	0x80000000

/* these are used by the cache flushing routines */
/* flush_cache types */
#define ICACHE  0
#define DCACHE  1
#define IADDR   2

#ifdef LANGUAGE_ASSEMBLY

/* most mips assemblers don't accept these, so make defines for them */
#define jr j
#define jalr jal

/* aliases for general registers */
#define zero		$0
#define	AT		$1		/* assembler temporaries */
#define	v0		$2		/* value holders */
#define	v1		$3
#define	a0		$4		/* arguments */
#define	a1		$5
#define	a2		$6
#define	a3		$7
#define	t0		$8		/* temporaries */
#define	t1		$9
#define	t2		$10
#define	t3		$11
#define	t4		$12
#define	t5		$13
#define	t6		$14
#define	t7		$15
#define	s0		$16		/* saved registers */
#define	s1		$17
#define	s2		$18
#define	s3		$19
#define	s4		$20
#define	s5		$21
#define	s6		$22
#define	s7		$23
#define	t8		$24		/* temporaries */
#define	t9		$25
#define	k0		$26		/* kernel registers */
#define	k1		$27
#define	gp		$28		/* global pointer */
#define	sp		$29		/* stack pointer */
#define	s8		$30		/* saved register */
#define	fp		$30		/* frame pointer (old usage) */
#define	ra		$31		/* return address */

/* System Control Coprocessor (CP0) registers */
#define C0_SR		$12		/* Processor Status */
#define C0_CAUSE	$13		/* Exception Cause */
#define C0_EPC		$14		/* Exception PC */
#define C0_BADADDR	$8		/* Bad Address */
#define C0_BADVADDR	$8		/* Bad Virtual Address */
#define C0_PRID		$15		/* Processor Rev Indentifier */

#ifdef LR33000
#include "lr33000.h"
#else 
#define C0_CTEXT	$4		/* Context */
#define C0_TLBHI	$10		/* TLB EntryHi */
#define C0_TLBLO	$2		/* TLB EntryLo */
#define C0_INX		$0		/* TLB Index */
#define C0_RAND		$1		/* TLB Random */
#endif

#ifdef LR33300
#include "lr33300.h"
#endif

/* Floating-Point Control registers */
#define FPA_CSR		$31		/* Control/Status register */
#define FPA_IRR		$0		/* Implementation/Rev register */
#define C1_CSR		$31
#define C1_FRID		$0

#else /* ================ LANGUAGE_C ======================== */

#define C0_SR		12		/* Processor Status */
#define C0_CAUSE	13		/* Exception Cause */
#define C0_EPC		14		/* Exception PC */
#define C0_BADADDR	8		/* Bad Address */
#define C0_BADVADDR	8		/* Bad Virtual Address */
#define C0_PRID		15		/* Processor Rev Indentifier */

#ifdef LR33000
#include "lr33000.h"
#else
#define C0_CTEXT	4		/* Context */
#define C0_TLBHI	10		/* TLB EntryHi */
#define C0_TLBLO	2		/* TLB EntryLo */
#define C0_INX		0		/* TLB Index */
#define C0_RAND		1		/* TLB Random */
#endif

#ifdef LR33300
#include "lr33300.h"
#endif

/* Floating-Point Control registers */
#define FPA_CSR		31		/* Control/Status register */
#define FPA_IRR		0		/* Implementation/Rev register */
#define C1_CSR		31
#define C1_FRID		0
#endif /* LANGUAGE_C */

/* Floating-Point Control register bits */
#define CSR_C		0x00800000
#define CSR_EXC		0x0003f000
#define CSR_EE		0x00020000
#define CSR_EV		0x00010000
#define CSR_EZ		0x00008000
#define CSR_EO		0x00004000
#define CSR_EU		0x00002000
#define CSR_EI		0x00001000
#define CSR_TV		0x00000800
#define CSR_TZ		0x00000400
#define CSR_TO		0x00000200
#define CSR_TU		0x00000100
#define CSR_TI		0x00000080
#define CSR_SV		0x00000040
#define CSR_SZ		0x00000020
#define CSR_SO		0x00000010
#define CSR_SU		0x00000008
#define CSR_SI		0x00000004
#define CSR_RM		0x00000003

/* Status Register */
#define SR_CUMASK	0xf0000000	/* Coprocessor usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define SR_CU2		0x40000000	/* coprocessor 2 usable */
#define SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define SR_CU0		0x10000000	/* Coprocessor 0 usable */
#define SR_BEV		0x00400000	/* Bootstrap Exception Vector */
#define SR_TS		0x00200000	/* TLB shutdown */
#define SR_PE		0x00100000	/* Parity Error */
#define SR_CM		0x00080000	/* Cache Miss */
#define SR_PZ		0x00040000	/* Parity Zero */
#define SR_SWC		0x00020000	/* Swap Caches */
#define SR_ISC		0x00010000	/* Isolate Cache */

#define SR_IMASK	0x0000ff00	/* Interrupt Mask */
#define SR_IMASK8	0x00000000	/* Interrupt Mask level=8 */
#define SR_IMASK7	0x00008000	/* Interrupt Mask level=7 */
#define SR_IMASK6	0x0000c000	/* Interrupt Mask level=6 */
#define SR_IMASK5	0x0000e000	/* Interrupt Mask level=5 */
#define SR_IMASK4	0x0000f000	/* Interrupt Mask level=4 */
#define SR_IMASK3	0x0000f800	/* Interrupt Mask level=3 */
#define SR_IMASK2	0x0000fc00	/* Interrupt Mask level=2 */
#define SR_IMASK1	0x0000fe00	/* Interrupt Mask level=1 */
#define SR_IMASK0	0x0000ff00	/* Interrupt Mask level=0 */

#define SR_IBIT8	0x00008000	/*  (Intr5) */
#define SR_IBIT7	0x00004000	/*  (Intr4) */
#define SR_IBIT6	0x00002000	/*  (Intr3) */
#define SR_IBIT5	0x00001000	/*  (Intr2) */
#define SR_IBIT4	0x00000800	/*  (Intr1) */
#define SR_IBIT3	0x00000400	/*  (Intr0) */
#define SR_IBIT2	0x00000200	/*  (Software Interrupt 1) */
#define SR_IBIT1	0x00000100	/*  (Software Interrupt 0) */

#define SR_KUO		0x00000020	/* Kernel/User mode, old */
#define SR_IEO		0x00000010	/* Interrupt Enable, old */
#define SR_KUP		0x00000008	/* Kernel/User mode, previous */
#define SR_IEP		0x00000004	/* Interrupt Enable, previous */
#define SR_KUC		0x00000002	/* Kernel/User mode, current */
#define SR_IEC		0x00000001	/* Interrupt Enable, current */

/* Cause Register */
#define CAUSE_BD		0x80000000	/* Branch Delay */
#define CAUSE_CEMASK		0x30000000	/* Coprocessor Error */
#define CAUSE_CESHIFT		28		/* Right justify CE  */
#define CAUSE_IPMASK		0x0000ff00	/* Interrupt Pending */
#define CAUSE_IPSHIFT		8		/* Right justify IP  */
#define CAUSE_IP8		0x00008000	/*  (Intr5) */
#define CAUSE_IP7		0x00004000	/*  (Intr4) */
#define CAUSE_IP6		0x00002000	/*  (Intr3) */
#define CAUSE_IP5		0x00001000	/*  (Intr2) */
#define CAUSE_IP4		0x00000800	/*  (Intr1) */
#define CAUSE_IP3		0x00000400	/*  (Intr0) */
#define CAUSE_SW2		0x00000200	/*  (Software Int 1) */
#define CAUSE_SW1		0x00000100	/*  (Software Int 0) */
#define CAUSE_EXCMASK		0x0000003c	/* Exception Code */
#define CAUSE_EXCSHIFT		2		/* Right justify EXC */

/* Exception Code */
#define EXC_INT		(0 << 2)	/* External interrupt */
#define EXC_MOD		(1 << 2)	/* TLB modification */
#define EXC_TLBL	(2 << 2)    	/* TLB miss (Load or Ifetch) */
#define EXC_TLBS	(3 << 2)	/* TLB miss (Save) */
#define EXC_ADEL	(4 << 2)    	/* Addr error (Load or Ifetch) */
#define EXC_ADES	(5 << 2)	/* Address error (Save) */
#define EXC_IBE		(6 << 2)	/* Bus error (Ifetch) */
#define EXC_DBE		(7 << 2)	/* Bus error (data load/store) */
#define EXC_SYS		(8 << 2)	/* System call */
#define EXC_BP		(9 << 2)	/* Break point */
#define EXC_RI		(10 << 2)	/* Reserved instruction */
#define EXC_CPU		(11 << 2)	/* Coprocessor unusable */
#define EXC_OVF		(12 << 2)	/* Arithmetic overflow */

#define HI_HALF(x)	(((unsigned long)(x)) >> 16)
#define LO_HALF(x)	((x) & 0xffff)

#include "setjmp.h"

/* FPU stuff */
#define CSR_EMASK	(0x3f<<12)
#define CSR_TMASK	(0x1f<<7)
#define CSR_SMASK	(0x1f<<2)

#ifdef LR33020
#include "lr33020.h"
#endif

#ifdef ATMIZER
#include "atmizer.h"
#endif

#ifdef BSO_TASKING
#ifdef LANGUAGE_ASSEMBLY
/* BSO has it's own symbol for setting the gp register */
#define _gp	__lc_zp+0x8000
/* BSO Release 1.0 doesn't like .comm directives */
#define comm   lcomm
#endif
#endif

#endif /* _MIPS_ */

