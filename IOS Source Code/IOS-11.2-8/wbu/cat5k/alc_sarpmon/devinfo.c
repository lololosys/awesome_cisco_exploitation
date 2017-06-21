/* $Id: devinfo.c,v 1.1.4.1 1996/05/09 15:00:19 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/devinfo.c,v $
 *------------------------------------------------------------------
 * devinfo.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: devinfo.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:19  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:56  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <termio.h>
#include <terms.h>
#include "pmon.h"

int p2681(),ptty(),ps2driver();
struct pttyinfo tty0dat = {(ubyte *)0xa0800000};
struct pttyinfo tty1dat = {(ubyte *)0xa0800010};
struct p2681info tty3dat = {(ubyte *)0xa0e00000};
ConfigEntry ConfigTable[] = {
{(Addr)&tty0dat,0,ptty,1024,1},
{(Addr)&tty1dat,0,ptty,1024,1},
{(Addr)&tty3dat,0,p2681,1024,B9600},
{(Addr)&tty3dat,1,p2681,1024,B9600},
{0}};
