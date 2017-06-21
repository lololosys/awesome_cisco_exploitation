/* $Id: dis_mips.c,v 3.2.58.1 1996/03/21 23:30:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/dis_mips.c,v $
 *------------------------------------------------------------------
 * dis_mips.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Disassembler for Orion.
 *------------------------------------------------------------------
 * $Log: dis_mips.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:06  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:43  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * Note : When the R4000 is in 64 bit mode, the 64 bit instructions
 * are used rather than the Cop 3 instructions (allthough the opcodes
 * overlap). The disassembler will allways decode the opcodes as 64 bit
 * instructions.
 */

#include "monitor.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "dis_mips.h"

extern int optind;

/* 
 * tables
 */
static char *regs[] = {
    "zero","at","v0","v1","a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9","k0","k1","gp","sp","s8","ra"
    };

static char *cp0regs[] = {
    "index","random","tlblo","3","context","5","5","7",
    "badvaddr","count","tlbhi","compare","sr","cause","epc","prid",
    "config","lladr","watchlo","watchhi","xcontext","21","22","23",
    "24","25","ecc","cacheerr","taglo","taghi","errorepc","31"
    };

/*
 * Note : match is unsigned short to save space; it is compared to the
 * op code bits of the instruction. Flags are in the high order bits.
 */

struct optab {
    unsigned short match;
    int (*opfunc)();
    char *str;
};

/*
 * Forward declarations
 */
static int f_special(), f_regim(), f_jump(), f_jal(), f_bra(), f_isig();
static int f_lui(), f_cop0(), f_copz(), f_s_l(), f_cache();
static int f_s_l_cop();

struct optab optab[] = {
    0x00, f_special, 0,
    0x01, f_regim, 0,
    0x02, f_jump, 0,
    0x03, f_jal, 0, 
    0x04, f_bra, "beq",
    0x05, f_bra, "bne",
    0x06, f_bra, "blez",
    0x07, f_bra, "bgtz",

    0x08, f_isig, "addi",
    0x09, f_isig, "addiu",
    0x0A, f_isig, "slti",
    0x0B, f_isig, "sltiu",
    0x0C, f_isig, "andi",
    0x0D, f_isig, "ori",
    0x0E, f_isig, "xori",
    0x0F, f_lui, 0,

    0x10, f_cop0, 0,
    0x11, f_copz, "1",
    0x12, f_copz, "2",
    0x14, f_bra, "beql",
    0x15, f_bra, "bnel",
    0x16, f_bra, "blezl",
    0x17, f_bra, "bgtzl",

    0x18, f_isig, "daddi",
    0x19, f_isig, "daddiu",
    0x1A, f_s_l, "ldl",
    0x1B, f_s_l, "ldr",

    0x20, f_s_l, "lb",
    0x21, f_s_l, "lh",
    0x22, f_s_l, "lwl",
    0x23, f_s_l, "lw",
    0x24, f_s_l, "lbu",
    0x25, f_s_l, "lhu",
    0x26, f_s_l, "lwr",
    0x27, f_s_l, "lwu",

    0x28, f_s_l, "sb",
    0x29, f_s_l, "sh",
    0x2A, f_s_l, "swl",
    0x2B, f_s_l, "sw",
    0x2C, f_s_l, "sdl",
    0x2D, f_s_l, "sdr",
    0x2E, f_s_l, "swr",
    0x2F, f_cache, 0,

    0x30, f_s_l, "ll",
    0x31, f_s_l_cop, "lwc1",
    0x32, f_s_l_cop, "lwc2",
    0x33, f_s_l_cop, "lwc3",
    0x34, f_s_l, "lld",
    0x35, f_s_l_cop, "ldc1",
    0x36, f_s_l_cop, "ldc2",
    0x37, f_s_l, "ld",

    0x38, f_s_l, "sc",
    0x39, f_s_l_cop, "swc1",
    0x3A, f_s_l_cop, "swc2",
    0x3B, f_s_l_cop, "swc3",
    0x3C, f_s_l, "scd",
    0x3D, f_s_l_cop, "sdc1",
    0x3E, f_s_l_cop, "sdc2",
    0x3F, f_s_l, "sd"
};

struct op_spec {
    unsigned short match;
    char *str;
};

struct op_spec op_spec[] = {
    0x00|F_RDRTRE,      "sll",
    0x02|F_RDRTRE,      "srl",
    0x03|F_RDRTRE,      "sra",
    0x04|F_RDRTRS,      "sllv",
    0x06|F_RDRTRS,      "srlv",
    0x07|F_RDRTRS,      "srav",

