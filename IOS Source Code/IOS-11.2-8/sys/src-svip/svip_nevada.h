/* $Id: svip_nevada.h,v 3.1.62.3 1996/07/08 22:54:08 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_nevada.h,v $
 *------------------------------------------------------------------
 * SVIP Nevada definitions
 *
 * 09/20/95  Srini		 - Its VIPER now..
 * 12/94, David Getchell - hacked from src-4k/nevasic.h
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_nevada.h,v $
 * Revision 3.1.62.3  1996/07/08  22:54:08  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/06/03  18:04:47  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:08  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:43  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_NEVADA_H__
#define __SVIP_NEVADA_H__

/* 
 * Although it is not nevada anymore, to avoid complications we can 
 * still call viper interrupt sub-system with the same name. 
 */

#ifndef ASMINCLUDE

typedef struct nev_asic_ {
    volatile ushort pa_isr1;         /* PA Interrupt Status Register 1 */
    volatile ushort pa_isr2;         /* PA Interrupt Status Register 2 */
    volatile ushort pa_imr;          /* PA Interrupt Mask Register  */
    volatile ushort pa_mimr;         /* PA Management Intrpt Mask Register */
    volatile ushort nev_isr;         /* Nevada Intrpt Status Register  */
    volatile ushort be_isr;          /* Bus Error Intrpt Status Register */
    volatile ushort pa_icr;          /* PA Interrupt Config Register  */
    volatile uchar  unused;          /* imr needs to be accessed as byte for
                                        source code compatibility */
    volatile uchar  nev_imr;         /* Nevada Interrupt Mask register */
    volatile ushort tcr;             /* Timer Config Register  */
    volatile ushort tvr;             /* Timer Value Register  */
    volatile ushort pad2[2];         /* Unused */
    volatile ushort cr1;             /* Control Register 1  */
    volatile ushort cr2;             /* Control Register 2  */
    volatile ushort cookie;          /* cookie register */
} nev_asic;

/*
 * Define a pointer to the ASIC
 */
#define IO_ASIC       ((nev_asic *)(ADRSPC_NEV_ASIC))
#define KSEG1_IO_ASIC ((nev_asic *)(KSEG1_ADRSPC_NEV_ASIC))

#else                 /* ASMINCLUDE */

#define IO_REG_ADDRESS(a)	(ADRSPC_NEV_ASIC + a)
#define KSEG1_IO_REG_ADDRESS(a) (KSEG1_ADRSPC_NEV_ASIC + a)

/*
 * Nevada Equiv. ASIC register addresses for assembly language code
 */
#define IO_PA_ISR1_REG	 IO_REG_ADDRESS(0x000)	/* PA Intrpt Status 1       */
#define IO_PA_ISR2_REG	 IO_REG_ADDRESS(0x002)	/* PA Intrpt Status 2       */
#define IO_PA_IMR_REG	 IO_REG_ADDRESS(0x004)	/* PA Intrpt Mask           */
#define IO_PA_MIMR_REG	 IO_REG_ADDRESS(0x006)	/* PA Mngmnt Intrpt Mask    */
#define IO_NEV_ISR_REG	 IO_REG_ADDRESS(0x008)	/* Nevada Intrpt Status     */
#define IO_BE_ISR_REG	 IO_REG_ADDRESS(0x00A)	/* Bus Error Intrpt Status  */
#define IO_PA_ICR_REG	 IO_REG_ADDRESS(0x00C)	/* PA Intrpt Config         */
#define IO_IMASK_REG     IO_REG_ADDRESS(0x00F)  /* Nevada Intrpt Mask       */
#define IO_TCR_REG	 IO_REG_ADDRESS(0x010)	/* Timer Config             */
#define IO_TVR_REG	 IO_REG_ADDRESS(0x012)	/* Timer Value              */
#define IO_CR1_REG	 IO_REG_ADDRESS(0x018)	/* Control Register 1       */
#define IO_CR2_REG	 IO_REG_ADDRESS(0x01A)	/* Control Register 2       */
#define IO_COOKIE_REG	 IO_REG_ADDRESS(0x01C)	/* cookie register          */
 
