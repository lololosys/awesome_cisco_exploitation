/* $Id: svip_pma.h,v 3.1.62.2 1996/07/08 22:54:11 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_pma.h,v $
 *------------------------------------------------------------------
 * SVIP PMA definitions
 *
 * 09/20/95  Srini		 - Its VIPER now..
 * 12/94, David Getchell - hacked from src-4k/nevasic.h
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_pma.h,v $
 * Revision 3.1.62.2  1996/07/08  22:54:11  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:12  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:46  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_PMA_H__
#define __SVIP_PMA_H__

#ifndef ASMINCLUDE 
typedef struct posted_read_reg_ {
    volatile ulong      unused1;
    volatile ulong      pr_reg;
} posted_read_reg_t;


#define PAD unsigned char

typedef struct pma_asic_ {
    volatile u64        err_reg;              /* 64 bit Error Register     */
    PAD pad1[32760];
    volatile u64        config_reg1;          /* 64 bit Config Register 1  */
    PAD pad2[4];
    volatile ulong      intr_mask_reg;        /* Interrupt Mask Reg. (CR2) */
    PAD pad3[500];
    volatile ulong      dev_vendor_id;        /* Device ID/Vendor ID - RO  */
    PAD pad4[4];
    volatile ulong      pci_status_cmd;       /* PCI Status or Command     */
    PAD pad5[4];
    volatile ulong      class_code_rev_id;    /* Class Code or Revision ID */
    PAD pad6[4];
    volatile ulong      pci_latency_time;     /* PCI Latency time          */
    PAD pad7[4];
    volatile ulong      pci_mem_loc;          /* PCI Memory Location       */
    PAD pad8[4];
    volatile ulong      pci_io_loc;           /* PCI IO Location           */
    PAD pad9[84];
    volatile ulong      pci_tmout_cntrs;      /* Time out counters         */
    PAD pad10[32120];
    posted_read_reg_t   posted_rd[16];        /* Posted Read Registers     */
    PAD pad26[65411];
    volatile ulong      pci_config_addr;      /* PCI Config Address        */
    PAD pad27[4];
    volatile ulong      pci_config_data;      /* PCI Config Data           */
} pma_asic;
#undef PAD


typedef struct pma_err_parse_ {
    ulong err_upper_mask;
    ulong err_lower_mask;
    char *err_desc;
} pma_err_parse;

#endif  /* ASMINCLUDE */

/*
 * Define a pointer to the ASIC
 */

#define PMA_ASIC       		((pma_asic *)(ADRSPC_PMA_ASIC))
#define KSEG1_PMA_ASIC          ((pma_asic *)(KSEG1_ADRSPC_PMA_ASIC))
#define PMA_REG_ADDRESS(a) 	(ADRSPC_PMA_ASIC + a)

/*
 * PMA Equiv. ASIC register addresses for assembly language code
 */

#define PMA_ERR_REG_UPPER 	PMA_REG_ADDRESS(0x000)   /* Error Reg Hi  */
#define PMA_ERR_REG_LOWER 	PMA_REG_ADDRESS(0x004)   /* Error Reg Lo */
#define PMA_CONFIG1_REG_UPPER 	PMA_REG_ADDRESS(0x8000)  /* Config Reg1 Hi */
#define PMA_CONFIG1_REG_LOWER 	PMA_REG_ADDRESS(0x8004)  /* Config Reg2 Lo */
#define PMA_INTR_MASK_REG 	PMA_REG_ADDRESS(0x800C)  /* Intrpt Mask Reg */
#define PMA_DEV_VENDOR_ID 	PMA_REG_ADDRESS(0x8204)  /* Dev ID, Vendor ID */
#define PMA_PCI_STAT_CMD 	PMA_REG_ADDRESS(0x820C)  /* PCI Status/Cmd */
#define PMA_CLASS_CODE_REV_ID 	PMA_REG_ADDRESS(0x8214)  /* PCI Class Code/
                                                            Revision ID */
#define PMA_BIST_HDR_TYPE 	PMA_REG_ADDRESS(0x821C)  /* BIST/Header Type
                                                            Latency timer/cache
                                                            size */
