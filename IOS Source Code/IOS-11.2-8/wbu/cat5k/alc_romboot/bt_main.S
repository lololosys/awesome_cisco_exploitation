| $Id: bt_main.S,v 1.1.4.2 1996/06/04 05:31:13 rlowe Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/bt_main.S,v $
|-----------------------------------------------------------------
| bt_main.S -- ACP PROM Boot main entry point.
|		(Based on Catalyst Boot code.)
|
|		These portable 68k assembly routines relate to
|		kicking-off the Boot and getting basic power-up
|		diags done.  We try to get to C code ASAP.  Some
|		(boot-time) exception handlers are provided here too.
|
| November 28, 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: bt_main.S,v $
| Revision 1.1.4.2  1996/06/04  05:31:13  rlowe
| Fix 68k assembly bugs found by 96q1 tools.
| CSCdi59414:  ATM card bootrom gets assembly errors from 96q1 tools
| Branch: California_branch
|
| Revision 1.1.4.1  1996/05/09  14:59:59  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:55  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:41:04  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:12:05  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:28  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:43:08  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.5  1995/10/14  08:46:49  rlowe
| Fixes to allow NMP/SCP manufacturing diags to be run on system powerup.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "mc68ec030_defs.h"
#include "boot_defs.h"
#include "lo_error.h"


| ----------------------------------------------------------------------------
|	Typical Boot code CPU register usage conventions:
|		a7 (isp, sp)	- CPU stack pointer
|		a5		- pointer to board HW I/O registers
|		a4		- pointer to NVRAM
|		a2		- memory test failure address
|		a1, a0		- scratch (also consumed by C calls)
|
|		d2		- memory test failure data (actually read)
|		d1		- scratch (used by C), memory test expected data
|		d0		- scratch (also consumed by C calls)
|
|	Most registers, including those listed above, can also be used
|	for scratch purposes.  There is some variation in register usage
|	between ROM and RAM based Boot code.
| ----------------------------------------------------------------------------
| ----------------------------------------------------------------------------
|	This is the main Boot entry point.  We can get here by reset
|	vector, or by calling __jmpboot/__jmpprom.  Groom HW and system
|	registers in case we did not come in through the reset vector.
|	Do not forget to set some (debug) leds.
| ----------------------------------------------------------------------------
		.text
		.globl	_main
		.globl	__Boot_main
_main		= __Boot_main		| Set this for linker, external users.
__Boot_main:				| Boot main entry point.
		movew	#0x2700,sr	| Mask all interrupts (excluding NMI).
					| Should now be using ISP not MSP/USP.
		lea	__bt_VectorTbl,a0
		movel	a0@,sp		| Load sp (ISP) from reset vector.
		movec	a0,vbr		| VBR points to Boot vector table.
		movel	#(CPU030_DISAB_CACHES+CPU030_FLUSH_CACHES),d0
		movec	d0,cacr		| Flush and disable Inst/Data caches.
		moveq	#(0),d0		| Clear out
		movec	d0,caar		|   the CPU Cache Address Register.
		movec	d0,MSP		|   and CPU Master Stack Pointer
		movec	d0,USP		|   and CPU User Stack Pointer.
		pmove	(L_BM_DisabAcu).L,tt0
		pmove	(L_BM_DisabAcu).L,tt1
					| Disable both AC0/AC1.
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		ASM_DBG_WRREG (&(((PUD_POWER_UP) & 0x0f) ^ 0x0f),a5)
					| Dim all debug LEDs.
		reset			| Ensure all other board HW is reset.
		nop			| Give time for reset to take hold.
		nop
		nop
		nop
		nop			| Reset should have taken by now.
		nop			|   Wait a touch longer to be sure.
|
|	Basic hardware, CPU are groomed at this point.
|	We enable the cpu instruction cache at this point.
|	Do NOT enable the data cache since we will be doing
|	memory diags.
|
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		ASM_DBG_WRREG (&(((PUD_POWER_UP) & 0x0f) ^ 0x0f),a5)
					| Dim all debug LEDs.
		ASM_CHKJPR_DEBUG (a5@(ASM_HW_RREG_DEBUG))
		jeq	L_BM_RstDone	| Disable i-cache if debug mode.

		movel	#(CPU030_ENAB_I_CACHE+CPU030_FLUSH_I_CACHE),d0
		movec	d0,cacr		| Flush / enable cpu instruction cache.