#define IO_ASIC_BASE     ADRSPC_NEV_ASIC

/* this definition are used by the rom monitor */
#define KSEG1_IO_PA_ISR1_REG	 KSEG1_IO_REG_ADDRESS(0x000)	/* PA Intrpt Status 1       */
#define KSEG1_IO_PA_ISR2_REG	 KSEG1_IO_REG_ADDRESS(0x002)	/* PA Intrpt Status 2       */
#define KSEG1_IO_PA_IMR_REG	 KSEG1_IO_REG_ADDRESS(0x004)	/* PA Intrpt Mask           */
#define KSEG1_IO_PA_MIMR_REG	 KSEG1_IO_REG_ADDRESS(0x006)	/* PA Mngmnt Intrpt Mask    */
#define KSEG1_IO_NEV_ISR_REG	 KSEG1_IO_REG_ADDRESS(0x008)	/* Nevada Intrpt Status     */
#define KSEG1_IO_BE_ISR_REG	 KSEG1_IO_REG_ADDRESS(0x00A)	/* Bus Error Intrpt Status  */
#define KSEG1_IO_PA_ICR_REG	 KSEG1_IO_REG_ADDRESS(0x00C)	/* PA Intrpt Config         */
#define KSEG1_IO_IMASK_REG       KSEG1_IO_REG_ADDRESS(0x00E)    /* Nevada Intrpt Mask       */
#define KSEG1_IO_TCR_REG	 KSEG1_IO_REG_ADDRESS(0x010)	/* Timer Config             */
#define KSEG1_IO_TVR_REG	 KSEG1_IO_REG_ADDRESS(0x012)	/* Timer Value              */
#define KSEG1_IO_CR1_REG	 KSEG1_IO_REG_ADDRESS(0x018)	/* Control Register 1       */
#define KSEG1_IO_CR2_REG	 KSEG1_IO_REG_ADDRESS(0x01A)	/* Control Register 2       */
#define KSEG1_IO_COOKIE_REG	 KSEG1_IO_REG_ADDRESS(0x01C)	/* cookie register          */

/*
 * Nevada Equiv. ASIC register offsets for assembly language code
 */
#define IO_IMASK_OFFSET         0x00F           /* Nevada Intrpt Mask       */
#define IO_TCR_OFFSET           0x010           /* Timer Config */
#define IO_TVR_OFFSET           0x012           /* Timer Value */

#endif   /* ASMINCLUDE */

/*
 * Definitions for PA Interrupt Status Register 1
 * (IO_ASIC->pa_isr1) a 16-bit read register
 */

#define PA_ISR1_PA0_INTR0 	0x0001	  /* PA 0, Interrpt 0  */
#define PA_ISR1_PA0_INTR1 	0x0002	  /* PA 0, Interrpt 1  */
#define PA_ISR1_PA0_INTR2 	0x0004	  /* PA 0, Interrpt 2  */
#define PA_ISR1_PA0_INTR3 	0x0008	  /* PA 0, Interrpt 3  */
#define PA_ISR1_PA0_INTR4 	0x0010	  /* PA 0, Interrpt 4  */
#define PA_ISR1_PA0_INTR5 	0x0020	  /* PA 0, Interrpt 5  */
#define PA_ISR1_PA0_INTR6 	0x0040	  /* PA 0, Interrpt 6  */
#define PA_ISR1_PA0_INTR7 	0x0080	  /* PA 0, Interrpt 7  */
#define PA_ISR1_PA1_INTR0 	0x0100	  /* PA 1, Interrpt 0  */
#define PA_ISR1_PA1_INTR1 	0x0200	  /* PA 1, Interrpt 1  */
#define PA_ISR1_PA1_INTR2 	0x0400	  /* PA 1, Interrpt 2  */
#define PA_ISR1_PA1_INTR3 	0x0800	  /* PA 1, Interrpt 3  */
#define PA_ISR1_PA1_INTR4 	0x1000	  /* PA 1, Interrpt 4  */
#define PA_ISR1_PA1_INTR5 	0x2000	  /* PA 1, Interrpt 5  */
#define PA_ISR1_PA1_INTR6 	0x4000	  /* PA 1, Interrpt 6  */
#define PA_ISR1_PA1_INTR7 	0x8000	  /* PA 1, Interrpt 7  */

