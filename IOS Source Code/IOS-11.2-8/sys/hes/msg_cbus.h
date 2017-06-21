/* $Id: msg_cbus.h,v 3.1.42.4 1996/08/28 12:45:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_cbus.h,v $
 *------------------------------------------------------------------
 * msg_cbus.h - Message file includes for cbus facility
 *
 * January 1996, Greg Stovall
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_cbus.h,v $
 * Revision 3.1.42.4  1996/08/28  12:45:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.42.3  1996/07/23  13:23:01  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.42.2  1996/06/06  18:17:53  motto
 * CSCdi58517:  CSNA Virtual Port x/2 hung in down/down state after
 * noShut/micro rel
 * Branch: California_branch
 *
 * Revision 3.1.42.1  1996/03/18  19:42:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/01/24  21:47:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  22:00:17  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
