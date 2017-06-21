/* $Id: start.S,v 3.2.58.1 1996/03/21 23:32:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/start.S,v $
 *------------------------------------------------------------------
 * start.S
 *
 * Mar 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * RSP start-up file with 64 bit support. 
 *------------------------------------------------------------------
 * $Log: start.S,v $
| Revision 3.2.58.1  1996/03/21  23:32:33  gstovall
| Branch: California_branch
| The ELC_branch now knows the joy of California dreaming.
|
| Revision 3.2.2.1  1995/12/01  04:24:25  mbeesley
| Branch: ELC_branch
| Reorganize Rom monitor header files to make it easier to port
| to new platforms.
|
| Revision 3.2  1995/11/17  18:47:03  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:09:09  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  22:41:25  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "signal_4k.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "regfpga.h"

#include "../../src-rsp/rsp_pcmap.h"

#define INIT_SREG SREG_CU0|SREG_CU1|SREG_BEV|SREG_DE

#define MEMORY_SIZE_64K	PHY_TO_KSEG1(0x10000)
#define	MEMORY_DATA_AA	0xAAAAAAAA
#define	MEMORY_DATA_55	0x55555555
#define	MEMORY_DATA_99	0x99999999
#define	MEMORY_DATA_66	0x66666666


	.text
	
	.globl evec
	.globl evece
	.globl _xstart
	.globl __init
	.globl __init_done
	.globl vec_start
	.globl vec_end
	.globl cache_exception
	.globl general_exception
	.globl monitor_stk
	.globl sys_dirty

/*
 * Startup code for the Monitor on RSP
 * Jump to the startup code
 */
start:
	.set noreorder
	.set noat
evec:					# base address : 0xBFC00000
	j 	_xstart			# 0x00 : Reset, Soft Reset, NMI
	nop
	.space	0x1F8			

vec_start:

/*************************************************************************
 * 0x200 : TLB Refill (EXL = 0)
 */
	sd	k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)	# save k0
	sd	k1,SP_OFS(REG_K1)-EXCPT_FRAME(sp)	# save k1
	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	la	k0,gen_handler				# get pointer to table
	addiu	sp,sp,-EXCPT_FRAME			# make a frame
	la	k1,general_exception			# upper-level handler

	j	k0					# jump to the handler
	nop						# (bd slot)

	.space	0x54

/*************************************************************************
 * 0x280 : XTLB Refill (EXL = 0, 64 bit)
 */
	sd	k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)	# save k0
	sd	k1,SP_OFS(REG_K1)-EXCPT_FRAME(sp)	# save k1
	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	la	k0,gen_handler				# get handler
	addiu	sp,sp,-EXCPT_FRAME			# make a frame
	la	k1,general_exception			# upper-level handler

	j	k0					# jump to the handler
	nop						# (bd slot)

	.space	0x54

/*************************************************************************
 * 0x300 : Refill Cache Error
 */
	sd	k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)	# save k0
	sd	k1,SP_OFS(REG_K1)-EXCPT_FRAME(sp)	# save k1
	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	la	k0,gen_handler				# get handler
	addiu	sp,sp,-EXCPT_FRAME			# make a frame
	la	k1,cache_exception			# upper level handler

	j	k0					# jump to the handler
	nop						# (bd slot)

	.space 	0x54

/*************************************************************************
 * 0x380 : General Exception
 */
	sd	k0,SP_OFS(REG_K0)-EXCPT_FRAME(sp)	# save k0
	sd	k1,SP_OFS(REG_K1)-EXCPT_FRAME(sp)	# save k1
	sd	ra,SP_OFS(REG_RA)-EXCPT_FRAME(sp)	# save ra
	sd	sp,SP_OFS(REG_SP)-EXCPT_FRAME(sp)	# save sp

	la	k0,gen_handler				# get handler
	addiu	sp,sp,-EXCPT_FRAME			# make a frame
	la	k1,general_exception			# get handler pointer

	j	k0
	nop						# (bd slot)

	.set	at
	.set	reorder
evece:

/*
 * speed_bd : Speed loop with the read instruction in the branch delay slot
 *
 * int speed_bd(start, end)
 *	unsigned int *start, *end;
 */