/*
 * Definitions for PA Interrupt Status Register 2
 * (IO_ASIC->pa_isr2) a 16-bit read register
 */

#define PA_ISR2_PA0_INTR8 	0x0001	  /* PA 0, Interrpt 8  */
#define PA_ISR2_PA0_INTR9 	0x0002	  /* PA 0, Interrpt 9  */
#define PA_ISR2_PA1_INTR8 	0x0004	  /* PA 1, Interrpt 8  */
#define PA_ISR2_PA1_INTR9 	0x0008	  /* PA 1, Interrpt 9  */
#define PA_ISR2_PA0_NMI_MASK    (PA_ISR2_PA0_INTR8 | PA_ISR2_PA0_INTR9)
#define PA_ISR2_PA1_NMI_MASK    (PA_ISR2_PA1_INTR8 | PA_ISR2_PA1_INTR9)

/*
 * Definitions for PA Interrupt Mask Register 
 * (IO_ASIC->pa_imr) a 16-bit read/write register
 */

#define PA_IMR_PA0_INTR0 	0x0001	  /* PA 0, Level  0  */
#define PA_IMR_PA0_INTR1 	0x0002	  /* PA 0, Level  1  */
#define PA_IMR_PA0_INTR2 	0x0004	  /* PA 0, Level  2  */
#define PA_IMR_PA0_INTR3 	0x0008	  /* PA 0, Level  3  */
#define PA_IMR_PA0_INTR4 	0x0010	  /* PA 0, Level  4  */
#define PA_IMR_PA0_INTR5 	0x0020	  /* PA 0, Level  5  */
#define PA_IMR_PA0_INTR6 	0x0040	  /* PA 0, Level  6  */
#define PA_IMR_PA0_INTR7 	0x0080	  /* PA 0, Level  7  */
#define PA_IMR_PA1_INTR0 	0x0100	  /* PA 1, Level  0  */
#define PA_IMR_PA1_INTR1 	0x0200	  /* PA 1, Level  1  */
#define PA_IMR_PA1_INTR2 	0x0400	  /* PA 1, Level  2  */
#define PA_IMR_PA1_INTR3 	0x0800	  /* PA 1, Level  3  */
#define PA_IMR_PA1_INTR4 	0x1000	  /* PA 1, Level  4  */
#define PA_IMR_PA1_INTR5 	0x2000	  /* PA 1, Level  5  */
#define PA_IMR_PA1_INTR6 	0x4000	  /* PA 1, Level  6  */
#define PA_IMR_PA1_INTR7 	0x8000	  /* PA 1, Level  7  */

#define PA_IMR_PA0_MASK         0x00FF    /* PA 0 - all ports */
#define PA_IMR_PA1_MASK         0xFF00    /* PA 1 - all ports */

/*
 * Definitions for PA PA Management Interrupt Mask Register 
 * (IO_ASIC->pa_mimr) a 16-bit read/write register
 */

#define PA_MIMR_PA0_INTR8 	0x0001	  /* PA 0, Interrpt 8  */
#define PA_MIMR_PA0_INTR9 	0x0002	  /* PA 0, Interrpt 9  */
#define PA_MIMR_PA1_INTR8 	0x0004	  /* PA 1, Interrpt 8  */
#define PA_MIMR_PA1_INTR9 	0x0008	  /* PA 1, Interrpt 9  */

#define PA_MIMR_PA0_MASK         0x0003    /* PA 0 - both intrpts */
#define PA_MIMR_PA1_MASK         0x000C    /* PA 1 - both intrpts */

