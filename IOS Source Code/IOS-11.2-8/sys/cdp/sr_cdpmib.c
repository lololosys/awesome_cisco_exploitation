/* $Id: sr_cdpmib.c,v 3.5.8.6 1996/08/13 15:02:56 tylin Exp $
 * $Source: /release/112/cvs/Xsys/cdp/sr_cdpmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Arun Sastry (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_cdpmib.c,v $
 * Revision 3.5.8.6  1996/08/13  15:02:56  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Incorporate with new ifmib_swidb_get() supporting function.
 * Branch: California_branch
 *
 * Revision 3.5.8.5  1996/07/11  17:19:42  tylin
 * CSCdi59144:  setting cdpInterfaceEnable, cdpInterfaceMessageInterval =>
 * noSuchNam
 *    Obsolete wrongly defined object cdpInterfaceMessageInterval and
 *    define new
 *    object group cdpGlobal corresponds to command '[no] cdp run', 'cdp
 *    timer'
 *    and 'cdp holdtime'
 * Branch: California_branch
 *
 * Revision 3.5.8.4  1996/07/08  18:52:46  tylin
 * CSCdi41112:  CDP MIB agent returned non-enumeration
 *    Define unknown address type
 * Branch: California_branch
 *
 * Revision 3.5.8.3  1996/06/11  07:37:26  tylin
 * CSCdi59947:  SNMP response returns malformed cdpCacheAddress (x.y.0.0).
 *   Return correct structure type for cdpmib_get_address_info().
 * Branch: California_branch
 *
 * Revision 3.5.8.2  1996/05/21  09:43:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.8.1  1996/05/21  06:25:25  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5  1996/02/27  21:36:10  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:44:11  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:22:49  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  08:49:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/17  07:31:10  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.3  1995/06/27  07:21:16  smackie
 * Convert CDP to use platform_get_string(). Tidy up subsystem entry
 * points to use the right prototype. (CSCdi36461)
 *
 * Revision 2.2  1995/06/18  19:39:11  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:15:11  hampton
 * Bump version numbers from 1.x to 2.x.
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

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_cdpmib.h"
#include "sr_cdpmib-mib.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "address.h"
#include "cdp_externs.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

static void init_cdpmib (subsystype *subsys)
{
    load_mib(cdpmib_OidList, cdpmib_OidListNum);
    load_oid(cdpmib_oid_table);
}

/*
 * cdpmib_interface_test
 *
 * Indirectly called by ifmib_swidb_get() to test if the underlying
 * sw idb supports CDP.
 */
static boolean cdpmib_interface_test (idbtype *swidb, long *index2)
{
    return (cdp_supported_int(swidb));
}

/*---------------------------------------------------------------------
 * Retrieve data from the cdpInterfaceEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cdpInterfaceEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cdpInterfaceEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cdpInterfaceIfIndex;


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
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &cdpInterfaceIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    data = k_cdpInterfaceEntry_get(serialNum, contextInfo, arg, searchType,
				   cdpInterfaceIfIndex);
    if ((arg == -1) || (data == NULL)) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->cdpInterfaceIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cdpInterfaceEnable
      case I_cdpInterfaceEnable:
	dp = &data->cdpInterfaceEnable;
	break;
#endif				       /* I_cdpInterfaceEnable */

#ifdef I_cdpInterfaceMessageInterval
      case I_cdpInterfaceMessageInterval:
	dp = &data->cdpInterfaceMessageInterval;
	break;
#endif				       /* I_cdpInterfaceMessageInterval */

#ifdef I_cdpInterfaceGroup
      case I_cdpInterfaceGroup:
	dp = &data->cdpInterfaceGroup;
	break;
#endif				       /* I_cdpInterfaceGroup */

#ifdef I_cdpInterfacePort
      case I_cdpInterfacePort:
	dp = &data->cdpInterfacePort;
	break;
#endif				       /* I_cdpInterfacePort */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the cdpInterfaceEntry data object.
 *---------------------------------------------------------------------*/
