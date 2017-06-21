/* $Id: msg_dspu.h,v 3.2.60.1 1996/08/28 12:42:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/msg_dspu.h,v $
 *------------------------------------------------------------------
 * DSPU Run-time error messages.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This file declares external all run-time error message structures.
 * If you add a new error message external declaration here, please update
 * msg_dspu.c too.
 *------------------------------------------------------------------
 * $Log: msg_dspu.h,v $
 * Revision 3.2.60.1  1996/08/28  12:42:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:05:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  21:45:21  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:29:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
