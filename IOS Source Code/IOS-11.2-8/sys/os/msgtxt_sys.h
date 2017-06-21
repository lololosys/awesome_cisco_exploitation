/* $Id: msgtxt_sys.h,v 3.1.2.1 1996/08/28 13:03:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/msgtxt_sys.h,v $
 *------------------------------------------------------------------
 * M S G T X T _ S Y S . H
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgtxt_sys.h,v $
 * Revision 3.1.2.1  1996/08/28  13:03:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:52  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Managed timer error messages
 */
extern const char msgtxt_notfenced[];

/*
 * Strings used in various driver error messages
 */
extern const char msgtxt_nomemory[];
extern const char msgtxt_initfail[];
extern const char msgtxt_memoryerror[];
extern const char msgtxt_badunit[];
extern const char msgtxt_badencap[];
extern const char msgtxt_ownerror[];
extern const char msgtxt_badringsize[];
extern const char msgtxt_badcable[];

/*
 * CiscoPro startup error messages
 */
extern const char msgtxt_ciscopro[];
