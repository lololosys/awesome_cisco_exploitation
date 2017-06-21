/* $Id: asm_4k_vip.h,v 3.1.60.1 1996/03/21 22:27:26 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_vip.h,v $
 *------------------------------------------------------------------
 * asm_4k_vip.h : Assembly definitions for VIPER/Checkers
 *
 * Sept 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip.h,v $
 * Revision 3.1.60.1  1996/03/21  22:27:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.2  1996/03/18  23:50:00  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.10.1  1996/02/10  00:25:36  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/28  22:08:25  getchell
 * Placeholder for VIPER development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ASM_4K_VIP_H__
#define __ASM_4K_VIP_H__

#include "target_cisco.h"
#include "../src-4k/signal_4k.h"

#define UPPER_MEMD_MASK         0x001f     # currently 2 meg memd limit
#define LOWER_MEMD_MASK         0xffff

/*
 * Stack is at least 32 bytes bigger then we need so that the create dirty 
 * exclusive operation does not wipe out other valuable information.
 */
#define VIP_CBUS_STACK_SIZE	160

/*
 * Define stack locations for context save/restore
 */
#define PUSH_T4_OFS	88
#define PUSH_RA_OFS	80
#define PUSH_SP_OFS	72
#define PUSH_K0_OFS	64
#define PUSH_K1_OFS	56
#define PUSH_AT_OFS	48
#define PUSH_T0_OFS	40
#define PUSH_T1_OFS	32
#define PUSH_T2_OFS	24
#define PUSH_T3_OFS	16
#define PUSH_EPC_OFS	8
#define PUSH_SREG_OFS	4

/*
 * Port Adaptor Network IO jumptable defintions
 */
#define PA_WRAPPER_OFS		0
#define PA_BASE_OFS		4
#define PA_SCRATCH0_OFS		8
#define PA_SCRATCH1_OFS		12
#define PA_IDB_OFS	        16
#define PA_SREG_OFS		(PA_IDB_OFS + (4 * MAX_INTERFACES_PER_PA))
#define PA_TABLE_SIZE		(PA_SREG_OFS + (4 * MAX_INTERFACES_PER_PA))

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

#endif /* __ASM_4K_VIP_H__ */

/* end of file */
