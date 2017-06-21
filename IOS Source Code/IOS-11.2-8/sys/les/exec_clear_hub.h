/* $Id: exec_clear_hub.h,v 3.3.10.1 1996/08/12 16:04:29 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_clear_hub.h,v $
 *------------------------------------------------------------------
 * HUB clear commands
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: exec_clear_hub.h,v $
 * Revision 3.3.10.1  1996/08/12  16:04:29  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/21  17:27:43  widmer
 * CSCdi49001:  privilege level configuration doesnt take hub ether
 * Make hub macro priv 0
 *
 * Revision 3.2  1995/11/17  17:36:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:15:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * clear hub {ether} <hub-num>
 * clear hub counters [{ether} <hub-num> [<port-num0>] [<port-num1>]]
 *
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <port-num0>
 * OBJ(int,4) = <port-num1>
 * OBJ(int,5) = TRUE iff a port-num was entered
 * OBJ(int,10) = TRUE == clear counters
 *		 FALSE == don't clear counters
 */

EOLNS	  (clear_hub_eol, clear_hub_command);
SET       (clear_hub_counters_port_set, clear_hub_eol, OBJ(int,5), TRUE);
HUB_RANGE(clear_hub_counters_port, clear_hub_counters_port_set, clear_hub_eol,
                OBJ(int,3), OBJ(int,4), "Port number");
TESTVAR   (clear_hub_test2, clear_hub_eol, no_alt,
	        NONE, NONE, NONE, OBJ(int,10), TRUE);
TESTVAR   (clear_hub_test1, clear_hub_counters_port, clear_hub_eol,
	        NONE, NONE, NONE, OBJ(int,10), TRUE);
HUB	  (clear_hub_hub, clear_hub_test1, clear_hub_test2,
	        OBJ(int,1), OBJ(int,2), "hub type");
KEYWORD_ID(clear_hub_counters, clear_hub_hub, clear_hub_hub,
	        OBJ(int,10), TRUE, "counters",
	        "Clear \"show hub\" counters", PRIV_CONF);
KEYWORD   (clear_hub, clear_hub_counters, ALTERNATE,
	        "hub", "Clear hub hardware/counters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	clear_hub