L_BM_RstDone:
		jra	AR_PowerUpDiags	| Do (assembly) power-up diags.
|
|	Used to clear/disable 68ec030 ACU or 68030 MMU/TT registers.
|	Leave this in text section to be accessible when data section
|	has not yet been copied.
|
L_BM_DisabAcu:
		.long	(CPU030_DISAB_ACU)
					| Value to disable CPU ACU.
|
|	Come here once power-up diags are done, or if they are skipped.
|	Clear any BSS/stack area used by Boot, copy the Boot data section,
|	then jump to C Boot entry point.
|
		.globl	__Boot_main_1
__Boot_main_1:
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		lea	AR_SvcWatchdog,a2
					| Get address of watchdog service.
		lea	_bbss,a1	| Get address of BSS start.
		moveq	#(0),d2		| Fill with zeros.
		movel	#(_ebss),d1	| Get address of BSS end,
		subl	a1,d1		|   then find size of BSS area.
		moveq	#(0),d0		| Clear return code register.
		lea	L_Boot_main_2,a0 |Get caller return address.
		jra	AR_BlockFill	| Go fill BSS area in DRAM with zeros.

L_Boot_main_2:
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		lea	AR_SvcWatchdog,a2
					| Get address of watchdog service.
		lea	_bdata,a1	| Get address of DATA (dest) start.
		movel	#(_etext),d2	| Get address of copy data source.
		movel	#(_edata),d1	| Get address of DATA end,
		subl	a1,d1		|   then find size of DATA area.
		moveq	#(0),d0		| Clear return code register.
		lea	L_Boot_main_3,a0 |Get caller return address.
		jra	AR_BlockCopy	| Copy DATA section from Boot to DRAM.

L_Boot_main_3:
		ASM_ADDR_HW_REGS (a5)	| Get base address of HW IO registers.
		ASM_PET_WATCHDOG (a5)	| Keep doggie happy.
		movel	#(0x00112233),sp@ |Write some sentinel values at TOS.
		movel	#(0x44556677),sp@(4)
		movel	#(0x8899aabb),sp@(8)
		movel	#(0xccddeeff),sp@(12)
		jbsr	_boot		| Enter C Boot code, should not return.

		jra	__Boot_main	| Unexpected, but just in case ...

| ----------------------------------------------------------------------------
|	Generic exception handler used in Boot CPU exception table.
| ----------------------------------------------------------------------------
		.globl	__error_hdlr	| Ensure entered into address/link map.
__error_hdlr:
		ori	#(0x0700),sr	| Mask out interrupts.
|
|	Save off exception stack frame -- assume standard (4 short word) format.
|
		movew	sp@,_bt_exception_status
		movel	sp@(2),_bt_exception_pc
		movew	sp@(6),_bt_exception_vector
|
|	Save off all general registers to exception info area.
|
		moveml	d0-d7/a0-a7,_bt_exception_info+96
|
|	Now copy 24 longwords from stack to info area; use many regs as scratch.
|
		moveml	sp@,d0-d7/a0-a3
		moveml	d0-d7/a0-a3,_bt_exception_info+0

		moveml	sp@(0x30),d0-d7/a0-a3
		moveml	d0-d7/a0-a3,_bt_exception_info+48
|
|	Save off special CPU registers.
|
		movec	vbr,d0		| Get VBR.
		movec	isp,d1		| Get ISP.
		movec	msp,d2		| Get MSP.
		movec	usp,d3		| Get USP.
		movec	cacr,d4		| Get CACR.
		moveml	d0-d4,_bt_exception_info+160
|
|	Now ensure CPU caches disabled and restore general registers used
|	above as scratch during copy to info area.
|	Then call C exception handler routine -- do NOT expect to return !
|
		movel	#(CPU030_DISAB_CACHES+CPU030_FLUSH_CACHES),d0
		movec	d0,cacr		| Flush and disable Inst/Data caches.
		pmove	(L_BM_DisabAcu).L,tt0
		pmove	(L_BM_DisabAcu).L,tt1
					| Disable both AC0/AC1.
		moveml	_bt_exception_info+96,d0-d7/a0-a3
		ASM_CALL_C_ROUTINE (_c_error_hdlr)
		rte			| Should never return to here !

