| $Id: c68k.S,v 1.11 1996/09/25 18:25:23 jliu Exp $
| $Source: /cscka/t1_neat/src/t1/c68k.S,v $
|------------------------------------------------------------------
| c68k.S  Neat Assembly routines
|
| October, 1995, Joe Liu
|
| Copyright (c) 1995-1997 by cisco Systems, Inc.
| All rights reserved.
|
|------------------------------------------------------------------
/* $Log: c68k.S,v $
 * Revision 1.11  1996/09/25 18:25:23  jliu
 *   o Neat bus error could occurred due to level 6 interrupt pending
 *     before neat memory got cleared. So main CPU could read undeterminated
 *     data from Neat memory since system code running before firmware
 *     download
 *
! Revision 1.10  1996/07/10  06:44:18  jliu
!     	o Created another timer(250ms) for framing process and one second
!           DSX1 performance data statistics in order to pass ACCUNET test
! 	o Code changed for both AT&T and ANSI fdl test
!         o Removed frame_mode in T1InfoType by using FrameType in frm_info
!
! Revision 1.9  1996/05/12  22:57:02  jliu
!     o Added bus error and illegal intruction handling routine
!
! Revision 1.8  1996/04/08  18:14:50  jliu
!     o Made some configuration changes to get tone data generation
!       works
!     o Implemented transmit underrun error interrupt
!
! Revision 1.7  1996/03/19  03:54:36  jliu
!    o change the Tx clock source to internal by default
!    o always use XBCKI for slip/transmit buffer clock which is derived
!      from TDM bus 2.044 Mhz
!    o when external network loopback is detected, need to change the transmit
!      clock to line and change it back to internal when that loopback
!      request goes away
!    o need to initialize Rx PCM control registers
!    o modify some internal registers setup for tone generation
!
! Revision 1.6  1996/03/01  22:23:08  jliu
!     o removed set sfc & dfc to 0x05 instructions which are for EST debug
!       tool
!
! Revision 1.5  1996/02/22  22:27:42  jliu
!     o update thge current T1 firmware for starting channelized T1 coding
!
! Revision 1.4  1996/02/14  00:23:41  towu
! Changed log comment.
!
! Revision 1.3  1996/02/13  07:23:26  jliu
! cvs diff -c c68k.S
!
| Revision 1.2  1996/02/13  07:03:08  jliu
|   o call init_quicc routine directly instead of indirectly
|
| Revision 1.1.1.1  1996/02/10  03:22:52  towu
| Create
|
|------------------------------------------------------------------
| $Endlog$
|
*/

/*
 *		Note on register handling:
 *
 *	The GNU C compiler which we use expects registers D2 - D7, A3 - A5
 *	to be the same upon exit from a procedure as they were going in.  A6
 *	and A7 are used as the frame pointer and the stack pointer respectively
 *	(so they will be the same upon exit as well, but not because you have
 *	to moveml them).  D0 is used to return the result from a function.  D1
 *	is a scratch register (also used to return 8 byte quantities, such as
 *	doubles).
 */

#include	"t1_chips.h"

/*
 *		First we put in some stuff for startup
 */
. = LOCAL_RAM_ADDR
	.globl init_sp, init_pc, evec
