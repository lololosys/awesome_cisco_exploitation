/* $Id: msg_snmp.c,v 3.1.2.5 1996/09/14 00:01:43 hampton Exp $
 * $Source: /release/112/cvs/Xsys/snmp/msg_snmp.c,v $
 *------------------------------------------------------------------
 * msg_snmp.c - SNMP Error Messages
 *
 * April 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_snmp.c,v $
 * Revision 3.1.2.5  1996/09/14  00:01:43  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/08/28  13:09:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/07/31  00:10:05  ramk
 * CSCdi57452:  SNMP Trap overflow on boot-up
 * Branch: California_branch
 * Trap queue implemented using buffers allocated from system buffer pool.
 * Buginfs changed to errmsgs.
 *
 * Revision 3.1.2.2  1996/05/01  13:39:32  jjohnson
 * CSCdi40034:  spurious access in SNMP - AddOID
 * Issue an errmsg and abort if an attempt is made to encode an invalid OID
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/04/10  05:16:59  jjohnson
 * CSCdi54210:  Cannot generate SNMP traps at interrupt level
 * Detect and prevent any evil doers
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/09  16:24:20  jjohnson
 * Initial Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SNMP);
msgdef_section("Simple Network Management Protocol error messages");


msgdef(TRAPINTR, SNMP, LOG_ERR, MSG_TRACEBACK,
       "Attempt to generate SNMP trap from interrupt level");
msgdef_explanation(
	"During the processing of an interrupt, the code attempted to
         generate an SNMP trap. This is not allowed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
         to your technical support representative.");
msgdef_ddts_component(
        "Determine the component from the stack trace, which indicates
         which subsystem is attempting to generate a trap. Search for
         similiar DDTS reports and open a DDTS.");

msgdef(TRAPDROP, SNMP, LOG_WARNING, 0, "%s");
msgdef_explanation(
	"An attempt was made to generate an SNMP trap. However, the trap
         could not be sent, because of the reason given in this message.
         Most likely, this message means that the SNMP traps process is
         not running.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
         to your technical support representative. Use the show process
         command to determine whether the SNMP traps process is running.
         If it is not running, reload the system. If, after reloading,
         the SNMP traps process is still not running, there may not be
         enough memory for it to run.");
msgdef_ddts_component("snmp");
msgdef_tac_details(
	"If this error occurs, it is likely that either the trap process
         could not start, or it terminated abnormally. The output of the
         show process command should verify whether the SNMP traps process
         is running. If it is not running, you need to determine whether
         it was unable to start or it terminated abnormally. Have the
         customer reload the system and watch the console messages for
         indications of why the process could not start (e.g. lack of
         memory). Then, have the customer issue a show process command
         once the system is up and running. If it is not running and
         there were no system resource error messages, search for similiar
         DDTS reports and open a DDTS.");

msgdef(BADVALUE, SNMP, LOG_ERR, MSG_TRACEBACK,
       "Maximum number of index supported is 8.");
msgdef_explanation(
	"An SNMP trap contains some number of MIB objects, and the size
         of the instance identifier for these objects is restricted. This
         message indicates that an attempt was made to generate an SNMP
         trap containing a MIB object whose size exceeded this limit.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
         to your technical support representative.");
msgdef_ddts_component("snmp");
msgdef_tac_details(
        "The maximum size of an instance identifier is a hard-coded constant.
         If necessary the maximum size can be increased. This will require
         modifying the source code and generating a new image. Search for
         similiar DDTS reports and open a DDTS.");

msgdef(BADOID, SNMP, LOG_ERR, MSG_TRACEBACK,
       "Attempt to generate an invalid object identifier");
msgdef_explanation(
        "A valid object identifier must contain at least two subidentifiers.
         An attempt was made to generate an object identifier whose size
         was less than two.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
         to your technical support representative.");
msgdef_ddts_component(
        "Determine the component from the stack trace, which indicates
         which subsystem is attempting to generate invalid object
         identifiers.  Search for similiar DDTS reports and open a DDTS.");

msgdef(CPUHOG, SNMP, LOG_ERR, 0,
       "Processing %s of %s");
msgdef_explanation(
       "SNMP took too much time processing a request");
msgdef_recommended_action(
	"If feasible, use the debug snmp packet command to determine the
        contents of the SNMP request that is causing the message.  Copy the
        error message exactly as it appears, and report it along with any
        debug output to your technical support representative.");
msgdef_ddts_component(
        "Determine the component from the debug output, which indicates
         which subsystem is processing a request. Search for similiar DDTS
         reports and open a DDTS.");
