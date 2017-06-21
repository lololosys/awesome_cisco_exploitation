/*
 * $Id: parser_defs_e1.h,v 3.1.40.1 1996/06/16 21:12:56 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/if/parser_defs_e1.h,v $
 *----------------------------------------------------------------------
 * File: parser_defs_e1.h
 *
 * 20-June-1994, Bill May (from parser_defs_t1.h)
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All Rights Reserved
 *----------------------------------------------------------------------
 * $Log: parser_defs_e1.h,v $
 * Revision 3.1.40.1  1996/06/16  21:12:56  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/01  04:58:21  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef __parser_defs_e1_h__
#define __parser_defs_e1_h__

enum e1_ctrlr_commands {
    E1_CTRLR_UNDEF,
    E1_CTRLR_SHUT,
    E1_CTRLR_FRAMING,
    E1_CTRLR_LINECODE,
    E1_CTRLR_CLOCKING,
    E1_CTRLR_CHANNEL,
    E1_CTRLR_LOOP,
    E1_CTRLR_PRI_CHANNEL,
};
#endif	/* __parser_defs_e1_h__ */
