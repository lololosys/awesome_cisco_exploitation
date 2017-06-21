/*
 *	P A R S E R _ D E F S _ L L C 2 . H
 *
 * $Id: parser_defs_llc2.h,v 3.1.40.1 1996/05/17 11:27:21 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/llc2/parser_defs_llc2.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_llc2.h,v $
 * Revision 3.1.40.1  1996/05/17  11:27:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.26.1  1996/03/15  05:55:52  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.1  1996/02/01  04:58:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  llc2 commands
 *
 *    NOTE: You must update the llc2_default[] table
 *            when adding/deleting llc2 commands
 *      
 */
#define LLC2_AKMAX               0
#define LLC2_AKDELAY_T           1
#define LLC2_IDLE_T              2
#define LLC2_LOC_WIND            3
#define LLC2_N2                  4
#define LLC2_T1_T                5
#define LLC1_DEF_TIMER           6
#define LLC2_TBUSY_T             7
#define LLC2_TREJ_T              8
#define LLC2_TPF_T               9
#define LLC2_NEG_VAL_T          10
#define LLC2_XID_RTRY_T         11
#define LLC2_TXQ_MAX            12
#define LLC2_EXPLORER_T         13
#define LLC2_EXPLORER_RETRY     14
#define LLC2_XID_T              15
#define LLC2_XID_RETRY          16
#define LLC2_L3_PID             17
#define LLC2_DYNWIND            18
#define LLC2_ADM_T		19

/*
 *  LLC2 exec commands
 */
#define PARSER_LLC_OPEN         1
#define PARSER_LLC_ATTACH       2
#define PARSER_LLC_SEND         3
#define PARSER_LLC_CLOSE        4
#define PARSER_LLC_STAT         5
#define PARSER_LLC_CHECKPAK     6
#define PARSER_LLC_OFFSET       7

