/* $Id: sr_dspumib.c,v 3.4.18.3 1996/07/03 20:41:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/sr_dspumib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * December 1994, Paul Pearce (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_dspumib.c,v $
 * Revision 3.4.18.3  1996/07/03  20:41:12  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.4.18.2  1996/04/03  00:11:57  jjohnson
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Implement a different mechanism for creating trap varbind lists
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/18  19:34:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/09  05:04:41  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.12.1  1996/02/20  00:47:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/09  22:12:36  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1995/12/14  08:23:26  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:06:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:56:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  21:06:36  rbatz
 * CSCdi36731:  DSPU accesses NULL pointer for SNMP access.
 *
 * Revision 2.2  1995/06/18  21:45:29  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:31:00  hampton
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
#include "address.h"
#include "dspumib_access.h"
#include "../snmp/snmp_api.h"
#include "sr_dspumib.h"
#include "sr_dspumib-mib.h"
#include "registry.h"
#include "snanm.h"
#include "../dspu/snanm_registry.regh"
#include "../parser/parser_defs_snmp.h"
#include "dspu_notification.h"
#include "../snmp/snmp_trap_fe.h"

typedef struct _vb_value {
    int type;
    void *value;
} vb_value;

void generateDSPUtrap(struct dspuTrap * pTrap);
static void do_dspu_trap(const OID *, const OID *, char *, OID *, 
                  int, int, int, vb_value *);


static char * trap_puStateChange = "ciscoDspuMIB.1.4.4.0.1";
static char * trap_puActivationFail = "ciscoDspuMIB.1.4.4.0.2";
static char * trap_luStateChange = "ciscoDspuMIB.1.5.3.0.1";
static char * trap_luActivationFail = "ciscoDspuMIB.1.5.3.0.2";

void
init_dspumib(void)
{
    load_mib(dspumib_OidList, dspumib_OidListNum);
    load_oid(dspumib_oid_table);
    register_snmp_trap(TRAP_DSPU, trap_puStateChange );
    register_snmp_trap(TRAP_DSPU, trap_puActivationFail );
    register_snmp_trap(TRAP_DSPU, trap_luStateChange );
    register_snmp_trap(TRAP_DSPU, trap_luActivationFail );
    reg_add_generateDSPUtrap (generateDSPUtrap, "generateDSPUtrap");
}


/*---------------------------------------------------------------------
 * Retrieve data from the dspuNode family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuNode_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    dspuNode_t     *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuNode_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuNodeRsrb
      case I_dspuNodeRsrb:
	dp = &data->dspuNodeRsrb;
	break;
#endif				       /* I_dspuNodeRsrb */

#ifdef I_dspuNodeRsrbLocalVirtualRing
      case I_dspuNodeRsrbLocalVirtualRing:
	dp = &data->dspuNodeRsrbLocalVirtualRing;
	break;
#endif				       /* I_dspuNodeRsrbLocalVirtualRing */

#ifdef I_dspuNodeRsrbBridgeNumber
      case I_dspuNodeRsrbBridgeNumber:
	dp = &data->dspuNodeRsrbBridgeNumber;
	break;
#endif				       /* I_dspuNodeRsrbBridgeNumber */

#ifdef I_dspuNodeRsrbTargetVirtualRing
      case I_dspuNodeRsrbTargetVirtualRing:
	dp = &data->dspuNodeRsrbTargetVirtualRing;
	break;
#endif				       /* I_dspuNodeRsrbTargetVirtualRing */

#ifdef I_dspuNodeRsrbVirtualMacAddress
      case I_dspuNodeRsrbVirtualMacAddress:
	dp = MakeOctetString(data->dspuNodeRsrbVirtualMacAddress->octet_ptr, data->dspuNodeRsrbVirtualMacAddress->length);
	break;
#endif				       /* I_dspuNodeRsrbVirtualMacAddress */

#ifdef I_dspuNodeDefaultPu
      case I_dspuNodeDefaultPu:
	dp = &data->dspuNodeDefaultPu;
	break;
#endif				       /* I_dspuNodeDefaultPu */

#ifdef I_dspuNodeDefaultPuWindowSize
      case I_dspuNodeDefaultPuWindowSize:
	dp = &data->dspuNodeDefaultPuWindowSize;
	break;
#endif				       /* I_dspuNodeDefaultPuWindowSize */

#ifdef I_dspuNodeDefaultPuMaxIframe
      case I_dspuNodeDefaultPuMaxIframe:
	dp = &data->dspuNodeDefaultPuMaxIframe;
	break;
#endif				       /* I_dspuNodeDefaultPuMaxIframe */

#ifdef I_dspuNodeActivationWindow
      case I_dspuNodeActivationWindow:
	dp = &data->dspuNodeActivationWindow;
	break;
#endif				       /* I_dspuNodeActivationWindow */

#ifdef I_dspuNodeLastConfigChgTime
      case I_dspuNodeLastConfigChgTime:
	dp = &data->dspuNodeLastConfigChgTime;
	break;
#endif				       /* I_dspuNodeLastConfigChgTime */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dspuNode data object.
 *---------------------------------------------------------------------*/
