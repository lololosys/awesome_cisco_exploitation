/* $Id: clpd6720_internals.h,v 3.2.56.2 1996/04/03 15:43:47 wfried Exp $
 * $Source: /release/112/cvs/Xsys/filesys/clpd6720_internals.h,v $
 *------------------------------------------------------------------
 * Include file for things that are used internally to the CLPD6720 driver
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clpd6720_internals.h,v $
 * Revision 3.2.56.2  1996/04/03  15:43:47  wfried
 * CSCdi52833:  PCMCIA Flash File System not working
 * Branch: California_branch
 * Fix Predator PCMCIA flash file system support. Major issues are byte
 * swapping, and PCMCIA controller timing.
 *
 * Revision 3.2.56.1  1996/03/21  22:36:25  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/01/03  05:56:53  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration and isolate Bootflash control
 * software from PCMCIA control software.
 *
 * Revision 3.2  1995/11/17  09:07:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__clpd6720_internals_h__)
#define __clpd6720_internals_h__

/*----------------------------------------------------------------------------*
 * Index constants for the registers -- these values are written to
 * the index register to access the internal registers via the data
 * register. 
 *----------------------------------------------------------------------------*/
#define SOCKET_0_START_INDEX      0x00
#define SOCKET_0_END_INDEX        0x3f
#define SOCKET_1_START_INDEX      0x40
#define SOCKET_1_END_INDEX        0x7f
#define DEVICE_0_START_INDEX      0x00
#define DEVICE_0_END_INDEX        0x7f
#define DEVICE_1_START_INDEX      0x80
#define DEVICE_1_END_INDEX        0xff

/*----------------------------------------------------------------------------*
 * Index constants for the registers -- these values are written to
 * the index register to access the internal registers via the data
 * register. 
 *----------------------------------------------------------------------------*/
#define CHIP_REVISION_IDX          0x00
#define INTERFACE_STATUS_IDX       0x01
#define POWER_CTRL_IDX             0x02
#define INTERRUPT_CTRL_IDX         0x03
#define CARD_STATUS_CHANGE_IDX     0x04
#define MGMT_INT_CFG_IDX           0x05
#define MAPPING_ENABLE_IDX         0x06
#define IO_WINDOW_CTRL_IDX         0x07
#define MISC_CTRL_1_IDX            0x16
#define MISC_CTRL_2_IDX            0x1e
#define FIFO_CTRL_IDX              0x17
#define CHIP_INFO_IDX              0x1f
#define ATA_CTRL_IDX               0x26

#define IO_MAP_0_START_LO_IDX      0x08
#define IO_MAP_0_START_HI_IDX      0x09
#define IO_MAP_0_END_LO_IDX        0x0a
#define IO_MAP_0_END_HI_IDX        0x0b
#define IO_MAP_0_OFFSET_LO_IDX     0x36
#define IO_MAP_0_OFFSET_HI_IDX     0x37

#define IO_MAP_1_START_LO_IDX      0x0c
#define IO_MAP_1_START_HI_IDX      0x0d
#define IO_MAP_1_END_LO_IDX        0x0e
#define IO_MAP_1_END_HI_IDX        0x0f
#define IO_MAP_1_OFFSET_LO_IDX     0x38
#define IO_MAP_1_OFFSET_HI_IDX     0x39

#define MAP_0_START_LO_IDX         0x10
#define MAP_0_START_HI_IDX         0x11
#define MAP_0_END_LO_IDX           0x12
#define MAP_0_END_HI_IDX           0x13
#define MAP_0_OFFSET_LO_IDX        0x14
#define MAP_0_OFFSET_HI_IDX        0x15

#define MAP_1_START_LO_IDX         0x18
#define MAP_1_START_HI_IDX         0x19
#define MAP_1_END_LO_IDX           0x1a
#define MAP_1_END_HI_IDX           0x1b
#define MAP_1_OFFSET_LO_IDX        0x1c
#define MAP_1_OFFSET_HI_IDX        0x1d

#define MAP_2_START_LO_IDX         0x20
#define MAP_2_START_HI_IDX         0x21
#define MAP_2_END_LO_IDX           0x22
#define MAP_2_END_HI_IDX           0x23
#define MAP_2_OFFSET_LO_IDX        0x24
#define MAP_2_OFFSET_HI_IDX        0x25

