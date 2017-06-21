/* $Id: cfg_logging.h,v 3.2.62.4 1996/06/19 04:43:56 smackie Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_logging.h,v $
 *------------------------------------------------------------------
 * C F G _ L O G G I N G . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_logging.h,v $
 * Revision 3.2.62.4  1996/06/19  04:43:56  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/06/18  10:32:27  che
 * CSCdi47953:  logging buffered can crash router
 * Branch: California_branch
 * Changed maximum size of logging buffer from MAXUINT to MAXINT.
 *
 * Revision 3.2.62.2  1996/06/17  23:35:13  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/21  06:36:28  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2  1995/11/17  18:45:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * logging <ipaddr>
 * no logging <ipaddr>
 *
 * (*OBJ(paddr,1)) = <ipaddr>
 *
 * The <ipaddr> is required in the 'no' form of the command.
 */
EOLS	(conf_log_eol, logger_command, P_LOG_HOST);
IPADDR_NAME(conf_log_addr, conf_log_eol, no_alt,
	    OBJ(paddr,1), "IP address of the logging host");
TEST_MULTIPLE_FUNCS(conf_log_assert, conf_log_addr, no_alt, NONE);
NVGENS   (conf_log_nvgen, conf_log_assert, logger_command, P_LOG_HOST);

LINK_TRANS(conf_log_extend_here, conf_log_nvgen);

/******************************************************************************
 *      logging source-interface <interface>
 *      no logging source-interface
 *      OBJ(idb,1) -- IDB to pull source IP address from
 */
