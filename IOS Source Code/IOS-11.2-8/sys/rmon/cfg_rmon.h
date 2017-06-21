/* $Id: cfg_rmon.h,v 3.4 1996/02/18 17:57:48 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/cfg_rmon.h,v $
 *------------------------------------------------------------------
 * cfg_rmon.h:  Remote Monitoring Global Configuration Commands
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rmon.h,v $
 * Revision 3.4  1996/02/18  17:57:48  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.3  1996/01/02  23:27:41  jjohnson
 * CSCdi46285:  Need to clean up the RMON user interface
 *
 * Revision 3.2  1995/11/17  18:39:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * rmon priority {low | normal | high}
 * <hidden command>
 */
EOLS    (conf_rmon_pri_eol, rmon_global_command, RMON_PRIORITY);

KEYWORD_ID (conf_rmon_pri_high, conf_rmon_pri_eol, NONE,
            OBJ(int,1), PRIO_HIGH,
            "high", "High priority process", PRIV_CONF);
KEYWORD_ID (conf_rmon_pri_normal, conf_rmon_pri_eol, conf_rmon_pri_high,
            OBJ(int,1), PRIO_NORMAL,
            "normal", "Normal priority process", PRIV_CONF);
KEYWORD_ID (conf_rmon_pri_low, conf_rmon_pri_eol, conf_rmon_pri_normal,
            OBJ(int,1), PRIO_LOW,
            "low", "Low priority process", PRIV_CONF);

NVGENS  (conf_rmon_pri_nvgen, conf_rmon_pri_low, rmon_global_command,
	 RMON_PRIORITY);
KEYWORD (conf_rmon_pri, conf_rmon_pri_nvgen, ALTERNATE,
         "priority", "Set the priority of the RMON process",
	 PRIV_CONF | PRIV_HIDDEN);

/*
 * rmon queuesize <length>
 */
PARAMS_KEYONLY (conf_rmon_queue, conf_rmon_pri,
                "queuesize", OBJ(int,1),
		RMON_QUEUE_SIZE_MIN, RMON_QUEUE_SIZE_MAX,
                rmon_global_command, RMON_QUEUESIZE,
                "Size of the RMON packet queue",
		"Queue size",
                PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE conf_rmon_queue