#define MAP_3_START_LO_IDX         0x28
#define MAP_3_START_HI_IDX         0x29
#define MAP_3_END_LO_IDX           0x2a
#define MAP_3_END_HI_IDX           0x2b
#define MAP_3_OFFSET_LO_IDX        0x2c
#define MAP_3_OFFSET_HI_IDX        0x2d

#define MAP_4_START_LO_IDX         0x30
#define MAP_4_START_HI_IDX         0x31
#define MAP_4_END_LO_IDX           0x32
#define MAP_4_END_HI_IDX           0x33
#define MAP_4_OFFSET_LO_IDX        0x34
#define MAP_4_OFFSET_HI_IDX        0x35

#define SETUP_TIMING_0_IDX         0x3a
#define COMMAND_TIMING_0_IDX       0x3b
#define RECOVERY_TIMING_0_IDX      0x3c

#define SETUP_TIMING_1_IDX         0x3d
#define COMMAND_TIMING_1_IDX       0x3e
#define RECOVERY_TIMING_1_IDX      0x3f

/*----------------------------------------------------------------------------*
 * Fields for the CHIP REVISION register (R/O)
 *----------------------------------------------------------------------------*/
#define REVISION_MASK        0x0f

#define INTERFACE_ID_MASK    0xc0
#define INTERFACE_ID_IO      0x00
#define INTERFACE_ID_MEM     0x40
#define INTERFACE_ID_MEM_IO  0x80

/*----------------------------------------------------------------------------*
 * Fields for the INTERFACE STATUS register (R/O)
 *----------------------------------------------------------------------------*/
#define BVD_MASK             0x03
#define BVD_BATTERY_OK       0x03
#define BVD_BATTERY_LOW      0x02
/* all other values (0 and 1) mean the card data was lost */

#define CD_1_MASK            0x04
#define CD_2_MASK            0x08
#define CD_MASK              (CD_1_MASK | CD_2_MASK)
#define CD_CARD_PRESENT      0x0c
#define CD_CARD_MISSING      0x00
/* all other values (4 and 8) mean the card is partially inserted */

#define SOCKET_WRITE_PROTECT_MASK  0x10
#define READY_MASK                 0x20
#define CARD_POWER_MASK            0x40
#define VPP_VALID_MASK             0X80


/*----------------------------------------------------------------------------*
 * Fields for the POWER CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
#define VPP1_POWER_OFF      0x00
#define VPP1_POWER_VCC      0x01
#define VPP1_POWER_12V      0x02
#define VCC_POWER_ENABLE    0x10
#define AUTO_POWER_ENABLE   0x20
#define CARD_ENABLE         0x80

/*----------------------------------------------------------------------------*
 * Fields for the INTERRUPT AND GENERAL CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
#define IRQ_LEVEL_MASK        0x0f
#define MGMT_INT_ON_IRQ       0x00
#define MGMT_INT_ON_INTR      0x10
#define CARD_IS_MEM           0x00
#define CARD_IS_IO            0x20
#define CARD_RESET_RELEASE    0x40
#define BVD1_IS_RI            0x80
#define BVD1_IS_STATUS_CHANGE 0x00

/*----------------------------------------------------------------------------*
 * Fields for the CARD STATUS CHANGE register (R/O)
 *----------------------------------------------------------------------------*/
/*
 * only one of these first two defines can be used at a time -- it
 * depends on how the BVD1/STSCHG line is configured (see Interrupt
 * Control register)
 */
#define BATTERY_DEAD_CHANGE_MASK            0x01
#define STATUS_CHANGE_MASK                  0x01

#define BATTERY_WARNING_CHANGE_MASK         0x02
#define READY_CHANGE_MASK                   0x04
#define CARD_DETECT_CHANGE_MASK             0x08

/*----------------------------------------------------------------------------*
 * Fields for the MANAGEMENT INTERRUPT CONFIGURATION register (R/W)
 *----------------------------------------------------------------------------*/
/*
 * only one of these first two defines can be used at a time -- it
 * depends on how the BVD1/STSCHG line is configured (see Interrupt
 * Control register)
 */
#define BATTERY_DEAD_CHANGE_ENABLE     0x01
#define STATUS_CHANGE_ENABLE           0x01
#define BATTERY_WARNING_CHANGE_ENABLE  0x02
#define READY_CHANGE_ENABLE            0x04
#define CARD_DETECT_CHANGE_ENABLE      0x08
#define MGMT_IRQ_MASK                  0xf0
#define MGMT_IRQ3                      0x30
#define MGMT_IRQ_SHIFT                 4    /* shift 4 bits left when writing */

