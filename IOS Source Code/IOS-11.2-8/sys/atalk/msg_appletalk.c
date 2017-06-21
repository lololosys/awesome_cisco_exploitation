/* $Id: msg_appletalk.c,v 3.3.58.3 1996/09/13 23:45:46 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/msg_appletalk.c,v $
 *------------------------------------------------------------------
 * msg_appletalk.c - Message file for appletalk facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_appletalk.c,v $
 * Revision 3.3.58.3  1996/09/13  23:45:46  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.58.2  1996/08/28  12:36:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.58.1  1996/07/10  22:19:40  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  08:45:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:59:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:10:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"



facdef(AT);
msgdef_section("AppleTalk error messages");

msgdef(NOTRUNNING, AT, LOG_ERR, 0,
    "AppleTalk not running");
#define AT_ERR_NOTRUNNING &msgsym(NOTRUNNING, AT)
msgdef_explanation(
	"You tried to show or change the AppleTalk configuration when AppleTalk
	routing was not turned on.");
msgdef_recommended_action(
	"Issue the appletalk routing command before issuing any
	other AppleTalk configuration or display command.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"This error is most likely the result of issuing the AppleTalk command
	when AppleTalk routing was not turned on.");


msgdef(ADDRINUSE, AT, LOG_NOTICE, 0,
    "%s: AppleTalk node warning; address %A already in use");
#define AT_ERR_ADDRINUSE &msgsym(ADDRINUSE, AT)
msgdef_explanation(
	"The initial hint address was in use. A search will be made for a valid
	address.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ADDRINVALID, AT, LOG_NOTICE, 0,
    "%s: AppleTalk node warning; address %A is invalid");
#define AT_ERR_ADDRINVALID &msgsym(ADDRINVALID, AT)
msgdef_explanation(
	"The hint address specified for the interface was not valid for the
	current configuration.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.
	The hint address is ignored and a random valid hint address is
	automatically computed.");
msgdef_ddts_component("atalk");


msgdef(DISCOFF, AT, LOG_NOTICE, 0,
    "%s: AppleTalk node warning; discovery mode disabled");
#define AT_ERR_DISCOFF &msgsym(DISCOFF, AT)
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Currently, this error message is not used.");


msgdef(NOADDRSAVAIL, AT, LOG_ERR, 0,
    "%s: AppleTalk node down; no addresses available");
#define AT_ERR_NOADDRSAVAIL &msgsym(NOADDRSAVAIL, AT)
msgdef_explanation(
	"No free node could be found on the interface.");
msgdef_recommended_action(
	"If your AppleTalk network has less than 250 nodes, issue the
	debug appletalk errors command and then call your technical support 
	representative. If your AppleTalk network has more than 250 nodes,
	split it into smaller networks by creating new segments and moving
	some nodes into the new segments.");
msgdef_ddts_component("atalk");


msgdef(NOSRCADDR, AT, LOG_ERR, 0,
    "%s: AppleTalk packet error; no source address available");
#define AT_ERR_NOSRCADDR &msgsym(NOSRCADDR, AT)
msgdef_explanation(
	"The router could not generate an AppleTalk packet 
	because no valid AppleTalk source address was available to identify this
	router in the packet.");
msgdef_recommended_action(
	"Configure an interface to have an address or cable range. Use the show
	appletalk command to verify that the interface is operational.");
msgdef_ddts_component("atalk");


msgdef(ADDRUSED, AT, LOG_INFO, 0,
    "%s: AppleTalk node up; using address %A");
#define AT_ERR_ADDRUSED &msgsym(ADDRUSED, AT)
msgdef_explanation(
	"No hint address or a bad hint address was specified. This message
	indicates which AppleTalk address will be used for the interface.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NETINVALID, AT, LOG_WARNING, 0, 
    "%s: AppleTalk node down; conflicts with active internet");
#define AT_ERR_NETINVALID &msgsym(NETINVALID, AT)
msgdef_explanation(
	"The configuration specified for the interface conflicts with the
	active AppleTalk internetwork. For example, a cable range might be 
	configured that overlaps an existing route but does not match the entry
	exactly.");
msgdef_recommended_action(
	"Use the appletalk cable-range command to reconfigure the interface so 
	that there are no conflicts with the existing AppleTalk rnetwork, 
	or take any other actions necessary to resolve the conflict. 
	AppleTalk will periodically attempt to restart the interface.");
msgdef_ddts_component("atalk");


msgdef(NETDISAGREES, AT, LOG_ERR, 0, 
    "%s: AppleTalk interface disabled; network incompatible with %#a");
#define AT_ERR_NETDISAGREES &msgsym(NETDISAGREES, AT)
msgdef_explanation(
	"The interface was disabled because the configured AppleTalk address or
	cable range for the interface conflicts with that of another router 
	on the attached network.");
msgdef_recommended_action(
	"Reconfigure the affected routers so that there are no
	conflicting addresses or cable ranges on the attached network.");
msgdef_ddts_component("atalk");


msgdef(DEFZONEERR, AT, LOG_ERR, 0,
    "%s: AppleTalk interface warning; default zone differs from %#a");
#define AT_ERR_DEFZONEERR &msgsym(DEFZONEERR, AT)
msgdef_explanation(
	"During the verification startup process,
	there was a disagreement between the default zone (the first zone
	defined after the appletalk cable-range command) and the router that
	confirmed the configuration. When an AppleTalk end node is first
	connected to a network, it joins the default zone until it is
	overridden.");
msgdef_recommended_action(
	"Use the appletalk zone command to correct the configuration so that 
	all routers are advertising the same default zone. Note that several 
	router implementations are known to mismanage the default zone.");
msgdef_ddts_component("atalk");


msgdef(ZONEDISAGREES, AT, LOG_ERR, 0,
    "%s: AppleTalk interface disabled; zone list incompatible with %#a");
#define AT_ERR_ZONEDISAGREES &msgsym(ZONEDISAGREES, AT)
msgdef_explanation(
	"The zone list of the configured interface is inconsistent with the
	zone list of other routers.");
msgdef_recommended_action(
	"Reconfigure the zone list on the router so that it does not
	conflict with that the zoen lists on other routers.");
msgdef_ddts_component("atalk");


msgdef(CONFIGOK, AT, LOG_INFO, 0, 
    "%s: AppleTalk interface enabled; verified by %#a");
#define AT_ERR_CONFIGOK &msgsym(CONFIGOK, AT)
msgdef_explanation(
	"The AppleTalk configuration was verified by consulting the indicated
	router.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ONLYROUTER, AT, LOG_INFO, 0, 
    "%s: AppleTalk interface enabled; no neighbors found");
#define AT_ERR_ONLYROUTER &msgsym(ONLYROUTER, AT)
msgdef_explanation(
	"No other AppleTalk routers were found on the network attached to the
	interface.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(MCMISMATCH, AT, LOG_ERR, 0, 
    "%s: AppleTalk interface warning; zone multicast %e differs from our multicast %e");
#define AT_ERR_MCMISMATCH &msgsym(MCMISMATCH, AT)
msgdef_explanation(
	"A computed multicast address disagrees with that provided by another
	AppleTalk router. The other AppleTalk router might be misconfigured or
	faulty.");
msgdef_recommended_action(
	"Correct the problem at the other router.");
msgdef_ddts_component("atalk");


msgdef(INTDOWN, AT, LOG_NOTICE, 0, 
    "%s: AppleTalk node down; line protocol down");
#define AT_ERR_INTDOWN &msgsym(INTDOWN, AT)
msgdef_explanation(
	"An AppleTalk hardware interface was disabled because of a bad serial
	line, a configuration command, or a bad interface.");
msgdef_recommended_action(
	"If the interface was not intentionally disabled, this message 
	indicates a hardware problem. In this case, repair or replace the 
	applicable hardware.");
msgdef_ddts_component("atalk");


msgdef(INTRESTART, AT, LOG_NOTICE, 0, 
    "%s: AppleTalk interface restarting; protocol restarted");
#define AT_ERR_INTRESTART &msgsym(INTRESTART, AT)
msgdef_explanation(
	"The hardware interface was reset because of a configuration change.
	After the reset, AppleTalk automatically reinitialized the interface.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(INTUP, AT, LOG_INFO, 0, 
    "%s: AppleTalk interface restarting; line protocol came up");
#define AT_ERR_INTUP &msgsym(INTUP, AT)
msgdef_explanation(
	"An AppleTalk interface that was previously shut down was restarted.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(BADROUTE, AT, LOG_INFO, 0, "Route to %#A has gone bad");
#define AT_ERR_BADROUTE &msgsym(BADROUTE, AT)
msgdef_explanation(
	"The system did not detect a route within the required timeout period.
	Therefore, the route is marked as bad. A network might have become 
	unreachable for some reason, perhaps because of a broken connection. 
	This message does not necessarily indicate an error condition.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(PATHCHANGE, AT, LOG_NOTICE, 0,
    "Route to %#A is now using path of type %s on %s via %#a");
#define AT_ERR_PATHCHANGE &msgsym(PATHCHANGE, AT)
msgdef_explanation(
	"The path to a network changed because either the old path went down or
	the new path was better.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(RTMPSTATE, AT, LOG_NOTICE, 0,
    "%s: RTMP path to %#A via %#a now in %s state (metric %d, %d hops)");
#define AT_ERR_RTMPSTATE &msgsym(RTMPSTATE, AT)
msgdef_explanation(
	"The state of a path changed.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(PATHNOTIFY, AT, LOG_INFO, 0,
    "%s: AppleTalk %s path to %#A down; reported bad by %#a");
#define AT_ERR_PATHNOTIFY &msgsym(PATHNOTIFY, AT)
msgdef_explanation(
	"A router went down somewhere on the AppleTalk network. The indicated
	peer notified this router of the change.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(PATHADD, AT, LOG_INFO, 0,
    "%s: AppleTalk %s path to network %#A added; via %#a (metric %d)");
#define AT_ERR_PATHADD &msgsym(PATHADD, AT)
msgdef_explanation(
	"A new AppleTalk path has been learned. The message indicates the source
	of the path, its distance in hops, its metric, and the type of path.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(PATHDEL, AT, LOG_INFO, 0,
    "%s: AppleTalk %s path to %#A via %#a has been deleted");
#define AT_ERR_PATHDEL &msgsym(PATHDEL, AT)
msgdef_explanation(
	"A path on the AppleTalk network aged out.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ROUTEOK, AT, LOG_INFO, 0,
    "%s: AppleTalk network up; %#A via %#a (%d hop %s)");
#define AT_ERR_ROUTEOK &msgsym(ROUTEOK, AT)
msgdef_explanation(
	"A routing update was received for a previously suspect route.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ROUTENOTIFY, AT, LOG_INFO, 0,
    "%s: AppleTalk network down; %#A reported bad by %#a");
#define AT_ERR_ROUTENOTIFY &msgsym(ROUTENOTIFY, AT)
msgdef_explanation(
	"A router went down somewhere on the AppleTalk network. The indicated
	peer notified this router of the change.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ROUTEDOWN, AT, LOG_INFO, 0,
    "%s: AppleTalk network down; interface to %#A disabled");
#define AT_ERR_ROUTEDOWN &msgsym(ROUTEDOWN, AT)
msgdef_explanation(
	"A connected route is being advertised as down (also known as \"notify
	neighbor\") because the configured interface is no longer 
	operational.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NEWROUTE, AT, LOG_INFO, 0,
    "%s: AppleTalk network added; %#A via %#a (%d hop %s)");
#define AT_ERR_NEWROUTE &msgsym(NEWROUTE, AT)
msgdef_explanation(
	"A new AppleTalk route was learned. The message indicates the
	source of the route and its distance in hops.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(DELROUTE, AT, LOG_INFO, 0,
    "AppleTalk network deleted; %#A removed from routing table");
#define AT_ERR_DELROUTE &msgsym(DELROUTE, AT)
msgdef_explanation(
	"A route on the AppleTalk network aged out.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NEWZONE, AT, LOG_INFO, 0,
    "AppleTalk zone added; zone %*s defined");
#define AT_ERR_NEWZONE &msgsym(NEWZONE, AT)
msgdef_explanation(
	"A new AppleTalk zone was created.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ZONEGC, AT, LOG_INFO, 0,
    "AppleTalk zone deleted; zone %s released");
#define AT_ERR_ZONEGC &msgsym(ZONEGC, AT)
msgdef_explanation(
	"The router removed an unused zone from the zone table.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NODEWRONG, AT, LOG_INFO, 0,
    "%s: AppleTalk node %a misconfigured; reply has been broadcast");
#define AT_ERR_NODEWRONG &msgsym(NODEWRONG, AT)
msgdef_explanation(
	"An AppleTalk node sent a GetNet Info request to this router specifying
	an invalid network number for the source of the GetNet Info request.
	This situation occurs when an AppleTalk end node is moved to another
	network and is therefore misconfigured for the current network.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(BUGREPORT, AT, LOG_DEBUG, MSG_PROCESS|MSG_TRACEBACK,
    "AppleTalk bug: %s");
#define AT_ERR_BUGREPORT &msgsym(BUGREPORT, AT)
msgdef_explanation(
	"The software detected an unexpected condition. The router takes
	corrective action and continues.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, note any AppleTalk problems 
	you experience, and call your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(ASSERTFAILED, AT, LOG_CRIT, MSG_PROCESS|MSG_TRACEBACK,
    "AppleTalk assertion failed: %s");
#define AT_ERR_ASSERTFAILED &msgsym(ASSERTFAILED, AT)
msgdef_explanation(
	"The software detected an inconsistency. Although this error is
	serious, the router attempts to continue. AppleTalk processing might be
	impaired.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, note any AppleTalk problems 
	you experience, and call your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(DEBUGMSG, AT, LOG_DEBUG, 0,
    "AppleTalk debug: %s");
#define AT_ERR_DEBUGMSG &msgsym(DEBUGMSG, AT)
msgdef_explanation(
	"This message is generated by a debug command and provides additional
	information about an AppleTalk condition.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, note any AppleTalk problems 
	you experience, and call your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(COMPATERR1, AT, LOG_NOTICE, 0,
    "%s: AppleTalk internet compatibility mode required by %#a");
#define AT_ERR_COMPATERR1 &msgsym(COMPATERR1, AT)
msgdef_explanation(
	"A neighboring router supports only extended AppleTalk networks with a
	cable range of 1 and with one assigned zone, for example, 25-25.");
msgdef_recommended_action(
	"Upgrade the routers that support only the original AppleTalk
	specification. In the meantime, observe the compatibility rules
	for AppleTalk Phase 1 and Phase 2 routers.");
msgdef_ddts_component("atalk");


msgdef(COMPATERR2, AT, LOG_NOTICE, 0,
    "AppleTalk neighbor incompatibility; %#A has multiple zones");
#define AT_ERR_COMPATERR2 &msgsym(COMPATERR2, AT)
msgdef_explanation(
	"Although this router has neighbors that require your internetwork to
	observe compatibility rules, AppleTalk has learned of a route with
	multiple zone names defined. This condition can cause node
	inconsistencies on your AppleTalk network.");
msgdef_recommended_action(
	"Upgrade the routers that support only the original AppleTalk
	specification. In the meantime, observe the compatibility rules
	for AppleTalk Phase 1 and Phase 2 routers.");
msgdef_ddts_component("atalk");


msgdef(COMPATERR3, AT, LOG_NOTICE, 0,
    "AppleTalk neighbor incompatibility; %#A has wide cable range");
#define AT_ERR_COMPATERR3 &msgsym(COMPATERR3, AT)
msgdef_explanation(
	"Although this router has neighbors that require your internetwork to
	observe compatibility rules, AppleTalk has learned of a route that has
	a cable range greater than 1. This condition can cause node
	inconsistencies on your AppleTalk network.");
msgdef_recommended_action(
	"Upgrade the routers that support only the original AppleTalk
	specification. In the meantime, observe the compatibility rules
	for AppleTalk Phase 1 and Phase 2 routers.");
msgdef_ddts_component("atalk");


msgdef(COMPATERR4, AT, LOG_NOTICE, 0,
    "AppleTalk EIGRP neighbor incompatibility; %#a has same router ID (%d)");
#define AT_ERR_COMPATERR4 &msgsym(COMPATERR4, AT)
msgdef_explanation(
	"Two routers on the network have the same router ID.");
msgdef_recommended_action(
	"Reconfigure the router to have a unique router ID. If the problem 
	recurs, copy the error message exactly as it appears and report it to 
	your technical support representative.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"This error is the result of two or more routers having the same
	router ID. Reassigning a unique router ID to the routers involved should
	resolve the problem.");


msgdef(FDDICOMPAT, AT, LOG_NOTICE, 0,
    "AppleTalk FDDI neighbor %#a using obsolete FDDI addressing");
#define AT_ERR_FDDICOMPAT &msgsym(FDDICOMPAT, AT)
msgdef_explanation(
	"Another AppleTalk router on the FDDI ring is running a release
	of the router software earlier than 9.0(2). Routers running 
	these early software releases use an encapsulation for 
	AppleTalk on FDDI rings that is not compatible with the Apple FDDITalk 
	implementation.");
msgdef_recommended_action(
	"Upgrade the routers running the software releases that use the
	pre-FDDITalk encapsulation for AppleTalk on FDDI rings.");
msgdef_ddts_component("atalk");


msgdef(LOSTNEIGHBOR, AT, LOG_NOTICE, 0,
    "%s: AppleTalk neighbor down; lost contact with %#a");
#define AT_ERR_LOSTNEIGHBOR &msgsym(LOSTNEIGHBOR, AT)
msgdef_explanation(
	"A peer router is unreachable.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NEWNEIGHBOR, AT, LOG_INFO, 0,
    "%s: AppleTalk neighbor added; %#a is operational");
#define AT_ERR_NEWNEIGHBOR &msgsym(NEWNEIGHBOR, AT)
msgdef_explanation(
	"The router discovered a new AppleTalk router on the network.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NEIGHBORUP, AT, LOG_INFO, 0,
    "%s: AppleTalk neighbor up; %#a has restarted");
#define AT_ERR_NEIGHBORUP &msgsym(NEIGHBORUP, AT)
msgdef_explanation(
	"A neighboring router to which this router previously lost connectivity
	reappeared on the network.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(BADNEIGHBOR, AT, LOG_NOTICE, 0,
    "%s: AppleTalk neighbor ignored; %#a is misconfigured");
#define AT_ERR_BADNEIGHBOR &msgsym(BADNEIGHBOR, AT)
msgdef_explanation(
	"A neighboring router's AppleTalk configuration does not agree with
	this router's AppleTalk configuration. This condition can result from a
	mismatch between the neighbor's network range and this router's network
	range.");
msgdef_recommended_action(
	"Modify either this router's or the neighboring router's configuration
	so that the network ranges agree.");
msgdef_ddts_component("atalk");


msgdef(ACQUIREMODE, AT, LOG_INFO, 0,
    "%s: AppleTalk interface initializing; acquiring %s");
#define AT_ERR_ACQUIREMODE &msgsym(ACQUIREMODE, AT)
msgdef_explanation(
	"The interface is determining the correct configuration. This message is
	displayed when discovery mode is enabled and no other router is
	detected.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(ZONEPROXY, AT, LOG_INFO, 0,
    "AppleTalk proxy defined; zone %s using network %#A");
#define AT_ERR_ZONEPROXY &msgsym(ZONEPROXY, AT)
msgdef_explanation(
	"This message reports the zone name and network number that are being
	advertised to support the appletalk nbp-proxy command.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.
	When proxy service is initiated, this message is
	generated each time the appletalk nbp-proxy command is issued.");
msgdef_ddts_component("atalk");


msgdef(NOPROXY, AT, LOG_INFO, 0,
    "AppleTalk proxy deleted; zone %s freed network %#A");
#define AT_ERR_NOPROXY &msgsym(NOPROXY, AT)
msgdef_explanation(
	"A previously configured Name Binding Protocol (NBP) proxy was deleted.
	The router can no longer convert NBP requests to a format understood by
	routers that support only the original AppleTalk specification.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NOTSUPPORTED, AT, LOG_NOTICE, 0,
    "%s: AppleTalk node down; line protocol not supported");
#define AT_ERR_NOTSUPPORTED &msgsym(NOTSUPPORTED, AT)
msgdef_explanation(
	"An encapsulation method on the specified interface does not
	support AppleTalk.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(DUPADDR, AT, LOG_NOTICE, 0,
    "%s: AppleTalk interface restarting; node address taken by %e");
#define AT_ERR_DUPADDR &msgsym(DUPADDR, AT)
msgdef_explanation(
	"Another AppleTalk node on a common network interface claimed the same
	AppleTalk address that this router was using. This error indicates a
	serious, unexpected problem. If it recurs, a physical network problem
	in which the nodes cannot communicate with each other might exist.");
msgdef_recommended_action(
	"The interface on this router restarts AppleTalk processing on the 
	common network interface to resolve the address conflict. No action 
	is required.");
msgdef_ddts_component("atalk");


msgdef(INTCLEARED, AT, LOG_NOTICE, 0,
    "%s: AppleTalk interface restarting; interface cleared");
#define AT_ERR_INTCLEARED &msgsym(INTCLEARED, AT)
msgdef_explanation(
	"A clear interface command was issued on an interface that is 
	currently routing AppleTalk.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(OLDMCI, AT, LOG_NOTICE, 0,
    "%s: AppleTalk route cache disabled; firmware is obsolete");
#define AT_ERR_OLDMCI &msgsym(OLDMCI, AT)
msgdef_explanation(
	"The firmware on the MCI controller card does not support AppleTalk
	fast switching.");
msgdef_recommended_action(
	"No corrective action is required. Normal AppleTalk operation continues,
	but the fast switching cache is disabled for the interface associated
	with the MCI card. If this message is unexpected, verify that the MCI 
	firmware for the interface specified in the error message is at level 
	1.7 or higher.");
msgdef_ddts_component("atalk");


msgdef(IFCONFLICT, AT, LOG_ERR, 0,
    "%s: AppleTalk interface disabled; conflicts with interface %s");
#define AT_ERR_IFCONFLICT &msgsym(IFCONFLICT, AT)
msgdef_explanation(
	"An attempt was made to configure an interface to have the same or a
	conflicting AppleTalk address or cable range as another interface on
	the same router.");
msgdef_recommended_action(
	"Verify that you are not specifying an AppleTalk address or cable range
	used previously on this router, and reconfigure the interface.");
msgdef_ddts_component("atalk");


msgdef(SUBROUTINE_ERROR, AT, LOG_ERR, MSG_PROCESS|MSG_TRACEBACK,
    "AppleTalk error: %s");
#define AT_ERR_SUBROUTINE_ERROR &msgsym(SUBROUTINE_ERROR, AT)
msgdef_explanation(
	"An internal error occurred. Specifically, a programming assertion in
	the AppleTalk networking code was violated.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and get the output 
	of the show version command and the contents of the router 
	configuration file. Then call your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(MACIPUP, AT, LOG_INFO, 0, 
    "%s: MacIP server operational, %i");
#define AT_ERR_MACIPUP &msgsym(MACIPUP, AT)
msgdef_explanation(
	"If the appletalk event-logging command is configured, this message
	appears on the console when a MacIP server is fully initialized and is
	ready to accept client requests. The AppleTalk zone served by the MacIP
	server replaces the zone specified in the appletalk zone command. The 
	IP address configured on the MacIP server replaces the IP address
	configured by the ip address command.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(MACIPDOWN, AT, LOG_INFO, 0, 
    "%s: MacIP server shutting down, %i");
#define AT_ERR_MACIPDOWN &msgsym(MACIPDOWN, AT)
msgdef_explanation(
	"If the appletalk event-logging command is configured, this message
	appears on the console when a MacIP server is no longer operational for
	any reason. MacIP servers shut down in response to MacIP configuration
	changes or errors, or a change in the condition or configuration of an
	AppleTalk or IP interface for which the MacIP server is providing
	service. The AppleTalk zone served by the MacIP server replaces the 
	zone specified in the appletalk zone command. The IP address configured 
	on the MacIP server replaces the IP address configured by the ip 
	address command.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(MACIPBADCONF, AT, LOG_ERR, 0, 
    "%s: MacIP inoperable configuration, %i");
#define AT_ERR_MACIPBADCONF &msgsym(MACIPBADCONF, AT)
msgdef_explanation(
	"If the appletalk event-logging command is configured, this message
	appears on the console when a MacIP server cannot begin or recover
	operations because of an invalid IP address configuration. This
	condition only occurs when the configured MacIP server's IP address is
	not valid on the same cable of any operable IP interface. The AppleTalk 
	zone served by the MacIP server replaces the zone specified in the 
	appletalk zone command. The IP address configured on the MacIP server 
	replaces the IP address configured by the ip address command. When 
	this error occurs, the MacIP configuration is considered invalid 
	and is deleted.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(MACIPNOCABLE, AT, LOG_ERR, 0, 
    "%s: MacIP inoperable IP interface, %i");
#define AT_ERR_MACIPNOCABLE &msgsym(MACIPNOCABLE, AT)
msgdef_explanation(
	"If the appletalk event-logging command is configured, this message
	appears on the console when a MacIP server can no longer operate due to
	a change in the condition of the IP interface it serves. This condition
	can occur if IP routing is shut down on the interface, or the IP
	address of the interface is changed so that the MacIP server's IP
	address is no longer valid on the same cable. The AppleTalk zone 
	served by the MacIP server replaces the zone specified in the appletalk 
	zone command. The IP address configured on the MacIP server replaces 
	the IP address configured by the ip address command.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NOSUCHROUTE, AT, LOG_NOTICE, 0,
    "No such route %d");
#define AT_ERR_NOSUCHROUTE &msgsym(NOSUCHROUTE, AT)
msgdef_explanation(
	"You specified a nonexistent route in a show appletalk route or
	clear appletalk route command.");
msgdef_recommended_action(
	"Specify a valid route, that is, one that appears in the output of the
	show appletalk route command.");
msgdef_ddts_component("atalk");


msgdef(NOSUCHNBR, AT, LOG_ERR, 0,
    "No such neighbor, %#a");
#define AT_ERR_NOSUCHNBR &msgsym(NOSUCHNBR, AT)
msgdef_explanation(
	"You specified a nonexistent neighbor address in a show appletalk 
	neighbor or clear appletalk neighbor.");
msgdef_recommended_action(
	"Specify a valid neighbor address, that is, one that appears in the
	output of the show appletalk neighbor command.");
msgdef_ddts_component("atalk");


msgdef(NBRDELETED, AT, LOG_INFO, 0,
    "Neighbor entry for %#a deleted");
#define AT_ERR_NBRDELETED &msgsym(NBRDELETED, AT)
msgdef_explanation(
	"The clear appletalk neighbor command displays this command when it
	completes successfully.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(REGPENDING, AT, LOG_INFO, 0,
     "NBP registration of %s at [%s] pending");
#define AT_ERR_REGPENDING &msgsym(REGPENDING, AT)
msgdef_explanation(
	"A name registration call is being performed.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("atalk");


msgdef(NOMEM, AT, LOG_ALERT, 0,
     "Could not allocate memory for %s at line %d in %s");
#define AT_ERR_NOMEM &msgsym(NOMEM, AT)
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Check to make sure that the router has enough memory. If not,
	verify that there is no memory leak. Use Topic to search for a similar 
	DDTS. If you find none, write a DDTS for this problem.");


msgdef(NOBESTPATH, AT, LOG_ERR, 0,
     "Could not compute best path for network %#A");
#define AT_ERR_NOBESTPATH &msgsym(NOBESTPATH, AT)
msgdef_explanation(
	"A path to the specified network could not be found.");
msgdef_recommended_action(
	"Use the show appletalk route command to verify that a path to the 
	specified network exists. If it does, copy the error message exactly 
	as it appears, and report it along with the output of the show
	appletalk route command to your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(NOROUTE, AT, LOG_ERR, 0,
     "No route to destination %a");
#define AT_ERR_NOROUTE &msgsym(NOROUTE, AT)
msgdef_explanation(
	"The router could not deliver a packet to a destination because it did
	not know where to send it.");
msgdef_recommended_action(
	"Use the show appletalk route command to verify that a path to the
	destination exists. If it does, copy the error message exactly
	as it appears, and report it with the output of the show appletalk 
	route command to your technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(CANTDEL, AT, LOG_ERR, 0,
     "Cannot delete %s route %d by clearing; remove it by reconfiguring");
#define AT_ERR_CANTDEL &msgsym(CANTDEL, AT)
msgdef_explanation(
	"You attempted to remove a route from the routing table by issuing 
	the clear appletalk route command. The command failed because the route 
	was on a directly connected interface, that is, it was created when
	you configured the router.");
msgdef_recommended_action(
	"Remove the route by reconfiguring the directly connected interface
	with which it is associated.");
msgdef_ddts_component("atalk");


msgdef(INVNBPCMD, AT, LOG_ERR, 0,
     "Invalid NBPtest command %s");
#define AT_ERR_INVNBPCMD &msgsym(INVNBPCMD, AT)
msgdef_explanation(
	"An unrecognized command was specified in the Name Binding Protocol 
	(NBP) test facility.");
msgdef_recommended_action(
	"Enter a valid command.");
msgdef_ddts_component("atalk");


/*
 * EIGRP informational logging messages.
 * These are used to supplement the information generated by
 * "appletalk event-logging"
 */

