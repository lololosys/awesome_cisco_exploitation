/* $Id: bcs_commonIF.h,v 1.1.4.1 1996/05/09 14:54:09 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_commonIF.h,v $
 *-----------------------------------------------------------------
 * bcs_commonIF.h -- Boot Code Service (BCS) internal interfaces.
 *		Function prototypes for common routines to both
 *		client and server sides.
 *
 *		To ensure backward compatibility with older Boot code,
 *		ADD but do NOT CHANGE any definitions given here.
 *
 * January 23, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_commonIF.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:09  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:36  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:05  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3.6.1  1996/01/18  05:02:29  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.3  1995/10/13  06:06:57  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BCS_COMMONIF_H
#define	__BCS_COMMONIF_H

#include "acp_basic_types.h"


extern u_int32 BCS_ImageProperty (register u_int32 * pHdr) ;

extern u_int32 BCS_CheckSum (register u_int32 * Addr, register u_int32 Len,
				register u_int32 Seed, u_int32 fAmBoot) ;

extern u_int8 * BCS_ShortFileName (register u_int32 * pHdr) ;
extern void BCS_DumpHdr (u_int8 * pTag, register u_int32 * pHdr) ;
extern u_int32 BCS_LkHighVersChks (u_int32 * pHdr, u_int32 CtlBits) ;
extern u_int32 BCS_LkChkFlashAddr (u_int32 Addr, u_int32 fOverride) ;
extern u_int32 BCS_LkChkTSarAddr (u_int32 Addr, u_int32 fOverride) ;
extern u_int32 BCS_LkChkRSarAddr (u_int32 Addr, u_int32 fOverride) ;
extern u_int32 BCS_LkChkDramAddr (u_int32 Addr, u_int32 fOverride) ;
extern u_int32 BCS_LkValidIBody (u_int32 * pHdr, u_int32 CtlBits, u_int32 fAmBoot) ;
extern u_int32 BCS_LkValidIHdr (register u_int32 * pHdr, u_int32 CtlBits,
				u_int32 I_Type, u_int32 fAmBoot) ;


extern u_int32 bcs_Glue1 (u_int32 BaseAddr) ;

#endif	/* __BCS_COMMONIF_H */
