/* $Id: msg_s4t68360.c,v 3.8.18.5 1996/09/14 00:01:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_s4t68360.c,v $
 *------------------------------------------------------------------
 * msg_s4t68360.c - Declaration of VIP 4T Serial Port Adapter messages
 *
 * 03/17/95     H.M. Robison     Created.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_s4t68360.c,v $
 * Revision 3.8.18.5  1996/09/14  00:01:29  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/08/28  13:07:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.18.3  1996/08/22  18:45:37  dujihara
 * CSCdi65825:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.8.18.2  1996/06/18  06:45:16  amcrae
 * CSCdi60053:  Ping on 4T fails with MTU 8000 on vip2
 * Branch: California_branch
 * VIP2 transmit ring enlarged. Sanity check for many particled packets
 *
 * Revision 3.8.18.1  1996/06/07  04:08:23  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.8  1996/02/11  20:24:16  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Read back cksm after '360 down load, purge magic numbers from init block.
 *
 * Revision 3.7  1996/02/09  01:53:59  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Use variable delay to bound timeout for '360 download and init.
 *
 * Revision 3.6  1996/02/08  02:16:45  rharagan
 * CSCdi48368:  4T driver should throw away one byte tx pkts
 * Throw'em away, don't propagate them.
 *
 * Revision 3.5  1996/01/20  04:59:31  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.4  1995/12/02  05:50:30  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/12/01  02:00:13  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:55:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(S4T68360);			/* Define the facility */
msgdef_section("S4T68360 error messages relate to the 4-port synchronous serial
	adapter based on the 68360 processor.");


msgdef(OWNERR, S4T68360, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "S4T68360 (%d/%d), Buffer ownership error, pak=0x%x.");
msgdef_explanation(
	"A software or hardware error occurred. The S4T driver detected that
	the transmit ring is an inconsistent and unrecoverable state.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PANIC, S4T68360, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "S4T68360 (bay %d),  Exception %d, trace %d");
msgdef_explanation(
	"The S4T firmware detected an illegal or unexpected CPU exception or
	condition. This may be caused by a software error or by a hardware
	failure.");
msgdef_recommended_action(
	"Perform a microcode reload of the Versatile Interface Processor (VIP).
	If the problem persists, it indicates that the S4T hardware failed.");
msgdef_ddts_component("");


msgdef(TOOSMALL, S4T68360, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "S4T68360, %s - packet was less than two bytes");
msgdef_explanation(
	"An output packet was detected that was smaller than the minimum
	allowable datagram size. An error in another driver or an error in the
	system software probably triggered this error message.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RINGSIZE, S4T68360, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "S4T68360, %s - too many particles for ring");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(MBXREAD, S4T68360, LOG_ERR, 0,
	     "S4T68360 (bay %d),Stale msg in %s - mbx0:%lx mbx1:%lx  mbx2:%lx", 
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The S4T has not responded to a message from the Versatile Interface
	Processor (VIP) within a specified time.");
msgdef_recommended_action(
	"Perform a microcode reload of the VIP. If the problem persists, it
	indicates that the S4T or VIP hardware failed.");
msgdef_ddts_component("");


msgdef(NOTS4T68360, S4T68360, LOG_ERR, 0,
       "S4T68360 (bay %d): Device reported %#x");
msgdef_explanation(
	"The S4T hardware failed. A non-S4T device pointed at the software for
	the S4T.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOTREADY, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d), Not ready for download.");
msgdef_explanation(
	"The S4T port adaptor hardware failed. The S4T port adaptor would not
	begin to download the operational microcode.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RESTART, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d), Restarting");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DWNLDFL, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d): PA down load failed.");
msgdef_explanation(
	"The S4T hardware has failed. It could not download its operational
	microcode.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(DWNLDCKSM, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d), Download checksum error (sent = 0x%x, received = 0x%x");
msgdef_explanation(
	"The download of the internal firmware to the S4T failed to checksum
	correctly. This usually indicates a hardware failure of the S4T or the
	Versatile Interface Processor (VIP).");
msgdef_recommended_action(
	"Perform a power cycle. If the problem persists, the S4T hardware may
	have a problem.");
msgdef_ddts_component("");


msgdef(NOTCMPLT, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d), PA down load failed to complete.");
msgdef_explanation(
	"The S4T port adaptor hardware failed. It did not acknowledge the
	completion of the operational microcode download.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INITFAIL, S4T68360, LOG_ALERT, 0,
       "S4T68360 (bay %d),  Init Failed at %s");
msgdef_explanation(
	"The S4T port adaptor failed to complete hardware initialization.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(STARTFAIL, S4T68360, LOG_ALERT, 0,
       "S4T68360, %s Start Failed at %s");
msgdef_explanation(
	"A software or hardware error occurred. The S4T interface is not
	responding to commands used to initialize it.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(STOPFAIL, S4T68360, LOG_ALERT, 0,
       "S4T68360, %s Stop Failed at %s");
msgdef_explanation(
	"The S4T port adaptor failed to respond to a request to disable an
	interface.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
