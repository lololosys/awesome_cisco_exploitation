/* $Id: errinfo.c,v 1.1.4.1 1996/05/09 15:01:05 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/errinfo.c,v $
 *------------------------------------------------------------------
 * errinfo.c -- Diagnostic Error Code Information
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: errinfo.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:05  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:58  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:32  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains "Diagnostic Error Code Information" that are included as*/
/* part of CALYPSO Diagnostics.                                               */
/*                                                                            */
/******************************************************************************/

#include	"errcode.h"

	char		*diag_error_info[NUMBER_OF_ERROR_CODES+1] =
	{ "MEMDIAG_DATA_BUS_STUCK_1_ERROR",                     /* errcode: 1 */
	  "MEMDIAG_DATA_BUS_STUCK_0_ERROR",
	  "MEMDIAG_DATA_BUS_SHORT_ERROR",
	  "MEMDIAG_ADDRESS_BUS_STUCK_1_ERROR",
	  "MEMDIAG_ADDRESS_BUS_STUCK_0_ERROR",
	  "MEMDIAG_ADDRESS_BUS_SHORT_ERROR",
	  "MEMDIAG_BOOTROM_DESCRIPTOR_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_BOOTROM_IMAGE_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_SERIAL_EEPROM_WRITE_TIMEOUT_ERROR",
	  "MEMDIAG_SERIAL_EEPROM_DATA_ERROR",
	  "MEMDIAG_SERIAL_EEPROM_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_FLASH_DESCRIPTOR_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_FLASH_IMAGE_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_DMP_FLASH_INVALID_LENGTH_ERROR",
	  "MEMDIAG_DMP_FLASH_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_BOOTNVRAM_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_NVRAM_CRESMIB_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_NVRAM_SMTMIB_CHECKSUM_FAIL_ERROR",
	  "MEMDIAG_NVRAM_READ_WRITE_TEST_FAIL_ERROR",
	  "MEMDIAG_SRAM_FAST_ACCESS_TEST_FAIL_ERROR",
	  "MEMDIAG_SRAM_READ_WRITE_TEST_FAIL_ERROR",
	  "MEMDIAG_DRAM_FAST_ACCESS_TEST_FAIL_ERROR",
	  "MEMDIAG_DRAM_READ_WRITE_TEST_FAIL_ERROR",
	  "MEMDIAG_QSRAM_FAST_ACCESS_TEST_FAIL_ERROR",
	  "MEMDIAG_QSRAM_READ_WRITE_TEST_FAIL_ERROR",
	  "MEMDIAG_FRAMEBUF_FAST_ACCESS_TEST_FAIL_ERROR",
	  "MEMDIAG_FRAMEBUF_READ_WRITE_TEST_FAIL_ERROR",
     "LPBKDIAG_LOOP1_ERROR",                                   /* 28 */
     "LPBKDIAG_LOOP2_ERROR",                                   /* 29 */
     "LPBKDIAG_LOOP3_ERROR",                                   /* 30 */
     "LPBKDIAG_LOOP4_ERROR",                                   /* 31 */
     "LPBKDIAG_VCC_OPEN_ERROR",                                /* 32 */
     "LPBKDIAG_GET_BUFFER_ERROR",                              /* 33 */
     "LPBKDIAG_NO_RXCELL_ERROR",                               /* 34 */
     "LPBKDIAG_PKT_DATA_ERROR",                                /* 35 */
     "LPBKDIAG_SEND_TO_NMP_ERROR",                             /* 36 */
     "LPBKDIAG_LCPPKT_SEQ_ERROR",                              /* 37 */
     "LPBKDIAG_TXSAR_SENDREQ_ERROR",                           /* 38 */
     "LPBKDIAG_SCP_SEND_ERROR",                                /* 39 */
     "LPBKDIAG_SCP_RCV_ERROR",                                 /* 40 */
     "LPBKDIAG_NOT_DEFINED_ERROR",                             /* 41 */
     "LPBKDIAG_NOT_DEFINED_ERROR",                             /* 42 */
     "LPBKDIAG_NOT_DEFINED_ERROR",                             /* 43 */

     "BIGADIAG_BIGA_ACCESSIBILITY_ERROR",
     "BIGADIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR",
     "BIGADIAG_STATUS_BIT_XMTD_STUCK_ERROR",
     "BIGADIAG_STATUS_BIT_RDSB_STUCK_ERROR",
     "BIGADIAG_STATUS_BIT_XMTD_NOT_SET_ERROR",
     "BIGADIAG_STATUS_BIT_RDSB_NOT_SET_ERROR",
     "BIGADIAG_PACKET_TRANSMIT_TIMEOUT_ERROR",
     "BIGADIAG_PACKET_RECEIVE_TIMEOUT_ERROR",
     "BIGADIAG_PACKET_DATA_ERROR",
     "BIGADIAG_QSRAM_ARBITRATION_ERROR",
     "BIGADIAG_FRAMEBUF_ARBITRATION_ERROR",

     "MACDIAG_MAC_ACCESSIBILITY_ERROR",
     "MACDIAG_MEMORY_WRITE_PTR_INCREMENT_FAIL_ERROR",
     "MACDIAG_MEMORY_READ_PTR_INCREMENT_FAIL_ERROR",
     "MACDIAG_MEMORY_READ_PTR_UNEXPECTED_INCREMENT_ERROR",
     "MACDIAG_INTERRUPT_TEST_FAIL_ERROR",
     "MACDIAG_BUFMEM_READ_WRITE_TEST_FAIL_ERROR",
     "MACDIAG_CAM_NOT_CONFIGURED_ERROR",
     "MACDIAG_CAM_DATA_ERROR",
     "MACDIAG_CAM_DATA_MISMATCH_ERROR",
     "MACDIAG_MEDIA_NOT_PRESENT_ERROR",
     "MACDIAG_RING_NOT_UP_AFTER_CLAIM_PROCESS_ERROR",
     "MACDIAG_STATUS_BIT_SRCVFRM_STUCK_ERROR",
     "MACDIAG_STATUS_BIT_SRCOMP_STUCK_ERROR",
     "MACDIAG_PACKET_TRANSMIT_TIMEOUT_ERROR",
     "MACDIAG_PACKET_RECEIVE_TIMEOUT_ERROR",
     "MACDIAG_PACKET_DATA_ERROR",
     "MACDIAG_PHY_BYPASS_CONFIGURATION_TIMEOUT_ERROR",
     "MACDIAG_PHY_LMLOOP_CONFIGURATION_TIMEOUT_ERROR",
     "MACDIAG_PHY_EBLOOP_CONFIGURATION_TIMEOUT_ERROR",
     "MACDIAG_PHY_XTLOOP_CONFIGURATION_TIMEOUT_ERROR",
     "MACDIAG_PHY_PMDLOOP_CONFIGURATION_TIMEOUT_ERROR",

	  ""
	};

