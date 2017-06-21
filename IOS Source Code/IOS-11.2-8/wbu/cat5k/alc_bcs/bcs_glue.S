| $Id: bcs_glue.S,v 1.1.4.1 1996/05/09 14:54:09 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_glue.S,v $
|-----------------------------------------------------------------
| bcs_glue.S -- Boot Code Service (BCS) Boot2-IOS Boot glue code.
|		68k assembly code for glueing between the ACP
|		Secondary Boot and IOS Boot images.
|
|		To ensure backward compatibility with older Boot code,
|		ADD but do NOT CHANGE any definitions given here.
|
| January 23, 1995, Rick Lowe
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: bcs_glue.S,v $
| Revision 1.1.4.1  1996/05/09  14:54:09  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:13:38  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:38:03  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:09:36  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:40:34  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:39:06  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.2  1995/10/13  06:06:58  rlowe
| Enable and use BCS checksums in PROM and flash Boots.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "boot_defs.h"

| ----------------------------------------------------------------------------
|	This is the Secondary Boot glue code to IOS.  Assume system stack
|	has been setup, and system RAM is valid, by way of Primary Boot.
| ----------------------------------------------------------------------------
		.text
		.globl	_bcs_Glue1
_bcs_Glue1:
		link	fp,#(0)		| Setup stack frame.
		moveml	d1-d7/a0-a6,sp@- |Save registers consumed by C.
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.

		movel	fp@(0x08+0x00),a1
					| Argument is base of IOS Boot (VecTbl).
		moveal	a1@(4),a0	| Grab IOS Boot Reset-PC.
		movel	a1@,d0		| Grab IOS Boot Reset-SP.
		movel	d0,sp@-		| Push IOS Boot Reset-SP to cur stack.
		movel	sp,__bcs_AcpBtSP |Save current (ACP Boot) SP.
		exg	d0,sp		| Swap from cur SP to IOS Boot SP.
		movel	d0,sp@-		| Push old SP to IOS Boot stack.
		moveq	#(0),d0		| Clear return code register.
		jbsr	a0@		| C Secondary Boot, return RC on error.
		moveal	__bcs_AcpBtSP,sp |Recover ACP Boot stack.
		addql	#(4),sp		| Pop IOS Boot Reset-SP off ACP Bt stack

		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		moveml	sp@+,d1-d7/a0-a6 |Restore registers consumed by C.
		unlk	fp		| Pop stack frame.
		rts			| Return to Primary Boot (d0.L is RC).

		.data
		.align	1
__bcs_AcpBtSP:
		.long	0
