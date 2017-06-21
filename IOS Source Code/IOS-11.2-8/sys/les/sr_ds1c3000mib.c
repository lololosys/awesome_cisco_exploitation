/* $Id: sr_ds1c3000mib.c,v 3.2.2.3 1996/07/01 18:45:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/sr_ds1c3000mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, David Williams (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ds1c3000mib.c,v $
 * Revision 3.2.2.3  1996/07/01  18:45:36  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.2.2  1996/06/22  23:47:18  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.2.2.1  1996/06/13  19:08:21  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2  1996/03/29  03:50:12  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:37  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
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
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_ds1c3000mib.h"
#include "sr_ds1c3000mib-mib.h"

/*
 *  system dependent header files
 */

#include "interface_private.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_les_hd64570.h"
#include "../les/if_c3000_csu_dsu.h"
#include "../parser/parser_defs_parser.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the dsx1CurrentEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
dsx1CurrentEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    dsx1CurrentEntry_t      *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     dsx1CurrentIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1CurrentIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1CurrentIndex = MAX(1, dsx1CurrentIndex);
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1CurrentEntry_get(serialNum, contextInfo, arg, searchType, dsx1CurrentIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> dsx1CurrentIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1CurrentIndex
      case I_dsx1CurrentIndex:
	dp = (void *) (&data -> dsx1CurrentIndex);
	break;
#endif				       /* I_dsx1CurrentIndex */

#ifdef I_dsx1CurrentESs
      case I_dsx1CurrentESs:
	dp = (void *) (&data -> dsx1CurrentESs);
	break;
#endif				       /* I_dsx1CurrentESs */

#ifdef I_dsx1CurrentSESs
      case I_dsx1CurrentSESs:
	dp = (void *) (&data -> dsx1CurrentSESs);
	break;
#endif				       /* I_dsx1CurrentSESs */

#ifdef I_dsx1CurrentSEFSs
      case I_dsx1CurrentSEFSs:
	dp = (void *) (&data -> dsx1CurrentSEFSs);
	break;
#endif				       /* I_dsx1CurrentSEFSs */

#ifdef I_dsx1CurrentUASs
      case I_dsx1CurrentUASs:
	dp = (void *) (&data -> dsx1CurrentUASs);
	break;
#endif				       /* I_dsx1CurrentUASs */

#ifdef I_dsx1CurrentCSSs
      case I_dsx1CurrentCSSs:
	dp = (void *) (&data -> dsx1CurrentCSSs);
	break;
#endif				       /* I_dsx1CurrentCSSs */

#ifdef I_dsx1CurrentPCVs
      case I_dsx1CurrentPCVs:
	dp = (void *) (&data -> dsx1CurrentPCVs);
	break;
#endif				       /* I_dsx1CurrentPCVs */

#ifdef I_dsx1CurrentLESs
      case I_dsx1CurrentLESs:
	dp = (void *) (&data -> dsx1CurrentLESs);
	break;
#endif				       /* I_dsx1CurrentLESs */

#ifdef I_dsx1CurrentBESs
      case I_dsx1CurrentBESs:
	dp = (void *) (&data -> dsx1CurrentBESs);
	break;
#endif				       /* I_dsx1CurrentBESs */

#ifdef I_dsx1CurrentDMs
      case I_dsx1CurrentDMs:
	dp = (void *) (&data -> dsx1CurrentDMs);
	break;
#endif				       /* I_dsx1CurrentDMs */

#ifdef I_dsx1CurrentLCVs
      case I_dsx1CurrentLCVs:
	dp = (void *) (&data -> dsx1CurrentLCVs);
	break;
#endif				       /* I_dsx1CurrentLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1IntervalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
dsx1IntervalEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    dsx1IntervalEntry_t     *data = NULL;
    unsigned long            buffer[2];
    OID                      inst;
    int                      carry;
    long                     dsx1IntervalIndex;
    long                     dsx1IntervalNumber;


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

    if ((InstToInt(incoming, 1 + object -> oid.length, &dsx1IntervalNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1IntervalNumber = MAX(1, dsx1IntervalNumber);
    }
    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1IntervalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1IntervalIndex = MAX(1, dsx1IntervalIndex);
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1IntervalEntry_get(serialNum, contextInfo, arg, searchType, dsx1IntervalIndex, dsx1IntervalNumber)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data -> dsx1IntervalIndex;
	inst.oid_ptr[1] = data -> dsx1IntervalNumber;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1IntervalIndex
      case I_dsx1IntervalIndex:
	dp = (void *) (&data -> dsx1IntervalIndex);
	break;
#endif				       /* I_dsx1IntervalIndex */

#ifdef I_dsx1IntervalNumber
      case I_dsx1IntervalNumber:
	dp = (void *) (&data -> dsx1IntervalNumber);
	break;
#endif				       /* I_dsx1IntervalNumber */

#ifdef I_dsx1IntervalESs
      case I_dsx1IntervalESs:
	dp = (void *) (&data -> dsx1IntervalESs);
	break;
#endif				       /* I_dsx1IntervalESs */

#ifdef I_dsx1IntervalSESs
      case I_dsx1IntervalSESs:
	dp = (void *) (&data -> dsx1IntervalSESs);
	break;
#endif				       /* I_dsx1IntervalSESs */

#ifdef I_dsx1IntervalSEFSs
      case I_dsx1IntervalSEFSs:
	dp = (void *) (&data -> dsx1IntervalSEFSs);
	break;
#endif				       /* I_dsx1IntervalSEFSs */

#ifdef I_dsx1IntervalUASs
      case I_dsx1IntervalUASs:
	dp = (void *) (&data -> dsx1IntervalUASs);
	break;
#endif				       /* I_dsx1IntervalUASs */

#ifdef I_dsx1IntervalCSSs
      case I_dsx1IntervalCSSs:
	dp = (void *) (&data -> dsx1IntervalCSSs);
	break;
#endif				       /* I_dsx1IntervalCSSs */

#ifdef I_dsx1IntervalPCVs
      case I_dsx1IntervalPCVs:
	dp = (void *) (&data -> dsx1IntervalPCVs);
	break;
#endif				       /* I_dsx1IntervalPCVs */

#ifdef I_dsx1IntervalLESs
      case I_dsx1IntervalLESs:
	dp = (void *) (&data -> dsx1IntervalLESs);
	break;
#endif				       /* I_dsx1IntervalLESs */

#ifdef I_dsx1IntervalBESs
      case I_dsx1IntervalBESs:
	dp = (void *) (&data -> dsx1IntervalBESs);
	break;
#endif				       /* I_dsx1IntervalBESs */

#ifdef I_dsx1IntervalDMs
      case I_dsx1IntervalDMs:
	dp = (void *) (&data -> dsx1IntervalDMs);
	break;
#endif				       /* I_dsx1IntervalDMs */

#ifdef I_dsx1IntervalLCVs
      case I_dsx1IntervalLCVs:
	dp = (void *) (&data -> dsx1IntervalLCVs);
	break;
#endif				       /* I_dsx1IntervalLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1TotalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
dsx1TotalEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    dsx1TotalEntry_t        *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     dsx1TotalIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1TotalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1TotalIndex = MAX(1, dsx1TotalIndex);
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1TotalEntry_get(serialNum, contextInfo, arg, searchType, dsx1TotalIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> dsx1TotalIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1TotalIndex
      case I_dsx1TotalIndex:
	dp = (void *) (&data -> dsx1TotalIndex);
	break;
#endif				       /* I_dsx1TotalIndex */

#ifdef I_dsx1TotalESs
      case I_dsx1TotalESs:
	dp = (void *) (&data -> dsx1TotalESs);
	break;
#endif				       /* I_dsx1TotalESs */

#ifdef I_dsx1TotalSESs
      case I_dsx1TotalSESs:
	dp = (void *) (&data -> dsx1TotalSESs);
	break;
#endif				       /* I_dsx1TotalSESs */

#ifdef I_dsx1TotalSEFSs
      case I_dsx1TotalSEFSs:
	dp = (void *) (&data -> dsx1TotalSEFSs);
	break;
#endif				       /* I_dsx1TotalSEFSs */

#ifdef I_dsx1TotalUASs
      case I_dsx1TotalUASs:
	dp = (void *) (&data -> dsx1TotalUASs);
	break;
#endif				       /* I_dsx1TotalUASs */

#ifdef I_dsx1TotalCSSs
      case I_dsx1TotalCSSs:
	dp = (void *) (&data -> dsx1TotalCSSs);
	break;
#endif				       /* I_dsx1TotalCSSs */

#ifdef I_dsx1TotalPCVs
      case I_dsx1TotalPCVs:
	dp = (void *) (&data -> dsx1TotalPCVs);
	break;
#endif				       /* I_dsx1TotalPCVs */

#ifdef I_dsx1TotalLESs
      case I_dsx1TotalLESs:
	dp = (void *) (&data -> dsx1TotalLESs);
	break;
#endif				       /* I_dsx1TotalLESs */

#ifdef I_dsx1TotalBESs
      case I_dsx1TotalBESs:
	dp = (void *) (&data -> dsx1TotalBESs);
	break;
#endif				       /* I_dsx1TotalBESs */

#ifdef I_dsx1TotalDMs
      case I_dsx1TotalDMs:
	dp = (void *) (&data -> dsx1TotalDMs);
	break;
#endif				       /* I_dsx1TotalDMs */

#ifdef I_dsx1TotalLCVs
      case I_dsx1TotalLCVs:
	dp = (void *) (&data -> dsx1TotalLCVs);
	break;
#endif				       /* I_dsx1TotalLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1FracEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
dsx1FracEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    dsx1FracEntry_t         *data = NULL;
    unsigned long            buffer[2];
    OID                      inst;
    int                      carry;
    long                     dsx1FracIndex;
    long                     dsx1FracNumber;


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

    if ((InstToInt(incoming, 1 + object -> oid.length, &dsx1FracNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1FracNumber = MAX(1, dsx1FracNumber);
    }
    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1FracIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1FracIndex = MAX(1, dsx1FracIndex);
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1FracEntry_get(serialNum, contextInfo, arg, searchType, dsx1FracIndex, dsx1FracNumber)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data -> dsx1FracIndex;
	inst.oid_ptr[1] = data -> dsx1FracNumber;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1FracIndex
      case I_dsx1FracIndex:
	dp = (void *) (&data -> dsx1FracIndex);
	break;
#endif				       /* I_dsx1FracIndex */

#ifdef I_dsx1FracNumber
      case I_dsx1FracNumber:
	dp = (void *) (&data -> dsx1FracNumber);
	break;
#endif				       /* I_dsx1FracNumber */

#ifdef I_dsx1FracIfIndex
      case I_dsx1FracIfIndex:
	dp = (void *) (&data -> dsx1FracIfIndex);
	break;
#endif				       /* I_dsx1FracIfIndex */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dsx1FracEntry data object.
 *---------------------------------------------------------------------*/
void
dsx1FracEntry_free(dsx1FracEntry_t * data)
{
    if (data != NULL) {

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dsx1FracEntry set/undo
 *---------------------------------------------------------------------*/
static int
dsx1FracEntry_cleanup(doList_t * trash)
{
    dsx1FracEntry_free(trash -> data);
#ifdef SR_SNMPv2
    dsx1FracEntry_free(trash -> undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the dsx1FracEntry family
 *---------------------------------------------------------------------*/
dsx1FracEntry_t         *
Clone_dsx1FracEntry(dsx1FracEntry_t * dsx1FracEntry)
{
    dsx1FracEntry_t         *data;

    if ((data = malloc(sizeof(dsx1FracEntry_t))) == NULL) {
	return NULL;
    }
    memcpy((char *) (data), (char *) (dsx1FracEntry), sizeof(dsx1FracEntry_t));


    return data;
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
dsx1FracEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID                     *incoming;
    ObjectInfo              *object;
    ObjectSyntax            *value;
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    int                      instLength = incoming -> length - object -> oid.length;
    doList_t                *dp;
    int                      found;
    int                      error_status = NO_ERROR;
    int                      carry = 0;
    dsx1FracEntry_t         *dsx1FracEntry;
    long                     dsx1FracIndex;
    long                     dsx1FracNumber;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 2) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1FracIndex, EXACT, &carry)) < 0) {
	error_status = NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 1 + object -> oid.length, &dsx1FracNumber, EXACT, &carry)) < 0) {
	error_status = NO_CREATION_ERROR;
    }
    if (error_status != NO_ERROR) {
	return error_status;
    }
    dsx1FracEntry = k_dsx1FracEntry_get(-1, contextInfo, -1, EXACT, dsx1FracIndex, dsx1FracNumber);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp -> next) {
	if ((dp -> setMethod == dsx1FracEntry_set) &&
	    (((dsx1FracEntry_t *) (dp -> data)) != NULL) &&
	    (((dsx1FracEntry_t *) (dp -> data)) -> dsx1FracIndex == dsx1FracIndex) &&
	    (((dsx1FracEntry_t *) (dp -> data)) -> dsx1FracNumber == dsx1FracNumber)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;

	dp -> setMethod = dsx1FracEntry_set;
	dp -> cleanupMethod = dsx1FracEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_dsx1FracEntry_UNDO
	dp -> undoMethod = dsx1FracEntry_undo;
#else				       /* SR_dsx1FracEntry_UNDO */
	dp -> undoMethod = NULL;
#endif				       /* SR_dsx1FracEntry_UNDO */
#endif				       /* SR_SNMPv2 */
	dp -> state = UNKNOWN;

	if (dsx1FracEntry != NULL) {
	    /* fill in existing values */
	    if ((dp -> data = (void *) Clone_dsx1FracEntry(dsx1FracEntry)) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    }
#ifdef SR_SNMPv2
	    if ((dp -> undodata = (void *) Clone_dsx1FracEntry(dsx1FracEntry)) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				       /* SR_SNMPv2 */

	} else {

#ifdef dsx1FracEntry_READ_CREATE
	    if ((dp -> data = malloc(sizeof(dsx1FracEntry_t))) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    } else {
		memset(dp -> data, 0, sizeof(dsx1FracEntry_t));
#ifdef SR_SNMPv2
		dp -> undodata = NULL;
#endif				       /* SR_SNMPv2 */

		/* Fill in reasonable default values for this new entry */
		((dsx1FracEntry_t *) (dp -> data)) -> dsx1FracIndex = (dsx1FracIndex);
		SET_VALID(I_dsx1FracIndex, ((dsx1FracEntry_t *) (dp -> data)) -> valid);

		((dsx1FracEntry_t *) (dp -> data)) -> dsx1FracNumber = (dsx1FracNumber);
		SET_VALID(I_dsx1FracNumber, ((dsx1FracEntry_t *) (dp -> data)) -> valid);

		error_status = k_dsx1FracEntry_set_defaults(dp);
	    }
#else				       /* dsx1FracEntry_READ_CREATE */
	    error_status = NO_ACCESS_ERROR;
#endif				       /* dsx1FracEntry_READ_CREATE */
	}
    }
    if (error_status != NO_ERROR) {
	return error_status;
    }
    switch (object -> nominator) {

#ifdef I_dsx1FracIfIndex
      case I_dsx1FracIfIndex:

	if (((value -> sl_value < 1) || (value -> sl_value > 2147483647))) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1FracEntry_t *) (dp -> data)) -> dsx1FracIfIndex = value -> sl_value;
	break;
#endif				       /* I_dsx1FracIfIndex */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dsx1FracEntry_test)\n"));
	return GEN_ERROR;

    }				       /* switch */

    /* Do system dependent testing in k_dsx1FracEntry_test */
    error_status = k_dsx1FracEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object -> nominator, ((dsx1FracEntry_t *) (dp -> data)) -> valid);
	error_status = k_dsx1FracEntry_ready(object, value, doHead, dp);
    }
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
dsx1FracEntry_set(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return (k_dsx1FracEntry_set((dsx1FracEntry_t *) (doCur -> data),
				contextInfo, doCur -> state));
}

#endif				/* SETS */



/****************************************************************************/
/* Start of system-dependent routines                                       */
/****************************************************************************/

/*
 * ProcTest helper routine used by ifmib_hwidb_get() to determine if the given
 *   hwidb is the type we are looking for.
 */
static boolean
test_csu_dsu_type ( hwidbtype *hwidb, ulong *p_types_bitmask ) 
{
    if ((hwidb->type == IDBTYPE_HD64570) &&
        (hwidb->csu_dsu_type & *p_types_bitmask)) {
	return TRUE;
    } else {
	return FALSE;
    }
}

/*
 * Find the csu_dsu_instance_t located at the given ifIndex.
 * Behavior controlled by searchType.   If searchType is EXACT then only a
 *   search at the given ifIndex is made.  If searchType is NEXT then a search
 *   is made at the given ifIndex and all subsequent ifIndexes in the system.
 * The type of csu_dsu searched for is controlled by the csu_dsu_types_bitmask
 *   parameter.  If any of the csu_dsu types represented by the bit_mask are
 *   present a successfull match is made.
 * The pp_csu_dsu handle will be initialized to the one found, or NULL if
 *   none found.  
 * The function returns the ifIndex of the instance found, or zero if none found.
 */
long
find_index_and_csu_dsu_instance (int serialNum,
				 int searchType,
				 long ifIndex,
				 csu_dsu_instance_t ** pp_csu_dsu,
				 ulong csu_dsu_types_bitmask)
{
    hwidbtype               *hwidb;

    /*
     * Find the requested type of csu/dsu.  If a get-next request, attempt to
     * find one at successive interfaces.
     */

    hwidb = ifmib_hwidb_get(serialNum, 
                            searchType, 
                            ifIndex, 
                            (ProcTest) test_csu_dsu_type, 
                            &csu_dsu_types_bitmask );

    if ( hwidb ) {
        
	*pp_csu_dsu = csu_dsu_get_instance( hwidb );
	return hwidb->snmp_if_index;

    } else {

	*pp_csu_dsu = NULL;
	return 0;
    }
}


/*
 * get/get-next RFC1406 dsx1CurrentTable
 * CSU/DSU card must be queried for all values.  Complete current data is
 *   retrieved and cached between calls in order to minimize CSU/DSU reads for
 *   get-bulk operations.
 */
dsx1CurrentEntry_t *
k_dsx1CurrentEntry_get (int serialNum,
		        ContextInfo * contextInfo,
		        int nominator,
		        int searchType,
		        long dsx1CurrentIndex)
{
    static dsx1CurrentEntry_t dsx1CurrentEntryData;
    static int               last_serialNum;
    t1_current_interval_user_data current;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    /*
     * Check to see if this was a get-bulk packet (request-id or serialNum
     * will be same as previous get) and if for same instance (previous
     * instance kept in static dsx1CurrentEntryData).  If so, return
     * information just retrieved from CSU/DSU.
     */
    if ((serialNum == last_serialNum) &&
	(dsx1CurrentIndex == dsx1CurrentEntryData.dsx1CurrentIndex)) {
	return &dsx1CurrentEntryData;
    }
    dsx1CurrentIndex = find_index_and_csu_dsu_instance(serialNum,
						       searchType,
						       dsx1CurrentIndex,
						       &p_csu_dsu,
						       CSU_DSU_FT1);

    if (p_csu_dsu) {
	if (csu_dsu_register_RW(p_csu_dsu,
				T1_CURRENT_INTERVAL_DATA_REG,
				T1_1406_INTERVAL_DATA_LEN,
				(char *) &current,
				READ_REGISTER) == DSU_REQUEST_COMPLETE) {

	    /*
	     * Initialize table index.  Must be done regardless of which
	     * nominator (object) being read.
	     */
	    dsx1CurrentEntryData.dsx1CurrentIndex = dsx1CurrentIndex;

	    /*
	     * Initialize each struct entry, so that the whole row in the
	     * table is ready.  Done to simplify code and avoid re-reading
	     * CSU registers for get-bulk operations.  Otherwise could switch
	     * on nominator and only initialize that struct entry.
	     */
	    dsx1CurrentEntryData.dsx1CurrentESs = current.t1_current_int.t1ES;
	    dsx1CurrentEntryData.dsx1CurrentSESs = current.t1_current_int.t1SES;
	    dsx1CurrentEntryData.dsx1CurrentSEFSs = current.t1_current_int.t1SEFS;
	    dsx1CurrentEntryData.dsx1CurrentUASs = current.t1_current_int.t1UAS;
	    dsx1CurrentEntryData.dsx1CurrentCSSs = current.t1_current_int.t1CSS;
	    dsx1CurrentEntryData.dsx1CurrentPCVs = current.t1_current_int.t1PCV;
	    dsx1CurrentEntryData.dsx1CurrentLESs = current.t1_current_int.t1LES;
	    dsx1CurrentEntryData.dsx1CurrentBESs = current.t1_current_int.t1BES;
	    dsx1CurrentEntryData.dsx1CurrentDMs = current.t1_current_int.t1DM;
	    dsx1CurrentEntryData.dsx1CurrentLCVs = current.t1_current_int.t1BPV;

	    /*
	     * record current request-id in order to determine if next call
	     * is a result of get-bulk.
	     */
	    last_serialNum = serialNum;

	    return &dsx1CurrentEntryData;
	}
    }
    return NULL;
}

/*
 * get/get-next RFC1406 dsx1IntervalTable
 * CSU/DSU card must be queried for all values.  Complete interval data is
 *   retrieved and cached between calls in order to minimize CSU/DSU reads for
 *   get-bulk operations.
 */
dsx1IntervalEntry_t     *
k_dsx1IntervalEntry_get (int serialNum,
			 ContextInfo * contextInfo,
			 int nominator,
			 int searchType,
			 long dsx1IntervalIndex,
			 long dsx1IntervalNumber)
{
    static dsx1IntervalEntry_t dsx1IntervalEntryData;
    static int               last_serialNum;
    static long              last_dsx1IntervalIndex;
    static long              last_dsx1IntervalNumber;

    uchar                    valid_intervals;     /* number valid intervals */
    uchar                    selected_interval;   /* interval we are reading */
    t1_interval_user_data    interval_data;       /* performance counters */
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    /*
     * Check to see if this was a get-bulk packet (request-id or serialNum
     * will be same as previous get) and if for same instance and same
     * interval.  If so, return information just retrieved from CSU/DSU.
     */
    if ((serialNum == last_serialNum) &&
	(dsx1IntervalIndex == dsx1IntervalEntryData.dsx1IntervalIndex) &&
    (last_dsx1IntervalNumber == dsx1IntervalEntryData.dsx1IntervalNumber)) {
	return &dsx1IntervalEntryData;
    }
    for (;;) {
	dsx1IntervalIndex = find_index_and_csu_dsu_instance(serialNum,
							    searchType,
							  dsx1IntervalIndex,
							    &p_csu_dsu,
							    CSU_DSU_FT1);
	if (p_csu_dsu) {

	    /*
	     * There is a FT1 CSU/DSU here.  See if its intervals are valid.
	     */

	    if (csu_dsu_register_RW(p_csu_dsu,
				    T1_VALID_INTERVALS_REG,
				    T1_VALID_INTERVALS_LEN,
				    &valid_intervals,
				    READ_REGISTER) == DSU_REQUEST_COMPLETE) {

		if ((dsx1IntervalNumber > valid_intervals) ||
		    (dsx1IntervalNumber > 96)) {
		    if (searchType == NEXT) {

			/*
			 * Get-Next request for Valid CSU/DSU, Invalid
			 * interval. Adjust table index to look for first
			 * interval on next instance of CSU/DSU.
			 */
			dsx1IntervalIndex++;
			dsx1IntervalNumber = 1;

			continue;         /*** CONTINUE looking at Next Instance ***/
		    }
		} else {

		    /*
		     * Archive interval data available for this CSU/DSU.
		     * Read it.
		     */

		    selected_interval = dsx1IntervalNumber;

		    if (csu_dsu_register_RW(p_csu_dsu,
					    T1_INTERVAL_SELECTOR_REG,
					    T1_INTERVAL_SELECTOR_LEN,
					    &selected_interval,
				  WRITE_REGISTER) == DSU_REQUEST_COMPLETE) {
			if (csu_dsu_register_RW(p_csu_dsu,
					      T1_SELECTED_INTERVAL_DATA_REG,
						T1_1406_INTERVAL_DATA_LEN,
						(char *) &interval_data,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {

			    /*
			     * Initialize table indexes.  Must be done
			     * regardless of which nominator (object) being
			     * read.
			     */
			    dsx1IntervalEntryData.dsx1IntervalIndex = dsx1IntervalIndex;
			    dsx1IntervalEntryData.dsx1IntervalNumber = dsx1IntervalNumber;

			    /*
			     * Initialize each struct entry, so that the
			     * whole row in the table is ready.  Done to
			     * simplify code and avoid re-reading CSU
			     * registers for get-bulk operations. Otherwise
			     * could switch on nominator and only initialize
			     * that struct entry.
			     */
			    dsx1IntervalEntryData.dsx1IntervalESs = interval_data.t1ES;
			    dsx1IntervalEntryData.dsx1IntervalSESs = interval_data.t1SES;
			    dsx1IntervalEntryData.dsx1IntervalSEFSs = interval_data.t1SEFS;
			    dsx1IntervalEntryData.dsx1IntervalUASs = interval_data.t1UAS;
			    dsx1IntervalEntryData.dsx1IntervalCSSs = interval_data.t1CSS;
			    dsx1IntervalEntryData.dsx1IntervalPCVs = interval_data.t1PCV;
			    dsx1IntervalEntryData.dsx1IntervalLESs = interval_data.t1LES;
			    dsx1IntervalEntryData.dsx1IntervalBESs = interval_data.t1BES;
			    dsx1IntervalEntryData.dsx1IntervalDMs = interval_data.t1DM;
			    dsx1IntervalEntryData.dsx1IntervalLCVs = interval_data.t1BPV;

			    /*
			     * Record current request-id in order to
			     * determine if next call is a result of
			     * get-bulk.
			     */
			    last_serialNum = serialNum;
			    last_dsx1IntervalIndex = dsx1IntervalIndex;
			    last_dsx1IntervalNumber = dsx1IntervalNumber;

			    return &dsx1IntervalEntryData;	/*** SUCCESS, got valid data ***/
			}
		    }
		}
	    }
	}
	return NULL;		/*** Everything else, FAILED to get valid data ***/
    }		
}


/*
 * get/get-next RFC1406 dsx1TotalTable
 * CSU/DSU card must be queried for all values.  Complete total data is
 *   retrieved and cached between calls in order to minimize CSU/DSU reads for
 *   get-bulk operations.
 */
dsx1TotalEntry_t        *
k_dsx1TotalEntry_get (int serialNum,
		      ContextInfo * contextInfo,
		      int nominator,
		      int searchType,
		      long dsx1TotalIndex)
{
    static dsx1TotalEntry_t  dsx1TotalEntryData;
    static int               last_serialNum;

    t1_total_user_data       total;
    csu_dsu_instance_t      *p_csu_dsu;

    /*
     * Check to see if this was a get-bulk packet (request-id or serialNum
     * will be same as previous get) and if for same instance.  If so, return
     * information just retrieved from CSU/DSU.
     */
    if ((serialNum == last_serialNum) &&
	(dsx1TotalIndex == dsx1TotalEntryData.dsx1TotalIndex)) {
	return &dsx1TotalEntryData;
    }
    dsx1TotalIndex = find_index_and_csu_dsu_instance(serialNum,
						     searchType,
						     dsx1TotalIndex,
						     &p_csu_dsu,
						     CSU_DSU_FT1);

    if (p_csu_dsu) {
	if (csu_dsu_register_RW(p_csu_dsu,
				T1_TOTAL_DATA_REG,
				T1_TOTAL_DATA_LEN,
				(char *) &total,
				READ_REGISTER) == DSU_REQUEST_COMPLETE) {

	    /*
	     * Initialize table index.  Must be done regardless of which
	     * nominator (object) being read.
	     */
	    dsx1TotalEntryData.dsx1TotalIndex = dsx1TotalIndex;

	    /*
	     * Initialize each struct entry, so that the whole row in the
	     * table is ready.  Done to simplify code and avoid re-reading
	     * CSU registers for get-bulk operations.  Otherwise could switch
	     * on nominator and only initialize that struct entry.
	     */
	    dsx1TotalEntryData.dsx1TotalESs = total.t1ES;
	    dsx1TotalEntryData.dsx1TotalSESs = total.t1SES;
	    dsx1TotalEntryData.dsx1TotalSEFSs = total.t1SEFS;
	    dsx1TotalEntryData.dsx1TotalUASs = total.t1UAS;
	    dsx1TotalEntryData.dsx1TotalCSSs = total.t1CSS;
	    dsx1TotalEntryData.dsx1TotalPCVs = total.t1PCV;
	    dsx1TotalEntryData.dsx1TotalLESs = total.t1LES;
	    dsx1TotalEntryData.dsx1TotalBESs = total.t1BES;
	    dsx1TotalEntryData.dsx1TotalDMs = total.t1DM;
	    dsx1TotalEntryData.dsx1TotalLCVs = total.t1BPV;

	    /*
	     * record current request-id in order to determine if next call
	     * is a result of get-bulk.
	     */
	    last_serialNum = serialNum;

	    return &dsx1TotalEntryData;
	}
    }
    return NULL;
}


/*
 * get/get-next RFC1406 dsx1FracTable.
 * All objects accessible in DRAM.
 */
dsx1FracEntry_t         *
k_dsx1FracEntry_get (int serialNum,
		     ContextInfo * contextInfo,
		     int nominator,
		     int searchType,
		     long dsx1FracIndex,
		     long dsx1FracNumber)
{
    static dsx1FracEntry_t   dsx1FracEntryData;
    csu_dsu_instance_t      *p_csu_dsu;
    long                     frac_index;

    /*
     * Table entry indexed by csu_dsu instance and T1 fraction number.
     */

    if (dsx1FracNumber > T1_FRACTION_MAX_TIMESLOTS) {
	if (searchType == NEXT) {

	    /*
	     * Trying get-next of non-existent fraction,  Look at next index.
	     */
	    dsx1FracIndex++;
	    dsx1FracNumber = 1;
	} else {
	    return NULL;
	}
    }
    frac_index = find_index_and_csu_dsu_instance(serialNum,
						 searchType,
						 dsx1FracIndex,
						 &p_csu_dsu,
						 CSU_DSU_FT1);
    if (p_csu_dsu) {
	if (frac_index != dsx1FracIndex) {

	    /*
	     * Must be a get-next which found a new index.  Start with first
	     * fraction.
	     */
	    dsx1FracNumber = 1;
	}
	dsx1FracEntryData.dsx1FracIndex = frac_index;
	dsx1FracEntryData.dsx1FracNumber = dsx1FracNumber;
	dsx1FracEntryData.dsx1FracIfIndex = (p_csu_dsu -> t1_timeslots_bitmap &
					     (0x1 << (dsx1FracNumber - 1))) ?
					    frac_index :
					    0;

	return &dsx1FracEntryData;
    } else {
	return NULL;
    }
}


#ifdef SETS
int
k_dsx1FracEntry_test(object, value, dp, contextInfo)
    ObjectInfo              *object;
    ObjectSyntax            *value;
    doList_t                *dp;
    ContextInfo             *contextInfo;
{

    return NO_ERROR;
}

int
k_dsx1FracEntry_ready(object, value, doHead, dp)
    ObjectInfo              *object;
    ObjectSyntax            *value;
    doList_t                *doHead;
    doList_t                *dp;
{

    dp -> state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_dsx1FracEntry_set_defaults(dp)
    doList_t                *dp;
{
/*    dsx1FracEntry_t *data = (dsx1FracEntry_t *) (dp->data);*/

    return NO_ERROR;
}

int
k_dsx1FracEntry_set(data, contextInfo, function)
    dsx1FracEntry_t         *data;
    ContextInfo             *contextInfo;
    int                      function;
{

    return GEN_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
#ifdef SR_dsx1FracEntry_UNDO
/* add #define SR_dsx1FracEntry_UNDO in ds1c3000mib.h to
 * include the undo routine for the dsx1FracEntry family.
 */
int
dsx1FracEntry_undo(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_dsx1FracEntry_UNDO */
#endif				/* SR_SNMPv2 */


void
init_ds1c3000mib (subsystype * subsys)
{
    long                    ifIndex;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    /*
     * Initialize the RFC1406 MIB subsystem.   For efficiency, MIB is loaded 
     *   only if a fractional T1 CSU/DSU interface module is present.
     */

    ifIndex = find_index_and_csu_dsu_instance( -1,            /* ignore serial num, not from a snmp request */
					       NEXT,          /* search all SNMP interfaces */
					       1,             /* Start searching at first instance */
					       &p_csu_dsu,    /* Will be initialized non-Null if FT1 present */ 
					       CSU_DSU_FT1 );

    if ( p_csu_dsu != NULL ) {
	load_mib(ds1c3000mib_OidList, ds1c3000mib_OidListNum);
	load_oid(ds1c3000mib_oid_table);
    }
}

/*
 * ds1c3000mib subsystem header
 */

#define MAJVERSION_ds1c3000mib 1
#define MINVERSION_ds1c3000mib 0
#define EDITVERSION_ds1c3000mib 0

SUBSYS_HEADER(ds1c3000mib,
	      MAJVERSION_ds1c3000mib,
	      MINVERSION_ds1c3000mib,
	      EDITVERSION_ds1c3000mib,
	      init_ds1c3000mib,
	      SUBSYS_CLASS_MANAGEMENT,
	      "req: integrated_csu_dsu",
              NULL);
      
