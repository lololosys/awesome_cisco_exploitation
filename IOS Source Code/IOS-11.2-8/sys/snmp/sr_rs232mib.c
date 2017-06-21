/* $Id: sr_rs232mib.c,v 3.4.10.5 1996/07/03 20:43:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_rs232mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1994, Nicholas Thille (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rs232mib.c,v $
 * Revision 3.4.10.5  1996/07/03  20:43:40  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.4.10.4  1996/07/01  18:46:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/05/21  10:02:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.2  1996/05/21  06:38:52  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.10.1  1996/03/18  21:48:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/16  07:30:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.1  1996/03/13  01:59:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4  1996/02/26  20:42:59  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/01/18  16:37:58  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.2  1995/11/17  19:00:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  07:34:06  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.1  1995/06/07  22:50:21  hampton
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
#include "interface_private.h"
#include "ttysrv.h"
#include "../snmp/snmp_api.h"
#include "snmp_interface.h"
#include "sr_rs232mib.h"
#include "../snmp/ifmibapi.h"
#include "sr_rs232mib-mib.h"


static void init_rs232mib (subsystype *subsys)
{
    load_mib(rs232mib_OidList, rs232mib_OidListNum);
    load_oid(rs232mib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the rs232 family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232_get (OID         *incoming,
		    ObjectInfo  *object,
		    int          searchType,
		    ContextInfo *contextInfo,
		    int          serialNum)
{
    int      arg;
    void    *dp = NULL;
    rs232_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232Number
      case I_rs232Number:
	dp = &data->rs232Number;
	break;
#endif				       /* I_rs232Number */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rs232PortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232PortEntry_get (OID         *incoming,
			     ObjectInfo  *object,
			     int          searchType,
			     ContextInfo *contextInfo,
			     int          serialNum)
{
    int               instLength;
    int               arg;
    void             *dp;
    rs232PortEntry_t *data;
    unsigned long     buffer[1];
    OID               inst;
    int               carry;
    long              rs232PortIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
    data = NULL;

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
	    return(NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &rs232PortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232PortEntry_get(serialNum, contextInfo, arg, searchType, rs232PortIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->rs232PortIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232PortIndex
      case I_rs232PortIndex:
	dp = &data->rs232PortIndex;
	break;
#endif				       /* I_rs232PortIndex */

#ifdef I_rs232PortType
      case I_rs232PortType:
	dp = &data->rs232PortType;
	break;
#endif				       /* I_rs232PortType */

#ifdef I_rs232PortInSigNumber
      case I_rs232PortInSigNumber:
	dp = &data->rs232PortInSigNumber;
	break;
#endif				       /* I_rs232PortInSigNumber */

#ifdef I_rs232PortOutSigNumber
      case I_rs232PortOutSigNumber:
	dp = &data->rs232PortOutSigNumber;
	break;
#endif				       /* I_rs232PortOutSigNumber */

#ifdef I_rs232PortInSpeed
      case I_rs232PortInSpeed:
	dp = &data->rs232PortInSpeed;
	break;
#endif				       /* I_rs232PortInSpeed */

#ifdef I_rs232PortOutSpeed
      case I_rs232PortOutSpeed:
	dp = &data->rs232PortOutSpeed;
	break;
#endif				       /* I_rs232PortOutSpeed */

#ifdef I_rs232PortInFlowType
      case I_rs232PortInFlowType:
	dp = &data->rs232PortInFlowType;
	break;
#endif				       /* I_rs232PortInFlowType */

#ifdef I_rs232PortOutFlowType
      case I_rs232PortOutFlowType:
	dp = &data->rs232PortOutFlowType;
	break;
#endif				       /* I_rs232PortOutFlowType */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the rs232PortEntry data object.
 *---------------------------------------------------------------------*/
static void rs232PortEntry_free (rs232PortEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after rs232PortEntry set/undo
 *---------------------------------------------------------------------*/
static int rs232PortEntry_cleanup (doList_t       *trash)
{
    rs232PortEntry_free(trash->data);
#ifdef SR_SNMPv2
    rs232PortEntry_free(trash->undodata);
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
int rs232PortEntry_test (OID          *incoming,
			 ObjectInfo   *object,
			 ObjectSyntax *value,
			 doList_t     *doHead,
			 doList_t     *doCur,
			 ContextInfo  *contextInfo)
{
    int               instLength;
    doList_t         *dp;
    VarBind          *vb;
    int               found;
    int               carry;
    rs232PortEntry_t *rs232PortEntry;
    long              rs232PortIndex;

    instLength = incoming->length - object->oid.length;
    dp = NULL;
    vb = NULL;
    carry = 0;
    rs232PortEntry = NULL;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &rs232PortIndex, EXACT, &carry)) < 0) {
	return(NO_CREATION_ERROR);
    }
    rs232PortEntry = k_rs232PortEntry_get(-1, contextInfo, object->nominator, EXACT, rs232PortIndex);

#ifndef rs232PortEntry_READ_CREATE

    if (rs232PortEntry == NULL) {
	return(NO_ACCESS_ERROR);
    }
#endif				       /* rs232PortEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == rs232PortEntry_set) &&
	    (((rs232PortEntry_t *) (dp->data)) != NULL) &&
	    (((rs232PortEntry_t *) (dp->data))->rs232PortIndex == rs232PortIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(rs232PortEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return(RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(rs232PortEntry_t));

	dp->setMethod = rs232PortEntry_set;
	dp->cleanupMethod = rs232PortEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_rs232PortIndex, ((rs232PortEntry_t *) (dp->data))->valid);
	((rs232PortEntry_t *) (dp->data))->rs232PortIndex = rs232PortIndex;

#ifdef rs232PortEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (rs232PortEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, rs232PortEntry, sizeof(rs232PortEntry_t));

	    SET_ALL_VALID(((rs232PortEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* rs232PortEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_rs232PortInSpeed
      case I_rs232PortInSpeed:

	SET_VALID(I_rs232PortInSpeed, ((rs232PortEntry_t *) (dp->data))->valid);

	((rs232PortEntry_t *) (dp->data))->rs232PortInSpeed = value->sl_value;
	break;
#endif				       /* I_rs232PortInSpeed */

#ifdef I_rs232PortOutSpeed
      case I_rs232PortOutSpeed:

	SET_VALID(I_rs232PortOutSpeed, ((rs232PortEntry_t *) (dp->data))->valid);

	((rs232PortEntry_t *) (dp->data))->rs232PortOutSpeed = value->sl_value;
	break;
#endif				       /* I_rs232PortOutSpeed */

#ifdef I_rs232PortInFlowType
      case I_rs232PortInFlowType:

	SET_VALID(I_rs232PortInFlowType, ((rs232PortEntry_t *) (dp->data))->valid);

	((rs232PortEntry_t *) (dp->data))->rs232PortInFlowType = value->sl_value;
	break;
#endif				       /* I_rs232PortInFlowType */

#ifdef I_rs232PortOutFlowType
      case I_rs232PortOutFlowType:

	SET_VALID(I_rs232PortOutFlowType, ((rs232PortEntry_t *) (dp->data))->valid);

	((rs232PortEntry_t *) (dp->data))->rs232PortOutFlowType = value->sl_value;
	break;
#endif				       /* I_rs232PortOutFlowType */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rs232PortEntry_test)\n"));
	return(GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return(NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int rs232PortEntry_set (doList_t    *doHead,
			doList_t    *doCur,
			ContextInfo *contextInfo)
{
    return(k_rs232PortEntry_set((rs232PortEntry_t *) (doCur->data),
				 contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rs232AsyncPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232AsyncPortEntry_get (OID         *incoming,
				  ObjectInfo  *object,
				  int          searchType,
				  ContextInfo *contextInfo,
				  int          serialNum)
{
    int                    instLength;
    int                    arg;
    void                  *dp;
    rs232AsyncPortEntry_t *data;
    unsigned long          buffer[1];
    OID                    inst;
    int                    carry;
    long                   rs232AsyncPortIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
    data = NULL;


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
	    return(NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &rs232AsyncPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232AsyncPortEntry_get(serialNum, contextInfo, arg, searchType, rs232AsyncPortIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->rs232AsyncPortIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232AsyncPortIndex
      case I_rs232AsyncPortIndex:
	dp = &data->rs232AsyncPortIndex;
	break;
#endif				       /* I_rs232AsyncPortIndex */

#ifdef I_rs232AsyncPortBits
      case I_rs232AsyncPortBits:
	dp = &data->rs232AsyncPortBits;
	break;
#endif				       /* I_rs232AsyncPortBits */

#ifdef I_rs232AsyncPortStopBits
      case I_rs232AsyncPortStopBits:
	dp = &data->rs232AsyncPortStopBits;
	break;
#endif				       /* I_rs232AsyncPortStopBits */

#ifdef I_rs232AsyncPortParity
      case I_rs232AsyncPortParity:
	dp = &data->rs232AsyncPortParity;
	break;
#endif				       /* I_rs232AsyncPortParity */

#ifdef I_rs232AsyncPortAutobaud
      case I_rs232AsyncPortAutobaud:
	dp = &data->rs232AsyncPortAutobaud;
	break;
#endif				       /* I_rs232AsyncPortAutobaud */

#ifdef I_rs232AsyncPortParityErrs
      case I_rs232AsyncPortParityErrs:
	dp = &data->rs232AsyncPortParityErrs;
	break;
#endif				       /* I_rs232AsyncPortParityErrs */

#ifdef I_rs232AsyncPortFramingErrs
      case I_rs232AsyncPortFramingErrs:
	dp = &data->rs232AsyncPortFramingErrs;
	break;
#endif				       /* I_rs232AsyncPortFramingErrs */

#ifdef I_rs232AsyncPortOverrunErrs
      case I_rs232AsyncPortOverrunErrs:
	dp = &data->rs232AsyncPortOverrunErrs;
	break;
#endif				       /* I_rs232AsyncPortOverrunErrs */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the rs232AsyncPortEntry data object.
 *---------------------------------------------------------------------*/
static void rs232AsyncPortEntry_free (rs232AsyncPortEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after rs232AsyncPortEntry set/undo
 *---------------------------------------------------------------------*/
static int rs232AsyncPortEntry_cleanup (doList_t *trash)
{
    rs232AsyncPortEntry_free(trash->data);
#ifdef SR_SNMPv2
    rs232AsyncPortEntry_free(trash->undodata);
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
int rs232AsyncPortEntry_test (OID          *incoming,
			      ObjectInfo   *object,
			      ObjectSyntax *value,
			      doList_t     *doHead,
			      doList_t     *doCur,
			      ContextInfo  *contextInfo)
{
    int                    instLength;
    doList_t              *dp;
    VarBind               *vb;
    int                    found;
    int                    carry;
    rs232AsyncPortEntry_t *rs232AsyncPortEntry;
    long                   rs232AsyncPortIndex;

    instLength = incoming->length - object->oid.length;
    dp = NULL;
    vb = NULL;
    carry = 0;
    rs232AsyncPortEntry = NULL;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return(NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &rs232AsyncPortIndex, EXACT, &carry)) < 0) {
	return(NO_CREATION_ERROR);
    }
    rs232AsyncPortEntry = k_rs232AsyncPortEntry_get(-1, contextInfo, object->nominator, EXACT, rs232AsyncPortIndex);

#ifndef rs232AsyncPortEntry_READ_CREATE

    if (rs232AsyncPortEntry == NULL) {
	return(NO_ACCESS_ERROR);
    }
#endif				       /* rs232AsyncPortEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == rs232AsyncPortEntry_set) &&
	    (((rs232AsyncPortEntry_t *) (dp->data)) != NULL) &&
	    (((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortIndex == rs232AsyncPortIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(rs232AsyncPortEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return(RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(rs232AsyncPortEntry_t));

	dp->setMethod = rs232AsyncPortEntry_set;
	dp->cleanupMethod = rs232AsyncPortEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_rs232AsyncPortIndex, ((rs232AsyncPortEntry_t *) (dp->data))->valid);
	((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortIndex = rs232AsyncPortIndex;

#ifdef rs232AsyncPortEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (rs232AsyncPortEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, rs232AsyncPortEntry, sizeof(rs232AsyncPortEntry_t));

	    SET_ALL_VALID(((rs232AsyncPortEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* rs232AsyncPortEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_rs232AsyncPortBits
      case I_rs232AsyncPortBits:

	SET_VALID(I_rs232AsyncPortBits, ((rs232AsyncPortEntry_t *) (dp->data))->valid);

	((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortBits = value->sl_value;
	break;
#endif				       /* I_rs232AsyncPortBits */

#ifdef I_rs232AsyncPortStopBits
      case I_rs232AsyncPortStopBits:

	SET_VALID(I_rs232AsyncPortStopBits, ((rs232AsyncPortEntry_t *) (dp->data))->valid);

	((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortStopBits = value->sl_value;
	break;
#endif				       /* I_rs232AsyncPortStopBits */

#ifdef I_rs232AsyncPortParity
      case I_rs232AsyncPortParity:

	SET_VALID(I_rs232AsyncPortParity, ((rs232AsyncPortEntry_t *) (dp->data))->valid);

	((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortParity = value->sl_value;
	break;
#endif				       /* I_rs232AsyncPortParity */

#ifdef I_rs232AsyncPortAutobaud
      case I_rs232AsyncPortAutobaud:

	SET_VALID(I_rs232AsyncPortAutobaud, ((rs232AsyncPortEntry_t *) (dp->data))->valid);

	((rs232AsyncPortEntry_t *) (dp->data))->rs232AsyncPortAutobaud = value->sl_value;
	break;
#endif				       /* I_rs232AsyncPortAutobaud */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rs232AsyncPortEntry_test)\n"));
	return(GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return(NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int rs232AsyncPortEntry_set (doList_t    *doHead,
			     doList_t    *doCur,
			     ContextInfo *contextInfo)
{
    return(k_rs232AsyncPortEntry_set((rs232AsyncPortEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rs232SyncPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232SyncPortEntry_get (OID         *incoming,
				 ObjectInfo  *object,
				 int          searchType,
				 ContextInfo *contextInfo,
				 int          serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    rs232SyncPortEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            rs232SyncPortIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
    data = NULL;

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
	    return(NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &rs232SyncPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232SyncPortEntry_get(serialNum, contextInfo, arg, searchType, rs232SyncPortIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->rs232SyncPortIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232SyncPortIndex
      case I_rs232SyncPortIndex:
	dp = &data->rs232SyncPortIndex;
	break;
#endif				       /* I_rs232SyncPortIndex */

#ifdef I_rs232SyncPortClockSource
      case I_rs232SyncPortClockSource:
	dp = &data->rs232SyncPortClockSource;
	break;
#endif				       /* I_rs232SyncPortClockSource */

#ifdef I_rs232SyncPortFrameCheckErrs
      case I_rs232SyncPortFrameCheckErrs:
	dp = &data->rs232SyncPortFrameCheckErrs;
	break;
#endif				       /* I_rs232SyncPortFrameCheckErrs */

#ifdef I_rs232SyncPortTransmitUnderrunErrs
      case I_rs232SyncPortTransmitUnderrunErrs:
	dp = &data->rs232SyncPortTransmitUnderrunErrs;
	break;
#endif				       /* I_rs232SyncPortTransmitUnderrunErrs */

#ifdef I_rs232SyncPortReceiveOverrunErrs
      case I_rs232SyncPortReceiveOverrunErrs:
	dp = &data->rs232SyncPortReceiveOverrunErrs;
	break;
#endif				       /* I_rs232SyncPortReceiveOverrunErrs */

#ifdef I_rs232SyncPortInterruptedFrames
      case I_rs232SyncPortInterruptedFrames:
	dp = &data->rs232SyncPortInterruptedFrames;
	break;
#endif				       /* I_rs232SyncPortInterruptedFrames */

#ifdef I_rs232SyncPortAbortedFrames
      case I_rs232SyncPortAbortedFrames:
	dp = &data->rs232SyncPortAbortedFrames;
	break;
#endif				       /* I_rs232SyncPortAbortedFrames */

#ifdef I_rs232SyncPortRole
      case I_rs232SyncPortRole:
	dp = &data->rs232SyncPortRole;
	break;
#endif				       /* I_rs232SyncPortRole */

#ifdef I_rs232SyncPortEncoding
      case I_rs232SyncPortEncoding:
	dp = &data->rs232SyncPortEncoding;
	break;
#endif				       /* I_rs232SyncPortEncoding */

#ifdef I_rs232SyncPortRTSControl
      case I_rs232SyncPortRTSControl:
	dp = &data->rs232SyncPortRTSControl;
	break;
#endif				       /* I_rs232SyncPortRTSControl */

#ifdef I_rs232SyncPortRTSCTSDelay
      case I_rs232SyncPortRTSCTSDelay:
	dp = &data->rs232SyncPortRTSCTSDelay;
	break;
#endif				       /* I_rs232SyncPortRTSCTSDelay */

#ifdef I_rs232SyncPortMode
      case I_rs232SyncPortMode:
	dp = &data->rs232SyncPortMode;
	break;
#endif				       /* I_rs232SyncPortMode */

#ifdef I_rs232SyncPortIdlePattern
      case I_rs232SyncPortIdlePattern:
	dp = &data->rs232SyncPortIdlePattern;
	break;
#endif				       /* I_rs232SyncPortIdlePattern */

#ifdef I_rs232SyncPortMinFlags
      case I_rs232SyncPortMinFlags:
	dp = &data->rs232SyncPortMinFlags;
	break;
#endif				       /* I_rs232SyncPortMinFlags */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the rs232SyncPortEntry data object.
 *---------------------------------------------------------------------*/
static void rs232SyncPortEntry_free (rs232SyncPortEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after rs232SyncPortEntry set/undo
 *---------------------------------------------------------------------*/
static int rs232SyncPortEntry_cleanup (doList_t *trash)
{
    rs232SyncPortEntry_free(trash->data);
#ifdef SR_SNMPv2
    rs232SyncPortEntry_free(trash->undodata);
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
int rs232SyncPortEntry_test (OID          *incoming,
			     ObjectInfo   *object,
			     ObjectSyntax *value,
			     doList_t     *doHead,
			     doList_t     *doCur,
			     ContextInfo  *contextInfo)
{
    int                   instLength;
    doList_t             *dp;
    VarBind              *vb;
    int                   found;
    int                   carry;
    rs232SyncPortEntry_t *rs232SyncPortEntry;
    long                  rs232SyncPortIndex;

    instLength = incoming->length - object->oid.length;
    dp = NULL;
    vb = NULL;
    carry = 0;
    rs232SyncPortEntry = NULL;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &rs232SyncPortIndex, EXACT, &carry)) < 0) {
	return(NO_CREATION_ERROR);
    }
    rs232SyncPortEntry = k_rs232SyncPortEntry_get(-1, contextInfo, object->nominator, EXACT, rs232SyncPortIndex);

#ifndef rs232SyncPortEntry_READ_CREATE

    if (rs232SyncPortEntry == NULL) {
	return(NO_ACCESS_ERROR);
    }
#endif				       /* rs232SyncPortEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == rs232SyncPortEntry_set) &&
	    (((rs232SyncPortEntry_t *) (dp->data)) != NULL) &&
	    (((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortIndex == rs232SyncPortIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(rs232SyncPortEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return(RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(rs232SyncPortEntry_t));

	dp->setMethod = rs232SyncPortEntry_set;
	dp->cleanupMethod = rs232SyncPortEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_rs232SyncPortIndex, ((rs232SyncPortEntry_t *) (dp->data))->valid);
	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortIndex = rs232SyncPortIndex;

#ifdef rs232SyncPortEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (rs232SyncPortEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, rs232SyncPortEntry, sizeof(rs232SyncPortEntry_t));

	    SET_ALL_VALID(((rs232SyncPortEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* rs232SyncPortEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_rs232SyncPortClockSource
      case I_rs232SyncPortClockSource:

	SET_VALID(I_rs232SyncPortClockSource, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortClockSource = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortClockSource */

#ifdef I_rs232SyncPortRole
      case I_rs232SyncPortRole:

	SET_VALID(I_rs232SyncPortRole, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortRole = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortRole */

#ifdef I_rs232SyncPortEncoding
      case I_rs232SyncPortEncoding:

	SET_VALID(I_rs232SyncPortEncoding, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortEncoding = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortEncoding */

#ifdef I_rs232SyncPortRTSControl
      case I_rs232SyncPortRTSControl:

	SET_VALID(I_rs232SyncPortRTSControl, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortRTSControl = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortRTSControl */

#ifdef I_rs232SyncPortRTSCTSDelay
      case I_rs232SyncPortRTSCTSDelay:

	SET_VALID(I_rs232SyncPortRTSCTSDelay, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortRTSCTSDelay = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortRTSCTSDelay */

#ifdef I_rs232SyncPortMode
      case I_rs232SyncPortMode:

	SET_VALID(I_rs232SyncPortMode, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortMode = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortMode */

#ifdef I_rs232SyncPortIdlePattern
      case I_rs232SyncPortIdlePattern:

	SET_VALID(I_rs232SyncPortIdlePattern, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortIdlePattern = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortIdlePattern */

#ifdef I_rs232SyncPortMinFlags
      case I_rs232SyncPortMinFlags:

	SET_VALID(I_rs232SyncPortMinFlags, ((rs232SyncPortEntry_t *) (dp->data))->valid);

	((rs232SyncPortEntry_t *) (dp->data))->rs232SyncPortMinFlags = value->sl_value;
	break;
#endif				       /* I_rs232SyncPortMinFlags */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rs232SyncPortEntry_test)\n"));
	return(GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return(NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int rs232SyncPortEntry_set (doList_t       *doHead,
			    doList_t       *doCur,
			    ContextInfo    *contextInfo)
{
    return(k_rs232SyncPortEntry_set((rs232SyncPortEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rs232InSigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232InSigEntry_get (OID         *incoming,
			      ObjectInfo  *object,
			      int          searchType,
			      ContextInfo *contextInfo,
			      int          serialNum)
{
    int                instLength;
    int                arg;
    void              *dp;
    rs232InSigEntry_t *data;
    unsigned long      buffer[2];
    OID                inst;
    int                carry;
    long               rs232InSigPortIndex;
    long               rs232InSigName;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
    data = NULL;

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
	    return(NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &rs232InSigName, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &rs232InSigPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232InSigEntry_get(serialNum, contextInfo, arg, searchType, rs232InSigPortIndex, rs232InSigName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->rs232InSigPortIndex;
	inst.oid_ptr[1] = data->rs232InSigName;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232InSigPortIndex
      case I_rs232InSigPortIndex:
	dp = &data->rs232InSigPortIndex;
	break;
#endif				       /* I_rs232InSigPortIndex */

#ifdef I_rs232InSigName
      case I_rs232InSigName:
	dp = &data->rs232InSigName;
	break;
#endif				       /* I_rs232InSigName */

#ifdef I_rs232InSigState
      case I_rs232InSigState:
	dp = &data->rs232InSigState;
	break;
#endif				       /* I_rs232InSigState */

#ifdef I_rs232InSigChanges
      case I_rs232InSigChanges:
	dp = &data->rs232InSigChanges;
	break;
#endif				       /* I_rs232InSigChanges */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rs232OutSigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *rs232OutSigEntry_get (OID         *incoming,
			       ObjectInfo  *object,
			       int          searchType,
			       ContextInfo *contextInfo,
			       int          serialNum)
{
    int                 instLength;
    int                 arg;
    void               *dp;
    rs232OutSigEntry_t *data;
    unsigned long       buffer[2];
    OID                 inst;
    int                 carry;
    long                rs232OutSigPortIndex;
    long                rs232OutSigName;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp= NULL;
    data = NULL;

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
	    return(NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &rs232OutSigName, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &rs232OutSigPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_rs232OutSigEntry_get(serialNum, contextInfo, arg, searchType, rs232OutSigPortIndex, rs232OutSigName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->rs232OutSigPortIndex;
	inst.oid_ptr[1] = data->rs232OutSigName;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_rs232OutSigPortIndex
      case I_rs232OutSigPortIndex:
	dp = &data->rs232OutSigPortIndex;
	break;
#endif				       /* I_rs232OutSigPortIndex */

#ifdef I_rs232OutSigName
      case I_rs232OutSigName:
	dp = &data->rs232OutSigName;
	break;
#endif				       /* I_rs232OutSigName */

#ifdef I_rs232OutSigState
      case I_rs232OutSigState:
	dp = &data->rs232OutSigState;
	break;
#endif				       /* I_rs232OutSigState */

#ifdef I_rs232OutSigChanges
      case I_rs232OutSigChanges:
	dp = &data->rs232OutSigChanges;
	break;
#endif				       /* I_rs232OutSigChanges */

      default:
	return(NULL);

    }				       /* switch */

    return(MakeVarBind(object, &inst, dp));

}






/*
 **********************************************************************
 *
 * KERNEL CODE begins here.  Above is all generated by SNMP Research
 * compiler.  Below code does the real work in getting the router
 * information.  
 *
 * First come the utility helper routines.
 *
 **********************************************************************
 */

static boolean snmp_async232_idb_tester (hwidbtype *hwidb, long* index2)

{
    return(hwidb->type == IDBTYPE_ASYNC);
}


static boolean snmp_sync232_idb_tester (hwidbtype *hwidb, long* index2)

{
/*************************** MISSING *********************/
/* Need to catch sync rs232 interfaces here.             */
/*************************** MISSING *********************/
    return(FALSE);
}


static boolean snmp_rs232_idb_tester (hwidbtype *hwidb, long* index2)

{
    if (snmp_async232_idb_tester(hwidb, index2))
	return(TRUE);
    else 
	return(snmp_sync232_idb_tester(hwidb, index2));
}


rs232_t *k_rs232_get (int          serialNum,
		      ContextInfo *contextInfo,
		      int          nominator)
{
    static rs232_t  rs232Data;
    hwidbtype *idb;

    rs232Data.rs232Number = 0;

    FOR_ALL_HWIDBS(idb){
        if (!(idb->status & IDB_DELETED) && idb->type == IDBTYPE_ASYNC && 
                 idb->hw_if_index <= idb_get_current_hw_index())
	    rs232Data.rs232Number++;
    }
    return(&rs232Data);
}

rs232PortEntry_t *k_rs232PortEntry_get (int          serialNum,
					ContextInfo *contextInfo,
					int          nominator,
					int          searchType,
					long         rs232PortIndex)
{
    static rs232PortEntry_t rs232PortEntryData;

    tt_soc    *tty;
    hwidbtype *hwidb;

    hwidb = ifmib_hwidb_get(serialNum, searchType, rs232PortIndex,
			    snmp_rs232_idb_tester, NULL);

    if (hwidb == NULL)
	return(NULL);

    if (hwidb->type != IDBTYPE_ASYNC) { 
/******************************* MISSING *******************************/
/* Deal with sync interfaces here.  For now, return NULL.              */
/******************************* MISSING *******************************/
	return(NULL);
    } else {
	tty = hwidb->idb_tty;

	if (tty == NULL)
	    return(NULL);

	rs232PortEntryData.rs232PortIndex = hwidb->snmp_if_index;
	rs232PortEntryData.rs232PortType = D_rs232PortType_rs232;

	rs232PortEntryData.rs232PortInSigNumber = 
	    snmp_num_modem_signals(tty, SNMP_MODEM_INPUT_SIGNAL);
	rs232PortEntryData.rs232PortOutSigNumber = 
	    snmp_num_modem_signals(tty, SNMP_MODEM_OUTPUT_SIGNAL);

	rs232PortEntryData.rs232PortInSpeed = tty->tty_rxspeed;
	rs232PortEntryData.rs232PortOutSpeed = tty->tty_txspeed;

	if (tty->capabilities & (HFLOWIN))
	    rs232PortEntryData.rs232PortInFlowType = 
		D_rs232PortInFlowType_ctsRts;
	else
	    rs232PortEntryData.rs232PortInFlowType = 
		D_rs232PortInFlowType_none;

	if (tty->capabilities & (HFLOWOUT))
	    rs232PortEntryData.rs232PortOutFlowType = 
		D_rs232PortOutFlowType_ctsRts;
	else 
	    rs232PortEntryData.rs232PortOutFlowType = 
		D_rs232PortOutFlowType_none;
	
    }

    return(&rs232PortEntryData);

}

#ifdef SETS
int k_rs232PortEntry_set (rs232PortEntry_t *data,
			  ContextInfo      *contextInfo,
			  int               function)
{
    return(GEN_ERROR);
}

#endif				       /* SETS */

rs232AsyncPortEntry_t *k_rs232AsyncPortEntry_get (int          serialNum,
						  ContextInfo *contextInfo,
						  int          nominator,
						  int          searchType,
						  long    rs232AsyncPortIndex)
{
    static rs232AsyncPortEntry_t rs232AsyncPortEntryData;

    hwidbtype *hwidb;
    tt_soc    *tty;

    hwidb = ifmib_hwidb_get(serialNum, searchType,
			    rs232AsyncPortIndex,
			    snmp_async232_idb_tester, NULL);

    if (hwidb == NULL)
	return(NULL);

    tty = hwidb->idb_tty;

    if (tty == NULL) 
	return(NULL);

    rs232AsyncPortEntryData.rs232AsyncPortIndex = hwidb->snmp_if_index;
    rs232AsyncPortEntryData.rs232AsyncPortBits = tty->tty_databits;
    switch (tty->tty_stopbits) {
      case STOPBITS15:
	rs232AsyncPortEntryData.rs232AsyncPortStopBits = 
	    D_rs232AsyncPortStopBits_oneAndHalf;
	break;
      case STOPBITS1:
	rs232AsyncPortEntryData.rs232AsyncPortStopBits = 
	    D_rs232AsyncPortStopBits_one;
	break;
      case STOPBITS2:
      default:
	rs232AsyncPortEntryData.rs232AsyncPortStopBits = 
	    D_rs232AsyncPortStopBits_two;
    }
    switch (tty->tty_parity) {
      case PARITY_NO:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_none;
	break;
      case PARITY_ODD:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_odd;
	break;
      case PARITY_EVEN:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_even;
	break;
      case PARITY_SPACE:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_space;
	break;
      case PARITY_MARK:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_mark;
	break;
      default:
	rs232AsyncPortEntryData.rs232AsyncPortParity = 
	    D_rs232AsyncPortParity_none;
	break;
    }
    if (tty->capabilities & AUTOBAUD)
	rs232AsyncPortEntryData.rs232AsyncPortAutobaud =
	    D_rs232AsyncPortAutobaud_enabled;
    else
	rs232AsyncPortEntryData.rs232AsyncPortAutobaud =
	    D_rs232AsyncPortAutobaud_disabled;

    rs232AsyncPortEntryData.rs232AsyncPortParityErrs = 
	hwidb->counters.parity;
    rs232AsyncPortEntryData.rs232AsyncPortFramingErrs = 
	hwidb->counters.input_frame;

    rs232AsyncPortEntryData.rs232AsyncPortOverrunErrs = tty->overrun;

    return(&rs232AsyncPortEntryData);
}

#ifdef SETS
int k_rs232AsyncPortEntry_set (rs232AsyncPortEntry_t *data,
			       ContextInfo           *contextInfo,
			       int                    function)
{
    return(GEN_ERROR);
}

#endif				       /* SETS */

rs232SyncPortEntry_t *k_rs232SyncPortEntry_get (int          serialNum,
						ContextInfo *contextInfo,
						int          nominator,
						int          searchType,
						long       rs232SyncPortIndex)
{
#ifdef NOT_YET
/******************************* MISSING *******************************/
/* Deal with sync interfaces here.  For now, return NULL.              */
/******************************* MISSING *******************************/
    static rs232SyncPortEntry_t rs232SyncPortEntryData;

    rs232SyncPortEntryData.rs232SyncPortIndex = hwidb->snmp_if_index;
    rs232SyncPortEntryData.rs232SyncPortClockSource = 
    rs232SyncPortEntryData.rs232SyncPortFrameCheckErrs = 
    rs232SyncPortEntryData.rs232SyncPortTransmitUnderrunErrs = 
    rs232SyncPortEntryData.rs232SyncPortReceiveOverrunErrs = 
    rs232SyncPortEntryData.rs232SyncPortInterruptedFrames = 
    rs232SyncPortEntryData.rs232SyncPortAbortedFrames = 
    rs232SyncPortEntryData.rs232SyncPortRole = 
    rs232SyncPortEntryData.rs232SyncPortEncoding = 
    rs232SyncPortEntryData.rs232SyncPortRTSControl = 
    rs232SyncPortEntryData.rs232SyncPortRTSCTSDelay = 
    rs232SyncPortEntryData.rs232SyncPortMode = 
    rs232SyncPortEntryData.rs232SyncPortIdlePattern = 
    rs232SyncPortEntryData.rs232SyncPortMinFlags = 

    return(&rs232SyncPortEntryData);
#else				       /* NOT_YET */
    return(NULL);
#endif				       /* NOT_YET */
}

#ifdef SETS
int k_rs232SyncPortEntry_set (rs232SyncPortEntry_t *data,
			      ContextInfo          *contextInfo,
			      int                   function)
{
    return(GEN_ERROR);
}

#endif				       /* SETS */

rs232InSigEntry_t *k_rs232InSigEntry_get (int          serialNum,
					  ContextInfo *contextInfo,
					  int          nominator,
					  int          searchType,
					  long         rs232InSigPortIndex,
					  long         rs232InSigName)
{
    static rs232InSigEntry_t rs232InSigEntryData;

    hwidbtype        *hwidb;
    tt_soc           *tty;
    snmp_signal_info  signal_info;
    boolean           next = (searchType != EXACT);

    while (TRUE) {			/* Loop until found or no more idbs */

	hwidb = ifmib_hwidb_get(serialNum, searchType,
				rs232InSigPortIndex,
				snmp_rs232_idb_tester, NULL);

	if (hwidb == NULL)
	    return(NULL);

	if (hwidb->type != IDBTYPE_ASYNC) { 
	    return(NULL);
	    /************************ MISSING ****************************/
	    /* Deal with sync interfaces here.  For now, return NULL.    */
	    /************************ MISSING ****************************/
	} else {
	    tty = hwidb->idb_tty;

	    if (tty == NULL)
		return(NULL);

	    if (snmp_get_modem_signal(tty, &signal_info, rs232InSigName,
				      next, SNMP_MODEM_INPUT_SIGNAL)) {
		rs232InSigEntryData.rs232InSigPortIndex = hwidb->snmp_if_index;
		rs232InSigEntryData.rs232InSigName = 
		    signal_info.rs232_signal_name;
		rs232InSigEntryData.rs232InSigState = 
		    signal_info.rs232_signal_state;

		return(&rs232InSigEntryData);
	    } 
	}
	/*
	 * If I didn't get a modem signal, there are no more for
	 * this interface, so move to next interface.
	 */
	rs232InSigPortIndex++;
	rs232InSigName = 1;
    }
}

rs232OutSigEntry_t *k_rs232OutSigEntry_get (int          serialNum,
					    ContextInfo *contextInfo,
					    int          nominator,
					    int          searchType,
					    long         rs232OutSigPortIndex,
					    long         rs232OutSigName)
{
    static rs232OutSigEntry_t rs232OutSigEntryData;

    hwidbtype        *hwidb;
    tt_soc           *tty;
    snmp_signal_info  signal_info;
    boolean           next = (searchType != EXACT);

    while (TRUE) {			/* Loop until found or no more idbs */

	hwidb = ifmib_hwidb_get(serialNum, searchType,
				rs232OutSigPortIndex,
				snmp_rs232_idb_tester, NULL);

	if (hwidb == NULL)
	    return(NULL);

	if (hwidb->type != IDBTYPE_ASYNC) { 
	    return(NULL);
	    /************************ MISSING ****************************/
	    /* Deal with sync interfaces here.  For now, return NULL.    */
	    /************************ MISSING ****************************/
	} else {
	    tty = hwidb->idb_tty;

	    if (tty == NULL)
		return(NULL);

	    if (snmp_get_modem_signal(tty, &signal_info, rs232OutSigName,
				      next, SNMP_MODEM_OUTPUT_SIGNAL)) {
		rs232OutSigEntryData.rs232OutSigPortIndex =
		    hwidb->snmp_if_index;
		rs232OutSigEntryData.rs232OutSigName = 
		    signal_info.rs232_signal_name; 
		rs232OutSigEntryData.rs232OutSigState =
		    signal_info.rs232_signal_state; 

		return(&rs232OutSigEntryData);
	    }
	}
	/*
	 * If I didn't get a modem signal, there are no more for
	 * this interface, so move to next interface.
	 */
	rs232OutSigPortIndex++;
	rs232OutSigName = 1;
    }
}


/*
 * rs232mib subsystem header
 */

#define MAJVERSION_rs232mib 1
#define MINVERSION_rs232mib 0
#define EDITVERSION_rs232mib 0

SUBSYS_HEADER(rs232mib,
              MAJVERSION_rs232mib,
              MINVERSION_rs232mib,
              EDITVERSION_rs232mib,
              init_rs232mib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
