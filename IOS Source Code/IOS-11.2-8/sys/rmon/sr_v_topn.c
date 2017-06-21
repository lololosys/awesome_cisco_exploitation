/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_topn.c,v 3.2 1995/11/17 18:40:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_topn.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_topn.c,v $
 * Revision 3.2  1995/11/17  18:40:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:50  shaker
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
 * Retrieve data from the hostTopNControlEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
hostTopNControlEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            hostTopNControlIndex;

#ifdef hostTopNControlEntry_DISABLE_CACHE
    hostTopNControlEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* hostTopNControlEntry_DISABLE_CACHE */
    static hostTopNControlEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* hostTopNControlEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &hostTopNControlIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostTopNControlIndex = MAX(1, hostTopNControlIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_hostTopNControlEntry_get(serialNum, contextInfo, arg, searchType, hostTopNControlIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->hostTopNControlIndex;
	}

#ifndef hostTopNControlEntry_DISABLE_CACHE
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
#endif				/* hostTopNControlEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_hostTopNControlIndex
      case I_hostTopNControlIndex:
	dp = &data->hostTopNControlIndex;
	break;
#endif				/* I_hostTopNControlIndex */

#ifdef I_hostTopNHostIndex
      case I_hostTopNHostIndex:
	dp = &data->hostTopNHostIndex;
	break;
#endif				/* I_hostTopNHostIndex */

#ifdef I_hostTopNRateBase
      case I_hostTopNRateBase:
	dp = &data->hostTopNRateBase;
	break;
#endif				/* I_hostTopNRateBase */

#ifdef I_hostTopNTimeRemaining
      case I_hostTopNTimeRemaining:
	dp = &data->hostTopNTimeRemaining;
	break;
#endif				/* I_hostTopNTimeRemaining */

#ifdef I_hostTopNDuration
      case I_hostTopNDuration:
	dp = &data->hostTopNDuration;
	break;
#endif				/* I_hostTopNDuration */

#ifdef I_hostTopNRequestedSize
      case I_hostTopNRequestedSize:
	dp = &data->hostTopNRequestedSize;
	break;
#endif				/* I_hostTopNRequestedSize */

#ifdef I_hostTopNGrantedSize
      case I_hostTopNGrantedSize:
	dp = &data->hostTopNGrantedSize;
	break;
#endif				/* I_hostTopNGrantedSize */

#ifdef I_hostTopNStartTime
      case I_hostTopNStartTime:
	dp = &data->hostTopNStartTime;
	break;
#endif				/* I_hostTopNStartTime */

#ifdef I_hostTopNOwner
      case I_hostTopNOwner:
	dp = MakeOctetString(data->hostTopNOwner->octet_ptr, data->hostTopNOwner->length);
	break;
#endif				/* I_hostTopNOwner */

#ifdef I_hostTopNStatus
      case I_hostTopNStatus:
	dp = &data->hostTopNStatus;
	break;
#endif				/* I_hostTopNStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the hostTopNControlEntry data object.
 *---------------------------------------------------------------------*/
void
hostTopNControlEntry_free(data)
    hostTopNControlEntry_t *data;
{
    if (data != NULL) {
	k_hostTopNControlEntryFreeUserpartData(data);
	FreeOctetString(data->hostTopNOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after hostTopNControlEntry set/undo
 *---------------------------------------------------------------------*/
static int
hostTopNControlEntry_cleanup(
    doList_t       *trash)
{
    hostTopNControlEntry_free(trash->data);

#ifdef SR_SNMPv2
    hostTopNControlEntry_free(trash->undodata);
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
hostTopNControlEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    hostTopNControlEntry_t *hostTopNControlEntry;
    long            hostTopNControlIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &hostTopNControlIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    hostTopNControlEntry = k_hostTopNControlEntry_get(-1, contextInfo, object->nominator, EXACT, hostTopNControlIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == hostTopNControlEntry_set) &&
	    (((hostTopNControlEntry_t *) (dp->data)) != NULL) &&
	    (((hostTopNControlEntry_t *) (dp->data))->hostTopNControlIndex == hostTopNControlIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(hostTopNControlEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(hostTopNControlEntry_t));

	dp->setMethod = hostTopNControlEntry_set;
	dp->cleanupMethod = hostTopNControlEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = hostTopNControlEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (hostTopNControlEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(hostTopNControlEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (hostTopNControlEntry), sizeof(hostTopNControlEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (hostTopNControlEntry), sizeof(hostTopNControlEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((hostTopNControlEntry_t *) (dp->data))->valid);

	    ((hostTopNControlEntry_t *) (dp->data))->hostTopNOwner =
		CloneOctetString(hostTopNControlEntry->hostTopNOwner);

#ifdef SR_SNMPv2
	    ((hostTopNControlEntry_t *) (dp->undodata))->hostTopNOwner =
		CloneOctetString(hostTopNControlEntry->hostTopNOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef hostTopNControlEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((hostTopNControlEntry_t *) (dp->data))->hostTopNControlIndex = hostTopNControlIndex;
	    SET_VALID(I_hostTopNControlIndex, ((hostTopNControlEntry_t *) (dp->data))->valid);

	    error_status = k_hostTopNControlEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* hostTopNControlEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* hostTopNControlEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_hostTopNHostIndex
      case I_hostTopNHostIndex:

	if (((value->sl_value < 1) || (value->sl_value > 65535))) {
	    return WRONG_VALUE_ERROR;
	}

	((hostTopNControlEntry_t *) (dp->data))->hostTopNHostIndex = value->sl_value;
	break;
#endif				/* I_hostTopNHostIndex */

#ifdef I_hostTopNRateBase
      case I_hostTopNRateBase:

	if ((value->sl_value < 1) || (value->sl_value > 7)) {
	    return WRONG_VALUE_ERROR;
	}

	((hostTopNControlEntry_t *) (dp->data))->hostTopNRateBase = value->sl_value;
	break;
#endif				/* I_hostTopNRateBase */

#ifdef I_hostTopNTimeRemaining
      case I_hostTopNTimeRemaining:

	((hostTopNControlEntry_t *) (dp->data))->hostTopNTimeRemaining = value->sl_value;
	break;
#endif				/* I_hostTopNTimeRemaining */

#ifdef I_hostTopNRequestedSize
      case I_hostTopNRequestedSize:

	((hostTopNControlEntry_t *) (dp->data))->hostTopNRequestedSize = value->sl_value;
	break;
#endif				/* I_hostTopNRequestedSize */

#ifdef I_hostTopNOwner
      case I_hostTopNOwner:

	if (((hostTopNControlEntry_t *) (dp->data))->hostTopNOwner != NULL) {
	    FreeOctetString(((hostTopNControlEntry_t *) (dp->data))->hostTopNOwner);
	}

	((hostTopNControlEntry_t *) (dp->data))->hostTopNOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_hostTopNOwner */

#ifdef I_hostTopNStatus
      case I_hostTopNStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
					hostTopNControlEntry == NULL ? 0L : hostTopNControlEntry->hostTopNStatus);
	if (error_status != 0)
	    return (error_status);

	((hostTopNControlEntry_t *) (dp->data))->hostTopNStatus = value->sl_value;
	break;
#endif				/* I_hostTopNStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in hostTopNControlEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_hostTopNControlEntry_test */
    error_status = k_hostTopNControlEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((hostTopNControlEntry_t *) (dp->data))->valid);
	error_status = k_hostTopNControlEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
hostTopNControlEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_hostTopNControlEntry_set((hostTopNControlEntry_t *) (doCur->data),
				       contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the hostTopNEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
hostTopNEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            hostTopNReport;
    long            hostTopNIndex;

#ifdef hostTopNEntry_DISABLE_CACHE
    hostTopNEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
#else				/* hostTopNEntry_DISABLE_CACHE */
    static hostTopNEntry_t *data;
    static unsigned long buffer[2];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* hostTopNEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 1 + object->oid.length, &hostTopNIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostTopNIndex = MAX(1, hostTopNIndex);
	}

	if ((InstToInt(incoming, 0 + object->oid.length, &hostTopNReport, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    hostTopNReport = MAX(1, hostTopNReport);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_hostTopNEntry_get(serialNum, contextInfo, arg, searchType, hostTopNReport, hostTopNIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 2;
	    inst.oid_ptr[0] = data->hostTopNReport;
	    inst.oid_ptr[1] = data->hostTopNIndex;
	}

#ifndef hostTopNEntry_DISABLE_CACHE
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
#endif				/* hostTopNEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_hostTopNReport
      case I_hostTopNReport:
	dp = &data->hostTopNReport;
	break;
#endif				/* I_hostTopNReport */

#ifdef I_hostTopNIndex
      case I_hostTopNIndex:
	dp = &data->hostTopNIndex;
	break;
#endif				/* I_hostTopNIndex */

#ifdef I_hostTopNAddress
      case I_hostTopNAddress:
	dp = MakeOctetString(data->hostTopNAddress->octet_ptr, data->hostTopNAddress->length);
	break;
#endif				/* I_hostTopNAddress */

#ifdef I_hostTopNRate
      case I_hostTopNRate:
	dp = &data->hostTopNRate;
	break;
#endif				/* I_hostTopNRate */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}
