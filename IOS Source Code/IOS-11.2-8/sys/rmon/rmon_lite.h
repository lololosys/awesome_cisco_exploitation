/* $Id: rmon_lite.h,v 3.3 1996/02/18 17:57:50 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_lite.h,v $
 *------------------------------------------------------------------
 * rmon_lite.h: Prototypes for event & alarm support routines
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_lite.h,v $
 * Revision 3.3  1996/02/18  17:57:50  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.2  1996/02/09  07:57:19  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.1  1996/02/09  06:12:07  jjohnson
 * Initial placeholder for rmon user interface enhancements
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RMON_LITE_H__
#define __RMON_LITE_H__

extern void rmon_lite_parser_init (void);
extern void rmon_lite_show_command (parseinfo *csb);

extern void show_rmon_events (void);
extern void nvgen_rmon_events (void);
extern void rmon_config_event (parseinfo *csb);
extern void delete_rmon_event (long eventIndex);
extern void update_rmon_event (long eventIndex,
			       long eventType,
			       char *eventCommunity,
			       char *eventDescription,
			       char *eventOwner);


extern void show_rmon_alarms (void);
extern void nvgen_rmon_alarms (void);
extern void rmon_config_alarm (parseinfo *csb);
extern void delete_rmon_alarm (long alarmIndex);
extern void update_rmon_alarm (long alarmIndex,
			       long alarmInterval,
			       long alarmSampleType,
			       long alarmRisingThreshold,
			       long alarmFallingThreshold,
			       long alarmRisingEventIndex,
			       long alarmFallingEventIndex,
			       char *alarmVariable,
			       char *alarmOwner);

#endif
