| $Id: sys_cache.S,v 1.1.4.1 1996/05/09 14:59:49 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_lowutil/sys_cache.S,v $
|-----------------------------------------------------------------
| sys_cache.S -- 680x0 cache control code.
|
| This file contains routines used by applications for controlling
| the CPU instruction and data caches.
|
| June 1995, Rick Lowe
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: sys_cache.S,v $
| Revision 1.1.4.1  1996/05/09  14:59:49  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:49  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:40:56  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:11:59  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:22  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:42:53  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
| 680x0 cache control code.
|	This file contains routines used by applications for controlling
|	the CPU instruction and data caches.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	06/06/95  RKL	- File originated.
| ----------------------------------------------------------------------------
		.text
		.align	1		| Align to even address.

#include "asm_mode_defs.h"
#include "mc68ec030_defs.h"

| ----------------------------------------------------------------------------
|	Allow test routines to control CPU I/D caches.
|	C Usage:	Sys_SetCpuCacheX (u_int32 NewCacheCtrl) ;
|	Returns:	D0.L, previous CACR value.
| ----------------------------------------------------------------------------
		.globl	_Sys_SetCpuCacheX
_Sys_SetCpuCacheX:
		movel	d1,sp@-		| Save D1.L for scratch use.
		movel	d2,sp@-		| Save D2.L for scratch use.
		movel	sp@(12),d1	| Grab new control value from stack.

		moveq	#(0),d0		| Prep D0.L as RC.
		pea	(7).w		| Mask out all possible interrupts.
		jbsr	_splx		|   (RC in D0.L).

		movec	cacr,d2		| Save current/old CACR
		movec	d1,cacr		|   and set new cache control.

		movel	d0,sp@		| Restore previous interrupt level.
		jbsr	_splx		|   (RC in D0.L).
		addql	#(4),sp

		movel	d2,d0		| Set D0.L as our return value.
		movel	sp@+,d2		| Restore D2.L.
		movel	sp@+,d1		| Restore D1.L.
		rts

| ----------------------------------------------------------------------------
|	Modify current CPU I/D cache contents.
|	C Usage:	Sys_ModCpuCACR (u_int32 NewCacheCtrl, u_int32 Mask) ;
|	Returns:	D0.L, previous CACR value.
| ----------------------------------------------------------------------------
		.globl	_Sys_ModCpuCACR
_Sys_ModCpuCACR:
		movel	d1,sp@-		| Save D1.L for scratch use.
		movel	d2,sp@-		| Save D2.L for scratch use.

		movel	sp@(16),d1	| Grab Mask from stack and 
		notl	d1		|   invert to zero significant bits.

		moveq	#(0),d0		| Prep D0.L as RC.
		pea	(7).w		| Mask out all possible interrupts.
		jbsr	_splx		|   (RC in D0.L).

		movec	cacr,d2		| Get current/old CACR
		andl	d2,d1		|   and mask out significant bits
		orl	sp@(16),d1	|   and factor in new value
		movec	d1,cacr		|   and write as new cache control.

		movel	d0,sp@		| Restore previous interrupt level.
		jbsr	_splx		|   (RC in D0.L).
		addql	#(4),sp

		movel	d2,d0		| Set D0.L as our return value.
		movel	sp@+,d2		| Restore D2.L.
		movel	sp@+,d1		| Restore D1.L.
		rts

| ----------------------------------------------------------------------------
|	Allow test routines to control CPU Access Control Units (ACUs).
|	C Usage:	Sys_SetCpuACUx (u_int32 AcuNbr, u_int32 NewACU_Val) ;
|	Returns:	D0.L, previous ACU value.
| ----------------------------------------------------------------------------
		.globl	_Sys_SetCpuACUx
_Sys_SetCpuACUx:
		moveq	#(0),d0		| Prep D0.L as RC.
		pea	(7).w		| Mask out all possible interrupts.
		jbsr	_splx		|   (RC in D0.L).

		tstl	sp@(8)		| Check for AC0/AC1.
		jne	L_SSCAx_1	| -- br. for AC1.

		pmove	tt0,sp@		| Save current ACx value.
		pmove	sp@(12),tt0	| Set new ACx value.
		jra	L_SSCAx_2
L_SSCAx_1:
		pmove	tt1,sp@		| Save current ACx value.
		pmove	sp@(12),tt1	| Set new ACx value.
L_SSCAx_2:
		movel	d0,sp@-		| Restore previous interrupt level.
		jbsr	_splx		|   (RC in D0.L).

		movel	sp@(4),d0	| Set return value as old ACx value.
		addql	#(8),sp		| Clean-up stack pointer.
		rts

| ----------------------------------------------------------------------------
|	Modify current CPU Access Control Units (ACUs).
|	C Usage:	Sys_ModCpuACUx (u_int32 AcuNbr,
|					u_int32 NewACU_Val, u_int32 ACU_Mask) ;
|	Returns:	D0.L, previous ACU value.
|	Set a bit in the Mask to make it significant for setting from Value.
| ----------------------------------------------------------------------------
		.globl	_Sys_ModCpuACUx
_Sys_ModCpuACUx:
		movel	d1,sp@-		| Save D1.L for scratch use.
		moveq	#(0),d0		| Prep D0.L as RC.
		pea	(7).w		| Mask out all possible interrupts.
		jbsr	_splx		|   (RC in D0.L).

		movel	sp@(20),d1	| Grab ACx Mask value and
		notl	d1		|   invert to zero significant bits.

		tstl	sp@(12)		| Check for AC0/AC1.
		jne	L_MSCAx_1	| -- br. for AC1.

		pmove	tt0,sp@		| Save current ACx value.
		jra	L_MSCAx_2
L_MSCAx_1:
		pmove	tt1,sp@		| Save current ACx value.
L_MSCAx_2:
		andl	sp@,d1		| Mask out significant bits of ACx
		orl	d1,sp@(16)	|    and factor in new Value.
		tstl	sp@(12)		| Check for AC0/AC1.
		jne	L_MSCAx_3	| -- br. for AC1.

		pmove	sp@(16),tt0	| Set new ACx value.
		jra	L_MSCAx_4
L_MSCAx_3:
		pmove	sp@(16),tt1	| Set new ACx value.
L_MSCAx_4:
		movel	d0,sp@-		| Restore previous interrupt level.
		jbsr	_splx		|   (RC in D0.L).

		movel	sp@(4),d0	| Set return value as old ACx value.
		addql	#(8),sp		| Clean-up stack pointer.
		movel	sp@+,d1		| Restore D1.L.
		rts

