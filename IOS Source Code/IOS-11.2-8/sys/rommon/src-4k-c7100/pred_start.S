/* $Id: pred_start.S,v 3.2.58.7 1996/07/18 17:49:01 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_start.S,v $
 *------------------------------------------------------------------
 * pred_start.S -- Predator Rom monitor init code
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_start.S,v $
 * Revision 3.2.58.7  1996/07/18  17:49:01  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010CSCdi63193: Teach the rom
 * monitor
 * about the incompatibilities between rev1 and rev2 gt64010s :
 *         o Addressing flip for bytes accessed thru PCI IO space
 *         o Internal PCI config register addressing differences
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/05/22  17:06:05  szhang
 * CSCdi58089:  ROM Monitor does not survive a Push Button Reset
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/04/30  17:25:14  szhang
 * CSCdi55355:  fault tolerant boot strategy causes boot cycle
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/04/20  01:11:00  szhang
 * CSCdi55174:  Reset Reason Reg is not read correctly
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/04/09  23:09:42  szhang
 * CSCdi53689:  Open() failed in PCMCIA monlib
 * Branch: California_branch
 * Monitor shouldn't reset the PCMCIA controller after the MONLIB
 * is launched.
 *
 * Revision 3.2.58.2  1996/04/04  17:06:00  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:31:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/03/07  00:51:55  szhang
 * CSCdi50915:  ROM monitor hang after reload
 * Branch: ELC_branch
 * The cause is that the port adaptors are not
 * shut down.
 *
 * Revision 3.2.2.7  1996/02/16  21:41:50  szhang
 * CSCdi49173:  Separate the startup and exception code
 * Branch: ELC_branch
 * Separate the startup and exception code into platform
 * dependent and platform independent files.
 *
 * Revision 3.2.2.6  1996/01/05  23:19:24  mansonw
 * Branch: ELC_branch
 *
 * Add memory sizing code.
 *
 * Revision 3.2.2.5  1995/12/08  05:00:06  mbeesley
 * Branch: ELC_branch
 * Setup first 5 TLB entiries, init PCI, PCMCIA and ralib. Clean up
 * exception/interrupt handling, fix NVRAM handling.
 *
 * Revision 3.2.2.4  1995/12/05  00:54:21  mbeesley
 * Branch: ELC_branch
 * Add support for the L2 cache.
 *
 * Revision 3.2.2.3  1995/12/01  21:31:34  mbeesley
 * Branch: ELC_branch
 * Fix LED register handling, add EMT call support and memory sizing.
 *
 * Revision 3.2.2.2  1995/12/01  04:23:59  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2.2.1  1995/11/23  19:14:52  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.2  1995/11/17  18:46:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "signal_4k.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "pred_plat_defs.h"

#include "../../src-4k-c7100/c7100_pcmap.h"
#include "../../src-4k/gt64010.h"
#include "../../src-4k-c7100/c7100_fpga.h"

#define INIT_SREG SREG_CU0|SREG_CU1|SREG_BEV|SREG_DE

#define MEMORY_SIZE_64K	PHY_TO_KSEG1(0x10000)
#define MEMORY_SIZE_16B PHY_TO_KSEG1(0x10)
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
	.globl pred_cache_exception
	.globl general_exception
	.globl monitor_stk
	.globl sys_dirty
	.globl sys_restart
	.globl monlib_launch
	.globl reason_reg

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
	la	k1,pred_cache_exception			# upper level handler

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

	la	t4,KSEG1_FPGA_LED_REG	# get LED reg addr
	lbu	t5,0(t4)		# read it
	li	t6,~LED0		# LED 0 on value
	li	t7,LED0			# LED 0 off value

	and	t8,t5,t6		# turn it on (0)
	sb	t8,0(t4)

	nop
	or	t8,t5,t7		# turn it off (1)
	sb	t8,0(t4)
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

	la	t4,KSEG1_FPGA_LED_REG	# get the LED reg address
	lbu	t5,0(t4)
	li	t6,~LED0		# LED on value
	li	t7,LED0			# LED off value
	and	t8,t5,t6
	sb	t8,0(t4)
	nop
	or	t8,t5,t7
	sb	t8,0(t4)
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

	la	t4,KSEG1_FPGA_LED_REG
	lbu	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sb	t8,0(t4)
	nop
	or	t8,t5,t7
	sb	t8,0(t4)
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

	la	t4,KSEG1_FPGA_LED_REG
	lbu	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sb	t8,0(t4)
	nop
	or	t8,t5,t7
	sb	t8,0(t4)
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

	la	t4,KSEG1_FPGA_LED_REG
	lbu	t5,0(t4)
	li	t6,~LED0
	li	t7,LED0
	and	t8,t5,t6
	sb	t8,0(t4)
	nop
	or	t8,t5,t7
	sb	t8,0(t4)
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
 * Startup point: The code branches here on power-on reset or on a
 * soft reset (watchdog timeout). Unfortunately, we do have to destroy 
 * some of the context (t0,k0,k1) before finding out which case brought 
 * us here. Maybe this can be improved later.
 */
