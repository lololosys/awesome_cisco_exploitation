/* $Id: cpu_mips.h,v 3.2.58.1 1996/03/21 23:30:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/cpu_mips.h,v $
 *------------------------------------------------------------------
 * cpu_mips.h
 *
 * April 1993, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * R4000/Orion processor definitions.
 *------------------------------------------------------------------
 * $Log: cpu_mips.h,v $
 * Revision 3.2.58.1  1996/03/21  23:30:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:24  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/16  21:41:33  szhang
 * CSCdi49173:  Separate the startup and exception code
 * Branch: ELC_branch
 * Separate the startup and exception code into platform
 * dependent and platform independent files.
 *
 * Revision 3.2.2.1  1995/12/05  23:12:24  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.2  1995/11/17  18:44:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/17  01:01:10  clev
 * CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
 *
 * Revision 2.1  1995/06/07  22:39:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Register names
 *
 *         Register $0        - allways 0
 *         Register $1($at)   - reserved for assembler
 *         Register $2..$3    - used for expression evaluation, return values
 *                              and static link passing to nested routines
 *         Register $4..$7    - first four scalar arguments, not preserved
 *                              across function calls
 *         Register $8..$15   - temporary registers, not preserved across
 *                              function calls
 *         Register $16..$23  - registers saved across function calls
 *         Register $24..$25  - temporary registers, not preserved across
 *                              function calls
 *         Register $26..$27  - reserved for kernel
 *         Register $28       - global pointer
 *         Register $29       - stack pointer
 *         Register $30       - register saved across function calls
 *         Register $31       - immediate return address
 */

#ifdef ASMINCLUDE

/****************************************************************/
/* Register names for assembler source files                    */
/****************************************************************/

#define zero    $0           /* allways zero               */
#define AT      $1           /* reserved for assembler     */

#define v0      $2           /* return value               */
#define v1      $3

#define a0      $4           /* arguments 0..3             */
#define a1      $5
#define a2      $6
#define a3      $7

#define t0      $8           /* temporary registers t0-t9  */
#define t1      $9           /* saved by caller            */
#define t2      $10
#define t3      $11
#define t4      $12 
#define t5      $13
#define t6      $14
#define t7      $15
#define t8      $24
#define t9      $25

#define s0      $16          /* saved by callee            */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define s8      $30

#define k0      $26          /* reserved for kernel        */
#define k1      $27

#define gp      $28          /* global pointer             */
#define sp      $29          /* stack pinter               */
#define ra      $31          /* return address             */

/*
 * Alternate register names
 */
#define r0	$0
#define r1	$1
#define r2	$2
#define r3	$3
#define r4	$4
#define r5	$5
#define r6	$6
#define r7	$7
#define r8	$8
#define r9	$9
#define r10	$10
#define r11	$11
#define r12	$12
#define r13	$13

#define r14	$14
#define r15	$15
#define r16	$16
#define r17	$17
#define r18	$18
#define r19	$19
#define r20	$20
#define r21	$21
#define r22	$22
#define r23	$23
#define r24	$24
#define r25	$25
#define r26	$26
#define r27	$27
#define r28	$28
#define r29	$29
#define r30	$30
#define r31	$31

/*
 * Floating point register names
 */
#define FPU_R0	$f0
#define FPU_R1	$f1
#define FPU_R2	$f2
#define FPU_R3	$f3
#define FPU_R4	$f4
#define FPU_R5	$f5
#define FPU_R6	$f6
#define FPU_R7	$f7
#define FPU_R8	$f8
#define FPU_R9	$f9
#define FPU_R10	$f10
#define FPU_R11	$f11
#define FPU_R12	$f12
#define FPU_R13	$f13
#define FPU_R14	$f14
#define FPU_R15	$f15
#define FPU_R16	$f16
#define FPU_R17	$f17
#define FPU_R18	$f18
#define FPU_R19	$f19
#define FPU_R20	$f20
#define FPU_R21	$f21
#define FPU_R22	$f22
#define FPU_R23	$f23
#define FPU_R24	$f24
#define FPU_R25	$f25
#define FPU_R26	$f26
#define FPU_R27	$f27
#define FPU_R28	$f28
#define FPU_R29	$f29
#define FPU_R30	$f30
#define FPU_R31	$f31

/*
 * CP0 Register definitions
 */
