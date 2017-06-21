/* $Id: msg_c7100.c,v 3.2.58.6 1996/09/14 00:01:56 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/msg_c7100.c,v $
 *------------------------------------------------------------------
 * msg_c7100.c - Predator error messages
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_c7100.c,v $
 * Revision 3.2.58.6  1996/09/14  00:01:56  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/08/28  13:11:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.4  1996/05/31  23:44:27  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/05/21  20:44:19  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.58.2  1996/04/06  02:05:47  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:48:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.5  1996/02/15  19:52:05  dcarroll
 * CSCdi49083:  Fix c7100 envm temp sensor support
 * Branch: ELC_branch
 * Add volage, temperature and snmp fixes.
 *
 * Revision 3.2.2.4  1996/01/23  02:18:16  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.2.2.3  1995/12/13  02:23:50  mbeesley
 * Branch: ELC_branch
 * Fix misdocumented register bit defintions. Add error interrupt
 * masks. Clear OIR interrupts at boot time. Ensure that PCI Signals
 * Invalid is not set at port adaptor power on.
 *
 * Revision 3.2.2.2  1995/11/30  02:14:50  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:36  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(C7200);
msgdef_section("");


msgdef(IONOT0, C7200, LOG_ERR, 0,
       "IO card discover in non-zero slot %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOMAC, C7200, LOG_ERR, 0,
       "Can't allocate MAC address for interface %u/%u");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(COOKIE, C7200, LOG_WARNING, 0,
       "Corrupt or missing MAC address cookie\n"
       " using random base %e");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PCIVALID, C7200, LOG_WARNING, 0,
       "PCI activation failed, bay %d, 0x%x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOCPUVER, C7200, LOG_WARNING, 0,
       "Invalid CPU ID, assuming revision 1");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CONFIG, C7200, LOG_ERR, 0,
       "Recommended port adapter configuration exceeded");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * Environmental Monitor messages
 */
facdef(ENVM);
msgdef_section("");


msgdef(NOFORK, ENVM, LOG_ERR, 0,
       "Could not start environment polling process");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LASTENV, ENVM, LOG_ERR, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOACK, ENVM, LOG_ERR, MSG_TRACEBACK, "Access to %s failed");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(ENVWARN, ENVM, LOG_WARNING, 0, "%s measured at %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(ENVCRIT, ENVM, LOG_CRIT,    0, "%s measured at %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PSCHANGE, ENVM, LOG_INFO, 0, "%s changed from %s to %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PSLEV, ENVM, LOG_INFO, 0, "%s state changed from %s to %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BLOWER, ENVM, LOG_ERR, 0, "%s may have failed");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SHUT, ENVM, LOG_EMERG, 0, "Environmental Monitor initiated shutdown");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CPUCARD, ENVM, LOG_ERR, 0, "Unknown CPU type %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
