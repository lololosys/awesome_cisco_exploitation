/* $Id: fslib_dev_intel_series_2plus.h,v 3.2 1995/11/17 09:08:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib_dev_intel_series_2plus.h,v $
 *------------------------------------------------------------------
 * Include file for the flash file system driver for Intel Series 2+ Cards
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_intel_series_2plus.h,v $
 * Revision 3.2  1995/11/17  09:08:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_dev_intel_series_2plus_h__)
#define __fslib_dev_intel_series_2plus_h__

/*----------------------------------------------------------------------------*
 * Flash command values
 *----------------------------------------------------------------------------*/
/* these are the command that work for the 28F008 and 28F016 chips */
#define IFLASH_CMD_READ_ARRAY        0xff
#define IFLASH_CMD_INTELLIGENT_ID    0x90
#define IFLASH_CMD_READ_STATUS_REG   0x70
#define IFLASH_CMD_CLEAR_STATUS_REG  0x50
#define IFLASH_CMD_BYTE_WRITE        0x40
#define IFLASH_CMD_BLOCK_ERASE       0x20
#define IFLASH_CMD_ERASE_CONFIRM     0xd0
#define IFLASH_CMD_ERASE_SUSPEND     0xb0

/* these are the command that work for the 28F016 chips only */
#define IFLASH_CMD_READ_EXT_STATUS_REG         0x71
#define IFLASH_CMD_PAGE_BUFFER_SWAP            0x72
#define IFLASH_CMD_READ_PAGE_BUFFER            0x75
#define IFLASH_CMD_SINGLE_LOAD_TO_PAGE_BUFFER  0x74
#define IFLASH_CMD_SEQ_LOAD_TO_PAGE_BUFFER     0xe0
#define IFLASH_CMD_PAGE_BUFFER_WRITE_TO_FLASH  0x0c
#define IFLASH_CMD_LOAD_BLOCK                  0x77
#define IFLASH_CMD_UPLOAD_STATUS_BITS          0x97
#define IFLASH_CMD_UPLOAD_DEVICE_INFO          0x99
#define IFLASH_CMD_ERASE_UNLOCKED_BLOCKS       0xa7
#define IFLASH_CMD_RYBY_ENABLE_LEVEL_MODE      
#define IFLASH_CMD_RYBY_PULSE_ON_WRITE         
#define IFLASH_CMD_RYBY_PULSE_ON_ERASE         
#define IFLASH_CMD_RYBY_DISABLE                
#define IFLASH_CMD_SLEEP                       0xe0
#define IFLASH_CMD_ABORT                       0x80

/*----------------------------------------------------------------------------*
 * Offsets when using the extened status registers
 *----------------------------------------------------------------------------*/
#define IFLASH_BLOCK_STATUS_OFFSET             0x04
#define IFLASH_GLOBAL_STATUS_OFFSET            0x08

/*----------------------------------------------------------------------------*
 * Sector size for the card
 *----------------------------------------------------------------------------*/
#define IFLASH_SECTOR_SIZE                    (1024 * 128)
#define IFLASH_BANK_SIZE                      (IFLASH_SECTOR_SIZE * 32)

/*----------------------------------------------------------------------------*
 * Status register definitions
 *----------------------------------------------------------------------------*/
#define IFLASH_STATUS_VPP
#define IFLASH_STATUS_DATA_WRITE
#define IFLASH_STATUS_ERASE
#define IFLASH_STATUS_ERASE_SUSPEND
#define IFLASH_STATUS_WRITE_STATE_MACHINE

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory control registers
 *----------------------------------------------------------------------------*/
#define IFLASH_CONFIG_OPTION_ATTRIB_OFFSET   0x4000
#define IFLASH_CONFIG_STATUS_ATTRIB_OFFSET   0x4002
#define IFLASH_CARD_STATUS_ATTRIB_OFFSET     0x4100
#define IFLASH_WRITE_PROTECT_ATTRIB_OFFSET   0x4104
#define IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET 0x410c
#define IFLASH_READY_BUSY_ATTRIB_OFFSET      0x4140

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory CONFIGURATION OPTION register
 *----------------------------------------------------------------------------*/
