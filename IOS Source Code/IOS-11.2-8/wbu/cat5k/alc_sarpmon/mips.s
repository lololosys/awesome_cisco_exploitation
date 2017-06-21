/* $Id: mips.s,v 1.1.4.1 1996/05/09 15:00:26 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/mips.s,v $
 *------------------------------------------------------------------
 * mips.s -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mips.s,v $
 * Revision 1.1.4.1  1996/05/09  15:00:26  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
| Revision 1.1  1996/03/11  07:44:04  rlowe
| Placeholders for California.
|
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* mips.s - startup code for PMON
 *	The contents of this file are not copyrighted in any
 *	way, and may therefore be used without restriction.
 */

#include <mips.h>
#include "pmon.h"

/*
 * If you are new to MIPS assembly language programming, you will find
 * the following texts useful:
 *
 * "MIPS Risc Architecture", by Jerry Kane, published by Prentice Hall,
 *  ISBN 0-13-584749-4.
 *
 * "MIPS Programmer's Handbook", by Erin Farquhar and Philip Bunce,
 * published by Morgan Kaufmann, ISBN 1-55860-297-6.
 *
 */

#ifdef LR33300
#if 0 /* use IS0 */
CACHECFG = (BIU_IS1|BIU_IS0|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#else
CACHECFG = (BIU_IS1|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#endif
#endif
	.text

reset_exception:
#ifdef ATMIZER	/* ATMizer must start with a full 32-bit jump */
	la	k0,_start
	j	k0
#else
	j	_start
#endif
	.align  8

utlb_miss_exception:
	la	k0,_exception
	li	k1,K1BASE
	or	k0,k1
	j	k0
	.align 7

general_exception:
	la	k0,_exception
	li	k1,K1BASE
	or	k0,k1
	j	k0
	.align  9

/*************************************************************
*  util_routines:
*	table of entry address used by clients to access PMON's
*	internal routines. It's the main reason that PMON must
*	be built with "-G 0" (no gp addressing).
*	This table starts at PROM base + 0x200
*/
	.globl util_routines
util_routines:
	.word	read		# 0
	.word	write		# 1
	.word	open		# 2
	.word	close		# 3
	.word	ioctl		# 4
	.word	printf		# 5
	.word	vsprintf	# 6
	.word	ttctl		# 7

	.word	cliexit		# 8
	.word	getenv		# 9
	.word	onintr		# 10
	.word	flush_cache	# 11
	.word	_exception	# 12
#ifdef FPEM
	.word	_fpstatesz	# 13
#ifdef NEWFP
	.word	0		# 14
#else
	.word	_fpinit		# 14
#endif
	.word	_fpstate	# 15
	.word	cop1		# 16
#else
	.word	0
	.word	0
	.word	0
	.word	0
#endif
	.word	adr2symoff	# 17
	.word	sym2adr		# 18
	.word	getclkfreq	# 19

#define STKSIZE 8192
	.comm	stack,STKSIZE	/* PMON's own stack */
	.comm	hndlrtbl,16*4
	.comm   flush_ptr,4

/*************************************************************
*  _start:
*	This is the entry point of the entire PROM Monitor
*/
	.globl _start
	.ent _start
_start:
	# force kSeg1 in case control is passed here from Kseg0
	la	t0,1f
	or	t0,K1BASE
	j	t0
1:

	# set SR and CAUSE to something sensible
	li	v0,SR_BEV
	.set noreorder
	.set noat
	mtc0	v0,C0_SR
	mtc0	zero,C0_CAUSE
	.set at
	.set reorder

#ifdef ATMIZER
BIUCFG = (BIU_NOSTR|BIU_BGNT|BIU_NOPAD|BIU_INTP|BIU_IS1|BIU_DS|BIU_DBLKSZ4)
	li	t0,BIUCFG
	sw      t0,M_BIU
SCRCFG = (SCR_RO52|SCR_TO52|SCR_HH0|SCR_HH1|SCR_HH2|SCR_CBS4|SCR_BS32)
	li      t0,SCRCFG
	sw      t0,M_SCR

	la	s0,atmizer_flush
#else /* !ATMIZER */
	#########################################################
	# first we have to figure out what type of cpu we have  #
	jal	getmachtype
#ifdef LR3000
	beq	v0,3000,lr30x0
	beq	v0,3010,lr30x0
#endif
#ifdef LR33300
	beq	v0,33300,lr333xx
	bne	v0,33310,lr33xxx

	############# 333xx #################
lr333xx:
	# need to enable cp2 and cp3
	.set noreorder
	mfc0	t0,C0_SR
	nop
	or	t0,(SR_CU2|SR_CU3)
	mtc0	t0,C0_SR
	nop		# give it time...
	nop
	nop
	.set reorder

	# SDRAM?
	bc3t	2f	# cpc3 = 1 for DRAM, 0 for SDRAM

	# yes it is configured for SDRAM
SDRAM_MODE = 0x00023000		/* was 0x00022000 */
DUMMY_LOC  = 0x80021000

	li	t0,(BIU_LDSCH|BIU_BGNT|BIU_RDPRI|CACHECFG)
	sw	t0,M_BIU
	li	t0,0x446
	sw	t0,M_SPEC2
	li	t0,0x50
	sw	t0,M_SRAM

	li	t1,M_RTIC	#refresh
	li	t0,375			# 512 rows in 8ms = 15us
	sw	t0,(t1)
	
	/*
	 step1: Set SYNC = 1; in DRAM config reg. and read a location; This
                makes sure that a pre-charge to all banks command is issued 
                to the SDRAM
	 */
	
	li	t0,(DRAM_SYNC|DRAM_DMARDY|(3<<DRAM_PGSZSHFT)|DRAM_RFEN)
	or	t0,((3<<DRAM_RFWESHFT)|DRAM_RDYEN|DRAM_PGMW|DRAM_DCE)
	or	t0,(2<<DRAM_CLSHFT|2<<DRAM_DPTHSHFT|DRAM_CS)
	# make DLP1 bit dependent on state of CPC2
	bc2f	1f
	or	s0,DRAM_DLP1
1: 	sw      s0,M_DRAM
	li	t1,DUMMY_LOC
	lw	t0,0(t1)

	/* step2: wait loop for approximately 100 instructions *

	
	li	t0,0x100
1: 	subu	t0,1
	bne	zero,t0,1b	

	/* step3: set SCFG, write mode reg and reset SCFG */

	or	t0,s0,DRAM_SCFG
	sw      t0,M_DRAM
	sw	t0,SDRAM_MODE	
	sw      s0,M_DRAM

	/* step4: wait for a long time to make sure that two CBR
		  refreshes take place */

	li      t0,0xffff
1: 	subu    t0,1
        bne     zero,t0,1b

	b	3f

2:	# regular DRAM config
	li	t1,M_RTIC	#refresh
	li	t0,375			# 512 rows in 8ms = 15us
	sw	t0,(t1)
#if 0
	li	t0,(BIU_LDSCH|BIU_BGNT|BIU_RDPRI|CACHECFG)
	sw	t0,M_BIU
	# dram = 
	li	t0,(DRAM_DMARDY|(3<<DRAM_PGSZSHFT)|DRAM_RFEN)
	or	t0,((3<<DRAM_RFWESHFT)|DRAM_RDYEN|DRAM_BFD|DRAM_DCE)
	or	t0,(2<<DRAM_CLSHFT)
	sw	t0,M_DRAM
#else
	li	t0,(BIU_LDSCH|BIU_BGNT|BIU_RDPRI|BIU_NOPAD|CACHECFG)
	sw	t0,M_BIU
	# dram = 
	li	t0,(DRAM_DMARDY|(6<<DRAM_PGSZSHFT)|DRAM_RFEN)
	or	t0,((3<<DRAM_RFWESHFT)|DRAM_RDYEN|DRAM_DCE|DRAM_PGMW)
	or	t0,(1<<DRAM_DPTHSHFT)|(1<<DRAM_CLSHFT)
	sw	t0,M_DRAM
#endif
	# spec2 = ??
	li	t0,(SPC_WAITENA|(4<<SPC_WAITSHFT)|SPC_CACHED|SPC_BLKENA)
	sw	t0,M_SPEC2
	# sram = ??
	li	t0,(SPC_WAITENA|(4<<SPC_WAITSHFT)|SPC_CACHED|SPC_BLKENA)
	sw	t0,M_SRAM

3:
	# spec0 = flash eprom sockets containing PMON
	lw	t0,M_SPEC0
	and	t0,(SPC_16WIDE|SPC_8WIDE)	/* don't touch these bits */
	or	t0,(SPC_WAITENA|(6<<SPC_WAITSHFT)|SPC_CACHED)
	sw	t0,M_SPEC0
	# spec1 = DUART
	li	t0,(SPC_WAITENA|(6<<SPC_WAITSHFT)|(8<<SPC_RECSHFT))
	sw	t0,M_SPEC1
	# spec3 = SONIC
	li	t0,(SPC_WAITENA|(16<<SPC_WAITSHFT))
	sw	t0,M_SPEC3

	# delay to allow RAM to stabilize
	li	t0,1000
1:	subu	t0,1
	bne	t0,zero,1b

	# select the correct cache flushing routines
	la	s0,r333_flush

	b	setUpDone
#endif /* LR33300 */

	############# 33xxx #################
lr33xxx:
	# must be a 33k processor (33000 33050 33020 or 33120)
CACHE_CFG = (CR_IBLK_4|CR_DBLK_4)
WAIT_CFG = ((6<<CR_IOWAITSHFT)|(6<<CR_PWAITSHFT))
MEM_CFG = (CR_RDYGEN|CR_RFSHEN|CR_DRAMEN)

	li	t0,375			# 512 rows in 8ms = 15us
	sw	t0,M_RTIC	# refresh

	# turn on the RAM
	li	t0,(CACHE_CFG|WAIT_CFG|MEM_CFG)
	sw	t0,M_CFGREG

	# delay to allow RAM to stabilize
	li	t0,1000
1:	subu	t0,1
	bne	t0,zero,1b

	# select the correct cache flushing routines
	la	s0,r33k_flush

	b	setUpDone

#ifdef LR3000
	############# 30x0 #################
lr30x0:
	# select the correct cache flushing routines
	la	s0,r3k_flush

	#b	setUpDone
#endif
#endif /* !ATMIZER */

	##############################################
	# all processor specific setup has been done #
setUpDone:
        # set up a K1seg stack
	la      sp,stack+STKSIZE-24
	or      sp,K1BASE

	# flush the dcache
	li      a0,DCACHE
	or	s0,K1BASE
	jal     s0

	li	a0,FDATA
	jal	cpdata
	jal	clrbss

#ifndef NON_CACHED
	# copy handler
	la      a0,handler
	la      a1,ehandler
	li      a2,0x80000000           # utlb miss
	jal     copyHandler
#ifdef LR33000
	li      a2,0x80000040           # debug 
	jal     copyHandler
#endif
	li      a2,0x80000080           # general vector
	jal     copyHandler
#endif

	# flush the caches
	li	a0,DCACHE
	jal	s0
	li	a0,ICACHE
	jal	s0

	# ok to use k0seg stack now
	la	sp,stack+STKSIZE-24

	# save the flush routine ptr for later use
	sw	s0,flush_ptr

	/*
	* Provide address for use by shrc function. This feature 
	* permits text to be placed in the PROM and to be read as a
	* sort of 'startup' file when PMON starts, it is normally 
	* disabled because it can cause problems if the PROM has 
	* junk in the end.
	*/
#ifdef ENB_SHRC
#ifdef SABLE
	li	a0,0xa0070000
#else

	# RPA: changed edata to _edata
	#	la	a0,edata
	#
	la	a0,_edata
	la	t1,_fdata
	subu	a0,t1

	#Ratinder P. S. Ahuja changed etext to _etext
	#la	t1,etext
	#

	la	t1,_etext

	addu	a0,t1
#endif
#endif
	la	t0,dbginit	# initialize PMON
	jal	t0
	# ints might be enabled from here on

	#########################################################
	#	Set initial client conditions			#
	#
	la	t6,DBGREG
	li	v0,CLIENTPC
	sw	v0,R_EPC*4(t6)
	jal	clienttos
	sw	v0,R_SP*4(t6)

	.set noreorder
#ifndef NON_CACHED
	# clear BEV
 	mfc0	v0,C0_SR
 	nop
  	li	t0,~SR_BEV
  	and	v0,t0
 	mtc0	v0,C0_SR
#endif

	mfc0	v0,C0_CAUSE
	nop
	sw	v0,R_CAUSE*4(t6)
	mfc0	t1,C0_PRID
	nop
	sw	t1,R_PRID*4(t6)		/* PRId */
	nop				# required because of 33000 bug

#ifdef LR33300
	mfc0	t1,C0_BPCM
	nop
	sw	t1,R_BPCM*4(t6)
	nop				# required because of 33000 bug
	mfc0	t1,C0_BDAM
	nop
	sw	t1,R_BDAM*4(t6)
	nop				# required because of 33000 bug
#endif

#ifdef FPEM
 	cfc1	t1,C1_CSR
 	nop
 	sw	t1,R_FCR*4(t6)
	nop				# required because of 33000 bug
 	cfc1	t1,C1_FRID
 	nop
 	sw	t1,R_FID*4(t6)
#endif
	.set reorder
	#							#
	#########################################################

	la	t0,main		
	jal	t0		# transfer to main part of PMON

	j	_start
	.end _start

/*************************************************************
*  handler:
*	This is the handler that gets copied to the exception vector 
*	addresses.
*/
	.globl handler
	.ent handler
handler:
	.set noat
	la	k0,_exception
	j	k0
ehandler:
	.set at
	.end handler


/*************************************************************
*  _exit:
*	This is an exit routine, it should never be called except when PMON
*	is aborted while running under SABLE.
*/
	.globl _exit
	.ent _exit
_exit:	
#ifndef SABLE 
	break 0
#endif
	j	ra
	.end _exit


/*************************************************************
*  cliexit:
*	This is called when a client terminates normally.
*/
	.globl cliexit
	.ent cliexit
cliexit:
	la	sp,stack+STKSIZE-24
	j	main
	.end cliexit

/*************************************************************
*  _go:
*	This routine is used to transfer control to a client program.
*/
	.globl	_go
	.ent _go
_go:
	li	a0,2
	jal	swlst

#ifdef FLOATINGPT
	lw	t0,cp1ok
	beq	t0,zero,1f
	la	t0,DBGREG
	.set noreorder
	lwc1	$f0,R_F0*4(t0)
	lwc1	$f1,R_F1*4(t0)
	lwc1	$f2,R_F2*4(t0)
	lwc1	$f3,R_F3*4(t0)
	lwc1	$f4,R_F4*4(t0)
	lwc1	$f5,R_F5*4(t0)
	lwc1	$f6,R_F6*4(t0)
	lwc1	$f7,R_F7*4(t0)
	lwc1	$f8,R_F8*4(t0)
	lwc1	$f9,R_F9*4(t0)
	lwc1	$f10,R_F10*4(t0)
	lwc1	$f11,R_F11*4(t0)
	lwc1	$f12,R_F12*4(t0)
	lwc1	$f13,R_F13*4(t0)
	lwc1	$f14,R_F14*4(t0)
	lwc1	$f15,R_F15*4(t0)
	lwc1	$f16,R_F16*4(t0)
	lwc1	$f17,R_F17*4(t0)
	lwc1	$f18,R_F18*4(t0)
	lwc1	$f19,R_F19*4(t0)
	lwc1	$f20,R_F20*4(t0)
	lwc1	$f21,R_F21*4(t0)
	lwc1	$f22,R_F22*4(t0)
	lwc1	$f23,R_F23*4(t0)
	lwc1	$f24,R_F24*4(t0)
	lwc1	$f25,R_F25*4(t0)
	lwc1	$f26,R_F26*4(t0)
	lwc1	$f27,R_F27*4(t0)
	lwc1	$f28,R_F28*4(t0)
	lwc1	$f29,R_F29*4(t0)
	lwc1	$f30,R_F30*4(t0)
	lwc1	$f31,R_F31*4(t0)
	lw	t1,R_FCR*4(t0)
	nop
	ctc1	t1,$31
1:	.set reorder
#endif
	la	k0,DBGREG
	# restored later		/* $1  (at) */
	lw	v0,R_V0*4(k0)		/* $2  (v0) */
	lw	v1,R_V1*4(k0)		/* $3  (v1) */
	lw	a0,R_A0*4(k0)		/* $4  (a0) */
	lw	a1,R_A1*4(k0)		/* $5  (a1) */
	lw	a2,R_A2*4(k0)		/* $6  (a2) */
	lw	a3,R_A3*4(k0)		/* $7  (a3) */
	# restored later		/* $8  (t0) */
	lw	t1,R_T1*4(k0)		/* $9  (t1) */
	lw	t2,R_T2*4(k0)		/* $10 (t2) */
	lw	t3,R_T3*4(k0)		/* $11 (t3) */
	lw	t4,R_T4*4(k0)		/* $12 (t4) */
	lw	t5,R_T5*4(k0)		/* $13 (t5) */
	lw	t6,R_T6*4(k0)		/* $14 (t6) */
	lw	t7,R_T7*4(k0)		/* $15 (t7) */
	lw	s0,R_S0*4(k0)		/* $16 (s0) */
	lw	s1,R_S1*4(k0)		/* $17 (s1) */
	lw	s2,R_S2*4(k0)		/* $18 (s2) */
	lw	s3,R_S3*4(k0)		/* $19 (s3) */
	lw	s4,R_S4*4(k0)		/* $20 (s4) */
	lw	s5,R_S5*4(k0)		/* $21 (s5) */
	lw	s6,R_S6*4(k0)		/* $22 (s6) */
	lw	s7,R_S7*4(k0)		/* $23 (s7) */
	lw	t8,R_T8*4(k0)		/* $24 (t8) */
	lw	t9,R_T9*4(k0)		/* $25 (t9) */
					/* $26 (k0) */
					/* $27 (k1) */
	lw	gp,R_GP*4(k0)		/* $28 (gp) */
	lw	sp,R_SP*4(k0)		/* $29 (sp) */
	lw	s8,R_FP*4(k0)		/* $30 (s8) */
	lw	ra,R_RA*4(k0)		/* $31 (ra) */
	lw	k1,R_HI*4(k0)		/* Hi */
	mthi	k1
	lw	k1,R_LO*4(k0)		/* Lo */
	mtlo	k1
	.set noreorder
#ifdef LR33000
	lw	k1,R_DCIC*4(k0)		/* DCIC */
	nop
	mtc0	k1,C0_DCIC
	lw	k1,R_BPC*4(k0)		/* BPC */
	nop
	mtc0	k1,C0_BPC
	lw	k1,R_BDA*4(k0)		/* BDA */
	nop
	mtc0	k1,C0_BDA
#else
	lw	k1,R_ENTRYHI*4(k0)	/* EntryHi */
	nop
	mtc0	k1,C0_TLBHI
	lw	k1,R_ENTRYLO*4(k0)	/* EntryLo */
	nop
	mtc0	k1,C0_TLBLO
	lw	k1,R_INDEX*4(k0)	/* Index */
	nop
	mtc0	k1,C0_INX
	lw	k1,R_CONTEXT*4(k0)	/* Context */
	nop
	mtc0	k1,C0_CTEXT
#endif
	.set noat
	# restore SR from Status but fixup the KU&IE field
	# to make sure that correct value is restored after the rfe
	mtc0	zero,C0_SR		# clear IEC (and everything else)
	lw	k1,R_STATUS*4(k0)	# get Status
	li	AT,0x3f			# KU&IE mask
	and	t0,k1,AT		# get KU&IE field
	not	AT			# invert mask
	and	k1,AT			# clear KU&IE field
	sll	t0,2			# fixup KU&IE field
	or	k1,t0			# insert KU&IE field
	mtc0	k1,C0_SR		# set SR

	lw	k1,R_CAUSE*4(k0)	/* Cause - For Sw field only */
	nop
	mtc0	k1,C0_CAUSE
	lw	k1,R_K1*4(k0)		/* $27 (k1) */
	lw	t0,R_T0*4(k0)		/* $8  (t0) */
	lw	AT,R_AT*4(k0)		/* $1  (AT) */
	lw	k0,R_EPC*4(k0)		/* EPC */
	nop
	j	k0			# transfer control to client
	rfe
	.set at
	.set reorder
	.end _go

/*************************************************************
*  _exception:
*	This routine is used to save the state of a client program after
*	an exception is encountered. But it first checks to see if there
*	is a user defined exception handler (via onintr), and then checks
*	to see if it's a floating-point instruction (if PMON has fp emulation
*	enabled.)
*/
	.globl	_exception
	.ent _exception
_exception:
	.set noat
	la	k0,DBGREG
	sw	k1,R_K1TMP*4(k0)

	# if (curlst == &pmlst) branch to exc2
	la	k0,curlst
	lw	k0,(k0)
	la	k1,pmlst
	beq	k0,k1,exc2	# skip if in PMON

	# see if we have a user defined handler
	.set noreorder
	mfc0	k0,C0_CAUSE
	nop
	.set reorder
	and	k0,CAUSE_EXCMASK
	la	k1,hndlrtbl
	addu	k0,k1		# calc table entry addr
	lw	k0,(k0)		# get contents of table entry
	beq	k0,zero,exc2
	lw	k0,4(k0)	# user routine addr
	la	k1,DBGREG
	lw	k1,R_K1TMP*4(k1) # restore k1
	j	k0		# jump to user handler

exc2:
#ifdef FPEM
	/* see if it's a cp1 unusable */
	.set noreorder
	mfc0	k0,C0_CAUSE
	nop
	.set reorder
	li	k1,(CAUSE_CEMASK|CAUSE_EXCMASK)
	and	k0,k1
	li	k1,((1<<CAUSE_CESHIFT)|(11<<CAUSE_EXCSHIFT))
	subu	k0,k1
	bne	k0,zero,1f

	la	k0,cop1		# jump to emulation package
	la	k1,DBGREG
	lw	k1,R_K1TMP*4(k1) # restore k1
	j	k0
#endif

1:	# if (curlst == &clilst) branch to 1f
	la	k0,curlst
	lw	k0,(k0)
	la	k1,clilst
	beq	k0,k1,1f

	.set noreorder
	mfc0	a0,C0_EPC
	nop
	mfc0	a1,C0_CAUSE
	nop
	.set reorder
	la	k1,DBGREG
	lw	k1,R_K1TMP*4(k1) # restore k1
	j	pmexception

1:	la	k0,DBGREG
	lw	k1,R_K1TMP*4(k0)	# restore k1
	sw	k1,R_K1*4(k0)		/* $27 (k1) */
	sw	AT,R_AT*4(k0)		/* $1  (AT) */
	.set at
	sw	v0,R_V0*4(k0)		/* $2  (v0) */
	sw	v1,R_V1*4(k0)		/* $3  (v1) */
	sw	a0,R_A0*4(k0)		/* $4  (a0) */
	sw	a1,R_A1*4(k0)		/* $5  (a1) */
	sw	a2,R_A2*4(k0)		/* $6  (a2) */
	sw	a3,R_A3*4(k0)		/* $7  (a3) */
	sw	t0,R_T0*4(k0)		/* $8  (t0) */
	sw	t1,R_T1*4(k0)		/* $9  (t1) */
	sw	t2,R_T2*4(k0)		/* $10 (t2) */
	sw	t3,R_T3*4(k0)		/* $11 (t3) */
	sw	t4,R_T4*4(k0)		/* $12 (t4) */
	sw	t5,R_T5*4(k0)		/* $13 (t5) */
	sw	t6,R_T6*4(k0)		/* $14 (t6) */
	sw	t7,R_T7*4(k0)		/* $15 (t7) */
	sw	s0,R_S0*4(k0)		/* $16 (s0) */
	sw	s1,R_S1*4(k0)		/* $17 (s1) */
	sw	s2,R_S2*4(k0)		/* $18 (s2) */
	sw	s3,R_S3*4(k0)		/* $19 (s3) */
	sw	s4,R_S4*4(k0)		/* $20 (s4) */
	sw	s5,R_S5*4(k0)		/* $21 (s5) */
	sw	s6,R_S6*4(k0)		/* $22 (s6) */
	sw	s7,R_S7*4(k0)		/* $23 (s7) */
	sw	t8,R_T8*4(k0)		/* $24 (t8) */
	sw	t9,R_T9*4(k0)		/* $25 (t9) */
					/* $26 (k0) */
	sw	gp,R_GP*4(k0)		/* $28 (gp) */
	sw	sp,R_SP*4(k0)		/* $29 (sp) */
	sw	s8,R_FP*4(k0)		/* $30 (s8) */
	sw	ra,R_RA*4(k0)		/* $31 (ra) */
	mfhi	k1
	sw	k1,R_HI*4(k0)		/* Hi */
	mflo	k1
	sw	k1,R_LO*4(k0)		/* Lo */
	.set noreorder
#ifdef LR33000
	mfc0	k1,C0_DCIC
	nop
	sw	k1,R_DCIC*4(k0)		/* DCIC */
	li	t0,~DCIC_DE		# make sure the DE bit is cleared
	and	k1,t0
	mtc0	k1,C0_DCIC
	mfc0	k1,C0_BPC
	nop
	sw	k1,R_BPC*4(k0)		/* BPC */
	mfc0	k1,C0_BDA
	nop
	sw	k1,R_BDA*4(k0)		/* BDA */
#else
	mfc0	k1,C0_TLBHI
	nop
	sw	k1,R_ENTRYHI*4(k0)	/* EntryHi */
	mfc0	k1,C0_TLBLO
	nop
	sw	k1,R_ENTRYLO*4(k0)	/* EntryLo */
	mfc0	k1,C0_INX
	nop
	sw	k1,R_INDEX*4(k0)	/* Index */
	mfc0	k1,C0_RAND
	nop
	sw	k1,R_RANDOM*4(k0)	/* Random */
	mfc0	k1,C0_CTEXT
	nop
	sw	k1,R_CONTEXT*4(k0)	/* Context */
#endif
	# save the SR value but fixup the KU & IE bits to 
	# reflect their state before the exception
	mfc0	k1,C0_SR		# get current SR value
	nop
	li	t0,0x3f			# KU&IE mask
	and	t1,k1,t0		# extract KU&IE bits
	not	t0			# invert the mask
	and	k1,t0			# clear KU&IE field
	srl	t1,2			# fixup KU&IE field
	or	k1,t1			# insert the KU&IE field
	sw	k1,R_STATUS*4(k0)	# save Status

	mfc0	k1,C0_CAUSE
	nop
	sw	k1,R_CAUSE*4(k0)	/* Cause */
	mfc0	k1,C0_EPC
	nop
	sw	k1,R_EPC*4(k0)		/* EPC */
	mfc0	k1,C0_BADVADDR
	nop
	sw	k1,R_BADVA*4(k0)	/* BadVA */
	.set reorder
#ifdef FLOATINGPT
	lw	t0,cp1ok
	beq	t0,zero,1f
	.set noreorder
	la	t0,DBGREG
	swc1	$f0,R_F0*4(t0)
	swc1	$f1,R_F1*4(t0)
	swc1	$f2,R_F2*4(t0)
	swc1	$f3,R_F3*4(t0)
	swc1	$f4,R_F4*4(t0)
	swc1	$f5,R_F5*4(t0)
	swc1	$f6,R_F6*4(t0)
	swc1	$f7,R_F7*4(t0)
	swc1	$f8,R_F8*4(t0)
	swc1	$f9,R_F9*4(t0)
	swc1	$f10,R_F10*4(t0)
	swc1	$f11,R_F11*4(t0)
	swc1	$f12,R_F12*4(t0)
	swc1	$f13,R_F13*4(t0)
	swc1	$f14,R_F14*4(t0)
	swc1	$f15,R_F15*4(t0)
	swc1	$f16,R_F16*4(t0)
	swc1	$f17,R_F17*4(t0)
	swc1	$f18,R_F18*4(t0)
	swc1	$f19,R_F19*4(t0)
	swc1	$f20,R_F20*4(t0)
	swc1	$f21,R_F21*4(t0)
	swc1	$f22,R_F22*4(t0)
	swc1	$f23,R_F23*4(t0)
	swc1	$f24,R_F24*4(t0)
	swc1	$f25,R_F25*4(t0)
	swc1	$f26,R_F26*4(t0)
	swc1	$f27,R_F27*4(t0)
	swc1	$f28,R_F28*4(t0)
	swc1	$f29,R_F29*4(t0)
	swc1	$f30,R_F30*4(t0)
	swc1	$f31,R_F31*4(t0)
	cfc1	t1,$31
	nop
	sw	t1,R_FCR*4(t0)
1:	.set reorder
#endif /* FLOATINGPT */
	la	sp,stack+STKSIZE-24
	la	t0,exception
	j	t0
	.end _exception

/*************************************************************
*  onintr()
*	Used to install user defined exception handlers.
*/
	.globl onintr
	.ent onintr
onintr: # a0=exec a1=struct
	sll	a0,2
	la	t0,idummy
	sw	t0,(a1)
	la	t1,hndlrtbl
	addu	t1,a0
	lw	t0,(t1)
	beq	t0,zero,1f
	sw	t0,(a1)
1:	sw	a1,(t1)
	j	ra
	.end onintr

/*************************************************************
*  clrhndlrs()
*	Used to remove user defined exception handlers.
*	also recopies exception vector handler.
*/
	.globl clrhndlrs
	.ent clrhndlrs
clrhndlrs:
	subu	sp,24
	sw	ra,20(sp)

	# remove user defined handlers
	la	t1,hndlrtbl
	li	t2,0
	li	t3,16
1:	sll	t0,t2,2
	addu	t0,t1
	sw	zero,(t0)
	addu	t2,1
	bne	t2,t3,1b

#ifndef NON_CACHED
	# recopy the handler
	la      a0,handler
	la      a1,ehandler
	li      a2,0x80000000           # utlb miss
	jal     copyHandler
#ifdef LR33000
	li      a2,0x80000040           # debug 
	jal     copyHandler
#endif
	li      a2,0x80000080           # general vector
	jal     copyHandler
	li	a0,ICACHE
	jal	flush_cache
#endif

	lw	ra,20(sp)
	addu	sp,24
	j	ra
	.end clrhndlrs

/*************************************************************
*  clienttos()
*	This routine returns the correct top-of-stack value.
*	Used by C routines, when they need to set the client stack pointer.
*/
	.globl clienttos
	.ent clienttos
clienttos:
	la	t0,memorysize
	lw	v0,(t0)
	addu	v0,CLIENTPC
	and	v0,~7 			# double word align
	subu	v0,24			# make room for a0-a3..
	j	ra
	.end clienttos

/*************************************************************
*  clr_iec()
*	This is used make sure that interrupts are disabled.
*/
	.globl clr_iec
	.ent clr_iec
clr_iec:
	.set noreorder
	mfc0	t0,C0_SR
	nop
	and	t0,~SR_IEC
	mtc0	t0,C0_SR
	.set reorder
	j	ra
	.end clr_iec

/*************************************************************
*  cp1init()
*	Used to initialize coprocessor 1 (the FPU)
*/
	.globl cp1init
	.ent cp1init
cp1init:
	.set noreorder
	mfc0	t0,C0_SR
	nop
	or	t0,SR_CU1
	mtc0	t0,C0_SR
	nop				# give it time..
	nop				# ..
	la	t6,DBGREG
 	cfc1	t1,C1_CSR
 	nop
 	sw	t1,R_FCR*4(t6)
	nop				# required because of 33000 bug
 	cfc1	t1,C1_FRID
 	nop
 	sw	t1,R_FID*4(t6)
	.set reorder
	j	ra
	.end cp1init

/*************************************************************
*  copyHandler:
*	Used to copy the exception handler to the various
*	vector addresses.
*/

	.globl copyHandler
	.ent copyHandler
copyHandler:
	# a0=src a1=end a2=dst
	# must not change a0 or a1
	# must force a0 & a1 to kseg1
	or      t0,a0,K1BASE
	or      t1,a1,K1BASE
1:      lw      v0,(t0)
	sw      v0,(a2)
	addu    t0,4
	addu    a2,4
	blt     t0,t1,1b
	j       ra
	.end copyHandler

/*************************************************************
*  flush_cache(type,adr)
*	Flush the designated cache (DCACHE, ICACHE, IADDR)
*	Note that this is not a real subroutine, it simply
*	transfers control to the routine pointed to by 'flush_ptr',
*	being very careful to not use Kseg0 just in case the caches
*	haven't been flushed yet.
*/
	.globl flush_cache
	.ent flush_cache
flush_cache: # a0=type a1=adr
	la	t1,flush_ptr
	or	t1,K1BASE
	lw	t0,(t1)
	or	t0,K1BASE
	bne	t0,zero,1f
	j	ra
     1:	j	t0
	.end flush_cache

#ifdef ATMIZER
/*************************************************************
*  atmizer_flush
*	dummy flush routine, the ATMizer has no flushable cache
*/
	.globl atmizer_flush
	.ent atmizer_flush
atmizer_flush:
	j	ra
	.end atmizer_flush

#endif

#ifdef CYGNUS
	.globl __main
	.ent __main
__main:
	j	ra
	.end __main
#endif

/*
 * This is used by onintr to terminate the list of user defined handlers
 * for a given EXC value.
 */
	.data
idummy:	.word	0
	.word	exc2

