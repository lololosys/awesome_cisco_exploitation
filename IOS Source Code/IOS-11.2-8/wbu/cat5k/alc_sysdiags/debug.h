/* $Id: debug.h,v 1.1.4.1 1996/05/09 15:00:55 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/debug.h,v $
 *-----------------------------------------------------------------
 * debug.h -- header file for Debug functions
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:55  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:47  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:22  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains "Debug-related Variable Definitions" that are included  */
/* as part of Diagnostics Driver.                                             */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

        enum            DEBUG_COMMAND_TYPE {
        DEBUG_COMMAND_TYPE_WRITE   = 'w',
        DEBUG_COMMAND_TYPE_READ    = 'r',
        DEBUG_COMMAND_TYPE_COMPARE = 'c',
        DEBUG_COMMAND_TYPE_SEARCH  = 's',
        DEBUG_COMMAND_TYPE_ASM     = 'a',
        DEBUG_COMMAND_TYPE_DASM    = 'd',
        DEBUG_COMMAND_TYPE_EXECUTE = 'e',
        DEBUG_COMMAND_TYPE_TEST    = 't'
        };

        enum            ARGUMENT_OPTION {
        ARGUMENT_OPTION_START_ADDRESS            = 's',
        ARGUMENT_OPTION_END_ADDRESS              = 'e',
        ARGUMENT_OPTION_LENGTH                   = 'l',
        ARGUMENT_OPTION_DATA                     = 'd',
        ARGUMENT_OPTION_ACCESS                   = 'a',
        ARGUMENT_OPTION_MODE                     = 'm',
        ARGUMENT_OPTION_INCREMENT_OR_INSTRUCTION = 'i',
        ARGUMENT_OPTION_NUMBER_OF_TIMES          = 'n',
        ARGUMENT_OPTION_TIME_DELAY               = 't',
        ARGUMENT_OPTION_VERBOSITY                = 'v',
        ARGUMENT_OPTION_WRITE_ADDRESS            = 'w',
        ARGUMENT_OPTION_READ_ADDRESS             = 'r',
        ARGUMENT_OPTION_COMPARE_DATA             = 'c'
        };

        enum            MINIMUM_ARGUMENTS {
        MINIMUM_WRITE_ARGUMENTS   = 3,
        MINIMUM_READ_ARGUMENTS    = 2,
        MINIMUM_COMPARE_ARGUMENTS = 3,
        MINIMUM_SEARCH_ARGUMENTS  = 3,
        MINIMUM_ASM_ARGUMENTS     = 3,
        MINIMUM_DASM_ARGUMENTS    = 2,
        MINIMUM_EXECUTE_ARGUMENTS = 2,
        MINIMUM_TEST_ARGUMENTS    = 3
        };

        enum            MAXIMUM_ARGUMENTS {
        MAXIMUM_WRITE_ARGUMENTS   = 9,
        MAXIMUM_READ_ARGUMENTS    = 6,
        MAXIMUM_COMPARE_ARGUMENTS = 9,
        MAXIMUM_SEARCH_ARGUMENTS  = 7,
        MAXIMUM_ASM_ARGUMENTS     = 4,
        MAXIMUM_DASM_ARGUMENTS    = 3,
        MAXIMUM_EXECUTE_ARGUMENTS = 5,
        MAXIMUM_TEST_ARGUMENTS    = 8
        };

#ifdef DEBUGFUNC
	enum	ASM_ERROR_CODE {
	ASM_INSTRUCTION_NOT_IMPLEMENTED = -6,
	ASM_UNEXPECTED_OPERAND = -5,
	ASM_MISSING_OPERAND = -4,
	ASM_INVALID_OPERAND = -3,
	ASM_OPERAND_OUT_OF_RANGE = -2,
	ASM_INVALID_INSTRUCTION_FORMAT = -1
	};

