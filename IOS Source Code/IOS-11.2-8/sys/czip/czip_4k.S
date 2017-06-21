/* $Id: czip_4k.S,v 3.2.56.1 1996/03/21 22:28:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/czip/czip_4k.S,v $
 *------------------------------------------------------------------
 * czip_4k.S - Assembler support for r4k zipped images
 *
 * April 1995, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: czip_4k.S,v $
 * Revision 3.2.56.1  1996/03/21  22:28:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/11/22  20:25:25  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:58:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/23  18:33:34  mbeesley
 * CSCdi36332:  zipped r4k bootstrap images do not work
 *
 * Revision 2.1  1995/06/07  20:22:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

/*
 * Include header files
 */
#if 	defined(SIERRA)
# include "../asm-mips/asm_4k_c4000.h"
#endif	/* SIERRA */

#if	defined(RSP)
# include "../asm-mips/asm_4k_rsp.h"
# include "../src-rsp/rsp_pcmap.h"
#endif	/* RSP */

#if	defined(PREDATOR)
# include "../asm-mips/asm_4k_c7100.h"
#endif	/* PREDATOR */

/*
 * Externals
 */
	.extern monstack, 4
	.extern monra, 4
	.extern memsize, 4
	.extern param0, 4
	.extern param1, 4
	.extern param2, 4
	.extern param3, 4
	.extern reg_s0, 4
	.extern reg_s1, 4
	.extern reg_s2, 4
	.extern reg_s3, 4
	.extern reg_s4, 4
	.extern reg_s5, 4
	.extern reg_s6, 4
	.extern reg_s7, 4
	.extern reg_s8, 4
	.extern _gp

/*
 * Storage
 */
	.globl	r4k_hclock
	.globl	r4k_clock

	.sdata
	.align 3

r4k_hclock:
	.word	0
r4k_clock:
	.word	0

        .text
	.align 2

/*
 * __start : Entry point of an image that is self decompressing.
 * This file must be the first file linked into the zipped image
 *
 * Notes : The zipped image executable is linked to run at the load
 * address. There is no need for any virtual memory programming. The
 * bundled router image that we will be decompressing is linked to
 * run at a virtual address. However the mksum_elf utility has kindly
 * translated all the kuseg addresses in this elf file to be usable
 * by a bootstrap that does not use any virtual addresses (IE kuseg->kseg0)
 */
FRAME(__start,sp,32,ra)
	.set	noreorder

	/*
	 * Load up the _gp register
	 */
	la	gp,_gp			# set gp register
	
	/*
	 * Save calling info
	 */
	sw	sp,monstack		# save stack pointer for later use
	sw	ra,monra		# save monitor return address
	sw	a0,param0		# save parameter 0
	sw	a1,param1		# save parameter 1
	sw	a2,param2		# save parameter 2
	sw	a3,param3		# save parameter 3
	sw	s0,reg_s0		# save register s0
	sw	s1,reg_s1		# save register s1
	sw	s2,reg_s2		# save register s2
	sw	s3,reg_s3		# save register s3
	sw	s4,reg_s4		# save register s4
	sw	s5,reg_s5		# save register s5
	sw	s6,reg_s6		# save register s6
	sw	s7,reg_s7		# save register s7
	sw	s8,reg_s8		# save register s8

	addiu	sp,sp,-32		# create a stack frame
	sw	ra,16(sp)		# save ra register

	jal	mon_getmemsize		# get memory size
	nop				# (bd slot)

	sw	v0,memsize		# save memory size

	/*
	 * Calculate where the stack for the unzipper should go
	 */
	li	t0,ADRSPC_K0BASE	# get KSeg0 base address
	li	t1,128			# get 128 byte buffer
	addu	t0,v0,t0		# add in memsize
	subu	t0,t0,t1		# take away 128 bytes

	move	sp,t0			# set this up as stack pointer

	jal	main			# call main
	move	a0,t0			# give sp as parameter (bd slot)