void
cdpInterfaceEntry_free(cdpInterfaceEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after cdpInterfaceEntry set/undo
 *---------------------------------------------------------------------*/
static int
cdpInterfaceEntry_cleanup(doList_t *trash)
{
    cdpInterfaceEntry_free(trash->data);
#ifdef SR_SNMPv2
    cdpInterfaceEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
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
cdpInterfaceEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             instLength = incoming->length - object->oid.length;
    doList_t        *dp;
    int             found;
    int             carry = 0;
    cdpInterfaceEntry_t *cdpInterfaceEntry;
    long            cdpInterfaceIfIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cdpInterfaceIfIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }

    /*
     * Retrieve the sw idb.
     */
    if (ifmib_swidb_get(0, EXACT, cdpInterfaceIfIndex,
			cdpmib_interface_test, NULL) == NULL) {
	return (NO_CREATION_ERROR);
    }

    cdpInterfaceEntry = k_cdpInterfaceEntry_get(-1, contextInfo, object->nominator, EXACT, cdpInterfaceIfIndex);

#ifndef cdpInterfaceEntry_READ_CREATE

    if (cdpInterfaceEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* cdpInterfaceEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == cdpInterfaceEntry_set) &&
	    (((cdpInterfaceEntry_t *) (dp->data)) != NULL) &&
	    (((cdpInterfaceEntry_t *) (dp->data))->cdpInterfaceIfIndex == cdpInterfaceIfIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(cdpInterfaceEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(cdpInterfaceEntry_t));

	dp->setMethod = cdpInterfaceEntry_set;
	dp->cleanupMethod = cdpInterfaceEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_cdpInterfaceIfIndex, ((cdpInterfaceEntry_t *) (dp->data))->valid);
	((cdpInterfaceEntry_t *) (dp->data))->cdpInterfaceIfIndex = cdpInterfaceIfIndex;

#ifdef cdpInterfaceEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (cdpInterfaceEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, cdpInterfaceEntry, sizeof(cdpInterfaceEntry_t));

	    SET_ALL_VALID(((cdpInterfaceEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* cdpInterfaceEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_cdpInterfaceEnable
      case I_cdpInterfaceEnable:

	if ((value->sl_value != D_cdpInterfaceEnable_true) &&
	    (value->sl_value != D_cdpInterfaceEnable_false))
	    return (WRONG_VALUE_ERROR);

	SET_VALID(I_cdpInterfaceEnable, ((cdpInterfaceEntry_t *) (dp->data))->valid);

	((cdpInterfaceEntry_t *) (dp->data))->cdpInterfaceEnable = value->sl_value;
	break;
#endif				       /* I_cdpInterfaceEnable */

#ifdef I_cdpInterfaceMessageInterval
      case I_cdpInterfaceMessageInterval:

	SET_VALID(I_cdpInterfaceMessageInterval, ((cdpInterfaceEntry_t *) (dp->data))->valid);

	((cdpInterfaceEntry_t *) (dp->data))->cdpInterfaceMessageInterval = value->sl_value;
	break;
#endif				       /* I_cdpInterfaceMessageInterval */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cdpInterfaceEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
cdpInterfaceEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_cdpInterfaceEntry_set((cdpInterfaceEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif				       /* SETS */

/*---------------------------------------------------------------------
 * Retrieve data from the cdpCacheEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cdpCacheEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cdpCacheEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            cdpCacheIfIndex;
    long            cdpCacheDeviceIndex;


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
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &cdpCacheDeviceIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cdpCacheIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    data = k_cdpCacheEntry_get(serialNum, contextInfo, arg, searchType,
			       cdpCacheIfIndex, cdpCacheDeviceIndex);
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data == NULL)) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->cdpCacheIfIndex;
	inst.oid_ptr[1] = data->cdpCacheDeviceIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cdpCacheAddressType
      case I_cdpCacheAddressType:
	dp = &data->cdpCacheAddressType;
	break;
#endif				       /* I_cdpCacheAddressType */

#ifdef I_cdpCacheAddress
      case I_cdpCacheAddress:
	dp = MakeOctetString(data->cdpCacheAddress->octet_ptr, data->cdpCacheAddress->length);
	break;
#endif				       /* I_cdpCacheAddress */

#ifdef I_cdpCacheVersion
      case I_cdpCacheVersion:
	dp = MakeOctetString(data->cdpCacheVersion->octet_ptr, data->cdpCacheVersion->length);
	break;
#endif				       /* I_cdpCacheVersion */

#ifdef I_cdpCacheDeviceId
      case I_cdpCacheDeviceId:
	dp = MakeOctetString(data->cdpCacheDeviceId->octet_ptr, data->cdpCacheDeviceId->length);
	break;
#endif				       /* I_cdpCacheDeviceId */

#ifdef I_cdpCacheDevicePort
      case I_cdpCacheDevicePort:
	dp = MakeOctetString(data->cdpCacheDevicePort->octet_ptr, data->cdpCacheDevicePort->length);
	break;
#endif				       /* I_cdpCacheDevicePort */

#ifdef I_cdpCachePlatform
      case I_cdpCachePlatform:
	dp = MakeOctetString(data->cdpCachePlatform->octet_ptr, data->cdpCachePlatform->length);
	break;
#endif				       /* I_cdpCachePlatform */

#ifdef I_cdpCacheCapabilities
      case I_cdpCacheCapabilities:
	dp = MakeOctetString(data->cdpCacheCapabilities->octet_ptr, data->cdpCacheCapabilities->length);
	break;
#endif				       /* I_cdpCacheCapabilities */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cdpGlobal family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cdpGlobal_get(incoming, object, searchType, contextInfo, serialNum)
    OID		*incoming;
    ObjectInfo	*object;
    int		searchType;
    ContextInfo	*contextInfo;
    int		serialNum;
{
    int		arg;
    void	*dp;
    cdpGlobal_t	*data = NULL;

    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cdpGlobal_get(serialNum, contextInfo, arg)) == NULL)
	arg = -1;

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cdpGlobalRun
      case I_cdpGlobalRun:
	dp = &data->cdpGlobalRun;
	break;
#endif					/* I_cdpGlobalRun */

#ifdef I_cdpGlobalMessageInterval
      case I_cdpGlobalMessageInterval:
	dp = &data->cdpGlobalMessageInterval;
	break;
#endif					/* I_cdpGlobalMessageInterval */

#ifdef I_cdpGlobalHoldTime
      case I_cdpGlobalHoldTime:
	dp = &data->cdpGlobalHoldTime;
	break;
#endif					/* I_cdpGlobalHoldTime */

      default:
	return ((VarBind *) NULL);
    }					/* switch */

    return (MakeVarBind(object, &ZeroOid, dp));
}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the cdpGlobal data object.
 *---------------------------------------------------------------------*/
void
cdpGlobal_free(cdpGlobal_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after cdpGlobal set/undo
 *---------------------------------------------------------------------*/
static int
cdpGlobal_cleanup(doList_t *trash)
{
    cdpGlobal_free(trash->data);
#ifdef SR_SNMPv2
    cdpGlobal_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
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
cdpGlobal_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t        *dp;
    int		    found;

    /*
     * Validate the object instance: 1) It must be of length 1 2) and the
     * instance must be 0.
     */
    if (snmp_scalar_instance(incoming, object, EXACT) == -1) {
	return (NO_CREATION_ERROR);
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == cdpGlobal_set) &&
	    (((cdpGlobal_t *) (dp->data)) != NULL)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(cdpGlobal_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(cdpGlobal_t));

	dp->setMethod = cdpGlobal_set;
	dp->cleanupMethod = cdpGlobal_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif					/* SR_SNMPv2 */
	dp->state = UNKNOWN;
    }

    switch (object->nominator) {

#ifdef I_cdpGlobalRun
      case I_cdpGlobalRun:
	if ((value->sl_value != D_cdpGlobalRun_true) &&
	    (value->sl_value != D_cdpGlobalRun_false))
	    return (WRONG_VALUE_ERROR);

	SET_VALID(I_cdpGlobalRun, ((cdpGlobal_t *) (dp->data))->valid);
	((cdpGlobal_t *) (dp->data))->cdpGlobalRun = value->sl_value;
	break;
#endif					/* I_cdpGlobalRun */

#ifdef I_cdpGlobalMessageInterval
      case I_cdpGlobalMessageInterval:
	if ((value->sl_value < CDP_MIN_XMIT_INTERVAL) ||
	    (value->sl_value > CDP_MAX_XMIT_INTERVAL))
	    return (WRONG_VALUE_ERROR);

	SET_VALID(I_cdpGlobalMessageInterval, ((cdpGlobal_t *) (dp->data))->valid);
	((cdpGlobal_t *) (dp->data))->cdpGlobalMessageInterval = value->sl_value;
	break;
#endif					/* I_cdpGlobalMessageInterval */

#ifdef I_cdpGlobalHoldTime
      case I_cdpGlobalHoldTime:
	if ((value->sl_value < CDP_MIN_TTL) || (value->sl_value > CDP_MAX_TTL))
	    return (WRONG_VALUE_ERROR);

	SET_VALID(I_cdpGlobalHoldTime, ((cdpGlobal_t *) (dp->data))->valid);
	((cdpGlobal_t *) (dp->data))->cdpGlobalHoldTime = value->sl_value;
	break;
#endif					/* I_cdpGlobalHoldTime */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cdpGlobal_test)\n"));
	return (GEN_ERROR);
    }					/* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
cdpGlobal_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_cdpGlobal_set((cdpGlobal_t *) (doCur->data), contextInfo,
			    doCur->state));
}
#endif					/* SETS */

cdpInterfaceEntry_t *
k_cdpInterfaceEntry_get(serialNum, contextInfo, nominator, searchType, cdpInterfaceIfIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cdpInterfaceIfIndex;
{

    static cdpInterfaceEntry_t cdpInterfaceEntryData;
    idbtype *swidb;

    if (!cdp_running)
	return (NULL);

    swidb = ifmib_swidb_get(serialNum, searchType, cdpInterfaceIfIndex, 
			    cdpmib_interface_test, NULL);

    if (swidb) {
	cdpInterfaceEntryData.cdpInterfaceIfIndex = swidb->snmp_if_index;
	cdpInterfaceEntryData.cdpInterfaceEnable = swidb->cdp_enabled ?
		    D_cdpInterfaceEnable_true : D_cdpInterfaceEnable_false; 
	cdpInterfaceEntryData.cdpInterfaceGroup = 0;
	cdpInterfaceEntryData.cdpInterfacePort = 0;

	return (&cdpInterfaceEntryData);
    }
    return (NULL);
}

#ifdef SETS
int
k_cdpInterfaceEntry_set(data, contextInfo, function)
    cdpInterfaceEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    idbtype *swidb;

    if (VALID(I_cdpInterfaceEnable, data->valid)) {
	swidb = ifmib_swidb_get(0, EXACT, data->cdpInterfaceIfIndex,
				cdpmib_interface_test, NULL);
	if (!swidb) {
	    return (NO_CREATION_ERROR);
	}

	if (data->cdpInterfaceEnable == D_cdpInterfaceEnable_true) {
	    if (!swidb->cdp_enabled)
		cdp_intf_enable(swidb, FALSE);
	} else {
	    if (swidb->cdp_enabled)
		cdp_intf_disable(swidb);
	}
    }

    return (NO_ERROR);
}

#endif				       /* SETS */

/*
 * cdpmib_get_address_info
 *
 * Given a CDP cache entry, return pointer to the first address in the entry. 
 * Remember that addresses are stored as an "array" in the CDP cache structure.
 */
static cdp_addr_structure *cdpmib_get_address_info (cdp_cache_type *entry)
{
    ulong num_addr;    
    cdp_addr_info *cdp_addr;
    cdp_addr_structure *addr;

    cdp_addr = (cdp_addr_info *)(entry->addr);
    num_addr = cdp_addr->num_addr;

    /* Sanity check */
    if (num_addr == 0) {
	return (NULL);	
    }
    addr = (cdp_addr_structure *)((uchar *)cdp_addr + CDP_ADDRESS_OVERHEAD);
    return (addr);
}

/*
 * cdpmib_get_cache_entry
 *
 * Return a pointer to the CDP cache entry that matches index for GET,
 * next 'larger' entry for GET-NEXT, or NULL if no entry was found. 
 */
static cdp_cache_type *cdpmib_get_cache_entry(int searchType, uint ifIndex,
					      uint device_index)
{
    int bucket;
    cdp_cache_type *entry, *found_entry = NULL;
    uint snmp_if_index;
    uint min_if_index, min_device_number;

    min_if_index = min_device_number = 0xFFFFFFFF;
    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	for (entry = cdp_table[bucket]; entry; entry = entry->next) {
	    snmp_if_index = entry->idb->snmp_if_index ?
		entry->idb->snmp_if_index : entry->idb->hwptr->snmp_if_index;
	    if (snmp_if_index < ifIndex)
		continue;

	    if (searchType == EXACT) {
		if (snmp_if_index != ifIndex ||
		    device_index != entry->device_number)
		    continue;
		return (entry);
	    } else {
		if (snmp_if_index > ifIndex ||
		    entry->device_number >= device_index) {
		    if (snmp_if_index < min_if_index ||
			(snmp_if_index == min_if_index &&
			 entry->device_number < min_device_number)) {
			min_if_index = snmp_if_index;
			min_device_number = entry->device_number;
			found_entry = entry;
		    }
		}
	    }
	}
    }

    return (found_entry);
}

cdpCacheEntry_t *
k_cdpCacheEntry_get(serialNum, contextInfo, nominator, searchType, cdpCacheIfIndex, cdpCacheDeviceIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cdpCacheIfIndex;
    long            cdpCacheDeviceIndex;
{

    static cdpCacheEntry_t cdpCacheEntryData;
    cdp_cache_type *cdp_cache_entry;
    cdp_addr_structure *addr;

    static OctetString addr_string, version_string, device_id_string,
           port_id_string, platform_string, capab_string;

    if (!cdp_running)
	return (NULL);

    cdp_cache_entry = cdpmib_get_cache_entry(searchType, (uint) cdpCacheIfIndex,
					     (uint) cdpCacheDeviceIndex);
    if (!cdp_cache_entry)
       return (NULL);

    cdpCacheEntryData.cdpCacheIfIndex = cdp_cache_entry->idb->snmp_if_index ?
	    cdp_cache_entry->idb->snmp_if_index :
	    cdp_cache_entry->idb->hwptr->snmp_if_index;
    cdpCacheEntryData.cdpCacheDeviceIndex = cdp_cache_entry->device_number;
    cdpCacheEntryData.cdpCacheAddress = &addr_string;

    addr = cdpmib_get_address_info(cdp_cache_entry);
    if (addr) {
        cdpCacheEntryData.cdpCacheAddressType = addr->type;
	addr_string.length = addr->length;
	addr_string.octet_ptr = addr->address;
    }
    else {
        cdpCacheEntryData.cdpCacheAddressType = D_cdpCacheAddressType_unknown;
	addr_string.length = 0;
	addr_string.octet_ptr = NULL;
    }

    cdpCacheEntryData.cdpCacheVersion = &version_string;
    version_string.length = cdp_cache_entry->version ?
	                      strlen(cdp_cache_entry->version) : 0;
    version_string.octet_ptr = cdp_cache_entry->version ?
	                      cdp_cache_entry->version : NULL;

    cdpCacheEntryData.cdpCacheDeviceId = &device_id_string;
    device_id_string.length = cdp_cache_entry->entry_name ?
	                     cdp_cache_entry->entry_name_len : 0;
    device_id_string.octet_ptr = cdp_cache_entry->entry_name ?
	                     cdp_cache_entry->entry_name : NULL;

    cdpCacheEntryData.cdpCacheDevicePort = &port_id_string;
    port_id_string.length = cdp_cache_entry->port_id ?
	                     strlen(cdp_cache_entry->port_id) : 0;
    port_id_string.octet_ptr = cdp_cache_entry->port_id ?
	                     cdp_cache_entry->port_id : NULL;

    cdpCacheEntryData.cdpCachePlatform = &platform_string;
    platform_string.length = cdp_cache_entry->platform ?
	                     strlen(cdp_cache_entry->platform) : 0;
    platform_string.octet_ptr = cdp_cache_entry->platform ?
	                     cdp_cache_entry->platform : NULL;

    cdpCacheEntryData.cdpCacheCapabilities = &capab_string;
    capab_string.length = CDP_CAPAB_FIELD_LEN;
    capab_string.octet_ptr = (uchar *)&cdp_cache_entry->capabilities;

    return (&cdpCacheEntryData);

}

cdpGlobal_t *
k_cdpGlobal_get(serialNum, contextInfo, nominator)
    int		serialNum;
    ContextInfo	*contextInfo;
    int		nominator;
{
    static cdpGlobal_t	cdpGlobalData;

    cdpGlobalData.cdpGlobalRun = cdp_running ?
	D_cdpGlobalRun_true : D_cdpGlobalRun_false;
    cdpGlobalData.cdpGlobalHoldTime = cdp_holdtime;
    cdpGlobalData.cdpGlobalMessageInterval = cdp_xmit_frequency;
    return (&cdpGlobalData);
}

#ifdef SETS
int
k_cdpGlobal_set(data, contextInfo, function)
    cdpGlobal_t	*data;
    ContextInfo	*contextInfo;
    int		function;
{
    if (VALID(I_cdpGlobalRun, data->valid)) {
	if (data->cdpGlobalRun == D_cdpGlobalRun_true) {
	    if (!cdp_running)
		cdp_start_process();
	} else {
	    cdp_explicitly_disarmed = TRUE;

	    if (cdp_running) {
		cdp_running = FALSE;

		/*
		 * "Unregister" CDP notification functions.
		 */
		reg_delete_will_reload(cdp_reload_notify);
		reg_delete_swif_goingdown(cdp_if_down_notify);
		reg_delete_swif_comingup(cdp_if_up_notify);
	    }
	}
    }

    if (VALID(I_cdpGlobalHoldTime, data->valid)) {
	cdp_holdtime = data->cdpGlobalHoldTime;
    }

    if (VALID(I_cdpGlobalMessageInterval, data->valid)) {
	cdp_xmit_frequency = data->cdpGlobalMessageInterval;
    }

    return (NO_ERROR);
}
#endif				/* SETS */



/*
 * cdpmib subsystem header
 */

#define MAJVERSION_cdpmib 1
#define MINVERSION_cdpmib 0
#define EDITVERSION_cdpmib 0

SUBSYS_HEADER(cdpmib,
              MAJVERSION_cdpmib,
              MINVERSION_cdpmib,
              EDITVERSION_cdpmib,
              init_cdpmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cdp",
              NULL);
      