    0x08|F_RS,          "jr",
    0x09|F_RDRS,        "jalr",
    0x0C|F_UNI,         "syscall",
    0x0D|F_UNI,         "break",
    0x0F|F_UNI,         "sync",
    
    0x10|F_RD,          "mfhi",
    0x11|F_RS,          "mthi",
    0x12|F_RD,          "mflo",
    0x13|F_RS,          "mtlo",
    0x14|F_RDRTRS,      "dsllv",
    0x16|F_RDRTRS,      "dsrlv",
    0x17|F_RDRTRS,      "dsrav",

    0x18|F_RSRT,        "mult",
    0x19|F_RSRT,        "multu",
    0x1A|F_RSRT,        "div",
    0x1B|F_RSRT,        "divu",
    0x1C|F_RSRT,        "dmult",
    0x1D|F_RSRT,        "dmultu",
    0x1E|F_RSRT,        "ddiv",
    0x1F|F_RSRT,        "ddivu",

    0x20,               "add",
    0x21,               "addu",
    0x22,               "sub",
    0x23,               "subu",
    0x24,               "and",
    0x25,               "or",
    0x26,               "xor",
    0x27,               "nor",
    
    0x2A,               "slt",
    0x2B,               "sltu",
    0x2C,               "dadd",
    0x2D,               "daddu",
    0x2E,               "dsub",
    0x2F,               "dsubu",
    
    0x30|F_RSRT,        "tge",
    0x31|F_RSRT,        "tgeu",
    0x32|F_RSRT,        "tlt",
    0x33|F_RSRT,        "tltu",
    0x34|F_RSRT,        "teq",
    0x36|F_RSRT,        "tne",

    0x38|F_RDRTRE,      "dsll",
    0x3A|F_RDRTRE,      "dsrl",
    0x3B|F_RDRTRE,      "dsra",
    0x3C|F_RDRTRE,      "dsll32",
    0x3E|F_RDRTRE,      "dsrl32",
    0x3F|F_RDRTRE,      "dsra32"
	
};

struct op_spec op_regim[] = {
    0x00,               "bltz",
    0x01,               "bgez",
    0x02,               "bltzl",
    0x03,               "bgezl",
    0x08|F_REGIMED,     "tgei",
    0x09|F_REGIMED,     "tgeiu",
    0x0A|F_REGIMED,     "tlti",
    0x0B|F_REGIMED,     "tltiu",
    0x0C|F_REGIMED,     "teqi",
    0x0E|F_REGIMED,     "tnei",
    0x10,               "bltzal",
    0x11,               "bgezal",
    0x12,               "bltzall",
    0x13,               "bgezall"
};

/*
 * disasm : Disassemble instructions for MIPS/Orion
 * dis [addr] [length]
 */
int
disasm(int argc, char *argv[])
{
    unsigned char *addr;
    char buf[80];
    int count, bytes, i;

    optind = 1;
    if ( argc == 1 ) {
	query_user( QU_START | QU_SIZE, &(NVRAM)->start, &(NVRAM)->length );
    } else if ( getnum(argv[optind++], 16, &(NVRAM)->start) == 0 ||
        (argc == 3 && getnum(argv[optind++], 16, &(NVRAM)->length) == 0) ||
	(argc > 3) ) {
	printf("usage : %s [addr] [length]\n", argv[0]);
	return(1);
    }
    addr = (unsigned char *)(NVRAM)->start;
    count = (argc == 2) ? 0x20 : (NVRAM)->length;
    
    while ( count > 0 ) {
	bytes = dis_instr( (unsigned int *)addr, buf);
	printf("0x%.8x: ", addr);
	for (i=0; i<12; i++)
	    if ( i < bytes )
		printf("%.2x", *addr++);
	    else
		printf("  ");
	count -= bytes;
	printf("%s\n", buf);
    }
    return(0);
} 

/*
 * dis_instr : Disassemble at addr into buf, return no. of bytes eaten
 */