/*
 * general message:
 * INFO-EIGRPLOG Logging "string" <DDP address> (<router-id>) on <interface>
 * for network <start-end>, metric <number / number>
 */
msgdef(EIGRPLOG, AT, LOG_INFO, 0,
     "EIGRP logging: %s from %a (%u) on %s for network %#A, metric %u / %u");
#define AT_ERR_EIGRPLOG &msgsym(EIGRPLOG, AT)
msgdef_explanation(
	"A new AppleTalk Enhanced IGRP path has been learned. The message 
	indicates the source of the path, its distance in hops, its metric, 
	and the type of path.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(EIGRPPEERUP, AT, LOG_INFO, 0,
     "AppleTalk EIGRP peer up: %a, router ID: %u, newly heard on %s");
#define AT_ERR_EIGRPPEERUP &msgsym(EIGRPPEERUP, AT)
msgdef_explanation(
	"A new neighbor appeared on the network.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(EIGRPPEERDN, AT, LOG_INFO, 0,
     "AppleTalk EIGRP peer down: %a, router ID: %u, just lost on %s");
#define AT_ERR_EIGRPPEERDN &msgsym(EIGRPPEERDN, AT)
msgdef_explanation(
	"A peer router is unreachable.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(EIGRPLINKUP, AT, LOG_INFO, 0,
     "Appletalk EIGRP link up: %s");
#define AT_ERR_EIGRPLINKUP &msgsym(EIGRPLINKUP, AT)
msgdef_explanation(
	"An AppleTalk Enhanced IGRP interface that was previously shut down 
	was restarted.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(EIGRPLINKDN, AT, LOG_INFO, 0,
     "AppleTalk EIGRP link down: %s");
#define AT_ERR_EIGRPLINKDN &msgsym(EIGRPLINKDN, AT)
msgdef_explanation(
	"An AppleTalk hardware interface was disabled.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(EIGRPLOTSOWORK, AT, LOG_WARNING, 0,
     "AppleTalk EIGRP WorkQ processed %u entries, required %u ms, did %s suspend");
#define AT_ERR_EIGRPLOTSOWORK &msgsym(EIGRPLOTSOWORK, AT)
msgdef_explanation(
	"AppleTalk Enhanced IGRP did not have enough time to process packets.");
msgdef_recommended_action(
	"This is a notification message only. No action is required.");
msgdef_ddts_component("eigrp-atalk");
msgdef_tac_details(
	"Currently, this message is not used.");


msgdef(OUTOFSYNC, AT, LOG_INFO, 0,
     "%s: AURP connection out of sync...terminating");
#define AT_ERR_OUTOFSYNC &msgsym(OUTOFSYNC, AT)
msgdef_explanation(
	"AURP detected packets that were out of sequence. This might be
	caused by a lost connection.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("aurp");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");


msgdef(PATHREJECTED, AT, LOG_WARNING, 0,
       "AURP path %#A rejected; conflict detected");
#define AT_ERR_PATHREJECTED &msgsym(PATHREJECTED, AT)
msgdef_explanation(
	"The router detected a conflicting cable range. A
	router is configured with a cable range that overlaps with an
	existing range but does not match the entry exactly.");
msgdef_recommended_action(
	"Use the show appletalk command to identify the overlapped cable range,
	and then reconfigure the misconfigured router.");
msgdef_ddts_component("aurp");


msgdef(DOMOVERFLOW, AT, LOG_ERR, 0,
       "Remap range overflow for %s domain %d"); 
#define AT_ERR_OVERFLOW &msgsym(DOMOVERFLOW, AT)
msgdef_explanation(
	"The remap range overflowed. If a network (range) cannot fit inside the
	remapping range, the network or cable range being advertised by
	a neighboring router is not accepted.  When an overflow occurs, 
	all interfaces in the domain are disabled.");
msgdef_recommended_action(
	"Use the appletalk domain remap-range command to increase the remapping 
	range. Then restart the interfaces.");
msgdef_ddts_component("atalk");


msgdef(DOMLOOP, AT, LOG_ERR, 0,
       "Loop detected on domain %d [%s]");
#define AT_ERR_LOOP &msgsym(DOMLOOP, AT)
msgdef_explanation(
	"The domain router detected a routing loop. Routing loops are
	not allowed.

	The domain router runs a background loop detection process. The domain 
	router starts this process when it receives a new zone list from a 
	domain that is identical to the list in the main routing table and 
	whose network range length matches that in this list.
	The loop detection process sends several AppleTalk 
	NBP loopback packets. When one of these packets is received on
	an interface that does not belong to the original domain, the domain
	router first shuts off all the domains involved in the loop, and then 
	shuts off all the interfaces in these domains. The loop detection does
	not catch all types of loops. It is your responsibility to
	make sure that no loop exists.");
msgdef_recommended_action(
	"Disable the cause of the routing loop. Then use the clear appletalk
	command to restart the interfaces involved.");
msgdef_ddts_component("atalk");


msgdef(NOVIRTUAL, AT, LOG_ERR, 0,
       "%s: client tried to connect, but no virtual network is declared");
#define AT_ERR_NOVIRTUAL &msgsym(NOVIRTUAL, AT)
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("atalk");
msgdef_tac_details(
	"Use Topic to search for a similar DDTS. If you find none, write
	a DDTS for this problem.");
