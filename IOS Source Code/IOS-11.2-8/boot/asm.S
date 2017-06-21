| $Id: asm.S,v 3.5.20.5 1996/05/10 23:16:40 gstovall Exp $
| $Source: /release/112/cvs/Xboot/asm.S,v $
|------------------------------------------------------------------
| asm.S -- assembler routines common to the 68000 based CSC1,
|          the 68020 based CSC2 & CSC3, and the 68040 based CSC4.
|
| June 1989, Chris Shaker
|
| Copyright (c) 1989-1996,97 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm.S,v $
| Revision 3.5.20.5  1996/05/10  23:16:40  gstovall
| CSCdi56792:  Need to remove unused files
| Branch: California_branch
| The beast still exists, finish killing off the CS500, and while I am in
| there, nuke the even crustier CSC1.
|
| Revision 3.5.20.4  1996/05/09  13:59:32  rbadri
| Branch: California_branch
| Synergy features
|
| Revision 3.5.20.3.6.1  1996/04/27  06:12:46  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 3.5.20.3  1996/04/12  18:11:28  smackie
| Make sure new CPUs show up in the old delay table too. (CSCdi53907)
| Branch: California_branch
|
| Revision 3.5.20.2  1996/04/05  02:41:35  yoko
| CSCdi53681:  add new cpu types
| Branch: California_branch
| define new cpu types and emt calls
|
| Revision 3.5.20.1.4.1  1996/04/08  01:34:05  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 3.5.20.1  1996/03/26  18:24:33  enf
| CSCdi52111:  Need to reserve #defines for new platform
| Branch: California_branch
| Define CPU_VOLCANO
|
| Revision 3.5.26.1  1996/03/22  09:06:40  rlowe
| Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 3.5.10.1  1996/03/03  21:18:44  rlowe
| Apply SYNALC content to merge branch.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 3.5.8.1  1996/03/02  01:06:21  rlowe
| Non-sync sync to mainline tag V111_1_0_3.
| Branch: Synalc3_Cal_V111_1_0_3_branch
|
| Revision 3.5  1996/01/30  21:10:54  vandys
| CSCdi46080:  Deep six CS500 in source base
|
| Revision 3.4.16.1  1996/02/29  07:41:42  rlowe
| First pass non-sync sync for Synergy ATM line card to V111_0_16.
| Branch: Synalc3_Cal_V111_0_16_branch
|
| Revision 3.4  1995/12/31  23:25:02  anil
| i86 CPU
|
| Revision 3.3  1995/11/20  19:35:05  mbeesley
| CSCdi43065:  need to reserve cpu types for future development
|
| Revision 3.2  1995/11/17  07:40:48  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  09:03:55  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.2  1995/11/08  20:13:42  shaker
| Merge Arkansas_branch into 11.1 mainline.
|
| Revision 2.1.52.1.2.1  1996/02/03  06:11:25  rlowe
| Non-sync sync of Synalc_catchup_branch to V111_0_5.
| Branch: Synalc_catchup_pretal_branch
|
| Revision 2.1.44.1  1995/10/03  07:04:24  rlowe
| Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
|
| Branch: Synalc2_LE2_V110_1_0_3_branch
|
| Revision 2.1  1995/06/07  19:11:25  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

#define ASMINCLUDE

#include "../reason.h"
#include "../defs.h"
#include "../cpu.h"

|
| Startup exception vector
|
	.text
	.even
	.globl start,evect
evect:	.long INITSTACKP		|reset: initial ssp
	.long start			|reset: initial pc

|
| Exception table
|
	.text
	.even
	.globl _evec,_evece,_flags,refreshe

_evec:	jmp intrzero			|00: Jump to zero handler
_flags:	.word 0				|06: Flags from EPROM programmer
	.long buserror			|08: bus error
	.long addrerror			|0C: address error
	.long intrill			|10: illegal instriction
	.long intrzdiv			|14: zero divide
	.long intrchk			|18: chk
	.long intrtrapv			|1C: trapv
	.long intrpriv			|20: privilege instruction
	.long intrtrace			|24: trace
	.long intr1010			|28: line 1010 Emulator
	.long intr1111			|2C: line 1111 Emulator
	.long intrundef			|30: Unassigned vector
	.long intrcoproc		|34: coprocessor protocol violation
	.long intrformerr		|38: format error
	.long intrundef			|3C: Uninitialized Interrupt
	.long intrundef			|40: Unassigned vectors:
	.long intrundef			|44:
	.long intrundef			|48:
	.long intrundef			|4C:
	.long intrundef			|50:
	.long intrundef			|54:
	.long intrundef			|58:
	.long intrundef			|5C:
	.long intrspur			|60: spurious interrupt
	.long intrlev1			|64: level 1 interrupt autovector
	.long intrlev2			|68: level 2
	.long intrlev3			|6C: level 3
	.long intrlev4			|70: level 4
	.long intrlev5			|74: level 5
	.long intrlev6			|78: level 6