#define CP0_INDEX      $0             /* TLB Index           */
#define CP0_RANDOM     $1             /* TLB Random          */
#define CP0_TLB_LO_0   $2             /* TLB Entry Lo0       */
#define CP0_TLB_LO_1   $3             /* TLB Entry Lo1       */
#define CP0_CONTEXT    $4             /* Kernel PTE pointer  */
#define CP0_PAGEMASK   $5             /* TLB Page Mask       */
#define CP0_WIRED      $6             /* TLB Wired           */
#define CP0_BADVADDR   $8             /* Bad Virtual Address */
#define CP0_COUNT      $9             /* Count               */
#define CP0_TLB_HI     $10            /* TLB Entry Hi        */
#define CP0_COMPARE    $11            /* Timer Compare       */
#define CP0_SREG       $12            /* Status              */
#define CP0_CAUSE      $13            /* Cause               */
#define CP0_EPC        $14            /* Exception PC        */
#define CP0_PRID       $15            /* Proc Rev ID         */
#define CP0_CONFIG     $16            /* Configuration       */
#define CP0_LLADDR     $17            /* Load/Link address   */
#define CP0_WATCHLO    $18            /* Low Watch address   */
#define CP0_WATCHHI    $19            /* High Watch address  */
#define CP0_XCONTEXT   $20            /* Extended context    */
#define CP0_ECC        $26            /* ECC and parity      */
#define CP0_CACHERR    $27            /* Cache Err/Status    */
#define CP0_TAGLO      $28            /* Cache Tag Lo        */
#define CP0_TAGHI      $29            /* Cache Tag Hi        */
#define CP0_ERR_EPC    $30            /* Error exception PC  */

/*
 * Define the assembler support macros for procedure framing
 * FRAME    : sets proc as global, generates debug info with .ent
 * ENDFRAME : generates debug into with .end
 */

#define FRAME(name, frame_register, offset, return_register)  \
    .globl name;                                              \
    .ent   name;                                              \
name:                                                         \
    .frame frame_register, offset, return_register

#define ENDFRAME(name)                                        \
    .end name

#endif           /* ASMINCLUDE */

/****************************************************************/
/* End of #ifdef ASMINCLUDE                                     */
/****************************************************************/

/****************************************************************/
/* Cache size, control constants                                */
/****************************************************************/

#define ORION_MINCACHE       0x2000      /* 8K Bytes    */
#define ORION_MAXCACHE       0x8000      /* 32K Bytes   */
#define CSIZE_SHIFT	     14		 /* 1024 lines  */


#define ICACHE_LINES         0x200       /* 512 lines         */
#define ILINE_SHIFT          0x20        /* 32bytes per line  */
#define ICACHE_SIZE          0x4000      /* 16K Bytes         */
#define CLINE_SHIFT	     5		 /* shift for 32bytes */

#define DCACHE_LINES         0x200       /* 512 lines         */
#define DLINE_SHIFT          0x20        /* 32bytes per line  */
#define DCACHE_SIZE          0x4000      /* 16K Bytes         */


/* This module define the constants for cache operation */

#define  CACHELEN	255
#define  CACHEADDR	0x8000    /* 0x80000000, for lui, we use 0x8000 */

#define ICACHE          0x0       /* bits 1..0 for op */
#define DCACHE          0x1

#define IC_INDEX_INV    0x0       /* bits 4..2 for op */
#define IC_INDEX_LDTAG  0x4
#define IC_INDEX_STTAG  0x8
#define IC_HIT_INV      0x10
#define IC_FILL         0x14
#define IC_HIT_WB       0x18

#define DC_INDEX_WB_INV 0x0
#define DC_INDEX_LDTAG  0x4
#define DC_INDEX_STTAG  0x8
#define DC_DIRTY_EXCL   0xC
#define DC_HIT_INV      0x10
#define DC_HIT_WB_INV   0x14
#define DC_HIT_WB       0x18

/****************************************************************/
/* CPU resource constants                                       */
/****************************************************************/

#define TLB_ENTRIES              48

/*
 * CP0 TLB Lo0 and Lo1 Register
 */
#define TLB_LO_PFNMASK          0x3FFFFFC0
#define TLB_LO_CMASK            0x00000038
#define TLB_LO_DMASK            0x00000004
#define TLB_LO_VMASK            0x00000002
#define TLB_LO_GMASK            0x00000001

#define TLB_LO_PFNSHIFT         6
#define TLB_LO_CSHIFT           3
#define TLB_LO_DSHIFT           2
#define TLB_LO_VSHIFT           1
#define TLB_LO_GSHIFT           0

/* Orion Specific */
#define TLB_WT_NO_WRITE_ALLOCATE    0
#define TLB_WT_WRITE_ALLOCATE       1


