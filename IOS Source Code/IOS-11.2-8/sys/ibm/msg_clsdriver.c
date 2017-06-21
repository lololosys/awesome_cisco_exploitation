/* $Id: msg_clsdriver.c,v 3.1.2.2 1996/09/13 23:49:07 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/msg_clsdriver.c,v $
 *------------------------------------------------------------------
 * M S G  _ C L S D R I V E R . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_clsdriver.c,v $
 * Revision 3.1.2.2  1996/09/13  23:49:07  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  12:46:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:16:56  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CLSDR);				/* Define the CLSDRIVER facility */
msgdef_section("Cisco Link Services driver error messages");


msgdef(NOINIT, CLSDR, LOG_ERR,0, "CLSDRIVER not initialized");
msgdef_explanation(
	"A CLSDRIVER initialization failed earlier. Trying to use the driver
	now results in an error condition.");
msgdef_recommended_action(
	"Remove and then restore the destination logical unit (DLU) in the
	configuration file and try and reboot the system. Report this error to
	your technical support representative.");
msgdef_ddts_component("");


msgdef(NOREMOVEPORT, CLSDR, LOG_ERR,0, "RemovePort: port 0x%x not on list");
msgdef_explanation(
	"The destination logical unit (DLU) is trying to remove an nonexistent
	port from the list of active ports because it does not have the correct
	list of active ports.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(NOREMOVECEP, CLSDR, LOG_ERR,0, "RemoveCep : Cep  0x%x not on list");
msgdef_explanation(
	"The destination logical unit (DLU) is trying to remove a nonexistent
	connection end point from the internal data structure. This creates a
	problem in the connection end point (CEP) maintenance logic.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(NODLUCREATE, CLSDR, LOG_ERR,0, "Cannot create DLU for this type %d");
msgdef_explanation(
	"The CLSDRIVER does not understand the destination logical unit (DLU)
	type. Currently, only STUN uses the CLSDRIVER. Any other value results
	in an error.");
msgdef_recommended_action(
	"Make note of all details associated with this error message and report
	them to your technical support representative.");
msgdef_ddts_component("");


msgdef(NOPORTCREATE, CLSDR, LOG_ERR,0, "Cannot create PORT structure " );
msgdef_explanation(
	"The CLSDRIVER cannot enable this port for processing.");
msgdef_recommended_action(
	"Try rebooting the router. Report this error to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(NOCLSSERVER, CLSDR,LOG_ERR,0, "unable to start the Server process");
msgdef_explanation(
	"Could not start the CLSDRIVER process, possibly due to a resource
	problem. The subsystem using CLSDRIVER does not work.");
msgdef_recommended_action(
	"Remove and then restore the destination logical unit (DLU) in the
	configuration file and try and reboot the system. Report this error to
	your technical support representative.");
msgdef_ddts_component("");


msgdef(INCORRECTPORT, CLSDR, LOG_ERR,0, "Incorrect port pointer in USapId");
msgdef_explanation(
	"Cisco Link Services I (CLSI) returned an incorrect port pointer in the
	USapID.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(NOCEP, CLSDR, LOG_ERR,0, "Cannot found cep in free pool ");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(WRONGMSG, CLSDR, LOG_ERR,0, "Should never be here, cls message type unknown 0x%x");
msgdef_explanation(
	"The CLS message type is unknown to the destination logical unit
	(DLU).");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(MSGERR, CLSDR, LOG_ERR,0, " %s  couldn't get a message");
msgdef_explanation(
	"The destination logical unit (DLU) could not send a correct message to
	Cisco Link Services I (CLSI) because the message was formed
	incorrectly. CLSI will not perform the desired action.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(ERRRTN, CLSDR, LOG_ERR,0, " %s  failed error 0x%x");
msgdef_explanation(
	"An error was returned from Cisco Link Services I (CLSI) to the
	destination logical unit (DLU) because CLSI did not perform the action
	desired by the DLU.");
msgdef_recommended_action(
	"Refer to CLSI software documentation for more information about this
	error. Report this error to your technical support representative.");
msgdef_ddts_component("");
