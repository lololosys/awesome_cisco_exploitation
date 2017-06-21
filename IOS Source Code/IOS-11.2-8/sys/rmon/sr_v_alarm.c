/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_alarm.c,v 3.2 1995/11/17 18:39:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_alarm.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_alarm.c,v $
 * Revision 3.2  1995/11/17  18:39:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:39  shaker
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
 * Retrieve data from the alarmEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
alarmEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            alarmIndex;

#ifdef alarmEntry_DISABLE_CACHE
    alarmEntry_t   *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* alarmEntry_DISABLE_CACHE */
    static alarmEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* alarmEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &alarmIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    alarmIndex = MAX(1, alarmIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_alarmEntry_get(serialNum, contextInfo, arg, searchType, alarmIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->alarmIndex;
	}

#ifndef alarmEntry_DISABLE_CACHE
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
#endif				/* alarmEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_alarmIndex
      case I_alarmIndex:
	dp = &data->alarmIndex;
	break;
#endif				/* I_alarmIndex */

#ifdef I_alarmInterval
      case I_alarmInterval:
	dp = &data->alarmInterval;
	break;
#endif				/* I_alarmInterval */

#ifdef I_alarmVariable
      case I_alarmVariable:
	dp = MakeOID(data->alarmVariable->oid_ptr, data->alarmVariable->length);
	break;
#endif				/* I_alarmVariable */

#ifdef I_alarmSampleType
      case I_alarmSampleType:
	dp = &data->alarmSampleType;
	break;
#endif				/* I_alarmSampleType */

#ifdef I_alarmValue
      case I_alarmValue:
	dp = &data->alarmValue;
	break;
#endif				/* I_alarmValue */

#ifdef I_alarmStartupAlarm
      case I_alarmStartupAlarm:
	dp = &data->alarmStartupAlarm;
	break;
#endif				/* I_alarmStartupAlarm */

#ifdef I_alarmRisingThreshold
      case I_alarmRisingThreshold:
	dp = &data->alarmRisingThreshold;
	break;
#endif				/* I_alarmRisingThreshold */

#ifdef I_alarmFallingThreshold
      case I_alarmFallingThreshold:
	dp = &data->alarmFallingThreshold;
	break;
#endif				/* I_alarmFallingThreshold */

#ifdef I_alarmRisingEventIndex
      case I_alarmRisingEventIndex:
	dp = &data->alarmRisingEventIndex;
	break;
#endif				/* I_alarmRisingEventIndex */

#ifdef I_alarmFallingEventIndex
      case I_alarmFallingEventIndex:
	dp = &data->alarmFallingEventIndex;
	break;
#endif				/* I_alarmFallingEventIndex */

#ifdef I_alarmOwner
      case I_alarmOwner:
	dp = MakeOctetString(data->alarmOwner->octet_ptr, data->alarmOwner->length);
	break;
#endif				/* I_alarmOwner */

#ifdef I_alarmStatus
      case I_alarmStatus:
	dp = &data->alarmStatus;
	break;
#endif				/* I_alarmStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the alarmEntry data object.
 *---------------------------------------------------------------------*/
void
alarmEntry_free(data)
    alarmEntry_t   *data;
{
    if (data != NULL) {
	k_alarmEntryFreeUserpartData(data);
	FreeOID(data->alarmVariable);
	FreeOctetString(data->alarmOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after alarmEntry set/undo
 *---------------------------------------------------------------------*/
static int
alarmEntry_cleanup(
    doList_t       *trash)
{
    alarmEntry_free(trash->data);

#ifdef SR_SNMPv2
    alarmEntry_free(trash->undodata);
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
alarmEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    alarmEntry_t   *alarmEntry;
    long            alarmIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &alarmIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    alarmEntry = k_alarmEntry_get(-1, contextInfo, object->nominator, EXACT, alarmIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == alarmEntry_set) &&
	    (((alarmEntry_t *) (dp->data)) != NULL) &&
	    (((alarmEntry_t *) (dp->data))->alarmIndex == alarmIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(alarmEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(alarmEntry_t));

	dp->setMethod = alarmEntry_set;
	dp->cleanupMethod = alarmEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = alarmEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (alarmEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(alarmEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (alarmEntry), sizeof(alarmEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (alarmEntry), sizeof(alarmEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((alarmEntry_t *) (dp->data))->valid);

	    ((alarmEntry_t *) (dp->data))->alarmVariable =
		CloneOID(alarmEntry->alarmVariable);

#ifdef SR_SNMPv2
	    ((alarmEntry_t *) (dp->undodata))->alarmVariable =
		CloneOID(alarmEntry->alarmVariable);
#endif				/* SR_SNMPv2 */

	    ((alarmEntry_t *) (dp->data))->alarmOwner =
		CloneOctetString(alarmEntry->alarmOwner);

#ifdef SR_SNMPv2
	    ((alarmEntry_t *) (dp->undodata))->alarmOwner =
		CloneOctetString(alarmEntry->alarmOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef alarmEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((alarmEntry_t *) (dp->data))->alarmIndex = alarmIndex;
	    SET_VALID(I_alarmIndex, ((alarmEntry_t *) (dp->data))->valid);

	    error_status = k_alarmEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* alarmEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* alarmEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_alarmInterval
      case I_alarmInterval:

	((alarmEntry_t *) (dp->data))->alarmInterval = value->sl_value;
	break;
#endif				/* I_alarmInterval */

#ifdef I_alarmVariable
      case I_alarmVariable:

	if (((alarmEntry_t *) (dp->data))->alarmVariable != NULL) {
	    FreeOID(((alarmEntry_t *) (dp->data))->alarmVariable);
	}

	((alarmEntry_t *) (dp->data))->alarmVariable =
	    MakeOID(value->oid_value->oid_ptr, value->oid_value->length);

	break;
#endif				/* I_alarmVariable */

#ifdef I_alarmSampleType
      case I_alarmSampleType:

	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}

	((alarmEntry_t *) (dp->data))->alarmSampleType = value->sl_value;
	break;
#endif				/* I_alarmSampleType */

#ifdef I_alarmStartupAlarm
      case I_alarmStartupAlarm:

	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}

	((alarmEntry_t *) (dp->data))->alarmStartupAlarm = value->sl_value;
	break;
#endif				/* I_alarmStartupAlarm */

#ifdef I_alarmRisingThreshold
      case I_alarmRisingThreshold:

	((alarmEntry_t *) (dp->data))->alarmRisingThreshold = value->sl_value;
	break;
#endif				/* I_alarmRisingThreshold */

#ifdef I_alarmFallingThreshold
      case I_alarmFallingThreshold:

	((alarmEntry_t *) (dp->data))->alarmFallingThreshold = value->sl_value;
	break;
#endif				/* I_alarmFallingThreshold */

#ifdef I_alarmRisingEventIndex
      case I_alarmRisingEventIndex:

	if (((value->sl_value < 0) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((alarmEntry_t *) (dp->data))->alarmRisingEventIndex = value->sl_value;
	break;
#endif				/* I_alarmRisingEventIndex */

#ifdef I_alarmFallingEventIndex
      case I_alarmFallingEventIndex:

	if (((value->sl_value < 0) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((alarmEntry_t *) (dp->data))->alarmFallingEventIndex = value->sl_value;
	break;
#endif				/* I_alarmFallingEventIndex */

#ifdef I_alarmOwner
      case I_alarmOwner:

	if (((alarmEntry_t *) (dp->data))->alarmOwner != NULL) {
	    FreeOctetString(((alarmEntry_t *) (dp->data))->alarmOwner);
	}

	((alarmEntry_t *) (dp->data))->alarmOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_alarmOwner */

#ifdef I_alarmStatus
      case I_alarmStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
			 alarmEntry == NULL ? 0L : alarmEntry->alarmStatus);
	if (error_status != 0)
	    return (error_status);

	((alarmEntry_t *) (dp->data))->alarmStatus = value->sl_value;
	break;
#endif				/* I_alarmStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in alarmEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_alarmEntry_test */
    error_status = k_alarmEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((alarmEntry_t *) (dp->data))->valid);
	error_status = k_alarmEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
alarmEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_alarmEntry_set((alarmEntry_t *) (doCur->data),
			     contextInfo, doCur->state));
}

#endif				/* SETS */