#define TLB_CACHE_RESERVE       0
#define TLB_UNCACHED            2
#define TLB_CACHE_NONCOHERENT   3
#define TLB_CACHE_COH_EXCL      4
#define TLB_CACHE_EXCL_WRITE    5
#define TLB_CACHE_UPDATE_WRITE  6

/*
 * CP0 TLB Hi Register
 */
#define TLB_HI_VPN2MASK         0xFFFFE000
#define TLB_HI_ASIDMASK         0x000000FF
#define TLB_MAX_ASID            0xFF

#define TLB_HI_VPN2SHIFT        13
#define TLB_HI_ASIDSHIFT        0

/*
 * CP0 TLB Page Register
 */
#define TLB_PAGE_MASK           0x01FFE000
#define TLB_PAGE_SHIFT          13

#define TLB_PAGESIZE_4K         0x000
#define TLB_PAGESIZE_16K        0x003
#define TLB_PAGESIZE_64K        0x00F
#define TLB_PAGESIZE_256K       0x03F
#define TLB_PAGESIZE_1M         0x0FF
#define TLB_PAGESIZE_4M         0x3FF
#define TLB_PAGESIZE_16M        0xFFF

#define TLB_4K_SHIFT            0
#define TLB_16K_SHIFT           2
#define TLB_64K_SHIFT           4
#define TLB_256K_SHIFT          6
#define TLB_1M_SHIFT            8
#define TLB_4M_SHIFT            10
#define TLB_16M_SHIFT           12

/*
 * CP0 TLB Index Register
 */
#define TLB_INX_INXDEXMASK          0x0000003F
#define TLB_INX_PROBEMASK           0x80000000

#define TLB_INX_INDEXSHIFT          0
#define TLB_INX_PROBESHIFT          31

/*
 * CP0 TLB Random Register
 */
#define TLB_RANDOM_MASK             0x0000003F
#define TLB_RANDOM_SHIFT            0

/*
 * CP0 TLB Wired Register
 */
#define TLB_WIRED_MASK              0x0000003F
#define TLB_WIRED_SHIFT             0

/*
 * CP0 TLB Context Register
 */
#define TLB_CTXT_BASEPTE_MASK       0xFF800000
#define TLB_CTXT_BADVPN2_MASK       0x007FFFF0

#define TLB_CTXT_BASEPTE_SHIFT      23
#define TLB_CTXT_BADVPN2_SHIFT      4

/*
 * CP0 Status Register
 */
#define SREG_CU0                    0x10000000
#define SREG_CU1		    0x20000000
#define SREG_BEV                    0x00400000
#define SREG_TS                     0x00200000
#define SREG_SR                     0x00100000
#define SREG_CH                     0x00040000
#define SREG_CE                     0x00020000
#define SREG_DE                     0x00010000
#define SREG_RP                     0x08000000
#define SREG_FR                     0x04000000
#define SREG_RE                     0x02000000
#define SREG_KX                     0x00000080
#define SREG_SX                     0x00000040
#define SREG_UX                     0x00000020
#define SREG_KSU                    0x00000018
#define SREG_ERL                    0x00000004
#define SREG_EXL                    0x00000002
#define SREG_IE                     0x00000001
#define SREG_IMASK8                 0x00000000
#define SREG_IMASK7                 0x00008000
#define SREG_IMASK6                 0x00004000
#define SREG_IMASK5                 0x00002000
#define SREG_IMASK4                 0x00001000
#define SREG_IMASK3                 0x00000800
#define SREG_IMASK2                 0x00000400
#define SREG_IMASK1                 0x00000200
#define SREG_IMASK0                 0x00000100

#define SREG_DS_MASK                0x00770000
#define SREG_CU_MASK                0xF0000000
#define SREG_IMASK                  0x0000FF00

#define SREG_CU_SHIFT               28
#define SREG_DS_SHIFT               16
#define SREG_IMASK_SHIFT            8

/*
 * CP0 Cause register
 */
#define CAUSE_BD_SLOT               0x80000000

#define CAUSE_MASK                  0x0000007C
#define CAUSE_CEMASK                0x30000000
#define CAUSE_IMASK                 0x0000FF00

#define CAUSE_SHIFT                 2
#define CAUSE_CESHIFT               28
#define CAUSE_ISHIFT                8

