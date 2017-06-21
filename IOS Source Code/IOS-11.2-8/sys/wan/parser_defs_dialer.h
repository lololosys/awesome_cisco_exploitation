/*
 *	P A R S E R _ D E F S _ D I A L E R . H
 *
 * $Id: parser_defs_dialer.h,v 3.1.40.2 1996/08/06 00:37:51 asb Exp $
 * $Source: /release/112/cvs/Xsys/wan/parser_defs_dialer.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_dialer.h,v $
 * Revision 3.1.40.2  1996/08/06  00:37:51  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/27  06:37:11  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.68.2  1996/04/23  00:24:37  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.1.68.1  1996/04/17  00:01:46  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1  1996/02/01  05:14:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.5  1996/01/12  16:47:11  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.4  1995/12/22  01:29:04  suresh
 * CSCdi29641:  Dialer dialer-group accepts 0 but does not work
 * Changed the dialer-group range to 1-10; cleaned code so that
 * dialer-list and dialer-group accesses are consistent
 *
 * Revision 3.3  1995/11/29  00:18:22  dblair
 * CSCdi41333:  DDR Callback and PPP multilink clean up
 *
 * Revision 3.2  1995/11/17  18:50:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/17  22:17:08  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.3  1995/09/30  05:04:54  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.2  1995/08/25  17:46:07  lbustini
 * CSCdi33266:  Dialer idle-timeout maximum doesnt work
 * Fix dialer timers upper bound.
 *
 * Revision 2.1  1995/06/07  22:29:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * Dialer commands
 */
#define NOPREFIX_DIALER          1
#define DIALER_INBAND            2
#define DIALER_IDLE              3
#define DIALER_FAST_IDLE         4
#define DIALER_REENABLE          5
#define DIALER_CARRIER           6
#define DIALER_STRING            7
#define DIALER_MAP               8
#define DIALER_HUNT              9
#define DIALER_LOAD		10
#define DIALER_PRIORITY		11
#define DIALER_DTR		12
#define DIALER_HOLDQ		13
#define DIALER_CALLER		14
#define DIALER_CALLBACK_SECURE  15
#define DIALER_REMOTE_NAME      16
#define DIALER_POOL             17
#define DIALER_POOL_MEMBER      18
#define DIALER_MAX_CALL         19
#define	DIALER_ROTOR		20

/*
 * dialer-list commands
 */
#define NO_DIALLST		1
#define DIALLST_PROTOCOL	2
#define DIALLST_ACCESS		3

/*
 * Dialer map class command
 */

#define DIALER_MAP_CALLBACK_SERVER  1
#define DIALER_MAP_VOICE_CALL       2
#define DIALER_MAP_SDN_CALL         3	/* NSF for outgoing call SDN */
#define DIALER_MAP_MEGA_CALL        4	/* NSF for outgoing call MEGACOMM */
#define DIALER_MAP_ACCU_CALL        5	/* NSF for outgoing call ACCUNET */
#define DIALER_MAP_IDLE             6   /* idle timer per destination */
#define DIALER_MAP_FAST_IDLE        7   /* fast idle timer per destination */
#define DIALER_MAP_REENABLE         8   /* enable timer per destination */
#define DIALER_MAP_CARRIER          9   /* carrier timer per destination */
#define DIALER_MAP_ISDN             10  /* carrier timer per destination */

/*
 * Dialer parity options
 */
#define	DIALER_INBAND_NOPARITY 	0
#define	DIALER_INBAND_ODDPARITY	1

/*
 * Dialer rotor options
 */
#define	DIALER_ROTOR_PRIORITY	0
#define	DIALER_ROTOR_BEST	1

/*
 * Dialer map keyword defines
 */
#define MAP_INVALID             0
#define MAP_NAME                1
#define MAP_SPEED               2
#define MAP_SPC                 3 

/*
 * Minimums and Maximums 
 */
#define DIALLST_MINLIST           1
#define DIALLST_MAXLIST           10
#define DIALER_MINPOOL            1
#define DIALER_MAXPOOL            255 
#define DIALER_MINPOOLPRIORITY    0
#define DIALER_MAXPOOLPRIORITY    255
#define DIALER_MINPOOLMINLINK     0
#define DIALER_MAXPOOLMINLINK     255
#define DIALER_MINPOOLMAXLINK     1
#define DIALER_MAXPOOLMAXLINK     255
#define DIALER_MINCALL            1
#define DIALER_MAXCALL            255

/*
 * Dialer timers upper bound
 */
#define DIALER_MAX_TIMER   (MAXINT/1000)

/*
 * Dialer show commands
 */

enum {
  SHOW_DIALER,
  SHOW_DIALER_MAP
};