#ifdef	SYNALC
	.long intrlev7			|7C: level 7 - SYNALC watchdog
#else
	.long refreshe			|7C: level 7 - refresh
#endif
	.long intrtrap			|80: trap 0
	.long trapbreak			|84: trap 1 - breakpoint
	.long trapcrash			|88: trap 2 - software forced crash
	.long intrtrap			|8C: trap 3
	.long intrtrap			|90: trap 4
	.long intrtrap			|94: trap 5
	.long intrtrap			|98: trap 6
	.long intrtrap			|9C: trap 7
	.long intrtrap			|A0: trap 8
	.long intrtrap			|A4: trap 9
	.long intrtrap			|A8: trap 10
	.long intrtrap			|AC: trap 11
	.long intrtrap			|B0: trap 12
	.long intrtrap			|B4: trap 13
	.long trapexit			|B8: trap 14 - exit
	.long trapemulate		|BC: trap 15 - emulate
#ifdef SYNALC
	.long intrundef			|C0: Unassigned vectors:
	.long intrundef			|C4:    (reserved by Motorola)
	.long intrundef			|C8:
	.long intrundef			|CC:
	.long intrundef			|D0:
	.long intrundef			|D4:
	.long intrundef			|D8:
	.long intrundef			|DC:
	.long intrundef			|E0:
	.long intrundef			|E4:
	.long intrundef			|E8:
	.long intrundef			|EC:
	.long intrundef			|F0:
	.long intrundef			|F4:
	.long intrundef			|F8:
	.long intrundef			|FC:
	.long intrundef			|100: MFP GPI0 - TxSar
	.long intrundef			|104: MFP GPI1 - RxSar
	.long intrundef			|108: MFP GPI2 - LCP Msg Posted
	.long intrundef			|10C: MFP GPI3 - LCP Msg Rcvd
	.long intrundef			|110: MFP TMRD - UART BRG
	.long intrundef			|114: MFP TMRC - Unused
	.long intrundef			|118: MFP GPI4 - NTC/PMC
	.long intrundef			|11C: MFP GPI5 - Unused
	.long intrundef			|120: MFP TMRB - Boot clock
	.long intrundef			|124: MFP TXER - UART Tx Error
	.long intrundef			|128: MFP TXBE - UART Tx BufEmp
	.long intrundef			|12C: MFP RXER - UART Rx Error
	.long intrundef			|130: MFP RXBF - UART Rx BufFull
	.long refreshe			|134: MFP TMRA - IOS clock
	.long intrundef			|138: MFP GPI6 - Unused
	.long intrundef			|13C: MFP GPI7 - Unused, Mfg jpr
#endif

_evece:	.long 0				|place holder for stupid assembler 

|
| Restart the system
|	restart(reason, pc, buserror_address, stack_trace_ptr)
|	long reason
|	long pc
|	long buserror_address
|	long stack_trace_ptr
| 
	.text
	.globl sinit
	.globl initc
	.globl enter
	.globl _monitor
	.globl _restart
	.globl _cpu_type
_restart:
#ifdef RP1
	clrl d1
	movc d1, cacr			|turn off data & instruc. caches
	.word	0xf4f8			|cpush bc  ; push data cache, then    
					|invalidate both caches
#else
#ifdef CSC2
	cmpb #CPU_CSC4, _cpu_type	|get CPU type; is this a csc/4?
	bne cont_restart		|continue with restart
	clrl d1
	movc d1, cacr			|turn off data & instruc. caches
	.word	0xf4f8			|cpush bc  ; push data cache, then    
					|invalidate both caches
#endif CSC2
#endif RP1

cont_restart:
	jsr _flush			|make sure UART is empty

#if defined(XX) || defined(PAN) || defined(SYNALC)
	clrl d0
	movc d0,cacr
	movw #CLEAR_CACHES,d0
	movc d0,cacr			|clear caches
