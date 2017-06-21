
/* $Id: svip_vfpga.h,v 1.1.6.1 1996/07/08 22:43:14 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_vfpga.h,v $
 *------------------------------------------------------------------
 * svip_vfpga.h
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_vfpga.h,v $
 * Revision 1.1.6.1  1996/07/08  22:43:14  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:06  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VIP_VFPGA_H__
#define __VIP_VFPGA_H__

/*
 * Defines for the Cisco VIPER virtual  FPGA
 *
 * VIPER read I/O devices on low 32 bits of data bus. The I/O devices
 * are connected to bits 31..16, hence address 4 or C are required.
 */

#ifndef ASMINCLUDE

#define PAD unsigned char

typedef struct vip_vfpga_ {
    PAD pad0[4];
    volatile unsigned char io_ctrl_reg;		/* I/O control register */
    PAD pad1[7];
    volatile unsigned char temp_ctrl;		/* temperature control */
    PAD pad2[7];
    volatile unsigned char temp_data_write;	/* temperature data write */
    PAD pad3[7];
    volatile unsigned short serial_eprom_ctrl;	/* serial Eprom control */
    PAD pad4[6];
    volatile unsigned char reset_reason_reg;	/* reset reason register */
    PAD pad5[7];
    volatile unsigned char temp_data_read;	/* temperature data read */
    PAD pad6[7];
    volatile unsigned char watchdog_reset; 	/* watchdog reset */
    PAD pad7[7];
    volatile unsigned short flash_simm_reg;	/* flash SIMM register */
    PAD pad8[6];
    volatile unsigned long led_reg; 		/* LED reg and watchdog enable */
    PAD pad9[4];
    volatile unsigned char sw_nmi_reg;		/* Software NMI register */
    PAD pad10[7];
    volatile unsigned char env_ad_converter;    /* Environmental Mon - A/D converter */
} vip_vfpga;

#undef PAD

/* 
 * Define a pointer to the FPGA
 */
#define KSEG1_VIP_VFPGA   ((vip_vfpga *)(KSEG1_ADRSPC_VIP_VFPGA))
#define REG_FPGA	  KSEG1_VIP_VFPGA

#else                /* ASMINCLUDE */

#define VIP_VFPGA_ADDRESS(a)	(KSEG1_ADRSPC_VIP_VFPGA + a)

/*
 * VIPER VFPGA register addresses for assembly language code
 */

#define KSEG1_VFPGA_IO_CTRL_REG		VIP_VFPGA_ADDRESS(0x04)
#define KSEG1_VFPGA_TEMP_CTRL		VIP_VFPGA_ADDRESS(0x0C)
#define KSEG1_VFPGA_TEMP_DATA_WRITE	VIP_VFPGA_ADDRESS(0x14)
#define KSEG1_VFPGA_SERIAL_EPROM_CTRL	VIP_VFPGA_ADDRESS(0x1C)
#define KSEG1_VFPGA_RESET_REASON_REG	VIP_VFPGA_ADDRESS(0x24)
#define KSEG1_VFPGA_TEMP_DATA_READ	VIP_VFPGA_ADDRESS(0x2C)
#define KSEG1_VFPGA_WATCHDOG_RESET	VIP_VFPGA_ADDRESS(0x34)
#define KSEG1_VFPGA_FLASH_SIMM_REG	VIP_VFPGA_ADDRESS(0x3C)
#define KSEG1_VFPGA_LED_REG		VIP_VFPGA_ADDRESS(0x44)
#define KSEG1_VFPGA_SW_NMI_REG		VIP_VFPGA_ADDRESS(0x4C)
#define KSEG1_VFPGA_ENV_AD_CONVERTER	VIP_VFPGA_ADDRESS(0x54)

#endif /* ASMINCLUDE */

/*
 * Defines for I/O Control Register
 */
#define MUX_SELECT2		0x80
#define MUX_SELECT1		0x40
#define MUX_SELECT0		0x20
#define NVRAM_SIZE		0x08
#define DUART_RESET		0x04
#define PCMCIA_RESET		0x02
#define FLASH_PROG		0x01

/*
 * Defines for Temperature Control Register
 * Note: the resets are active with 0.
 */
#define CPU_OUTLET_CLK		0x80
#define CPU_INTLET_CLK		0x40
#define IO_OUTLET_CLK		0x20
#define IO_INLET_CLK		0x10
#define CPU_OUTLET_RST		0x08
#define CPU_INLET_RST		0x04
#define IO_OUTLET_RST		0x02
#define IO_INLET_RST		0x01

/*
 * Defines for Temperature Data Write Register
 */
#define DATA_OUT_ENABLE		0x10
#define CPU_OUTLET_DATA_OUT	0x08
#define CPU_INLET_DATA_OUT	0x04
#define IO_OUTLET_DATA_OUT	0x02
#define IO_INLET_DATA_OUT	0x01

/*
 * Defines for Serial EEPROM Control Register
 */
#define DO2_DATA_OUT_MIDPLANE	0x80
#define DO2_DATA_OUT_CPU	0x40
#define CS2_CHIP_SEL_MIDPLANE	0x20
#define SK2_CLOCK_MIDPLANE 	0x10
#define DI2_DATA_IN_MIDPLANE	0x08
#define CS1_CHIP_SEL_CPU	0x04
#define SK1_CLOCK_CPU		0x02
#define DI1_DATA_IN_CPU		0x01

/*
 * Defines for Reset Reason Register
 */
#define SW_PERIF_RST    0x8
#define SW_NMI	        0x4
#define PUSH_BUTTON_NMI 0x2
#define SW_WATCHDOG_RST 0x1

/*
 * Defines for Temperature Data Read Register 
 */

#define CPU_OUTLET_DATA_IN	0x08
#define CPU_INLET_DATA_IN	0x04
#define IO_OUTLET_DATA_IN	0x02
#define IO_INLET_DATA_IN	0x01

/*
 * Defines for Flash SIMM Register(16 bit)
 */
#define FBANK2A			0x2000
#define FSIMMA			0x1000
#define FLSH_SZ_MASK		0x0E00
#define FLSH_SZ_1MB		0
#define FLSH_SZ_2MB		1
#define FLSH_SZ_4MB		2
#define FLSH_SZ_8MB		3
#define FLSH_SZ_16MB		4
#define FLSH_SZ_32MB		5
#define HWREV			0x000F

/* 
 * Defines for LED Register
 */
#define WATCHDOG_ENABLE		0x80
#define LED3			0x08
#define LED2			0x04
#define LED1			0x02
#define LED0			0x01
#define LEDS_OFF                0x0F
#define LEDS_ON                 0xF0

#endif   /* !__VIP_VFPGA_H__ */