#ifndef RISC
#define	OPCODE_MASK	0xF000
	enum	OPCODE_TYPE {
	OPCODE_0000 = 0x0000,
	OPCODE_0001 = 0x1000,
	OPCODE_0010 = 0x2000,
	OPCODE_0011 = 0x3000,
	OPCODE_0100 = 0x4000,
	OPCODE_0101 = 0x5000,
	OPCODE_0110 = 0x6000,
	OPCODE_0111 = 0x7000,
	OPCODE_1000 = 0x8000,
	OPCODE_1001 = 0x9000,
	OPCODE_1010 = 0xA000,
	OPCODE_1011 = 0xB000,
	OPCODE_1100 = 0xC000,
	OPCODE_1101 = 0xD000,
	OPCODE_1110 = 0xE000,
	OPCODE_1111 = 0xF000
	};

	enum	OPCODE_0000_INSTRUCTION_INDEX {
	ORI_B,   ORI_W,   ORI_L,   ORI,
	ANDI_B,  ANDI_W,  ANDI_L,  ANDI,
	EORI_B,  EORI_W,  EORI_L,  EORI,
	SUBI_B,  SUBI_W,  SUBI_L,  
	ADDI_B,  ADDI_W,  ADDI_L,   
	CMPI_B,  CMPI_W,  CMPI_L, 
	CMP2_B,  CMP2_W,  CMP2_L, 
	CHK2_B,  CHK2_W,  CHK2_L, 
	BTST,    BCHG,    BCLR,    BSET,
	MOVEP_W, MOVEP_L,
	MOVES_B, MOVES_W, MOVES_L,
	CAS_B,   CAS_W,   CAS_L,
	CAS2_W,  CAS2_L,  CALLM
	};
	enum	OPCODE_0001_INSTRUCTION_INDEX {
	MOVE_B
	};
	enum	OPCODE_0010_INSTRUCTION_INDEX {
	MOVE_L, MOVEA_L
	};
	enum	OPCODE_0011_INSTRUCTION_INDEX {
	MOVE_W, MOVEA_W
	};
	enum	OPCODE_0100_INSTRUCTION_INDEX {
	NEGX_B,  NEGX_W,  NEGX_L,  MOVE_1,
	CLR_B,   CLR_W,   CLR_L,   MOVE_2,
	NEG_B,   NEG_W,   NEG_L,   MOVE_3,
	NOT_B,   NOT_W,   NOT_L,   MOVE_4,
	NBCD,    LINK_W,  LINK_L,  SWAP,    BKPT,    PEA,
	EXT_W,   EXT_L,   EXTB_L,  MOVEM_W, MOVEM_L,
	TST_B,   TST_W,   TST_L,   TAS,     BGND,    ILLEGAL,
	MULU_L,  MULS_L,  DIVU_L,  DIVS_L,
	CHK_W,   CHK_L,   LEA,
	TRAP,    UNLK,    MOVE_5,  RESET,   NOP,     STOP,    RTE,
	RTD,     RTS,     TRAPV,   RTR,     MOVEC,   JSR,     JMP
	};
	enum	OPCODE_0101_INSTRUCTION_INDEX {
	ADDQ_B, ADDQ_W, ADDQ_L,
	SUBQ_B, SUBQ_W, SUBQ_L,
	DBT,    DBF,    DBHI,   DBLS,
	DBCC,   DBCS,   DBNE,   DBEQ,
	DBVC,   DBVS,   DBPL,   DBMI,
	DBGE,   DBLT,   DBGT,   DBLE,
	ST,     SF,     SHI,    SLS,
	SCC,    SCS,    SNE,    SEQ,
	SVC,    SVS,    SPL,    SMI,
	SGE,    SLT,    SGT,    SLE,
	TRAPT,  TRAPF,  TRAPHI, TRAPLS,
	TRAPCC, TRAPCS, TRAPNE, TRAPEQ,
	TRAPVC, TRAPVS, TRAPPL, TRAPMI,
	TRAPGE, TRAPLT, TRAPGT, TRAPLE
	};
	enum	OPCODE_0110_INSTRUCTION_INDEX {
	BRA, BSR, BHI, BLS,
	BCC, BCS, BNE, BEQ,
	BVC, BVS, BPL, BMI,
	BGE, BLT, BGT, BLE
	};
	enum	OPCODE_0111_INSTRUCTION_INDEX {
	MOVEQ
	};

	char		*instruction[16][60] =
	{ { "ori.b",   "ori.w",   "ori.l",   "ori",
	    "andi.b",  "andi.w",  "andi.l",  "andi",
	    "eori.b",  "eori.w",  "eori.l",  "eori",
	    "subi.b",  "subi.w",  "subi.l",
	    "addi.b",  "addi.w",  "addi.l",
	    "cmpi.b",  "cmpi.w",  "cmpi.l",
	    "cmp2.b",  "cmp2.w",  "cmp2.l",
	    "chk2.b",  "chk2.w",  "chk2.l",
	    "btst",    "bchg",    "bclr",    "bset",
	    "movep.w", "movep.l" ,
	    "moves.b", "moves.w", "moves.l",
	    "cas.b",   "cas.w",   "cas.l",
	    "cas2.w",  "cas2.l",  "callm",   "" },
	  { "move.b", "" },
	  { "move.l", "movea.l", "" },
	  { "move.w", "movea.w", "" },
	  { "negx.b", "negx.w", "negx.l", "move",
	    "clr.b",  "clr.w",  "clr.l",  "move",
	    "neg.b",  "neg.w",  "neg.l",  "move",
	    "not.b",  "not.w",  "not.l",  "move",
	    "nbcd",   "link.w", "link.l", "swap",    "bkpt",    "pea", 
	    "ext.w",  "ext.l",  "extb.l", "movem.w", "movem.l",
	    "tst.b",  "tst.w",  "tst.l",  "tas",     "bgnd",    "illegal",
	    "mulu.l", "muls.l", "divu.l", "divs.l",
	    "chk.w",  "chk.l",  "lea", 
	    "trap",   "unlk",   "move",   "reset",   "nop",     "stop",   "rte",
	    "rtd",    "rts",    "trapv",  "rtr",     "movec",   "jsr",    "jmp",
	    "" },
	  { "addq.b", "addq.w", "addq.l",
	    "subq.b", "subq.w", "subq.l",
	    "dbt",    "dbf",    "dbhi",   "dbls",  
	    "dbcc",   "dbcs",   "dbne",   "dbeq",
	    "dbvc",   "dbvs",   "dbpl",   "dbmi",  
	    "dbge",   "dblt",   "dbgt",   "dble",
	    "st",     "sf",     "shi",    "sls",   
	    "scc",    "scs",    "sne",    "seq",
	    "svc",    "svs",    "spl",    "smi",   
	    "sge",    "slt",    "sgt",    "sle",
	    "trapt",  "trapf",  "traphi", "trapls",
	    "trapcc", "trapcs", "trapne", "trapeq",
	    "trapvc", "trapvs", "trappl", "trapmi",
	    "trapge", "traplt", "trapgt", "traple", "" },
	  { "bra",    "bsr",    "bhi",    "bls",   
	    "bcc",    "bcs",    "bne",    "beq", 
            "bvc",    "bvs",    "bpl",    "bmi",   
	    "bge",    "blt",    "bgt",    "ble", "" },
	  { "moveq", "" },
	  { "or.b", "or.w", "or.l", "divu.w",
	    "or.b", "or.w", "or.l", "divs.w",
	    "sbcd", "pack", "unpk", "" },
	  { "sub.b",  "sub.w",  "sub.l",  "suba.w", 
            "sub.b",  "sub.w",  "sub.l",  "suba.l", 
	    "subx.b", "subx.w", "subx.l", "" },
	  { "" },
	  { "cmp.b",  "cmp.w",  "cmp.l",  "cmpa.w", 
            "eor.b",  "eor.w",  "eor.l",  "cmpa.l",
	    "cmpm.b", "cmpm.w", "cmpm.l", "" },
	  { "and.b", "and.w", "and.l", "mulu.w",
	    "and.b", "and.w", "and.l", "muls.w",
	    "abcd",  "exg", "" },
	  { "add.b",  "add.w",  "add.l",  "adda.w", 
            "add.b",  "add.w",  "add.l",  "adda.l", 
	    "addx.b", "addx.w", "addx.l", "" },
	  { "asr.b",  "asr.w",  "asr.l",
	    "asl.b",  "asl.w",  "asl.l",
	    "lsr.b",  "lsr.w",  "lsr.l",
	    "lsl.b",  "lsl.w",  "lsl.l",
	    "roxr.b", "roxr.w", "roxr.l",
	    "roxl.b", "roxl.w", "roxl.l",
	    "ror.b",  "ror.w",  "ror.l",
	    "rol.b",  "rol.w",  "rol.l",
	    "bftst",  "bfextu",  
	    "bfchg",  "bfexts",
	    "bfclr",  "bfffo",
	    "bfset",  "bfins", "" },
	  { "" } };