/* Define both PA isr regs together for accessing them as 32 bit 
   quantity for performance reasons. 

   PA_IMR has the same structure */

#ifndef ASMINCLUDE

typedef struct pa_isr_ {
    volatile uint unused    :12;
    volatile uint pa1_intr9 :1;
    volatile uint pa1_intr8 :1;
    volatile uint pa0_intr9 :1;
    volatile uint pa0_intr8 :1;
    volatile uint pa1_intr7 :1;
    volatile uint pa1_intr6 :1;
    volatile uint pa1_intr5 :1;
    volatile uint pa1_intr4 :1;
    volatile uint pa1_intr3 :1;
    volatile uint pa1_intr2 :1;
    volatile uint pa1_intr1 :1;
    volatile uint pa1_intr0 :1;
    volatile uint pa0_intr7 :1;
    volatile uint pa0_intr6 :1;
    volatile uint pa0_intr5 :1;
    volatile uint pa0_intr4 :1;
    volatile uint pa0_intr3 :1;
    volatile uint pa0_intr2 :1;
    volatile uint pa0_intr1 :1;
    volatile uint pa0_intr0 :1;
} pa_isr, pa_imr;

#endif

/*
 * Definitions for Nevada Equivalent Interrupt Status Register 
 * (IO_ASIC->nev_isr) a 16-bit read register
 */
#define NEV_ISR_PA0_5V_COMP 	0x0004	  /* PA0 = 5V Compatible */
#define NEV_ISR_PA0_3V_COMP 	0x0008	  /* PA0 = 3V Compatible */
#define NEV_ISR_DBUS_INTR   	0x0010	  /* DBUS Interrupt      */
#define NEV_ISR_MALU_INTR   	0x0020	  /* MALU ATTN  Interrupt */
#define NEV_ISR_TIM_INTR        0x0040    /* Timer Interrupt */
#define NEV_ISR_CBUS_STALL_INTR 0x0080    /* CBUS STALL Interrupt */
#define NEV_ISR_PA1_5V_COMP 	0x0100	  /* PA1 = 5V Compatible */
#define NEV_ISR_PA1_3V_COMP 	0x0200	  /* PA1 = 3V Compatible */
#define NEV_ISR_BUS_ERROR       0x0400    /* Bus error  - read be_isr */

/*
 * Definitions for Bus Error Interrupt Status Register 
 * (IO_ASIC->be_isr) a 16-bit read register
 */

#define BE_ISR_CYA_ERR_INTR	     0x0001  /* CYA Error Interrupt */	
#define BE_ISR_PMA_ERR_INTR	     0x0002  /* PMA Error Interrupt */	
#define BE_ISR_PCI0_SYS_ERR	     0x0004  /* PCI SYS ERROR, PCI Bus 0 */  
#define BE_ISR_PCI1_SYS_ERR	     0x0008  /* PCI SYS ERROR, PCI Bus 1 */  
#define BE_ISR_PCI2_SYS_ERR	     0x0010  /* PCI SYS ERROR, PCI Bus 2 */  
#define BE_ISR_PCI0_PAR_ERR          0x0020  /* PCI PAR ERROR, PCI Bus 0 */  
#define BE_ISR_PCI1_PAR_ERR	     0x0040  /* PCI PAR ERROR, PCI Bus 1 */  
#define BE_ISR_PCI2_PAR_ERR	     0x0080  /* PCI PAR ERROR, PCI Bus 2 */  
#define BE_ISR_RENO_WR_PAR_ERR01     0x0100  /* RENO WR PAR ERROR BYTES 0+1 */
#define BE_ISR_RENO_RD_PAR_ERR01     0x0200  /* RENO RD PAR ERROR BYTES 0+1 */
#define BE_ISR_RENO_WR_PAR_ERR23     0x0400  /* RENO WR PAR ERROR BYTES 2+3 */
#define BE_ISR_RENO_RD_PAR_ERR23     0x0800  /* RENO RD PAR ERROR BYTES 2+3 */
#define BE_ISR_DRAM_RENO_WR_BUS_ERR  0x1000  /* DRAM RENO WRITE  BUS ERROR  */
#define BE_ISR_RENO_IO_BUS_ERR	     0x2000  /* RENO I/O  BUS ERROR  */

