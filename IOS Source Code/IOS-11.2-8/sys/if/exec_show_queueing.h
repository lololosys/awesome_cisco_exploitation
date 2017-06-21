/* $Id: exec_show_queueing.h,v 3.3.26.3 1996/08/12 16:02:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_queueing.h,v $
 *------------------------------------------------------------------
 * Show current state of defined queue customization lists
 *
 * February 1993, Bruce Cole
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_queueing.h,v $
 * Revision 3.3.26.3  1996/08/12  16:02:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.26.2  1996/05/21  09:49:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.26.1  1996/04/17  13:40:07  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/18  02:12:56  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:27:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:25  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show queue <interface> [<queue-number>]
 *
 * OBJ(idb,1) = <interface>
 * OJB(int,1) = <queue-number>, default is DEFAULT_QUEUE() set in function.
 */
EOLNS (show_queue_eols, show_queue_command);
NUMBER (show_queue_num, show_queue_eols, show_queue_eols, OBJ(int,1),
	0, PRIORITY_MAXTYPE-1, "Queue number");
SET (show_queue_num_default, show_queue_num, OBJ(int,1), MAXUINT);
INTERFACE (show_queue_interface, show_queue_num_default, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB);
KEYWORD (show_queue, show_queue_interface, ALTERNATE,
	 "queue", "Show queue contents", PRIV_USER|PRIV_USER_HIDDEN);
  
/******************************************************************
 * show queueing [fair | custom | priority | interface <interface>]
 * 
 * OBJ(idb,1) = <interface>
 */
EOLNS	(show_queueing_eol, show_queueing_command);
EOLNS	(show_q_interface_eol, show_queue_counts);
KEYWORD_ID (show_queueing_fair, show_queueing_eol, show_queueing_eol,
	    OBJ(int,1), QUEUE_FAIR,
	    "fair", "fair queueing configuration", PRIV_USER);
KEYWORD_ID (show_queueing_red, show_queueing_eol, show_queueing_fair,
	    OBJ(int,1), QUEUE_RED,
	    "red", "random early detection configuration", PRIV_USER);
KEYWORD_ID (show_queueing_pri, show_queueing_eol, show_queueing_red,
	    OBJ(int,1), QUEUE_PRIORITY,
	    "priority", "priority queueing list configuration", PRIV_USER);
KEYWORD_ID (show_queueing_cust, show_queueing_eol, show_queueing_pri,
	    OBJ(int,1), QUEUE_CUSTOM,
	    "custom", "custom queueing list configuration", PRIV_USER);
SET (show_queueing_args, show_queueing_cust, OBJ(int,1), QUEUE_ALL);
INTERFACE (show_q_get_interface, show_q_interface_eol, no_alt,
      	   OBJ(idb,1), IFTYPE_HWIDB    );
KEYWORD (show_queueing_interface, show_q_get_interface,
	 show_queueing_args, common_str_interface, NULL, PRIV_HIDDEN);
KEYWORD_MM (show_queueing, show_queueing_interface, show_queue,
	    "queueing", "Show queueing configuration",
	    PRIV_USER|PRIV_USER_HIDDEN, 6);

#undef ALTERNATE
#define ALTERNATE show_queueing
