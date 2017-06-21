/* $Id: msgtxt_flash.h,v 3.1.2.1 1996/08/28 12:40:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/msgtxt_flash.h,v $
 *------------------------------------------------------------------
 * M S G T X T _ F L A S H . H
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgtxt_flash.h,v $
 * Revision 3.1.2.1  1996/08/28  12:40:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:16:31  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern const char flash_writefail_msg[];
extern const char flash_erasefail_msg[];
