/* $Id: exec_show_hub.h,v 3.3.10.1 1996/08/12 16:04:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_hub.h,v $
 *------------------------------------------------------------------
 * HUB show commands
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: exec_show_hub.h,v $
 * Revision 3.3.10.1  1996/08/12  16:04:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/21  17:27:44  widmer
 * CSCdi49001:  privilege level configuration doesnt take hub ether
 * Make hub macro priv 0
 *
 * Revision 3.2  1995/11/17  17:37:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:24:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * show hub [{ether} <hub-num> [<port-num0> [<port-num1>]]]
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <port-num0>
 * OBJ(int,4) = <port-num1>
 * OBJ(int,5) = TRUE iff a hub-num was entered
 * OBJ(int,6) = TRUE iff a port-num0 was entered
 */

EOLNS	  (show_hub_eol, show_hub_command);
SET       (show_hub_port_num_set, show_hub_eol, OBJ(int,6), TRUE);
HUB_RANGE(show_hub_port_num, show_hub_port_num_set, show_hub_eol,
	        OBJ(int,3), OBJ(int,4), "Port number");
SET       (show_hub_num_set, show_hub_port_num, OBJ(int,5), TRUE);
HUB	    (show_hub_hub, show_hub_num_set, show_hub_eol,
	        OBJ(int,1), OBJ(int,2), "Hub to show");
KEYWORD   (show_hub, show_hub_hub, ALTERNATE,
	        "hub", "Hub status and configuration", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_hub
