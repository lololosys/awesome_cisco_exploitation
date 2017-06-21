/* $Id: cbi_flsh_plat.h,v 1.1.4.1 1996/05/09 14:54:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/cbi_flsh_plat.h,v $
 *------------------------------------------------------------------
 * cbi_flsh_plat.h -- Encapsulate platform-dependencies for
 *			common "syndnld"/flash code.
 *
 * May 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbi_flsh_plat.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:13  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2.10.1  1996/01/23  08:49:27  rlowe
 * Suppress excessive trace and propeller output from code image
 * download and flash burn routines.
 *
 * CSCdi46640:  Excessive console output from ACP flash driver.
 * Branch: ATM21_branch
 *
 * Revision 1.2  1995/06/13  23:28:15  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.1  1995/06/02  02:24:16  rlowe
 * Make CBI/flash (target) code portable between NMP and ACP.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__CBI_FLSH_PLAT_H
#define	__CBI_FLSH_PLAT_H

#ifdef	SYNALC			/* ATM line card. */
#include "cbi_defs.h"

#define	printf	buginf

	/* ALC does NOT show propeller when burning flash. */
#define	FLSH_SHOW_PROPELLER	(0)

#else				/* NMP */
#include "types.h"
#include "ec040map.h"
#include "register.h"
#include "nvram.h"

	/* Other platforms DO show propeller when burning flash. */
#define	FLSH_SHOW_PROPELLER	(1)

#endif

#endif	/* __CBI_FLSH_PLAT_H */
