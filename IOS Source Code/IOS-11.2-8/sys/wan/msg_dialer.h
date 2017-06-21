/* $Id: msg_dialer.h,v 3.1.2.3 1996/08/28 13:22:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_dialer.h,v $
 *------------------------------------------------------------------
 * msg_dialer.h - Header for dialer message definitions.
 *
 * March, 1996, Lionel Bustini
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dialer.h,v $
 * Revision 3.1.2.3  1996/08/28  13:22:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.2  1996/07/23  13:31:41  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.1  1996/04/27  06:37:01  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1  1996/04/22  22:54:01  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
