/* $Id: cfg_service.h,v 3.2.62.2 1996/08/12 16:05:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_service.h,v $
 *------------------------------------------------------------------
 * C F G _ S E R V I C E . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_service.h,v $
 * Revision 3.2.62.2  1996/08/12  16:05:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/10  04:52:33  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 * Make "service slave-core" hidden.
 *
 * Revision 3.2  1995/11/17  18:45:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/29  21:21:17  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers".
 *
 * Revision 2.2  1995/07/11  23:45:59  billw
 * CSCdi35800:  Need knob to supress user view of IP addresses
 *
 * Revision 2.1  1995/06/07  22:13:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] service { config | decimal-tty | finger | nagle | pad |
 *		slave-coredump | slave-logger | telnet-zeroidle | 
 * 		tcp-keepalives-in | tcp-keepalives-out | password-encryption | 
 * 		exec-wait | linenumber | tcp-small-servers | old-slip-prompts}
 *
 * service timestamps [{debug | log}] [{uptime | datetime [msec] [localtime] 
 *                                       [show-timezone]}]
 * no service timestamps [{debug | log}]
 *  OBJ(int,1) = service (debug/log)
 *  OBJ(int,2) = type (uptime/datetime)
 *  OBJ(int,3) = msec flag
 *  OBJ(int,4) = localtime flag
 *  OBJ(int,5) = show-timezone flag
 */

LINK_TRANS(cfg_service_extend_here, no_alt);

EOLS	(conf_service_vtylog_eol, service_command, SERVICE_VTYLOG);
KEYWORD (conf_service_vtylog, conf_service_vtylog_eol,
	 cfg_service_extend_here, "pt-vty-logging",
	 "Log significant VTY-Async events", PRIV_CONF);

EOLS	(conf_service_hideaddr_eol, service_command, SERVICE_HIDEADDR);
KEYWORD (conf_service_hideaddr, conf_service_hideaddr_eol,
	 conf_service_vtylog, "hide-telnet-addresses",
	 "Hide destination addresses in telnet command", PRIV_CONF);

EOLS	(conf_service_oldslip_eol, service_command, SERVICE_OLDSLIP);
KEYWORD (conf_service_oldslip, conf_service_oldslip_eol,
	 conf_service_hideaddr, "old-slip-prompts",
	 "Allow old scripts to operate with slip/ppp", PRIV_CONF);

EOLS	(conf_service_tcpservers_eol, service_command, SERVICE_TCPSERVERS);
KEYWORD (conf_service_tcpservers, conf_service_tcpservers_eol, conf_service_oldslip,
	 "tcp-small-servers",
	 "Enable small TCP servers (e.g., ECHO)", PRIV_CONF);

EOLS	(conf_service_udpservers_eol, service_command, SERVICE_UDPSERVERS);
KEYWORD (conf_service_udpservers, conf_service_udpservers_eol, conf_service_tcpservers,
	 "udp-small-servers",
	 "Enable small UDP servers (e.g., ECHO)", PRIV_CONF);

EOLS	(conf_service_prompt_config_eol, service_command, SERVICE_PROMPT);
KEYWORD	(conf_service_prompt_config, conf_service_prompt_config_eol, no_alt,
	 "config", "Enable configuration mode prompt", PRIV_CONF);
KEYWORD	(conf_service_prompt, conf_service_prompt_config,
	 conf_service_udpservers,
	 "prompt", "Enable mode specific prompt", PRIV_CONF);

EOLS    (conf_service_compresscfg_eol, service_command, SERVICE_COMPRESS_CFG);
KEYWORD (conf_service_compresscfg, conf_service_compresscfg_eol, conf_service_prompt,
	 "compress-config", "Compress the configuration file", PRIV_CONF);

EOLS	(conf_service_internal_eol, service_command, SERVICE_INTERNAL);
KEYWORD (conf_service_internal, conf_service_internal_eol, conf_service_compresscfg,
	 "internal", "", PRIV_CONF|PRIV_HIDDEN);

