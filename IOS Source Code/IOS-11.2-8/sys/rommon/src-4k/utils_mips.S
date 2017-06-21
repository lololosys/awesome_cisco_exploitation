/* $Id: utils_mips.S,v 3.1.24.1 1996/03/21 23:30:18 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/utils_mips.S,v $
 *------------------------------------------------------------------
 * utils_mips.S
 *
 * Feb 1996, mansonw
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: utils_mips.S,v $
| Revision 3.1.24.1  1996/03/21  23:30:18  gstovall
| Branch: California_branch
| The ELC_branch now knows the joy of California dreaming.
|
| Revision 3.1.2.1  1996/02/16  21:41:34  szhang
| CSCdi49173:  Separate the startup and exception code
| Branch: ELC_branch
| Separate the startup and exception code into platform
| dependent and platform independent files.
|
| Revision 3.1  1996/02/16  01:39:38  mansonw
| Prepare for code modulization for exception and utility code
|
 *------------------------------------------------------------------
 * $Endlog$
 */


#define ASMINCLUDE

#include "signal_4k.h"
#include "cpu_mips.h"

	.text

	.globl sys_restart
/*
 * system_handler:
 * This code is executed when an exception occurs during the
 * initialization phase of a loaded image (IE it is not ready to handle
 * interrupts itself yet) or the loaded image does not want to handle the
 * exception but would rather the monitor handled it (IE syscall). 
 *
 * Entry : 	k0 saved in FPU_R0
 *		k1 saved in FPU_R1
 *		AT saved in FPU_R2
 *
 * Exit :	Make frame. Copy k0,k1,AT to frame from FPU.
 *		Save RA,sp in frame. Load k1 with handler address 
 */
FRAME(system_handler,sp,0,ra)
	.set	noreorder	
	.set	noat

	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	addiu	sp,sp,-EXCPT_FRAME			# make frame
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)		# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)		# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)		# save copy of AT
	
	la	k0,general_exception_handling		# get entry point
	la	k1,general_exception			# get handler address

	j	k0					# call entry point
	nop

	.set	at
	.set	reorder
ENDFRAME(system_handler)


/*
 * General Exception handler
 *
 * Note : To the asm code, the exception stack frame is an array of longlong.
 * However, to the 32 bit compiler it is an array of longs. Hence the funny
 * arithmetic with the offsets. When 64 bit compiler is available, this
 * arithmetic needs to be removed IE becomes REG_???*8(sp)
 *
 * An exception handler will be called with the following:
 * 	a0 - pointer to frame
 *
 */
FRAME(gen_handler,sp,0,ra)
	.set	noreorder

	.set	noat
	sd	AT,SP_OFS(REG_AT)(sp)		# save AT
	.set	at

general_exception_handling:
	
	/*
	 * Other handlers jump here after the following work has been 
	 * done. Frame made on present stack, k0,k1,ra,AT and sp
	 * have been saved in the frame. Handler address in k1.
	 */

	sd	v0,SP_OFS(REG_V0)(sp)		# save v0
	sd	v1,SP_OFS(REG_V1)(sp)		# save v1
	sd 	a0,SP_OFS(REG_A0)(sp)		# save a0
	sd 	a1,SP_OFS(REG_A1)(sp)		# save a1
	sd 	a2,SP_OFS(REG_A2)(sp)		# save a2
	sd 	a3,SP_OFS(REG_A3)(sp)		# save a3
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	mfc0	a1,CP0_SREG			# get status reg
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfhi	t1				# get HI
	mflo	t2				# get LO

	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5
	sd	t0,SP_OFS(REG_EPC)(sp)		# save EPC reg
	sd	t1,SP_OFS(REG_HI)(sp)		# save HI
	sd	t2,SP_OFS(REG_LO)(sp)		# save LO
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9

	sw	a1,SP_OFS(REG_SREG)(sp)		# save status register
	mfc0	k0,CP0_CAUSE			# get Cause register
	dmfc0	t2,CP0_BADVADDR			# get BadVAddr
	dmfc0	t0,CP0_ERR_EPC			# get Error EPC reg

	sd	t2,SP_OFS(REG_BADVADDR)(sp)	# save BadVAddr reg
	sd	t0,SP_OFS(REG_ERR_EPC)(sp)	# save Error EPC reg
	sw	k0,SP_OFS(REG_CAUSE)(sp)	# save the cause reg

	sd	zero,SP_OFS(REG_ZERO)(sp)	# save zero, for emulators
	sd	s0,SP_OFS(REG_S0)(sp)		# save s0
	sd	s1,SP_OFS(REG_S1)(sp)		# save s1
	sd	s2,SP_OFS(REG_S2)(sp)		# save s2
	sd	s3,SP_OFS(REG_S3)(sp)		# save s3
	sd	s4,SP_OFS(REG_S4)(sp)		# save s4
	sd	s5,SP_OFS(REG_S5)(sp)		# save s5
	sd	s6,SP_OFS(REG_S6)(sp)		# save s6
	sd	s7,SP_OFS(REG_S7)(sp)		# save s7
	sd	s8,SP_OFS(REG_S8)(sp)		# save s8

	move	a0,sp				# setup parameter 0

	jal	k1				# jump to handler
	sd	gp,SP_OFS(REG_GP)(sp)		# save gp (bd slot)

	/*
	 * Ensure that all interrupts are masked until eret.
	 * We will do this by restoring the stacked status register
	 * which the handler's are NOT allowed to touch !!
	 */

