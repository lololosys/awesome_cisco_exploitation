/* $Id: ipcboot.h,v 1.1.4.1 1996/05/09 14:55:21 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_ibipc/ipcboot.h,v $
 *------------------------------------------------------------------
 * ipcboot.h
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcboot.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:21  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:58  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:49  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:17  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/10/04  01:07:44  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.2  1995/05/05  16:32:15  jjchen
 * Created memory buffer structure.
 *
 * Revision 1.1  1995/05/01  23:56:50  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.1  1995/03/29  05:46:30  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef unsigned long ulong;

typedef int Int32;
typedef int boolean;

typedef struct idbtype_ {
	int ifnum;
} idbtype;

#define NUM_OF_PAKS 2
#define DATAGRAM_SIZE 800

typedef struct paktype_ {
	idbtype * if_output;
	int linktype;
	int acct_proto;
	uChar * datagramstart;
	uInt32  datagramsize;
	uChar datagram[DATAGRAM_SIZE];
} paktype;

typedef struct queuetype_ {
	uChar * mp;
} queuetype;

#define NUM_OF_MB 3
#define MB_SIZE 384

typedef struct memblk_ {
	uChar used;
	uChar mp[MB_SIZE];
} memblk;

#define NULL 0
#define timer_t int
#define blockproc void
#define TMT_ONCE 1

/* macros */

#define s_suspend ipc_dl_timer
#define destroy_timer(arg) arg = 0
#define stop_timer destroy_timer
#define s_killp(arg) return
#define create_timer(arg1, arg2, arg3, arg4, arg5, arg6) 0
#define wakeup(arg1)
#define set_timer(arg1, arg2) arg1 = arg2

/* external function */
extern int systeminitBLOCK();
