/* $Id: msg_ipfast.c,v 3.3.60.4 1996/09/13 23:49:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/msg_ipfast.c,v $
 *------------------------------------------------------------------
 * msg_ipfast.c - Message file for IP fast switching facility
 *
 * June 1994, Paul Traina
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipfast.c,v $
 * Revision 3.3.60.4  1996/09/13  23:49:37  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/08/28  12:50:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.2  1996/08/06  08:27:40  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.3.60.1  1996/06/24  16:04:52  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.3  1995/11/18  01:36:39  pst
 * CSCdi43643:  minor route-cache mods...
 *
 * Revision 3.2  1995/11/17  09:36:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/09  20:21:03  pst
 * CSCdi41527:  IP route cache code can shut down SSE
 *
 * Revision 2.1  1995/06/07  21:00:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IPFAST);
msgdef_section("IP fast-switching error messages");


msgdef(RADIXINIT, IPFAST, LOG_CRIT, 0,
       "Error initializing IP fast cache structures");
msgdef_explanation(
	"A programming error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RADIXDELETE, IPFAST, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS,
       "Error trying to delete prefix entry for %i/%d "
       "(expected %#x, got %#x)");
msgdef_explanation(
	"Problems related to the IP route cache triggered this debugging
	message.");
msgdef_recommended_action(
	"The router should correct for this error. If the router cannot forward
	packets properly or is experiencing other problems, copy the error
	message exactly as it appears and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(RADIXINSERT, IPFAST, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS,
       "Error trying to insert prefix entry for %i/%d "
       "(expected %#x, got %#x)");
msgdef_explanation(
	"Problems related to the IP route cache triggered this debugging
	message.");
msgdef_recommended_action(
	"The router should correct for this error. If the router cannot forward
	packets properly or is experiencing other problems, copy the error
	message exactly as it appears and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(BADIF, IPFAST, LOG_CRIT, 0,
	     "No support available for interface %s (programming error)",
	     30*ONESEC);
msgdef_explanation(
	"A programming error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(INTLEV, IPFAST, LOG_DEBUG, MSG_TRACEBACK,
	     "Cache update request at interrupt level: %i(%s)->%i(%s)",
	     5*ONESEC);
msgdef_explanation(
	"A programming error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVALREQ, IPFAST, LOG_INFO, MSG_TRACEBACK | MSG_PROCESS,
       "Cache invalidation request for %s");
msgdef_explanation(
	"The system is invalidating all cache requests for a given
	interface.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(INVALIDATE, IPFAST, LOG_INFO, MSG_TRACEBACK | MSG_PROCESS,
	     "Cache invalidation occurring for %s", ONESEC);
msgdef_explanation(
	"The system is invalidating all cache entries for a given interface.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


#ifdef IPCACHE_SANITY
msgdef(SANITY, IPFAST, LOG_ERR, MSG_PROCESS,
       "Found existing %s specific entry %i%m..%m->%i (%s) when "
       "attempting to create %i%d..%d->%i (%s)");
msgdef_explanation(
	"Problems related to the IP route cache triggered this debugging
	message.");
msgdef_recommended_action(
	"The router should correct for this error. If the router cannot forward
	packets properly or is experiencing other problems, copy the error
	message exactly as it appears and report it to your technical support
	representative.");
msgdef_ddts_component("");
#endif

msgdef_limit(INVALIDADDR, IPFAST, LOG_INFO, MSG_TRACEBACK | MSG_PROCESS,
	     "Invalid cache destination %i/%d", ONESEC);
msgdef_explanation(
	"The system attempted to add a cache entry for an invalid
 address.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