#if !defined(XX) && !defined(SYNALC)
	.globl _disable_nmi
	jbsr _disable_nmi
#endif /* !defined(XX) */
#endif

	movl #INITSTACKP,a0		|where to save reboot stuff
	movl sp@(4),a0@+		|save reason
	movl sp@(8),a0@+		|save pc at last reboot
	movl sp@(12),a0@+		|save buserr address


	moveq #BOOT_STACK_FRAMES,d0	|how many stack frames to copy
	movl sp@(16),a1			|get pointer to stack trace
#if defined(MC68020) || defined(MC68040)
	tstl a1				|mova does not set condition codes!
#else
	cmpl #0,a1
#endif
	beq zero_stack_trace		|if null pointer, zero stack trace

save_frames:
	movl a1@+,a0@+			|copy frame pointer
	movl a1@+,a0@+			|copy pc
	subql #1, d0			|one less frame to copy
	bne save_frames			|copy next frame
	bra frames_done			|done

zero_stack_trace:
	moveq #0,d1			|NULL for zero_stack_trace
zero_next_frame:
	movl d1,a0@+			|zero frame pointer
	movl d1,a0@+			|zero pc
	subql #1,d0			|one less frame to zero
	bne zero_next_frame		|zero next frame

frames_done:
#ifdef SYNALC
|
| Force HW into known state by forcing a hard reset, down to bottom Boot.
| While this does not preserve DRAM state, it does put the board into a
| known state.  The HW Reset Reason register should tell us that we are
| restarting; but DRAM could be trashed by diags.  Another answer would
| be to repeat much of the Boot1/2 HW grooming here, in the IOS Boot.
|
	jra _A_ResetCPU			|force a board-reset (both HW + SW)

#else  /* SYNALC */

#ifndef NORESET
	reset				|make sure timer sleeps
#endif
#endif /* SYNALC */

	movl #INITSTACKP,sp		|reset stack pointer
#ifndef MC68000
	lea  _evec,a0			|get address of vectors
	movc a0,vbr			|Set VBR
#endif MC68000
	movl #0,a6			|clear frame pointer for dumpstack
	moveq #0,d0			|Clear "startup" to preserve reasons
	jmp sinit			|init returns to initc

|
| Startup code
|
| NOTE: We put a value in d0 for the variable, startup, in init.c
|       before invoking sinit. We cannot use memory yet.
|

start:
	movl #1000,d0			|loop here until rest of machine
loop:	subql #1,d0			|gets warmed up
	bgt loop			|again?
	movl #0,a6			|clear frame pointer for dumpstack
#ifndef MC68000
	lea  _evec,a0			|get address of vectors
	movc a0,vbr			|Set VRB
#endif MC68000
	moveq #1,d0			|Set "startup" to erase reasons
	jmp sinit			|initialize, setup memory

initc:					|(sinit returns here)
#ifdef RP1
	cmpb #CPU_RP1, _cpu_type	|get CPU type; is this an RP1?
	beq set_adrspc  		|yes, its a 68040...
#else
#ifdef CSC2
	cmpb #CPU_CSC4, _cpu_type	|get CPU type; is this a csc/4?
	beq set_adrspc  		|yes, its a csc/4...
#endif CSC2
#endif RP1

#if defined(MC68020) && ! defined(XX) && ! defined(PAN) && ! defined(SYNALC)
	movl #ENA_ICACHE,d0		|XX enables caches in init.c
	movc d0,cacr			|enable cache
#endif MC68020
	bra cont_initc			|continue with initc

#if defined(CSC2) || defined(RP1)
set_adrspc:
	.word	0xf4d8			|cinva bc ; invalidate both caches 
	clrl d0
	.long	0x4e7b0003		|movc d0, tc
					|disable MMU support
#ifdef CSC2
	movl #0x0200a040, d0
	.long	0x4e7b0007		|movc d0, dtt1
					|set 0x02xxxxxx to non-cachable/serial
	movl #0x0000a020, d0
	.long	0x4e7b0006		|movc d0, dtt0
					|set 0x00xxxxxx to cachable/copy-back
#else
#ifdef RP1
        movl #0x11ffa040, d0
        .long   0x4e7b0007              |movc d0, dtt1
                                        |set 0x11xxxxxx -> 0xffffffff
                                        | to non-cachable/serial
        movl #0x000fa020, d0
        .long   0x4e7b0006              |movc d0, dtt0
                                        |set 0x00000000 -> 0x0fffffff
                                        |to cachable/copy-back