#define CAUSE_INTERRUPT             0
#define CAUSE_TLB_MOD               1
#define CAUSE_TLB_LOAD              2
#define CAUSE_TLB_SAVE              3
#define CAUSE_ADDR_LOAD             4
#define CAUSE_ADDR_SAVE             5
#define CAUSE_BUS_INSTR             6
#define CAUSE_BUS_DATA              7
#define CAUSE_SYSCALL               8
#define CAUSE_BP                    9
#define CAUSE_ILLOP                 10
#define CAUSE_CP_UNUSABLE           11
#define CAUSE_OVFLW                 12
#define CAUSE_TRAP                  13
#define CAUSE_VIRT_COHERENCY_INSTR  14
#define CAUSE_FPE                   15
#define CAUSE_WATCH                 23
#define CAUSE_VIRT_COHERENCY_DATA   31

#define CAUSE_IBIT7                 0x00008000
#define CAUSE_IBIT6                 0x00004000
#define CAUSE_IBIT5                 0x00002000
#define CAUSE_IBIT4                 0x00001000
#define CAUSE_IBIT3                 0x00000800
#define CAUSE_IBIT2                 0x00000400
#define CAUSE_IBIT1                 0x00000200
#define CAUSE_IBIT0                 0x00000100

#define LEVEL1		1
#define LEVEL2		2
#define LEVEL3		3
#define LEVEL4		4
#define LEVEL5		5
#define LEVEL6		6
#define LEVEL7		7

/*
 * CP0 Config Register
 */
#define CONFIG_MASTER_CHECKER       0x80000000
#define CONFIG_SYS_CLOCK            0x70000000
#define CONFIG_PATTERN              0x0F000000
#define CONFIG_CACHE2_BLOCKSIZE     0x00C00000
#define CONFIG_CACHE2_SPLIT         0x00200000
#define CONFIG_CACHE2_WIDTH         0x00100000
#define CONFIG_SYSPORT_WIDTH        0x000C0000
#define CONFIG_CACHE2_PRESENT       0x00020000
#define CONFIG_DIRTY_SHARED         0x00010000
#define CONFIG_BIG_INDIAN           0x00008000
#define CONFIG_ECC_MODE             0x00004000
#define CONFIG_BLOCK_ORDER          0x00002000
#define CONFIG_ICACHE_SIZE          0x00000E00
#define CONFIG_DCACHE_SIZE          0x000001C0
#define CONFIG_ICACHE_BLOCK         0x00000020
#define CONFIG_DCACHE_LINE          0x00000010
#define CONFIG_CACHE2_CU            0x00000008
#define CONFIG_KSEG0_COHERENCY      0x00000007

#define CONFIG_MASTER_CHECKER_SHIFT         31
#define CONFIG_SYS_CLOCK_SHIFT              28
#define CONFIG_PATTERN_SHIFT                24
#define CONFIG_CACHE2_BLOCKSIZE_SHIFT       22
#define CONFIG_CACHE2_SPLIT_SHIFT           21
#define CONFIG_CACHE2_WIDTH_SHIFT           20
#define CONFIG_SYSPORT_WIDTH_SHIFT          18
#define CONFIG_CACHE2_PRESENT_SHIFT         17
#define CONFIG_DIRTY_SHARED_SHIFT           16
#define CONFIG_BIG_INDIAN_SHIFT             15
#define CONFIG_ECC_MODE_SHIFT               14
#define CONFIG_BLOCK_ORDER_SHIFT            13
#define CONFIG_ICACHE_SIZE_SHIFT            9
#define CONFIG_DCACHE_SIZE_SHIFT            6
#define CONFIG_ICACHE_BLOCK_SHIFT           5
#define CONFIG_DCACHE_LINE_SHIFT            4
#define CONFIG_CACHE2_CU_SHIFT              3
#define CONFIG_KSEG0_COHERENCY_SHIFT        0

/*
 * CP0 PRID Register
 */
#define PRID_IMP                    0x0000FF00
#define PRID_REV                    0x000000FF

#define PRID_IMP_SHIFT              8
#define PRID_REV_SHIFT              0

/*
 * CP0 Watch Hi/Lo Registers
 */
#define WATCH_PADDR0                0xFFFFFFF8
#define WATCH_PADDR1                0x0000000F
#define WATCH_READ                  0x00000002
#define WATCH_WRITE                 0x00000001

#define WATCH_PADDR0_SHIFT          3
#define WATCH_PADDR1_SHIFT          0
#define WATCH_READ_SHIFT            1
#define WATCH_WRITE_SHIFT           0

/*
 * CP0 ECC Register
 */
#define ECC_MASK                    0x000000FF

