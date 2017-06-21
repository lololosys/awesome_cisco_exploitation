/* $Id: dclasic.h,v 3.3 1996/01/17 23:31:36 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src-4k-rsp/dclasic.h,v $
 *------------------------------------------------------------------
 * dclasic.h
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * DCL Register definition header file inlcuding register masks.
 *------------------------------------------------------------------
 * $Log: dclasic.h,v $
 * Revision 3.3  1996/01/17  23:31:36  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:46:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DCLASIC_H__
#define __DCLASIC_H__

#ifndef ASMINCLUDE

#define PAD unsigned char

typedef struct {
    /* Physical Base Address is 0x1E840000 */
    PAD pad0[4];
    volatile unsigned short ct0;         /* counter timer 0 register */
    PAD pad1[6];
    volatile unsigned short ct1;         /* counter timer 1 register */
    PAD pad2[6];
    volatile unsigned short ct2;         /* counter timer 2 register */
    PAD pad3[6];
    volatile unsigned char  ccr;         /* counter control register */
    PAD pad4[7];
    volatile unsigned char int_mask;     /* interrupt mask register */
    PAD pad5[7];
    volatile unsigned char int_vect;     /* interrupt vector register */
    PAD pad6[7];
    volatile unsigned char int_eoi;      /* end of interrupt register */
    PAD pad7[7];
    volatile unsigned char int_vmi;      /* valid masked interrupt register */
    PAD pad8[7];
    volatile unsigned char clr_tint;     /* clear timer interrupt register */
    PAD pad9[7];
    volatile unsigned short asic_reg;    /* ASIC Register */
    PAD pad10[6];
    volatile unsigned char dram_config;  /* dram config. register */
    PAD pad11[7];
    volatile unsigned char bank_config;  /* dram bank config. register */
    PAD pad12[7];
    volatile unsigned short dram_exc1;   /* dram exception Reg1 */
    PAD pad13[6];
    volatile unsigned short dram_exc2;   /* dram exception Reg2 */
    PAD pad14[6];
    volatile unsigned short memd_exc1;   /* memd exception Reg1 */
    PAD pad15[6];
    volatile unsigned short memd_exc2;   /* memd exception Reg2 */
    PAD pad16[6];
    volatile unsigned short io_exc1;     /* i/o exception Reg1 */
    PAD pad17[6];
    volatile unsigned short io_exc2;     /* i/o exception Reg2 */
    PAD pad18[6];
    volatile unsigned short watch1;      /* watch Reg1 */
    PAD pad19[6];
    volatile unsigned short watch2;      /* watch Reg2 */
    PAD pad20[6];
    volatile unsigned short watch_mask1; /* watch mask Reg 1 */
    PAD pad21[6];
    volatile unsigned short watch_mask2; /* watch mask Reg 2 */
    PAD pad22[6];
    volatile unsigned char watch_ctl ;   /* watch control Reg */
    PAD pad23[7];
    volatile unsigned char watch_status; /* watch status Reg */
    PAD pad24[7];
    volatile unsigned char trap_status;  /* trap status Reg */
} DCL_STRUCT;

#undef PAD

/*
 * Define a pointer to the DCL ASIC
 */
#define DCL_ASIC      ((DCL_STRUCT *)(KSEG1_ADRSPC_DCL_ASIC))


#else       /* ASMINCLUDE */

#define DCL_REG_ADDRESS(a)    (KSEG1_ADRSPC_DCL_ASIC + a)

/*
 * IRSP DCL register addresses for assembly language code 
 */
#define DCL_CT0_REG	DCL_REG_ADDRESS(0x004)		/* ct0 */
#define DCL_CT1_REG	DCL_REG_ADDRESS(0x00C)		/* ct1 */
#define DCL_CT2_REG	DCL_REG_ADDRESS(0x014)		/* ct2 */
#define DCL_CCR_REG	DCL_REG_ADDRESS(0x01C)		/* ccr */
#define DCL_IMASK_REG	DCL_REG_ADDRESS(0x024)		/* int_mask */
#define DCL_IVECT_REG	DCL_REG_ADDRESS(0x02C)		/* int_vect */
#define DCL_IEOI_REG	DCL_REG_ADDRESS(0x034)		/* int_eoi */
#define DCL_IVMI_REG	DCL_REG_ADDRESS(0x03C)		/* int_vmi */
#define DCL_CTI_REG	DCL_REG_ADDRESS(0x044)		/* clr_tint */
#define DCL_ASIC_REG	DCL_REG_ADDRESS(0x04C)		/* asic_reg */
#define DCL_DCONF_REG	DCL_REG_ADDRESS(0x054)		/* dram_config */
#define DCL_BCONF_REG	DCL_REG_ADDRESS(0x05C)		/* bank_config */
#define DCL_DEXC1_REG	DCL_REG_ADDRESS(0x064)		/* dram_exc1 */
#define DCL_DEXC2_REG	DCL_REG_ADDRESS(0x06C)		/* dram_exc2 */
#define DCL_MEXC1_REG	DCL_REG_ADDRESS(0x074)		/* memd_exc1 */
#define DCL_MEXC2_REG	DCL_REG_ADDRESS(0x07C)		/* memd_exc2 */
#define DCL_IEXC1_REG	DCL_REG_ADDRESS(0x084)		/* io_exc1 */
#define DCL_IEXC2_REG	DCL_REG_ADDRESS(0x08C)		/* io_exc2 */
#define DCL_WAT1_REG	DCL_REG_ADDRESS(0x094)		/* watch1 */
#define DCL_WAT2_REG	DCL_REG_ADDRESS(0x09C)		/* watch2 */
#define DCL_WMSK1_REG	DCL_REG_ADDRESS(0x0A4)		/* watch_mask1 */
#define DCL_WMSK2_REG	DCL_REG_ADDRESS(0x0AC)		/* watch_mask2 */
#define DCL_WCTL_REG	DCL_REG_ADDRESS(0x0B4)		/* watch_ctl */
#define DCL_WSTS_REG	DCL_REG_ADDRESS(0x0BC)		/* watch_status */
#define DCL_TSTS_REG	DCL_REG_ADDRESS(0x0C4)		/* trap_status */