#endif RP1
#endif CSC2
                                        |by default, all other address space is
                                        |set to cachable/write-through; this
                                        |includes ROM adddress space 0x01xxxxxx
	movl #BOTH_CACHE_040, d0	|load cache control value into register
        movc d0, cacr			|enable both instruction and data
					|cache on 68040 
#endif CSC2 || RP1

cont_initc:
	movl #INITSTACKP,sp		|return here from start and restart
	movl #0,a6			|clear frame pointer for dumpstack
	movl #0,sp@-			|save (fake) pc
	movw sr,sp@-			|save sr
	movw #0,sp@-			|padding
	movl #EXC_RESET,sp@-		|save longword reason
enter:
	subql #4,sp			|make room for usp
	moveml d0-d7/a0-a7,sp@-		|save registers on stack
	movl usp,a0			|get usp
	movl a0,sp@(64)			|save it on stack
	orw #0x0700,sr			|set priority level to 7
	jsr _monitor			|jump to the command parser
	movl sp@(64),a0			|get usp back
	movl a0,usp			|save it where it belongs
	moveml sp@+,d0-d7/a0-a7		|restore registers from stack
	addl #10,sp			|pop usp, reason and padding
	rte

|
| Enable cache
|

#ifdef MC68020

	.globl _cacheon,_cacheoff,_icacheon,_icacheoff

_cacheon:
	movw #CLEAR_CACHES,d0
	movc d0,cacr			|clear caches (kill any crud in cache)
	movl #ENABLE_CACHES,d0
	movc d0,cacr			|enable cache
	rts

_cacheoff:
	clrl d0
	movc d0,cacr			|disable caches
	movw #CLEAR_CACHES,d0
	movc d0,cacr			|clear caches
	rts

_icacheon:
	movw #CLEAR_CACHES,d0
	movc d0,cacr			|clear caches (kill any crud in cache)
	movl #ENA_ICACHE,d0
	movc d0,cacr			|enable cache
	rts

_icacheoff:
	movc cacr,d0
	andw #~ENA_ICACHE,d0
	movc d0,cacr			|disable instruction cache
	orw #CLR_ICACHE,d0
	movc d0,cacr			|clear instruction cache
	rts
#endif /* MC68020 */

|
| Enable/Disable caches on 68020, 68030, and 68040 processors:
|
#if defined(MC68020) || defined(MC68040)
	.globl _setcacr
_setcacr:
	movc cacr,d0			|return the current contents
	movl sp@(4),d1			|get the new cacr value from stack
	movc d1,cacr			|put it into the cacr

        cmpb    #CPU_CSC4, _cpu_type	| Is this a CSC/4?
        beq     is_68040		| Yes, it is a 68040

	cmpb	#CPU_RP1, _cpu_type	| Is this an RP1?
	beq	is_68040		| Yes, this is a 68040

	bra	not_68040		| No, this is a 68020 or 68030

is_68040:
	.word	0xf4f8			|cpush bc  ; push data cache, then    
					|invalidate both caches

not_68040:
        rts

|
| return the current contents of the CACR
|
	.globl _getcacr
_getcacr:
	movc cacr,d0			|return the current contents
	rts

#endif /* MC68020 || MC68040 */

|
| Addressing errors
|
addrerror:
	movl sp,_error			|save pointer to stack frame
	movw #0,sp@-			|padding
	movl #EXC_ADRERR,sp@-		|long reason
	jmp enter			|enter monitor

|
| Coprocessor protocol violation
|

intrcoproc:
	movw #0,sp@-			|padding
	pea coprocmsg			|message address
	jmp enter			|enter monitor

coprocmsg: .asciz "Coprocessor protocol violation"
	.even
	.text

|
| Format error
|

intrformerr:
	movw #0,sp@-			|padding
	pea formmsg			|message address
	jmp enter			|enter monitor

formmsg: .asciz "Format error"
	.even
	.text



|
| Trace Exception
|
	.text
	.even
	.globl intrtrace
intrtrace:
	movw #0,sp@-			|padding
	movl #EXC_TRACE,sp@-		|long reason
	jmp enter			|enter monitor


|
| Trap routines
|

|
| Trap 1 -- Breakpoint
|
	.text
	.even
	.globl trapbreak
trapbreak:
	movw #0,sp@-			|padding
	movl #EXC_BREAK,sp@-		|long reason
	jmp enter			|enter monitor