_xstart:
	.set	noreorder

	mfc0	k0,CP0_SREG			# get the status register
	li	k1,INIT_SREG			# set Bootstrap vectors, 
						# Cp0,1 usable. 32 fpu regs
	mtc0	k1,CP0_SREG
	nop
	mtc0	zero,CP0_CAUSE			# zero Cause Register
	nop

       /*
        * Program the device access parameters into the gt64010
        */
       move    t1, zero
 
       li      t0, CPU_INTERFACE_CONF_INIT
       sw      t0, KSEG1_GT64010_CPU_INTERFACE_CONF(t1)
 
       li      t0, CS20_LOW_INIT
       sw      t0, KSEG1_GT64010_CS20_LOW(t1)
 
       li      t0, CS20_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS20_HIGH(t1)
 
       li      t0, CS3_BOOT_LOW_INIT
       sw      t0, KSEG1_GT64010_CS3_BOOT_LOW(t1)
 
       li      t0, CS3_BOOT_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS3_BOOT_HIGH(t1)

       li      t0, PCI_IO_LOW_INIT
       sw      t0, KSEG1_GT64010_PCI_IO_LOW(t1)

       li      t0, PCI_IO_HIGH_INIT
       sw      t0, KSEG1_GT64010_PCI_IO_HIGH(t1)

       li      t0, PCI_MEM_LOW_INIT
       sw      t0, KSEG1_GT64010_PCI_MEM_LOW(t1)

       li      t0, PCI_MEM_HIGH_INIT
       sw      t0, KSEG1_GT64010_PCI_MEM_HIGH(t1)
 
       li      t0, CS0_LOW_INIT
       sw      t0, KSEG1_GT64010_CS0_LOW(t1)
 
       li      t0, CS0_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS0_HIGH(t1)
 
       li      t0, CS1_LOW_INIT
       sw      t0, KSEG1_GT64010_CS1_LOW(t1)
 
       li      t0, CS1_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS1_HIGH(t1)
 
       li      t0, CS2_LOW_INIT
       sw      t0, KSEG1_GT64010_CS2_LOW(t1)
 
       li      t0, CS2_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS2_HIGH(t1)
 
       li      t0, CS3_LOW_INIT
       sw      t0, KSEG1_GT64010_CS3_LOW(t1)
 
       li      t0, CS3_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS3_HIGH(t1)
 
       li      t0, BOOT_CS_LOW_INIT
       sw      t0, KSEG1_GT64010_BOOT_CS_LOW(t1)
 
       li      t0, BOOT_CS_HIGH_INIT
       sw      t0, KSEG1_GT64010_BOOT_CS_HIGH(t1)
 
       li      t0, DEVICE_BANK0_PARAM_INIT
       sw      t0, KSEG1_GT64010_DEVICE_BANK0_PARAM(t1)
 
       li      t0, DEVICE_BANK1_PARAM_INIT
       sw      t0, KSEG1_GT64010_DEVICE_BANK1_PARAM(t1)
 
       li      t0, DEVICE_BANK2_PARAM_INIT_REV1
       sw      t0, KSEG1_GT64010_DEVICE_BANK2_PARAM(t1)
 
       la     t1, reason_reg
       li     t0, KSEG1_FPGA_RESET_REASON_REG
       lb     t2, 0(t0)
       sb     t2, 0(t1)

       /*
        * Disable the watchdog
        */
       li      k1, KSEG1_FPGA_LED_REG          # disable the watchdog
       sb      zero, 0(k1)                     # so that it will not cause 
                                               # another hard reset
       /*
        * Reset all IO devices
        */
       li      k1, KSEG1_ADRSPC_MP_FPGA        # load midplane register addr
       li      t0, KSEG1_ADRSPC_PROM           # use to flush the io wr. buffer
       
       sw      zero, 0x20(k1)                  # clear pkt_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x48(k1)                  # clear mgmt_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x78(k1)                  # clear err_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x58(k1)                  # clear pa_ctrl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
 
       /*
        * Perform an MB0 reset
        */
       li      k1, ADRSPC_MB0_RESET            # load reset address
       sw      zero, 0(k1)                     # assert reset
 
       li      t0,100                          # loop 100 times

