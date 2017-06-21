/* $Id: asm_4k_c4000.h,v 3.2.56.1 1996/03/21 22:27:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_c4000.h,v $
 *------------------------------------------------------------------
 * asm_4k_c4000.h : Assembly definitions for Sierra
 * 		    (R4K based 4000 platform)
 *
 * Novemeber 1993, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000.h,v $
 * Revision 3.2.56.1  1996/03/21  22:27:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/11/22  20:24:56  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:48:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "target_cisco.h"
#include "../src-4k/signal_4k.h"
#include "../src-4k/nevasic.h"
#include "../src-4k-c4000/pcmap.h"

/*
 * Define NMI stack, counter and subroutine
 */
#define NMI_STACK	l7stk
#define NMI_COUNTER	l7cnt
#define NMI_SUBR	lev7subr
#define NMI_SREG_MASK	0xffff83fd

/*
 * Define level for handling signals, exceptions etc
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

/*
 * Lance specific defines
 */
#define DEIM_A_CSR_OFS		0x400 	/* offset of CSR A register 	*/
#define DEIM_B_CSR_OFS		0xC00 	/* offset of CSR B register 	*/

/*
 * Am79970 specific defines
 */
#define AM79970_A_CSR_OFS       0x10010 /* offset of CSR A register 	*/

/* 
 * TMS380 specific defines
 */
#define TRIM_A_SIFSTS_OFS	0x0406	/* offset to SIFSTS reg A	*/
#define TRIM_B_SIFSTS_OFS	0x10406	/* offset to SIFSTS reg B     	*/
/*
 * MK5025 specific defines
 */
#define DSIM_VECTOR_OFS		0x202	/* offset to vector register	*/ 
#define DSIM_A_CSR_OFS		0x380   /* offset to CSR reg for A	*/
#define DSIM_B_CSR_OFS		0x480	/* offset to CSR reg for B	*/

/*
 * HD64570 specific defines
 */
#define QSIM_PORT23_OFS         1       /* offset of port 2/3 reg << 16   */
#define QSIM_INT_SRC_OFS        0x10000 /* offset of interrupt source reg */ 
#define QSIM_P0_INT_OFS         0x6     /* offset of port 0 intr cntl reg */
#define QSIM_P1_INT_OFS         0xA     /* offset of port 1 intr cntl reg */
#define QSIM_P2_INT_OFS         0x6     /* offset of port 2 intr cntl reg */
#define QSIM_P3_INT_OFS         0xA     /* offset of port 3 intr cntl reg */

/*
 * Munich32 specific defines
 */
#define M32_INT_REG_OFS		0x44	/* offset to interrupt cntl reg	*/

/*
 * Level 2 jumptable defines
 */
#define LEV2_WRAPPER_OFS	0	/* wrapper position		*/
#define LEV2_BASE_OFS		4	/* slot_base_address position 	*/
#define LEV2_IACK_OFS		8	/* slot_iack_reg position	*/
#define LEV2_IDB_A		12	/* idb_a position		*/
#define LEV2_IDB_B		16	/* idb_b position		*/
#define LEV2_IDB_C		20	/* idb_c position		*/
#define LEV2_IDB_D		24	/* idb_d position		*/
#define LEV2_IDB_E		28	/* idb_e position		*/
#define LEV2_IDB_F		32	/* idb_f position		*/

/*
 * Defines to maintain round robin priority of level 2 interrupts
 */
#define LEV2_SLOT1_NEXT		0x04
#define LEV2_SLOT2_NEXT		0x08
#define LEV2_SLOT0_NEXT		0x0C

#define LEV2_TABLE_SIZE		44	/* sizeof(lev2_table_t) */

/*
 * Level 2/3 status reg mask
 */
#define LEVEL2_SREG_MASK    	0xffffc7fd
#define LEVEL3_SREG_MASK        0xffff87fd

/*
 * Floating pointer register usage by level 2 handler
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

/* End of file */

