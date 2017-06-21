/*
 *	P A R S E R _ D E F S _ L A N M G R . H
 *
 * $Id: parser_defs_lanmgr.h,v 3.2.30.1 1996/07/25 05:47:02 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/ibm/parser_defs_lanmgr.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_lanmgr.h,v $
 * Revision 3.2.30.1  1996/07/25  05:47:02  kchiu
 * CSCdi35303:  Token ring shut down with different segment number
 * Branch: California_branch
 *
 * Revision 3.2  1996/02/08  17:57:26  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.1  1996/02/01  04:58:15  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1996/01/29  07:00:59  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/18  03:23:09  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.2  1995/11/17  18:51:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * lanmgr defines
 */
#define LM_ALTERNATE 0
#define LM_CRS 1
#define LM_LOSS 2
#define LM_NOTIF 3
#define LM_PASSWORD 4
#define LM_REM 5
#define LM_RING 6
#define LM_RPS 7
#define LM_SNMP_ONLY 8
#define LM_SOFTERR 9
#define LM_RS 10
#define LM_DISABLED 11
#define LM_MAC_USE_EXP_BUF 12
#define LM_PATHTRACE_DISABLED 13
#define LM_RINGNUM_MISMATCH_ALLOWED 14
#define LM_PATHTRACE_DISABLE_ORIGIN 1
#define LM_PATHTRACE_DISABLE_ALL 2

/*
 * LanMgr show commands
 */
enum {
    SHOW_LANMGR_BRIDGE,
    SHOW_LANMGR_CONFIG,
    SHOW_LANMGR_INTERFACE,
    SHOW_LANMGR_RING,
    SHOW_LANMGR_STATION,
    SHOW_LANMGR_ZEBRA,
};
