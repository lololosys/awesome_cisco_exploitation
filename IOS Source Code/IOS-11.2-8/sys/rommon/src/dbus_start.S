/* $Id: dbus_start.S,v 3.5.4.2 1996/07/08 22:42:38 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/dbus_start.S,v $
 *------------------------------------------------------------------
 * dbus_start.S
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_start.S,v $
| Revision 3.5.4.2  1996/07/08  22:42:38  mwu
| CSCdi60922:  11.2 Core Features Commit
| Branch: California_branch
|
| Revision 3.5.4.1  1996/03/21  23:27:00  gstovall
| Branch: California_branch
| The ELC_branch now knows the joy of California dreaming.
|
| Revision 3.2.2.3  1996/03/19  01:01:01  mbeesley
| Branch: ELC_branch
| Sync to V111_1_3
|
| Revision 3.2.2.2  1996/02/08  08:58:00  gstovall
| Branch: ELC_branch
| Sync to ELC_baseline_960205.
|
| Revision 3.2.2.1  1995/12/01  04:20:08  mbeesley
| Branch: ELC_branch
| Reorganize Rom monitor header files to make it easier to port
| to new platforms.
|
| Revision 3.5  1996/03/11  05:59:52  tkam
| CSCdi51218:  Need to implement fault history mechanism
| Added fault history support and misc. cleanup.
|
| Revision 3.4  1996/02/01  02:27:12  mansonw
| CSCdi47569:  HSA slave refuses to switch to master state
| Fixed the HSA slave to gain master during hot insertion/removal
|
| Revision 3.3  1996/01/17  23:31:16  mansonw
| CSCdi47058:  Fix the broken ROMMON build
|
| Revision 3.2  1995/11/17  18:41:28  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:05:08  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.2  1995/11/08  21:23:46  shaker
| Merge Arkansas_branch into 11.1 mainline.
|
| Revision 2.1  1995/10/06  23:11:22  szhang
| placeholder for DBUS handlers in RSP2
|
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Startup module for Monitor standalone programs that expect
** a system code interface
** Provide an entry point, zero the .bss section, and provide
** an exit routine for the standalone program.
**
** start(int code, struct plb *plb, struct pib *pib)
**
** The argument "code" is for launching a system image
** and is ignored here.
*/
#define ASMINCLUDE

#include "mon_boot.h"
#include "monlib.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#if defined(SVIP)
#include "../../src-svip/svip_pcmap.h"
#else
#include "../../src-rsp/rsp_pcmap.h"
#endif

	.align 2
	.lcomm save_ra,4
	.lcomm save_sp,4

	.globl mifp
	.comm mifp,4

	.globl	main
	.globl  exit

/*
** startup function
**
** int start(code, plb, pib)
*/
FRAME(start,sp,0,ra)
	.set	noreorder
	la	t0,_edata	# zero the .bss section
	la	t1,_end
clrbss:
	sb	zero,0(t0)	# zero a location
	addiu	t0,t0,1		# bump pointer
	blt	t0,t1,clrbss	# loop til done	
	nop

	lw	t0,PIB_MIFP(a2)	# pick up our mifp
	sw	ra,save_ra	# save the return address
	sw	sp,save_sp	# and the stack pointer

	sw	t0,mifp		# set our globl mifp

# The following 2 instructions would give an
# argc,argv program interface
#	lw	a0,PIB_ARGC(a2) # pick up our argc
#	lw	a1,PIB_ARGV(a2) # pick up our argv

	jal	main
	nop

	b	ret_exit
	nop
	.set	reorder
ENDFRAME(start)


/*
** The exit routine
*/

FRAME(exit,sp,0,ra)
        .set    noreorder
        move    v0,a0           # set return value
ret_exit:
        lw      ra,save_ra      # restore our return value
        lw      sp,save_sp      # and our stack pointer
        j       ra              # return
        nop
        .set    reorder
ENDFRAME(exit)

/*
** Provide a dummy routine __main for the
** brain dead tool chain.
*/
        .globl __main
FRAME(__main,sp,0,ra)
        .set    noreorder
        j       ra
        nop
        .set    reorder
ENDFRAME(__main)


