/* $Id: cpu.c,v 3.6.4.3 1996/05/10 23:16:47 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/cpu.c,v $
 *------------------------------------------------------------------
 * cpu.c --  character array names for the CPU's
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpu.c,v $
 * Revision 3.6.4.3  1996/05/10  23:16:47  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.6.4.2  1996/05/09  13:59:38  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.6.4.1.10.1  1996/04/27  06:12:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.4.1  1996/04/05  02:41:39  yoko
 * CSCdi53681:  add new cpu types
 * Branch: California_branch
 * define new cpu types and emt calls
 *
 * Revision 3.6.20.1  1996/04/08  01:34:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.10.1  1996/03/22  09:06:43  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/03/09  23:33:34  jturner
 * CSCdi51191:  Make cpu type addition
 * Grab cpu type for new platform development.
 *
 * Revision 3.5.20.1  1996/03/03  21:18:47  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.18.1  1996/03/02  01:06:25  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/12  01:34:48  lcheung
 * add new cpu type to arkansas
 * CSCdi46793:  add new cpu type
 *
 * Revision 3.4.8.1  1996/02/29  07:41:45  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/31  23:25:03  anil
 * i86 CPU
 *
 * Revision 3.3  1995/11/20  19:35:07  mbeesley
 * CSCdi43065:  need to reserve cpu types for future development
 *
 * Revision 3.2  1995/11/17  07:40:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:13:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1.52.1.2.1  1996/02/03  06:11:28  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:27  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:11:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cpu.h"

/*
 * CPU name strings, indexed by CPU type number:
 */

char *cputypes[CPU_UNKNOWN+1] = {
	"",
	"",
	"",
	"",
	"CSC1",		/* Discontinued product */
	"UNIX",
	"CSC2",
	"",
	"CSC3",
	"STS10X",	/* Discontinued product */
	"IGS",		/* IGS with MC68020 processor */
	"3000",		/* IGS with EC68030 processor (the 3000) */
	"500-CS",	/* Discontinued product */
	"CSC4",
	"4000",         /* XX */
	"2000",		/* IGS- variants, e.g. BRUT */
	"RP1",          /* CLEAN */
	"3000",		/* Bass/Pinot IGS- variants */
	"3000",	       	/* Cristal IGS- variants */
	"2500",		/* Cancun IGS- variants */
        "4000",         /* C4000 with R4600 */
	"RSP",		/* integrated route-switch processor */
	"1500",         /* sapphire, 68360/QUICC */
	"C5kALC",	/* Catalyst 5000 (Synergy) ATM Line Card */
	"VIP",		/* R4600 based IP for 7000/7500 */
	"C7100",	/* PCI based midrange router */
	"i86",		/* Intel x86 based router	*/
        "LS1010",       /* LS1010 ATM Switch            */
	"AS5200",       /* AS5200 - Cancun superset */
        "C3500",        /* PCI based modular access router (MARs) */ 
	"",
	"",
	"",
	"",
	"",
	"",
        "BFRP",         /* 36 - BFR Route Processor */
        "BFLC",         /* 37 - BFR Line Card */
        "",
        "",
        "",
        "",
        "NITRO",        /* 42 - Dial Aggregation Card */
	"Unknown"
};

