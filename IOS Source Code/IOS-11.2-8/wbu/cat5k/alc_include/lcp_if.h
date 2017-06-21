/* $Id: lcp_if.h,v 1.1.4.1 1996/05/09 14:55:36 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/lcp_if.h,v $
 *-----------------------------------------------------------------
 * lcp_if.h -- ATM line-card LCP (driver) API definitions.
 *
 * March 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:36  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:13  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:11:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:06  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:34  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.17  1995/08/10  21:00:04  rbau
 * modified LCP_DIAG_SIZ to accommodate the new PMD infos in RESET_ACK message
 *
 * Revision 1.16  1995/08/01  00:23:57  rbau
 * Add stuff for diag results to be returned to NMP.  As per developer
 * request.
 *
 * Revision 1.15  1995/07/12  23:21:54  rlowe
 * Moved some simpler extern API definitions to new files "lcp_api.h".
 * This helps clients of these APIs avoid having to include some header
 * files from syn_common.
 *
 * Revision 1.14  1995/06/29  17:27:44  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.13  1995/06/27  20:23:57  rlowe
 * Force use of new LCP support.
 *
 * Revision 1.12  1995/06/20  08:33:30  rlowe
 * Add new APIs for own ACP internal IP address, SNMP ATM interface
 * starting index, SNMP ATM interface index range.
 *
 * Revision 1.11  1995/06/09  09:48:47  rlowe
 * Allow everyone else to keep using old (FCC) LCP proms until we are ready
 * to integrate them.
 *
 * Revision 1.10  1995/06/09  08:47:35  rlowe
 * Revise definition of LCP_ACP_DRIVER_SAP to follow SAPs in scp_defs.h.
 *
 * Revision 1.9  1995/06/09  06:17:20  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 *
 * Revision 1.8  1995/05/19  21:49:42  rlowe
 * Add LCP_Ntwk_MacAddr to provide network-side MAC address block for
 * atm driver/lane startup.  Also futz around current syn_common instability.
 *
 * Revision 1.7  1995/05/13  22:34:08  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.6  1995/05/04  10:38:25  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.5  1995/05/04  07:20:26  rlowe
 * Add new APIs LCP_NMP_PhysSlotNbr and LCP_NMP_MacAddr.
 *
 * Revision 1.4  1995/04/27  01:52:27  rlowe
 * Cleanup LCP_ReadMacAddr, and LCP_PhysSlotNbr; added LCP_LogicalCardNbr.
 * Now provide a default MAC address (and logical card number) if LCP EEPROM
 * is not configured.
 *
 * Revision 1.3  1995/03/29  08:16:40  rlowe
 * Archive current LCP driver as integration continues.
 *
 * Revision 1.2  1995/03/24  06:29:10  rlowe
 * Force LCP driver init to sync on Reset Ack message.  Allow mechanism for
 * deferred/late sync.  Provide APIs to request slot number/MAC address
 * from LCP driver, as reported by LCP at init time.  Provide utility
 * routine to format LCP message headers.
 *
 * Revision 1.1  1995/03/15  10:38:20  rlowe
 * Changes for LCP driver.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef	ACP_REV0_LCP		/* Do NOT enable old LCP support. */

#ifndef _LCP_IF_H
#define _LCP_IF_H

#ifndef	__syn_ser_eeprom_H__
#include "../sysif/syn_ser_eeprom.h"
#endif

#ifndef	_SCP_MSGS_H
#include "../sysif/scp_msgs.h"
#endif

#ifndef	_SCP_DEFS_H
#include "../sysif/scp_defs.h"
#endif

#ifndef	_LCP_API_H
#include "lcp_api.h"
#endif

/* --------------------------------------------	*/
/*	Some nice constants, part of API.	*/
/* --------------------------------------------	*/

#define	LCP_MSGHDR_SIZ		(SCP_HEADER_LENGTH)		/* 005 */
#define	LCP_OHEAD_SIZ		(LCP_MSGHDR_SIZ + 1)		/* 006 */
#define	LCP_MAX_DATA_SIZ	(MAX_SCP_PKT_DATA_SIZE)		/* 249 */
#define	LCP_MAX_MSG_SIZ		(MAX_SCP_PKT_SIZE)		/* 255 */
#define	LCP_DPR_BUF_SIZ		(LCP_MAX_MSG_SIZ + 1)		/* 256 */
#define	LCP_MALLOC_SIZ		(LCP_DPR_BUF_SIZ)		/* 256 */
#define	LCP_DIAG_SIZ		(MPU_XTRA_DIAG_BYTES+ADDITIONAL_INFO_SIZE) /* 36+28 */

#ifdef	ACP_REV0_LCP					/* legacy stuff */
#define	LCP_ACP_SAP_FLAG	(0x80)				/* 0x80 */
#define	LCP_NMP_CFG_SAP		(0x01)				/* 0x01 */
#define	LCP_ACP_OWN_RESET_SAP	(0x02)				/* 0x02 */
#define	LCP_LCL_8051_SAP	(0xff)				/* 0xff */
#else
#define	LCP_NMP_CFG_SAP		(NMP_CONFIG_SAP)		/* 0x01 */
#define	LCP_ACP_DRIVER_SAP	(ALC_SCP_SAP)			/* 0x60 */
#define	LCP_ACP_OWN_RESET_SAP	(ALC_RESET_ACK_SAP)		/* 0x61 */
#define	LCP_LCL_8051_SAP	(0x00)				/* 0x00 */
#endif

typedef struct eeprom_data	tLCP_EEpromData ;
typedef MAC_ACC			tLCP_MacAddrU ;
typedef union msg_space 	tLCP_MsgBufU  ;
typedef SCPHDR			tLCP_MsgHdr   ;


/* ----------------------------	*/
/*	Driver entry points.	*/
/* ----------------------------	*/

extern s_int32 LCP_TransmitPoll (u_int8 fChkFree, u_int8 fChkAck,
				 u_int8 fOnce, tLCP_MsgBufU * pTx) ;
extern tLCP_MsgBufU * LCP_ReceivePoll (u_int8 fOnce, tLCP_MsgBufU * pRx) ;
extern tLCP_MacAddrU * LCP_ReadMacAddr (tLCP_MacAddrU * pRtn) ;
extern tLCP_MacAddrU * LCP_NMP_MacAddr (tLCP_MacAddrU * pRtn) ;
extern tLCP_MacAddrU * LCP_Ntwk_MacAddr (tLCP_MacAddrU * pRtn, u_int32 * pNbr) ;

extern tLCP_MsgBufU * LCPP_FmtHeader (tLCP_MsgBufU * pBuf, u_int8 fLocal,
				u_int8 MsgLen, u_int8 DstSap, u_int8 SrcSap,
				u_int8 MsgOpCode) ;

#endif	/* _LCP_IF_H */
