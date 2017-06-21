/* $Id: gt64010.h,v 3.2.58.4 1996/07/18 17:57:54 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/gt64010.h,v $
 *------------------------------------------------------------------
 * gt64010.h - Galileo system control for R4000
 *
 * June, 1995, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gt64010.h,v $
 * Revision 3.2.58.4  1996/07/18  17:57:54  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/18  17:52:18  ssangiah
 * CSCdi59308:  PCMCIA removal/insertion crashes the system
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/05/24  00:17:09  mbeesley
 * CSCdi57936:  GT64010 Errors should reload the box
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:46:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.11  1996/02/26  18:44:49  szhang
 * CSCdi49349:  Predator ROM monitor needs to support packed NVRAM
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/02/19  23:24:10  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.2.2.9  1996/02/12  19:14:07  szhang
 * Branch: ELC_branch
 * Move the GT64010 register initial values to pred_plat_defs.h
 *
 * Revision 3.2.2.8  1996/01/26  22:10:29  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.2.2.7  1996/01/09  21:00:53  mbeesley
 * Branch: ELC_branch
 * Fix naming of PCI memory address config registers.
 *
 * Revision 3.2.2.6  1996/01/05  23:23:37  mansonw
 * Branch: ELC_branch
 *
 * Change Init value for Galileo for memory sizing.
 *
 * Revision 3.2.2.5  1995/12/09  20:48:49  mbeesley
 * Branch: ELC_branch
 * Fix UART address, GT64010 Vendor/Dev ID and DRAM config registers.
 *
 * Revision 3.2.2.4  1995/12/08  05:01:01  mbeesley
 * Branch: ELC_branch
 * Get NVRAM bank config correct. Fix some defines for PCI registers.
 *
 * Revision 3.2.2.3  1995/12/05  19:58:43  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization.
 *
 * Revision 3.2.2.2  1995/12/05  00:54:11  mbeesley
 * Branch: ELC_branch
 * Add support for the L2 cache.
 *
 * Revision 3.2.2.1  1995/11/22  21:42:51  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:40:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/07  16:42:17  szhang
 * Initial placeholder commit for Predator system controller gt64010
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GT64010_H__
#define __GT64010_H__

/*
 * Defines for the GT64010 internal registers
 */

#ifndef ASMINCLUDE

#define PAD unsigned char

