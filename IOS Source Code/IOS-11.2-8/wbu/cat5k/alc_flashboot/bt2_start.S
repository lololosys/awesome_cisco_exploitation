| $Id: bt2_start.S,v 1.1.4.2 1996/05/31 01:02:49 venkat Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_flashboot/bt2_start.S,v $
| ----------------------------------------------------------------------------
| bt2_start.S -- 680x0 Boot2 start-up code.
|
| This file provides the main entry point for the ACP Secondary Boot.
| It is called by the Primary Boot using the Image Header for the
| Secondary Boot image.  This basically sets up the Data/Bss sections
| for the Secondary Boot.  (Use Primary Boot stack and exception vectors
| for now.)  Then we call the "C" main routine for the Secondary Boot.
| Diddle the stack so that if the main routine returns, it will
| ignore this routine and get back directly to the PROM Boot caller.
|
| December 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
| ----------------------------------------------------------------------------
| $Log: bt2_start.S,v $
| Revision 1.1.4.2  1996/05/31  01:02:49  venkat
| CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
| Branch: California_branch
| Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
| tool
| chain
|
| Revision 1.1.4.1  1996/05/09  14:55:13  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:14:41  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:39:31  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:10:50  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:41:41  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:40:09  rlowe
| Placeholders for California.
|
| ----------------------------------------------------------------------------
| $Endlog$

| ----------------------------------------------------------------------------
| 680x0 Boot2 start-up code.
|	This file provides the main entry point for the ACP Secondary Boot.
|	It is called by the Primary Boot using the Image Header for the
|	Secondary Boot image.  This basically sets up the Data/Bss sections
|	for the Secondary Boot.  (Use Primary Boot stack and exception vectors
|	for now.)  Then we call the "C" main routine for the Secondary Boot.
|	Diddle the stack so that if the main routine returns, it will
|	ignore this routine and get back directly to the PROM Boot caller.
| ----------------------------------------------------------------------------
| Modification History (Synergy):
|	12/21/94  RKL	- File originated.
| ----------------------------------------------------------------------------

#include "asm_mode_defs.h"
#include "boot_defs.h"

| ----------------------------------------------------------------------------
|	This is the Secondary Boot main entry point.  Assume system stack
|	has been setup, and system RAM is valid, by way of Primary Boot.
| ----------------------------------------------------------------------------
		.text
		.globl	__Bt2_Start
__Bt2_Start:				| Secondary Boot main entry point.
		link	fp,#(0)		| Setup stack frame.
		moveml	d1-d7/a0-a6,sp@- |Save registers consumed by C.
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.

		lea	AR_SvcWatchdog,a2
					| Get address of watchdog service.
		lea	_bbss,a1	| Get address of BSS start.
		moveq	#(0),d2		| Fill with zeros.
		movel	#(_ebss),d1	| Get address of BSS end,
		subl	a1,d1		|   then find size of BSS area.
		moveq	#(0),d0		| Clear return code register.
		lea	L_Bt2_start_2,a0 |Get caller return address.
		jra	AR_BlockFill	| Go fill BSS area in DRAM with zeros.

L_Bt2_start_2:
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.

		lea	AR_SvcWatchdog,a2
					| Get address of watchdog service.
		lea	_bdata,a1	| Get address of DATA (dest) start.
		movel	#(_etext),d2	| Get address of copy data source.
		movel	#(_edata),d1	| Get address of DATA end,
		subl	a1,d1		|   then find size of DATA area.
		moveq	#(0),d0		| Clear return code register.
		lea	L_Bt2_start_3,a0 |Get caller return address.
		jra	AR_BlockCopy	| Copy DATA section from Boot to DRAM.

L_Bt2_start_3:
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.

		moveml	sp@+,d1-d7/a0-a6 |Restore consumed registers.
		unlk	fp		| Pop our stack frame.
		jra	_Bt2_Main	| Hit C Secondary Boot.
|
|	Do NOT change the parameters to this function call without ALSO
|	changing the calls in the Primary Boot (boot.c) or BCS (bcs_ServIf.c).
|
