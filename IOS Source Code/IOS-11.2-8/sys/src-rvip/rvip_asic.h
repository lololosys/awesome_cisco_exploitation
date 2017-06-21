/* $Id: rvip_asic.h,v 3.4.6.1 1996/03/21 23:50:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_asic.h,v $
 *------------------------------------------------------------------
 * rvip_asic.h - RVIP ASIC definitions
 *
 * 12/94, David Getchell
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_asic.h,v $
 * Revision 3.4.6.1  1996/03/21  23:50:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/03/18  23:35:52  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.4  1996/03/06  21:24:48  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.3  1996/03/06  01:50:20  jwelder
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.2  1995/11/17  18:50:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RVIP_ASIC_H__
#define __RVIP_ASIC_H__

/* 
 * TBD - we could break some of this out into a platform independent
 * nevasic file
*/

#ifndef ASMINCLUDE

#define PAD unsigned char

typedef struct nev_asic_{
    volatile unsigned short ct0;         /* counter timer 0 register */
    PAD pad1[6];
    volatile unsigned short ct1;         /* counter timer 1 register */
    PAD pad2[6];
    volatile unsigned short ct2;         /* counter timer 2 register */
    PAD pad3[6];
    volatile unsigned char  ccr;         /* counter control register */
    PAD pad4[231];
    volatile unsigned char int_mask;     /* interrupt mask register */
    PAD pad5[7];
    volatile unsigned char int_vect;     /* interrupt vector register */
    PAD pad6[7];
    volatile unsigned char int_eoi;      /* end of interrupt register */
    PAD pad7[7];
    volatile unsigned char int_vmi;      /* valid masked interrupt register */
    PAD pad8[231];
    volatile unsigned short cfga;        /* control reg (A) */
    PAD pad9[6];
    volatile unsigned short cfgb;        /* control reg (B) */
    PAD pad10[6];
    volatile unsigned short stata;       /* status reg (A) */
    PAD pad11[6];
    volatile unsigned short statb;       /* status reg (B) */
    PAD pad12[230];
    volatile unsigned short csa;         /* conf control/status A reg */
    PAD pad13[6];
    volatile unsigned char csb;          /* conf control/status B reg */
    PAD pad14[7];
    volatile unsigned char csc;          /* conf control/status C reg */
    PAD pad15[239];
    volatile unsigned char cookie;       /* cookie register */
    PAD pad16[7];
    volatile unsigned short static_in;   /* static input reg */
    PAD pad17[6];
    volatile unsigned char asic_reg;     /* ASIC register */
    PAD pad18[239];
    volatile unsigned char cti;  	 /* clear timer interrupt reg */
} nev_asic;

#undef PAD

/*
 * Define a pointer to the ASIC
 */
#define IO_ASIC       ((nev_asic *)(ADRSPC_NEV_ASIC))
#define IO_ASIC_KSEG1 ((nev_asic *)(PHY_TO_KSEG1(PHY_ADRSPC_NEV_ASIC)))

#else                 /* ASMINCLUDE */

#define IO_REG_ADDRESS(a)	(ADRSPC_NEV_ASIC + a)

/*
 * Nevada ASIC register addresses for assembly language code
 */
#define IO_CT0_REG		IO_REG_ADDRESS(0x000)	/* ct0  	*/
#define IO_WATCHDOG_REG		IO_REG_ADDRESS(0x000)	/* ct0		*/
#define IO_CT1_REG		IO_REG_ADDRESS(0x008)	/* ct1		*/
#define IO_CT2_REG		IO_REG_ADDRESS(0x010)	/* ct2		*/
#define IO_CCR_REG		IO_REG_ADDRESS(0x018)	/* ccr		*/
#define IO_IMASK_REG		IO_REG_ADDRESS(0x100)	/* int_mask	*/
#define IO_IVECT_REG		IO_REG_ADDRESS(0x108)	/* int_vect	*/
#define IO_IEOI_REG		IO_REG_ADDRESS(0x110)	/* int_eoi	*/
#define IO_IVMI_REG		IO_REG_ADDRESS(0x118)	/* int_vmi	*/
#define IO_CFGA_REG		IO_REG_ADDRESS(0x200)	/* ctl_a	*/
#define IO_CFGB_REG	        IO_REG_ADDRESS(0x208)	/* ctl_b	*/
#define IO_STATA_REG     	IO_REG_ADDRESS(0x210)	/* stat_a	*/
#define IO_STATB_REG		IO_REG_ADDRESS(0x218)	/* stat_b	*/
#define IO_CSA_REG      	IO_REG_ADDRESS(0x300)	/* csa		*/
#define IO_CSB_REG	        IO_REG_ADDRESS(0x308)	/* csb		*/
#define IO_CSC_REG	        IO_REG_ADDRESS(0x310)	/* csc		*/
#define IO_COOKIE_REG		IO_REG_ADDRESS(0x400)	/* cookie	*/
#define IO_STATIC_IN_REG	IO_REG_ADDRESS(0x408)	/* static_in	*/
#define IO_ASIC_REG		IO_REG_ADDRESS(0x410)	/* asic_reg	*/
#define IO_CTI_REG		IO_REG_ADDRESS(0x500)	/* cti		*/