typedef struct gt64010_regs_t_ {

    /**********************************************************
     * CPU Interface
     **********************************************************/

    /* 0x000: 
     * CPU interface configuration 
     */
    volatile unsigned long cpu_interface_conf;
    PAD pad1[4];

    /**********************************************************
     * Processor Address Space
     **********************************************************/

    /* 0x008: 
     * RAS[1:0] low decode address 
     */
    volatile unsigned long ras10_low;
    PAD pad2[4];

    /* 0x010:
     * RAS[1:0] high decode address 
     */
    volatile unsigned long ras10_high;
    PAD pad3[4];

    /* 0x018:
     * RAS[3:2] low decode address 
     */
    volatile unsigned long ras32_low;
    PAD pad4[4];

    /* 0x020:
     * RAS[3:2] high decode address 
     */
    volatile unsigned long ras32_high;
    PAD pad5[4];

    /* 0x028:
     * CS[2:0] low decode address 
     */
    volatile unsigned long cs20_low;
    PAD pad6[4];

    /* 0x030:
     * CS[2:0] high decode address
     */
    volatile unsigned long cs20_high;
    PAD pad7[4];

    /* 0x038:
     * CS[3] and Boot CS low decode address
     */
    volatile unsigned long cs3_boot_low;
    PAD pad8[4];

    /* 0x040:
     * CS[3] and Boot CS high decode address
     */
    volatile unsigned long cs3_boot_high;
    PAD pad9[4];

    /* 0x048:
     * PCI I/O low decode address
     */
    volatile unsigned long pci_io_low;
    PAD pad10[4];

    /* 0x050:
     * PCI I/O high decode address
     */
    volatile unsigned long pci_io_high;
    PAD pad11[4];

    /* 0x058:
     * PCI memory low decode address
     */
    volatile unsigned long pci_mem_low;
    PAD pad12[4];

    /*
     * 0x060:
     * PCI memory high decode address
     */
    volatile unsigned long pci_mem_high;
    PAD pad13[4];

    /* 0x068:
     * Internal space decode
     */
    volatile unsigned long internal_spc_decode;
    PAD pad14[4];

    /* 0x070:
     * Bus error address low
     */
    volatile unsigned long bus_err_low;
    PAD pad15[4];

    /* 0x078:
     * Bus error address high
     */
    volatile unsigned long bus_err_high;
    PAD pad16[0x384];

    /**********************************************************
     * DRAM and Device Address Space registers
     **********************************************************/

    /* 0x400:
     * RAS[0] low decode address
     */
    volatile unsigned long ras0_low;

    /* 0x404:
     * RAS[0] high decode address
     */
    volatile unsigned long ras0_high;

    /* 0x408:
     * RAS[1] low decode address
     */
    volatile unsigned long ras1_low;

    /* 0x40C:
     * RAS[1] high decode address
     */
    volatile unsigned long ras1_high;

    /* 0x410:
     * RAS[2] low decode address
     */
    volatile unsigned long ras2_low;

    /* 0x414:
     * RAS[2] high decode address
     */
    volatile unsigned long ras2_high;

    /* 0x418:
     * RAS[3] low decode address
     */
    volatile unsigned long ras3_low;
    
    /* 0x41C:
     * RAS[3] high decode address
     */
    volatile unsigned long ras3_high;

    /* 0x420:
     * CS[0] low decode address
     */
    volatile unsigned long cs0_low;

    /* 0x424:
     * CS[0] high decode address
     */
    volatile unsigned long cs0_high;

    /* 0x428:
     * CS[1] low decode address
     */
    volatile unsigned long cs1_low;

    /* 0x42C:
     * CS[1] high decode address
     */
    volatile unsigned long cs1_high;

    /* 0x430:
     * CS[2] low decode address
     */
    volatile unsigned long cs2_low;

    /* 0x434:
     * CS[2] high decode address
     */
    volatile unsigned long cs2_high;

    /* 0x438:
     * CS[3] low decode address
     */
    volatile unsigned long cs3_low;

    /* 0x43C:
     * CS[3] high decode address
     */
    volatile unsigned long cs3_high;

    /* 0x440:
     * Boot CS low decode address
     */
    volatile unsigned long boot_cs_low;
    
    /* 0x444:
     * Boot CS high decode address
     */
    volatile unsigned long boot_cs_high;

    /**********************************************************
     * DRAM Configuration
     **********************************************************/

    /*
     * 0x448:
     * DRAM configuration register
     */
    volatile unsigned long dram_config;

    /**********************************************************
     * DRAM Parameters
     **********************************************************/

    /* 0x44C:
     * DRAM bank 0 parameter
     */
    volatile unsigned long dram_bank0_param;

    /* 0x450:
     * DRAM bank 1 parameter
     */
    volatile unsigned long dram_bank1_param;

    /* 0x454:
     * DRAM bank 2 parameter
     */
    volatile unsigned long dram_bank2_param;

    /* 0x458:
     * DRAM bank 3 parameter
     */
    volatile unsigned long dram_bank3_param;

    /**********************************************************
     * Device Parameters
     **********************************************************/

    /* 0x45C:
     * Device bank 0 parameters
     */
    volatile unsigned long device_bank0_param;

    /* 0x460:
     * Device bank 1 parameters
     */
    volatile unsigned long device_bank1_param;

    /* 0x464:
     * Device bank 2 parameters
     */
    volatile unsigned long device_bank2_param;

    /* 0x468:
     * Device bank 3 parameters
     */
    volatile unsigned long device_bank3_param;

    /* 0x46C:
     * Device boot bank parameters
     */
    volatile unsigned long boot_bank_param;

    /**********************************************************
     * Address decode error
     **********************************************************/

    /* 0x470:
     * Address Decode error
     */
    volatile unsigned long addr_decode_err;
    PAD pad17[0x38C];

    /**********************************************************
     * DMA record
     **********************************************************/

    /* 0x800:
     * Channel 0 DMA byte count
     */
    volatile unsigned long ch0_dma_count;

    /* 0x804:
     * Channel 1 DMA byte count
     */
    volatile unsigned long ch1_dma_count;

    /* 0x808:
     * Channel 2 DMA byte count
     */
    volatile unsigned long ch2_dma_count;

    /* 0x80C:
     * Channel 3 DMA byte count
     */
    volatile unsigned long ch3_dma_count;

    /* 0x810:
     * Channel 0 DMA source address
     */
    volatile unsigned long ch0_dma_src;

    /* 0x814:
     * Channel 1 DMA source address
     */
    volatile unsigned long ch1_dma_src;

    /* 0x818:
     * Channel 2 DMA source address
     */
    volatile unsigned long ch2_dma_src;

    /* 0x81C:
     * Channel 3 DMA source address
     */
    volatile unsigned long ch3_dma_src;

    /* 0x820:
     * Channel 0 DMA destination address
     */
    volatile unsigned long ch0_dma_dst;

    /* 0x824:
     * Channel 1 DMA destination address
     */
    volatile unsigned long ch1_dma_dst;

    /* 0x828:
     * Channel 2 DMA destination address
     */
    volatile unsigned long ch2_dma_dst;

    /* 0x82C:
     * Channel 3 DMA destination address
     */
    volatile unsigned long ch3_dma_dst;

    /* 0x830:
     * Channel 0 DMA next record pointer
     */
    volatile unsigned long ch0_next_ptr;

    /* 0x834:
     * Channel 1 DMA next record pointer
     */
    volatile unsigned long ch1_next_ptr;

    /* 0x838:
     * Channel 2 DMA next record pointer
     */
    volatile unsigned long ch2_next_ptr;

    /* 0x83C:
     * Channel 3 DMA next record pointer
     */
    volatile unsigned long ch3_next_ptr;

    /**********************************************************
     * DMA Control
     **********************************************************/

    /* 0x840:
     * Channel 0 DMA control
     */
    volatile unsigned long ch0_ctrl;

    /* 0x844:
     * Channel 1 DMA control
     */
    volatile unsigned long ch1_ctrl;

    /* 0x848:
     * Channel 2 DMA control
     */
    volatile unsigned long ch2_ctrl;

    /* 0x84C:
     * Channel 3 DMA control
     */
    volatile unsigned long ch3_ctrl;

    /**********************************************************
     * Timer/Counter Control
     **********************************************************/
    
    /* 0x850:
     * Timer/Counter 0
     */
    volatile unsigned long ct0;

    /* 0x854:
     * Timer/Counter 1
     */
    volatile unsigned long ct1;

    /* 0x858:
     * Timer/Counter 2
     */
    volatile unsigned long ct2;

    /* 0x85C:
     * Timer/Counter 3
     */
    volatile unsigned long ct3;

    /**********************************************************
     * DMA Arbiter Control
     **********************************************************/

    /* 0x860:
     * Arbiter control
     */
    volatile unsigned long arbiter_ctrl;

    /**********************************************************
     * Timer/Counter Control
     **********************************************************/

    /* 0x864:
     * Timer/counter control
     */
    volatile unsigned long counter_ctrl;
    PAD pad18[0x398];

    /**********************************************************
     * PCI Internal
     **********************************************************/

    /* 0xC00:
     * PCI command
     */
    volatile unsigned long pci_cmd;

    /* 0xC04:
     * PCI time out
     */
    volatile unsigned long pci_timeout;

    /* 0xC08:
     * PCI RAS[1:0] Size
     */
    volatile unsigned long pci_ras10_size;

    /* 0xC0C:
     * PCI RAS[3:2] Size
     */
    volatile unsigned long pci_ras32_size;

    /* 0xC10:
     * PCI CS[2:0] Size
     */
    volatile unsigned long pci_cs20_size;

    /* 0xC14:
     * PCI CS[3:Boot] Size
     */
    volatile unsigned long pci_cs3_boot_size;

    /**********************************************************
     * Interrupts
     **********************************************************/

    /* 0xC18:
     * Interrupt Cause
     */
    volatile unsigned long pci_int_cause;

    /* 0xC1C:
     * CPU Interrupt Mask
     */
    volatile unsigned long pci_cpu_mask;
    PAD pad19[4];

    /* 0xC24:
     * PCI Interrupt mask
     */
    volatile unsigned long pci_pci_mask;

    /**********************************************************
     * PCI Internal
     **********************************************************/
    
    /* 0xC28:
     * PCI Serr Mask
     */
    volatile unsigned long pci_serr_mask;
    PAD pad20[8];

    /* 0xC34:
     * PCI Interrupt acknowledge
     */
    volatile unsigned long pci_int_ack;
    PAD pad21[0xc0];

    /**********************************************************
     * PCI Configuration
     **********************************************************/

    /* 0xCF8:
     * PCI configuration address
     */
    volatile unsigned long pci_conf_addr;

    /* 0xCFC:
     * PCI configuration data
     */
    volatile unsigned long pci_conf_data;

} gt64010_regs_t;

