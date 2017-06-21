/* $Id: svip_cya.h,v 3.1.62.6 1996/08/30 01:33:37 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_cya.h,v $
 *------------------------------------------------------------------
 * svip_cya.h:  SVIP CYA definitions
 *
 * 09/20/95  Srini		 - Its VIPER now..
 * 12/94, David Getchell - hacked from src-4k/nevasic.h
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_cya.h,v $
 * Revision 3.1.62.6  1996/08/30  01:33:37  kao
 * CSCdi67591:  CYA_RD_UNUSED_PREAD bit should not be set on CYA of VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/05/18  17:54:24  kao
 * CSCdi54117:  vip2 looses it with serial encaps other than HDLC
 * cbus stall handling plus two s/w work-around for CYA h/w bugs
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/04/12  17:44:10  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/08  05:38:47  getchell
 * CSCdi52876:  corrupt sw2idb pointer
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/06  21:56:27  ondiaye
 * CSCdi52394:  svip support for merged qe codes header file
 *              Also takes care of CSCdi52731 and the DRQ dealock.
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.2  1996/03/07  17:29:40  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:21:58  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:36  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Following are the defines for Memory mapping seen from CYA 
 */

#define PCIMEM_BASE_ADDR_IN_CYA 0x90000000  /* PCIMEM seen from CYA */


/*
 * Following are the defines for PCIMEM requirements in cya_cntrl_t.sbar register
 */
#define PCIMEM_SBAR_SIZE	0x200       /* temporary memd write buffer */
#define PCIMEM_SBAR_ALIGNMENT	PCIMEM_SBAR_SIZE
					    /* above buffer need to be aligned */

/*
 * Following are the defines for CYA Error Interrupt register 
 * bit assignments
 */

#define CYA_FORCED_ERR_INTR	0x80000000  /* Forced Error Interrupt */
#define CYA_BE_FROM_PMA_ON_CYA	0x40000000  /* Bus Error Signal from PMA on 
					       CYA master Cycle */
#define CYA_SOFT_DBUS_ERR     	0x20000000  /* Error in handling "soft" DBUS 
					       data port */
#define CYA_OVF_INT_ARB_FIFO   	0x10000000  /* Overflow of internal 
					       arbitration FIFO */

#define CYA_DMA_XMT_ERR        	0x04000000  /* DMA Transmit Error  */
#define CYA_DMA_RCV_ERR        	0x02000000  /* DMA Receive Error  */
#define CYA_QE_LOW_PRI_INTR    	0x01000000  /* Queueing Engine LOW priority
					       Interrupt */
#define CYA_QE_PGM_MPAR_INTR   	0x00800000  /* Queueing Engine Program Memory
					       Parity Error */
#define CYA_CPU_REUSE_PREAD     0x00400000  /* ORION re-uses (writes to) a 
					       read-ahead register before
					       completing previous use 
					       (reading back data) */
#define CYA_RD_UNUSED_PREAD   	0x00200000  /* Readback from unused posted
					       read */ 
#define CYA_RD_BAD_DATA       	0x00100000  /* Readback of bad data (CyBus 
					       Error ot Posted read 
					       register Error)  */ 
#define CYA_GLOCK_REQ_UNAVAIL  	0x00080000  /* GLobal Lock requested when 
					       unavilable for long period*/
#define CYA_RD_DURING_STALL    	0x00040000  /* CBUS read (non=posted) during
					       CBUS stall or CBUS stall during
					       DBUS read (non-posted). */
#define CYA_RD_PREAD_NOT_CMPL 	0x00020000  /* Read (during CBUS stall) of 
					       CBUS posted read that hasn't 
					       completed */
#define CYA_OVF_WR_FIFO_2MEMD 	0x00010000  /* Overflow of ORION write FIFO to
					       MEMD (during CBUS stall)  */

#define CYA_ILL_ADRS_FROM_CPU 	0x00008000  /* Illegal address from ORION */
#define CYA_RETRY_NOT_SAME_ADRS	0x00004000  /* Re-try of access at end of I/O
					       stall is not same address */
#define CYA_REUSE_PREAD_STALL   0x00002000  /* ORION re-uses (writes to) a 
					       read-ahead register before
					       completing previous use
					       (reading back data) and there 
					       is CBUS stall */
