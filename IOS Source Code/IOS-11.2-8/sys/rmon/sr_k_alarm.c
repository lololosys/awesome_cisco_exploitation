/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_alarm.c,v 3.10.12.1 1996/07/01 23:48:10 snyder Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_alarm.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_alarm.c,v $
 * Revision 3.10.12.1  1996/07/01  23:48:10  snyder
 * CSCdi61839:  remove plural support from rmon
 * Branch: California_branch
 *              changes computed pluralizing of seconds to fixed second(s)
 *              48 bytes saved
 *
 * Revision 3.10  1996/02/19  06:05:56  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.9  1996/02/19  02:24:36  jjohnson
 * CSCdi49291:  Memory leak in rmon
 * Callers of function i_GetMib() must free the returned VarBind
 *
 * Revision 3.8  1996/02/18  17:57:51  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.7  1996/02/15  22:35:17  jjohnson
 * CSCdi49107: rmon incorrectly issues an initial event for deltaValue
 * alarms.  another patch that i supplied to the vendor.  for deltaValue
 * alarms, don't take the first sample until half the interval has
 * expired, and don't allow an initial alarm until two samples have been
 * taken (i.e. one interval has elapsed)
 *
 * Revision 3.6  1996/02/15  21:16:37  jjohnson
 * CSCdi49102:  rmon samples too frequently for certain alarmInterval
 * values.  scale then divide to avoid off-by-one error.
 *
 * Revision 3.5  1996/02/15  19:28:42  jjohnson
 * CSCdi48677:  bad alarmValue when alarmSampleType is deltaValue
 * vendor-supplied patch didn't work, so i supplied them with a patch.
 * if you want something done right...
 *
 * Revision 3.4  1996/02/15  16:48:12  jjohnson
 * CSCdi48677:  bad alarmValue when alarmSampleType is deltaValue
 * Apply patch 02.12.1277 supplied by the vendor
 *
 * Revision 3.3  1996/02/09  07:57:23  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */


#include "master.h"
#include "sr_rmonmib.h"
#include "rmon_util.h"
#include "logger.h"
#include "../os/free.h"

VarBind        *i_GetMib SR_PROTOTYPE((OID * object));

/* flags for alarm variables */
#define	AF_RISINGOK		0x0001
#define	AF_FALLINGOK		0x0002
#define	AF_RISINGALARM		0x0004
#define	AF_FALLINGALARM		0x0008
#define	AF_SENDRISING		0x0010
#define	AF_SENDFALLING		0x0020
#define	AF_FIRSTPOLL		0x0040
#define AF_PREPOLL		0x0080

/* statistics regarding sending alarms */
unsigned long   alarmFails = 0;

/* strings for use w/ MakeOIDFromDot */
static char     rmonString[] = "1.3.6.1.2.1.16";
static char     alarmIndexString[] = "1.3.6.1.2.1.16.3.1.1.1";
static char     alarmVariableString[] = "1.3.6.1.2.1.16.3.1.1.3";
static char     alarmSampleTypeString[] = "1.3.6.1.2.1.16.3.1.1.4";
static char     alarmValueString[] = "1.3.6.1.2.1.16.3.1.1.5";
static char     alarmRisingThresholdString[] = "1.3.6.1.2.1.16.3.1.1.7";
static char     alarmFallingThresholdString[] = "1.3.6.1.2.1.16.3.1.1.8";

/* global data describing the alarmTable entries */
static SnmpType        alarmEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* alarmIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmInterval */
    {OBJECT_ID_TYPE, SR_READ_WRITE, -1},	/* alarmVariable */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmSampleType */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* alarmValue */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmStartupAlarm */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmRisingThreshold */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmFallingThreshold */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmRisingEventIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmFallingEventIndex */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* alarmOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* alarmStatus */
};

static Index_t         alarmEntryIndex[1];
static RmonTable       alarmTable;

