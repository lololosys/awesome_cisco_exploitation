/* $Id: msg_ip.c,v 3.3.4.3 1996/09/13 23:49:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/msg_ip.c,v $
 *------------------------------------------------------------------
 * msg_ip.c - Message file for ip facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ip.c,v $
 * Revision 3.3.4.3  1996/09/13  23:49:36  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.4.2  1996/08/28  12:50:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.4.1  1996/06/18  01:45:54  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/09  22:55:52  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  09:36:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  23:50:20  myeung
 * CSCdi30267:  router does a tight loop in ip_sendnet when fed a weird
 * sequence
 * - Add ability to detect, drop and log information about the looping
 *   packet
 *
 * Revision 2.1  1995/06/07  21:00:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IP);
msgdef_section("Internet Protocol error messages");


msgdef(CLASS, IP, LOG_WARNING, 0, 
       "Bad IP address and mask %i%m in class_resolve()");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(DUPADDR, IP, LOG_WARNING, 0,
       "Duplicate address %i on %s, sourced by %e", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"Another system is using your IP address.");
msgdef_recommended_action(
	"Change the IP address of one of the two systems.");
msgdef_ddts_component("");


msgdef(DESTHOST, IP, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "src=%i, dst=%i, NULL desthost");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADIPALIGN, IP, LOG_ERR, 0,
       "Invalid alignment in packet for IP. %s=%#x");
msgdef_explanation(
	"The packet data structure is misaligned. This condition may result in
	a small amount of overhead in processing IP traffic.");
msgdef_recommended_action(
	"Enter a show hardware command and report the output, along with this
	error message, to your technical support representative.");
msgdef_ddts_component("");


msgdef(BADSROUTE, IP, LOG_ERR, 0,
       "Improper source route.  Length %d Ptr %d");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LOOPPAK, IP, LOG_ERR, 0,
	     "Looping packet detected and dropped -"
	     "\nsrc=%i, dst=%i, hl=%u, tl=%u, prot=%u, sport=%u, dport=%u"
	     "\nin=%s, nexthop=%i, out=%s\noptions=%s",
	     1 * ONEMIN);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WEBINST_START, IP, LOG_NOTICE, 0,
	     "Attempting web install from host %i", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WEBINST_KILL, IP, LOG_NOTICE, 0,
	     "Terminating DNS process", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WEBINST_COMP, IP, LOG_NOTICE, 0,
	     "Selected IP address %i", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WEBINST_RESP, IP, LOG_NOTICE, 0,
	     "Sending DNS response to %i (request was for %i)",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CNTRFULL, IP, LOG_ERR, MSG_PROCESS|MSG_TRACEBACK,
       "IP counter block is full (setting protocol %d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