.globl	vec_start
.globl	vec_end
.ent	vec_start

vec_start:

/*************************************************************************
 * 0x200 : TLB Refill (EXL = 0)
 */
        sd      k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)       # save k0
        li	k0,0xbfc00204
	j	k0                                      # jump to the handler
        nop                                             # (bd slot)

        .space  0x68

/*************************************************************************
 * 0x280 : XTLB Refill (EXL = 0, 64 bit)
 */
        sd      k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)       # save k0
        li	k0,0xbfc00284
        j       k0                                      # jump to the handler
        nop                                             # (bd slot)

        .space  0x68

/*************************************************************************
 * 0x300 : Refill Cache Error
 */
        sd      k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)       # save k0
        li	k0,0xbfc00304
        j       k0                                      # jump to the handler
        nop                                             # (bd slot)

        .space  0x68

/*************************************************************************
 * 0x380 : General Exception
 */
        sd      k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)       # save k0
        sd      k1,SP_OFS(REG_K1)-EXCPT_FRAME(sp)       # save k1
	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	la	k0,gen_handler				# get handler
	addiu	sp,sp,-EXCPT_FRAME			# make a frame
	la	k1,general_exception			# get handler pointer

	j	k0					# jump to the handler
	nop

vec_end:

.end vec_start


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
 * cache_flush : 
 * Flush both the Instruction and Data caches. If config == 0, then
 * do not alter config register. Else program config cache attribute
 * into the config register. If stack != -1, move stack to KSeg(Stack)
 *
 * void cache_flush(config, stack)
 *	ulong config, stack;
 */
FRAME(cache_flush,sp,0,ra)
	.set	noreorder

	/*
	 * Turn off interrupts
	 */
	mfc0	t8,CP0_SREG		# read status register
	li	t0,~SREG_IE		# get IE mask
	and	t0,t8,t0		# turn IE off
	mtc0	t0,CP0_SREG		# re-write status register

	/*
	 * First job is to do a write back invalidate on the D cache
	 */
	li	t0,ADRSPC_K0BASE	# get K0 base address
	li	t1,DCACHE_LINES		# get no. of lines in D cache
	li	t2,DLINE_SHIFT		# get the line shift for D cache

dcache_loop:
	blez	t1,do_icache		# finished ?
	nop				# (bd slot)	
	cache	1,0(t0)			# Index Write back invalidate D cache
	nop				# (paranoid)
	addiu	t1,t1,-1		# t1 -= 1
	addu	t0,t0,t2		# increment the line index

	b	dcache_loop		# do it again
	nop				# (bd slot)

do_icache:
	/*
	 * And the next thing is to do an Index Invalidate on I cache
	 */
	li	t0,ADRSPC_K0BASE	# get K0 base address
	li	t1,ICACHE_LINES		# get no. of lines in I cache
	li	t2,ILINE_SHIFT		# get the line shift for I cache

icache_loop:
	blez	t1,cache_done		# finished ?
	nop				# (bd slot)
	cache	0,0(t0)			# Index Invalidate I cache line
	nop				# (paranoid)
	addiu	t1,t1,-1		# t1 -= 1
	addu	t0,t0,t2		# increment the line index

	b	icache_loop		# do it again
	nop				# (bd slot)

cache_done:
	/*
	 * If we have been asked to change the config register, do it here
	 */
	beq	a0,zero,do_stack	# config != 0 ?
	mfc0	t0,CP0_CONFIG		# read config register (bd slot)

	/*
	 * Zero out KSeg0 Cache field and install new 
	 * attribute
	 */
	li	t1,~CONFIG_KSEG0_COHERENCY
	and	t0,t0,t1		
	or	t0,t0,a0		

	/*
	 * Setup for 32byte cache lines
	 */
	ori	t0,t0,(CONFIG_ICACHE_BLOCK|CONFIG_DCACHE_LINE)
		
	mtc0	t0,CP0_CONFIG		# re-write config register
	nop				# load delays
	nop
	
