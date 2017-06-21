/* $Id: sr_lapbmib.c,v 3.5.10.4 1996/07/01 18:46:54 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/sr_lapbmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Nicholas Thille (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lapbmib.c,v $
 * Revision 3.5.10.4  1996/07/01  18:46:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/05/21  10:10:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/05/21  06:44:28  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.10.1  1996/03/18  22:49:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  07:57:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/13  02:13:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5  1996/02/23  21:39:03  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:57:03  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:29:26  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  18:06:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:35:13  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:33:17  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:21:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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
#include "sr_lapbmib.h"
#include "sr_lapbmib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

#include "interface_private.h"
#include "lapb.h"

static const ulong nullOIDstr[] = { 0, 0, 0 };
static OID         lapbnullOID = { (sizeof(nullOIDstr) / sizeof(long)),
			       (ulong *)&nullOIDstr };

static OID   lapbProtocolVersionSupported;

void
init_lapbmib (subsystype *subsys)
{
    load_mib(lapbmib_OidList, lapbmib_OidListNum);
    load_oid(lapbmib_oid_table);
    lapbProtocolVersionSupported.oid_ptr = 
	(unsigned long *)&IDlapbProtocolCcittV1984;
    lapbProtocolVersionSupported.length = LNlapbProtocolCcittV1984;
}








/*---------------------------------------------------------------------
 * Retrieve data from the lapbAdmnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *lapbAdmnEntry_get (OID         *incoming,
			    ObjectInfo  *object,
			    int          searchType,
			    ContextInfo *contextInfo,
			    int          serialNum)
{
    int              instLength;
    int              arg;
    void            *dp;
    lapbAdmnEntry_t *data;
    unsigned long    buffer[1];
    OID              inst;
    int              carry;
    long             lapbAdmnIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
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
	    return (NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbAdmnIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lapbAdmnEntry_get(serialNum, contextInfo, arg, searchType, lapbAdmnIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->lapbAdmnIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_lapbAdmnIndex
      case I_lapbAdmnIndex:
	dp = &data->lapbAdmnIndex;
	break;
#endif				       /* I_lapbAdmnIndex */

#ifdef I_lapbAdmnStationType
      case I_lapbAdmnStationType:
	dp = &data->lapbAdmnStationType;
	break;
#endif				       /* I_lapbAdmnStationType */

#ifdef I_lapbAdmnControlField
      case I_lapbAdmnControlField:
	dp = &data->lapbAdmnControlField;
	break;
#endif				       /* I_lapbAdmnControlField */

#ifdef I_lapbAdmnTransmitN1FrameSize
      case I_lapbAdmnTransmitN1FrameSize:
	dp = &data->lapbAdmnTransmitN1FrameSize;
	break;
#endif				       /* I_lapbAdmnTransmitN1FrameSize */

#ifdef I_lapbAdmnReceiveN1FrameSize
      case I_lapbAdmnReceiveN1FrameSize:
	dp = &data->lapbAdmnReceiveN1FrameSize;
	break;
#endif				       /* I_lapbAdmnReceiveN1FrameSize */

#ifdef I_lapbAdmnTransmitKWindowSize
      case I_lapbAdmnTransmitKWindowSize:
	dp = &data->lapbAdmnTransmitKWindowSize;
	break;
#endif				       /* I_lapbAdmnTransmitKWindowSize */

#ifdef I_lapbAdmnReceiveKWindowSize
      case I_lapbAdmnReceiveKWindowSize:
	dp = &data->lapbAdmnReceiveKWindowSize;
	break;
#endif				       /* I_lapbAdmnReceiveKWindowSize */

#ifdef I_lapbAdmnN2RxmitCount
      case I_lapbAdmnN2RxmitCount:
	dp = &data->lapbAdmnN2RxmitCount;
	break;
#endif				       /* I_lapbAdmnN2RxmitCount */

#ifdef I_lapbAdmnT1AckTimer
      case I_lapbAdmnT1AckTimer:
	dp = &data->lapbAdmnT1AckTimer;
	break;
#endif				       /* I_lapbAdmnT1AckTimer */

#ifdef I_lapbAdmnT2AckDelayTimer
      case I_lapbAdmnT2AckDelayTimer:
	dp = &data->lapbAdmnT2AckDelayTimer;
	break;
#endif				       /* I_lapbAdmnT2AckDelayTimer */

#ifdef I_lapbAdmnT3DisconnectTimer
      case I_lapbAdmnT3DisconnectTimer:
	dp = &data->lapbAdmnT3DisconnectTimer;
	break;
#endif				       /* I_lapbAdmnT3DisconnectTimer */

#ifdef I_lapbAdmnT4IdleTimer
      case I_lapbAdmnT4IdleTimer:
	dp = &data->lapbAdmnT4IdleTimer;
	break;
#endif				       /* I_lapbAdmnT4IdleTimer */

#ifdef I_lapbAdmnActionInitiate
      case I_lapbAdmnActionInitiate:
	dp = &data->lapbAdmnActionInitiate;
	break;
