/* $Id: asm_4k_rvip.h,v 3.2.56.1 1996/03/21 22:27:17 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_rvip.h,v $
 *------------------------------------------------------------------
 * asm_4k_rvip.h : Assembly definitions for Checkers
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_rvip.h,v $
 * Revision 3.2.56.1  1996/03/21  22:27:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/10  00:25:29  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:25:05  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../src-rvip/rvip_asic.h"
#include "../src-rvip/rvip_pcmap.h"
#include "../src-rvip/rvip.h"
#include "../src-rvip/rvip_dma.h"

/*
 * Define NMI stack, counter and subroutine
 */
#define NMI_STACK	l5stk
#define NMI_COUNTER	l5cnt
#define NMI_SUBR	lev5subr
#define NMI_SREG_MASK   0xffff60fd

/*
 * Define level for handling signals, exceptions etc
 */
#define SIG_LEVEL	LEVEL4

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
#define LEVEL1_SREG_MASK   0xfffff5fd	/* 1111 0101 - PA IO,sw1 off */
#define LEVEL2_SREG_MASK   0xfffff5fd	/* 1111 0101 - PA IO,sw1 */
#define LEVEL3_SREG_MASK   0xffffe5fd	/* 1110 0101 - PA IO,sw1,DMA off */
#define LEVEL34_SREG_MASK  0xffffe0fd	/* 1110 0000 - PA IO,sw1,DMA,Nev off */
#define LEVEL6_SREG_MASK   0xffff20fd	/* 0010 0000 - cbus on */
#define LEVEL7_SREG_MASK   0xffff00fd	/* 0000 0000 - all off */

/*
 * Floating pointer register usage by level 1 handler
 * Note: FPU_R0..R2 are already allocated
 */
#define FPU_SP_REG		FPU_R3	/* sp	*/
#define FPU_T0_REG		FPU_R4  /* t0	*/
#define FPU_T1_REG		FPU_R5  /* t0	*/
#define FPU_T2_REG		FPU_R6  /* t0	*/
#define FPU_T3_REG		FPU_R7  /* t0	*/
#define FPU_K0_REG		FPU_R8	/* k0	*/
#define FPU_AT_REG		FPU_R9	/* AT	*/
#define FPU_K1_REG		FPU_R10	/* k1	*/
#define FPU_EPC_REG		FPU_R11	/* EPC	*/
#define FPU_SREG_REG		FPU_R12	/* SR	*/
#define FPU_LO_REG		FPU_R13	/* LO	*/
#define FPU_HI_REG		FPU_R14	/* HI	*/
#define FPU_V0_REG		FPU_R15	/* v0	*/
#define FPU_V1_REG		FPU_R16	/* v1	*/
#define FPU_A0_REG		FPU_R17	/* a0	*/
#define FPU_A1_REG		FPU_R18	/* a1	*/
#define FPU_A2_REG		FPU_R19	/* a2	*/
#define FPU_A3_REG		FPU_R20	/* a3	*/
#define FPU_T4_REG		FPU_R21	/* t4	*/
#define FPU_T5_REG		FPU_R22	/* t5	*/
#define FPU_T6_REG		FPU_R23	/* t6	*/
#define FPU_T7_REG		FPU_R24	/* t7	*/
#define FPU_T8_REG		FPU_R25	/* t8	*/
#define FPU_T9_REG		FPU_R26	/* t9	*/
#define FPU_RA_REG		FPU_R27	/* ra	*/
#define FPU_S0_REG		FPU_R28	/* s0	*/
#define FPU_S1_REG		FPU_R29	/* s1	*/
#define FPU_S2_REG		FPU_R30	/* s2	*/
#define FPU_S3_REG		FPU_R31 /* s3	*/

/* end of file */
