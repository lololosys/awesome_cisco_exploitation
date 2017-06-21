/* $Id: c7100_fpga.h,v 3.2.56.5 1996/07/18 17:57:58 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_fpga.h,v $
 *------------------------------------------------------------------
 * c7100_fpga.h - Predator FPGA support
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_fpga.h,v $
 * Revision 3.2.56.5  1996/07/18  17:57:58  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.2.56.4  1996/06/22  03:33:24  mbeesley
 * CSCdi61026:  PCI PERR/SERR is now supported, use it
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/05/21  20:44:17  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.56.2  1996/05/01  13:28:22  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.56.1  1996/03/21  23:47:56  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.9  1996/02/20  02:43:07  mbeesley
 * CSCdi49348:  IOS needs to support packed NVRAM on Predator
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/15  19:52:03  dcarroll
 * CSCdi49083:  Fix c7100 envm temp sensor support
 * Branch: ELC_branch
 * Add volage, temperature and snmp fixes.
 *
 * Revision 3.2.2.7  1996/01/09  21:27:45  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.2.2.6  1996/01/03  06:06:52  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration.
 *
 * Revision 3.2.2.5  1995/12/13  02:23:48  mbeesley
 * Branch: ELC_branch
 * Fix misdocumented register bit defintions. Add error interrupt
 * masks. Clear OIR interrupts at boot time. Ensure that PCI Signals
 * Invalid is not set at port adaptor power on.
 *
 * Revision 3.2.2.4  1995/12/08  19:21:31  dcarroll
 * Branch: ELC_branch
 * OIR and ENVMON improvements.
 *
 * Revision 3.2.2.3  1995/11/30  02:14:16  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1995/11/23  19:09:50  szhang
 * Branch: ELC_branch
 * Added KSEG1 address for Midplane FPGA.
 *
 * Revision 3.2.2.1  1995/11/22  21:43:11  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C7100_FPGA_H__
#define __C7100_FPGA_H__

/*
 * Defines for the Cisco Predator FPGA
 *
 * Predator read I/O devices on low 32 bits of data bus. The I/O devices
 * are connected to bits 31..16, hence address 4 or C are required.
 */

#if !defined(ASMINCLUDE)

#define PAD unsigned char

typedef struct pred_io_fpga_ {

    /*
     * 0x1E840204 : IO control register
     */
    PAD pad0[4];
    volatile unsigned char io_ctrl_reg;

    /*
     * 0x1E84020C : Temperature Control
     */
    PAD pad1[7];
    volatile unsigned char temp_ctrl;

    /*
     * 0x1E840214 : Temperature Data Write
     */
    PAD pad2[7];
    volatile unsigned char temp_data_write;

    /*
     * 0x1E84021C : Serial EEPROM Control
     */
    PAD pad3[7];
    volatile unsigned char serial_eprom_ctrl;

    /*
     * 0x1E840224 : Reset Reason Register
     */
    PAD pad4[7];
    volatile unsigned char reset_reason_reg;

    /*
     * 0x1E84022C : Temperature Data Read
     */
    PAD pad5[7];
    volatile unsigned char temp_data_read;

    /*
     * 0x1E840234 : Watchdog Reset Register
     */
    PAD pad6[7];
    volatile unsigned char watchdog_reset;

    /*
     * 0x1E84023C : Flash SIMM Register
     */
    PAD pad7[7];
    volatile unsigned short flash_simm_reg;

    /*
     * 0x1E840244 : LED Register and Watchdog Enable
     */
    PAD pad8[6];
    volatile unsigned char led_reg;

    /*
     * 0x1E84024C : Software NMI register
     */
    PAD pad9[7];
    volatile unsigned char sw_nmi_reg;

    /*
     * 0x1E840254 : ENVM A/D Converter
     */
    PAD pad10[10];
    volatile unsigned char env_ad_converter;

} pred_io_fpga;

#undef PAD

/* 
 * Define a pointer to the IO FPGA
 */
#define KSEG1_PRED_FPGA   	((pred_io_fpga *)(KSEG1_ADRSPC_PRED_FPGA))
#define C7100_IO_FPGA		((pred_io_fpga *)(ADRSPC_IO_FPGA))

#else                /* ASMINCLUDE */

#define PRED_FPGA_ADDRESS(a)	(KSEG1_ADRSPC_PRED_FPGA + a)

/*
 * Predator FPGA register addresses for assembly language code
 */

