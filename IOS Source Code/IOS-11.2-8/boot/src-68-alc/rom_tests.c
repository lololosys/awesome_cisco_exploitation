/* $Id: rom_tests.c,v 3.1.58.1 1996/05/09 13:54:33 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/rom_tests.c,v $
 *------------------------------------------------------------------
 * rom_tests.c -- Synergy ATM line card special IOS ROM monitor Test menu
 *		  (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rom_tests.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:33  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:35:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:26  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:27  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:27  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:27  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:12:03  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:58  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:59  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:57  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:11  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../m68vectors.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"

extern flash_test();
extern cflash_test();

void print_menu() {
    printf("ROM Monitor Test Menu (for firmware hacks only):\n\
  H - This here help menu\n\
  F - Flash EPROM test\n\
  X - Xtended Flash EPROM test\n\
  C - Continuous Flash EPROM test\n\
  Q - Quit, return to main menu\n");
}

void rom_monitor_tests()
{
    print_menu();
    while(1) {
	printf("> ");
	flush();
	gets(bufu.buf);
	
	switch(bufu.buf[0]) {
	    
	case 'f': case 'F':
	    flash_test(0);
	    break;

	case 'x': case 'X':
	    flash_test(1);
	    break;

	case 'c': case 'C':
	    cflash_test();
	    break;

	case 'h': case 'H': case '?':
	    print_menu();
	    break;
	    
	case 'q': case 'Q':
	    return;
	    break;
	}
    }
}
