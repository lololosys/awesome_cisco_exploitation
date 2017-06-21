/* $Id: diag.h,v 1.1.4.1 1996/05/09 15:00:57 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/diag.h,v $
 *------------------------------------------------------------------
 * diag.h -- 
 *
 * 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc 
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diag.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:57  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:06  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:49  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:24  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
  Modification History:
	12/15/93 Chandra - Modified MINOR_ALARM to be just FAN_BAD.  All others
	                   are under MAJOR_ALARM category.
	02/10/94 Chandra - Added MAC_EXTERNAL_FAIL & BIGA_EXTERNAL_FAIL equates
*/

#define DRAM_BYTE_TEST	0x1
#define FB_TEST		0x2
#define QSRAM_TEST	0x4
#define NVRAM_TEST	0x1000000	 /* this one is only for board debug */
#define ROM_CK_TEST	0x8
#define FLASH_CK_TEST	0x10

#define STOP_ON_ERROR 1


/* Boot Diagnostics Status bit definitions */
#define	BOOTROM_BAD	0x0001
#define	FLASH_BAD	0x0002

/* System Diagnostic Status bit definitions */
#define EEPROM_BAD	0x0001
#define NVRAM_BAD	0x0002
#define QUEUE_SRAM_BAD	0x0004
#define FRAME_BUF_BAD	0x0008
#define TEMPFAN_BAD	0x0010
#define POWER_BAD	0x0020
#define FDDI_PHY_BAD	0x0040
#define FDDI_MAC_BAD	0x0080
#define	FDDI_BIGA_BAD	0x0100
#define ETHER_BAD	0x0200
#define DMP_FLASH_BAD	0x0400
#define	DMP_SRAM_BAD	0x0800
#define	DMP_DRAM_BAD	0x1000
#define	DMP_COMM_BAD	0x2000
#define DMP_STATUS_UNKNOWN	0x4000
#define	NMP_STATUS_UNKNOWN	0x8000

#define MINOR_ALARM	(TEMPFAN_BAD)

#define MAJOR_ALARM	(EEPROM_BAD | NVRAM_BAD | QUEUE_SRAM_BAD | FRAME_BUF_BAD | POWER_BAD | FDDI_PHY_BAD | FDDI_MAC_BAD | FDDI_BIGA_BAD | ETHER_BAD | DMP_FLASH_BAD | DMP_STATUS_UNKNOWN | NMP_STATUS_UNKNOWN)


/* Power Diagnostic Status bit definitions */
#define POWER_PLUS_12_BAD	0x0001
#define POWER_MINUS_12_BAD	0x0002

/* Temperature Fan Status bit definitions */
#define	TEMPFAN_TEMP_HIGH	0x0001
#define	TEMPFAN_FAN1_BAD	0x0002
#define	TEMPFAN_FAN2_BAD	0x0004

/* PHY Status bit definitions */
#define	PHY_ACC_FAIL		0x0001
#define	PHY_LB_FAIL		0x0002

/* MAC Status bit definitions */
#define	MAC_ACC_FAIL		0x0001
#define	MAC_BUF_FAIL		0x0002
#define	MAC_INTLB_FAIL		0x0004
#define	MAC_EXTLB_FAIL		0x0008
#define	MAC_LOADGEN_FAIL	0x0010
#define	MAC_CAM_FAIL		0x0020
#define	MAC_EXTERNAL_FAIL	(MAC_EXTLB_FAIL|MAC_LOADGEN_FAIL)

/* BIGA Status bit definitions */
#define	BIGA_ACC_FAIL		0x0001
#define	BIGA_MAC_BUF_FAIL	0x0002
#define	BIGA_TXDMA_FAIL		0x0004
#define	BIGA_RXDMA_FAIL		0x0008
#define	BIGA_INTLB_FAIL		0x0010
#define	BIGA_EXTLB_FAIL		0x0020
#define	BIGA_LOADGEN_FAIL	0x0040
#define	BIGA_EXTERNAL_FAIL	(BIGA_EXTLB_FAIL|BIGA_LOADGEN_FAIL)

/* Ethernet Port Status bit definitions */
#define	ETI_ACC_FAIL		0x0001
#define	ETI_CAM_FAIL		0x0002
#define	ETI_LB_FAIL		0x0004

/* DMP Exception */
#define	COMM_DMP_TRAP		0x0E3FFC00
#define	COMM_DMP_ENCHECK_DATA_VALID	0x0E3FFF80
#define	COMM_DMP_ENCHECK		0x0E3FFF84

