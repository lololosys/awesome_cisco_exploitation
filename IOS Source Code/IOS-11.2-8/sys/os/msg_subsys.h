/* $Id: msg_subsys.h,v 3.2.66.2 1996/08/28 13:03:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_subsys.h,v $
 *------------------------------------------------------------------
 * msg_subsys.h -- header for subsystem message definitions.
 *
 * October, 1995, David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_subsys.h,v $
 * Revision 3.2.66.2  1996/08/28  13:03:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.66.1  1996/05/24  19:29:52  hampton
 * Improve the granularity of low memory corruption checks during
 * subsystem initialization.  [CSCdi58674]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  00:18:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/18  08:17:49  dstine
 * Placeholders for message headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
