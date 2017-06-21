/* $Id: msg_parser.c,v 3.4.12.3 1996/09/14 00:01:15 hampton Exp $
 * $Source: /release/112/cvs/Xsys/parser/msg_parser.c,v $
 *------------------------------------------------------------------
 * Parser error messages
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_parser.c,v $
 * Revision 3.4.12.3  1996/09/14  00:01:15  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/08/28  13:04:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.12.1  1996/07/23  13:27:26  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4  1996/02/20  18:02:12  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.3  1996/02/07  16:14:36  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  18:48:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(PARSER);
msgdef_section("Parser error messages");


msgdef(MULTIPLEIFS, PARSER, LOG_WARNING, 0,
       "interface_action: multiple ifs present when unit_only set");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LINKPOINT, PARSER, LOG_WARNING, 0,
       "Parser reached link_point");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CREATEINT, PARSER, LOG_ERR, 0,
       "Can't create any more subinterfaces");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NUMHELP, PARSER, LOG_WARNING, 0,
       "general_number_short_help: Invalid %s number flag");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NVGEN, PARSER, LOG_WARNING, 0,
       "nvgen_token called but csb->nvgen not set");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PROTOADDR, PARSER, LOG_WARNING, 0,
       "protoaddr_action: Unknown link_type %d");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MULFUNCS, PARSER, LOG_WARNING, 0,
       "unknown test in test_multiple_funcs '%c'");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOLINK, PARSER, LOG_ERR, 0,
       "no link_point(%d) in the %s %s command chain");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSUBCMD, PARSER, LOG_ERR, MSG_TRACEBACK,
       "Unrecognized subcommand %d in %s command '%s'");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVLDLINE, PARSER, LOG_WARNING, 0,
       "Invalid line in NV generation: %t\n");
msgdef_explanation(
	"The parser failed an internal software check.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INTDISABLE, PARSER, LOG_CRIT, 0,
       "Interrupts disabled in mode %s by command '%s'");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADRANGE, PARSER, LOG_WARNING, 0,
       "Bad range <%d-%d> for command '%s'");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCFG, PARSER, LOG_WARNING, 0,
       "Unexpected end of configuration file.\n");
msgdef_explanation(
	"This message occurs when a configuration is read from the Trivial File
	Transfer Protocol (TFTP) server or nonvolatile RAM (NVRAM) and the end
	of the file is encountered before the end statement. The configuration
	may be corrupted or incomplete. What was read is in effect.");
msgdef_recommended_action(
	"Make sure the configuration is good then execute either a copy
	running-config startup-config command to write the good configuration
	to NVRAM, or a copy running-config tftp command to write to a network
	TFTP server.");
msgdef_ddts_component("");


msgdef(INVLDNVGEN, PARSER, LOG_WARNING,  MSG_TRACEBACK,
       "Invalid  function called in NVGEN of '%s'");
msgdef_explanation(
	"An error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears. Execute a show version
	command, and copy the displayed information. Contact your technical
	support representative.");
msgdef_ddts_component("");