#endif  /* ASMINCLUDE */

/*
 * Defines for the counter control register (ccr) above.
 */
#define CCR_CT2_EN  0x10    /* write high to enable counter 2  (r/w) */
#define CCR_CT1_EN  0x20    /* write high to enable counter 1  (r/w) */
#define CCR_CT0_EN  0x40    /* write high to enable counter 0  (r/w) */

/*
 * Literals for the interrupt mask register IO_ASIC->int_mask and the
 * end of interrupt register IO_ASIC->int_eoi
 * The valid masked interrupt register IO_ASIC->int_vmi is inverse logic
 * (active low), and thus requires the one's complement
 * of the literal (ie: ~INT_LEV1).
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
 * Defines for the interrupt vector register (IO_ASIC->int_vect).
 * that contains the value of the highest priority interrupt that caused
 * the interrupt
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
 * RVIP nevada interrupt priority assignments
 * Do no remove if unused - of documentation value
 */
#define NEV_PA_STALL_INT 0x40   /* PA Present/Stall interrupt */
#define NEV_PA_STALL_VEC 0x40   /* PA Present/Stall interrupt vector */
#define NEV_TIMER_INT    0x20   /* External Timer */
#define NEV_TIMER_VEC    0x18   /* External Timer vector */
#define NEV_DRAM_ERR_INT 0x10   /* DRAM Write error */
#define NEV_DRAM_ERR_VEC 0x14   /* DRAM Write error vector*/
#define NEV_IO_ERR_INT   0x08   /* Write error */
#define NEV_IO_ERR_VEC   0x10   /* Write error vector*/
#define NEV_PMAC_ERR_INT 0x02   /* PMAC Error interrupt */
#define NEV_PMAC_ERR_VEC 0x08   /* PMAC Error interrupt vector */
#define NEV_PA_NMI_INT   0x01   /* PA NMI */
#define NEV_PA_NMI_VEC   0x04   /* PA NMI vector*/
#define NEV_INT_MASK     0x7B   /* Indication of valid interrupts */

/*
 * Definitions for Control Register A
 * (IO_ASIC->cfga) a 16-bit read/write register
 */
#define CFGA_FLEX_DCLK1        0x8000 /* clock to configure flex parts */
#define CFGA_FLEX_DCLK0        0x4000 /* clock to configure flex parts */
#define CFGA_FLEX_PMACPM_DCLK  0x2000 /* PMACPM flex clock */
#define CFGA_FLEX_PMACIO_DCLK  0x1000 /* PMACIO flex clock */
#define CFGA_EEPROM_CLOCK_PA0  0x0800 /* EEPROM chip select - PA0 */
#define CFGA_EEPROM_CLOCK_PA1  0x0400 /* EEPROM chip select - PA1 */
#define CFGA_SW_HW_INT         0x0100 /* Software generated HW interrupt */
#define CFGA_EEPROM_DATA_PA0   0x0080 /* unused  - PA0 */
#define CFGA_EEPROM_SELECT_PA0 0x0010 /* EEPROM chip select - PA0 */
#define CFGA_EEPROM_DATA_PA1   0x0008 /* unused  - PA1 */
#define CFGA_EEPROM_SELECT_PA1 0x0001 /* EEPROM chip select - PA1 */

/*
 * Definitions for Control Register B
 * (IO_ASIC->cfgb) a 16-bit read/write register
 */