/* We could access both nev_isr and ber_isr together for performance reasons.
*/

#ifndef ASMINCLUDE

typedef struct nev_isr_ {
    volatile uint unused2         :2;
    volatile uint riobe           :1;  /* RENO I/O  BUS ERROR  */
    volatile uint drwbe           :1;  /* DRAM RENO WRITE  BUS ERROR  */
    volatile uint rrpe23          :1;  /* RENO READ PARITY ERROR BYTES 2+3 */
    volatile uint rwpe23          :1;  /* RENO WRITE PARITY ERROR BYTES 2+3 */
    volatile uint rrpe01          :1;  /* RENO READ PARITY ERROR BYTES 0+1 */
    volatile uint rwpe01          :1;  /* RENO WRITE PARITY ERROR BYTES 0+1 */
    volatile uint pci2pe          :1;  /* PCI PARITY ERROR, PCI Bus 2 */
    volatile uint pci1pe          :1;  /* PCI PARITY ERROR, PCI Bus 1 */
    volatile uint pci0pe          :1;  /* PCI PARITY ERROR, PCI Bus 0 */
    volatile uint pci2se          :1;  /* PCI SYSTEM ERROR, PCI Bus 2 */
    volatile uint pci1se          :1;  /* PCI SYSTEM ERROR, PCI Bus 1 */
    volatile uint pci0se          :1;  /* PCI SYSTEM ERROR, PCI Bus 0 */
    volatile uint pei             :1;  /* PMA Error Interupt */	
    volatile uint cei             :1;  /* CYA Error Interupt */	
    volatile uint unused1         :6;
    volatile uint pa1_3v_cmp      :1;  /* PA1 = 3V Compatible */
    volatile uint pa1_5v_cmp      :1;  /* PA1 = 5V Compatible */
    volatile uint cbus_stall_intr :1;  /* CBUS STALL Interrupt */
    volatile uint tim_intr        :1;  /* Timer Interrupt */
    volatile uint malu_intr       :1;  /* MALU ATTN Interrupt */
    volatile uint dbus_intr       :1;  /* DBUS Intewrrupt */
    volatile uint pa0_3v_cmp      :1;  /* PA0 = 3V Compatible */
    volatile uint pa0_5v_cmp      :1;  /* PA0 = 5V Compatible */
    volatile uint reserved        :2;  /* PA hot swap - not supported */
} nev_isr;

#endif

/*
 * Definitions for PA Interrupt Configuration  Register
 * (IO_ASIC->pa_icr) a 16-bit read/write register
 */


#define PA_ICR_PA0_INTR0 	0x0001	  /* PA 0, Level  0  */
#define PA_ICR_PA0_INTR1 	0x0002	  /* PA 0, Level  1  */
#define PA_ICR_PA0_INTR2 	0x0004	  /* PA 0, Level  2  */
#define PA_ICR_PA0_INTR3 	0x0008	  /* PA 0, Level  3  */
#define PA_ICR_PA0_INTR4 	0x0010	  /* PA 0, Level  4  */
#define PA_ICR_PA0_INTR5 	0x0020	  /* PA 0, Level  5  */
#define PA_ICR_PA0_INTR6 	0x0040	  /* PA 0, Level  6  */
#define PA_ICR_PA0_INTR7 	0x0080	  /* PA 0, Level  7  */
#define PA_ICR_PA1_INTR0 	0x0100	  /* PA 1, Level  0  */
#define PA_ICR_PA1_INTR1 	0x0200	  /* PA 1, Level  1  */
#define PA_ICR_PA1_INTR2 	0x0400	  /* PA 1, Level  2  */
#define PA_ICR_PA1_INTR3 	0x0800	  /* PA 1, Level  3  */
#define PA_ICR_PA1_INTR4 	0x1000	  /* PA 1, Level  4  */
#define PA_ICR_PA1_INTR5 	0x2000	  /* PA 1, Level  5  */
#define PA_ICR_PA1_INTR6 	0x4000	  /* PA 1, Level  6  */
#define PA_ICR_PA1_INTR7 	0x8000	  /* PA 1, Level  7  */