sys_restart:
	/*
	 * There seems to be a bug in the R4400 processor
	 * that causes interrupts to be enabled if the status register
	 * is written with the EXL bit and the IE bit both set at the
	 * same time. For this reason, we clear IE, set EXL and then reset
	 * IE to its original state.
	 */
	mfc0	s0,CP0_SREG			# get status register
	lw	s2,SP_OFS(REG_SREG)(sp)		# get sreg copy
	li	s1,~SREG_IE			# get IE mask
	and	s0,s0,s1			# mask IE 
	mtc0	s0,CP0_SREG			# disable IE (EXL=0,IE=0)

	and	s3,s2,s1			# mask IE bit in sreg copy
	mtc0	s3,CP0_SREG			# disable IE (EXL=1,IE=0)
	nop					# give it time to settle

	mtc0	s2,CP0_SREG			# restore sreg (EXL=1,IE=1)
	nop					# give it time to settle

	ld	s7,SP_OFS(REG_S7)(sp)		# restore s7
	ld	s8,SP_OFS(REG_S8)(sp)		# restore s8
	ld	k1,SP_OFS(REG_EPC)(sp)		# get stacked EPC
	ld	t0,SP_OFS(REG_ERR_EPC)(sp)	# get Error EPC reg
	dmtc0	k1,CP0_EPC			# restore EPC
	dmtc0	t0,CP0_ERR_EPC			# restore Error EPC reg
	ld	s0,SP_OFS(REG_S0)(sp)		# restore s0
	ld	s1,SP_OFS(REG_S1)(sp)		# restore s1
	ld	s2,SP_OFS(REG_S2)(sp)		# restore s2
	ld	s3,SP_OFS(REG_S3)(sp)		# restore s3
	ld	s4,SP_OFS(REG_S4)(sp)		# restore s4
	ld	s5,SP_OFS(REG_S5)(sp)		# restore s5
	ld	s6,SP_OFS(REG_S6)(sp)		# restore s6

	ld	v0,SP_OFS(REG_V0)(sp)		# restore v0
	ld	v1,SP_OFS(REG_V1)(sp)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(sp)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(sp)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(sp)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(sp)		# restore a3
	ld	t4,SP_OFS(REG_HI)(sp)		# get HI
	ld	t5,SP_OFS(REG_LO)(sp)		# get LO
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0
	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	mthi	t4				# restore HI
	mtlo	t5				# restore LO
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3
	ld	t4,SP_OFS(REG_T4)(sp)		# restore t4
	ld	t5,SP_OFS(REG_T5)(sp)		# restore t5
	ld	t6,SP_OFS(REG_T6)(sp)		# restore t6
	ld	t7,SP_OFS(REG_T7)(sp)		# restore t7
	ld	t8,SP_OFS(REG_T8)(sp)		# restore t8
	ld	t9,SP_OFS(REG_T9)(sp)		# restore t9
	
	ld	gp,SP_OFS(REG_GP)(sp)		# restore gp
	ld	ra,SP_OFS(REG_RA)(sp)		# restore ra

	.set	noat

	/*
 	 * Undecided here - whether to increment the stack pointer
	 * or pop it from the stack. For now, restore it. This allows
	 * a down loaded image to be re-started while at the same time
	 * keeping different stacks for the monitor and the hosted image.
	 */
	ld	k0,SP_OFS(REG_K0)(sp)		# restore k0
	ld	k1,SP_OFS(REG_K1)(sp)		# restore k1
	ld	AT,SP_OFS(REG_AT)(sp)		# restore AT
	ld	gp,SP_OFS(REG_GP)(sp)		# restore gp
	nop					# maybe dont need this ?

	ld	sp,SP_OFS(REG_SP)(sp)		# restore the SP
	nop					# nop

	eret					# return
	nop					# instruction not executed

	.set	at
	.set	reorder
