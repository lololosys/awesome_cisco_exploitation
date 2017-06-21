/* $Id: parser_defs_ipigrp2.h,v 3.1.40.1 1996/04/02 05:37:19 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/parser_defs_ipigrp2.h,v $
 *------------------------------------------------------------------
 * parser_defs_ipigrp2.c
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_ipigrp2.h,v $
 * Revision 3.1.40.1  1996/04/02  05:37:19  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1  1996/02/01  04:58:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/04  03:08:14  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.1  1995/06/07  22:30:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * IP-IGRP2 commands.
 */
enum {
    IPIGRP2_HELLO=1,
    IPIGRP2_HOLDTIME,
    IPIGRP2_SPLIT,
    IPIGRP2_SUMMARY,
    IPIGRP2_EVENT_LOGGING,
    IPIGRP2_EVENT_LOG_SIZE,
    IPIGRP2_LOG,
    IPIGRP2_KILL_EVERYONE,
    IPIGRP2_BW_PCT,
    IPIGRP2_NBR_LOGGING
};


/*
 * Debug command
 */
enum {
    IPIGRP2_DEB_FSM=0,
    IPIGRP2_DEB_NEI
};
