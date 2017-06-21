/* $Id: cfg_rmon_alarm.h,v 3.2.20.1 1996/08/12 16:06:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/rmon/cfg_rmon_alarm.h,v $
 *------------------------------------------------------------------
 * cfg_rmon_alarm.h:  Parse chain for configuring an RMON alarm
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rmon_alarm.h,v $
 * Revision 3.2.20.1  1996/08/12  16:06:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/18  17:57:48  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.1  1996/02/17  17:10:51  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * rmon alarm <num> <variable> <interval> { delta | absolute }
 *                  rising-threshold <value> [<num>]
 *                  falling-threshold <value> [<num>]
 *                  [owner <string>]
 */
EOLNS    (cfg_rmon_alarm_eol, rmon_config_alarm);

STRING  (cfg_rmon_alarm_owner_string, cfg_rmon_alarm_eol, no_alt,
         OBJ(string,2), "Alarm owner");
KEYWORD_OR (cfg_rmon_alarm_owner, cfg_rmon_alarm_owner_string,
	    cfg_rmon_alarm_eol,
	    OBJ(int,8), RMON_KEY_OWNER,
	    "owner", "Specify an owner for the alarm", PRIV_CONF);

NUMBER  (cfg_rmon_alarm_falling_num, cfg_rmon_alarm_owner,
	 cfg_rmon_alarm_owner,
         OBJ(int,7), 1, 65535, "Event to fire on falling threshold crossing");
SIGNED_DEC(cfg_rmon_alarm_falling_value, cfg_rmon_alarm_falling_num, no_alt,
	   OBJ(int,6), MININT, MAXINT, "falling threshold value");
KEYWORD (cfg_rmon_alarm_falling, cfg_rmon_alarm_falling_value, no_alt,
         "falling-threshold", "Configure the falling threshold", PRIV_CONF);

NUMBER  (cfg_rmon_alarm_rising_num, cfg_rmon_alarm_falling,
	 cfg_rmon_alarm_falling,
         OBJ(int,5), 1, 65535, "Event to fire on rising threshold crossing");
SIGNED_DEC(cfg_rmon_alarm_rising_value, cfg_rmon_alarm_rising_num, no_alt,
	   OBJ(int,4), MININT, MAXINT, "rising threshold value");
KEYWORD (cfg_rmon_alarm_rising, cfg_rmon_alarm_rising_value, no_alt,
         "rising-threshold", "Configure the rising threshold", PRIV_CONF);

KEYWORD_ID(cfg_rmon_alarm_abs, cfg_rmon_alarm_rising, no_alt,
           OBJ(int,3), D_alarmSampleType_absoluteValue,
           "absolute", "Test each sample directly", PRIV_CONF);
KEYWORD_ID(cfg_rmon_alarm_delta, cfg_rmon_alarm_rising, cfg_rmon_alarm_abs,
           OBJ(int,3), D_alarmSampleType_deltaValue,
           "delta", "Test delta between samples", PRIV_CONF);

NUMBER  (cfg_rmon_alarm_interval, cfg_rmon_alarm_delta, no_alt,
         OBJ(int,2), 1, MAXUINT, "Sample interval");
STRING  (cfg_rmon_alarm_variable, cfg_rmon_alarm_interval, no_alt,
         OBJ(string,1), "MIB object to monitor");
NOPREFIX(cfg_rmon_alarm_noprefix, cfg_rmon_alarm_variable, cfg_rmon_alarm_eol);
NUMBER  (cfg_rmon_alarm_num, cfg_rmon_alarm_noprefix, no_alt,
         OBJ(int,1), 1, 65535, "alarm number");

NVGENNS  (cfg_rmon_alarm_nvgen, cfg_rmon_alarm_num, rmon_config_alarm);
KEYWORD (cfg_rmon_alarm, cfg_rmon_alarm_nvgen, ALTERNATE,
         "alarm", "Configure an RMON alarm", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE cfg_rmon_alarm
