/* $Id: msg_tr.c,v 3.3.60.3 1996/09/13 23:49:31 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/msg_tr.c,v $
 *------------------------------------------------------------------
 * msg_tr.c - Message file for tr facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tr.c,v $
 * Revision 3.3.60.3  1996/09/13  23:49:31  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/08/28  12:48:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.1  1996/05/17  11:22:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/05/02  22:04:37  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  14:38:15  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/11/17  09:30:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:53:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TR);
msgdef_section("Token Ring error messages");


/* Messages in all images: */
msgdef(PANICTYPE, TR, LOG_ERR, 0, "Unit %d, %s error");
msgdef_explanation(
	"This message is similar to the %TR-3-PANIC error message, but
	indicates a nonfatal error. This message appears in very unusual
	situations that should not arise in normal operation.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PANIC, TR, LOG_ERR, 0, "Unit %d, panic %-08x %-08x %-08x");
msgdef_explanation(
	"The Token Ring monitor firmware detected a fatal error that indicates
	an impending interface failure.");
msgdef_recommended_action(
	"Copy the error message and the interface message exactly as they
	appear and then report them to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(PANICINF, TR, LOG_ERR, 0,
       "Unit %d, PI %-08x %-08x %-08x %-08x %-08x %-08x");
msgdef_explanation(
	"This message is similar to the "%TR-3-PANIC" error message, but
	indicates a nonfatal error. This message appears in very unusual
	situations that should not arise in normal operation.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADMUL, TR, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Unit %d, Can't set TR address to hardware multicast address %e");
msgdef_explanation(
	"An attempt was made to set the Token Ring interface MAC address to a
	reserved multicast address.");
msgdef_recommended_action(
	"Check your configuration. Make sure that your XNS and/or Novell Token
	Ring addresses have not inadvertently been set to reserved multicast
	addresses.");
msgdef_ddts_component("");


msgdef(BADSTART, TR, LOG_ERR, 0,
       "Unit %d, Start completion and wrong idb state - state= %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(RESETFAIL, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, reset failed, error code %-08x.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(STATE, TR, LOG_INFO, 0, "TR%d: Ring Status: %s");
msgdef_explanation(
	"This message is displayed when the Token Ring's status changed as
	determined by the chipset. This information also is used to
	automatically determine whether the interface is still usable to
	propagate network traffic.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(STATRING, TR, LOG_INFO, 0, "TR%d: Ring Status: %s %s");
msgdef_explanation(
	"This message is displayed when the Token Ring's status changed as
	determined by the chipset. This information also is used to
	automatically determine whether the interface is still usable to
	propagate network traffic. The meaning of each status code is shown in
	the "Token Ring Status Codes" Table.

	Code	Explanation			Fatal
	0x8000	Signal loss			Yes
	0x4000	Hard error			Yes
	0x2000	Soft error			No
	0x1000	Transmit beacon			Yes
	0x0800	Lobe wire fault			Yes
	0x0400	Auto removal error		Yes
	0x0100	Receive request removed		Yes
	0x0080	Counter overflow		No
	0x0040	Single station			No
	0x0020	Ring recovery			No
");
msgdef_recommended_action(
	"Check the Token Ring for the indicated condition.");
msgdef_ddts_component("");


msgdef(INTFAIL, TR, LOG_WARNING, 0,
       "Unit %d interface failure: %#x %#x %#x, idb state %d");
msgdef_explanation(
	"The Token Ring Monitor firmware detected a fatal error due either to
	an internal software problem or to a hardware failure.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADRNGNUM, TR, LOG_ERR, 0,
       "Unit %d, ring number (%d) doesn't match established number (%d).");
msgdef_explanation(
	"The number you have configured for the local ring does not match the
	value currently in use on the ring.");
msgdef_recommended_action(
	"Check the configuration to make sure you used the correct ring number.
	If it is correct, confirm the configuration of all other bridges on the
	ring to make sure they are using the same ring number.");
msgdef_ddts_component("");


msgdef(WIREFAULT, TR, LOG_ERR, 0, "Unit %d, wire fault: check the lobe cable MAU connection.");
msgdef_explanation(
	"The Token Ring hardware is reporting a wire fault condition.");
msgdef_recommended_action(
	"Check the cable connecting the router to the Token Ring media
	attachment unit (MAU).");
msgdef_ddts_component("");


msgdef(OPENFAIL, TR, LOG_ERR, 0, "Unit %d, open failed: %s, %s");
msgdef_explanation(
	"The Token Ring interface was unable to insert itself into the ring.
	This is an indication of a busy or broken ring. The first character
	string in this error message indicates the stage of initialization at
	which the error occurred, and the second describes the error.");
msgdef_recommended_action(
	"Try to open the interface again. This generally can be accomplished by
	issuing the clear interface tokenring command. If the error message
	recurs, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(OPENFAIL2, TR, LOG_ERR, 0, "Unit %d, open failed: check the lobe cable DB-9 connection.");
msgdef_explanation(
	"The Token Ring interface was unable to insert itself into the ring,
	and the error code returned indicates a wiring problem.");
msgdef_recommended_action(
	"Check the cable connecting the router to the Token Ring MAU, and try
	to open the interface again. This generally can be accomplished by
	issuing the clear interface tokenring command. If the error message
	recurs, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(SETGRPFAIL, TR, LOG_ERR, 0,
       "Unit %d, set group address failed (code %#x).");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SETFUNFAIL, TR, LOG_ERR, 0,
       "Unit %d, set functional address failed (code %#x).");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(RIFTBLTRASH, TR, LOG_ERR, MSG_TRACEBACK,
       "Trashed %s RIF table entry (%#x) found and zeroed.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(RIFBADMAGIC, TR, LOG_ERR, MSG_TRACEBACK,
       "Invalid magic number (%#x) in RIF %s table entry (%#x).");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


/* Messages used by TRIP boards */
msgdef(SETBRIDGEFAIL, TR, LOG_ERR, 0,
       "Unit %d, set bridge failed (code %#x).");
msgdef_explanation(
	"A hardware error occurred. The source routing accelerator (SRA) chip
	on the Token Ring card could not be initialized.");
msgdef_recommended_action(
	"Call your technical support representative for assistance or a
	replacement card.");
msgdef_ddts_component("");


/* Messages used by TMS380 driver */
msgdef(BADUNIT, TR, LOG_ERR, 0,
       "Bad unit number %d");
msgdef_explanation(
	"This error message pertains only to the IGS. When displayed, it
	indicates that the system cannot find the chipset registers where it
	expects them to be. This is most likely a hardware error.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance. It will be necessary to determine the hardware unit
	configuration.");
msgdef_ddts_component("");


msgdef(BADBUFFSIZE, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, bad buffersize = %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADFUNC, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, bad functional address=0x%08x, ring mode=0x%08x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADBRDGPARMS, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, bad bridge parameters: bridge_num=%d, max_rd=%d\nthisring=%d, targetring=%d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INITFAIL, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, init failed. result code=%#x, error code=%#x");
msgdef_explanation(
	"The Token Ring hardware failed to initialize properly.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MODEFAIL, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, change mode failed. result code=%#x, error code=%#x");
msgdef_explanation(
	"An internal hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOFIRM, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, no TMS380 firmware present. eagle=%#x, magic=%#x");
msgdef_explanation(
	"An internal hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADFIRMTYPE, TR, LOG_ERR, MSG_TRACEBACK,
       "Unit %d, bad firmware type code during %s. type=%#x, fw=%#x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADFIRM, TR, LOG_ERR, 0, "Unit %d, Tokenring firmware download failed\n  got %#x, expected %#x, at address 0x%04x.%04x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, TR, LOG_ERR, 0,
	     "Unit %d, no memory for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


/* Message used by VIP 4R port adapter */
msgdef(DIAGFAIL, TR, LOG_ERR, 0,
       "Interface %s failed init diags (0x%04x), %s");
msgdef_explanation(
	"The microcode attempted to run its diagnostics on the chip and
	failed.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(ADPCHK, TR, LOG_ERR, 0,
       "Interface %s, adapter check error (%04x %04x %04x %04x)");
msgdef_explanation(
	"The Token Ring monitor firmware detected a fatal error on the
	interface card.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");