|
| Trap 14 -- Exit
|
	.text
	.even
	.globl trapexit
trapexit:
	movw #0,sp@-			|padding
	movl #EXC_EXIT,sp@-		|long reason
	jmp enter			|enter monitor

|
| Trap 15 -- Emulation
|
	.text
	.even
	.globl trapemulate
trapemulate:
	movw #0,sp@-			|padding
	movl #EXC_EMT,sp@-		|long reason
	jmp enter			|enter monitor

|
| Random interrupt and trap service routines
|

|
| Illegal Instruction Exception
|
	.text
	.globl intrill
intrill:
	movw #0,sp@-			|padding
	pea illmsg			|message address
	jmp enter			|enter monitor

illmsg:	.asciz "Illegal Instruction"

|
| Zero Divide Exception
|
	.text
	.even
	.globl intrzdiv
intrzdiv:
	movw #0,sp@-			|padding
	pea zdivmsg			|message address
	jmp enter			|enter monitor

zdivmsg: .asciz "Zero Divide"

|
| CHK Exception
|
	.text
	.even
	.globl intrchk
intrchk:
	movw #0,sp@-			|padding
	pea chkmsg			|message address
	jmp enter			|enter monitor

chkmsg:	.asciz "CHK"

|
| TRAPV Exception
|
	.text
	.even
	.globl intrtrapv
intrtrapv:
	movw #0,sp@-			|padding
	pea trapvmsg			|message address
	jmp enter			|enter monitor

trapvmsg: .asciz "TRAPV"

|
| Privilege Instruction Exception
|
	.text
	.even
	.globl intrpriv
intrpriv:
	movw #0,sp@-			|padding
	pea privmsg			|message address
	jmp enter			|enter monitor

privmsg: .asciz "Privilege Instruction"

|
| Line 1010 Emulator Exception
|
	.text
	.even
	.globl intr1010
intr1010:
	movw #0,sp@-			|padding
	pea L1010msg			|message address
	jmp enter			|enter monitor

L1010msg: .asciz "Line 1010 Emulator"

|
| Line 1111 Emulator Exception
|
	.text
	.even
	.globl intr1111
intr1111:
	movw #0,sp@-			|padding
	pea L1111msg			|message address
	jmp enter			|enter monitor

L1111msg: .asciz "Line 1111 Emulator"

|
| Spurious Exception
|
	.text
	.even
	.globl intrspur
intrspur:
	movw #0,sp@-			|padding
	pea spurmsg			|message address
	jmp enter			|enter monitor

spurmsg: .asciz "Spurious Interrupt"

|
| Jump to Zero
|
	.text
	.even
	.globl intrzero
intrzero:
	pea zeromsg			|message address
	jmp enter			|enter monitor

zeromsg: .asciz "Jump to zero"

|
| Undefined Exception
|
	.text
	.even
	.globl intrundef
intrundef:
	movw #0,sp@-			|padding
	pea undefmsg			|message address
	jmp enter			|enter monitor

undefmsg: .asciz "Undefined"

|
| Level 1 Exception
|
	.text
	.even
	.globl intrlev1
intrlev1:
	movw #0,sp@-			|padding
	pea lev1msg			|message address
	jmp enter			|enter monitor

lev1msg: .asciz "Level 1"

|
| Level 2 Exception
|
	.text
	.even
	.globl intrlev2
intrlev2:
	movw #0,sp@-			|padding
	pea lev2msg			|message address
	jmp enter			|enter monitor

lev2msg: .asciz "Level 2"

|
| Level 3 Exception
|
	.text
	.even
	.globl intrlev3
intrlev3:
	movw #0,sp@-			|padding
	pea lev3msg			|message address
	jmp enter			|enter monitor

lev3msg: .asciz "Level 3"

|
| Level 4 Exception
|
	.text
	.even
	.globl intrlev4
intrlev4:
	movw #0,sp@-			|padding
	pea lev4msg			|message address
	jmp enter			|enter monitor

lev4msg: .asciz "Level 4"

|
| Level 5 Exception
|
	.text
	.even
	.globl intrlev5
intrlev5:
	movw #0,sp@-			|padding
	pea lev5msg			|message address
	jmp enter			|enter monitor

lev5msg: .asciz "Level 5"

|
| Level 6 Exception
|
	.text
	.even
	.globl intrlev6
intrlev6:

#ifdef XX
	movw #0,sp@-			|padding
	movl #EXC_SHMEM_PE,sp@-		|long reason
	jmp enter			|enter monitor