reload_loop:
	jal	cz_reload_system	# main should never return
	nop				# (bd slot)

	b	reload_loop		# go back and reload
	nop				# (bd slot)

	.set	reorder
ENDFRAME(__start)

/*
 * emt_call 
 * ROM Monitor system call
 * Note : We pass parameters in registers. This limits the total number
 *        to four (a0..a3). Also, the ROM Monitor syscall handler may
 *	  not support branch delay slot, so ensure that the syscall 
 *	  instruction is not in one.
 */
 #
 # int emt_call (int emt_type, [,arg1[,arg2,[arg3]]])
 #
FRAME(emt_call,sp,0,ra)
	.set	noreorder

	syscall			# Take a syscall trap. Arguments are allready
				# in registers a0..a3
	nop			# A bit paranoid !!

	j	ra		# return
	nop			# bd slot

	.set	reorder
ENDFRAME(emt_call)

/*
 * copy_and_launch :
 * This routine is a position independant routine that copies the uncompressed
 * router image to the correct starting point, re-installs the original 
 * rom monitor stack pointer, flushes the caches and then calls the entry
 * point of the router image
 *
 * Note : This function is relocated to upper memory so that the copy does
 *	  not walk over the instruction stream.
 *
 * void copy_and_launch (sec_t *ptr, image_info_t *i);
 */
	.globl	pic_code_end

FRAME(copy_and_launch,sp,0,ra)
	.set	noreorder

	/*
	 * Relocate the image
	 */
	lw	t1,4(a1)		# get copy of no. of sections
	move	t2,a0			# get copy of secs_ptr
	move	t0,zero			# zero out temp

sec_loop:
	sltu	t8,t0,t1		# set t8 if t0 < t1
	beq	zero,t8,finished_secs	# finished ?
	nop				# (bd slot)

	/*
	 * Relocate this section
	 */
	addiu	t0,t0,1			# increment section counter
	lw	t4,0(t2)		# get source address
	lw	t5,4(t2)		# get dest. address
	lw	t6,8(t2)		# get section size (in bytes)
	addiu	t2,t2,12		# bump pointer to next section

	move	t7,zero			# zero out byte count

sec_copy:
	sltu	t8,t7,t6		# set t8 if t7 < t6
	beq	zero,t8,sec_loop	# is this section finished ?
	nop				# (bd slot)

	lbu	t8,0(t4)		# read byte
	addiu	t4,t4,1			# bump pointer
	sb	t8,0(t5)		# write byte
	addiu	t5,t5,1			# bump pointer
	addiu	t7,t7,1			# bump counter

	b	sec_copy		# do the next byte
	nop				# (bd slot)