do_stack:
	/*
	 * If we have to move the stack, do it here
	 */
	addiu	t0,zero,-1
	beq	t0,a1,cache_finished	# should we move the stack ?
	nop				# (bd slot)

	beq	zero,a1,kseg0_stack
	nop

	/*
	 * Move the stack to KSeg1
	 */
	li	t0,0x1fffffff		# physical address mask
	and	sp,sp,t0		# get physical address
	li	t1,ADRSPC_K1BASE	# get KSeg1 base
	daddu	sp,sp,t1		# set stack to KSeg1

	b	cache_finished
	nop

kseg0_stack:
	/*
	 * Move the stack to KSeg0
	 */
	li	t0,0x1fffffff		# physical address mask
	and	sp,sp,t0		# get physical address
	li	t1,ADRSPC_K0BASE	# get KSeg0 base
	daddu	sp,sp,t1		# set stack to KSeg0

cache_finished:
	j	ra			# return
	mtc0	t8,CP0_SREG		# re-write sreg (bd slot)

	.set	reorder
ENDFRAME(cache_flush)


/*
** The monitor system call interface for stand alone programs
**
** Note:
** In the code:
**
**	li	t0,MIF_OPEN
**	b	do_moncall
**	nop
**
** The programmer may be tempted to do the "li" in the branch
** delay slot.  This will work if the immediate data is 16 bits
** but will break with no warning if the data is ever bigger
** than 16 bits.  These wrappers were coded to be safe.
*/
	.globl getdevnum
FRAME(getdevnum,sp,0,ra)
	.set	noreorder
	li	t0,MIF_RA_GETDEVNUM # pick up the offset into jump table
do_moncall:
	lw	t1,mifp		# pick up the jump table pointer
	nop
	addu	t0,t0,t1	# add in the offset
	lw	t1,0(t0)	# pick up routine vector
	nop
	j	t1		# jump to it
	nop
	.set	reorder
ENDFRAME(getdevnum)

	.globl open
FRAME(open,sp,0,ra)
	.set	noreorder
	li	t0,MIF_OPEN
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(open)

	.globl close
FRAME(close,sp,0,ra)
	.set	noreorder
	li	t0,MIF_CLOSE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(close)

	.globl read
FRAME(read,sp,0,ra)
	.set	noreorder
	li	t0,MIF_READ
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(read)

	.globl raw_read
FRAME(raw_read,sp,0,ra)
	.set	noreorder
	li	t0,MIF_RA_READ
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(raw_read)

	.globl write
FRAME(write,sp,0,ra)
	.set	noreorder
	li	t0,MIF_WRITE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(write)

	.globl lseek
FRAME(lseek,sp,0,ra)
	.set	noreorder
	li	t0,MIF_LSEEK
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(lseek)

	.globl stat
FRAME(stat,sp,0,ra)
	.set	noreorder
	li	t0,MIF_STAT
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(stat)

	.globl fstat
FRAME(fstat,sp,0,ra)
	.set	noreorder
	li	t0,MIF_FSTAT
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(fstat)

	.globl printf
FRAME(printf,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PRINTF
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(printf)

	.globl sprintf
FRAME(sprintf,sp,0,ra)
	.set	noreorder
	li	t0,MIF_SPRINTF
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(sprintf)

	.globl puts
FRAME(puts,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PUTS
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(puts)

	.globl putchar
FRAME(putchar,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PUTCHAR
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(putchar)

	.globl getchar
FRAME(getchar,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETCHAR
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getchar)

	.globl getline
FRAME(getline,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETLINE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getline)

	.globl shcmdline
FRAME(shcmdline,sp,0,ra)
	.set	noreorder
	li	t0,MIF_SHCMDLINE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(shcmdline)

	.globl getfreememstart
FRAME(getfreememstart,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETFREEMEMSTART
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getfreememstart)

	.globl launch
FRAME(launch,sp,0,ra)
	.set	noreorder
	li	t0,MIF_LAUNCH
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(launch)


/*
** Provide the EMT call interface for backward compatibility
**
** emt_call
** ROM monitor emulation trap support
**
** int emt_call(traptype[,arg1[,arg2[,arg3]]])
*/
	.globl emt_call
FRAME(emt_call,sp,0,ra)
	.set	noreorder
	syscall
	nop
        j       ra              # return
        nop
	.set	reorder
ENDFRAME(emt_call)