#else /* RISC */
	char		*instruction[3][64] =
	{ { "sll",  "",      "srl",  "sra",  "sllv",    "",      "srlv", "srav",
            "jr",   "jalr",  "",     "",     "syscall", "break", "",     "",
	    "mfhi", "mthi",  "mflo", "mtlo", "",        "",      "",     "",
	    "mult", "multu", "div",  "divu", "",        "",      "",     "",
	    "add",  "addu",  "sub",  "subu", "and",     "or",    "xor",  "nor",
	    "",     "",      "slt",  "sltu", "",        "",      "",     "",
            "",     "",      "",     "",     "",        "",      "",     "",
            "",     "",      "",     "",     "",        "",      "",     "" },
	  { "bltz", "bgez",  "",     "",     "",        "",      "",     "",
	    "",     "",      "",     "",     "",        "",      "",     "",
	    "bltzal", "bgezal", "",  "",     "",        "",      "",     "",
	    "",     "",      "",     "",     "",        "",      "",     "" },
	  { "",     "",      "j",    "jal",  "beq",     "bne",   "blez", "bgtz",
	    "addi", "addiu", "slti", "sltiu","andi",    "ori",   "xori", "lui",
	    "",     "",      "",     "",     "",        "",      "",     "",
	    "",     "",      "",     "",     "",        "",      "",     "",
	    "lb",   "lh",    "lwl",  "lw",   "lbu",     "lhu",   "lwr",  "",
	    "sb",   "sh",    "swl",  "sw",   "",        "",      "swr",  "",
	    "lwc0", "",      "",     "",     "",        "",      "",     "",
	    "swc0", "",      "",     "",     "",        "",      "",     "" } };

	char		*cop_instruction[3][32] =
	{ { "mfc0", "", "cfc0", "", "mtc0", "", "ctc0", "",
	    "mfc1", "", "cfc1", "", "mtc1", "", "ctc1", "",
	    "mfc2", "", "cfc2", "", "mtc2", "", "ctc2", "",
	    "mfc3", "", "cfc3", "", "mtc3", "", "ctc3", "" },
	  { "bc0f", "bc0t", "", "", "", "", "", "",
	    "bc1f", "bc1t", "", "", "", "", "", "",
	    "bc2f", "bc2t", "", "", "", "", "", "",
	    "bc3f", "bc3t", "", "", "", "", "", "" },
	  { "",     "tlbr", "tlbwi", "", "",     "", "tlbwr","",
	    "tlbp", "",     "",      "", "",     "", "",     "",
	    "rfe",  "",     "",      "", "",     "", "",     "",
	    "",     "",     "",      "", "",     "", "",     "" } };

	char		*reg_name[32] =
	{ "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
	  "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",
	  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
	  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31" };

	char		*reg_name_a[32] =
	{ "zero",  "at",  "v0",  "v1",  "a0",  "a1",  "a2",  "a3",
	  "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	  "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	  "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra" };

	char		*reg_name_c0[32] =
	{ "index", "random", "entry_lo", "config", "context", "", "", "",
	  "bad_vaddr", "", "entry_hi", "", "sr", "cause", "epc", "prid",
	  "", "", "", "", "", "", "", "",
	  "", "", "", "", "", "", "", "" };

	union instruction_type {
	   struct {
	      unsigned	op : 6;
	      unsigned	rs : 5;
	      unsigned	rt : 5;
	      unsigned	immediate : 16;
	   }		i_type;
	   struct {
	      unsigned	op : 6;
	      unsigned	target : 26;
	   }		j_type;
	   struct {
	      unsigned	op : 6;
	      unsigned	rs : 5;
	      unsigned	rt : 5;
	      unsigned	rd : 5;
	      unsigned	shamt : 5;
	      unsigned	funct : 6;
	   }		r_type;
	   struct {
	      unsigned	op : 6;
	      unsigned	operand : 26;
	   }		all;
	   uInt32	mc;
	};

	enum		OPCODE_TYPE {
	OPCODE_SPECIAL = 0,
	OPCODE_BCOND   = 1,
	OPCODE_J       = 2,
	OPCODE_JAL     = 3,
	OPCODE_BEQ     = 4,
	OPCODE_BNE     = 5,
	OPCODE_BLEZ    = 6,
	OPCODE_BGTZ    = 7,
	OPCODE_ADDI    = 8,
	OPCODE_ADDIU   = 9,
	OPCODE_SLTI    = 10,
	OPCODE_SLTIU   = 11,
	OPCODE_ANDI    = 12,
	OPCODE_ORI     = 13,
	OPCODE_XORI    = 14,
	OPCODE_LUI     = 15,
	OPCODE_COP0    = 16,
	OPCODE_COP1    = 17,
	OPCODE_COP2    = 18,
	OPCODE_COP3    = 19,
	OPCODE_LB      = 32,
	OPCODE_LH      = 33,
	OPCODE_LWL     = 34,
	OPCODE_LW      = 35,
	OPCODE_LBU     = 36,
	OPCODE_LHU     = 37,
	OPCODE_LWR     = 38,
	OPCODE_SB      = 40,
	OPCODE_SH      = 41,
	OPCODE_SWL     = 42,
	OPCODE_SW      = 43,
	OPCODE_SWR     = 46,
	OPCODE_LWC0    = 48,
	OPCODE_SWC0    = 56
	};

	enum		SPECIAL_TYPE {
	SPECIAL_SLL     = 0,
	SPECIAL_SRL     = 2,
	SPECIAL_SRA     = 3,
	SPECIAL_SLLV    = 4,
	SPECIAL_SRLV    = 6,
	SPECIAL_SRAV    = 7,
	SPECIAL_JR      = 8,
	SPECIAL_JALR    = 9,
	SPECIAL_SYSCALL = 12,
	SPECIAL_BREAK   = 13,
	SPECIAL_MFHI    = 16,
	SPECIAL_MTHI    = 17,
	SPECIAL_MFLO    = 18,
	SPECIAL_MTLO    = 19,
	SPECIAL_MULT    = 24,
	SPECIAL_MULTU   = 25,
	SPECIAL_DIV     = 26,
	SPECIAL_DIVU    = 27
	};

	enum		BCOND_TYPE {
	BCOND_BLTZ   = 0,
	BCOND_BGEZ   = 1,
	BCOND_BLTZAL = 16,
	BCOND_BGEZAL = 17
	};

	enum		COP_TYPE {
	COP_MF = 0,
	COP_CF = 2,
	COP_MT = 4,
	COP_CT = 6,
	COP_BC = 8
	};

	enum		BC0_TYPE {
	BC_F = 0,
	BC_T = 1
	};
#endif /* RISC */
#endif /* DEBUGFUNC */

	char		read_fmt_byte[] = "%02X ";
	char		read_fmt_word[] = "%04X ";
	char		read_fmt_long[] = "%08lX ";

	char		compare_fmt_byte[] =
	" %08lX: Expected => %02X, Actual => %02X\n";
	char		compare_fmt_word[] =
	" %08lX: Expected => %04X, Actual => %04X\n";
	char		compare_fmt_long[] =
	" %08lX: Expected => %08lX, Actual => %08lX\n";

	char		test_fmt_byte[] =
	" %02X => @%08lX,   @%08lX => %02X    -> %u\n"; 
	char		test_fmt_word[] = 
	" %04X => @%08lX,   @%08lX => %04X    -> %u\n"; 
	char		test_fmt_long[] =
	" %08lX => @%08lX,   @%08lX => %08lX    -> %u\n"; 

