/* $Id: main_synalc.c,v 3.1.62.1 1996/05/09 14:39:53 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/os/main_synalc.c,v $
 *------------------------------------------------------------------
 * main_synalc.c -- Synergy ATM line card
 * 
 * Based on main_les.c, 31-August-1995, Rick Lowe
 * Based on main_gs.c, 8-January-1989, Kirk Lougheed
 * Hacked for Pancake, 4-December-1989, Chris Shaker
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main_synalc.c,v $
 * Revision 3.1.62.1  1996/05/09  14:39:53  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.90.1  1996/04/27  07:17:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.80.1  1996/04/08  01:59:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.68.1  1996/03/22  09:41:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.52.1  1996/03/03  21:24:36  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.50.1  1996/03/02  01:20:46  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.34.1  1996/02/29  07:49:58  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  12:46:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.32.1  1996/02/03  07:08:16  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1.2.1  1995/09/28  02:14:52  rlowe
 * Do not allow netconfig on the SYNALC; want to use default config
 * instead.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.1  1995/08/31  14:01:48  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/31  13:57:12  rlowe
 * Placeholder for Synalc2_branch.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../os/gdb.h"
#include "boxtypes.h"
#include "exception.h"


/*
 * Fundamental configuration variables
 */

/*************************************************************/
/* WARNING! Do not add any initialized variables above       */
/* data_start in this file. The code below relies on the     */
/* fact that data_start is the first initialized variable    */
/* in the system image!                                      */
/*************************************************************/
int data_start = 0;		/* Start of initialized data */


boolean romcode = ROM_ENABLE;           /* running out of RAM? */
boolean router_enable = FALSE ;		/* cannot route packets */
boolean x25_enable = X25_ENABLE;        /* commercial X.25? */
boolean bridge_enable = BRIDGE_ENABLE;  /* bridging? */
boolean srswitch_enable = SRSWITCH_ENABLE; /* srswitch */
boolean bootstrap_enable = FALSE;       /* we are not a bootstrap */
boolean lat_enable = LAT_ENABLE;        /* LAT support ? */
int boxtype = BOXTYPE_GS;               /* Pretend we are natively a router */
int ciscopro_image = CISCOPRO_IMAGE;    /* image is runnable on a
					   CiscoPro router */


/*
 * _start
 * For M68k based platforms, the world begins here and this must be 
 * the first code in the text segment.
 * For R4k based platforms, some postion independant code executes to setup
 * the correct virtual address space, and then it calls _start .
 */

#define	BOOTLOAD	2	/* run code */
#define	VERSION		3	/* get code version */
#define	DEBUG		4	/* Run code in debug mode */
#define	SETUP_DEBUG	5	/* Just set up debugger and return */

boolean _start (
    int code,
    boolean arg)
{
    long int ptrdiff;
    
    /*
     * Copy the initialized data section from ROM to RAM:
     * For images linked to run from DRAM,
     *  for 68K platfroms, data_start will follow etext within an
     *  alignment fudge factor not more than a double long.
     *  for the MIPS platforms, _fdata will follow etext within
     *  8K bytes.
     * So we'll play safe and assume the data region will start
     * within 32K bytes for run-from-RAM images.
     * For Run-from-ROM or run-from-FLASH images, data_start will be
     * in DRAM and will not be anywhere near etext (atleast 16M away).
     * This is the case where we want to move the data section.
     */
    ptrdiff = (long)DATA_START - (long)TEXT_END;
    if ((ptrdiff < 0) || (ptrdiff > (1024*32))) {
        /* Not running from DRAM. Move data section. */
        bcopy (TEXT_END, DATA_START, ((long)DATA_END - (long)DATA_START));
    }

    switch(code) {
      case BOOTLOAD:
      case DEBUG:
      case SETUP_DEBUG:
	init_exception_handler();	/* Initialize exception handler */
	if (code != BOOTLOAD)		/* Enable kernel debugging */
	  gdb_kernel_enable();

	breakpoint();			/* Take an initial breakpoint */

	if (code == SETUP_DEBUG)	/* Get out if we're supposed to */
	  break;

	return(main(arg));		/* Start the system */

      case VERSION:
	return(SYS_MAJORVERSION*256+SYS_MINORVERSION);
      default:
	return(FALSE);
    }
    return(TRUE);
}