/*----------------------------------------------------------------------------*
 * Constants for use in the IRQ fields.  For use in
 *   Mangement Interrupt Configuration Register
 *   Interrupt and General Control Register 
 *----------------------------------------------------------------------------*/
#define IRQ_DISABLE               0x00
#define IRQ_3                     0x03
#define IRQ_4                     0x04
#define IRQ_5                     0x05
#define IRQ_7                     0x07
#define IRQ_9                     0x09
#define IRQ_10                    0x0a
#define IRQ_11                    0x0b
#define IRQ_12                    0x0c
#define IRQ_14                    0x0e
#define IRQ_15                    0x0f

/*----------------------------------------------------------------------------*
 * Fields for the MAPPING ENABLE register (R/W)
 *----------------------------------------------------------------------------*/
#define MAP_ALL_DISABLE      0x00
#define MAP_0_ENABLE         0x01
#define MAP_1_ENABLE         0x02
#define MAP_2_ENABLE         0x04
#define MAP_3_ENABLE         0x08
#define MAP_4_ENABLE         0x10
#define IO_MAP_0_ENABLE      0x40
#define IO_MAP_1_ENABLE      0x80

#define MAP_0_MASK           0x01
#define MAP_1_MASK           0x02
#define MAP_2_MASK           0x04
#define MAP_3_MASK           0x08
#define MAP_4_MASK           0x10
#define IO_MAP_0_MASK        0x40
#define IO_MAP_1_MASK        0x80

/*----------------------------------------------------------------------------*
 * Fields for the IO WINDOW CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
#define IO_WINDOW_0_SIZE_8_BIT          0x00
#define IO_WINDOW_0_SIZE_16_BIT         0x01

#define IO_WINDOW_0_FIXED_SIZE          0x00
#define IO_WINDOW_0_AUTO_SIZE           0x02

#define IO_WINDOW_0_TIMING_SELECT_0     0x00
#define IO_WINDOW_0_TIMING_SELECT_1     0x08

#define IO_WINDOW_1_SIZE_8_BIT          0x00
#define IO_WINDOW_1_SIZE_16_BIT         0x10

#define IO_WINDOW_1_FIXED_SIZE          0x00
#define IO_WINDOW_1_AUTO_SIZE           0x20

#define IO_WINDOW_1_TIMING_SELECT_0     0x00
#define IO_WINDOW_1_TIMING_SELECT_1     0x80


/*----------------------------------------------------------------------------*
 * Fields for the MEMORY MAP START ADDRESS HIGH (upper bits) register (R/W)
 *----------------------------------------------------------------------------*/
#define MEM_WINDOW_DATA_SIZE_8_BIT      0x00
#define MEM_WINDOW_DATA_SIZE_16_BIT     0x80

/*----------------------------------------------------------------------------*
 * Fields for the MEMORY MAP END ADDRESS HIGH (upper bits) register (R/W)
 *----------------------------------------------------------------------------*/
#define CARD_TIMER_SELECT_MASK   0xc0
#define CARD_TIMER_SELECT_0      0x00
#define CARD_TIMER_SELECT_1      0x40

/*----------------------------------------------------------------------------*
 * Fields for the MEMORY MAP ADDRESS OFFSET HIGH (upper bits) register (R/W)
 *----------------------------------------------------------------------------*/
#define WRITE_ENABLE            0x00
#define WRITE_PROTECT           0x80
#define WRITE_PROTECT_MASK      0x80

#define REG_PIN_INACTIVE        0x00
#define REG_PIN_ACTIVE          0x40
#define COMMON_MEM              REG_PIN_INACTIVE
#define ATTRIBUTE_MEM           REG_PIN_ACTIVE

/*----------------------------------------------------------------------------*
 * Fields for the MISC CONTROL 1 register (R/W)
 *----------------------------------------------------------------------------*/
#define POWER_5V_DETECT_MASK      0x01

#define VCC_5V                    0x00
#define VCC_3V                    0x02

#define MGMT_INT_LEVEL            0x00
#define MGMT_INT_PULSE            0x04