FRAME(speed_bd,sp,0,ra)
	.set	noreorder
2:
	subu	t0,a1,a0		# compare start and end
	blez	t0,1f			# if ( (end-start) <= 0 ) branch
	addiu	a0,4			# start += 1;
	b	2b			# branch
	lw	t1,-4(a0)		# t1 = *(start-1)
1:
	j	ra			# return
	move	v0,zero			# set return to success
	.set	reorder
ENDFRAME(speed_bd)

/*
 * speed_nobd : Speed loop with the read instruction NOT in branch delay slot
 *
 * int speed_nobd(start, end)
 *	unsigned int *start, *end;
 */
FRAME(speed_nobd,sp,0,ra)
	.set	noreorder
2:
	subu	t0,a1,a0		# compare start and end
	blez	t0,1f			# if ( (end-start) <= 0 ) branch
	addiu	a0,4			# start += 1;
	lw	t1,-4(a0)		# t1 = *(start-1)
	b	2b			# branch
	nop	
1:
	j	ra			# return
	move	v0,zero			# set return to success
	.set	reorder
ENDFRAME(speed_nobd)

#ifndef RSP
/*
 * mem_verify:
 * Fast memory verification for DRAM test
 *
 * If (mode == 0) 
 *    Loop no. of passes
 *        Write address, read address and verify
 * Else if (mode == 1)
 *    Fill memory with pattern (data is address)
 *    Loop no. of passes or inifinity
 *	  Read memory and verify
 * Else if (mode == 2)
 *    Fill memory (odd addr == -1, even addr == 0)
 *    Loop no. of passes or infinity
 *        Read memory and verify
 *
 * void mem_verify(long *addr, long *end, long pass, long mode)
 */
FRAME(mem_verify,sp,64,ra)
	.set 	noreorder

	addiu	sp,sp,-64
	sw	a0,16(sp)
	sw	a1,20(sp)
	sw	a2,24(sp)
	sw	a3,28(sp)
	sw	s0,32(sp)
	sw	s1,36(sp)
	sw	s2,40(sp)
	sw	ra,44(sp)

	li	t0,3
	beq	t0,a3,split_mode
	nop

	li	t0,2
	beq	t0,a3,alt_mode
	nop

	li	t0,1
	beq	t0,a3,read_mode
	nop

write_mode:

	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address

	/*
	 * write/read loop
	 */
wr_loop:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,wr_end	# done ?
	nop			# (bd slot)

	sw	s0,0(s0)	# write the address as data
	nop			
	nop
	nop
	lw	a2,0(s0)	# read it back
	nop			# load delay
	
	beq	s0,a2,wr_ok	# check it
	nop			# (bd slot)

	la	t4,FPGA_LED_REG		# get LED reg addr
	lh	t5,0(t4)		# read it
	li	t6,~LED0		# LED 0 on value
	li	t7,LED0			# LED 0 off value

	and	t8,t5,t6		# turn it on (0)
	sh	t8,0(t4)

	nop
	or	t8,t5,t7		# turn it off (1)
	sh	t8,0(t4)
	nop

	la	t0,dram_mem_error	# function address
	move	a0,s0			# addr
	move	a1,s0			# expected data
	jal	t0			# call error reporter
	nop

wr_ok:
	b	wr_loop		# continue
	addiu	s0,s0,4		# increment the address (bd slot)
	
wr_end:
	/*
	 * Should we do another pass ?
	 */
	lw	a2,24(sp)	# read pass count
	li	t0,0x01		# load "1"
	beq	a2,t0,ver_done	# finished ?
	nop			# (bd slot)

	addiu	a2,a2,-1	# subtract "1"
	blez	a2,wpass_inf	# are we set up for infinite passes ?
	nop			# (bd slot)

	sw	a2,24(sp)	# save our count
	b	write_mode	# do it again
	nop			# (bd slot)

wpass_inf:
	b	write_mode	# do it again
	sw	zero,24(sp)	# save infinite pass count (bd slot)

	/*
	 * Fill memory with data == address. Read back and verify
	 */
read_mode:
	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address

	/*
	 * Fill the block with the address
	 */
