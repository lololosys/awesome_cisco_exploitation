/* $Id: setcache.c,v 3.1.58.1 1996/05/09 13:54:35 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/setcache.c,v $
 *------------------------------------------------------------------
 * setcache.c -- Synergy ATM line card (68ec030) cache enable routine
 *		 (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: setcache.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:35  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:35:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:27  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:28  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:28  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:28  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:09:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:12:04  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:59  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:59  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:58  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:12  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:58  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:40  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:59  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * set caches on or off, works in 4 modes:
 * $    -- toggle instr & data cache state
 * $ d  -- toggle data cache state
 * $ i  -- toggle instr cache state
 * $ 0xwhatever -- set cacr to 0xwhatever 
 *
 */

/*
 * Cache control register bits for the 68030
 */
#include "pcmap.h"
static char enamsg[] = "enabled";
static char dismsg[] = "disabled";

void setcache(cp)
char *cp;
{
    int orig, new;

    orig = getcacr();
    switch ( *cp ) {
    case '0':
	new = gethexnum(cp+2);
	break;
    case 'I':
	if(orig & CACHE_I) new = orig & ~CACHE_I;
	else new = orig | CACHE_I;
	break;
    case 'D':
	if(orig & CACHE_D) new = orig & ~CACHE_D;
	else new = orig | CACHE_D;
	break;
    case '\0':
	if(orig & CACHE_I || orig & CACHE_D) new = CLEAR_CACHES;
	else new = ENABLE_CACHES;
	break;
    default:
	printf("illegal cache command.\n");
	new = orig;
	break;
    }
    printf("set cacr to %x from %x\n",new, orig);
    printf("instruction cache %s\n", new & ENA_ICACHE ? enamsg : dismsg );
    printf("instruction burst %s\n", new & INST_BURST ? enamsg : dismsg );
    printf("data cache %s\n", new & ENA_DCACHE ? enamsg : dismsg );
    printf("data burst %s\n", new & DATA_BURST ? enamsg : dismsg );
    printf("write allocate %s\n", new & WRITE_ALLOCATE ? enamsg : dismsg );
    setcacr(new);
}
