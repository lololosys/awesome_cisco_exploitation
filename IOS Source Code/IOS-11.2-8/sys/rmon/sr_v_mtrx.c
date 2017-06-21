/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_mtrx.c,v 3.2 1995/11/17 18:40:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_mtrx.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_mtrx.c,v $
 * Revision 3.2  1995/11/17  18:40:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:48  shaker
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
 * Retrieve data from the matrixControlEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
matrixControlEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            matrixControlIndex;

#ifdef matrixControlEntry_DISABLE_CACHE
    matrixControlEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* matrixControlEntry_DISABLE_CACHE */
    static matrixControlEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* matrixControlEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &matrixControlIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    matrixControlIndex = MAX(1, matrixControlIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_matrixControlEntry_get(serialNum, contextInfo, arg, searchType, matrixControlIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->matrixControlIndex;
	}

#ifndef matrixControlEntry_DISABLE_CACHE
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
#endif				/* matrixControlEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_matrixControlIndex
      case I_matrixControlIndex:
	dp = &data->matrixControlIndex;
	break;
#endif				/* I_matrixControlIndex */

#ifdef I_matrixControlDataSource
      case I_matrixControlDataSource:
	dp = MakeOID(data->matrixControlDataSource->oid_ptr, data->matrixControlDataSource->length);
	break;
#endif				/* I_matrixControlDataSource */

#ifdef I_matrixControlTableSize
      case I_matrixControlTableSize:
	dp = &data->matrixControlTableSize;
	break;
#endif				/* I_matrixControlTableSize */

#ifdef I_matrixControlLastDeleteTime
      case I_matrixControlLastDeleteTime:
	dp = &data->matrixControlLastDeleteTime;
	break;
#endif				/* I_matrixControlLastDeleteTime */

#ifdef I_matrixControlOwner
      case I_matrixControlOwner:
	dp = MakeOctetString(data->matrixControlOwner->octet_ptr, data->matrixControlOwner->length);
	break;
#endif				/* I_matrixControlOwner */

#ifdef I_matrixControlStatus
      case I_matrixControlStatus:
	dp = &data->matrixControlStatus;
	break;
#endif				/* I_matrixControlStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the matrixControlEntry data object.
 *---------------------------------------------------------------------*/
void
matrixControlEntry_free(data)
    matrixControlEntry_t *data;
{
    if (data != NULL) {
	k_matrixControlEntryFreeUserpartData(data);
	FreeOID(data->matrixControlDataSource);
	FreeOctetString(data->matrixControlOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after matrixControlEntry set/undo
 *---------------------------------------------------------------------*/
static int
matrixControlEntry_cleanup(
    doList_t       *trash)
{
    matrixControlEntry_free(trash->data);

#ifdef SR_SNMPv2
    matrixControlEntry_free(trash->undodata);
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
matrixControlEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    matrixControlEntry_t *matrixControlEntry;
    long            matrixControlIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &matrixControlIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    matrixControlEntry = k_matrixControlEntry_get(-1, contextInfo, object->nominator, EXACT, matrixControlIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == matrixControlEntry_set) &&
	    (((matrixControlEntry_t *) (dp->data)) != NULL) &&
	    (((matrixControlEntry_t *) (dp->data))->matrixControlIndex == matrixControlIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(matrixControlEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(matrixControlEntry_t));

	dp->setMethod = matrixControlEntry_set;
	dp->cleanupMethod = matrixControlEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = matrixControlEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (matrixControlEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(matrixControlEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (matrixControlEntry), sizeof(matrixControlEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (matrixControlEntry), sizeof(matrixControlEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((matrixControlEntry_t *) (dp->data))->valid);

	    ((matrixControlEntry_t *) (dp->data))->matrixControlDataSource =
		CloneOID(matrixControlEntry->matrixControlDataSource);

#ifdef SR_SNMPv2
	    ((matrixControlEntry_t *) (dp->undodata))->matrixControlDataSource =
		CloneOID(matrixControlEntry->matrixControlDataSource);
#endif				/* SR_SNMPv2 */

	    ((matrixControlEntry_t *) (dp->data))->matrixControlOwner =
		CloneOctetString(matrixControlEntry->matrixControlOwner);

#ifdef SR_SNMPv2
	    ((matrixControlEntry_t *) (dp->undodata))->matrixControlOwner =
		CloneOctetString(matrixControlEntry->matrixControlOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef matrixControlEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((matrixControlEntry_t *) (dp->data))->matrixControlIndex = matrixControlIndex;
	    SET_VALID(I_matrixControlIndex, ((matrixControlEntry_t *) (dp->data))->valid);

	    error_status = k_matrixControlEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* matrixControlEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* matrixControlEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_matrixControlDataSource
      case I_matrixControlDataSource:

	if (((matrixControlEntry_t *) (dp->data))->matrixControlDataSource != NULL) {
	    FreeOID(((matrixControlEntry_t *) (dp->data))->matrixControlDataSource);
	}

	((matrixControlEntry_t *) (dp->data))->matrixControlDataSource =
	    MakeOID(value->oid_value->oid_ptr, value->oid_value->length);

	break;
#endif				/* I_matrixControlDataSource */

#ifdef I_matrixControlOwner
      case I_matrixControlOwner:

	if (((matrixControlEntry_t *) (dp->data))->matrixControlOwner != NULL) {
	    FreeOctetString(((matrixControlEntry_t *) (dp->data))->matrixControlOwner);
	}

	((matrixControlEntry_t *) (dp->data))->matrixControlOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_matrixControlOwner */

#ifdef I_matrixControlStatus
      case I_matrixControlStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
					matrixControlEntry == NULL ? 0L : matrixControlEntry->matrixControlStatus);
	if (error_status != 0)
	    return (error_status);

	((matrixControlEntry_t *) (dp->data))->matrixControlStatus = value->sl_value;
	break;
#endif				/* I_matrixControlStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in matrixControlEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_matrixControlEntry_test */
    error_status = k_matrixControlEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((matrixControlEntry_t *) (dp->data))->valid);
	error_status = k_matrixControlEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
matrixControlEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_matrixControlEntry_set((matrixControlEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the matrixSDEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
matrixSDEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            matrixSDIndex;
    int             matrixSDIndex_offset;
    OctetString    *matrixSDSourceAddress;
    int             matrixSDSourceAddress_offset;
    OctetString    *matrixSDDestAddress;
    int             matrixSDDestAddress_offset;
    int             index;
    int             final_index;

#ifdef matrixSDEntry_DISABLE_CACHE
    matrixSDEntry_t *data;
    unsigned long   buffer[MAXBUFSIZE];
    OID             inst;
#else				/* matrixSDEntry_DISABLE_CACHE */
    static matrixSDEntry_t *data;
    static unsigned long buffer[MAXBUFSIZE];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* matrixSDEntry_DISABLE_CACHE */

	matrixSDIndex_offset = object->oid.length;
	matrixSDSourceAddress_offset = matrixSDIndex_offset + 1;
	matrixSDDestAddress_offset = matrixSDSourceAddress_offset + GetVariableIndexLength(incoming, matrixSDSourceAddress_offset);
	final_index = matrixSDDestAddress_offset + GetVariableIndexLength(incoming, matrixSDDestAddress_offset);

	if (searchType == EXACT) {
	    if (final_index != incoming->length) {
		return ((VarBind *) NULL);
	    }
	    carry = 0;
	}
	else {
	    carry = 1;
	}

	if ((InstToVariableOctetString(incoming, matrixSDDestAddress_offset, &matrixSDDestAddress, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToVariableOctetString(incoming, matrixSDSourceAddress_offset, &matrixSDSourceAddress, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToInt(incoming, matrixSDIndex_offset, &matrixSDIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    matrixSDIndex = MAX(1, matrixSDIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_matrixSDEntry_get(serialNum, contextInfo, arg, searchType, matrixSDIndex, matrixSDSourceAddress, matrixSDDestAddress)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    index = 0;
	    inst.oid_ptr[index++] = (unsigned long) data->matrixSDIndex;
	    inst.oid_ptr[index++] = data->matrixSDSourceAddress->length;
	    for (i = 0; i < data->matrixSDSourceAddress->length; i++) {
		inst.oid_ptr[index++] = (unsigned long) data->matrixSDSourceAddress->octet_ptr[i];
	    }

	    inst.oid_ptr[index++] = data->matrixSDDestAddress->length;
	    for (i = 0; i < data->matrixSDDestAddress->length; i++) {
		inst.oid_ptr[index++] = (unsigned long) data->matrixSDDestAddress->octet_ptr[i];
	    }

	    inst.length = index;
	}

	if (matrixSDSourceAddress != NULL) {
	    FreeOctetString(matrixSDSourceAddress);
	}

	if (matrixSDDestAddress != NULL) {
	    FreeOctetString(matrixSDDestAddress);
	}

#ifndef matrixSDEntry_DISABLE_CACHE
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
#endif				/* matrixSDEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_matrixSDSourceAddress
      case I_matrixSDSourceAddress:
	dp = MakeOctetString(data->matrixSDSourceAddress->octet_ptr, data->matrixSDSourceAddress->length);
	break;
#endif				/* I_matrixSDSourceAddress */

#ifdef I_matrixSDDestAddress
      case I_matrixSDDestAddress:
	dp = MakeOctetString(data->matrixSDDestAddress->octet_ptr, data->matrixSDDestAddress->length);
	break;
#endif				/* I_matrixSDDestAddress */

#ifdef I_matrixSDIndex
      case I_matrixSDIndex:
	dp = &data->matrixSDIndex;
	break;
#endif				/* I_matrixSDIndex */

#ifdef I_matrixSDPkts
      case I_matrixSDPkts:
	dp = &data->matrixSDPkts;
	break;
#endif				/* I_matrixSDPkts */

#ifdef I_matrixSDOctets
      case I_matrixSDOctets:
	dp = &data->matrixSDOctets;
	break;
#endif				/* I_matrixSDOctets */

#ifdef I_matrixSDErrors
      case I_matrixSDErrors:
	dp = &data->matrixSDErrors;
	break;
#endif				/* I_matrixSDErrors */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the matrixDSEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
matrixDSEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            matrixDSIndex;
    int             matrixDSIndex_offset;
    OctetString    *matrixDSDestAddress;
    int             matrixDSDestAddress_offset;
    OctetString    *matrixDSSourceAddress;
    int             matrixDSSourceAddress_offset;
    int             index;
    int             final_index;

#ifdef matrixDSEntry_DISABLE_CACHE
    matrixDSEntry_t *data;
    unsigned long   buffer[MAXBUFSIZE];
    OID             inst;
#else				/* matrixDSEntry_DISABLE_CACHE */
    static matrixDSEntry_t *data;
    static unsigned long buffer[MAXBUFSIZE];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* matrixDSEntry_DISABLE_CACHE */

	matrixDSIndex_offset = object->oid.length;
	matrixDSDestAddress_offset = matrixDSIndex_offset + 1;
	matrixDSSourceAddress_offset = matrixDSDestAddress_offset + GetVariableIndexLength(incoming, matrixDSDestAddress_offset);
	final_index = matrixDSSourceAddress_offset + GetVariableIndexLength(incoming, matrixDSSourceAddress_offset);

	if (searchType == EXACT) {
	    if (final_index != incoming->length) {
		return ((VarBind *) NULL);
	    }
	    carry = 0;
	}
	else {
	    carry = 1;
	}

	if ((InstToVariableOctetString(incoming, matrixDSSourceAddress_offset, &matrixDSSourceAddress, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToVariableOctetString(incoming, matrixDSDestAddress_offset, &matrixDSDestAddress, searchType, &carry)) < 0) {
	    arg = -1;
	}

	if ((InstToInt(incoming, matrixDSIndex_offset, &matrixDSIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    matrixDSIndex = MAX(1, matrixDSIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_matrixDSEntry_get(serialNum, contextInfo, arg, searchType, matrixDSIndex, matrixDSDestAddress, matrixDSSourceAddress)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    index = 0;
	    inst.oid_ptr[index++] = (unsigned long) data->matrixDSIndex;
	    inst.oid_ptr[index++] = data->matrixDSDestAddress->length;
	    for (i = 0; i < data->matrixDSDestAddress->length; i++) {
		inst.oid_ptr[index++] = (unsigned long) data->matrixDSDestAddress->octet_ptr[i];
	    }

	    inst.oid_ptr[index++] = data->matrixDSSourceAddress->length;
	    for (i = 0; i < data->matrixDSSourceAddress->length; i++) {
		inst.oid_ptr[index++] = (unsigned long) data->matrixDSSourceAddress->octet_ptr[i];
	    }

	    inst.length = index;
	}

	if (matrixDSDestAddress != NULL) {
	    FreeOctetString(matrixDSDestAddress);
	}

	if (matrixDSSourceAddress != NULL) {
	    FreeOctetString(matrixDSSourceAddress);
	}

#ifndef matrixDSEntry_DISABLE_CACHE
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
#endif				/* matrixDSEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_matrixDSSourceAddress
      case I_matrixDSSourceAddress:
	dp = MakeOctetString(data->matrixDSSourceAddress->octet_ptr, data->matrixDSSourceAddress->length);
	break;
#endif				/* I_matrixDSSourceAddress */

#ifdef I_matrixDSDestAddress
      case I_matrixDSDestAddress:
	dp = MakeOctetString(data->matrixDSDestAddress->octet_ptr, data->matrixDSDestAddress->length);
	break;
#endif				/* I_matrixDSDestAddress */

#ifdef I_matrixDSIndex
      case I_matrixDSIndex:
	dp = &data->matrixDSIndex;
	break;
#endif				/* I_matrixDSIndex */

#ifdef I_matrixDSPkts
      case I_matrixDSPkts:
	dp = &data->matrixDSPkts;
	break;
#endif				/* I_matrixDSPkts */

#ifdef I_matrixDSOctets
      case I_matrixDSOctets:
	dp = &data->matrixDSOctets;
	break;
#endif				/* I_matrixDSOctets */

#ifdef I_matrixDSErrors
      case I_matrixDSErrors:
	dp = &data->matrixDSErrors;
	break;
#endif				/* I_matrixDSErrors */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}
