/* $Id: parser_defs_vines.h,v 3.1 1996/02/01 05:14:23 hampton Exp $
 * $Source: /release/111/cvs/Xsys/vines/parser_defs_vines.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ V I N E S . H
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_vines.h,v $
 * Revision 3.1  1996/02/01  05:14:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:01:07  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Vines protocol types */
#define VINES_IP		 0
#define VINES_IPC		 1
#define VINES_SPP		 2
#define VINES_ARP		 4
#define VINES_RTP		 5
#define VINES_ICP		 6
#define NVINESIPTYPES		 7

/* Vines configuration commands */ 
#define VINES_ACCESSGROUP	 0
#define VINES_ACCESSLIST	 1
#define VINES_ARPENABLE		 2
#define VINES_DECIMAL		 3
#define VINES_ENCAPS		 4
#define VINES_HELPER		 5
#define VINES_HOST		 6
#define VINES_COST		 7
#define VINES_NET_FILTER_IN      8
#define VINES_NET_FILTER_OUT     9
#define VINES_NEIGHBOR		10
#define VINES_PROPAGATE		11
#define VINES_REDIRECTCMD	12
#define VINES_ROUTE		13
#define VINES_ROUTEC		14
#define VINES_ROUTER_FILTER_IN  15
#define VINES_ROUTING		16
#define VINES_SERVERLESS	17
#define VINES_SINGLE		18
#define VINES_SPLIT		19
#define VINES_UPDATEDELTAS      20
#define VINES_UPDATEINTERVAL    21
#define VINES_SRTP_ENABLED      22
#define VINES_ENHANCEMENTS	23
#define VINES_SVC_CFG_LINE      24
#define VINES_SVC_CFG_SLOT      25
#define VINES_HOSTNAME		26
#define VINES_PROXY		27
#define   VINES_PROXY_SEARCH        1
#define   VINES_PROXY_DISTANCE      2
#define VINES_CISCO_SVC		28
#define   VINES_CISCO_HOPS          1
#define   VINES_CISCO_PORT          2
#define   VINES_CISCO_PLATFORM      3
#define NVINESCMDS		29

/*
 * Server Service Commands
 */
#define VINES_TIME_DEST		 1
#define VINES_TIME_LIST		 2
#define VINES_TIME_PARTICIPATE	 3
#define VINES_TIME_SET		 4
#define VINES_TIME_USE		 5

/*
 * Streettalk Service Commands
 */
#define VINES_STREETTALK	 1

#define SHOW_VINES_ACCESS        0
#define SHOW_VINES_CACHE         1
#define SHOW_VINES_HOST          2
#define SHOW_VINES_INTERFACE     3
#define SHOW_VINES_IPC	         4
#define SHOW_VINES_NEIGHBORS     5
#define SHOW_VINES_ROUTE         6
#define SHOW_VINES_SERVICES      7
#define   SH_V_S_DATA               1
#define   SH_V_S_TREE               2
#define SHOW_VINES_TIMERS        8
#define SHOW_VINES_TRAFFIC       9
#define SHOW_VINES_BOTH          10

#define CLEAR_VINES_CACHE        1
#define CLEAR_VINES_NEIGHBOR     2
#define CLEAR_VINES_ROUTE        3
#define CLEAR_VINES_SERVER       4
#define CLEAR_VINES_TRAFFIC      5
#define CLEAR_VINES_CACHE_COUNTERS  6

/*
 * Subkeys for various commands.
 */
#define DO_ALL       1
#define DO_INTERFACE 2
#define DO_NEIGHBOR  3
#define DO_ONE       4
#define DO_SERVER    5
#define BY_METRIC    6


/* Vines min and max access group numbers */
#define VINESMINOACC		  1
#define VINESMAXOACC		100
#define VINESMINEACC		101
#define VINESMAXEACC		200
#define VINESMINSACC		201
#define VINESMAXSACC		300

#define VINESMINACC		  1
#define VINESMAXACC 		300
