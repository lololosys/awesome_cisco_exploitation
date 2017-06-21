| $Id: czip_68.S,v 3.2 1995/11/17 08:58:06 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/czip/czip_68.S,v $
|------------------------------------------------------------------
| czip_68.S -- Assembler support routines for gzip
|	       (common to the 68000, 68020 and 68040 processors)
|
| December 1994, Michael Beesley
|
| Copyright (c) 1986-1995 by cisco Systems, Inc.
| All rights reserved.
|
|------------------------------------------------------------------
| /*
| $Log: czip_68.S,v $
| Revision 3.2  1995/11/17  08:58:06  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  11:15:12  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.2  1995/06/28  03:04:45  rmontino
| CSCdi36503:  AGS zipped images
|
| Revision 2.1  1995/06/07  20:22:12  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */

#define ASMINCLUDE
#include "../../boot/cpu.h"
#include "../machine/target_cisco.h"

/*
 * Define sixteen megabytes
 */
#define SIXTEEN_MEG	0x01000000

 |
 | __start : Entry point of an image that is self decompressing
 | This file must be the first file linked into the gzipped image
 |
	.globl	__start
	.globl  _monstack
	.globl	_memsize
	.globl	_mon_getmemsize
	.globl	_main
	.globl	_cz_reload_system

__start:

	/*
	 * The first thing to do is to save the rom monitor stack
	 * pointer so that the router image can begin executing
	 * at the correct place
	 */
	movl	sp, _monstack
	
	/*
	 * Get main memory size
	 */
	jsr	_mon_getmemsize

	/*
	 * If the memory size is larger than 16 Meg, use only 16 Meg
	 * (this is for the 4000M which has discontigous main memory).
	 */
	cmpl	#SIXTEEN_MEG, d0
	ble	sixteen_meg_or_less

	movl	#SIXTEEN_MEG, d0

sixteen_meg_or_less:
	/*
	 * save the size of memory
	 */
	movl	d0, _memsize

	/*
	 * Calculate where the stack should go
	 */
	addl	#RAMBASE,d0	| add our DRAM start address 
	subl	#128,d0		| subtract 128 bytes off the size of memory
	movl	d0,sp		| set this up as the stack pointer

	movl	sp,sp@-		| load stack pointer as parameter
	jsr	_main		| call main, will never return
1:
	jsr	_cz_reload_system	| just in case, continually reload
	bra	1b
	
 |
 | emt_call
 | ROM monitor emulation trap support
 | 
 | int emt_call (traptype[,arg1[,arg2[,arg3]]])
 | 

	.globl _emt_call

_emt_call:
	link a6,#0
	movl a6@(20),sp@-
	movl a6@(16),sp@-
	movl a6@(12),sp@-
	movl a6@(8),sp@-
	trap #15
 |
 | The next instruction fixes
 | up the stack for us
 |
	unlk a6
	rts


 |
 | copy_and_launch :
 | This routine is a position independant routine that copies the uncompressed
 | router image to the correct starting point, re-installs the original 
 | rom monitor stack pointer, flushes the caches and then calls the entry
 | point of the router image
 |
 | Note : This function is relocated to upper memory so that the copy does
 |	  not walk over the instruction stream.
 |
 | void copy_and_launch (uchar *src, uchar *dest, int bytes, 
 |			 void *entry, int cpu_type, ulong *monstack,
 |			 ulong *bss_start, ulong bss_size)
 |
	.globl	_copy_and_launch
	.globl	_pic_code_end

_copy_and_launch:

	/*
	 * Read source, destination and bytes
	 */
	movl	sp@(4),a0		| source
	movl	sp@(8),a1		| destination
	movl	sp@(12),d0		| bytes
	movl	sp@(16),a2		| entry point
	movl	sp@(20),d2		| cputype
	movl	sp@(24),a3		| original stack

	/*
	 * relocate the image
	 */
cloop:
	movb	a0@+,a1@+		| move a byte
	subql	#1,d0			| decrement
	bne	cloop			| more to do ?

	/*
	 * Flush the caches
	 */
	cmpl	#CPU_CSC4,d2		| Is this a CSC/4?
	bne	lcf_rp1			| No, check for RP1
	.word	0xf4f8			| cpush bc  ; push data cache,
                                	| then invalidate both caches
	bra	launch

lcf_rp1:
	cmpl    #CPU_RP1,d2    		| Is this a RP/1?
        bne     lcf020                 	| No, do it the 68020/68030 way
        .word   0xf4f8                  | cpush bc  ; push data cache
                                        | then invalidate both caches
	bra	launch
	
lcf020:	cmpl	#CPU_LEMONADE,d2 	| Is this a terminal server?
	beq	launch			| Handle the 68331

	cmpl	#CPU_SAPPHIRE,d2	| Is this a 68360 platform ?
	beq	launch			| Punt; no cache on 360

	movec	cacr,d0			| Get current cache control
	oril	#0x0804,d0		| Flush i/d cache
	movec	d0,cacr			| Now do the flush

launch:
	/*
	 * Invoke the image on the original rom monitor stack
	 */
	movl	a3,sp			| setup original stack pointer

	jmp	a2@			| call entry point

	nop				| padding
	nop				| 

_pic_code_end:

 |
 | cache_flush : Flush data and instruction caches
 | void cache_flush (void)
 |
	.globl	_cache_flush
	.globl	_cpu_type

_cache_flush:
	cmpl	#CPU_CSC4, _cpu_type		| Is this a CSC/4?
	bne	cf_rp1				| No, check for RP1
	.word	0xf4f8				| cpush bc  ; push data cache,
                                        	| then invalidate both caches
	rts					| all done

cf_rp1:
        cmpl    #CPU_RP1, _cpu_type     	| Is this a RP/1?
        bne     cf020                   	| No, do it the 68020/68030 way
        .word   0xf4f8                  	| cpush bc  ; push data cache
                                        	| then invalidate both caches
        rts                             	| all done

cf020:	cmpl	#CPU_LEMONADE, _cpu_type	| Is this a terminal server?
	beq	cf68331				| Handle the 68331

	cmpl	#CPU_SAPPHIRE, _cpu_type	| Is this a sapphire (68360) ?
	beq	cf68331				| No cache on 68360

	movec	cacr,d0				| Get current cache control
	oril	#0x0804,d0			| Flush i/d cache
	movec	d0,cacr				| Now do the flush
	rts					| All done

cf68331:					| No caches on 68331
	rts

/* End of file */
