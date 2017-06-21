/* $Id: nevasic.h,v 3.3 1996/02/11 02:16:55 clev Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/nevasic.h,v $
 *------------------------------------------------------------------
 * Nevada ASIC definitions
 *
 * 10/93, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nevasic.h,v $
 * Revision 3.3  1996/02/11  02:16:55  clev
 * CSCdi44945:  Support new memory options
 *
 * Add code to support the 64mb dual bank SIMM.
 *
 * Revision 3.2  1995/11/17  18:40:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:21:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:52:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NEVASIC_H__
#define __NEVASIC_H__

/*
 * Defines for the cisco Nevada ASIC
 * 
 * Sierra and IRSP read I/O devices on low 32 bits of data bus
 * IE address 4..7 or C..F . On Sierra, I/O devices are connected 
 * to bits 31..16, hence address 4 or C are required.
 */

#ifndef ASMINCLUDE

#define PAD unsigned char

typedef struct nev_asic_{
    PAD pad0[4];
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
    volatile unsigned short sys_ctl;     /* system control reg (A) */
    PAD pad9[6];
    volatile unsigned short shmem_ctl;   /* shared memory control reg (B) */
    PAD pad10[6];
    volatile unsigned short shmem_stat;  /* shared memory status reg (A) */
    PAD pad11[6];
    volatile unsigned short nim_stat;    /* network int status reg (B) */
    PAD pad12[230];
    volatile unsigned short sys_stat0;   /* conf system status A reg */
    PAD pad13[6];
    volatile unsigned char sys_stat1;    /* conf system status B reg */
    PAD pad14[7];
    volatile unsigned char sys_stat2;    /* conf system status C reg */
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
#define KSEG1_IO_ASIC ((nev_asic *)(KSEG1_ADRSPC_NEV_ASIC))

#else                 /* ASMINCLUDE */

#define IO_REG_ADDRESS(a)	(ADRSPC_NEV_ASIC + a)

/*
 * Nevada ASIC register addresses for assembly language code
 */
#define IO_CT0_REG		IO_REG_ADDRESS(0x004)	/* ct0  	*/
#define IO_WATCHDOG_REG		IO_REG_ADDRESS(0x004)	/* ct0		*/
#define IO_CT1_REG		IO_REG_ADDRESS(0x00C)	/* ct1		*/
#define IO_CT2_REG		IO_REG_ADDRESS(0x014)	/* ct2		*/
#define IO_CCR_REG		IO_REG_ADDRESS(0x01C)	/* ccr		*/
#define IO_IMASK_REG		IO_REG_ADDRESS(0x104)	/* int_mask	*/
#define IO_IVECT_REG		IO_REG_ADDRESS(0x10C)	/* int_vect	*/
#define IO_IEOI_REG		IO_REG_ADDRESS(0x114)	/* int_eoi	*/
#define IO_IVMI_REG		IO_REG_ADDRESS(0x11C)	/* int_vmi	*/
#define IO_SYS_CTL_REG		IO_REG_ADDRESS(0x204)	/* sys_ctl	*/
#define IO_SHMEM_CTL_REG	IO_REG_ADDRESS(0x20C)	/* shmem_ctl	*/
#define IO_SHMEM_STAT_REG	IO_REG_ADDRESS(0x214)	/* shmem_stat	*/
#define IO_NIM_STAT_REG		IO_REG_ADDRESS(0x21C)	/* nim_stat	*/
#define IO_SYS_STAT0_REG	IO_REG_ADDRESS(0x304)	/* sys_stat0	*/
#define IO_SYS_STAT1_REG	IO_REG_ADDRESS(0x30C)	/* sys_stat1	*/
#define IO_SYS_STAT2_REG	IO_REG_ADDRESS(0x314)	/* sys-stat2	*/
#define IO_COOKIE_REG		IO_REG_ADDRESS(0x404)	/* cookie	*/
#define IO_STATIC_IN_REG	IO_REG_ADDRESS(0x40C)	/* static_in	*/
#define IO_ASIC_REG		IO_REG_ADDRESS(0x414)	/* asic_reg	*/
#define IO_CTI_REG		IO_REG_ADDRESS(0x504)	/* cti		*/

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
 * Definitions for Control Register A
 * (IO_ASIC->sys_ctl) a 16-bit read/write register
 */
#define DIAG_LED_1              0x8000 /* led ds1.  active low */
#define DIAG_LED_2              0x4000 /* led ds7.  active low */
#define DIAG_LED_3              0x2000 /* led ds9.  active low */
#define DIAG_LED_4              0x1000 /* led ds10. active low */
#define CONTROL_LED_ALL         0xf000 /* to control/reset all LEDs */
#define EPROMSZ_BITS            0xC00  /* EPROM size bits */
#define EPROM_SHIFT             10     /* shift value for EPROM size bits */
#define EPROM_512K              0x0    /* 512K of EPROM */
#define EPROM_256K              0x1    /* 256K of EPROM */
#define EPROM_128K              0x2    /* 128K of EPROM */
#define EPROM_INV               0x3    /* invalid */
#define NVRAM_SIZE_128K         0x0080 /* NVRAM size, 0 = 512K, 1 = 128K */
#define CONTROL_FLASH_VPP       0x0040 /* enable flash progr, active high */
#define CONTROL_RUNLAMP         0x0010 /* sys run light, active low */
#define CONTROL_MEMORY1         0x0004 /* cpu memory config bit 1 
					  (see defines below) */
#define CONTROL_MEMORY0         0x0002 /* cpu memory config bit 0 
					  (see defines below) */
#define CONTROL_MEM_MASK        0x000E /* cpu memory config bit mask 
					  (see defines below) */
#define CONTROL_MEM_SHIFT       1      /* cpu memory shift value */

/*
 * CPU memory defines
 */
#define CONTROL_128MEG_MEM       0x00
#define CONTROL_32MEG_MEM        0x01
#define CONTROL_8MEG_MEM         0x02
#define CONTROL_2MEG_MEM         0x03
/* new for rev 1 hardware, 2m becomes 16m */
#define CONTROL_16MEG_MEM        0x03
/* new for rev 4 hardware, a new register bit is defined for 64m */
#define CONTROL_64MEG_MEM        0x04

/*
 * Definitions for Control Register B
 * (IO_ASIC->shmem_ctl) a 16-bit read/write register
 */
#define SHMEM_SLOT_RESET   0x8000  /* Reset all slots (active low) */
#define SHMEM_SLOT3_LED    0x4000  /* Slot 3 status LED (active lo) */
#define SHMEM_SLOT2_LED    0x2000  /* Slot 2 status LED (active lo) */
#define SHMEM_SLOT1_LED    0x1000  /* Slot 1 status LED (active lo) */
#define SHMEM_SLOT_LEDS    0x7000  /* All of the slot status LEDs 
				      (active lo) */
#define SHMEM_SLOT1_BIT	   12	   /* Slot 0 LED Bit position */
#define SHMEM_CONFIG0      0x0800  /* Shared memory configuration bit */
#define SHMEM_CONFIG1      0x0400  /* Shared memory configuration bit */
#define SHMEM_SIZE_BITS    0x0c00  /* Shared memory configuration info */
#define SHMEM_SIZ_SHIFT    10      /* Shared memory configuration info */
#define SHMEM_PARITY_ODD   0x0200  /* Shared memory parity sex 
				      (ODD active hi) */
#define SHMEM_PERR_CLEAR   0x0100  /* Shared memory parity error clear 
				      (write lo then hi) */

#define SHMEM_16MEG        0x0
#define SHMEM_4MEG         0x1
#define SHMEM_1MEG         0x2
#define SHMEM_8MEG         0x3

/*
 * Definitions for Status Register A
 * (IO_ASIC->shmem_stat) a 16-bit read only register
 */
#define SHMEM_SLOT3_PERR   0x80  /* Parity error caused by slot 3 access 
				    (active hi) */
#define SHMEM_SLOT2_PERR   0x40  /* Parity error caused by slot 2 access 
				    (active hi) */
#define SHMEM_SLOT1_PERR   0x20  /* Parity error caused by slot 1 access 
				    (active hi) */
#define SHMEM_MPU_PERR     0x10  /* Parity error caused by MPU access 
				    (active hi) */
#define SHMEM_BYTE3_PERR   0x08  /* Parity error in byte 3 (active hi) */
#define SHMEM_BYTE2_PERR   0x04  /* Parity error in byte 2 (active hi) */
#define SHMEM_BYTE1_PERR   0x02  /* Parity error in byte 1 (active hi) */
#define SHMEM_BYTE0_PERR   0x01  /* Parity error in byte 0 (active hi) */
#define SHMEM_PARITY_MASK  0xff  /* Mask for all of the parity information */

/*
 * Definitions for Status Register B
 * (IO_ASIC->nim_stat) a 16-bit read only register
 */
#define NMS_NIM1_LIRQ  0x01    /* NIM 1 low-priority interrupt request 
				  (active low) */
#define NMS_NIM2_LIRQ  0x02    /* NIM 2 low-priority interrupt request 
				  (active low) */
#define NMS_NIM3_LIRQ  0x04    /* NIM 3 low-priority interrupt request 
				  (active low) */
#define NMS_NIM1_HIRQ  0x010   /* NIM 1 high-priority interrupt request 
				  (active low) */
#define NMS_NIM2_HIRQ  0x020   /* NIM 2 high-priority interrupt request 
				  (active low) */
#define NMS_NIM3_HIRQ  0x040   /* NIM 3 high-priority interrupt request 
				  (active low) */
#define NMS_NIM_HIRQ_SH 4      /* NIMS high-prior int req bits shift value */
#define NMS_NIM1_PRES  0x2000  /* NIM 1 present (active low) */
#define NMS_NIM2_PRES  0x4000  /* NIM 2 present (active low) */
#define NMS_NIM3_PRES  0x8000  /* NIM 3 present (active low) */

/*
 * Definitions for Config Control Status Register A
 * (IO_ASIC->sys_stat0) a 16-bit read or write register
 */
#define SS0_HWREV_BITS 0x0f    /* hardware revision bits */
#define SS0_BSSIZ_BITS 0x70    /* boot SIMM size bits (see below) */
#define SS0_BSSIZ_SH   4       /* boot SIMM size shift value */
#define SS0_BSIMM_PRES 0x80    /* boot SIMM present (active low) */
#define SS0_BSIMM_2BNK 0x100   /* 1st boot SIMM has 2 banks (active low) */
#define SS0_FLSIZ_BITS 0xe00   /* flash size bits (see below) */
#define SS0_FLSIZ_SH   9       /* flash size shift value */
#define SS0_FLSIZ_MASK 0x0007  /* flash mask value */
#define SS0_FSIM1_PRES 0x1000  /* flash SIMM 1 present (active low) */
#define SS0_FSIM1_2BNK 0x2000  /* flash SIMM 1 has 2 banks (active low) */
#define SS0_FSIM2_PRES 0x4000  /* flash SIMM 2 present (active low) */
#define SS0_FSIM2_2BNK 0x8000  /* flash SIMM 2 has 2 banks (active low) */

#define SS0_SSIZ_1M    0x00    /* 1 mb SIMM size */
#define SS0_SSIZ_2M    0x01    /* 2 mb SIMM size */
#define SS0_SSIZ_4M    0x02    /* 4 mb SIMM size */
#define SS0_SSIZ_8M    0x03    /* 8 mb SIMM size */
#define SS0_SSIZ_16M   0x04    /* 16 mb SIMM size */
#define SS0_SSIZ_32M   0x05    /* 32 mb SIMM size */

/*
 * Definitions for Configurable Control Status Register B
 * (IO_ASIC->sys_stat1) a 8-bit read or write register
 */
#define CONTROL_OVER_TEMP	0x02 /* over temp, 0 if over temp */
#define CONTROL_LOCTO		0x01 /* Local transaction timeout, active 0 */

/*
 * Definitions for Configurable Control Status Register C
 * (IO_ASIC->sys_stat2) a 8-bit read or write register
 */
#define SS2_CLR_IOWBE 0x04  /* clear I/O write bus error, active low */
#define SS2_CLR_SMWBE 0x02  /* clear shared mem write bus error, active low */
#define SS2_CLR_MMWBE 0x01  /* clear main mem write bus error, active low */
#define SS2_CLR_ALLBE 0x07  /* clear all write bus errors */

#define SS2_LO_ENABLE 0x40  /* enable Lo NIM interrupts, active low */
#define SS2_HI_ENABLE 0x80  /* enable Hi NIM interrupts, active hi  */

/*
 * Definitions for Configurable Control Status Register C
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


#endif /* !__NEVASIC_H__ */
