/* $Id: cfg_rmon_event.h,v 3.2.20.1 1996/08/12 16:06:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rmon/cfg_rmon_event.h,v $
 *------------------------------------------------------------------
 * cfg_rmon_event.h:  Parse chain for configuring an RMON event
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rmon_event.h,v $
 * Revision 3.2.20.1  1996/08/12  16:06:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/18  17:57:49  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.1  1996/02/17  17:10:50  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * rmon event <num> [log] [trap <community>]
 *                  [description <string>]
 *                  [owner <string>]
 */
pdecl(cfg_rmon_event_opts);

EOLNS    (cfg_rmon_event_eol, rmon_config_event);
STRING  (cfg_rmon_event_owner_string, cfg_rmon_event_opts, no_alt,
         OBJ(string,3), "Event owner");
KEYWORD_ORTEST(cfg_rmon_event_owner, cfg_rmon_event_owner_string, NONE,
               OBJ(int,2), RMON_KEY_OWNER,
               "owner", "Specify an owner for the event", PRIV_CONF);
STRING  (cfg_rmon_event_dsc_string, cfg_rmon_event_opts, no_alt,
         OBJ(string,2), "Event description");
KEYWORD_ORTEST(cfg_rmon_event_dsc, cfg_rmon_event_dsc_string,
	       cfg_rmon_event_owner,
               OBJ(int,2), RMON_KEY_DESCRIPTION,
               "description", "Specify a description of the event", PRIV_CONF);
STRING  (cfg_rmon_event_trap_string, cfg_rmon_event_opts, no_alt,
         OBJ(string,1), "SNMP community string");
KEYWORD_ORTEST(cfg_rmon_event_trap, cfg_rmon_event_trap_string,
	       cfg_rmon_event_dsc,
               OBJ(int,2), RMON_KEY_TRAP,
               "trap", "Generate SNMP trap when the event fires", PRIV_CONF);
KEYWORD_ORTEST(cfg_rmon_event_log, cfg_rmon_event_opts, cfg_rmon_event_trap,
               OBJ(int,2), RMON_KEY_LOG,
               "log", "Generate RMON log when the event fires", PRIV_CONF);
NOP     (cfg_rmon_event_opts, cfg_rmon_event_log, cfg_rmon_event_eol);
NOPREFIX(cfg_rmon_event_noprefix, cfg_rmon_event_opts, cfg_rmon_event_eol);
NUMBER  (cfg_rmon_event_num, cfg_rmon_event_noprefix, no_alt,
         OBJ(int,1), 1, 65535, "event number");
NVGENNS  (cfg_rmon_event_nvgen, cfg_rmon_event_num, rmon_config_event);
KEYWORD (cfg_rmon_event, cfg_rmon_event_nvgen, ALTERNATE,
         "event", "Configure an RMON event", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE cfg_rmon_event