/* Need to access as a short register */

#ifndef ASMINCLUDE

typedef struct pa_icr_ {
    volatile uint pa2_intr7 :1;
    volatile uint pa2_intr6 :1;
    volatile uint pa2_intr5 :1;
    volatile uint pa2_intr4 :1;
    volatile uint pa2_intr3 :1;
    volatile uint pa2_intr2 :1;
    volatile uint pa2_intr1 :1;
    volatile uint pa2_intr0 :1;
    volatile uint pa0_intr7 :1;
    volatile uint pa0_intr6 :1;
    volatile uint pa0_intr5 :1;
    volatile uint pa0_intr4 :1;
    volatile uint pa0_intr3 :1;
    volatile uint pa0_intr2 :1;
    volatile uint pa0_intr1 :1;
    volatile uint pa0_intr0 :1;
} pa_icr;

#endif


/*
 * Definitions for Nevada Interrupt request mask  Register
 * (IO_ASIC->nev_imr) a 16-bit read/write register
 */

#define NEV_IMR_BERROR_INTR 	0x0008	  /* BUS ERROR [0:13] Interrupts.
                                             0 => don't generate int req */ 
#define NEV_IMR_DBUS_INTR 	0x0010	  /* DBUS Interrupt. 
                                             0 => don't generate int req */ 
#define NEV_IMR_MALU_INTR 	0x0020	  /* MALU ATTN Interrupt. 
                                             0 => don't generate int req */ 
#define NEV_IMR_TIMER_INTR 	0x0040	  /* TIMER Interrupt. 
                                             0 => don't generate int req */ 
#define NEV_IMR_CSTALL_INTR 	0x0080	  /* CBUS_STALL Interrupt.
                                             0 => don't generate int req */ 

/* Need to access as a short register */

#ifndef ASMINCLUDE

typedef struct nev_imr_ {
    volatile uint unused1  :7 ; /* Unused */
    volatile uint berr     :1 ; /* BUS ERROR [0:13] Interrupts.
                                   0 => don't generate int req */ 
    volatile uint cstall   :1 ; /* CBUS_STALL Interrupt.
                                   0 => don't generate int req */ 
    volatile uint timer    :1 ; /* TIMER Interrupt.
                                   0 => don't generate int req */ 
    volatile uint malu     :1 ; /* MALU ATTN Interrupt.
                                   0 => don't generate int req */ 
    volatile uint dbus     :1 ; /* DBUS Interrupt.
                                   0 => don't generate int req */ 
    volatile uint unused2  :4 ; /* Unused */
} nev_imr;

#endif

/*
 * Definitions for Timer Configuration Register 
 * (IO_ASIC->tcr) a 16-bit  register
 */

#define TCR_ENB_TIMER	0x0001   	/* Enable Timer */

#ifndef ASMINCLUDE

/* Access as short register */
typedef struct tcr_ {
    volatile uint unused    :15;
    volatile uint enb_tim   :1;
} tcr;

#endif

/*
 * Timer Value Register 
 * (IO_ASIC->tvr) a 16-bit  register
 */

/* Access as short register */

#ifndef ASMINCLUDE

typedef struct tvr_ {
    volatile uint tim_val   :16;
} tvr;

#endif


/*
 * Definitions for Control Register 1
 * (IO_ASIC->cr1) a 16-bit read/write register
 */

#define	 CR1_PB_PAR_ODD		0x0001   /* PBUS parity 
					    0-even, 1-odd */
#define	 CR1_PB_ERR_EN		0x0002   /* PBUS Parity Checking.
					    0-disable, 1-enable */
#define	 CR1_ENB_PA0_LED	0x1000   /* Enable PA0 LED.  
					    0-LED off, 1-LED on */
