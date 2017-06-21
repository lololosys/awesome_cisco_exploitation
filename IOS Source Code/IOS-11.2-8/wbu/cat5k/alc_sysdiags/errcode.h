/* $Id: errcode.h,v 1.1.4.1 1996/05/09 15:01:04 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/errcode.h,v $
 *------------------------------------------------------------------
 * errcode.h -- Diagnostic Error Codes
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: errcode.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:04  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:57  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:31  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains "Diagnostic Error Codes" that are included as part of   */
/* CALYPSO Diagnostics.                                                       */
/*                                                                            */
/******************************************************************************/

#define	NUMBER_OF_ERROR_CODES	75

	enum	ERROR_CODE {
	DIAG_WRITE_ERROR                                                 =   -4,
	DIAG_READ_ERROR                                                  =   -3,
	DIAG_PARITY_ERROR                                                =   -2,
	DIAG_READ_WRITE_FAIL_ERROR                                       =   -1,
	MEMDIAG_OK                                                       =    0,
	IODIAG_OK                                                        =    0,
	BIGADIAG_OK                                                      =    0,
	PHYDIAG_OK                                                       =    0,
	MACDIAG_OK                                                       =    0,
	LPBKDIAG_OK                                                      =    0,
	MEMDIAG_DATA_BUS_STUCK_1_ERROR                                   =    1,
	MEMDIAG_DATA_BUS_STUCK_0_ERROR                                   =    2,
	MEMDIAG_DATA_BUS_SHORT_ERROR                                     =    3,
	MEMDIAG_ADDRESS_BUS_STUCK_1_ERROR                                =    4,
	MEMDIAG_ADDRESS_BUS_STUCK_0_ERROR                                =    5,
	MEMDIAG_ADDRESS_BUS_SHORT_ERROR                                  =    6,
	MEMDIAG_BOOTROM_DESCRIPTOR_CHECKSUM_FAIL_ERROR                   =    7,
	MEMDIAG_BOOTROM_IMAGE_CHECKSUM_FAIL_ERROR                        =    8,
	MEMDIAG_SERIAL_EEPROM_WRITE_TIMEOUT_ERROR                        =    9,
	MEMDIAG_SERIAL_EEPROM_DATA_ERROR                                 =   10,
	MEMDIAG_SERIAL_EEPROM_CHECKSUM_FAIL_ERROR                        =   11,
	MEMDIAG_FLASH_DESCRIPTOR_CHECKSUM_FAIL_ERROR                     =   12,
	MEMDIAG_FLASH_IMAGE_CHECKSUM_FAIL_ERROR                          =   13,
	MEMDIAG_DMP_FLASH_INVALID_LENGTH_ERROR                           =   14,
	MEMDIAG_DMP_FLASH_CHECKSUM_FAIL_ERROR                            =   15,
	MEMDIAG_BOOTNVRAM_CHECKSUM_FAIL_ERROR                            =   16,
	MEMDIAG_NVRAM_CRESMIB_CHECKSUM_FAIL_ERROR                        =   17,
	MEMDIAG_NVRAM_SMTMIB_CHECKSUM_FAIL_ERROR                         =   18,
	MEMDIAG_NVRAM_READ_WRITE_TEST_FAIL_ERROR                         =   19,
	MEMDIAG_SRAM_FAST_ACCESS_TEST_FAIL_ERROR                         =   20,
	MEMDIAG_SRAM_READ_WRITE_TEST_FAIL_ERROR                          =   21,
	MEMDIAG_DRAM_FAST_ACCESS_TEST_FAIL_ERROR                         =   22,
	MEMDIAG_DRAM_READ_WRITE_TEST_FAIL_ERROR                          =   23,
	MEMDIAG_QSRAM_FAST_ACCESS_TEST_FAIL_ERROR                        =   24,
	MEMDIAG_QSRAM_READ_WRITE_TEST_FAIL_ERROR                         =   25,
	MEMDIAG_FRAMEBUF_FAST_ACCESS_TEST_FAIL_ERROR                     =   26,
	MEMDIAG_FRAMEBUF_READ_WRITE_TEST_FAIL_ERROR                      =   27,

   LPBKDIAG_LOOP1_ERROR                                           ,  /* 28 */
   LPBKDIAG_LOOP2_ERROR                                           ,  /* 29 */
   LPBKDIAG_LOOP3_ERROR                                           ,  /* 30 */
   LPBKDIAG_LOOP4_ERROR                                           ,  /* 31 */
   LPBKDIAG_VCCOPEN_ERROR                                         ,  /* 32 */
   LPBKDIAG_GETBUF_ERROR                                          ,  /* 33 */
   LPBKDIAG_NORXCELL_ERROR                                        ,  /* 34 */
   LPBKDIAG_PKTDATA_ERROR                                         ,  /* 35 */
   LPBKDIAG_SENDNMP_ERROR                                         ,  /* 36 */
   LPBKDIAG_LCPPKT_SEQ_ERROR                                      ,  /* 37 */
   LPBKDIAG_TXSAR_SENDREQ_ERROR                                   ,  /* 38 */
   LPBKDIAG_SCP_SEND_ERROR                                        ,  /* 39 */
   LPBKDIAG_SCP_RCV_ERROR                                         ,  /* 40 */
   LPBKDIAG_NOTDEFINED_ERROR1                                     ,  /* 41 */
   LPBKDIAG_NOTDEFINED_ERROR2                                     ,  /* 42 */
   LPBKDIAG_NOTDEFINED_ERROR3                                     ,  /* 43 */

   BIGADIAG_BIGA_ACCESSIBILITY_ERROR                              ,  /* 44 */
   BIGADIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR                     ,  /* 45 */
   BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR                           ,  /* 46 */
   BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR                           ,  /* 47 */
   BIGADIAG_STATUS_BIT_XMTD_NOT_SET_ERROR                         ,  /* 48 */
   BIGADIAG_STATUS_BIT_RDSB_NOT_SET_ERROR                         ,  /* 49 */
   BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR                         ,  /* 50 */
   BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR                          ,  /* 51 */
   BIGADIAG_PACKET_DATA_ERROR                                     ,  /* 52 */
   BIGADIAG_QSRAM_ARBITRATION_ERROR                               ,  /* 53 */
   BIGADIAG_FRAMEBUF_ARBITRATION_ERROR                            ,  /* 54 */

   MACDIAG_MAC_ACCESSIBILITY_ERROR                                ,  /* 55 */
   MACDIAG_MEMORY_WRITE_PTR_INCREMENT_FAIL_ERROR                  ,  /* 56 */
   MACDIAG_MEMORY_READ_PTR_INCREMENT_FAIL_ERROR                   ,  /* 57 */
   MACDIAG_MEMORY_READ_PTR_UNEXPECTED_INCREMENT_ERROR             ,  /* 58 */
   MACDIAG_INTERRUPT_TEST_FAIL_ERROR                              ,  /* 59 */
   MACDIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR                      ,  /* 60 */
   MACDIAG_CAM_NOT_CONFIGURED_ERROR                               ,  /* 61 */
   MACDIAG_CAM_DATA_ERROR                                         ,  /* 62 */
   MACDIAG_CAM_DATA_MISMATCH_ERROR                                ,  /* 63 */
   MACDIAG_MEDIA_NOT_PRESENT_ERROR                                ,  /* 64 */
   MACDIAG_RING_NOT_UP_AFTER_CLAIM_PROCESS_ERROR                  ,  /* 65 */
   MACDIAG_STATUS_BIT_SRCVFRM_STUCK_ERROR                         ,  /* 66 */
   MACDIAG_STATUS_BIT_SRCOMP_STUCK_ERROR                          ,  /* 67 */
   MACDIAG_PACKET_TRANSMIT_TIMEOUT_ERROR                          ,  /* 68 */
   MACDIAG_PACKET_RECEIVE_TIMEOUT_ERROR                           ,  /* 69 */
   MACDIAG_PACKET_DATA_ERROR                                      ,  /* 70 */
   MACDIAG_PHY_BYPASS_CONFIGURATION_TIMEOUT_ERROR                 ,  /* 71 */
   MACDIAG_PHY_LMLOOP_CONFIGURATION_TIMEOUT_ERROR                 ,  /* 72 */
   MACDIAG_PHY_EBLOOP_CONFIGURATION_TIMEOUT_ERROR                 ,  /* 73 */
   MACDIAG_PHY_XTLOOP_CONFIGURATION_TIMEOUT_ERROR                 ,  /* 74 */
   MACDIAG_PHY_PMDLOOP_CONFIGURATION_TIMEOUT_ERROR                ,  /* 75 */

   LAST_ERROR_CODE                                                  
	};

