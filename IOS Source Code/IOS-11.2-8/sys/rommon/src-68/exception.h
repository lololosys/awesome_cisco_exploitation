/* $Id: exception.h,v 3.2 1995/11/17 18:48:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-68/exception.h,v $
 *------------------------------------------------------------------
 * exception.h
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Structure to represent Moto 68k exception stack frame
 *------------------------------------------------------------------
 * $Log: exception.h,v $
 * Revision 3.2  1995/11/17  18:48:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

/*
** exception.h
**
** Structure to represent the exception stack frames of the 680x0
** processor.
*/

struct excpt_stack_frame {
	unsigned short sr;			/* status register */
	unsigned long pc;			/* program counter */
	unsigned int type : 4;			/* stack contents type */
	unsigned int offset : 12;		/* vector offset */
	union {
	    struct {
		unsigned long instaddr;	        /* instruction address */
	    } type2;
	    struct {
		unsigned long instaddr;	        /* instruction address */
		unsigned long ireg[4];		/* internal registers */
	    } type9;
	    struct {
		unsigned long eaddr;            /* effective address */
		unsigned short ssw;		/* special status word */
		unsigned short wb3s;            /* writeback 3 status */
		unsigned short wb2s;            /* writeback 2 status */
		unsigned short wb1s;            /* writeback 1 status */
		unsigned long faddr;		/* data cycle fault address */
		unsigned long wb3a;             /* writeback 3 address */
		unsigned long wb3d;             /* writeback 3 data */
		unsigned long wb2a;             /* writeback 2 address */
		unsigned long wb2d;             /* writeback 2 data */
		unsigned long wb1a;             /* writeback 1 address */
		unsigned long wb1d;             /* writeback 1 data */
		unsigned long pd1;              /* push data lw1 */
		unsigned long pd2;              /* push data lw2 */
		unsigned long pd3;              /* push data lw3 */
	    } type7;
	    struct {
		unsigned short ireg1;		/* internal register */
		unsigned short ssw;		/* special status word */
		unsigned short ipsc;		/* instruction pipe stage c */
		unsigned short ipsb;		/* instruction pipe stage b */
		unsigned long faddr;		/* data cycle fault address */
		unsigned short ireg2;		/* internal register */
		unsigned short ireg3;		/* internal register */
		unsigned long outbuf;		/* data output buffer */
		unsigned short ireg4;		/* internal register */
		unsigned short ireg5;		/* internal register */
	    } typea;
	    struct {
		unsigned short ireg1;		/* internal register */
		unsigned short ssw;		/* special status word */
		unsigned short ipsc;		/* instruction pipe stage c */
		unsigned short ipsb;		/* instruction pipe stage b */
		unsigned long faddr;		/* data cycle fault address */
		unsigned short ireg2[2];	/* internal register */
		unsigned long outbuf;		/* data output buffer */
		unsigned short ireg3[4];	/* internal register */
		unsigned long sbaddr;		/* stage b address */
		unsigned short ireg4[2];	/* internal register */
		unsigned long inbuf;		/* data input buffer */
		unsigned short ireg5[22];	/* internal register */
	    } typeb;
	    struct {
		unsigned long faddr;     	/* faulted address */
		unsigned long dbuf;
		unsigned long current_pc;	/* current instruction pc */
		unsigned short xfer_count;	/* internal transfer count */
		unsigned short ssw;     	/* special status word */
	    } typec;
	} stack;
};

/*
 * Values in type field
 */
#define	BERR_SHORT	0x0
#define BERR_THROW	0x1
#define	BERR_INSTR	0x2
#define	BERR_COPROC	0x9
#define	BERR_SHORTBUS	0xA
#define	BERR_LONGBUS	0xB

/*
 * Format of special status word
 */