rd_fill:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,rd_end1	# done filling memory ?
	nop			# (bd slot)

	sw	s0,0(s0)	# write the address as data

	b	rd_fill		# continue
	addiu	s0,s0,4		# increment the address (bd slot)
	
rd_end1:

	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address

	/*
	 * Read the block and check
	 */
rd_loop:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,rd_end	# done with this pass ?
	nop			# (bd slot)

	lw	a2,0(s0)	# read the address
	nop			# load delay

	beq	a2,s0,rd_ok	# read ok ?
	nop

	la	t4,FPGA_LED_REG		# get the LED reg address
	lh	t5,0(t4)
	li	t6,~LED0		# LED on value
	li	t7,LED0			# LED off value
	and	t8,t5,t6
	sh	t8,0(t4)
	nop
	or	t8,t5,t7
	sh	t8,0(t4)
	nop

	la	t0,dram_mem_error	# function address
	move	a0,s0			# addr
	move	a1,s0			# expected data
	jal	t0			# call error reporter
	nop

rd_ok:
	b	rd_loop		# continue with the next one ?
	addiu	s0,s0,4		# increment the address (bd slot)
	
rd_end:
	/*
	 * Should we do another pass ?
	 */
	lw	a2,24(sp)	# read pass count
	li	t0,0x01		# load "1"
	beq	a2,t0,ver_done	# finished ?
	nop			# (bd slot)

	addiu	a2,a2,-1	# subtract "1"
	blez	a2,rpass_inf	# are we set up for infinite passes ?
	nop			# (bd slot)

	sw	a2,24(sp)	# save our count
	b	rd_end1		# do it again
	nop			# (bd slot)

rpass_inf:
	b	rd_end1		# do it again
	sw	zero,24(sp)	# save infinite pass count (bd slot)

	/*
	 * Fill memory. Odd addr = -1, even addr = 0.
	 * Read back and verify
	 */
alt_mode:
	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address

	/*
	 * Fill the block with the address
	 */
alt_fill:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,altend1 # done filling memory ?
	nop			# (bd slot)

	andi	t0,s0,0x04	# and address with 0x04
	beq	zero,t0,aeven1	# address even ?
	nop			# (bd slot)

	li	t0,0xffffffff	# load -1
	sw	t0,0(s0)	# write -1

	b	alt_fill	# continue
	addiu	s0,s0,4		# increment the address (bd slot)

	
aeven1:
	sw	zero,0(s0)	# write zero into location

	b	alt_fill	# continue
	addiu	s0,s0,4		# increment the address (bd slot)
	
altend1:

	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address

	/*
	 * Read the block and check
	 */
alt_loop:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,alt_end	# done with this pass ?
	nop			# (bd slot)

	lw	a2,0(s0)	# read the address
	nop			# load delay

	andi	t0,s0,0x04	# and address with 0x04
	beq	t0,zero,aeven2	# address even ?
	nop			# (bd slot)

	li	t0,0xffffffff	# load -1
	b	a2_check
	nop			# (bd slot)

aeven2:
	move	t0,zero		# get zero

a2_check:
	beq	t0,a2,alt_ok	# read ok ?
	nop

	la	t4,FPGA_LED_REG
	lh	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sh	t8,0(t4)
	nop
	or	t8,t5,t7
	sh	t8,0(t4)
	nop

	move	a1,t0			# expected data
	la	t0,dram_mem_error	# function address
	move	a0,s0			# addr
	jal	t0			# call error reporter
	nop

alt_ok:
	b	alt_loop	# continue with the next one ?
	addiu	s0,s0,4		# increment the address (bd slot)
	
alt_end:
	/*
	 * Should we do another pass ?
	 */
	lw	a2,24(sp)	# read pass count
	li	t0,0x01		# load "1"
	beq	a2,t0,ver_done	# finished ?
	nop			# (bd slot)

	addiu	a2,a2,-1	# subtract "1"
	blez	a2,apass_inf	# are we set up for infinite passes ?
	nop			# (bd slot)

	sw	a2,24(sp)	# save our count
	b	altend1		# do it again
	nop			# (bd slot)

