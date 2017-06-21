/* $Id: msg_ucode.c,v 3.2.60.3 1996/09/13 23:46:44 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_ucode.c,v $
 *------------------------------------------------------------------ 
 *  msg_ucode.c - Message file for downloadable ucode facility
 *
 *  William H. Palmer, Tue Oct  6 23:27:14 1992  
 *
 *  Copyright (c) 1992-1996 by cisco Systems, Inc.  
 *  All rights reserved.  
 *------------------------------------------------------------------
 * $Log: msg_ucode.c,v $
 * Revision 3.2.60.3  1996/09/13  23:46:44  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:45:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/03/18  19:43:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:47:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:19:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(UCODE);				/* Define the UCODE facility */
msgdef_section("Microcode error messages");


msgdef(BADCHKSUM, UCODE, LOG_ERR, 0,
       "Bad checksum in %s, found 0x%x expected 0x%x");
msgdef_explanation(
	"The microcode file is corrupted in some way; the checksum computed
	after reading the file from Flash does not match the one in the file.
	The file will not be downloaded, and the onboard ROM microcode will be
	used instead.");
msgdef_recommended_action(
	"Reload the microcode. If the message recurs, call your technical
	support representative for assistance.");
msgdef_ddts_component("");


msgdef(HDRCORRUPT, UCODE, LOG_ERR, 0,
       "Ucode header corrupted in %s, found 0x%x expected 0x%x");
msgdef_explanation(
	"The microcode file is corrupted in some way; the checksum computed
	after reading the file from Flash does not match the one in the file.
	The file will not be downloaded, and the onboard ROM microcode will be
	used instead.");
msgdef_recommended_action(
	"Reload the microcode. If the message recurs, call your technical
	support representative for assistance.");
msgdef_ddts_component("");


msgdef(LDFAIL, UCODE, LOG_ERR, 0,
       "Unable to download ucode from %s in slot %d, trying %s ucode");
msgdef_explanation(
	"For some reason, the microcode file specified by the configuration is
	not suitable for downloading, or another error occurred. The onboard
	ROM microcode will be loaded so the interface can remain
	operational.");
msgdef_recommended_action(
	"Attempt to reload the microcode. If the message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(NOFILE, UCODE, LOG_ERR, 0,
       "Ucode file %s not found, system ucode loaded in slot %d");
msgdef_explanation(
	"The file specified by the configuration was not found in Flash. The
	onboard ROM microcode will be loaded so the interface can remain
	operational.");
msgdef_recommended_action(
	"Use show flash to determine if the file is located in Flash. If the
	file is there, attempt to reload the microcode. If the message recurs,
	call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(BADHWVER, UCODE, LOG_ERR, 0,
       "Ucode file %s requires %s version %d.x hardware");
msgdef_explanation(
	"A mismatch was detected during an attempt to load a microcode file
	into an interface processor. The hardware requires a different version
	than the one specified.");
msgdef_recommended_action(
	"Use the required microcode version.");
msgdef_ddts_component("");


msgdef(RDFAIL, UCODE, LOG_ERR, 0,
       "Unable to read ucode file %s from flash");
msgdef_explanation(
	"For some reason, the microcode file was not read from Flash. Flash
	might be locked by another process or otherwise unavailable.");
msgdef_recommended_action(
	"Use the show flash command to determine whether Flash is in use and
	issue another microcode reload when Flash is free. If the message
	recurs, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(TOOBIG, UCODE, LOG_ERR, 0,
       "File %s at length %d is too long for buffer (size %d)");
msgdef_explanation(
	"The file is too large for the buffer. The onboard ROM microcode will
	be loaded.");
msgdef_recommended_action(
	"If the error message recurs after the onboard ROM microcode is loaded,
	call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(WRONGHARD, UCODE, LOG_ERR, 0,
       "%s is %s ucode not %s, microcode/hardware mismatch");
msgdef_explanation(
	"The specified microcode file is for a different interface processor
	than specified in the configuration. The user has made a configuration
	error. The onboard ROM microcode will be loaded.");
msgdef_recommended_action(
	"Reenter your microcode configuration command after confirming the
	interface type and reload the microcode.");
msgdef_ddts_component("");


msgdef(OBSOLETE, UCODE, LOG_NOTICE, 0,
       "FDDI unit %d has obsolete microcode: please upgrade it");
msgdef_explanation(
	"This message occurs when an FDDI interface processor is found running
	microcode that does not support microcode CMT, which is required for
	all software releases from 10.2 on.");
msgdef_recommended_action(
	"Upgrade the microcode on the interface processor.");
msgdef_ddts_component("");


msgdef(VERSIONCK, UCODE, LOG_ERR, 0,
       "Inappropriate version %u.%u for %s");
msgdef_explanation(
	"An attempt was made to load an inappropriate version of microcode.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(NOBUF, UCODE, LOG_ERR, 0,
       "Unable to allocate memory for ucode buffer");
msgdef_explanation(
	"A buffer is required to decompress microcode before loading it into an
	interface processor. This buffer could not be allocated for some
	reason.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(NOROM, UCODE, LOG_ERR, 0,
       "Invalid attempt to load ROM ucode in slot %d");
msgdef_explanation(
	"Some interface processors cannot load microcode from ROM.");
msgdef_recommended_action(
	"Remove the erroneous configuration statement.");
msgdef_ddts_component("");



msgdef(IPCINITFAIL, UCODE, LOG_ERR, 0,    
       "Unable to initialize IPC %s services");
msgdef_explanation(
	"The InterProcess Communication (IPC) service used to download
	microcode to certain interface processors has failed to initialize.");
msgdef_recommended_action(
	"Certain Interface Processors will not load properly. The router must
	be reloaded to clear the problem.");
msgdef_ddts_component("");


msgdef(IPCBUFFAIL, UCODE, LOG_ERR, 0,    
       "Unable to obtain IPC resources");
msgdef_explanation(
	"The InterProcess Communication (IPC) service used to download
	microcode to certain interface processors has failed to obtain a
	buffer.");
msgdef_recommended_action(
	"Certain interface processors will not load properly.  Try the reload
	command again. If this message recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(IPCINVALID, UCODE, LOG_ERR, 0,    
       "Invalid IPC request (%d) received from (%x)");
msgdef_explanation(
	"The InterProcess Communication (IPC) service used to download
	microcode to certain interface processors has received an invalid
	message.");
msgdef_recommended_action(
	"Verify that the proper revisions of code are selected.");
msgdef_ddts_component("");