#define CYA_INV_BE_FROM_CPU   	0x00000200  /* Invalid Byte Enables from ORION
					       ---non-contiguous or all zero */
#define CYA_SINGLE_IDLE_CYCLE  	0x00000100  /* Single Idle cycle between
					       packet bus grants */
#define CYA_ODD_PBUS_GNTS  	0x00000080  /* Odd number of Packet Bus grant
					       cycles */
#define CYA_PAR_ERR_PBUS  	0x00000010  /* Parity Error in data coming 
					       from PBUS */
#define CYA_PAR_ERR_INTERNAL  	0x00000008  /* Parity Error Internal to CYA 
					       (Data Path) */ 
#define CYA_PAR_ERR_CBUS  	0x00000004  /* Parity Error in data coming 
					       from CBUS */
#define CYA_MISSING_ACK_CBUS  	0x00000002  /* Missing ACK on CBUS access */
#define CYA_NACK_PRESENT_CBUS  	0x00000001  /* NACK present on CBUS access */

#define CYA_CODE_SIZE           0x200       /* program memory size */
#define CYA_DATA_SIZE           0x80
#define QE_XMEM_START           0x6        /* internal qe Xmem offset from 
					       start of XMEM */
#define QE_YMEM_START           0xa        /* internal qe Ymem offset from 
					       start of YMEM */

/*
 * Following are the defines for CYA Other Interrupt register 
 * bit assignments
 * NOTE : 
 *	Bits 22 through 14 are the same as Error Interrupt register.
 */


#define CYA_STALL_INDICATION  	 0x80000000  /* CBUS stall indication (raw -
						may not be cleared by 
						software */
#define CYA_STALL_IN_PROGRESS  	 0x40000000  /* CBUS stall in progress */  
#define CYA_CBUS_STALL_FORCED    0x20000000  /* CBUS stall forced */
#define CYA_CBUS_STALL_START   	 0x10000000  /* CBUS stall Starting */
#define CYA_CBUS_STALL_END     	 0x08000000  /* CBUS stall Ending */
#define CYA_OI_REUSE_PREAD_STALL 0x00800000  /* ORION re-uses (writes to) a 
						read-ahead register before 
						completing previous use 
						(reading back data) and there 
						is CBUS stall */
#define CYA_EXCP_INTR_FORCED 	 0x00002000  /* Exception Interrupt forced */
#define CYA_QE_INTR_FORCED	 0x00000200  /* QE interrupt forced */
#define CYA_QE_DMA_INTR 	 0x00000100  /* QE High Priority Interrupt. 
					        (Low is bit 24 of Error 
					        Interrupt Register )  */
#define CYA_DBUS_INTR_FORCED	 0x00000020  /* DBUS interrupt forced */
#define CYA_DBUS_INTR		 0x00000010  /* DBUS interrupt  */
#define CYA_MALU_ATTN_FORCED	 0x00000002  /* MALU_ATTN  forced */
#define CYA_MALU_ATTN		 0x00000001  /* MALU_AATTN   */

#define CYA_BUS_ERROR_EVENTS    (CYA_OI_REUSE_PREAD_STALL \
	| CYA_RD_BAD_DATA | CYA_GLOCK_REQ_UNAVAIL | CYA_RD_DURING_STALL \
	| CYA_RD_PREAD_NOT_CMPL | CYA_OVF_WR_FIFO_2MEMD)
/*
 * CYA Miscellaneous Configuration Register definitions
 */
#define CYA_MCR_CSI    0x00000010   /* CBUS stall ignore. 
				       0=CBus stall will keep PBus from 
				         becoming stalled for long time.
				       1=CBus stall will cause long Packet
				         Bus stall */
#define CYA_MCR_BCR    0x00000008   /* Block CBus requests
				       0=normal. 
				       1= block--this bit automatically 
				          set by any DBus reset */

#define CYA_MCR_CSDI   0x00000004   /* CxBus special DMA improvement. 
				       0=present DMA arbitration scheme.
				       1="improved" DMA arbitration scheme 
				         (Cxbus only, never use on CyBus) */
