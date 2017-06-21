/*$Id: parser_defs_exec.h,v 3.5 1996/01/29 07:00:53 hampton Exp $
 *------------------------------------------------------------------
 *
 *	P A R S E R _ D E F S _ E X E C . H
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_exec.h,v $
 * Revision 3.5  1996/01/29  07:00:53  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/20  04:15:09  dblair
 * CSCdi45363:  MLP information no longer displayed in show dialer output
 * Creating show ppp multilink command
 *
 * Revision 3.3  1995/12/05  22:17:28  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI Address Registration will now ignore the Selector byte.
 *
 * Revision 3.2  1995/11/17  18:50:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:20:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/21  06:39:32  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.5  1995/08/23  18:18:49  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Add new show commands.
 *
 * Revision 2.4  1995/08/12  07:10:03  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.3  1995/08/12  04:20:01  bdas
 * CSCdi36875:  Need to provide a show isdn history command for call
 * history mib
 *
 * Revision 2.2  1995/06/16  06:23:14  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  22:29:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Global commands */
#define HISTORY			 0
#define HISTORY_ON		 1
#define HISTORY_OFF		 2
#define HISTORY_PRINT		 3
#define	HISTORY_SIZE		 4
#define DUMP			 5

/* Exec and privileged command function defines */

#define CMD_CONNECT_DEFAULT           1
#define CMD_CONNECT_RLOGIN            2
#define CMD_CONNECT_TELNET            3
#define CMD_CONNECT_TN3270            4
#define CMD_DISABLE                   5
#define CMD_ENABLE                    6
#define CMD_END                       7		/* Analogous to exit, but is NVGEN'ed */
#define CMD_EXIT                      8
#define CMD_STARTAPPN  		      9 
#define CMD_STOPAPPN  		     10 
#define CMD_DEBUGAPPN  		     11
#define CMD_CONNECT_TUNNEL	     12


/* Show defines */
#define SHOW_CONTROLLERS_ALL        (1)
#define SHOW_CONTROLLERS_CACHE      (SHOW_CONTROLLERS_ALL+1)
#define SHOW_FLASH		    (SHOW_CONTROLLERS_CACHE+1)
#define SHOW_BOOTFLASH              (SHOW_FLASH+1)
#define SHOW_NETBIOS_CACHE          (SHOW_BOOTFLASH+1)
#define SHOW_NETBIOS_NBF_SESS       (SHOW_NETBIOS_CACHE+1)
#define SHOW_NODE                   (SHOW_NETBIOS_NBF_SESS+1)
#define SHOW_NODE_COUNTERS          (SHOW_NODE+1)
#define SHOW_NODE_STATUS            (SHOW_NODE_COUNTERS+1)
#define SHOW_NODE_SUMMARY           (SHOW_NODE_STATUS+1)
#define SHOW_APPN                   (SHOW_NODE_SUMMARY+1)
#define SHOW_DSPU                   (SHOW_APPN+1)
#define SHOW_PPP_MULTILINK          (SHOW_DSPU+1)

/* Write command destinations */
#define WRITE_UNUSED 0
#define WRITE_CORE 1
#define WRITE_ERASE 2
#define WRITE_MEMORY 3
#define WRITE_NETWORK 4
#define WRITE_TERMINAL 5
