/* $Id: cbi_defs.h,v 1.1.4.1 1996/05/09 14:55:34 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/cbi_defs.h,v $
 *------------------------------------------------------------------
 * cbi_defs.h -- Interface CBI/flash code from syn_common to ATM lc.
 *
 * May 30, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbi_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:34  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:03  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:31  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5  1995/07/27  22:03:46  rlowe
 * Merge ALC combined binary image support into BDG (Boot download
 * glue).
 *
 * Revision 1.4  1995/07/27  10:03:58  rlowe
 * Add function prototype for "CBI_Explode", which is the ALC embedded
 * exploder.
 *
 * Revision 1.3  1995/07/27  07:13:26  rlowe
 * Turn "yield_cpu" from macro definition into a real procedure.  Needed
 * to support a true standalone target image exploder (which uses the
 * flash driver).
 *
 * Revision 1.2  1995/06/09  06:16:34  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to ACP.
 * Setup for ACP Rev-2 HW address map.
 *
 * Revision 1.1  1995/06/02  01:02:08  rlowe
 * Changes for ACP Rev 2 HW.  Also support flash code from CBI tools for
 * download.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__CBI_DEFS_H
#define	__CBI_DEFS_H

#include "acp_basic_types.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "mfp_hw_defs.h"
#include "bt_nvram.h"
#include "mc68ec030_defs.h"


	/* These are needed by CBI clients/callers. */
typedef	u_int8	u_char ;
typedef u_int16	u_short ;
typedef	u_int32	u_long ;

#define	CBI_FIRST_FLASH_BLK	(0)
#define	CBI_NBR_FLASH_BLKS	(16)


	/* These are needed for CBI internals/porting. */
#define	FLASH_START		(ACP_HW_FLASH_BASE)

#define	CBI_FLASH_CTL		(* (volatile u_int8 *) \
				 (C_ADDR_HW_REGS (ACP_WRRG_PGM_FLASH)))

#define CF_AMD_BUSWAIT  (7)     /* Buzz delay when accessing AMD flashes. */

#define	usec10()		do { MFP_Poll_10uSec () ; } while (0)
#define msec(x)			do { MFP_Poll_mSec (x) ; } while (0)
#define	wrenable()		do { PUNCH_WD ; CBI_FLASH_CTL = ON ; } while (0)
#define	wrdisable()		do { PUNCH_WD ; CBI_FLASH_CTL = OFF ;} while (0)
#define	CBIF_RecordPgmFailure()	\
		do { \
			PUNCH_WD ; \
			++ ((tAcpBtNvram *)(BOOT_NVRAM_AREA))->FlashPgmErrs ; \
		} while (0)


	/* These are needed by our glue code for CBI. */
	/* Use ACU to inhibit data cache during data  */
	/* reads/writes in ACP flash address range.   */
	/* Instr fetches, instr cache NOT affected.   */
#define	CBI_FLASH_ACU	(CPU030_ACU_VALUE (ACP_HW_FLASH_BASE,           \
					   ACP_HW_FLASH_DECODE_AREA,    \
					   ACU_ENABLE, ACU_INHIB_CACHE, \
					   ACU_RDWR, CPU030_FC_SDATA,   \
					   ACU_FC_MSK_DATA))


	/* This is the embedded exploder main entry point. */
#define	CBI_BOOT_IF_REV		0	/* Boots only know this IF rev. */

extern long CBI_Explode (long IF_rev, char * pBuf, u_long DnldSz,
                         int (*pLogErr)(), void (*pCallBack)()) ;


#endif	/* __CBI_DEFS_H */