#define CYA_MCR_QCAP   0x00000002   /* QE CyBus arbitration point. 
				       0=present arbitration scheme.
				       1="improved" arbitration scheme */
#define CYA_MCR_RESET  0x00000001   /* Soft reset
				       0=default. 
				       1=force reset of whole CYA. 
				         Bit returns to zero by itself */

#define TX_STATUS_WORD              0
#define IPC_STATUS_WORD             1
#define IPC_STATUS_WORD_BIT         1
#define QE_NUM_QUEUES 16
#define QE_NUM_STATUS 2
#define QUEUES_PER_STATUS_REGISTER 16
#define QE_FAST_QUEUE 2             /* number of fast queues */
#define MAX_TX_QUEUES              (QE_NUM_QUEUES+1)  /* +1 is for IPC  */
#define CHAN_TXQ_FLAG          0x80000000     /* this flag tell the QE this plughole 
						 is channelized */

#define PR_MASK_4BYTE          0xf0000000   /* 4 bytes for posted read */
#define PR_MASK_4BYTE_UPPER    0xf000       /* 4 bytes for posted read */
#define PR_MASK_2BYTE          0x30000000   /* 2 bytes for posted read */

#ifndef ASMINCLUDE

/*
 * Following are CYA-DMA engine specifc Information.
 */

/* DMA command register definition */

/* A typical DMA command resgisters structure  */

typedef struct cya_dma_t_ {
    uint :32 ;
    volatile ulong cmd0;                /* Command register    */
    uint :32 ;
    volatile ulong csum_base0;          /* rd ->Checksum result reg  */
                                        /* wr -> Pkt memory base adrs reg  */
    uint :32 ;
    volatile ulong padrs0;       	/* wr -> Pkt memory base adrs reg */
    uint :32 ;
    volatile ulong madrs0;       	/* wr -> MEMD  adrs reg (bits 23-0) */
    uint :32 ;
    volatile ulong len0;       	        /* wr -> DMA length in bytes */
    uint :32 ;
    volatile ulong unused0;
    uint :32 ;
    volatile ulong sar;            	/* Source Address Register  */
    uint :32 ;
    volatile ulong dar;            	/* Destination Address Register  */
    uint :32 ;
    volatile ulong cmd1;                /* Command register    */
    uint :32 ;
    volatile ulong csum_base1;          /* rd ->Checksum result reg  */
                                        /* wr -> Pkt memory base adrs reg */
    uint :32 ;
    volatile ulong padrs1;       	/* wr -> Pkt memory base adrs reg */
    uint :32 ;
    volatile ulong madrs1;       	/* wr -> MEMD  adrs reg (bits 23-0) */
    uint :32 ;
    volatile ulong len1;       	        /* wr -> DMA length in bytes */
    uint :32 ;
    volatile ulong state;               /* DMA state register */
    uint :32 ;
    volatile ulong count;               /* DMA count register */
    uint :32 ;
    volatile ulong ctrl;                /* DMA control register */

} cya_dma_t ;

/*
 * Following represents  pkt bus interface register set, that allows QE to
 * access packet memory. 
 */
typedef struct cya_qe_pkt_ {
    uint :32 ;
    volatile uint pbe	;		/* Byte enable for write  */
    uint :32 ;
    volatile uint pra	;		/* Pkt mem adrs to do a 64-bit rd*/
    uint :32 ;
    volatile uint pwa64	;		/* Pkt mem adrs to do a 64-bit wr*/
    uint :32 ;
    volatile uint pwa32	;		/* Pkt mem adrs to do a 32-bit wr*/
    uint :32 ;
    volatile uint phd  	;		/* High 32 bits of read or write */
    uint :32 ;
    volatile uint pld  	;		/* Low 32 bits of read or write 
					   pkt mem data */
} cya_qe_pkt;


/*
 * Following represents  CBUS interface register set, that allows QE to
 * access memd . 
 */