apass_inf:
	b	altend1		# do it again
	sw	zero,24(sp)	# save infinite pass count (bd slot)

	/*
	 * Fill memory with data == address. Read back and verify
	 * Split the address range in two, doing alternate spaces
	 */
split_mode:
	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address
	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address
	subu	s2,s1,s0	# s2 = end - start
	srl	s2,s2,1		# divide by two
	addu	s1,s0,s2	# new end address

	/*
	 * Fill the block with the address
	 */
split_fill:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,sp_end1	# done filling memory ?
	nop			# (bd slot)

	sw	s0,0(s0)	# write the address as data
	addu	t0,s0,s2	# get half way up address range
	sw	t0,0(t0)	# write address as data

	b	split_fill	# continue
	addiu	s0,s0,4		# increment the address (bd slot)
	
sp_end1:

	lw	s0,16(sp)	# get starting address
	lw	s1,20(sp)	# get ending address
	subu	s2,s1,s0	# s2 = end - start
	srl	s2,s2,1		# divide by two
	addu	s1,s0,s2	# new end address

	/*
	 * Read the block and check
	 */
sp_loop:
	sltu	t0,s1,s0	# if (addr > end) t0 = 1
	bne	zero,t0,sp_end	# done with this pass ?
	nop			# (bd slot)

	lw	a2,0(s0)	# read the address
	nop			# load delay

	beq	a2,s0,sp_ok	# read ok ?
	nop

	la	t4,FPGA_LED_REG
	lh	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sh	t8,0(t4)
	nop
	or	t8,t5,t7
	sh	t8,0(t4)
	nop

	la	t0,dram_mem_error	# function address
	move	a0,s0			# addr
	move	a1,s0			# expected data
	jal	t0			# call error reporter
	nop

sp_ok:
	addu	t0,s0,s2	# get address half way up range
	lw	a2,0(t0)	# read the address
	nop			# load delay

	beq	a2,t0,sp_ok1	# read ok ?
	nop

	la	t4,FPGA_LED_REG
	lh	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sh	t8,0(t4)
	nop
	or	t8,t5,t7
	sh	t8,0(t4)
	nop

	la	t1,dram_mem_error	# function address
	move	a0,t0			# addr
	move	a1,t0			# expected data
	jal	t1			# call error reporter
	nop

sp_ok1:
	b	sp_loop		# continue with the next one ?
	addiu	s0,s0,4		# increment the address (bd slot)
	
sp_end:
	/*
	 * Should we do another pass ?
	 */
	lw	a2,24(sp)	# read pass count
	li	t0,0x01		# load "1"
	beq	a2,t0,ver_done	# finished ?
	nop			# (bd slot)

	addiu	a2,a2,-1	# subtract "1"
	blez	a2,spass_inf	# are we set up for infinite passes ?
	nop			# (bd slot)

	sw	a2,24(sp)	# save our count
	b	sp_end1		# do it again
	nop			# (bd slot)

spass_inf:
	b	sp_end1		# do it again
	sw	zero,24(sp)	# save infinite pass count (bd slot)

ver_done:
	lw	ra,44(sp)
	lw	a0,16(sp)
	lw	a1,20(sp)
	lw	a2,24(sp)
	lw	a3,28(sp)
	lw	s0,32(sp)
	lw	s1,36(sp)
	lw	s2,40(sp)
	addiu	sp,sp,64

	j	ra	
	nop

	.set	reorder
ENDFRAME(mem_verify)

#endif

vec_end:

/*
 * The remainder lives in EPROM always
 */

/*
 * r4k_exception_table:
 * This table is used only by loaded images. During initialization and
 * for EMT call's into the ROM monitor, the loaded image will copy the 
 * appropriate vector from the table below, into its own virtual exception
 * table. Hence, when an interrupt occurs (during image initialization)
 * or when an EMT call is issued, the monitors handler will be dispatched.
 * This handler must accomodate for the manner in which context is saved
 * by the loaded image.
 */
	.text
	.align 3

	.globl r4k_exception_table

