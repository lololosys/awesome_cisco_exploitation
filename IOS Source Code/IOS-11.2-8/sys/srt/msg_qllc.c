/* $Id: msg_qllc.c,v 3.1.62.4 1996/09/14 00:02:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/msg_qllc.c,v $
 *------------------------------------------------------------------
 * QLLC error messages.
 *
 * May, 1994	Duane DeCapite 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_qllc.c,v $
 * Revision 3.1.62.4  1996/09/14  00:02:14  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/09/07  05:35:09  snyder
 * CSCdi68412:  fix file header, so change log works
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(QLLC);				/* Define the QLLC facility */
msgdef_section("Qualified Logical Link Control error messages");


msgdef(NOMEM, QLLC, LOG_ERR, 0, "Not enough memory available");
msgdef_explanation(
	"There is not enough memory in the system to complete this request.");
msgdef_recommended_action(
	"Add more memory to the router. Otherwise, reduce the configuration or
	the load on the router.");
msgdef_ddts_component("");


msgdef(NOLLC2, QLLC, LOG_ERR, 0, "Unable to open an llc2 session");
msgdef_explanation(
	"An LLC2 session could not be established with the destination MAC
	address.");
msgdef_recommended_action(
	"Verify that the qllc partner statement in the configuration file is
	correct, and that the partner is on the desired LAN.");
msgdef_ddts_component("");


msgdef(NULLPTR, QLLC, LOG_ERR, 0, "%s ptr is null");
msgdef_explanation(
	"The specified structure was not configured.");
msgdef_recommended_action(
	"Confirm the configuration commands for the structure.");
msgdef_ddts_component("");


msgdef(NOQLLC, QLLC, LOG_WARNING, 0, "Unable to open qllc session, current state is %s");
msgdef_explanation(
	"A QLLC session could not be established.");
msgdef_recommended_action(
	"Verify that the qllc partner and the qllc xid commands are correct.");
msgdef_ddts_component("");


msgdef(QLLCPRIM, QLLC, LOG_INFO, 0, "Qllc primitive is an %s %s, da %e, sa %e");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DIFFPRTR, QLLC, LOG_ERR, 0, "%e - Different partner - originally %e - now %e");
msgdef_explanation(
	"The partner for this QLLC virtual MAC address does not match the MAC
	address that was defined with the qllc partner command.");
msgdef_recommended_action(
	"Verify that the qllc partner statement in the configuration file is
	correct.");
msgdef_ddts_component("");


msgdef(NONULLXID, QLLC, LOG_ERR, 0, "Couldn't make null xid - %e -%e");
msgdef_explanation(
	"An attempt to create an IEEE XID failed.");
msgdef_recommended_action(
	"Verify that the qllc partner statement in the configuration file is
	correct, and that the partner is on the desired LAN.");
msgdef_ddts_component("");


msgdef(NOXID2, QLLC, LOG_ERR, 0, "Couldn't make xid - %e -%e");
msgdef_explanation(
	"The QLLC XID could not be forwarded to the LAN.");
msgdef_recommended_action(
	"Verify that the qllc partner and the qllc xid commands are correct.");
msgdef_ddts_component("");


msgdef(BADSTATE, QLLC, LOG_ERR, 0, "Bad qllc state - %s - %e");
msgdef_explanation(
	"An invalid LLC primitive was detected.");
msgdef_recommended_action(
	"Verify that the Token Ring ports and any participating LAN devices are
	configured correctly.");
msgdef_ddts_component("");


msgdef(NOMACADDR, QLLC, LOG_ERR, 0, "No lnx entry for macaddr %e");
msgdef_explanation(
	"No virtual MAC address was defined for this interface.");
msgdef_recommended_action(
	"Define the virtual MAC address, using either the x25 map qllc or the
	x25 pvc qllc command.");
msgdef_ddts_component("");


msgdef(BADOPCODE, QLLC, LOG_ERR, 0, "Opcode %s is invalid");
msgdef_explanation(
	"Either remote source-route bridging or local acknowledgment is
	configured incorrectly.");
msgdef_recommended_action(
	"Verify that remote source-route bridging and local acknowledgment are
	configured correctly.");
msgdef_ddts_component("");


msgdef(IFRAME, QLLC, LOG_ERR, 0,"%s");
msgdef_explanation(
	"An I-Frame was discarded due to network congestion.");
msgdef_recommended_action(
	"Verify that the LAN is not beaconing and that it is not in a congested
	state. Copy the error message exactly as it appears, and report it to
	your technical support representative.");
msgdef_ddts_component("");


msgdef(BADSTATEEVENT, QLLC, LOG_ERR, 0, "Bad qllc state - %s event - %s macaddr - %e");
msgdef_explanation(
	"The LLC primitive specified placed the router in an invalid state.");
msgdef_recommended_action(
	"Verify that the Token Ring ports and any participating LAN devices are
	configured correctly.");
msgdef_ddts_component("");


msgdef(NOPAKENQ, QLLC, LOG_ERR, 0, "Pak enqueue failed");
msgdef_explanation(
	"A packet was not sent to the LAN.");
msgdef_recommended_action(
	"Verify that the LAN partner is configured correctly, and that the
	partner is on the desired LAN.");
msgdef_ddts_component("");


msgdef(BADRSRBOPCODE, QLLC, LOG_ERR, 0, "Bad opcode %04x from %e to %e");
msgdef_explanation(
	"Either remote source-route bridging is incorrectly configured, or the
	other RSRB device is down.");
msgdef_recommended_action(
	"Verify that remote source-route bridging is configured correctly with
	the right version of the IOS software.");
msgdef_ddts_component("");


msgdef(LNXNOTFOUND, QLLC, LOG_ERR, 0, "lnx_remove_macaddr_hash did not find target lnx");
msgdef_explanation(
	"The qllc srb command was not defined for this interface.");
msgdef_recommended_action(
	"Add a valid qllc srb statement for this serial interface.");
msgdef_ddts_component("");


msgdef(BADQLLCSTATE, QLLC, LOG_ERR, 0, "Bad qllc state - %s");
msgdef_explanation(
	"An invalid QLLC primitive was detected.");
msgdef_recommended_action(
	"Verify that the partner QLLC device is configured correctly.");
msgdef_ddts_component("");


msgdef(GENERRMSG, QLLC, LOG_ERR, 0, "%s");
msgdef_explanation(
	"The text string provided with this error message describes the
	specific QLLC problem.");
msgdef_recommended_action(
	"Follow the instructions that appear with the error message. Copy the
	error message exactly as it appears, and report it to your technical
	support representative.");
msgdef_ddts_component("");

msgdef(BAD_XID, QLLC, LOG_ERR, 0, "Sna configuration error for %e: Lan device is PU2.1, X.25 device is PU2.0 (XID Format 1)");
msgdef_explanation(
	"The IBM Gateway (3172) or Front-End Processor on the LAN 
        is sending XID Format 3 which is used
	to communicate with PU2.1 devices.  The X.25 device is a 
	PU2.0 (3174) that is configured to send XID Format 1 
	messages. This is a very rare situation - for example,
	Revision A5.6 on, and C5.1 on the 3172.  Probably the revision
	levels on the IBM equipment are incompatible.");
msgdef_recommended_action(
    	"Check the revision levels on the external equipment, and
	upgrade them if necessary");
msgdef_ddts_component("");


msgdef(NO_QLLCBUFFER, QLLC, LOG_ERR, 0, "M bit Reassembly failed - couldn't allocate a packet");
msgdef_explanation(
	"The router ran out of memory to allocate buffers.");
msgdef_recommended_action(
	"Make sure that the router configuration is adequate for the service
	expected of it. You might want to tune the buffer pools, or upgrade to
	a larger router. At the very least, you will need more memory.");
msgdef_ddts_component("");


msgdef(BAD_SUBADDRESS, QLLC, LOG_ERR, 0, "%s is not a subaddress of %s");
msgdef_explanation(
	"A remote X.25 device is calling the router for QLLC service using an
	X.121 address that is not a proper subaddress of the router's X.121
	address.");
msgdef_recommended_action(
	"Enable subaddressing on the router, and make sure that the remote X.25
	device uses the correct subaddress for the QLLC service it will use.");
       
msgdef_ddts_component("");


msgdef_limit(INCALL_CFG, QLLC, LOG_ERR, 0,
    "Incoming call: No QLLC Service Access Point Configured for x.25 subaddress %s", (10 * ONESEC));
msgdef_explanation(
	"A remote X.25 device is calling the router for QLLC service using a
	subaddress that was not configured by the X.25 routing facility. The
	subaddress was not configured for QLLC service.");
msgdef_recommended_action(
	"Correct the QLLC configuration. Configure only the subaddress on the
	QLLC service, not the complete X.121 address that the remote X.25
	device uses.");
msgdef_ddts_component("");


msgdef_limit(INCALL_NO_PARTNER, QLLC, LOG_ERR, 0, 
    "Incoming call: No partner Mac Address configured - X.25 subaddress %s", (10 * ONESEC));
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(PARTNER_MISMATCH, QLLC, LOG_ERR, 0, 
    "Outgoing call: Partner Mac Address %e doesn't match configured partner %e", (10 * ONESEC));
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(QLLCMAP_ERR, QLLC, LOG_ERR, 0, 
    "Incoming call: QLLC map entry not found", (10 * ONESEC));
msgdef_explanation(
	"A software error in the router code occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NO_QSR, QLLC, LOG_ERR, 0,
    "No QLLC Service Access Points defined", (10 * ONESEC));
msgdef_explanation(
	"No QLLC services have been configured, even though the router will
	accept incoming calls for QLLC.");
msgdef_recommended_action(
	"Configure the QLLC service required.");
msgdef_ddts_component("");


msgdef_limit(NO_RESOURCE, QLLC, LOG_ERR, 0,
    "Incoming Call: Insufficient resources available", (10 * ONESEC));
msgdef_explanation(
	"This message is reporting insufficient system memory.");
msgdef_recommended_action(
	"Upgrade the memory.");
msgdef_ddts_component("");