ENDFRAME(gen_handler)


/*
 * getcp0_sreg : Return the Status Register
 * long getcp0_sreg()
 */
FRAME(getcp0_sreg,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_SREG
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_sreg)

/*
 * getcp0_cerr : Return the Cache Err Register
 * long getcp0_cerr()
 */
FRAME(getcp0_cerr,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_CACHERR
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_cerr)

/*
 * getcp0_config : Return the Config Register
 * long getcp0_config()
 */
FRAME(getcp0_config,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_CONFIG
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_config)

/*
 * getcp0_prid : Return the PrID Register
 * long getcp0_prid()
 */
FRAME(getcp0_prid,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_PRID
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_prid)


/*
 * getcpu_sp : Return the Stack pointer
 * long getcpu_sp()
 */
FRAME(getcpu_sp,sp,0,ra)
	.set	noreorder
	move	v0,sp
	j	ra
	nop
	.set	reorder
ENDFRAME(getcpu_sp)

/*
 * getcp0_compare : Return the Compare Register
 * long getcp0_compare()
 */
FRAME(getcp0_compare,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_COMPARE
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_compare)

/*
 * getcp0_count : Return the Count Register
 * long getcp0_count()
 */
FRAME(getcp0_count,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_COUNT
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_count)

/*
 * getcp0_cause : Return the Cause Register
 * long getcp0_cause()
 */