_init_delay0:
       addu    t0, t0, -1                      # decrement
       bgt     t0, zero, _init_delay0          # loop if not done
       nop                                     # (bd slot)
 
       sw      zero, 0(k1)                     # de-assert reset
 
       li      t0,1000                         # loop 1000 times (let the
                                               # port adaptors power down)
_init_delay1:
       addu    t0, t0, -1                      # decrement
       bgt     t0, zero, _init_delay1          # loop if not done
       nop                                     # (bd slot)
       
       /*
        * See if this was an NMI reset
        */
	li	k1,SREG_SR			# get Soft Reset bit mask
	and	t0,k0,k1			# and status reg with SR mask
	bgtz	t0,soft_reset  			# branch if bit set
	nop

/*
 * Description:
 * 
 *	Initialize the internal registers in the Galileo
 */

	li	t0, KSEG1_ADRSPC_GT64010_INTERNAL_SPC_DECODE_INIT
	lw	t2, 0(t0)
	
	move 	t1, zero

	li	t0, RAS10_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS10_LOW(t1)

	li 	t0, RAS10_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS10_HIGH(t1)

	li	t0, RAS32_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS32_LOW(t1)
	
	li	t0, RAS32_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS32_HIGH(t1)

	li	t0, CS20_LOW_INIT
	sw	t0, KSEG1_GT64010_CS20_LOW(t1)

	li	t0, CS20_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS20_HIGH(t1)

	li	t0, CS3_BOOT_LOW_INIT
	sw	t0, KSEG1_GT64010_CS3_BOOT_LOW(t1)

	li	t0, CS3_BOOT_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS3_BOOT_HIGH(t1)

	li	t0, RAS0_LOW_INIT	
	sw	t0, KSEG1_GT64010_RAS0_LOW(t1)

	li	t0, RAS0_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS0_HIGH(t1)
	
	li	t0, RAS1_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS1_LOW(t1)

	li	t0, RAS1_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS1_HIGH(t1)

	li	t0, RAS2_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS2_LOW(t1)

	li	t0, RAS2_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS2_HIGH(t1)

	li	t0, RAS3_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS3_LOW(t1)

	li	t0, RAS3_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS3_HIGH(t1)

	li	t0, DRAM_CONFIG_INIT
	sw	t0, KSEG1_GT64010_DRAM_CONFIG(t1)
	lw	t0, KSEG1_GT64010_DRAM_CONFIG(t1)

	li	t0, DRAM_BANK0_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK0_PARAM(t1)

	li	t0, DRAM_BANK1_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK1_PARAM(t1)

	li	t0, DRAM_BANK2_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK2_PARAM(t1)

	li	t0, DRAM_BANK3_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK3_PARAM(t1)

