/* $Id: dec_ppb.h,v 3.4.44.2 1996/06/04 00:31:11 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/dec_ppb.h,v $
 *------------------------------------------------------------------
 * dec_ppb.h - Definitions for DEC21050 PCI-PCI bridge
 *
 * March 15 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dec_ppb.h,v $
 * Revision 3.4.44.2  1996/06/04  00:31:11  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.4.44.1  1996/04/02  22:55:10  mbeesley
 * CSCdi53300:  PA power-on sequence can leave bus interface off
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/19  20:24:12  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.3  1995/12/01  01:59:56  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:53:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Defines
 */

#define REG16_CFG_RESET      0x00000100 /* to reset the 21050. cleared 
					   when done */

/*
 * Define max init loop
 */
#define DEC21150_MAX_INIT_LOOP		200

/*
 * Typedef of the DEC 21150 registers
 */
typedef union {
    struct {
	ushort devid;
	ushort vid;
    } s;
    uint i;
} reg0_t;

#define reg_devid s.devid
#define reg_vid   s.vid

typedef union {
    struct {
	ushort sts;
	ushort cmd;
    } s;
    uint i;
} reg1_t;

typedef union {
    struct {
	uint bcc :8;
	uint scc :8;
	uint pif :8;
	uint revid :8;
    } s;
    uint i;
} reg2_t;

typedef union {
    struct {
	uint rsvd :8;
	uint hdrtype :8;
	uint pmlt :8;
	uint cachesz :8;
    } s;
    uint i;
} reg3_t;

typedef union {
    struct {
	uint smlt :8;
	uint subbus :8;
	uint secbus :8;
	uint pribus :8;
    } s;
    uint i;
} reg6_t;

#define reg_subbus s.subbus
#define reg_secbus s.secbus
#define reg_pribus s.pribus

typedef union {
    struct {
	uint sec_sts :16;
	uint io_limit :8;
	uint io_base :8;
    } s;
    uint i;
} reg7_t;

#define reg_sec_sts  s.sec_sts
#define reg_io_limit s.io_limit
#define reg_io_base  s.io_base
#define reg7_adjust(x) (((x) & 0xF000) >> 8)

typedef union {
    struct {
	uint mem_lim_addr :16;
	uint mem_base_addr :16;
    } s;
    uint i;
} reg8_t;

typedef union {
    struct {
	uint prefm_lim_addr :16;
	uint prefm_base_addr :16;
    } s;
    uint i;
} reg9_t;

typedef union {
    struct {
	uint bcntl :16;
	uint int_pin :8;
	uint rsvd :8;
    } s;
    uint i;
} reg15_t;
 
typedef union {
    struct {
	uint serr :8;
	uint blcntr :8;
	uint dcntl :8;
	uint ccntl :8;
    } err_diag_ccl;
    uint i;
} reg16_t;
 
typedef union {
    struct {
	uint rsvd :16;
	uint stmr :8;
	uint ptmr :8;
    } s;
    uint i;
} reg17_t;

typedef struct dec21050_cfg_space_ {
    volatile reg0_t  reg0;
    volatile reg1_t  reg1;
    volatile reg2_t  reg2;
    volatile reg3_t  reg3;
    volatile uint    rsvd1[2];               
    volatile reg6_t  reg6;
    volatile reg7_t  reg7;
    volatile reg8_t  reg8;
    volatile reg9_t  reg9;
    volatile uint    rsvd2[5];
    volatile reg15_t reg15;
    volatile reg16_t reg16;
    volatile reg17_t reg17;;
    volatile uint s_write_attempt_ctr;
    volatile uint p_write_attempt_ctr;
} dec21050_cfg_space_t;

/*
 * Defines to offsets to each of the registers
 */
#define REG0_OFFSET	0x000
#define REG1_OFFSET	0x004
#define REG2_OFFSET	0x008
#define REG3_OFFSET	0x00C
#define REG6_OFFSET	0x018
#define REG7_OFFSET	0x01C
#define REG8_OFFSET	0x020
#define REG9_OFFSET	0x024
#define REG15_OFFSET	0x03C
#define REG16_OFFSET	0x040
#define REG17_OFFSET	0x044

/*
 * Prototypes
 */
extern boolean 	dec21050_bridge_config(pci_bridge_config_t *, boolean);
extern void   	dec21050_print_regs(pci_bridge_config_t *, uint);

/* end of file */
