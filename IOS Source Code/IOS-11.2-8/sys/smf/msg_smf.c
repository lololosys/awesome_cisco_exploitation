/* $Id: msg_smf.c,v 3.3.4.2 1996/09/14 00:01:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/smf/msg_smf.c,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) message facility.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: msg_smf.c,v $
 * Revision 3.3.4.2  1996/09/14  00:01:38  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/08/28  13:09:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/03/07  23:31:32  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.2  1995/11/17  18:52:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/26  00:28:57  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert SMF to use a chunk instead of malloc() and free().
 *
 * Revision 2.2  1995/06/20  07:17:54  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:44:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SMF);
msgdef_section("Software MAC filter error message");


msgdef(INVALID_ACTION, SMF, LOG_WARNING, MSG_TRACEBACK,
       "failed to create SMF entry for %e on %s with result %08x");
#define SMF_ERR_INVALID_ACTION &msgsym(INVALID_ACTION, SMF)
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