EOLS	(conf_service_linenumber_eol, service_command, SERVICE_LINE_NUMBER);
KEYWORD	(conf_service_linenumber, conf_service_linenumber_eol,
	 conf_service_internal,
	 "linenumber",
	 "enable line number banner for each exec", PRIV_CONF);

EOLS	(conf_service_execwait_eol, service_command, SERVICE_EXECWAIT);
KEYWORD	(conf_service_execwait, conf_service_execwait_eol, conf_service_linenumber,
	 "exec-wait", "Delay EXEC startup on noisy lines", PRIV_CONF);

EOLS	(conf_service_pwdencrypt_eol, service_command, SERVICE_PWD_ENCRYPT);
KEYWORD	(conf_service_pwdencrypt, conf_service_pwdencrypt_eol, conf_service_execwait,
	 "password-encryption",
	 "Encrypt system passwords", PRIV_CONF);

pdecl(svc_timestamps_options);
EOLNS	(svc_timestamps_eol, service_timestamps_cmd);
KEYWORD_ID(svc_timestamps_showtz, svc_timestamps_options, svc_timestamps_eol,
	   OBJ(int,5), SERVICE_TIMESTAMP_SHOWTZ, "show-timezone",
	   "Add time zone information to timestamp", PRIV_CONF);
IFELSE (svc_timestamps_options3, svc_timestamps_eol,
        svc_timestamps_showtz, (GETOBJ(int,5)==SERVICE_TIMESTAMP_SHOWTZ));
KEYWORD_ID(svc_timestamps_localtime, svc_timestamps_options, 
	   svc_timestamps_options3, OBJ(int,4), SERVICE_TIMESTAMP_LOCAL,
	   "localtime", "Use local time zone for timestamps", PRIV_CONF);
IFELSE (svc_timestamps_options2, svc_timestamps_options3, 
        svc_timestamps_localtime, (GETOBJ(int,4)==SERVICE_TIMESTAMP_LOCAL));
KEYWORD_ID(svc_timestamps_msec, svc_timestamps_options, 
	   svc_timestamps_options2, OBJ(int,3), SERVICE_TIMESTAMP_MSEC, 
	   "msec", "Include milliseconds in timestamp", PRIV_CONF);
IFELSE (svc_timestamps_options, svc_timestamps_options2,
	svc_timestamps_msec, (GETOBJ(int,3)==SERVICE_TIMESTAMP_MSEC));
KEYWORD_ID(svc_timestamps_datetime, svc_timestamps_options,
           svc_timestamps_eol, OBJ(int,2), SERVICE_TIMESTAMP_DATETIME, 
           "datetime", "Timestamp with date and time", PRIV_CONF);
KEYWORD (svc_timestamps_uptime, svc_timestamps_eol, svc_timestamps_datetime,
	   "uptime", "Timestamp with system uptime", PRIV_CONF);
NVGENNS	(svc_timestamps_nv, svc_timestamps_uptime, service_timestamps_cmd);
NOPREFIX(svc_timestamps_no, svc_timestamps_nv, svc_timestamps_eol);
SET     (svc_timestamps_noparm, svc_timestamps_eol, OBJ(int,1),
	 NUM_SERVICE_TIMESTAMPS);
SPLIT   (svc_timestamps_nvcheck, svc_timestamps_eol, svc_timestamps_noparm,
	 nvgen, FALSE);
SPLIT   (svc_timestamps_no2, svc_timestamps_noparm, svc_timestamps_nvcheck,
	 sense, FALSE);
KEYWORD_ID(svc_timestamps_log, svc_timestamps_no, svc_timestamps_no2,
	   OBJ(int,1), SERVICE_TIMESTAMPS_LOG, "log", 
	   "Timestamp log messages", PRIV_CONF);
KEYWORD_ID(svc_timestamps_debug, svc_timestamps_no, svc_timestamps_log,
	   OBJ(int,1), SERVICE_TIMESTAMPS_DEBUG, "debug",
	   "Timestamp debug messages", PRIV_CONF);
