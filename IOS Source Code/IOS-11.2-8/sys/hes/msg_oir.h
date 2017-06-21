/* $Id: msg_oir.h,v 3.1.42.3 1996/08/28 12:45:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_oir.h,v $
 *------------------------------------------------------------------
 * msg_oir.h - Online Insertion/Removal message facility includes
 *
 * January 1996, Greg Stovall
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_oir.h,v $
 * Revision 3.1.42.3  1996/08/28  12:45:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.42.2  1996/05/28  20:36:52  shaker
 * CSCdi58549:  Notify users of non EOIR capable hardware
 * Branch: California_branch
 * Let users know why we're resetting the interface.
 *
 * Revision 3.1.42.1  1996/03/18  19:42:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/01/24  21:47:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  22:00:22  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
