/*
 * $Id: parser_defs_t1.h,v 3.1.40.2 1996/07/11 22:39:26 jliu Exp $
 * $Source: /release/112/cvs/Xsys/if/parser_defs_t1.h,v $
 *----------------------------------------------------------------------
 * File: parser_defs_t1.h
 *
 * 8-July-1993, Ronnie B. Kon
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *----------------------------------------------------------------------
 * $Log: parser_defs_t1.h,v $
 * Revision 3.1.40.2  1996/07/11  22:39:26  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *            o Added new fdl command for t1 controller in order to support
 *              CSU function
 *
 * Revision 3.1.40.1  1996/06/16  21:12:58  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/01  04:58:25  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *----------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __parser_defs_t1_h__
#define __parser_defs_t1_h__

enum t1_ctrlr_commands {
    T1_CTRLR_UNDEF,
    T1_CTRLR_SHUT,
    T1_CTRLR_FRAMING,
    T1_CTRLR_CLOCKING,
    T1_CTRLR_LINECODE,
    T1_CTRLR_CABLELENGTH,
    T1_CTRLR_CHANNEL,
    T1_CTRLR_LOOP,
    T1_CTRLR_PRI_CHANNEL,
    T1_CTRLR_CAS_CHANNEL,
    T1_CTRLR_TONE_A_LAW,
    T1_CTRLR_FDL,
};
#endif	/* __parser_defs_t1_h__ */
