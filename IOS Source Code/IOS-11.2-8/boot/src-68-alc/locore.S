| $Id: locore.S,v 3.1.58.1 1996/05/09 13:54:26 rbadri Exp $
| $Source: /release/112/cvs/Xboot/src-68-alc/locore.S,v $
|------------------------------------------------------------------
| locore.s -- Synergy ATM line card IOS ROM monitor assembler assist
|	      routines, tables, and data (based on c3000)
|
| April 13, 1995, Rick Lowe
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: locore.S,v $
| Revision 3.1.58.1  1996/05/09  13:54:26  rbadri
| Branch: California_branch
| Synergy features to the mainline.
|
| Revision 3.1.88.1  1996/04/27  06:13:24  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 3.1.78.1  1996/04/08  01:34:55  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 3.1.64.1  1996/03/22  09:07:20  rlowe
| Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 3.1.50.1  1996/03/03  21:19:19  rlowe
| Apply SYNALC content to merge branch.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 3.1.48.1  1996/03/02  01:07:18  rlowe
| Non-sync sync to mainline tag V111_1_0_3.
| Branch: Synalc3_Cal_V111_1_0_3_branch
|
| Revision 3.1.32.1  1996/02/29  07:42:20  rlowe
| First pass non-sync sync for Synergy ATM line card to V111_0_16.
| Branch: Synalc3_Cal_V111_0_16_branch
|
| Revision 3.1  1995/11/09  09:08:52  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1.62.1  1996/02/03  06:11:57  rlowe
| Non-sync sync of Synalc_catchup_branch to V111_0_5.
| Branch: Synalc_catchup_pretal_branch
|
| Revision 2.1.44.1  1995/09/11  01:39:52  rlowe
| Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
| yielding Synalc2_LE2_V110_1_0_3_branch.
|
| Branch: Synalc2_LE2_V110_1_0_3_branch
|
| Revision 2.1.28.2  1995/09/02  00:01:59  rlowe
| Fix includes so that makefile can give search paths, rather than
| hardcoding them.
|
| Branch: Synalc2_branch
|
| Revision 2.1.28.1  1995/08/22  18:18:47  lcheung
| move files from synergy_atmlc_branch to synalc2_branch
| Branch: Synalc2_branch
|
| Revision 2.1  1995/06/07  19:15:15  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.1.2.1  1995/04/19  02:28:51  rlowe
| Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
|
| Branch: Synergy_ATMlc_branch
|
| Revision 1.1  1995/04/14 05:22:06  rlowe
| Placeholders for IOS port to Synergy ATM line card.
|
|------------------------------------------------------------------
| Revision 1.1  1995/02/01  10:59:56  rlowe
| Ram:s initial merge of ALC IOS-Boot code (from older 10.2 tree).
|
| Revision 1.1  1995/01/11  23:04:37  rlowe
| Initial port of IOS Boot to ACP.  Got Monitor up.
|
| Revision 1.3  1994/12/20  01:05:50  rlowe
| Fix comment character typos.
|
| Revision 1.2  1994/12/20  00:51:47  rlowe
| Fix comment character typos.
|
| Revision 1.1.1.1  1994/08/31  22:16:54  rahuja
| IPS ver 10.2 snapshot
|
|------------------------------------------------------------------
| $Endlog$
|

#define	ASMINCLUDE

#include "../defs.h"
#include "../reason.h"
#include "acp_hw_defs.h"
#include "mfp_acp_defs.h"

	.globl _dcacheon
_dcacheon:
	movl #CLR_DCACHE,d0		|clear Dcache (kill any crud in cache)
	movc d0,cacr
	movb _cache,d0
	beq dcacheon1			|Leave both caches off if icache off
	movl #ENABLE_CACHES,d0
dcacheon1:
	movc d0,cacr
	rts

	.globl _dcacheoff
_dcacheoff:
	movb _cache,d0
	beq dcacheoff1			|Icache is off, too.
	movl #ENA_ICACHE,d0
	movc d0,cacr			|disable Dcache, enable Icache
	movl #CLR_DC_EIC,d0		|clear Dcache, enable Icache
	movc d0,cacr
	rts