#define KSEG1_FPGA_IO_CTRL_REG		PRED_FPGA_ADDRESS(0x04)
#define KSEG1_FPGA_TEMP_CTRL		PRED_FPGA_ADDRESS(0x0C)
#define KSEG1_FPGA_TEMP_DATA_WRITE	PRED_FPGA_ADDRESS(0x14)
#define KSEG1_FPGA_SERIAL_EPROM_CTRL	PRED_FPGA_ADDRESS(0x1C)
#define KSEG1_FPGA_RESET_REASON_REG	PRED_FPGA_ADDRESS(0x24)
#define KSEG1_FPGA_TEMP_DATA_READ	PRED_FPGA_ADDRESS(0x2C)
#define KSEG1_FPGA_WATCHDOG_RESET	PRED_FPGA_ADDRESS(0x34)
#define KSEG1_FPGA_FLASH_SIMM_REG	PRED_FPGA_ADDRESS(0x3C)
#define KSEG1_FPGA_LED_REG		PRED_FPGA_ADDRESS(0x44)
#define KSEG1_FPGA_SW_NMI_REG		PRED_FPGA_ADDRESS(0x4C)
#define KSEG1_FPGA_ENV_AD_CONVERTER	PRED_FPGA_ADDRESS(0x54)

#endif /* ASMINCLUDE */

/*
 * Defines for I/O Control Register
 */
#define MUX_SELECT2		0x80
#define MUX_SELECT1		0x40
#define MUX_SELECT0		0x20
#define NVRAM_SIZE		0x08
#define DUART_RESET		0x04
#define NVRAM_PACKED		0x04
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
#define SW_PERIF_RST    	0x8
#define SW_NMI	        	0x4
#define PUSH_BUTTON_NMI 	0x2
#define SW_WATCHDOG_RST 	0x1

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
#define FLSH_SZ_SHIFT		9
#define FLSH_SZ_1MB		0
#define FLSH_SZ_2MB		1
#define FLSH_SZ_4MB		2
#define FLSH_SZ_8MB		3
#define FLSH_SZ_16MB		4
#define FLSH_SZ_32MB		5
#define HWREV_MASK		0x000F

#define IOCARD_PACKED_NVRAM_REV	0x0004

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

/*
 * Typedef for the Midplane register set
 */

#if !defined(ASMINCLUDE)

#define PAD unsigned char

typedef struct pred_mp_fpga_ {

    /*
     * 0x00: Revision
     */
    volatile unsigned long pld_rev_lsw;
    PAD pad0[4];

    /*
     * 0x08: Test register
     */
    volatile unsigned long scratch_lsw;
    PAD pad1[4];

    /*
     * 0x10: PA network IO interrupt status
     */
    volatile unsigned long pkt_int_stat_lsw;
    PAD pad2[4];

    /*
     * 0x18: PA network IO interrupt status
     */
    volatile unsigned long pkt_int_stat_nsw;
    PAD pad3[4];

    /*
     * 0x20: PA network IO interrupt mask
     */
    volatile unsigned long pkt_int_enbl_lsw;
    PAD pad4[4];

    /*
     * 0x28: PA network IO interrupt mask
     */
    volatile unsigned long pkt_int_enbl_nsw;
    PAD pad5[4];

    /*
     * 0x30: 8KHz config
     */
    volatile unsigned long eight_khz_config;
    PAD pad6[4];

    /*
     * 0x38: 8KHz status
     */
    volatile unsigned long eight_khz_status;
    PAD pad7[4];

    /*
     * 0x40: PA management interrupt status
     */
    volatile unsigned long mgmt_int_stat_lsw;
    PAD pad8[4];

    /*
     * 0x48: PA management interrupt enable
     */
    volatile unsigned long mgmt_int_enbl_lsw;
    PAD pad9[4];

    /*
     * 0x50: Port Adaptor Status
     */
    volatile unsigned long pa_status_lsw;
    PAD pad10[4];

    /*
     * 0x58: Port Adaptor Control
     */
    volatile unsigned long pa_ctrl_lsw;
    PAD pad11[4];

    /*
     * 0x60: Port Adpator test port
     */
    volatile unsigned long pa_tst_port_lsw;
    PAD pad12[4];

    /*
     * 0x68: Port Adaptor test port
     */
    volatile unsigned long pa_tst_port_nsw;
    PAD pad13[4];

    /*
     * 0x70: Port Adaptor Error Interrupt Status
     */
    volatile unsigned long err_int_stat_lsw;
    PAD pad14[4];

    /*
     * 0x78: Port Adpator Error Interrupt Enable
     */
    volatile unsigned long err_int_enbl_lsw;
    PAD pad15[4];

} pred_mp_fpga;