typedef struct cya_qe_cbus_ {
    uint :32 ;
    volatile uint cra64	;		/* Memd address to do a 64 bit rd. */
    uint :32 ;
    volatile uint cra32	;		/* Memd address to do a 32 bit rd. */
    uint :32 ;
    volatile uint cwa64	;		/* Memd address to do a 64 bit wr. */
    uint :32 ;
    volatile uint cwa32	;		/* Memd address to do a 32 bit wr. */
    uint :32 ;
    volatile uint cwa16	;		/* Memd address to do a 16 bit wr. */
    uint :32 ;
    volatile uint cwa8 	;		/* Memd address to do a 8  bit wr. */
    uint :32 ;
    volatile uint chd  	;		/* High 32 bits of read or write 
					   pkt mem data*/
    uint :32 ;
    volatile uint cld  	;		/* Low 32 bits of read or write 
					   pkt mem data*/
    uint :32 ;
    volatile uint stat  ;		/* Cbus/Pkt status register */
    uint :32 ;
    volatile uint cmd   ;		/* Cbus command register */
} cya_qe_cbus;

/*
 * bit definition of cya_qe_cbus.stat register
 */
#define QE_CBUS_WRITE_COMPLETE	0x8

/*
* Following represents  QE diag register set. 
*/

typedef struct cya_qe_diag_ {
    uint :32 ;
    volatile ulong  opt     ;		/* QE Options register */
    uint :32 ;
    volatile uint ss    ;		/* QE Single Step register */
    volatile ulong unknown[6]	;               /* Unknown */
    uint :32 ;
    volatile uint aop   ;		/* QE A_OP internal register */
    uint :32 ;
    volatile uint bop   ;		/* QE B_OP internal register */
    volatile ulong unkown[2]	;		/* Unknown  */
    uint :32 ;
    volatile uint pc    ;		/* QE Program Counter register */
    uint :32 ;
    volatile uint dcmd  ;		/* Diang QE  Command register */
    uint :32 ;
    volatile uint valid ;		/* Diang QE  Command valid    */
    uint :32 ;
    volatile uint resp  ;		/* Diang QE  Command response   */

} cya_qe_diag;

#define QE_RUN_BIT 0x4
#define QE_DEBUG_PC_BIT 0x10
#define QE_JS_WAIT_DISABLE_BIT 0x20

typedef struct pstd_rd_regs_ {
    volatile ulong reg0u;             /* Posted read 0 readback upper word */
    volatile ulong reg0;              /* Posted read 0 readback lower word */
    volatile ulong reg1u;             /* Posted read 1 readback upper word */
    volatile ulong reg1;              /* Posted read 1 readback lower word */
    volatile ulong reg2u;             /* Posted read 2 readback upper word */
    volatile ulong reg2;              /* Posted read 2 readback lower word */
    volatile ulong reg3u;             /* Posted read 3 readback upper word */
    volatile ulong reg3;              /* Posted read 3 readback lower word */
} pstd_rd_regs_t;

typedef struct cya_cntrl_ {
    volatile ulong eisr;              /* Error Intrpt Source Register RONLY */
    ulong :32 ;
    volatile ulong eicr;              /* Error Intrpt Clear Register WONLY */
    ulong :32 ;
    volatile ulong eier;              /* Error Interrupt Enable Register */
    volatile ulong unused7[3];        /* Unused */
    volatile ulong oisr;              /* Other Intrpt Source Register RONLY */
    ulong :32 ;
    volatile ulong oicr;              /* Other Intrpt Clear Register WONLY */
    ulong :32 ;
    volatile ulong oier;              /* Other Interrupt Enable Register */
    ulong :32 ;
    volatile ulong sbar;              /* Stall Buffer Address Register */
    volatile ulong unused11[9];       /* Unused */
    volatile ulong cber;              /* CyBus Error Register */
    volatile ulong unused12[3];       /* Unused */
    volatile ulong pber;              /* PKT Bus Error Register */
    volatile ulong unused13[347];     /* Unused */
    volatile ulong glrqr;             /* Global lock request register RONLY */
    ulong :32 ;
    volatile ulong glrli;             /* Global lock release - immediate WO */
    ulong :32 ;
    volatile ulong glrlc;             /* Global lock release - cond WO */
    ulong :32 ;
    volatile ulong glto;              /* Global lock request time out */
} cya_cntrl_t;

typedef struct double_word_ {
    ulong :32 ;
    volatile ulong reg;
} double_word_t;