/*
 * Setup Galileo for I/O access 
 */
	li	t0, CS0_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS0_LOW(t1)

	li	t0, CS0_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS0_HIGH(t1)

	li	t0, CS1_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS1_LOW(t1)

	li	t0, CS1_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS1_HIGH(t1)

	li	t0, CS2_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS2_LOW(t1)

	li	t0, CS2_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS2_HIGH(t1)

	li	t0, CS3_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS3_LOW(t1)

	li	t0, CS3_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS3_HIGH(t1)

	li	t0, BOOT_CS_LOW_INIT
	sw 	t0, KSEG1_GT64010_BOOT_CS_LOW(t1)

	li	t0, BOOT_CS_HIGH_INIT
	sw	t0, KSEG1_GT64010_BOOT_CS_HIGH(t1)

	li	t0, DEVICE_BANK0_PARAM_INIT
	sw	t0, KSEG1_GT64010_DEVICE_BANK0_PARAM(t1)

	li	t0, DEVICE_BANK1_PARAM_INIT
	sw	t0, KSEG1_GT64010_DEVICE_BANK1_PARAM(t1)

	li	t0, DEVICE_BANK2_PARAM_INIT_REV1
	sw	t0, KSEG1_GT64010_DEVICE_BANK2_PARAM(t1)

	/*
	 * Set up the secondary cache
	 */
	mfc0	t7,CP0_SREG			# read status register
 	li	t2,SREG_DE|SREG_KX		# get DE,KX mask
 	or	t6,t7,t2			# assert DE,KX
 	mtc0	t6,CP0_SREG			# write status register
 	nop					# let it settle
 
 	/*
 	 * Do an uncached read of the level2 cache controller
 	 * with TagOP[1] High, TagOP[0] Low. The address needs
 	 * to be in the following format :
 	 *
 	 * 0x9000 000X XXX0 0000 - Where XX XX is defined by platform
 	 * header file.
 	 */
 	li	t1,0x9000			# get base address
 	dsll	t1,t1,48			# 0x9000 0000 0000 0000
 	li	t2,LVL2_CACHE_TAGLINES_10	# load XXXX
 	dsll	t2,t2,20			# 0x0000 000X XXX0 0000
 	or	t1,t1,t2			# 0x9000 000X XXX0 0000
 
 	lw	t2,0(t1)			# flush the cache
 	nop					# load delay
 
 	/*
 	 * Restore the status register
 	 */
 	mtc0	t7,CP0_SREG			# write status register
 	nop					# let it settle

/*
 * Description:
 *
 *       This test will store the data Pattern in the specified
 *       address of each location. The test works on ascending order.
 *
 *	t1 = 0xAAAAAAAA55555555
 *	t2 = 0x9999999966666666
 */

        li      t0, 0x0e			/* stage 1 */
        li      t1, KSEG1_FPGA_LED_REG
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
	j	__init_done			# return to the monitor prompt
	nop

	.set	at
	.set	reorder

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

       /*
        * Program the device access parameters into the gt64010
        */
       move    t1, zero
 
       li      t0, CPU_INTERFACE_CONF_INIT
       sw      t0, KSEG1_GT64010_CPU_INTERFACE_CONF(t1)
 
       li      t0, CS20_LOW_INIT
       sw      t0, KSEG1_GT64010_CS20_LOW(t1)
 
       li      t0, CS20_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS20_HIGH(t1)
 
       li      t0, CS3_BOOT_LOW_INIT
       sw      t0, KSEG1_GT64010_CS3_BOOT_LOW(t1)
 
       li      t0, CS3_BOOT_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS3_BOOT_HIGH(t1)

       li      t0, PCI_IO_LOW_INIT
       sw      t0, KSEG1_GT64010_PCI_IO_LOW(t1)

       li      t0, PCI_IO_HIGH_INIT
       sw      t0, KSEG1_GT64010_PCI_IO_HIGH(t1)

       li      t0, PCI_MEM_LOW_INIT
       sw      t0, KSEG1_GT64010_PCI_MEM_LOW(t1)

       li      t0, PCI_MEM_HIGH_INIT
       sw      t0, KSEG1_GT64010_PCI_MEM_HIGH(t1)
 
       li      t0, CS0_LOW_INIT
       sw      t0, KSEG1_GT64010_CS0_LOW(t1)
 
       li      t0, CS0_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS0_HIGH(t1)
 
       li      t0, CS1_LOW_INIT
       sw      t0, KSEG1_GT64010_CS1_LOW(t1)
 
       li      t0, CS1_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS1_HIGH(t1)
 
       li      t0, CS2_LOW_INIT
       sw      t0, KSEG1_GT64010_CS2_LOW(t1)
 
       li      t0, CS2_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS2_HIGH(t1)
 
       li      t0, CS3_LOW_INIT
       sw      t0, KSEG1_GT64010_CS3_LOW(t1)
 
       li      t0, CS3_HIGH_INIT
       sw      t0, KSEG1_GT64010_CS3_HIGH(t1)
 
       li      t0, BOOT_CS_LOW_INIT
       sw      t0, KSEG1_GT64010_BOOT_CS_LOW(t1)
 
       li      t0, BOOT_CS_HIGH_INIT
       sw      t0, KSEG1_GT64010_BOOT_CS_HIGH(t1)
 
       li      t0, DEVICE_BANK0_PARAM_INIT
       sw      t0, KSEG1_GT64010_DEVICE_BANK0_PARAM(t1)
 
       li      t0, DEVICE_BANK1_PARAM_INIT
       sw      t0, KSEG1_GT64010_DEVICE_BANK1_PARAM(t1)
 
       li      t0, DEVICE_BANK2_PARAM_INIT_REV1
       sw      t0, KSEG1_GT64010_DEVICE_BANK2_PARAM(t1)
 
       la     t1, reason_reg
       li     t0, KSEG1_FPGA_RESET_REASON_REG
       lb     t2, 0(t0)
       sb     t2, 0(t1)

       /*
        * Disable the watchdog
        */
       li      k1, KSEG1_FPGA_LED_REG          # disable the watchdog
       sb      zero, 0(k1)                     # so that it will not cause 
                                               # another hard reset
       /*
        * Reset all IO devices
        */
       li      k1, KSEG1_ADRSPC_MP_FPGA        # load midplane register addr
       li      t0, KSEG1_ADRSPC_PROM           # use to flush the io wr. buffer
       
       sw      zero, 0x20(k1)                  # clear pkt_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x48(k1)                  # clear mgmt_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x78(k1)                  # clear err_int_enbl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
       sw      zero, 0x58(k1)                  # clear pa_ctrl_lsw
       lw      t1, 0(t0)                       # flush the io write buffer
 
       /*
        * Perform an MB0 reset
        */
       li      k1, ADRSPC_MB0_RESET            # load reset address
       sw      zero, 0(k1)                     # assert reset
 
       li      t0,100                          # loop 100 times

