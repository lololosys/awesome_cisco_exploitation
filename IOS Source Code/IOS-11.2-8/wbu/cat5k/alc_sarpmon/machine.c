/* $Id: machine.c,v 1.1.4.1 1996/05/09 15:00:24 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/machine.c,v $
 *------------------------------------------------------------------
 * machine.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: machine.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:24  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:31  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:06  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:01  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <mips.h>
#include <ctype.h>
#include "pmon.h"

/*
 * The disassembler in this module is used by pmon/dis.c, tools/rdsrec.c,
 * and tools/rdfrec.c.
 */

#define comma()		strcat(dest,",")
#define rd()		strcat(dest,regname[(int)RD_(inst)])
#define rs()		strcat(dest,regname[(int)RS_(inst)])
#define rt()		strcat(dest,regname[(int)RT_(inst)])
#define fd()		strcat(dest,c1reg[(int)FD_(inst)])
#define fs()		strcat(dest,c1reg[(int)FS_(inst)])
#define ft()		strcat(dest,c1reg[(int)FT_(inst)])
#define c0()		strcat(dest,c0reg[(int)RD_(inst)])
#define c1()		strcat(dest,c1reg[(int)RD_(inst)])
#define c2()		strcat(dest,regs_c2d[(int)RD_(inst)])
#define cn()		strcat(dest,regs_hw[(int)RD_(inst)])
#define c0ft()		strcat(dest,c0reg[(int)RT_(inst)])
#define c1ft()		strcat(dest,c1reg[(int)RT_(inst)])
#define cnft()		strcat(dest,regs_hw[(int)RT_(inst)])
#define cc1()		strcat(dest,regs_hw[(int)RD_(inst)])
#define cc2()		strcat(dest,regs_c2c[(int)RD_(inst)])

/* software register names */
char *regs_sw[] = {
	"zero",  "at", "v0",  "v1",  "a0",  "a1",  "a2",  "a3",
	"t0",  "t1",  "t2",  "t3",  "t4",  "t5",  "t6",  "t7",
	"s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
	"t8",  "t9",  "k0",  "k1",  "gp",  "sp",  "s8",  "ra"
	};

/* hardware register names */
char *regs_hw[] = {
	"$0",  "$1",  "$2",  "$3",  "$4",  "$5",  "$6",  "$7",
	"$8",  "$9",  "$10", "$11", "$12", "$13", "$14", "$15",
	"$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
	"$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
	};

char *regs_c0[] = {
	"C0_INX",     "C0_RAND",  "C0_TLBLO",  "C0_BPC",  
	"C0_CTEXT",   "C0_BDA",   "$6",        "C0_DCIC",
#ifdef LR33300
 	"C0_BADADDR", "C0_BDAM",  "C0_TLBHI",  "C0_BPCM", 
#else
 	"C0_BADADDR", "$9",       "C0_TLBHI",  "$11", 
#endif
	"C0_SR",      "C0_CAUSE", "C0_EPC",    "C0_PRID",
	"$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
	"$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
	};

char *c1reg[] = {
	"$f0",  "$f1",  "$f2",  "$f3",  "$f4",  "$f5",  "$f6",  "$f7",
	"$f8",  "$f9",  "$f10", "$f11", "$f12", "$f13", "$f14", "$f15",
	"$f16", "$f17", "$f18", "$f19", "$f20", "$f21", "$f22", "$f23",
	"$f24", "$f25", "$f26", "$f27", "$f28", "$f29", "$f30", "$f31"
	};

#ifdef LR33000
char *regs_c2d[] = {
	"C2_SCRSTART", "C2_SCRPITCH", "C2_HWCRSRSTART", "C2_HWCRSRCURR", 
	"C2_SAMEXTENT", "C2_NXTDISP", "C2_CURDISP", "C2_LINECOUNT",
	"C2_VBLKSIZE", "C2_SRCLINE", "C2_SRCCURR", "C2_SRCPITCH",
	"C2_DESTLINE", "C2_DESTCURR", "C2_DESTPITCH", "C2_GBLKSIZE",
	"C2_SERPULS", "C2_HLINTR", "C2_BLANKS", "C2_SYNCS",
	"C2_SYNCRESET", "C2_BLANKE", "C2_HWCRSR", "C2_VHWCONFIG",
	"C2_PSTXB", "C2_PSRCVB", "$26", "$27", "C2_SRCDATA",
	"C2_DESTDATA", "C2_LEFTMASK", "C2_RIGHTMASK" 
	};

