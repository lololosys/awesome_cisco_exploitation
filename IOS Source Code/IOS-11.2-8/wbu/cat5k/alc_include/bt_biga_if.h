/* $Id: bt_biga_if.h,v 1.1.4.1 1996/05/09 14:55:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/bt_biga_if.h,v $
 *------------------------------------------------------------------
 * bt_biga_if.h -- Boot time BIGA interface header file for IPC.
 *
 * June 12, 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_biga_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:01  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:28  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/07/12  23:20:28  rlowe
 * Added extern for API BT_nmp_send2.
 *
 * Revision 1.2  1995/06/13  04:45:23  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.1  1995/06/12  23:32:18  sakumar
 * Added BOOT BIGA driver interface file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _BT_BIGA_IF_H_
#define _BT_BIGA_IF_H_

/*
 * External Function Prototypes
 */

extern void BT_biga_init (void);
extern int BT_nmp_send2(UINT16 portindex, UINT32 datalen, void *pData, void * pS_Hdr);
extern int BT_nmp_send(UINT16 portindex, UINT32 datalen, void *pData);
extern int BT_biga_rx_pkt(UINT8 fOnce);

#endif /* _BT_BIGA_IF_H_ */
