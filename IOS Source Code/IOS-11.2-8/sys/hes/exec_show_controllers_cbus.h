/* $Id: exec_show_controllers_cbus.h,v 3.4.4.1 1996/08/12 16:01:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_controllers_cbus.h,v $
 *------------------------------------------------------------------
 * Cbus show controllers commands 
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_cbus.h,v $
 * Revision 3.4.4.1  1996/08/12  16:01:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/03/07  12:30:43  dkerr
 * CSCdi50949:  Customer can crash router getting cbus lowcore dump
 * Set lowcore capture trigger by default and have results displayable
 * from "show controller cxbus memd-core".
 *
 * Revision 3.3  1996/01/29  07:08:40  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:15:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#if defined(CSC2) || defined(RSP)
#define SHOW_ON_RP	PRIV_HIDDEN
#else
#define SHOW_ON_RP	0
#endif

/******************************************************************
 * show controllers cbus memd-capture (7000)
 * show controllers cbus memd-core (7000)
 * show controllers cbus (on AGS+, 7000, RSP)
 * show controllers cxbus (for 7000 finger memory)
 *
 */
EOLNS	(show_control_cbus_eol, show_cbus);

KEYWORD_ID (show_control_memd_capture, show_control_cbus_eol,
	    show_control_cbus_eol,
            OBJ(int,2), TRUE,
            "memd-capture", "Capture and display current cbus memory",
	    PRIV_OPR | SHOW_ON_RP);
KEYWORD_ID (show_control_memd_core, show_control_memd_capture,
	    show_control_memd_capture,
            OBJ(int,1), TRUE,
            "memd-core", "Cbus core memory at time of CBUS-3-INITERR",
	    PRIV_USER | SHOW_ON_RP);
KEYWORD (show_control_cxbus, show_control_memd_core, NONE,
         "cxbus", NULL,
         PRIV_USER | PRIV_HIDDEN);
KEYWORD (show_control_cbus, show_control_memd_core, show_control_cxbus,
         "cbus", "Cbus interface internal state",
         PRIV_USER);

extern int ncBus;       
TEST_INT(show_control_cbus_test, show_control_cbus, NONE, ALTERNATE,
         ncBus);


#undef	ALTERNATE
#define	ALTERNATE	show_control_cbus_test