#define SYS_IRQ_LEVEL             0x00
#define SYS_IRQ_PULSE             0x08

#define SPEAKER_DISABLE           0x00
#define SPEAKER_ENABLE            0x10

#define INPACK_IGNORED            0x00
#define INPACK_ENABLE             0x80

/*----------------------------------------------------------------------------*
 * Fields for the FIFO CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
/* use when reading the register for FIFO status */
#define FIFO_STATUS_MASK      0x80
#define FIFO_NOT_EMPTY        0x00
#define FIFO_EMPTY            0x80

/* use when writing the register for FIFO control */
#define FIFO_FLUSH            0x80

/*----------------------------------------------------------------------------*
 * Fields for the MISC CONTROL 2 register (R/W)
 *----------------------------------------------------------------------------*/
#define NORMAL_CLOCK              0x00
#define BYPASS_FREQ_SYNTH         0x01

#define RUN_CLOCK_ALWAYS          0x00
#define STOP_CLOCK_WHEN_POSSIBLE  0x02
#define LOW_POWER_DYNAMIC_MODE    STOP_CLOCK_WHEN_POSSIBLE

#define SUSPEND_POWER_MODE        0x04
#define NORMAL_POWER_MODE         0x00

#define VDD_3V_CORE_MODE          0x00
#define VDD_5V_CORE_MODE          0x08

#define IRQ12_NORMAL_MODE         0x00
#define IRQ12_LED_MODE            0x10

#define BIT7_NORMAL_MODE          0x00
#define BIT7_TRISTATE_MODE        0x20

#define NON_DMA_MODE              0x00
#define DMA_MODE                  0x40

#define IRQ15_NORMAL_MODE         0x00
#define IRQ15_RI_MODE             0x80

/*----------------------------------------------------------------------------*
 * Fields for the CHIP INFORMATION register (R/O)
 *----------------------------------------------------------------------------*/
#define CHIP_REV_LEVEL_MASK      0x3c
#define CHIP_REV_LEVEL_SHIFT     2

#define DUAL_SLOT_MASK           0x40

#define CHIP_ID_MASK             0xc0

/*----------------------------------------------------------------------------*
 * Fields for the ATA CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
#define ATA_MODE_ENABLE       0x01
#define ATA_MODE_DISABLE      0x00

#define SPEAKER_IS_LED_INPUT  0x02

#define ATA_ADDR_BITS_MASK    0xfc

/*----------------------------------------------------------------------------*
 * Fields for the DMA CONTROL register (R/W)
 *----------------------------------------------------------------------------*/
#define SOCKET_PULLUP_ENABLE      0x00
#define SOCKET_PULLUP_DISABLE     0x20

#define DMA_DISABLE               0x00
#define DMA_ENABLE_INPACK         0x40
#define DMA_ENABLE_WP_IOIS16      0x80
#define DMA_ENABLE_BVD2_SPKR      0xc0

/*----------------------------------------------------------------------------*
 * Fields for the SETUP TIMING register
 * Fields for the COMMAND TIMING register
 * Fields for the RECOVERY TIMING register
 *----------------------------------------------------------------------------*/
#define PRESCALAR_MASK      0xc0
#define PRESCALAR_1         0x00
#define PRESCALAR_16        0x40
#define PRESCALAR_256       0x80
#define PRESCALAR_4096      0xc0


/*----------------------------------------------------------------------------*
 * constants for window mapping in clpd6720.c
 *----------------------------------------------------------------------------*/
#define MAX_LOCAL_ADDR       0x00ffffff
#define MAX_PCMCIA_ADDR      0x03ffffff

#define LOCAL_ADDR_MASK      0x00ffffff
#define PCMCIA_ADDR_MASK     0x03ffffff

#define WINDOW_ADDR_MASK     0x00fff000
#define WINDOW_ADDR_LO_MASK  0x000ff000
#define WINDOW_ADDR_LO_SHIFT         12
#define WINDOW_ADDR_HI_MASK  0x00f00000
#define WINDOW_ADDR_HI_SHIFT         20

#define WINDOW_OFFSET_MASK     0x03fff000
#define WINDOW_OFFSET_LO_MASK  0x000ff000
#define WINDOW_OFFSET_LO_SHIFT         12
#define WINDOW_OFFSET_HI_MASK  0x03f00000
#define WINDOW_OFFSET_HI_SHIFT         20

#endif
