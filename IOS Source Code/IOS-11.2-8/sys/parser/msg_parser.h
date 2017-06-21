/* $Id: msg_parser.h,v 3.3.12.2 1996/08/28 13:04:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/parser/msg_parser.h,v $
 *------------------------------------------------------------------
 * Header file for parser error messages
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: msg_parser.h,v $
 * Revision 3.3.12.2  1996/08/28  13:04:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.12.1  1996/07/23  13:27:28  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3  1996/02/20  18:02:14  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.2  1995/11/17  18:48:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