#define CFGB_RESET_PA0         0x8000 /* Reset PA0 - active low */
#define CFGB_RESET_PA1         0x4000 /* Reset PA1 - active low */
#define CFGB_POWERON_PA1       0x0100 /* Power on PA1 */
#define CFGB_POWERON_PA0       0x0200 /* Power on PA0 */
#define CFGB_RESET_FLEX        0x2000 /* Reset Flex parts */
#define CFGB_ENBL_LED_PA1      0x0040 /* Enable LED PA1 */
#define CFGB_ENBL_LED_PA0      0x0020 /* Enable LED PA0 */
#define CFGB_PROM_MASK         0x0018 /* Boot Rom size */
#define CFGB_DRAM_CFG_MASK     0x0006 /* DRAM configuration */
#define CFGB_DRAM_VAL_MASK_128 0x0006 /* DRAM configuration value for 128 MB */
#define CFGB_DRAM_VAL_MASK_32  0x0004 /* DRAM configuration value for 32 MB */
#define CFGB_DRAM_VAL_MASK_8   0x0002 /* DRAM configuration value for 8 MB */
#define CFGB_DRAM_VAL_MASK_2   0x0000 /* DRAM configuration value for 2 MB */
#define CFGB_BOOT              0x0001 /* Change RENO from kseg to kuseg */

/*
 * Definitions for Status Register A
 * (IO_ASIC->stata) a 16-bit read only register
 */
#define STATA_PRSNT_PA0        0x0008 /* PA0 is present */
#define STATA_PRSNT_PA1        0x0004 /* PA1 is present */
#define STATA_STALL_PA0        0x0002 /* Stall signal - PA0 */
#define STATA_STALL_PA1        0x0001 /* Stall signal - PA1 */

/*
 * Definitions for Status Register B
 * (IO_ASIC->statb) a 16-bit read only register
 */
#define STATB_EEPROM_WR_EN     0x8000 /* EEPROM write enable */
#define STATB_BOARD_ID_MASK    0x0F00 /* Board revision level */
#define STATB_IO_WR_BERR       0x0010 /* I/O write bus error */       

/*
 * Definitions for Config Control Status Register A
 * (IO_ASIC->csa) a 16-bit read or write register (configured as
 * a status register)
 */
#define CSA_FLX_PCI0_DONE     0x4000 /* */
#define CSA_FLX_PCI1_DONE     0x2000 /* */
#define CSA_FLX_PMAC_PM_DONE  0x1000 /* PMAC PM FPGA */
#define CSA_FLX_PMAC_IO_DONE  0x0800 /* PMAC IO FPGA */
#define CSA_FLX_CH_DONE       0x0400 /* Channel FPGA */
#define CSA_FLX_CB_DATA_DONE  0x0200 /* CBUS Data FPGA */
#define CSA_FLX_CB_CTRL_DONE  0x0100 /* CBUS Control FPGA */
#define CSA_FLX_PCI0_DI       0x0040 /* EEPROM data input */
#define CSA_FLX_PCI1_DI       0x0020 /* EEPROM data input */
#define CSA_FLX_PMAC_PM_STAT  0x0010 /* PMAC PM FPGA - Active low */
#define CSA_FLX_PMAC_IO_STAT  0x0008 /* PMAC IO FPGA - Active low */
#define CSA_FLX_CH_STAT       0x0004 /* Channel FPGA - Active low */
#define CSA_FLX_CB_DATA_STAT  0x0002 /* CBUS Data FPGA - Active low */
#define CSA_FLX_CB_CTRL_STAT  0x0001 /* CBUS Control FPGA - Active low */

/*
 * Definitions for Configurable Control Status Register B
 * (IO_ASIC->csb) a 8-bit read or write register (configured as a 
 * control register)
 */
#define CSB_CLR_DRAM_WR_BERR  0x02   /* Clear a DRAM write bus error */
#define CSB_CLR_IO_WR_BERR    0x01   /* Clear an I/O write bus error */

/*
 * Definitions for Configurable Control Status Register C
 * (IO_ASIC->csc) a 8-bit read or write register (configured as an
 * control register - output)
 */
#define CSC_FLEX_CONFIG      0x80 /* All FLEX FPGAs - Active low */
#define CSC_FLX_PCI0_D0      0x40 /* EEPROM data output */
#define CSC_FLX_PCI1_D0      0x20 /* EEPROM data output */
#define CSC_FLX_PMAC_PM_D0   0x10 /* PMAC PM FPGA */
#define CSC_FLX_PMAC_IO_D0   0x08 /* PMAC IO FPGA */
#define CSC_FLX_CH_D0        0x04 /* Channel FPGA */
#define CSC_FLX_CB_DATA_D0   0x02 /* CBUS Data FPGA */
#define CSC_FLX_CB_CNTRL_D0  0x01 /* CBUS Control FPGA */