FRAME(getcp0_cause,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_CAUSE
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(getcp0_cause)

/*
 * setcp0_sreg : Set Status Register , return old value
 * long setcp0_sreg(sreg)
 *	long sreg;
 */
FRAME(setcp0_sreg,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_SREG
	mtc0	a0,CP0_SREG
	j	ra
	nop	
	.set	reorder
ENDFRAME(setcp0_sreg)


/*
 * setcp0_compare : Set Compare Register, return old value
 * long setcp0_comnpare(compare)
 * 	long conmpare;
 */
FRAME(setcp0_compare,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_COMPARE
	mtc0	a0,CP0_COMPARE
	j	ra
	nop	
	.set	reorder
ENDFRAME(setcp0_compare)

/*
 * setcp0_count : Set Count Register, return old value
 * long setcp0_count(count)
 * 	long count;
 */
FRAME(setcp0_count,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_COUNT
	mtc0	a0,CP0_COUNT
	j	ra
	nop	
	.set	reorder
ENDFRAME(setcp0_count)

/*
 * setcp0_config : Set Config Register, return old value
 * long setcp0_config(config)
 * 	long config;
 */
FRAME(setcp0_config,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_CONFIG
	mtc0	a0,CP0_CONFIG
	j	ra
	nop	
	.set	reorder
ENDFRAME(setcp0_config)

/*
 * setcp0_cause : Set Cause Register, return old value
 * long setcp0_cause(cause)
 * 	long cause;
 */
FRAME(setcp0_cause,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_CAUSE
	nop
	mtc0	a0,CP0_CAUSE
	nop
	j	ra
	nop	
	.set	reorder
ENDFRAME(setcp0_cause)


/*
 * setcpu_sp : Set the Stack pointer, return old Stack Pointer
 * long setcpu_sp(sp)
 *	long sp;
 */
FRAME(setcpu_sp,sp,0,ra)
	.set	noreorder
	move	v0,sp
	move	sp,a0
	j	ra
	nop
	.set	reorder
ENDFRAME(setcpu_sp)


/*
 * Memory access functions
 */

/*
 * ld_access: Read an eight byte quantity
 *
 * void ld_access(unsigned long *addr, *t, *tu)
 */
FRAME(ld_access, sp, 0, ra)
	.set	noreorder

	ld	t0,0(a0)		# read 8 bytes
	nop				# in case of bus error
	nop
	sw	t0,0(a1)		# *t = t0
	dsrl	t0,t0,32		# t0 >> 32
	sw	t0,0(a2)		# *tu = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(ld_access)

/*
 * ldr_access: Read a 5/6/7 byte quantity via ldr
 *
 * void ldr_access(unsigned long *addr, *t, *tu)
 */
FRAME(ldr_access, sp, 0, ra)
	.set	noreorder

	ldr	t0,0(a0)		# read 5/6/7 bytes
	nop				# in case of bus error
	nop

	sw	t0,0(a1)		# *t = t0
	dsrl	t0,t0,32		# t0 >> 32
	sw	t0,0(a2)		# *tu = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(ldr_access)

/*
 * ldl_access: Read a 5/6/7 byte quantity via ldl
 *
 * void ldl_access(unsigned long *addr, *t, *tu)
 */
FRAME(ldl_access, sp, 0, ra)
	.set	noreorder

	ldl	t0,0(a0)		# read 5/6/7 bytes
	nop				# in case of bus error
	nop

	sw	t0,0(a1)		# *t = t0
	dsrl	t0,t0,32		# t0 >> 32
	sw	t0,0(a2)		# *tu = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(ldl_access)

/*
 * lwr_access: Read a 1/2/3/4 byte quantity via lwr
 *
 * void lwr_access(unsigned long *addr, *t)
 */
FRAME(lwr_access, sp, 0, ra)
	.set	noreorder

	lwr	t0,0(a0)		# read 1/2/3/4 bytes
	nop				# in case of bus error
	nop

	sw	t0,0(a1)		# *t = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(lwr_access)

/*
 * lwl_access: Read a 1/2/3/4 byte quantity via lwl
 *
 * void lwl_access(unsigned long *addr, *t)
 */
FRAME(lwl_access, sp, 0, ra)
	.set	noreorder

	lwl	t0,0(a0)		# read 1/2/3/4 bytes
	nop				# in case of bus error
	nop

	sw	t0,0(a1)		# *t = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(lwl_access)

/*
 * lw_access: Read a 4 byte quantity
 *
 * void lw_access(unsigned long *addr, *t)
 */
FRAME(lw_access, sp, 0, ra)
	.set	noreorder

	lw	t0,0(a0)		# read 4 bytes
	nop				# in case of bus error
	nop

	sw	t0,0(a1)		# *t = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(lw_access)

/*
 * lh_access: Read a 2 byte quantity
 *
 * void lh_access(unsigned long *addr, *t)
 */
FRAME(lh_access, sp, 0, ra)
	.set	noreorder

	lh	t0,0(a0)		# read 2 bytes
	nop				# in case of bus error
	nop

	sh	t0,0(a1)		# *t = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(lh_access)

/*
 * lb_access: Read a 1 byte quantity
 *
 * void lb_access(unsigned long *addr, *t)
 */
FRAME(lb_access, sp, 0, ra)
	.set	noreorder

	lbu	t0,0(a0)		# read 1 byte
	nop				# in case of bus error
	nop

	sb	t0,0(a1)		# *t = t0

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(lb_access)

/*
 * sd_access: Write an eight byte quantity
 *
 * void sd_access(unsigned long *addr, t, tu)
 */
FRAME(sd_access, sp, 0, ra)
	.set	noreorder

	li	t1,0xffffffff		
	dsll	t1,t1,32		# t1 = 0xffffffff 00000000
	dsrl	t2,t1,32		# t2 = 0x00000000 ffffffff

	and	a1,a1,t2		# a1 = 0x00000000 a1
	and	a2,a2,t2
	dsll	a2,a2,32 		# a2 =         a2 00000000
	or	a2,a2,a1		# a2 =         a2 a1

	sd	a2,0(a0)		# *addr = a2
	nop
	nop

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(sd_access)

/*
 * sdr_access: Write a 5/6/7 byte quantity via sdr
 *
 * void sdr_access(unsigned long *addr, t, tu)
 */
FRAME(sdr_access, sp, 0, ra)
	.set	noreorder

	li	t1,0xffffffff		
	dsll	t1,t1,32		# t1 = 0xffffffff 00000000
	dsrl	t2,t1,32		# t2 = 0x00000000 ffffffff

	and	a1,a1,t2		# a1 = 0x00000000 a1
	and	a2,a2,t2
	dsll	a2,a2,32		# a2 =         a2 00000000
	or	a2,a2,a1		# a2 =         a2 a1

	sdr	a2,0(a0)		# *addr = a2
	nop
	nop

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(sdr_access)

/*
 * sdl_access: Write a 5/6/7 byte quantity via sdl
 *
 * void sdl_access(unsigned long *addr, t, tu)
 */
FRAME(sdl_access, sp, 0, ra)
	.set	noreorder

	li	t1,0xffffffff		
	dsll	t1,t1,32		# t1 = 0xffffffff 00000000
	dsrl	t2,t1,32		# t2 = 0x00000000 ffffffff

	and	a1,a1,t2		# a1 = 0x00000000 a1
	and	a2,a2,t2
	dsll	a2,a2,32		# a2 =         a2 00000000
	or	a2,a2,a1		# a2 =         a2 a1

	sdl	a2,0(a0)		# *addr = a2
	nop
	nop

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(sdl_access)

/*
 * swr_access: Write a 1/2/3/4 byte quantity via swr
 *
 * void sdr_access(unsigned long *addr, t)
 */
FRAME(swr_access, sp, 0, ra)
	.set	noreorder

	swr	a1,0(a0)		# *addr = a1
	nop
	nop

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(swr_access)

/*
 * swl_access: Write a 1/2/3/4 byte quantity via swl
 *
 * void swl_access(unsigned long *addr, t)
 */
FRAME(swl_access, sp, 0, ra)
	.set	noreorder

	swl	a1,0(a0)		# *addr = a1
	nop
	nop

	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(swl_access)

/*
 * TLB functions
 */

/*
 * setasid : Set the ASID
 * void setasid(int asid)
 */
FRAME(setasid,sp,0,ra)
	.set	noreorder
	and	a0,TLB_HI_ASIDMASK
	mtc0	a0,CP0_TLB_HI
	j	ra
	nop
	.set	reorder
ENDFRAME(setasid)

/*
 * getasid : Return the present ASID
 * int getasid()
 */
FRAME(getasid,sp,0,ra)
	.set	noreorder
	mfc0	v0,CP0_TLB_HI
	nop
	and	v0,TLB_HI_ASIDMASK
	j	ra
	nop
	.set	reorder
ENDFRAME(getasid)

/*
 * gettlb : Get index TLB entry. Return 0 for success, 1 for fail
 * int gettlb(int index, struct tlb_t *tlb)
 */
FRAME(gettlb,sp,0,ra)
	.set	noreorder
	mfc0	t0,CP0_SREG		# save status register
	nop
	move	t1,t0			# get copy of status register
	mfc0	t2,CP0_TLB_HI		# get current ASID
	and	t0,~SREG_IE		# disable interrupts
	mtc0	t0,CP0_SREG
	mtc0	a0,CP0_INDEX		# write TLB index
	nop
	tlbr				# read TLB entry
	nop
	mfc0	t3,CP0_PAGEMASK		# read page mask
	mfc0	t4,CP0_TLB_HI		# read entry Hi
	sw	t3,0(a1)		# tlb->pmask = CP0_PAGEMASK
	sw	t4,4(a1)		# tlb->hi = CP0_TLB_HI
	mfc0	t3,CP0_TLB_LO_0		# read entry lo 0
	mfc0	t4,CP0_TLB_LO_1		# read entry lo 1
	sw	t3,8(a1)		# tlb->lo0 = CP0_TLB_LO_0
	sw	t4,12(a1)		# tlb->lo1 = CP0_TLB_LO_1
	mtc0	t2,CP0_TLB_HI		# restore current ASID
	mtc0	t1,CP0_SREG		# restore status register
	move	v0,zero			# return success
	j	ra			# return
	nop
	.set 	reorder
ENDFRAME(gettlb)

/*
 * settlb : Set indexed TLB entry. Return 0 for success, 1 for fail
 * int settlb(int index, struct tlb_t *tlb)
 */
FRAME(settlb,sp,0,ra)
	.set	noreorder
	mfc0	t0,CP0_SREG		# save status register
	nop
	move	t1,t0			# get copy of status register
	and	t0,~SREG_IE		# disable interrupts
	mtc0	t0,CP0_SREG
	lw	t2,0(a1)		# t2 = tlb->pmask
	lw	t3,4(a1)		# t3 = tlb->hi
	mtc0	t2,CP0_PAGEMASK		# write Page Mask register
	mtc0	t3,CP0_TLB_HI		# write Entry Hi register
	lw	t2,8(a1)		# t2 = tlb->lo0
	lw	t3,12(a1)		# t3 = tlb->lo1
	mtc0	t2,CP0_TLB_LO_0		# write Entry Lo 0 register
	mtc0	t3,CP0_TLB_LO_1		# write Entry Lo 1 register
	mtc0	a0,CP0_INDEX		# write TLB index
	nop
	tlbwi				# write TLB entry
	nop
	mtc0	t1,CP0_SREG		# restore status register
	move	v0,zero			# return success
	j	ra			# return
	nop
	.set 	reorder
ENDFRAME(settlb)

/*
 * tlbprobe : Probe the TLB for a match with vaddr
 * int tlbprobe(struct entryhi_t *hireg)
 * Return -1 if no match, else TLB index
 */
FRAME(tlbprobe,sp,0,ra)
	.set 	noreorder
	mfc0	t0,CP0_SREG		# get status register
	mfc0	t3,CP0_TLB_HI		# save Entry Hi
	move	t1,t0
	and	t0,~SREG_IE		# disable interrupts
	lw	t2,0(a0)		# t2 = *hireg
	mtc0	t0,CP0_SREG		# set status register
	mtc0	t2,CP0_TLB_HI		# set Entry Hi register
	nop
	tlbp     			# probe tlb
	nop
	mfc0	v1,CP0_INDEX		# get index register
	li	v0,-1			# set up return value
	bltz	v1,failed		# branch if neg. IE probe failed
	nop	
	move	v0,v1			# store index in return register
failed:
	mtc0	t3,CP0_TLB_HI		# restore original entry hi
	mtc0	t1,CP0_SREG		# restore original status register
	j	ra 			# return
	nop
	.set	reorder
ENDFRAME(tlbprobe)

/*
 * Cache functions
 */

/*
 * Store a data cache line from TagLo, TagHi
 * void dstore_tag(taglo, taghi, index)
 *      long taglo, taghi, index;
 */ 
FRAME(dstore_tag,sp,0,ra)
	.set	noreorder
	mtc0	a0,CP0_TAGLO
	mtc0	a1,CP0_TAGHI
	nop
	cache	9,0(a2)		# Index Store Tag, Primary D Cache
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(dstore_tag)

/*
 * Store an instruction cache line from TagLo, TagHi
 * void istore_tag(taglo, taghi, index)
 *      long taglo, taghi, index;
 */
FRAME(istore_tag,sp,0,ra)
	.set	noreorder
	mtc0	a0,CP0_TAGLO
	mtc0	a1,CP0_TAGHI
	nop
	cache	8,0(a2)		# Index Store Tag, Primary I Cache
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(istore_tag)


/*
 * sys_call : make a system call
 */
FRAME(sys_call,sp,0,ra)
	.set	noreorder
	nop
	syscall
	nop
	j	ra
	nop
	.set    reorder
ENDFRAME(sys_call)

/*
 * break_pt : Take a break point
 */
FRAME(break_pt,sp,0,ra)
	.set	noreorder
	nop
	break	0
	nop
	j	ra
	nop
	.set    reorder
ENDFRAME(break_pt)

/*
 * ill_opcode : Execute an illegal op code
 * void ill_opcode()
 */
FRAME(ill_opcode,sp,0,ra)
	.set 	noreorder
	.word	0x04180000		# Some illegal instruction
	nop
	j	ra
	nop
	.set	reorder
ENDFRAME(ill_opcode)