finished_secs:
	
	/*
	 * We have relocated the image to its execution address.
	 * The next thing to do is to flush the caches.
	 */
	mfc0	t8,CP0_SREG		# read status register
	li	t7,~SREG_IE		# get IE mask
	and	t7,t8,t7		# disable IE
	mtc0	t7,CP0_SREG		# write back status register
	nop				# let it settle

	/*
	 * Next, write back invalidate the D cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_DCACHE_LINES	# get no. of lines in D cache
	li	t2,R4K_DLINE_SHIFT	# get the line shift for D cache

dcache_loop1:

	blez	t1,do_icache1		# finished ?
	nop				# (bd slot)	
	
	cache	1,0(t0)			# Index Write back invalidate D cache
	nop				# (paranoid)

	addiu	t1,t1,-1		# t1 -= 1
	addu	t0,t0,t2		# increment the line index

	b	dcache_loop1		# do it again
	nop				# (bd slot)

do_icache1:

	/*
	 * And the next thing is to do an Index Invalidate on I cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_ICACHE_LINES	# get no. of lines in I cache
	li	t2,R4K_ILINE_SHIFT	# get the line shift for I cache

icache_loop1:

	blez	t1,cache_done1		# finished ?
	nop				# (bd slot)

	cache	0,0(t0)			# Index Invalidate I cache line
	nop				# (paranoid)

	addiu	t1,t1,-1		# t1 -= 1
	addu	t0,t0,t2		# increment the line index

	b	icache_loop1		# do it again
	nop				# (bd slot)

cache_done1:
	mtc0	t8,CP0_SREG		# restore original status register

	/*
	 * Install the rom monitor's original stack pointer,
	 * and install the parameters passed from the monitor
	 * up to the running image
	 */
	move	t0,a1			# get info pointer
	lw	sp,8(t0)		# get rom monitors stack pointer
	lw	ra,12(t0)		# get monitors return address
	lw	a0,16(t0)		# get parameter 0
	lw	a1,20(t0)		# get parameter 1
	lw	a2,24(t0)		# get parameter 2
	lw	a3,28(t0)		# get parameter 3
	lw	s0,32(t0)		# restore s0
	lw	s1,36(t0)		# restore s1
	lw	s2,40(t0)		# restore s2
	lw	s3,44(t0)		# restore s3
	lw	s4,48(t0)		# restore s4
	lw	s5,52(t0)		# restore s5
	lw	s6,56(t0)		# restore s6
	lw	s7,60(t0)		# restore s7
	lw	s8,64(t0)		# restore s8

	lw	t0,0(t0)		# get entry point
	nop				# load delay

	j	t0			# call entry point
	nop				# (bd slot)
	
	.set	reorder
ENDFRAME(copy_and_launch)

pic_code_end:				# mark end of PIC code

/*
 * cache_flush:
 *
 * Flush both the Instruction and Data caches
 */
 #
 # void cache_flush (void)
 #
FRAME(cache_flush,sp,0,ra)
	.set	noreorder

	/*
	 * First job is to disable interrupts. The Orion erratta
	 * sheet recommends this to avoid a bug in the two way set
	 * associative cache (which kills gdb debugging sometimes).
	 * This is not be a problem with regard to 4ms timer tick
	 * or watchdog as worst case, D cache write back causes
	 * 512 cache line writes (512 * 340ns) plus some I cache fills
	 * (max 8 fills) == 200us.
	 */
	mfc0	t8,CP0_SREG		# read status register
	li	t7,~SREG_IE		# get IE mask
	and	t7,t8,t7		# disable IE
	mtc0	t7,CP0_SREG		# write back status register
	nop				# let it settle

	/*
	 * Next, write back invalidate the D cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_DCACHE_LINES	# get no. of lines in D cache
	li	t2,R4K_DLINE_SHIFT	# get the line shift for D cache

dcache_loop:

	blez	t1,do_icache	# finished ?
	nop			# (bd slot)	
	
	cache	1,0(t0)		# Index Write back invalidate D cache line
	nop			# (paranoid)

	addiu	t1,t1,-1	# t1 -= 1
	addu	t0,t0,t2	# increment the line index

	b	dcache_loop	# do it again
	nop			# (bd slot)

do_icache:

	/*
	 * And the next thing is to do an Index Invalidate on I cache
	 */
	li	t0,R4K_K0BASE		# get K0 base address
	li	t1,R4K_ICACHE_LINES	# get no. of lines in I cache
	li	t2,R4K_ILINE_SHIFT	# get the line shift for I cache

icache_loop:

	blez	t1,cache_done	# finished ?
	nop			# (bd slot)

	cache	0,0(t0)		# Index Invalidate I cache line
	nop			# (paranoid)

	addiu	t1,t1,-1	# t1 -= 1
	addu	t0,t0,t2	# increment the line index

	b	icache_loop	# do it again
	nop			# (bd slot)

cache_done:

	j	ra		# return
	mtc0	t8,CP0_SREG	# restore original status register (bd slot)

	.set	reorder
ENDFRAME(cache_flush)

/* end of file */
