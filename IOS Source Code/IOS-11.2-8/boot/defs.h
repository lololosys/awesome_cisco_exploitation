/* $Id: defs.h,v 3.5.20.2 1996/05/09 13:59:44 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/defs.h,v $
 *------------------------------------------------------------------
 * defs.h -- bootstrap definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs.h,v $
 * Revision 3.5.20.2  1996/05/09  13:59:44  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.5.20.1.16.1  1996/04/27  06:12:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.20.1.6.1  1996/04/08  01:34:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.20.1  1996/03/24  02:53:06  hochan
 * CSCdi52457:  Modify MINOR Version Number for 112
 * Branch: California_branch
 *
 * Revision 3.5.26.1  1996/03/22  09:06:45  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.10.1  1996/03/03  21:18:50  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.8.1  1996/03/02  01:06:28  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/30  21:10:57  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.4.20.1  1996/02/29  07:41:48  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/08  19:29:37  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.3  1995/12/02  00:19:06  clev
 * CSCdi44652:  mkpromsrec script missing in these releases
 *
 * And bump the version number.
 *
 * Revision 3.2  1995/11/17  07:41:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.18.1  1996/02/03  06:11:30  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2  1995/08/29  00:18:08  clev
 * CSCdi37286:  Boot version reported by mon_version() is incorrect
 *
 * Get the version number right.
 *
 * Revision 2.1.44.1  1995/09/11  01:39:00  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/09/02  00:19:01  rlowe
 * Provide includes for Synergy ATM line card.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:11:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GNUC__
#define	volatile
#endif

#ifndef	ASMINCLUDE
#include "types.h"
#endif	/* ASMINCLUDE */

#ifdef RP1
#include "../src-68-c7000/defs_rp1.h"
#include "../src-68-c7000/pcmap.h"
#ifndef	ASMINCLUDE
#include "../src-68-c7000/confreg.h"
#include "../src-68-c7000/buserr.h"
#include "../src-68-c7000/console.h"
#include "../src-68-c7000/debug.h"
#endif	/* ASMINCLUDE */
#else 

#ifdef CSC2
#include "../src-68-ags/defs_csc2.h"
#include "../src-68-ags/pcmap.h"
#ifndef	ASMINCLUDE
#include "../src-68-ags/confreg.h"
#include "../src-68-ags/buserr.h"
#include "../src-68-ags/console.h"
#endif	/* ASMINCLUDE */
#else

#ifdef PAN
#include "../src-68-c3000/defs_pan.h"
#include "../src-68-c3000/pcmap.h"
#ifndef	ASMINCLUDE
#include "../src-68-c3000/confreg.h"
#include "../src-68-ags/buserr.h"
#include "../src-68-c3000/cookie.h"
#include "../src-68-c3000/console.h"
#endif	/* ASMINCLUDE */
#else

#ifdef XX
#include "../src-68-c4000/defs_xx.h"
#include "../src-68-c4000/pcmap.h"
#ifndef	ASMINCLUDE
#include "../src-68-c4000/confreg.h"
#include "../src-c4000/cookie.h"
#include "../src-68-c4000/buserr.h"
#include "../src-68-c4000/console.h"
#endif	/* ASMINCLUDE */
#else

#ifdef SYNALC
#include "../src-68-alc/defs_alc.h"
#include "../src-68-alc/pcmap.h"
#ifndef ASMINCLUDE
#include "../src-68-alc/confreg.h"
#include "../src-68-ags/buserr.h"
#include "../src-68-alc/cookie.h"
#include "../src-68-alc/console.h"
#endif  /* ASMINCLUDE */
#else

#error "No CPU type was specified"

#endif SYNALC
#endif XX
#endif PAN
#endif CSC2
#endif RP1

/*
 * Version information
 */

#define	MAJORVERSION 11
#define	MINORVERSION 2

/*
 * Boolean constants
 */

#define TRUE	1
#define FALSE	0

/*
 * Here so ROM monitor can use system level include files
 */

#define	MULTIMEMBASE	MBMEMLOWADR


/*
 * Console input behavior
 */

#define	CERASE1	'\b'			/* backspace */
#define	CERASE2	'\177'			/* delete */
#define	LKILL	'\025'			/* ^U */

/*
 * Length of a system image filename :
 * (min size (as determined by flh) = 48 bytes src filename
 * + 48 bytes dst filename + 17 bytes numeric IP addr (XXX.XXX.XXX.XXX plus
 * spaces) + 1 byte null terminator.)
 */
#define FNAME_LEN	128

/*
 * States for abortstate
 */

#define	ABORT_IGNORE	0		/* ignore abort */
#define	ABORT_INTERNAL	1		/* internal rom abort */
#define	ABORT_EXTERNAL	2		/* running system code */

#define BOOTINFOSIZE	128		/* For system image to save boot
					 * information across images */
/*
 * States for system_state
 */

#define SYSTATE_MONITOR 0	/* Running the ROM monitor */
#define SYSTATE_BOOTING 1	/* Running a bootstrap */
#define SYSTATE_RUNNING 2	/* Running the system image */

/*
 * Common non-volatile memory definitions
 */

#define DISABLE_BREAK	0x0100
#define IGNORE_CONFIG   0x0040
#define BOOT_DEFAULT	0x0001

#define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT)

/*
 * Basic memory sizes
 */

#define ONE_K            0x00000400
#define TWO_K            0x00000800
#define FOUR_K           0x00001000
#define EIGHT_K          0x00002000
#define SIXTEEN_K        0x00004000
#define THIRTYTWO_K      0x00008000
#define SIXTYFOUR_K      0x00010000

#define HALF_MEG         0x00080000
#define ONE_MEG          0x00100000
#define TWO_MEG          0x00200000
#define FOUR_MEG         0x00400000
#define EIGHT_MEG        0x00800000
#define SIXTEEN_MEG      0x01000000
#define THIRTYTWO_MEG    0x02000000
#define SIXTYFOUR_MEG    0x04000000

#define ONE_GIG          0x40000000
