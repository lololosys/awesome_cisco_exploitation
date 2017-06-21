/* $Id: msg_ospf.c,v 3.3.18.3 1996/09/13 23:50:04 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_ospf.c,v $
 *------------------------------------------------------------------
 * msg_ospf.c - Message file for ospf facility
 *
 * July 17, 1992, Phanindra Jujjavarapu 
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ospf.c,v $
 * Revision 3.3.18.3  1996/09/13  23:50:04  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/08/28  12:53:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.18.1  1996/07/29  22:56:36  myeung
 * CSCdi61864:  no ip route while configuring causes Bus Error and reload
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/08  03:05:45  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.2  1995/11/17  17:35:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  06:55:04  myeung
 * CSCdi34206:  OSPF Memory Leak on border area router
 *
 * Revision 2.1  1995/06/07  21:08:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(OSPF);
msgdef_section("Open Shortest Path First error messages");


#ifdef OSPF_COMPLAIN_IVL
#undef OSPF_COMPLAIN_IVL
#endif
#define OSPF_COMPLAIN_IVL (5*ONESEC)	/* No more than one per 5 sec */

msgdef_limit(NOMEMORY, OSPF, LOG_ERR, 0, "No memory for %s ", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef_limit(INTERNALERR, OSPF, LOG_ERR, 0, "Internal error: %s", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOCONNDB, OSPF, LOG_ERR, 0, 
	     "No database entry for connected address %i", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"While calculating OSPF routes, the router could not find the
	link-state advertisement that represents the connected route in the
	router.");
msgdef_recommended_action(
	"Clear the IP routes in the routing table by entering the command clear
	ip route <address>.");
msgdef_ddts_component("");


msgdef_limit(NOLSA, OSPF, LOG_ERR, 0, 
	     "Failed to find this routers LSA in %s", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The router is not able to find its own router link-state
	advertisement. This can occur occasionally and self-correct. However,
	if this message recurs, restart the OSPF process.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOBACKBONE, OSPF, LOG_ERR, 0, 
	     "Flagged as being an ABR without a backbone area", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The router was flagged as an Area Border Router (ABR) without backbone
	area in the router.");
msgdef_recommended_action(
	"Restart the OSPF process.");
msgdef_ddts_component("");


msgdef_limit(NOSRCPDB, OSPF, LOG_INFO, 0, 
	     "ex_route_callback(): Can't find the src protocol to "
	     "redistribute net %i%m", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"OSPF attempted to redistribute a route but could not fine a valid
	source protocol.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef_limit(NOTREDIST1, OSPF, LOG_INFO, 0, 
	     "ex_route_callback(): do not redistribute net %i%m, %s",
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"For information only.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef_limit(NOTREDIST3, OSPF, LOG_INFO, 0, 
	     "build_ex_route(): don't redistribute net %i%m, %i "
	     "advertises it already", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"For information only.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef_limit(NOTREDIST4, OSPF, LOG_WARNING, 0, 
	     "Database scanner: external LSA %i%m is lost, reinstalls", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The software detected an unexpected condition. The router will take
	corrective action and continue.");
msgdef_recommended_action(
	"Record the entire error message and note any OSPF problem you
	experience. Report the error message to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(NOTREDIST5, OSPF, LOG_WARNING, MSG_PROCESS | MSG_TRACEBACK, 
	     "db_free: external LSA %i%m", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ERRRCV, OSPF, LOG_WARNING, 0, 
	     "Received invalid packet: %s from %i, %s", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An invalid OSPF packet was received. Details are included in the error
	message. The cause might be a misconfigured OSPF or an internal error
	in the sender.");
msgdef_recommended_action(
	"Check the OSPF configuration of the receiver and the sender for
	inconsistency.");
msgdef_ddts_component("");


msgdef_limit(UNKNOWNSTATE, OSPF, LOG_ERR, 0, 
	     "Reached unknown state in neighbor state machine", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NONEIGHBOR, OSPF, LOG_WARNING, 0, 
	     "Received %s from unknown neighbor %i", OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"OSPF hello, database description, or database request packet was
	received, but the router could not identify the sender.");
msgdef_recommended_action(
	"This situation should correct itself. If the message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(BADLENGTH, OSPF, LOG_WARNING, 0,
	     "Invalid length %d in OSPF packet from %i (ID %i), %s", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The system received an OSPF packet with a length field of less than
	normal header size or inconsistent with the size of the IP packet in
	which it arrived. This indicates an error in the sender of the
	packet.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADLSATYPE, OSPF, LOG_WARNING, 0,
	     "Invalid lsa: %s Type %d, LSID %i from %i, %i, %s", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"The router received an LSA with invalid LSA Type. The cause is either
	memory corruption or unexpected behavior on a router.");
msgdef_recommended_action(
	"From neighbor address, locate the problem router and reboot it. To
	determine what is causing this problem, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(CONFLICTING_LSAID, OSPF, LOG_WARNING, 0, 
	     "Found LSA with the same host bit set but using different mask"
	     "\n  Existing: LSA ID %i%m"
	     "\n  New: Destination %i%m", 
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NORTRID, OSPF, LOG_WARNING, 0,
	     "Could not allocate router ID",
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"OSPF failed while attempting to allocate a router ID from the IP
	address of one of its interfaces.");
msgdef_recommended_action(
	"Make sure that there is at lease one interface that is up and has a
	valid IP address. If there are multiple OSPF processes running on the
	router, each process needs its own unique router ID. You must have
	enough "up" interfaces so that each of them can obtain a router ID.");
msgdef_ddts_component("");


msgdef_limit(OSPFINTDOWN, OSPF, LOG_WARNING, 0,
	     "Interface %s is up but OSPF state is down.  Clean up",
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An inconsistency in an internal state was found and corrected.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(LOW_NET_METRIC, OSPF, LOG_WARNING, 0,
	     "Network %i metric %d goes down. In range of summary %i %i"
	     "\n with metric %d",
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(VIRTUAL_IN_NON_BACKBONE, OSPF, LOG_WARNING, 0,
	     "Virtual link information found in non-backbone area: %s",
	     OSPF_COMPLAIN_IVL);
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(DBEXIST, OSPF, LOG_ERR, 0, 
       "area %s lsid %i mask %i adv %i type %d");
msgdef_explanation(
	"OSPF has problem in locating the LSA; It would lead to memory leak");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(RDB_NO_LSA, OSPF, LOG_ERR, 0,
       "lsid %i adv %i type %d gateway %i metric %d network %i mask %i "
       "protocol %#x attr %#x net-metric %d");
msgdef_explanation(
	"OSPF found inconsistency between its database and IP routing table");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(ZERO_BANDWIDTH, OSPF, LOG_INFO, MSG_PROCESS | MSG_TRACEBACK,
       "interface %s has zero bandwidth")
msgdef_explanation(
	"The interface report its bandwidth as zero");
msgdef_recommended_action(
	"This message is for information only. No action is required.");
msgdef_ddts_component("");

msgdef(RCVSELF, OSPF, LOG_WARNING, 0,
       "from %s src %i dst %i id %i type %d if_state %d");
msgdef_explanation(
	"OSPF receive packet originated by itself");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(NOSELF, OSPF,  LOG_ERR, MSG_TRACEBACK,
       "if %s if_state %d");
msgdef_explanation(
	"An internal error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(ADJCHG, OSPF, LOG_NOTICE, 0,
       "Process %d, Nbr %i on %s from %s to %s, %s");
msgdef_explanation(
	"An OSPF neighbor has changed state. The message describes 
         the change and the reason for it. This message appears only 
         if log-adjacency-changes command is configured for the
         OSPF process.");
msgdef_recommended_action(
	"This message is for information only. No action is required.");
msgdef_ddts_component("");