dcacheoff1:
	movc d0,cacr			|disable both caches
	movl #CLEAR_CACHES,d0
	movc d0,cacr			|clear both caches
	rts

#ifndef	SYNALC
|
| Flicker lights
|
	.globl lights
lights:
lights1:
	movl #ADRSPC_CONTROL,a0		|control register address
	movl #CONTROL_RUNLAMP,d0	|lamp bit
	notw d0				|make mask
	andw d0,a0@			|disable lamp
	movq #(0xFF),d1			|busy wait
lights2:
	dbra d1,lights2
	movl #CONTROL_RUNLAMP,d0	|lamp bit
	orw d0,a0@			|now light it
	movl #0xFFFFFF,d1		|busy wait
lights3:
	dbra d1,lights3
	bra lights1
#else
|
| Flicker lights
|
	.globl lights
lights:
	movl	#(CLEAR_CACHES),d0
	movc	d0,cacr			|disable cpu caches

	lea	(ACP_HW_IOREG_BASE),a0	|hw control registers
	movl	#((ACP_PMD_STAT_LED_RED<<16)+ACP_PMD_LEVL_LED_YEL),d0
					|lamp control bits
	movl	#((ACP_PMD_LEVL_LED_YEL<<16)+ACP_PMD_LEVL_LED_YEL),d1
					|more lamp control bits
lights2:
	movb	#(1),a0@(ACP_WRRG_WATCHDOG)
	movb	#(0),a0@(ACP_WRRG_WATCHDOG)
					|punch watchdog
	movml	d0-d1,a0@(ACP_WRRG_PMD_STAT_LED)
					|write led control bits (1 per byte)
	moval	#(0x00020000),a1	|setup busy wait
lights3:
	subql	#(1),a1
	tstl	a1
	jgt	lights3			|busy loop until count exhausted

	eoril	#((ACP_PMD_STAT_LED_YEL<<16)+ACP_PMD_LEVL_LED_YEL),d0
	eoril	#((ACP_PMD_LEVL_LED_YEL<<16)+ACP_PMD_LEVL_LED_YEL),d1
	jra	lights2			|display alternate led pattern
#endif

|
| Bus error handler routines
|
	.text
	.even
	.globl buserror
	.globl _buserror 
	.globl _error
	.globl _buserrhandler
	.globl _catchbe
_catchbe:
	andw #0xCEFF,sp@(10)		|clear rerun flags
	rte

_buserror:
buserror:
	movl sp,_error			|save pointer to stack frame
	tstl _buserrhandler		|does someone want this
	beq buserror1S			|not if zero
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movl _buserrhandler,a0		|put address of handler in a0
	jsr a0@				|call handler
	moveml sp@+,d0-d1/a0-a1		|restore registers
	andw #0xCEFF,sp@(10)		|clear rerun flags
	rte				|and return
buserror1S:
	andw #0xCEFF,sp@(10)		|clear rerun flags
	movw #0,sp@-			|padding
	movl #EXC_BUSERR,sp@-		|long reason
	jmp enter			|enter monitor
#ifndef	SYNALC
|
| Refresh (Level 7) Exception
|
	.globl	refreshe
	.globl _refresh
	.globl _abort
	.globl _abortstate
	.globl _clock
	.globl _hclock

refreshe:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movb ADRSPC_TCONTROL,d0		|read timer control (ack)
	addql #REFRESHTIME,_clock	|increment the millisecond clock
	bcc  refreshe0S			|branch if carry clear
	addql #1,_hclock		|we carried, so increment high clock
refreshe0S:
	jsr _refresh			|call main routine
	moveml sp@+,d0-d1/a0-a1		|restore registers
	tstb _abort			|abort seen?
	bne refreshe1S			|only if set
	rte				|otherwise return
refreshe1S:
	cmpb #ABORT_INTERNAL,_abortstate |someone else wants it
	bne refreshe2S			|no, perform normally
	rte				|then return
refreshe2S:
	clrb _abort			|zero abort
	cmpb #ABORT_IGNORE,_abortstate |should we just ignore it
	bne refreshe3S			|no, hand off to monitor
	rte				|then return
refreshe3S:
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor
#else
|
| Refresh Exception -- OS Timer tick interrupt (REFRESHTIME (4) ms).
|
	.globl	refreshe