#define	SSW_FC		0x8000		/* Fault on Stage C of the I pipe */
#define	SSW_FB		0x4000		/* Fault on Stage B of the I pipe */
#define	SSW_RC		0x2000		/* Rerun flag for Stage C of I pipe */
#define	SSW_RB		0x1000		/* Rerun flag for Stage B of I pipe */
#define	SSW_DF		0x0100		/* Fault/Rerun flag for Data Cycle */
#define	SSW_RM		0x0080		/* Read-Modify-Write on Data Cycle */
#define	SSW_RW		0x0040		/* R/W for Data Cycle - 1=Read */
#define	SSW_SIZ_MASK	0x0030		/* Mask for Size code */
#define	SSW_ADDR_MASK	0x0007		/* Mask of address space */

/*
 * Address space encodings
 */

#define	ADR_USERDATA	0x1		/* User Data Space */
#define	ADR_USERPROG	0x2		/* User Program Space */
#define	ADR_SUPDATA	0x5		/* Supervisor Data Space */
#define	ADR_SUPPROG	0x6		/* Supervisor Program Space */
#define	ADR_CPU		0x7		/* CPU Space */

/*
** Vector numbers
*/
#define	VN_BERR      	2		/* bus error vector */
#define	VN_ADDR_ERR  	3		/* address error vector */
#define	VN_ILL_INSTR  	4		/* illegal instruction vector */
#define	VN_ZERO_DIV  	5		/* zero divide vector */
#define	VN_CHK_INSTR  	6		/* check instruction vector */
#define	VN_TRAP_INSTR  	7		/* trap_cc vector */
#define	VN_PRIV_VIOL  	8		/* privlege violation vector */
#define	VN_TRACE  	9		/* trace vector */
#define	VN_1010_EMU  	10		/* line 1010 emulator vector */
#define	VN_1111_EMU  	11		/* line 1111 emulator vector */
#define	VN_HDWRE_BRK  	12		/* hardware breakpoint vector */
#define	VN_FORM_ERR  	14		/* format error vector */
#define	VN_UNINIT_INT  	15		/* uninitialized interrupt vector */
#define	VN_SPUR_INT  	24		/* spurious interrupt vector */
#define	VN_AVECT1  	25		/* autovector 1 vector */
#define	VN_AVECT2  	26		/* autovector 2 vector */
#define	VN_AVECT3  	27		/* autovector 3 vector */
#define	VN_AVECT4  	28		/* autovector 4 vector */
#define	VN_AVECT5  	29		/* autovector 5 vector */
#define	VN_AVECT6  	30		/* autovector 6 vector */
#define	VN_AVECT7  	31		/* autovector 7 vector */
#define	VN_TRAP0  	32		/* trap 0 vector */
#define	VN_TRAP1  	33		/* trap 1 vector */
#define	VN_TRAP2  	34		/* trap 2 vector */
#define	VN_TRAP3  	35		/* trap 3 vector */
#define	VN_TRAP4  	36		/* trap 4 vector */
#define	VN_TRAP5  	37		/* trap 5 vector */
#define	VN_TRAP6  	38		/* trap 6 vector */
#define	VN_TRAP7  	39		/* trap 7 vector */
#define	VN_TRAP8  	40		/* trap 8 vector */
#define	VN_TRAP9  	41		/* trap 9 vector */
#define	VN_TRAP10  	42		/* trap 10 vector */
#define	VN_TRAP11  	43		/* trap 11 vector */
#define	VN_TRAP12  	44		/* trap 12 vector */
#define	VN_TRAP13  	45		/* trap 13 vector */
#define	VN_TRAP14  	46		/* trap 14 vector */
#define	VN_TRAP15  	47		/* trap 15 vector */

/*
** Status register defines
*/
#define SR_TRACE_MSK    0xc000          /* trace bits mask */
#define SR_TRACE_SGL    0x8000          /* single step trace */
#define SR_TRACE_BRA    0x4000          /* trace to branch */
#define SR_SUPVSR_STATE 0x2000          /* supervisor state bit */
#define SR_INT_PRI_MSK  0x0700          /* interrupt priority mask */
#define SR_INT_PRI_SH   8               /* interrupt priority shift */

#endif /* __EXCEPTION_H__ */
