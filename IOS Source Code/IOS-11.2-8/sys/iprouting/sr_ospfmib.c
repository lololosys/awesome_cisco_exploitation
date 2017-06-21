/* $Id: sr_ospfmib.c,v 3.5.6.3 1996/07/03 20:42:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/sr_ospfmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ospfmib.c,v $
 * Revision 3.5.6.3  1996/07/03  20:42:37  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.6.2  1996/07/01  18:45:08  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/03/18  20:29:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  06:58:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/07  09:50:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/03/02  03:54:23  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:49:58  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:26:02  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  17:37:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:33:13  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:28:07  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:11:15  hampton
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

/* The RFC 1253 was defined in such a way that it assumes only one process
 * in each router,but cisco router does allow more than one process running
 * in the same router,in the current RFC there is no instance defined to 
 * allow agent to identify which process' mib objects the manager is really 
 * interested, so in the current implementation we alway use the first 
 * process. This is a limitation of the current RFC.
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_ospfmib.h"
#include "sr_ospfmib-mib.h"
#include "interface_private.h"
#include "route.h"
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"

static ospfGeneralGroup_t ospfGeneralGroupData;
static ospfAreaEntry_t ospfAreaEntryData;
static ospfStubAreaEntry_t ospfStubAreaEntryData;
static ospfLsdbEntry_t ospfLsdbEntryData;
static ospfAreaRangeEntry_t ospfAreaRangeEntryData;
static ospfHostEntry_t ospfHostEntryData;
static ospfIfEntry_t ospfIfEntryData;
static ospfIfMetricEntry_t ospfIfMetricEntryData;
static ospfVirtIfEntry_t ospfVirtIfEntryData;
static ospfNbrEntry_t ospfNbrEntryData;
static ospfVirtNbrEntry_t ospfVirtNbrEntryData;
static ospf_idb_info temp_ospf_info;


void
init_ospfmib (subsystype *subsys)
{
    memset(&ospfGeneralGroupData, 0, sizeof(ospfGeneralGroupData));
    memset(&ospfAreaEntryData, 0, sizeof(ospfAreaEntryData));
    memset(&ospfStubAreaEntryData, 0, sizeof(ospfStubAreaEntryData));
    memset(&ospfLsdbEntryData, 0, sizeof(ospfLsdbEntryData));
    memset(&ospfAreaRangeEntryData, 0, sizeof(ospfAreaRangeEntryData));
    memset(&ospfHostEntryData, 0, sizeof(ospfHostEntryData));
    memset(&ospfIfEntryData, 0, sizeof(ospfIfEntryData));
    memset(&ospfIfMetricEntryData, 0, sizeof(ospfIfMetricEntryData));
    memset(&ospfVirtIfEntryData, 0, sizeof(ospfVirtIfEntryData));
    memset(&ospfNbrEntryData, 0, sizeof(ospfNbrEntryData));
    memset(&ospfVirtNbrEntryData, 0, sizeof(ospfVirtNbrEntryData));
    load_mib(ospfmib_OidList, ospfmib_OidListNum);
    load_oid(ospfmib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the ospfGeneralGroup family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfGeneralGroup_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    ospfGeneralGroup_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfGeneralGroup_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfRouterId
      case I_ospfRouterId:
	dp = IPToOctetString(data->ospfRouterId);
	break;
#endif				       /* I_ospfRouterId */

#ifdef I_ospfAdminStat
      case I_ospfAdminStat:
	dp = &data->ospfAdminStat;
	break;
#endif				       /* I_ospfAdminStat */

#ifdef I_ospfVersionNumber
      case I_ospfVersionNumber:
	dp = &data->ospfVersionNumber;
	break;
#endif				       /* I_ospfVersionNumber */

#ifdef I_ospfAreaBdrRtrStatus
      case I_ospfAreaBdrRtrStatus:
	dp = &data->ospfAreaBdrRtrStatus;
	break;
#endif				       /* I_ospfAreaBdrRtrStatus */

#ifdef I_ospfASBdrRtrStatus
      case I_ospfASBdrRtrStatus:
	dp = &data->ospfASBdrRtrStatus;
	break;
#endif				       /* I_ospfASBdrRtrStatus */

#ifdef I_ospfExternLSACount
      case I_ospfExternLSACount:
	dp = &data->ospfExternLSACount;
	break;
#endif				       /* I_ospfExternLSACount */

#ifdef I_ospfExternLSACksumSum
      case I_ospfExternLSACksumSum:
	dp = &data->ospfExternLSACksumSum;
	break;
#endif				       /* I_ospfExternLSACksumSum */

#ifdef I_ospfTOSSupport
      case I_ospfTOSSupport:
	dp = &data->ospfTOSSupport;
	break;
#endif				       /* I_ospfTOSSupport */

#ifdef I_ospfOriginateNewLSAs
      case I_ospfOriginateNewLSAs:
	dp = &data->ospfOriginateNewLSAs;
	break;
#endif				       /* I_ospfOriginateNewLSAs */

#ifdef I_ospfRxNewLSAs
      case I_ospfRxNewLSAs:
	dp = &data->ospfRxNewLSAs;
	break;
#endif				       /* I_ospfRxNewLSAs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfGeneralGroup data object.
 *---------------------------------------------------------------------*/
static void
ospfGeneralGroup_free(ospfGeneralGroup_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfGeneralGroup set/undo
 *---------------------------------------------------------------------*/
static int
ospfGeneralGroup_cleanup(doList_t       *trash)
{
    ospfGeneralGroup_free(trash->data);
#ifdef SR_SNMPv2
    ospfGeneralGroup_free(trash->undodata);
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
ospfGeneralGroup_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    /* ospfGeneralGroup_t *ospfGeneralGroup; */

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance: 1) It must be of length 1  2) and the
     * instance must be 0.
     */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
	return (NO_CREATION_ERROR);
    }
    found = 0;

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfGeneralGroup_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfGeneralGroup_t));

	dp->setMethod = ospfGeneralGroup_set;
	dp->cleanupMethod = ospfGeneralGroup_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

    }
    switch (object->nominator) {

#ifdef I_ospfRouterId
      case I_ospfRouterId:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfRouterId, ((ospfGeneralGroup_t *) (dp->data))->valid);

	((ospfGeneralGroup_t *) (dp->data))->ospfRouterId = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfRouterId */

#ifdef I_ospfAdminStat
      case I_ospfAdminStat:

	SET_VALID(I_ospfAdminStat, ((ospfGeneralGroup_t *) (dp->data))->valid);

	((ospfGeneralGroup_t *) (dp->data))->ospfAdminStat = value->sl_value;
	break;
#endif				       /* I_ospfAdminStat */

#ifdef I_ospfASBdrRtrStatus
      case I_ospfASBdrRtrStatus:

	SET_VALID(I_ospfASBdrRtrStatus, ((ospfGeneralGroup_t *) (dp->data))->valid);

	((ospfGeneralGroup_t *) (dp->data))->ospfASBdrRtrStatus = value->sl_value;
	break;
#endif				       /* I_ospfASBdrRtrStatus */

#ifdef I_ospfTOSSupport
      case I_ospfTOSSupport:

	SET_VALID(I_ospfTOSSupport, ((ospfGeneralGroup_t *) (dp->data))->valid);

	((ospfGeneralGroup_t *) (dp->data))->ospfTOSSupport = value->sl_value;
	break;
#endif				       /* I_ospfTOSSupport */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfGeneralGroup_test)\n"));
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
ospfGeneralGroup_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    return (k_ospfGeneralGroup_set((ospfGeneralGroup_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfAreaEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfAreaEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum
    )
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfAreaEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    unsigned long   ospfAreaId;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 4
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 4) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 0 + object->oid.length, &ospfAreaId, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfAreaEntry_get(serialNum, contextInfo, arg, searchType, ospfAreaId)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = (data->ospfAreaId >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfAreaId >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfAreaId >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfAreaId >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfAreaId
      case I_ospfAreaId:
	dp = IPToOctetString(data->ospfAreaId);
	break;
#endif				       /* I_ospfAreaId */

#ifdef I_ospfAuthType
      case I_ospfAuthType:
	dp = &data->ospfAuthType;
	break;
#endif				       /* I_ospfAuthType */

#ifdef I_ospfImportASExtern
      case I_ospfImportASExtern:
	dp = &data->ospfImportASExtern;
	break;
#endif				       /* I_ospfImportASExtern */

#ifdef I_ospfSpfRuns
      case I_ospfSpfRuns:
	dp = &data->ospfSpfRuns;
	break;
#endif				       /* I_ospfSpfRuns */

#ifdef I_ospfAreaBdrRtrCount
      case I_ospfAreaBdrRtrCount:
	dp = &data->ospfAreaBdrRtrCount;
	break;
#endif				       /* I_ospfAreaBdrRtrCount */

#ifdef I_ospfASBdrRtrCount
      case I_ospfASBdrRtrCount:
	dp = &data->ospfASBdrRtrCount;
	break;
#endif				       /* I_ospfASBdrRtrCount */

#ifdef I_ospfAreaLSACount
      case I_ospfAreaLSACount:
	dp = &data->ospfAreaLSACount;
	break;
#endif				       /* I_ospfAreaLSACount */

#ifdef I_ospfAreaLSACksumSum
      case I_ospfAreaLSACksumSum:
	dp = &data->ospfAreaLSACksumSum;
	break;
#endif				       /* I_ospfAreaLSACksumSum */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfAreaEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfAreaEntry_free(ospfAreaEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfAreaEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfAreaEntry_cleanup(doList_t *trash)
{
    ospfAreaEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfAreaEntry_free(trash->undodata);
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
ospfAreaEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfAreaEntry_t *ospfAreaEntry;
    unsigned long   ospfAreaId;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 4) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfAreaId, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfAreaEntry = k_ospfAreaEntry_get(-1, contextInfo, object->nominator, EXACT, ospfAreaId);

#ifndef ospfAreaEntry_READ_CREATE

    if (ospfAreaEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfAreaEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfAreaEntry_set) &&
	    (((ospfAreaEntry_t *) (dp->data)) != NULL) &&
	    (((ospfAreaEntry_t *) (dp->data))->ospfAreaId == ospfAreaId)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfAreaEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfAreaEntry_t));

	dp->setMethod = ospfAreaEntry_set;
	dp->cleanupMethod = ospfAreaEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfAreaId, ((ospfAreaEntry_t *) (dp->data))->valid);
	((ospfAreaEntry_t *) (dp->data))->ospfAreaId = ospfAreaId;

