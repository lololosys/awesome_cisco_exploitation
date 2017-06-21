/* $Id: defs_alc.h,v 3.1.58.1 1996/05/09 13:54:21 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/defs_alc.h,v $
 *------------------------------------------------------------------
 * defs_alc.h -- Synergy ATM line card IOS ROM monitor definitions
 *		 (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs_alc.h,v $
 * Revision 3.1.58.1  1996/05/09  13:54:21  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:50  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.2  1996/03/22  23:26:44  rlowe
 * Give relative path to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.64.1  1996/03/22  09:07:16  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:15  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:13  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:16  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:53  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:49  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/02  00:01:49  rlowe
 * Fix includes so that makefile can give search paths, rather than
 * hardcoding them.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:41  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/06/09  09:19:47  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to
 * ACP.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.2  1995/05/17  00:27:42  rlowe
 * Support downloading to Rev 2 ATM line cards.  Move downloaded code base
 * from $40001000 to $40004000.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.1  1995/04/19  02:28:47  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:04  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.2  1995/02/04  06:09:30  rlowe
 * Add extra argument for MFP_MaskIntr and MFP_UnmaskIntr.  Adjust
 * to use image address constants from ACP header files.
 *
 * Revision 1.1  1995/02/01  10:59:55  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:34  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:49  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../wbu/cat5k/alc_include/acp_image_map.h"


#define	REFRESHTIME	4		/* time in milliseconds for refresh */
#define	UARTFREQUENCY	16
#define	REFRFREQUENCY	(REFRESHTIME*1000)
#define	CPUTYPE		CPU_ALC

#define	ERROR_ADDRESS	error->stack.typea.faddr

#define	BADADDRESS	0xffffffff	/* address of bad location */

#define BUSERROR	1		/* We support bus errors */

#define DCACHE		1		/* We support data cache */

#define NO_COOKIE_PROM  0xF1            /* There is no cookie PROM */

#define NO_COOKIE_PROM_CANCUN 0xFF      /* Cancun own version of no_cookie*/

#ifndef	SYNALC
/*
 * Address of 8254 registers
 */
#define	CTDEVBASEADDR	0x02120000
#endif

/*
 * Where net bootable binaries are loaded into memory
 */

#define INITSTACKP	(ARAM_IOS_KERN_TEXT)
					/* stack pointer starts (goes down) */
#ifdef	ASMINCLUDE
#else	/* C language */
extern long _end ;

extern unsigned long ALC_LoadAcpCode (unsigned char *, unsigned char *, unsigned long) ;

#define LOWCORE0 ((long *) & _end /* (ADRSPC_RAM + 0x10) ** RKL */ )
#endif
					/* start page 0 memory test here */
#define LOWCORE1 ((long *) (INITSTACKP)) /* finish page 0 mem test here */

/*
 * Serial NVRAM stuff
 */
#ifndef	SYNALC
#define X2444_PORT *(uchar *)0x02110060        /* Data register for X2444 */

#define X2444_CE 0x08           /* CE: Chip enable, also seen as CS */
#define X2444_SK 0x04           /* SK: Serial clock in */
#define X2444_DI 0x02           /* DI: Serial data in  */
#define X2444_DO 0x01           /* DO: Serial data out */
#endif
