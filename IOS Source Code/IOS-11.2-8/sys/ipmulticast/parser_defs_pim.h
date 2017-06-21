/* $Id: parser_defs_pim.h,v 3.3.10.1 1996/09/10 05:31:51 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/parser_defs_pim.h,v $
 *------------------------------------------------------------------
 * parser_defs_pim.h - PIM parser constants.
 * 
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_pim.h,v $
 * Revision 3.3.10.1  1996/09/10  05:31:51  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/22  09:03:12  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.2  1996/02/08  03:39:26  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.1  1996/02/01  04:58:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/17  19:25:03  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.2  1995/11/17  18:51:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define PIM_RP_ADDRESS         1
#define PIM_MSG_INTERVAL       2
#define PIM_DM_MODE            3
#define PIM_SM_MODE            4
#define PIM_S_D_MODE           5
#define PIM_QUERY_INTVL        6
#define PIM_NBMA_MODE          7
#define PIM_SPT_THRESHOLD      8
#define PIM_ACCEPT_RP          9
#define PIM_SHOW_NBR          10
#define PIM_SHOW_RP           11
#define PIM_SHOW_IDB          12
#define PIM_SEND_RP_ANNOUNCE  13
#define PIM_SEND_RP_DISCOVERY 14
#define PIM_ANNOUNCE_FILTER   15
#define PIM_FAST_REG_STOP     16
#define PIM_VERSION           17