int
dis_instr(unsigned int *addr, char *buf)
{
    union instr_r4k *op;
    int i;

    if ( (int)addr & 0x3 ) {
	printf("Warning : address not word aligned, 0x%x\n", addr);
	(int)addr &= -4;
    }
    buf[0] = 0;
    op = (union instr_r4k *)addr;
    for (i=0; i<(sizeof(optab)/sizeof(struct optab)); i++) {
	if ( optab[i].match == op->i_t.op )
	    break;
    }
    if ( i == (sizeof(optab)/sizeof(struct optab)) ) {
	sprintf(buf, ".long 0x%.8x", *addr);
	return(4);
    } else {
	return( (*optab[i].opfunc)(op, buf, optab[i].str) );
    }
}

/*
 * f_bra : Handle branch instructions
 */
static int
f_bra(union instr_r4k *op, char *buf, char *str)
{
    long loc = (long)op + 4;
    short disp = (short)(op->i_t.s_imd);

    loc = (loc + (disp << 2));
    sprintf(buf, "%s %s, %s, #0x%x", str, regs[op->i_t.rs], regs[op->i_t.rt],
	    loc);
    return(4);
}

/*
 * f_special : Handle SPECIAL instructions
 */
static int 
f_special(union instr_r4k *op, char *buf, char *str)
{
    int i;
    
    for (i=0; i<(sizeof(op_spec)/sizeof(struct op_spec)); i++) {
	if ( (op_spec[i].match & SPEC_MASK) == op->r_t.func )
	    break;
    }
    if ( i == sizeof(op_spec)/sizeof(struct op_spec) ) {
	sprintf(buf, "Unknown special opcode");
	return(4);
    }
    if ( op_spec[i].match & F_RDRTRE ) {
	if ( op->op_code == 0 ) {
	    sprintf(buf, "nop");
	} else {
	    sprintf(buf, "%s %s, %s, %d", op_spec[i].str, regs[op->r_t.rd],
		    regs[op->r_t.rt], op->r_t.re);
	}
    } else if ( op_spec[i].match & F_RDRTRS ) {
	sprintf(buf, "%s %s, %s, %s", op_spec[i].str, regs[op->r_t.rd],
		regs[op->r_t.rt], regs[op->r_t.rs]);
    } else if ( op_spec[i].match & F_UNI ) {
	sprintf(buf, "%s", op_spec[i].str);
    } else if ( op_spec[i].match & F_RS ) {
	sprintf(buf, "%s %s", op_spec[i].str, regs[op->r_t.rs]);
    } else if ( op_spec[i].match & F_RD ) {
	sprintf(buf, "%s %s", op_spec[i].str, regs[op->r_t.rd]);
    } else if ( op_spec[i].match & F_RDRS ) {
	sprintf(buf, "%s %s, %s", op_spec[i].str, regs[op->r_t.rd], 
		regs[op->r_t.rs]);
    } else if ( op_spec[i].match & F_RSRT ) {
	sprintf(buf, "%s %s, %s", op_spec[i].str, regs[op->r_t.rs], 
		regs[op->r_t.rt]);
    } else {
	sprintf(buf, "%s %s, %s, %s", op_spec[i].str,
		regs[op->r_t.rd], regs[op->r_t.rs], regs[op->r_t.rt]);
    }
    return(4);
}

/*
 * f_regim
 */
static int 
f_regim(union instr_r4k *op, char *buf, char *str)
{
    int i;
    long loc = (long)op + 4;
    short disp = (short)(op->i_t.s_imd);

    loc = (loc + (disp << 2));
    for (i=0; i<(sizeof(op_regim)/sizeof(struct op_spec)); i++) {
	if ( (op_regim[i].match & REGIM_MASK) == op->r_t.rt )
	    break;
    }

    if ( i == sizeof(op_regim)/sizeof(struct op_spec) ) {
	sprintf(buf, "Unknown regimm opcode");
	return(4);
    }
    if ( op_regim[i].match & F_REGIMED ) {
	sprintf(buf, "%s %s, #0x%x", op_regim[i].str, regs[op->r_t.rs], disp);
    } else {
	sprintf(buf, "%s %s, #0x%x", op_regim[i].str, regs[op->r_t.rs], loc);
    }
    return(4);
}

/*
 * f_jump
 */
static int 
f_jump(union instr_r4k *op, char *buf, char *str)
{
    long target = ((long)op + 8) & HI_ADDR_MASK;

    sprintf(buf, "j 0x%x", target + (op->j_t.target << 2) );
    return(4);
}

/*
 * f_jal
 */
static int 
f_jal(union instr_r4k *op, char *buf, char *str)
{
    long target = ((long)op + 8) & HI_ADDR_MASK;

    sprintf(buf, "jal 0x%x", target + (op->j_t.target << 2) );
    return(4);
}