r4k_exception_table:
	.word	system_handler		/* interrupt 		*/
	.word	gen_handler		/* TLB mod		*/
	.word	gen_handler		/* TLB Load		*/
	.word	gen_handler		/* TLB Store 		*/
	.word	gen_handler		/* Addr Err (load) 	*/
	.word	gen_handler		/* Addr Err (store)	*/
	.word	gen_handler		/* Bus Err (fetch)	*/
	.word	gen_handler		/* Bus Err (load)	*/
	.word	system_handler		/* System call		*/
	.word	gen_handler		/* Break point		*/
	.word	gen_handler		/* Illegal opcode	*/
	.word	gen_handler		/* Coporcessor unusable	*/
	.word	gen_handler		/* Arithmetic overflow	*/
	.word	gen_handler		/* Trap			*/
	.word	gen_handler		/* Virtual Coherency	*/
	.word	gen_handler		/* FPU exception	*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Watch exception	*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Reserved		*/
	.word	gen_handler		/* Virtual Coherency	*/
	.word	gen_handler		/* Cache error		*/
	.word	gen_handler		/* TLB exception	*/
	.word	gen_handler		/* XTLB exception	*/

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
 * Startup point: The code branches here on power-on reset or on a
 * soft reset (watchdog timeout). Unfortunately, we do have to destroy 
 * some of the context (t0,k0,k1) before finding out which case brought 
 * us here. Maybe this can be improved later.
 */
_xstart:
	.set	noreorder
	.set	noat

	mfc0	k0,CP0_SREG			# get the status register
	li	k1,INIT_SREG			# set Bootstrap vectors, 
						# Cp0,1 usable. 32 fpu regs
	mtc0	k1,CP0_SREG
	nop
	mtc0	zero,CP0_CAUSE			# zero Cause Register
	nop

	li	k1,SREG_SR			# get Soft Reset bit mask
	and	t0,k0,k1			# and status reg with SR mask
	bgtz	t0,soft_reset  			# branch if bit set
	nop


/*
 * Description:
 *
 *       This test will store the data Pattern in the specified
 *       address of each location. The test works on ascending order.
 *
 *	t1 = 0xAAAAAAAA55555555
 *	t2 = 0x9999999966666666
 */
	.set	at

        li      t0, 1			/* stage 1 */
        li      t1, FPGA_LED_REG
        sb      t0, 0(t1)
	
	li	t0, MEMORY_SIZE_64K
	li	t1, MEMORY_DATA_AA	/* First data pattern */
	dsll	t1, 32
	or	t1, MEMORY_DATA_55	/* t1 = 0xAAAAAAAA55555555 */
	li	t2, MEMORY_DATA_99	/* Second data pattern */
	dsll	t2, 32
	or	t2, MEMORY_DATA_66	/* t2 = 0x9999999966666666 */
	li	t4, KSEG1_ADRSPC_RAM    /* beginning of the dram */
	add	t3, t0, -16		/* Top of address -16 */
WriteAddress:
	sd	t1, 8(t3)
	sd	t2, 0(t3)
	bne	t3, t4,  WriteAddress
	addiu	t3, t3, -16

	add	t3, t0, -16		/* Top of address -16 */
CheckAddress:
	ld	a0, 8(t3)
	bne	t1, a0, Pattern1Fail
	ld	v0, 0(t3)
	bne	t2, v0, Pattern2Fail
	move	a0, v0
	bne	t3, t4, CheckAddress
	addiu	t3, t3, -16

	j	__init_call
	nop

Pattern1Fail:
	li	a0, 1
	j	lit_errorLEDs
	nop

Pattern2Fail:
	li	a0, 2
	j	lit_errorLEDs
	nop

	.set	noat

__init_call:
	li	sp,MON_INIT_SP-STACK_PARAMS	# initialize stack pointer
	addiu	a0,zero,RESET_COLD		# setup parameter 1, reset_type

	jal	init				# initialize the box
	move	a1,zero				# context pointer (NULL) (bd)
	nop

__init_done:
	lw	sp,monitor_stk			# setup stack pointer
	jal	main
	nop	

	b	__init_done			# keep calling main
	nop
	nop

