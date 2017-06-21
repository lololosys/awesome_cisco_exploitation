/* $Id: sr_channelmib.c,v 3.8.8.4 1996/08/05 20:00:20 johnchap Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_channelmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1994, Mauro D. Zallocco (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_channelmib.c,v $
 * Revision 3.8.8.4  1996/08/05  20:00:20  johnchap
 * CSCdi55031:  Missing variables in CIP MIB
 * Branch: California_branch
 *
 * Revision 3.8.8.3  1996/07/30  18:48:19  lmenditt
 * CSCdi62904:  Spurious memory access made at k_cipCardSubChannelEntry_get
 * Branch: California_branch
 *
 * Revision 3.8.8.2  1996/07/01  18:44:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.8.1  1996/06/26  12:46:49  johnchap
 * CSCdi55031:  Missing variables in CIP MIB
 * Branch: California_branch
 * Added cipCardSubChannelCmdretries to cipCardSubChannelTable
 * Added cipCardDtrBrdLastStat and cipCardDtrBrdNextStat
 * tocipCardDaughterBoardTable
 *
 * Revision 3.8  1996/02/27  22:12:40  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/01/18  15:45:40  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.6  1995/12/14  08:23:58  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.5  1995/12/05  15:12:49  motto
 * CSCdi30997:  snmp sub-agent for IBM channel claw cfg returns too many
 * instances
 *
 * Revision 3.4  1995/12/05  14:48:49  motto
 * CSCdi41938:  SNMP gets of the CIP daughter brd rets NULL if I/F not
 * cfgd w/devic
 *
 * Revision 3.3  1995/11/17  09:19:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:42:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:01:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:24:39  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:43:37  hampton
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
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "../h/interface_private.h"
#include "channelmib_utils.h"
#include "sysmgt_channel.h"
#include "sr_channelmib.h"
#include "sr_channelmib-mib.h"
#include "../snmp/snmp_registry.h"
#include "if_cip.h"
#include "ucode.h"
#include "../dev/flash_private.h"
#include "if_fci.h"
#include "slot.h"
#include "cbus_registry.h"
#include "../snmp/snmp_trap_fe.h"
#include "../h/types.h"
#include "sr_snallcmib.h"
#include "sysmgt_snallc.h"
#include "../dspu/cipllc_types.h"
#include "../parser/parser_defs_snmp.h"
  

static void cip_chantrap( ushort code, cip_linktype *chan_data);

static char * trap_cardDtrBrdLinkFailure =  "channel.1.5.2";

static void init_channelmib (subsystype *subsys)
{
    load_mib(channelmib_OidList, channelmib_OidListNum);
    load_oid(channelmib_oid_table);
    reg_add_cip_cipchan_trap(cip_chantrap, "cip_chantrap");
    register_snmp_trap(TRAP_LLC2, trap_cardDtrBrdLinkFailure);
}
  





/*---------------------------------------------------------------------
 * Retrieve data from the cipCardEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardEntryName
      case I_cipCardEntryName:
	dp = MakeOctetString(data->cipCardEntryName->octet_ptr, data->cipCardEntryName->length);
	break;
#endif				       /* I_cipCardEntryName */

#ifdef I_cipCardEntryTotalMemory
      case I_cipCardEntryTotalMemory:
	dp = &data->cipCardEntryTotalMemory;
	break;
#endif				       /* I_cipCardEntryTotalMemory */

#ifdef I_cipCardEntryFreeMemory
      case I_cipCardEntryFreeMemory:
	dp = &data->cipCardEntryFreeMemory;
	break;
#endif				       /* I_cipCardEntryFreeMemory */

#ifdef I_cipCardEntryCpuUtilization
      case I_cipCardEntryCpuUtilization:
	dp = &data->cipCardEntryCpuUtilization;
	break;
#endif				       /* I_cipCardEntryCpuUtilization */

#ifdef I_cipCardEntryCpuLoad1m
      case I_cipCardEntryCpuLoad1m:
	dp = &data->cipCardEntryCpuLoad1m;
	break;
#endif				       /* I_cipCardEntryCpuLoad1m */

#ifdef I_cipCardEntryCpuLoad5m
      case I_cipCardEntryCpuLoad5m:
	dp = &data->cipCardEntryCpuLoad5m;
	break;
#endif				       /* I_cipCardEntryCpuLoad5m */

#ifdef I_cipCardEntryCpuLoad60m
      case I_cipCardEntryCpuLoad60m:
	dp = &data->cipCardEntryCpuLoad60m;
	break;
#endif				       /* I_cipCardEntryCpuLoad60m */

#ifdef I_cipCardEntryTimeSinceLastReset
      case I_cipCardEntryTimeSinceLastReset:
	dp = &data->cipCardEntryTimeSinceLastReset;
	break;
#endif				       /* I_cipCardEntryTimeSinceLastReset */

#ifdef I_cipCardEntryDmaLoad1m
      case I_cipCardEntryDmaLoad1m:
	dp = &data->cipCardEntryDmaLoad1m;
	break;
#endif				       /* I_cipCardEntryDmaLoad1m */

#ifdef I_cipCardEntryDmaLoad5m
      case I_cipCardEntryDmaLoad5m:
	dp = &data->cipCardEntryDmaLoad5m;
	break;
#endif				       /* I_cipCardEntryDmaLoad5m */

#ifdef I_cipCardEntryDmaLoad60m
      case I_cipCardEntryDmaLoad60m:
	dp = &data->cipCardEntryDmaLoad60m;
	break;
#endif				       /* I_cipCardEntryDmaLoad60m */

#ifdef I_cipCardEntryMajorSwRevisionNr
    case I_cipCardEntryMajorSwRevisionNr:
       dp = (void *) (&data->cipCardEntryMajorSwRevisionNr);
       break;
#endif /* I_cipCardEntryMajorSwRevisionNr */

#ifdef I_cipCardEntryMinorSwRevisionNr
    case I_cipCardEntryMinorSwRevisionNr:
       dp = (void *) (&data->cipCardEntryMinorSwRevisionNr);
       break;
#endif /* I_cipCardEntryMinorSwRevisionNr */

#ifdef I_cipCardEntryMajorHwRevisionNr
    case I_cipCardEntryMajorHwRevisionNr:
       dp = (void *) (&data->cipCardEntryMajorHwRevisionNr);
       break;
#endif /* I_cipCardEntryMajorHwRevisionNr */

#ifdef I_cipCardEntryMinorHwRevisionNr
    case I_cipCardEntryMinorHwRevisionNr:
       dp = (void *) (&data->cipCardEntryMinorHwRevisionNr);
       break;
#endif /* I_cipCardEntryMinorHwRevisionNr */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the cipCardEntry data object.
 *---------------------------------------------------------------------*/