/*
 * typedef the gt64010 PIC config registers
 */
typedef struct gt64010_pci_regs_t_ {
    
    /* 0x000:
     * Device / Vendor ID
     */
    volatile unsigned long dev_ven_id;

    /* 0x004:
     * Status / Command
     */
    volatile unsigned long stat_cmd;

    /* 0x008:
     * Class code / revision ID
     */
    volatile unsigned long class_rev_id;

    /* 0x00C:
     * Header, Latency, Cacheline
     */
    volatile unsigned long latency_timer;

    /* 0x010:
     * RAS[1:0] Base address
     */
    volatile unsigned long ras10_base;

    /* 0x014:
     * RAS[3:2] Base address
     */
    volatile unsigned long ras32_base;

    /* 0x018:
     * CS[2:0] Base address
     */
    volatile unsigned long cs20_base;

    /* 0x01C:
     * CS[3] and Boot Base address
     */
    volatile unsigned long cs3_base;
    
    /* 0x020:
     * Memory Map base
     */
    volatile unsigned long memmapbase;

    /* 0x024:
     * IO Map base
     */
    volatile unsigned long iomapbase;
    PAD pad1[0x14];

    /*
     * 0x03C:
     * Interrupt pin and line
     */
    volatile unsigned long intpin_line;

} gt64010_pci_regs_t;