#define	 CR1_ENB_PA1_LED	0x2000   /* Enable PA1 LED.  
					    0-LED off, 1-LED on */
#define	 CR1_ENB_MB_LED		0x4000   /* Enable MB LED.  
					    0-LED off, 1-LED on */
#define  CR1_DIAG_MODE		0x0001	 /* Diagnostic Mode. 
					    0-normal mode, 1-Diag mode */

/* CR1 reg bits 11,10 define DRAM SIZE */
#define CR1_DRAM_SZ_MASK        0x0c00
#define CR1_DRAM_SZ_64MB        0x0000  /*11->0  10->0  */
#define CR1_DRAM_SZ_32MB        0x0400  /*11->0  10->1  */
#define CR1_DRAM_SZ_16MB        0x0800  /*11->1  10->0  */
#define CR1_DRAM_SZ_8MB         0x0C00  /*11->1  10->1  */

/* CR1 reg bits 9,8 define EPROM SIZE */
#define CR1_EPROM_SZ_MASK       0x0300
#define CR1_EPROM_SZ_1MB        0x0000  /*9->0  8->0  */
#define CR1_EPROM_SZ_512KB      0x0100  /*9->0  8->1  */
#define CR1_EPROM_SZ_256KB      0x0200  /*9->1  8->0  */
#define CR1_EPROM_SZ_128KB      0x0300  /*9->1  8->1  */

/* CR1 reg bits 7,6, and 5 define SRAM (PKT MEM) SIZE */
#define CR1_SRAM_SZ_MASK        0x00E0
#define CR1_SRAM_SZ_16MB        0x0000  /*7->0  6->0   7->0  */
#define CR1_SRAM_SZ_8MB         0x0020  /*7->0  6->0   7->1  */
#define CR1_SRAM_SZ_4MB         0x0040  /*7->0  6->1   7->0  */
#define CR1_SRAM_SZ_2MB         0x0060  /*7->0  6->1   7->1  */
#define CR1_SRAM_SZ_1MB         0x0080  /*7->1  6->0   7->0  */
#define CR1_SRAM_SZ_512KB       0x00A0  /*7->1  6->0   7->1  */
#define CR1_SRAM_SZ_256KB       0x00C0  /*7->1  6->1   7->0  */
#define CR1_SRAM_SZ_UNUSED      0x00E0  /*7->1  6->1   7->1  */

/* CR1 reg bits 4,3, and 2 define MEMD SIZE */
#define CR1_MEMD_SZ_16MB        0x0000  /*4->0  3->0   2->0  */
#define CR1_MEMD_SZ_8MB         0x0004  /*4->0  3->0   2->1  */
#define CR1_MEMD_SZ_4MB         0x0008  /*4->0  3->1   2->0  */
#define CR1_MEMD_SZ_2MB         0x000C  /*4->0  3->1   2->1  */
#define CR1_MEMD_SZ_1MB         0x0010  /*4->1  3->0   2->0  */
#define CR1_MEMD_SZ_512KB       0x0014  /*4->1  3->0   2->1  */
#define CR1_MEMD_SZ_256KB       0x0018  /*4->1  3->1   2->0  */
#define CR1_MEMD_SZ_UNUSED      0x001C  /*4->1  3->1   2->1  */


/*
 * Definitions for Control Register 2
 * (IO_ASIC->cr2) a 16-bit read/write register
 */

#define	 NEV_CR2_RESET_PA0	0x0001   /* Reset bridge 0 (active low) */
#define	 NEV_CR2_RESET_PA1	0x0002   /* Reset bridge 1 (active low) */


/* This represents Nevada Control Reg.1 */
/* Access as short register */

#ifndef ASMINCLUDE

