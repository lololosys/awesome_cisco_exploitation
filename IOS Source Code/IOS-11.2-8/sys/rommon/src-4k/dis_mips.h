/* $Id: dis_mips.h,v 3.2 1995/11/17 18:45:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k/dis_mips.h,v $
 *------------------------------------------------------------------
 * dis_mips.h
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Disassembler header file for MIPS/Orion.
 *------------------------------------------------------------------
 * $Log: dis_mips.h,v $
 * Revision 3.2  1995/11/17  18:45:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * structure to map opcode encodings for MIPS/Orion CPU
 */

union instr_r4k {
    unsigned int op_code;

    struct {  /* jump type instruction */
	unsigned op : 6;
	unsigned target : 26;
    } j_t;

    struct {  /* immediate type instruction */
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	signed s_imd : 16;
    } i_t;

    struct {
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned u_imd : 16;
    } u_t;

    struct {  /* register type instruction */
	unsigned op : 6;
	unsigned rs : 5;
	unsigned rt : 5;
	unsigned rd : 5;
	unsigned re : 5;
	unsigned func : 6;
    } r_t;

    struct {
	unsigned op : 6;
	unsigned base : 5;
	unsigned func : 3;
	unsigned cache : 2;
	signed s_imd;
    } c_t;

};

/*
 * Defines for the decoding opcodes
 */

#define SPEC_MASK              0x3F
#define REGIM_MASK             0x1F
#define COP0_C0MASK            0x10
#define HI_ADDR_MASK           0xF0000000

#define F_REGIMED              0x8000
#define F_RDRTRE               0x8000
#define F_RDRTRS               0x4000
#define F_UNI                  0x2000
#define F_RS                   0x1000
#define F_RD                   0x0800
#define F_RDRS                 0x0400
#define F_RSRT                 0x0200

#define COP0_TLBR              0x01
#define COP0_TLBWI             0x02
#define COP0_TLBWR             0x06
#define COP0_TLBP              0x08
#define COP0_ERET              0x18

#define COPZ_BCF               0x00
#define COPZ_BCT               0x01
#define COPZ_BCFL              0x02
#define COPZ_BCTL              0x03

#define COPZ_MF                0x00
#define COPZ_DMF               0x01
#define COPZ_CF                0x02
#define COPZ_MT                0x04
#define COPZ_DMT               0x05
#define COPZ_CT                0x06
#define COPZ_BC                0x08

/*
 * Defines for interpreting address error and branch instructions
 */
#define OPCODE_LHU             0x25
#define OPCODE_LH              0x21
#define OPCODE_LW              0x23
#define OPCODE_LWU             0x2F
#define OPCODE_SH              0x29
#define OPCODE_SW              0x2B

#define OPCODE_SPECIAL         0x00
#define OPCODE_JALR            0x09
#define OPCODE_JR              0x08

#define OPCODE_REGIMM          0x01
#define OPCODE_BGEZ            0x01
#define OPCODE_BGEZAL          0x11
#define OPCODE_BLTZ            0x00
#define OPCODE_BLTZAL          0x10
#define OPCODE_BGEZL           0x03
#define OPCODE_BGEZALL         0x13
#define OPCODE_BLTZL           0x02
#define OPCODE_BLTZALL         0x12

#define OPCODE_J               0x02
#define OPCODE_JAL             0x03
#define OPCODE_BEQ             0x04
#define OPCODE_BGTZ            0x07
#define OPCODE_BLEZ            0x06
#define OPCODE_BNE             0x05
#define OPCODE_BEQL            0x24
#define OPCODE_BGTZL           0x17
#define OPCODE_BLEZL           0x16
#define OPCODE_BNEL            0x15

/*
 * Opcodes to allow virtual stack trace decoding
 */
#define OPCODE_JR_RA           0x03E00008
#define OPCODE_ADDIU           0x09
#define OPCODE_LW              0x23
#define OPCODE_SW              0x2B

/*
 * Define for a MIPS breakpoint
 */
#define MIPS_BREAK_POINT       0x0000000D

/* End of Module */




