/* $Id: msg_ucode.h,v 3.1.42.3 1996/08/28 12:45:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_ucode.h,v $
 *------------------------------------------------------------------ 
 *  msg_ucode.h - Message include file for downloadable ucode facility
 *
 *  January 1996, Greg Stovall
 *
 *  Copyright (c) 1996 by cisco Systems, Inc.  
 *  All rights reserved.  
 *------------------------------------------------------------------
 * $Log: msg_ucode.h,v $
 * Revision 3.1.42.3  1996/08/28  12:45:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.42.2  1996/04/06  19:28:28  foster
 * CSCdi52285:  OIR causes HSA system to panic with %DBUS-3-BADHSACONF
 * Branch: California_branch
 *
 * Revision 3.1.42.1  1996/03/18  19:43:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/01/24  21:47:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  22:00:24  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