#undef PAD

/*
 * Define a pointer to the system control Galileo GT64010
 */
#define KSEG1_GT64010       	((gt64010_regs_t *)(KSEG1_ADRSPC_GT64010_BASE))
#define GT64010_REGS		((gt64010_regs_t *)(ADRSPC_GT64010))

#else  /* ASMINCLUDE */

#define KSEG1_GT64010_ADDR(a)   (0xB4000000 + (a))

/*
 * define the register address for assembly language code 
 */

/*
 * Processor Interface Registers
 */
#define KSEG1_GT64010_CPU_INTERFACE_CONF  KSEG1_GT64010_ADDR(0x000) /* CPU interface config */
#define KSEG1_GT64010_RAS10_LOW           KSEG1_GT64010_ADDR(0x008) /* RAS[1:0] low decode address */
#define KSEG1_GT64010_RAS10_HIGH          KSEG1_GT64010_ADDR(0x010) /* RAS[1:0] high decode address */
#define KSEG1_GT64010_RAS32_LOW           KSEG1_GT64010_ADDR(0x018) /* RAS[3:2] low decode address */
#define KSEG1_GT64010_RAS32_HIGH          KSEG1_GT64010_ADDR(0x020) /* RAS[3:2] high decode address */
#define KSEG1_GT64010_CS20_LOW            KSEG1_GT64010_ADDR(0x028) /* CS[2:0] low decode address */
#define KSEG1_GT64010_CS20_HIGH           KSEG1_GT64010_ADDR(0x030) /* CS[2:0] high decode address */
#define KSEG1_GT64010_CS3_BOOT_LOW        KSEG1_GT64010_ADDR(0x038) /* CS3 low decode address */
#define KSEG1_GT64010_CS3_BOOT_HIGH       KSEG1_GT64010_ADDR(0x040) /* CS3 high decode address */
#define KSEG1_GT64010_PCI_IO_LOW          KSEG1_GT64010_ADDR(0x048) /* PCI I/O low decode address */
#define KSEG1_GT64010_PCI_IO_HIGH         KSEG1_GT64010_ADDR(0x050) /* PCI I/O high decode address */
#define KSEG1_GT64010_PCI_MEM_LOW         KSEG1_GT64010_ADDR(0x058) /* PCI memory low decode address */
#define KSEG1_GT64010_PCI_MEM_HIGH        KSEG1_GT64010_ADDR(0x060) /* PCI memory high decode address */
#define KSEG1_GT64010_INTERNAL_SPC_DECODE KSEG1_GT64010_ADDR(0x068) /* internal space decode */
#define KSEG1_GT64010_BUS_ERR_LOW         KSEG1_GT64010_ADDR(0x070) /* bus error address low */
#define KSEG1_GT64010_BUS_ERR_HIGH        KSEG1_GT64010_ADDR(0x078) /* bus error address high */

/*
 * DRAM and Device Registers 
 */
