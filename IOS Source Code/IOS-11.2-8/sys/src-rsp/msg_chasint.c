/* $Id: msg_chasint.c,v 3.3.8.2 1996/09/14 00:01:59 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/msg_chasint.c,v $
 *------------------------------------------------------------------
 * Messages for high-end chassis interface
 *
 * October 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_chasint.c,v $
 * Revision 3.3.8.2  1996/09/14  00:01:59  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.8.1  1996/08/28  13:13:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/02/28  09:33:45  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.2  1995/11/17  18:46:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/10  19:11:55  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.2  1995/07/17  08:39:47  schaefer
 * CSCdi36180: medley of chassis interface fixes
 * 7513 variable speed blower, shutdown after 7505 fan failure
 * 7507 support, 7513 fan failure warnings
 *
 * Revision 2.1  1995/06/07  22:57:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CI);
msgdef_section("Chassis Interface (75xx platform environmental monitor card)
	error messages");


msgdef(NOTFOUND, CI, LOG_WARNING, 0, "Chassis Interface not found");
msgdef_explanation(
	"The system software could not detect the presence of a CI card.");
msgdef_recommended_action(
	"Verify that the CI card is correctly and fully installed in the
	backplane. If the error recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SLOTCOMP, CI, LOG_ERR, 0, "Inconsistent %s slot %02x");
msgdef_explanation(
	"The CI software detected a possible inconsistency in the CI
	hardware.");
msgdef_recommended_action(
	"Confirm that the CI card is correctly and fully installed in the
	backplane. If error recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(CTRLRTYPE, CI, LOG_ERR, 0, "Wrong controller type %02x");
msgdef_explanation(
	"The CI was manufactured incorrectly.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INT, CI, LOG_CRIT, 0, "Unexpected interrupt - status %02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(COND, CI, LOG_WARNING, 0,
       "Restarting with %u recent soft power shutdowns\n\t\t(or system just powered-on)");
msgdef_explanation(
	"A soft power failure occurs when the router shuts itself down due to a
	detected over temperature condition. This is called a soft shutdown.
	When the temperature goes up past the "board shutdown" trip point the
	cards are shutdown but the power supplies, fans, and CI continue to
	run.

	If the system then cools past the restart trip point, the system will
	restart.  The system counts the number of times this happens to keep
	the up/down cycle from repeating indefinitely.  If the above counter
	ever reaches zero, the system undergoes a hard shutdown, which requires
	a power cycle to recover.  The soft shutdown counter is reset to its
	maximum value after the system has been up for six hours.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOACK, CI, LOG_ERR, MSG_TRACEBACK,
       "%s acknowledge failure, cmd %02x, addr %02x, data %02x");
msgdef_explanation(
	"The CI software failed to communicate with the CI hardware.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BPLANE, CI, LOG_INFO, 0, "CI type %u differs from NVRAM type %u");
msgdef_explanation(
	"The chassis type as reported by the CI is different from that
	programmed into NVRAM.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(ENVWARN, CI, LOG_WARNING, 0, "%s measured at %s");
msgdef_explanation(
	"One of the measured environmental test points exceeds the warning
	threshold.");
msgdef_recommended_action(
	"Closely monitor the condition and correct if possible. If the
	situation worsens, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(ENVCRIT, CI, LOG_CRIT,    0, "%s measured at %s");
msgdef_explanation(
	"One of the measured environmental test points exceeds an extreme
	threshold.");
msgdef_recommended_action(
	"Correct the specified condition or the router may shut itself down or
	fail to operate properly.");
msgdef_ddts_component("");


msgdef(PSCHANGE, CI, LOG_INFO, 0,
       "Power supply %u changed from %s to %s");
msgdef_explanation(
	"The CI reported that a power supply was either installed, removed, or
	changed.");
msgdef_recommended_action(
	"If the situation does not correspond with a physical change in the
	system's power supplies, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(PSFAIL, CI, LOG_ERR, 0, "Power supply %u failure");
msgdef_explanation(
	"One of the router's power supplies failed.");
msgdef_recommended_action(
	"Replace the failed power supply as soon as possible.");
msgdef_ddts_component("");


msgdef(BLOWER, CI, LOG_ERR, 0, "%s fan failure");
msgdef_explanation(
	"One of the router's cooling fans failed.");
msgdef_recommended_action(
	"Replace the fan as soon as possible or the router may shut itself down
	or fail to operate properly.");
msgdef_ddts_component("");


msgdef(BLOWSHUT, CI, LOG_ALERT, 0,
       "Blower failure, shutdown in %d seconds");
msgdef_explanation(
	"One of the six blowers in a five-slot chassis has failed. Because each
	blower cools a separate region of the cards in the chassis, all blowers
	are needed for router operation. If a blower failure persists for 2
	minutes, the router will shut itself down and will need to be power
	cycled.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MAXOUTPUT, CI, LOG_WARNING, 0,
       "Power supply %u %dV is at or near rated output");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SYSLOAD, CI, LOG_WARNING, 0,
       "System %dV load exceeds single power supply %u capacity");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNBALANCE, CI, LOG_WARNING, 0,
       "Unbalanced %dV output from dual power supplies");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