#else
	movw #0,sp@-			|padding
	pea lev6msg			|message address
	jmp enter			|enter monitor

lev6msg: .asciz "Level 6"
#endif XX

#ifdef	SYNALC
|
| Level 7 Exception
|
	.text
	.even
	.globl intrlev7
intrlev7:
	movw #0,sp@-			|padding
	pea lev7msg			|message address
	jmp enter			|enter monitor

lev7msg: .asciz "Level 7"
#endif	SYNALC



|
| Software forced crash
|
	.text
	.even
	.globl trapcrash
trapcrash:
	movw #0,sp@-			|padding
	pea crashmsg			|message address
	jmp enter			|enter monitor

crashmsg: .asciz "Software forced crash"
|
| Undefined Trap Exception
|
	.text
	.even
	.globl intrtrap
intrtrap:
	movw #0,sp@-			|padding
	pea trapmsg			|message address
	jmp enter			|enter monitor

trapmsg: .asciz "Undefined Trap"

|
| Set Interrupt Level
|
	.text
	.even
	.globl _sil
_sil:
	movl	sp@(4),d0		|get new interrupt level
	andw	#0x07,d0		|use only values 0-7 (bits 0,1,2)
	lslw	#8,d0			|shift new level left 8 bits
	movw	sr,d1			|get current sr
	andw	#0xF8FF,d1		|clear current interrupt level
	orw	d0,d1			|"or" new level into sr bits
	movw	sr,d0			|return in d0 the current copy of sr
	lsrw	#8,d0			|shift intr level back to lsb
	andw	#0x0007,d0		|ignore all the extra cruft
	movw	d1,sr			|set new interrupt level
	rts

|
| Byte swap routine for shorts
|
	.text
	.even
	.globl	_swaps
_swaps:
	movl	sp@(4),d0		|get word to be swapped
	rolw	#8,d0			|an 8-bit rotate is a byte swap
	rts

|
| Byte swap routine for longs
|
	.text
	.even
	.globl	_swapl
_swapl:
	movl	sp@(4),d0		|get long to be swapped
	rolw	#8,d0			|swap bytes 3 and 4 -> 1 2 4 3
	swap	d0			|swap words 1 and 2 -> 4 3 1 2
	rolw	#8,d0			|swap bytes 1 and 2 -> 4 3 2 1
	swap	d0			|swap words 1 and 2 -> 2 1 4 3
	rts				|all done

| bcopy
| Very fast byte copy.  By making the byte count a multiple of 2 or 4,
| and having the pointers word or long aligned, it is possible to speed 
| up the copy significantly, so fudge byte counts whenever possible.
|
| bcopy(source,destation,bytes)
|    unsigned char *source,*destination;
|    int bytes;

	.globl _bcopy
_bcopy:	movl	sp@(4),a0	| pick up source
	movl	a0,d1
	movl	sp@(8),a1	| pick up destination
	movl	a1,d0
	orl	d0,d1
	movl	sp@(12),d0	| pick up numbytes
	orl	d0,d1		| or together all 3

	cmpl	a0,a1		| copy up or down?
	bgt	bcopyup

	btst	#0,d1		| the direction of copy is down
	beq	_bcopy2S

_bcopy1S:
	movb	a0@+,a1@+
	subql	#1,d0
	bgt	_bcopy1S
	rts

_bcopy2S:
	btst	#1,d0
	beq	_bcopy4S
_bcopy3S:
	movw	a0@+,a1@+
	subql	#2,d0
	bgt	_bcopy3S
	rts

_bcopy4S:
	movl	a0@+,a1@+
	subql	#4,d0
	bgt	_bcopy4S
	rts

bcopyup:
	addl	d0,a0		| the direction of copy is up
	addl	d0,a1		| start copy from end and work back

	btst	#0,d1		| is everything even?
	beq	bcopy2Sup
bcopy1Sup:
	movb	a0@-,a1@-
	subql	#1,d0
	bgt	bcopy1Sup
	rts

bcopy2Sup:
	btst	#1,d0		| is count lword aligned?
	beq	bcopy4Sup

bcopy3Sup:
	movw	a0@-,a1@-
	subql	#2,d0
	bgt	bcopy3Sup
	rts

bcopy4Sup:
	movl	a0@-,a1@-
	subql	#4,d0
	bgt	bcopy4Sup
	rts
	

	.globl _make_buserror
