/* $Id: msg_arap.c,v 3.2.60.3 1996/09/13 23:45:43 hampton Exp $
 * $Source: /release/112/cvs/Xsys/arap/msg_arap.c,v $
 *------------------------------------------------------------------
 * msg_arap.c - Message file for ARAP
 *
 * January 1994, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_arap.c,v $
 * Revision 3.2.60.3  1996/09/13  23:45:43  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:35:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/03/18  18:50:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:27:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:31:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:39:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:54:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:54:37  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:05:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"

#define ARAP_COMPLAIN_LEVEL (10*ONESEC) /* No more than one per 10 sec */

facdef(ARAP);
msgdef_section("Apple Remote Access protocol error messages");

msgdef(PAKINVALID, ARAP, LOG_NOTICE, 0,
    "TTY %d: ARAP invalid packet received");
#define ARAP_ERR_PAKINVALID &msgsym(PAKINVALID, ARAP)
msgdef_explanation(
	"A corrupted packet reached the ARA protocol code.");
msgdef_recommended_action(
	"Check for excessive CPU usage or excessive line noise.");
msgdef_ddts_component("");


msgdef(ADDRUSED, ARAP, LOG_INFO, 0,
    "TTY %d: ARAP started for user %s; address %d.%d");
#define ARAP_ERR_ADDRUSED &msgsym(ADDRUSED, ARAP)
msgdef_explanation(
	"This message appears only when ARA protocol logging is configured. The
	message notes that an ARA protocol user has logged in.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef(ADDRFREE, ARAP, LOG_INFO, 0,
    "TTY %d: ARAP ended for user %s; address %A; %d seconds connected");
#define ARAP_ERR_ADDRFREE &msgsym(ADDRFREE, ARAP)
msgdef_explanation(
	"This message appears only when ARA protocol logging is configured. The
	message notes that an ARA protocol user has logged out.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef(INITCONFUSED, ARAP, LOG_NOTICE, 0,
    "TTY %d: ARAP unexpected initialization packet %s");
#define ARAP_ERR_INITCONFUSED &msgsym(INITCONFUSED, ARAP)
msgdef_explanation(
	"The startup negotiation between a client and the ARA protocol server
	fell out of order. If this message is received sporadically, it
	indicates line noise.");
msgdef_recommended_action(
	"If this message recurs, reinstall the ARA client software.");
msgdef_ddts_component("");


msgdef(CONFAILED, ARAP, LOG_WARNING, 0,
    "TTY %d: ARAP connection failed: %s");
#define ARAP_ERR_CONFAILED &msgsym(CONFAILED, ARAP)
msgdef_explanation(
	"The ARA protocol client was denied access, probably due to a
	configuration problem on the server. The error message should indicate
	the problem. This message does not indicate access denials due to bad
	passwords.");
msgdef_recommended_action(
	"Investigate the problem reported in the error message.");
msgdef_ddts_component("");


msgdef(BADCONFIG, ARAP, LOG_ERR, 0,
    "TTY %d: unhandled configuration command");
#define ARAP_ERR_BADCONFIG &msgsym(BADCONFIG, ARAP)
msgdef_explanation(
	"A configuration command was not recognized.");
msgdef_recommended_action(
	"Check the configuration file on the server for any invalid
	configuration commands.");
msgdef_ddts_component("");


msgdef(BADPAK, ARAP, LOG_ERR, 0,
    "TTY %d: Bad packet type in arap_send_msg");
#define ARAP_ERR_BADPAK &msgsym(BADPAK, ARAP)
msgdef_explanation(
	"Internal data structures are corrupted.");
msgdef_recommended_action(
	"Check for abnormally high CPU usage.");
msgdef_ddts_component("");


msgdef(BADEXIT, ARAP, LOG_INFO, 0,
    "TTY %d: exited user %s: %s");
#define ARAP_ERR_BADEXIT &msgsym(BADEXIT, ARAP)
msgdef_explanation(
	"A user was disconnected from ARA protocol at an unexpected time. The
	disconnection may be due to modems dropping, server error, client
	problems, or any number of other difficulties.");
msgdef_recommended_action(
	"To determine the cause of the disconnection, correlate the receipt of
	this message with the user problem reports.");
msgdef_ddts_component("");


msgdef_limit(XTICKLE, ARAP, LOG_INFO,  0,
       "TTY %d: exited user %s: Other side stopped answering ARAP tickles\
 (mnp4 pak resent %d mnp4 pak sent %d last %d now %d)", ARAP_COMPLAIN_LEVEL);
msgdef_explanation(
	"The router disconnected as the remote Macintosh client stopped\
	sending ARAP tickle or data packets.");
msgdef_recommended_action(
	"To determine the cause of the disconnection, correlate the receipt of
	this message with the user problem reports.");
msgdef_ddts_component("");


msgdef(MAXRESENDS, ARAP, LOG_INFO, 0, "TTY %d");
msgdef_explanation(
	"The router resent MNP4 packets maximum number of times without\
	receiving an acknowledgement.");
msgdef_recommended_action(
	"To determine the cause of the disconnection, correlate the receipt of
	this message with the user problem reports.");
msgdef_ddts_component("");


msgdef(NULLRESEND, ARAP, LOG_EMERG, MSG_TRACEBACK, "TTY %d ");
msgdef_explanation(
	"An internal software error occured.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef(NOTNULLRESEND, ARAP, LOG_EMERG, MSG_TRACEBACK, "TTY %d");
msgdef_explanation(
	"An internal software error occured.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LRPHASERROR, ARAP, LOG_INFO, 0,
    "TTY %d: Received LT in CONNECTION PHASE", ARAP_COMPLAIN_LEVEL);
msgdef_explanation(
	"The router received an invalid packet during ARA connection setup\
	phase.");
msgdef_recommended_action(
	"Check for excessive line noise.\
	Check the ARAP client software configuration for\
	possible problems.");
msgdef_ddts_component("");


msgdef_limit(RCVNOPAK, ARAP, LOG_INFO, 0,
    "TTY %d: Receive getbuffer failure. dropping frame", ARAP_COMPLAIN_LEVEL);
msgdef_explanation(
	"No buffer exists for an incoming packet.");
msgdef_recommended_action(
	"Examine buffer usage statistics. Possibly more memory is\
	required.");
msgdef_ddts_component("");


msgdef_limit(RCVGIANT, ARAP, LOG_INFO, 0,
    "TTY %d: Rcv giant. dropping frame", ARAP_COMPLAIN_LEVEL);
msgdef_explanation(
	"The router received an oversized MNP4 frame.");
msgdef_recommended_action(
	"Check for excessive line noise.\
	Check the ARAP client software configuration for\
	possible problems.");
msgdef_ddts_component("");


msgdef(DEFAULT, ARAP, LOG_EMERG, MSG_TRACEBACK, "TTY %d");
msgdef_explanation(
	"An internal software error occured.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef(WAITBADMSG, ARAP, LOG_EMERG, MSG_TRACEBACK, "TTY %d: arg msg bad %x");
msgdef_explanation(
	"The router received an invalid packet during ARAP MNP4 connection\
	 setup phase.");
msgdef_recommended_action(
	"Check for excessive line noise.\
	Check the ARAP client software configuration for\
	possible problems.");
msgdef_ddts_component("");


msgdef(VSERROR, ARAP, LOG_EMERG, MSG_TRACEBACK, "TTY %d: VS queued error");
msgdef_explanation(
	"An internal software error occured.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef(MNP4T401, ARAP, LOG_INFO, 0, "TTY %d: T401 %d baud rate %d");
msgdef_explanation(
	"This message indicates that the router received MNP4 connection\
	request. MNP4 acknowledgement timer value is also indicated.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(RESENDSLOW, ARAP, LOG_INFO, 0, "TTY %d");
msgdef_explanation(
	"The router repeated resending of MNP4 packets as the\
	Macintosh client failed to acknowledge previously resent packets.");
msgdef_recommended_action(
	"If this error message persists for several ARAP connections\
	report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(NOLOG, ARAP, LOG_INFO, 0, "TTY %d: Could not malloc log ");
msgdef_explanation(
	"Memory not available for internal MNP4 logging.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef(MNP4RCVDISC, ARAP, LOG_INFO, 0, "TTY %d: MNP4 input disconnect ");
msgdef_explanation(
	"This indicates that the router received disconnect request\
	from the remote Macintosh client.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(REMOTEERROR, ARAP, LOG_INFO, 0, "TTY %d: arap input error ");
msgdef_explanation(
	"ARAP connection was terminated without a known cause.");
msgdef_recommended_action(
	"Report this error to you technical support representative.");
msgdef_ddts_component("");


msgdef(TIMERERROR, ARAP, LOG_INFO, 0, "TTY %d: MNP4 timeout error");
msgdef_explanation(
	"The router is dropping the connection after doing maximum resends.");
msgdef_recommended_action(
	"To determine the cause of the disconnection, correlate the receipt of
	this message with the user problem reports.");
msgdef_ddts_component("");


msgdef(NOEXTTACACS, ARAP, LOG_WARNING, 0,
    "TTY %d: arap TACACS is configured but extended TACACS is not.");
#define ARAP_ERR_NOEXTTACACS &msgsym(NOEXTTACACS, ARAP)
msgdef_explanation(
	"Terminal Access Controller Access Control System (TACACS)
	authentication failed because extended TACACS is not configured.");
msgdef_recommended_action(
	"Configure extended TACACS.");
msgdef_ddts_component("");


msgdef(ABORTED, ARAP, LOG_ERR, 0,
    "TTY %d: arap callback aborted");
#define ARAP_ERR_ABORTED &msgsym(ABORTED, ARAP)
msgdef_explanation(
	"An unexpected message was received during the Microcom Networking
	Protocol version 4 (MNP4) link setup.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(TIMEOUT, ARAP, LOG_WARNING, 0,
    "TTY %d: arap expected message timeout");
#define ARAP_ERR_TIMEOUT &msgsym(TIMEOUT, ARAP)
msgdef_explanation(
	"The software timed out during the MNP4 link setup.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");