static void
dspuNode_free(dspuNode_t *data)
{
    if (data != NULL) {
	if (data->dspuNodeRsrbVirtualMacAddress != NULL) {
	    FreeOctetString(data->dspuNodeRsrbVirtualMacAddress);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dspuNode set/undo
 *---------------------------------------------------------------------*/
static int
dspuNode_cleanup(doList_t *trash)
{
    dspuNode_free(trash->data);
#ifdef SR_SNMPv2
    dspuNode_free(trash->undodata);
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
dspuNode_test(incoming, object, value, doHead, doCur, contextInfo)
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
	if ((dp->data = malloc(sizeof(dspuNode_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dspuNode_t));

	dp->setMethod = dspuNode_set;
	dp->cleanupMethod = dspuNode_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

    }
    switch (object->nominator) {

#ifdef I_dspuNodeRsrb
      case I_dspuNodeRsrb:

	SET_VALID(I_dspuNodeRsrb, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeRsrb = value->sl_value;
	break;
#endif				       /* I_dspuNodeRsrb */

#ifdef I_dspuNodeRsrbLocalVirtualRing
      case I_dspuNodeRsrbLocalVirtualRing:

	SET_VALID(I_dspuNodeRsrbLocalVirtualRing, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeRsrbLocalVirtualRing = value->sl_value;
	break;
#endif				       /* I_dspuNodeRsrbLocalVirtualRing */

#ifdef I_dspuNodeRsrbBridgeNumber
      case I_dspuNodeRsrbBridgeNumber:

	SET_VALID(I_dspuNodeRsrbBridgeNumber, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeRsrbBridgeNumber = value->sl_value;
	break;
#endif				       /* I_dspuNodeRsrbBridgeNumber */

#ifdef I_dspuNodeRsrbTargetVirtualRing
      case I_dspuNodeRsrbTargetVirtualRing:

	SET_VALID(I_dspuNodeRsrbTargetVirtualRing, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeRsrbTargetVirtualRing = value->sl_value;
	break;
#endif				       /* I_dspuNodeRsrbTargetVirtualRing */

#ifdef I_dspuNodeRsrbVirtualMacAddress
      case I_dspuNodeRsrbVirtualMacAddress:

	SET_VALID(I_dspuNodeRsrbVirtualMacAddress, ((dspuNode_t *) (dp->data))->valid);

	if (((dspuNode_t *) (dp->data))->dspuNodeRsrbVirtualMacAddress != NULL) {
	    FreeOctetString(((dspuNode_t *) (dp->data))->dspuNodeRsrbVirtualMacAddress);
	}
	((dspuNode_t *) (dp->data))->dspuNodeRsrbVirtualMacAddress =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dspuNodeRsrbVirtualMacAddress */

#ifdef I_dspuNodeDefaultPu
      case I_dspuNodeDefaultPu:

	SET_VALID(I_dspuNodeDefaultPu, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeDefaultPu = value->sl_value;
	break;
#endif				       /* I_dspuNodeDefaultPu */

#ifdef I_dspuNodeDefaultPuWindowSize
      case I_dspuNodeDefaultPuWindowSize:

	SET_VALID(I_dspuNodeDefaultPuWindowSize, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeDefaultPuWindowSize = value->sl_value;
	break;
#endif				       /* I_dspuNodeDefaultPuWindowSize */

#ifdef I_dspuNodeDefaultPuMaxIframe
      case I_dspuNodeDefaultPuMaxIframe:

	SET_VALID(I_dspuNodeDefaultPuMaxIframe, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeDefaultPuMaxIframe = value->sl_value;
	break;
#endif				       /* I_dspuNodeDefaultPuMaxIframe */

#ifdef I_dspuNodeActivationWindow
      case I_dspuNodeActivationWindow:

	SET_VALID(I_dspuNodeActivationWindow, ((dspuNode_t *) (dp->data))->valid);

	((dspuNode_t *) (dp->data))->dspuNodeActivationWindow = value->sl_value;
	break;
#endif				       /* I_dspuNodeActivationWindow */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dspuNode_test)\n"));
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
dspuNode_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_dspuNode_set((dspuNode_t *) (doCur->data),
			   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dspuPoolClassEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuPoolClassEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuPoolClassEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dspuPoolClassIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPoolClassIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuPoolClassEntry_get(serialNum, contextInfo, arg, searchType, dspuPoolClassIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dspuPoolClassIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuPoolClassName
      case I_dspuPoolClassName:
	dp = MakeOctetString(data->dspuPoolClassName->octet_ptr, data->dspuPoolClassName->length);
	break;
#endif				       /* I_dspuPoolClassName */

#ifdef I_dspuPoolClassInactivityTimeout
      case I_dspuPoolClassInactivityTimeout:
	dp = &data->dspuPoolClassInactivityTimeout;
	break;
#endif				       /* I_dspuPoolClassInactivityTimeout */

#ifdef I_dspuPoolClassOperUpStreamLuDefs
      case I_dspuPoolClassOperUpStreamLuDefs:
	dp = &data->dspuPoolClassOperUpStreamLuDefs;
	break;
#endif				       /* I_dspuPoolClassOperUpStreamLuDefs */

#ifdef I_dspuPoolClassOperDnStreamLuDefs
      case I_dspuPoolClassOperDnStreamLuDefs:
	dp = &data->dspuPoolClassOperDnStreamLuDefs;
	break;
#endif				       /* I_dspuPoolClassOperDnStreamLuDefs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dspuPoolClassEntry data object.
 *---------------------------------------------------------------------*/
static void
dspuPoolClassEntry_free(dspuPoolClassEntry_t *data)
{
    if (data != NULL) {
	if (data->dspuPoolClassName != NULL) {
	    FreeOctetString(data->dspuPoolClassName);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dspuPoolClassEntry set/undo
 *---------------------------------------------------------------------*/
static int
dspuPoolClassEntry_cleanup(doList_t *trash)
{
    dspuPoolClassEntry_free(trash->data);
#ifdef SR_SNMPv2
    dspuPoolClassEntry_free(trash->undodata);
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
dspuPoolClassEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             carry = 0;
    dspuPoolClassEntry_t *dspuPoolClassEntry;
    long            dspuPoolClassIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPoolClassIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dspuPoolClassEntry = k_dspuPoolClassEntry_get(-1, contextInfo, object->nominator, EXACT, dspuPoolClassIndex);

#ifndef dspuPoolClassEntry_READ_CREATE

    if (dspuPoolClassEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dspuPoolClassEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dspuPoolClassEntry_set) &&
	    (((dspuPoolClassEntry_t *) (dp->data)) != NULL) &&
	    (((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassIndex == dspuPoolClassIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dspuPoolClassEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dspuPoolClassEntry_t));

	dp->setMethod = dspuPoolClassEntry_set;
	dp->cleanupMethod = dspuPoolClassEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dspuPoolClassIndex, ((dspuPoolClassEntry_t *) (dp->data))->valid);
	((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassIndex = dspuPoolClassIndex;

#ifdef dspuPoolClassEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dspuPoolClassEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dspuPoolClassEntry, sizeof(dspuPoolClassEntry_t));

	    SET_ALL_VALID(((dspuPoolClassEntry_t *) (dp->data))->valid);

	    ((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassName =
		CloneOctetString(dspuPoolClassEntry->dspuPoolClassName);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dspuPoolClassEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dspuPoolClassName
      case I_dspuPoolClassName:

	SET_VALID(I_dspuPoolClassName, ((dspuPoolClassEntry_t *) (dp->data))->valid);

	if (((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassName != NULL) {
	    FreeOctetString(((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassName);
	}
	((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dspuPoolClassName */

#ifdef I_dspuPoolClassInactivityTimeout
      case I_dspuPoolClassInactivityTimeout:

	SET_VALID(I_dspuPoolClassInactivityTimeout, ((dspuPoolClassEntry_t *) (dp->data))->valid);

	((dspuPoolClassEntry_t *) (dp->data))->dspuPoolClassInactivityTimeout = value->sl_value;
	break;
#endif				       /* I_dspuPoolClassInactivityTimeout */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dspuPoolClassEntry_test)\n"));
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
dspuPoolClassEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_dspuPoolClassEntry_set((dspuPoolClassEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dspuPooledLuEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuPooledLuEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuPooledLuEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            dspuPoolClassIndex;
    long            dspuPuOperIndex;
    long            dspuLuOperLuLocalAddress;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 3
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 3) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 2 + object->oid.length, &dspuLuOperLuLocalAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dspuPuOperIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPoolClassIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuPooledLuEntry_get(serialNum, contextInfo, arg, searchType, dspuPoolClassIndex, dspuPuOperIndex, dspuLuOperLuLocalAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 3;
	inst.oid_ptr[0] = data->dspuPoolClassIndex;
	inst.oid_ptr[1] = data->dspuPuOperIndex;
	inst.oid_ptr[2] = data->dspuLuOperLuLocalAddress;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuPooledLuPeerPuIndex
      case I_dspuPooledLuPeerPuIndex:
	dp = &data->dspuPooledLuPeerPuIndex;
	break;
#endif				       /* I_dspuPooledLuPeerPuIndex */

#ifdef I_dspuPooledLuPeerLuLocalAddress
      case I_dspuPooledLuPeerLuLocalAddress:
	dp = &data->dspuPooledLuPeerLuLocalAddress;
	break;
#endif				       /* I_dspuPooledLuPeerLuLocalAddress */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dspuPuAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuPuAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuPuAdminEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dspuPuAdminIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuAdminIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuPuAdminEntry_get(serialNum, contextInfo, arg, searchType, dspuPuAdminIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dspuPuAdminIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuPuAdminName
      case I_dspuPuAdminName:
	dp = MakeOctetString(data->dspuPuAdminName->octet_ptr, data->dspuPuAdminName->length);
	break;
#endif				       /* I_dspuPuAdminName */

#ifdef I_dspuPuAdminType
      case I_dspuPuAdminType:
	dp = &data->dspuPuAdminType;
	break;
#endif				       /* I_dspuPuAdminType */

#ifdef I_dspuPuAdminRemoteMacAddress
      case I_dspuPuAdminRemoteMacAddress:
	dp = MakeOctetString(data->dspuPuAdminRemoteMacAddress->octet_ptr, data->dspuPuAdminRemoteMacAddress->length);
	break;
#endif				       /* I_dspuPuAdminRemoteMacAddress */

#ifdef I_dspuPuAdminRemoteSapAddress
      case I_dspuPuAdminRemoteSapAddress:
	dp = &data->dspuPuAdminRemoteSapAddress;
	break;
#endif				       /* I_dspuPuAdminRemoteSapAddress */

#ifdef I_dspuPuAdminLocalSapAddress
      case I_dspuPuAdminLocalSapAddress:
	dp = &data->dspuPuAdminLocalSapAddress;
	break;
#endif				       /* I_dspuPuAdminLocalSapAddress */

#ifdef I_dspuPuAdminXid
      case I_dspuPuAdminXid:
	dp = &data->dspuPuAdminXid;
	break;
#endif				       /* I_dspuPuAdminXid */

#ifdef I_dspuPuAdminXidFmt
      case I_dspuPuAdminXidFmt:
	dp = &data->dspuPuAdminXidFmt;
	break;
#endif				       /* I_dspuPuAdminXidFmt */

#ifdef I_dspuPuAdminWindowSize
      case I_dspuPuAdminWindowSize:
	dp = &data->dspuPuAdminWindowSize;
	break;
#endif				       /* I_dspuPuAdminWindowSize */

#ifdef I_dspuPuAdminMaxIframe
      case I_dspuPuAdminMaxIframe:
	dp = &data->dspuPuAdminMaxIframe;
	break;
#endif				       /* I_dspuPuAdminMaxIframe */

#ifdef I_dspuPuAdminLinkRetryCount
      case I_dspuPuAdminLinkRetryCount:
	dp = &data->dspuPuAdminLinkRetryCount;
	break;
#endif				       /* I_dspuPuAdminLinkRetryCount */

#ifdef I_dspuPuAdminLinkRetryTimeout
      case I_dspuPuAdminLinkRetryTimeout:
	dp = &data->dspuPuAdminLinkRetryTimeout;
	break;
#endif				       /* I_dspuPuAdminLinkRetryTimeout */

#ifdef I_dspuPuAdminStartPu
      case I_dspuPuAdminStartPu:
	dp = &data->dspuPuAdminStartPu;
	break;
#endif				       /* I_dspuPuAdminStartPu */

#ifdef I_dspuPuAdminDlcType
      case I_dspuPuAdminDlcType:
	dp = &data->dspuPuAdminDlcType;
	break;
#endif				       /* I_dspuPuAdminDlcType */

#ifdef I_dspuPuAdminDlcUnit
      case I_dspuPuAdminDlcUnit:
	dp = &data->dspuPuAdminDlcUnit;
	break;
#endif				       /* I_dspuPuAdminDlcUnit */

#ifdef I_dspuPuAdminDlcPort
      case I_dspuPuAdminDlcPort:
	dp = &data->dspuPuAdminDlcPort;
	break;
#endif				       /* I_dspuPuAdminDlcPort */

#ifdef I_dspuPuAdminFocalPoint
      case I_dspuPuAdminFocalPoint:
	dp = &data->dspuPuAdminFocalPoint;
	break;
#endif				       /* I_dspuPuAdminFocalPoint */

#ifdef I_dspuPuAdminRowStatus
      case I_dspuPuAdminRowStatus:
	dp = &data->dspuPuAdminRowStatus;
	break;
#endif				       /* I_dspuPuAdminRowStatus */

#ifdef I_dspuPuAdminRemoteAddress
      case I_dspuPuAdminRemoteAddress:
        dp = MakeOctetString(
                        data->dspuPuAdminRemoteAddress->octet_ptr,
                        data->dspuPuAdminRemoteAddress->length);
        break;
#endif /* I_dspuPuAdminRemoteAddress */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dspuPuAdminEntry data object.
 *---------------------------------------------------------------------*/
static void
dspuPuAdminEntry_free(dspuPuAdminEntry_t *data)
{
    if (data != NULL) {
	if (data->dspuPuAdminName != NULL) {
	    FreeOctetString(data->dspuPuAdminName);
	}
	if (data->dspuPuAdminRemoteMacAddress != NULL) {
	    FreeOctetString(data->dspuPuAdminRemoteMacAddress);
	}
        if (data->dspuPuAdminRemoteAddress != NULL) {
            FreeOctetString(data->dspuPuAdminRemoteAddress);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dspuPuAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
dspuPuAdminEntry_cleanup(doList_t *trash)
{
    dspuPuAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
    dspuPuAdminEntry_free(trash->undodata);
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
dspuPuAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             carry = 0;
    dspuPuAdminEntry_t *dspuPuAdminEntry;
    long            dspuPuAdminIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuAdminIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dspuPuAdminEntry = k_dspuPuAdminEntry_get(-1, contextInfo, object->nominator, EXACT, dspuPuAdminIndex);

#ifndef dspuPuAdminEntry_READ_CREATE

    if (dspuPuAdminEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dspuPuAdminEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dspuPuAdminEntry_set) &&
	    (((dspuPuAdminEntry_t *) (dp->data)) != NULL) &&
	    (((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminIndex == dspuPuAdminIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dspuPuAdminEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dspuPuAdminEntry_t));

	dp->setMethod = dspuPuAdminEntry_set;
	dp->cleanupMethod = dspuPuAdminEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dspuPuAdminIndex, ((dspuPuAdminEntry_t *) (dp->data))->valid);
	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminIndex = dspuPuAdminIndex;

#ifdef dspuPuAdminEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dspuPuAdminEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dspuPuAdminEntry, sizeof(dspuPuAdminEntry_t));

	    SET_ALL_VALID(((dspuPuAdminEntry_t *) (dp->data))->valid);

	    ((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminName =
		CloneOctetString(dspuPuAdminEntry->dspuPuAdminName);

	    ((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRemoteMacAddress =
		CloneOctetString(dspuPuAdminEntry->dspuPuAdminRemoteMacAddress);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dspuPuAdminEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dspuPuAdminName
      case I_dspuPuAdminName:

	SET_VALID(I_dspuPuAdminName, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	if (((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminName != NULL) {
	    FreeOctetString(((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminName);
	}
	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dspuPuAdminName */

#ifdef I_dspuPuAdminType
      case I_dspuPuAdminType:

	SET_VALID(I_dspuPuAdminType, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminType = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminType */

#ifdef I_dspuPuAdminRemoteMacAddress
      case I_dspuPuAdminRemoteMacAddress:

	SET_VALID(I_dspuPuAdminRemoteMacAddress, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	if (((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRemoteMacAddress != NULL) {
	    FreeOctetString(((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRemoteMacAddress);
	}
	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRemoteMacAddress =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dspuPuAdminRemoteMacAddress */

#ifdef I_dspuPuAdminRemoteSapAddress
      case I_dspuPuAdminRemoteSapAddress:

	SET_VALID(I_dspuPuAdminRemoteSapAddress, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRemoteSapAddress = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminRemoteSapAddress */

#ifdef I_dspuPuAdminLocalSapAddress
      case I_dspuPuAdminLocalSapAddress:

	SET_VALID(I_dspuPuAdminLocalSapAddress, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminLocalSapAddress = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminLocalSapAddress */

#ifdef I_dspuPuAdminXid
      case I_dspuPuAdminXid:

	SET_VALID(I_dspuPuAdminXid, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminXid = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminXid */

#ifdef I_dspuPuAdminXidFmt
      case I_dspuPuAdminXidFmt:

	SET_VALID(I_dspuPuAdminXidFmt, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminXidFmt = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminXidFmt */

#ifdef I_dspuPuAdminWindowSize
      case I_dspuPuAdminWindowSize:

	SET_VALID(I_dspuPuAdminWindowSize, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminWindowSize = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminWindowSize */

#ifdef I_dspuPuAdminMaxIframe
      case I_dspuPuAdminMaxIframe:

	SET_VALID(I_dspuPuAdminMaxIframe, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminMaxIframe = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminMaxIframe */

#ifdef I_dspuPuAdminLinkRetryCount
      case I_dspuPuAdminLinkRetryCount:

	SET_VALID(I_dspuPuAdminLinkRetryCount, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminLinkRetryCount = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminLinkRetryCount */

#ifdef I_dspuPuAdminLinkRetryTimeout
      case I_dspuPuAdminLinkRetryTimeout:

	SET_VALID(I_dspuPuAdminLinkRetryTimeout, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminLinkRetryTimeout = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminLinkRetryTimeout */

#ifdef I_dspuPuAdminStartPu
      case I_dspuPuAdminStartPu:

	SET_VALID(I_dspuPuAdminStartPu, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminStartPu = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminStartPu */

#ifdef I_dspuPuAdminDlcType
      case I_dspuPuAdminDlcType:

	SET_VALID(I_dspuPuAdminDlcType, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminDlcType = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminDlcType */

#ifdef I_dspuPuAdminDlcUnit
      case I_dspuPuAdminDlcUnit:

	SET_VALID(I_dspuPuAdminDlcUnit, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminDlcUnit = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminDlcUnit */

#ifdef I_dspuPuAdminDlcPort
      case I_dspuPuAdminDlcPort:

	SET_VALID(I_dspuPuAdminDlcPort, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminDlcPort = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminDlcPort */

#ifdef I_dspuPuAdminFocalPoint
      case I_dspuPuAdminFocalPoint:

	SET_VALID(I_dspuPuAdminFocalPoint, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminFocalPoint = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminFocalPoint */

#ifdef I_dspuPuAdminRowStatus
      case I_dspuPuAdminRowStatus:

	SET_VALID(I_dspuPuAdminRowStatus, ((dspuPuAdminEntry_t *) (dp->data))->valid);

	((dspuPuAdminEntry_t *) (dp->data))->dspuPuAdminRowStatus = value->sl_value;
	break;
#endif				       /* I_dspuPuAdminRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dspuPuAdminEntry_test)\n"));
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
dspuPuAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_dspuPuAdminEntry_set((dspuPuAdminEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dspuPuOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuPuOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuPuOperEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dspuPuOperIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuOperIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuPuOperEntry_get(serialNum, contextInfo, arg, searchType, dspuPuOperIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dspuPuOperIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuPuOperName
      case I_dspuPuOperName:
	dp = MakeOctetString(data->dspuPuOperName->octet_ptr, data->dspuPuOperName->length);
	break;
#endif				       /* I_dspuPuOperName */

#ifdef I_dspuPuOperType
      case I_dspuPuOperType:
	dp = &data->dspuPuOperType;
	break;
#endif				       /* I_dspuPuOperType */

#ifdef I_dspuPuOperRemoteMacAddress
      case I_dspuPuOperRemoteMacAddress:
	dp = MakeOctetString(data->dspuPuOperRemoteMacAddress->octet_ptr, data->dspuPuOperRemoteMacAddress->length);
	break;
#endif				       /* I_dspuPuOperRemoteMacAddress */

#ifdef I_dspuPuOperRemoteSapAddress
      case I_dspuPuOperRemoteSapAddress:
	dp = &data->dspuPuOperRemoteSapAddress;
	break;
#endif				       /* I_dspuPuOperRemoteSapAddress */

#ifdef I_dspuPuOperLocalSapAddress
      case I_dspuPuOperLocalSapAddress:
	dp = &data->dspuPuOperLocalSapAddress;
	break;
#endif				       /* I_dspuPuOperLocalSapAddress */

#ifdef I_dspuPuOperXid
      case I_dspuPuOperXid:
	dp = &data->dspuPuOperXid;
	break;
#endif				       /* I_dspuPuOperXid */

#ifdef I_dspuPuOperXidFmt
      case I_dspuPuOperXidFmt:
	dp = &data->dspuPuOperXidFmt;
	break;
#endif				       /* I_dspuPuOperXidFmt */

#ifdef I_dspuPuOperWindowSize
      case I_dspuPuOperWindowSize:
	dp = &data->dspuPuOperWindowSize;
	break;
#endif				       /* I_dspuPuOperWindowSize */

#ifdef I_dspuPuOperMaxIframe
      case I_dspuPuOperMaxIframe:
	dp = &data->dspuPuOperMaxIframe;
	break;
#endif				       /* I_dspuPuOperMaxIframe */

#ifdef I_dspuPuOperLinkRetryCount
      case I_dspuPuOperLinkRetryCount:
	dp = &data->dspuPuOperLinkRetryCount;
	break;
#endif				       /* I_dspuPuOperLinkRetryCount */

#ifdef I_dspuPuOperLinkRetryTimeout
      case I_dspuPuOperLinkRetryTimeout:
	dp = &data->dspuPuOperLinkRetryTimeout;
	break;
#endif				       /* I_dspuPuOperLinkRetryTimeout */

#ifdef I_dspuPuOperStartPu
      case I_dspuPuOperStartPu:
	dp = &data->dspuPuOperStartPu;
	break;
#endif				       /* I_dspuPuOperStartPu */

#ifdef I_dspuPuOperDlcType
      case I_dspuPuOperDlcType:
	dp = &data->dspuPuOperDlcType;
	break;
#endif				       /* I_dspuPuOperDlcType */

#ifdef I_dspuPuOperDlcUnit
      case I_dspuPuOperDlcUnit:
	dp = &data->dspuPuOperDlcUnit;
	break;
#endif				       /* I_dspuPuOperDlcUnit */

#ifdef I_dspuPuOperDlcPort
      case I_dspuPuOperDlcPort:
	dp = &data->dspuPuOperDlcPort;
	break;
#endif				       /* I_dspuPuOperDlcPort */

#ifdef I_dspuPuOperFocalPoint
      case I_dspuPuOperFocalPoint:
	dp = &data->dspuPuOperFocalPoint;
	break;
#endif				       /* I_dspuPuOperFocalPoint */

#ifdef I_dspuPuOperState
      case I_dspuPuOperState:
	dp = &data->dspuPuOperState;
	break;
#endif				       /* I_dspuPuOperState */

#ifdef I_dspuPuOperFsmState
      case I_dspuPuOperFsmState:
	dp = &data->dspuPuOperFsmState;
	break;
#endif				       /* I_dspuPuOperFsmState */

#ifdef I_dspuPuOperStartTime
      case I_dspuPuOperStartTime:
	dp = &data->dspuPuOperStartTime;
	break;
#endif				       /* I_dspuPuOperStartTime */

#ifdef I_dspuPuOperLastStateChgTime
      case I_dspuPuOperLastStateChgTime:
	dp = &data->dspuPuOperLastStateChgTime;
	break;
#endif				       /* I_dspuPuOperLastStateChgTime */

#ifdef I_dspuPuOperRemoteAddress
    case I_dspuPuOperRemoteAddress:
	dp = MakeOctetString(
                        data->dspuPuOperRemoteAddress->octet_ptr, 
                        data->dspuPuOperRemoteAddress->length);
       break;
#endif /* I_dspuPuOperRemoteMacAddress */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dspuPuStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuPuStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuPuStatsEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dspuPuOperIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuOperIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuPuStatsEntry_get(serialNum, contextInfo, arg, searchType, dspuPuOperIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dspuPuOperIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuPuStatsSentBytes
      case I_dspuPuStatsSentBytes:
	dp = &data->dspuPuStatsSentBytes;
	break;
#endif				       /* I_dspuPuStatsSentBytes */

#ifdef I_dspuPuStatsRcvdBytes
      case I_dspuPuStatsRcvdBytes:
	dp = &data->dspuPuStatsRcvdBytes;
	break;
#endif				       /* I_dspuPuStatsRcvdBytes */

#ifdef I_dspuPuStatsSentFrames
      case I_dspuPuStatsSentFrames:
	dp = &data->dspuPuStatsSentFrames;
	break;
#endif				       /* I_dspuPuStatsSentFrames */

#ifdef I_dspuPuStatsRcvdFrames
      case I_dspuPuStatsRcvdFrames:
	dp = &data->dspuPuStatsRcvdFrames;
	break;
#endif				       /* I_dspuPuStatsRcvdFrames */

#ifdef I_dspuPuStatsSentNegativeRsps
      case I_dspuPuStatsSentNegativeRsps:
	dp = &data->dspuPuStatsSentNegativeRsps;
	break;
#endif				       /* I_dspuPuStatsSentNegativeRsps */

#ifdef I_dspuPuStatsRcvdNegativeRsps
      case I_dspuPuStatsRcvdNegativeRsps:
	dp = &data->dspuPuStatsRcvdNegativeRsps;
	break;
#endif				       /* I_dspuPuStatsRcvdNegativeRsps */

#ifdef I_dspuPuStatsActiveLus
      case I_dspuPuStatsActiveLus:
	dp = &data->dspuPuStatsActiveLus;
	break;
#endif				       /* I_dspuPuStatsActiveLus */

#ifdef I_dspuPuStatsInactiveLus
      case I_dspuPuStatsInactiveLus:
	dp = &data->dspuPuStatsInactiveLus;
	break;
#endif				       /* I_dspuPuStatsInactiveLus */

#ifdef I_dspuPuStatsBindLus
      case I_dspuPuStatsBindLus:
	dp = &data->dspuPuStatsBindLus;
	break;
#endif				       /* I_dspuPuStatsBindLus */

#ifdef I_dspuPuStatsActivationFailures
      case I_dspuPuStatsActivationFailures:
	dp = &data->dspuPuStatsActivationFailures;
	break;
#endif				       /* I_dspuPuStatsActivationFailures */

#ifdef I_dspuPuStatsLastActivationFailureReason
      case I_dspuPuStatsLastActivationFailureReason:
	dp = &data->dspuPuStatsLastActivationFailureReason;
	break;
#endif				       /* I_dspuPuStatsLastActivationFailureRe
				        * ason */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dspuLuAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuLuAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuLuAdminEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dspuPuAdminIndex;
    long            dspuLuAdminLuLocalAddress;


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

    if ((InstToInt(incoming, 1 + object->oid.length, &dspuLuAdminLuLocalAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuAdminIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuLuAdminEntry_get(serialNum, contextInfo, arg, searchType, dspuPuAdminIndex, dspuLuAdminLuLocalAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dspuPuAdminIndex;
	inst.oid_ptr[1] = data->dspuLuAdminLuLocalAddress;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuLuAdminType
      case I_dspuLuAdminType:
	dp = &data->dspuLuAdminType;
	break;
#endif				       /* I_dspuLuAdminType */

#ifdef I_dspuLuAdminPoolClassName
      case I_dspuLuAdminPoolClassName:
	dp = MakeOctetString(data->dspuLuAdminPoolClassName->octet_ptr, data->dspuLuAdminPoolClassName->length);
	break;
#endif				       /* I_dspuLuAdminPoolClassName */

#ifdef I_dspuLuAdminPeerPuIndex
      case I_dspuLuAdminPeerPuIndex:
	dp = &data->dspuLuAdminPeerPuIndex;
	break;
#endif				       /* I_dspuLuAdminPeerPuIndex */

#ifdef I_dspuLuAdminPeerLuLocalAddress
      case I_dspuLuAdminPeerLuLocalAddress:
	dp = &data->dspuLuAdminPeerLuLocalAddress;
	break;
#endif				       /* I_dspuLuAdminPeerLuLocalAddress */

#ifdef I_dspuLuAdminRowStatus
      case I_dspuLuAdminRowStatus:
	dp = &data->dspuLuAdminRowStatus;
	break;
#endif				       /* I_dspuLuAdminRowStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dspuLuAdminEntry data object.
 *---------------------------------------------------------------------*/
static void
dspuLuAdminEntry_free(dspuLuAdminEntry_t *data)
{
    if (data != NULL) {
	if (data->dspuLuAdminPoolClassName != NULL) {
	    FreeOctetString(data->dspuLuAdminPoolClassName);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dspuLuAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
dspuLuAdminEntry_cleanup(doList_t *trash)
{
    dspuLuAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
    dspuLuAdminEntry_free(trash->undodata);
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
dspuLuAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             carry = 0;
    dspuLuAdminEntry_t *dspuLuAdminEntry;
    long            dspuPuAdminIndex;
    long            dspuLuAdminLuLocalAddress;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 2) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuAdminIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dspuLuAdminLuLocalAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dspuLuAdminEntry = k_dspuLuAdminEntry_get(-1, contextInfo, object->nominator, EXACT, dspuPuAdminIndex, dspuLuAdminLuLocalAddress);

#ifndef dspuLuAdminEntry_READ_CREATE

    if (dspuLuAdminEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dspuLuAdminEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dspuLuAdminEntry_set) &&
	    (((dspuLuAdminEntry_t *) (dp->data)) != NULL) &&
	    (((dspuLuAdminEntry_t *) (dp->data))->dspuPuAdminIndex == dspuPuAdminIndex) &&
	    (((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminLuLocalAddress == dspuLuAdminLuLocalAddress)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dspuLuAdminEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dspuLuAdminEntry_t));

	dp->setMethod = dspuLuAdminEntry_set;
	dp->cleanupMethod = dspuLuAdminEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dspuPuAdminIndex, ((dspuLuAdminEntry_t *) (dp->data))->valid);
	((dspuLuAdminEntry_t *) (dp->data))->dspuPuAdminIndex = dspuPuAdminIndex;

	SET_VALID(I_dspuLuAdminLuLocalAddress, ((dspuLuAdminEntry_t *) (dp->data))->valid);
	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminLuLocalAddress = dspuLuAdminLuLocalAddress;

#ifdef dspuLuAdminEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dspuLuAdminEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dspuLuAdminEntry, sizeof(dspuLuAdminEntry_t));

	    SET_ALL_VALID(((dspuLuAdminEntry_t *) (dp->data))->valid);

	    ((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPoolClassName =
		CloneOctetString(dspuLuAdminEntry->dspuLuAdminPoolClassName);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dspuLuAdminEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dspuLuAdminType
      case I_dspuLuAdminType:

	SET_VALID(I_dspuLuAdminType, ((dspuLuAdminEntry_t *) (dp->data))->valid);

	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminType = value->sl_value;
	break;
#endif				       /* I_dspuLuAdminType */

#ifdef I_dspuLuAdminPoolClassName
      case I_dspuLuAdminPoolClassName:

	SET_VALID(I_dspuLuAdminPoolClassName, ((dspuLuAdminEntry_t *) (dp->data))->valid);

	if (((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPoolClassName != NULL) {
	    FreeOctetString(((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPoolClassName);
	}
	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPoolClassName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dspuLuAdminPoolClassName */

#ifdef I_dspuLuAdminPeerPuIndex
      case I_dspuLuAdminPeerPuIndex:

	SET_VALID(I_dspuLuAdminPeerPuIndex, ((dspuLuAdminEntry_t *) (dp->data))->valid);

	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPeerPuIndex = value->sl_value;
	break;
#endif				       /* I_dspuLuAdminPeerPuIndex */

#ifdef I_dspuLuAdminPeerLuLocalAddress
      case I_dspuLuAdminPeerLuLocalAddress:

	SET_VALID(I_dspuLuAdminPeerLuLocalAddress, ((dspuLuAdminEntry_t *) (dp->data))->valid);

	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminPeerLuLocalAddress = value->sl_value;
	break;
#endif				       /* I_dspuLuAdminPeerLuLocalAddress */

#ifdef I_dspuLuAdminRowStatus
      case I_dspuLuAdminRowStatus:

	SET_VALID(I_dspuLuAdminRowStatus, ((dspuLuAdminEntry_t *) (dp->data))->valid);

	((dspuLuAdminEntry_t *) (dp->data))->dspuLuAdminRowStatus = value->sl_value;
	break;
#endif				       /* I_dspuLuAdminRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dspuLuAdminEntry_test)\n"));
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
dspuLuAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_dspuLuAdminEntry_set((dspuLuAdminEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dspuLuOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuLuOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuLuOperEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dspuPuOperIndex;
    long            dspuLuOperLuLocalAddress;


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

    if ((InstToInt(incoming, 1 + object->oid.length, &dspuLuOperLuLocalAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuPuOperIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuLuOperEntry_get(serialNum, contextInfo, arg, searchType, dspuPuOperIndex, dspuLuOperLuLocalAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dspuPuOperIndex;
	inst.oid_ptr[1] = data->dspuLuOperLuLocalAddress;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuLuOperType
      case I_dspuLuOperType:
	dp = &data->dspuLuOperType;
	break;
#endif				       /* I_dspuLuOperType */

#ifdef I_dspuLuOperPoolClassName
      case I_dspuLuOperPoolClassName:
	dp = MakeOctetString(data->dspuLuOperPoolClassName->octet_ptr, data->dspuLuOperPoolClassName->length);
	break;
#endif				       /* I_dspuLuOperPoolClassName */

#ifdef I_dspuLuOperPeerPuIndex
      case I_dspuLuOperPeerPuIndex:
	dp = &data->dspuLuOperPeerPuIndex;
	break;
#endif				       /* I_dspuLuOperPeerPuIndex */

#ifdef I_dspuLuOperPeerLuLocalAddress
      case I_dspuLuOperPeerLuLocalAddress:
	dp = &data->dspuLuOperPeerLuLocalAddress;
	break;
#endif				       /* I_dspuLuOperPeerLuLocalAddress */

#ifdef I_dspuLuOperState
      case I_dspuLuOperState:
	dp = &data->dspuLuOperState;
	break;
#endif				       /* I_dspuLuOperState */

#ifdef I_dspuLuOperFsmState
      case I_dspuLuOperFsmState:
	dp = &data->dspuLuOperFsmState;
	break;
#endif				       /* I_dspuLuOperFsmState */

#ifdef I_dspuLuOperSessionState
      case I_dspuLuOperSessionState:
	dp = &data->dspuLuOperSessionState;
	break;
#endif				       /* I_dspuLuOperSessionState */

#ifdef I_dspuLuOperLastActivationFailureReason
      case I_dspuLuOperLastActivationFailureReason:
        dp = &data->dspuLuOperLastActivationFailureReason;
        break;
#endif /* I_dspuLuOperLastActivationFailureReason */ 

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dspuSapEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dspuSapEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dspuSapEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    long            dspuSapDlcType;
    long            dspuSapDlcUnit;
    long            dspuSapDlcPort;
    long            dspuSapAddress;


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

    if ((InstToInt(incoming, 3 + object->oid.length, &dspuSapAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &dspuSapDlcPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dspuSapDlcUnit, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuSapDlcType, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dspuSapEntry_get(serialNum, contextInfo, arg, searchType, dspuSapDlcType, dspuSapDlcUnit, dspuSapDlcPort, dspuSapAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = data->dspuSapDlcType;
	inst.oid_ptr[1] = data->dspuSapDlcUnit;
	inst.oid_ptr[2] = data->dspuSapDlcPort;
	inst.oid_ptr[3] = data->dspuSapAddress;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dspuSapType
      case I_dspuSapType:
	dp = &data->dspuSapType;
	break;
#endif				       /* I_dspuSapType */

#ifdef I_dspuSapOperState
      case I_dspuSapOperState:
	dp = &data->dspuSapOperState;
	break;
#endif				       /* I_dspuSapOperState */

#ifdef I_dspuSapRowStatus
      case I_dspuSapRowStatus:
	dp = &data->dspuSapRowStatus;
	break;
#endif				       /* I_dspuSapRowStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dspuSapEntry data object.
 *---------------------------------------------------------------------*/
static void
dspuSapEntry_free(dspuSapEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dspuSapEntry set/undo
 *---------------------------------------------------------------------*/
static int
dspuSapEntry_cleanup(doList_t *trash)
{
    dspuSapEntry_free(trash->data);
#ifdef SR_SNMPv2
    dspuSapEntry_free(trash->undodata);
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
dspuSapEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             carry = 0;
    dspuSapEntry_t *dspuSapEntry;
    long            dspuSapDlcType;
    long            dspuSapDlcUnit;
    long            dspuSapDlcPort;
    long            dspuSapAddress;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 4) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dspuSapDlcType, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dspuSapDlcUnit, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &dspuSapDlcPort, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 3 + object->oid.length, &dspuSapAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dspuSapEntry = k_dspuSapEntry_get(-1, contextInfo, object->nominator, EXACT, dspuSapDlcType, dspuSapDlcUnit, dspuSapDlcPort, dspuSapAddress);

#ifndef dspuSapEntry_READ_CREATE

    if (dspuSapEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dspuSapEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dspuSapEntry_set) &&
	    (((dspuSapEntry_t *) (dp->data)) != NULL) &&
	    (((dspuSapEntry_t *) (dp->data))->dspuSapDlcType == dspuSapDlcType) &&
	    (((dspuSapEntry_t *) (dp->data))->dspuSapDlcUnit == dspuSapDlcUnit) &&
	    (((dspuSapEntry_t *) (dp->data))->dspuSapDlcPort == dspuSapDlcPort) &&
	    (((dspuSapEntry_t *) (dp->data))->dspuSapAddress == dspuSapAddress)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dspuSapEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dspuSapEntry_t));

	dp->setMethod = dspuSapEntry_set;
	dp->cleanupMethod = dspuSapEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dspuSapDlcType, ((dspuSapEntry_t *) (dp->data))->valid);
	((dspuSapEntry_t *) (dp->data))->dspuSapDlcType = dspuSapDlcType;

	SET_VALID(I_dspuSapDlcUnit, ((dspuSapEntry_t *) (dp->data))->valid);
	((dspuSapEntry_t *) (dp->data))->dspuSapDlcUnit = dspuSapDlcUnit;

	SET_VALID(I_dspuSapDlcPort, ((dspuSapEntry_t *) (dp->data))->valid);
	((dspuSapEntry_t *) (dp->data))->dspuSapDlcPort = dspuSapDlcPort;

	SET_VALID(I_dspuSapAddress, ((dspuSapEntry_t *) (dp->data))->valid);
	((dspuSapEntry_t *) (dp->data))->dspuSapAddress = dspuSapAddress;

#ifdef dspuSapEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dspuSapEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dspuSapEntry, sizeof(dspuSapEntry_t));

	    SET_ALL_VALID(((dspuSapEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dspuSapEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dspuSapType
      case I_dspuSapType:

	SET_VALID(I_dspuSapType, ((dspuSapEntry_t *) (dp->data))->valid);

	((dspuSapEntry_t *) (dp->data))->dspuSapType = value->sl_value;
	break;
#endif				       /* I_dspuSapType */

#ifdef I_dspuSapRowStatus
      case I_dspuSapRowStatus:

	SET_VALID(I_dspuSapRowStatus, ((dspuSapEntry_t *) (dp->data))->valid);

	((dspuSapEntry_t *) (dp->data))->dspuSapRowStatus = value->sl_value;
	break;
#endif				       /* I_dspuSapRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dspuSapEntry_test)\n"));
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
dspuSapEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_dspuSapEntry_set((dspuSapEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				       /* SETS */






dspuNode_t     *
k_dspuNode_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{

    static dspuNode_t dspuNodeData;

    static OctetString rsrbVirtualMacAddr;


    dspuNodeData.dspuNodeRsrbVirtualMacAddress = &rsrbVirtualMacAddr;
    if (GetDspuNode(&dspuNodeData)) 
      return (&dspuNodeData);
    else
      return(NULL);

} /* End k_dspuNodeEntry_get() */

#ifdef SETS
int
k_dspuNode_set(data, contextInfo, function)
    dspuNode_t     *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

dspuPoolClassEntry_t *
k_dspuPoolClassEntry_get(serialNum, contextInfo, nominator, searchType, dspuPoolClassIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPoolClassIndex;
{

    static dspuPoolClassEntry_t dspuPoolClassEntryData;

    static OctetString poolClassName;


    dspuPoolClassEntryData.dspuPoolClassIndex = dspuPoolClassIndex;
    dspuPoolClassEntryData.dspuPoolClassName = &poolClassName;
    if ( GetDspuPoolClassEntry(&dspuPoolClassEntryData, searchType) )
      return (&dspuPoolClassEntryData);
    else
      return(NULL);

} /* End k_dspuPoolClassEntry_get() */

#ifdef SETS
int
k_dspuPoolClassEntry_set(data, contextInfo, function)
    dspuPoolClassEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

dspuPooledLuEntry_t *
k_dspuPooledLuEntry_get(serialNum, contextInfo, nominator, searchType, dspuPoolClassIndex, dspuPuOperIndex, dspuLuOperLuLocalAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPoolClassIndex;
    long            dspuPuOperIndex;
    long            dspuLuOperLuLocalAddress;
{

    static dspuPooledLuEntry_t dspuPooledLuEntryData;

    dspuPooledLuEntryData.dspuPoolClassIndex = dspuPoolClassIndex;
    dspuPooledLuEntryData.dspuPuOperIndex = dspuPuOperIndex;
    dspuPooledLuEntryData.dspuLuOperLuLocalAddress = dspuLuOperLuLocalAddress;
    if ( GetDspuPooledLuEntry(&dspuPooledLuEntryData, searchType) )
      return (&dspuPooledLuEntryData);
    else
      return(NULL);

} /* End k_dspuPooledLuEntry_get() */

dspuPuAdminEntry_t *
k_dspuPuAdminEntry_get(serialNum, contextInfo, nominator, searchType, dspuPuAdminIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPuAdminIndex;
{
    static dspuPuAdminEntry_t dspuPuAdminEntryData;

    static OctetString puAdminName;
    static OctetString puAdminRemoteMacAddr;
    static OctetString   puAdminRemoteAddr;
    static unsigned char physMacAddr[STATIONLEN_MAX]; 
    static unsigned char physAddr[STATIONLEN_MAX];

    puAdminRemoteMacAddr.octet_ptr = physMacAddr;
    puAdminRemoteMacAddr.length = 0;
    puAdminRemoteAddr.octet_ptr = physAddr;
    puAdminRemoteAddr.length = 0;

    dspuPuAdminEntryData.dspuPuAdminIndex = dspuPuAdminIndex;
    dspuPuAdminEntryData.dspuPuAdminName = &puAdminName;
    dspuPuAdminEntryData.dspuPuAdminRemoteMacAddress = &puAdminRemoteMacAddr;
    dspuPuAdminEntryData.dspuPuAdminRemoteAddress = &puAdminRemoteAddr;
    if ( GetDspuPuAdminEntry(&dspuPuAdminEntryData, searchType) )
      return (&dspuPuAdminEntryData);
    else
      return(NULL);

} /* End k_dspuPuAdminEntry_get() */

#ifdef SETS
int
k_dspuPuAdminEntry_set(data, contextInfo, function)
    dspuPuAdminEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

dspuPuOperEntry_t *
k_dspuPuOperEntry_get(serialNum, contextInfo, nominator, searchType, dspuPuOperIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPuOperIndex;
{

    static dspuPuOperEntry_t dspuPuOperEntryData;

    static OctetString puOperName;
    static OctetString puOperRemoteMacAddr;
    static unsigned char physMacAddr[STATIONLEN_MAX];
    static OctetString puOperRemoteAddr;
    static unsigned char physAddr[STATIONLEN_MAX];

    puOperRemoteMacAddr.octet_ptr = physMacAddr;
    puOperRemoteMacAddr.length = 0;
    puOperRemoteAddr.octet_ptr = physAddr;
    puOperRemoteAddr.length = 0;

    dspuPuOperEntryData.dspuPuOperIndex = dspuPuOperIndex;
    dspuPuOperEntryData.dspuPuOperName = &puOperName;
    dspuPuOperEntryData.dspuPuOperRemoteMacAddress = &puOperRemoteMacAddr;
    dspuPuOperEntryData.dspuPuOperRemoteAddress = &puOperRemoteAddr;
    if ( GetDspuPuOperEntry(&dspuPuOperEntryData, searchType) )
      return (&dspuPuOperEntryData);
    else
      return(NULL);

} /* End k_dspuPuOperEntry_get() */

dspuPuStatsEntry_t *
k_dspuPuStatsEntry_get(serialNum, contextInfo, nominator, searchType, dspuPuOperIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPuOperIndex;
{

    static dspuPuStatsEntry_t dspuPuStatsEntryData;


    dspuPuStatsEntryData.dspuPuOperIndex = dspuPuOperIndex;
    if ( GetDspuPuStatsEntry(&dspuPuStatsEntryData, searchType) )
      return (&dspuPuStatsEntryData);
    else
      return(NULL);

} /* End k_dspuPuStatsEntry_get() */

dspuLuAdminEntry_t *
k_dspuLuAdminEntry_get(serialNum, contextInfo, nominator, searchType, dspuPuAdminIndex, dspuLuAdminLuLocalAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPuAdminIndex;
    long            dspuLuAdminLuLocalAddress;
{

    static dspuLuAdminEntry_t dspuLuAdminEntryData;

    static OctetString luAdminPoolClassName;


    dspuLuAdminEntryData.dspuPuAdminIndex = dspuPuAdminIndex;
    dspuLuAdminEntryData.dspuLuAdminLuLocalAddress = dspuLuAdminLuLocalAddress;
    dspuLuAdminEntryData.dspuLuAdminPoolClassName = &luAdminPoolClassName;
    if ( GetDspuLuAdminEntry(&dspuLuAdminEntryData, searchType) )
      return (&dspuLuAdminEntryData);
    else
      return(NULL);

} /* End k_dspuLuAdminEntry_get() */

#ifdef SETS
int
k_dspuLuAdminEntry_set(data, contextInfo, function)
    dspuLuAdminEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

dspuLuOperEntry_t *
k_dspuLuOperEntry_get(serialNum, contextInfo, nominator, searchType, dspuPuOperIndex, dspuLuOperLuLocalAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuPuOperIndex;
    long            dspuLuOperLuLocalAddress;
{

    static dspuLuOperEntry_t dspuLuOperEntryData;

    static OctetString luOperPoolClassName;


    dspuLuOperEntryData.dspuPuOperIndex = dspuPuOperIndex;
    dspuLuOperEntryData.dspuLuOperLuLocalAddress = dspuLuOperLuLocalAddress;
    dspuLuOperEntryData.dspuLuOperPoolClassName = &luOperPoolClassName;
    if ( GetDspuLuOperEntry(&dspuLuOperEntryData, searchType) )
      return (&dspuLuOperEntryData);
    else
      return(NULL);

} /* End k_dspuLuOperEntry_get() */

dspuSapEntry_t *
k_dspuSapEntry_get(serialNum, contextInfo, nominator, searchType, dspuSapDlcType, dspuSapDlcUnit, dspuSapDlcPort, dspuSapAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            dspuSapDlcType;
    long            dspuSapDlcUnit;
    long            dspuSapDlcPort;
    long            dspuSapAddress;
{

    static dspuSapEntry_t dspuSapEntryData;


    dspuSapEntryData.dspuSapDlcType = dspuSapDlcType;
    dspuSapEntryData.dspuSapDlcUnit = dspuSapDlcUnit;
    dspuSapEntryData.dspuSapDlcPort = dspuSapDlcPort;
    dspuSapEntryData.dspuSapAddress = dspuSapAddress;
    if ( GetDspuSapEntry(&dspuSapEntryData, searchType) )
      return (&dspuSapEntryData);
    else
      return(NULL);

} /* End k_dspuSapEntry_get() */

#ifdef SETS
int
k_dspuSapEntry_set(data, contextInfo, function)
    dspuSapEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}
#endif				       /* SETS */


#define PU_STATE_CHANGE_VBCOUNT 2
#define PU_STATE_CHANGE_TRAP_NUM 1

static const OID varbinds_puStateChange[PU_STATE_CHANGE_VBCOUNT] = {
    OIDconversion(dspuPuOperName),
    OIDconversion(dspuPuOperState)
};

static const OID enterprise_puTrap = {LNdspuPuTrapsPrefix, 
                                     (ulong *)IDdspuPuTrapsPrefix};

/*=====================================================================*
* PuStateChangeTrap
*======================================================================*
*
* Description:
*   Generates SNMP trap when dspuPuOperState changes.
*
**/
void PuStateChangeTrap (struct dspuTrap * pTrap)
{
  int       idx;
  OID       indexOID[PU_STATE_CHANGE_VBCOUNT];
  u_long    index[1];
  vb_value  dataP[PU_STATE_CHANGE_VBCOUNT];
  int       operState;
 
  /* Fill in the indexing information */
  index[0] =  pTrap->puIdx;
  for (idx = 0; idx < PU_STATE_CHANGE_VBCOUNT; idx++) {
       indexOID[idx].length = 1;
       indexOID[idx].oid_ptr = index;
  }

  /* Fill in the data pointers */
  dataP[0].value = MakeOctetString(pTrap->puName, MAX_ENTRYNAME_LN);
  dataP[0].type = OCTET_PRIM_TYPE;
  if (pTrap->stateOrReason)
      operState = D_dspuPuOperState_active;
  else
      operState = D_dspuPuOperState_inactive;
  dataP[1].value = &operState;
  dataP[1].type = INTEGER_TYPE;

  do_dspu_trap(varbinds_puStateChange, &enterprise_puTrap, 
               trap_puStateChange, indexOID, PU_STATE_CHANGE_VBCOUNT, 
               ENTERPRISE_TRAP, PU_STATE_CHANGE_TRAP_NUM, dataP);
  return;
}


#define PU_ACTIVATION_FAIL_VBCOUNT 3
#define PU_ACTIVATION_FAIL_TRAP_NUM 2

static const OID varbinds_puActivationFail[PU_ACTIVATION_FAIL_VBCOUNT] = {
    OIDconversion(dspuPuOperName),
    OIDconversion(dspuPuOperState),
    OIDconversion(dspuPuStatsLastActivationFailureReason)
};
 
/*=====================================================================*
* PuActFailureTrap
*======================================================================*
*
* Description:
*   Generates SNMP trap on PU activation failure
*
**/
void PuActFailureTrap (struct dspuTrap * pTrap)
{
  int       idx;
  OID       indexOID[PU_ACTIVATION_FAIL_VBCOUNT];
  u_long    index[1];
  vb_value  dataP[PU_ACTIVATION_FAIL_VBCOUNT];
  int       operState = D_dspuLuOperState_inactive;
  int       failRsn;

  /* Fill in the indexing information */
  index[0] =  pTrap->puIdx;
  for (idx = 0; idx < PU_ACTIVATION_FAIL_VBCOUNT; idx++) {
       indexOID[idx].length = 1;
       indexOID[idx].oid_ptr = index;
  }

  switch (pTrap->stateOrReason) {
     case PuActFailRsn_None:
          failRsn = D_dspuPuStatsLastActivationFailureReason_noError;
          break;
     case PuActFailRsn_InternalError:
          failRsn = D_dspuPuStatsLastActivationFailureReason_internalError;
          break;
     case PuActFailRsn_ConfigError:
          failRsn = D_dspuPuStatsLastActivationFailureReason_configurationError;
          break;
     case PuActFailRsn_NegativeRsp:
          failRsn = D_dspuPuStatsLastActivationFailureReason_puNegativeResponse;
          break;
     case PuActFailRsn_AlreadyActive:
          failRsn = D_dspuPuStatsLastActivationFailureReason_puAlreadyActive;
          break;
     case PuActFailRsn_Other:
     default:
          failRsn = D_dspuPuStatsLastActivationFailureReason_otherError;
          break;
  }

  /* Fill in the data pointers */
  dataP[0].value = MakeOctetString(pTrap->puName, MAX_ENTRYNAME_LN);
  dataP[0].type = OCTET_PRIM_TYPE;
  dataP[1].value = &operState;
  dataP[1].type = INTEGER_TYPE;
  dataP[2].value = &failRsn;
  dataP[2].type = INTEGER_TYPE;

  do_dspu_trap(varbinds_puActivationFail, &enterprise_puTrap,
               trap_puActivationFail, indexOID, PU_ACTIVATION_FAIL_VBCOUNT, 
               ENTERPRISE_TRAP, PU_ACTIVATION_FAIL_TRAP_NUM, dataP);
  return;

} /* End PuActFailureTrap() */


static const OID enterprise_luTrap = OIDconversion(dspuLuTrapsPrefix);

#define LU_STATE_CHANGE_VBCOUNT 2
#define LU_STATE_CHANGE_TRAP_NUM 1

static const OID varbinds_luStateChange[LU_STATE_CHANGE_VBCOUNT] = {
    OIDconversion(dspuPuOperName),
    OIDconversion(dspuLuOperState)
};
 
/*=====================================================================*
* LuStateChangeTrap
*======================================================================*
*
* Description:
*   Generates SNMP trap when LuOperState changes
*
**/
void LuStateChangeTrap (struct dspuTrap *pTrap)
{
  OID       indexOID[LU_STATE_CHANGE_VBCOUNT];
  u_long    index[2];
  vb_value  dataP[LU_STATE_CHANGE_VBCOUNT];
  int       operState;

  if (pTrap->stateOrReason)
      operState = D_dspuLuOperState_active;
  else
      operState = D_dspuLuOperState_inactive;

  index[0] =  pTrap->puIdx;
  index[1] =  pTrap->luIdx;

  /* Fill in an indexOID for each varBind */
  indexOID[0].length = 1;
  indexOID[0].oid_ptr = index;
  indexOID[1].length = 2;
  indexOID[1].oid_ptr = index;
  
  /* Fill in the data pointers */
  dataP[0].value = MakeOctetString(pTrap->puName, MAX_ENTRYNAME_LN);
  dataP[0].type = OCTET_PRIM_TYPE;
  dataP[1].value = &operState;
  dataP[1].type = INTEGER_TYPE;

  do_dspu_trap(varbinds_luStateChange, &enterprise_luTrap, 
               trap_luStateChange, indexOID, LU_STATE_CHANGE_VBCOUNT, 
               ENTERPRISE_TRAP, LU_STATE_CHANGE_TRAP_NUM, dataP);
   return;
 
} /* End LuStateChangeTrap() */

#define LU_ACTIVATION_FAIL_VBCOUNT 3
#define LU_ACTIVATION_FAIL_TRAP_NUM 2

static const OID varbinds_luActivationFail[LU_ACTIVATION_FAIL_VBCOUNT] = {
    OIDconversion(dspuPuOperName),
    OIDconversion(dspuLuOperState),
    OIDconversion(dspuLuOperLastActivationFailureReason)
};
 
/*=====================================================================*
* LuActFailureTrap
*======================================================================*
*
* Description:
*   Generates SNMP trap on Downstream LU activation failure
*
**/
void LuActFailureTrap (struct dspuTrap * pTrap)
{
  OID       indexOID[LU_ACTIVATION_FAIL_VBCOUNT];
  u_long    index[2];
  vb_value  dataP[LU_ACTIVATION_FAIL_VBCOUNT];
  int       operState = D_dspuLuOperState_inactive;
  int       failRsn;

  index[0] =  pTrap->puIdx;
  index[1] =  pTrap->luIdx;

  /* Fill in an indexOID for each varBind */
  indexOID[0].length  = 1;
  indexOID[0].oid_ptr = index;
  indexOID[1].length  = 2;
  indexOID[1].oid_ptr = index;
  indexOID[2].length  = 2;
  indexOID[2].oid_ptr = index;

  switch (pTrap->stateOrReason) {
     case LuActFailRsn_None:
          failRsn = D_dspuLuOperLastActivationFailureReason_noError;
          break;
     case LuActFailRsn_NegativeRsp:
          failRsn = D_dspuLuOperLastActivationFailureReason_luNegativeResponse;
          break;
     case LuActFailRsn_Other:
     default:
          failRsn = D_dspuLuOperLastActivationFailureReason_otherError;
          break;
  }

  /* Fill in the data pointers */
  dataP[0].value = MakeOctetString(pTrap->puName, MAX_ENTRYNAME_LN);
  dataP[0].type = OCTET_PRIM_TYPE;
  dataP[1].value = &operState;
  dataP[1].type = INTEGER_TYPE;
  dataP[2].value = &failRsn;
  dataP[2].type = INTEGER_TYPE;

  do_dspu_trap(varbinds_luActivationFail, &enterprise_luTrap, 
               trap_luActivationFail, indexOID, LU_ACTIVATION_FAIL_VBCOUNT,
               ENTERPRISE_TRAP, LU_ACTIVATION_FAIL_TRAP_NUM, dataP);
  return;

}

/*=====================================================================*
* generateDSPUtrap
*======================================================================*
*
* Description:
*   Interfaces with the DSPU notification code to send traps.
*
**/
void generateDSPUtrap (struct dspuTrap * pTrap)
{
    switch (pTrap->trapType) {
        case PU_STATECHANGE:
           PuStateChangeTrap(pTrap);
           break;
        case PU_ACTFAIL:
            PuActFailureTrap(pTrap);
            break;
        case LU_STATECHANGE:
            LuStateChangeTrap(pTrap);
            break;
        case LU_ACTFAIL:
            LuActFailureTrap(pTrap);
            break;
        default:
            break;
    }
}

/*=====================================================================*
* do_dspu_trap
*======================================================================*
*
* Description:
*   Interfaces with the SNMP code to do the actual sending of the trap.
*
**/
void do_dspu_trap (const OID*       vbList,
                   const OID*       entOid,
                   char*            v2TrapOID,
                   OID*             indexOID,
                   int              vbLength,
                   int              genericTrapNum,
                   int              specificTrapNum,
                   vb_value         dataP[] )
{
  int       vbIndex;
  VarBind   *previous = NULL;
  VarBind   *current;
  VarBind   *varBinds = NULL;
 
  for (vbIndex = 0; vbIndex < vbLength; vbIndex++) {  
       current = MakeVarBindWithValue((OID *) &vbList[vbIndex],
				      &indexOID[vbIndex],
				      dataP[vbIndex].type,
				      dataP[vbIndex].value);
       if (current) {
	   /*
	    * if this is the first VarBind, make it the head of
	    * the list, otherwise attach it to the end
	    */
	   if (varBinds == NULL)
	       varBinds = current;
	   else
	       previous->next_var = current;

	   current->next_var = NULL;
	   previous = current;
       } else {
	   FreeVarBindList(varBinds);
	   varBinds = NULL;
       }
  }

  /* Make the call to generate the trap!  It will free the varbinds. */
  do_trap(genericTrapNum, specificTrapNum, varBinds, (OID *)entOid, v2TrapOID);

}
      