_init_delay2:
       addu    t0, t0, -1                      # decrement
       bgt     t0, zero, _init_delay2          # loop if not done
       nop                                     # (bd slot)
 
       sw      zero, 0(k1)                     # de-assert reset
 
       li      t0,1000                         # loop 1000 times (let the
                                               # port adaptors power down)
_init_delay3:
       addu    t0, t0, -1                      # decrement
       bgt     t0, zero, _init_delay3          # loop if not done
       nop                                     # (bd slot)
       
/*
 * Description:
 * 
 *	Initialize the internal registers in the Galileo
 */

	li	t0, KSEG1_ADRSPC_GT64010_INTERNAL_SPC_DECODE_INIT
	lw	t2, 0(t0)
	
	move 	t1, zero

	li	t0, RAS10_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS10_LOW(t1)

	li 	t0, RAS10_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS10_HIGH(t1)

	li	t0, RAS32_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS32_LOW(t1)
	
	li	t0, RAS32_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS32_HIGH(t1)

	li	t0, CS20_LOW_INIT
	sw	t0, KSEG1_GT64010_CS20_LOW(t1)

	li	t0, CS20_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS20_HIGH(t1)

	li	t0, CS3_BOOT_LOW_INIT
	sw	t0, KSEG1_GT64010_CS3_BOOT_LOW(t1)

	li	t0, CS3_BOOT_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS3_BOOT_HIGH(t1)

	li	t0, RAS0_LOW_INIT	
	sw	t0, KSEG1_GT64010_RAS0_LOW(t1)

	li	t0, RAS0_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS0_HIGH(t1)
	
	li	t0, RAS1_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS1_LOW(t1)

	li	t0, RAS1_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS1_HIGH(t1)

	li	t0, RAS2_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS2_LOW(t1)

	li	t0, RAS2_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS2_HIGH(t1)

	li	t0, RAS3_LOW_INIT
	sw	t0, KSEG1_GT64010_RAS3_LOW(t1)

	li	t0, RAS3_HIGH_INIT
	sw	t0, KSEG1_GT64010_RAS3_HIGH(t1)

	li	t0, DRAM_CONFIG_INIT
	sw	t0, KSEG1_GT64010_DRAM_CONFIG(t1)
	lw	t0, KSEG1_GT64010_DRAM_CONFIG(t1)

	li	t0, DRAM_BANK0_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK0_PARAM(t1)

	li	t0, DRAM_BANK1_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK1_PARAM(t1)

	li	t0, DRAM_BANK2_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK2_PARAM(t1)

	li	t0, DRAM_BANK3_PARAM_INIT 
	sw	t0, KSEG1_GT64010_DRAM_BANK3_PARAM(t1)

