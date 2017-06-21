/* $Id: sr_icsudsumib.c,v 3.2.2.4 1996/07/03 20:43:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/sr_icsudsumib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, David Williams (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_icsudsumib.c,v $
 * Revision 3.2.2.4  1996/07/03  20:43:05  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.2.2.3  1996/07/01  18:45:37  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.2.2  1996/06/22  23:47:13  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.2.2.1  1996/06/13  19:08:26  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2  1996/03/29  03:50:14  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:32  sagarwal
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
#include "sr_icsudsumib.h"
#include "sr_icsudsumib-mib.h"

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

/*
 * Using "Find CSU/DSU instance" routine implemented in sr_ds1c3000mib.c
 */
extern long              find_index_and_csu_dsu_instance(int serialNum, int searchType, long ifIndex, csu_dsu_instance_t ** pp_csu_dsu, ulong csu_dsu_types_bitmask);

/*
 * Start of system-independent routines
 */



/*---------------------------------------------------------------------
 * Retrieve data from the dsx1ConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
dsx1ConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    dsx1ConfigEntry_t       *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     dsx1LineIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1LineIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (searchType == NEXT) {
	dsx1LineIndex = MAX(1, dsx1LineIndex);
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1ConfigEntry_get(serialNum, contextInfo, arg, searchType, dsx1LineIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> dsx1LineIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1LineIndex
      case I_dsx1LineIndex:
	dp = (void *) (&data -> dsx1LineIndex);
	break;
#endif				       /* I_dsx1LineIndex */

#ifdef I_dsx1IfIndex
      case I_dsx1IfIndex:
	dp = (void *) (&data -> dsx1IfIndex);
	break;
#endif				       /* I_dsx1IfIndex */

#ifdef I_dsx1TimeElapsed
      case I_dsx1TimeElapsed:
	dp = (void *) (&data -> dsx1TimeElapsed);
	break;
#endif				       /* I_dsx1TimeElapsed */

#ifdef I_dsx1ValidIntervals
      case I_dsx1ValidIntervals:
	dp = (void *) (&data -> dsx1ValidIntervals);
	break;
#endif				       /* I_dsx1ValidIntervals */

#ifdef I_dsx1LineType
      case I_dsx1LineType:
	dp = (void *) (&data -> dsx1LineType);
	break;
#endif				       /* I_dsx1LineType */

#ifdef I_dsx1LineCoding
      case I_dsx1LineCoding:
	dp = (void *) (&data -> dsx1LineCoding);
	break;
#endif				       /* I_dsx1LineCoding */

#ifdef I_dsx1SendCode
      case I_dsx1SendCode:
	dp = (void *) (&data -> dsx1SendCode);
	break;
#endif				       /* I_dsx1SendCode */

#ifdef I_dsx1CircuitIdentifier
      case I_dsx1CircuitIdentifier:
	dp = (void *) (CloneOctetString(data -> dsx1CircuitIdentifier));
	break;
#endif				       /* I_dsx1CircuitIdentifier */

#ifdef I_dsx1LoopbackConfig
      case I_dsx1LoopbackConfig:
	dp = (void *) (&data -> dsx1LoopbackConfig);
	break;
#endif				       /* I_dsx1LoopbackConfig */

#ifdef I_dsx1LineStatus
      case I_dsx1LineStatus:
	dp = (void *) (&data -> dsx1LineStatus);
	break;
#endif				       /* I_dsx1LineStatus */

#ifdef I_dsx1SignalMode
      case I_dsx1SignalMode:
	dp = (void *) (&data -> dsx1SignalMode);
	break;
#endif				       /* I_dsx1SignalMode */

#ifdef I_dsx1TransmitClockSource
      case I_dsx1TransmitClockSource:
	dp = (void *) (&data -> dsx1TransmitClockSource);
	break;
#endif				       /* I_dsx1TransmitClockSource */

#ifdef I_dsx1Fdl
      case I_dsx1Fdl:
	dp = (void *) (&data -> dsx1Fdl);
	break;
#endif				       /* I_dsx1Fdl */

#ifdef I_ciscoICsuDsuT1LineBuildOut
      case I_ciscoICsuDsuT1LineBuildOut:
	dp = (void *) (&data -> ciscoICsuDsuT1LineBuildOut);
	break;
#endif				       /* I_ciscoICsuDsuT1LineBuildOut */

#ifdef I_ciscoICsuDsuT1DteLineCode
      case I_ciscoICsuDsuT1DteLineCode:
	dp = (void *) (&data -> ciscoICsuDsuT1DteLineCode);
	break;
#endif				       /* I_ciscoICsuDsuT1DteLineCode */

#ifdef I_ciscoICsuDsuT1SupportRemoteAlarmIndication
      case I_ciscoICsuDsuT1SupportRemoteAlarmIndication:
	dp = (void *) (&data -> ciscoICsuDsuT1SupportRemoteAlarmIndication);
	break;
#endif				       /* I_ciscoICsuDsuT1SupportRemoteAlarmIndication */

#ifdef I_ciscoICsuDsuT1FullBandwidthRemoteLoopcode
    case I_ciscoICsuDsuT1FullBandwidthRemoteLoopcode:
       dp = (void *) (&data->ciscoICsuDsuT1FullBandwidthRemoteLoopcode);
       break;
#endif 					/* I_ciscoICsuDsuT1FullBandwidthRemoteLoopcode */

#ifdef I_ciscoICsuDsuT1PayloadRemoteLoopcode
    case I_ciscoICsuDsuT1PayloadRemoteLoopcode:
       dp = (void *) (&data->ciscoICsuDsuT1PayloadRemoteLoopcode);
       break;
#endif 					/* I_ciscoICsuDsuT1PayloadRemoteLoopcode */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dsx1ConfigEntry data object.
 *---------------------------------------------------------------------*/
