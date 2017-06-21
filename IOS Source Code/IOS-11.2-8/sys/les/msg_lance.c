/* $Id: msg_lance.c,v 3.3.62.2 1996/09/13 23:50:35 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_lance.c,v $
 *------------------------------------------------------------------
 * msg_lance.c - Message file for lance facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lance.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:35  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:42:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:34:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/09  19:32:32  sgannu
 * CSCdi38309:  catwoman msg causes modular build failure
 *
 * Revision 2.3  1995/07/25  21:48:48  abaindur
 * CSCdi37628:  LOSTCARR msgs too frequent when c1000 ethernet cable
 * unplugged
 * Rate limit message. Also fix handling of line protocol state during
 * carrier loss, and counting of deferred transmits. Remove a bogus
 * check for idb->status in ethernet_enable.
 *
 * Revision 2.2  1995/06/20  07:17:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:38:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(LANCE);
msgdef_section("Local area network controller Ethernet interface error
	messages");


msgdef(INITFAIL, LANCE, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"The hardware failed to initialize correctly.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(LATECOLL, LANCE, LOG_NOTICE, 0, "Unit %d, late collision error");
msgdef_explanation(
	"An Ethernet transceiver is malfunctioning, the Ethernet is overloaded,
	or the Ethernet cable is too long.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");

msgdef(LOSTCARR, LANCE, LOG_NOTICE, 0, 
       "Unit %d, lost carrier. Transceiver problem?");
msgdef_explanation(
	"An Ethernet transceiver is unplugged or faulty.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(UNDERFLO, LANCE, LOG_ERR, 0, "Unit %d, underflow error");
msgdef_explanation(
	"The Ethernet hardware is requesting data faster than the system can
	supply it. This condition might indicate that the system is at the
	limit of its performance.");
msgdef_recommended_action(
	"A small number of underflows might not be a cause for concern. You can
	monitor underflows with the show interface command. Reducing the load
	on the router or installing a higher-performance router should
	alleviate this problem.");
msgdef_ddts_component("");


msgdef(COLL, LANCE, LOG_NOTICE, 0, "Unit %d, excessive collisions. TDR=%d");
msgdef_explanation(
	"An Ethernet cable is broken or unterminated, or the transceiver is
	unplugged. The time delay reflectometer (TDR) counter is an internal
	LANCE counter that counts the time (in "ticks" of 100 ns each) from the
	start of a transmission to the occurrence of a collision. Because a
	transmission travels about 35 feet per tick, this value is useful to
	determine the approximate distance to a cable fault.");
msgdef_recommended_action(
	"If the transceiver appears to be properly terminated, repair or
	replace the Ethernet interface card.");
msgdef_ddts_component("");


msgdef(MEMERR, LANCE, LOG_ALERT, 0, msgtxt_memoryerror);
msgdef_explanation(
	"An Ethernet interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(BABBLE, LANCE, LOG_WARNING, 0, "Unit %d, babble error, csr0 = 0x%x");
msgdef_explanation(
	"An Ethernet interface is malfunctioning.");
msgdef_recommended_action(
	"Determine whether the malfunction stems from a software or hardware
	error by turning off all fastswitching.

	Error messages showing that the packet is too large for the link
	indicate a software error at a high level.

	Copy the error message exactly as it appears, and report it to your
	technical support representative.

	Receiving no error messages indicates a hardware error. 

	Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(BADUNIT, LANCE, LOG_ERR, 0, msgtxt_badunit);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(SPURIDON, LANCE, LOG_ERR, 0,
       "Unit %d, spurious IDON interrupt, csr%d=0x%04x");
msgdef_explanation(
	"An Ethernet interface generated a spurious Initialization Done
	interrupt.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(OWNERR, LANCE, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, msgtxt_ownerror);
msgdef_explanation(
	"An Ethernet interface is malfunctioning, or an internal software error
	occurred.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(BADCABLE, LANCE, LOG_ERR, 0, msgtxt_badcable);
msgdef_explanation(
	"Ethernet cable is not connected.");
msgdef_recommended_action(
	"Check Ethernet cable connection.");
msgdef_ddts_component("");
