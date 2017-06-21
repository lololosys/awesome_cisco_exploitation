/* $Id: parser_defs_sscop.h,v 3.1 1996/02/01 04:57:13 hampton Exp $
 * $Source: /release/111/cvs/Xsys/atm/parser_defs_sscop.h,v $
 *-------------------------------------------------------------------
 * parser/parser_defs_sscop.h:  SSCOP parsing specific constants
 *
 * November 1993,  Percy P. Khabardar  
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_sscop.h,v $
 * Revision 3.1  1996/02/01  04:57:13  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  sscop config commands
 */
#define SSCOP_POLL_TIMER_T          0
#define SSCOP_KEEPALIVE_TIMER_T     1
#define SSCOP_CC_TIMER_T            2
#define SSCOP_MAX_CC                3
#define SSCOP_SEND_WINDOW           4
#define SSCOP_RCV_WINDOW            5
#define SSCOP_MAX_STAT              6
#define SSCOP_QUICK_POLL            7