soft_reset:
	move	k1,sp				# preserve the old stk pointer
	li	sp,MON_INIT_SP-STACK_PARAMS	# initialize stack pointer
	
	/*
	 * Stack the World
	 */
	addiu	sp,sp,-EXCPT_FRAME		# make an exception frame
	sd	AT,((REG_AT-1)*4)(sp)		# save AT
	sd	k1,((REG_SP-1)*4)(sp)		# save stack pointer
	sd	zero,SP_OFS(REG_ZERO)(sp)	# save zero register
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
	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9
	sd	s0,SP_OFS(REG_S0)(sp)		# save s0
	sd	s1,SP_OFS(REG_S1)(sp)		# save s1
	sd	s2,SP_OFS(REG_S2)(sp)		# save s2
	sd	s3,SP_OFS(REG_S3)(sp)		# save s3
	sd	s4,SP_OFS(REG_S4)(sp)		# save s4
	sd	s5,SP_OFS(REG_S5)(sp)		# save s5
	sd	s6,SP_OFS(REG_S6)(sp)		# save s6
	sd	s7,SP_OFS(REG_S7)(sp)		# save s7
	sd	s8,SP_OFS(REG_S8)(sp)		# save s8
	sd	gp,SP_OFS(REG_GP)(sp)		# save gp
	sd	ra,SP_OFS(REG_RA)(sp)		# save ra
	mfc0	t0,CP0_SREG			# get status reg
	dmfc0	t1,CP0_EPC			# get EPC reg
	sw	t0,SP_OFS(REG_SREG)(sp)		# save status reg
	dmfc0	t2,CP0_BADVADDR			# get BadVAddr
	sd	t1,SP_OFS(REG_EPC)(sp)		# save EPC reg
	dmfc0	t0,CP0_ERR_EPC			# get Error EPC reg
	sd	t2,SP_OFS(REG_BADVADDR)(sp)	# save BadVAddr reg
	mfc0	t1,CP0_CAUSE			# get Cause reg
	sd	t0,SP_OFS(REG_ERR_EPC)(sp)	# save Error EPC reg
	move	a1,sp				# setup pointer to frame
	sw	t1,SP_OFS(REG_CAUSE)(a1)	# save Cause register

	/*
	 * Setup the parameters and call init. Don't
	 * have to worry about resetting the stack pointer
	 * as init will return to init_done, which will re-initialize it
	 */
	addiu	a0,zero,RESET_NMI		# setup parameter 1, reset_type
	jal	init				# initialize the box
	nop

	.set	at
	.set	reorder

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
 * do_reset : Cold reset the machine ; does not return
 * void do_reset()
 */
/*
FRAME(do_reset,sp,0,ra)
	.set	noreorder

	li	t0, FPGA_SYSRST_REG
	sb	zero, 0(t0)

	nop
	nop
	nop

	.set 	reorder
ENDFRAME(do_reset)
*/


/*
 * do_reset : soft reset the machine ; does not return
 * void do_reset()
 */
FRAME(do_reset,sp,0,ra)
	.set	noreorder
	
	li	k1,INIT_SREG			# set Bootstrap vectors, 
						# Cp0 and 1 usable
	mtc0	k1,CP0_SREG
	nop
	mtc0	zero,CP0_CAUSE			# zero Cause Register
	nop

	li	sp,MON_INIT_SP-STACK_PARAMS	# initialize stack pointer
	addiu	a0,zero,RESET_WARM		# setup parameter 1, reset_type
	move	a1,zero				# context pointer (NULL)
	jal	init				# initialize the box
	nop
	nop
	.set 	reorder
ENDFRAME(do_reset)

/*
 * lights : Pulse out an error number on the RUN LED, never returns
 * a0 = no of pulses
 */
FRAME(lights,sp,0,ra)
	.set noreorder

	move	a1,a0				# get no. of flashes
	li	t3,LEDS_OFF			# get run lamp bit (off)
	li	t4,LEDS_ON			# get run lamp bit (on)
	li	t0,FPGA_LED_REG			# get control register addr

	lbu	t8,0(t0)			# read control register
	nop

lights_loop:
	or	t7,t8,t3			# turn lamp off
	sb	t7,0(t0)			# set the register

	li	a3,0x40000			# get spin count
lights1:
	addiu	a3,a3,-1			# decrement a3
	bne	a3,zero,lights1			# finished ?
	nop					# (bd slot)

	and	t7,t8,t4			# turn lamp on
	sb	t7,0(t0)			# write the register

	li	a3,0x40000			# get spin count