#undef PAD

/* 
 * Define a pointer to the Midplane FPGA
 */
#define C7100_MP_FPGA		((pred_mp_fpga *)(ADRSPC_MP_FPGA))
#define KSEG1_C7100_MP_FPGA     ((pred_mp_fpga *)(KSEG1_ADRSPC_MP_FPGA))

#endif /* ASMINCLUDE */

/*
 * Definitions for pkt_int_stat_lsw
 */
#define PKT_INTR_STATUS_BAY0	0x000000ff	/* status for bay 0 pkt intr */
#define PKT_INTR_STATUS_BAY1	0x0000ff00	/* status for bay 1 pkt intr */
#define PKT_INTR_STATUS_BAY4	0x00ff0000	/* status for bay 4 pkt intr */
#define PKT_INTR_STATUS_BAY3	0xff000000	/* status for bay 3 pkt intr */

/*
 * Definitons for pkt_int_stat_nsw
 */
#define PKT_INTR_STATUS_BAY2	0x0000ff00	/* status for bay 2 pkt intr */
#define PKT_INTR_STATUS_BAY6	0x00ff0000	/* status for bay 6 pkt intr */
#define PKT_INTR_STATUS_BAY5	0xff000000	/* status for bay 5 pkt intr */

/*
 * Definitions for pkt_int_enbl_lsw
 */
#define PKT_INTR_ENABLE_BAY0	0x000000ff	/* enable for bay 0 pkt intr */
#define PKT_INTR_ENABLE_BAY1	0x0000ff00	/* enable for bay 1 pkt intr */
#define PKT_INTR_ENABLE_BAY4	0x00ff0000	/* enable for bay 4 pkt intr */
#define PKT_INTR_ENABLE_BAY3	0xff000000	/* enable for bay 3 pkt intr */

/*
 * Definitions for pkt_int_enbl_nsw
 */
#define PKT_INTR_ENABLE_BAY2	0x0000ff00	/* enable for bay 2 pkt intr */
#define PKT_INTR_ENABLE_BAY6	0x00ff0000	/* enable for bay 6 pkt intr */
#define PKT_INTR_ENABLE_BAY5	0xff000000	/* enable for bay 5 pkt intr */

/*
 * Definitions for mgmt_int_stat_lsw
 */
#define MGMT_INTR_STATUS_PCI_PERR   0x00000004	/* PCI parity error status */
#define MGMT_INTR_STATUS_PCI_SYSERR 0x00000008	/* PCI psystem error status */
#define MGMT_INTR_STATUS_PS0_FAIL   0x00000010  /* P.S. 0 fail status */
#define MGMT_INTR_STATUS_PS1_FAIL   0x00000020  /* P.S. 1 fail status */
#define MGMT_INTR_STATUS_SUBDEC     0x00000040	/* Subtractive decoder event */
#define MGMT_INTR_STATUS_OIRTEST    0x00000080  /* OIR test port status */

#define MGMT_INTR_STATUS_PS_FAIL 				\
    (MGMT_INTR_STATUS_PS0_FAIL | MGMT_INTR_STATUS_PS1_FAIL)

#define MGMT_INTR_STATUS_PCI_ERRORS 				\
    (MGMT_INTR_STATUS_PCI_PERR | MGMT_INTR_STATUS_PCI_SYSERR)

#define PCMCIA_INTR_STATUS	0x00000001	/* PCMCIA intr status */
#define MGMT_INTR_STATUS_BAY0	0x00000002	/* mgmt intr status, IO slot */

#define MGMT_INTR_STATUS_BAY1	0x00000300	/* mgmt intr status, bay 1 */
#define MGMT_INTR_STATUS_BAY2	0x00003000	/* mgmt intr status, bay 2 */
#define MGMT_INTR_STATUS_BAY4	0x00030000	/* mgmt intr status, bay 4 */
#define MGMT_INTR_STATUS_BAY6	0x00300000	/* mgmt intr status, bay 6 */
#define MGMT_INTR_STATUS_BAY3	0x03000000	/* mgmt intr status, bay 3 */
#define MGMT_INTR_STATUS_BAY5	0x30000000	/* mgmt intr status, bay 5 */

/*
 * Definitions for mgmt_int_enbl_lsw
 */
