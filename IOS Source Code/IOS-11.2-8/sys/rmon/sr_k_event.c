/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_event.c,v 3.5.12.2 1996/04/04 19:40:31 sdurham Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_event.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_event.c,v $
 * Revision 3.5.12.2  1996/04/04  19:40:31  sdurham
 * CSCdi53050:  RMON eventLastTimeSent has bogus value if event added via
 * cli
 * Branch: California_branch
 *             Initialize it to zero at creation
 *
 * Revision 3.5.12.1  1996/03/29  18:16:47  sdurham
 * CSCdi50963:  System restart during MIB walk of rmon log table
 * Branch: California_branch
 *          initialize log entry data.
 *
 * Revision 3.5  1996/02/19  06:05:58  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.4  1996/02/18  17:57:52  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.3  1996/02/09  07:57:25  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:14  shaker
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
#include "rmon_registry.h"
#include "rmon_util.h"
#include "logger.h"
#include "../os/free.h"

/* global data describing the eventTable entries */
static SnmpType        eventEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* eventIndex */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* eventDescription */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* eventType */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* eventCommunity */
    {TIME_TICKS_TYPE, SR_READ_ONLY, -1},	/* eventLastTimeSent */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* eventOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* eventStatus */
};

static Index_t         eventEntryIndex[1];
static RmonTable       eventTable;

/* statistics about success and failure of table operations */
unsigned long   logCreateFails = 0;

void DeleteeventTableEntry SR_PROTOTYPE((int index));

/* this routine allocates and initializes a log table entry */
logEntry_t     *
CreateLogEntry(index, descr)
    long            index;
    OctetString    *descr;
{
    logEntry_t     *logp;

    /* allocate the new entry */
    logp = malloc_named(sizeof(logEntry_t), "RMON Log Data");
    if (logp) {
	/* initialize the entry */
	memset(logp, 0, sizeof(logEntry_t));
	if ((logp->logDescription = CloneOctetString(descr)) == 0) {
	    /* not enough memory */
	    free(logp);
	    logp = 0;
	}
	else {
	    logp->logEventIndex = index;
	    logp->logTime = TimerCurTime();
	}
    }

    /* return the new entry, or NULL if none */
    return (logp);
}

/* this routine frees a log entry */
void
FreeLogEntry(logp)
    logEntry_t     *logp;
{
    if (logp != 0) {
	FreeOctetString(logp->logDescription);
	free(logp);
    }
}

/* this routine deletes a log entries from the event it is associated with */
void
DeleteLogEntry(evp)
    eventEntry_t   *evp;
{
    logEntry_t     *logp;

    /* remove an entry from the end of the list */
    if ((logp = evp->last) != 0) {
	evp->last = logp->prev;
	if (evp->last == 0) {
	    evp->first = 0;
	}

	/* free the entry */
	FreeLogEntry(logp);
    }
}

/* this routine frees a log table associated with an event */
void
DeleteLogTable(evp)
    eventEntry_t   *evp;
{
    /* delete entries until none are left */
    while (evp->first != 0) {
	DeleteLogEntry(evp);
    }
}

/* this routine adds a log entry to the table maintained by an event */
void
AddLogEntry(evp, descr)
    eventEntry_t   *evp;
    OctetString    *descr;
{
    logEntry_t     *logp;

    /* if we're short of memory free some up */
    if (HeapLowMem() != 0)
	DeleteLogEntry(evp);

    /* first allocate a log entry */
    if ((logp = CreateLogEntry(evp->eventIndex, descr)) == 0) {
	logCreateFails++;
    }
    else {
	/* insert it into the table */
	if (evp->first == 0) {
	    /* create the table */
	    evp->first = logp;
	    evp->last = logp;
	    logp->logIndex = 1;
	}
	else {
	    /* table exists, add it at the beginning */
	    logp->logIndex = evp->first->logIndex + 1;
	    logp->next = evp->first;
	    evp->first->prev = logp;
	    evp->first = logp;
	}
	evp->first->prev = 0;
	evp->last->next = 0;
    }
}

/* this routine causes whatever is supposed to happen when an event comes true
 * to happen. */
