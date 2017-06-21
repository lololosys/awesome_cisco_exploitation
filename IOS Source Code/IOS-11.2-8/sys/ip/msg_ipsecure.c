/* $Id: msg_ipsecure.c,v 3.2.60.3 1996/09/13 23:49:40 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/msg_ipsecure.c,v $
 *------------------------------------------------------------------
 * msg_ipsecure.c - Message file for ipsecurity facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipsecure.c,v $
 * Revision 3.2.60.3  1996/09/13  23:49:40  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:50:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/05/04  01:04:52  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.2  1995/11/17  09:36:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:00:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SEC);
msgdef_section("IP security error messages");


msgdef(TOOMANY, SEC, LOG_WARNING, 0,
       "Box secured, too many options on internal packet");
msgdef_explanation(
	"No room for all desired IP header options. Packet discarded.");
msgdef_recommended_action(
	"Configure for fewer IP header options.");
msgdef_ddts_component("");


msgdef(NOOPT, SEC, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Box secured, no option on internal packet");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOTSEC, SEC, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "First opt in tcb not BASIC security");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SECINS, SEC, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Security opt in tcb not SECINSERT");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOMAX, SEC, LOG_ERR, 0,
       "No default for NLESO defined");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(IPACCESSLOGNP, SEC, LOG_INFO, 0,
	     "list %s %s %d %i %s-> %i, %d packet%s",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A packet matching the log criteria for the given access list was
	detected.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(IPACCESSLOGDP, SEC, LOG_INFO, 0,
	     "list %s %s %s %i %s-> %i (%d/%d), %d packet%s",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A packet matching the log criteria for the given access list was
	detected.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(IPACCESSLOGP, SEC, LOG_INFO, 0,
	     "list %s %s %s %i(%d) %s-> %i(%d), %d packet%s",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A packet matching the log criteria for the given access list was
	detected.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");