#define MGMT_INTR_ENABLE_PCI_PERR   0x00000004	/* PCI parity error enable */
#define MGMT_INTR_ENABLE_PCI_SYSERR 0x00000008	/* PCI psystem error enable */
#define MGMT_INTR_ENABLE_PS0_FAIL   0x00000010  /* Power Supply 0 fail enbl */
#define MGMT_INTR_ENABLE_PS1_FAIL   0x00000020  /* Power Supply 1 fail enbl */
#define MGMT_INTR_ENABLE_SUBDEC     0x00000040	/* Subtractive decoder event */
#define MGMT_INTR_ENABLE_OIRTEST    0X00000080  /* OIR test port enable */

#define MGMT_INTR_ENABLE_PS_FAIL 				\
    (MGMT_INTR_ENABLE_PS0_FAIL | MGMT_INTR_ENABLE_PS1_FAIL)

#define MGMT_INTR_ENABLE_PCI_ERRORS 				\
    (MGMT_INTR_ENABLE_PCI_PERR | MGMT_INTR_ENABLE_PCI_SYSERR)

#define PCMCIA_INTR_ENABLE	0x00000001	/* PCMCIA intr enable */
#define MGMT_INTR_ENABLE_BAY0	0x00000002	/* mgmt intr enable, IO slot */

#define MGMT_INTR_ENABLE_BAY1	0x00000300	/* mgmt intr enable, bay 1 */
#define MGMT_INTR_ENABLE_BAY2	0x00003000	/* mgmt intr enable, bay 2 */
#define MGMT_INTR_ENABLE_BAY4	0x00030000	/* mgmt intr enable, bay 4 */
#define MGMT_INTR_ENABLE_BAY6	0x00300000	/* mgmt intr enable, bay 6 */
#define MGMT_INTR_ENABLE_BAY3	0x03000000	/* mgmt intr enable, bay 3 */
#define MGMT_INTR_ENABLE_BAY5	0x30000000	/* mgmt intr enable, bay 5 */

/*
 * Definitions for pa_status_lsw
 */
#define PCI_INVALID_SIG_BAY0	0x00000001	/* IO slot signals invalid */
#define PCI_3VOK_BAY0		0x00000002	/* IO card 3V capable */
#define PCI_5VOK_BAY0		0x00000004	/* IO card 5V capable */

#define PCI_INVALID_SIG_BAY1	0x00000100	/* Bay 1 signals invalid */
#define PCI_5VOK_BAY1		0x00000200	/* Bay 1 5V capable */
#define PCI_3VOK_BAY1		0x00000400	/* Bay 1 3V capable */

#define PCI_INVALID_SIG_BAY2	0x00001000	/* Bay 2 signals invalid */
#define PCI_5VOK_BAY2		0x00002000	/* Bay 2 5V capable */
#define PCI_3VOK_BAY2		0x00004000	/* Bay 2 3V capable */

#define PCI_INVALID_SIG_BAY4	0x00010000	/* Bay 4 signals invalid */
#define PCI_5VOK_BAY4		0x00020000	/* Bay 4 5V capable */
#define PCI_3VOK_BAY4		0x00040000	/* Bay 4 3V capable */

#define PCI_INVALID_SIG_BAY6	0x00100000	/* Bay 6 signals invalid */
#define PCI_5VOK_BAY6		0x00200000	/* Bay 6 5V capable */
#define PCI_3VOK_BAY6		0x00400000	/* Bay 6 3V capable */

#define PCI_INVALID_SIG_BAY3	0x01000000	/* Bay 3 signals invalid */
#define PCI_5VOK_BAY3		0x02000000	/* Bay 3 5V capable */
#define PCI_3VOK_BAY3		0x04000000	/* Bay 3 3V capable */

#define PCI_INVALID_SIG_BAY5	0x10000000	/* Bay 5 signals invalid */
#define PCI_5VOK_BAY5		0x20000000	/* Bay 5 5V capable */
#define PCI_3VOK_BAY5		0x40000000	/* Bay 5 3V capable */

/*
 * Definitions for pa_ctrl_lsw
 */
#define IOSLOT_RESET		0x00000001	/* Reset IO slot, active low */
#define IOLED_ENABLE		0x00000002	/* IO slot LED enable */

#define POWER_ENABLE_BAY1	0x00000100	/* Enable power, bay 1 */
#define LED_ENABLE_BAY1		0x00000200	/* Enable LED, bay 1 */

#define POWER_ENABLE_BAY2	0x00001000	/* Enable power, bay 2 */
#define LED_ENABLE_BAY2		0x00002000	/* Enable LED, bay 2 */