lights2:
	addiu	a3,a3,-1			# decrement a3
	bne	a3,zero,lights2			# finished ?
	nop					# (bd slot)

	addiu	a1,a1,-1
	bnez	a1,lights_loop			# do another flash ?
	nop					# (bd slot)

	or	t7,t8,t3			# turn lamp off
	sb	t7,0(t0)			# set the register
	li	a3,0x80000			# get spin count
lights3:
	addiu	a3,a3,-1			# decrement a3
	bne	a3,zero,lights3			# finished ?
	nop					# (bd slot)

	b	lights				# start again
	nop					# (bd slot)

ENDFRAME(lights)


/*
 * lit_errorLEDs : Turn on LED error code on the UNUSED LEDs, never returns
 * a0 = code in hex form
 */
FRAME(lit_errorLEDs,sp,0,ra)
	.set noreorder
        xor     t0, a0, 0xFF			# LOW is on
	and	t0, t0, 0x80			# turn on the MSB bit for error
        li      t1, FPGA_LED_REG
        sb      t0, 0(t1)

error_loop:
        b       error_loop                      # loop forever
        nop                                     

ENDFRAME(lit_errorLEDs)


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
 * Initialize data cache
 * void init_dcache(void)
 *      
 */
FRAME(init_dcache,sp,0,ra)
	.set	noreorder

/* Setup base address, loop counter and the TagLo registers */
	lui	t1, CACHEADDR   # cacheable virtual address
	li	t0, CACHELEN    # set up loop counter
	mtc0	zero, CP0_TAGLO	# setup TagLo to invalidate tags 

/* Walk through DCache doing Index_Store_Tag. Tag now invalid */
dcache_init_loop:
	cache	DC_INDEX_STTAG+DCACHE, 0x0(t1)    # index store tag, set0 
	cache   DC_INDEX_STTAG+DCACHE, 0x2000(t1) # index store tag, set1

/* Now clear all dirty bits and set data and parity to known good values */
	lw	zero, 0x0(t1)   # clear dirty bits and set data and parity
	lw	zero, 0x2000(t1) # to known good values 

/* Re-invalidate the entries */
	cache 	DC_INDEX_STTAG+DCACHE, 0x0(t1)     # index store tag - set 0
	cache   DC_INDEX_STTAG+DCACHE, 0x2000(t1)  # index store tag - set 1

/* Finish the loop */
	addu	t1, 0x20		# increment address pointer
	bgtz	t0, dcache_init_loop    # see if loop done
	addi	t0, -1			# decrement loop counter 
	
	j	ra
	nop
	.set	reorder
ENDFRAME(init_dcache)


/*
 * Initialize instruction cache
 * void init_icache(void)
 *      
 */
FRAME(init_icache,sp,0,ra)
	.set	noreorder

/* Setup base address, loop counter and the TagLo registers */
	lui	t1, CACHEADDR   # cacheable virtual address
	li	t0, CACHELEN    # set up loop counter
	mtc0	zero, CP0_TAGLO	# setup TagLo to invalidate tags 

/* Walk through ICache doing Index Store Tag. Tag now invalid */
icache_init_loop:
	cache	IC_INDEX_STTAG+ICACHE, 0x0(t1)	  # index store tag, set0
	cache   IC_INDEX_STTAG+ICACHE, 0x2000(t1) # index store tag, set1

/* Next fill the data area with good parity */
	cache 	IC_FILL+ICACHE, 0x0(t1)		# fill Icache data from memory
	cache   IC_FILL+ICACHE, 0x2000(t1)	# fill Icache data from memory

/* Re-invalidate the tags */
	cache 	IC_INDEX_STTAG+ICACHE, 0x0(t1)    # index store tag, set0
	cache   IC_INDEX_STTAG+ICACHE, 0x2000(t1) # index store tag, set1

/* Finish the loop */
	addu	t1, 0x20		# increment address pointer
	bgtz	t0, icache_init_loop	# jump back if count < last addr
	addi	t0, -1			# decrement loop counter
	
	j	ra
	nop
	.set	reorder
