/* $Id: msg_ipx.c,v 3.2.62.4 1996/09/14 00:10:24 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/msg_ipx.c,v $
 *------------------------------------------------------------------
 * msg_ipx.c - Message file for Novell IPX
 *
 * September 23, 1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipx.c,v $
 * Revision 3.2.62.4  1996/09/14  00:10:24  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/28  13:24:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/06/18  17:39:33  akr
 * CSCdi56186:  %SYS-3-INTPRINT: Illegal printing attempt from interrupt
 * level
 * Branch: California_branch
 * Changed unsolicited printf to errmsg.
 *
 * Revision 3.2.62.1  1996/05/03  02:17:09  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.2  1995/11/17  19:22:15  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  21:53:22  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:27:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IPX);
msgdef_section("IPX error messages");


msgdef(BADPATHS, IPX, LOG_ERR, 0,
       "Invalid number of paths (%d) for %q");
msgdef_explanation(
	"An internal inconsistency was detected in the IPX routing table
	structure.");
msgdef_recommended_action(
	"Copy and save this message. If possible, clear
	the IPX routing table by issuing the clear ipx route command. (This
	will result in lost connectivity until the routing table is rebuilt.)
	If the message recurs, copy and save the message, along with the
        output from the  command show ipx route, and call your technical
        support representative for assistance.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "In the IPX routing table, an inconsistency that should never happen
        was detected in paths to a network. The side effects of this
        inconsistency are unknown. Debugging should focus on trying to catch
        strange events that happen with the IPX routing table. The show ipx
        route, debug ipx routing activity, and debug ipx routing event
        commands are the primary useful commands in attempting to determine
        the cause of the inconsistency. Gather the above information, search
        for any similar known issues, and open a DDTS report.");


msgdef_limit(BADIGRPSAP, IPX, LOG_ERR, 0,
	     "Cannot send incremental SAP update to peer on %s.\n"
	     "\tIncreasing output-sap-delay may help.", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An incremental SAP update could not be sent, because this might cause
        the SAP tables between peers to become out of synch.");
msgdef_recommended_action(
        "Increase the output SAP delay on the listed interface to reduce
        buffer usage. If, after changing the delay, the message remains,
        copy the error message exactly as it appears and report it to your
        technical support representative.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "An incremental SAP update could not be sent. This might cause the
        SAP tables between peers to become out of synch. Aside from buffer
        or memory starvation, this should never happen. First, try increasing
        the IPX output SAP delay to something greater than
        the propagation delay of the link, for example, to 80 ms for a 56-kbps
        link. Second, try increasing the output buffers for the specific
        interface in question. If the message still occurs, get a log using the
        debug ipx sap activity and debug ipx sap event commands when the error
        message is occurring, and open a DDTS report after first searching
        for any similar known issues.");


msgdef(BADIGRPSAPTYPE, IPX, LOG_ERR, 0,
       "Unsupported SAP type for EIGRP being %s - type %x");
msgdef_explanation(
	"There are two types of service access points (SAPs), general queries
	and general responses. This message is sent when you receive or attempt
	to send an SAP with a type code which is not a general query or a
	general response. It is also sent when you receive a general query not
	addressed to the broadcast address.");
msgdef_recommended_action(
        "Copy the error message exactly as it appears, and report it to your
        technical support representative. If possible, also provide the output
        of the debug ipx sap activity and debug ipx sap event commands for the
        period during which this message appeared.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "Enhanced IGRP knows about only a few SAP operation types. When an
        unknown SAP operation type is received, this message is logged. The
        cause might be a device that is sending a corrupted packet that has an
        operation field with a value out of the known range, or it might be a
        new vewsion of NetWare that is using a new SAP operation that is
        unknown to Enhanced IGRP. Use SAP debugging or an analyzer to
        determine whether the packet is corrupt or whether it is a new SAP
        operation type. If the SAP packet is corrupt, correct the problem on
        the device that is sending the packet. If it is a new SAP operation
        type, inform the PX routing development engineering group. You do not
        need to file a DDTS unless the source of the new or strange SAP is a
        Cisco router.");


msgdef(BADCOUNT, IPX, LOG_ERR, 0, "Bad IPX server count. %s %d, %s %d");
msgdef_explanation(
	"The internal SAP service count does not match the number of services
	currently present and displayed by the show ipx servers command.");
msgdef_recommended_action(
        "If this message repeats several times, report the problem to your
        technical support representative, along with the output of show
        ipx servers command taken before the message appeared and again after
        the message appeared.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "The number of services in the SAP table did not match a counter that
        tracks the number of services. This problem was automatically
        corrected.  However, it would be useful to determine why the counter
        is getting out of sync with the service table. If the customer is
        willing, have them send the output of the show ipx servers,
        debug ipx sap activity, and debug ipx sap event commands from just
        before the message is displayed. After you gather this debugging
        information and search for similar DDTS reports, open a DDTS.");


msgdef(NOVACCESSLOG, IPX, LOG_INFO, 0,
       "%d %s %s %s %x.%e %s %x.%e %d pkt%s");
msgdef_explanation(
	"If IPX access violation logging is enabled, these messages are
        displayed when an access list logging event has occurred.");
msgdef_recommended_action(
	"Verify that the host system is permitted or denied as defined in the
        access list.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "The IPX access-list logging feature uses these error mesages to log
        normal access-list logging events. These are not errors, and no DDTS is
        required.");


msgdef(TOOMANYNETS, IPX, LOG_ERR, 0, "Too many networks");
msgdef_explanation(
	"The limit of connected IPX networks has been reached, and
        you cannot configure additional new IPX interfaces.");
msgdef_recommended_action(
	"Do not configure IPX on any additional interfaces. Also, you might
        want to remove IPX configuration commands from interfaces that are
        not active.");
msgdef_ddts_component("novell");
msgdef_tac_details(
        "In the router there is a hard limit of currently about 255 configured
        interfaces. When the user tries to configure beyond this limit this
        message is generated. The number of configured interfaces can not be
        increased, possibly the customer should reduce the number of
        configured interfaces. No DDTS is required.");