void
FireEvent(eventIndex, alarmFunction, dataval, datap)
    long            eventIndex;
    void            (*alarmFunction) (eventEntry_t   *evp,
				      unsigned long type,
				      alarmEntry_t *alarmp);
    unsigned long   dataval;
    void           *datap;
{
    int             index;
    eventEntry_t   *evp;

    /* search for the event */
    eventTable.tip[0].value.uint_val = eventIndex;
    if ((index = SearchTable(&eventTable, EXACT)) != -1) {
	/* get a pointer to the event structure */
	evp = (eventEntry_t *) eventTable.tp[index];
	if (evp->eventStatus == D_eventStatus_valid) {
	    /* update the event's state */
	    evp->eventLastTimeSent = TimerCurTime();

	    /* log events if necessary */
	    if (evp->eventType == D_eventType_log
		|| evp->eventType == D_eventType_log_and_trap) {
		/* use the event's description clause, by default */
		AddLogEntry(evp, evp->eventDescription);
	    }

	    /* send an alarm (i.e., a trap), if necessary */
	    if (evp->eventType == D_eventType_snmp_trap
		|| evp->eventType == D_eventType_log_and_trap) {
		/* send the trap */
		if (alarmFunction != 0) {
		    (*alarmFunction) (evp, dataval, datap);
		}
	    }

	    /* update channel data control state */
	    reg_invoke_rmon_event(eventIndex);
	}
    }
}