#define POWER_ENABLE_BAY4	0x00010000	/* Enable power, bay 4 */
#define LED_ENABLE_BAY4		0x00020000	/* Enable LED, bay 4 */

#define POWER_ENABLE_BAY6	0x00100000	/* Enable power, bay 6 */
#define LED_ENABLE_BAY6		0x00200000	/* Enable LED, bay 6 */

#define POWER_ENABLE_BAY3	0x01000000	/* Enable power, bay 3 */
#define LED_ENABLE_BAY3		0x02000000	/* Enable LED, bay 3 */

#define POWER_ENABLE_BAY5	0x10000000	/* Enable power, bay 5 */
#define LED_ENABLE_BAY5		0x20000000	/* Enable LED, bay 5 */

/*
 * Defintions for pa_tst_port_lsw
 */
#define BAY0_EEPROM_CLOCK_MASK	0x00000008	/* Mask for Clock */
#define BAY0_EEPROM_SELECT_MASK	0x00000002	/* Mask for Select */
#define BAY0_EEPROM_DIN_MASK	0x00000010	/* Mask for DI */
#define BAY0_EEPROM_DOUT_MASK	0x00000040	/* Mask for DO */

#define BAY1_EEPROM_CLOCK_MASK	0x00000800	/* Mask for Clock */
#define BAY1_EEPROM_SELECT_MASK	0x00000200	/* Mask for Select */
#define BAY1_EEPROM_DIN_MASK	0x00001000	/* Mask for DI */
#define BAY1_EEPROM_DOUT_MASK	0x00004000	/* Mask for DO */

#define BAY3_EEPROM_CLOCK_MASK	0x08000000	/* Mask for Clock */
#define BAY3_EEPROM_SELECT_MASK	0x02000000	/* Mask for Select */
#define BAY3_EEPROM_DIN_MASK	0x10000000	/* Mask for DI */
#define BAY3_EEPROM_DOUT_MASK	0x40000000	/* Mask for DO */

#define BAY4_EEPROM_CLOCK_MASK	0x00080000	/* Mask for Clock */
#define BAY4_EEPROM_SELECT_MASK	0x00020000	/* Mask for Select */
#define BAY4_EEPROM_DIN_MASK	0x00100000	/* Mask for DI */
#define BAY4_EEPROM_DOUT_MASK	0x00400000	/* Mask for DO */

/*
 * Defintions for pa_tst_port_nsw
 */
#define POWER_SUPPLY_0_ENABLE   0x00000001      /* PS0 enable/shutoff */
#define POWER_SUPPLY_1_ENABLE   0x00000002      /* PS1 enable/shutoff */
#define POWER_SUPPLY_0_FAIL_RAW 0x00000004      /* PS0 raw fail bit */
#define POWER_SUPPLY_1_FAIL_RAW 0x00000008      /* PS1 raw fail bit */
#define BAY2_EEPROM_CLOCK_MASK	0x00000800	/* Mask for Clock */
#define BAY2_EEPROM_SELECT_MASK	0x00000200	/* Mask for Select */
#define BAY2_EEPROM_DIN_MASK	0x00001000	/* Mask for DI */
#define BAY2_EEPROM_DOUT_MASK	0x00004000	/* Mask for DO */

#define BAY5_EEPROM_CLOCK_MASK	0x08000000	/* Mask for Clock */
#define BAY5_EEPROM_SELECT_MASK	0x02000000	/* Mask for Select */
#define BAY5_EEPROM_DIN_MASK	0x10000000	/* Mask for DI */
#define BAY5_EEPROM_DOUT_MASK	0x40000000	/* Mask for DO */

#define BAY6_EEPROM_CLOCK_MASK	0x00080000	/* Mask for Clock */
#define BAY6_EEPROM_SELECT_MASK	0x00020000	/* Mask for Select */
#define BAY6_EEPROM_DIN_MASK	0x00100000	/* Mask for DI */
#define BAY6_EEPROM_DOUT_MASK	0x00400000	/* Mask for DO */

/*
 * Defintions for err_int_stat_lsw, err_int_stat_lsw
 */
#define C7100_GT64010_ERR_MASK	0x00000001
#define C7100_OIR_EVENT_MASK	0x0000007E
#define C7100_PKT_SRAM_ERR_MASK	0x00000080

/*
 * Defintions for err_int_enbl_lsw
 */

#endif   /* !__C7100_FPGA_H__ */

/* end of file */