#ifdef ospfAreaEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfAreaEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfAreaEntry, sizeof(ospfAreaEntry_t));

	    SET_ALL_VALID(((ospfAreaEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfAreaEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfAreaId
      case I_ospfAreaId:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfAreaId, ((ospfAreaEntry_t *) (dp->data))->valid);

	((ospfAreaEntry_t *) (dp->data))->ospfAreaId = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfAreaId */

#ifdef I_ospfAuthType
      case I_ospfAuthType:

	SET_VALID(I_ospfAuthType, ((ospfAreaEntry_t *) (dp->data))->valid);

	((ospfAreaEntry_t *) (dp->data))->ospfAuthType = value->sl_value;
	break;
#endif				       /* I_ospfAuthType */

#ifdef I_ospfImportASExtern
      case I_ospfImportASExtern:

	SET_VALID(I_ospfImportASExtern, ((ospfAreaEntry_t *) (dp->data))->valid);

	((ospfAreaEntry_t *) (dp->data))->ospfImportASExtern = value->sl_value;
	break;
#endif				       /* I_ospfImportASExtern */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfAreaEntry_test)\n"));
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
ospfAreaEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
)
{
    return (k_ospfAreaEntry_set((ospfAreaEntry_t *) (doCur->data),
				contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfStubAreaEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfStubAreaEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum
    )
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfStubAreaEntry_t *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    unsigned long   ospfStubAreaID;
    long            ospfStubTOS;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 5
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 5) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 4 + object->oid.length, &ospfStubTOS, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfStubAreaID, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfStubAreaEntry_get(serialNum, contextInfo, arg, searchType, ospfStubAreaID, ospfStubTOS)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 5;
	inst.oid_ptr[0] = (data->ospfStubAreaID >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfStubAreaID >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfStubAreaID >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfStubAreaID >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfStubTOS;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfStubAreaID
      case I_ospfStubAreaID:
	dp = IPToOctetString(data->ospfStubAreaID);
	break;
#endif				       /* I_ospfStubAreaID */

#ifdef I_ospfStubTOS
      case I_ospfStubTOS:
	dp = &data->ospfStubTOS;
	break;
#endif				       /* I_ospfStubTOS */

#ifdef I_ospfStubMetric
      case I_ospfStubMetric:
	dp = &data->ospfStubMetric;
	break;
#endif				       /* I_ospfStubMetric */

#ifdef I_ospfStubStatus
      case I_ospfStubStatus:
	dp = &data->ospfStubStatus;
	break;
#endif				       /* I_ospfStubStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfStubAreaEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfStubAreaEntry_free(ospfStubAreaEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfStubAreaEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfStubAreaEntry_cleanup(doList_t *trash)
{
    ospfStubAreaEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfStubAreaEntry_free(trash->undodata);
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
ospfStubAreaEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfStubAreaEntry_t *ospfStubAreaEntry;
    unsigned long   ospfStubAreaID;
    long            ospfStubTOS;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 5) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfStubAreaID, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfStubTOS, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfStubAreaEntry = k_ospfStubAreaEntry_get(-1, contextInfo, object->nominator, EXACT, ospfStubAreaID, ospfStubTOS);

#ifndef ospfStubAreaEntry_READ_CREATE

    if (ospfStubAreaEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfStubAreaEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfStubAreaEntry_set) &&
	    (((ospfStubAreaEntry_t *) (dp->data)) != NULL) &&
	    (((ospfStubAreaEntry_t *) (dp->data))->ospfStubAreaID == ospfStubAreaID) &&
	(((ospfStubAreaEntry_t *) (dp->data))->ospfStubTOS == ospfStubTOS)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfStubAreaEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfStubAreaEntry_t));

	dp->setMethod = ospfStubAreaEntry_set;
	dp->cleanupMethod = ospfStubAreaEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfStubAreaID, ((ospfStubAreaEntry_t *) (dp->data))->valid);
	((ospfStubAreaEntry_t *) (dp->data))->ospfStubAreaID = ospfStubAreaID;

	SET_VALID(I_ospfStubTOS, ((ospfStubAreaEntry_t *) (dp->data))->valid);
	((ospfStubAreaEntry_t *) (dp->data))->ospfStubTOS = ospfStubTOS;