#define PMA_PCI_MEM_LOC 	PMA_REG_ADDRESS(0x8224)   /* PCI Mem Location */
#define PMA_PCI_IO_LOC 		PMA_REG_ADDRESS(0x822C)   /* PCI IO Location */
#define PMA_TIMER_CNTR 		PMA_REG_ADDRESS(0x8284)   /* Timer Counter */
#define PMA_POSTED_READ_REG0 	PMA_REG_ADDRESS(0x10004)  /* Posted Rd Reg 0 */
#define PMA_PR0_OFFSET          0                         /* Offset from PR0 */
#define PMA_POSTED_READ_REG1 	PMA_REG_ADDRESS(0x1000C)  /* Posted Rd Reg 1 */
#define PMA_PR1_OFFSET          8                         /* Offset from PR0 */
#define PMA_POSTED_READ_REG2 	PMA_REG_ADDRESS(0x10014)  /* Posted Rd Reg 2 */
#define PMA_PR2_OFFSET          16                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG3 	PMA_REG_ADDRESS(0x1001C)  /* Posted Rd Reg 3 */
#define PMA_PR3_OFFSET          24                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG4 	PMA_REG_ADDRESS(0x10024)  /* Posted Rd Reg 4 */
#define PMA_PR4_OFFSET          32                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG5 	PMA_REG_ADDRESS(0x1002C)  /* Posted Rd Reg 5 */
#define PMA_PR5_OFFSET          40                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG6 	PMA_REG_ADDRESS(0x10034)  /* Posted Rd Reg 6 */
#define PMA_PR6_OFFSET          48                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG7 	PMA_REG_ADDRESS(0x1003C)  /* Posted Rd Reg 7 */
#define PMA_PR7_OFFSET          56                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG8 	PMA_REG_ADDRESS(0x10044)  /* Posted Rd Reg 8 */
#define PMA_PR8_OFFSET          64                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG9 	PMA_REG_ADDRESS(0x1004C)  /* Posted Rd Reg 9 */
#define PMA_PR9_OFFSET          72                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG10 	PMA_REG_ADDRESS(0x10054)  /* Posted Rd Reg 10 */
#define PMA_PR10_OFFSET         80                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG11 	PMA_REG_ADDRESS(0x1005C)  /* Posted Rd Reg 11 */
#define PMA_PR11_OFFSET         88                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG12 	PMA_REG_ADDRESS(0x10064)  /* Posted Rd Reg 12 */
#define PMA_PR12_OFFSET         96                        /* Offset from PR0 */
#define PMA_POSTED_READ_REG13 	PMA_REG_ADDRESS(0x1006C)  /* Posted Rd Reg 13 */
#define PMA_PR13_OFFSET         104                       /* Offset from PR0 */
#define PMA_POSTED_READ_REG14 	PMA_REG_ADDRESS(0x10074)  /* Posted Rd Reg 14 */
#define PMA_PR14_OFFSET         112                       /* Offset from PR0 */
#define PMA_POSTED_READ_REG15 	PMA_REG_ADDRESS(0x1007C)  /* Posted Rd Reg 15 */
#define PMA_PR15_OFFSET         120                       /* Offset from PR0 */
#define PMA_PCI_CONFIG_ADDR	PMA_REG_ADDRESS(0x20004)  /* PCI Configuration
                                                             Address */
#define PMA_PCI_CONFIG_DATA	PMA_REG_ADDRESS(0x2000C)  /* PCI Configuration
                                                             Data */

/*
 * PMA Error Register Upper Mask
 */

#define ERRU_PCI_TX_ERR            0xf0000000        /* PCI TX Parity Err */
#define ERRU_MTARGET_ABRT          0x08000000        /* PCI TX Parity Err */
#define ERRU_PCI_MASTER_ADDR       0x03ffffff        /* PCI Master Addr */

/* 
 * PMA Error Register Lower Mask
 */
#define ERRL_PBUS_PERR         0xff000000    /* Packet Bus Parity Err */
#define ERRL_PCI_PERR          0x00800000    /* PCI Parity Err */
#define ERRL_PCI_RETRY_TMOUT   0x00400000    /* PCI Retry Time-out */
#define ERRL_TRDY_TMOUT        0x00200000    /* TRDY Time-out */
#define ERRL_IRDY_TMOUT        0x00100000    /* IRDY Time-out */
#define ERRL_DEVSEL_TMOUT      0x00080000    /* DEVSEL Time-out */
#define ERRL_GNT_TMOUT         0x00040000    /* GNT Time-out */
#define ERRL_PCI_TADDR_TOOBIG  0x00020000    /* PCI Target ADDR > SRAM size */
#define ERRL_PCI_SERR          0x00010000    /* PCI SERR */
#define ERRL_PKTMEM_PERR       0x0000ff00    /* Packet Mem Parity Err */
#define ERRL_UNUSED            0x00000080
#define ERRL_PBUS_ADDR_TOOBIG  0x00000040    /* Packet Bus Addr > SRAM size */
#define ERRL_PCI_MA_ERR2       0x00000020    /* PCI Master Access Error 2 */
#define ERRL_PBUS_TMOUT_CYA    0x00000010    /* Packet Bus Timeout CYA */
#define ERRL_PBUS_TMOUT_CPU    0x00000008    /* Packet Bus Timeout CPU */
#define ERRL_PBUS_SM_ERR       0x00000004    /* Packet Bus State Machine Err */
#define ERRL_ILEG_CPU_BWRITE   0x00000002    /* Illegal CPU Burst Write */
#define ERRL_PCI_MA_ERR1       0x00000001    /* PCI Master Access Error 1 */
	