KEYWORD	(conf_service_timestamps, svc_timestamps_debug, 
	 conf_service_pwdencrypt, "timestamps", 
	 "Timestamp debug/log messages",  PRIV_CONF);

EOLS	(conf_service_tcpkout_eol, service_command, SERVICE_TCPKOUT);
KEYWORD	(conf_service_tcpkout, conf_service_tcpkout_eol, conf_service_timestamps,
	 "tcp-keepalives-out",
	 "Generate keepalives on idle outgoing network connections", PRIV_CONF);

EOLS	(conf_service_tcpkin_eol, service_command, SERVICE_TCPKIN);
KEYWORD	(conf_service_tcpkin, conf_service_tcpkin_eol, conf_service_tcpkout,
	 "tcp-keepalives-in",
	 "Generate keepalives on idle incoming network connections", PRIV_CONF);

EOLS	(conf_service_telnetzero_eol, service_command, SERVICE_TELNETZERO);
KEYWORD	(conf_service_telnetzero, conf_service_telnetzero_eol, conf_service_tcpkin,
	 "telnet-zeroidle", "Set TCP window 0 when connection is idle",
	 PRIV_CONF);

EOLS	(conf_service_pad_eol, service_command, SERVICE_PAD);
KEYWORD	(conf_service_pad, conf_service_pad_eol, conf_service_telnetzero,
	 "pad", "Enable PAD commands", PRIV_CONF);

EOLS    (conf_service_slavecore_eol, service_command, SERVICE_SLAVECORE);
KEYWORD (conf_service_slavecore, conf_service_slavecore_eol, conf_service_pad,
         "slave-coredump", NULL /*"Enable coredump capability of slave IPs"*/,
        PRIV_CONF|PRIV_HIDDEN);
 
EOLS    (conf_service_slavelog_eol, service_command, SERVICE_SLAVELOG);
KEYWORD (conf_service_slavelog, conf_service_slavelog_eol,
         conf_service_slavecore,
         "slave-log", "Enable log capability of slave IPs",
        PRIV_CONF);
 
EOLS    (conf_service_nagle_eol, service_command, SERVICE_NAGLE);
KEYWORD (conf_service_nagle, conf_service_nagle_eol, conf_service_slavelog,
         "nagle", "Enable Nagle's congestion control algorithm", PRIV_CONF);

EOLS	(conf_service_finger_eol, service_command, SERVICE_FINGER);
KEYWORD	(conf_service_finger, conf_service_finger_eol, conf_service_nagle,
	 "finger", "Allow responses to finger requests", PRIV_CONF);

EOLS	(conf_service_decimal_eol, service_command, SERVICE_DECIMAL);
KEYWORD	(conf_service_decimal, conf_service_decimal_eol, conf_service_finger,
	 "decimal-tty", "Interpret tty line numbers in decimal.", PRIV_CONF);

EOLS	(conf_service_callback_eol, service_command, SERVICE_CALLBACK);
KEYWORD	(conf_service_callback, conf_service_callback_eol, conf_service_decimal,
	 "exec-callback", "Enable exec callback", PRIV_CONF);

EOLS	(conf_service_config_eol, service_command, SERVICE_CONFIG);
KEYWORD	(conf_service_config, conf_service_config_eol, conf_service_callback,
	 "config", "TFTP load config files", PRIV_CONF);

EOLS	(conf_service_log_backtrace_eol, service_command, SERVICE_LOG_BACKTRACE);
KEYWORD	(conf_service_log_backtrace, conf_service_log_backtrace_eol, no_alt,
	 "backtrace", NULL /*"Print backtrace in error messages"*/,
	 PRIV_CONF|PRIV_HIDDEN);
KEYWORD	(conf_service_log, conf_service_log_backtrace, conf_service_config,
	 "log", NULL /*"Print extra information in error messages"*/,
	 PRIV_CONF|PRIV_HIDDEN);

KEYWORD	(conf_service, conf_service_log, ALTERNATE,
	 "service", "Modify use of network based services", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_service

