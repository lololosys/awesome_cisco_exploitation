/* $Id: cfg_syslog_history.h,v 3.1.48.1 1996/05/21 06:36:30 mordock Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_syslog_history.h,v $
 *------------------------------------------------------------------
 * cfg_syslog_history.h
 *
 * September 1995, Anke Dosedal
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_syslog_history.h,v $
 * Revision 3.1.48.1  1996/05/21  06:36:30  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:22  anke
 * Placeholder for Syslog MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] logging history {emergencies | alerts | critical | errors | warnings |
 *                      notifications | informational | debugging }
 * [no] logging history size {1-500}
 *
 * csb->which = LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING,
 *              LOG_ERR, LOG_CRIT, LOG_ALERT or LOG_EMERG
 * OBJ(int,2) = maximum severity we want to hear from
 * OBJ(int,1) = size of syslog history table
 */
EOLS  (conf_log_history_eol, set_history_logging, P_LOG_HISTORY);
KEYWORD_ID(conf_log_history_debug, conf_log_history_eol, no_alt, 
         OBJ(int,2), LOG_DEBUG,
         "debugging", "Debugging messages", PRIV_ROOT);
KEYWORD_ID(conf_log_history_info, conf_log_history_eol,
         conf_log_history_debug,
         OBJ(int,2), LOG_INFO,
         "informational", "Informational messages", PRIV_ROOT);
KEYWORD_ID(conf_log_history_notify, conf_log_history_eol,
         conf_log_history_info,
         OBJ(int,2), LOG_NOTICE,
         "notifications", "Normal but significant conditions", PRIV_ROOT);
KEYWORD_ID(conf_log_history_warn, conf_log_history_eol,
         conf_log_history_notify,
         OBJ(int,2), LOG_WARNING,
         "warnings", "Warning conditions", PRIV_ROOT);
KEYWORD_ID(conf_log_history_errors, conf_log_history_eol,
         conf_log_history_warn,
         OBJ(int,2), LOG_ERR,
         "errors", "Error conditions", PRIV_ROOT);
KEYWORD_ID(conf_log_history_critical, conf_log_history_eol,
         conf_log_history_errors,
         OBJ(int,2), LOG_CRIT,
         "critical", "Critical conditions", PRIV_ROOT);
KEYWORD_ID(conf_log_history_alerts, conf_log_history_eol,
         conf_log_history_critical,
         OBJ(int,2), LOG_ALERT,
         "alerts", "Immediate action needed", PRIV_ROOT);
KEYWORD_ID(conf_log_history_emerg, conf_log_history_eol,
         conf_log_history_alerts,
         OBJ(int,2), LOG_EMERG,
         "emergencies", "System is unusable", PRIV_ROOT);

PARAMS_KEYONLY(conf_log_history_size, conf_log_history_emerg, "size",
             OBJ(int,1), 1, 500,
             set_history_logging, P_LOG_HISTORY_SIZE,
             "Set history table size", "History size", PRIV_ROOT);

KEYWORD_ID(conf_log_history, conf_log_history_size, ALTERNATE,
	   OBJ(int,3), P_LOG_HISTORY,
 	   "history", "Configure syslog history table", PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE   conf_log_history