#ifdef ospfStubAreaEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfStubAreaEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfStubAreaEntry, sizeof(ospfStubAreaEntry_t));

	    SET_ALL_VALID(((ospfStubAreaEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfStubAreaEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfStubAreaID
      case I_ospfStubAreaID:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfStubAreaID, ((ospfStubAreaEntry_t *) (dp->data))->valid);

	((ospfStubAreaEntry_t *) (dp->data))->ospfStubAreaID = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfStubAreaID */

#ifdef I_ospfStubTOS
      case I_ospfStubTOS:

	SET_VALID(I_ospfStubTOS, ((ospfStubAreaEntry_t *) (dp->data))->valid);

	((ospfStubAreaEntry_t *) (dp->data))->ospfStubTOS = value->sl_value;
	break;
#endif				       /* I_ospfStubTOS */

#ifdef I_ospfStubMetric
      case I_ospfStubMetric:

	SET_VALID(I_ospfStubMetric, ((ospfStubAreaEntry_t *) (dp->data))->valid);

	((ospfStubAreaEntry_t *) (dp->data))->ospfStubMetric = value->sl_value;
	break;
#endif				       /* I_ospfStubMetric */

#ifdef I_ospfStubStatus
      case I_ospfStubStatus:

	SET_VALID(I_ospfStubStatus, ((ospfStubAreaEntry_t *) (dp->data))->valid);

	((ospfStubAreaEntry_t *) (dp->data))->ospfStubStatus = value->sl_value;
	break;
#endif				       /* I_ospfStubStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfStubAreaEntry_test)\n"));
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
ospfStubAreaEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    return (k_ospfStubAreaEntry_set((ospfStubAreaEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfLsdbEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfLsdbEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum
    )
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfLsdbEntry_t *data = NULL;
    unsigned long   buffer[13];
    OID             inst;
    int             carry;
    unsigned long   ospfLsdbAreaId;
    long            ospfLsdbType;
    unsigned long   ospfLsdbLSID;
    unsigned long   ospfLsdbRouterId;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 13
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 13) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 9 + object->oid.length, &ospfLsdbRouterId, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 5 + object->oid.length, &ospfLsdbLSID, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfLsdbType, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfLsdbAreaId, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfLsdbEntry_get(serialNum, contextInfo, arg, searchType, ospfLsdbAreaId, ospfLsdbType, ospfLsdbLSID, ospfLsdbRouterId)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 13;
	inst.oid_ptr[0] = (data->ospfLsdbAreaId >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfLsdbAreaId >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfLsdbAreaId >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfLsdbAreaId >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfLsdbType;
	inst.oid_ptr[5] = (data->ospfLsdbLSID >> 24) & 0xff;
	inst.oid_ptr[6] = (data->ospfLsdbLSID >> 16) & 0xff;
	inst.oid_ptr[7] = (data->ospfLsdbLSID >> 8) & 0xff;
	inst.oid_ptr[8] = (data->ospfLsdbLSID >> 0) & 0xff;
	inst.oid_ptr[9] = (data->ospfLsdbRouterId >> 24) & 0xff;
	inst.oid_ptr[10] = (data->ospfLsdbRouterId >> 16) & 0xff;
	inst.oid_ptr[11] = (data->ospfLsdbRouterId >> 8) & 0xff;
	inst.oid_ptr[12] = (data->ospfLsdbRouterId >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfLsdbAreaId
      case I_ospfLsdbAreaId:
	dp = IPToOctetString(data->ospfLsdbAreaId);
	break;
#endif				       /* I_ospfLsdbAreaId */

#ifdef I_ospfLsdbType
      case I_ospfLsdbType:
	dp = &data->ospfLsdbType;
	break;
#endif				       /* I_ospfLsdbType */

#ifdef I_ospfLsdbLSID
      case I_ospfLsdbLSID:
	dp = IPToOctetString(data->ospfLsdbLSID);
	break;
#endif				       /* I_ospfLsdbLSID */

#ifdef I_ospfLsdbRouterId
      case I_ospfLsdbRouterId:
	dp = IPToOctetString(data->ospfLsdbRouterId);
	break;
#endif				       /* I_ospfLsdbRouterId */

#ifdef I_ospfLsdbSequence
      case I_ospfLsdbSequence:
	dp = &data->ospfLsdbSequence;
	break;
#endif				       /* I_ospfLsdbSequence */

#ifdef I_ospfLsdbAge
      case I_ospfLsdbAge:
	dp = &data->ospfLsdbAge;
	break;
#endif				       /* I_ospfLsdbAge */

#ifdef I_ospfLsdbChecksum
      case I_ospfLsdbChecksum:
	dp = &data->ospfLsdbChecksum;
	break;
#endif				       /* I_ospfLsdbChecksum */

#ifdef I_ospfLsdbAdvertisement
      case I_ospfLsdbAdvertisement:
	dp = MakeOctetString(data->ospfLsdbAdvertisement->octet_ptr, data->ospfLsdbAdvertisement->length);
	break;
#endif				       /* I_ospfLsdbAdvertisement */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ospfAreaRangeEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfAreaRangeEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum
    )
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfAreaRangeEntry_t *data = NULL;
    unsigned long   buffer[8];
    OID             inst;
    int             carry;
    unsigned long   ospfAreaRangeAreaID;
    unsigned long   ospfAreaRangeNet;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 8
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 8) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 4 + object->oid.length, &ospfAreaRangeNet, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfAreaRangeAreaID, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfAreaRangeEntry_get(serialNum, contextInfo, arg, searchType, ospfAreaRangeAreaID, ospfAreaRangeNet)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 8;
	inst.oid_ptr[0] = (data->ospfAreaRangeAreaID >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfAreaRangeAreaID >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfAreaRangeAreaID >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfAreaRangeAreaID >> 0) & 0xff;
	inst.oid_ptr[4] = (data->ospfAreaRangeNet >> 24) & 0xff;
	inst.oid_ptr[5] = (data->ospfAreaRangeNet >> 16) & 0xff;
	inst.oid_ptr[6] = (data->ospfAreaRangeNet >> 8) & 0xff;
	inst.oid_ptr[7] = (data->ospfAreaRangeNet >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfAreaRangeAreaID
      case I_ospfAreaRangeAreaID:
	dp = IPToOctetString(data->ospfAreaRangeAreaID);
	break;
#endif				       /* I_ospfAreaRangeAreaID */

#ifdef I_ospfAreaRangeNet
      case I_ospfAreaRangeNet:
	dp = IPToOctetString(data->ospfAreaRangeNet);
	break;
#endif				       /* I_ospfAreaRangeNet */

#ifdef I_ospfAreaRangeMask
      case I_ospfAreaRangeMask:
	dp = IPToOctetString(data->ospfAreaRangeMask);
	break;
#endif				       /* I_ospfAreaRangeMask */

#ifdef I_ospfAreaRangeStatus
      case I_ospfAreaRangeStatus:
	dp = &data->ospfAreaRangeStatus;
	break;
#endif				       /* I_ospfAreaRangeStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfAreaRangeEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfAreaRangeEntry_free(ospfAreaRangeEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfAreaRangeEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfAreaRangeEntry_cleanup(doList_t *trash)
{
    ospfAreaRangeEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfAreaRangeEntry_free(trash->undodata);
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
ospfAreaRangeEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfAreaRangeEntry_t *ospfAreaRangeEntry;
    unsigned long   ospfAreaRangeAreaID;
    unsigned long   ospfAreaRangeNet;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 8) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfAreaRangeAreaID, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToIP(incoming, 4 + object->oid.length, &ospfAreaRangeNet, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfAreaRangeEntry = k_ospfAreaRangeEntry_get(-1, contextInfo, object->nominator, EXACT, ospfAreaRangeAreaID, ospfAreaRangeNet);

#ifndef ospfAreaRangeEntry_READ_CREATE

    if (ospfAreaRangeEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfAreaRangeEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfAreaRangeEntry_set) &&
	    (((ospfAreaRangeEntry_t *) (dp->data)) != NULL) &&
	    (((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeAreaID == ospfAreaRangeAreaID) &&
	    (((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeNet == ospfAreaRangeNet)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfAreaRangeEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfAreaRangeEntry_t));

	dp->setMethod = ospfAreaRangeEntry_set;
	dp->cleanupMethod = ospfAreaRangeEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfAreaRangeAreaID, ((ospfAreaRangeEntry_t *) (dp->data))->valid);
	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeAreaID = ospfAreaRangeAreaID;

	SET_VALID(I_ospfAreaRangeNet, ((ospfAreaRangeEntry_t *) (dp->data))->valid);
	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeNet = ospfAreaRangeNet;

#ifdef ospfAreaRangeEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfAreaRangeEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfAreaRangeEntry, sizeof(ospfAreaRangeEntry_t));

	    SET_ALL_VALID(((ospfAreaRangeEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfAreaRangeEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfAreaRangeAreaID
      case I_ospfAreaRangeAreaID:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfAreaRangeAreaID, ((ospfAreaRangeEntry_t *) (dp->data))->valid);

	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeAreaID = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfAreaRangeAreaID */

#ifdef I_ospfAreaRangeNet
      case I_ospfAreaRangeNet:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfAreaRangeNet, ((ospfAreaRangeEntry_t *) (dp->data))->valid);

	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeNet = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfAreaRangeNet */

#ifdef I_ospfAreaRangeMask
      case I_ospfAreaRangeMask:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfAreaRangeMask, ((ospfAreaRangeEntry_t *) (dp->data))->valid);

	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeMask = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfAreaRangeMask */

#ifdef I_ospfAreaRangeStatus
      case I_ospfAreaRangeStatus:

	SET_VALID(I_ospfAreaRangeStatus, ((ospfAreaRangeEntry_t *) (dp->data))->valid);

	((ospfAreaRangeEntry_t *) (dp->data))->ospfAreaRangeStatus = value->sl_value;
	break;
#endif				       /* I_ospfAreaRangeStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfAreaRangeEntry_test)\n"));
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
ospfAreaRangeEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    return (k_ospfAreaRangeEntry_set((ospfAreaRangeEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfHostEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfHostEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum
    )
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfHostEntry_t *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    unsigned long   ospfHostIpAddress;
    long            ospfHostTOS;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 5
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 5) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 4 + object->oid.length, &ospfHostTOS, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfHostIpAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfHostEntry_get(serialNum, contextInfo, arg, searchType, ospfHostIpAddress, ospfHostTOS)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 5;
	inst.oid_ptr[0] = (data->ospfHostIpAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfHostIpAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfHostIpAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfHostIpAddress >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfHostTOS;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfHostIpAddress
      case I_ospfHostIpAddress:
	dp = IPToOctetString(data->ospfHostIpAddress);
	break;
#endif				       /* I_ospfHostIpAddress */

#ifdef I_ospfHostTOS
      case I_ospfHostTOS:
	dp = &data->ospfHostTOS;
	break;
#endif				       /* I_ospfHostTOS */

#ifdef I_ospfHostMetric
      case I_ospfHostMetric:
	dp = &data->ospfHostMetric;
	break;
#endif				       /* I_ospfHostMetric */

#ifdef I_ospfHostStatus
      case I_ospfHostStatus:
	dp = &data->ospfHostStatus;
	break;
#endif				       /* I_ospfHostStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfHostEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfHostEntry_free(ospfHostEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfHostEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfHostEntry_cleanup(doList_t *trash)
{
    ospfHostEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfHostEntry_free(trash->undodata);
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
ospfHostEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfHostEntry_t *ospfHostEntry;
    unsigned long   ospfHostIpAddress;
    long            ospfHostTOS;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 5) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfHostIpAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfHostTOS, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfHostEntry = k_ospfHostEntry_get(-1, contextInfo, object->nominator, EXACT, ospfHostIpAddress, ospfHostTOS);

#ifndef ospfHostEntry_READ_CREATE

    if (ospfHostEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfHostEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfHostEntry_set) &&
	    (((ospfHostEntry_t *) (dp->data)) != NULL) &&
	    (((ospfHostEntry_t *) (dp->data))->ospfHostIpAddress == ospfHostIpAddress) &&
	    (((ospfHostEntry_t *) (dp->data))->ospfHostTOS == ospfHostTOS)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfHostEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfHostEntry_t));

	dp->setMethod = ospfHostEntry_set;
	dp->cleanupMethod = ospfHostEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfHostIpAddress, ((ospfHostEntry_t *) (dp->data))->valid);
	((ospfHostEntry_t *) (dp->data))->ospfHostIpAddress = ospfHostIpAddress;

	SET_VALID(I_ospfHostTOS, ((ospfHostEntry_t *) (dp->data))->valid);
	((ospfHostEntry_t *) (dp->data))->ospfHostTOS = ospfHostTOS;

#ifdef ospfHostEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfHostEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfHostEntry, sizeof(ospfHostEntry_t));

	    SET_ALL_VALID(((ospfHostEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfHostEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfHostIpAddress
      case I_ospfHostIpAddress:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfHostIpAddress, ((ospfHostEntry_t *) (dp->data))->valid);

	((ospfHostEntry_t *) (dp->data))->ospfHostIpAddress = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfHostIpAddress */

#ifdef I_ospfHostTOS
      case I_ospfHostTOS:

	SET_VALID(I_ospfHostTOS, ((ospfHostEntry_t *) (dp->data))->valid);

	((ospfHostEntry_t *) (dp->data))->ospfHostTOS = value->sl_value;
	break;
#endif				       /* I_ospfHostTOS */

#ifdef I_ospfHostMetric
      case I_ospfHostMetric:

	SET_VALID(I_ospfHostMetric, ((ospfHostEntry_t *) (dp->data))->valid);

	((ospfHostEntry_t *) (dp->data))->ospfHostMetric = value->sl_value;
	break;
#endif				       /* I_ospfHostMetric */

#ifdef I_ospfHostStatus
      case I_ospfHostStatus:

	SET_VALID(I_ospfHostStatus, ((ospfHostEntry_t *) (dp->data))->valid);

	((ospfHostEntry_t *) (dp->data))->ospfHostStatus = value->sl_value;
	break;
#endif				       /* I_ospfHostStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfHostEntry_test)\n"));
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
ospfHostEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    return (k_ospfHostEntry_set((ospfHostEntry_t *) (doCur->data),
				contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfIfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfIfEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfIfEntry_t  *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    unsigned long   ospfIfIpAddress;
    long            ospfAddressLessIf;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 5
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 5) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 4 + object->oid.length, &ospfAddressLessIf, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfIfIpAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfIfEntry_get(serialNum, contextInfo, arg, searchType, ospfIfIpAddress, ospfAddressLessIf)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 5;
	inst.oid_ptr[0] = (data->ospfIfIpAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfIfIpAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfIfIpAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfIfIpAddress >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfAddressLessIf;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfIfIpAddress
      case I_ospfIfIpAddress:
	dp = IPToOctetString(data->ospfIfIpAddress);
	break;
#endif				       /* I_ospfIfIpAddress */

#ifdef I_ospfAddressLessIf
      case I_ospfAddressLessIf:
	dp = &data->ospfAddressLessIf;
	break;
#endif				       /* I_ospfAddressLessIf */

#ifdef I_ospfIfAreaId
      case I_ospfIfAreaId:
	dp = IPToOctetString(data->ospfIfAreaId);
	break;
#endif				       /* I_ospfIfAreaId */

#ifdef I_ospfIfType
      case I_ospfIfType:
	dp = &data->ospfIfType;
	break;
#endif				       /* I_ospfIfType */

#ifdef I_ospfIfAdminStat
      case I_ospfIfAdminStat:
	dp = &data->ospfIfAdminStat;
	break;
#endif				       /* I_ospfIfAdminStat */

#ifdef I_ospfIfRtrPriority
      case I_ospfIfRtrPriority:
	dp = &data->ospfIfRtrPriority;
	break;
#endif				       /* I_ospfIfRtrPriority */

#ifdef I_ospfIfTransitDelay
      case I_ospfIfTransitDelay:
	dp = &data->ospfIfTransitDelay;
	break;
#endif				       /* I_ospfIfTransitDelay */

#ifdef I_ospfIfRetransInterval
      case I_ospfIfRetransInterval:
	dp = &data->ospfIfRetransInterval;
	break;
#endif				       /* I_ospfIfRetransInterval */

#ifdef I_ospfIfHelloInterval
      case I_ospfIfHelloInterval:
	dp = &data->ospfIfHelloInterval;
	break;
#endif				       /* I_ospfIfHelloInterval */

#ifdef I_ospfIfRtrDeadInterval
      case I_ospfIfRtrDeadInterval:
	dp = &data->ospfIfRtrDeadInterval;
	break;
#endif				       /* I_ospfIfRtrDeadInterval */

#ifdef I_ospfIfPollInterval
      case I_ospfIfPollInterval:
	dp = &data->ospfIfPollInterval;
	break;
#endif				       /* I_ospfIfPollInterval */

#ifdef I_ospfIfState
      case I_ospfIfState:
	dp = &data->ospfIfState;
	break;
#endif				       /* I_ospfIfState */

#ifdef I_ospfIfDesignatedRouter
      case I_ospfIfDesignatedRouter:
	dp = IPToOctetString(data->ospfIfDesignatedRouter);
	break;
#endif				       /* I_ospfIfDesignatedRouter */

#ifdef I_ospfIfBackupDesignatedRouter
      case I_ospfIfBackupDesignatedRouter:
	dp = IPToOctetString(data->ospfIfBackupDesignatedRouter);
	break;
#endif				       /* I_ospfIfBackupDesignatedRouter */

#ifdef I_ospfIfEvents
      case I_ospfIfEvents:
	dp = &data->ospfIfEvents;
	break;
#endif				       /* I_ospfIfEvents */

#ifdef I_ospfIfAuthKey
      case I_ospfIfAuthKey:
	dp = MakeOctetString(data->ospfIfAuthKey->octet_ptr, data->ospfIfAuthKey->length);
	break;
#endif				       /* I_ospfIfAuthKey */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfIfEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfIfEntry_free(ospfIfEntry_t  *data)
{
    if (data != NULL) {
	if (data->ospfIfAuthKey != NULL) {
	    FreeOctetString(data->ospfIfAuthKey);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfIfEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfIfEntry_cleanup(doList_t *trash)
{
    ospfIfEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfIfEntry_free(trash->undodata);
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
ospfIfEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfIfEntry_t  *ospfIfEntry;
    unsigned long   ospfIfIpAddress;
    long            ospfAddressLessIf;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 5) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfIfIpAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfAddressLessIf, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfIfEntry = k_ospfIfEntry_get(-1, contextInfo, object->nominator, EXACT, ospfIfIpAddress, ospfAddressLessIf);

#ifndef ospfIfEntry_READ_CREATE

    if (ospfIfEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfIfEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfIfEntry_set) &&
	    (((ospfIfEntry_t *) (dp->data)) != NULL) &&
	    (((ospfIfEntry_t *) (dp->data))->ospfIfIpAddress == ospfIfIpAddress) &&
	    (((ospfIfEntry_t *) (dp->data))->ospfAddressLessIf == ospfAddressLessIf)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfIfEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfIfEntry_t));

	dp->setMethod = ospfIfEntry_set;
	dp->cleanupMethod = ospfIfEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfIfIpAddress, ((ospfIfEntry_t *) (dp->data))->valid);
	((ospfIfEntry_t *) (dp->data))->ospfIfIpAddress = ospfIfIpAddress;

	SET_VALID(I_ospfAddressLessIf, ((ospfIfEntry_t *) (dp->data))->valid);
	((ospfIfEntry_t *) (dp->data))->ospfAddressLessIf = ospfAddressLessIf;

#ifdef ospfIfEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfIfEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfIfEntry, sizeof(ospfIfEntry_t));

	    SET_ALL_VALID(((ospfIfEntry_t *) (dp->data))->valid);

	    ((ospfIfEntry_t *) (dp->data))->ospfIfAuthKey =
		CloneOctetString(ospfIfEntry->ospfIfAuthKey);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfIfEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfIfIpAddress
      case I_ospfIfIpAddress:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfIfIpAddress, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfIpAddress = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfIfIpAddress */

#ifdef I_ospfAddressLessIf
      case I_ospfAddressLessIf:

	SET_VALID(I_ospfAddressLessIf, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfAddressLessIf = value->sl_value;
	break;
#endif				       /* I_ospfAddressLessIf */

#ifdef I_ospfIfAreaId
      case I_ospfIfAreaId:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfIfAreaId, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfAreaId = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfIfAreaId */

#ifdef I_ospfIfType
      case I_ospfIfType:

	SET_VALID(I_ospfIfType, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfType = value->sl_value;
	break;
#endif				       /* I_ospfIfType */

#ifdef I_ospfIfAdminStat
      case I_ospfIfAdminStat:

	SET_VALID(I_ospfIfAdminStat, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfAdminStat = value->sl_value;
	break;
#endif				       /* I_ospfIfAdminStat */

#ifdef I_ospfIfRtrPriority
      case I_ospfIfRtrPriority:

	SET_VALID(I_ospfIfRtrPriority, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfRtrPriority = value->sl_value;
	break;
#endif				       /* I_ospfIfRtrPriority */

#ifdef I_ospfIfTransitDelay
      case I_ospfIfTransitDelay:

	SET_VALID(I_ospfIfTransitDelay, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfTransitDelay = value->sl_value;
	break;
#endif				       /* I_ospfIfTransitDelay */

#ifdef I_ospfIfRetransInterval
      case I_ospfIfRetransInterval:

	SET_VALID(I_ospfIfRetransInterval, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfRetransInterval = value->sl_value;
	break;
#endif				       /* I_ospfIfRetransInterval */

#ifdef I_ospfIfHelloInterval
      case I_ospfIfHelloInterval:

	SET_VALID(I_ospfIfHelloInterval, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfHelloInterval = value->sl_value;
	break;
#endif				       /* I_ospfIfHelloInterval */

#ifdef I_ospfIfRtrDeadInterval
      case I_ospfIfRtrDeadInterval:

	SET_VALID(I_ospfIfRtrDeadInterval, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfRtrDeadInterval = value->sl_value;
	break;
#endif				       /* I_ospfIfRtrDeadInterval */

#ifdef I_ospfIfPollInterval
      case I_ospfIfPollInterval:

	SET_VALID(I_ospfIfPollInterval, ((ospfIfEntry_t *) (dp->data))->valid);

	((ospfIfEntry_t *) (dp->data))->ospfIfPollInterval = value->sl_value;
	break;
#endif				       /* I_ospfIfPollInterval */

#ifdef I_ospfIfAuthKey
      case I_ospfIfAuthKey:

	SET_VALID(I_ospfIfAuthKey, ((ospfIfEntry_t *) (dp->data))->valid);

	if (((ospfIfEntry_t *) (dp->data))->ospfIfAuthKey != NULL) {
	    FreeOctetString(((ospfIfEntry_t *) (dp->data))->ospfIfAuthKey);
	}
	((ospfIfEntry_t *) (dp->data))->ospfIfAuthKey =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_ospfIfAuthKey */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfIfEntry_test)\n"));
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
ospfIfEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_ospfIfEntry_set((ospfIfEntry_t *) (doCur->data),
			      contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfIfMetricEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfIfMetricEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfIfMetricEntry_t *data = NULL;
    unsigned long   buffer[6];
    OID             inst;
    int             carry;
    unsigned long   ospfIfMetricIpAddress;
    long            ospfIfMetricAddressLessIf;
    long            ospfIfMetricTOS;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 6
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 6) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 5 + object->oid.length, &ospfIfMetricTOS, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT && ospfIfMetricTOS > 28) { /* MAX TOS is 28 */
	ospfIfMetricTOS = 0;
	carry = 1;
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfIfMetricAddressLessIf, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfIfMetricIpAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfIfMetricEntry_get(serialNum, contextInfo, arg, searchType, ospfIfMetricIpAddress, ospfIfMetricAddressLessIf, ospfIfMetricTOS)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 6;
	inst.oid_ptr[0] = (data->ospfIfMetricIpAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfIfMetricIpAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfIfMetricIpAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfIfMetricIpAddress >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfIfMetricAddressLessIf;
	inst.oid_ptr[5] = data->ospfIfMetricTOS;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfIfMetricIpAddress
      case I_ospfIfMetricIpAddress:
	dp = IPToOctetString(data->ospfIfMetricIpAddress);
	break;
#endif				       /* I_ospfIfMetricIpAddress */

#ifdef I_ospfIfMetricAddressLessIf
      case I_ospfIfMetricAddressLessIf:
	dp = &data->ospfIfMetricAddressLessIf;
	break;
#endif				       /* I_ospfIfMetricAddressLessIf */

#ifdef I_ospfIfMetricTOS
      case I_ospfIfMetricTOS:
	dp = &data->ospfIfMetricTOS;
	break;
#endif				       /* I_ospfIfMetricTOS */

#ifdef I_ospfIfMetricMetric
      case I_ospfIfMetricMetric:
	dp = &data->ospfIfMetricMetric;
	break;
#endif				       /* I_ospfIfMetricMetric */

#ifdef I_ospfIfMetricStatus
      case I_ospfIfMetricStatus:
	dp = &data->ospfIfMetricStatus;
	break;
#endif				       /* I_ospfIfMetricStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfIfMetricEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfIfMetricEntry_free(ospfIfMetricEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfIfMetricEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfIfMetricEntry_cleanup(doList_t *trash)
{
    ospfIfMetricEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfIfMetricEntry_free(trash->undodata);
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
ospfIfMetricEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfIfMetricEntry_t *ospfIfMetricEntry;
    unsigned long   ospfIfMetricIpAddress;
    long            ospfIfMetricAddressLessIf;
    long            ospfIfMetricTOS;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 6) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfIfMetricIpAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfIfMetricAddressLessIf, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 5 + object->oid.length, &ospfIfMetricTOS, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfIfMetricEntry = k_ospfIfMetricEntry_get(-1, contextInfo, object->nominator, EXACT, ospfIfMetricIpAddress, ospfIfMetricAddressLessIf, ospfIfMetricTOS);

#ifndef ospfIfMetricEntry_READ_CREATE

    if (ospfIfMetricEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfIfMetricEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfIfMetricEntry_set) &&
	    (((ospfIfMetricEntry_t *) (dp->data)) != NULL) &&
	    (((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricIpAddress == ospfIfMetricIpAddress) &&
	    (((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricAddressLessIf == ospfIfMetricAddressLessIf) &&
	    (((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricTOS == ospfIfMetricTOS)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfIfMetricEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfIfMetricEntry_t));

	dp->setMethod = ospfIfMetricEntry_set;
	dp->cleanupMethod = ospfIfMetricEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfIfMetricIpAddress, ((ospfIfMetricEntry_t *) (dp->data))->valid);
	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricIpAddress = ospfIfMetricIpAddress;

	SET_VALID(I_ospfIfMetricAddressLessIf, ((ospfIfMetricEntry_t *) (dp->data))->valid);
	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricAddressLessIf = ospfIfMetricAddressLessIf;

	SET_VALID(I_ospfIfMetricTOS, ((ospfIfMetricEntry_t *) (dp->data))->valid);
	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricTOS = ospfIfMetricTOS;

#ifdef ospfIfMetricEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfIfMetricEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfIfMetricEntry, sizeof(ospfIfMetricEntry_t));

	    SET_ALL_VALID(((ospfIfMetricEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfIfMetricEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfIfMetricIpAddress
      case I_ospfIfMetricIpAddress:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfIfMetricIpAddress, ((ospfIfMetricEntry_t *) (dp->data))->valid);

	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricIpAddress = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfIfMetricIpAddress */

#ifdef I_ospfIfMetricAddressLessIf
      case I_ospfIfMetricAddressLessIf:

	SET_VALID(I_ospfIfMetricAddressLessIf, ((ospfIfMetricEntry_t *) (dp->data))->valid);

	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricAddressLessIf = value->sl_value;
	break;
#endif				       /* I_ospfIfMetricAddressLessIf */

#ifdef I_ospfIfMetricTOS
      case I_ospfIfMetricTOS:

	SET_VALID(I_ospfIfMetricTOS, ((ospfIfMetricEntry_t *) (dp->data))->valid);

	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricTOS = value->sl_value;
	break;
#endif				       /* I_ospfIfMetricTOS */

#ifdef I_ospfIfMetricMetric
      case I_ospfIfMetricMetric:

	SET_VALID(I_ospfIfMetricMetric, ((ospfIfMetricEntry_t *) (dp->data))->valid);

	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricMetric = value->sl_value;
	break;
#endif				       /* I_ospfIfMetricMetric */

#ifdef I_ospfIfMetricStatus
      case I_ospfIfMetricStatus:

	SET_VALID(I_ospfIfMetricStatus, ((ospfIfMetricEntry_t *) (dp->data))->valid);

	((ospfIfMetricEntry_t *) (dp->data))->ospfIfMetricStatus = value->sl_value;
	break;
#endif				       /* I_ospfIfMetricStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfIfMetricEntry_test)\n"));
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
ospfIfMetricEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_ospfIfMetricEntry_set((ospfIfMetricEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfVirtIfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfVirtIfEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfVirtIfEntry_t *data = NULL;
    unsigned long   buffer[8];
    OID             inst;
    int             carry;
    unsigned long   ospfVirtIfAreaID;
    unsigned long   ospfVirtIfNeighbor;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 8
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 8) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 4 + object->oid.length, &ospfVirtIfNeighbor, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfVirtIfAreaID, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfVirtIfEntry_get(serialNum, contextInfo, arg, searchType, ospfVirtIfAreaID, ospfVirtIfNeighbor)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 8;
	inst.oid_ptr[0] = (data->ospfVirtIfAreaID >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfVirtIfAreaID >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfVirtIfAreaID >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfVirtIfAreaID >> 0) & 0xff;
	inst.oid_ptr[4] = (data->ospfVirtIfNeighbor >> 24) & 0xff;
	inst.oid_ptr[5] = (data->ospfVirtIfNeighbor >> 16) & 0xff;
	inst.oid_ptr[6] = (data->ospfVirtIfNeighbor >> 8) & 0xff;
	inst.oid_ptr[7] = (data->ospfVirtIfNeighbor >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfVirtIfAreaID
      case I_ospfVirtIfAreaID:
	dp = IPToOctetString(data->ospfVirtIfAreaID);
	break;
#endif				       /* I_ospfVirtIfAreaID */

#ifdef I_ospfVirtIfNeighbor
      case I_ospfVirtIfNeighbor:
	dp = IPToOctetString(data->ospfVirtIfNeighbor);
	break;
#endif				       /* I_ospfVirtIfNeighbor */

#ifdef I_ospfVirtIfTransitDelay
      case I_ospfVirtIfTransitDelay:
	dp = &data->ospfVirtIfTransitDelay;
	break;
#endif				       /* I_ospfVirtIfTransitDelay */

#ifdef I_ospfVirtIfRetransInterval
      case I_ospfVirtIfRetransInterval:
	dp = &data->ospfVirtIfRetransInterval;
	break;
#endif				       /* I_ospfVirtIfRetransInterval */

#ifdef I_ospfVirtIfHelloInterval
      case I_ospfVirtIfHelloInterval:
	dp = &data->ospfVirtIfHelloInterval;
	break;
#endif				       /* I_ospfVirtIfHelloInterval */

#ifdef I_ospfVirtIfRtrDeadInterval
      case I_ospfVirtIfRtrDeadInterval:
	dp = &data->ospfVirtIfRtrDeadInterval;
	break;
#endif				       /* I_ospfVirtIfRtrDeadInterval */

#ifdef I_ospfVirtIfState
      case I_ospfVirtIfState:
	dp = &data->ospfVirtIfState;
	break;
#endif				       /* I_ospfVirtIfState */

#ifdef I_ospfVirtIfEvents
      case I_ospfVirtIfEvents:
	dp = &data->ospfVirtIfEvents;
	break;
#endif				       /* I_ospfVirtIfEvents */

#ifdef I_ospfVirtIfAuthKey
      case I_ospfVirtIfAuthKey:
	dp = MakeOctetString(data->ospfVirtIfAuthKey->octet_ptr, data->ospfVirtIfAuthKey->length);
	break;
#endif				       /* I_ospfVirtIfAuthKey */

#ifdef I_ospfVirtIfStatus
      case I_ospfVirtIfStatus:
	dp = &data->ospfVirtIfStatus;
	break;
#endif				       /* I_ospfVirtIfStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfVirtIfEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfVirtIfEntry_free(ospfVirtIfEntry_t *data)
{
    if (data != NULL) {
	if (data->ospfVirtIfAuthKey != NULL) {
	    FreeOctetString(data->ospfVirtIfAuthKey);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfVirtIfEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfVirtIfEntry_cleanup(doList_t *trash)
{
    ospfVirtIfEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfVirtIfEntry_free(trash->undodata);
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
ospfVirtIfEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfVirtIfEntry_t *ospfVirtIfEntry;
    unsigned long   ospfVirtIfAreaID;
    unsigned long   ospfVirtIfNeighbor;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 8) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfVirtIfAreaID, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToIP(incoming, 4 + object->oid.length, &ospfVirtIfNeighbor, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfVirtIfEntry = k_ospfVirtIfEntry_get(-1, contextInfo, object->nominator, EXACT, ospfVirtIfAreaID, ospfVirtIfNeighbor);

#ifndef ospfVirtIfEntry_READ_CREATE

    if (ospfVirtIfEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfVirtIfEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfVirtIfEntry_set) &&
	    (((ospfVirtIfEntry_t *) (dp->data)) != NULL) &&
	    (((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAreaID == ospfVirtIfAreaID) &&
	    (((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfNeighbor == ospfVirtIfNeighbor)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfVirtIfEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfVirtIfEntry_t));

	dp->setMethod = ospfVirtIfEntry_set;
	dp->cleanupMethod = ospfVirtIfEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfVirtIfAreaID, ((ospfVirtIfEntry_t *) (dp->data))->valid);
	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAreaID = ospfVirtIfAreaID;

	SET_VALID(I_ospfVirtIfNeighbor, ((ospfVirtIfEntry_t *) (dp->data))->valid);
	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfNeighbor = ospfVirtIfNeighbor;

#ifdef ospfVirtIfEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfVirtIfEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfVirtIfEntry, sizeof(ospfVirtIfEntry_t));

	    SET_ALL_VALID(((ospfVirtIfEntry_t *) (dp->data))->valid);

	    ((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAuthKey =
		CloneOctetString(ospfVirtIfEntry->ospfVirtIfAuthKey);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfVirtIfEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfVirtIfAreaID
      case I_ospfVirtIfAreaID:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfVirtIfAreaID, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAreaID = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfVirtIfAreaID */

#ifdef I_ospfVirtIfNeighbor
      case I_ospfVirtIfNeighbor:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfVirtIfNeighbor, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfNeighbor = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfVirtIfNeighbor */

#ifdef I_ospfVirtIfTransitDelay
      case I_ospfVirtIfTransitDelay:

	SET_VALID(I_ospfVirtIfTransitDelay, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfTransitDelay = value->sl_value;
	break;
#endif				       /* I_ospfVirtIfTransitDelay */

#ifdef I_ospfVirtIfRetransInterval
      case I_ospfVirtIfRetransInterval:

	SET_VALID(I_ospfVirtIfRetransInterval, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfRetransInterval = value->sl_value;
	break;
#endif				       /* I_ospfVirtIfRetransInterval */

#ifdef I_ospfVirtIfHelloInterval
      case I_ospfVirtIfHelloInterval:

	SET_VALID(I_ospfVirtIfHelloInterval, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfHelloInterval = value->sl_value;
	break;
#endif				       /* I_ospfVirtIfHelloInterval */

#ifdef I_ospfVirtIfRtrDeadInterval
      case I_ospfVirtIfRtrDeadInterval:

	SET_VALID(I_ospfVirtIfRtrDeadInterval, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfRtrDeadInterval = value->sl_value;
	break;
#endif				       /* I_ospfVirtIfRtrDeadInterval */

#ifdef I_ospfVirtIfAuthKey
      case I_ospfVirtIfAuthKey:

	SET_VALID(I_ospfVirtIfAuthKey, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	if (((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAuthKey != NULL) {
	    FreeOctetString(((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAuthKey);
	}
	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfAuthKey =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_ospfVirtIfAuthKey */

#ifdef I_ospfVirtIfStatus
      case I_ospfVirtIfStatus:

	SET_VALID(I_ospfVirtIfStatus, ((ospfVirtIfEntry_t *) (dp->data))->valid);

	((ospfVirtIfEntry_t *) (dp->data))->ospfVirtIfStatus = value->sl_value;
	break;
#endif				       /* I_ospfVirtIfStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfVirtIfEntry_test)\n"));
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
ospfVirtIfEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_ospfVirtIfEntry_set((ospfVirtIfEntry_t *) (doCur->data),
				  contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfNbrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfNbrEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfNbrEntry_t *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    unsigned long   ospfNbrIpAddr;
    long            ospfNbrAddressLessIndex;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 5
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 5) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 4 + object->oid.length, &ospfNbrAddressLessIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfNbrIpAddr, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfNbrEntry_get(serialNum, contextInfo, arg, searchType, ospfNbrIpAddr, ospfNbrAddressLessIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 5;
	inst.oid_ptr[0] = (data->ospfNbrIpAddr >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfNbrIpAddr >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfNbrIpAddr >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfNbrIpAddr >> 0) & 0xff;
	inst.oid_ptr[4] = data->ospfNbrAddressLessIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfNbrIpAddr
      case I_ospfNbrIpAddr:
	dp = IPToOctetString(data->ospfNbrIpAddr);
	break;
#endif				       /* I_ospfNbrIpAddr */

#ifdef I_ospfNbrAddressLessIndex
      case I_ospfNbrAddressLessIndex:
	dp = &data->ospfNbrAddressLessIndex;
	break;
#endif				       /* I_ospfNbrAddressLessIndex */

#ifdef I_ospfNbrRtrId
      case I_ospfNbrRtrId:
	dp = IPToOctetString(data->ospfNbrRtrId);
	break;
#endif				       /* I_ospfNbrRtrId */

#ifdef I_ospfNbrOptions
      case I_ospfNbrOptions:
	dp = &data->ospfNbrOptions;
	break;
#endif				       /* I_ospfNbrOptions */

#ifdef I_ospfNbrPriority
      case I_ospfNbrPriority:
	dp = &data->ospfNbrPriority;
	break;
#endif				       /* I_ospfNbrPriority */

#ifdef I_ospfNbrState
      case I_ospfNbrState:
	dp = &data->ospfNbrState;
	break;
#endif				       /* I_ospfNbrState */

#ifdef I_ospfNbrEvents
      case I_ospfNbrEvents:
	dp = &data->ospfNbrEvents;
	break;
#endif				       /* I_ospfNbrEvents */

#ifdef I_ospfNbrLSRetransQLen
      case I_ospfNbrLSRetransQLen:
	dp = &data->ospfNbrLSRetransQLen;
	break;
#endif				       /* I_ospfNbrLSRetransQLen */

#ifdef I_ospfNBMANbrStatus
      case I_ospfNBMANbrStatus:
	dp = &data->ospfNBMANbrStatus;
	break;
#endif				       /* I_ospfNBMANbrStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ospfNbrEntry data object.
 *---------------------------------------------------------------------*/
static void
ospfNbrEntry_free(ospfNbrEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ospfNbrEntry set/undo
 *---------------------------------------------------------------------*/
static int
ospfNbrEntry_cleanup(doList_t *trash)
{
    ospfNbrEntry_free(trash->data);
#ifdef SR_SNMPv2
    ospfNbrEntry_free(trash->undodata);
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
ospfNbrEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int             found;
    int             carry = 0;
    ospfNbrEntry_t *ospfNbrEntry;
    unsigned long   ospfNbrIpAddr;
    long            ospfNbrAddressLessIndex;

    return (NOT_WRITABLE_ERROR);

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 5) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfNbrIpAddr, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &ospfNbrAddressLessIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    ospfNbrEntry = k_ospfNbrEntry_get(-1, contextInfo, object->nominator, EXACT, ospfNbrIpAddr, ospfNbrAddressLessIndex);

#ifndef ospfNbrEntry_READ_CREATE

    if (ospfNbrEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ospfNbrEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ospfNbrEntry_set) &&
	    (((ospfNbrEntry_t *) (dp->data)) != NULL) &&
	(((ospfNbrEntry_t *) (dp->data))->ospfNbrIpAddr == ospfNbrIpAddr) &&
	    (((ospfNbrEntry_t *) (dp->data))->ospfNbrAddressLessIndex == ospfNbrAddressLessIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(ospfNbrEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(ospfNbrEntry_t));

	dp->setMethod = ospfNbrEntry_set;
	dp->cleanupMethod = ospfNbrEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_ospfNbrIpAddr, ((ospfNbrEntry_t *) (dp->data))->valid);
	((ospfNbrEntry_t *) (dp->data))->ospfNbrIpAddr = ospfNbrIpAddr;

	SET_VALID(I_ospfNbrAddressLessIndex, ((ospfNbrEntry_t *) (dp->data))->valid);
	((ospfNbrEntry_t *) (dp->data))->ospfNbrAddressLessIndex = ospfNbrAddressLessIndex;

#ifdef ospfNbrEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (ospfNbrEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, ospfNbrEntry, sizeof(ospfNbrEntry_t));

	    SET_ALL_VALID(((ospfNbrEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* ospfNbrEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_ospfNbrIpAddr
      case I_ospfNbrIpAddr:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_ospfNbrIpAddr, ((ospfNbrEntry_t *) (dp->data))->valid);

	((ospfNbrEntry_t *) (dp->data))->ospfNbrIpAddr = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_ospfNbrIpAddr */

#ifdef I_ospfNbrAddressLessIndex
      case I_ospfNbrAddressLessIndex:

	SET_VALID(I_ospfNbrAddressLessIndex, ((ospfNbrEntry_t *) (dp->data))->valid);

	((ospfNbrEntry_t *) (dp->data))->ospfNbrAddressLessIndex = value->sl_value;
	break;
#endif				       /* I_ospfNbrAddressLessIndex */

#ifdef I_ospfNbrPriority
      case I_ospfNbrPriority:

	SET_VALID(I_ospfNbrPriority, ((ospfNbrEntry_t *) (dp->data))->valid);

	((ospfNbrEntry_t *) (dp->data))->ospfNbrPriority = value->sl_value;
	break;
#endif				       /* I_ospfNbrPriority */

#ifdef I_ospfNBMANbrStatus
      case I_ospfNBMANbrStatus:

	SET_VALID(I_ospfNBMANbrStatus, ((ospfNbrEntry_t *) (dp->data))->valid);

	((ospfNbrEntry_t *) (dp->data))->ospfNBMANbrStatus = value->sl_value;
	break;
#endif				       /* I_ospfNBMANbrStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ospfNbrEntry_test)\n"));
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
ospfNbrEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_ospfNbrEntry_set((ospfNbrEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ospfVirtNbrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ospfVirtNbrEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ospfVirtNbrEntry_t *data = NULL;
    unsigned long   buffer[8];
    OID             inst;
    int             carry;
    unsigned long   ospfVirtNbrArea;
    unsigned long   ospfVirtNbrRtrId;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 8
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 8) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 4 + object->oid.length, &ospfVirtNbrRtrId, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ospfVirtNbrArea, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ospfVirtNbrEntry_get(serialNum, contextInfo, arg, searchType, ospfVirtNbrArea, ospfVirtNbrRtrId)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 8;
	inst.oid_ptr[0] = (data->ospfVirtNbrArea >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ospfVirtNbrArea >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ospfVirtNbrArea >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ospfVirtNbrArea >> 0) & 0xff;
	inst.oid_ptr[4] = (data->ospfVirtNbrRtrId >> 24) & 0xff;
	inst.oid_ptr[5] = (data->ospfVirtNbrRtrId >> 16) & 0xff;
	inst.oid_ptr[6] = (data->ospfVirtNbrRtrId >> 8) & 0xff;
	inst.oid_ptr[7] = (data->ospfVirtNbrRtrId >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ospfVirtNbrArea
      case I_ospfVirtNbrArea:
	dp = IPToOctetString(data->ospfVirtNbrArea);
	break;
#endif				       /* I_ospfVirtNbrArea */

#ifdef I_ospfVirtNbrRtrId
      case I_ospfVirtNbrRtrId:
	dp = IPToOctetString(data->ospfVirtNbrRtrId);
	break;
#endif				       /* I_ospfVirtNbrRtrId */

#ifdef I_ospfVirtNbrIpAddr
      case I_ospfVirtNbrIpAddr:
	dp = IPToOctetString(data->ospfVirtNbrIpAddr);
	break;
#endif				       /* I_ospfVirtNbrIpAddr */

#ifdef I_ospfVirtNbrOptions
      case I_ospfVirtNbrOptions:
	dp = &data->ospfVirtNbrOptions;
	break;
#endif				       /* I_ospfVirtNbrOptions */

#ifdef I_ospfVirtNbrState
      case I_ospfVirtNbrState:
	dp = &data->ospfVirtNbrState;
	break;
#endif				       /* I_ospfVirtNbrState */

#ifdef I_ospfVirtNbrEvents
      case I_ospfVirtNbrEvents:
	dp = &data->ospfVirtNbrEvents;
	break;
#endif				       /* I_ospfVirtNbrEvents */

#ifdef I_ospfVirtNbrLSRetransQLen
      case I_ospfVirtNbrLSRetransQLen:
	dp = &data->ospfVirtNbrLSRetransQLen;
	break;
#endif				       /* I_ospfVirtNbrLSRetransQLen */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}





ospfGeneralGroup_t *
k_ospfGeneralGroup_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    pdbtype *pdb;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    ospfGeneralGroupData.ospfRouterId = pdb->ospf_rtr_id;
    ospfGeneralGroupData.ospfAdminStat = ospf_running ? 
                                         D_ospfAdminStat_enabled : 
                                         D_ospfAdminStat_disabled;
    ospfGeneralGroupData.ospfVersionNumber = OSPF_VERSION;
    ospfGeneralGroupData.ospfAreaBdrRtrStatus = pdb->ospf_area_brtr ? 
                                         D_ospfAreaBdrRtrStatus_true :
                                         D_ospfAreaBdrRtrStatus_false;
    ospfGeneralGroupData.ospfASBdrRtrStatus = pdb->ospf_as_brtr ?
                                         D_ospfASBdrRtrStatus_true :
                                         D_ospfASBdrRtrStatus_false;
    ospfGeneralGroupData.ospfExternLSACount = pdb->mib_table[EXLSA_COUNT];
    ospfGeneralGroupData.ospfExternLSACksumSum = pdb->mib_table[EXLSA_CHKSUMSUM];
    ospfGeneralGroupData.ospfTOSSupport = pdb->ospf_tos_option ? 
                                         D_ospfTOSSupport_true : 
                                         D_ospfTOSSupport_false;
    ospfGeneralGroupData.ospfOriginateNewLSAs = pdb->mib_table[ORNLSA_COUNT];
    ospfGeneralGroupData.ospfRxNewLSAs = pdb->mib_table[RCNLSA_COUNT];

    return (&ospfGeneralGroupData);
}

#ifdef SETS
int
k_ospfGeneralGroup_set(
    ospfGeneralGroup_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfAreaEntry_t *
k_ospfAreaEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfAreaId)
{
    pdbtype *pdb;
    areatype *area = NULL, *cur_area = NULL;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    if (searchType == EXACT) {
	for (area = (areatype*)pdb->ospf_area_list; area != NULL; 
	     area = area->ar_next) 
	    if (area->ar_id == ospfAreaId)
		break;
    } else { /* get next */
	for (cur_area = (areatype*)pdb->ospf_area_list; cur_area != NULL; 
	     cur_area = cur_area->ar_next) {
	    if (cur_area->ar_id < ospfAreaId)
		continue;
	    else if (area != NULL) {
		if (cur_area->ar_id > area->ar_id)
		    continue;
	    }
	    area = cur_area;
	}
    }

    if (area == NULL)
        return(NULL);

    ospfAreaEntryData.ospfAreaId = area->ar_id;
    ospfAreaEntryData.ospfAuthType = area->ar_autype;
    ospfAreaEntryData.ospfImportASExtern = (!area->ar_stub) ?
                                            D_ospfImportASExtern_true :
                                            D_ospfImportASExtern_false;
    ospfAreaEntryData.ospfSpfRuns =         area->ar_ospfSpfRuns;     
    ospfAreaEntryData.ospfAreaBdrRtrCount = area->ar_bdr_count;
    ospfAreaEntryData.ospfASBdrRtrCount =   area->ar_asr_count;
    ospfAreaEntryData.ospfAreaLSACount =    area->ar_lsa_count;
    ospfAreaEntryData.ospfAreaLSACksumSum = area->ar_lsa_chksumsum;

    return (&ospfAreaEntryData);
}

#ifdef SETS
int
k_ospfAreaEntry_set(
    ospfAreaEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */


ospfStubAreaEntry_t *
k_ospfStubAreaEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfStubAreaID,
    long            ospfStubTOS)
{
    pdbtype *pdb;
    areatype *area = NULL, *cur_area = NULL;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    if (searchType == EXACT) {
	for (area = pdb->ospf_area_list; area != NULL; 
	     area = area->ar_next) 
	    if ((area->ar_stub) &&
		(area->ar_id == ospfStubAreaID) &&
		(area->ar_pdb_ptr->ospf_tos_option == ospfStubTOS))
		break;
    } else { /* get next */
	for (cur_area = pdb->ospf_area_list; cur_area != NULL; 
	     cur_area = cur_area->ar_next) {
	    if (cur_area->ar_stub) {
		if ((cur_area->ar_id < ospfStubAreaID) ||
		    (cur_area->ar_id == ospfStubAreaID && 
		     cur_area->ar_pdb_ptr->ospf_tos_option < ospfStubTOS))
		    continue;
		else if (area != NULL) {
		    if ((cur_area->ar_id > area->ar_id) ||
			((cur_area->ar_id == area->ar_id) &&
			 (cur_area->ar_pdb_ptr->ospf_tos_option > 
			 area->ar_pdb_ptr->ospf_tos_option)))
			continue;
		}
		area = cur_area;
	    }
	}
    }

    if (area == NULL)
        return(NULL);

    ospfStubAreaEntryData.ospfStubAreaID = area->ar_id;
    ospfStubAreaEntryData.ospfStubTOS = area->ar_pdb_ptr->ospf_tos_option;
    ospfStubAreaEntryData.ospfStubMetric = area->ar_stub_default_cost;
    ospfStubAreaEntryData.ospfStubStatus = D_ospfStubStatus_valid;

    return (&ospfStubAreaEntryData);
}

#ifdef SETS
int
k_ospfStubAreaEntry_set(
    ospfStubAreaEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */


ospfLsdbEntry_t *
k_ospfLsdbEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfLsdbAreaId,
    long            ospfLsdbType,
    unsigned long   ospfLsdbLSID,
    unsigned long   ospfLsdbRouterId)
{
    static OctetString LsdbAd;
    pdbtype* pdb;
    areatype *area = NULL, *cur_area = NULL;
    lsdbtype* db = NULL, *cur_db = NULL;
    int type = 0, cur_type = 0;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    if (searchType == EXACT) {
	for (area = pdb->ospf_area_list; area != NULL; 
	     area = area->ar_next) 
	    if (area->ar_id == ospfLsdbAreaId) {
		db = db_find_lsa(area, ospfLsdbLSID, ospfLsdbRouterId, ospfLsdbType);
		break;
	    }
    } else { /* get next */
	for (cur_area = pdb->ospf_area_list; cur_area != NULL; 
	     cur_area = cur_area->ar_next) {
	    if (cur_area->ar_id >= ospfLsdbAreaId) {
		if (cur_area->ar_id == ospfLsdbAreaId && ospfLsdbType > 1)
		    cur_type = ospfLsdbType; /* same area, start from req type */
		else
		    cur_type = LSA_TYPE_RTR; /* different area, start from 1 */
		for (; cur_type <= LSA_TYPE_ASE; cur_type++) { /* link state type upto 5 */
		    for (cur_db = 
			 db_get_first_db(cur_area->ar_lsdbase[cur_type]); 
			 cur_db; cur_db = db_get_next_db(cur_db)) { 
			if (cur_db->db_lsa_ptr == NULL) 
		     	    continue;
			if ((cur_area->ar_id == ospfLsdbAreaId && 
			     cur_type == ospfLsdbType &&
			     LSA_ID(cur_db) < ospfLsdbLSID) ||
			     (cur_area->ar_id == ospfLsdbAreaId && 
			     cur_type == ospfLsdbType &&
			     LSA_ID(cur_db) == ospfLsdbLSID &&
			     ADV_RTR(cur_db) < ospfLsdbRouterId))
			    continue;
			else if (area != NULL && db != NULL && db->db_lsa_ptr != NULL) {
			    if ((cur_area->ar_id > area->ar_id) ||
				(cur_area->ar_id == area->ar_id && cur_type > type) ||
				(cur_area->ar_id == area->ar_id && cur_type == type &&
				 LSA_ID(cur_db) > LSA_ID(db)) ||
				(cur_area->ar_id == area->ar_id && cur_type == type &&
				 LSA_ID(cur_db) == LSA_ID(db) && 
				 ADV_RTR(cur_db) > ADV_RTR(db)))
				continue;
			}
			area = cur_area;
			type = cur_type;
			db = cur_db;
			break;
		    }
		}
	    }
	}
    }
    if ((area == NULL) || (db == NULL) || (db->db_lsa_ptr == NULL))
        return(NULL);

    ospfLsdbEntryData.ospfLsdbAreaId = area->ar_id;
    ospfLsdbEntryData.ospfLsdbType = LSA_TYPE(db);
    ospfLsdbEntryData.ospfLsdbLSID = LSA_ID(db);
    ospfLsdbEntryData.ospfLsdbRouterId = ADV_RTR(db);
    ospfLsdbEntryData.ospfLsdbSequence = db->db_lsa_ptr->lsh_seq;
    ospfLsdbEntryData.ospfLsdbAge = REAL_AGE(db);
    ospfLsdbEntryData.ospfLsdbChecksum = db->db_lsa_ptr->lsh_chksum;
    ospfLsdbEntryData.ospfLsdbAdvertisement = &LsdbAd;
    LsdbAd.length = db->db_lsa_ptr->lsh_length;
    LsdbAd.octet_ptr = (uchar*) db->db_lsa_ptr;

    return (&ospfLsdbEntryData);
}

ospfAreaRangeEntry_t *
k_ospfAreaRangeEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfAreaRangeAreaID,
    unsigned long   ospfAreaRangeNet)
{
    pdbtype *pdb;
    areatype *area = NULL, *cur_area = NULL;
    addr_range *range = NULL, *cur_range = NULL;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    if (searchType == EXACT) {
	for (area = pdb->ospf_area_list; area != NULL; 
	     area = area->ar_next) 
	    for (range = area->ar_range_list; range != NULL;
		 range = range->net_next)
		if (area->ar_id == ospfAreaRangeAreaID && 
		    range->net_address == ospfAreaRangeNet)
		    goto range_found;
	range_found:
    } else { /* get next */
	for (cur_area = pdb->ospf_area_list; cur_area != NULL; 
	     cur_area = cur_area->ar_next) {
	    for (cur_range = cur_area->ar_range_list; cur_range != NULL;
		 cur_range = cur_range->net_next) {
		if ((cur_area->ar_id < ospfAreaRangeAreaID) ||
		    (cur_area->ar_id == ospfAreaRangeAreaID &&
		     cur_range->net_address < ospfAreaRangeNet))
		    continue;
		else if (area != NULL && range != NULL) {
		    if ((cur_area->ar_id > area->ar_id) ||
			(cur_area->ar_id == area->ar_id &&
			 cur_range->net_address > range->net_address))
			continue;
		}
		area = cur_area;
		range = cur_range;
	    }
	}
    }

    if (area == NULL || range == NULL)
        return(NULL);

    ospfAreaRangeEntryData.ospfAreaRangeAreaID = area->ar_id;
    ospfAreaRangeEntryData.ospfAreaRangeNet = range->net_address;
    ospfAreaRangeEntryData.ospfAreaRangeMask = range->net_mask;
    ospfAreaRangeEntryData.ospfAreaRangeStatus = D_ospfAreaRangeStatus_valid;

    return (&ospfAreaRangeEntryData);
}

#ifdef SETS
int
k_ospfAreaRangeEntry_set(
    ospfAreaRangeEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfHostEntry_t *
k_ospfHostEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfHostIpAddress,
    long            ospfHostTOS)
{
    pdbtype     *pdb;
    areatype    *area = NULL;
    ushort      i;
    link_info   *link = NULL, *cur_link = NULL;
    ushort      link_count;
    ushort      link_sz;
    lsdbtype    *lsdb;
    rtr_lsatype *rtr_lsa;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    if (searchType == EXACT) {
	for (area = pdb->ospf_area_list; area;area = area->ar_next) {
	    lsdb = db_find_lsa(area, pdb->ospf_rtr_id, pdb->ospf_rtr_id,
			       LSA_TYPE_RTR);
	    if (!lsdb)
		continue;
	    rtr_lsa = (rtr_lsatype *)lsdb->db_lsa_ptr;
	    if (!rtr_lsa)
		continue;
	    link_count = rtr_lsa->la_lnk_cnt;
	    link = &(rtr_lsa->la_link);
	    link_sz = LINK_INFO_SIZE;
	    for (i=0; i < link_count ;
		 i++, link=(link_info *)((uchar *)link + link_sz)) {
		if (link->lnk_con_type == LINK_IF_STUB_NET) {
		    if (link->lnk_data == 0xffffffff) {
			if (link->lnk_id == ospfHostIpAddress) 
			    goto host_found;
		    }
		}
	    }
	}
    host_found:
    } else { /* get next */
	for (area = pdb->ospf_area_list; area;area = area->ar_next) {
	    lsdb = db_find_lsa(area, pdb->ospf_rtr_id, pdb->ospf_rtr_id,
			       LSA_TYPE_RTR);
	    if (!lsdb)
		continue;
	    rtr_lsa = (rtr_lsatype *)lsdb->db_lsa_ptr;
	    if (!rtr_lsa)
		continue;
	    link_count = rtr_lsa->la_lnk_cnt;
	    cur_link = &(rtr_lsa->la_link);
	    link_sz = LINK_INFO_SIZE;
	    for (i=0; i < link_count ;
		 i++, cur_link=(link_info *)((uchar *)cur_link + link_sz)) {
		if (cur_link->lnk_con_type == LINK_IF_STUB_NET) {
		    if (cur_link->lnk_data == 0xffffffff) {
			if ((cur_link->lnk_id < ospfHostIpAddress) ||
			    (cur_link->lnk_id == ospfHostIpAddress &&
			     0 < ospfHostTOS)) 
			    continue;
			else if (link != NULL) {
			    if (cur_link->lnk_id > link->lnk_id) 
				continue;
			}
			link = cur_link;
		    }
		}
	    }
	}
    }

    if (link == NULL) 
	return (NULL);

    ospfHostEntryData.ospfHostIpAddress = link->lnk_id;
    ospfHostEntryData.ospfHostTOS = 0;
    ospfHostEntryData.ospfHostMetric = link->lnk_tos0_metric;
    /* There's RFC consistency problem, the MIB require metric to be
       greater than 0, but OSPF spec will report 0 cost in some cases */
    ospfHostEntryData.ospfHostStatus = D_ospfHostStatus_valid;

    return (&ospfHostEntryData);
}

#ifdef SETS
int
k_ospfHostEntry_set(
    ospfHostEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */


/*
 * ospf_to_mib_if_type
 * Convert OSPF internal if type value to MIB value
 */
static ushort ospf_to_mib_if_type (ushort ospf_if_type)
{
    switch (ospf_if_type) {
    case IF_BROADCAST: return OSPF_MIB_IFT_BROADCAST;
    case IF_NONBROADCAST: return OSPF_MIB_IFT_NBMA;
    case IF_LOOPBACK:
    case IF_POINT_TO_POINT: return OSPF_MIB_IFT_P_TO_P;
    case IF_POINT_TO_M_POINT: return OSPF_MIB_IFT_P_TO_MP;
    default: return 0;
    }
}


/*
 * ospf_to_mib_if_state
 * Convert OSPF internal if state value to MIB value
 */
static ushort ospf_to_mib_if_state (ushort ospf_if_state)
{
    switch (ospf_if_state) {
    case IFS_DOWN: return OSPF_MIB_IFS_DOWN;
    case IFS_LOOPBACK: return OSPF_MIB_IFS_LOOPBACK;
    case IFS_WAITING: return OSPF_MIB_IFS_WAITING;
    case IFS_POINT_TO_M_POINT:
    case IFS_POINT_TO_POINT: return OSPF_MIB_IFS_P_TO_P;
    case IFS_DR: return OSPF_MIB_IFS_DR;
    case IFS_BACKUP: return OSPF_MIB_IFS_BACKUP;
    case IFS_OTHER: return OSPF_MIB_IFS_OTHER;
    default: return 0;
    }
}

ospfIfEntry_t  *
k_ospfIfEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfIfIpAddress,
    long            ospfAddressLessIf)
{
    static OctetString AuthKey;
    idbtype *idb = NULL, *cur_idb = NULL;
    unsigned long ip_address = 0, cur_ip_address = 0;
    long if_index = 0, cur_if_index = 0;
    ospf_idb_info *ospf_info;

    if (searchType == EXACT) {
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_unnumbered || idb->ip_address == 0) {
		if (ospfIfIpAddress == 0 &&
		    idb->hwptr->snmp_if_index == ospfAddressLessIf) {
		    ip_address = 0;
		    if_index = idb->hwptr->snmp_if_index;
		    break;
		}
	    } else { /* ip address exists */
		if (idb->ip_address == ospfIfIpAddress &&
		    ospfAddressLessIf == 0) {
		    ip_address = idb->ip_address;
		    if_index = 0;
		    break;
		}
	    }
	}
    } else { /* get next */
	FOR_ALL_SWIDBS(cur_idb) {

	    if (cur_idb->ip_unnumbered || cur_idb->ip_address == 0) {
		cur_ip_address = 0;
		cur_if_index = cur_idb->hwptr->snmp_if_index;
	    } else {
		cur_if_index = 0;
		cur_ip_address = cur_idb->ip_address;
	    }

	    if ((cur_ip_address < ospfIfIpAddress) ||
		(cur_ip_address == ospfIfIpAddress &&
		 cur_if_index < ospfAddressLessIf))
		continue;
	    else if (idb != NULL) {
		if ((cur_ip_address > ip_address) ||
		    (cur_ip_address == ip_address &&
		     cur_if_index > if_index))
		    continue;
	    }
	    idb = cur_idb;
	    ip_address = cur_ip_address;
	    if_index = cur_if_index;
	}
    }

    if (idb == NULL)
	return (NULL);

    ospf_info = idb->ospf_info;
    if (!ospf_info) {
        ospf_info = &temp_ospf_info;
	memset(ospf_info, 0, sizeof(ospf_info));
        ospf_if_set_default(NULL, idb, ospf_info);
    }

    ospfIfEntryData.ospfIfIpAddress = ip_address;
    ospfIfEntryData.ospfAddressLessIf = if_index;
    if (ospf_info->area_ptr)
	ospfIfEntryData.ospfIfAreaId = ospf_info->area_ptr->ar_id;
    else
	ospfIfEntryData.ospfIfAreaId = 0; /* default to area 0 */
    ospfIfEntryData.ospfIfType = ospf_to_mib_if_type(ospf_info->if_type);
    ospfIfEntryData.ospfIfAdminStat = (ospf_info->enabled) ? 
	D_ospfIfAdminStat_enabled : D_ospfIfAdminStat_disabled;
    ospfIfEntryData.ospfIfRtrPriority = ospf_info->rtr_pri;
    ospfIfEntryData.ospfIfTransitDelay = ospf_info->if_transdly;
    ospfIfEntryData.ospfIfRetransInterval = ospf_info->retr_interval;
    ospfIfEntryData.ospfIfHelloInterval = ospf_info->hello_interval;
    ospfIfEntryData.ospfIfRtrDeadInterval = ospf_info->dead_interval;
    if (ospf_info->oi_nbr) {
	ospfIfEntryData.ospfIfPollInterval = 
	    ospf_info->oi_nbr->nbr_poll_interval;
    } else {
	ospfIfEntryData.ospfIfPollInterval = POLL_INTERVAL; /* No neighbor */
    }
    ospfIfEntryData.ospfIfState = ospf_to_mib_if_state(ospf_info->if_state);
    if (ospf_info->dr)
	ospfIfEntryData.ospfIfDesignatedRouter = ospf_info->dr->nbr_ipaddr;
    else
	ospfIfEntryData.ospfIfDesignatedRouter = 0;
    if (ospf_info->bdr)
	ospfIfEntryData.ospfIfBackupDesignatedRouter = ospf_info->bdr->nbr_ipaddr;
    else
	ospfIfEntryData.ospfIfBackupDesignatedRouter = 0;
    ospfIfEntryData.ospfIfEvents = ospf_info->if_event_count;
    ospfIfEntryData.ospfIfAuthKey = &AuthKey;
    AuthKey.length = 0;
    AuthKey.octet_ptr = NULL;

    return (&ospfIfEntryData);
}

#ifdef SETS
int
k_ospfIfEntry_set(
    ospfIfEntry_t  *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfIfMetricEntry_t *
k_ospfIfMetricEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfIfMetricIpAddress,
    long            ospfIfMetricAddressLessIf,
    long            ospfIfMetricTOS)
{
    idbtype *idb = NULL, *cur_idb = NULL;
    unsigned long ip_address = 0, cur_ip_address = 0;
    long if_index = 0, cur_if_index = 0;
    int TOS_index = 0, i = 0;
    ospf_idb_info *ospf_info;

    if (searchType == EXACT) {
	FOR_ALL_SWIDBS(idb) { /* TOS is 0, 4, 8, ...,28 */
	    if (idb->ip_unnumbered || idb->ip_address == 0) {
		if (ospfIfMetricIpAddress == 0 &&
		    idb->hwptr->snmp_if_index == ospfIfMetricAddressLessIf &&
		    ospfIfMetricTOS%4 == 0 &&
		    ospfIfMetricTOS/4 < 8 &&
		    ospfIfMetricTOS/4 >= 0) {
		    ip_address = 0;
		    if_index = idb->hwptr->snmp_if_index;
		    TOS_index = ospfIfMetricTOS/4;
		    break;
		}
	    } else { /* ip address exists */
		if (idb->ip_address == ospfIfMetricIpAddress &&
		    ospfIfMetricAddressLessIf == 0 &&
		    ospfIfMetricTOS%4 == 0 &&
		    ospfIfMetricTOS/4 < 8 &&
		    ospfIfMetricTOS/4 >= 0) {
		    ip_address = idb->ip_address;
		    if_index = 0;
		    TOS_index = ospfIfMetricTOS/4;
		    break;
		}
	    }
	}
    } else { /* get next */
	FOR_ALL_SWIDBS(cur_idb) {

	    if (cur_idb->ip_unnumbered || cur_idb->ip_address == 0) {
		cur_ip_address = 0;
		cur_if_index = cur_idb->hwptr->snmp_if_index;
	    } else {
		cur_if_index = 0;
		cur_ip_address = cur_idb->ip_address;
	    }

	    for (i = 0; i < 8; i++) {

		if ((cur_ip_address < ospfIfMetricIpAddress) ||
		    (cur_ip_address == ospfIfMetricIpAddress &&
		     cur_if_index < ospfIfMetricAddressLessIf) ||
		    (cur_ip_address == ospfIfMetricIpAddress &&
		     cur_if_index == ospfIfMetricAddressLessIf &&
		     (i*4) < ospfIfMetricTOS))
		    continue;
		else if (idb != NULL) {
		    if ((cur_ip_address > ip_address) ||
			(cur_ip_address == ip_address &&
			 cur_if_index > if_index) ||
			(cur_ip_address == ip_address &&
			 cur_if_index == if_index &&
			 (i*4) > TOS_index))
			continue;
		}
		idb = cur_idb;
		ip_address = cur_ip_address;
		if_index = cur_if_index;
		TOS_index = i;
	    }
	}
    }

    if (idb == NULL)
	return (NULL);

    ospf_info = idb->ospf_info;
    if (!ospf_info) { 
       ospf_info = &temp_ospf_info;
       memset(ospf_info, 0, sizeof(ospf_info));
       ospf_if_set_default(NULL, idb, ospf_info);
    }
    ospfIfMetricEntryData.ospfIfMetricIpAddress = ip_address;
    ospfIfMetricEntryData.ospfIfMetricAddressLessIf = if_index;
    ospfIfMetricEntryData.ospfIfMetricTOS = TOS_index * 4;
    ospfIfMetricEntryData.ospfIfMetricMetric = ospf_info->if_cost[TOS_index];
    ospfIfMetricEntryData.ospfIfMetricStatus = (ospf_info->enabled) ? 
	D_ospfIfMetricStatus_valid : D_ospfIfMetricStatus_invalid;

    return (&ospfIfMetricEntryData);
}

#ifdef SETS
int
k_ospfIfMetricEntry_set(
    ospfIfMetricEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfVirtIfEntry_t *
k_ospfVirtIfEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfVirtIfAreaID,
    unsigned long   ospfVirtIfNeighbor)
{
    static OctetString AuthKey;
    pdbtype *pdb;
    areatype *backbone;
    idbtype *idb = NULL, *cur_idb = NULL;
    ospf_idb_info *ospf_info;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    backbone = pdb->ospf_backbone;
    if (backbone == NULL)
	return (NULL);

    if (searchType == EXACT) {
	for (idb = backbone->ar_if_list; idb ; idb = idb->ospf_info->next) {
	    ospf_info = idb->ospf_info;
	    if (ospf_info->if_type == IF_VIRTUAL_LINK) {
		if (ospf_info->vlink_trans->ar_id == ospfVirtIfAreaID &&
		    ospf_info->vnbr_id == ospfVirtIfNeighbor)
		    break;
	    }
	}
    } else { /* get next */
	for (cur_idb = backbone->ar_if_list; cur_idb ; cur_idb = cur_idb->ospf_info->next) {
	    ospf_info = cur_idb->ospf_info;
	    if (ospf_info->if_type == IF_VIRTUAL_LINK) {
		if ((ospf_info->vlink_trans->ar_id < ospfVirtIfAreaID) ||
		    (ospf_info->vlink_trans->ar_id == ospfVirtIfAreaID &&
		     ospf_info->vnbr_id < ospfVirtIfNeighbor))
		    continue;
		else if (idb != NULL) {
		    if ( (ospf_info->vlink_trans->ar_id > 
			  ospf_info->vlink_trans->ar_id) ||
			(ospf_info->vlink_trans->ar_id ==
			 ospf_info->vlink_trans->ar_id &&
			 ospf_info->vnbr_id > ospf_info->vnbr_id)) 
			continue;
		}
		idb = cur_idb;
	    }
	}
    }

    if (idb == NULL)
        return (NULL);

    ospf_info = idb->ospf_info;
    ospfVirtIfEntryData.ospfVirtIfAreaID = ospf_info->vlink_trans->ar_id;
    ospfVirtIfEntryData.ospfVirtIfNeighbor = ospf_info->vnbr_id;
    ospfVirtIfEntryData.ospfVirtIfTransitDelay = ospf_info->if_transdly;
    ospfVirtIfEntryData.ospfVirtIfRetransInterval = ospf_info->retr_interval;
    ospfVirtIfEntryData.ospfVirtIfHelloInterval = ospf_info->hello_interval;
    ospfVirtIfEntryData.ospfVirtIfRtrDeadInterval = ospf_info->dead_interval;
    ospfVirtIfEntryData.ospfVirtIfState = (ospf_info->if_state) + 1;
    ospfVirtIfEntryData.ospfVirtIfEvents = ospf_info->if_event_count;
    ospfVirtIfEntryData.ospfVirtIfAuthKey = &AuthKey;
    AuthKey.length = 0;
    AuthKey.octet_ptr = NULL;
    ospfVirtIfEntryData.ospfVirtIfStatus = D_ospfVirtIfStatus_valid;

    return (&ospfVirtIfEntryData);
}

#ifdef SETS
int
k_ospfVirtIfEntry_set(
    ospfVirtIfEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfNbrEntry_t *
k_ospfNbrEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfNbrIpAddr,
    long            ospfNbrAddressLessIndex)
{
    nbrtype *nbr = NULL , *cur_nbr = NULL;
    idbtype *idb = NULL, *cur_idb = NULL;
    unsigned long ip_address = 0, cur_ip_address = 0;
    long if_index = 0, cur_if_index = 0;
    ospf_idb_info *ospf_info;

    if (searchType == EXACT) {
	FOR_ALL_SWIDBS(idb) {
	    ospf_info = idb->ospf_info;
	    if (!ospf_info)
		continue;
	    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
		if (idb->ip_unnumbered || idb->ip_address == 0) {
		    if (ospfNbrIpAddr == 0 &&
			idb->hwptr->snmp_if_index == ospfNbrAddressLessIndex) {
			ip_address = 0;
			if_index = idb->hwptr->snmp_if_index;
			goto nbr_found;
		    }
		} else { /* ip address exists */
		    if (nbr->nbr_ipaddr == ospfNbrIpAddr &&
			ospfNbrAddressLessIndex == 0) {
			ip_address = nbr->nbr_ipaddr;
			if_index = 0;
			goto nbr_found;
		    }
		}
	    }
	}
	nbr_found:
    } else { /* get next */
	FOR_ALL_SWIDBS(cur_idb) {

	    ospf_info = cur_idb->ospf_info;
	    
	    if (!ospf_info)
		continue;

	    if (cur_idb->ip_unnumbered || cur_idb->ip_address == 0) {
		cur_ip_address = 0;
		cur_if_index = cur_idb->hwptr->snmp_if_index;
	    }	    

	    for (cur_nbr = ospf_info->oi_nbr; cur_nbr; 
		 cur_nbr = cur_nbr->nbr_next) {
		
		if (!cur_idb->ip_unnumbered && cur_idb->ip_address != 0) {
		    cur_if_index = 0;
		    cur_ip_address = cur_nbr->nbr_ipaddr;
		}

		if ((cur_ip_address < ospfNbrIpAddr) ||
		    (cur_ip_address == ospfNbrIpAddr &&
		     cur_if_index < ospfNbrAddressLessIndex))
		    continue;
		else if (idb != NULL) {
		    if ((cur_ip_address > ip_address) ||
			(cur_ip_address == ip_address &&
			 cur_if_index > if_index))
			continue;
		}
		idb = cur_idb;
		nbr = cur_nbr;
		ip_address = cur_ip_address;
		if_index = cur_if_index;
	    }
	}
    }

    if (idb == NULL || nbr == NULL)
	return (NULL);

    ospfNbrEntryData.ospfNbrIpAddr = ip_address;
    ospfNbrEntryData.ospfNbrAddressLessIndex = if_index;
    ospfNbrEntryData.ospfNbrRtrId = nbr->nbr_id;
    ospfNbrEntryData.ospfNbrOptions = nbr->nbr_options;
    ospfNbrEntryData.ospfNbrPriority = nbr->nbr_pri;
    ospfNbrEntryData.ospfNbrState = (nbr->nbr_state) + 1;
    ospfNbrEntryData.ospfNbrEvents = nbr->nbr_event_count;
    ospfNbrEntryData.ospfNbrLSRetransQLen = nbr->nbr_retrans_queue.count;
    ospfNbrEntryData.ospfNBMANbrStatus = D_ospfNBMANbrStatus_valid;

    return (&ospfNbrEntryData);
}

#ifdef SETS
int
k_ospfNbrEntry_set(
    ospfNbrEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

ospfVirtNbrEntry_t *
k_ospfVirtNbrEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   ospfVirtNbrArea,
    unsigned long   ospfVirtNbrRtrId)
{
    pdbtype *pdb;
    areatype *backbone;
    idbtype *idb = NULL, *cur_idb = NULL;
    nbrtype *nbr = NULL;
    ospf_idb_info *ospf_info;

    pdb = ospf_pdbs_ptr;
    if (pdb == NULL)
        return (NULL);

    backbone = pdb->ospf_backbone;
    if (backbone == NULL)
	return (NULL);

    if (searchType == EXACT) {
	for (idb = backbone->ar_if_list; idb ; idb = idb->ospf_info->next) {
	    ospf_info = idb->ospf_info;
	    if (ospf_info->if_type == IF_VIRTUAL_LINK &&
		ospf_info->oi_nbr) {
		if (ospf_info->vlink_trans->ar_id == ospfVirtNbrArea &&
		    ospf_info->vnbr_id == ospfVirtNbrRtrId)
		    break;
	    }
	}
    } else { /* get next */
	for (cur_idb = backbone->ar_if_list; cur_idb ; cur_idb = cur_idb->ospf_info->next) {
	    ospf_info = cur_idb->ospf_info;
	    if (ospf_info->if_type == IF_VIRTUAL_LINK &&
		ospf_info->oi_nbr) {
		if ((ospf_info->vlink_trans->ar_id < ospfVirtNbrArea) ||
		    (ospf_info->vlink_trans->ar_id == ospfVirtNbrArea &&
		     ospf_info->vnbr_id < ospfVirtNbrRtrId))
		    continue;
		else if (idb != NULL) {
		    if ( (ospf_info->vlink_trans->ar_id > 
			  ospf_info->vlink_trans->ar_id) ||
			(ospf_info->vlink_trans->ar_id ==
			 ospf_info->vlink_trans->ar_id &&
			 ospf_info->vnbr_id > ospf_info->vnbr_id))
			continue;
		}
		idb = cur_idb;
	    }
	}
    }

    if (idb == NULL)
        return (NULL);

    ospf_info = idb->ospf_info;
    nbr = ospf_info->oi_nbr;

    ospfVirtNbrEntryData.ospfVirtNbrArea = ospf_info->vlink_trans->ar_id;
    ospfVirtNbrEntryData.ospfVirtNbrRtrId = ospf_info->vnbr_id;
    ospfVirtNbrEntryData.ospfVirtNbrIpAddr = ospf_info->vnbr_ipaddr;
    ospfVirtNbrEntryData.ospfVirtNbrOptions = nbr->nbr_options;
    ospfVirtNbrEntryData.ospfVirtNbrState = (nbr->nbr_state) + 1;
    ospfVirtNbrEntryData.ospfVirtNbrEvents = nbr->nbr_event_count;
    ospfVirtNbrEntryData.ospfVirtNbrLSRetransQLen = nbr->nbr_retrans_queue.count;

    return (&ospfVirtNbrEntryData);
}



/*
 * ospfmib subsystem header
 */

#define MAJVERSION_ospfmib 1
#define MINVERSION_ospfmib 0
#define EDITVERSION_ospfmib 0

SUBSYS_HEADER(ospfmib,
              MAJVERSION_ospfmib,
              MINVERSION_ospfmib,
              EDITVERSION_ospfmib,
              init_ospfmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: ospf",
              NULL);
      