/*
 * Setup Galileo for I/O access 
 */
	li	t0, CS0_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS0_LOW(t1)

	li	t0, CS0_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS0_HIGH(t1)

	li	t0, CS1_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS1_LOW(t1)

	li	t0, CS1_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS1_HIGH(t1)

	li	t0, CS2_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS2_LOW(t1)

	li	t0, CS2_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS2_HIGH(t1)

	li	t0, CS3_LOW_INIT
	sw 	t0, KSEG1_GT64010_CS3_LOW(t1)

	li	t0, CS3_HIGH_INIT
	sw	t0, KSEG1_GT64010_CS3_HIGH(t1)

	li	t0, BOOT_CS_LOW_INIT
	sw 	t0, KSEG1_GT64010_BOOT_CS_LOW(t1)

	li	t0, BOOT_CS_HIGH_INIT
	sw	t0, KSEG1_GT64010_BOOT_CS_HIGH(t1)

	li	t0, DEVICE_BANK0_PARAM_INIT
	sw	t0, KSEG1_GT64010_DEVICE_BANK0_PARAM(t1)

	li	t0, DEVICE_BANK1_PARAM_INIT
	sw	t0, KSEG1_GT64010_DEVICE_BANK1_PARAM(t1)

	li	t0, DEVICE_BANK2_PARAM_INIT_REV1
	sw	t0, KSEG1_GT64010_DEVICE_BANK2_PARAM(t1)

	/*
	 * Set up the secondary cache
	 */
	mfc0	t7,CP0_SREG			# read status register
 	li	t2,SREG_DE|SREG_KX		# get DE,KX mask
 	or	t6,t7,t2			# assert DE,KX
 	mtc0	t6,CP0_SREG			# write status register
 	nop					# let it settle
 
 	/*
 	 * Do an uncached read of the level2 cache controller
 	 * with TagOP[1] High, TagOP[0] Low. The address needs
 	 * to be in the following format :
 	 *
 	 * 0x9000 000X XXX0 0000 - Where XX XX is defined by platform
 	 * header file.
 	 */
 	li	t1,0x9000			# get base address
 	dsll	t1,t1,48			# 0x9000 0000 0000 0000
 	li	t2,LVL2_CACHE_TAGLINES_10	# load XXXX
 	dsll	t2,t2,20			# 0x0000 000X XXX0 0000
 	or	t1,t1,t2			# 0x9000 000X XXX0 0000
 
 	lw	t2,0(t1)			# flush the cache
 	nop					# load delay
 
 	/*
 	 * Restore the status register
 	 */
 	mtc0	t7,CP0_SREG			# write status register
 	nop					# let it settle

	li	sp,MON_INIT_SP-STACK_PARAMS	# initialize stack pointer
	addiu	a0,zero,RESET_WARM		# setup parameter 1, reset_type
	jal	init
	move	a1,zero				# context pointer (NULL)

	j	__init_done			# return to the monitor prompt
	nop

	.set 	reorder
ENDFRAME(do_reset)

/*
 * lit_errorLEDs : Turn on LED error code on the UNUSED LEDs, never returns
 * a0 = code in hex form
 */
	.globl lights

FRAME(lit_errorLEDs,sp,0,ra)
	.set noreorder

lights:

        xor     t0, a0, 0xFF			# LOW is on
	and	t0, t0, LEDS_OFF		# Disable watchdog
        li      t1, KSEG1_FPGA_LED_REG
        sb      t0, 0(t1)