#endif				       /* I_lapbAdmnActionInitiate */

#ifdef I_lapbAdmnActionRecvDM
      case I_lapbAdmnActionRecvDM:
	dp = &data->lapbAdmnActionRecvDM;
	break;
#endif				       /* I_lapbAdmnActionRecvDM */

      default:
	return (NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the lapbAdmnEntry data object.
 *---------------------------------------------------------------------*/
static void lapbAdmnEntry_free (lapbAdmnEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lapbAdmnEntry set/undo
 *---------------------------------------------------------------------*/
static int lapbAdmnEntry_cleanup (doList_t *trash)
{
    lapbAdmnEntry_free(trash->data);
#ifdef SR_SNMPv2
    lapbAdmnEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Undo a previous set of the lapbAdmnEntry family.
 *---------------------------------------------------------------------*/
#ifdef SR_SNMPv2
int lapbAdmnEntry_undo (doList_t    *doHead,
			doList_t    *doCur,
			ContextInfo *contextInfo)
{
    return UNDO_FAILED_ERROR;
}

#endif	/* SR_SNMPv2 */

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
int lapbAdmnEntry_test (OID          *incoming,
			ObjectInfo   *object,
			ObjectSyntax *value,
			doList_t     *doHead,
			doList_t     *doCur,
			ContextInfo  *contextInfo)
{
    int              instLength;
    doList_t        *dp;
    VarBind         *vb;
    int              found;
    int              carry;
    lapbAdmnEntry_t *lapbAdmnEntry;
    long             lapbAdmnIndex;

    instLength = incoming->length - object->oid.length;
    vb = NULL;
    carry = 0;

    /*
     * Validate the object instance
     * 
     */

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbAdmnIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    lapbAdmnEntry = k_lapbAdmnEntry_get(-1, contextInfo, object->nominator, EXACT, lapbAdmnIndex);

#ifndef lapbAdmnEntry_READ_CREATE

    if (lapbAdmnEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* lapbAdmnEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == lapbAdmnEntry_set) &&
	    (((lapbAdmnEntry_t *) (dp->data)) != NULL) &&
	(((lapbAdmnEntry_t *) (dp->data))->lapbAdmnIndex == lapbAdmnIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(lapbAdmnEntry_t))) == NULL) {
	    DPRINTF((0, "snmpd: Cannot allocate memory\n"));
	    return (GEN_ERROR);
	}
	memset(dp->data, 0, sizeof(lapbAdmnEntry_t));

	dp->setMethod = lapbAdmnEntry_set;
	dp->cleanupMethod = lapbAdmnEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = lapbAdmnEntry_undo;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_lapbAdmnIndex, ((lapbAdmnEntry_t *) (dp->data))->valid);
	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnIndex = lapbAdmnIndex;

#ifdef lapbAdmnEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (lapbAdmnEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, lapbAdmnEntry, sizeof(lapbAdmnEntry_t));

	    SET_ALL_VALID(((lapbAdmnEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* lapbAdmnEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_lapbAdmnStationType
      case I_lapbAdmnStationType:

	SET_VALID(I_lapbAdmnStationType, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnStationType = value->sl_value;
	break;
#endif				       /* I_lapbAdmnStationType */

#ifdef I_lapbAdmnControlField
      case I_lapbAdmnControlField:

	SET_VALID(I_lapbAdmnControlField, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnControlField = value->sl_value;
	break;
#endif				       /* I_lapbAdmnControlField */

#ifdef I_lapbAdmnTransmitN1FrameSize
      case I_lapbAdmnTransmitN1FrameSize:

	SET_VALID(I_lapbAdmnTransmitN1FrameSize, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnTransmitN1FrameSize = value->sl_value;
	break;
#endif				       /* I_lapbAdmnTransmitN1FrameSize */

#ifdef I_lapbAdmnReceiveN1FrameSize
      case I_lapbAdmnReceiveN1FrameSize:

	SET_VALID(I_lapbAdmnReceiveN1FrameSize, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnReceiveN1FrameSize = value->sl_value;
	break;
#endif				       /* I_lapbAdmnReceiveN1FrameSize */

#ifdef I_lapbAdmnTransmitKWindowSize
      case I_lapbAdmnTransmitKWindowSize:

	SET_VALID(I_lapbAdmnTransmitKWindowSize, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnTransmitKWindowSize = value->sl_value;
	break;
#endif				       /* I_lapbAdmnTransmitKWindowSize */

#ifdef I_lapbAdmnReceiveKWindowSize
      case I_lapbAdmnReceiveKWindowSize:

	SET_VALID(I_lapbAdmnReceiveKWindowSize, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnReceiveKWindowSize = value->sl_value;
	break;
#endif				       /* I_lapbAdmnReceiveKWindowSize */

#ifdef I_lapbAdmnN2RxmitCount
      case I_lapbAdmnN2RxmitCount:

	SET_VALID(I_lapbAdmnN2RxmitCount, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnN2RxmitCount = value->sl_value;
	break;
#endif				       /* I_lapbAdmnN2RxmitCount */

#ifdef I_lapbAdmnT1AckTimer
      case I_lapbAdmnT1AckTimer:

	SET_VALID(I_lapbAdmnT1AckTimer, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnT1AckTimer = value->sl_value;
	break;
#endif				       /* I_lapbAdmnT1AckTimer */

#ifdef I_lapbAdmnT2AckDelayTimer
      case I_lapbAdmnT2AckDelayTimer:

	SET_VALID(I_lapbAdmnT2AckDelayTimer, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnT2AckDelayTimer = value->sl_value;
	break;
#endif				       /* I_lapbAdmnT2AckDelayTimer */

#ifdef I_lapbAdmnT3DisconnectTimer
      case I_lapbAdmnT3DisconnectTimer:

	SET_VALID(I_lapbAdmnT3DisconnectTimer, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnT3DisconnectTimer = value->sl_value;
	break;
#endif				       /* I_lapbAdmnT3DisconnectTimer */

#ifdef I_lapbAdmnT4IdleTimer
      case I_lapbAdmnT4IdleTimer:

	SET_VALID(I_lapbAdmnT4IdleTimer, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnT4IdleTimer = value->sl_value;
	break;
#endif				       /* I_lapbAdmnT4IdleTimer */

#ifdef I_lapbAdmnActionInitiate
      case I_lapbAdmnActionInitiate:

	SET_VALID(I_lapbAdmnActionInitiate, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnActionInitiate = value->sl_value;
	break;
#endif				       /* I_lapbAdmnActionInitiate */

#ifdef I_lapbAdmnActionRecvDM
      case I_lapbAdmnActionRecvDM:

	SET_VALID(I_lapbAdmnActionRecvDM, ((lapbAdmnEntry_t *) (dp->data))->valid);

	((lapbAdmnEntry_t *) (dp->data))->lapbAdmnActionRecvDM = value->sl_value;
	break;
#endif				       /* I_lapbAdmnActionRecvDM */

      default:
	DPRINTF((0, "snmpd: Internal error (invalid nominator in lapbAdmnEntry)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int lapbAdmnEntry_set (doList_t    *doHead,
		       doList_t    *doCur,
		       ContextInfo *contextInfo)
{
    return (k_lapbAdmnEntry_set((lapbAdmnEntry_t *) (doCur->data),
				contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lapbOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *lapbOperEntry_get (OID            *incoming,
			    ObjectInfo     *object,
			    int             searchType,
			    ContextInfo    *contextInfo,
			    int             serialNum)
{
    int              instLength;
    int              arg;
    void            *dp;
    lapbOperEntry_t *data;
    unsigned long    buffer[1];
    OID              inst;
    int              carry;
    long             lapbOperIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
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
	    return (NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbOperIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lapbOperEntry_get(serialNum, contextInfo, arg, searchType, lapbOperIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->lapbOperIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_lapbOperIndex
      case I_lapbOperIndex:
	dp = &data->lapbOperIndex;
	break;
#endif				       /* I_lapbOperIndex */

#ifdef I_lapbOperStationType
      case I_lapbOperStationType:
	dp = &data->lapbOperStationType;
	break;
#endif				       /* I_lapbOperStationType */

#ifdef I_lapbOperControlField
      case I_lapbOperControlField:
	dp = &data->lapbOperControlField;
	break;
#endif				       /* I_lapbOperControlField */

#ifdef I_lapbOperTransmitN1FrameSize
      case I_lapbOperTransmitN1FrameSize:
	dp = &data->lapbOperTransmitN1FrameSize;
	break;
#endif				       /* I_lapbOperTransmitN1FrameSize */

#ifdef I_lapbOperReceiveN1FrameSize
      case I_lapbOperReceiveN1FrameSize:
	dp = &data->lapbOperReceiveN1FrameSize;
	break;
#endif				       /* I_lapbOperReceiveN1FrameSize */

#ifdef I_lapbOperTransmitKWindowSize
      case I_lapbOperTransmitKWindowSize:
	dp = &data->lapbOperTransmitKWindowSize;
	break;
#endif				       /* I_lapbOperTransmitKWindowSize */

#ifdef I_lapbOperReceiveKWindowSize
      case I_lapbOperReceiveKWindowSize:
	dp = &data->lapbOperReceiveKWindowSize;
	break;
#endif				       /* I_lapbOperReceiveKWindowSize */

#ifdef I_lapbOperN2RxmitCount
      case I_lapbOperN2RxmitCount:
	dp = &data->lapbOperN2RxmitCount;
	break;
#endif				       /* I_lapbOperN2RxmitCount */

#ifdef I_lapbOperT1AckTimer
      case I_lapbOperT1AckTimer:
	dp = &data->lapbOperT1AckTimer;
	break;
#endif				       /* I_lapbOperT1AckTimer */

#ifdef I_lapbOperT2AckDelayTimer
      case I_lapbOperT2AckDelayTimer:
	dp = &data->lapbOperT2AckDelayTimer;
	break;
#endif				       /* I_lapbOperT2AckDelayTimer */

#ifdef I_lapbOperT3DisconnectTimer
      case I_lapbOperT3DisconnectTimer:
	dp = &data->lapbOperT3DisconnectTimer;
	break;
#endif				       /* I_lapbOperT3DisconnectTimer */

#ifdef I_lapbOperT4IdleTimer
      case I_lapbOperT4IdleTimer:
	dp = &data->lapbOperT4IdleTimer;
	break;
#endif				       /* I_lapbOperT4IdleTimer */

#ifdef I_lapbOperPortId
      case I_lapbOperPortId:
	dp = MakeOID(data->lapbOperPortId->oid_ptr, data->lapbOperPortId->length);
	break;
#endif				       /* I_lapbOperPortId */

#ifdef I_lapbOperProtocolVersionId
      case I_lapbOperProtocolVersionId:
	dp = MakeOID(data->lapbOperProtocolVersionId->oid_ptr, data->lapbOperProtocolVersionId->length);
	break;
#endif				       /* I_lapbOperProtocolVersionId */

      default:
	return (NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the lapbOperEntry data object.
 *---------------------------------------------------------------------*/
static void lapbOperEntry_free (lapbOperEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lapbOperEntry set/undo
 *---------------------------------------------------------------------*/
static int lapbOperEntry_cleanup (doList_t *trash)
{
    lapbOperEntry_free(trash->data);
#ifdef SR_SNMPv2
    lapbOperEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Undo a previous set of the lapbOperEntry family.
 *---------------------------------------------------------------------*/
#ifdef SR_SNMPv2
int lapbOperEntry_undo (doList_t    *doHead,
			doList_t    *doCur,
			ContextInfo *contextInfo)
{
    return UNDO_FAILED_ERROR;
}

#endif	/* SR_SNMPv2 */

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
int lapbOperEntry_test (OID          *incoming,
			ObjectInfo   *object,
			ObjectSyntax *value,
			doList_t     *doHead,
			doList_t     *doCur,
			ContextInfo  *contextInfo)
{
    int              instLength;
    doList_t        *dp;
    VarBind         *vb;
    int              found;
    int              carry;
    lapbOperEntry_t *lapbOperEntry;
    long             lapbOperIndex;

    instLength = incoming->length - object->oid.length;
    vb = NULL;
    carry = 0;

    /*
     * Validate the object instance
     * 
     */

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbOperIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    lapbOperEntry = k_lapbOperEntry_get(-1, contextInfo, object->nominator, EXACT, lapbOperIndex);

#ifndef lapbOperEntry_READ_CREATE

    if (lapbOperEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* lapbOperEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == lapbOperEntry_set) &&
	    (((lapbOperEntry_t *) (dp->data)) != NULL) &&
	(((lapbOperEntry_t *) (dp->data))->lapbOperIndex == lapbOperIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(lapbOperEntry_t))) == NULL) {
	    DPRINTF((0, "snmpd: Cannot allocate memory\n"));
	    return (GEN_ERROR);
	}
	memset(dp->data, 0, sizeof(lapbOperEntry_t));

	dp->setMethod = lapbOperEntry_set;
	dp->cleanupMethod = lapbOperEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = lapbOperEntry_undo;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_lapbOperIndex, ((lapbOperEntry_t *) (dp->data))->valid);
	((lapbOperEntry_t *) (dp->data))->lapbOperIndex = lapbOperIndex;

#ifdef lapbOperEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (lapbOperEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, lapbOperEntry, sizeof(lapbOperEntry_t));

	    SET_ALL_VALID(((lapbOperEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* lapbOperEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_lapbOperT4IdleTimer
      case I_lapbOperT4IdleTimer:

	SET_VALID(I_lapbOperT4IdleTimer, ((lapbOperEntry_t *) (dp->data))->valid);

	((lapbOperEntry_t *) (dp->data))->lapbOperT4IdleTimer = value->sl_value;
	break;
#endif				       /* I_lapbOperT4IdleTimer */

      default:
	DPRINTF((0, "snmpd: Internal error (invalid nominator in lapbOperEntry)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int lapbOperEntry_set (doList_t    *doHead,
		       doList_t    *doCur,
		       ContextInfo *contextInfo)
{
    return (k_lapbOperEntry_set((lapbOperEntry_t *) (doCur->data),
				contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lapbFlowEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *lapbFlowEntry_get (OID         *incoming,
			    ObjectInfo  *object,
			    int          searchType,
			    ContextInfo *contextInfo,
			    int          serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    lapbFlowEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lapbFlowIfIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
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
	    return (NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbFlowIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lapbFlowEntry_get(serialNum, contextInfo, arg, searchType, lapbFlowIfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->lapbFlowIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_lapbFlowIfIndex
      case I_lapbFlowIfIndex:
	dp = &data->lapbFlowIfIndex;
	break;
#endif				       /* I_lapbFlowIfIndex */

#ifdef I_lapbFlowStateChanges
      case I_lapbFlowStateChanges:
	dp = &data->lapbFlowStateChanges;
	break;
#endif				       /* I_lapbFlowStateChanges */

#ifdef I_lapbFlowChangeReason
      case I_lapbFlowChangeReason:
	dp = &data->lapbFlowChangeReason;
	break;
#endif				       /* I_lapbFlowChangeReason */

#ifdef I_lapbFlowCurrentMode
      case I_lapbFlowCurrentMode:
	dp = &data->lapbFlowCurrentMode;
	break;
#endif				       /* I_lapbFlowCurrentMode */

#ifdef I_lapbFlowBusyDefers
      case I_lapbFlowBusyDefers:
	dp = &data->lapbFlowBusyDefers;
	break;
#endif				       /* I_lapbFlowBusyDefers */

#ifdef I_lapbFlowRejOutPkts
      case I_lapbFlowRejOutPkts:
	dp = &data->lapbFlowRejOutPkts;
	break;
#endif				       /* I_lapbFlowRejOutPkts */

#ifdef I_lapbFlowRejInPkts
      case I_lapbFlowRejInPkts:
	dp = &data->lapbFlowRejInPkts;
	break;
#endif				       /* I_lapbFlowRejInPkts */

#ifdef I_lapbFlowT1Timeouts
      case I_lapbFlowT1Timeouts:
	dp = &data->lapbFlowT1Timeouts;
	break;
#endif				       /* I_lapbFlowT1Timeouts */

#ifdef I_lapbFlowFrmrSent
      case I_lapbFlowFrmrSent:
	dp = MakeOctetString(data->lapbFlowFrmrSent->octet_ptr, data->lapbFlowFrmrSent->length);
	break;
#endif				       /* I_lapbFlowFrmrSent */

#ifdef I_lapbFlowFrmrReceived
      case I_lapbFlowFrmrReceived:
	dp = MakeOctetString(data->lapbFlowFrmrReceived->octet_ptr, data->lapbFlowFrmrReceived->length);
	break;
#endif				       /* I_lapbFlowFrmrReceived */

#ifdef I_lapbFlowXidReceived
      case I_lapbFlowXidReceived:
	dp = MakeOctetString(data->lapbFlowXidReceived->octet_ptr, data->lapbFlowXidReceived->length);
	break;
#endif				       /* I_lapbFlowXidReceived */

      default:
	return (NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the lapbXidEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *lapbXidEntry_get (OID         *incoming,
			   ObjectInfo  *object,
			   int          searchType,
			   ContextInfo *contextInfo,
			   int          serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    lapbXidEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lapbXidIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
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
	    return (NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbXidIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_lapbXidEntry_get(serialNum, contextInfo, arg, searchType, lapbXidIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->lapbXidIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_lapbXidIndex
      case I_lapbXidIndex:
	dp = &data->lapbXidIndex;
	break;
#endif				       /* I_lapbXidIndex */

#ifdef I_lapbXidAdRIdentifier
      case I_lapbXidAdRIdentifier:
	dp = MakeOctetString(data->lapbXidAdRIdentifier->octet_ptr, data->lapbXidAdRIdentifier->length);
	break;
#endif				       /* I_lapbXidAdRIdentifier */

#ifdef I_lapbXidAdRAddress
      case I_lapbXidAdRAddress:
	dp = MakeOctetString(data->lapbXidAdRAddress->octet_ptr, data->lapbXidAdRAddress->length);
	break;
#endif				       /* I_lapbXidAdRAddress */

#ifdef I_lapbXidParameterUniqueIdentifier
      case I_lapbXidParameterUniqueIdentifier:
	dp = MakeOctetString(data->lapbXidParameterUniqueIdentifier->octet_ptr, data->lapbXidParameterUniqueIdentifier->length);
	break;
#endif				       /* I_lapbXidParameterUniqueIdentifier */

#ifdef I_lapbXidGroupAddress
      case I_lapbXidGroupAddress:
	dp = MakeOctetString(data->lapbXidGroupAddress->octet_ptr, data->lapbXidGroupAddress->length);
	break;
#endif				       /* I_lapbXidGroupAddress */

#ifdef I_lapbXidPortNumber
      case I_lapbXidPortNumber:
	dp = MakeOctetString(data->lapbXidPortNumber->octet_ptr, data->lapbXidPortNumber->length);
	break;
#endif				       /* I_lapbXidPortNumber */

#ifdef I_lapbXidUserDataSubfield
      case I_lapbXidUserDataSubfield:
	dp = MakeOctetString(data->lapbXidUserDataSubfield->octet_ptr, data->lapbXidUserDataSubfield->length);
	break;
#endif				       /* I_lapbXidUserDataSubfield */

      default:
	return (NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the lapbXidEntry data object.
 *---------------------------------------------------------------------*/
static void lapbXidEntry_free (lapbXidEntry_t *data)
{
    if (data != NULL) {
	if (data->lapbXidAdRIdentifier != NULL) {
	    FreeOctetString(data->lapbXidAdRIdentifier);
	}
	if (data->lapbXidAdRAddress != NULL) {
	    FreeOctetString(data->lapbXidAdRAddress);
	}
	if (data->lapbXidParameterUniqueIdentifier != NULL) {
	    FreeOctetString(data->lapbXidParameterUniqueIdentifier);
	}
	if (data->lapbXidGroupAddress != NULL) {
	    FreeOctetString(data->lapbXidGroupAddress);
	}
	if (data->lapbXidPortNumber != NULL) {
	    FreeOctetString(data->lapbXidPortNumber);
	}
	if (data->lapbXidUserDataSubfield != NULL) {
	    FreeOctetString(data->lapbXidUserDataSubfield);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lapbXidEntry set/undo
 *---------------------------------------------------------------------*/
static int lapbXidEntry_cleanup (doList_t *trash)
{
    lapbXidEntry_free(trash->data);
#ifdef SR_SNMPv2
    lapbXidEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Undo a previous set of the lapbXidEntry family.
 *---------------------------------------------------------------------*/
#ifdef SR_SNMPv2
int lapbXidEntry_undo (doList_t    *doHead,
		       doList_t    *doCur,
		       ContextInfo *contextInfo)
{
    return UNDO_FAILED_ERROR;
}

#endif	/* SR_SNMPv2 */

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
int lapbXidEntry_test (OID            *incoming,
		       ObjectInfo     *object,
		       ObjectSyntax   *value,
		       doList_t       *doHead,
		       doList_t       *doCur,
		       ContextInfo    *contextInfo)
{
    int             instLength;
    doList_t       *dp;
    VarBind        *vb;
    int             found;
    int             carry;
    lapbXidEntry_t *lapbXidEntry;
    long            lapbXidIndex;

    instLength = incoming->length - object->oid.length;
    vb = NULL;
    carry = 0;

    /*
     * Validate the object instance
     * 
     */

    if ((InstToInt(incoming, 0 + object->oid.length, &lapbXidIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    lapbXidEntry = k_lapbXidEntry_get(-1, contextInfo, object->nominator, EXACT, lapbXidIndex);

#ifndef lapbXidEntry_READ_CREATE

    if (lapbXidEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* lapbXidEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == lapbXidEntry_set) &&
	    (((lapbXidEntry_t *) (dp->data)) != NULL) &&
	  (((lapbXidEntry_t *) (dp->data))->lapbXidIndex == lapbXidIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(lapbXidEntry_t))) == NULL) {
	    DPRINTF((0, "snmpd: Cannot allocate memory\n"));
	    return (GEN_ERROR);
	}
	memset(dp->data, 0, sizeof(lapbXidEntry_t));

	dp->setMethod = lapbXidEntry_set;
	dp->cleanupMethod = lapbXidEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = lapbXidEntry_undo;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_lapbXidIndex, ((lapbXidEntry_t *) (dp->data))->valid);
	((lapbXidEntry_t *) (dp->data))->lapbXidIndex = lapbXidIndex;

#ifdef lapbXidEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (lapbXidEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, lapbXidEntry, sizeof(lapbXidEntry_t));

	    SET_ALL_VALID(((lapbXidEntry_t *) (dp->data))->valid);

	    ((lapbXidEntry_t *) (dp->data))->lapbXidAdRIdentifier =
		CloneOctetString(value->os_value);


	    ((lapbXidEntry_t *) (dp->data))->lapbXidAdRAddress =
		CloneOctetString(value->os_value);


	    ((lapbXidEntry_t *) (dp->data))->lapbXidParameterUniqueIdentifier =
		CloneOctetString(value->os_value);


	    ((lapbXidEntry_t *) (dp->data))->lapbXidGroupAddress =
		CloneOctetString(value->os_value);


	    ((lapbXidEntry_t *) (dp->data))->lapbXidPortNumber =
		CloneOctetString(value->os_value);


	    ((lapbXidEntry_t *) (dp->data))->lapbXidUserDataSubfield =
		CloneOctetString(value->os_value);


	} else {


	    /* Fill in default values here */


	}
#endif				       /* lapbXidEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_lapbXidAdRIdentifier
      case I_lapbXidAdRIdentifier:

	SET_VALID(I_lapbXidAdRIdentifier, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidAdRIdentifier != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidAdRIdentifier);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidAdRIdentifier =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidAdRIdentifier */

#ifdef I_lapbXidAdRAddress
      case I_lapbXidAdRAddress:

	SET_VALID(I_lapbXidAdRAddress, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidAdRAddress != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidAdRAddress);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidAdRAddress =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidAdRAddress */

#ifdef I_lapbXidParameterUniqueIdentifier
      case I_lapbXidParameterUniqueIdentifier:

	SET_VALID(I_lapbXidParameterUniqueIdentifier, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidParameterUniqueIdentifier != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidParameterUniqueIdentifier);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidParameterUniqueIdentifier =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidParameterUniqueIdentifier */

#ifdef I_lapbXidGroupAddress
      case I_lapbXidGroupAddress:

	SET_VALID(I_lapbXidGroupAddress, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidGroupAddress != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidGroupAddress);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidGroupAddress =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidGroupAddress */

#ifdef I_lapbXidPortNumber
      case I_lapbXidPortNumber:

	SET_VALID(I_lapbXidPortNumber, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidPortNumber != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidPortNumber);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidPortNumber =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidPortNumber */

#ifdef I_lapbXidUserDataSubfield
      case I_lapbXidUserDataSubfield:

	SET_VALID(I_lapbXidUserDataSubfield, ((lapbXidEntry_t *) (dp->data))->valid);

	if (((lapbXidEntry_t *) (dp->data))->lapbXidUserDataSubfield != NULL) {
	    FreeOctetString(((lapbXidEntry_t *) (dp->data))->lapbXidUserDataSubfield);
	}
	((lapbXidEntry_t *) (dp->data))->lapbXidUserDataSubfield =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_lapbXidUserDataSubfield */

      default:
	DPRINTF((0, "snmpd: Internal error (invalid nominator in lapbXidEntry)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int lapbXidEntry_set (doList_t    *doHead,
		      doList_t    *doCur,
		      ContextInfo *contextInfo)
{
    return (k_lapbXidEntry_set((lapbXidEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				       /* SETS */


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

static boolean snmp_lapb_idb_tester (hwidbtype *idb, long* index2)
{
    return (is_lapb(idb));
}

lapbAdmnEntry_t *k_lapbAdmnEntry_get (int          serialNum,
				      ContextInfo *contextInfo,
				      int          nominator,
				      int          searchType,
				      long         lapbAdmnIndex)
{
    static lapbAdmnEntry_t lapbAdmnEntryData;
    hwidbtype             *hwidb;

    hwidb = ifmib_hwidb_get(serialNum, searchType, lapbAdmnIndex,
			    snmp_lapb_idb_tester, NULL);
    if (hwidb==NULL)
	return (NULL);

    lapbAdmnEntryData.lapbAdmnIndex = hwidb->snmp_if_index;
    if (IS_DCE(hwidb, lapb_dce))
	lapbAdmnEntryData.lapbAdmnStationType = D_lapbAdmnStationType_dce;
    else
	lapbAdmnEntryData.lapbAdmnStationType = D_lapbAdmnStationType_dte;
    lapbAdmnEntryData.lapbAdmnControlField = 
	D_lapbAdmnControlField_modulo8;	/* fixed at modulo 8 */
    lapbAdmnEntryData.lapbAdmnTransmitN1FrameSize = hwidb->lapb_n1;
    lapbAdmnEntryData.lapbAdmnReceiveN1FrameSize = hwidb->lapb_n1;
    lapbAdmnEntryData.lapbAdmnTransmitKWindowSize = hwidb->lapb_k;
    lapbAdmnEntryData.lapbAdmnReceiveKWindowSize = hwidb->lapb_k;
    lapbAdmnEntryData.lapbAdmnN2RxmitCount = hwidb->lapb_n2;
    lapbAdmnEntryData.lapbAdmnT1AckTimer = hwidb->lapb_t1;
    lapbAdmnEntryData.lapbAdmnT2AckDelayTimer = 0;
    lapbAdmnEntryData.lapbAdmnT3DisconnectTimer = 0;
    lapbAdmnEntryData.lapbAdmnT4IdleTimer = MAX_POSITIVE_32;
    lapbAdmnEntryData.lapbAdmnActionInitiate = 
	D_lapbAdmnActionInitiate_sendSABM;	/* fixed at send SABM */
    lapbAdmnEntryData.lapbAdmnActionRecvDM = 
	D_lapbAdmnActionInitiate_sendSABM;	/* fixed at send SABM */

    return (&lapbAdmnEntryData);
}

int k_lapbAdmnEntry_set (lapbAdmnEntry_t *data,
			 ContextInfo    *contextInfo,
			 int             function)

{
    return (GEN_ERROR);
}

lapbOperEntry_t *k_lapbOperEntry_get (int             serialNum,
				      ContextInfo    *contextInfo,
				      int             nominator,
				      int             searchType,
				      long            lapbOperIndex)
{
    static lapbOperEntry_t lapbOperEntryData;
    hwidbtype             *hwidb;

    hwidb = ifmib_hwidb_get(serialNum, searchType, lapbOperIndex,
			    snmp_lapb_idb_tester, NULL);
    if (hwidb==NULL)
	return (NULL);

    lapbOperEntryData.lapbOperIndex = hwidb->snmp_if_index;
    if (IS_DCE(hwidb, lapb_dce))
	lapbOperEntryData.lapbOperStationType = D_lapbOperStationType_dce;
    else
	lapbOperEntryData.lapbOperStationType = D_lapbOperStationType_dte;
    lapbOperEntryData.lapbOperControlField = D_lapbOperControlField_modulo8;
    lapbOperEntryData.lapbOperTransmitN1FrameSize = hwidb->lapb_n1;
    lapbOperEntryData.lapbOperReceiveN1FrameSize = hwidb->lapb_n1;
    lapbOperEntryData.lapbOperTransmitKWindowSize = hwidb->lapb_k;
    lapbOperEntryData.lapbOperReceiveKWindowSize = hwidb->lapb_k;
    lapbOperEntryData.lapbOperN2RxmitCount = hwidb->lapb_n2;
    lapbOperEntryData.lapbOperT1AckTimer = hwidb->lapb_t1;
    lapbOperEntryData.lapbOperT2AckDelayTimer = 0;
    lapbOperEntryData.lapbOperT3DisconnectTimer = 0;
    lapbOperEntryData.lapbOperT4IdleTimer = MAX_POSITIVE_32;
    lapbOperEntryData.lapbOperPortId = &lapbnullOID;
    lapbOperEntryData.lapbOperProtocolVersionId =
	&lapbProtocolVersionSupported; 

    return (&lapbOperEntryData);
}

int k_lapbOperEntry_set (lapbOperEntry_t *data,
			 ContextInfo    *contextInfo,
			 int             function)
{
    return (GEN_ERROR);
}

lapbFlowEntry_t *k_lapbFlowEntry_get (int          serialNum,
				      ContextInfo *contextInfo,
				      int          nominator,
				      int          searchType,
				      long         lapbFlowIfIndex)
{
    static lapbFlowEntry_t lapbFlowEntryData;
    static OctetString     lapbFlowFrmrSent;
    static OctetString     lapbFlowFrmrReceived;
    static OctetString     lapbFlowXidReceived;

    hwidbtype              *hwidb;

    hwidb = ifmib_hwidb_get(serialNum, searchType, lapbFlowIfIndex,
			    snmp_lapb_idb_tester, NULL);
    if (hwidb==NULL)
	return (NULL);

    lapbFlowEntryData.lapbFlowIfIndex = hwidb->snmp_if_index;
    lapbFlowEntryData.lapbFlowStateChanges = hwidb->lapb_mib_state_changes;
    lapbFlowEntryData.lapbFlowChangeReason = hwidb->lapb_state_reason;
    lapbFlowEntryData.lapbFlowCurrentMode = hwidb->lapb_mib_state;
    lapbFlowEntryData.lapbFlowBusyDefers = hwidb->lapb_tx_deferrals;
    lapbFlowEntryData.lapbFlowRejOutPkts = hwidb->lapb_rejssent;
    lapbFlowEntryData.lapbFlowRejInPkts = hwidb->lapb_rejsrcvd;
    lapbFlowEntryData.lapbFlowT1Timeouts = hwidb->lapb_t1_retx;

    lapbFlowEntryData.lapbFlowFrmrSent = &lapbFlowFrmrSent;
    lapbFlowEntryData.lapbFlowFrmrSent->octet_ptr = hwidb->lapb_frmr_txdata;
    lapbFlowEntryData.lapbFlowFrmrSent->length = hwidb->lapb_frmr_txlen;

    lapbFlowEntryData.lapbFlowFrmrReceived = &lapbFlowFrmrReceived;
    lapbFlowEntryData.lapbFlowFrmrReceived->octet_ptr =
	hwidb->lapb_frmr_rxdata; 
    lapbFlowEntryData.lapbFlowFrmrReceived->length = hwidb->lapb_frmr_rxlen;

    lapbFlowEntryData.lapbFlowXidReceived = &lapbFlowXidReceived;
    lapbFlowEntryData.lapbFlowXidReceived->octet_ptr = NULL;
    lapbFlowEntryData.lapbFlowXidReceived->length = 0;

    return (&lapbFlowEntryData);
}

lapbXidEntry_t *k_lapbXidEntry_get (int          serialNum,
				    ContextInfo *contextInfo,
				    int          nominator,
				    int          searchType,
				    long         lapbXidIndex)
{
#ifdef NOT_YET
    static lapbXidEntry_t lapbXidEntryData;

    lapbXidEntryData.lapbXidIndex = 
    lapbXidEntryData.lapbXidAdRIdentifier = 
    lapbXidEntryData.lapbXidAdRAddress = 
    lapbXidEntryData.lapbXidParameterUniqueIdentifier = 
    lapbXidEntryData.lapbXidGroupAddress = 
    lapbXidEntryData.lapbXidPortNumber = 
    lapbXidEntryData.lapbXidUserDataSubfield = 

    return (&lapbXidEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}

int k_lapbXidEntry_set (lapbXidEntry_t *data,
			ContextInfo    *contextInfo,
			int             function)
{
    return (GEN_ERROR);
}


/*
 * lapbmib subsystem header
 */

#define MAJVERSION_lapbmib 1
#define MINVERSION_lapbmib 0
#define EDITVERSION_lapbmib 0

SUBSYS_HEADER(lapbmib,
              MAJVERSION_lapbmib,
              MINVERSION_lapbmib,
              EDITVERSION_lapbmib,
              init_lapbmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: x25",
              NULL);
      