typedef struct cya_qe_xmem_ {
    double_word_t  txq_addr[QE_NUM_QUEUES];  /* Tx Status word masks */
    ulong :32 ;
    volatile ulong ipc_qaddr;           
    ulong :32 ;
    volatile ulong drq_ptr;           /* DRQ ptr. (0x49424) */
    double_word_t sts_mask[QE_NUM_STATUS];           /* status mask of the 16 q's above */ 
    ulong :32 ;
    volatile ulong drq_len;           /* DRQ length (0x4942C) */
    ulong :32 ;
    volatile ulong bhdr_zero_cont;   /* counts the num of times we read an empty txQ
                                      after reading its status bit as 1 */
    volatile ulong unused[84];       /* the rest of  XMEM data memory*/

} cya_qe_xmem_t;

typedef struct cya_qe_ymem_ {
    double_word_t  dtq_txq_id[QE_NUM_QUEUES];     /* dtq txq num */
    ulong :32 ;
    volatile ulong dtq_ipc_id;         /* ipc Q num */
    ulong :32 ;
    volatile ulong status;             /* Tx Status register */
    double_word_t  fastq_sts_bit[QE_FAST_QUEUE]; /* fast Txq mask bit */
    ulong :32 ;
    volatile ulong dtq_ptr;           /* DTQ ptr (0x49644) */
    ulong :32 ;
    volatile ulong dtq_len;           /* DTQ Len (reg 0xC9) */
    ulong :32 ;
    volatile ulong dtq_bfr_size;       /* Size of tx buffers */
    volatile ulong unused[82];       /* the rest of YMEM data memory */
} cya_qe_ymem_t;


typedef struct qe_addrs_space_ {
    volatile ulong unused1[30];       /* unused */  
    ulong :32 ;
    volatile ulong acc;               /* QE accumulator */
    ulong :32 ;
    volatile ulong ix;  	      /* QE Index register */
    ulong :32 ;
    volatile ulong rtc;	              /* QE Free running counter */
    ulong :32 ;
    volatile ulong ix_sel;	      /* QE Index Select Register */
    ulong :32 ;
    volatile ulong jix;	              /* QE Jump Index Register */
    ulong :32 ;
    volatile ulong cmd; 	      /* QE Command register */
    volatile ulong unused2[98];       /* unused */  
    cya_dma_t 	   tx_dma;            /* transmit DMA registers */
    cya_dma_t 	   rx_dma;            /* receive DMA registers */
    cya_qe_pkt     pkt_if;            /* Pkt memory interface regs */
    cya_qe_cbus    memd_if;           /* MEMD  interface regs */
    volatile ulong unused3[32];       /* unused */  
    cya_qe_xmem_t  xmem;              /* QE X memory */
    cya_qe_ymem_t  ymem;              /* QE Y memory */
    cya_qe_diag    diag;	      /* QE Diag section */

} qe_addrs_space;

/*
 * CYA Internal locations can be treated as 64-bit registers (each register
 * takes up 8 bytes of address space). with the upper 32 bits ignored, or be
 * accessed as 32 bit registers. This applies to DBUS, EEPROM, Interrupt, and 
 * Queueing Engine locations, but NOT to read-back of the posted read 
 * registers, which can return 64 bits.
 */

typedef struct cya_asic_ {
    dbus_hardware_regs  dreg ;	      /* Dbus register set */

    volatile ulong unused1[48];       /* Unused */

    volatile ulong eeprom[64];        /* EEPROM addresses */

    volatile ulong unused2[896];      /* Unused */

    pstd_rd_regs_t pr;                /* Posted Read registers */

    volatile ulong unused3[2105];     /* Unused */

    volatile ulong cfg;               /* Misc Configuration register */

    volatile ulong unused4[71];       /* Unused */

    cya_cntrl_t    cntrl;             /* CYA control/status registers */

    volatile ulong unused5[3576];     /* Unused. */  

    volatile ulong qe_pgm_mem[1012];  /* QE Program Memory (512 8 byte 
					 locations)*/  

    qe_addrs_space qe;                /* QE register address space */
  
} cya_asic;


#define CYA_ASIC       ((cya_asic *)(ADRSPC_CYA_ASIC))

extern ulong cya_diag_opt_shdw;

