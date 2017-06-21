/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_host.c,v 3.2 1995/11/17 18:40:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_host.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_host.c,v $
 * Revision 3.2  1995/11/17  18:40:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:46  shaker
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
 * Retrieve data from the hostControlEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
hostControlEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            hostControlIndex;

#ifdef hostControlEntry_DISABLE_CACHE
    hostControlEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* hostControlEntry_DISABLE_CACHE */
    static hostControlEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* hostControlEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &hostControlIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostControlIndex = MAX(1, hostControlIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_hostControlEntry_get(serialNum, contextInfo, arg, searchType, hostControlIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->hostControlIndex;
	}

#ifndef hostControlEntry_DISABLE_CACHE
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
#endif				/* hostControlEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_hostControlIndex
      case I_hostControlIndex:
	dp = &data->hostControlIndex;
	break;
#endif				/* I_hostControlIndex */

#ifdef I_hostControlDataSource
      case I_hostControlDataSource:
	dp = MakeOID(data->hostControlDataSource->oid_ptr, data->hostControlDataSource->length);
	break;
#endif				/* I_hostControlDataSource */

#ifdef I_hostControlTableSize
      case I_hostControlTableSize:
	dp = &data->hostControlTableSize;
	break;
#endif				/* I_hostControlTableSize */

#ifdef I_hostControlLastDeleteTime
      case I_hostControlLastDeleteTime:
	dp = &data->hostControlLastDeleteTime;
	break;
#endif				/* I_hostControlLastDeleteTime */

#ifdef I_hostControlOwner
      case I_hostControlOwner:
	dp = MakeOctetString(data->hostControlOwner->octet_ptr, data->hostControlOwner->length);
	break;
#endif				/* I_hostControlOwner */

#ifdef I_hostControlStatus
      case I_hostControlStatus:
	dp = &data->hostControlStatus;
	break;
#endif				/* I_hostControlStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the hostControlEntry data object.
 *---------------------------------------------------------------------*/
void
hostControlEntry_free(data)
    hostControlEntry_t *data;
{
    if (data != NULL) {
	k_hostControlEntryFreeUserpartData(data);
	FreeOID(data->hostControlDataSource);
	FreeOctetString(data->hostControlOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after hostControlEntry set/undo
 *---------------------------------------------------------------------*/
static int
hostControlEntry_cleanup(
    doList_t       *trash)
{
    hostControlEntry_free(trash->data);

#ifdef SR_SNMPv2
    hostControlEntry_free(trash->undodata);
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
hostControlEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    hostControlEntry_t *hostControlEntry;
    long            hostControlIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &hostControlIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    hostControlEntry = k_hostControlEntry_get(-1, contextInfo, object->nominator, EXACT, hostControlIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == hostControlEntry_set) &&
	    (((hostControlEntry_t *) (dp->data)) != NULL) &&
	    (((hostControlEntry_t *) (dp->data))->hostControlIndex == hostControlIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(hostControlEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(hostControlEntry_t));

	dp->setMethod = hostControlEntry_set;
	dp->cleanupMethod = hostControlEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = hostControlEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (hostControlEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(hostControlEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (hostControlEntry), sizeof(hostControlEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (hostControlEntry), sizeof(hostControlEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((hostControlEntry_t *) (dp->data))->valid);

	    ((hostControlEntry_t *) (dp->data))->hostControlDataSource =
		CloneOID(hostControlEntry->hostControlDataSource);

#ifdef SR_SNMPv2
	    ((hostControlEntry_t *) (dp->undodata))->hostControlDataSource =
		CloneOID(hostControlEntry->hostControlDataSource);
#endif				/* SR_SNMPv2 */

	    ((hostControlEntry_t *) (dp->data))->hostControlOwner =
		CloneOctetString(hostControlEntry->hostControlOwner);

#ifdef SR_SNMPv2
	    ((hostControlEntry_t *) (dp->undodata))->hostControlOwner =
		CloneOctetString(hostControlEntry->hostControlOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef hostControlEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((hostControlEntry_t *) (dp->data))->hostControlIndex = hostControlIndex;
	    SET_VALID(I_hostControlIndex, ((hostControlEntry_t *) (dp->data))->valid);

	    error_status = k_hostControlEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* hostControlEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* hostControlEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_hostControlDataSource
      case I_hostControlDataSource:

	if (((hostControlEntry_t *) (dp->data))->hostControlDataSource != NULL) {
	    FreeOID(((hostControlEntry_t *) (dp->data))->hostControlDataSource);
	}

	((hostControlEntry_t *) (dp->data))->hostControlDataSource =
	    MakeOID(value->oid_value->oid_ptr, value->oid_value->length);

	break;
#endif				/* I_hostControlDataSource */

#ifdef I_hostControlOwner
      case I_hostControlOwner:

	if (((hostControlEntry_t *) (dp->data))->hostControlOwner != NULL) {
	    FreeOctetString(((hostControlEntry_t *) (dp->data))->hostControlOwner);
	}

	((hostControlEntry_t *) (dp->data))->hostControlOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_hostControlOwner */

#ifdef I_hostControlStatus
      case I_hostControlStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
	hostControlEntry == NULL ? 0L : hostControlEntry->hostControlStatus);
	if (error_status != 0)
	    return (error_status);

	((hostControlEntry_t *) (dp->data))->hostControlStatus = value->sl_value;
	break;
#endif				/* I_hostControlStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in hostControlEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_hostControlEntry_test */
    error_status = k_hostControlEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((hostControlEntry_t *) (dp->data))->valid);
	error_status = k_hostControlEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
hostControlEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_hostControlEntry_set((hostControlEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the hostEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
hostEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    int             carry;
    int             i;
    long            hostIndex;
    int             hostIndex_offset;
    OctetString    *hostAddress;
    int             hostAddress_offset;
    int             index;
    int             final_index;

#ifdef hostEntry_DISABLE_CACHE
    hostEntry_t    *data;
    unsigned long   buffer[MAXBUFSIZE];
    OID             inst;
#else				/* hostEntry_DISABLE_CACHE */
    static hostEntry_t *data;
    static unsigned long buffer[MAXBUFSIZE];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* hostEntry_DISABLE_CACHE */

	hostIndex_offset = object->oid.length;
	hostAddress_offset = hostIndex_offset + 1;
	final_index = hostAddress_offset + GetVariableIndexLength(incoming, hostAddress_offset);

	if (searchType == EXACT) {
	    if (final_index != incoming->length) {
		return ((VarBind *) NULL);
	    }
	    carry = 0;
	}
	else {
	    carry = 1;
	}

	if ((InstToVariableOctetString(incoming, hostAddress_offset, &hostAddress, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToInt(incoming, hostIndex_offset, &hostIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostIndex = MAX(1, hostIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_hostEntry_get(serialNum, contextInfo, arg, searchType, hostIndex, hostAddress)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    index = 0;
	    inst.oid_ptr[index++] = (unsigned long) data->hostIndex;
	    inst.oid_ptr[index++] = data->hostAddress->length;
	    for (i = 0; i < data->hostAddress->length; i++) {
		inst.oid_ptr[index++] = (unsigned long) data->hostAddress->octet_ptr[i];
	    }

	    inst.length = index;
	}

	if (hostAddress != NULL) {
	    FreeOctetString(hostAddress);
	}

#ifndef hostEntry_DISABLE_CACHE
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
#endif				/* hostEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_hostAddress
      case I_hostAddress:
	dp = MakeOctetString(data->hostAddress->octet_ptr, data->hostAddress->length);
	break;
#endif				/* I_hostAddress */

#ifdef I_hostCreationOrder
      case I_hostCreationOrder:
	dp = &data->hostCreationOrder;
	break;
#endif				/* I_hostCreationOrder */

#ifdef I_hostIndex
      case I_hostIndex:
	dp = &data->hostIndex;
	break;
#endif				/* I_hostIndex */

#ifdef I_hostInPkts
      case I_hostInPkts:
	dp = &data->hostInPkts;
	break;
#endif				/* I_hostInPkts */

#ifdef I_hostOutPkts
      case I_hostOutPkts:
	dp = &data->hostOutPkts;
	break;
#endif				/* I_hostOutPkts */

#ifdef I_hostInOctets
      case I_hostInOctets:
	dp = &data->hostInOctets;
	break;
#endif				/* I_hostInOctets */

#ifdef I_hostOutOctets
      case I_hostOutOctets:
	dp = &data->hostOutOctets;
	break;
#endif				/* I_hostOutOctets */

#ifdef I_hostOutErrors
      case I_hostOutErrors:
	dp = &data->hostOutErrors;
	break;
#endif				/* I_hostOutErrors */

#ifdef I_hostOutBroadcastPkts
      case I_hostOutBroadcastPkts:
	dp = &data->hostOutBroadcastPkts;
	break;
#endif				/* I_hostOutBroadcastPkts */

#ifdef I_hostOutMulticastPkts
      case I_hostOutMulticastPkts:
	dp = &data->hostOutMulticastPkts;
	break;
#endif				/* I_hostOutMulticastPkts */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the hostTimeEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
hostTimeEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            hostTimeIndex;
    long            hostTimeCreationOrder;

#ifdef hostTimeEntry_DISABLE_CACHE
    hostTimeEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
#else				/* hostTimeEntry_DISABLE_CACHE */
    static hostTimeEntry_t *data;
    static unsigned long buffer[2];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* hostTimeEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 1 + object->oid.length, &hostTimeCreationOrder, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostTimeCreationOrder = MAX(1, hostTimeCreationOrder);
	}

	if ((InstToInt(incoming, 0 + object->oid.length, &hostTimeIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostTimeIndex = MAX(1, hostTimeIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_hostTimeEntry_get(serialNum, contextInfo, arg, searchType, hostTimeIndex, hostTimeCreationOrder)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 2;
	    inst.oid_ptr[0] = data->hostTimeIndex;
	    inst.oid_ptr[1] = data->hostTimeCreationOrder;
	}

#ifndef hostTimeEntry_DISABLE_CACHE
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
#endif				/* hostTimeEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_hostTimeAddress
      case I_hostTimeAddress:
	dp = MakeOctetString(data->hostTimeAddress->octet_ptr, data->hostTimeAddress->length);
	break;
#endif				/* I_hostTimeAddress */

#ifdef I_hostTimeCreationOrder
      case I_hostTimeCreationOrder:
	dp = &data->hostTimeCreationOrder;
	break;
#endif				/* I_hostTimeCreationOrder */

#ifdef I_hostTimeIndex
      case I_hostTimeIndex:
	dp = &data->hostTimeIndex;
	break;
#endif				/* I_hostTimeIndex */

#ifdef I_hostTimeInPkts
      case I_hostTimeInPkts:
	dp = &data->hostTimeInPkts;
	break;
#endif				/* I_hostTimeInPkts */

#ifdef I_hostTimeOutPkts
      case I_hostTimeOutPkts:
	dp = &data->hostTimeOutPkts;
	break;
#endif				/* I_hostTimeOutPkts */

#ifdef I_hostTimeInOctets
      case I_hostTimeInOctets:
	dp = &data->hostTimeInOctets;
	break;
#endif				/* I_hostTimeInOctets */

#ifdef I_hostTimeOutOctets
      case I_hostTimeOutOctets:
	dp = &data->hostTimeOutOctets;
	break;
#endif				/* I_hostTimeOutOctets */

#ifdef I_hostTimeOutErrors
      case I_hostTimeOutErrors:
	dp = &data->hostTimeOutErrors;
	break;
#endif				/* I_hostTimeOutErrors */

#ifdef I_hostTimeOutBroadcastPkts
      case I_hostTimeOutBroadcastPkts:
	dp = &data->hostTimeOutBroadcastPkts;
	break;
#endif				/* I_hostTimeOutBroadcastPkts */

#ifdef I_hostTimeOutMulticastPkts
      case I_hostTimeOutMulticastPkts:
	dp = &data->hostTimeOutMulticastPkts;
	break;
#endif				/* I_hostTimeOutMulticastPkts */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}
