/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * Disable/Enable Interface logging configuration
 *
 * March 1997, Dana Blair
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */



#define LOG_INTF_DIAL 0
#define LOG_INTF_DISABLE 0


EOLNS	(conf_intf_log_eol, logging_interface_command);
NVGENNS   (conf_intf_log_nvgen, conf_intf_log_eol,
	   logging_interface_command);

KEYWORD_ID (conf_intf_log_status, conf_intf_log_nvgen, no_alt,
	    OBJ(int,2), LOG_INTF_DIAL,
	    "link-status", "UPDOWN and CHANGE messages", PRIV_CONF);

KEYWORD_ID (conf_intf_log_events, conf_intf_log_status, no_alt,
	    OBJ(int,1), LOG_INTF_DISABLE,
	    "event", "Interface events", PRIV_CONF);

KEYWORD	(conf_intf_logging, conf_intf_log_events, ALTERNATE,
	 "logging", "Configure logging for interface", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_intf_logging
