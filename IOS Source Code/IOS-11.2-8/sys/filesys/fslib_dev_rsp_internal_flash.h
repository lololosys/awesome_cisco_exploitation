/* $Id: fslib_dev_rsp_internal_flash.h,v 3.2 1995/11/17 09:08:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib_dev_rsp_internal_flash.h,v $
 *------------------------------------------------------------------
 * Include file for flash file system RSP internal flash driver
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_rsp_internal_flash.h,v $
 * Revision 3.2  1995/11/17  09:08:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_dev_rsp_internal_flash_h__)
#define __fslib_dev_rsp_internal_flash_h__


/*----------------------------------------------------------------------------*
 * Commands for the Intel Flash Chips
 *----------------------------------------------------------------------------*/
#define RSPFLASH_BANK_SIZE           (4 * 1024 * 1024)

/*----------------------------------------------------------------------------*
 * Commands for the Intel Flash Chips
 *----------------------------------------------------------------------------*/
#define RSPFLASH_CMD_READ_ARRAY        0xff
#define RSPFLASH_CMD_INTELLIGENT_ID    0x90
#define RSPFLASH_CMD_READ_STATUS_REG   0x70
#define RSPFLASH_CMD_CLEAR_STATUS_REG  0x50
#define RSPFLASH_CMD_BYTE_WRITE        0x40
#define RSPFLASH_CMD_BLOCK_ERASE       0x20
#define RSPFLASH_CMD_ERASE_CONFIRM     0xd0
#define RSPFLASH_CMD_ERASE_SUSPEND     0xb0

/*----------------------------------------------------------------------------*
 * Definitions of the status bits in the status register
 *----------------------------------------------------------------------------*/
#define RSPFLASH_STATUS_WSMS              0x80    /* Write State Machine Status */
#  define RSPFLASH_STATUS_WSMS_READY      0x80
#  define RSPFLASH_STATUS_WSMS_BUSY       0x00
#define RSPFLASH_STATUS_ESS               0x40    /* Erase Suspend Status */
#  define RSPFLASH_STATUS_ESS_SUSPENDED   0x40
#  define RSPFLASH_STATUS_ESS_COMPLETED   0x00
#define RSPFLASH_STATUS_ES                0x20    /* Erase Status */
#  define RSPFLASH_STATUS_ES_ERROR        0x20
#  define RSPFLASH_STATUS_ES_OK           0x00
#define RSPFLASH_STATUS_BWS               0x01    /* Byte Write Status */
#  define RSPFLASH_STATUS_BWS_ERROR       0x01
#  define RSPFLASH_STATUS_BWS_OK          0x00
#define RSPFLASH_STATUS_VPPS              0x08    /* Vpp Status */
#  define RSPFLASH_STATUS_VPPS_LOW        0x08
#  define RSPFLASH_STATUS_VPPS_OK         0x00

/*----------------------------------------------------------------------------*
 * Macro to create a 4-byte wide command from a single byte command.
 * This is used often becasue the RSP flash is only 32-bit accessable.
 *----------------------------------------------------------------------------*/
#define RSPFLASH_CMD_32(cmd) (((cmd)<<24)|((cmd)<<16)|((cmd)<<8)|(cmd))

/*----------------------------------------------------------------------------*
 * External declarations of the functions
 *----------------------------------------------------------------------------*/
extern int rsp_internal_flash_read_fn (int dev_num, fslib_ptr src, char *dst, ulong length);
extern int rsp_internal_flash_write_fn (int dev_num, char *src, fslib_ptr dst, ulong length);
extern int rsp_internal_flash_erase_fn (int dev_num, fslib_ptr sector, ulong length);
extern int rsp_internal_flash_init_fn (int dev_num);
extern int rsp_internal_flash_show_info_fn (int dev_num);


#endif /* !defined(__fslib_dev_rsp_internal_flash_h__) */