#define KSEG1_GT64010_RAS0_LOW            KSEG1_GT64010_ADDR(0x400) /* RAS0 low decode address */
#define KSEG1_GT64010_RAS0_HIGH           KSEG1_GT64010_ADDR(0x404) /* RAS0 high decode address */
#define KSEG1_GT64010_RAS1_LOW            KSEG1_GT64010_ADDR(0x408) /* RAS1 low decode address */
#define KSEG1_GT64010_RAS1_HIGH           KSEG1_GT64010_ADDR(0x40c) /* RAS1 high decode address */
#define KSEG1_GT64010_RAS2_LOW            KSEG1_GT64010_ADDR(0x410) /* RAS2 low decode address */
#define KSEG1_GT64010_RAS2_HIGH           KSEG1_GT64010_ADDR(0x414) /* RAS2 high decode address */
#define KSEG1_GT64010_RAS3_LOW            KSEG1_GT64010_ADDR(0x418) /* RAS3 low decode address */
#define KSEG1_GT64010_RAS3_HIGH           KSEG1_GT64010_ADDR(0x41c) /* RAS3 high decode address */
#define KSEG1_GT64010_CS0_LOW             KSEG1_GT64010_ADDR(0x420) /* CS0 low decode address */
#define KSEG1_GT64010_CS0_HIGH            KSEG1_GT64010_ADDR(0x424) /* CS0 high decode address */
#define KSEG1_GT64010_CS1_LOW             KSEG1_GT64010_ADDR(0x428) /* CS1 low decode address */
#define KSEG1_GT64010_CS1_HIGH            KSEG1_GT64010_ADDR(0x42c) /* CS1 high decode address */
#define KSEG1_GT64010_CS2_LOW             KSEG1_GT64010_ADDR(0x430) /* CS2 low decode address */
#define KSEG1_GT64010_CS2_HIGH            KSEG1_GT64010_ADDR(0x434) /* CS2 high decode address */
#define KSEG1_GT64010_CS3_LOW             KSEG1_GT64010_ADDR(0x438) /* CS3 low decode address */
#define KSEG1_GT64010_CS3_HIGH            KSEG1_GT64010_ADDR(0x43c) /* CS3 high decode address */
#define KSEG1_GT64010_BOOT_CS_LOW         KSEG1_GT64010_ADDR(0x440) /* Boot CS low decode address */
#define KSEG1_GT64010_BOOT_CS_HIGH        KSEG1_GT64010_ADDR(0x444) /* Boot CS high decode address */
#define KSEG1_GT64010_DRAM_CONFIG         KSEG1_GT64010_ADDR(0x448) /* DRAM configuration register */
#define KSEG1_GT64010_DRAM_BANK0_PARAM    KSEG1_GT64010_ADDR(0x44c) /* DRAM bank 0 parameter */
#define KSEG1_GT64010_DRAM_BANK1_PARAM    KSEG1_GT64010_ADDR(0x450) /* DRAM bank 1 parameter */
#define KSEG1_GT64010_DRAM_BANK2_PARAM    KSEG1_GT64010_ADDR(0x454) /* DRAM bank 2 parameter */
#define KSEG1_GT64010_DRAM_BANK3_PARAM    KSEG1_GT64010_ADDR(0x458) /* DRAM bank 3 parameter */
#define KSEG1_GT64010_DEVICE_BANK0_PARAM  KSEG1_GT64010_ADDR(0x45c) /* device bank 0 parameter */
#define KSEG1_GT64010_DEVICE_BANK1_PARAM  KSEG1_GT64010_ADDR(0x460) /* device bank 1 parameter */
#define KSEG1_GT64010_DEVICE_BANK2_PARAM  KSEG1_GT64010_ADDR(0x464) /* device bank 2 parameter */
#define KSEG1_GT64010_DEVICE_BANK3_PARAM  KSEG1_GT64010_ADDR(0x468) /* device bank 3 parameter */
#define KSEG1_GT64010_BOOT_BANK_PARAM     KSEG1_GT64010_ADDR(0x46c) /* Device boot bank parameter */


/*
 * DMA Registers
 */
#define KSEG1_GT64010_CH0_COUNT            KSEG1_GT64010_ADDR(0x800) /* channel 0 DMA byte count */
#define KSEG1_GT64010_CH1_COUNT            KSEG1_GT64010_ADDR(0x804) /* channel 1 DMA byte count */
#define KSEG1_GT64010_CH2_COUNT            KSEG1_GT64010_ADDR(0x808) /* channel 2 DMA byte count */
#define KSEG1_GT64010_CH3_COUNT            KSEG1_GT64010_ADDR(0x80c) /* channel 3 DMA byte count */
#define KSEG1_GT64010_CH0_SRC              KSEG1_GT64010_ADDR(0x810) /* channel 0 DMA source addr */
#define KSEG1_GT64010_CH1_SRC              KSEG1_GT64010_ADDR(0x814) /* channel 1 DMA source addr */
#define KSEG1_GT64010_CH2_SRC              KSEG1_GT64010_ADDR(0x818) /* channel 2 DMA source addr */
#define KSEG1_GT64010_CH3_SRC              KSEG1_GT64010_ADDR(0x81c) /* channel 3 DMA source addr */
#define KSEG1_GT64010_CH0_DST              KSEG1_GT64010_ADDR(0x820) /* channel 0 DMA dest addr */
#define KSEG1_GT64010_CH1_DST              KSEG1_GT64010_ADDR(0x824) /* channel 1 DMA dest addr */
#define KSEG1_GT64010_CH2_DST              KSEG1_GT64010_ADDR(0x828) /* channel 2 DMA dest addr */
#define KSEG1_GT64010_CH3_DST              KSEG1_GT64010_ADDR(0x82c) /* channel 3 DMA dest addr */
#define KSEG1_GT64010_CH0_NEXT_PTR         KSEG1_GT64010_ADDR(0x830) /* channel 0 DMA next rec ptr */
#define KSEG1_GT64010_CH1_NEXT_PTR         KSEG1_GT64010_ADDR(0x834) /* channel 1 DMA next rec ptr */
#define KSEG1_GT64010_CH2_NEXT_PTR         KSEG1_GT64010_ADDR(0x838) /* channel 2 DMA next rec ptr */
#define KSEG1_GT64010_CH3_NEXT_PTR         KSEG1_GT64010_ADDR(0x83c) /* channel 3 DMA next rec ptr */
#define KSEG1_GT64010_CH0_CTRL             KSEG1_GT64010_ADDR(0x840) /* channel 0 DMA control */
#define KSEG1_GT64010_CH1_CTRL             KSEG1_GT64010_ADDR(0x844) /* channel 1 DMA control */
#define KSEG1_GT64010_CH2_CTRL             KSEG1_GT64010_ADDR(0x848) /* channel 2 DMA control */
#define KSEG1_GT64010_CH3_CTRL             KSEG1_GT64010_ADDR(0x84c) /* channel 3 DMA control */
#define KSEG1_GT64010_CT0                  KSEG1_GT64010_ADDR(0x850) /* timer/counter 0 */
#define KSEG1_GT64010_CT1                  KSEG1_GT64010_ADDR(0x854) /* timer/counter 1 */
#define KSEG1_GT64010_CT2                  KSEG1_GT64010_ADDR(0x858) /* timer/counter 2 */
#define KSEG1_GT64010_CT3                  KSEG1_GT64010_ADDR(0x85c) /* timer/counter 3 */
#define KSEG1_GT64010_ARBITER_CTRL         KSEG1_GT64010_ADDR(0x860) /* arbitor control */
#define KSEG1_GT64010_COUNTER_CTRL         KSEG1_GT64010_ADDR(0x864) /* timer/counter control */
#define KSEG1_GT64010_DMA_ERR_ADDR         KSEG1_GT64010_ADDR(0x868) /* DMA error address */

