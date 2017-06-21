/* $Id: msg_tbridge.c,v 3.1.62.4 1996/09/14 00:09:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/msg_tbridge.c,v $
 *------------------------------------------------------------------
 * msg_tbridge.c - Message system definitions for Transparent Bridging
 *
 * October 1994, Tony Speakman
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * 
 * 
$Endlog$
 */


#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TBRIDGE);
msgdef_section("Transparent bridging error messages");


msgdef_limit(GIANT, TBRIDGE, LOG_WARNING, 0x0,
	     "Giant received on %s, %d exceeds %d\n DA %e SA %e [0x%08x]",
	     (5 * ONESEC));
#define TBRIDGE_ERR_GIANT &msgsym(GIANT, TBRIDGE)
msgdef_explanation(
	"A packet was received that exceeds the expected maximum transmission
	unit (MTU) length for the receiving interface. The first four bytes of
	the packet following the source address are also provided to assist
	diagnosis.");
msgdef_recommended_action(
	"Use the address and packet information provided to trace the source of
	these packets and reduce its MTU to conform to the standard for the
	given media.");
msgdef_ddts_component("");


msgdef_limit(NOVCDROP, TBRIDGE, LOG_WARNING, 0,
	     "Destination VC unknown for packet", (5 * ONESEC));
#define TBRIDGE_ERR_NOVCDROP &msgsym(NOVCDROP, TBRIDGE)
msgdef_explanation(
	"A transparently bridged packet destined for an ATM interface cannot be
	fully addressed.");
msgdef_recommended_action(
	"Clear the bridge table to force re-learning of all addresses.");
msgdef_ddts_component("");


msgdef_limit(NOVCFLOOD, TBRIDGE, LOG_WARNING, 0,
	     "No VC's configured for bridging on %s", (5 * ONESEC));
#define TBRIDGE_ERR_NOVCFLOOD &msgsym(NOVCFLOOD, TBRIDGE)
msgdef_explanation(
	"An ATM interface was configured for transparent bridging without also
	specifying the associated virtual circuits.");
msgdef_recommended_action(
	"Configure the required virtual circuits on the specified interface.");
msgdef_ddts_component("");


msgdef_limit(INVALIDMEDIA, TBRIDGE, LOG_WARNING, 0x0,
	     "%s received on %s - invalid media for transparent bridging",
	     (5 * ONESEC));
#define TBRIDGE_ERR_INVALIDMEDIA &msgsym(INVALIDMEDIA, TBRIDGE)
msgdef_explanation(
	"Transparent bridging was configured on an interface that does not
	support it.");
msgdef_recommended_action(
	"Remove transparent bridging from the configuration on the specified
	interface.");
msgdef_ddts_component("");


msgdef_limit(SMF_ACTION, TBRIDGE, LOG_WARNING, 0x0,
	     "invalid action found for %e on %s - %08x",
	     (5 * ONESEC));
#define TBRIDGE_ERR_SMF_ACTION &msgsym(SMF_ACTION, TBRIDGE)
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BAD_ENCAP, TBRIDGE, LOG_WARNING, 0x0,
	     "Unexpected encapsulation received on %s %s",
	     (5 * ONESEC));
#define TBRIDGE_ERR_BAD_ENCAP &msgsym(BAD_ENCAP, TBRIDGE)
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(BADXMAC, TBRIDGE, LOG_WARNING, MSG_TRACEBACK,
	     "XMACACCESS: Invalid extended access list %d", (60 * ONESEC));
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