ENDFRAME(init_icache)


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

/*
 * Storage for the monitors stack pointer, 8 bytes
 */
	.align 3
	.comm mon_stack,8
	.comm mon_sr,8

	.globl	sys_state

/*
 * launch : Launch a loaded image
 * void launch (code, plb, pib, entry)
 */
FRAME(launch,sp,80,ra)
	.set	noreorder
	addiu	sp,sp,-80		# create a frame
	sd	ra,16(sp)		# save ra
	sd	a0,24(sp)		# save code
	sd 	a1,32(sp)		# save plb
	sd 	a2,40(sp)		# save pib
	sd 	a3,48(sp)		# save entry

	move	a0,zero			# set param1 to zero
	li	a1,-1			# set param2 to -1
	jal	cache_flush		# flush I/D caches
	nop

	ld	a0,24(sp)		# restore code
	ld 	a1,32(sp)		# restore plb
	ld 	a2,40(sp)		# restore pib
	ld 	a3,48(sp)		# restore entry

	la	t0,mon_stack		# get address of sp save area
	sw	sp,0(t0)		# save the stack pointer

	la	t0,mon_sr		# get address of sr save area
	mfc0	k0,CP0_SREG		# get the status register
	sw	k0,0(t0)		# save the sr

	addiu	t1,zero,SYSTEM_RUNNING	# set t1 to SYSTEM_RUNNING
	la	t0,sys_state		# get address of sys_state
	sw	t1,0(t0)		# set sys_state to SYSTEM_RUNNING

	li	t1,1
	la	t0,sys_dirty
	sb	t1, 0(t0)		# set dirty flag

	move	sp,a3			# setup scheduler stack pointer
	daddiu	sp,sp,-64		# 64 bytes before entry point
	li	t0,-7			# mask for 64bit alignment
	and	sp,sp,t0		# make sure sp is aligned

	jal	a3			# call the system code
	nop

	la	t0,mon_stack		# get address of sp save area
	lw	sp,0(t0)		# restore our sp

	la	t0,mon_sr		# get address of sr save area
	lw	k0,0(t0)
	mtc0	k0,CP0_SREG		# restore our sr

	sd	v0,24(sp)		# save return code from launched image
/*	jal	reset_io	*/	# kill the interfaces
	nop
	move	a0,zero			# set param1 to zero
	li	a1,-1			# set param2 to -1
	jal	cache_flush		# flush I/D caches
	nop
	jal	disconint		# disable console interrupts
	nop
	jal	enacon			# enable console xmitter and receiver
	nop
	ld	v0,24(sp)		# restore return code from launched image

	addiu	t1,zero,MONITOR_RUNNING	# set t1 to MONITOR_RUNNING
	la	t0,sys_state		# get address of sys_state
	ld	ra,16(sp)		# restore ra
	sw	t1,0(t0)		# set sys_state to MONITOR_RUNNING
	j	ra			# return
	addiu	sp,sp,80		# remove stack frame
	.set	reorder
ENDFRAME(launch)

/*
 * relaunch : relaunch a loaded image
 * void relaunch (context)
 *	struct context_t *context;
 */
FRAME(relaunch,sp,0,ra)
	.set	noreorder
	la	t0,sys_state		# get address of sys_state
	addiu	t1,zero,SYSTEM_RUNNING	# set t1 to SYSTEM_RUNNING
	sw	t1,0(t0)		# set sys_state to SYSTEM_RUNNING
	b	sys_restart		# restart the system code
	move	sp,a0			# set stack pointer to context
	.set	reorder
ENDFRAME(relaunch)


/***********************************************
**	Delay_us(n): Delay in 10 * n microseconds
**      Spin for approximately n microseconds.  This loop may need to be
**      retimed for different platforms. The value entered has to be 1
**      or greater in order for this function to work, 0 is not valid!!
**
**      For millisecond, just set a value of 100us, and so on.
************************************************/
FRAME(delay_10us, sp, 0, ra)
        sll     a0, a0, 1
1:
        addu    a0, a0, -1
        bgt     a0, zero, 1b
        nop

        j       ra
        nop
ENDFRAME(delay_10us)


/* End of Module */