#define IFLASH_CFGOPT_SRESET                  0x80
#define IFLASH_CFGOPT_LEVLREQ                 0x40
#define IFLASH_CFGOPT_CONFIG_INDEX_MASK       0x3f
#define IFLASH_CFGOPT_CONFIG_INDEX_VCC5_VPP0     1
#define IFLASH_CFGOPT_CONFIG_INDEX_VCC5_VPP12    2
#define IFLASH_CFGOPT_CONFIG_INDEX_VCC3_VPP0     3
#define IFLASH_CFGOPT_CONFIG_INDEX_VCC3_VPP12    4

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory CARD CONFIGURATION AND STATUS register
 *----------------------------------------------------------------------------*/
#define IFLASH_CFGSTS_PWRDWN                  0x04

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory CARD STATUS register
 *----------------------------------------------------------------------------*/
#define IFLASH_CARDSTS_SRESET                 0x20
#define IFLASH_CARDSTS_CMWP                   0x10
#define IFLASH_CARDSTS_PWRDWN                 0x08
#define IFLASH_CARDSTS_CISWP                  0x04
#define IFLASH_CARDSTS_WP                     0x02
#define IFLASH_CARDSTS_RDYBSY                 0x01

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory WRITE PROTECTION register
 *----------------------------------------------------------------------------*/
#define IFLASH_WRPROT_BLKEN                   0x04
#define IFLASH_WRPROT_CMWP                    0x02
#define IFLASH_WRPROT_CISWP                   0x01

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory VOLTAGE CONTROL register
 *----------------------------------------------------------------------------*/
#define IFLASH_VOLTCTRL_VCC_LEVEL_MASK        0x80
#define IFLASH_VOLTCTRL_VCC_LEVEL_3V          0x80
#define IFLASH_VOLTCTRL_VCC_LEVEL_5V          0x00
#define IFLASH_VOLTCTRL_VPP_VALID             0x02
#define IFLASH_VOLTCTRL_VPP_GEN               0x01

/*----------------------------------------------------------------------------*
 * Defines for the attribute memory READY/BUSY MODE register
 *----------------------------------------------------------------------------*/
#define IFLASH_RDYBSY_RACK                    0x02
#define IFLASH_RDYBSY_MODE_MASK               0x01
#define IFLASH_RDYBSY_MODE_HIPERF             0x01
#define IFLASH_RDYBSY_MODE_PCMCIA             0x00

/*----------------------------------------------------------------------------*
 * Defines for the common memory COMPATIBLE STATUS register
 *----------------------------------------------------------------------------*/
#define IFLASH_COMPAT_STATUS_WSMS             0x80
#define IFLASH_COMPAT_STATUS_ESS              0x40
#define IFLASH_COMPAT_STATUS_ES               0x20
#define IFLASH_COMPAT_STATUS_DWS              0x10
#define IFLASH_COMPAT_STATUS_VPPS             0x08

/*----------------------------------------------------------------------------*
 * Defines for the common memory GLOBAL STATUS register
 *----------------------------------------------------------------------------*/
#define IFLASH_GLOBAL_STATUS_WSMS             0x80
#define IFLASH_GLOBAL_STATUS_OSS              0x40
#define IFLASH_GLOBAL_STATUS_DOS              0x20
#define IFLASH_GLOBAL_STATUS_DSS              0x10
#define IFLASH_GLOBAL_STATUS_QS               0x08
#define IFLASH_GLOBAL_STATUS_PBAS             0x04
#define IFLASH_GLOBAL_STATUS_PBS              0x02
#define IFLASH_GLOBAL_STATUS_PBSS             0x01

/*----------------------------------------------------------------------------*
 * Defines for the common memory BLOCK STATUS register
 *----------------------------------------------------------------------------*/
#define IFLASH_BLOCK_STATUS_BS                0x80
#define IFLASH_BLOCK_STATUS_BLS               0x40
#define IFLASH_BLOCK_STATUS_BOS               0x20
#define IFLASH_BLOCK_STATUS_BOAS              0x10
#define IFLASH_BLOCK_STATUS_QS                0x08
#define IFLASH_BLOCK_STATUS_VPPS              0x04

/*----------------------------------------------------------------------------*
 * External declarations of the driver functions
 *----------------------------------------------------------------------------*/
extern int intel_series_2plus_read_fn (int dev_num, fslib_ptr src, char *dst, ulong length);
extern int intel_series_2plus_write_fn (int dev_num, char *src, fslib_ptr dst, ulong length);
extern int intel_series_2plus_erase_fn (int dev_num, fslib_ptr sector, ulong length);
extern int intel_series_2plus_init_fn (int dev_num);
extern int intel_series_2plus_show_info_fn (int dev_num);


#endif /* !defined(__fslib_dev_intel_series_2plus_h__) */