char *regs_c2c[] = {
	"C2_SRCSKEW", "C2_SRCSHIFT", "C2_COLOR0", "C2_COLOR1",
	"C2_GCPCNTRL", "C2_RASTEROP", "C2_PLANEMASK", "$7", "$8",
	"C2_CONFIG", "$10", "$11", "$12", "$13", "$14", "$15", "$16",
	"$17", "$18", "$19", "$20", "$21", "$22", "$23", "C2_PSSTAT",
	"C2_PSCOMM", "$26", "$27", "$28", "$29", "$30", "$31" 
	}; 
#endif

DISTBL distbl[] = {
	{ "add",	0xfc0007ffL,	0x00000020L,	RD_RS_RT	},
	{ "addi",	0xfc000000L,	0x20000000L,	RT_RS_SIMM	},
	{ "addiu",	0xfc000000L,	0x24000000L,	RT_RS_SIMM	},
	{ "addu",	0xfc0007ffL,	0x00000021L,	RD_RS_RT	},
	{ "and",	0xfc0007ffL,	0x00000024L,	RD_RS_RT	},
	{ "andi",	0xfc000000L,	0x30000000L,	RT_RS_IMM	},
	{ "bc0f",	0xff610000L,	0x41000000L,	OFF		},
	{ "bc1f",	0xff610000L,	0x45000000L,	OFF		},
	{ "bc2f",	0xff610000L,	0x49000000L,	OFF		},
	{ "bc3f",	0xff610000L,	0x4d000000L,	OFF		},
	{ "bc0t",	0xff610000L,	0x41010000L,	OFF		},
	{ "bc1t",	0xff610000L,	0x45010000L,	OFF		},
	{ "bc2t",	0xff610000L,	0x49010000L,	OFF		},
	{ "bc3t",	0xff610000L,	0x4d010000L,	OFF		},
	{ "beq",	0xfc000000L,	0x10000000L,	RS_RT_OFF	},
	{ "bgez",	0xfc1f0000L,	0x04010000L,	RS_OFF		},
	{ "bgezal",	0xfc1f0000L,	0x04110000L,	RS_OFF		},
	{ "bgtz",	0xfc1f0000L,	0x1c000000L,	RS_OFF		},
	{ "blez",	0xfc1f0000L,	0x18000000L,	RS_OFF		},
	{ "bltz",	0xfc1f0000L,	0x04000000L,	RS_OFF		},
	{ "bltzal",	0xfc1f0000L,	0x04100000L,	RS_OFF		},
	{ "bne",	0xfc000000L,	0x14000000L,	RS_RT_OFF	},
	{ "break",	0xfc00003fL,	0x0000000dL,	BPCODE		},
	{ "cfc0",	0xffe007ffL,	0x40400000L,	RT_RD		},
	{ "cfc1",	0xffe007ffL,	0x44400000L,	RT_CC1		},
#ifdef LR33000
	{ "cfc2",	0xffe007ffL,	0x48400000L,	RT_CC2		},
#else
	{ "cfc2",	0xffe007ffL,	0x48400000L,	RT_CN		},
#endif
	{ "cfc3",	0xffe007ffL,	0x4c400000L,	RT_CN		},
	{ "tlbp",	0xffffffffL,	0x42000008L,	NONE		},
	{ "tlbr",	0xffffffffL,	0x42000001L,	NONE		},
	{ "tlbwi",	0xffffffffL,	0x42000002L,	NONE		},
	{ "tlbwr",	0xffffffffL,	0x42000006L,	NONE		},
	{ "rfe",	0xffffffffL,	0x42000010L,	NONE		},
	{ "cop0",	0xfe000000L,	0x42000000L,	COFUN		},
	{ "add.s",	0xfee0003fL,	0x46000000L,	FT_FS_FD	},
	{ "add.d",	0xfee0003fL,	0x46200000L,	FT_FS_FD	},
	{ "sub.s",	0xfee0003fL,	0x46000001L,	FT_FS_FD	},
	{ "sub.d",	0xfee0003fL,	0x46200001L,	FT_FS_FD	},
	{ "mul.s",	0xfee0003fL,	0x46000002L,	FT_FS_FD	},
	{ "mul.d",	0xfee0003fL,	0x46200002L,	FT_FS_FD	},
	{ "div.s",	0xfee0003fL,	0x46000003L,	FT_FS_FD	},
	{ "div.d",	0xfee0003fL,	0x46200003L,	FT_FS_FD	},
	{ "abs.s",	0xfee0003fL,	0x46000005L,	FS_FD		},
	{ "abs.d",	0xfee0003fL,	0x46200005L,	FS_FD		},
	{ "mov.s",	0xfee0003fL,	0x46000006L,	FS_FD		},
	{ "mov.d",	0xfee0003fL,	0x46200006L,	FS_FD		},
	{ "neg.s",	0xfee0003fL,	0x46000007L,	FS_FD		},
	{ "neg.d",	0xfee0003fL,	0x46200007L,	FS_FD		},
	{ "cvt.s.w",	0xfee0003fL,	0x46800020L,	FS_FD		},
	{ "cvt.s.d",	0xfee0003fL,	0x46200020L,	FS_FD		},
	{ "cvt.d.s",	0xfee0003fL,	0x46000021L,	FS_FD		},
	{ "cvt.d.w",	0xfee0003fL,	0x46800021L,	FS_FD		},
	{ "cvt.w.d",	0xfee0003fL,	0x46200024L,	FS_FD		},
	{ "cvt.w.s",	0xfee0003fL,	0x46000024L,	FS_FD		},
	{ "c.f.s",	0xfee0003fL,	0x46000030L,	FT_FS		},
	{ "c.f.d",	0xfee0003fL,	0x46200030L,	FT_FS		},
	{ "c.un.s",	0xfee0003fL,	0x46000031L,	FT_FS		},
	{ "c.un.d",	0xfee0003fL,	0x46200031L,	FT_FS		},
	{ "c.eq.s",	0xfee0003fL,	0x46000032L,	FT_FS		},
	{ "c.eq.d",	0xfee0003fL,	0x46200032L,	FT_FS		},
	{ "c.ueq.s",	0xfee0003fL,	0x46000033L,	FT_FS		},
	{ "c.ueq.d",	0xfee0003fL,	0x46200033L,	FT_FS		},
	{ "c.olt.s",	0xfee0003fL,	0x46000034L,	FT_FS		},
	{ "c.olt.d",	0xfee0003fL,	0x46200034L,	FT_FS		},
	{ "c.ult.s",	0xfee0003fL,	0x46000035L,	FT_FS		},
	{ "c.ult.d",	0xfee0003fL,	0x46200035L,	FT_FS		},
	{ "c.ole.s",	0xfee0003fL,	0x46000036L,	FT_FS		},
	{ "c.ole.d",	0xfee0003fL,	0x46200036L,	FT_FS		},
	{ "c.ule.s",	0xfee0003fL,	0x46000037L,	FT_FS		},
	{ "c.ule.d",	0xfee0003fL,	0x46200037L,	FT_FS		},
	{ "c.sf.s",	0xfee0003fL,	0x46000038L,	FT_FS		},
	{ "c.sf.d",	0xfee0003fL,	0x46200038L,	FT_FS		},
	{ "c.ngle.s",	0xfee0003fL,	0x46000039L,	FT_FS		},
	{ "c.ngle.d",	0xfee0003fL,	0x46200039L,	FT_FS		},
	{ "c.seq.s",	0xfee0003fL,	0x4600003aL,	FT_FS		},
	{ "c.seq.d",	0xfee0003fL,	0x4620003aL,	FT_FS		},
	{ "c.ngl.s",	0xfee0003fL,	0x4600003bL,	FT_FS		},
	{ "c.ngl.d",	0xfee0003fL,	0x4620003bL,	FT_FS		},
	{ "c.lt.s",	0xfee0003fL,	0x4600003cL,	FT_FS		},
	{ "c.lt.d",	0xfee0003fL,	0x4620003cL,	FT_FS		},
	{ "c.nge.s",	0xfee0003fL,	0x4600003dL,	FT_FS		},
	{ "c.nge.d",	0xfee0003fL,	0x4620003dL,	FT_FS		},
	{ "c.le.s",	0xfee0003fL,	0x4600003eL,	FT_FS		},
	{ "c.le.d",	0xfee0003fL,	0x4620003eL,	FT_FS		},
	{ "c.ngt.s",	0xfee0003fL,	0x4600003fL,	FT_FS		},
	{ "c.ngt.d",	0xfee0003fL,	0x4620003fL,	FT_FS		},
	{ "cop1",	0xfe000000L,	0x46000000L,	COFUN		},
#ifdef LR33020
	{ "sstep",	0xfeffffffL,	0x4a00ffffL,	NONE		},
	{ "sbstep",	0xfeffffffL,	0x4a40ffffL,	NONE		},
	{ "wstep",	0xfeffffffL,	0x4a80ffffL,	NONE		},
	{ "wstep_l",	0xfeffffffL,	0x4a88ffffL,	NONE		},
	{ "wstep_r",	0xfeffffffL,	0x4a84ffffL,	NONE		},
	{ "wstep_l_r",	0xfeffffffL,	0x4a8cffffL,	NONE		},
	{ "wstep_s",	0xfeffffffL,	0x4aa0ffffL,	NONE		},
	{ "wstep_s_l",	0xfeffffffL,	0x4aa8ffffL,	NONE		},
	{ "wstep_s_r",	0xfeffffffL,	0x4aa4ffffL,	NONE		},
	{ "wstep_s_l_r",0xfeffffffL,	0x4aacffffL,	NONE		},
	{ "wstep_sb",	0xfeffffffL,	0x4ab0ffffL,	NONE		},
	{ "wstep_sb_l",	0xfeffffffL,	0x4ab8ffffL,	NONE		},
	{ "wstep_sb_r",	0xfeffffffL,	0x4ab4ffffL,	NONE		},
	{ "wstep_sb_l_r",0xfeffffffL,	0x4abcffffL,	NONE		},
	{ "wstep_four",	0xfeffffffL,	0x4a82ffffL,	NONE		},
	{ "wstep_bfour",0xfeffffffL,	0x4a81ffffL,	NONE		},
	{ "bstep_bfour",0xfeffffffL,	0x4ac1ffffL,	NONE		},
	{ "bstep",	0xfeffffffL,	0x4ac0ffffL,	NONE		},
	{ "bstep_l",	0xfeffffffL,	0x4ac8ffffL,	NONE		},
	{ "bstep_r",	0xfeffffffL,	0x4ac4ffffL,	NONE		},
	{ "bstep_l_r",	0xfeffffffL,	0x4accffffL,	NONE		},
	{ "bstep_s",	0xfeffffffL,	0x4ae0ffffL,	NONE		},
	{ "bstep_s_l",	0xfeffffffL,	0x4ae8ffffL,	NONE		},
	{ "bstep_s_r",	0xfeffffffL,	0x4ae4ffffL,	NONE		},
	{ "bstep_s_l_r",0xfeffffffL,	0x4aecffffL,	NONE		},
	{ "bstep_sb",	0xfeffffffL,	0x4af0ffffL,	NONE		},
	{ "bstep_sb_l",	0xfeffffffL,	0x4af8ffffL,	NONE		},
	{ "bstep_sb_r",	0xfeffffffL,	0x4af4ffffL,	NONE		},
	{ "bstep_sb_l_r",0xfeffffffL,	0x4afcffffL,	NONE		},
	{ "wstep_i",	0xfeffffffL,	0x4a807fffL,	NONE		},
	{ "wstep_l_i",	0xfeffffffL,	0x4a887fffL,	NONE		},
	{ "wstep_r_i",	0xfeffffffL,	0x4a847fffL,	NONE		},
	{ "wstep_l_r_i",0xfeffffffL,	0x4a8c7fffL,	NONE		},
	{ "wstep_s_i",	0xfeffffffL,	0x4aa07fffL,	NONE		},
	{ "wstep_s_l_i",0xfeffffffL,	0x4aa87fffL,	NONE		},
	{ "wstep_s_r_i",0xfeffffffL,	0x4aa47fffL,	NONE		},
	{ "wstep_s_l_r_i",0xfeffffffL,	0x4aac7fffL,	NONE		},
	{ "wstep_sb_i",	0xfeffffffL,	0x4ab07fffL,	NONE		},
	{ "wstep_sb_l_i",0xfeffffffL,	0x4ab87fffL,	NONE		},
	{ "wstep_sb_r_i",0xfeffffffL,	0x4ab47fffL,	NONE		},
	{ "wstep_sb_l_r_i",0xfeffffffL,	0x4abc7fffL,	NONE		},
	{ "wstep_four_i",0xfeffffffL,	0x4a827fffL,	NONE		},
	{ "wstep_bfour_i",0xfeffffffL,	0x4a817fffL,	NONE		},
	{ "bstep_bfour_i",0xfeffffffL,	0x4ac17fffL,	NONE		},
	{ "bstep_i",	0xfeffffffL,	0x4ac07fffL,	NONE		},
	{ "bstep_l_i",	0xfeffffffL,	0x4ac87fffL,	NONE		},
	{ "bstep_r_i",	0xfeffffffL,	0x4ac47fffL,	NONE		},
	{ "bstep_l_r_i",0xfeffffffL,	0x4acc7fffL,	NONE		},
	{ "bstep_s_i",	0xfeffffffL,	0x4ae07fffL,	NONE		},
	{ "bstep_s_l_i",0xfeffffffL,	0x4ae87fffL,	NONE		},
	{ "bstep_s_r_i",0xfeffffffL,	0x4ae47fffL,	NONE		},
	{ "bstep_s_l_r_i",0xfeffffffL,	0x4aec7fffL,	NONE		},
#endif /* LR33020 */
	{ "cop2",	0xfe000000L,	0x4a000000L,	COFUN		},
	{ "cop3",	0xfe000000L,	0x4e000000L,	COFUN		},
	{ "ctc0",	0xffe007ffL,	0x40c00000L,	RT_RD		},
	{ "ctc1",	0xffe007ffL,	0x44c00000L,	RT_CC1		},
#ifdef LR33000
	{ "ctc2",	0xffe007ffL,	0x48c00000L,	RT_CC2		},
#else
	{ "ctc2",	0xffe007ffL,	0x48c00000L,	RT_CN		},
#endif
	{ "ctc3",	0xffe007ffL,	0x4cc00000L,	RT_CN		},
	{ "div",	0xfc00ffffL,	0x0000001aL,	RS_RT		},
	{ "divu",	0xfc00ffffL,	0x0000001bL,	RS_RT		},
	{ "j",		0xfc000000L,	0x08000000L,	TARGET		},
	{ "jal",	0xfc000000L,	0x0c000000L,	TARGET		},
	{ "jalr",	0xfc1f07ffL,	0x00000009L,	JALR		},
	{ "jr",		0xfc1fffffL,	0x00000008L,	JR		},
	{ "lb",		0xfc000000L,	0x80000000L,	LOAD_STORE	},
	{ "lbu",	0xfc000000L,	0x90000000L,	LOAD_STORE	},
	{ "lh",		0xfc000000L,	0x84000000L,	LOAD_STORE	},
	{ "lhu",	0xfc000000L,	0x94000000L,	LOAD_STORE	},
	{ "lui",	0xfc000000L,	0x3c000000L,	RT_IMM		},
	{ "lw",		0xfc000000L,	0x8c000000L,	LOAD_STORE	},
	{ "lwc0",	0xfc000000L,	0xc0000000L,	LDSTC0		},
	{ "lwc1",	0xfc000000L,	0xc4000000L,	LDSTC1		},
	{ "lwc2",	0xfc000000L,	0xc8000000L,	LDSTCN		},
	{ "lwc3",	0xfc000000L,	0xcc000000L,	LDSTCN		},
	{ "lwl",	0xfc000000L,	0x88000000L,	LOAD_STORE	},
	{ "lwr",	0xfc000000L,	0x98000000L,	LOAD_STORE	},
	{ "mfc0",	0xffe007ffL,	0x40000000L,	RT_C0		},
	{ "mfc1",	0xffe007ffL,	0x44000000L,	RT_C1		},
#ifdef LR33000
	{ "mfc2",	0xffe007ffL,	0x48000000L,	RT_C2		},
#else
	{ "mfc2",	0xffe007ffL,	0x48000000L,	RT_CN		},
#endif
	{ "mfc3",	0xffe007ffL,	0x4c000000L,	RT_CN		},
	{ "mfhi",	0xffff07ffL,	0x00000010L,	RD		},
	{ "mflo",	0xffff07ffL,	0x00000012L,	RD		},
	{ "mtc0",	0xffe007ffL,	0x40800000L,	RT_C0		},
	{ "mtc1",	0xffe007ffL,	0x44800000L,	RT_C1		},
#ifdef LR33000
	{ "mtc2",	0xffe007ffL,	0x48800000L,	RT_C2		},
#else
	{ "mtc2",	0xffe007ffL,	0x48800000L,	RT_CN		},
#endif
	{ "mtc3",	0xffe007ffL,	0x4c800000L,	RT_CN		},
	{ "mthi",	0xfc1fffffL,	0x00000011L,	RS		},
	{ "mtlo",	0xfc1fffffL,	0x00000013L,	RS		},
	{ "mult",	0xfc00ffffL,	0x00000018L,	RS_RT		},
	{ "multu",	0xfc00ffffL,	0x00000019L,	RS_RT		},
	{ "nor",	0xfc0007ffL,	0x00000027L,	RD_RS_RT	},
	{ "or",		0xfc0007ffL,	0x00000025L,	RD_RS_RT	},
	{ "ori",	0xfc000000L,	0x34000000L,	RT_RS_IMM	},
	{ "sb",		0xfc000000L,	0xa0000000L,	LOAD_STORE	},
	{ "sh",		0xfc000000L,	0xa4000000L,	LOAD_STORE	},
	{ "sll",	0xffe0003fL,	0x00000000L,	RD_RT_SFT	},
	{ "sllv",	0xfc0007ffL,	0x00000004L,	RD_RT_RS	},
	{ "slt",	0xfc0007ffL,	0x0000002aL,	RD_RS_RT	},
	{ "slti",	0xfc000000L,	0x28000000L,	RT_RS_SIMM	},
	{ "sltiu",	0xfc000000L,	0x2c000000L,	RT_RS_SIMM	},
	{ "sltu",	0xfc0007ffL,	0x0000002bL,	RD_RS_RT	},
	{ "sra",	0xffe0003fL,	0x00000003L,	RD_RT_SFT	},
	{ "srav",	0xfc0007ffL,	0x00000007L,	RD_RT_RS	},
	{ "srl",	0xffe0003fL,	0x00000002L,	RD_RT_SFT	},
	{ "srlv",	0xfc0007ffL,	0x00000006L,	RD_RT_RS	},
	{ "sub",	0xfc0007ffL,	0x00000022L,	RD_RS_RT	},
	{ "subu",	0xfc0007ffL,	0x00000023L,	RD_RS_RT	},
	{ "sw",		0xfc000000L,	0xac000000L,	LOAD_STORE	},
	{ "swc0",	0xfc000000L,	0xe0000000L,	LDSTC0		},
	{ "swc1",	0xfc000000L,	0xe4000000L,	LDSTC1		},
	{ "swc2",	0xfc000000L,	0xe8000000L,	LDSTCN		},
	{ "swc3",	0xfc000000L,	0xec000000L,	LDSTCN		},
	{ "swl",	0xfc000000L,	0xa8000000L,	LOAD_STORE	},
	{ "swr",	0xfc000000L,	0xb8000000L,	LOAD_STORE	},
	{ "syscall",	0xffffffffL,	0x0000000cL,	NONE		},
	{ "xor",	0xfc0007ffL,	0x00000026L,	RD_RS_RT	},
	{ "xori",	0xfc000000L,	0x38000000L,	RT_RS_IMM	},

	/* must be last !! never be move/remove */
	{ ".word",	0x00000000L,	0x00000000L,	WORD		}
};

