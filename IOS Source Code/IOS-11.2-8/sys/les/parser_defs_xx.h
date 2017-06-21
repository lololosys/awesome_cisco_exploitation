/*
 *	P A R S E R _ D E F S _ X X . H
 *
 * $Id: parser_defs_xx.h,v 3.2 1996/02/13 21:26:20 rbatz Exp $
 * $Source: /release/111/cvs/Xsys/les/parser_defs_xx.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_xx.h,v $
 * Revision 3.2  1996/02/13  21:26:20  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.1  1996/02/01  04:58:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:53:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  00:21:37  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  22:31:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * XX platform specific commands
 */
#define PARSER_SQUELCH_REDUCED          1
#define PARSER_SQUELCH_NORMAL           2
#define PARSER_MEDIATYPE_10BASET        3
#define PARSER_MEDIATYPE_AUI            4
