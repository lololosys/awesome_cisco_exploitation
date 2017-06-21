/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_filt.c,v 3.2 1995/11/17 18:40:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_filt.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_filt.c,v $
 * Revision 3.2  1995/11/17  18:40:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:43  shaker
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

extern void k_channelEntryFreeUserpartData(channelEntry_t *data);

#define MAXBUFSIZE 32

/*---------------------------------------------------------------------
 * Retrieve data from the filterEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
filterEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            filterIndex;

#ifdef filterEntry_DISABLE_CACHE
    filterEntry_t  *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* filterEntry_DISABLE_CACHE */
    static filterEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* filterEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &filterIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    filterIndex = MAX(1, filterIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_filterEntry_get(serialNum, contextInfo, arg, searchType, filterIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->filterIndex;
	}

#ifndef filterEntry_DISABLE_CACHE
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
#endif				/* filterEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_filterIndex
      case I_filterIndex:
	dp = &data->filterIndex;
	break;
#endif				/* I_filterIndex */

#ifdef I_filterChannelIndex
      case I_filterChannelIndex:
	dp = &data->filterChannelIndex;
	break;
#endif				/* I_filterChannelIndex */

#ifdef I_filterPktDataOffset
      case I_filterPktDataOffset:
	dp = &data->filterPktDataOffset;
	break;
#endif				/* I_filterPktDataOffset */

#ifdef I_filterPktData
      case I_filterPktData:
	dp = MakeOctetString(data->filterPktData->octet_ptr, data->filterPktData->length);
	break;
#endif				/* I_filterPktData */

#ifdef I_filterPktDataMask
      case I_filterPktDataMask:
	dp = MakeOctetString(data->filterPktDataMask->octet_ptr, data->filterPktDataMask->length);
	break;
#endif				/* I_filterPktDataMask */

#ifdef I_filterPktDataNotMask
      case I_filterPktDataNotMask:
	dp = MakeOctetString(data->filterPktDataNotMask->octet_ptr, data->filterPktDataNotMask->length);
	break;
#endif				/* I_filterPktDataNotMask */

#ifdef I_filterPktStatus
      case I_filterPktStatus:
	dp = &data->filterPktStatus;
	break;
#endif				/* I_filterPktStatus */

#ifdef I_filterPktStatusMask
      case I_filterPktStatusMask:
	dp = &data->filterPktStatusMask;
	break;
#endif				/* I_filterPktStatusMask */

#ifdef I_filterPktStatusNotMask
      case I_filterPktStatusNotMask:
	dp = &data->filterPktStatusNotMask;
	break;
#endif				/* I_filterPktStatusNotMask */

#ifdef I_filterOwner
      case I_filterOwner:
	dp = MakeOctetString(data->filterOwner->octet_ptr, data->filterOwner->length);
	break;
#endif				/* I_filterOwner */

#ifdef I_filterStatus
      case I_filterStatus:
	dp = &data->filterStatus;
	break;
#endif				/* I_filterStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the filterEntry data object.
 *---------------------------------------------------------------------*/
void
filterEntry_free(data)
    filterEntry_t  *data;
{
    if (data != NULL) {
	k_filterEntryFreeUserpartData(data);
	FreeOctetString(data->filterPktData);
	FreeOctetString(data->filterPktDataMask);
	FreeOctetString(data->filterPktDataNotMask);
	FreeOctetString(data->filterOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after filterEntry set/undo
 *---------------------------------------------------------------------*/
static int
filterEntry_cleanup(
    doList_t       *trash)
{
    filterEntry_free(trash->data);

#ifdef SR_SNMPv2
    filterEntry_free(trash->undodata);
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
filterEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    filterEntry_t  *filterEntry;
    long            filterIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &filterIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    filterEntry = k_filterEntry_get(-1, contextInfo, object->nominator, EXACT, filterIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == filterEntry_set) &&
	    (((filterEntry_t *) (dp->data)) != NULL) &&
	    (((filterEntry_t *) (dp->data))->filterIndex == filterIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(filterEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(filterEntry_t));

	dp->setMethod = filterEntry_set;
	dp->cleanupMethod = filterEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = filterEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (filterEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(filterEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (filterEntry), sizeof(filterEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (filterEntry), sizeof(filterEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((filterEntry_t *) (dp->data))->valid);

	    ((filterEntry_t *) (dp->data))->filterPktData =
		CloneOctetString(filterEntry->filterPktData);

#ifdef SR_SNMPv2
	    ((filterEntry_t *) (dp->undodata))->filterPktData =
		CloneOctetString(filterEntry->filterPktData);
#endif				/* SR_SNMPv2 */

	    ((filterEntry_t *) (dp->data))->filterPktDataMask =
		CloneOctetString(filterEntry->filterPktDataMask);

#ifdef SR_SNMPv2
	    ((filterEntry_t *) (dp->undodata))->filterPktDataMask =
		CloneOctetString(filterEntry->filterPktDataMask);
#endif				/* SR_SNMPv2 */

	    ((filterEntry_t *) (dp->data))->filterPktDataNotMask =
		CloneOctetString(filterEntry->filterPktDataNotMask);

#ifdef SR_SNMPv2
	    ((filterEntry_t *) (dp->undodata))->filterPktDataNotMask =
		CloneOctetString(filterEntry->filterPktDataNotMask);
#endif				/* SR_SNMPv2 */

	    ((filterEntry_t *) (dp->data))->filterOwner =
		CloneOctetString(filterEntry->filterOwner);

#ifdef SR_SNMPv2
	    ((filterEntry_t *) (dp->undodata))->filterOwner =
		CloneOctetString(filterEntry->filterOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef filterEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((filterEntry_t *) (dp->data))->filterIndex = filterIndex;
	    SET_VALID(I_filterIndex, ((filterEntry_t *) (dp->data))->valid);

	    error_status = k_filterEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* filterEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* filterEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_filterChannelIndex
      case I_filterChannelIndex:

	if (((value->sl_value < 1) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((filterEntry_t *) (dp->data))->filterChannelIndex = value->sl_value;
	break;
#endif				/* I_filterChannelIndex */

#ifdef I_filterPktDataOffset
      case I_filterPktDataOffset:

	((filterEntry_t *) (dp->data))->filterPktDataOffset = value->sl_value;
	break;
#endif				/* I_filterPktDataOffset */

#ifdef I_filterPktData
      case I_filterPktData:

	if (((filterEntry_t *) (dp->data))->filterPktData != NULL) {
	    FreeOctetString(((filterEntry_t *) (dp->data))->filterPktData);
	}

	((filterEntry_t *) (dp->data))->filterPktData =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_filterPktData */

#ifdef I_filterPktDataMask
      case I_filterPktDataMask:

	if (((filterEntry_t *) (dp->data))->filterPktDataMask != NULL) {
	    FreeOctetString(((filterEntry_t *) (dp->data))->filterPktDataMask);
	}

	((filterEntry_t *) (dp->data))->filterPktDataMask =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_filterPktDataMask */

#ifdef I_filterPktDataNotMask
      case I_filterPktDataNotMask:

	if (((filterEntry_t *) (dp->data))->filterPktDataNotMask != NULL) {
	    FreeOctetString(((filterEntry_t *) (dp->data))->filterPktDataNotMask);
	}

	((filterEntry_t *) (dp->data))->filterPktDataNotMask =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_filterPktDataNotMask */

#ifdef I_filterPktStatus
      case I_filterPktStatus:

	((filterEntry_t *) (dp->data))->filterPktStatus = value->sl_value;
	break;
#endif				/* I_filterPktStatus */

#ifdef I_filterPktStatusMask
      case I_filterPktStatusMask:

	((filterEntry_t *) (dp->data))->filterPktStatusMask = value->sl_value;
	break;
#endif				/* I_filterPktStatusMask */

#ifdef I_filterPktStatusNotMask
      case I_filterPktStatusNotMask:

	((filterEntry_t *) (dp->data))->filterPktStatusNotMask = value->sl_value;
	break;
#endif				/* I_filterPktStatusNotMask */

#ifdef I_filterOwner
      case I_filterOwner:

	if (((filterEntry_t *) (dp->data))->filterOwner != NULL) {
	    FreeOctetString(((filterEntry_t *) (dp->data))->filterOwner);
	}

	((filterEntry_t *) (dp->data))->filterOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_filterOwner */

#ifdef I_filterStatus
      case I_filterStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
		      filterEntry == NULL ? 0L : filterEntry->filterStatus);
	if (error_status != 0)
	    return (error_status);

	((filterEntry_t *) (dp->data))->filterStatus = value->sl_value;
	break;
#endif				/* I_filterStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in filterEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_filterEntry_test */
    error_status = k_filterEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((filterEntry_t *) (dp->data))->valid);
	error_status = k_filterEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
filterEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_filterEntry_set((filterEntry_t *) (doCur->data),
			      contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the channelEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
channelEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            channelIndex;

#ifdef channelEntry_DISABLE_CACHE
    channelEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* channelEntry_DISABLE_CACHE */
    static channelEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* channelEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &channelIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    channelIndex = MAX(1, channelIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_channelEntry_get(serialNum, contextInfo, arg, searchType, channelIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->channelIndex;
	}

#ifndef channelEntry_DISABLE_CACHE
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
#endif				/* channelEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_channelIndex
      case I_channelIndex:
	dp = &data->channelIndex;
	break;
#endif				/* I_channelIndex */

#ifdef I_channelIfIndex
      case I_channelIfIndex:
	dp = &data->channelIfIndex;
	break;
#endif				/* I_channelIfIndex */

#ifdef I_channelAcceptType
      case I_channelAcceptType:
	dp = &data->channelAcceptType;
	break;
#endif				/* I_channelAcceptType */

#ifdef I_channelDataControl
      case I_channelDataControl:
	dp = &data->channelDataControl;
	break;
#endif				/* I_channelDataControl */

#ifdef I_channelTurnOnEventIndex
      case I_channelTurnOnEventIndex:
	dp = &data->channelTurnOnEventIndex;
	break;
#endif				/* I_channelTurnOnEventIndex */

#ifdef I_channelTurnOffEventIndex
      case I_channelTurnOffEventIndex:
	dp = &data->channelTurnOffEventIndex;
	break;
#endif				/* I_channelTurnOffEventIndex */

#ifdef I_channelEventIndex
      case I_channelEventIndex:
	dp = &data->channelEventIndex;
	break;
#endif				/* I_channelEventIndex */

#ifdef I_channelEventStatus
      case I_channelEventStatus:
	dp = &data->channelEventStatus;
	break;
#endif				/* I_channelEventStatus */

#ifdef I_channelMatches
      case I_channelMatches:
	dp = &data->channelMatches;
	break;
#endif				/* I_channelMatches */

#ifdef I_channelDescription
      case I_channelDescription:
	dp = MakeOctetString(data->channelDescription->octet_ptr, data->channelDescription->length);
	break;
#endif				/* I_channelDescription */

#ifdef I_channelOwner
      case I_channelOwner:
	dp = MakeOctetString(data->channelOwner->octet_ptr, data->channelOwner->length);
	break;
#endif				/* I_channelOwner */

#ifdef I_channelStatus
      case I_channelStatus:
	dp = &data->channelStatus;
	break;
#endif				/* I_channelStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the channelEntry data object.
 *---------------------------------------------------------------------*/
void
channelEntry_free(data)
    channelEntry_t *data;
{
    if (data != NULL) {
	k_channelEntryFreeUserpartData(data);
	FreeOctetString(data->channelDescription);
	FreeOctetString(data->channelOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after channelEntry set/undo
 *---------------------------------------------------------------------*/
static int
channelEntry_cleanup(
    doList_t       *trash)
{
    channelEntry_free(trash->data);

#ifdef SR_SNMPv2
    channelEntry_free(trash->undodata);
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
channelEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    channelEntry_t *channelEntry;
    long            channelIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &channelIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    channelEntry = k_channelEntry_get(-1, contextInfo, object->nominator, EXACT, channelIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == channelEntry_set) &&
	    (((channelEntry_t *) (dp->data)) != NULL) &&
	  (((channelEntry_t *) (dp->data))->channelIndex == channelIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(channelEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(channelEntry_t));

	dp->setMethod = channelEntry_set;
	dp->cleanupMethod = channelEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = channelEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (channelEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(channelEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (channelEntry), sizeof(channelEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (channelEntry), sizeof(channelEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((channelEntry_t *) (dp->data))->valid);

	    ((channelEntry_t *) (dp->data))->channelDescription =
		CloneOctetString(channelEntry->channelDescription);

#ifdef SR_SNMPv2
	    ((channelEntry_t *) (dp->undodata))->channelDescription =
		CloneOctetString(channelEntry->channelDescription);
#endif				/* SR_SNMPv2 */

	    ((channelEntry_t *) (dp->data))->channelOwner =
		CloneOctetString(channelEntry->channelOwner);

#ifdef SR_SNMPv2
	    ((channelEntry_t *) (dp->undodata))->channelOwner =
		CloneOctetString(channelEntry->channelOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef channelEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((channelEntry_t *) (dp->data))->channelIndex = channelIndex;
	    SET_VALID(I_channelIndex, ((channelEntry_t *) (dp->data))->valid);

	    error_status = k_channelEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* channelEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* channelEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_channelIfIndex
      case I_channelIfIndex:

	if (((value->sl_value < 1) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelIfIndex = value->sl_value;
	break;
#endif				/* I_channelIfIndex */

#ifdef I_channelAcceptType
      case I_channelAcceptType:

	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelAcceptType = value->sl_value;
	break;
#endif				/* I_channelAcceptType */

#ifdef I_channelDataControl
      case I_channelDataControl:

	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelDataControl = value->sl_value;
	break;
#endif				/* I_channelDataControl */

#ifdef I_channelTurnOnEventIndex
      case I_channelTurnOnEventIndex:

	if (((value->sl_value < 0) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelTurnOnEventIndex = value->sl_value;
	break;
#endif				/* I_channelTurnOnEventIndex */

#ifdef I_channelTurnOffEventIndex
      case I_channelTurnOffEventIndex:

	if (((value->sl_value < 0) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelTurnOffEventIndex = value->sl_value;
	break;
#endif				/* I_channelTurnOffEventIndex */

#ifdef I_channelEventIndex
      case I_channelEventIndex:

	if (((value->sl_value < 0) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelEventIndex = value->sl_value;
	break;
#endif				/* I_channelEventIndex */

#ifdef I_channelEventStatus
      case I_channelEventStatus:

	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}

	((channelEntry_t *) (dp->data))->channelEventStatus = value->sl_value;
	break;
#endif				/* I_channelEventStatus */

#ifdef I_channelDescription
      case I_channelDescription:

	if (((channelEntry_t *) (dp->data))->channelDescription != NULL) {
	    FreeOctetString(((channelEntry_t *) (dp->data))->channelDescription);
	}

	((channelEntry_t *) (dp->data))->channelDescription =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_channelDescription */

#ifdef I_channelOwner
      case I_channelOwner:

	if (((channelEntry_t *) (dp->data))->channelOwner != NULL) {
	    FreeOctetString(((channelEntry_t *) (dp->data))->channelOwner);
	}

	((channelEntry_t *) (dp->data))->channelOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_channelOwner */

#ifdef I_channelStatus
      case I_channelStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
		   channelEntry == NULL ? 0L : channelEntry->channelStatus);
	if (error_status != 0)
	    return (error_status);

	((channelEntry_t *) (dp->data))->channelStatus = value->sl_value;
	break;
#endif				/* I_channelStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in channelEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_channelEntry_test */
    error_status = k_channelEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((channelEntry_t *) (dp->data))->valid);
	error_status = k_channelEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
channelEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_channelEntry_set((channelEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				/* SETS */
