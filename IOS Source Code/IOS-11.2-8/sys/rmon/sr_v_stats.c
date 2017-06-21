/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_v_stats.c,v 3.2 1995/11/17 18:40:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_v_stats.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_v_stats.c,v $
 * Revision 3.2  1995/11/17  18:40:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:49  shaker
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
 * Retrieve data from the etherStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
etherStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    long            etherStatsIndex;

#ifdef etherStatsEntry_DISABLE_CACHE
    etherStatsEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
#else				/* etherStatsEntry_DISABLE_CACHE */
    static etherStatsEntry_t *data;
    static unsigned long buffer[1];
    static OID      inst;
    static int      last_serialNum;
    static OID     *last_incoming;

    /* if the cache is not valid */
    if ((serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
	(CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||
	(!NEXT_ROW(data->valid)) ||
	(!VALID(object->nominator, data->valid))) {
#endif				/* etherStatsEntry_DISABLE_CACHE */

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

	if ((InstToInt(incoming, 0 + object->oid.length, &etherStatsIndex, searchType, &carry)) < 0) {
	    arg = -1;
	}
	if (searchType == NEXT) {
	    etherStatsIndex = MAX(1, etherStatsIndex);
	}

	if (carry) {
	    arg = -1;
	}

	/*
	 * Retrieve the data from the kernel-specific routine.
	 */
	if ((arg == -1) || (data = k_etherStatsEntry_get(serialNum, contextInfo, arg, searchType, etherStatsIndex)) == NULL) {
	    arg = -1;
	}

	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->etherStatsIndex;
	}

#ifndef etherStatsEntry_DISABLE_CACHE
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
#endif				/* etherStatsEntry_DISABLE_CACHE */

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_etherStatsIndex
      case I_etherStatsIndex:
	dp = &data->etherStatsIndex;
	break;
#endif				/* I_etherStatsIndex */

#ifdef I_etherStatsDataSource
      case I_etherStatsDataSource:
	dp = MakeOID(data->etherStatsDataSource->oid_ptr, data->etherStatsDataSource->length);
	break;
#endif				/* I_etherStatsDataSource */

#ifdef I_etherStatsDropEvents
      case I_etherStatsDropEvents:
	dp = &data->etherStatsDropEvents;
	break;
#endif				/* I_etherStatsDropEvents */

#ifdef I_etherStatsOctets
      case I_etherStatsOctets:
	dp = &data->etherStatsOctets;
	break;
#endif				/* I_etherStatsOctets */

#ifdef I_etherStatsPkts
      case I_etherStatsPkts:
	dp = &data->etherStatsPkts;
	break;
#endif				/* I_etherStatsPkts */

#ifdef I_etherStatsBroadcastPkts
      case I_etherStatsBroadcastPkts:
	dp = &data->etherStatsBroadcastPkts;
	break;
#endif				/* I_etherStatsBroadcastPkts */

#ifdef I_etherStatsMulticastPkts
      case I_etherStatsMulticastPkts:
	dp = &data->etherStatsMulticastPkts;
	break;
#endif				/* I_etherStatsMulticastPkts */

#ifdef I_etherStatsCRCAlignErrors
      case I_etherStatsCRCAlignErrors:
	dp = &data->etherStatsCRCAlignErrors;
	break;
#endif				/* I_etherStatsCRCAlignErrors */

#ifdef I_etherStatsUndersizePkts
      case I_etherStatsUndersizePkts:
	dp = &data->etherStatsUndersizePkts;
	break;
#endif				/* I_etherStatsUndersizePkts */

#ifdef I_etherStatsOversizePkts
      case I_etherStatsOversizePkts:
	dp = &data->etherStatsOversizePkts;
	break;
#endif				/* I_etherStatsOversizePkts */

#ifdef I_etherStatsFragments
      case I_etherStatsFragments:
	dp = &data->etherStatsFragments;
	break;
#endif				/* I_etherStatsFragments */

#ifdef I_etherStatsJabbers
      case I_etherStatsJabbers:
	dp = &data->etherStatsJabbers;
	break;
#endif				/* I_etherStatsJabbers */

#ifdef I_etherStatsCollisions
      case I_etherStatsCollisions:
	dp = &data->etherStatsCollisions;
	break;
#endif				/* I_etherStatsCollisions */

#ifdef I_etherStatsPkts64Octets
      case I_etherStatsPkts64Octets:
	dp = &data->etherStatsPkts64Octets;
	break;
#endif				/* I_etherStatsPkts64Octets */

#ifdef I_etherStatsPkts65to127Octets
      case I_etherStatsPkts65to127Octets:
	dp = &data->etherStatsPkts65to127Octets;
	break;
#endif				/* I_etherStatsPkts65to127Octets */

#ifdef I_etherStatsPkts128to255Octets
      case I_etherStatsPkts128to255Octets:
	dp = &data->etherStatsPkts128to255Octets;
	break;
#endif				/* I_etherStatsPkts128to255Octets */

#ifdef I_etherStatsPkts256to511Octets
      case I_etherStatsPkts256to511Octets:
	dp = &data->etherStatsPkts256to511Octets;
	break;
#endif				/* I_etherStatsPkts256to511Octets */

#ifdef I_etherStatsPkts512to1023Octets
      case I_etherStatsPkts512to1023Octets:
	dp = &data->etherStatsPkts512to1023Octets;
	break;
#endif				/* I_etherStatsPkts512to1023Octets */

#ifdef I_etherStatsPkts1024to1518Octets
      case I_etherStatsPkts1024to1518Octets:
	dp = &data->etherStatsPkts1024to1518Octets;
	break;
#endif				/* I_etherStatsPkts1024to1518Octets */

#ifdef I_etherStatsOwner
      case I_etherStatsOwner:
	dp = MakeOctetString(data->etherStatsOwner->octet_ptr, data->etherStatsOwner->length);
	break;
#endif				/* I_etherStatsOwner */

#ifdef I_etherStatsStatus
      case I_etherStatsStatus:
	dp = &data->etherStatsStatus;
	break;
#endif				/* I_etherStatsStatus */

      default:
	return ((VarBind *) NULL);

    }				/* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
/*----------------------------------------------------------------------
 * Free the etherStatsEntry data object.
 *---------------------------------------------------------------------*/
void
etherStatsEntry_free(data)
    etherStatsEntry_t *data;
{
    if (data != NULL) {
	k_etherStatsEntryFreeUserpartData(data);
	FreeOID(data->etherStatsDataSource);
	FreeOctetString(data->etherStatsOwner);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after etherStatsEntry set/undo
 *---------------------------------------------------------------------*/
static int
etherStatsEntry_cleanup(
    doList_t       *trash)
{
    etherStatsEntry_free(trash->data);

#ifdef SR_SNMPv2
    etherStatsEntry_free(trash->undodata);
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
etherStatsEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    etherStatsEntry_t *etherStatsEntry;
    long            etherStatsIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &etherStatsIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    etherStatsEntry = k_etherStatsEntry_get(-1, contextInfo, object->nominator, EXACT, etherStatsIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == etherStatsEntry_set) &&
	    (((etherStatsEntry_t *) (dp->data)) != NULL) &&
	    (((etherStatsEntry_t *) (dp->data))->etherStatsIndex == etherStatsIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = (void *) malloc(sizeof(etherStatsEntry_t))) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp->data, 0, sizeof(etherStatsEntry_t));

	dp->setMethod = etherStatsEntry_set;
	dp->cleanupMethod = etherStatsEntry_cleanup;

#ifdef SR_SNMPv2
	dp->undoMethod = etherStatsEntry_undo;
#endif				/* SR_SNMPv2 */

	dp->state = UNKNOWN;

	if (etherStatsEntry != NULL) {

#ifdef SR_SNMPv2
	    if ((dp->undodata = (void *) malloc(sizeof(etherStatsEntry_t))) == NULL) {
		return RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    /* fill in existing values */
	    memcpy((char *) (dp->data), (char *) (etherStatsEntry), sizeof(etherStatsEntry_t));

#ifdef SR_SNMPv2
	    memcpy((char *) (dp->undodata), (char *) (etherStatsEntry), sizeof(etherStatsEntry_t));
#endif				/* SR_SNMPv2 */

	    SET_ALL_VALID(((etherStatsEntry_t *) (dp->data))->valid);

	    ((etherStatsEntry_t *) (dp->data))->etherStatsDataSource =
		CloneOID(etherStatsEntry->etherStatsDataSource);

#ifdef SR_SNMPv2
	    ((etherStatsEntry_t *) (dp->undodata))->etherStatsDataSource =
		CloneOID(etherStatsEntry->etherStatsDataSource);
#endif				/* SR_SNMPv2 */

	    ((etherStatsEntry_t *) (dp->data))->etherStatsOwner =
		CloneOctetString(etherStatsEntry->etherStatsOwner);

#ifdef SR_SNMPv2
	    ((etherStatsEntry_t *) (dp->undodata))->etherStatsOwner =
		CloneOctetString(etherStatsEntry->etherStatsOwner);
#endif				/* SR_SNMPv2 */

	}
	else {

#ifdef etherStatsEntry_READ_CREATE
	    /* Try to fill in reasonable default values for this new entry */
	    ((etherStatsEntry_t *) (dp->data))->etherStatsIndex = etherStatsIndex;
	    SET_VALID(I_etherStatsIndex, ((etherStatsEntry_t *) (dp->data))->valid);

	    error_status = k_etherStatsEntry_set_defaults(dp);
	    if (error_status != NO_ERROR) {
		return error_status;
	    }
#else				/* etherStatsEntry_READ_CREATE */
	    return NO_ACCESS_ERROR;
#endif				/* etherStatsEntry_READ_CREATE */
	}
    }

    switch (object->nominator) {

#ifdef I_etherStatsDataSource
      case I_etherStatsDataSource:

	if (((etherStatsEntry_t *) (dp->data))->etherStatsDataSource != NULL) {
	    FreeOID(((etherStatsEntry_t *) (dp->data))->etherStatsDataSource);
	}

	((etherStatsEntry_t *) (dp->data))->etherStatsDataSource =
	    MakeOID(value->oid_value->oid_ptr, value->oid_value->length);

	break;
#endif				/* I_etherStatsDataSource */

#ifdef I_etherStatsOwner
      case I_etherStatsOwner:

	if (((etherStatsEntry_t *) (dp->data))->etherStatsOwner != NULL) {
	    FreeOctetString(((etherStatsEntry_t *) (dp->data))->etherStatsOwner);
	}

	((etherStatsEntry_t *) (dp->data))->etherStatsOwner =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				/* I_etherStatsOwner */

#ifdef I_etherStatsStatus
      case I_etherStatsStatus:

	if ((value->sl_value < 1) || (value->sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}

	error_status = CheckEntryStatus(value->sl_value,
	  etherStatsEntry == NULL ? 0L : etherStatsEntry->etherStatsStatus);
	if (error_status != 0)
	    return (error_status);

	((etherStatsEntry_t *) (dp->data))->etherStatsStatus = value->sl_value;
	break;
#endif				/* I_etherStatsStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in etherStatsEntry_test)\n"));
	return GEN_ERROR;

    }				/* switch */

    /* Do system dependent testing in k_etherStatsEntry_test */
    error_status = k_etherStatsEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object->nominator, ((etherStatsEntry_t *) (dp->data))->valid);
	error_status = k_etherStatsEntry_ready(object, value, doHead, dp);
    }

    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
etherStatsEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_etherStatsEntry_set((etherStatsEntry_t *) (doCur->data),
				  contextInfo, doCur->state));
}

#endif				/* SETS */