/*
 * PCI Internal Registers
 */
#define KSEG1_GT64010_PCI_CMD              KSEG1_GT64010_ADDR(0xc00) /* PCI command */
#define KSEG1_GT64010_PCI_TIMEOUT          KSEG1_GT64010_ADDR(0xc04) /* PCI time out */
#define KSEG1_GT64010_RAS10_SIZE           KSEG1_GT64010_ADDR(0xc08) /* PCI RAS[1:0] Size */
#define KSEG1_GT64010_RAS32_SIZE           KSEG1_GT64010_ADDR(0xc0c) /* PCI RAS[3:2] Size */
#define KSEG1_GT64010_CS10_SIZE            KSEG1_GT64010_ADDR(0xc10) /* PCI CS[1:0] Size */
#define KSEG1_GT64010_CS32_SIZE            KSEG1_GT64010_ADDR(0xc14) /* PCI CS[3:2] Size */
#define KSEG1_GT64010_INT_CAUSE            KSEG1_GT64010_ADDR(0xc18) /* PCI interrupt cause */
#define KSEG1_GT64010_CPU_MASK             KSEG1_GT64010_ADDR(0xc1c) /* PCI CPU Mask */
#define KSEG1_GT64010_PCI_MASK             KSEG1_GT64010_ADDR(0xc24) /* PCI PCI Mask */
#define KSEG1_GT64010_SERR_MASK            KSEG1_GT64010_ADDR(0xc28) /* PCI SErr Mask */
#define KSEG1_GT64010_INT_ACK              KSEG1_GT64010_ADDR(0xc34) /* PCI interrupt acknowledge */
#define KSEG1_GT64010_CONF_ADDR            KSEG1_GT64010_ADDR(0xcf8) /* PCI configuration address */
#define KSEG1_GT64010_CONF_DATA            KSEG1_GT64010_ADDR(0xcfc) /* PCI configuration data */

#endif /* ASMINCLUDE */

/*
 * Defines for the CPU Interface Configuration
 */
#define CPU_CONFIG_SCACHE		0x20000
#define CPU_CONFIG_BLOCK_MODE		0x40000
#define CPU_CONFIG_R4K_MODE		0x80000
#define CPU_CONFIG_LITTLE_ENDIAN	0x100000
	
/*
 * Defines for DRAM Configuration 
 */
#define DRAM_CONFIG_NON_STGRF		0x100
#define DRAM_CONFIG_ADS_ONLY		0x200
#define DRAM_CONFIG_EXTERN_LATCH	0x400

/*
 * Defines for DRAM Bank0 Parameters
 */
#define DRAM_BANK_CASW_2CYCLE		0x1000000
#define DRAM_BANK_RAS2CASW_3CYCLE	0x2000000
#define DRAM_BANK_CASR_2CYCLE		0x4000000
#define DRAM_BANK_RAS2CASR_3CYCLE	0x8000000
#define DRAM_BANK_XKRF_HALFK		0x0
#define DRAM_BANK_XKRF_1K		0x10000000
#define DRAM_BANK_XKRF_2K		0x20000000
#define DRAM_BANK_XKRF_4K		0x30000000
#define DRAM_BANK_BANKWDTH_64BIT	0x40000000
#define DRAM_BANK_LOC_ODD		0x80000000
#define DRAM_BANK_PARITY		0x10000

/*
 * defines for the Device Bank0 Parameter
 */