#endif      /* ASMINCLUDE */


/*
 * DCL ASIC register address offsets for assembly language code
 */
#define DCL_REG_CT0_OFS             0x004
#define DCL_REG_CT1_OFS             0x00C
#define DCL_REG_CT2_OFS             0x014
#define DCL_REG_CCR_OFS             0x01C
#define DCL_REG_INT_MASK_OFS        0x024
#define DCL_REG_INT_VECT_OFS        0x02C
#define DCL_REG_INT_EOI_OFS         0x034
#define DCL_REG_INT_VMI_OFS         0x03C
#define DCL_REG_CLR_TINT_OFS        0x04C
#define DCL_REG_DRAM_CONFIG_OFS     0x054
#define DCL_REG_BANK_CONFIG_OFS     0x05C
#define DCL_REG1_DRAM_EXCEPT_OFS    0x064
#define DCL_REG2_DRAM_EXCEPT_OFS    0x06C
#define DCL_REG1_MEMD_EXCEPT_OFS    0x074
#define DCL_REG2_MEMD_EXCEPT_OFS    0x07C
#define DCL_REG1_IO_EXCEPT_OFS      0x084
#define DCL_REG2_IO_EXCEPT_OFS      0x08C
#define DCL_REG1_WATCH_OFS          0x094
#define DCL_REG2_WATCH_OFS          0x09C
#define DCL_REG1_WATCH_MASK_OFS     0x0A4
#define DCL_REG2_WATCH_MASK_OFS     0x0AC
#define DCL_REG_WATCH_CTL_OFS       0x0B4
#define DCL_REG_WATCH_STATUS_OFS    0x0BC
#define DCL_REG_TRAP_STATUS_OFS     0x0C4


/*
** Defines for ASIC Register
*/
#define ASIC_REV                    0xF000
#define ASIC_REV_BIT_OFS            12
#define DRAM_WRITEBUFF_EN           0x800
#define MEMD_WRITEBUFF_EN           0x400
#define IO_WRITEBUFF_EN             0x200
#define DRAM_RDARNDWR_EN            0x100
#define MEMD_RDARNDWR_EN            0x80
#ifdef RSP2
#define BOB_MODE                    0x40
#endif

/*
** Defines for the counter control register (ccr) above.
*/
#define CCR_CT2_EN  0x10    /* write high to enable counter 2  (r/w) */
#define CCR_CT1_EN  0x20    /* write high to enable counter 1  (r/w) */
#define CCR_CT0_EN  0x40    /* write high to enable watchdog   (r/w) */

/*
** Defines for the interrupt mask register and the
** end of interrupt register above.
** The valid masked interrupt register is inverse logic
** (active low), and thus requires the ones compliment
** of the define (ie: ~INT_LEV1).
*/
#define IMR_MASK_ALL   0xff    /* mask all interrupts */
#define IMR_MASK_NONE  0x00    /* unmask all interrupts */
#define INT_LEV0       0x01    /* level 0 interrupts (r/w) */
#define INT_LEV1       0x02    /* level 1 interrupts (r/w) */
#define INT_LEV2       0x04    /* level 2 interrupts (r/w) */
#define INT_LEV3       0x08    /* level 3 interrupts (r/w) */
#define INT_LEV4       0x10    /* level 4 interrupts (r/w) */
#define INT_LEV5       0x20    /* level 5 interrupts (r/w) */
#define INT_LEV6       0x40    /* level 6 interrupts (r/w) */
#define INT_LEV7       0x80    /* level 7 interrupts (r/w) */
#define INT_LEV(i) (0x01 << i) /* the shift method for above */

/*
** Defines for the interrupt vector register above.
*/
#define IVR_NO_INT     0x00    /* no interrupt pending */
#define IVR_LEV_0      0x04    /* level 0 interrupt pending */
#define IVR_LEV_1      0x08    /* level 1 interrupt pending */
#define IVR_LEV_2      0x0c    /* level 2 interrupt pending */
#define IVR_LEV_3      0x10    /* level 3 interrupt pending */
#define IVR_LEV_4      0x14    /* level 4 interrupt pending */
#define IVR_LEV_5      0x18    /* level 5 interrupt pending */
#define IVR_LEV_6      0x1c    /* level 6 interrupt pending */
#define IVR_LEV_7      0x20    /* level 7 interrupt pending */


/*
** Defines for the trap status register.
*/

#define WATCH_TRAP	0x10	/* Watch register */
#define IO_TRAP		0x20	/* Bus Exception from IO */
#define MEMD_TRAP	0x40	/* Bus Exception from MEMD */

/*
 * DRAM config register 
 */
#define nBanks            0x80  /* 0 - 1 bank, 1 - 2 banks */

/* 
 *Bank Config Register
 */
#define BK1_64M         0x10    /* bank 1, 64 M */
#define BK1_32M         0x20    /*         32 M */
#define BK1_16M         0x40    /*         16 M */
#define BK1_8M          0x80    /*          8 M */

#define BK2_64M         0x1     /* bank 2, 64 M */
#define BK2_32M         0x2     /*         32 M */
#define BK2_16M         0x4     /*         16 M */
#define BK2_8M          0x8     /*          8 M */

/* end of module */

#endif /* __DCLASIC_H__ */















