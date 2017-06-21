/* $Id: target_cisco.h,v 3.3.20.6 1996/06/16 21:17:14 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/machine/target_cisco.h,v $
 *------------------------------------------------------------------
 * target_cisco.h - Definitions for cisco specific hardware.
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: target_cisco.h,v $
 * Revision 3.3.20.6  1996/06/16  21:17:14  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.20.5  1996/05/10  23:33:41  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.3.20.4  1996/05/09  14:34:25  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.20.3.14.1  1996/04/27  07:16:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.20.3.4.1  1996/04/08  01:59:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.20.3  1996/03/28  02:15:13  hampton
 * Poison symbols for recently added platforms.  [CSCdi52864]
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/03/21  22:57:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.20.1  1996/03/18  21:11:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.1  1996/03/22  09:40:55  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.10.1  1996/03/03  21:24:17  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.8.1  1996/03/02  01:20:23  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/30  20:58:54  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.2.32.1  1996/02/29  07:49:39  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:50:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:15:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TARGET_CISCO_H__
#define __TARGET_CISCO_H__

/*
 * Include definitions for platform defines.  If you add another
 * clause here, you must also change the poison statement below!
 */

#ifdef CSC2
#include "cisco_ags.h"
#endif

#ifdef RP1
#include "cisco_rp1.h"
#endif

#ifdef RSP
#include "cisco_rsp.h"
#endif

#ifdef RVIP
#include "cisco_rvip.h"
#endif

#ifdef SVIP
#include "cisco_svip.h"
#endif

#ifdef XX
#include "cisco_c4000.h"
#endif

#ifdef PAN
#include "cisco_c3000.h"
#endif

#ifdef BRASIL
#include "../../boot/src-68-c3000/pcmap.h"
#include "../src-68-as5200/as5200_pcmap.h"
#include "cisco_c3000.h"
#endif

#ifdef SIERRA
#include "cisco_c4500.h"
#endif

#ifdef SAPPHIRE
#include "cisco_c1000.h"
#endif

#ifdef PREDATOR
#include "cisco_c7100.h"
#endif

#ifdef SYNALC
#include "cisco_alc.h"
#endif

#define LES_BOX (defined(PAN) | defined(XX) | defined(SIERRA) | \
		 defined (SAPPHIRE) | defined (SYNALC) | defined (BRASIL))

#if (CSC2 || RP1 || RSP || RVIP || SVIP || XX || PAN || BRASIL || \
     SIERRA || SAPPHIRE || PREDATOR || SYNALC)
/* nada */
#else
#pragma poison CSC2 RP1 RSP RVIP SVIP XX PAN BRASIL
#pragma poison SIERRA SAPPHIRE PREDATOR MULTIBUS
#endif

/*
 * Include definitions for processor defines
 */

#ifdef OBJECT_68
#include "cisco_68.h"
#endif

#ifdef OBJECT_36
#include "cisco_36.h"
#endif

#ifdef OBJECT_4K
#include "cisco_4k.h"
#endif

#endif /* __TARGET_CISCO_H__ */