#define DEVICE_BANK_DEV_8BIT		0x0
#define DEVICE_BANK_DEV_16BIT		0x1000
#define DEVICE_BANK_DEV_32BIT		0x2000
#define DEVICE_BANK_DEV_64BIT		0x3000
#define DEVICE_BANK_ODD_BANK		0x8000
#define DEVICE_BANK_PARITY_SUPPORT	0x40

/*
 * defines for the interrupt cause register
 */
#define INT_SUMMARY                    	0x01000000
#define INT_DRAM_OUT_OF_RANGE          	0x02000000
#define INT_DMA_OUT_OF_RANGE           	0x04000000
#define INT_CPU_OUT_OF_RANGE           	0x08000000
#define INT_DMA0_COMP                  	0x10000000
#define INT_DMA1_COMP                  	0x20000000
#define INT_DMA2_COMP                  	0x40000000
#define INT_DMA3_COMP                  	0x80000000
#define INT_T0_EXP                     	0x00010000
#define INT_T1_EXP                     	0x00020000
#define INT_T2_EXP                     	0x00040000
#define INT_T3_EXP                     	0x00080000
#define INT_MASTER_READ_PAR            	0x00100000
#define INT_SLAVE_WRITE_PAR            	0x00200000
#define INT_MASTER_WRITE_PAR           	0x00400000
#define INT_SLAVE_READ_PAR             	0x00800000
#define INT_ADDRESS_PARITY             	0x00000100
#define INT_DRAM_ERROR                 	0x00000200
#define INT_MASTER_ABORT               	0x00000400
#define INT_TARGET_ABORT               	0x00000800
#define INT_RETRY_CNT_EXP              	0x00001000
#define CPU_INT_PART1                  	0x0000E000
#define CPU_INT_PART2                  	0x00000003
#define PCI_INT                        	0x0000003C
#define CPU_INT_SUMMARY                	0x00000040
#define CPU_PCI_SUMMARY                	0x00000080

/*
 * defines for ths CPU mask register
 */
#define INT_SUMMARY_MASK             	0x01000000
#define INT_DRAM_OUT_OF_RANGE_MASK     	0x02000000
#define INT_DMA_OUT_OF_RANGE_MASK    	0x04000000
#define INT_CPU_OUT_OF_RANGE_MASK      	0x08000000
#define INT_DMA0_COMP_MASK            	0x10000000
#define INT_DMA1_COMP_MASK           	0x20000000
#define INT_DMA2_COMP_MASK           	0x40000000
#define INT_DMA3_COMP_MASK           	0x80000000
#define INT_T0_EXP_MASK                	0x00010000
#define INT_T1_EXP_MASK                	0x00020000
#define INT_T2_EXP_MASK              	0x00040000
#define INT_T3_EXP_MASK              	0x00080000
#define INT_MASTER_READ_PAR_MASK     	0x00100000
#define INT_SLAVE_WRITE_PAR_MASK      	0x00200000
#define INT_MASTER_WRITE_PAR_MASK    	0x00400000
#define INT_SLAVE_READ_PAR_MASK      	0x00800000
#define INT_ADDRESS_PARITY_MASK      	0x00000100
#define INT_DRAM_ERROR_MASK          	0x00000200
#define INT_MASTER_ABORT_MASK        	0x00000400
#define INT_TARGET_ABORT_MASK        	0x00000800
#define INT_RETRY_CNT_EXP_MASK       	0x00001000
#define CPU_INT_PART1_MASK           	0x0000E000
#define CPU_INT_PART2_MASK         	0x00000003
#define PCI_INT_MASK                   	0x0000003C
#define CPU_INT_SUMMARY_MASK        	0x00000040
#define CPU_PCI_SUMMARY_MASK         	0x00000080

/*
 * Define the error interrupt mask
 */
#define GT64010_ERR_INTR_MASK		\
    (INT_DRAM_OUT_OF_RANGE_MASK |	\
     INT_DMA_OUT_OF_RANGE_MASK |	\
     INT_CPU_OUT_OF_RANGE_MASK |	\
     INT_MASTER_READ_PAR_MASK |		\
     INT_SLAVE_WRITE_PAR_MASK |		\
     INT_MASTER_WRITE_PAR_MASK |	\
     INT_SLAVE_READ_PAR_MASK |		\
     INT_ADDRESS_PARITY_MASK |		\
     INT_DRAM_ERROR_MASK |		\
     INT_MASTER_ABORT_MASK |		\
     INT_TARGET_ABORT_MASK |		\
     INT_RETRY_CNT_EXP_MASK)

/*
 * Define the CPU interrupt mask
 */