init_sp:
evec:   .long	(LOCAL_STACK_POINTER)	/* initial SP */
init_pc:
	.long	start			/* initial PC */
	.long	badvec		        /* bus error */
	.long	badvec			/* addr error */

	.long	badvec	        	/* illegal instruction */
	.long	badvec			/* zero divide */
	.long	badvec			/* chk instr */
	.long	badvec			/* cpTRAPcc instr */

	.long	badvec			/* priv violation */
	.long	badvec			/* trace */
	.long	badvec			/* line 1010 emul */
	.long	badvec			/* line 1111 emul */

	.long	badvec			/* (reserved) */
	.long	badvec			/* coprocessor violation */
	.long	badvec			/* format err */
	.long	badvec			/* uninit int */

	.long	badvec			/* vector 16 */
	.long	badvec			/* vector 17 */
	.long	badvec			/* vector 18 */
	.long	badvec			/* vector 19 */
	.long	badvec			/* vector 20 */
	.long	badvec			/* vector 21 */
	.long	badvec			/* vector 22 */
	.long	badvec			/* vector 23 */

	.long	badvec			/* spurious interrupt */
	.long	badvec			/* level 1 autovector */
	.long	_mpu_int_handler	/* level 2 autovector */
	.long	_framer_int_handler	/* level 3 autovector */
	.long	badvec	                /* level 4 autovector */
	.long	_backplane_int_handler	/* level 5 autovector */
	.long	_fdl_int_handler	/* level 6 autovector */
	.long	badvec			/* level 7 autovector */

	.long	badvec			/* trap 0 */
	.long	badvec			/* trap 1 */
	.long	badvec			/* trap 2 */
	.long	badvec			/* trap 3 */
	.long	badvec			/* trap 4 */
	.long	badvec			/* trap 5 */
	.long	badvec			/* trap 6 */
	.long	badvec			/* trap 7 */
	.long	badvec			/* trap 8 */
	.long	badvec			/* trap 9 */
	.long	badvec			/* trap 10 */
	.long	badvec			/* trap 11 */
	.long	badvec			/* trap 12 */
	.long	badvec			/* trap 13 */
	.long	badvec			/* trap 14 */
	.long	badvec			/* trap 15 */

	.long	badvec			/* FPCP branch set on unordered cond */
	.long	badvec			/* FPCP inexact */
	.long	badvec			/* FPCP div by zero */
	.long	badvec			/* FPCP underflow */

	.long	badvec			/* FPCP operand err */
	.long	badvec			/* FPCP oveflow */
	.long	badvec			/* FPCP signaling NAN */
	.long	badvec			/* unassigned, reserved */

	.long	badvec			/* not used by EC030 */
	.long	badvec			/* not used by EC030 */
	.long	badvec			/* not used by EC030 */

	.long	badvec			/* vector 59 */
	.long	badvec			/* vector 60 */
	.long	badvec			/* vector 61 */
	.long	badvec			/* vector 62 */
	.long	badvec			/* vector 63 */

	.long	badvec			/* vector 64 */
	.long	badvec			/* vector 65 */
	.long	badvec			/* vector 66 */
	.long	badvec			/* vector 67 */
	.long	badvec			/* vector 68 */
	.long	badvec			/* vector 69 */
	.long	badvec			/* vector 70 */
	.long	badvec			/* vector 71 */
	.long	badvec			/* vector 72 */
	.long	badvec			/* vector 73 */
	.long	badvec			/* vector 74 */
	.long	badvec			/* vector 75 */
	.long	badvec			/* vector 76 */
	.long	badvec			/* vector 77 */
	.long	badvec			/* vector 78 */
	.long	badvec			/* vector 79 */
	.long	badvec			/* vector 80 */
	.long	badvec			/* vector 81 */
	.long	badvec			/* vector 82 */
	.long	badvec			/* vector 83 */
	.long	badvec			/* vector 84 */
	.long	badvec			/* vector 85 */
	.long	badvec			/* vector 86 */
	.long	badvec			/* vector 87 */
	.long	badvec			/* vector 88 */
	.long	badvec			/* vector 89 */
	.long	badvec			/* vector 90 */
	.long	badvec			/* vector 91 */
	.long	badvec			/* vector 92 */
	.long	badvec			/* vector 93 */
	.long	badvec			/* vector 94 */
	.long	badvec			/* vector 95 */
	.long	badvec			/* vector 96 */
	.long	badvec			/* vector 97 */
	.long	badvec			/* vector 98 */
	.long	badvec			/* vector 99 */
	.long	badvec			/* vector 100 */
	.long	badvec			/* vector 101 */
	.long	badvec			/* vector 102 */
	.long	badvec			/* vector 103 */
	.long	badvec			/* vector 104 */
	.long	badvec			/* vector 105 */
	.long	badvec			/* vector 106 */
	.long	badvec			/* vector 107 */
	.long	badvec			/* vector 108 */
	.long	badvec			/* vector 109 */
	.long	badvec			/* vector 110 */
	.long	badvec			/* vector 111 */
	.long	badvec			/* vector 112 */
	.long	badvec			/* vector 113 */
	.long	badvec			/* vector 114 */
	.long	badvec			/* vector 115 */
	.long	badvec			/* vector 116 */
	.long	badvec			/* vector 117 */
	.long	badvec			/* vector 118 */
	.long	badvec			/* vector 119 */
	.long	badvec			/* vector 120 */
	.long	badvec			/* vector 121 */
	.long	badvec			/* vector 122 */
	.long	badvec			/* vector 123 */
	.long	badvec			/* vector 124 */
	.long	badvec			/* vector 125 */
	.long	badvec			/* vector 126 */
	.long	badvec			/* vector 127 */
	.long	badvec			/* vector 128 */
	.long	badvec			/* vector 129 */
	.long	badvec			/* vector 130 */
	.long	badvec			/* vector 131 */
	.long	badvec			/* vector 132 */
	.long	badvec			/* vector 133 */
	.long	badvec			/* vector 134 */
	.long	badvec			/* vector 135 */
	.long	badvec			/* vector 136 */
	.long	badvec			/* vector 137 */
	.long	badvec			/* vector 138 */
	.long	badvec			/* vector 139 */
	.long	badvec			/* vector 140 */
	.long	badvec			/* vector 141 */
	.long	badvec			/* vector 142 */
	.long	badvec			/* vector 143 */
	.long	badvec			/* vector 144 */
	.long	badvec			/* vector 145 */
	.long	badvec			/* vector 146 */
	.long	badvec			/* vector 147 */
	.long	badvec			/* vector 148 */
	.long	badvec			/* vector 149 */
	.long	badvec			/* vector 150 */
	.long	badvec			/* vector 151 */
	.long	badvec			/* vector 152 */
	.long	badvec			/* vector 153 */
	.long	badvec			/* vector 154 */
	.long	badvec			/* vector 155 */
	.long	badvec			/* vector 156 */
	.long	badvec			/* vector 157 */
	.long	badvec			/* vector 158 */
	.long	badvec			/* vector 159 */
	.long	_error_int_handler     	/* vector 160 */
	.long	badvec			/* vector 161 */
	.long	badvec			/* vector 162 */
	.long	badvec			/* vector 163 */
	.long	badvec			/* vector 164 */
	.long	badvec			/* vector 165 */
	.long	badvec			/* vector 166 */
	.long	badvec			/* vector 167 */
	.long	badvec			/* vector 168 */
	.long	badvec			/* vector 169 */
	.long	badvec			/* vector 170 */
	.long	badvec			/* vector 171 */
	.long	badvec			/* vector 172 */
	.long	badvec			/* vector 173 */
	.long	badvec			/* vector 174 */
	.long	badvec			/* vector 175 */
	.long	badvec			/* vector 176 */
	.long	badvec			/* vector 177 */
	.long	_timer2_int_handler	/* vector 178 */
	.long	badvec			/* vector 179 */
	.long	badvec			/* vector 180 */
	.long	badvec			/* vector 181 */
	.long	badvec			/* vector 182 */
	.long	badvec			/* vector 183 */
	.long	badvec			/* vector 184 */
	.long	_timer1_int_handler	/* vector 185 */
	.long	badvec			/* vector 186 */
	.long	badvec			/* vector 187 */
	.long	badvec			/* vector 188 */
	.long	badvec			/* vector 189 */
	.long	_txe_int_handler	/* vector 190 */
	.long	badvec			/* vector 191 */
	.long	badvec			/* vector 192 */
	.long	badvec			/* vector 193 */
	.long	badvec			/* vector 194 */
	.long	badvec			/* vector 195 */
	.long	badvec			/* vector 196 */
	.long	badvec			/* vector 197 */
	.long	badvec			/* vector 198 */
	.long	badvec			/* vector 199 */
	.long	badvec			/* vector 200 */
	.long	badvec			/* vector 201 */
	.long	badvec			/* vector 202 */
	.long	badvec			/* vector 203 */
	.long	badvec			/* vector 204 */
	.long	badvec			/* vector 205 */
	.long	badvec			/* vector 206 */
	.long	badvec			/* vector 207 */
	.long	badvec			/* vector 208 */
	.long	badvec			/* vector 209 */
	.long	badvec			/* vector 210 */
	.long	badvec			/* vector 211 */
	.long	badvec			/* vector 212 */
	.long	badvec			/* vector 213 */
	.long	badvec			/* vector 214 */
	.long	badvec			/* vector 215 */
	.long	badvec			/* vector 216 */
	.long	badvec			/* vector 217 */
	.long	badvec			/* vector 218 */
	.long	badvec			/* vector 219 */
	.long	badvec			/* vector 220 */
	.long	badvec			/* vector 221 */
	.long	badvec			/* vector 222 */
	.long	badvec			/* vector 223 */
	.long	badvec			/* vector 224 */
	.long	badvec			/* vector 225 */
	.long	badvec			/* vector 226 */
	.long	badvec			/* vector 227 */
	.long	badvec			/* vector 228 */
	.long	badvec			/* vector 229 */
	.long	badvec			/* vector 230 */
	.long	badvec			/* vector 231 */
	.long	badvec			/* vector 232 */
	.long	badvec			/* vector 233 */
	.long	badvec			/* vector 234 */
	.long	badvec			/* vector 235 */
	.long	badvec			/* vector 236 */
	.long	badvec			/* vector 237 */
	.long	badvec			/* vector 238 */
	.long	badvec			/* vector 239 */
	.long	badvec			/* vector 240 */
	.long	badvec			/* vector 241 */
	.long	badvec			/* vector 242 */
	.long	badvec			/* vector 243 */
	.long	badvec			/* vector 244 */
	.long	badvec			/* vector 245 */
	.long	badvec			/* vector 246 */
	.long	badvec			/* vector 247 */
	.long	badvec			/* vector 248 */
	.long	badvec			/* vector 249 */
	.long	badvec			/* vector 250 */
	.long	badvec			/* vector 251 */
	.long	badvec			/* vector 252 */
	.long	badvec			/* vector 253 */
	.long	badvec			/* vector 254 */
	.long	badvec			/* vector 255 */

