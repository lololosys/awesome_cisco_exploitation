/* $Id: msg_nhrp.c,v 3.2.46.3 1996/09/14 00:10:07 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_nhrp.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_nhrp.c,v $
 * Revision 3.2.46.3  1996/09/14  00:10:07  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.46.2  1996/08/28  13:22:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.46.1  1996/07/23  13:31:42  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2  1996/01/03  23:32:49  bcole
 * CSCdi46376:  NHRP: Finish post branch split file moves
 *
 * Revision 3.1  1995/11/09  13:50:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/06  21:55:02  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(NHRP);
msgdef_section("Next Hop Resolution Protocol error message");
