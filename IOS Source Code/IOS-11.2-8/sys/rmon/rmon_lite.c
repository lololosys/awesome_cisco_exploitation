/* $Id: rmon_lite.c,v 3.3 1996/02/18 17:57:50 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_lite.c,v $
 *------------------------------------------------------------------
 * rmon_lite.c:  Remote Monitoring MIB Support
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_lite.c,v $
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

#include "master.h"
#include "config.h"
#include "rmon_lite.h"
#include "parser_defs_rmon.h"
#include "rmon_registry.h"
#include "sr_rmonmibdefs.h"

void
rmon_lite_show_command (parseinfo *csb)
{
    uint	flagword;

    automore_enable(NULL);
    flagword = GETOBJ(int, 1);
    if (flagword & (1<<RMON_SHOW_EVENTS))
	show_rmon_events();
    if (flagword & (1<<RMON_SHOW_ALARMS))
	show_rmon_alarms();
    reg_invoke_rmon_show_command(flagword);
    automore_disable();
}

void
rmon_config_event (parseinfo *csb)
{
    char       *eventCommunity;
    char       *eventDescription;
    char       *eventOwner;
    long	eventIndex;
    long	eventType;

    if (csb->nvgen) {
	nvgen_rmon_events();
    } else {
	eventIndex = GETOBJ(int, 1);
	if (csb->sense) {
	    /*
	     * calculate the event type
	     */
	    switch (GETOBJ(int,2) & (RMON_KEY_TRAP | RMON_KEY_LOG)) {
	    case 0:
		eventType = D_eventType_none;
		break;
	    case RMON_KEY_LOG:
		eventType = D_eventType_log;
		break;
	    case RMON_KEY_TRAP:
		eventType = D_eventType_snmp_trap;
		break;
	    default:
		eventType = D_eventType_log_and_trap;
		break;
	    }

	    /*
	     * pull out the other parameters
	     */
	    eventCommunity = GETOBJ(string,1);
	    if (GETOBJ(int,2) & RMON_KEY_DESCRIPTION)
		eventDescription = GETOBJ(string,2);
	    else
		eventDescription = NULL;
	    if (GETOBJ(int,2) & RMON_KEY_OWNER)
		eventOwner = GETOBJ(string,3);
	    else
		eventOwner = NULL;
	    update_rmon_event(eventIndex, eventType, eventCommunity,
			      eventDescription, eventOwner);
	} else {
	    delete_rmon_event(eventIndex);
	}
    }
}

void
rmon_config_alarm (parseinfo *csb)
{
    long	alarmIndex;
    long        alarmInterval;
    long        alarmSampleType;
    long        alarmRisingThreshold;
    long        alarmFallingThreshold;
    long        alarmRisingEventIndex;
    long        alarmFallingEventIndex;
    char       *alarmVariable;
    char       *alarmOwner;

    if (csb->nvgen) {
	nvgen_rmon_alarms();
    } else {
        alarmIndex = GETOBJ(int, 1);
        if (csb->sense) {
	    alarmInterval = GETOBJ(int,2);
	    alarmSampleType = GETOBJ(int,3);
	    alarmRisingThreshold = GETOBJ(int,4);
	    alarmRisingEventIndex = GETOBJ(int,5);
	    alarmFallingThreshold = GETOBJ(int,6);
	    alarmFallingEventIndex = GETOBJ(int,7);
	    alarmVariable = GETOBJ(string,1);
	    alarmOwner = (GETOBJ(int,8) & RMON_KEY_OWNER) ?
		GETOBJ(string,2) : NULL;
	    update_rmon_alarm(alarmIndex,
			      alarmInterval,
			      alarmSampleType,
			      alarmRisingThreshold,
			      alarmFallingThreshold,
			      alarmRisingEventIndex,
			      alarmFallingEventIndex,
			      alarmVariable,
			      alarmOwner);
	} else {
            delete_rmon_alarm(alarmIndex);
	}
    }
}