. = LOCAL_RAM_ADDR+0x400		/* end of vector space */


/*
 *	System startup code.
 */
start:
	movew	#0x2700,sr	/* supervisor mode, no interrupts */
	movel	init_sp,sp	/* in case we got here from a download */
     	movql   #0x7,d0         /* select cpu space */
   	movc	d0,sfc          /* load SFC to indicate cpu space */
        movc    d0,dfc 		/* load DFC to indicate cpu space */
	movl	#MBAR_VALUE,d0  /* load d0 with addr and valid bit set */
        movsl   d0,MBAR_ADDR    /* write the value into MBAR */
     	movql   #0x5,d0         /* select cpu space */
   	movc	d0,sfc          /* load SFC to indicate cpu space */
        movc    d0,dfc 		/* load DFC to indicate cpu space */
        lea     evec,a0
        movc    a0,vbr          /* initialize VBR to 0x0 */
    
        movl    #0x3ff,d0
        clrl    d1
        movl    #DPRAM_ADDR,a0  /* a0 is the dual port ram base */
clrlp:
        movl    d1,a0@+
        dbf     d0,clrlp        /* decrement d0 & loop back to clrlp until */
                                /* d0 < 0 */
	jsr	_init_quicc	/* initialize everything */

	.globl start2		/* sinit jumps here as a return */