typedef struct nev_cr1_ {
   volatile uint  diag       :1 ;     /* Diag. mode. 
					 0=normal,1=diag mode */
   volatile uint  mb_led     :1 ;     /* Enable MB LED. 
					 0=LED off,1=LED on  */
   volatile uint  pa2_led    :1 ;     /* Enable PA2 LED. 
					 0=LED off,1=LED on  */
   volatile uint  PA0_led    :1 ;     /* Enable PA2 LED. 
					 0=LED off,1=LED on  */
   volatile uint  dram_sz    :2 ;     /* DRAM SIZE[1:0]  */
   volatile uint  eprom_sz   :2 ;     /* EPROM SIZE[1:0]  */
   volatile uint  sram_sz    :3 ;     /* SRAM SIZE[2:0]  */
   volatile uint  memd_sz    :3 ;     /* MEMD SIZE[2:0]  */
   volatile uint  pb_par_en  :1 ;     /* PBUS Parity error enable. 
                                        1=enable party check. */
   volatile uint  pb_par_odd :1 ;     /* PBUS Parity. 
					 0=Even, 1=Odd parity */

} nev_cr1 ;

#endif

/* This represents Nevada Control Reg.2 */
/* Access as short register */

#ifndef ASMINCLUDE

typedef struct nev_cr2_ {
   volatile uint  unused     :14 ;  /* Unused */
   volatile uint  rst_br2    :1  ;  /* Reset Bridge2. 0=Reset,
                                       1=Normal operation.*/
   volatile uint  rst_br1    :1  ;  /* Reset Bridge1. 0=Reset,
                                       1=Normal operation.*/
} nev_cr2 ;

#endif

/*
 * Definitions for JTAG/Cookie Register 
 * (IO_ASIC->cookie) a 16-bit  register
 */

#define COOKIE_PA0_TCLK		0x0001	  /* PA0 JTAG+EEPROM+FPGA pgm clk */
#define COOKIE_PA0_TDIN		0x0002	  /* PA0 JTAG+EEPROM+FPGA data in */
#define COOKIE_PA0_TDOUT	0x0004	  /* PA0 JTAG+EEPROM  data out */
#define COOKIE_PA0_TEECS	0x0008	  /* PA0 EEPROM reset/CS pin */
#define COOKIE_PA0_TJTAG_MODE	0x0010	  /* PA0 JTAG TMS  pin */
#define COOKIE_PA0_TFPGA_MODE	0x0020	  /* PA0 FPGA mode  (cfg/pgm) */
#define COOKIE_PA0_TFPGA_DONE	0x0040	  /* PA0 FPGA reset  (e.g. done) */

#define COOKIE_PA2_TCLK		0x0100	  /* PA1 JTAG+EEPROM+FPGA pgm clk */
#define COOKIE_PA2_TDIN		0x0200	  /* PA1 JTAG+EEPROM+FPGA data in */
#define COOKIE_PA2_TDOUT	0x0400	  /* PA1 JTAG+EEPROM  data out */
#define COOKIE_PA2_TEECS	0x0800	  /* PA1 EEPROM reset/CS pin */
#define COOKIE_PA2_TJTAG_MODE	0x1000	  /* PA1 JTAG TMS  pin */
#define COOKIE_PA2_TFPGA_MODE	0x2000	  /* PA1 FPGA mode  (cfg/pgm) */
#define COOKIE_PA2_TFPGA_DONE	0x4000	  /* PA1 FPGA reset  (e.g. done) */

#ifndef ASMINCLUDE

typedef struct cookie_ {
    volatile uint unused2    :1;
    volatile uint pa1_tfdone :1;
    volatile uint pa1_tfmode :1;
    volatile uint pa1_tjmode :1;
    volatile uint pa1_teecs  :1;
    volatile uint pa1_tdout  :1;
    volatile uint pa1_tdin   :1;
    volatile uint pa1_tclk   :1;
    volatile uint unused1    :1;
    volatile uint pa0_tfdone :1;
    volatile uint pa0_tfmode :1;
    volatile uint pa0_tjmode :1;
    volatile uint pa0_teecs  :1;
    volatile uint pa0_tdout  :1;
    volatile uint pa0_tdin   :1;
    volatile uint pa0_tclk   :1;
} cookie;

#endif

#endif  /* !__SVIP_NEVADA_H__ */


