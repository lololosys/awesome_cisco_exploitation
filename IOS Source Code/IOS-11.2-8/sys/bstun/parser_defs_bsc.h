/* $Id: parser_defs_bsc.h,v 3.1.40.1 1996/09/10 08:48:52 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/parser_defs_bsc.h,v $
 *------------------------------------------------------------------
 * parser_defs_bsc.h - BSC Configuration command constants
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_bsc.h,v $
 * Revision 3.1.40.1  1996/09/10  08:48:52  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:15  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/03  01:03:25  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.1  1995/06/07  22:29:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 *  BSC commands.
 */

#define BSC_CMD_PRIMARY           0
#define BSC_CMD_SECONDARY         1
#define BSC_CMD_CONTENTION        2
#define BSC_CMD_CHARSET           3
#define BSC_CMD_FDX               4
#define BSC_CMD_PAUSE             5
#define BSC_CMD_SERVLIM           6
#define BSC_CMD_POLL_TIMEOUT      7
#define BSC_CMD_RETRIES           8
#define BSC_CMD_SPECPOLL          9
#define BSC_CMD_HOST_TIMEOUT     10