_make_buserror:
	|
	| Save context:
	|
	movl	d2,sp@-			| get a work register
	moveb _abortstate,d1		| savstate = abortstate;
	movew _buserrcnt,a1		| savbuserrcnt = buserrcnt;
	movel _buserrhandler,d2		| savbuserrhandler = buserrhandler;

	|
	| Set up buserrnote() as the bus error handler, then
	| generate a bus error:
	|
	movel #_buserrnote,_buserrhandler | buserrhandler = buserrnote;
	moveb #ABORT_INTERNAL,_abortstate | abortstate = ABORT_INTERNAL;
	lea  BADADDRESS,a0		| get a bad address
	movw a0@,d0			| read a word from it
	nop
	nop				| With a 68000, we do not know
	nop				| which one of these NOPs will
	nop				| be executed after the RTE from
	nop				| the buserror!

	|
	| Restore context:
	|
	moveb d1,_abortstate		| abortstate = savstate;
	movew a1,_buserrcnt		| buserrcnt = savbuserrcnt;
	movel d2,_buserrhandler		| buserrhandler = savbuserrhandler;
	movl	sp@+,d2			| Restore work register
	rts

|
| ram_dispatch(code, name, addr) - Save context and invoke system image in RAM
| int	code
| char *name
|
| Chris Shaker, June 21, 1989
|
	.globl save_regs
	.lcomm save_regs,48
	.lcomm save_regs_end,0

	.text
	.even
	.globl _ram_dispatch
_ram_dispatch:
	movl	sp@(4),d0		| get code
	movl	sp@(8),d1		| get name
	movl	sp@(12),a1		| get entry point
	movl	#save_regs_end,a0	| addr of one long past end
	moveml	d2-d7/a2-a7,a0@-	| save context

	movel	d1,sp@-			| push name onto stack for _start()
	movel	d0,sp@-			| push code onto stack for _start()
	jbsr	a1@			| invoke system code in RAM

	movl	#save_regs,a0		| addr of start
	moveml	a0@+,d2-d7/a2-a7	| restore context
	rts

|
| prom2_dispatch(code, name) - Save context and invoke system image in ROM
| int code
| char *name
|
| Chris Shaker, June 21, 1989
|
	.text
	.even
	.globl _prom2_dispatch
_prom2_dispatch:
	movl	sp@(4),d0		| get code
	movl	sp@(8),d1		| get name
	movl	#save_regs_end,a0	| addr of one long past end
	moveml	d2-d7/a2-a7,a0@-	| save context

	movel	d1,sp@-			| push name onto stack for _start()
	movel	d0,sp@-			| push code onto stack for _start()
	movel	_prom2,a0
	jbsr	a0@			| invoke system code in RAM

	movl	#save_regs,a0		| addr of start
	moveml	a0@+,d2-d7/a2-a7	| restore context
	rts
|
|
| Perform a spin-wait delay in milliseconds
|
| delay(ms)
| uint ms
|
	.globl _cpu_type

	.globl _delay
_delay:
	movl sp@(4),d0		| Number of milliseconds to delay
	movl #delay_table,a0	| Get address of table
	movb _cpu_type,d1	| Get CPU type code returned from monitor ROM
	extw d1			| Sign extend to a word
	extl d1			| And now a longword
	asll #1,d1		| Compute cpu_type * 2
	addl d1,a0		| Address of number of inner loops per ms =
				|	table + cpu_type * 2
	movw a0@,a1		| Get number of inner loops per millisecond
outer_loop:
	movl a1,d1		| Reinitialize inner loop counter
inner_loop:
	subql #1,d1		| One less inner loop to do
	beq inner_done		| Inner loop done?
	bra inner_loop		| No, keep going

inner_done:
	subql #1,d0		| One less outer loop to do
	bne outer_loop		| More outer loops to do?

	rts			| Return
 |
 | Lookup table with number of inner loops per millisecond,
 | indexed by CPU type number:
 |
