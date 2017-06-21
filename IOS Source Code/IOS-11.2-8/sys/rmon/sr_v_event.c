/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_event.c,v 3.2 1995/11/17 18:40:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_event.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_event.c,v $
 * Revision 3.2  1995/11/17  18:40:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:42  shaker
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
#include "../snmp/sr_diag.h"


#define MAXBUFSIZE 32

/*---------------------------------------------------------------------
 * Retrieve data from the eventEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
eventEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    int             carry;
    long            eventIndex;

#ifdef eventEntry_DISABLE_CACHE
    eventEntry_t   *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* eventEntry_DISABLE_CACHE */
    static eventEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* eventEntry_DISABLE_CACHE */

	/*
	 * Check the object instance.
	 * 
	 * An EXACT search requires that the instance be of length 1
	 * 
	 * A NEXT search requires that the requested object does not
	 * lexicographically precede the current object type.
	 */

	if (searchType == EXACT) {
	    if (instLength != 1) {
		return ((VarBind *) NULL);
	    }
	    carry = 0;
	}
	else {
	    carry = 1;
	}

	if ((InstToInt(incoming, 0 + object->oid.length, &eventIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    eventIndex = MAX(1, eventIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_eventEntry_get(serialNum, contextInfo, arg, searchType, eventIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->eventIndex;
	}

#ifndef eventEntry_DISABLE_CACHE
	if (arg == -1) {
	    data = NULL;
	}
	else {
	    last_serialNum = serialNum;
	    if (last_incoming != NULL) {
		FreeOID(last_incoming);
	    }
	    last_incoming = CloneOID(incoming);
	}
    }
#endif				/* eventEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_eventIndex
      case I_eventIndex:
	dp = &data->eventIndex;
	break;
#endif				/* I_eventIndex */

#ifdef I_eventDescription
      case I_eventDescription:
	dp = MakeOctetString(data->eventDescription->octet_ptr, data->eventDescription->length);
	break;
#endif				/* I_eventDescription */

#ifdef I_eventType
      case I_eventType:
	dp = &data->eventType;
	break;
#endif				/* I_eventType */

#ifdef I_eventCommunity
      case I_eventCommunity:
	dp = MakeOctetString(data->eventCommunity->octet_ptr, data->eventCommunity->length);
	break;
#endif				/* I_eventCommunity */

#ifdef I_eventLastTimeSent
      case I_eventLastTimeSent:
	dp = &data->eventLastTimeSent;
	break;
#endif				/* I_eventLastTimeSent */

#ifdef I_eventOwner
      case I_eventOwner:
	dp = MakeOctetString(data->eventOwner->octet_ptr, data->eventOwner->length);
	break;
#endif				/* I_eventOwner */

#ifdef I_eventStatus
      case I_eventStatus:
	dp = &data->eventStatus;
	break;
#endif				/* I_eventStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the eventEntry data object.
 *---------------------------------------------------------------------*/
void
eventEntry_free(data)
    eventEntry_t   *data;
{
    if (data != NULL) {
	k_eventEntryFreeUserpartData(data);
	FreeOctetString(data->eventDescription);
	FreeOctetString(data->eventCommunity);
	FreeOctetString(data->eventOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after eventEntry set/undo
 *---------------------------------------------------------------------*/
static int
eventEntry_cleanup(
    doList_t       *trash)
{
    eventEntry_free(trash->data);

#ifdef SR_SNMPv2
    eventEntry_free(trash->undodata);
#endif				/* SR_SNMPv2 */

    return NO_ERROR;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int
eventEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             error_status;
    int             carry = 0;
    eventEntry_t   *eventEntry;
    long            eventIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &eventIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    eventEntry = k_eventEntry_get(-1, contextInfo, object->nominator, EXACT, eventIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == eventEntry_set) &&
	    (((eventEntry_t *) (dp->data)) != NULL) &&
	    (((eventEntry_t *) (dp->data))->eventIndex == eventIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(eventEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(eventEntry_t));

	dp->setMethod = eventEntry_set;
	dp->cleanupMethod = eventEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = eventEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (eventEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(eventEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (eventEntry), sizeof(eventEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (eventEntry), sizeof(eventEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((eventEntry_t *) (dp->data))->valid);

	    ((eventEntry_t *) (dp->data))->eventDescription =
		CloneOctetString(eventEntry->eventDescription);

#ifdef SR_SNMPv2
	    ((eventEntry_t *) (dp->undodata))->eventDescription =
		CloneOctetString(eventEntry->eventDescription);
#endif				/* SR_SNMPv2 */

	    ((eventEntry_t *) (dp->data))->eventCommunity =
		CloneOctetString(eventEntry->eventCommunity);

#ifdef SR_SNMPv2
	    ((eventEntry_t *) (dp->undodata))->eventCommunity =
		CloneOctetString(eventEntry->eventCommunity);
#endif				/* SR_SNMPv2 */

	    ((eventEntry_t *) (dp->data))->eventOwner =
		CloneOctetString(eventEntry->eventOwner);

#ifdef SR_SNMPv2
	    ((eventEntry_t *) (dp->undodata))->eventOwner =
		CloneOctetString(eventEntry->eventOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef eventEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((eventEntry_t *) (dp->data))->eventIndex = eventIndex;
	    SET_VALID(I_eventIndex, ((eventEntry_t *) (dp->data))->valid);

	    error_status = k_eventEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* eventEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* eventEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_eventDescription
      case I_eventDescription:

	if (((eventEntry_t *) (dp->data))->eventDescription != NULL) {
	    FreeOctetString(((eventEntry_t *) (dp->data))->eventDescription);
	}

	((eventEntry_t *) (dp->data))->eventDescription =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_eventDescription */

#ifdef I_eventType
      case I_eventType:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	((eventEntry_t *) (dp->data))->eventType = value->sl_value;
	break;
#endif				/* I_eventType */

#ifdef I_eventCommunity
      case I_eventCommunity:

	if (((eventEntry_t *) (dp->data))->eventCommunity != NULL) {
	    FreeOctetString(((eventEntry_t *) (dp->data))->eventCommunity);
	}

	((eventEntry_t *) (dp->data))->eventCommunity =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_eventCommunity */

#ifdef I_eventOwner
      case I_eventOwner:

	if (((eventEntry_t *) (dp->data))->eventOwner != NULL) {
	    FreeOctetString(((eventEntry_t *) (dp->data))->eventOwner);
	}

	((eventEntry_t *) (dp->data))->eventOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_eventOwner */

#ifdef I_eventStatus
      case I_eventStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
			 eventEntry == NULL ? 0L : eventEntry->eventStatus);
	if (error_status != 0)
	    return (error_status);

	((eventEntry_t *) (dp->data))->eventStatus = value->sl_value;
	break;
#endif				/* I_eventStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in eventEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_eventEntry_test */
    error_status = k_eventEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((eventEntry_t *) (dp->data))->valid);
	error_status = k_eventEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
eventEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_eventEntry_set((eventEntry_t *) (doCur->data),
			     contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the logEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
logEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    int             carry;
    long            logEventIndex;
    long            logIndex;

#ifdef logEntry_DISABLE_CACHE
    logEntry_t     *data;
    unsigned long   buffer[2];
    OID             inst;
#else				/* logEntry_DISABLE_CACHE */
    static logEntry_t *data;
    static unsigned long buffer[2];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* logEntry_DISABLE_CACHE */

	/*
	 * Check the object instance.
	 * 
	 * An EXACT search requires that the instance be of length 2
	 * 
	 * A NEXT search requires that the requested object does not
	 * lexicographically precede the current object type.
	 */

	if (searchType == EXACT) {
	    if (instLength != 2) {
		return ((VarBind *) NULL);
	    }
	    carry = 0;
	}
	else {
	    carry = 1;
	}

	if ((InstToInt(incoming, 1 + object->oid.length, &logIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToInt(incoming, 0 + object->oid.length, &logEventIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    logEventIndex = MAX(1, logEventIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_logEntry_get(serialNum, contextInfo, arg, searchType, logEventIndex, logIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 2;
	    inst.oid_ptr[0] = data->logEventIndex;
	    inst.oid_ptr[1] = data->logIndex;
	}

#ifndef logEntry_DISABLE_CACHE
	if (arg == -1) {
	    data = NULL;
	}
	else {
	    last_serialNum = serialNum;
	    if (last_incoming != NULL) {
		FreeOID(last_incoming);
	    }
	    last_incoming = CloneOID(incoming);
	}
    }
#endif				/* logEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_logEventIndex
      case I_logEventIndex:
	dp = &data->logEventIndex;
	break;
#endif				/* I_logEventIndex */

#ifdef I_logIndex
      case I_logIndex:
	dp = &data->logIndex;
	break;
#endif				/* I_logIndex */

#ifdef I_logTime
      case I_logTime:
	dp = &data->logTime;
	break;
#endif				/* I_logTime */

#ifdef I_logDescription
      case I_logDescription:
	dp = MakeOctetString(data->logDescription->octet_ptr, data->logDescription->length);
	break;
#endif				/* I_logDescription */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}
