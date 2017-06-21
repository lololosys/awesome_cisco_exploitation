/* $Id: msg_vines.c,v 3.2.62.2 1996/09/14 00:09:59 hampton Exp $
 * $Source: /release/112/cvs/Xsys/vines/msg_vines.c,v $
 *------------------------------------------------------------------
 * msg_vines.c - Message file for vines facility
 *
 * July 19, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_vines.c,v $
 * Revision 3.2.62.2  1996/09/14  00:09:59  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:19:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:57:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(VINES);				/* Define the VINES facility */
msgdef_section("Banyan Virtual Integrated Network Service error messages");


msgdef(CORRUPTENTRY, VINES, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Detected a corrupted entry in the %s table.");
msgdef_explanation(
	"The VINES code detected that an internal table entry was corrupted.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef(INVALIDPATH, VINES, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Bad %s in path for neighbor entry %z.");
msgdef_explanation(
	"While searching the neighbor table, the code determined that a pointer
	that should point to a VINES path structure actually points to some
	other structure.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef(INVALIDROUTE, VINES, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Bad %s in route for server entry %Z.");
msgdef_explanation(
	"While searching the routing table, the code determined that a pointer
	that should point to a VINES route structure actually points to some
	other structure.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef(CONSISTENCY, VINES, LOG_CRIT, MSG_PROCESS,
       "Error %s.");
msgdef_explanation(
	"The VINES code ran into a situation that should not happen. For
	example, an IPC port number needs to be allocated, but all 65535 port
	numbers are currently in use.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(NOBUFFERS, VINES, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "No buffers available for %s", 10*ONESEC);
msgdef_explanation(
	"There were no buffers available to send a packet.");
msgdef_recommended_action(
	"Examine your system for memory or buffer leaks.");
msgdef_ddts_component("vines");


msgdef_limit(NOMEMORY, VINES, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "No memory available for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("vines");


msgdef_limit(ENCAPFAILED, VINES, LOG_CRIT, 0,
       "Encapsulation failed for %#z via %#z on %s", 10*ONESEC);
msgdef_explanation(
	"The cache population routing for VINES fast switching encountered an
	encapsulation failure when building a cache entry for a neighbor.");
msgdef_recommended_action(
	"Examine your configuration for causes of the encapsulation failure.
	Look for missing map statements, interfaces that have gone down, and so
	on.");
msgdef_ddts_component("vines");


msgdef_limit(CACHEFAILED, VINES, LOG_CRIT, 0,
       "Cannot build cache entry (%d bytes) for %#z on %s", 10*ONESEC);
msgdef_explanation(
	"The cache population routing for VINES fast switching determined that
	the MAC header on a packet is too large to fit into the cache.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(CACHEUNSUPP, VINES, LOG_CRIT, 0,
       "Caching of %s not supported on %s", 5*ONESEC);
msgdef_explanation(
	"The cache population routing for VINES fast switching was called for
	an interface that does not support VINES fast switching.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef(DUPADDR, VINES, LOG_CRIT, 0,
       "Duplicate address detected on interface %s");
msgdef_explanation(
	"The router detected that its VINES address is being used by another
	router.");
msgdef_recommended_action(
	"Identify the other router that is using this address by manual
	inspection of the routers. Issue the command vines routing recompute on
	both routers. If a router does not support the recompute keyword, then
	on that router you must disable VINES, write the configuration to
	memory, reboot the router, and then reconfigure VINES.");
msgdef_ddts_component("vines");


msgdef(BADPARAM, VINES, LOG_CRIT, MSG_TRACEBACK,
       "%s called with %s parameter set to %s.");
msgdef_explanation(
	"One of the VINES lookup functions was called with an illegal
	argument.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(REASSEMBLY, VINES, LOG_CRIT, MSG_TRACEBACK,
       "SRTP reassembly code returned invalid result code %u.", 10*ONESEC);
msgdef_explanation(
	"An internal software occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("vines");


msgdef_limit(BADTIMER, VINES, LOG_CRIT, 0,
       "RTP timer for %s was found disabled, and was reset.", 10*ONESEC);
msgdef_explanation(
	"An internal interface timer is not running. This condition usually
	occurs after an interface reset.");
msgdef_recommended_action(
	"None. The system automatically restarts the timer.");
msgdef_ddts_component("vines");


msgdef_limit(IPCNOROUTINE, VINES, LOG_INFO, 0,
       "IPC port %d registered without an upcall (%s).", 10*ONESEC);
msgdef_explanation(
	"There is an inconsistency in the active VINES data structure for
	handling IPC DATA messages. A message was received for a port that
	should have a listener, but the listener routine cannot be found.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(RPCNOSERVICE, VINES, LOG_INFO, 0,
       "IPC port %d registered without a service.", 10*ONESEC);
msgdef_explanation(
	"There is an inconsistency in the active VINES data structure for
	handling Net RPC messages. A message was received for a port that
	should have a service description, but that service description cannot
	be found.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(RTNNOTFOUND, VINES, LOG_INFO, 0,
       "%s service %s routine not found while \"%s\".", 10*ONESEC);
msgdef_explanation(
	"There is an inconsistency in the active VINES service description data
	structures. A service handler routine cannot find the description of
	its own service.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(FNNOTFOUND, VINES, LOG_INFO, 0,
       "%s service %s routine missing \"%s\" vector.", 10*ONESEC);
msgdef_explanation(
	"There is an inconsistency in the static VINES data structures for
	handling application-layer packets.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(NOVENCAP, VINES, LOG_CRIT, 0,
       "Encapsulation failed on %s for address %#z", 10*ONESEC);
msgdef_explanation(
	"The VINES code tried to send a packet on an interface that does not
	have a new encapsulation routine.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("vines");


msgdef_limit(INIT, VINES, LOG_CRIT, 0,
       "Init code for %s doesn't match defined constant.", 10*ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("vines");
