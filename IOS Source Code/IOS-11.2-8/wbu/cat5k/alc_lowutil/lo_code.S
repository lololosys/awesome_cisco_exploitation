| $Id: lo_code.S,v 1.1.4.1 1996/05/09 14:59:45 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/lo_code.S,v $
|-----------------------------------------------------------------
| lo_code.S -- General 680x0 low-level utility routines.
|              Assembly routines that may be useful to any code.
|
| November 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: lo_code.S,v $
| Revision 1.1.4.1  1996/05/09  14:59:45  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:45  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:50  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:53  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:17  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:48  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
| General 680x0 low-level utility routines.
| These are assembly routines that may be useful to any code.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	based on Catalyst, .asciz "@ ( # ) vector.s	1.4 10/22/93"
|	11/23/94  RKL	- Grabbed relevant Catalyst code for Synergy baseline.
|			  Modified Catalyst source to become Synergy ACP Boot.
| ----------------------------------------------------------------------------

		.text
		.asciz	"%W% %G%"
		.align	1		| Align to even address.

#include "asm_mode_defs.h"
#include "boot_defs.h"

| ----------------------------------------------------------------------------
|	This routine calls a piece of assembly code as if it were a subroutine.
| ----------------------------------------------------------------------------
		.text
		.globl	__asm_call
__asm_call:
		moveml	d1-d7/a0-a6,sp@-

		moveal	sp@(0x3C),a0    | get start_address
		moveal	sp@(0x40),a1    | get end_address

		movel	a1@(2),sp@-     | save contents @end_address+2
		movew	#0x4E75,a1@(2)  | put "rts" @end_address+2

		jsr	a0@             | call execute_code @start_address

		movel	sp@+,d0         | restore contents @end_address+2
		moveal	sp@(0x40),a1
		movel	d0,a1@(2)

		moveml	sp@+,d1-d7/a0-a6
		rts

| ----------------------------------------------------------------------------
|	These routines jump to well-known entry points in various places.
| ----------------------------------------------------------------------------
		.globl	__jmpfl
__jmpfl:
		ASM_ADDR_FLASH_CODE (a0)
		jmp	a0@		| Hit main FLASH entry point.
		rts			|   (Actually non-Boot FLASH segment.)

		.globl	__jmpram
__jmpram:
		ASM_ADDR_DRAM_CODE (a0)
		jmp	a0@		| Hit main DRAM entry point.
		rts

		.globl	__jmpprom
		.globl	__jmpboot
__jmpprom	= __jmpboot		| Set this for linker, external users.
__jmpboot:
		ASM_ADDR_BOOT_CODE (a0)
		jmp	a0@		| Hit main Boot entry point.
		rts			|   (It is actually a FLASH segment.)

| ----------------------------------------------------------------------------
|	Generic stub routine that just does nothing.
| ----------------------------------------------------------------------------
		.globl	_CallStub
_CallStub:
		rts			| Stub just returns to caller.