/*
 * CP0 Cache Error Register
 */
#define CACHE_ERR_ER                0x80000000
#define CACHE_ERR_EC                0x40000000
#define CACHE_ERR_ED                0x20000000
#define CACHE_ERR_ET                0x10000000
#define CACHE_ERR_ES                0x08000000
#define CACHE_ERR_EE                0x04000000
#define CACHE_ERR_EB                0x02000000
#define CACHE_ERR_EI                0x01000000
#define CACHE_ERR_SLDX              0x00FFFFF8
#define CACHE_ERR_PLDX              0x00000007

#define CACHE_ERR_ER_SHIFT          31
#define CACHE_ERR_EC_SHIFT          30
#define CACHE_ERR_ED_SHIFT          29
#define CACHE_ERR_ET_SHIFT          28
#define CACHE_ERR_ES_SHIFT          27
#define CACHE_ERR_EE_SHIFT          26
#define CACHE_ERR_EB_SHIFT          25
#define CACHE_ERR_EI_SHIFT          24
#define CACHE_ERR_SLDX_SHIFT         3
#define CACHE_ERR_PLDX_SHIFT         0

/*
 * CP0 TagLo and TagHi
 */
#define TAGLO_PTAGLO                0xFFFFFF00
#define TAGLO_PSTATE                0x000000C0
#define TAGLO_PARITY                0x00000001
#define TAGLO_FBIT                  0x00000002

#define TAGLO_PTAGLO_SHIFT          8
#define TAGLO_PSTATE_SHIFT          6
#define TAGLO_PARITY_SHIFT          0
#define TAGLO_FBIT_SHIFT            1

#define VECTOR_OFFSET      0x200     /* Offset between KSeg0 and KSeg1
					vectors (BEV 0 and 1) */
#define VECTOR_MASK        0x8200    /* Mask with ra to allow relocatable
					vectors */
#define STACK_PARAMS       0x20      /* Buffer space on the stack when calling
					C code from assembler */

#ifndef ASMINCLUDE

struct orion_config {
    unsigned cm : 1;
    unsigned ec : 3;
    unsigned ep : 4;
    unsigned sb : 2;
    unsigned ss : 1;
    unsigned sw : 1;
    unsigned ew : 2;
    unsigned sc : 1;
    unsigned sm : 1;
    unsigned be : 1;
    unsigned em : 1;
    unsigned eb : 1;
    unsigned dummy : 1;
    unsigned ic : 3;
    unsigned dc : 3;
    unsigned ib : 1;
    unsigned db : 1;
    unsigned cu : 1;
    unsigned k0 : 3;
};

struct orion_sreg {
    unsigned cu : 4;
    unsigned rp : 1;
    unsigned fr : 1;
    unsigned re : 1;
    unsigned d1 : 2;
    unsigned bev : 1;
    unsigned ts : 1;
    unsigned sr : 1;
    unsigned d2 : 1;
    unsigned ch : 1;
    unsigned ce : 1;
    unsigned de : 1;
    unsigned im : 8;
    unsigned kx : 1;
    unsigned sx : 1;
    unsigned ux : 1;
    unsigned ksu : 2;
    unsigned erl : 1;
    unsigned exl : 1;
    unsigned ie : 1;
};

/*
 * Note : These structures should not be changed without
 * altering the assembly language routines for TLB manipulation
 */

struct pmask_t {
    unsigned dummy0 : 7;
    unsigned mask : 12;
    unsigned dummy1 : 13;
};

struct entryhi_t {
    unsigned vpn2 : 19;
    unsigned dummy : 5;
    unsigned asid : 8;
};

struct entrylo_t {
    unsigned dummy : 2;
    unsigned pfn : 24;
    unsigned c : 3;
    unsigned d : 1;
    unsigned v : 1;
    unsigned g : 1;
};

struct tlb_t {
    struct pmask_t pmask;
    struct entryhi_t hi;
    struct entrylo_t lo0;
    struct entrylo_t lo1;
};

/*
 * Cache Error register description
 */
union cerr_t {

    unsigned int cerr;

    struct {
	unsigned er : 1;
	unsigned ec : 1;
	unsigned ed : 1;
	unsigned et : 1;
	unsigned es : 1;
	unsigned ee : 1;
	unsigned eb : 1;
	unsigned dummy : 25;
    } f;
};

/*
 * typedef for the exception info array
 */
typedef struct r4k_exception_t_ {
    char *str;
    void (*func)();
} r4k_exception_t;

#endif /* ASMINCLUDE */

/* End of Module */