#define GT64010_CPU_INTR_MASK		\
    (INT_DMA0_COMP_MASK |		\
     INT_DMA1_COMP_MASK |		\
     INT_DMA2_COMP_MASK |		\
     INT_DMA3_COMP_MASK |		\
     INT_T0_EXP_MASK |			\
     INT_T1_EXP_MASK |			\
     INT_T2_EXP_MASK |			\
     INT_T3_EXP_MASK)

/*
 * Define the DMA mask
 */
#define GT64010_DMA_MASK		\
    (INT_DMA0_COMP |			\
     INT_DMA1_COMP |			\
     INT_DMA2_COMP |			\
     INT_DMA3_COMP)

/*
 * Define the timer mask
 */
#define GT64010_TIMER_MASK	       	\
    (INT_T0_EXP |			\
     INT_T1_EXP |			\
     INT_T2_EXP |			\
     INT_T3_EXP)

/*
 * defines for the SErr mask register
 */
#define SERR_ADDRESS_PAR_MASK         0x01000000
#define SERR_MASTER_WRITE_PAR_MASK    0x02000000
#define SERR_MASTER_READ_PAR_MASK     0x04000000
#define SERR_DRAM_ERR_MASK            0x08000000
#define SERR_MASTER_ABORT_MASK        0x10000000
#define SERR_TARGET_ABORT_MASK        0x20000000

/*
 * define GT64010 Vendor ID
 */
#define GT64010_VENDOR_ID		0xAB11
#define GT64010_DEVICE_ID		0x4601

/*
 * define GT64010 Class and Revsion ID register
 */
#define REVISION_ID                   0xFF000000
#define BASE_CLASS                    0x000000FF
#define SUB_CLASS                     0x0000FF00
#define REVISION_ID_SHIFT             24
#define SUB_CLASS_SHIFT               8

#define GT64010_REV1		      1
#define GT64010_REV2		      2

/*
 * define the offsets to the GT64010 PCI config registers
 */
#define GT64010_VENDOR_REG_OFFSET	0x000
#define GT64010_STAT_CMD_REG_OFFSET	0x004
#define GT64010_CLASS_REV_REG_OFFSET	0x008
#define GT64010_LATENCY_REG_OFFSET	0x00C
#define GT64010_RAS10_BASE_REG_OFFSET	0x010
#define GT64010_RAS32_BASE_REG_OFFSET	0x014
#define GT64010_CS20_BASE_REG_OFFSET	0x018
#define GT64010_CS3_BASE_REG_OFFSET	0x01C
#define GT64010_MEMMAPBASE_REG_OFFSET	0x020
#define GT64010_IOMAPBASE_REG_OFFSET	0x024
#define GT64010_INTPIN_LINE_REG_OFFSET	0x03C

/*
 * defines for the PCI config addr/data registers
 */
#define GT64010_REGNUM_SHIFT		24
#define GT64010_FUNC_SHIFT		16
#define GT64010_DEVNUM_SHIFT		19
#define GT64010_BUSNO_SHIFT		8
#define GT64010_CONFIG_ENABLE		0x00000080

/*
 * Timer support
 */
#define GT64010_TIMER_0			0
#define GT64010_TIMER_1			1
#define GT64010_TIMER_2			2
#define GT64010_TIMER_3			3

#define GT64010_MAX_TIMERS		4

/*
 * Macro to convert from usecs to GT64010 internal time domain.
 * This is assuming a 50Mhz external Sys.Ad bus (20ns).
 */
#define GT64010_USEC(x)			(vaxorder((x)*50))

#if !defined(ASMINCLUDE)

/*
 * Timer callback vector
 */
typedef void (*gt64010_timer_vector_t)(void);

/*
 * Timer control type
 */
typedef struct gt64010_timer_cntl_t_ {
    gt64010_timer_vector_t	func;
    volatile ulong 		*counter;
    ulong			tenable_mask;
    ulong			cenable_mask;
} gt64010_timer_cntl_t;

/*
 * Extern for gt64010 revision
 */
extern uchar	gt64010_rev_level;

/*
 * Function prototypes
 */
extern void	gt64010_init(void);
extern void	gt64010_show_pci_registers(void);
extern void	gt64010_setup_parallel_bridge(uchar);
extern void	gt64010_restore_parallel_bridge(uchar);
extern boolean	gt64010_error_interrupt(void *);
extern boolean	gt64010_errors_are_fatal(void);
extern uchar 	ralib_check_gt64010_rev(void);

/*
 * Timer / Counter support
 */
extern void	gt64010_install_timer(int, gt64010_timer_vector_t);
extern void	gt64010_start_timer(int, unsigned long);
extern void	gt64010_start_counter(int, unsigned long);
extern void	gt64010_stop_timer(int);
extern void	gt64010_stop_counter(int);

/*
 * Extern variables
 */
extern volatile tinybool pcmcia_read_flag;  /* Global PCMCIA read flag */

#endif /* ASMINCLUDE */

#endif /* __GT64010_H__ */
