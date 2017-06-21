/* $Id: pmon.h,v 1.1.4.1 1996/05/09 15:00:28 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/pmon.h,v $
 *------------------------------------------------------------------
 * pmon.h -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pmon.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:36  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:10  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:06  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* pmon.h - Part of the debugger for MIPS CPUs
 *
 * PMON Written for LSI LOGIC Corp. by Phil Bunce.
 *		Phil Bunce, 100 Dolores St. Ste 242, Carmel CA 93923
 *			pjb@carmel.com
 */

#ifndef _MIPS_
#include "mips.h"
#endif

#define TIKRATE 256000

#define MAX_BPT	 32	/* max number of user breakpoints */
#define MAXREC	550	/* max download record size */
#define LINESZ	200	/* max command line length */
#define MAX_AC	100	/* max number of args to commands */
#define PATSZ    80	/* max pattern length for search & fill */

#define NO_BPT		-1L
#define BPT_CODE	0x0000000dL		/* BREAK instruction */
#define SIGNATURE	0x4572696e

/* trace_mode states */
#define TRACE_NO	0	/* no trace/go */
#define TRACE_TB	1	/* trace & break */
#define TRACE_TG	2	/* trace & go */
#define TRACE_GB	3	/* go & break */
#define TRACE_DC	4	/* debug mode continue */
#define TRACE_DS	5	/* debug mode step */
#define TRACE_TN	6	/* multistep trace */

#define CNTRL(x) (x & 0x1f)

#ifdef LANGUAGE_C
#define loop for (;;)

typedef short		half;		/* 2 bytes */
typedef long		word;		/* 4 bytes */
typedef unsigned char	ubyte;
typedef unsigned char	uchar;
typedef unsigned short	uhalf;
typedef unsigned short	ushort;
typedef unsigned long	uword;
typedef unsigned long	ulong;

typedef int Func();

#define MAXSYM 12
typedef struct Sym {
	struct Sym *anext;
	struct Sym *nnext;
	char *lname;
	unsigned int value;
	char name[MAXSYM];
	} Sym;

typedef struct Demo {
	char *name;
	Func *addr;
	} Demo;

typedef struct Optdesc {
	char *name;
	char *desc;
	short flags; 	
	} Optdesc;

typedef struct Cmd {
	char *name;
	char *opts;
	Optdesc *optdesc;
	char *desc;
	Func *func;
	int minac;
	int maxac;
	} Cmd;

typedef struct Bps {
	unsigned long addr;
	unsigned long value;
	char *cmdstr;
	} Bps;

typedef struct Stopentry {
	unsigned long addr;
	unsigned long value;
	char name[10];
	char sense;
	} Stopentry;

typedef enum {
	RD_RS_RT, RT_RS_IMM, OFF, RS_RT_OFF, RS_OFF, NONE, RT_RD, COFUN,
	RS_RT, TARGET, JALR, RS, RD_RT_SFT, LOAD_STORE, RT_IMM, RD, RD_RT_RS,
	RT_RS_SIMM, RT_SIMM, JR, RT_C0, RT_C1, RT_CN, RT_CC1, LDSTC0, LDSTC1,
	LDSTCN, WORD, FT_FS_FD, FS_FD, FT_FS, RT_C2, RT_CC2, BPCODE
} TYPE;

typedef struct {
	char *str;
	long mask, code;
	TYPE type;
	} DISTBL;

typedef struct RegSpec { /* register field info */
	ubyte size;	/* size (width) of field */
	ubyte lsb;	/* position of field's lsb */
	char *name;	/* field name */
	ubyte base;	/* display base e.g. 2 8 10 16 */
	char **values; 	/* use this if base=0 */
	ubyte ro;	/* read only */
	} RegSpec;

typedef struct RegList { /* register info */
	word *reg;	/* address of register value */
	RegSpec *spec;	/* field info */
	char *name;	/* primary name */
	char *aname;	/* alternate name */
	char regnum;	/* register number */
	short flags; 	
	} RegList;

/* defs for RegList flags field */
#define F_ANAME 0x0001	/* print aname */
#define F_GPR	0x0002	/* gp reg */
#define F_00	0x0004	/* 33000 only */
#define F_020	0x0008	/* 33020 only */
#define F_50	0x0010	/* 33050 only */
#define F_CF	0x0020	/* call function for value */
#define F_WO	0x0040	/* write only */
#define F_RO	0x0080	/* read only */
#define F_333	0x0100	/* 33300 & 33310 only */
#define F_120	0x0200	/* 33120 only */
#define F_20	(F_020|F_120)	/* common to 33020 & 33120 only */
#define F_ATM	0x0400  /* ATMizer */
#define F_ALL	(F_00|F_20|F_50|F_333)

#define FS_(x)		(((x) >> 11) & ((1L <<  5) - 1))
#define FT_(x)		(((x) >> 16) & ((1L <<  5) - 1))
#define FD_(x)		(((x) >>  6) & ((1L <<  5) - 1))
#define RS_(x)		(((x) >> 21) & ((1L <<  5) - 1))
#define RT_(x)		(((x) >> 16) & ((1L <<  5) - 1))
#define RD_(x)		(((x) >> 11) & ((1L <<  5) - 1))
#define IMM_(x)		(((x) >>  0) & ((1L << 16) - 1))
#define TARGET_(x)	(((x) >>  0) & ((1L << 26) - 1))
#define SHAMT_(x)	(((x) >>  6) & ((1L <<  5) - 1))

