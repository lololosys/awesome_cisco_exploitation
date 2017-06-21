/* $Id: parser_defs_atmsig.h,v 3.1.38.1 1996/05/09 14:11:36 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm/parser_defs_atmsig.h,v $
 *------------------------------------------------------------------------
 * parser/parser_defs_atmsig.h: ATM Signalling parsing specific constants
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------------
 * $Log: parser_defs_atmsig.h,v $
 * Revision 3.1.38.1  1996/05/09  14:11:36  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.70.1  1996/04/27  06:33:59  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.58.1  1996/04/10  23:35:56  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1  1996/02/01  04:57:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:48  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:49:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/18  14:24:01  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.1  1995/06/07  22:28:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ATM Signalling config commands
 */
#define ATMSIG_MAX_CONN_PENDING  0
#define ATMSIG_MAX_RETRY         1
#define ATMSIG_TIMER316          2 
#define ATMSIG_TIMER308          3 
#define ATMSIG_TIMER322          4 
#define ATMSIG_TIMER313          5 
#define ATMSIG_TIMER310          6 
#define ATMSIG_TIMER303          7 
#define ATMSIG_TIMER398          8
#define ATMSIG_TIMER399          9
#define ATMSIG_UNI_VERSION      10 
#define ATMSIG_UNI_SIDE         11 
#define ATMSIG_TIMER309         12

#define ATMSIG_UNI_SIDE_USER     0 
#define ATMSIG_UNI_SIDE_NETWORK  1 

#define ATMSIG_UNI_30            2
#define ATMSIG_UNI_31            3

/*
 * ATM Signalling exec commands 
 */
#define PARSER_ATMSIG_CLOSE       1
#define PARSER_ATMSIG_STATUS      2 

/*
 * ATMSIG show commands
 */
enum {
    SHOW_ATMSIG_SUMMARY,
    SHOW_ATMSIG_VC,
};