start2:
	/* set up the stack */
	movel	#(LOCAL_STACK_POINTER),sp
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	movel	#hara_kiri,sp@-
	jmp	_Main
	/* NOTREACHED */


/*
 *              We save things off into low RAM, so as to leave the
 *      stack alone as much as possible.  We call the C routine
 *      with the stack set up as sr, sp(a7), a6-a0, d7-d0.
 */
hara_kiri:
        movml   d0-d7/a0-a7,sp@- /* save entire CPU context */
        jsr     _hara_kiri_mesg
        movml   sp@+,d0-d7/a0-a7 /* pop our registers */
        rte

/*
 * Some sort of unexpected vector
 * For the predecrement mode address, a7-a0 then d7-d0 got transfer
 * For both control mode and postincrement mode address, d0-d7 then a0-a7 got
 * transfer
 */
.global _badvec
_badvec:
badvec:
         movml   d0-d7/a0-a7,sp@- /* save entire CPU context */
         jsr     _exception
         movml   sp@+,d0-d7/a0-a7 /* pop our registers */
         jmp     start


/*************************************************************
 *
 * The interrupt handlers
 */
.even
.globl _fdl_int_handler
_fdl_int_handler:
        movml   d0-d7/a0-a7,sp@-
	jsr	_fdl_int
        movml   sp@+,d0-d7/a0-a7
        rte

.even
.globl _backplane_int_handler
_backplane_int_handler:
        movml   d0-d7/a0-a7,sp@-
	jsr	_backplane_int
        movml   sp@+,d0-d7/a0-a7
        rte

/* we handle the 250ms timeout interrupt here */
.even
.globl _timer1_int_handler
_timer1_int_handler:
        movml   a0/a1/d0/d1,sp@-
        jsr     _timer1_int
        movml   sp@+,a0/a1/d0/d1
        rte

/* we handle the 1ms timeout interrupt here */
.even
.globl _timer2_int_handler
_timer2_int_handler:
        movml   a0/a1/d0/d1,sp@-
        jsr     _timer2_int
        movml   sp@+,a0/a1/d0/d1
        rte

.even
.globl _txe_int_handler
_txe_int_handler:
        movml   a0/a1/d0/d1,sp@-
        jsr     _txe_int
        movml   sp@+,a0/a1/d0/d1
        rte

.even
.globl _framer_int_handler
_framer_int_handler:
        movml   d0-d7/a0-a7,sp@-
	jsr	_framer_int
        movml   sp@+,d0-d7/a0-a7
	rte

.even
.globl _mpu_int_handler
_mpu_int_handler:
        movml   d0-d7/a0-a7,sp@-
	jsr	_mpu_int
        movml   sp@+,d0-d7/a0-a7
	rte


/* If a bit in the CIMR is masked at the same time that the corresponding */
/* CIPR bit causes an interrupt request to the IMB, then the interrupt is */
/* not processed, but the error vector is issued if the interrupt ack */
/* cycle occurs with no other CPM interrupts pending */
.globl _error_int_handler
.even
_error_int_handler:
        rte