delay_table:
	.word	-1	| CPU_SMI8
	.word	-1	| CPU_CADLINC
	.word	-1	| CPU_SMI10
	.word	-1	| CPU_FTI10
	.word	384	| CPU_CSC1
	.word	-1	| CPU_NIM186
	.word	1000	| CPU_CSC2
	.word	-1	| CPU_MPU12
	.word	2500	| CPU_CSC3
	.word	384	| CPU_STS1
	.word	1333	| CPU_PAN
	.word	2500	| CPU_MERLOT
	.word	1000	| CPU_LEMONADE (RIP)
	.word	3570	| CPU_CSC4
	.word	3330	| CPU_XX        (provisional)
	.word	2500	| CPU_IGS_BRUT  (provisional)
 	.word	3570	| CPU_RP1       (provisional)
        .word   2500    | CPU_BASS      (provisional)
        .word   2500    | CPU_CRISTAL   (provisional)
	.word	2500	| CPU_CANCUN	(provisional)
	.word	-1	| CPU_SIERRA	(not a 68k-based product)
	.word	-1	| CPU_RSP	(not a 68k-based product)
	.word   1000    | CPU_SAPPHIRE  (guess - but no need to fix)
	.word	1786	| CPU_SYNALC    (SYNALC i-cache enabled by PROM Boot)
	.word   -1	| CPU_VIP	(not a 68k-based product)
	.word	-1	| CPU_C7100	(not a 68k-based product)
	.word   -1  | CPU_i86   (not a 68k-based product)
	.word	-1	| CPU_RHINO
	.word	-1	| CPU_BRASIL
	.word	-1	| CPU_VOLCANO	(not used -- QUICC has built in tmr)
        .word   -1      | CPU_C3500     (not a 68k-based product)
        .word   -1      | CPU_NP1
        .word   -1      | CPU_ASP
        .word   -1      | CPU_MALIBU
        .word   -1      | CPU_RINCON
        .word   -1      | CPU_JANEIRO
        .word   -1      | CPU_BFRP   	(not a 68k-based product)
        .word   -1      | CPU_BFLC   	(not a 68k-based product)
	.word	-1	| 
	.word	-1	| 
	.word	-1	| 
	.word	-1	| 
        .word   -1      | CPU_NITRO     (not a 68k-based product)
	.word	-1	| CPU_UNKNOWN

#ifndef MC60000
| set the vector base register (VBR) to the value passed
| on the stack
| return the current contents
	.globl _setvbr
_setvbr:
	movl sp@(4),d1
	movc vbr,d0			|return the current contents
	movc d1,vbr
	rts
#endif MC68000

|
| reset_stack
|
| used by Flash load helper code to reset the stack before dropping into the 
| rom monitor/rxboot image.  Use this routine caringly.  It will reset the
| stack which will screw up your stack variables.
|

        .globl _reset_stack
_reset_stack:
        movl sp@(0),d1
        movl #INITSTACKP,sp             |return here from start and restart
        movl #0,a6                      |clear frame pointer for dumpstack
        movl #0,sp@-                    |save (fake) pc
        movw sr,sp@-                    |save sr
        movw #0,sp@-                    |padding
        movl #EXC_RESET,sp@-            |save longword reason
        pea enter
        movl d1,sp@-                    | restore old return address
        rts


|
| btouchw, btouchr, stouchw, stouchr
|
| Very carefull byte/short read/write touch routines.  The instruction that 
| might bus error is followed by nops so that our bus error routine can
| continue to be naive, and not worry about fixing up the return
| address.
|

	.text
	.globl	touch_start
	.globl	touch_end

touch_start:				| Start of "touch" routines

| btouchw(addr, val)
|    unsigned char *addr
|    int val

	.text
	.globl _btouchw
_btouchw:
	movl	sp@(4),a0	| addr
	movl	sp@(8),d0	| value
	movb	d0,a0@		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| stouchw(addr, val)
|    unsigned char *addr
|    int val

	.globl _stouchw
_stouchw:
	movl	sp@(4),a0	| addr
	movl	sp@(8),d0	| value
	movw	d0,a0@		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| ltouchw(addr, val)
|    unsigned char *addr
|    int val

	.globl	_ltouchw
_ltouchw:
	movl	sp@(4),a0	| addr
	movl	sp@(8),d0	| value
	movl	d0,a0@		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts


| int btouchr(addr)
|    unsigned char *addr

	.globl _btouchr
_btouchr:
	movl	sp@(4),a0	| addr
	movb	a0@,d0		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| int stouchr(addr)
|    unsigned char *addr

	.globl _stouchr
_stouchr:
	movl	sp@(4),a0	| addr
	movw	a0@,d0		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| int ltouchr(addr)
|    unsigned char *addr

	.globl _ltouchr
_ltouchr:
	movl	sp@(4),a0	| addr
	movl	a0@,d0		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

touch_end:				| End of "touch" routines