EOLS	(conf_log_source_eol, logger_command, P_LOG_SOURCE);
FORWARD_INTERFACE(conf_log_source_name, conf_log_source_eol, no_alt,
	 OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX(conf_log_source_noprefix, conf_log_source_name,
	 conf_log_source_eol);
NVGENS	(conf_log_source_nvgen, conf_log_source_noprefix,
	 logger_command, P_LOG_SOURCE);
KEYWORD_ID(conf_log_source, conf_log_source_nvgen, conf_log_extend_here,
	 OBJ(int,3), P_LOG_SOURCE,
	 "source-interface",
	 "Specify interface for source address in logging transactions",
	 PRIV_CONF);

/******************************************************************
 * logging facility {kern|user|mail|daemon|auth|syslog|lpr|news|
 *		     uucp|sys9|sys10|sys11|sys12|sys13|cron|
 *		     local0|local1|local2|local3|local4|local5|local6|local7}
 * no logging facility
 */
EOLS	(clf_eol, logger_command, P_LOG_FACILITY );
KEYWORD_ID(clf_local7, clf_eol, no_alt, OBJ(int,4), P_LOG_FAC_LOCAL7,
	   "local7", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local6, clf_eol, clf_local7, OBJ(int,4), P_LOG_FAC_LOCAL6,
	   "local6", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local5, clf_eol, clf_local6, OBJ(int,4), P_LOG_FAC_LOCAL5,
	   "local5", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local4, clf_eol, clf_local5, OBJ(int,4), P_LOG_FAC_LOCAL4,
	   "local4", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local3, clf_eol, clf_local4, OBJ(int,4), P_LOG_FAC_LOCAL3,
	   "local3", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local2, clf_eol, clf_local3, OBJ(int,4), P_LOG_FAC_LOCAL2,
	   "local2", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local1, clf_eol, clf_local2, OBJ(int,4), P_LOG_FAC_LOCAL1,
	   "local1", "Local use", PRIV_CONF );
KEYWORD_ID(clf_local0, clf_eol, clf_local1, OBJ(int,4), P_LOG_FAC_LOCAL0,
	   "local0", "Local use", PRIV_CONF );
KEYWORD_ID(clf_cron, clf_eol, clf_local0, OBJ(int,4), P_LOG_FAC_CRON,
	   "cron", "Cron/at facility", PRIV_CONF );
KEYWORD_ID(clf_sys14, clf_eol, clf_cron, OBJ(int,4), P_LOG_FAC_SYS14,
	   "sys14", "System use", PRIV_CONF );
KEYWORD_ID(clf_sys13, clf_eol, clf_sys14, OBJ(int,4), P_LOG_FAC_SYS13,
	   "sys13", "System use", PRIV_CONF );
KEYWORD_ID(clf_sys12, clf_eol, clf_sys13, OBJ(int,4), P_LOG_FAC_SYS12,
	   "sys12", "System use", PRIV_CONF );
KEYWORD_ID(clf_sys11, clf_eol, clf_sys12, OBJ(int,4), P_LOG_FAC_SYS11,
	   "sys11", "System use", PRIV_CONF );
KEYWORD_ID(clf_sys10, clf_eol, clf_sys11, OBJ(int,4), P_LOG_FAC_SYS10,
	   "sys10", "System use", PRIV_CONF );
KEYWORD_ID(clf_sys9, clf_eol, clf_sys10, OBJ(int,4), P_LOG_FAC_SYS9,
	   "sys9", "System use", PRIV_CONF );
KEYWORD_ID(clf_uucp, clf_eol, clf_sys9, OBJ(int,4), P_LOG_FAC_UUCP,
	   "uucp", "Unix-to-Unix copy system", PRIV_CONF );
KEYWORD_ID(clf_news, clf_eol, clf_uucp, OBJ(int,4), P_LOG_FAC_NEWS,
	   "news", "USENET news", PRIV_CONF );
KEYWORD_ID(clf_lpr, clf_eol, clf_news, OBJ(int,4), P_LOG_FAC_LPR,
	   "lpr", "Line printer system", PRIV_CONF );
KEYWORD_ID(clf_syslog, clf_eol, clf_lpr, OBJ(int,4), P_LOG_FAC_SYSLOG,
	   "syslog", "Syslog itself", PRIV_CONF );
KEYWORD_ID(clf_auth, clf_eol, clf_syslog, OBJ(int,4), P_LOG_FAC_AUTH,
	   "auth", "Authorization system", PRIV_CONF );
KEYWORD_ID(clf_daemon, clf_eol, clf_auth, OBJ(int,4), P_LOG_FAC_DAEMON,
	   "daemon", "System daemons", PRIV_CONF );
KEYWORD_ID(clf_mail, clf_eol, clf_daemon, OBJ(int,4), P_LOG_FAC_MAIL,
	   "mail", "Mail system", PRIV_CONF );
KEYWORD_ID(clf_user, clf_eol, clf_mail, OBJ(int,4), P_LOG_FAC_USER,
	   "user", "User process", PRIV_CONF );
KEYWORD_ID(clf_kern, clf_eol, clf_user, OBJ(int,4), P_LOG_FAC_KERN,
	   "kern", "Kernel", PRIV_CONF );
NOPREFIX(conf_log_facility_noprefix, clf_kern, clf_eol );
KEYWORD_ID(conf_log_facility, conf_log_facility_noprefix, conf_log_source,
	   OBJ(int,3), P_LOG_FACILITY,
	   "facility", "Facility parameter for syslog messages", PRIV_CONF);

/******************************************************************
 * logging on
 * no logging on
 */
EOLS	(conf_log_on_eol, logger_command, P_LOG_ON);
KEYWORD_ID(conf_log_on, conf_log_on_eol, conf_log_facility,
	   OBJ(int,3), P_LOG_ON,
	   "on", "Enable logging to all supported destinations", PRIV_CONF);

/******************************************************************
 * [no] logging { console | monitor | trap | buffered [size] }
 *		  [ emergencies | alerts | critical | errors | warnings |
 *		    notifications | informational | debugging ]
 *
 * OBJ(int,4) = Optional size of P_LOG_BUFFERED buffer
 * OBJ(int,3) = P_LOG_CONSOLE, P_LOG_MONITOR, P_LOG_TRAP, P_LOG_BUFFERED
 * OBJ(int,2) = LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING,
 *		LOG_NOTICE, LOG_INFO, LOG_DEBUG, LOG_DEFAULT
 */
EOLS	(conf_log_level_eol, logger_command, P_LOG_LEVEL);

/*
 * [ emergencies | alerts | critical | errors | warnings |
 *   notifications | informational | debugging ]
 */
KEYWORD_ID (conf_log_debug, conf_log_level_eol, conf_log_level_eol,
	    OBJ(int,2), LOG_DEBUG,
	    "debugging", "Debugging messages", PRIV_CONF);

KEYWORD_ID (conf_log_info, conf_log_level_eol, conf_log_debug,
	    OBJ(int,2), LOG_INFO,
	    "informational", "Informational messages", PRIV_CONF);

KEYWORD_ID (conf_log_notify, conf_log_level_eol, conf_log_info, 
	    OBJ(int,2), LOG_NOTICE,
	    "notifications", "Normal but significant conditions", PRIV_CONF);

KEYWORD_ID (conf_log_warn, conf_log_level_eol, conf_log_notify,
	    OBJ(int,2), LOG_WARNING,
	    "warnings", "Warning conditions", PRIV_CONF);

KEYWORD_ID (conf_log_error, conf_log_level_eol, conf_log_warn,
	    OBJ(int,2), LOG_ERR,
	    "errors", "Error conditions", PRIV_CONF);

KEYWORD_ID (conf_log_critical, conf_log_level_eol, conf_log_error,
	    OBJ(int,2), LOG_CRIT,
	    "critical", "Critical conditions", PRIV_CONF);

KEYWORD_ID (conf_log_alert, conf_log_level_eol, conf_log_critical,
	    OBJ(int,2), LOG_ALERT,
	    "alerts", "Immediate action needed", PRIV_CONF);

KEYWORD_ID (conf_log_emerg, conf_log_level_eol, conf_log_alert,
	    OBJ(int,2), LOG_EMERG,
	    "emergencies", "System is unusable", PRIV_CONF);

SET (conf_log_set_default, conf_log_emerg, OBJ(int,2), LOG_DEFAULT);

NOPREFIX(conf_log_level_noprefix, conf_log_set_default, conf_log_level_eol);
NVGENS	(conf_log_level_nvgen, conf_log_level_noprefix,
	 logger_command, P_LOG_LEVEL);

/*
 * { console | monitor | trap }
 */
KEYWORD_ID (conf_log_trap, conf_log_level_nvgen, conf_log_on,
	    OBJ(int,3), P_LOG_TRAP,
	    "trap", "Set syslog server logging level", PRIV_CONF);

KEYWORD_ID (conf_log_monitor, conf_log_level_nvgen, conf_log_trap,
	    OBJ(int,3), P_LOG_MONITOR,
	    "monitor", "Set terminal line (monitor) logging level", PRIV_CONF);

KEYWORD_ID (conf_log_console, conf_log_level_nvgen, conf_log_monitor,
	    OBJ(int,3), P_LOG_CONSOLE,
	    "console", "Set console logging level", PRIV_CONF);

/*
 * { buffered [ size ] }
 */
NUMBER (conf_log_size, conf_log_level_nvgen, conf_log_level_nvgen,
 	OBJ(int,4), 4096, MAXINT, "Logging buffer size");

NOPREFIX (conf_log_buf_noprefix,conf_log_size,conf_log_level_eol);

NVGENS (conf_log_buf_nvgen, conf_log_buf_noprefix,
	 logger_command, P_LOG_LEVEL);

KEYWORD_ID(conf_log_buf, conf_log_buf_nvgen, conf_log_console,
	   OBJ(int,3), P_LOG_BUFFERED,
	   "buffered", "Set buffered logging parameters",
	   PRIV_CONF);

KEYWORD	(conf_logging, conf_log_buf, ALTERNATE,
	 "logging", "Modify message logging facilities", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_logging
