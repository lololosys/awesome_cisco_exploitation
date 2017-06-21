/* $Id: cfg_router_igrp2.h,v 3.1.62.1 1996/04/02 05:36:47 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_igrp2.h,v $
 *------------------------------------------------------------------
 * cfg_router_igrp2.h - IP EIGRP router specific commands
 *
 * July 1995, David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_igrp2.h,v $
 * Revision 3.1.62.1  1996/04/02  05:36:47  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1  1995/11/09  12:05:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/04  03:07:46  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * eigrp event-log-size <nn>
 * no eigrp event-log-size
 *
 * OBJ(int,1) = number of entries
 *
 * Used to set the EIGRP event log size
 */
EOLS (router_igrp2_evlogsize_eol, ipigrp2_commands, IPIGRP2_EVENT_LOG_SIZE);
NUMBER  (router_igrp2_evlogsize_size, router_igrp2_evlogsize_eol, no_alt,
	 OBJ(int,1), 0, MAXUINT, "Event log size");
NOPREFIX(router_igrp2_evlogsize_no, router_igrp2_evlogsize_size,
 	 router_igrp2_evlogsize_eol);
NVGENS  (router_igrp2_evlogsize_nv, router_igrp2_evlogsize_no,
 	 ipigrp2_commands, IPIGRP2_EVENT_LOG_SIZE);
KEYWORD (router_igrp2_evlogsize, router_igrp2_evlogsize_nv,
 	 no_alt, "event-log-size",
 	 "Set IP-EIGRP event log size", PRIV_CONF|PRIV_HIDDEN);
 
/**********************************************************************
 * eigrp event-logging
 * no eigrp event-logging
 *
 * Used to log EIGRP routing events to enabled terminals or syslogd.
 */
EOLS (router_igrp2_evlog_eol, ipigrp2_commands, IPIGRP2_EVENT_LOGGING);
KEYWORD (router_igrp2_evlog, router_igrp2_evlog_eol,
 	 router_igrp2_evlogsize, "event-logging",
  	 "Log IP-EIGRP routing events", PRIV_CONF|PRIV_HIDDEN);
  
  
/**********************************************************************
 * eigrp log-neighbor-changes
 * no eigrp log-neighbor-changes
 *
 * Used to log EIGRP routing events to enabled terminals or syslogd.
 */
EOLS (router_igrp2_nbrlog_eol, ipigrp2_commands, IPIGRP2_NBR_LOGGING);
KEYWORD (router_igrp2_nbrlog, router_igrp2_nbrlog_eol,
 	 router_igrp2_evlog, "log-neighbor-changes",
  	 "Enable/Disable IP-EIGRP neighbor logging", PRIV_CONF);
  
  
/**********************************************************************
 * [no] eigrp kill-everyone
 *
 * Used to log EIGRP routing events to enabled terminals or syslogd.
 */
EOLS (router_igrp2_kill_eol, ipigrp2_commands, IPIGRP2_KILL_EVERYONE);
KEYWORD (router_igrp2_kill, router_igrp2_kill_eol,
	 router_igrp2_nbrlog, "kill-everyone",
	 "Kill all adjacencies on SIA", PRIV_CONF|PRIV_HIDDEN);


/**********************************************************************
 * eigrp log-event-type [dual] [xmit] [transport]
 * no eigrp log-event-type
 *
 * OBJ(int,1) = TRUE if logging DUAL events
 * OBJ(int,2) = TRUE if logging transmission events
 * OBJ(int,3) = TRUE if logging transport events
 */
pdecl(router_igrp2_log_options);
EOLS (router_igrp2_log_eol, ipigrp2_commands, IPIGRP2_LOG);
KEYWORD_ID (router_igrp2_log_transport, router_igrp2_log_options,
 	    router_igrp2_log_eol, OBJ(int,3), TRUE, "transport",
 	    "Log transport events", PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (router_igrp2_log_xmit, router_igrp2_log_options,
 	    router_igrp2_log_transport, OBJ(int,2), TRUE, "xmit",
 	    "Log transmission events", PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (router_igrp2_log_dual, router_igrp2_log_options,
 	    router_igrp2_log_xmit, OBJ(int,1), TRUE, "dual",
 	    "Log DUAL events", PRIV_CONF|PRIV_HIDDEN);
NOP	(router_igrp2_log_options, router_igrp2_log_dual, NONE);
NOPREFIX(router_igrp2_log_no, router_igrp2_log_options, router_igrp2_log_eol);
NVGENS	(router_igrp2_log_nv, router_igrp2_log_no, ipigrp2_commands,
 	 IPIGRP2_LOG);
KEYWORD (router_igrp2_log, router_igrp2_log_nv, router_igrp2_kill,
	 "log-event-type", "Set event types logged", PRIV_CONF|PRIV_HIDDEN);


/**********************************************************************
 * eigrp router sub-command chain head
 */

KEYWORD (router_igrp2_kw, router_igrp2_log, NONE,
	 "eigrp", "EIGRP specific commands", PRIV_CONF);

ASSERT	(router_igrp2, router_igrp2_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype == PROC_IGRP2));

#undef	ALTERNATE
#define	ALTERNATE	router_igrp2
