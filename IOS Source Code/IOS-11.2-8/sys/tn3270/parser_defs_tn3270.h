/* $Id: parser_defs_tn3270.h,v 3.2.6.1 1996/03/27 23:28:20 irfan Exp $
 * $Source: /release/111/cvs/Xsys/tn3270/parser_defs_tn3270.h,v $
 *------------------------------------------------------------------
 * P A R S E R _ D E F S _ T N 3 2 7 0 . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_tn3270.h,v $
 * Revision 3.2.6.1  1996/03/27  23:28:20  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.2  1996/03/07  00:37:31  irfan
 * CSCdi48513:  TN3270 sends excessive cursor-move strings
 * Add "[no] tn3270 optimize-cursor-move" & "[no] tn3270 status-message".
 *
 * Revision 3.1  1996/02/01  04:59:24  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:53:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* TN3270 parameters */
#define TN3270_NONE		 0
#define TN3270_8_BIT_CMD	 1
#define TN3270_DATASTREAM	 2
#define TN3270_NULL_PROCESSING	 3
#define TN3270_RESET_REQUIRED	 4
#define TN3270_OPTIMIZE_POSITION 5
#define TN3270_STATUS_MSG        6
#define TN3270_TYPEAHEAD         7
