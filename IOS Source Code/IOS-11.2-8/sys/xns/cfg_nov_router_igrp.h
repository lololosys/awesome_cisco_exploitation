/* $Id: cfg_nov_router_igrp.h,v 3.2.4.2 1996/08/12 16:09:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_nov_router_igrp.h,v $
 *------------------------------------------------------------------
 * Parser chains for IPX EIGRP
 *
 * March 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_nov_router_igrp.h,v $
 * Revision 3.2.4.2  1996/08/12  16:09:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.4.1  1996/05/03  02:15:57  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.2  1996/03/19  03:06:30  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1  1996/03/18  19:03:57  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 * log-event-type [dual] [xmit] [transport]
 * no log-event-type
 *
 * OBJ(int,1) = TRUE if logging DUAL events
 * OBJ(int,2) = TRUE if logging transmission events
 * OBJ(int,3) = TRUE if logging transport events
 */
pdecl(nov_igrp_log_options);
EOLNS (nov_igrp_log_eol, novigrp_event_log_type);
KEYWORD_ID (nov_igrp_log_transport, nov_igrp_log_options, nov_igrp_log_eol,
	    OBJ(int,3), TRUE,
	    "transport", NULL /*"Log transport events"*/, PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (nov_igrp_log_xmit, nov_igrp_log_options, nov_igrp_log_transport,
	    OBJ(int,2), TRUE,
	    "xmit", NULL /*"Log transmission events"*/, PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (nov_igrp_log_dual, nov_igrp_log_options, nov_igrp_log_xmit,
	    OBJ(int,1), TRUE,
	    "dual", NULL /*"Log DUAL events"*/, PRIV_CONF|PRIV_HIDDEN);
NOP	(nov_igrp_log_options, nov_igrp_log_dual, NONE);
NOPREFIX(nov_igrp_log_no, nov_igrp_log_options, nov_igrp_log_eol);
NVGENNS	(nov_igrp_log_nv, nov_igrp_log_no, novigrp_event_log_type);
KEYWORD (nov_igrp_log, nov_igrp_log_nv, ALTERNATE,
	 "log-event-type", NULL /*"Set event types logged"*/,
	 PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * event-log-size <nn>
 * no event-log-size
 *
 * OBJ(int,1) = number of entries
 *
 * Used to set the EIGRP event log size
 */
PARAMS_KEYONLY(nov_igrp_evlogsize, nov_igrp_log, "event-log-size",
	       OBJ(int,1), 0, MAXUINT,
	       novigrp_event_log_size, 0,
	       NULL /*Set IP-EIGRP event log size"*/,
	       NULL /*"Event log size"*/, PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * log-neighbor-changes
 * no log-neighbor-changes
 *
 * Used to log EIGRP routing events to enabled terminals or syslogd.
 */
EOLNS (nov_igrp_nbrlog_eol, novigrp_log_nbr_changes);
KEYWORD (nov_igrp_nbrlog, nov_igrp_nbrlog_eol,
 	 nov_igrp_evlogsize, "log-neighbor-changes",
  	 "Enable/Disable IPX-EIGRP neighbor logging", PRIV_CONF);
  
  
/**********************************************************************
 * Format:
 * [no] event-logging
 *
 * Used to log EIGRP routing events to enabled terminals or syslogd.
 *
 * router subcommand ("ipx router eigrp" chain only)
 */
EOLNS (nov_igrp_evlog_eol, novigrp_event_logging);
KEYWORD (nov_igrp_evlog_kw, nov_igrp_evlog_eol, nov_igrp_nbrlog,
	 "event-logging", NULL /*"Log IPX-EIGRP events"*/,
	 PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * Format:
 * [no] timers active-time [<max-active-time> | disabled]
 *
 * OBJ(int,1) = <max-active-time>
 *
 * router subcommand 
 */
EOLNS    (nov_router_timers_active_eol, novigrp_timers);
NUMBER  (nov_router_timers_active_time, nov_router_timers_active_eol,
	 nov_router_timers_active_eol,
	 OBJ(int,1), 1, MAXUINT, "EIGRP active-state time limit in minutes");
KEYWORD (nov_router_timers_active_disabled,
	 nov_router_timers_active_eol, nov_router_timers_active_time,
	 "disabled", "disable EIGRP time limit for active state", PRIV_CONF);
NOPREFIX(nov_router_timers_active_noprefix, nov_router_timers_active_disabled,
	 nov_router_timers_active_eol);
NVGENNS  (nov_router_timers_active_nvgen, nov_router_timers_active_noprefix,
	 novigrp_timers);
KEYWORD (nov_router_timers_active, nov_router_timers_active_nvgen, no_alt,
	 "active-time", "EIGRP time limit for active state", PRIV_CONF);
KEYWORD (nov_router_timers_kw, nov_router_timers_active, nov_igrp_evlog_kw,
	 "timers", "Adjust routing timers", PRIV_CONF);

IFELSE  (nov_igrp_evlog, nov_router_timers_kw, ALTERNATE,
	 (((novpdbtype *)csb->protocol)->proctype == NET_IGRP));

#undef  ALTERNATE
#define ALTERNATE 	nov_igrp_evlog