static inline void set_qe_opt (ulong option)
{
    cya_diag_opt_shdw |= option;
    CYA_ASIC->qe.diag.opt = cya_diag_opt_shdw;
}

static inline void clear_qe_opt (ulong option)
{
    cya_diag_opt_shdw &= ~option;
    CYA_ASIC->qe.diag.opt = cya_diag_opt_shdw;
}


static inline void stop_qe (void)
{
    clear_qe_opt(QE_RUN_BIT);
}

static inline void start_qe (void)
{
    set_qe_opt(QE_RUN_BIT);
}

#endif  /* ASMINCLUDE */

/*
* CYA ASIC specific information starts here.
*/
#ifdef ASMINCLUDE

#define CYA_REG_ADDRESS(a)	(ADRSPC_CYA_ASIC + a)

/*
 * CYA ASIC register addresses for assembly language code
 * 
 * These have been fixed to match the current definition of 
 * PHY_ADRSPC_CYA_ASIC.  Everything would be much clearer if
 * PHY_ADRSPC_CYA_ASIC was 0x10040000 instead of 0x10041000.
 */
#define CYA_PRB0_REG     CYA_REG_ADDRESS(0x1000)  /* Posted Read Back reg 0  */
#define CYA_PR0_REG      CYA_REG_ADDRESS(0x1004)  /* Posted Read reg 0       */
#define CYA_PRB1_REG     CYA_REG_ADDRESS(0x1008)  /* Posted Read Back reg 1  */
#define CYA_PR1_REG      CYA_REG_ADDRESS(0x100C)  /* Posted Read reg 1       */
#define CYA_PRB2_REG     CYA_REG_ADDRESS(0x1010)  /* Posted Read Back reg 2  */
#define CYA_PR2_REG      CYA_REG_ADDRESS(0x1014)  /* Posted Read reg 2       */
#define CYA_PRB3_REG     CYA_REG_ADDRESS(0x1018)  /* Posted Read Back reg 3  */
#define CYA_PR3_REG      CYA_REG_ADDRESS(0x101C)  /* Posted Read reg 3       */
#define CYA_MC_REG       CYA_REG_ADDRESS(0x3104)  /* Misc configuration reg  */
#define CYA_EIS_REG      CYA_REG_ADDRESS(0x3224)  /* Error Intrpt Source reg */
#define CYA_EIC_REG      CYA_REG_ADDRESS(0x322C)  /* Error Intrpt Clear reg  */
#define CYA_EIE_REG      CYA_REG_ADDRESS(0x3234)  /* Error Intrpt Enable reg */
#define CYA_OIS_REG      CYA_REG_ADDRESS(0x3244)  /* Other Intrpt Source reg */
#define CYA_OIC_REG      CYA_REG_ADDRESS(0x324C)  /* Other Intrpt Clear reg  */
#define CYA_OIE_REG      CYA_REG_ADDRESS(0x3254)  /* Other Intrpt Enable reg */
#define CYA_SBA_REG      CYA_REG_ADDRESS(0x325C)  /* Stall Buffer Addr reg   */
#define CYA_CBE_REG      CYA_REG_ADDRESS(0x3284)  /* Cbus Error reg          */
#define CYA_PBE_REG      CYA_REG_ADDRESS(0x3294)  /* Packet Bus Error reg    */
#define CYA_GLRQ_REG     CYA_REG_ADDRESS(0x3804)  /* Global Lock Request reg */
#define CYA_GLRI_REG     CYA_REG_ADDRESS(0x380C)  /* Glbl Lck Rel Immediate  */
#define CYA_GLRC_REG     CYA_REG_ADDRESS(0x3814)  /* Glbl Lck Rel Cond reg   */
#define CYA_GLTO_REG     CYA_REG_ADDRESS(0x381C)  /* Glbl Lck Time Out reg   */
#define CYA_QE_REG       CYA_REG_ADDRESS(0x7000)  /* QE Address space        */
#define CYA_FIFO_RAM_DS  CYA_REG_ADDRESS(0xB000)  /* FIFO RAM diag space     */

/* FIXME:  This appears to be part of the Nevada?  Find a home for this. */
#define IO_EXT_INT_LO_REG       0x40
 
#endif  /* ASMINCLUDE */