/* 
 * PMA Configuration Register 1 upper mask
 */

#define CR1U_PBUS_SM_STATE        0xffe00000  /* Pkt Bus State Machine State */
#define CR1U_CYA_TMOUT_CNTR_MASK  0x000ffc00  /* CYA Timed Out Counter */
#define CR1U_CPU_TMOUT_CNTR_MASK  0x000003ff  /* CPU Timed Out Counter */
#define PMA_CR1U_DEFAULT CR1U_CYA_TMOUT_CNTR_MASK | CR1U_CPU_TMOUT_CNTR_MASK

/* 
 * PMA ConfigUration Register 1 Lower mask
 */

#define CR1L_SRAM_SIZE_MASK       0xff000000  /* SRAM Size */
#define CR1L_SRAM_BANK_SIZE_MASK  0x00fe0000  /* SRAM Bank Size */
#define CR1L_RESET_PCI_MASTER     0x00000020  /* Reset PCI Mstr Cmd and fifo*/
#define CR1L_RESET_PMA            0x00000010  /* Reset PMA */
#define CR1L_EN_DIAG_MODE         0x00000004  /* Enable Diag Mode in Err reg */
#define CR1L_EN_CYA_POSTED_RD     0x00000002  /* Enable CYA Posted Rd Access */
#define CR1L_POSTED_RD_LOOPBACK   0x00000001  /* Posted Read Loopback */

#define CR1L_SRAM_SIZE_16M        0x00000000
#define CR1L_SRAM_SIZE_8M         0x80000000
#define CR1L_SRAM_SIZE_4M         0xC0000000
#define CR1L_SRAM_SIZE_2M         0xE0000000
#define CR1L_SRAM_SIZE_1M         0xF0000000
#define CR1L_SRAM_SIZE_512K       0xF8000000

#define CR1L_SRAM_BANK_16M        0x00000000
#define CR1L_SRAM_BANK_8M         0x00800000
#define CR1L_SRAM_BANK_4M         0x00C00000
#define CR1L_SRAM_BANK_2M         0x00E00000
#define CR1L_SRAM_BANK_1M         0x00F00000
#define CR1L_SRAM_BANK_512K       0x00F80000
#define CR1L_SRAM_BANK_256K       0x00FC0000

#define PMA_CR1L_DEFAULT 0

/*
 * PMA Posted Read bit masks
 */
#define PMA_PR_ADDR_MASK          0x003FFFFF
#define PMA_IO_PR_ACCESS          0x10000000
#define PMA_MEM_PR_ACCESS         0x00000000

/* 
 * PMA Configuration Register 2 - INTERRUPT MASK 
 */
#define INTM_PBUS_GT_SRAM         0x10000000  /* PktBUS > SRAM */
#define INTM_PCI_MA_ACC_TO_BERR   0x08000000  /* PCI Master Access Err */
#define INTM_PBUS_PERR            0x04000000  /* Pkt BUS Parity Err */
#define INTM_CYA_STALL_TMOUT      0x02000000  /* CYA STALL timed out */
#define INTM_CPU_STALL_TMOUT      0x01000000  /* CPU STALL timed out */
#define INTM_PCI_TX_PAR_TMOUT     0x00010000  /* PCI TX Parity Timed out */
#define INTM_PERR_TMOUT           0x00008000  /* PERR Timed out */
#define INTM_RETRY_TMOUT          0x00004000  /* RETRY Timed out */
#define INTM_TRDY_TMOUT           0x00002000  /* TRDY Timed out */
#define INTM_IRDY_TMOUT           0x00001000  /* IRDY Timed out */
#define INTM_DEVSEL_TMOUT         0x00000800  /* DEVSEL Timed out */
#define INTM_GNT_TMOUT            0x00000400  /* GNT Timed out */
#define INTM_PCI_GT_SRAM_SIZ      0x00000200  /* PCI > SRAM SIZE */
#define INTM_PCI_SERR             0x00000100  /* PCI SERR */
#define INTM_PKTMEM_PERR          0x00000008  /* Packet Memory Parity Error */
#define INTM_PBUS_STATE_MACH_ERR  0x00000004  /* Pkt Bus State Machine Error */
#define INTM_PBUS_BW_ALIGN_ERR    0x00000002  /* Pkt Bus Burst Wr misaligned */
#define INTM_PCI_MASTER_ACC_ERR   0x00000001  /* PCI Master Access Error */


#define PMA_INTR_MASK_REG_DEFAULT 0xE0FE00F0  /* Enable all valid intrpts */

#define PMA_PCI_STAT_CMD_DEFAULT  0xF9800347
#define PCI_LATENCY_TIME_DEFAULT  0x00002000  /* Just a WAG at latency time  */
#define PMA_TIMER_CNTR_DEFAULT    0xFFFFFFFF  /* Default Timer Counters */

#endif /* !__SVIP_PMA_H__ */
