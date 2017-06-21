/* $Id: atmtest.c,v 3.2.58.1 1996/03/18 19:00:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/atm-test/atmtest.c,v $
 *------------------------------------------------------------------
 * atmtest_init.c -- To make atmtest a subsystem.
 * August 1995, Biren Fondekar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest.c,v $
 * Revision 3.2.58.1  1996/03/18  19:00:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:31:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:37:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:47:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:02:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "subsys.h"
#include "ttysrv.h"
#include "config.h"
#include "interface_private.h"
#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_private.h"
#include "../tcl/tcl.h"
#include "../atm-test/atmtest.h"
#include "../atm-test/atmtest_sig_cmds.h"
#include "../atm-test/atmtest_lane_cmds.h"

/*
 * atmtest subsystem header
 */
#define ATMTEST_MAJVERSION  1
#define ATMTEST_MINVERSION  0
#define ATMTEST_EDITVERSION 1

SUBSYS_HEADER(atmtest, ATMTEST_MAJVERSION, ATMTEST_MINVERSION, ATMTEST_EDITVERSION,
	      atmtest_init, SUBSYS_CLASS_LIBRARY,
              "seq: tcl",
              "req: tcl");

/*
 * atmtest_init
 * Define the subsystem main function. Add some services
 */
void atmtest_init (subsystype *subsys)
{

    /*
     * If the Tcl interpreter does not already exist, create and
     * initialize it.
     */
    if (interp == NULL) 
       interp = Tcl_CreateInterp();

    /*
     * Now create the atmtest commands
     */ 
    atmtest_create_commands(interp);
}

/*
 * atmtest_create_commands
 * Create new Tcl commands required to implement the LANE/ATM Signalling
 * test tool.
 */

void atmtest_create_commands (Tcl_Interp *interp)
{
  /*
   * LANE Send message commands
   * 
   * These commands are used to test the different LAN Emulation
   * modules by allowing the user to compose and send LANE control
   * packets out a specified interface.
   */
  Tcl_CreateCommand(interp, "tclhelp", Tcl_HelpCmd, 
		    (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "lsend", Tcl_LaneSendCmd,
		    (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "set_lane_def", Tcl_SetLaneDefCmd, 
		    (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sh_vars", Tcl_ShowVarsCmd, 
		    (ClientData) NULL, NULL);

  /*
   * ATM Signalling related message commands
   *
   * These commands are used to test ATM signalling related functionality.
   *
   */
  Tcl_CreateCommand(interp, "si",  Tcl_siCmd,  (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sln", Tcl_slnCmd, (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "scn", Tcl_scnCmd, (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "ss",  Tcl_ssCmd,  (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sc",  Tcl_scCmd,  (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sr",  Tcl_srCmd,  (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "src", Tcl_srcCmd, (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sap", Tcl_sapCmd, (ClientData) NULL, NULL);
  Tcl_CreateCommand(interp, "sdp", Tcl_sdpCmd, (ClientData) NULL, NULL);
}