/*
 * f_isig
 */
static int 
f_isig(union instr_r4k *op, char *buf, char *str)
{
    short disp = (short)op->u_t.u_imd;
    
    sprintf(buf, "%s %s, %s, #0x%x", str, regs[op->u_t.rt], regs[op->u_t.rs],
	    disp);
    return(4);
}

/*
 * f_lui
 */
static int 
f_lui(union instr_r4k *op, char *buf, char *str)
{
    sprintf(buf, "lui %s, #0x%x", regs[op->u_t.rt], op->u_t.u_imd);
    return(4);
}

/*
 * f_cop0
 */
static int 
f_cop0(union instr_r4k *op, char *buf, char *str)
{
    if ( op->r_t.rs & COP0_C0MASK ) {
	switch ( op->r_t.func ) {
	case COP0_TLBR  : sprintf(buf, "tlbr"); break;
	case COP0_TLBWI : sprintf(buf, "tlbwi"); break;
	case COP0_TLBWR : sprintf(buf, "tlbwr"); break;
	case COP0_TLBP  : sprintf(buf, "tlbp"); break;
	case COP0_ERET  : sprintf(buf, "eret"); break;
	default         : sprintf(buf, "cop0 ???"); break;
	}
    } else {
	return ( f_copz(op, buf, "0") );
    }
    return(4);
}

/*
 * copz_regs
 */
static char *
copz_regs(int reg, char *str, char *buf)
{
    if ( *str == '0' ) {
	sprintf(buf, "%s", cp0regs[reg]);
    } else {
	sprintf(buf, "$%d", reg);
    }
}	

/*
 * f_copz
 */
static int 
f_copz(union instr_r4k *op, char *buf, char *str)
{
    long loc = (long)op + 4;
    short disp = (short)(op->i_t.s_imd);
    char *mnem;

    loc = (loc + (disp << 2));
    switch ( op->r_t.rs ) {
    case COPZ_BC:
	switch ( op->r_t.rt ) {
	case COPZ_BCF  : buf += sprintf(buf, "bc%sf", str); break;
	case COPZ_BCT  : buf += sprintf(buf, "bc%st", str); break;
	case COPZ_BCFL : buf += sprintf(buf, "bc%sfl", str); break;
	case COPZ_BCTL : buf += sprintf(buf, "bc%stl", str); break;
	default        : buf += sprintf(buf, "bc%s???", str); break;
	}
	sprintf(buf, " #0x%x", loc);
	return(4);
    case COPZ_MF  : mnem = "mfc"; break;
    case COPZ_DMF : mnem = "dmfc"; break;
    case COPZ_CF  : mnem = "cfc"; break;
    case COPZ_MT  : mnem = "mtc"; break;
    case COPZ_DMT : mnem = "dmtc"; break;
    case COPZ_CT  : mnem = "ctc"; break;
    default       : mnem = "cop ???"; break;
    }
    buf += sprintf(buf, "%s%s %s, ", mnem, str, regs[op->r_t.rt] );
    copz_regs(op->r_t.rd, str, buf);
    return(4);
}

/*
 * f_s_l
 */
static int 
f_s_l(union instr_r4k *op, char *buf, char *str)
{
    sprintf(buf, "%s %s, %d(%s)", str, regs[op->i_t.rt], 
	    op->i_t.s_imd, regs[op->i_t.rs]);
    return(4);
}

static char *cache_func[] = {
    "index invalidate", "load tag", "store tag", "create dirty",
    "hit invalidate", "writeback invalidate", "hit writeback", 
    "hit set virtual"
    };

static char *cache_name[] = {
    "I", "D", "SI", "SD" 
    };

/*
 * f_cache
 */
static int 
f_cache(union instr_r4k *op, char *buf, char *str)
{
    sprintf(buf, "cache [%s] %s, %d(%s)", cache_name[op->c_t.cache],
	    cache_func[op->c_t.func], op->c_t.s_imd, regs[op->c_t.base]);
    return(4);
}

/*
 * f_s_l_cop
 */
static int 
f_s_l_cop(union instr_r4k *op, char *buf, char *str)
{
    sprintf(buf, "%s %s, %d(%s)", str, regs[op->i_t.rt], 
	    op->i_t.s_imd, regs[op->i_t.rs]);
    return(4);
}

/* End of Module */