/*
 * Definitions for Serial EEPROM control register
 * (IO_ASIC->cookie) a 8-bit read/write register.
 * Note: before reading data from X2444_DO, X2444_DI must be set
 * to a 1.
 */
#define X2444_CE 0x01           /* CE: Chip enable, also seen as CS */
#define X2444_SK 0x02           /* SK: Serial clock in */
#define X2444_DI 0x04           /* DI: Serial data in  */
#define X2444_DO 0x08           /* DO: Serial data out */

/*
 * definitions for ASIC Register
 * (IO_ASIC->asic) a 8-bit read/write register
 */
#define ASIC_REV_MASK     0xF0     /* Mask for ASIC revision */
#define ASIC_CONT_C       0x04     /* Control (high) or status (low) reg C */
#define ASIC_CONT_B       0x02     /* Control (high) or status (low) reg B */
#define ASIC_CONT_A       0x01     /* Control (high) or status (low) reg A */

#ifndef ASMINCLUDE

typedef struct pmac_config_ {
    volatile uint reserved :8;
    volatile uint bus      :8;
    volatile uint device   :5;
    volatile uint function :3;
    volatile uint reg      :6;
    volatile uint zeros    :2;
} pmac_config_t;

typedef struct pmac_intrpt_mask_ {
    volatile uint reserved :12;

    volatile uint PA0_NMI  :2;
    volatile uint PA0_int  :8;
    volatile uint PA1_NMI  :2;
    volatile uint PA1_int  :8;
} pmac_intrpt_mask_t;

typedef struct pmac_intrpt_status_ {
    volatile uint reserved :5;
    volatile uint errors   :7;
    volatile uint PA0_NMI  :2;
    volatile uint PA0_int  :8;
    volatile uint PA1_NMI  :2;
    volatile uint PA1_int  :8;
} pmac_intrpt_status_t;

/*
 * Definitions for the PCI ASIC Interrupt Mask Register
 */
typedef struct pci_asic_ {
    pmac_config_t         pmac_config_reg;
    pmac_intrpt_mask_t    pmac_intrpt_mask_reg;
    pmac_intrpt_status_t  pmac_intrpt_status_reg;
} pci_asic_t;

/* Defines for pmac_intrpt_status_reg and pmac_mask_reg */
#define PMAC_ERROR_MASK    0x07f00000
#define PMAC_ERROR_SHFT            20

#define PMAC_PA0_NMI_MASK  0x000C0000
#define PMAC_PA0_NMI_SHFT          18 
#define PMAC_PA0_PKT_MASK  0x0003fc00
#define PMAC_PA0_PKT_SHFT          10
#define PMAC_PA1_NMI_MASK  0x00000300
#define PMAC_PA1_NMI_SHFT           8 
#define PMAC_PA1_PKT_MASK  0x000000ff
#define PMAC_PA1_PKT_SHFT           0
#define NMI_PER_PA                  2

/* Defines for the 7 bit error field in the status register */
#define PMAC_IO_PARITY_ERROR  0x40   /* I/O Parity Error */
#define PMAC_PM_FIFO_ERROR    0x10   /* Packet Memory FIFO Error */
#define PMAC_PM_CTRLR_ERROR   0x08   /* Packet Memory Controller Error */
#define PMAC_PCI_BUS_TIMEOUT  0x04   /* Primary PCI Bus Timeout */
#define PMAC_PCI_PARITY_ERROR 0x02   /* Primary PCI Parity Error */
#define PMAC_PCI_SYSTEM_ERROR 0x01   /* Primary PCI System Error */
 
/*
 * Define a pointer to the PCI ASIC
 */
#define PCI_ASIC       ((pci_asic_t *)(ADRSPC_PMAC))

typedef struct dma_reg0_ {
  uint reserved   : 5;
  uint direction  : 1;
  uint int_enable : 1;
  uint enable     : 1;
  uint memd_addr  :24;
}dma_reg0_t;

typedef struct dma_reg1_ {
    uint reserved   :8;
    uint pci_addr   :24;
}dma_reg1_t;