static void cipCardEntry_free(cipCardEntry_t *data)
{
    if (data != NULL) {
	if (data->cipCardEntryName != NULL) {
	    FreeOctetString(data->cipCardEntryName);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardEntry set/undo
 *---------------------------------------------------------------------*/
static int cipCardEntry_cleanup(doList_t *trash)
{
    cipCardEntry_free(trash->data);
#ifdef SR_SNMPv2
    cipCardEntry_free(trash->undodata);
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
cipCardEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;

#if 0
    VarBind        *vb = (VarBind *) NULL;
#endif

    int             found;
    int             carry = 0;
    cipCardEntry_t *cipCardEntry;
    long            cipCardEntryIndex;

    return NOT_WRITABLE_ERROR; /* set not supported */
    
    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    cipCardEntry = k_cipCardEntry_get(-1, contextInfo, object->nominator, EXACT, cipCardEntryIndex);

#ifndef cipCardEntry_READ_CREATE

    if (cipCardEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* cipCardEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == cipCardEntry_set) &&
	    (((cipCardEntry_t *) (dp->data)) != NULL) &&
	    (((cipCardEntry_t *) (dp->data))->cipCardEntryIndex == cipCardEntryIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(cipCardEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(cipCardEntry_t));

	dp->setMethod = cipCardEntry_set;
	dp->cleanupMethod = cipCardEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_cipCardEntryIndex, ((cipCardEntry_t *) (dp->data))->valid);
	((cipCardEntry_t *) (dp->data))->cipCardEntryIndex = cipCardEntryIndex;

#ifdef cipCardEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (cipCardEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, cipCardEntry, sizeof(cipCardEntry_t));

	    SET_ALL_VALID(((cipCardEntry_t *) (dp->data))->valid);

	    ((cipCardEntry_t *) (dp->data))->cipCardEntryName =
		CloneOctetString(cipCardEntry->cipCardEntryName);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* cipCardEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_cipCardEntryName
      case I_cipCardEntryName:

	SET_VALID(I_cipCardEntryName, ((cipCardEntry_t *) (dp->data))->valid);

	if (((cipCardEntry_t *) (dp->data))->cipCardEntryName != NULL) {
	    FreeOctetString(((cipCardEntry_t *) (dp->data))->cipCardEntryName);
	}
	((cipCardEntry_t *) (dp->data))->cipCardEntryName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardEntryName */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardEntry_test)\n"));
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
cipCardEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_cipCardEntry_set((cipCardEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipCardDaughterBoardEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardDaughterBoardEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardDaughterBoardEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;


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

    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardDaughterBoardEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex, cipCardDtrBrdIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
	inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardDtrBrdType
      case I_cipCardDtrBrdType:
	dp = &data->cipCardDtrBrdType;
	break;
#endif				       /* I_cipCardDtrBrdType */

#ifdef I_cipCardDtrBrdStatus
      case I_cipCardDtrBrdStatus:
	dp = &data->cipCardDtrBrdStatus;
	break;
#endif				       /* I_cipCardDtrBrdStatus */

#ifdef I_cipCardDtrBrdSignal
      case I_cipCardDtrBrdSignal:
	dp = &data->cipCardDtrBrdSignal;
	break;
#endif				       /* I_cipCardDtrBrdSignal */

#ifdef I_cipCardDtrBrdOnline
      case I_cipCardDtrBrdOnline:
	dp = &data->cipCardDtrBrdOnline;
	break;
#endif				       /* I_cipCardDtrBrdOnline */

#ifdef I_implicitIncidents
      case I_implicitIncidents:
	dp = &data->implicitIncidents;
	break;
#endif				       /* I_implicitIncidents */

#ifdef I_codeViolationErrors
      case I_codeViolationErrors:
	dp = &data->codeViolationErrors;
	break;
#endif				       /* I_codeViolationErrors */

#ifdef I_linkFailureSignalOrSyncLoss
      case I_linkFailureSignalOrSyncLoss:
	dp = &data->linkFailureSignalOrSyncLoss;
	break;
#endif				       /* I_linkFailureSignalOrSyncLoss */

#ifdef I_linkFailureNOSs
      case I_linkFailureNOSs:
	dp = &data->linkFailureNOSs;
	break;
#endif				       /* I_linkFailureNOSs */

#ifdef I_linkFailureSequenceTimeouts
      case I_linkFailureSequenceTimeouts:
	dp = &data->linkFailureSequenceTimeouts;
	break;
#endif				       /* I_linkFailureSequenceTimeouts */

#ifdef I_linkFailureInvalidSequences
      case I_linkFailureInvalidSequences:
	dp = &data->linkFailureInvalidSequences;
	break;
#endif				       /* I_linkFailureInvalidSequences */

#ifdef I_linkIncidentTrapCause
      case I_linkIncidentTrapCause:
	dp = &data->linkIncidentTrapCause;
	break;
#endif				       /* I_linkIncidentTrapCause */

#ifdef I_cipCardDtrBrdLastStat
      case I_cipCardDtrBrdLastStat:
        dp = (void *) (&data->cipCardDtrBrdLastStat);
        break;
#endif                                  /* I_cipCardDtrBrdLastStat */

#ifdef I_cipCardDtrBrdNextStat
      case I_cipCardDtrBrdNextStat:
        dp = (void *) (&data->cipCardDtrBrdNextStat);
        break;
#endif                                  /* I_cipCardDtrBrdNextStat */

#ifdef I_cipCardDtrBrdChannelLoad1m
      case I_cipCardDtrBrdChannelLoad1m:
        dp = (void *) (&data->cipCardDtrBrdChannelLoad1m);
        break;
#endif                                 /* I_cipCardDtrBrdChannelLoad1m */

#ifdef I_cipCardDtrBrdChannelLoad5m
      case I_cipCardDtrBrdChannelLoad5m:
        dp = (void *) (&data->cipCardDtrBrdChannelLoad5m);
        break;
#endif                                 /* I_cipCardDtrBrdChannelLoad5m */

#ifdef I_cipCardDtrBrdChannelLoad60m
      case I_cipCardDtrBrdChannelLoad60m:
        dp = (void *) (&data->cipCardDtrBrdChannelLoad60m);
        break;
#endif                                 /* I_cipCardDtrBrdChannelLoad60m */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardSubChannelEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardSubChannelEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardSubChannelEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;


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

    if ((InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardSubChannelEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 3;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
	inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
	inst.oid_ptr[2] = data->cipCardSubChannelIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardSubChannelIndex
      case I_cipCardSubChannelIndex:
	dp = &data->cipCardSubChannelIndex;
	break;
#endif				       /* I_cipCardSubChannelIndex */

#ifdef I_cipCardSubChannelConnections
      case I_cipCardSubChannelConnections:
	dp = &data->cipCardSubChannelConnections;
	break;
#endif				       /* I_cipCardSubChannelConnections */

#ifdef I_cipCardSubChannelCancels
      case I_cipCardSubChannelCancels:
	dp = &data->cipCardSubChannelCancels;
	break;
#endif				       /* I_cipCardSubChannelCancels */

#ifdef I_cipCardSubChannelSelectiveResets
      case I_cipCardSubChannelSelectiveResets:
	dp = &data->cipCardSubChannelSelectiveResets;
	break;
#endif				       /* I_cipCardSubChannelSelectiveResets */

#ifdef I_cipCardSubChannelSystemResets
      case I_cipCardSubChannelSystemResets:
	dp = &data->cipCardSubChannelSystemResets;
	break;
#endif				       /* I_cipCardSubChannelSystemResets */

#ifdef I_cipCardSubChannelDeviceErrors
      case I_cipCardSubChannelDeviceErrors:
	dp = &data->cipCardSubChannelDeviceErrors;
	break;
#endif				       /* I_cipCardSubChannelDeviceErrors */

#ifdef I_cipCardSubChannelWriteBlocksDropped
      case I_cipCardSubChannelWriteBlocksDropped:
	dp = &data->cipCardSubChannelWriteBlocksDropped;
	break;
#endif				       /* I_cipCardSubChannelWriteBlocksDroppe
				        * d */

#ifdef I_cipCardSubChannelLastSenseData
      case I_cipCardSubChannelLastSenseData:
	dp = MakeOctetString(data->cipCardSubChannelLastSenseData->octet_ptr, data->cipCardSubChannelLastSenseData->length);
	break;
#endif				       /* I_cipCardSubChannelLastSenseData */

#ifdef I_cipCardSubChannelLastSenseDataTime
      case I_cipCardSubChannelLastSenseDataTime:
	dp = &data->cipCardSubChannelLastSenseDataTime;
	break;
#endif				       /* I_cipCardSubChannelLastSenseDataTime
				        *  */

#ifdef I_cipCardSubChannelCuBusies
      case I_cipCardSubChannelCuBusies:
	dp = &data->cipCardSubChannelCuBusies;
	break;
#endif				       /* I_cipCardSubChannelCuBusies */

#ifdef I_cipCardSubChannelCmdRetries
      case I_cipCardSubChannelCmdRetries:
        dp = (void *) (&data->cipCardSubChannelCmdRetries);
        break;
#endif                                 /* I_cipCardSubChannelCmdRetries */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardClawEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardClawEntry_get (incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardClawEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;


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

    if ((InstToInt(incoming, 3 + object->oid.length, &cipCardClawIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardClawEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
	inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
	inst.oid_ptr[2] = data->cipCardSubChannelIndex;
	inst.oid_ptr[3] = data->cipCardClawIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardClawIndex
      case I_cipCardClawIndex:
	dp = &data->cipCardClawIndex;
	break;
#endif				       /* I_cipCardClawIndex */

#ifdef I_cipCardClawConnected
      case I_cipCardClawConnected:
	dp = &data->cipCardClawConnected;
	break;
#endif				       /* I_cipCardClawConnected */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardClawConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardClawConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardClawConfigEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;


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

    if ((InstToInt(incoming, 3 + object->oid.length, &cipCardClawIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardClawConfigEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
	inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
	inst.oid_ptr[2] = data->cipCardSubChannelIndex;
	inst.oid_ptr[3] = data->cipCardClawIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardClawConfigPath
      case I_cipCardClawConfigPath:
	dp = MakeOctetString(data->cipCardClawConfigPath->octet_ptr, data->cipCardClawConfigPath->length);
	break;
#endif				       /* I_cipCardClawConfigPath */

#ifdef I_cipCardClawConfigDevice
      case I_cipCardClawConfigDevice:
	dp = MakeOctetString(data->cipCardClawConfigDevice->octet_ptr, data->cipCardClawConfigDevice->length);
	break;
#endif				       /* I_cipCardClawConfigDevice */

#ifdef I_cipCardClawConfigIpAddr
      case I_cipCardClawConfigIpAddr:
	dp = IPToOctetString(data->cipCardClawConfigIpAddr);
	break;
#endif				       /* I_cipCardClawConfigIpAddr */

#ifdef I_cipCardClawConfigHostName
      case I_cipCardClawConfigHostName:
	dp = MakeOctetString(data->cipCardClawConfigHostName->octet_ptr, data->cipCardClawConfigHostName->length);
	break;
#endif				       /* I_cipCardClawConfigHostName */

#ifdef I_cipCardClawConfigRouterName
      case I_cipCardClawConfigRouterName:
	dp = MakeOctetString(data->cipCardClawConfigRouterName->octet_ptr, data->cipCardClawConfigRouterName->length);
	break;
#endif				       /* I_cipCardClawConfigRouterName */

#ifdef I_cipCardClawConfigHostAppl
      case I_cipCardClawConfigHostAppl:
	dp = MakeOctetString(data->cipCardClawConfigHostAppl->octet_ptr, data->cipCardClawConfigHostAppl->length);
	break;
#endif				       /* I_cipCardClawConfigHostAppl */

#ifdef I_cipCardClawConfigRouterAppl
      case I_cipCardClawConfigRouterAppl:
	dp = MakeOctetString(data->cipCardClawConfigRouterAppl->octet_ptr, data->cipCardClawConfigRouterAppl->length);
	break;
#endif				       /* I_cipCardClawConfigRouterAppl */

#ifdef I_cipCardClawConfigBroadcastEnable
    case I_cipCardClawConfigBroadcastEnable:
       dp = (void *) (&data->cipCardClawConfigBroadcastEnable);
       break;
#endif                                 /* I_cipCardClawConfigBroadcastEnable */

#ifdef I_cipCardClawConfigRowStatus
    case I_cipCardClawConfigRowStatus:
       dp = (void *) (&data->cipCardClawConfigRowStatus);
       break;
#endif                                 /* I_cipCardClawConfigRowStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the cipCardClawConfigEntry data object.
 *---------------------------------------------------------------------*/
static void cipCardClawConfigEntry_free(cipCardClawConfigEntry_t *data)
{
    if (data != NULL) {
	if (data->cipCardClawConfigPath != NULL) {
	    FreeOctetString(data->cipCardClawConfigPath);
	}
	if (data->cipCardClawConfigDevice != NULL) {
	    FreeOctetString(data->cipCardClawConfigDevice);
	}
	if (data->cipCardClawConfigHostName != NULL) {
	    FreeOctetString(data->cipCardClawConfigHostName);
	}
	if (data->cipCardClawConfigRouterName != NULL) {
	    FreeOctetString(data->cipCardClawConfigRouterName);
	}
	if (data->cipCardClawConfigHostAppl != NULL) {
	    FreeOctetString(data->cipCardClawConfigHostAppl);
	}
	if (data->cipCardClawConfigRouterAppl != NULL) {
	    FreeOctetString(data->cipCardClawConfigRouterAppl);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardClawConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int cipCardClawConfigEntry_cleanup(doList_t *trash)
{
    cipCardClawConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    cipCardClawConfigEntry_free(trash->undodata);
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
cipCardClawConfigEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;

#if 0
    VarBind        *vb = (VarBind *) NULL;
#endif

    int             found;
    int             carry = 0;
    cipCardClawConfigEntry_t *cipCardClawConfigEntry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;

    return NOT_WRITABLE_ERROR; /* set not supported */
    
    /*
     * Validate the object instance
     * 
     */
    if (instLength != 4) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 3 + object->oid.length, &cipCardClawIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    cipCardClawConfigEntry = k_cipCardClawConfigEntry_get(-1, contextInfo, object->nominator, EXACT, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex);

#ifndef cipCardClawConfigEntry_READ_CREATE

    if (cipCardClawConfigEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* cipCardClawConfigEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == cipCardClawConfigEntry_set) &&
	    (((cipCardClawConfigEntry_t *) (dp->data)) != NULL) &&
	    (((cipCardClawConfigEntry_t *) (dp->data))->cipCardEntryIndex == cipCardEntryIndex) &&
	    (((cipCardClawConfigEntry_t *) (dp->data))->cipCardDtrBrdIndex == cipCardDtrBrdIndex) &&
	    (((cipCardClawConfigEntry_t *) (dp->data))->cipCardSubChannelIndex == cipCardSubChannelIndex) &&
	    (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawIndex == cipCardClawIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(cipCardClawConfigEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(cipCardClawConfigEntry_t));

	dp->setMethod = cipCardClawConfigEntry_set;
	dp->cleanupMethod = cipCardClawConfigEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_cipCardEntryIndex, ((cipCardClawConfigEntry_t *) (dp->data))->valid);
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardEntryIndex = cipCardEntryIndex;

	SET_VALID(I_cipCardDtrBrdIndex, ((cipCardClawConfigEntry_t *) (dp->data))->valid);
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardDtrBrdIndex = cipCardDtrBrdIndex;

	SET_VALID(I_cipCardSubChannelIndex, ((cipCardClawConfigEntry_t *) (dp->data))->valid);
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardSubChannelIndex = cipCardSubChannelIndex;

	SET_VALID(I_cipCardClawIndex, ((cipCardClawConfigEntry_t *) (dp->data))->valid);
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawIndex = cipCardClawIndex;

#ifdef cipCardClawConfigEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (cipCardClawConfigEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, cipCardClawConfigEntry, sizeof(cipCardClawConfigEntry_t));

	    SET_ALL_VALID(((cipCardClawConfigEntry_t *) (dp->data))->valid);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigPath =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigPath);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigDevice =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigDevice);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostName =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigHostName);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterName =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigRouterName);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostAppl =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigHostAppl);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterAppl =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigRouterAppl);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigBroadcastEnable =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigBroadcastEnable);

	    ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRowStatus =
		CloneOctetString(cipCardClawConfigEntry->cipCardClawConfigRowStatus);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* cipCardClawConfigEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_cipCardClawConfigPath
      case I_cipCardClawConfigPath:

	SET_VALID(I_cipCardClawConfigPath, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigPath != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigPath);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigPath =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigPath */

#ifdef I_cipCardClawConfigDevice
      case I_cipCardClawConfigDevice:

	SET_VALID(I_cipCardClawConfigDevice, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigDevice != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigDevice);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigDevice =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigDevice */

#ifdef I_cipCardClawConfigIpAddr
      case I_cipCardClawConfigIpAddr:

	if (value->os_value->length != 4) {
	    return (WRONG_LENGTH_ERROR);
	}
	SET_VALID(I_cipCardClawConfigIpAddr, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigIpAddr = OctetStringToIP(value->os_value);
	break;
#endif				       /* I_cipCardClawConfigIpAddr */

#ifdef I_cipCardClawConfigHostName
      case I_cipCardClawConfigHostName:

	SET_VALID(I_cipCardClawConfigHostName, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostName != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostName);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigHostName */

#ifdef I_cipCardClawConfigRouterName
      case I_cipCardClawConfigRouterName:

	SET_VALID(I_cipCardClawConfigRouterName, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterName != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterName);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterName =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigRouterName */

#ifdef I_cipCardClawConfigHostAppl
      case I_cipCardClawConfigHostAppl:

	SET_VALID(I_cipCardClawConfigHostAppl, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostAppl != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostAppl);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigHostAppl =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigHostAppl */

#ifdef I_cipCardClawConfigRouterAppl
      case I_cipCardClawConfigRouterAppl:

	SET_VALID(I_cipCardClawConfigRouterAppl, ((cipCardClawConfigEntry_t *) (dp->data))->valid);

	if (((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterAppl != NULL) {
	    FreeOctetString(((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterAppl);
	}
	((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRouterAppl =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_cipCardClawConfigRouterAppl */

#ifdef I_cipCardClawConfigBroadcastEnable
   case I_cipCardClawConfigBroadcastEnable:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigBroadcastEnable = value->sl_value;
     break;
#endif                                 /* I_cipCardClawConfigBroadcastEnable */

#ifdef I_cipCardClawConfigRowStatus
   case I_cipCardClawConfigRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cipCardClawConfigRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cipCardClawConfigEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cipCardClawConfigRowStatus_notInService) || (value->sl_value == D_cipCardClawConfigRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			        /* modifying an existing row */
        if ((value->sl_value == D_cipCardClawConfigRowStatus_createAndGo) || (value->sl_value == D_cipCardClawConfigRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((cipCardClawConfigEntry_t *) (dp->data))->cipCardClawConfigRowStatus = value->sl_value;
     break;
#endif                                 /* I_cipCardClawConfigRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardClawConfigEntry_test)\n"));
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
cipCardClawConfigEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_cipCardClawConfigEntry_set((cipCardClawConfigEntry_t *) (doCur->data),
					 contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipCardClawDataXferStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
cipCardClawDataXferStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    cipCardClawDataXferStatsEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;


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

    if ((InstToInt(incoming, 3 + object->oid.length, &cipCardClawIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cipCardClawDataXferStatsEntry_get(serialNum, contextInfo, arg, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = data->cipCardEntryIndex;
	inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
	inst.oid_ptr[2] = data->cipCardSubChannelIndex;
	inst.oid_ptr[3] = data->cipCardClawIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_cipCardClawDataXferStatsBlocksRead
      case I_cipCardClawDataXferStatsBlocksRead:
	dp = &data->cipCardClawDataXferStatsBlocksRead;
	break;
#endif				       /* I_cipCardClawDataXferStatsBlocksRead */

#ifdef I_cipCardClawDataXferStatsBlocksWritten
      case I_cipCardClawDataXferStatsBlocksWritten:
	dp = &data->cipCardClawDataXferStatsBlocksWritten;
	break;
#endif				       /* I_cipCardClawDataXferStatsBlocksWritten */

#ifdef I_cipCardClawDataXferStatsBytesRead
      case I_cipCardClawDataXferStatsBytesRead:
	dp = &data->cipCardClawDataXferStatsBytesRead;
	break;
#endif				       /* I_cipCardClawDataXferStatsBytesRead */

#ifdef I_cipCardClawDataXferStatsHCBytesRead
      case I_cipCardClawDataXferStatsHCBytesRead:
	dp = MakeOctetString(
                 (unsigned char *)data->cipCardClawDataXferStatsHCBytesRead, 
                 sizeof(*data->cipCardClawDataXferStatsHCBytesRead));
	break;
#endif			       /* I_cipCardClawDataXferStatsHCBytesRead */

#ifdef I_cipCardClawDataXferStatsBytesWritten
      case I_cipCardClawDataXferStatsBytesWritten:
	dp = &data->cipCardClawDataXferStatsBytesWritten;
	break;
#endif				       /* I_cipCardClawDataXferStatsBytesWritten */

#ifdef I_cipCardClawDataXferStatsHCBytesWritten
      case I_cipCardClawDataXferStatsHCBytesWritten:
	dp = MakeOctetString(
	      (unsigned char *)data->cipCardClawDataXferStatsHCBytesWritten,
              sizeof(*data->cipCardClawDataXferStatsHCBytesWritten));
	break;
#endif			       /* I_cipCardClawDataXferStatsHCBytesWritten */

#ifdef I_cipCardClawDataXferStatsReadBlocksDropped
      case I_cipCardClawDataXferStatsReadBlocksDropped:
	dp = &data->cipCardClawDataXferStatsReadBlocksDropped;
	break;
#endif				       /* I_cipCardClawDataXferStatsReadBlocksDropped
				        *  */

#ifdef I_cipCardClawDataXferStatsWriteBlocksDropped
      case I_cipCardClawDataXferStatsWriteBlocksDropped:
	dp = &data->cipCardClawDataXferStatsWriteBlocksDropped;
	break;
#endif				       /* I_cipCardClawDataXferStatsWriteBlocksDroppe
				        * d */

#ifdef I_cipCardClawDataXferStatsBufferGetRetryCount
      case I_cipCardClawDataXferStatsBufferGetRetryCount:
	dp = &data->cipCardClawDataXferStatsBufferGetRetryCount;
	break;
#endif				       /* I_cipCardClawDataXferStatsBufferGetRetryCou
				        * nt */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardApplicationEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardApplicationEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             arg = object->nominator;
    void            *dp;
    cipCardApplicationEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            cipCardEntryIndex;
    int             cipCardEntryIndex_offset;
    OctetString *   cipCardApplicationNameIndex = NULL;
    int             cipCardApplicationNameIndex_offset;
    int             index;
    int             final_index;

    cipCardEntryIndex_offset = object->oid.length;
    cipCardApplicationNameIndex_offset = cipCardEntryIndex_offset + 1;
    final_index = cipCardApplicationNameIndex_offset + 32;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToFixedOctetString(incoming, cipCardApplicationNameIndex_offset, &cipCardApplicationNameIndex, searchType, &carry, 32)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, cipCardEntryIndex_offset, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardApplicationEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex, cipCardApplicationNameIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->cipCardEntryIndex;
        for(i = 0; i < data->cipCardApplicationNameIndex->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->cipCardApplicationNameIndex->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(cipCardApplicationNameIndex);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardApplicationRevision
    case I_cipCardApplicationRevision:
       dp = (void *) (&data->cipCardApplicationRevision);
       break;
#endif /* I_cipCardApplicationRevision */

#ifdef I_cipCardApplicationCompileInfo
    case I_cipCardApplicationCompileInfo:
       dp = (void *) (CloneOctetString(data->cipCardApplicationCompileInfo));
       break;
#endif /* I_cipCardApplicationCompileInfo */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

cipCardEntry_t *
k_cipCardEntry_get(serialNum, contextInfo, nominator, searchType, cipCardEntryIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
{
    static cipCardEntry_t cipCardEntryData;
    cipCardEntry_t * resultP = NULL;
    cip_idb_infotype *cip = NULL;
    static char cardName[20];
    static OctetString cipCardEntryName;
    cip_version_t cip_version;
    int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
        
    cip = cip_cardInfoGet(&cipCardEntryIndex, getType);
    
    if (cip) {
      cipCardEntryData.cipCardEntryIndex = cipCardEntryIndex;
      
      sprintf(cardName, "CIP CARD SLOT %d", cipCardEntryIndex);
      InitOctetString(&cipCardEntryName, (unsigned char *)&cardName[0], 
		      strlen(cardName));
      cipCardEntryData.cipCardEntryName = &cipCardEntryName;
      
      cipCardEntryData.cipCardEntryTotalMemory = 
	(cip->dram_size + cip->sram_size) / 1024;
      
      cipCardEntryData.cipCardEntryFreeMemory = 
	(cip->dram_free + cip->sram_free) / 1024;
      
      cipCardEntryData.cipCardEntryCpuUtilization = cip->cpu_util;
      
      /* 
       * For CPU Load and DMA Load metrics, the cip will return a 
       * value above 100% if the metrics are not available (ie: the cip
       * interfaces are shut down).  We will return a value of 0% for
       * these cases.
       */
      cipCardEntryData.cipCardEntryCpuLoad1m = 
	  (cip->cip_cpu_load.one_min <= 100) ?
	      cip->cip_cpu_load.one_min : 0;
      
      cipCardEntryData.cipCardEntryCpuLoad5m = 
	  (cip->cip_cpu_load.five_min <= 100) ? 
	      cip->cip_cpu_load.five_min : 0;
      
      cipCardEntryData.cipCardEntryCpuLoad60m =
	  (cip->cip_cpu_load.sixty_min <= 100) ?  
	      cip->cip_cpu_load.sixty_min : 0;
      
      cipCardEntryData.cipCardEntryDmaLoad1m = 
	  (cip->cip_dma_load.one_min <= 100) ?
	      cip->cip_dma_load.one_min : 0;
      
      cipCardEntryData.cipCardEntryDmaLoad5m = 
	  (cip->cip_dma_load.five_min <= 100) ?
	      cip->cip_dma_load.five_min : 0;
      
      cipCardEntryData.cipCardEntryDmaLoad60m = 
	  (cip->cip_dma_load.sixty_min <= 100) ?
	      cip->cip_dma_load.sixty_min : 0;
      
      cipCardEntryData.cipCardEntryTimeSinceLastReset =
         cip_timeSinceLastReset(cip);
      
      get_cip_version(cipCardEntryIndex, &cip_version);
      cipCardEntryData.cipCardEntryMajorSwRevisionNr = 
	  cip_version.sw_version_major;
      cipCardEntryData.cipCardEntryMinorSwRevisionNr =
	  cip_version.sw_version_minor;
      cipCardEntryData.cipCardEntryMajorHwRevisionNr = 
	  cip_version.hw_version_major;
      cipCardEntryData.cipCardEntryMinorHwRevisionNr = 
	  cip_version.hw_version_minor;

      resultP = &cipCardEntryData;
    }
    return resultP;
}

#ifdef SETS
int
k_cipCardEntry_set(data, contextInfo, function)
    cipCardEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

cipCardDaughterBoardEntry_t *
k_cipCardDaughterBoardEntry_get(serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
{
    static cipCardDaughterBoardEntry_t cipCardDaughterBoardEntryData;
    cipCardDaughterBoardEntry_t *resultP = NULL;
    cip_idb_infotype *cip = NULL;
    int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

    int last_stat = 0;
    int next_stat = 0;

    cip = cip_interfaceInfoGet(&cipCardEntryIndex, &cipCardDtrBrdIndex, getType);
    
    if (cip) {
      /* change from millisec to TimeTicks (.01 second units) and round */
      last_stat = ELAPSED_TIME(cip->last_love_letter);
      if (last_stat <= (cip_stats_interval * ONESEC))  {
        last_stat = (last_stat + 5) / 10;
        next_stat = (cip_stats_interval * ONESEC/10) - last_stat;
      } else  {
        /* interface not talking: set next/last = 0 */
        last_stat = 0;
      }

      cipCardDaughterBoardEntryData.cipCardEntryIndex = cipCardEntryIndex;
      cipCardDaughterBoardEntryData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
      
      cipCardDaughterBoardEntryData.cipCardDtrBrdType  = 
        (cip->daughter_type == CIP_PCA_DAUGHTER) ?
        D_cipCardDtrBrdType_busAndTag : D_cipCardDtrBrdType_escon;
        
      cipCardDaughterBoardEntryData.cipCardDtrBrdStatus = 
	cip_daughterBoardStatus(cip) ?
	D_cipCardDtrBrdStatus_true : D_cipCardDtrBrdStatus_false;

      cipCardDaughterBoardEntryData.cipCardDtrBrdSignal = 
	cip_daughterBoardSignal(cip) ?
        D_cipCardDtrBrdSignal_true:D_cipCardDtrBrdSignal_false;

      cipCardDaughterBoardEntryData.cipCardDtrBrdOnline = 
	cip_daughterBoardOffline(cip) ? 
        D_cipCardDtrBrdOnline_false : D_cipCardDtrBrdOnline_true;

      cipCardDaughterBoardEntryData.implicitIncidents = 
	cip->link_incident[LIR_IMPL_INCIDENT];

      cipCardDaughterBoardEntryData.codeViolationErrors = 
	cip->link_incident[LIR_BER_EXCEEDED];
      
      cipCardDaughterBoardEntryData.linkFailureSignalOrSyncLoss = 
	cip->link_incident[LIR_LINK_FAILED];
      
      cipCardDaughterBoardEntryData.linkFailureNOSs = 
	cip->link_incident[LIR_NOS];
      
      cipCardDaughterBoardEntryData.linkFailureSequenceTimeouts = 
	cip->link_incident[LIR_SEQ_TO];
      
      cipCardDaughterBoardEntryData.linkFailureInvalidSequences = 
	cip->link_incident[LIR_INVALID_SEQ];
      
      cipCardDaughterBoardEntryData.linkIncidentTrapCause = 
        D_linkIncidentTrapCause_liOther;
      
      cipCardDaughterBoardEntryData.cipCardDtrBrdLastStat = last_stat;
      
      cipCardDaughterBoardEntryData.cipCardDtrBrdNextStat = next_stat;
      
      /* 
       * For Channel Adapter Load, the cip will return a 
       * value above 100% if the metrics are not available (ie: the cip
       * interfaces are shut down).  We will return a value of 0% for
       * these cases.
       */
      cipCardDaughterBoardEntryData.cipCardDtrBrdChannelLoad1m = 
	  (cip->cip_channel_load.one_min <= 100) ?
	      cip->cip_channel_load.one_min : 0;

      cipCardDaughterBoardEntryData.cipCardDtrBrdChannelLoad5m = 
	  (cip->cip_channel_load.five_min <= 100) ?
	      cip->cip_channel_load.five_min : 0;

      cipCardDaughterBoardEntryData.cipCardDtrBrdChannelLoad60m = 
	  (cip->cip_channel_load.sixty_min <= 100) ?
	      cip->cip_channel_load.sixty_min : 0;

      resultP = &cipCardDaughterBoardEntryData;
    }

    return resultP;
}

cipCardSubChannelEntry_t *
k_cipCardSubChannelEntry_get(serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
{
  static cipCardSubChannelEntry_t cipCardSubChannelEntryData;
  cipCardSubChannelEntry_t* resultP = NULL;
  cip_vctype *vc = NULL;
  int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
  long subchannel_path, subchannel_device;
  static unsigned char last_sense[2];
  static OctetString last_sense_OS;
  ulonglong nr, answer, divisor, addor;
  int i;
  
  index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
		       &subchannel_device);

  vc = cip_getSubchannel(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
			 &subchannel_path, &subchannel_device, getType);

  if (vc) {
    cipCardSubChannelEntryData.cipCardEntryIndex = cipCardEntryIndex;
    cipCardSubChannelEntryData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;


    cipCardSubChannelEntryData.cipCardSubChannelIndex = 
      path_device_to_index(subchannel_path, subchannel_device);
    
    cipCardSubChannelEntryData.cipCardSubChannelConnections = 
      vc->stats.connections;
    
    cipCardSubChannelEntryData.cipCardSubChannelCancels = vc->stats.cancels;
    
    cipCardSubChannelEntryData.cipCardSubChannelSelectiveResets = 
      vc->stats.sel_resets;
    
    cipCardSubChannelEntryData.cipCardSubChannelSystemResets = 
      vc->stats.system_resets;
    
    cipCardSubChannelEntryData.cipCardSubChannelDeviceErrors = 
      vc->stats.dev_errors;
    
    cipCardSubChannelEntryData.cipCardSubChannelWriteBlocksDropped = 
      cip_subchannelWriteBlocksDropped(vc);
    
    last_sense[0] = vc->last_sense & 0xFF00;
    last_sense[1] = vc->last_sense & 0x00FF;
    InitOctetString(&last_sense_OS, last_sense, sizeof(last_sense));
    cipCardSubChannelEntryData.cipCardSubChannelLastSenseData = &last_sense_OS;
    
    /* perform the long division of the 64 bit number by 500,000 to
       convert from 20ns to 1/100 seconds time unit */
    /* dividing ulonglong by 500000 produced link error */
    divisor = 500000;
    divisor <<= (13+32);  /* shift it left as far as it will go */
    
    /* addor = 1 << (13+32); this produces a compiler warning 
                             "left shift count >= width of type!" */
    addor = 1;
    addor <<= (13+32);
    nr = vc->last_sense_time;
    answer = 0;
    
    for (i = 13+32; i > 0; i--) {
      if (nr > divisor) {
        nr -= divisor;
        answer += addor;
      };
      divisor >>= 1;
      addor >>= 1;
    };
    
    cipCardSubChannelEntryData.cipCardSubChannelLastSenseDataTime = 
      snmp_sysUpTime() - answer;
    
#if 0
    /* replace the above division code with the following when 
       compiler/linker allows division of longlong
    */
    cipCardSubChannelEntryData.cipCardSubChannelLastSenseDataTime = 
      snmp_sysUpTime() - (vc->last_sense_time/500000);  
      /* 1/500000 is 20 ns to 1/100 secs conversion factor */
      /* dividing ulonglong by 500000 produced link error */
#endif

    cipCardSubChannelEntryData.cipCardSubChannelCuBusies = vc->stats.cu_busy;
    
    cipCardSubChannelEntryData.cipCardSubChannelCmdRetries = 
      vc->stats.cmd_retries;

    resultP = &cipCardSubChannelEntryData;

  };

  return resultP;
}

cipCardClawEntry_t *
k_cipCardClawEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;
{
  static cipCardClawEntry_t cipCardClawEntryData;
  cipCardClawEntry_t * resultP = NULL;

  cip_vctype *vc;
  long subchannel_path, subchannel_device;
  int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
  
  index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
		       &subchannel_device);
    
  if ((vc = cip_getSubchannelClaw(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
					&subchannel_path, &subchannel_device, 
					&cipCardClawIndex, getType)) != NULL) {
      cipCardClawEntryData.cipCardEntryIndex = cipCardEntryIndex;
      cipCardClawEntryData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
      cipCardClawEntryData.cipCardSubChannelIndex = 
	  path_device_to_index(subchannel_path, subchannel_device);
      cipCardClawEntryData.cipCardClawIndex = cipCardClawIndex;
      cipCardClawEntryData.cipCardClawConnected = 
	  (cip_getDataStats(vc, cipCardClawIndex)->connected) ?
	      D_cipCardClawConnected_true : D_cipCardClawConnected_false;
      resultP = &cipCardClawEntryData;
  }
  
  return resultP;
}

cipCardClawConfigEntry_t *
k_cipCardClawConfigEntry_get(serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;
{
  static cipCardClawConfigEntry_t cipCardClawConfigEntryData;
  cipCardClawConfigEntry_t * resultP = NULL;

  cip_vctype *vc, *vc_config;
  long broadcast;
  long subchannel_path, subchannel_device;
  int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
  
  static unsigned char path[2], device[2];
  static OctetString path_OS, device_OS, hostname_OS, hostappl_OS, cipname_OS, 
                     cipappl_OS;

  index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
		       &subchannel_device);
    
  if ((vc = cip_getSubchannelClawConfig(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
	    &subchannel_path, &subchannel_device, &cipCardClawIndex, getType))
      != NULL) {
    cipCardClawConfigEntryData.cipCardEntryIndex = cipCardEntryIndex;
    cipCardClawConfigEntryData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
    cipCardClawConfigEntryData.cipCardSubChannelIndex = 
      path_device_to_index(subchannel_path, subchannel_device);
    cipCardClawConfigEntryData.cipCardClawIndex = cipCardClawIndex;
    
    path[0] = vc->path >> 8;
    path[1] = vc->path & 0x00FF;
    InitOctetString(&path_OS, &path[0], sizeof(path));
    cipCardClawConfigEntryData.cipCardClawConfigPath = &path_OS;

    device[0] = vc->device >> 8;
    device[1] = vc->device & 0x00FF;
    InitOctetString(&device_OS, &device[0], sizeof(device));
    cipCardClawConfigEntryData.cipCardClawConfigDevice = &device_OS;

    /* if config info is not filled in for the vc, get it from its partner. */
    vc_config = vc;
    if (vc->hostname[0] == '\0' && vc->claw_pair) {
      vc_config = vc->claw_pair;
    };
    
    cipCardClawConfigEntryData.cipCardClawConfigIpAddr = vc_config->ipaddr;

    InitOctetString(&hostname_OS, vc_config->hostname, strlen(vc_config->hostname));
    cipCardClawConfigEntryData.cipCardClawConfigHostName = &hostname_OS;
    
    InitOctetString(&cipname_OS, vc_config->cipname, strlen(vc_config->cipname));
    cipCardClawConfigEntryData.cipCardClawConfigRouterName = &cipname_OS;

    InitOctetString(&hostappl_OS, vc_config->hostappl, strlen(vc_config->hostappl));
    cipCardClawConfigEntryData.cipCardClawConfigHostAppl = &hostappl_OS;

    InitOctetString(&cipappl_OS, vc_config->cipappl, strlen(vc_config->cipappl));
    cipCardClawConfigEntryData.cipCardClawConfigRouterAppl = &cipappl_OS;

    broadcast = (vc->flags & CIP_VC_BRDCST) ? D_cipCardClawConfigBroadcastEnable_true : 
                                              D_cipCardClawConfigBroadcastEnable_false;
    cipCardClawConfigEntryData.cipCardClawConfigBroadcastEnable = broadcast;
    
    cipCardClawConfigEntryData.cipCardClawConfigRowStatus = D_cipCardClawConfigRowStatus_active;

    resultP = &cipCardClawConfigEntryData;
  };
  return resultP;
}

#ifdef SETS
int
k_cipCardClawConfigEntry_set(data, contextInfo, function)
    cipCardClawConfigEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

cipCardClawDataXferStatsEntry_t *
k_cipCardClawDataXferStatsEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex, cipCardClawIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;
    long            cipCardClawIndex;
{
  static cipCardClawDataXferStatsEntry_t cipCardClawDataXferStatsEntryData;
  static UInt64 bytesRead_int64, bytesWitten_int64;
    
  cipCardClawDataXferStatsEntry_t * resultP = NULL;
  data_stats *data_statsP;
  cip_vctype *vc;
  long subchannel_path, subchannel_device;
  int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
  leveltype level;
  
  
  index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
		       &subchannel_device);
    
  vc = cip_getSubchannelClaw(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
         &subchannel_path, &subchannel_device, &cipCardClawIndex, getType);
  if (vc && (data_statsP = cip_getDataStats(vc, cipCardClawIndex)) != NULL) {
      
      cipCardClawDataXferStatsEntryData.cipCardEntryIndex = cipCardEntryIndex;
      cipCardClawDataXferStatsEntryData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
      
      cipCardClawDataXferStatsEntryData.cipCardSubChannelIndex =
	  path_device_to_index(subchannel_path, subchannel_device);
      
      cipCardClawDataXferStatsEntryData.cipCardClawIndex = cipCardClawIndex;
      
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsBlocksRead = 
	  data_statsP->blocks_read;
      
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsBlocksWritten = 
	  data_statsP->blocks_written;
      
      /*      
      * Net interrupts are disabled while copying to longlong so we 
      * don't get clobbered by a new love letter.
      */
      level = raise_interrupt_level(NETS_DISABLE);
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsBytesRead = 
	  data_statsP->bytes_read_low;
      
      bytesRead_int64.big_end = data_statsP->bytes_read_high;
      bytesRead_int64.little_end = data_statsP->bytes_read_low;
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsHCBytesRead =
	  &bytesRead_int64;
      
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsBytesWritten = 
	  data_statsP->bytes_written_low;
      
      bytesWitten_int64.big_end = data_statsP->bytes_written_high;
      bytesWitten_int64.little_end = data_statsP->bytes_written_low;
      cipCardClawDataXferStatsEntryData.cipCardClawDataXferStatsHCBytesWritten = 
	  &bytesWitten_int64;
      
      reset_interrupt_level(level);
      
      cipCardClawDataXferStatsEntryData.
	  cipCardClawDataXferStatsReadBlocksDropped = 
	      data_statsP->read_blocks_dropped;
      
      cipCardClawDataXferStatsEntryData.
	  cipCardClawDataXferStatsWriteBlocksDropped = 
	      data_statsP->write_blocks_dropped;
      
    cipCardClawDataXferStatsEntryData.
	cipCardClawDataXferStatsBufferGetRetryCount = 
	    data_statsP->failed_memd_buf;
      
      resultP = &cipCardClawDataXferStatsEntryData;
  }
  
  return resultP;
}

cipCardApplicationEntry_t *
k_cipCardApplicationEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, int searchType, long cipCardEntryIndex, OctetString *cipCardApplicationNameIndex)
{

    static OctetString cipCardApplicationCompileInfo;
    static OctetString cipCardApplicationName;
    static cipCardApplicationEntry_t cipCardApplicationEntryData;
    int i;
    static char cip_appl_name[32];
    static cip_appl_version_t cip_appl_version;

    cipCardApplicationEntry_t *result_p = NULL;
    int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

    strncpy(cip_appl_version.name,cipCardApplicationNameIndex->octet_ptr, 32);
    if (get_cip_appl_version(getType, &cipCardEntryIndex, 
			     &cip_appl_version)) {

	strncpy(cip_appl_name, cip_appl_version.name,32);
	for (i = strlen(cip_appl_name); i < 32; i++)
	    cip_appl_name[i] = '\0';
	InitOctetString(&cipCardApplicationName, cip_appl_name, 32);

	cipCardApplicationEntryData.cipCardApplicationNameIndex = 
	    &cipCardApplicationName;

	cipCardApplicationEntryData.cipCardApplicationRevision = 
	    cip_appl_version.revision;

	InitOctetString(&cipCardApplicationCompileInfo, 
			cip_appl_version.compile_info, 
			strlen(cip_appl_version.compile_info));

	cipCardApplicationEntryData.cipCardApplicationCompileInfo = 
	       &cipCardApplicationCompileInfo;
	cipCardApplicationEntryData.cipCardEntryIndex = 
	    cipCardEntryIndex;
	result_p = &cipCardApplicationEntryData;
    }
    return result_p;
}

/* Length and OID definitions for the cipCardDtrBrdLinkFailureEvent
 */
 

static const OID enterprise_cardDtrBrdLinkFailureTrap = {LNcipCardDtrBrdLinkFailure,
                                          (ulong *)IDcipCardDtrBrdLinkFailure};
 
#define CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT 3
#define CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_NUM 1

static const OID  varbinds_CardDtrBrdLinkFailure[CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT] = 
{ 
     OIDconversion(cipCardDtrBrdStatus),
     OIDconversion(cipCardDtrBrdSignal),
     OIDconversion(linkIncidentTrapCause)
};

/* 
 * cipCipCardDtrBrdLinkFailure - fills in the SNMP info for
 *                         cipCardLinkFailure trap
 */
void cipCardDtrBrdLinkFailure ( cip_linktype *clf_data,
                             ushort code)
{
  int           idx;
  OID           indexOID[CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT];
  /* the index should contain the which field and the slot of the dtr brd */
  ulong         index[2];
  void *        dataP[CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT];
  index[0] = clf_data->slot; /*the slot field*/
  index[1] = clf_data->which; /* which dtr brd */
  /* Fill in the indexing information */
  for (idx = 0; idx < CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT; idx++) {
       indexOID[idx].length = 2;
       indexOID[idx].oid_ptr = index;
  }
  /* Fill in the data pointers */ 
  dataP[0] = &clf_data->dtr_brd_status;
  dataP[1] = &clf_data->dtr_brd_signal;
  dataP[2] = &clf_data->link_incident_cause;
  do_channel_trap(varbinds_CardDtrBrdLinkFailure, &enterprise_cardDtrBrdLinkFailureTrap, 
                 trap_cardDtrBrdLinkFailure, indexOID, 
                 CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_VBCOUNT, ENTERPRISE_TRAP, 
                 CIP_CARD_DTR_BRD_LINK_FAILURE_TRAP_NUM, dataP);
}

static void cip_chantrap ( ushort code, cip_linktype *chan_data)
{
    switch (code) {
       case CARD_LINK_FAILURE_EVENT:
            if (chan_link_fail_traps_enabled){
                cipCardDtrBrdLinkFailure(chan_data, code);
            }
            break;
       default:
            break;
    }
}

/*
 * channelmib subsystem header
 */

#define MAJVERSION_channelmib 1
#define MINVERSION_channelmib 0
#define EDITVERSION_channelmib 0

SUBSYS_HEADER(channelmib,
              MAJVERSION_channelmib,
              MINVERSION_channelmib,
              EDITVERSION_channelmib,
              init_channelmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
