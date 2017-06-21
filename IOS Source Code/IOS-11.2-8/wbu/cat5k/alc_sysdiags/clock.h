/* $Id: clock.h,v 1.1.4.1 1996/05/09 15:00:51 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/clock.h,v $
 *------------------------------------------------------------------
 * clock.h -- Include file for the Real-time clock in the NVRAM (from Catalyst)
 *
 * October, 1991, Vinod
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clock.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:51  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:43  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:19  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLOCK_H__
#define __CLOCK_H__

/* NVRAM end address is defined as a word boundary (i.e. end -2 ), 
   but for the clock we need the byte at the last location only. */
/*#define SCRATCH_ADDR (NVRAM_END + 1)*/
#define SCRATCH_ADDR 0x2001FFFF

union clock_struct {
  struct {
    uInt32 clk_high;
    uInt32 clk_low;
  } lw;
  struct {
    unsigned yr : 8;
    unsigned mon : 8;
    unsigned dt : 8;
    unsigned unused3 : 2;
    unsigned osc_bar : 1;
    unsigned reset_bar : 1;
    unsigned weekday : 4;
    unsigned format : 1;
    unsigned unused5 : 1;
    unsigned hr : 6;
    unsigned min : 8;
    unsigned sec : 8;
    unsigned centisecond : 8;
  } parts;
};


struct realtime {
  uChar year;
  uChar month;
  uChar date;
  uChar day;
  uChar hour;
  uChar minute;
  uChar second;
  uChar hundredth;
};


#endif