typedef struct dma_reg2_ {
    uint length     :16;
    uint chksm_seed :16;
}dma_reg2_t;

typedef struct dma_reg3_ {
    uint fpga_parity_inv     :1;
    uint reserved            :9;
    uint dma_fifo_err        :1;
    uint dma_cbus_err        :1;
    uint dma_cbus_parity_err :1;
    uint dma_fpga_parity_err :1;
    uint dma_err             :1;
    uint dma_active          :1;
    uint checksum_result     :16;
}dma_reg3_t;

typedef struct dma_reg4_ {
    uint reserved            :10;
    uint cbus_timeout        :1;
    uint cpu_cbus_err        :1;
    uint cpu_cbus_parity_err :1;
    uint cpu_fpga_parity_err :1;
    uint cpu_err             :1;
    uint reserved2           :17;
}dma_reg4_t;

typedef struct dma_reg5_ {
    uint reserved            :25;
    uint cbus_parity_inv     :1;
    uint cbus_parity_valid   :1;
    uint fifo_soft_reset     :1;
    uint err_detect_enable   :1;
    uint reserved2           :3;
}dma_reg5_t;

typedef struct dma_reg6_ {
    uint cbus_lock_request   :32;
}dma_reg6_t;

typedef struct dma_reg7_ {
    uint reserved            :15;
    uint cbus_lock_granted   :1;
    uint reserved2           :16;
}dma_reg7_t;


typedef struct dma_regs_ {
  volatile union {
    dma_reg0_t reg0;          /* 0x00 memd_addr, direction, enables */
    uint int0;
  }w0;
  volatile union {
    dma_reg1_t reg1;
    uint int1;                /* 0x04 pci_addr */
  }w1;
  volatile union {
    dma_reg2_t reg2;
    uint int2;                /* 0x08 length of transfer */
  }w2;
  volatile union {
    dma_reg3_t reg3;
    uint int3;                /* 0x0C checksum result and dma cycle errors */
  }w3;
  volatile union {
    dma_reg4_t reg4;
    uint int4;                /* 0x10 cpu cycle errors */
  }w4;
  volatile union {
    dma_reg5_t reg5;
    uint int5;                /* 0x14 misc. */
  }w5;
  volatile union {
    dma_reg6_t reg6;
    uint int6;                /* 0x18 cbus lock request */
  }w6;
  volatile union {
    dma_reg7_t reg7;
    uint int7;                /* 0x1C cbus lock grant */
  }w7;
} dma_regs_t;

#define dma_reg0 w0.reg0
#define dma_int0 w0.int0
#define dma_reg1 w1.reg1
#define dma_int1 w1.int1
#define dma_length w2.reg2.length
#define dma_reg2 w2.reg2
#define dma_int2 w2.int2
#define dma_reg3 w3.reg3
#define dma_int3 w3.int3
#define dma_reg4 w4.reg4
#define dma_int4 w4.int4
#define dma_reg5 w5.reg5
#define dma_int5 w5.int5
#define dma_reg6 w6.reg6
#define dma_int6 w6.int6
#define dma_reg7 w7.reg7
#define dma_int7 w7.int7

#define DMA_ASIC       ((dma_regs_t *)(ADRSPC_DMA))

#else     /* ASMINCLUDE */

#define DMA_REG_ADDRESS(a)       (ADRSPC_DMA + a)
 
/*
 * Nevada ASIC register addresses for assembly language code
 */
#define DMA_MEMD_REG            DMA_REG_ADDRESS(0x00)  /* memd addr, control */
#define DMA_PCI_REG             DMA_REG_ADDRESS(0x04)  /* pci addr           */
#define DMA_LENGTH_REG          DMA_REG_ADDRESS(0x08)  /* dma length - 1     */
#define DMA_ERR_REG             DMA_REG_ADDRESS(0x0C)  /* dma cycle errors   */
#define DMA_CPU_ERR_REG         DMA_REG_ADDRESS(0x10)  /* cpu cycle errors   */
#define DMA_MISC_REG            DMA_REG_ADDRESS(0x14)  /* misc               */
#define DMA_LOCK_REQ_REG        DMA_REG_ADDRESS(0x18)  /* cbus lock request  */
#define DMA_LOCK_GRNT_REG       DMA_REG_ADDRESS(0x14)  /* cbus lock grant    */
 
#endif  /* ASMINCLUDE */

#endif /* !__RVIP_ASIC_H__ */
 