long inst;
char **regname,**c0reg;

/*************************************************************
*  long disasm(dest,addr,bits)
*	disassemble instruction 'bits'
*/
long disasm(dest,addr,bits)
char *dest;
long addr;
long bits;
{
DISTBL *pt;
char *pc,tmp[40],*p;
long v,v1,w;
int i;

inst = bits;
if (regname == 0) regname = regs_sw;
if (c0reg == 0) c0reg = regs_c0;

#ifdef PMCC
if (!matchenv("regstyle")) {
	regname = regs_hw;
	c0reg = regs_hw;
	}
#endif

if(!adr2symoff(dest,addr,12)) sprintf(dest,"%08lx",addr);
sprintf(tmp," %08lx ",inst);
strcat(dest,tmp);

if(inst == 0L){
	strcat(dest,"nop");
	return(addr + 4L);
	}

pt = get_distbl(inst);
i = strlen(pt->str);
strcat(dest,pt->str);
while(i++ < 8) strcat(dest," ");
switch(pt->type) {
	case FT_FS_FD:	fd(), comma(), fs(), comma(), ft();	break;
	case FS_FD:	fd(), comma(), fs();			break;
	case FT_FS:	fs(), comma(), ft();			break;
	case RT_RS_IMM:	rt(), comma(), rs(), comma(),imm(dest);	break;
	case RT_RS_SIMM:rt(), comma(), rs(),comma(),simm(dest);	break;
	case RT_IMM:	rt(), comma(),imm(dest);		break;
	case RT_SIMM:	rt(), comma(), simm(dest);		break;
	case RT_RD:	rt(), comma();
	case RD:	rd();					break;
	case RT_C0:	rt(),comma(),c0();			break;
	case RT_C1:	rt(),comma(),c1();			break;
#ifdef LR33000
	case RT_C2:	rt(),comma(),c2();			break;
	case RT_CC2:	rt(),comma(),cc2();			break;
#endif
	case RT_CN:	rt(),comma(),cn();			break;
	case RT_CC1:	rt(),comma(),cc1();			break;
	case RD_RT_RS:	rd(), comma(), rt(), comma();
	case JR: case RS:
			rs();					break;
	case RD_RS_RT:	rd(), comma();
	case RS_RT:	rs(), comma(), rt();			break;
	case RD_RT_SFT:	rd(), comma(), rt(), comma();
			sprintf(tmp,"0x%x",SHAMT_(inst));
			strcat(dest,tmp);
			mkcomment(dest,"# %d",SHAMT_(inst));
			break;
	case RS_RT_OFF:
	case RS_OFF:
			rs(), comma();
			if(pt->type == RS_RT_OFF)
				rt(), comma();
	case OFF:
			v = IMM_(inst);
			if(v & (1L << 15)) v |= 0xffff0000L;
			v1 = addr + 4L + (v << 2);
			if (!adr2symoff(tmp,v1,0)) sprintf(tmp,"%x",v1);
			strcat(dest,tmp);
			mkcomment(dest,"# 0x%08lx",v1);
			break;
	case BPCODE:	sprintf(tmp,"%d",(inst>>16)&0x3ff);
			strcat(dest,tmp);
			break;
	case COFUN:	sprintf(tmp,"0x%x",inst & 0x01ffffffL);
			strcat(dest,tmp);
			break;
	case NONE:						break;
	case TARGET:
			v = (inst & 0x03ffffffL) << 2;
			v |= (addr & 0xf0000000L);
			if (!adr2symoff(tmp,v,0)) sprintf(tmp,"%x",v);
			strcat(dest,tmp);
			mkcomment(dest,"# 0x%08lx",v);
			break;
	case JALR:
			if(RD_(inst) != 31L)
				rd(), comma();
			rs();
			break;
	case LDSTC0:
			v = IMM_(inst);
			if(v & (1L << 15)) v |= 0xffff0000L;
			c0ft(), comma();
			sprintf(tmp,"%d",v);
			strcat(dest,tmp);
			strcat(dest,"(");
			rs();
			strcat(dest,")");
			mkcomment(dest,"# 0x%x",v);
			break;
	case LDSTC1:
			v = IMM_(inst);
			if(v & (1L << 15)) v |= 0xffff0000L;
			c1ft(), comma();
			sprintf(tmp,"%d",v);
			strcat(dest,tmp);
			strcat(dest,"(");
			rs();
			strcat(dest,")");
			mkcomment(dest,"# 0x%x",v);
			break;
	case LDSTCN:
			v = IMM_(inst);
			if(v & (1L << 15)) v |= 0xffff0000L;
			cnft(), comma();
			sprintf(tmp,"%d",v);
			strcat(dest,tmp);
			strcat(dest,"(");
			rs();
			strcat(dest,")");
			mkcomment(dest,"# 0x%x",v);
			break;
	case LOAD_STORE:
			v = IMM_(inst);
			if(v & (1L << 15)) v |= 0xffff0000L;
			rt(), comma();
			sprintf(tmp,"%d",v);
			strcat(dest,tmp);
			strcat(dest,"(");
			rs();
			strcat(dest,")");
			mkcomment(dest,"# 0x%x",v);
			break;
	case WORD:
			sprintf(tmp,"%08lx",inst);
			strcat(dest,tmp);
			strcat(dest,"      # ");
#ifdef PMCC
			w = addr;
			for(i = 0; i < 4 ; i++){
				v = load_byte(w++);
#else
			p = (char *)&inst;
			for(i = 0; i < 4 ; i++){
				v = *p++;
#endif
				if (isprint(v)) strccat(dest,v);
				else strccat(dest,'.');
				}
			break;
	}
return(addr + 4L);
}

/*************************************************************
*  DISTBL *get_distbl(bits)
*/
DISTBL *get_distbl(bits)
long bits;
{
DISTBL *pt = distbl;

while((bits & pt->mask) != pt->code) ++pt;
return(pt);
}

#ifdef PMCC
/*************************************************************
*  int is_branch(adr)
*/
int is_branch(adr)
word adr;
{
DISTBL	*pt;
uword inst;

inst = load_word(adr);
pt = get_distbl(inst);
switch(pt->type){
	case OFF: 
	case RS_RT_OFF: 
	case RS_OFF:
	case TARGET: 
	case JALR: 
	case JR:
		return(1);
	default:
		return(0);
	}
}

/*************************************************************
*  int is_conditional_branch(adr)
*/
int is_conditional_branch(adr)
word adr;
{
DISTBL	*pt;
uword inst;

inst = load_word(adr);
pt = get_distbl(inst);
switch(pt->type) {
	case OFF: 
	case RS_RT_OFF: 
	case RS_OFF:
		return(1);
	default:
		return(0);
	}
}

/*************************************************************
*  is_jr(adr)
*/
is_jr(adr)
unsigned long adr;
{
DISTBL	*pt;
uword inst;

inst = load_word(adr);
pt = get_distbl(inst);
if (pt->type == JR) return(1);
return(0);
}

/*************************************************************
*  is_jal(adr)
*/
is_jal(adr)
word adr;
{
uword inst;
int op;

inst = load_word(adr);
op = getfield(inst,6,26);
if (op == 3) return(1);
if (op == 0 && getfield(inst,6,0) == 9) return(1);
return(0);
}

/*************************************************************
*  word branch_target_address(adr)
*/
word branch_target_address(adr)
word adr;
{
DISTBL	*pt;
word	val;
uword inst;

inst = load_word(adr);
pt = get_distbl(inst);
switch(pt->type) {
	case OFF: 
	case RS_RT_OFF: 
	case RS_OFF:
		val = inst & 0xffff;
		if(val & 0x8000)
			val |= 0xffff0000;
		return(adr + 4 + (val << 2));
	case TARGET:
		val = inst & 0x3ffffff;
		return(((adr + 4) & 0xf0000000) | (val << 2));
	case JALR: case JR:
		val = RS_(inst); 
		return(Gpr[val]);
	default:
		return(0);
	}
}

/*************************************************************
*  is_writeable(adr)
*/
is_writeable(adr)
unsigned long adr;
{
unsigned long x;

x = load_word(adr);
store_word(adr,~x);
flush_cache(DCACHE);
if (load_word(adr) != ~x) return(0);
store_word(adr,x);
return(1);
}
#endif

/*************************************************************
*  simm(dest)
*	signed immediate value
*/
simm(dest)
char *dest;
{
char tmp[20];
long v;

v = IMM_(inst);
sprintf(tmp,"0x%x",v);
strcat(dest,tmp);
if (v >= 0 && v <= 9) return;
if(v & (1L << 15)) v |= 0xffff0000L;
mkcomment(dest,"# %d",v);
}

/*************************************************************
*  imm(dest)
*	unsigned immediate value
*/
imm(dest)
char *dest;
{
char tmp[20];
long v;

v = IMM_(inst);
sprintf(tmp,"0x%x",v);
strcat(dest,tmp);
if (v >= 0 && v <= 9) return;
mkcomment(dest,"# %d",v);
}

/*************************************************************
*  mkcomment(p,fmt,v)
*	generate an appropriate comment
*/
mkcomment(p,fmt,v)
char *p,*fmt;
long v;
{
char tmp[20];
int n;

if (v >= 0 && v <= 9) return;
for (n=60-strlen(p);n>0;n--) strcat(p," ");
sprintf(tmp,fmt,v);
strcat(p,tmp);
}

/*************************************************************
*  isprint(c) 
*	returns true if c is a printing character 
*/
isprint(c)
int c;
{
if (c >= ' ' && c <= '~') return(1);
return(0);
}