refreshe:
	moveml	d0-d1/a0-a1,sp@-	|save registers not saved by C code
	subql	#(1),_clk_cor_cnt	|has current correction period expired ?
	jgt	refreshe_x1		|  -- br. if not
	subql	#(1),_clk_cor_adj	|apply correction one tick at a time
	jgt	refreshe0S		|  -- br. if adjustments not finished
					|finished, so reload adjustment counters
	movl	#(MFP_4MS_TMR_CORREC_CNT),_clk_cor_cnt
	movl	#(MFP_4MS_TMR_CORREC_ADJ),_clk_cor_adj
	jra	refreshe0S		|finish OS tick service

refreshe_x1:
	addql #REFRESHTIME,_clock	|increment the millisecond clock
	bcc  refreshe0S			|branch if carry clear
	addql #1,_hclock		|we carried, so increment high clock
refreshe0S:
	jsr _refresh			|call main routine
	moveml sp@+,d0-d1/a0-a1		|restore registers
	tstb _abort			|abort seen?
	bne refreshe1S			|only if set
	rte				|otherwise return

refreshe1S:
	cmpb #ABORT_INTERNAL,_abortstate |someone else wants it
	bne refreshe2S			|no, perform normally
	rte				|then return
refreshe2S:
	clrb _abort			|zero abort
	cmpb #ABORT_IGNORE,_abortstate |should we just ignore it
	bne refreshe3S			|no, hand off to monitor
	rte				|then return
refreshe3S:
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor
#endif

#ifndef	SYNALC
|
| Refresh (Level 7) Exception for the BRUT project
|
	.globl _refreshe_brut
	.globl _refresh_brut

_refreshe_brut:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movb ADRSPC_TCONTROL,d0		|read timer control (ack)
	addql #REFRESHTIME,_clock	|increment the millisecond clock
	bcc  refreshe0S_brut		|branch if carry clear
	addql #1,_hclock		|we carried, so increment high clock
refreshe0S_brut:
	jsr _refresh_brut		|call C isr for this interrupt
	moveml sp@+,d0-d1/a0-a1		|restore registers
	tstb _abort			|abort seen?
	bne refreshe1S_brut		|only if set
	rte				|otherwise return
refreshe1S_brut:
	cmpb #ABORT_INTERNAL,_abortstate |someone else wants it
	bne refreshe2S_brut		|no, perform normally
	rte				|then return
refreshe2S_brut:
	clrb _abort			|zero abort
	cmpb #ABORT_IGNORE,_abortstate  |should we just ignore it
	bne refreshe3S_brut		|no, hand off to monitor
	rte				|then return
refreshe3S_brut:
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor
#endif

#ifndef	SYNALC
|
| Jump to diagnostic helper for init.c
|
| This routine exists so that init.c can jump to Brut diagnostic code
| without using DRAM (stack) or worrying about register allocation
| by the gcc compiler.
|
        .globl  brut_diag_jump

brut_diag_jump:
        movl    BRUT_DIAG_VECTOR_ADDR,a0  |indirect once     
        movl    a0@,a1                    |indirect twice
        jmp     a1@                       |jump to Brut diagnostics
#endif

|
| Set the Access Control Unit (ACU) registers
|
| setacu(reg,val)
|	reg - ACU register to be set (AC0 or AC1)
|	val - value to be written to ACU register
|
| Note that any non-zero value for reg will cause AC1 to be
| the target

	.globl _setacu
_setacu:
	|pmove instruction has limited effective address capabilities
	lea  sp@(8),a0		|set up a0 to point to val on stack
	lea  sp@(4),a1		|set up a1 to point to reg on stack
	tstl a1@		|which register?
	bne  setac1
	.long 0xf0110a00	|(pmove ac0,(a1)) save the current contents
	.long 0xf0100800	|(pmove (a0),ac0) write new value
	bra  retacu
setac1:
	.long 0xf0110e00	|(pmove ac1,(a1)) save the current contents
	.long 0xf0100c00	|(pmove (a0),ac1) write new value
retacu:
	movl  a1@,d0		|return the original (current) contents
	rts