/* external data declarations */
extern DISTBL *get_distbl();
extern DISTBL distbl[];
extern word DBGREG[];	/* debugger's register value holder */
extern int histno;	/* current history number */
extern char *searching;
extern Cmd CmdTable[];
extern jmp_buf jmpb;
extern char line[LINESZ+1];
extern int trace_mode;
extern Bps Bpt[];
extern Bps BptTmp;
extern Bps BptTrc;
extern Bps BptTrcb;
extern char date[];
extern char vers[];
extern char tools[];
extern int machtype;
extern unsigned long initial_sr;
extern char prnbuf[];
extern int memorysize;
extern int clkfreq;
extern int vflag;

#ifdef FLOATINGPT
#ifndef NEWFP
extern struct c1state *pmc1dat;	/* state info for FPU in PMON */
extern struct c1state *clic1dat;	/* state info for FPU in client */
#endif
#endif

/* function declarations */
ubyte *scan_byte();
char *malloc();
Sym *findvalue();
char *getenv();
char *findbang();
char *gethistn();
char *getexcname();
unsigned long getpchist();
word dispmem();
long disasm();
int cXc2(),mXc2();
ulong read_target();

#define cmpstr(x,y) ((strcmp(x,y) == 0)?1:0)
#define getfield(w,s,p) ((((unsigned long)w)&(((1<<s)-1)<<p))>>p)
#define load_byte(x)	read_target(x,1)
#define load_half(x)	read_target(x,2)
#define load_word(x)	read_target(x,4)
#define store_byte(x,v)	write_target(x,v,1),flush_cache(IADDR,x)
#define store_half(x,v)	write_target(x,v,2),flush_cache(IADDR,x)
#define store_word(x,v)	write_target(x,v,4),flush_cache(IADDR,x)

/* macros to increment and decrement x, modulus mod */
#define incmod(x,mod) (((x+1) > mod)?0:x+1)
#define decmod(x,mod) (((x-1) < 0)?mod:x-1)

#endif /* LANGUAGE_C */

#define Gpr		(&DBGREG[R_ZERO])
#define Hi		DBGREG[R_HI]
#define Lo		DBGREG[R_LO]
#define Epc		DBGREG[R_EPC]
#define Status		DBGREG[R_STATUS]
#define Cause		DBGREG[R_CAUSE]
#define Badva		DBGREG[R_BADVA]
#define Prid		DBGREG[R_PRID]

#define Fpr		(&DBGREG[R_F0])
#define Fcr		DBGREG[R_FCR]
#define Fid		DBGREG[R_FID]

#ifdef LR33000
#define Dcic		DBGREG[R_DCIC]
#define Bpc		DBGREG[R_BPC]
#define Bda		DBGREG[R_BDA]
#else
#define Context		DBGREG[R_CONTEXT]
#define Entryhi		DBGREG[R_ENTRYHI]
#define Entrylo		DBGREG[R_ENTRYLO]
#define Index		DBGREG[R_INDEX]
#define Random		DBGREG[R_RANDOM]
#endif

#ifdef LR33300
#define Bpcm		DBGREG[R_BPCM]
#define Bdam		DBGREG[R_BDAM]
#endif

/* Target registers index */
#define	R_ZERO		0
#define	R_AT		1
#define	R_V0		2
#define	R_V1		3
#define	R_A0		4
#define	R_A1		5
#define	R_A2		6
#define	R_A3		7
#define	R_T0		8
#define	R_T1		9
#define	R_T2		10
#define	R_T3		11
#define	R_T4		12
#define	R_T5		13
#define	R_T6		14
#define	R_T7		15
#define	R_S0		16
#define	R_S1		17
#define	R_S2		18
#define	R_S3		19
#define	R_S4		20
#define	R_S5		21
#define	R_S6		22
#define	R_S7		23
#define	R_T8		24
#define	R_T9		25
#define	R_K0		26
#define	R_K1		27
#define	R_GP		28
#define	R_SP		29
#define	R_FP		30
#define	R_RA		31
#define R_HI		32
#define R_LO		33

#ifdef LR33000
#define R_DCIC		34
#define R_BPC		35
#define R_BDA		36
#ifdef LR33300
#define R_BPCM		37
#define R_BDAM		41
#endif
#else
#define R_ENTRYHI	34
#define R_ENTRYLO	35
#define R_INDEX		36
#define R_RANDOM	37
#define R_CONTEXT	41
#endif

#define R_STATUS	38
#define R_CAUSE		39
#define R_EPC		40
#define R_BADVA		42
#define R_PRID		43
#define R_K1TMP		44

#define R_F0		45
#define R_F1		46
#define R_F2		47
#define R_F3		48
#define R_F4		49
#define R_F5		50
#define R_F6		51
#define R_F7		52
#define R_F8		53
#define R_F9		54
#define R_F10		55
#define R_F11		56
#define R_F12		57
#define R_F13		58
#define R_F14		59
#define R_F15		60
#define R_F16		61
#define R_F17		62
#define R_F18		63
#define R_F19		64
#define R_F20		65
#define R_F21		66
#define R_F22		67
#define R_F23		68
#define R_F24		69
#define R_F25		70
#define R_F26		71
#define R_F27		72
#define R_F28		73
#define R_F29		74
#define R_F30		75
#define R_F31		76
#define R_FCR		77
#define R_FID		78
#define NREGS		79

/* stuff for LEDs on Pocket Rocket and RacerX boards */

#ifdef RACERX
#define RED_LED	0x20
#define GRN_LED 0x40
#else
#define RED_LED	0x02
#define GRN_LED 0x01
#define SIOBASE 0xbe000000
#endif

#ifdef LANGUAGE_C
#define LED_OFF (*((volatile unsigned char *)SIOBASE+0x38))
#define LED_ON (*((volatile unsigned char *)SIOBASE+0x3c))
#else
#define LED_OFF SIOBASE+0x38
#define LED_ON  SIOBASE+0x3c
#endif