void
dsx1ConfigEntry_free(dsx1ConfigEntry_t * data)
{
    if (data != NULL) {
	FreeOctetString(data -> dsx1CircuitIdentifier);

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dsx1ConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
dsx1ConfigEntry_cleanup(doList_t * trash)
{
    dsx1ConfigEntry_free(trash -> data);
#ifdef SR_SNMPv2
    dsx1ConfigEntry_free(trash -> undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the dsx1ConfigEntry family
 *---------------------------------------------------------------------*/
dsx1ConfigEntry_t       *
Clone_dsx1ConfigEntry(dsx1ConfigEntry_t * dsx1ConfigEntry)
{
    dsx1ConfigEntry_t       *data;

    if ((data = malloc(sizeof(dsx1ConfigEntry_t))) == NULL) {
	return NULL;
    }
    memcpy((char *) (data), (char *) (dsx1ConfigEntry), sizeof(dsx1ConfigEntry_t));

    data -> dsx1CircuitIdentifier = CloneOctetString(dsx1ConfigEntry -> dsx1CircuitIdentifier);

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
dsx1ConfigEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    dsx1ConfigEntry_t       *dsx1ConfigEntry;
    long                     dsx1LineIndex;

    /*
     * Validate the object instance
     *
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object -> oid.length, &dsx1LineIndex, EXACT, &carry)) < 0) {
	error_status = NO_CREATION_ERROR;
    }
    if (error_status != NO_ERROR) {
	return error_status;
    }
    dsx1ConfigEntry = k_dsx1ConfigEntry_get(-1, contextInfo, -1, EXACT, dsx1LineIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp -> next) {
	if ((dp -> setMethod == dsx1ConfigEntry_set) &&
	    (((dsx1ConfigEntry_t *) (dp -> data)) != NULL) &&
	    (((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1LineIndex == dsx1LineIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;

	dp -> setMethod = dsx1ConfigEntry_set;
	dp -> cleanupMethod = dsx1ConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_dsx1ConfigEntry_UNDO
	dp -> undoMethod = dsx1ConfigEntry_undo;
#else				       /* SR_dsx1ConfigEntry_UNDO */
	dp -> undoMethod = NULL;
#endif				       /* SR_dsx1ConfigEntry_UNDO */
#endif				       /* SR_SNMPv2 */
	dp -> state = UNKNOWN;

	if (dsx1ConfigEntry != NULL) {
	    /* fill in existing values */
	    if ((dp -> data = (void *) Clone_dsx1ConfigEntry(dsx1ConfigEntry)) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    }
#ifdef SR_SNMPv2
	    if ((dp -> undodata = (void *) Clone_dsx1ConfigEntry(dsx1ConfigEntry)) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    }
#endif				       /* SR_SNMPv2 */

	} else {

#ifdef dsx1ConfigEntry_READ_CREATE
	    if ((dp -> data = malloc(sizeof(dsx1ConfigEntry_t))) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    } else {
		memset(dp -> data, 0, sizeof(dsx1ConfigEntry_t));
#ifdef SR_SNMPv2
		dp -> undodata = NULL;
#endif				       /* SR_SNMPv2 */

		/* Fill in reasonable default values for this new entry */
		((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1LineIndex = (dsx1LineIndex);
		SET_VALID(I_dsx1LineIndex, ((dsx1ConfigEntry_t *) (dp -> data)) -> valid);

		error_status = k_dsx1ConfigEntry_set_defaults(dp);
	    }
#else				       /* dsx1ConfigEntry_READ_CREATE */
	    error_status = NO_ACCESS_ERROR;
#endif				       /* dsx1ConfigEntry_READ_CREATE */
	}
    }
    if (error_status != NO_ERROR) {
	return error_status;
    }
    switch (object -> nominator) {

#ifdef I_dsx1LineType
      case I_dsx1LineType:

	if ((value -> sl_value < 1) || (value -> sl_value > 7)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1LineType = value -> sl_value;
	break;
#endif				       /* I_dsx1LineType */

#ifdef I_dsx1LineCoding
      case I_dsx1LineCoding:

	if ((value -> sl_value < 1) || (value -> sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1LineCoding = value -> sl_value;
	break;
#endif				       /* I_dsx1LineCoding */

#ifdef I_dsx1SendCode
      case I_dsx1SendCode:

	if ((value -> sl_value < 1) || (value -> sl_value > 8)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1SendCode = value -> sl_value;
	break;
#endif				       /* I_dsx1SendCode */

#ifdef I_dsx1CircuitIdentifier
      case I_dsx1CircuitIdentifier:

	if (((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1CircuitIdentifier != NULL) {
	    FreeOctetString(((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1CircuitIdentifier);
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1CircuitIdentifier =
	    CloneOctetString(value -> os_value);

	break;
#endif				       /* I_dsx1CircuitIdentifier */

#ifdef I_dsx1LoopbackConfig
      case I_dsx1LoopbackConfig:

	if ((value -> sl_value < 1) || (value -> sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1LoopbackConfig = value -> sl_value;
	break;
#endif				       /* I_dsx1LoopbackConfig */

#ifdef I_dsx1SignalMode
      case I_dsx1SignalMode:

	if ((value -> sl_value < 1) || (value -> sl_value > 4)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1SignalMode = value -> sl_value;
	break;
#endif				       /* I_dsx1SignalMode */

#ifdef I_dsx1TransmitClockSource
      case I_dsx1TransmitClockSource:

	if ((value -> sl_value < 1) || (value -> sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}
	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1TransmitClockSource = value -> sl_value;
	break;
#endif				       /* I_dsx1TransmitClockSource */

#ifdef I_dsx1Fdl
      case I_dsx1Fdl:

	switch (value -> sl_value) {
	  case 1:
	  case 2:
	  case 4:
	  case 8:
	    break;
	  default:
	    return WRONG_VALUE_ERROR;
	}

	((dsx1ConfigEntry_t *) (dp -> data)) -> dsx1Fdl = value -> sl_value;
	break;
#endif				       /* I_dsx1Fdl */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dsx1ConfigEntry_test)\n"));
	return GEN_ERROR;

    }				       /* switch */

    /* Do system dependent testing in k_dsx1ConfigEntry_test */
    error_status = k_dsx1ConfigEntry_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object -> nominator, ((dsx1ConfigEntry_t *) (dp -> data)) -> valid);
	error_status = k_dsx1ConfigEntry_ready(object, value, doHead, dp);
    }
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
dsx1ConfigEntry_set(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return (k_dsx1ConfigEntry_set((dsx1ConfigEntry_t *) (doCur -> data),
				  contextInfo, doCur -> state));
}

#endif				/* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuStaticConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuStaticConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    ciscoICsuDsuStaticConfigEntry_t *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     ifIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuStaticConfigEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> ifIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuType
      case I_ciscoICsuDsuType:
	dp = (void *) (&data -> ciscoICsuDsuType);
	break;
#endif				       /* I_ciscoICsuDsuType */

#ifdef I_ciscoICsuDsuHwRevision
      case I_ciscoICsuDsuHwRevision:
	dp = (void *) (CloneOctetString(data -> ciscoICsuDsuHwRevision));
	break;
#endif				       /* I_ciscoICsuDsuHwRevision */

#ifdef I_ciscoICsuDsuSwRevision
      case I_ciscoICsuDsuSwRevision:
	dp = (void *) (CloneOctetString(data -> ciscoICsuDsuSwRevision));
	break;
#endif				       /* I_ciscoICsuDsuSwRevision */

#ifdef I_ciscoICsuDsuProtocolRevision
      case I_ciscoICsuDsuProtocolRevision:
	dp = (void *) (CloneOctetString(data -> ciscoICsuDsuProtocolRevision));
	break;
#endif				       /* I_ciscoICsuDsuProtocolRevision */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuTestReportEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuTestReportEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    ciscoICsuDsuTestReportEntry_t *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     ifIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuTestReportEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> ifIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuLastSelfTestResult
      case I_ciscoICsuDsuLastSelfTestResult:
	dp = (void *) (&data -> ciscoICsuDsuLastSelfTestResult);
	break;
#endif				       /* I_ciscoICsuDsuLastSelfTestResult */

#ifdef I_ciscoICsuDsuTimeOfLastSelfTest
      case I_ciscoICsuDsuTimeOfLastSelfTest:
	dp = (void *) (&data -> ciscoICsuDsuTimeOfLastSelfTest);
	break;
#endif				       /* I_ciscoICsuDsuTimeOfLastSelfTest */

#ifdef I_ciscoICsuDsuNumResets
      case I_ciscoICsuDsuNumResets:
	dp = (void *) (&data -> ciscoICsuDsuNumResets);
	break;
#endif				       /* I_ciscoICsuDsuNumResets */

#ifdef I_ciscoICsuDsuTimeOfLastReset
      case I_ciscoICsuDsuTimeOfLastReset:
	dp = (void *) (&data -> ciscoICsuDsuTimeOfLastReset);
	break;
#endif				       /* I_ciscoICsuDsuTimeOfLastReset */

#ifdef I_ciscoICsuDsuLoopbackStatus
      case I_ciscoICsuDsuLoopbackStatus:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackStatus);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackStatus */

#ifdef I_ciscoICsuDsuLoopbackNumErrors
      case I_ciscoICsuDsuLoopbackNumErrors:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackNumErrors);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackNumErrors */

#ifdef I_ciscoICsuDsuLoopbackDuration
      case I_ciscoICsuDsuLoopbackDuration:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackDuration);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackDuration */

#ifdef I_ciscoICsuDsuLoopbackPoint
      case I_ciscoICsuDsuLoopbackPoint:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackPoint);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackPoint */

#ifdef I_ciscoICsuDsuLoopbackPattern
      case I_ciscoICsuDsuLoopbackPattern:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackPattern);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackPattern */

#ifdef I_ciscoICsuDsuUserDefinedPattern
      case I_ciscoICsuDsuUserDefinedPattern:
	dp = (void *) (CloneOctetString(data -> ciscoICsuDsuUserDefinedPattern));
	break;
#endif				       /* I_ciscoICsuDsuUserDefinedPattern */

#ifdef I_ciscoICsuDsuLoopbackCode
      case I_ciscoICsuDsuLoopbackCode:
	dp = (void *) (&data -> ciscoICsuDsuLoopbackCode);
	break;
#endif				       /* I_ciscoICsuDsuLoopbackCode */

#ifdef I_ciscoICsuDsuEndTimeOfLastLoopback
      case I_ciscoICsuDsuEndTimeOfLastLoopback:
	dp = (void *) (&data -> ciscoICsuDsuEndTimeOfLastLoopback);
	break;
#endif				       /* I_ciscoICsuDsuEndTimeOfLastLoopback */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuT1StatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuT1StatusEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    ciscoICsuDsuT1StatusEntry_t *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     ifIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuT1StatusEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> ifIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuT1LoopStatus
      case I_ciscoICsuDsuT1LoopStatus:
	dp = (void *) (&data -> ciscoICsuDsuT1LoopStatus);
	break;
#endif				       /* I_ciscoICsuDsuT1LoopStatus */

#ifdef I_ciscoICsuDsuT1LossOfSignals
      case I_ciscoICsuDsuT1LossOfSignals:
	dp = (void *) (&data -> ciscoICsuDsuT1LossOfSignals);
	break;
#endif				       /* I_ciscoICsuDsuT1LossOfSignals */

#ifdef I_ciscoICsuDsuT1LossOfFrames
      case I_ciscoICsuDsuT1LossOfFrames:
	dp = (void *) (&data -> ciscoICsuDsuT1LossOfFrames);
	break;
#endif				       /* I_ciscoICsuDsuT1LossOfFrames */

#ifdef I_ciscoICsuDsuT1RemoteAlarmIndications
      case I_ciscoICsuDsuT1RemoteAlarmIndications:
	dp = (void *) (&data -> ciscoICsuDsuT1RemoteAlarmIndications);
	break;
#endif				       /* I_ciscoICsuDsuT1RemoteAlarmIndicatio
				        * ns */

#ifdef I_ciscoICsuDsuT1AlarmIndicationSignals
      case I_ciscoICsuDsuT1AlarmIndicationSignals:
	dp = (void *) (&data -> ciscoICsuDsuT1AlarmIndicationSignals);
	break;
#endif				       /* I_ciscoICsuDsuT1AlarmIndicationSigna
				        * ls */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuSw56kConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuSw56kConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    ciscoICsuDsuSw56kConfigEntry_t *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     ifIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuSw56kConfigEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> ifIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuSw56kNetworkType
      case I_ciscoICsuDsuSw56kNetworkType:
	dp = (void *) (&data -> ciscoICsuDsuSw56kNetworkType);
	break;
#endif				       /* I_ciscoICsuDsuSw56kNetworkType */

#ifdef I_ciscoICsuDsuSw56kClockSource
      case I_ciscoICsuDsuSw56kClockSource:
	dp = (void *) (&data -> ciscoICsuDsuSw56kClockSource);
	break;
#endif				       /* I_ciscoICsuDsuSw56kClockSource */

#ifdef I_ciscoICsuDsuSw56kLoopRate
      case I_ciscoICsuDsuSw56kLoopRate:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLoopRate);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLoopRate */

#ifdef I_ciscoICsuDsuSw56kScramblerEnabled
      case I_ciscoICsuDsuSw56kScramblerEnabled:
	dp = (void *) (&data -> ciscoICsuDsuSw56kScramblerEnabled);
	break;
#endif				       /* I_ciscoICsuDsuSw56kScramblerEnabled */

#ifdef I_ciscoICsuDsuSw56kRemoteLoopbackEnabled
      case I_ciscoICsuDsuSw56kRemoteLoopbackEnabled:
	dp = (void *) (&data -> ciscoICsuDsuSw56kRemoteLoopbackEnabled);
	break;
#endif				       /* I_ciscoICsuDsuSw56kRemoteLoopbackEna
				        * bled */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuSw56kLineStatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuSw56kLineStatusEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      instLength = incoming -> length - object -> oid.length;
    int                      arg = object -> nominator;
    void                    *dp;
    ciscoICsuDsuSw56kLineStatusEntry_t *data = NULL;
    unsigned long            buffer[1];
    OID                      inst;
    int                      carry;
    long                     ifIndex;


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

    if ((InstToInt(incoming, 0 + object -> oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuSw56kLineStatusEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {

	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data -> ifIndex;
    }


    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuSw56kDialingStatus
      case I_ciscoICsuDsuSw56kDialingStatus:
	dp = (void *) (&data -> ciscoICsuDsuSw56kDialingStatus);
	break;
#endif				       /* I_ciscoICsuDsuSw56kDialingStatus */

#ifdef I_ciscoICsuDsuSw56kLoopStatus
      case I_ciscoICsuDsuSw56kLoopStatus:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLoopStatus);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLoopStatus */

#ifdef I_ciscoICsuDsuSw56kReceivedOosOofs
      case I_ciscoICsuDsuSw56kReceivedOosOofs:
	dp = (void *) (&data -> ciscoICsuDsuSw56kReceivedOosOofs);
	break;
#endif				       /* I_ciscoICsuDsuSw56kReceivedOosOofs */

#ifdef I_ciscoICsuDsuSw56kLostSealingCurrents
      case I_ciscoICsuDsuSw56kLostSealingCurrents:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLostSealingCurrents);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLostSealingCurren
				        * ts */

#ifdef I_ciscoICsuDsuSw56kLostReceiveSignals
      case I_ciscoICsuDsuSw56kLostReceiveSignals:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLostReceiveSignals);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLostReceiveSignal
				        * s */

#ifdef I_ciscoICsuDsuSw56kLostFrameSyncs
      case I_ciscoICsuDsuSw56kLostFrameSyncs:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLostFrameSyncs);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLostFrameSyncs */

#ifdef I_ciscoICsuDsuSw56kLoopRateSearches
      case I_ciscoICsuDsuSw56kLoopRateSearches:
	dp = (void *) (&data -> ciscoICsuDsuSw56kLoopRateSearches);
	break;
#endif				       /* I_ciscoICsuDsuSw56kLoopRateSearches */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ciscoICsuDsuMIBNotificationEnables family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind                 *
ciscoICsuDsuMIBNotificationEnables_get(incoming, object, searchType, contextInfo, serialNum)
    OID                     *incoming;
    ObjectInfo              *object;
    int                      searchType;
    ContextInfo             *contextInfo;
    int                      serialNum;
{
    int                      arg;
    void                    *dp;
    ciscoICsuDsuMIBNotificationEnables_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ciscoICsuDsuMIBNotificationEnables_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ciscoICsuDsuEnableT1LoopStatusNotification
      case I_ciscoICsuDsuEnableT1LoopStatusNotification:
	dp = (void *) (&data -> ciscoICsuDsuEnableT1LoopStatusNotification);
	break;
#endif				       /* I_ciscoICsuDsuEnableT1LoopStatusNoti
				        * fication */

#ifdef I_ciscoICsuDsuEnableSw56kLoopStatusNotification
      case I_ciscoICsuDsuEnableSw56kLoopStatusNotification:
	dp = (void *) (&data -> ciscoICsuDsuEnableSw56kLoopStatusNotification);
	break;
#endif				       /* I_ciscoICsuDsuEnableSw56kLoopStatusN
				        * otification */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ciscoICsuDsuMIBNotificationEnables data object.
 *---------------------------------------------------------------------*/
void
ciscoICsuDsuMIBNotificationEnables_free(ciscoICsuDsuMIBNotificationEnables_t * data)
{
    if (data != NULL) {

	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ciscoICsuDsuMIBNotificationEnables set/undo
 *---------------------------------------------------------------------*/
static int
ciscoICsuDsuMIBNotificationEnables_cleanup(doList_t * trash)
{
    ciscoICsuDsuMIBNotificationEnables_free(trash -> data);
#ifdef SR_SNMPv2
    ciscoICsuDsuMIBNotificationEnables_free(trash -> undodata);
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
ciscoICsuDsuMIBNotificationEnables_test(incoming, object, value, doHead, doCur, contextInfo)
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

    /*
     * Validate the object instance: 1) It must be of length 1  2) and the
     * instance must be 0.
     */
    if (instLength != 1 || incoming -> oid_ptr[incoming -> length - 1] != 0) {
	return (NO_CREATION_ERROR);
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp -> next) {
	if ((dp -> setMethod == ciscoICsuDsuMIBNotificationEnables_set) &&
	(((ciscoICsuDsuMIBNotificationEnables_t *) (dp -> data)) != NULL)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp -> data = malloc(sizeof(ciscoICsuDsuMIBNotificationEnables_t))) == NULL) {
	    DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp -> data, 0, sizeof(ciscoICsuDsuMIBNotificationEnables_t));
#ifdef SR_SNMPv2
	if ((dp -> undodata = malloc(sizeof(ciscoICsuDsuMIBNotificationEnables_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
	memset(dp -> undodata, 0, sizeof(ciscoICsuDsuMIBNotificationEnables_t));
#endif				       /* SR_SNMPv2 */

	dp -> setMethod = ciscoICsuDsuMIBNotificationEnables_set;
	dp -> cleanupMethod = ciscoICsuDsuMIBNotificationEnables_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ciscoICsuDsuMIBNotificationEnables_UNDO
	dp -> undoMethod = ciscoICsuDsuMIBNotificationEnables_undo;
#else				       /* SR_ciscoICsuDsuMIBNotificationEnable
				        * s_UNDO */
	dp -> undoMethod = NULL;
#endif				       /* SR_ciscoICsuDsuMIBNotificationEnable
				        * s_UNDO */
#endif				       /* SR_SNMPv2 */
	dp -> state = UNKNOWN;

    }
    if (error_status != NO_ERROR) {
	return error_status;
    }
    switch (object -> nominator) {

#ifdef I_ciscoICsuDsuEnableT1LoopStatusNotification
      case I_ciscoICsuDsuEnableT1LoopStatusNotification:

	if ((value -> sl_value < 1) || (value -> sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	((ciscoICsuDsuMIBNotificationEnables_t *) (dp -> data)) -> ciscoICsuDsuEnableT1LoopStatusNotification = value -> sl_value;
	break;
#endif				       /* I_ciscoICsuDsuEnableT1LoopStatusNoti
				        * fication */

#ifdef I_ciscoICsuDsuEnableSw56kLoopStatusNotification
      case I_ciscoICsuDsuEnableSw56kLoopStatusNotification:

	if ((value -> sl_value < 1) || (value -> sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	((ciscoICsuDsuMIBNotificationEnables_t *) (dp -> data)) -> ciscoICsuDsuEnableSw56kLoopStatusNotification = value -> sl_value;
	break;
#endif				       /* I_ciscoICsuDsuEnableSw56kLoopStatusN
				        * otification */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoICsuDsuMIBNotificationEnables_test)\n"));
	return GEN_ERROR;

    }				       /* switch */

    /*
     * Do system dependent testing in
     * k_ciscoICsuDsuMIBNotificationEnables_test
     */
    error_status = k_ciscoICsuDsuMIBNotificationEnables_test(object, value, dp, contextInfo);

    if (error_status == NO_ERROR) {
	SET_VALID(object -> nominator, ((ciscoICsuDsuMIBNotificationEnables_t *) (dp -> data)) -> valid);
	error_status = k_ciscoICsuDsuMIBNotificationEnables_ready(object, value, doHead, dp);
    }
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
ciscoICsuDsuMIBNotificationEnables_set(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return (k_ciscoICsuDsuMIBNotificationEnables_set((ciscoICsuDsuMIBNotificationEnables_t *) (doCur -> data),
					      contextInfo, doCur -> state));
}

#endif				/* SETS */


/****************************************************************************/
/* Start of system-dependent routines                                       */
/****************************************************************************/

/*
 * get/get-next RFC1406 dsx1ConfigTable
 * Most objects accessible in DRAM, however CSU/DSU card must be queried for
 *   some values.  Therefore no addition caching or optimization for get-bulk
 *   is done.
 *
 * Note on searchType parameter.  k_xxx_get() will receive:
 *  GET_REQUEST_TYPE or EXACT
 *  GET_NEXT_REQUEST_TYPE or NEXT
 *  GET_BULK_REQUEST_TYPE is not seen by user routines, instead mapped into
 *    multiple GET_REQUEST_TYPE by snmp engine.
 */
dsx1ConfigEntry_t       *
k_dsx1ConfigEntry_get (int serialNum,
		       ContextInfo * contextInfo,
		       int nominator,
		       int searchType,
		       long dsx1LineIndex)
{
    static dsx1ConfigEntry_t dsx1ConfigEntryData;

    /*
     * Circuit id (dsx1ConfigEntryData_circuit_id) is unknown.  Initialize a
     * NULL octet string for later retrieval.
     */
    static OctetString       ostring_null = {NULL, 0};
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    dsx1LineIndex = find_index_and_csu_dsu_instance(serialNum,
						    searchType,
						    dsx1LineIndex,
						    &p_csu_dsu,
						    CSU_DSU_FT1);
    if (p_csu_dsu) {
	dsx1ConfigEntryData.dsx1LineIndex = dsx1LineIndex;	/* initialize table
								 * index */

	switch (nominator) {	       /* get particular object  */
	  case I_dsx1LineIndex:       /* initialized above */
	    break;

	  case I_dsx1IfIndex:
	    dsx1ConfigEntryData.dsx1IfIndex = dsx1LineIndex;
	    break;

	  case I_dsx1TimeElapsed:
	    {
		unsigned short           time_remaining;

		if (csu_dsu_register_RW(p_csu_dsu,
					T1_CURRENT_INTERVAL_SECONDS_REG,
					T1_CURRENT_INTERVAL_SECONDS_LEN,
					(char *) &time_remaining,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    dsx1ConfigEntryData.dsx1TimeElapsed = time_remaining;
		} else {
		    return NULL;
		}
		break;
	    }

	  case I_dsx1ValidIntervals:
	    {
		unsigned char            valid_intervals;

		if (csu_dsu_register_RW(p_csu_dsu,
					T1_VALID_INTERVALS_REG,
					T1_VALID_INTERVALS_LEN,
					(char *) &valid_intervals,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    dsx1ConfigEntryData.dsx1ValidIntervals = valid_intervals;
		} else {
		    return NULL;
		}
		break;
	    }

	  case I_dsx1LineType:
	    if (p_csu_dsu -> t1_framing_type == CSU_DSU_FRAMING_ESF) {
		dsx1ConfigEntryData.dsx1LineType = D_dsx1LineType_dsx1ESF;
	    } else {
		dsx1ConfigEntryData.dsx1LineType = D_dsx1LineType_dsx1D4;	/* SF and D4 are
										 * synonymous */
	    }
	    break;

	  case I_dsx1LineCoding:
	    if (p_csu_dsu -> t1_line_type == CSU_DSU_LINECODE_B8ZS) {
		dsx1ConfigEntryData.dsx1LineCoding = D_dsx1LineCoding_dsx1B8ZS;
	    } else {
		dsx1ConfigEntryData.dsx1LineCoding = D_dsx1LineCoding_dsx1AMI;
	    }
	    break;

	  case I_dsx1SendCode:
	    if (p_csu_dsu -> current_loopback_command == CSU_DSU_LOOPBACK_REMOTE) {
		switch (p_csu_dsu -> current_loopback_state) {
		    case DSU_IN_LOOPBACK:
			switch (p_csu_dsu->dsu_loopback_test_pattern) {
		            case CSU_DSU_LPBACK_PAT_NOSEL:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendNoCode;
			        break;
			    case CSU_DSU_LPBACK_PAT_3in24:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1Send3in24Pattern;
			        break;
			    case CSU_DSU_LPBACK_PAT_QRW:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendQRS;
			        break;
			    default:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendOtherTestPattern;
			        break;
			}
			break;

		    case DSU_ATTEMPTING_LOOPUP:
			switch (p_csu_dsu ->dsu_loopback_type) {
		            case CSU_DSU_LPBACK_TYPE_DEF:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendLineCode;
			        break;
			    case CSU_DSU_LPBACK_TYPE_FR:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendPayloadCode;
			        break;
			    case CSU_DSU_LPBACK_TYPE_SJ:
			    default:
			        dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendOtherTestPattern;
			}
			break;

		    case DSU_ATTEMPTING_LOOPDOWN:
			dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendResetCode;
			break;

		    default:
			dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendNoCode;
		}
	    } else {
		/*
		 * Handling the remaining current_loopback_command states.
		 * DTE loopbacks  - at this point we can only send user data - no test patterns
		 * Line loopbacks - we're looped so we don't know what is on network
		 * No loopback    - sending normal data.
		 */
		dsx1ConfigEntryData.dsx1SendCode = D_dsx1SendCode_dsx1SendNoCode;
	    }
	    break;

	  case I_dsx1CircuitIdentifier:

	    /*
	     * Circuit ID is unknown.  Return NULL valued Octet string.
	     */
	    dsx1ConfigEntryData.dsx1CircuitIdentifier = &ostring_null;
	    break;

	  case I_dsx1LoopbackConfig:
	    {
		uchar                    test_type;

		if (csu_dsu_register_RW(p_csu_dsu,
					DSU_TEST_TYPE_SELECT_REG,
					DSU_TEST_TYPE_SELECT_LEN,
					&test_type,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    if (test_type == DSU_LOOPBACK_NONE) {
			dsx1ConfigEntryData.dsx1LoopbackConfig = D_dsx1LoopbackConfig_dsx1NoLoop;
		    } else if (test_type == DSU_LOCAL_PAYLOAD_LOOPBACK) {
			dsx1ConfigEntryData.dsx1LoopbackConfig = D_dsx1LoopbackConfig_dsx1PayloadLoop;
		    } else if (test_type == DSU_LOCAL_NET_LOOPBACK) {
			dsx1ConfigEntryData.dsx1LoopbackConfig = D_dsx1LoopbackConfig_dsx1LineLoop;
		    } else {

			/*
			 * Some other test or loopback is running on the
			 * interface.
			 */
			dsx1ConfigEntryData.dsx1LoopbackConfig = D_dsx1LoopbackConfig_dsx1OtherLoop;
		    }
		} else {
		    return NULL;
		}
		break;
	    }
	  case I_dsx1LineStatus:
	    {
		ushort                   unit_status;
		ushort                   t1_status;
		ushort                   dsx1_line_status = 0;

		if (csu_dsu_register_RW(p_csu_dsu,
					T1_UNIT_STATUS_REG,
					T1_UNIT_STATUS_LEN,
					(char *) &unit_status,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    if (csu_dsu_register_RW(p_csu_dsu,
					    T1_RECEIVER_STATUS_REG,
					    T1_RECEIVER_STATUS_LEN,
					    (char *) &t1_status,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {

			if (unit_status & B_FT1_UNIT_STATUS_SENDING_YELLOW) {
			    dsx1_line_status |= B_dsx1LineStatus_XmtFarEndLOF;
			}
			if (unit_status & B_FT1_UNIT_STATUS_SENDING_AIS) {
			    dsx1_line_status |= B_dsx1LineStatus_XmtAIS;
			}

			/*
                         * Following alarms have been deemed to not affect
                         *    dsx1_line_status.
			 * - B_FT1_UNIT_STATUS_LOSS_RTS_OR_DTR
			 * - B_FT1_UNIT_STATUS_EXCESS_ZEROS
			 * - B_FT1_UNIT_STATUS_FERR
			 * - B_FT1_UNIT_STATUS_IN_TEST
			 * Mapping rest of alarm bits to Other Failure.
			 */
			if (unit_status & B_FT1_UNIT_STATUS_UNAVAIL_SIGNAL_STATE ) {
			    dsx1_line_status |= B_dsx1LineStatus_OtherFailure;
			}
			if (t1_status & B_FT1_RX_STATUS_CARRIER_LOSS) {
			    dsx1_line_status |= B_dsx1LineStatus_LossOfSignal;
			}
			if (t1_status & B_FT1_RX_STATUS_LOSS_OF_SYNC) {
			    dsx1_line_status |= B_dsx1LineStatus_LossOfFrame;
			}
			if (t1_status & B_FT1_RX_STATUS_YELLOW) {
			    dsx1_line_status |= B_dsx1LineStatus_RcvFarEndLOF;
			}
			if (t1_status & B_FT1_RX_STATUS_BLUE) {
			    dsx1_line_status |= B_dsx1LineStatus_RcvAIS;
			}

			/*
			 * Rest of t1 status bits:
			 *   - B_FT1_RX_STATUS_SET_CODE
			 *   - B_FT1_RX_STATUS_RESET_CODE
			 *  (going in or out of Loopback) do not affect
			 *  line_status.
			 */

			/*
			 * Indicate if near end is looped.
			 */
			if ((p_csu_dsu -> current_loopback_command == CSU_DSU_LOOPBACK_DTE) ||
			    (p_csu_dsu -> current_loopback_command == CSU_DSU_LOOPBACK_LINE)) {
			    dsx1_line_status |= B_dsx1LineStatus_LoopbackState;
			}

			/*
			 * Finally check alarm status to see if remote side
			 *    has placed us in loopback.
			 */
			if (p_csu_dsu -> dsu_network_alarm_status & B_FT1_ALARM_STATUS_NET_PAYLOAD_LOOPBACK) {
			    dsx1_line_status |= B_dsx1LineStatus_LoopbackState;
			}

			dsx1ConfigEntryData.dsx1LineStatus = dsx1_line_status ?
			    dsx1_line_status :
			    B_dsx1LineStatus_NoAlarm;
			break;
		    }
		}

		/*
		 * Oops, can't read any realtime status information.  Since
		 * this should Rarely happen, could potentially return
		 * OtherFailure to give user some clue that a problem may
		 * exist.  But a NULL return is consistent with other get
		 * object implementations and should also indicate problem.
		 */
		return NULL;
	    }

	  case I_dsx1SignalMode:

	    /*
	     * Hardware does not support any of the signalling modes.
	     */
	    dsx1ConfigEntryData.dsx1SignalMode = D_dsx1SignalMode_none;
	    break;

	  case I_dsx1TransmitClockSource:
	    dsx1ConfigEntryData.dsx1TransmitClockSource =
		(p_csu_dsu -> dsu_clocksource == CSU_DSU_CLK_SRC_LINE) ?
		D_dsx1TransmitClockSource_loopTiming :
		D_dsx1TransmitClockSource_localTiming;
	    break;

	  case I_dsx1Fdl:

	    /*
	     * Hardware supports both Ansi-T1-403 and Att-54016
	     */
	    dsx1ConfigEntryData.dsx1Fdl = D_dsx1Fdl_dsx1Ansi_T1_403 | D_dsx1Fdl_dsx1Att_54016;
	    break;

	  case I_ciscoICsuDsuT1LineBuildOut:
	    if (p_csu_dsu -> t1_line_build_out == CSU_DSU_LBO_NONE) {
		dsx1ConfigEntryData.ciscoICsuDsuT1LineBuildOut = D_ciscoICsuDsuT1LineBuildOut_buildOut0;
	    } else if (p_csu_dsu -> t1_line_build_out == CSU_DSU_LBO_7_5_DB) {
		dsx1ConfigEntryData.ciscoICsuDsuT1LineBuildOut = D_ciscoICsuDsuT1LineBuildOut_buildOut7p5;
	    } else {
		dsx1ConfigEntryData.ciscoICsuDsuT1LineBuildOut = D_ciscoICsuDsuT1LineBuildOut_buildOut15;
	    }
	    break;

	  case I_ciscoICsuDsuT1DteLineCode:
	    {
		uchar                    line_coding;

		if (csu_dsu_register_RW(p_csu_dsu, DSU_DATA_TRANSMIT_OPTIONS_REG,
				DSU_DATA_TRANSMIT_OPTIONS_LEN, &line_coding,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    dsx1ConfigEntryData.ciscoICsuDsuT1DteLineCode =
			(line_coding == DSU_TRANSMIT_INVERTED_SCRAMBLED) ?
			D_ciscoICsuDsuT1DteLineCode_inverted :
			D_ciscoICsuDsuT1DteLineCode_normal;
		} else {
		    return NULL;
		}
		break;
	    }
	  case I_ciscoICsuDsuT1SupportRemoteAlarmIndication:
	    {
		uchar                    yellow_enable;

		if (csu_dsu_register_RW(p_csu_dsu,
					T1_YELLOW_ALARM_OPTION_REG,
					T1_YELLOW_ALARM_OPTION_LEN,
					&yellow_enable,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    dsx1ConfigEntryData.ciscoICsuDsuT1SupportRemoteAlarmIndication =
			(yellow_enable == T1_YELLOW_ALARM_ENABLED) ?
			D_ciscoICsuDsuT1SupportRemoteAlarmIndication_true :
			D_ciscoICsuDsuT1SupportRemoteAlarmIndication_false;
		} else {
		    return NULL;
		}
		break;
	    }
	  case I_ciscoICsuDsuT1FullBandwidthRemoteLoopcode:
	    {
		switch (p_csu_dsu -> t1_full_bw_loopcode) {
		  case CSU_DSU_RMT_LPBACK_TYPE_DEF:
		    dsx1ConfigEntryData.ciscoICsuDsuT1FullBandwidthRemoteLoopcode = D_ciscoICsuDsuT1FullBandwidthRemoteLoopcode_standard;
		    break;
		  case CSU_DSU_RMT_LPBACK_TYPE_ALT:
		    dsx1ConfigEntryData.ciscoICsuDsuT1FullBandwidthRemoteLoopcode = D_ciscoICsuDsuT1FullBandwidthRemoteLoopcode_alternate;
		    break;
		  case CSU_DSU_RMT_LPBACK_TYPE_DISABLE:
		    dsx1ConfigEntryData.ciscoICsuDsuT1FullBandwidthRemoteLoopcode = D_ciscoICsuDsuT1FullBandwidthRemoteLoopcode_disabled;
		    break;
		  default:
		    return NULL;
		}
		break;
	    }
	  case I_ciscoICsuDsuT1PayloadRemoteLoopcode:
	    {
		switch (p_csu_dsu -> t1_fractional_bw_loopcode) {
		  case CSU_DSU_RMT_LPBACK_TYPE_DEF:
		    dsx1ConfigEntryData.ciscoICsuDsuT1PayloadRemoteLoopcode = D_ciscoICsuDsuT1PayloadRemoteLoopcode_standard;
		    break;
		  case CSU_DSU_RMT_LPBACK_TYPE_ALT:
		    dsx1ConfigEntryData.ciscoICsuDsuT1PayloadRemoteLoopcode = D_ciscoICsuDsuT1PayloadRemoteLoopcode_alternate;
		    break;
		  case CSU_DSU_RMT_LPBACK_TYPE_V54:
		    dsx1ConfigEntryData.ciscoICsuDsuT1PayloadRemoteLoopcode = D_ciscoICsuDsuT1PayloadRemoteLoopcode_v54;
		    break;
		  case CSU_DSU_RMT_LPBACK_TYPE_DISABLE:
		    dsx1ConfigEntryData.ciscoICsuDsuT1PayloadRemoteLoopcode = D_ciscoICsuDsuT1PayloadRemoteLoopcode_disabled;
		    break;
		  default:
		    return NULL;
		}
		break;
	    }

	  default:
	    return NULL;
	}

	return &dsx1ConfigEntryData;
    } else {
	return NULL;
    }
}



#ifdef SETS
int
k_dsx1ConfigEntry_test(object, value, dp, contextInfo)
    ObjectInfo              *object;
    ObjectSyntax            *value;
    doList_t                *dp;
    ContextInfo             *contextInfo;
{

    return NO_ERROR;
}

int
k_dsx1ConfigEntry_ready(object, value, doHead, dp)
    ObjectInfo              *object;
    ObjectSyntax            *value;
    doList_t                *doHead;
    doList_t                *dp;
{

    dp -> state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_dsx1ConfigEntry_set_defaults(dp)
    doList_t                *dp;
{
    /* dsx1ConfigEntry_t *data = (dsx1ConfigEntry_t *) (dp->data); */

    return NO_ERROR;
}

int
k_dsx1ConfigEntry_set(data, contextInfo, function)
    dsx1ConfigEntry_t       *data;
    ContextInfo             *contextInfo;
    int                      function;
{

    return GEN_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
#ifdef SR_dsx1ConfigEntry_UNDO
/* add #define SR_dsx1ConfigEntry_UNDO in icsudsumib.h to
 * include the undo routine for the dsx1ConfigEntry family.
 */
int
dsx1ConfigEntry_undo(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_dsx1ConfigEntry_UNDO */
#endif				/* SR_SNMPv2 */



ciscoICsuDsuStaticConfigEntry_t *
k_ciscoICsuDsuStaticConfigEntry_get(int serialNum,
				     ContextInfo * contextInfo,
				     int nominator,
				     int searchType,
				     long ifIndex)
{
    static uchar             ostring_char_buf[MAX_CSU_DSU_REVISION_LENGTH];
    static OctetString       ostring_revision =
    {ostring_char_buf, 0};
    static ciscoICsuDsuStaticConfigEntry_t ciscoICsuDsuStaticConfigEntryData =
    {0, &ostring_revision, &ostring_revision, &ostring_revision, 0, {0}};

    csu_dsu_instance_t      *p_csu_dsu = NULL;
    ushort                  protocol_rev;

    ifIndex = find_index_and_csu_dsu_instance(serialNum,
					      searchType,
					      ifIndex,
					      &p_csu_dsu,
		 (CSU_DSU_FT1 | CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));
    if (p_csu_dsu) {

	/*
	 * Found instance.  Read data directly into static character buffer.
	 * Note that strings returned from CSU are '\0' terminated whereas we
	 * are returning octet strings which do not include null character.
	 */
	switch (nominator) {
	  case I_ciscoICsuDsuType:
	    if (p_csu_dsu -> module_type & CSU_DSU_FT1) {
		ciscoICsuDsuStaticConfigEntryData.ciscoICsuDsuType = D_ciscoICsuDsuType_fractionalT1;
	    } else if (p_csu_dsu -> module_type & CSU_DSU_SW56_2_WIRE) {
		ciscoICsuDsuStaticConfigEntryData.ciscoICsuDsuType = D_ciscoICsuDsuType_twoWireSwitched56k;
	    } else {

		/*
		 * Must be 4 wire
		 */
		ciscoICsuDsuStaticConfigEntryData.ciscoICsuDsuType = D_ciscoICsuDsuType_fourWireSwitched56k;
	    }
	    break;

	  case I_ciscoICsuDsuHwRevision:
	    if (csu_dsu_register_RW(p_csu_dsu,
				    DSU_HW_REVISION_REG,
				    DSU_HW_REVISION_LEN,
				    ostring_char_buf,
				    READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		ostring_revision.length = DSU_HW_REVISION_LEN - 1;
	    } else {
		return NULL;
	    }
	    break;

	  case I_ciscoICsuDsuSwRevision:
	    if (csu_dsu_register_RW(p_csu_dsu,
				    DSU_SW_REVISION_REG,
				    DSU_SW_REVISION_LEN,
				    ostring_char_buf,
				    READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		ostring_revision.length = DSU_SW_REVISION_LEN - 1;
	    } else {
		return NULL;
	    }
	    break;

	  case I_ciscoICsuDsuProtocolRevision:
	    if (csu_dsu_register_RW(p_csu_dsu,
				    DSU_INT_PROTOCOL_REV_REG,
				    DSU_INT_PROTOCOL_REV_LEN,
				    &protocol_rev,
				    READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		sprintf( ostring_char_buf, "%d.%d", (protocol_rev >> 8),
						    (protocol_rev & 0xff) );
		ostring_revision.length = DSU_INT_PROTOCOL_REV_LEN + 1;
	    } else {
		return NULL;
	    }
	    break;

	  default:
	    return NULL;
	}
	ciscoICsuDsuStaticConfigEntryData.ifIndex = ifIndex;

	return &ciscoICsuDsuStaticConfigEntryData;
    }
    return NULL;
}


boolean
get_ciscoICsuDsuTestReportEntry ( csu_dsu_instance_t            *p_csu_dsu,
				  ciscoICsuDsuTestReportEntry_t *p_test_report,
				  int 				nominator )
{
    static uchar             ostring_pattern_buf[MAX_USER_PATTERN_LENGTH];
    static OctetString       ostring_user_pattern = {ostring_pattern_buf, MAX_USER_PATTERN_LENGTH};

    switch (nominator) {
      case I_ciscoICsuDsuLastSelfTestResult:
	{
	    char                     selftest_result;

	    /*
	     * MIB defined with bitmap returned by hardware.  Both FT1
	     * and SW56 use same bitmap and register location.
	     */
	    if (csu_dsu_register_RW(p_csu_dsu,
				    DSU_SELF_TEST_RESULTS_REG,
				    DSU_SELF_TEST_RESULTS_LEN,
				    &selftest_result,
				    READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		p_test_report->ciscoICsuDsuLastSelfTestResult = selftest_result;
	    } else {
		return FALSE;
	    }
	    break;
	}
      case I_ciscoICsuDsuTimeOfLastSelfTest:
	p_test_report->ciscoICsuDsuTimeOfLastSelfTest = SNMP_ABSOLUTE_TIME(p_csu_dsu -> selftest_start_time);
	break;
    
      case I_ciscoICsuDsuNumResets:
	p_test_report->ciscoICsuDsuNumResets = p_csu_dsu -> num_module_resets;
	break;

      case I_ciscoICsuDsuTimeOfLastReset:
	p_test_report->ciscoICsuDsuTimeOfLastReset = SNMP_ABSOLUTE_TIME(p_csu_dsu -> time_module_reset);
	break;

      case I_ciscoICsuDsuLoopbackStatus:
        if (p_csu_dsu -> current_loopback_command != CSU_DSU_LPBACK_NONE) {
            p_test_report->ciscoICsuDsuLoopbackStatus = D_ciscoICsuDsuLoopbackStatus_inProgress;
        }
        else if (p_csu_dsu -> last_loopback_command != CSU_DSU_LPBACK_NONE) {
            if (p_csu_dsu -> last_loopback_command == CSU_DSU_LOOPBACK_REMOTE) {
                if (!p_csu_dsu->dsu_last_remote_loopback_passed) {
                    p_test_report->ciscoICsuDsuLoopbackStatus = D_ciscoICsuDsuLoopbackStatus_failed;
                }
                else {
                    if (p_csu_dsu->dsu_last_loopback_pattern != CSU_DSU_LPBACK_PAT_NOSEL) {
                        p_test_report->ciscoICsuDsuLoopbackStatus =
                            p_csu_dsu->dsu_last_loopback_synced_pattern ?
                            D_ciscoICsuDsuLoopbackStatus_completed :
                            D_ciscoICsuDsuLoopbackStatus_noSyncWithTestPattern;
                    }
                    else {
                        p_test_report->ciscoICsuDsuLoopbackStatus = D_ciscoICsuDsuLoopbackStatus_completed;
                   }
                }
            }
            else {
                p_test_report->ciscoICsuDsuLoopbackStatus = D_ciscoICsuDsuLoopbackStatus_completed;
            }
        }
        else {
            p_test_report->ciscoICsuDsuLoopbackStatus = D_ciscoICsuDsuLoopbackStatus_neverPerformed;
        }
        break;

      case I_ciscoICsuDsuLoopbackNumErrors:
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE) &&
	    (p_csu_dsu->dsu_last_remote_loopback_passed == TRUE) &&
	    (p_csu_dsu->dsu_last_loopback_pattern != CSU_DSU_LPBACK_PAT_NOSEL) &&
	    (p_csu_dsu->dsu_last_loopback_synced_pattern)) {
	    p_test_report->ciscoICsuDsuLoopbackNumErrors = p_csu_dsu->dsu_last_loopback_bit_errors;
	} else {
	    return (FALSE);
	}
	break;

      case I_ciscoICsuDsuLoopbackDuration:
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command != CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->dsu_last_remote_loopback_passed == TRUE)) {
	    p_test_report->ciscoICsuDsuLoopbackDuration =
		SNMP_DELTA_TIME(CLOCK_DIFF_UNSIGNED(p_csu_dsu->last_loopback_start_time, 
						    p_csu_dsu->last_loopback_end_time));
            /*
             * For remote loopbacks with test-pattern, it's possible that
             * the module went in and out of sync. If that;s the case, return
             * the in-sync duration (T1 modules only)
             */
            if (p_csu_dsu->module_type == CSU_DSU_FT1)
                if ((p_csu_dsu->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE) &&
                    (p_csu_dsu->dsu_last_loopback_pattern != CSU_DSU_LPBACK_PAT_NOSEL) &&
                    (p_csu_dsu->dsu_last_loopback_synced_pattern)) {
                    p_test_report->ciscoICsuDsuLoopbackDuration =
                        SNMP_DELTA_TIME(CLOCK_DIFF_UNSIGNED(p_csu_dsu->last_loopback_pattern_sync_duration,
							    p_csu_dsu->last_loopback_end_time));
                }
	} 
	else
	    return (FALSE);

	break;

      case I_ciscoICsuDsuLoopbackPoint:
	if (p_csu_dsu -> current_loopback_command == CSU_DSU_LPBACK_NONE) {
	    switch (p_csu_dsu -> last_loopback_command) {
	      case CSU_DSU_LOOPBACK_DTE:
		/*
		 * dtePayload is not supported by boa (2524)
		 */
		p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_dteFull;
		break;

	      case CSU_DSU_LOOPBACK_LINE:
		if (p_csu_dsu -> dsu_last_loopback_type == CSU_DSU_LPBACK_TYPE_FR) {
		    p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_linePayload;
		} else {
		    p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_lineFull;
		}
		break;

	      case CSU_DSU_LOOPBACK_REMOTE:
		if (p_csu_dsu -> dsu_last_loopback_type == CSU_DSU_LPBACK_TYPE_SJ) {
		    p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_remoteSmartJack;
		} else if (p_csu_dsu -> dsu_last_loopback_type == CSU_DSU_LPBACK_TYPE_FR) {
		    p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_remotePayload;
		} else {
		    p_test_report->ciscoICsuDsuLoopbackPoint = D_ciscoICsuDsuLoopbackPoint_remoteFull;
		}
		break;

	      default:
		return FALSE;
	        }
	    } else {
		return FALSE;
	    }
	    break;

      case I_ciscoICsuDsuLoopbackPattern:
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE)) {
	    switch (p_csu_dsu -> dsu_last_loopback_pattern) {
	      case CSU_DSU_LPBACK_PAT_NOSEL:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_noPattern;
		break;
	      case CSU_DSU_LPBACK_PAT_QRW:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternQRW;
		break;
	      case CSU_DSU_LPBACK_PAT_1IN8:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern1In8;
		break;
	      case CSU_DSU_LPBACK_PAT_3in24:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern3In24;
		break;
	      case CSU_DSU_LPBACK_PAT_1IN2:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern1In2;
		break;
	      case CSU_DSU_LPBACK_PAT_1IN1:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern1In1;
		break;
	      case CSU_DSU_LPBACK_PAT_0IN1:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern0In1;
		break;
	      case CSU_DSU_LPBACK_PAT_1IN3:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern1In3;
		break;
	      case CSU_DSU_LPBACK_PAT_1IN5:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern1In5;
		break;
	      case CSU_DSU_LPBACK_PAT_UP:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternUser;
		break;
	      case CSU_DSU_LPBACK_PAT_2047:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern2047;
		break;
	      case CSU_DSU_LPBACK_PAT_511:
		p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_pattern511;
		break;
	      case CSU_DSU_LPBACK_PAT_STR_PAT:
		switch (p_csu_dsu->dsu_last_loopback_user_pattern ) {
		  case 1:
		    p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternStressDDS1;
		    break;
		  case 2:
		    p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternStressDDS2;
		    break;
		  case 3:
		    p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternStressDDS3;
		    break;
		  case 4:
		    p_test_report->ciscoICsuDsuLoopbackPattern = D_ciscoICsuDsuLoopbackPattern_patternStressDDS4;
		    break;
		  default :
		    return FALSE;
		}
		break;
	      default:
		return FALSE;
	    }
	} else {
	    return FALSE;
	}
        break;

      case I_ciscoICsuDsuUserDefinedPattern:
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command    == CSU_DSU_LOOPBACK_REMOTE) &&
	    (p_csu_dsu->dsu_last_loopback_pattern == CSU_DSU_LPBACK_PAT_UP)) {

	    /*
	     * Convert binary representation of userloopback pattern into a
	     *   string.  Binary is 32 bits long, with MSByte containing user
	     *   pattern bit length, and remaining 24 bits containing the
	     *   pattern.
	     * Note that the bitpattern is stored in reverse order of how user 
	     *   entered it (least significant bit contains leftmost char of pattern).
	     */
	    int   num_valid_bits;     /* length user pattern, (upto size of string buffer) */
	    ulong user_pattern;       /* the user pattern */
	    int   bit_position;       /* current bit being converted to character */
	    int   i_str_buf;          /* current index into string buffer */

	    user_pattern = p_csu_dsu -> dsu_last_loopback_user_pattern;
	    num_valid_bits = (user_pattern >> CSU_DSU_MAX_USR_PAT_BITS) & 0xff;
	    num_valid_bits = (num_valid_bits > sizeof(ostring_pattern_buf)) ? 
			     sizeof(ostring_pattern_buf) : num_valid_bits;
	    ostring_user_pattern.length = num_valid_bits;
	    p_test_report->ciscoICsuDsuUserDefinedPattern = &ostring_user_pattern;

	    for( i_str_buf = 0, bit_position = 1;
		 i_str_buf < num_valid_bits;
		 i_str_buf++, bit_position <<= 1)
	    {
		ostring_pattern_buf[ i_str_buf ] = (user_pattern & bit_position) ? '1' : '0';
	    }
	    break;
	} else {
	    return FALSE;
	}

      case I_ciscoICsuDsuLoopbackCode:

	/*
	 * This is kind of messy.  A different code can be used for full
	 * or fractional loopback, but we have only one object.  Try to
	 * return the right value.  If last performed a remote fractional
	 * loopback then return that code, otherwise return one used for
	 * full loopbacks.   Note that our Sw56k modules only supports v54.
	 */
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE)) {

	    if ( p_csu_dsu->module_type & (CSU_DSU_SW56_2_WIRE | 
					   CSU_DSU_SW56_4_WIRE) ) {
		p_test_report->ciscoICsuDsuLoopbackCode = D_ciscoICsuDsuLoopbackCode_v54;
	    } else {

		uchar loopcode;

    	        if (p_csu_dsu -> dsu_last_loopback_type == CSU_DSU_LPBACK_TYPE_FR) {
		    loopcode = p_csu_dsu->t1_fractional_bw_loopcode;
	        } else {
		    loopcode = p_csu_dsu->t1_full_bw_loopcode;
	        }

	        if (loopcode == CSU_DSU_RMT_LPBACK_TYPE_ALT) {
		    p_test_report->ciscoICsuDsuLoopbackCode = D_ciscoICsuDsuLoopbackCode_alternate;
	        } else if (loopcode == CSU_DSU_RMT_LPBACK_TYPE_V54) {
		    p_test_report->ciscoICsuDsuLoopbackCode = D_ciscoICsuDsuLoopbackCode_v54;
	        } else {
		    p_test_report->ciscoICsuDsuLoopbackCode = D_ciscoICsuDsuLoopbackCode_standard;
	        }

	    }
            break;

	} else {
	    return FALSE;
	}

      case I_ciscoICsuDsuEndTimeOfLastLoopback:
	if ((p_csu_dsu->current_loopback_command == CSU_DSU_LPBACK_NONE) &&
	    (p_csu_dsu->last_loopback_command != CSU_DSU_LPBACK_NONE)) {
	    p_test_report->ciscoICsuDsuEndTimeOfLastLoopback = SNMP_ABSOLUTE_TIME(p_csu_dsu -> last_loopback_end_time);
	    break;
	} else {
	    return FALSE;
        }


      default:
	return FALSE;
    }
    return TRUE; 
}


ciscoICsuDsuTestReportEntry_t *
k_ciscoICsuDsuTestReportEntry_get (int serialNum,
				   ContextInfo * contextInfo,
				   int nominator,
				   int searchType,
				   long ifIndex)
{
    static ciscoICsuDsuTestReportEntry_t ciscoICsuDsuTestReportEntryData;

    csu_dsu_instance_t      *p_csu_dsu = NULL;
    boolean		    found_instance_data;

    for( ;; ) {

	ifIndex = find_index_and_csu_dsu_instance(serialNum,
					          searchType,
					          ifIndex,
					          &p_csu_dsu,
		             (CSU_DSU_FT1 | CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));
	if (p_csu_dsu) {

	    found_instance_data = get_ciscoICsuDsuTestReportEntry ( p_csu_dsu,
								    &ciscoICsuDsuTestReportEntryData,
								    nominator );
	    if( found_instance_data ) {
		ciscoICsuDsuTestReportEntryData.ifIndex = ifIndex;
		return &ciscoICsuDsuTestReportEntryData;
	    } else if ( searchType == NEXT ) {
		ifIndex++;
		continue;                        
	    } else {
		return NULL;
	    }

	} else {
	    return NULL;
	}
    }
}


static ulong
ciscoICsuDsu_map_T1LoopStatus( ulong hw_loop_status )
{
    ulong mib_loop_status = 0;

    if (hw_loop_status & B_FT1_ALARM_STATUS_NET_CARRIER_LOSS)
	mib_loop_status |= B_ciscoICsuDsuLoopStatus_lossOfSignal;
    if (hw_loop_status & B_FT1_ALARM_STATUS_NET_SYNC_LOSS)
	mib_loop_status |= B_ciscoICsuDsuLoopStatus_lossOfFrame;
    if (hw_loop_status & B_FT1_ALARM_STATUS_NET_AIS_RECEIVED)
	mib_loop_status |= B_ciscoICsuDsuLoopStatus_detectedAIS;
    if (hw_loop_status & B_FT1_ALARM_STATUS_NET_YELLOW_RECEIVED)
	mib_loop_status |= B_ciscoICsuDsuLoopStatus_detectedRAI;
    if (hw_loop_status & B_FT1_ALARM_STATUS_NET_PAYLOAD_LOOPBACK)
	mib_loop_status |= B_ciscoICsuDsuLoopStatus_placedInLoopback;

    return mib_loop_status;
}


ciscoICsuDsuT1StatusEntry_t *
k_ciscoICsuDsuT1StatusEntry_get (int serialNum,
				 ContextInfo * contextInfo,
				 int nominator,
				 int searchType,
				 long ifIndex)
{
    static ciscoICsuDsuT1StatusEntry_t ciscoICsuDsuT1StatusEntryData;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    ifIndex = find_index_and_csu_dsu_instance(serialNum,
					      searchType,
					      ifIndex,
					      &p_csu_dsu,
					      CSU_DSU_FT1);
    if (p_csu_dsu) {
	switch (nominator) {
	  case I_ciscoICsuDsuT1LoopStatus:
	    ciscoICsuDsuT1StatusEntryData.ciscoICsuDsuT1LoopStatus = 
	        ciscoICsuDsu_map_T1LoopStatus( p_csu_dsu->dsu_network_alarm_status );
	    break;

	  case I_ciscoICsuDsuT1LossOfSignals:
	    ciscoICsuDsuT1StatusEntryData.ciscoICsuDsuT1LossOfSignals =
		p_csu_dsu -> alarm_counts[T1_CARRIER_LOSS].count;
	    break;

	  case I_ciscoICsuDsuT1LossOfFrames:
	    ciscoICsuDsuT1StatusEntryData.ciscoICsuDsuT1LossOfFrames =
		p_csu_dsu -> alarm_counts[T1_FRAME_SYNC_LOSS].count;
	    break;

	  case I_ciscoICsuDsuT1RemoteAlarmIndications:
	    ciscoICsuDsuT1StatusEntryData.ciscoICsuDsuT1RemoteAlarmIndications =
		p_csu_dsu -> alarm_counts[T1_RXING_RAI].count;
	    break;

	  case I_ciscoICsuDsuT1AlarmIndicationSignals:
	    ciscoICsuDsuT1StatusEntryData.ciscoICsuDsuT1AlarmIndicationSignals =
		p_csu_dsu -> alarm_counts[T1_RXING_AIS].count;
	    break;

	  default:
	    return NULL;
	}

	ciscoICsuDsuT1StatusEntryData.ifIndex = ifIndex;

	return &ciscoICsuDsuT1StatusEntryData;
    }
    return NULL;
}



ciscoICsuDsuSw56kConfigEntry_t *
k_ciscoICsuDsuSw56kConfigEntry_get (int serialNum,
				    ContextInfo * contextInfo,
				    int nominator,
				    int searchType,
				    long ifIndex)
{
    static ciscoICsuDsuSw56kConfigEntry_t ciscoICsuDsuSw56kConfigEntryData;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    ifIndex = find_index_and_csu_dsu_instance(serialNum,
					      searchType,
					      ifIndex,
					      &p_csu_dsu,
			       (CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));

    if (p_csu_dsu) {
	switch (nominator) {
	  case I_ciscoICsuDsuSw56kNetworkType:
	    if (p_csu_dsu -> sw56_network_type == CSU_DSU_NWKTYPE_DDS) {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kNetworkType = D_ciscoICsuDsuSw56kNetworkType_dds;
	    } else if (p_csu_dsu -> sw56_switched_type == CSU_DSU_SW_CARRIER_ATT ) {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kNetworkType = D_ciscoICsuDsuSw56kNetworkType_att;
	    } else if (p_csu_dsu -> sw56_switched_type == CSU_DSU_SW_CARRIER_SPRINT ) {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kNetworkType = D_ciscoICsuDsuSw56kNetworkType_sprint;
	    } else {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kNetworkType = D_ciscoICsuDsuSw56kNetworkType_otherCarrier;
	    }
	    break;

	  case I_ciscoICsuDsuSw56kClockSource:
	    ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kClockSource =
		(p_csu_dsu -> dsu_clocksource == CSU_DSU_CLK_SRC_LINE) ?
		D_ciscoICsuDsuSw56kClockSource_line :
		D_ciscoICsuDsuSw56kClockSource_internal;
	    break;

	  case I_ciscoICsuDsuSw56kLoopRate:
	    if (p_csu_dsu -> module_type == CSU_DSU_SW56_2_WIRE) {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps56k;
	    } else {
		uchar                    line_rate;

		if (csu_dsu_register_RW(p_csu_dsu,
					SW56_DDS_LINE_RATE_REG,
					SW56_DDS_LINE_RATE_LEN,
					(char *) &line_rate,
				   READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		    switch (line_rate) {
		      case SW56_LINE_RATE_2400:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps2400;
			break;
		      case SW56_LINE_RATE_4800:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps4800;
			break;
		      case SW56_LINE_RATE_9600:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps9600;
			break;
		      case SW56_LINE_RATE_19200:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps19k;
			break;
		      case SW56_LINE_RATE_38400:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps38k;
			break;
		      case SW56_LINE_RATE_64K:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps64k;
			break;

		      case SW56_LINE_RATE_56K:
		      default:
			ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kLoopRate = D_ciscoICsuDsuSw56kLoopRate_bps56k;
			break;
		    }
		} else {
		    return NULL;
		}
	    }
	    break;

	  case I_ciscoICsuDsuSw56kScramblerEnabled:
	    if ((p_csu_dsu -> dsu_data_scrambler_mode == CSU_DSU_DCODE_NOSEL) ||
	    (p_csu_dsu -> dsu_data_scrambler_mode == CSU_DSU_DCODE_NORMAL)) {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kScramblerEnabled = D_ciscoICsuDsuSw56kScramblerEnabled_false;
	    } else {
		ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kScramblerEnabled = D_ciscoICsuDsuSw56kScramblerEnabled_true;
	    }
	    break;

	  case I_ciscoICsuDsuSw56kRemoteLoopbackEnabled:
	    ciscoICsuDsuSw56kConfigEntryData.ciscoICsuDsuSw56kRemoteLoopbackEnabled =
		(p_csu_dsu -> sw56_remote_loopback_enable) ?
		D_ciscoICsuDsuSw56kRemoteLoopbackEnabled_true :
		D_ciscoICsuDsuSw56kRemoteLoopbackEnabled_false;
	    break;

	  default:
	    return NULL;
	}

	ciscoICsuDsuSw56kConfigEntryData.ifIndex = ifIndex;

	return &ciscoICsuDsuSw56kConfigEntryData;
    }
    return NULL;
}

ciscoICsuDsuSw56kLineStatusEntry_t *
k_ciscoICsuDsuSw56kLineStatusEntry_get (int serialNum,
				        ContextInfo * contextInfo,
				        int nominator,
				        int searchType,
				        long ifIndex)
{
    static ciscoICsuDsuSw56kLineStatusEntry_t ciscoICsuDsuSw56kLineStatusEntryData;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    ifIndex = find_index_and_csu_dsu_instance(serialNum,
					      searchType,
					      ifIndex,
					      &p_csu_dsu,
			       (CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));
    if (p_csu_dsu) {
	switch (nominator) {
	  case I_ciscoICsuDsuSw56kDialingStatus:
	    {
		uchar                    dial_status;

		if (p_csu_dsu->sw56_network_type != CSU_DSU_NWKTYPE_DDS) {
		    if (csu_dsu_register_RW(p_csu_dsu,
					    SW56_DIAL_STATUS_REG,
					    SW56_DIAL_STATUS_LEN,
					    (char *) &dial_status,
				            READ_REGISTER) == DSU_REQUEST_COMPLETE) {
		        if (dial_status & B_SW56_DIAL_STATUS_DIALLING) {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_dialing;
		        } else if (dial_status & B_SW56_DIAL_STATUS_CONNECTION_ACTIVE) {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_online;
		        } else if (dial_status & B_SW56_DIAL_STATUS_SWITCH_BUSY) {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_noWinkFromSwitch;
		        } else if (dial_status & B_SW56_DIAL_STATUS_REMOTE_END_BUSY) {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_numberBusy;
		        } else if (dial_status & B_SW56_DIAL_STATUS_NO_ANSWER) {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_noAnswer;
		        } else {
			    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kDialingStatus = D_ciscoICsuDsuSw56kDialingStatus_idle;
		        }

			break;
		    }
		}
		return NULL;
	    }

	  case I_ciscoICsuDsuSw56kLoopStatus:

	    /*
	     * MIB defined to use CSU/DSU hardware bit definitions.
	     */
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kLoopStatus =
		p_csu_dsu -> dsu_network_alarm_status;
	    break;

	  case I_ciscoICsuDsuSw56kReceivedOosOofs:
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kReceivedOosOofs =
		p_csu_dsu -> alarm_counts[SW56_RXING_OOS_OOF].count;
	    break;

	  case I_ciscoICsuDsuSw56kLostSealingCurrents:
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kLostSealingCurrents =
		p_csu_dsu -> alarm_counts[SW56_CURRENT_LOSS].count;
	    break;

	  case I_ciscoICsuDsuSw56kLostReceiveSignals:
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kLostReceiveSignals =
		p_csu_dsu -> alarm_counts[SW56_CARRIER_LOSS].count;
	    break;

	  case I_ciscoICsuDsuSw56kLostFrameSyncs:
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kLostFrameSyncs =
		p_csu_dsu -> alarm_counts[SW56_FRAME_SYNC_LOSS].count;
	    break;

	  case I_ciscoICsuDsuSw56kLoopRateSearches:
	    ciscoICsuDsuSw56kLineStatusEntryData.ciscoICsuDsuSw56kLoopRateSearches =
		p_csu_dsu -> alarm_counts[SW56_RATE_ADAPTING].count;
	    break;

	  default:
	    return NULL;
	}

	ciscoICsuDsuSw56kLineStatusEntryData.ifIndex = ifIndex;
	return &ciscoICsuDsuSw56kLineStatusEntryData;
    }
    return NULL;
}


/****************************************************************************/
/*                           T R A P S                                      */
/****************************************************************************/

/*
 * Objects controlling whether particular trap is generated or surpressed.
 */
static ciscoICsuDsuMIBNotificationEnables_t ciscoICsuDsuMIBNotificationEnablesData =
{D_ciscoICsuDsuEnableT1LoopStatusNotification_false, D_ciscoICsuDsuEnableSw56kLoopStatusNotification_false, {0}};

/*
 * snmpv1 Enterprise trap
 */
static const OID         icsudsu_v1_enterpriseOID =
{
    LNciscoICsuDsuMIBNotificationPrefix,
    (ulong *) IDciscoICsuDsuMIBNotificationPrefix
};

/*
 * snmpv2 Traps
 */
static const OID         t1_loop_status_trapOID =
{
    LNciscoICsuDsuEnableT1LoopStatusNotification,
    (ulong *) IDciscoICsuDsuEnableT1LoopStatusNotification
};
static const OID         sw56_loop_status_trapOID =
{
    LNciscoICsuDsuEnableSw56kLoopStatusNotification,
    (ulong *) IDciscoICsuDsuEnableSw56kLoopStatusNotification
};

/*
 * Varbinds used in Traps
 */
static const OID         t1_loop_status_oid =
{
    LNciscoICsuDsuT1LoopStatus,
    (ulong *) IDciscoICsuDsuT1LoopStatus
};
static const OID         sw56_loop_status_oid =
{
    LNciscoICsuDsuSw56kLoopStatus,
    (ulong *) IDciscoICsuDsuSw56kLoopStatus
};

void
send_snmp_t1_loop_status_trap (int new_status,
			       ulong ifIndex)
{
    OID                      instance_oid;
    char                     ent_v2_trap[80];
    VarBind                 *p_vb;
    ulong                    mib_loop_status;

    if (ciscoICsuDsuMIBNotificationEnablesData.ciscoICsuDsuEnableT1LoopStatusNotification == D_ciscoICsuDsuEnableT1LoopStatusNotification_true) {

	mib_loop_status = ciscoICsuDsu_map_T1LoopStatus( new_status );

	/*
	 * Convert dot notation in readable notation, for snmpv2 trap
	 * parameter.
	 */
	if (MakeDotFromOID((OID *) & t1_loop_status_trapOID, ent_v2_trap) == -1) {
	    return;
	}
	instance_oid.oid_ptr = &ifIndex;
	instance_oid.length = 1;

	p_vb = MakeVarBindWithValue((OID *) & t1_loop_status_oid,
				    &instance_oid,
				    INTEGER_TYPE,
				    &mib_loop_status);
	if (p_vb) {
	    p_vb -> next_var = NULL;

	    /*
	     * Send the trap.  Routine will free varbind allocated above.
	     */
	    do_trap(ENTERPRISE_TRAP, T1_LOOP_STATUS_V1_SPEC_TRAP, p_vb, (OID *) & icsudsu_v1_enterpriseOID, ent_v2_trap);
	}
    }
}

void
send_snmp_sw56_loop_status_trap (int new_status,
				 ulong ifIndex)
{
    OID                      instance_oid;
    char                     ent_v2_trap[80];
    VarBind                 *p_vb;

    if (ciscoICsuDsuMIBNotificationEnablesData.ciscoICsuDsuEnableSw56kLoopStatusNotification == D_ciscoICsuDsuEnableSw56kLoopStatusNotification_true) {

	/*
	 * The MIB LoopStatus definition uses the same bit values as returned 
	 * by HW. Therefore use new_status directly.
	 * 
	 * Convert dot notation in readable notation, for snmpv2 trap
	 * parameter.
	 */
	if (MakeDotFromOID((OID *) & t1_loop_status_trapOID, ent_v2_trap) == -1) {
	    return;
	}
	instance_oid.oid_ptr = &ifIndex;
	instance_oid.length = 1;

	p_vb = MakeVarBindWithValue((OID *) & sw56_loop_status_oid,
				    &instance_oid,
				    INTEGER_TYPE,
				    &new_status);
	if (p_vb) {
	    p_vb -> next_var = NULL;

	    /*
	     * Send the trap.  Routine will free varbind allocated above.
	     */
	    do_trap(ENTERPRISE_TRAP, SW56_LOOP_STATUS_V1_SPEC_TRAP, p_vb, (OID *) & icsudsu_v1_enterpriseOID, ent_v2_trap);
	}
    }
}

/*
 * Registered function to send loop status traps.  Simply decides which trap
 *   routine to call.
 */
void
csu_dsu_send_snmp_loop_status_trap (int module_type,
				    int new_status,
				    ulong ifIndex)
{
    if (module_type == CSU_DSU_FT1) {
	send_snmp_t1_loop_status_trap(new_status, ifIndex);
    } else {
	send_snmp_sw56_loop_status_trap(new_status, ifIndex);
    }
}

ciscoICsuDsuMIBNotificationEnables_t *
k_ciscoICsuDsuMIBNotificationEnables_get (int serialNum,
					  ContextInfo * contextInfo,
					  int nominator)
{
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    /*
     * Return particular NotificationEnable if an instance of the same type
     * of CSU/DSU is present in the system.  Otherwise return NULL.
     */

    if (nominator == I_ciscoICsuDsuEnableT1LoopStatusNotification) {
	find_index_and_csu_dsu_instance(-1, NEXT, 1, &p_csu_dsu, CSU_DSU_FT1);
    } else if (nominator == I_ciscoICsuDsuEnableSw56kLoopStatusNotification) {
	find_index_and_csu_dsu_instance(-1, NEXT, 1, &p_csu_dsu, (CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));
    }
    if (p_csu_dsu) {
	return &ciscoICsuDsuMIBNotificationEnablesData;	/* A Module present */
    } else {
	return NULL;		       /* No Modules present */
    }
}

#ifdef SETS
/*
 * Routine extremely similiar to k_ciscoICsuDsuMIBNotificationEnables_get(),
 *   except that calling parameters and return values are different.  Internal
 *   logic practically identical.
 */
int
k_ciscoICsuDsuMIBNotificationEnables_test(ObjectInfo * object,
					   ObjectSyntax * value,
					   doList_t * dp,
					   ContextInfo * contextInfo)
{
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    if (object -> nominator == I_ciscoICsuDsuEnableT1LoopStatusNotification) {
	find_index_and_csu_dsu_instance(-1, NEXT, 1, &p_csu_dsu, CSU_DSU_FT1);
    } else if (object -> nominator == I_ciscoICsuDsuEnableSw56kLoopStatusNotification) {
	find_index_and_csu_dsu_instance(-1, NEXT, 1, &p_csu_dsu, (CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));
    }
    if (p_csu_dsu) {
	return NO_ERROR;
    } else {
	return WRONG_VALUE_ERROR;
    }
}

int
k_ciscoICsuDsuMIBNotificationEnables_ready(ObjectInfo * object,
					    ObjectSyntax * value,
					    doList_t * doHead,
					    doList_t * dp)
{
    dp -> state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ciscoICsuDsuMIBNotificationEnables_set(ciscoICsuDsuMIBNotificationEnables_t * data,
					 ContextInfo * contextInfo,
					 int function)
{
    if (VALID(I_ciscoICsuDsuEnableT1LoopStatusNotification, data -> valid)) {
	ciscoICsuDsuMIBNotificationEnablesData.ciscoICsuDsuEnableT1LoopStatusNotification =
	data -> ciscoICsuDsuEnableT1LoopStatusNotification;
    }
    if (VALID(I_ciscoICsuDsuEnableSw56kLoopStatusNotification, data -> valid)) {
	ciscoICsuDsuMIBNotificationEnablesData.ciscoICsuDsuEnableSw56kLoopStatusNotification =
	    data -> ciscoICsuDsuEnableSw56kLoopStatusNotification;
    }
    return NO_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
#ifdef SR_ciscoICsuDsuMIBNotificationEnables_UNDO
/* add #define SR_ciscoICsuDsuMIBNotificationEnables_UNDO in icsudsumib.h to
 * include the undo routine for the ciscoICsuDsuMIBNotificationEnables family.
 */
int
ciscoICsuDsuMIBNotificationEnables_undo(doHead, doCur, contextInfo)
    doList_t                *doHead;
    doList_t                *doCur;
    ContextInfo             *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_ciscoICsuDsuMIBNotificationEnables_UNDO */
#endif				/* SR_SNMPv2 */



void
init_icsudsumib (subsystype * subsys)
{
    long                    ifIndex;
    csu_dsu_instance_t      *p_csu_dsu = NULL;

    /*
     * Initialize the RFC1406 MIB subsystem.   For efficiency, MIB is loaded
     *   only if a CSU/DSU (T1 or Sw56) interface module is present.
     */

    ifIndex = find_index_and_csu_dsu_instance( -1, NEXT, 1, &p_csu_dsu,
		(CSU_DSU_FT1 | CSU_DSU_SW56_2_WIRE | CSU_DSU_SW56_4_WIRE));

    if ( p_csu_dsu != NULL ) {
	load_mib(icsudsumib_OidList, icsudsumib_OidListNum);
	load_oid(icsudsumib_oid_table);
	reg_add_csu_dsu_send_snmp_loop_status_trap(csu_dsu_send_snmp_loop_status_trap,
				      "csu_dsu_send_snmp_loop_status_trap");
    }
}

/*
 * icsudsumib subsystem header
 */

#define MAJVERSION_icsudsumib 1
#define MINVERSION_icsudsumib 0
#define EDITVERSION_icsudsumib 0

SUBSYS_HEADER(icsudsumib,
	      MAJVERSION_icsudsumib,
	      MINVERSION_icsudsumib,
	      EDITVERSION_icsudsumib,
	      init_icsudsumib,
	      SUBSYS_CLASS_MANAGEMENT,
	      "req: integrated_csu_dsu",
              NULL);
      