/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_eventEntry_initialize()
{
    eventEntryIndex[0].offset = I_eventIndex;
    eventEntryIndex[0].type = T_uint;

    eventTable.nindices = 1;
    eventTable.tip = eventEntryIndex;
    eventTable.nitems = 0;
    eventTable.rowsize = sizeof(eventEntry_t);
    if ((eventTable.tp = (void **) malloc(sizeof(eventEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_eventEntry_terminate()
{
    while (eventTable.nitems > 0) {
	DeleteeventTableEntry(0);
    }

    free(eventTable.tp);
    return 1;
}

int
k_logEntry_initialize()
{
    return 1;
}

int
k_logEntry_terminate()
{
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
eventTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    eventEntry_t   *data;

    /* dummy up an entry to delete */
    data = (eventEntry_t *) tdp->UserData2;
    data->eventStatus = D_eventStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_eventEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the eventTable */
void
DeleteeventTableEntry(index)
    int             index;
{
    eventEntry_t   *data;

    /* get a pointer to the old entry */
    data = (eventEntry_t *) eventTable.tp[index];

    /* free any log entries associated with this event */
    DeleteLogTable(data);

    /* free the old entry and remove it from the table */
    eventEntry_free(data);
    RemoveTableEntry(&eventTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_eventEntryFreeUserpartData(data)
    eventEntry_t   *data;
{
    /* nothing to free by default */
}

eventEntry_t   *
k_eventEntry_get(serialNum, contextInfo, nominator, searchType, eventIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            eventIndex;
{
    int             index;

    eventTable.tip[0].value.uint_val = eventIndex;
    if ((index = SearchTable(&eventTable, searchType)) == -1) {
	return NULL;
    }

    return (eventEntry_t *) eventTable.tp[index];

}

#ifdef SETS
int
k_eventEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_eventEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    eventEntry_t   *data = (eventEntry_t *) dp->data;

    if (data->eventStatus == D_eventStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;
    }

    return NO_ERROR;
}

int
k_eventEntry_set_defaults(dp)
    doList_t       *dp;
{

    eventEntry_t   *data = (eventEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    /* by default, take no action on this event */
    data->eventType = D_eventType_none;

    if ((data->eventDescription = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->eventCommunity = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->eventOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->eventStatus = D_eventStatus_createRequest;
    data->TimerId = -1;
    data->first = 0;
    data->last = 0;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_eventEntry_set(data, contextInfo, function)
    eventEntry_t   *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    eventEntry_t   *new = NULL;

    /* find this entry in the table */
    eventTable.tip[0].value.uint_val = data->eventIndex;
    if ((index = SearchTable(&eventTable, EXACT)) != -1) {
	new = (eventEntry_t *) eventTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->eventStatus == D_eventStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeleteeventTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	eventTable.tip[0].value.uint_val = data->eventIndex;
	if ((index = NewTableEntry(&eventTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (eventEntry_t *) eventTable.tp[index];
	name_memory(new, "RMON Event Data");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, eventTableDeleteCallback)) == -1) {
	    DeleteeventTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_eventStatus, eventEntryTypeTable,
		    (unsigned long *)new,
		    (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private state data */
    new->first = data->first;
    new->last = data->last;

    /* if the event is no longer a logging event, get rid of old log entries */
    if (new->eventType != D_eventType_log
	&& new->eventType != D_eventType_log_and_trap) {
	DeleteLogTable(new);
    }

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->eventStatus == D_eventStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;
    }
    else {
	if (new->eventStatus == D_eventStatus_createRequest) {
	    new->eventStatus = D_eventStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
eventEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

logEntry_t     *
k_logEntry_get(serialNum, contextInfo, nominator, searchType, logEventIndex, logIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            logEventIndex;
    long            logIndex;
{
    eventEntry_t   *evp;
    logEntry_t     *logp, *prevlogp;
    logEntry_t     *data;
    int             index;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the history control entry */
	eventTable.tip[0].value.uint_val = logEventIndex;
	if ((index = SearchTable(&eventTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	evp = (eventEntry_t *) eventTable.tp[index];

	/* search the history, newest first, for the entry */
	prevlogp = 0;
	for (logp = evp->first; logp != 0; logp = logp->next) {
	    if (logp->logIndex <= logIndex) {
		break;
	    }
	    prevlogp = logp;
	}

	/* check for an exact match */
	if (logp != 0) {
	    if (logp->logIndex == logIndex) {
		data = logp;
	    }
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    if (prevlogp != 0) {
		/* the preceding entry was the next larger */
		data = prevlogp;
	    }
	    else {
		/* no entries, look in the next table */
		logEventIndex++;
		logIndex = 0;
	    }
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

/******************** cisco supplied routines follow ********************/

static const char defaultDescription[] = "";
static const char defaultOwner[] = "config";

void
show_rmon_events (void)
{
    eventEntry_t   *eventp;
    logEntry_t     *logp;
    int             i;
    OctetString    *tempOS;
    char           *owner;
    char           *description;
    char	   *community;

    automore_enable(NULL);
    if (eventTable.nitems == 0) {
        printf("\nEvent table is empty");
        automore_disable();
        return;
    }

    for (i = 0; i < eventTable.nitems; i++) {
        eventp = (eventEntry_t *) eventTable.tp[i];
        mem_lock(eventp);

        /* turn the community string from an octet string to a real string */
        community = malloc(eventp->eventCommunity->length + 1);
        if (community) {
            tempOS = eventp->eventCommunity;
            memcpy(community, tempOS->octet_ptr, tempOS->length);
        }

        /* turn the description string from an octet string to a real string */
        description = malloc(eventp->eventDescription->length + 1);
        if (description) {
            tempOS = eventp->eventDescription;
            memcpy(description, tempOS->octet_ptr, tempOS->length);
        }

        /* turn the owner string from an octet string to a real string */
        owner = malloc(eventp->eventOwner->length + 1);
        if (owner) {
            tempOS = eventp->eventOwner;
            memcpy(owner, tempOS->octet_ptr, tempOS->length);
        }

        printf("\nEvent %d is %s, owned by %s",
               eventp->eventIndex,
               rmon_status_string(eventp->eventStatus),
               owner ? owner : "unknown");
        printf("\n Description is %s",
               description ? description : "unknown");
        printf("\n Event firing causes ");
	switch (eventp->eventType) {
	case D_eventType_none:
	    printf("nothing");
	    break;
	case D_eventType_log:
	    printf("log");
	    break;
	case D_eventType_log_and_trap:
	    printf("log and ");
	    /* fall thru */
	case D_eventType_snmp_trap:
	    printf("trap to community %s", community);
	}
	printf(", last fired %TC", eventp->eventLastTimeSent);

        free(owner);
        free(community);
        free(description);

	if (eventp->last) {
	    printf("\n Current log entries:");
	    automore_header("      index       time   description");
	    for (logp = eventp->last; logp; logp = logp->prev) {

		description = malloc(logp->logDescription->length + 1);
		if (description) {
		    tempOS = logp->logDescription;
		    memcpy(description, tempOS->octet_ptr, tempOS->length);
		}

		printf("\n  %-9d  %-9TC   %s",
		       logp->logIndex,
		       logp->logTime,
		       description ? description : "unknown");

		free(description);
	    }
	    automore_header(NULL);
	}

        free(eventp);
    }
    automore_disable();

}

void
nvgen_rmon_events (void)
{
    eventEntry_t   *eventp;
    int             i;
    OctetString    *tempOS;
    char           *owner;
    char           *description;
    char	   *community;

    for (i = 0; i < eventTable.nitems; i++) {
        eventp = (eventEntry_t *) eventTable.tp[i];
	if (eventp->eventStatus != D_eventStatus_valid)
	    continue;

        /* turn the community string from an octet string to a real string */
        community = malloc(eventp->eventCommunity->length + 1);
        if (community) {
            tempOS = eventp->eventCommunity;
            memcpy(community, tempOS->octet_ptr, tempOS->length);
        }

        /* turn the description string from an octet string to a real string */
        description = malloc(eventp->eventDescription->length + 1);
        if (description) {
            tempOS = eventp->eventDescription;
            memcpy(description, tempOS->octet_ptr, tempOS->length);
        }

        /* turn the owner string from an octet string to a real string */
        owner = malloc(eventp->eventOwner->length + 1);
        if (owner) {
            tempOS = eventp->eventOwner;
            memcpy(owner, tempOS->octet_ptr, tempOS->length);
        }

	nv_write(TRUE, "rmon event %d", eventp->eventIndex);
	nv_add((eventp->eventType == D_eventType_log) ||
	       (eventp->eventType == D_eventType_log_and_trap),
	       " log");
	nv_add((eventp->eventType == D_eventType_snmp_trap) ||
	       (eventp->eventType == D_eventType_log_and_trap),
	       " trap %S", community);
	nv_add(strcmp(description,defaultDescription),
	       " description %S", description);
	nv_add(strcmp(owner,defaultOwner),
	       " owner %S", owner);

        free(owner);
        free(community);
        free(description);
    }
}

void
delete_rmon_event (long eventIndex)
{
    int		    index;

    /* find this entry in the table */
    eventTable.tip[0].value.uint_val = eventIndex;
    index = SearchTable(&eventTable, EXACT);
    if (index == -1) {
	printf("\nUnknown event number");
    } else {
	DeleteeventTableEntry(index);
    }
}

void
update_rmon_event (long eventIndex,
		   long eventType,
		   char *eventCommunity,
		   char *eventDescription,
		   char *eventOwner)
{
    int		    index;
    eventEntry_t    newep;
    eventEntry_t   *oldep = NULL;

    /* see if this is an existing event */
    eventTable.tip[0].value.uint_val = eventIndex;
    index = SearchTable(&eventTable, EXACT);
    if (index != -1) {
	/*
	 * it's an existing entry, use it for default octet strings
	 */
	oldep = (eventEntry_t *) eventTable.tp[index];
    }

    newep.eventIndex = eventIndex;
    newep.eventType = eventType;
    newep.eventStatus = D_eventStatus_valid;
    newep.TimerId = -1;
    newep.first = 0;  
    newep.last = 0;
    newep.eventLastTimeSent = 0;

    if (eventCommunity) {
	newep.eventCommunity = MakeOctetStringFromText(eventCommunity);
    } else if (oldep) {
	newep.eventCommunity = CloneOctetString(oldep->eventCommunity);
    } else {
	newep.eventCommunity = MakeOctetStringFromText("");
    }
    if (eventDescription) {
	newep.eventDescription = MakeOctetStringFromText(eventDescription);
    } else if (oldep) {
	newep.eventDescription = CloneOctetString(oldep->eventDescription);
    } else {
	newep.eventDescription = MakeOctetStringFromText((char *)defaultDescription);
    }
    if (eventOwner) {
	newep.eventOwner = MakeOctetStringFromText(eventOwner);
    } else if (oldep) {
	newep.eventOwner = CloneOctetString(oldep->eventOwner);
    } else {
	newep.eventOwner = MakeOctetStringFromText((char *)defaultOwner);
    }

    k_eventEntry_set(&newep, NULL, ADD_MODIFY);

    FreeOctetString(newep.eventCommunity);
    FreeOctetString(newep.eventDescription);
    FreeOctetString(newep.eventOwner);
}
