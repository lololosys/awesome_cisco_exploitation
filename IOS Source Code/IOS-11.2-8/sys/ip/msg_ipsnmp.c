/* $Id: msg_ipsnmp.c,v 3.1.10.3 1996/09/13 23:49:42 hampton Exp $
 *------------------------------------------------------------------
 * msg_ipsnmp.c - Message file for snmp facility - specific to ip
 *      - replaces msg_snmp.c
 *
 * March 1996, Sandra Durham
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipsnmp.c,v $
 * Revision 3.1.10.3  1996/09/13  23:49:42  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.10.2  1996/08/28  12:50:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.10.1  1996/06/01  00:21:25  jjohnson
 * CSCdi56776:  SYS-2-GETBUF, Process= IP SNMP
 * Don't try processing messages with an overly-large udp->length,
 * complain about them instead
 *
 * Revision 3.1  1996/03/14  20:54:21  sdurham
 * CSCdi51552:  Rationalize file naming. Change msg_snmp.c to msg_ipsnmp.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IP_SNMP);
msgdef_section("Simple Network Management Protocol error messages");


msgdef(SOCKET, IP_SNMP, LOG_ERR, 0, "can't open UDP socket");
msgdef_explanation(
	"The SNMP server was unable to open a port for receiving SNMP requests.
	This usually happens when the SNMP server is started using the
	snmp-server community configuration command on a router or
	communication server with an interface that has no IP address
	configured. Another possible cause is low memory.");
msgdef_recommended_action(
	"Configure at least one interface with an IP address or specify the no
	snmp-server command to remove the SNMP server process. Call your
	technical support representative if problems persist or if it becomes
	necessary to add memory.");
msgdef_ddts_component("");


msgdef(NOTRAPIP, IP_SNMP, LOG_WARNING, 0, 
       "SNMP trap source %s has no ip address");
msgdef_explanation(
	"The user entered an snmp-server trap-source command. The interface
	requested for use as the source address has no address associated with
	it. The SNMP trap is being sent anyway, but without the source address
	that the user intended.");
msgdef_recommended_action(
	"There are two possible solutions. Either enter a no snmp-server
	trap-source command to disable the request for a specific source IP
	address, or add an IP address to the interface referenced in the
	snmp-server trap-source configuration command.");
msgdef_ddts_component("");


msgdef(TOOBIG, IP_SNMP, LOG_WARNING, 0,
       "Oversize message from %i, datagram size %u, udp length %u");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