error_loop:
        b       error_loop                      # loop forever
        nop                                     

	.set	reorder
ENDFRAME(lit_errorLEDs)

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

	la	t0, monlib_launch
	lw	t1, 0(t0)
	li	t2, 1
	beq	t1, t2, no_reset_io
	nop

	jal	reset_io		# kill the interfaces
	nop

no_reset_io:
	move	a0,zero			# set param1 to zero
	li	a1,-1			# set param2 to -1
	jal	cache_flush		# flush I/D caches
	nop

	jal	disconint		# disable console interrupts
	nop

	jal	enacon			# enable console xmitter and receiver
	nop
	ld	v0,24(sp)		# restore return code from 
					# launched image
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
	nop
	la	t0, sys_restart         # restart the system code
	j	t0
	move	sp,a0			# set stack pointer to context
	.set	reorder
ENDFRAME(relaunch)


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
	and	t7,t8,t0		# turn IE off
	mtc0	t7,CP0_SREG		# re-write status register

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
 	 * Assuming that the level 2 cache is present, flush it.
 	 * Put the processor in 64bit mode, with no parity
 	 */
 	li	t2,SREG_DE|SREG_KX		# get DE,KX mask
 	or	t6,t7,t2			# assert DE,KX
 	mtc0	t6,CP0_SREG			# write status register
 	nop					# let it settle
 
 	/*
 	 * Do an uncached read of the level2 cache controller
 	 * with TagOP[1] High, TagOP[0] Low. The address needs
 	 * to be in the following format :
 	 *
 	 * 0x9000 000X XXX0 0000 - Where XX XX is defined by platform
 	 * header file.
 	 */
 	li	t1,0x9000			# get base address
 	dsll	t1,t1,48			# 0x9000 0000 0000 0000
 	li	t2,LVL2_CACHE_TAGLINES_10	# load XXXX
 	dsll	t2,t2,20			# 0x0000 000X XXX0 0000
 	or	t1,t1,t2			# 0x9000 000X XXX0 0000
 
 	lw	t2,0(t1)			# flush the cache
 	nop					# load delay
 
 	/*
 	 * Restore the status register
 	 */
 	mtc0	t7,CP0_SREG			# write status register
 	nop					# let it settle

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

/*
 * lw_instr: Just execute this instruction.
 *
 * int lw_instr(unsigned long *addr)
 */
FRAME(lw_instr, sp, 0, ra)
	.set 	noreorder

	lw	v0, 0(a0)              # execute the lw instruction
	nop
	nop
	
	j	ra
	nop
	nop
	
	.set	reorder
ENDFRAME(lw_instr)


/*
 * vaxorder:
 *
 * VAX order byte swap for longs [ 1 2 3 4 -> 4 3 2 1 ]
 */
 
 #
 # ulong vaxorder (ulong num)
 #
FRAME(vaxorder,sp,0,ra)
        .set    noreorder
 
        andi    t0,a0,0xff      # t0 = num & 0xff       [0 0 0 4]
        or      v0,t0,zero      # v0 = t0               [0 0 0 4]
        sll     v0,v0,8         # v0 = v0 << 8          [0 0 4 0]
        andi    t0,a0,0xff00    # t0 = num & 0xff00     [0 0 3 0]
        srl     t0,t0,8         # t0 = t0 >> 8          [0 0 0 3]
        or      v0,v0,t0        # v0 = v0 | t0          [0 0 4 3]
        sll     v0,v0,8         # v0 = v0 << 8          [0 4 3 0]
        srl     a0,a0,16        # num = num >> 16       [0 0 1 2]
        andi    t0,a0,0xff      # t0 = num & 0xff       [0 0 0 2]
        or      v0,v0,t0        # v0 = v0 | t0          [0 4 3 2]
        sll     v0,v0,8         # v0 = v0 << 8          [4 3 2 0]
        andi    t0,a0,0xff00    # t0 = num & 0xff00     [0 0 1 0]
        srl     t0,t0,8         # t0 = t0 >> 8          [0 0 0 1]
 
        j       ra              # return
        or      v0,v0,t0        # v0 = v0 | t0          [4 3 2 1] (bd slot)
 
        .set    reorder
ENDFRAME(vaxorder)

/* End of Module */