void DeletealarmTableEntry SR_PROTOTYPE((int index));

/* This is the definition of rmon traps for alarms, as given in the defunct
 * internet draft draft-ietf-rmon-trap-00.  This is referred to as a
 * "companion document" by rfc1271.
 *
 *    risingAlarm TRAP-TYPE
 *	  ENTERPRISE { rmon }
 *	  VARIABLES { alarmIndex, alarmVariable, alarmSampleType,
 *		      alarmValue, alarmRisingThreshold }
 *	  DESCRIPTION
 *	      "The SNMP trap that is generated when an alarm
 *	      entry crosses its rising threshold and generates
 *	      an event that is configured for sending SNMP
 *	      traps."
 *	  ::= 1
 *
 *    fallingAlarm TRAP-TYPE
 *	  ENTERPRISE { rmon }
 *	  VARIABLES { alarmIndex, alarmVariable, alarmSampleType,
 *		      alarmValue, alarmFallingThreshold }
 *	  DESCRIPTION
 *	      "The SNMP trap that is generated when an alarm
 *	      entry crosses its falling threshold and generates
 *	      an event that is configured for sending SNMP
 *	      traps."
 *	  ::= 2
 *
 * This routine sends rising or falling alarms, as indicated by the type
 * parameter.
 */
void
SendRisingOrFallingAlarm(evp, type, alarmp)
    eventEntry_t   *evp;
    unsigned long   type;
    alarmEntry_t   *alarmp;
{
    VarBind        *vb[5];
    OID            *name, *variable;
    char            oidName[64];

    /* clear the varbind list */
    memset(vb, 0, sizeof(vb));

    /* alarmSampleType */
    sprintf(oidName, "%s.%ld", alarmIndexString, alarmp->alarmIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[0] = MakeVarBindWithValue(name, (OID *) NULL, INTEGER_TYPE,
				      (void *) &alarmp->alarmIndex)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* build the list, one varbind at a time */
    sprintf(oidName, "%s.%ld", alarmVariableString, alarmp->alarmIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((variable = CloneOID(alarmp->alarmVariable)) == 0) {
	goto fail;
    }
    if ((vb[1] = MakeVarBindWithValue(name, (OID *) NULL, OBJECT_ID_TYPE,
				      (void *) variable)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* alarmSampleType */
    sprintf(oidName, "%s.%ld", alarmSampleTypeString, alarmp->alarmIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[2] = MakeVarBindWithValue(name, (OID *) NULL, INTEGER_TYPE,
				 (void *) &alarmp->alarmSampleType)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* alarmValue */
    sprintf(oidName, "%s.%ld", alarmValueString, alarmp->alarmIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[3] = MakeVarBindWithValue(name, (OID *) NULL, INTEGER_TYPE,
				      (void *) &alarmp->alarmValue)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* threshold */
    sprintf(oidName, "%s.%ld", type == AF_SENDRISING ?
	    alarmRisingThresholdString : alarmFallingThresholdString,
	    alarmp->alarmIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[4] = MakeVarBindWithValue(name, (OID *) NULL, INTEGER_TYPE,
	    type == AF_SENDRISING ? (void *) &alarmp->alarmRisingThreshold :
			   (void *) &alarmp->alarmFallingThreshold)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* get the name of the trap to send */
    if ((name = MakeOIDFromDot(rmonString)) == 0) {
	goto fail;
    }

    /* link the varbinds together */
    vb[0]->next_var = vb[1];
    vb[1]->next_var = vb[2];
    vb[2]->next_var = vb[3];
    vb[3]->next_var = vb[4];
    vb[4]->next_var = 0;

    /* do the trap */
    RmonTrap(ENTERPRISE_TRAP, type == AF_SENDRISING ? (long) 1 : (long) 2,
	     vb[0], name, (char *) NULL, evp);

    /* free the enterprise OID */
    FreeOID(name);
    return;

fail:
    alarmFails++;
    FreeOID(name);
    FreeVarBind(vb[0]);
    FreeVarBind(vb[1]);
    FreeVarBind(vb[2]);
    FreeVarBind(vb[3]);
    FreeVarBind(vb[4]);
}

/* this routine takes deltas between the current alarm value and the rising
 * and falling thresholds, and determines whether or not to fire events. */
void
CheckAlarmValue(alarmp, risingDiff, fallingDiff)
    alarmEntry_t   *alarmp;
    long            risingDiff, fallingDiff;
{
    /* check rising events */
    if (risingDiff >= 0) {
	/* check whether this event is enabled */
	if ((alarmp->flags & AF_RISINGOK) != 0) {
	    /* send a rising event */
	    alarmp->flags |= AF_SENDRISING;
	}

	/* if the threshold was crossed, enable falling events */
	if (fallingDiff > 0) {
	    alarmp->flags |= AF_FALLINGOK;
	}
    }

    /* check falling events */
    if (fallingDiff <= 0) {
	/* check whether this event is enabled */
	if ((alarmp->flags & AF_FALLINGOK) != 0) {
	    /* send a falling event */
	    alarmp->flags |= AF_SENDFALLING;
	}

	/* if the threshold was crossed, enable rising events */
	if (risingDiff < 0) {
	    alarmp->flags |= AF_RISINGOK;
	}
    }
}

/* this routine is responsible for checking alarm variables periodically */
void
AlarmCallback(tdp)
    TimeOutDescriptor *tdp;
{
    alarmEntry_t   *alarmp;
    VarBind        *vb;
    union {
	long            slvalue;
	unsigned long   ulvalue;
    }               value, delta;

    /* get the alarm descriptor from the timeout descriptor */
    alarmp = (alarmEntry_t *) tdp->UserData2;
    if (alarmp->alarmStatus != D_alarmStatus_valid) {
	/* alarm has been cancelled */
	return;
    }

    /* try to get the variable */
    vb = i_GetMib(alarmp->alarmVariable);
    if (vb == 0) {

#ifdef	SR_FTC			/* fast timer-cancel */
	/* cancel this timer callback */
	CancelTimeout((int) tdp->TimerId);

	/* delete the alarm table entry */
	alarmp->alarmCallbackTimer = 0;
	alarmTableDeleteCallback(tdp);
#else				/* SR_FTC */
	/*
	 * couldn't get the variable -- set status to invalid.  This table
	 * entry will be reclaimed by the agent during the next pass through
	 * UpdateTables().
	 */
	alarmp->alarmStatus = D_alarmStatus_invalid;
#endif				/* SR_FTC */

	return;
    }

#ifdef DEBUG
    buginf(" \nt=%d, v=%d, lv=%d, ld=%d",
	   TimerCurTime(),
	   vb->value.ul_value,
	   alarmp->lastValue,
	   alarmp->lastDelta);
#endif

    /* get the values to use int the comparison */
    if (alarmp->alarmSampleType == D_alarmSampleType_absoluteValue) {
	value.ulvalue = vb->value.ul_value;
        (unsigned long) alarmp->alarmValue = value.ulvalue;
    }
    else {
	/*
	 * must be a delta value, so calculate the new value using the saved
	 * data in the alarm structure
	 */
	if (vb->value.type == INTEGER_TYPE) {
	    delta.slvalue = vb->value.sl_value - alarmp->lastValue;
	    value.slvalue = alarmp->lastDelta + delta.slvalue;
	    alarmp->alarmValue = value.slvalue;
	    alarmp->lastValue = vb->value.sl_value;
	    alarmp->lastDelta = delta.slvalue;
	}
	else {
	    delta.ulvalue = vb->value.ul_value -
		(unsigned long) alarmp->lastValue;
	    value.ulvalue = (unsigned long) alarmp->lastDelta + delta.ulvalue;
	    (unsigned long) alarmp->alarmValue = value.ulvalue;
	    (unsigned long) alarmp->lastValue = vb->value.ul_value;
	    (unsigned long) alarmp->lastDelta = delta.ulvalue;
	}
    }

#ifdef DEBUG
    buginf(", av=%d, lv=%d, ld=%d",
	   alarmp->alarmValue,
	   alarmp->lastValue,
	   alarmp->lastDelta);
#endif

    /* see if this was the pre-poll for a delta alarm */
    if ((alarmp->flags & AF_PREPOLL) != 0) {
	FreeVarBind(vb);
	alarmp->flags &= ~AF_PREPOLL;
	return;
    }

    /* check the alarm value */
    if (vb->value.type == INTEGER_TYPE) {
	/* determine whether to send flags */
	CheckAlarmValue(alarmp,
			value.slvalue - alarmp->alarmRisingThreshold,
			value.slvalue - alarmp->alarmFallingThreshold);
    }
    else {
	/* determine whether to send flags */
	CheckAlarmValue(alarmp,
	       value.ulvalue - (unsigned long) alarmp->alarmRisingThreshold,
	     value.ulvalue - (unsigned long) alarmp->alarmFallingThreshold);
    }


    /* check for rising events */
    if ((alarmp->flags & AF_SENDRISING) != 0) {
	if ((alarmp->flags & AF_FIRSTPOLL) == 0
	 || alarmp->alarmStartupAlarm != D_alarmStartupAlarm_fallingAlarm) {
	    /* send the alarm */
	    FireEvent(alarmp->alarmRisingEventIndex, SendRisingOrFallingAlarm,
		      (unsigned long) AF_SENDRISING, alarmp);
	}

	/* clear the flags */
	alarmp->flags &= ~(AF_SENDRISING | AF_RISINGOK);
    }

    /* check for falling events */
    if ((alarmp->flags & AF_SENDFALLING) != 0) {
	if ((alarmp->flags & AF_FIRSTPOLL) == 0
	  || alarmp->alarmStartupAlarm != D_alarmStartupAlarm_risingAlarm) {
	    /* send the alarm */
	    FireEvent(alarmp->alarmFallingEventIndex, SendRisingOrFallingAlarm,
		      (unsigned long) AF_SENDFALLING, alarmp);
	}

	/* clear the flags */
	alarmp->flags &= ~(AF_SENDFALLING | AF_FALLINGOK);
    }

    /* no longer the first check */
    if ((alarmp->flags & AF_FIRSTPOLL) != 0)
	alarmp->flags &= ~AF_FIRSTPOLL;

    FreeVarBind(vb);
}


#ifndef	SR_FTC			/* fast timer-cancel */
/* this routine goes through the alarm table and removes invalid entries */
void
CheckAlarmTable()
{
    alarmEntry_t   *alarmp;
    int             i;

    /* go through the entire table */
    for (i = 0; i < alarmTable.nitems; i++) {
	alarmp = (alarmEntry_t *) alarmTable.tp[i];
	if (alarmp->alarmStatus == D_alarmStatus_invalid) {
	    /* get rid of the entry */
	    DeletealarmTableEntry(i);
	}
    }
}

#endif				/* SR_FTC */

/* this routine sets a periodic timer to monitor alarm variables */
void           *
SetAlarmTimer(initial, interval, alarmp)
    long	    initial;
    long            interval;
    alarmEntry_t   *alarmp;
{
    TimeOutDescriptor *tdp;

    tdp = malloc_named(sizeof(TimeOutDescriptor), "RMON Alrm Timer");
    if (tdp) {
	/* fill in the descriptor */
	tdp->CallBack = AlarmCallback;
	tdp->UserData2 = (void *) alarmp;

	/* set the timer */
	if ((tdp->TimerId = SetPeriodicTimeout((unsigned long) initial,
					       (unsigned long) interval,
					       (void *) tdp)) == (-1)) {
	    /* free the descriptor */
	    free(tdp);
	    tdp = 0;
	}
    }

    return ((void *) tdp);
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_alarmEntry_initialize()
{
    alarmEntryIndex[0].offset = I_alarmIndex;
    alarmEntryIndex[0].type = T_uint;

    alarmTable.nindices = 1;
    alarmTable.tip = alarmEntryIndex;
    alarmTable.nitems = 0;
    alarmTable.rowsize = sizeof(alarmEntry_t);
    if ((alarmTable.tp = (void **) malloc(sizeof(alarmEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_alarmEntry_terminate()
{
    while (alarmTable.nitems > 0) {
	DeletealarmTableEntry(0);
    }

    free(alarmTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
alarmTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    alarmEntry_t   *data;

    /* dummy up an entry to delete */
    data = (alarmEntry_t *) tdp->UserData2;
    data->alarmStatus = D_alarmStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_alarmEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the alarmTable */
void
DeletealarmTableEntry(index)
    int             index;
{
    alarmEntry_t   *data;
    TimeOutDescriptor *tdp;

    /* get a pointer to the old entry */
    data = (alarmEntry_t *) alarmTable.tp[index];

    /* free the periodic timer associated with the entry */
    if (data->alarmCallbackTimer != 0) {
	tdp = (TimeOutDescriptor *) data->alarmCallbackTimer;
	if (tdp->TimerId != -1) {
	    /* cancel the timer */
	    CancelTimeout((int) tdp->TimerId);
	}
	free(tdp);
	data->alarmCallbackTimer = 0;
    }

    /* free the old entry and remove it from the table */
    alarmEntry_free(data);
    RemoveTableEntry(&alarmTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_alarmEntryFreeUserpartData(data)
    alarmEntry_t   *data;
{
    /* nothing to free by default */
}

alarmEntry_t   *
k_alarmEntry_get(serialNum, contextInfo, nominator, searchType, alarmIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            alarmIndex;
{
    int             index;

    alarmTable.tip[0].value.uint_val = alarmIndex;
    if ((index = SearchTable(&alarmTable, searchType)) == -1) {
	return NULL;
    }

    return (alarmEntry_t *) alarmTable.tp[index];

}

#ifdef SETS
int
k_alarmEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    alarmEntry_t   *undodata = (alarmEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    alarmEntry_t   *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure the interval is positive (how can you have a negative
     * interval?)
     */
    if (object->nominator == I_alarmInterval && value->sl_value <= 0) {
	return (INCONSISTENT_VALUE_ERROR);
    }

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->alarmStatus == D_alarmStatus_valid) {
	switch (object->nominator) {
	  case I_alarmOwner:
	  case I_alarmStatus:
	    /* these can be modified while the alarm is valid */
	    break;

	  default:
	    if (CmpValues(object->nominator, alarmEntryTypeTable,
			  (unsigned long *) undodata,
			  value->ul_value) != 0) {
		/*
		 * can't change the value for this variable once the alarm is
		 * valid
		 */
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return (error_status);
}

int
k_alarmEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    alarmEntry_t   *data = (alarmEntry_t *) dp->data;
    VarBind        *vb;

    if (data->alarmStatus == D_alarmStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->alarmStatus == D_alarmStatus_valid) {
	    vb = 0;		/* assume no varbind */
	    if (!VALID(I_alarmVariable, data->valid)
		|| (vb = i_GetMib(data->alarmVariable)) == 0
		|| (vb->value.type != INTEGER_TYPE

#ifdef	SR_SNMPv2
		    && vb->value.type != INTEGER_32_TYPE
		    && vb->value.type != COUNTER_32_TYPE
		    && vb->value.type != GAUGE_32_TYPE
		    && vb->value.type != U_INTEGER_32_TYPE
#endif				/* SR_SNMPv2 */

		    && vb->value.type != COUNTER_TYPE
		    && vb->value.type != GAUGE_TYPE
		    && vb->value.type != TIME_TICKS_TYPE)) {
		/* can't access the value, or it's a compound data type */
		dp->state = UNKNOWN;
	    }
	    FreeVarBind(vb);
	}
    }

    return NO_ERROR;
}

int
k_alarmEntry_set_defaults(dp)
    doList_t       *dp;
{

    alarmEntry_t   *data = (alarmEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    /*
     * we have to assign all kinds of defaults here.  The manager station
     * should use the entryStatus convention (or else be very specific) to
     * ensure that it gets what it wants.
     */
    data->alarmInterval = 10;	/* seconds, quite arbitrary */
    data->alarmSampleType = D_alarmSampleType_absoluteValue;	/* arbitrary */
    data->alarmStartupAlarm = D_alarmStartupAlarm_risingOrFallingAlarm;	/* arbitrary */
    data->alarmRisingThreshold = 0;	/* arbitrary */
    data->alarmFallingThreshold = 0;	/* arbitrary */
    data->alarmRisingEventIndex = 0;	/* arbitrary, no event generated */
    data->alarmFallingEventIndex = 0;	/* arbitrary, no event generated */

    /*
     * this is an invalid variable, so the manager station, must at least
     * give the variable oid
     */
    if ((data->alarmVariable = MakeOIDFromDot("0.0")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->alarmOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->alarmStatus = D_alarmStatus_createRequest;
    data->TimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_alarmEntry_set(data, contextInfo, function)
    alarmEntry_t   *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    alarmEntry_t   *new = NULL;
    VarBind        *vb;
    long	    initial;
    long            interval;

    /* find this entry in the table */
    alarmTable.tip[0].value.uint_val = data->alarmIndex;
    if ((index = SearchTable(&alarmTable, EXACT)) != -1) {
	new = (alarmEntry_t *) alarmTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->alarmStatus == D_alarmStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeletealarmTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	alarmTable.tip[0].value.uint_val = data->alarmIndex;
	if ((index = NewTableEntry(&alarmTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (alarmEntry_t *) alarmTable.tp[index];
	name_memory(new, "RMON Alarm");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, alarmTableDeleteCallback)) == -1) {
	    DeletealarmTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_alarmStatus, alarmEntryTypeTable,
		    (unsigned long *)new,
		    (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->alarmStatus == D_alarmStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;

	/* once again, make sure we can get the value */
	if ((vb = i_GetMib(new->alarmVariable)) == 0) {
	    DeletealarmTableEntry(index);

#if ((defined(SR_SNMPv1) && ! ( defined(SR_SNMPv2) )) || defined(WANT_ALL_ERRORS))
	    return (BAD_VALUE_ERROR);
#else				/* ((defined(SR_SNMPv1) && ! (
				 * defined(SR_SNMPv2) )) ||
				 * defined(WANT_ALL_ERRORS)) */
	    return (INCONSISTENT_VALUE_ERROR);
#endif				/* ((defined(SR_SNMPv1) && ! (

				 * defined(SR_SNMPv2) )) ||
				 * defined(WANT_ALL_ERRORS)) */
	}

	/* if a delta value, save the current value and halve the interval */
	initial = TimerCurTime();
	interval = new->alarmInterval * 100;
	if (new->alarmSampleType == D_alarmSampleType_deltaValue) {
	    (unsigned long) new->lastValue = vb->value.ul_value;
	    new->lastDelta = 0;
	    interval /= 2;
	    initial += interval;
	    new->flags |= AF_PREPOLL;
	}

	FreeVarBind(vb);

	/* set the timeout */
	if ((new->alarmCallbackTimer = SetAlarmTimer(initial, interval, new))
	    == 0) {
	    DeletealarmTableEntry(index);
	    return (GEN_ERROR);
	}

	/* set flags */
	new->flags |= (AF_RISINGOK | AF_FALLINGOK | AF_FIRSTPOLL);
    }
    else {
	if (new->alarmStatus == D_alarmStatus_createRequest) {
	    new->alarmStatus = D_alarmStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
alarmEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

/******************** cisco supplied routines follow ********************/

static const char defaultOwner[] = "config";

static const char *
startup_alarm_string (long value)
{
    switch (value) {
    case D_alarmStartupAlarm_risingAlarm:
	return "rising";
    case D_alarmStartupAlarm_fallingAlarm:
	return "falling";
    case D_alarmStartupAlarm_risingOrFallingAlarm:
	return "rising or falling";
    default:
	return "unknown";
    }
}


/*
 * show rmon alarms
 * output a string similiar to the following for every alarm:
 *    Alarm 12345 is under creation, owned by foo
 *      Monitors oid.oid.oid.oid.oid.oid every xxxx seconds
 *      Taking absolute samples, last value was xxxxxx
 *      Rising threshold is xxxxx, assigned to event xxxxxx
 *      Falling threshold is xxxxx, assigned to event xxxxx
 */

void
show_rmon_alarms (void)
{
    alarmEntry_t   *alarmp;
    int             i, status;
    OctetString    *ownerOS;
    char	   *owner;
    char           *object;

    automore_enable(NULL);
    if (alarmTable.nitems == 0) {
	printf("\nAlarm table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < alarmTable.nitems; i++) {
        alarmp = (alarmEntry_t *) alarmTable.tp[i];
	mem_lock(alarmp);

	/* turn the owner string from an octet string to a real string */
	owner = malloc(alarmp->alarmOwner->length + 1);
	if (owner) {
	    ownerOS = alarmp->alarmOwner;
	    memcpy(owner, ownerOS->octet_ptr, ownerOS->length);
	}

	/* get a textual representation of the object */
	object = malloc(MAX_OCTET_STRING_SIZE);
	if (object) {
	    status = MakeDotFromOID(alarmp->alarmVariable, object);
	    if (status == -1) {
		free(object);
		object = NULL;
	    }
	}

	printf("\nAlarm %d is %s, owned by %s",
	       alarmp->alarmIndex,
	       rmon_status_string(alarmp->alarmStatus),
	       owner ? owner : "unknown");
	printf("\n Monitors %s every %d second(s)",
	       object ? object : "unknown",
	       alarmp->alarmInterval);
	printf("\n Taking %s samples, last value was %d",
	       (alarmp->alarmSampleType == D_alarmSampleType_deltaValue) ?
	       "delta" : "absolute",
	       alarmp->alarmValue);
	printf("\n Rising threshold is %d, assigned to event %d",
	       alarmp->alarmRisingThreshold,
	       alarmp->alarmRisingEventIndex);
	printf("\n Falling threshold is %d, assigned to event %d",
	       alarmp->alarmFallingThreshold,
	       alarmp->alarmFallingEventIndex);
	printf("\n On startup enable %s alarm",
	       startup_alarm_string(alarmp->alarmStartupAlarm));

	free(owner);
	free(object);
	free(alarmp);
    }
    automore_disable();
}

void
nvgen_rmon_alarms (void)
{
    alarmEntry_t   *alarmp;
    int             i, status;
    OctetString    *ownerOS;
    char	   *owner;
    char           *object;

    for (i = 0; i < alarmTable.nitems; i++) {
        alarmp = (alarmEntry_t *) alarmTable.tp[i];
        if (alarmp->alarmStatus != D_alarmStatus_valid)
            continue;

	/* turn the owner string from an octet string to a real string */
	owner = malloc(alarmp->alarmOwner->length + 1);
	if (!owner)
	    return;	/* no memory */

	ownerOS = alarmp->alarmOwner;
	memcpy(owner, ownerOS->octet_ptr, ownerOS->length);

	/* get a textual representation of the object */
	object = malloc(MAX_OCTET_STRING_SIZE);
	if (!object) {
	    free(owner);
	    return;	/* no memory */
	}

	status = MakeDotFromOID(alarmp->alarmVariable, object);
	if (status == -1) {
	    free(object);
	    free(owner);
	    continue;	/* unable to convert oid ?? */
	}

        nv_write(TRUE, "rmon alarm %u %s %u %s rising-threshold %d",
		 alarmp->alarmIndex,
		 object,
		 alarmp->alarmInterval,
		 (alarmp->alarmSampleType == D_alarmSampleType_deltaValue) ?
		 "delta" : "absolute",
		 alarmp->alarmRisingThreshold);
	nv_add(alarmp->alarmRisingEventIndex, " %u",
	       alarmp->alarmRisingEventIndex);
	nv_add(TRUE, " falling-threshold %d",
	       alarmp->alarmFallingThreshold);
	nv_add(alarmp->alarmFallingEventIndex, " %u",
	       alarmp->alarmFallingEventIndex);
	nv_add(strcmp(owner,defaultOwner),
               " owner %S", owner);

	free(owner);
	free(object);
    }
}

void
delete_rmon_alarm (long alarmIndex)
{
    int             index;

    /* find this entry in the table */
    alarmTable.tip[0].value.uint_val = alarmIndex;
    index = SearchTable(&alarmTable, EXACT);
    if (index == -1) {
        printf("\nUnknown alarm number");
    } else {
        DeletealarmTableEntry(index);
    }
}

void
update_rmon_alarm (long alarmIndex,
		   long alarmInterval,
		   long alarmSampleType,
		   long alarmRisingThreshold,
		   long alarmFallingThreshold,
		   long alarmRisingEventIndex,
		   long alarmFallingEventIndex,
		   char *alarmVariable,
		   char *alarmOwner)
{
    int             index;
    int             status;
    alarmEntry_t    alarm;
    doList_t        dolist;

    /* remove any pre-existing entry */
    alarmTable.tip[0].value.uint_val = alarmIndex;
    index = SearchTable(&alarmTable, EXACT);
    if (index != -1) {
	DeletealarmTableEntry(index);
    }

    /* set the defaults */
    memset(&dolist, 0, sizeof(doList_t));
    memset(&alarm, 0, sizeof(alarmEntry_t));
    alarm.alarmIndex = alarmIndex;
    dolist.data = &alarm;
    status = k_alarmEntry_set_defaults(&dolist);
    FreeOID(alarm.alarmVariable);
    FreeOctetString(alarm.alarmOwner);
    if (status != NO_ERROR) {
	printf("\nUnable to create alarm");
	return;
    }

    alarm.alarmStatus = D_alarmStatus_valid;
    alarm.alarmInterval = alarmInterval;
    alarm.alarmSampleType = alarmSampleType;
    alarm.alarmRisingThreshold = alarmRisingThreshold;
    alarm.alarmFallingThreshold = alarmFallingThreshold;
    alarm.alarmRisingEventIndex = alarmRisingEventIndex;
    alarm.alarmFallingEventIndex = alarmFallingEventIndex;

    alarm.alarmVariable = MakeOIDFromDot(alarmVariable);
    if (alarm.alarmVariable == NULL) {
	printf("\nUnknown object: %s", alarmVariable);
	return;
    }

    if (alarmOwner) {
	alarm.alarmOwner = MakeOctetStringFromText(alarmOwner);
    } else {
	alarm.alarmOwner = MakeOctetStringFromText((char *)defaultOwner);
    }

    status = k_alarmEntry_set(&alarm, NULL, ADD_MODIFY);
    if (status != NO_ERROR) {
	if (status == INCONSISTENT_VALUE_ERROR)
	    printf("\nUnknown object: %s", alarmVariable);
	else
	    printf("\nAlarm creation failure");
    }
    FreeOID(alarm.alarmVariable);
    FreeOctetString(alarm.alarmOwner);
}
