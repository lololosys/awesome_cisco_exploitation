/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_hist.c,v 3.2 1995/11/17 18:40:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_hist.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_hist.c,v $
 * Revision 3.2  1995/11/17  18:40:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:45  shaker
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
 * Retrieve data from the historyControlEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
historyControlEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            historyControlIndex;

#ifdef historyControlEntry_DISABLE_CACHE
    historyControlEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* historyControlEntry_DISABLE_CACHE */
    static historyControlEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* historyControlEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &historyControlIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    historyControlIndex = MAX(1, historyControlIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_historyControlEntry_get(serialNum, contextInfo, arg, searchType, historyControlIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->historyControlIndex;
	}

#ifndef historyControlEntry_DISABLE_CACHE
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
#endif				/* historyControlEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_historyControlIndex
      case I_historyControlIndex:
	dp = &data->historyControlIndex;
	break;
#endif				/* I_historyControlIndex */

#ifdef I_historyControlDataSource
      case I_historyControlDataSource:
	dp = MakeOID(data->historyControlDataSource->oid_ptr, data->historyControlDataSource->length);
	break;
#endif				/* I_historyControlDataSource */

#ifdef I_historyControlBucketsRequested
      case I_historyControlBucketsRequested:
	dp = &data->historyControlBucketsRequested;
	break;
#endif				/* I_historyControlBucketsRequested */

#ifdef I_historyControlBucketsGranted
      case I_historyControlBucketsGranted:
	dp = &data->historyControlBucketsGranted;
	break;
#endif				/* I_historyControlBucketsGranted */

#ifdef I_historyControlInterval
      case I_historyControlInterval:
	dp = &data->historyControlInterval;
	break;
#endif				/* I_historyControlInterval */

#ifdef I_historyControlOwner
      case I_historyControlOwner:
	dp = MakeOctetString(data->historyControlOwner->octet_ptr, data->historyControlOwner->length);
	break;
#endif				/* I_historyControlOwner */

#ifdef I_historyControlStatus
      case I_historyControlStatus:
	dp = &data->historyControlStatus;
	break;
#endif				/* I_historyControlStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the historyControlEntry data object.
 *---------------------------------------------------------------------*/
void
historyControlEntry_free(data)
    historyControlEntry_t *data;
{
    if (data != NULL) {
	k_historyControlEntryFreeUserpartData(data);
	FreeOID(data->historyControlDataSource);
	FreeOctetString(data->historyControlOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after historyControlEntry set/undo
 *---------------------------------------------------------------------*/
static int
historyControlEntry_cleanup(
    doList_t       *trash)
{
    historyControlEntry_free(trash->data);

#ifdef SR_SNMPv2
    historyControlEntry_free(trash->undodata);
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
historyControlEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    historyControlEntry_t *historyControlEntry;
    long            historyControlIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &historyControlIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    historyControlEntry = k_historyControlEntry_get(-1, contextInfo, object->nominator, EXACT, historyControlIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == historyControlEntry_set) &&
	    (((historyControlEntry_t *) (dp->data)) != NULL) &&
	    (((historyControlEntry_t *) (dp->data))->historyControlIndex == historyControlIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(historyControlEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(historyControlEntry_t));

	dp->setMethod = historyControlEntry_set;
	dp->cleanupMethod = historyControlEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = historyControlEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (historyControlEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(historyControlEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (historyControlEntry), sizeof(historyControlEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (historyControlEntry), sizeof(historyControlEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((historyControlEntry_t *) (dp->data))->valid);

	    ((historyControlEntry_t *) (dp->data))->historyControlDataSource =
		CloneOID(historyControlEntry->historyControlDataSource);

#ifdef SR_SNMPv2
	    ((historyControlEntry_t *) (dp->undodata))->historyControlDataSource =
		CloneOID(historyControlEntry->historyControlDataSource);
#endif				/* SR_SNMPv2 */

	    ((historyControlEntry_t *) (dp->data))->historyControlOwner =
		CloneOctetString(historyControlEntry->historyControlOwner);

#ifdef SR_SNMPv2
	    ((historyControlEntry_t *) (dp->undodata))->historyControlOwner =
		CloneOctetString(historyControlEntry->historyControlOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef historyControlEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((historyControlEntry_t *) (dp->data))->historyControlIndex = historyControlIndex;
	    SET_VALID(I_historyControlIndex, ((historyControlEntry_t *) (dp->data))->valid);

	    error_status = k_historyControlEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* historyControlEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* historyControlEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_historyControlDataSource
      case I_historyControlDataSource:

	if (((historyControlEntry_t *) (dp->data))->historyControlDataSource != NULL) {
	    FreeOID(((historyControlEntry_t *) (dp->data))->historyControlDataSource);
	}

	((historyControlEntry_t *) (dp->data))->historyControlDataSource =
	    MakeOID(value->oid_value->oid_ptr, value->oid_value->length);

	break;
#endif				/* I_historyControlDataSource */

#ifdef I_historyControlBucketsRequested
      case I_historyControlBucketsRequested:

	if (((value->sl_value < 1) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((historyControlEntry_t *) (dp->data))->historyControlBucketsRequested = value->sl_value;
	break;
#endif				/* I_historyControlBucketsRequested */

#ifdef I_historyControlInterval
      case I_historyControlInterval:

	if (((value->sl_value < 1) || (value->sl_value > 3600))) {
	    return WRONG_VALUE_ERROR;
	}

	((historyControlEntry_t *) (dp->data))->historyControlInterval = value->sl_value;
	break;
#endif				/* I_historyControlInterval */

#ifdef I_historyControlOwner
      case I_historyControlOwner:

	if (((historyControlEntry_t *) (dp->data))->historyControlOwner != NULL) {
	    FreeOctetString(((historyControlEntry_t *) (dp->data))->historyControlOwner);
	}

	((historyControlEntry_t *) (dp->data))->historyControlOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_historyControlOwner */

#ifdef I_historyControlStatus
      case I_historyControlStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
					historyControlEntry == NULL ? 0L : historyControlEntry->historyControlStatus);
	if (error_status != 0)
	    return (error_status);

	((historyControlEntry_t *) (dp->data))->historyControlStatus = value->sl_value;
	break;
#endif				/* I_historyControlStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in historyControlEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_historyControlEntry_test */
    error_status = k_historyControlEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((historyControlEntry_t *) (dp->data))->valid);
	error_status = k_historyControlEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
historyControlEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_historyControlEntry_set((historyControlEntry_t *) (doCur->data),
				      contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the etherHistoryEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
etherHistoryEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            etherHistoryIndex;
    long            etherHistorySampleIndex;

#ifdef etherHistoryEntry_DISABLE_CACHE
    etherHistoryEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
#else				/* etherHistoryEntry_DISABLE_CACHE */
    static etherHistoryEntry_t *data;
    static unsigned long buffer[2];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* etherHistoryEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 1 + object->oid.length, &etherHistorySampleIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToInt(incoming, 0 + object->oid.length, &etherHistoryIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    etherHistoryIndex = MAX(1, etherHistoryIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_etherHistoryEntry_get(serialNum, contextInfo, arg, searchType, etherHistoryIndex, etherHistorySampleIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 2;
	    inst.oid_ptr[0] = data->etherHistoryIndex;
	    inst.oid_ptr[1] = data->etherHistorySampleIndex;
	}

#ifndef etherHistoryEntry_DISABLE_CACHE
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
#endif				/* etherHistoryEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_etherHistoryIndex
      case I_etherHistoryIndex:
	dp = &data->etherHistoryIndex;
	break;
#endif				/* I_etherHistoryIndex */

#ifdef I_etherHistorySampleIndex
      case I_etherHistorySampleIndex:
	dp = &data->etherHistorySampleIndex;
	break;
#endif				/* I_etherHistorySampleIndex */

#ifdef I_etherHistoryIntervalStart
      case I_etherHistoryIntervalStart:
	dp = &data->etherHistoryIntervalStart;
	break;
#endif				/* I_etherHistoryIntervalStart */

#ifdef I_etherHistoryDropEvents
      case I_etherHistoryDropEvents:
	dp = &data->etherHistoryDropEvents;
	break;
#endif				/* I_etherHistoryDropEvents */

#ifdef I_etherHistoryOctets
      case I_etherHistoryOctets:
	dp = &data->etherHistoryOctets;
	break;
#endif				/* I_etherHistoryOctets */

#ifdef I_etherHistoryPkts
      case I_etherHistoryPkts:
	dp = &data->etherHistoryPkts;
	break;
#endif				/* I_etherHistoryPkts */

#ifdef I_etherHistoryBroadcastPkts
      case I_etherHistoryBroadcastPkts:
	dp = &data->etherHistoryBroadcastPkts;
	break;
#endif				/* I_etherHistoryBroadcastPkts */

#ifdef I_etherHistoryMulticastPkts
      case I_etherHistoryMulticastPkts:
	dp = &data->etherHistoryMulticastPkts;
	break;
#endif				/* I_etherHistoryMulticastPkts */

#ifdef I_etherHistoryCRCAlignErrors
      case I_etherHistoryCRCAlignErrors:
	dp = &data->etherHistoryCRCAlignErrors;
	break;
#endif				/* I_etherHistoryCRCAlignErrors */

#ifdef I_etherHistoryUndersizePkts
      case I_etherHistoryUndersizePkts:
	dp = &data->etherHistoryUndersizePkts;
	break;
#endif				/* I_etherHistoryUndersizePkts */

#ifdef I_etherHistoryOversizePkts
      case I_etherHistoryOversizePkts:
	dp = &data->etherHistoryOversizePkts;
	break;
#endif				/* I_etherHistoryOversizePkts */

#ifdef I_etherHistoryFragments
      case I_etherHistoryFragments:
	dp = &data->etherHistoryFragments;
	break;
#endif				/* I_etherHistoryFragments */

#ifdef I_etherHistoryJabbers
      case I_etherHistoryJabbers:
	dp = &data->etherHistoryJabbers;
	break;
#endif				/* I_etherHistoryJabbers */

#ifdef I_etherHistoryCollisions
      case I_etherHistoryCollisions:
	dp = &data->etherHistoryCollisions;
	break;
#endif				/* I_etherHistoryCollisions */

#ifdef I_etherHistoryUtilization
      case I_etherHistoryUtilization:
	dp = &data->etherHistoryUtilization;
	break;
#endif				/* I_etherHistoryUtilization */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}
