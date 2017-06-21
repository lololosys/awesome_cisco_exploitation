/* $Id: sr_srbmib.c,v 3.5.28.7 1996/07/18 20:29:38 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/sr_srbmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * This is rfc1525
 * This was made with 
 *	mibcomp -g dot1dPortPair -g dot1dSr -I rfc1213-mib.def srbmib.my
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_srbmib.c,v $
 * Revision 3.5.28.7  1996/07/18  20:29:38  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.5.28.6  1996/07/09  06:05:58  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.5.28.5  1996/07/03  20:43:57  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.28.4  1996/07/01  18:46:33  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.28.3  1996/05/17  12:14:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.14.3  1996/04/26  15:23:36  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.14.2  1996/04/26  07:58:52  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.14.1  1996/04/03  21:59:43  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.28.2  1996/03/26  21:22:55  jjohnson
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 * Finish bstewart's commits which didn't make the branch pull
 * Branch: California_branch
 *
 * Revision 3.5.28.1  1996/03/18  22:11:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/13  02:02:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5  1996/01/18  15:53:09  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/14  08:28:05  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  18:56:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:31:38  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:02:10  hampton
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

#include "sr_srbmib.h"
#include "sr_srbmib-mib.h"
#include "interface_generic.h"
#include "packet.h"
#include "ieee.h"
#include "../if/rif.h"
#include "srb_sb_private.h"


static hwidbtype * srb_dot1d_idb(long *port, int old_next);


void
init_srbmib (subsystype *subsys)
{
    load_mib(srbmib_OidList, srbmib_OidListNum);
    load_oid(srbmib_oid_table);
}


/*---------------------------------------------------------------------
 * Retrieve data from the dot1dSr family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
#ifdef NOT_YET

VarBind        *
dot1dSr_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    dot1dSr_t      *data=NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot1dSr_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot1dSrBridgeLfMode
      case I_dot1dSrBridgeLfMode:
	dp = &data->dot1dSrBridgeLfMode;
	break;
#endif				       /* I_dot1dSrBridgeLfMode */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#endif  /* NOT_YET */

#ifdef SETS

#ifdef NOT_YET
/*----------------------------------------------------------------------
 * Free the dot1dSr data object.
 *---------------------------------------------------------------------*/
static void
dot1dSr_free(dot1dSr_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dot1dSr set/undo
 *---------------------------------------------------------------------*/
static int
dot1dSr_cleanup(doList_t *trash)
{
    dot1dSr_free(trash->data);
#ifdef SR_SNMPv2
    dot1dSr_free(trash->undodata);
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
dot1dSr_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    /*VarBind        *vb = (VarBind *) NULL;*/
    int             found;
    /*int             carry = 0;*/
    /*dot1dSr_t      *dot1dSr;*/

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
	if ((dp->data = malloc(sizeof(dot1dSr_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dot1dSr_t));

	dp->setMethod = dot1dSr_set;
	dp->cleanupMethod = dot1dSr_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

    }
    switch (object->nominator) {

#ifdef I_dot1dSrBridgeLfMode
      case I_dot1dSrBridgeLfMode:

	SET_VALID(I_dot1dSrBridgeLfMode, ((dot1dSr_t *) (dp->data))->valid);

	((dot1dSr_t *) (dp->data))->dot1dSrBridgeLfMode = value->sl_value;
	break;
#endif				       /* I_dot1dSrBridgeLfMode */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dot1dSr_test)\n"));
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
dot1dSr_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_dot1dSr_set((dot1dSr_t *) (doCur->data),
			  contextInfo, doCur->state));
}

#endif				       /* SETS */
#endif  /* NOT_YET */


/*---------------------------------------------------------------------
 * Retrieve data from the dot1dSrPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dot1dSrPortEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dot1dSrPortEntry_t *data=NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dot1dSrPort;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &dot1dSrPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot1dSrPortEntry_get(serialNum, contextInfo, arg, searchType, dot1dSrPort)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dot1dSrPort;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot1dSrPort
      case I_dot1dSrPort:
	dp = &data->dot1dSrPort;
	break;
#endif				       /* I_dot1dSrPort */

#ifdef I_dot1dSrPortHopCount
      case I_dot1dSrPortHopCount:
	dp = &data->dot1dSrPortHopCount;
	break;
#endif				       /* I_dot1dSrPortHopCount */

#ifdef I_dot1dSrPortLocalSegment
      case I_dot1dSrPortLocalSegment:
	dp = &data->dot1dSrPortLocalSegment;
	break;
#endif				       /* I_dot1dSrPortLocalSegment */

#ifdef I_dot1dSrPortBridgeNum
      case I_dot1dSrPortBridgeNum:
	dp = &data->dot1dSrPortBridgeNum;
	break;
#endif				       /* I_dot1dSrPortBridgeNum */

#ifdef I_dot1dSrPortTargetSegment
      case I_dot1dSrPortTargetSegment:
	dp = &data->dot1dSrPortTargetSegment;
	break;
#endif				       /* I_dot1dSrPortTargetSegment */

#ifdef I_dot1dSrPortLargestFrame
      case I_dot1dSrPortLargestFrame:
	dp = &data->dot1dSrPortLargestFrame;
	break;
#endif				       /* I_dot1dSrPortLargestFrame */

#ifdef I_dot1dSrPortSTESpanMode
      case I_dot1dSrPortSTESpanMode:
	dp = &data->dot1dSrPortSTESpanMode;
	break;
#endif				       /* I_dot1dSrPortSTESpanMode */

#ifdef I_dot1dSrPortSpecInFrames
      case I_dot1dSrPortSpecInFrames:
	dp = &data->dot1dSrPortSpecInFrames;
	break;
#endif				       /* I_dot1dSrPortSpecInFrames */

#ifdef I_dot1dSrPortSpecOutFrames
      case I_dot1dSrPortSpecOutFrames:
	dp = &data->dot1dSrPortSpecOutFrames;
	break;
#endif				       /* I_dot1dSrPortSpecOutFrames */

#ifdef I_dot1dSrPortApeInFrames
      case I_dot1dSrPortApeInFrames:
	dp = &data->dot1dSrPortApeInFrames;
	break;
#endif				       /* I_dot1dSrPortApeInFrames */

#ifdef I_dot1dSrPortApeOutFrames
      case I_dot1dSrPortApeOutFrames:
	dp = &data->dot1dSrPortApeOutFrames;
	break;
#endif				       /* I_dot1dSrPortApeOutFrames */

#ifdef I_dot1dSrPortSteInFrames
      case I_dot1dSrPortSteInFrames:
	dp = &data->dot1dSrPortSteInFrames;
	break;
#endif				       /* I_dot1dSrPortSteInFrames */

#ifdef I_dot1dSrPortSteOutFrames
      case I_dot1dSrPortSteOutFrames:
	dp = &data->dot1dSrPortSteOutFrames;
	break;
#endif				       /* I_dot1dSrPortSteOutFrames */

#ifdef I_dot1dSrPortSegmentMismatchDiscards
      case I_dot1dSrPortSegmentMismatchDiscards:
	dp = &data->dot1dSrPortSegmentMismatchDiscards;
	break;
#endif				       /* I_dot1dSrPortSegmentMismatchDiscards
				        *  */

#ifdef I_dot1dSrPortDuplicateSegmentDiscards
      case I_dot1dSrPortDuplicateSegmentDiscards:
	dp = &data->dot1dSrPortDuplicateSegmentDiscards;
	break;
#endif				       /* I_dot1dSrPortDuplicateSegmentDiscard
				        * s */

#ifdef I_dot1dSrPortHopCountExceededDiscards
      case I_dot1dSrPortHopCountExceededDiscards:
	dp = &data->dot1dSrPortHopCountExceededDiscards;
	break;
#endif				       /* I_dot1dSrPortHopCountExceededDiscard
				        * s */

#ifdef I_dot1dSrPortDupLanIdOrTreeErrors
      case I_dot1dSrPortDupLanIdOrTreeErrors:
	dp = &data->dot1dSrPortDupLanIdOrTreeErrors;
	break;
#endif				       /* I_dot1dSrPortDupLanIdOrTreeErrors */

#ifdef I_dot1dSrPortLanIdMismatches
      case I_dot1dSrPortLanIdMismatches:
	dp = &data->dot1dSrPortLanIdMismatches;
	break;
#endif				       /* I_dot1dSrPortLanIdMismatches */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dot1dSrPortEntry data object.
 *---------------------------------------------------------------------*/
static void
dot1dSrPortEntry_free(dot1dSrPortEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dot1dSrPortEntry set/undo
 *---------------------------------------------------------------------*/
static int
dot1dSrPortEntry_cleanup(doList_t *trash)
{
    dot1dSrPortEntry_free(trash->data);
#ifdef SR_SNMPv2
    dot1dSrPortEntry_free(trash->undodata);
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
dot1dSrPortEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    /*VarBind        *vb = (VarBind *) NULL;*/
    int             found;
    int             carry = 0;
    dot1dSrPortEntry_t *dot1dSrPortEntry;
    long            dot1dSrPort;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dot1dSrPort, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dot1dSrPortEntry = k_dot1dSrPortEntry_get(-1, contextInfo, object->nominator, EXACT, dot1dSrPort);

#ifndef dot1dSrPortEntry_READ_CREATE

    if (dot1dSrPortEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dot1dSrPortEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dot1dSrPortEntry_set) &&
	    (((dot1dSrPortEntry_t *) (dp->data)) != NULL) &&
	(((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPort == dot1dSrPort)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dot1dSrPortEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dot1dSrPortEntry_t));

	dp->setMethod = dot1dSrPortEntry_set;
	dp->cleanupMethod = dot1dSrPortEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dot1dSrPort, ((dot1dSrPortEntry_t *) (dp->data))->valid);
	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPort = dot1dSrPort;

#ifdef dot1dSrPortEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dot1dSrPortEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dot1dSrPortEntry, sizeof(dot1dSrPortEntry_t));

	    SET_ALL_VALID(((dot1dSrPortEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dot1dSrPortEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dot1dSrPortHopCount
      case I_dot1dSrPortHopCount:

	SET_VALID(I_dot1dSrPortHopCount, ((dot1dSrPortEntry_t *) (dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortHopCount = value->sl_value;
	break;
#endif				       /* I_dot1dSrPortHopCount */

#ifdef I_dot1dSrPortLocalSegment
      case I_dot1dSrPortLocalSegment:

	SET_VALID(I_dot1dSrPortLocalSegment, ((dot1dSrPortEntry_t *) (dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortLocalSegment = value->sl_value;
	break;
#endif				       /* I_dot1dSrPortLocalSegment */

#ifdef I_dot1dSrPortBridgeNum
      case I_dot1dSrPortBridgeNum:

	SET_VALID(I_dot1dSrPortBridgeNum, ((dot1dSrPortEntry_t *) (dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortBridgeNum = value->sl_value;
	break;
#endif				       /* I_dot1dSrPortBridgeNum */

#ifdef I_dot1dSrPortTargetSegment
      case I_dot1dSrPortTargetSegment:

	SET_VALID(I_dot1dSrPortTargetSegment, ((dot1dSrPortEntry_t *) (dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortTargetSegment = value->sl_value;
	break;
#endif				       /* I_dot1dSrPortTargetSegment */

#ifdef I_dot1dSrPortLargestFrame
      case I_dot1dSrPortLargestFrame:

	return (NOT_WRITABLE_ERROR);    /* currently unsupported */

	SET_VALID(I_dot1dSrPortLargestFrame, ((dot1dSrPortEntry_t *) 
		(dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortLargestFrame = 
		value->sl_value;
	break;
#endif				       /* I_dot1dSrPortLargestFrame */

#ifdef I_dot1dSrPortSTESpanMode
      case I_dot1dSrPortSTESpanMode:
	if ((value->sl_value != D_dot1dSrPortSTESpanMode_auto_span) &&
	    (value->sl_value != D_dot1dSrPortSTESpanMode_disabled) &&
	    (value->sl_value != D_dot1dSrPortSTESpanMode_forced))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dot1dSrPortSTESpanMode, ((dot1dSrPortEntry_t *) 
		(dp->data))->valid);

	((dot1dSrPortEntry_t *) (dp->data))->dot1dSrPortSTESpanMode = 
		value->sl_value;
	break;
#endif				       /* I_dot1dSrPortSTESpanMode */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dot1dSrPortEntry_test)\n"));
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
dot1dSrPortEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_dot1dSrPortEntry_set((dot1dSrPortEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dot1dPortPair family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
#ifdef NOT_YET

VarBind        *
dot1dPortPair_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    dot1dPortPair_t *data=NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot1dPortPair_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot1dPortPairTableSize
      case I_dot1dPortPairTableSize:
	dp = &data->dot1dPortPairTableSize;
	break;
#endif				       /* I_dot1dPortPairTableSize */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}
#endif  /* NOT_YET */

/*---------------------------------------------------------------------
 * Retrieve data from the dot1dPortPairEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
#ifdef NOT_YET

VarBind        *
dot1dPortPairEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dot1dPortPairEntry_t *data=NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dot1dPortPairLowPort;
    long            dot1dPortPairHighPort;


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

    if ((InstToInt(incoming, 1 + object->oid.length, &dot1dPortPairHighPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dot1dPortPairLowPort, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot1dPortPairEntry_get(serialNum, contextInfo, arg, searchType, dot1dPortPairLowPort, dot1dPortPairHighPort)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dot1dPortPairLowPort;
	inst.oid_ptr[1] = data->dot1dPortPairHighPort;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot1dPortPairLowPort
      case I_dot1dPortPairLowPort:
	dp = &data->dot1dPortPairLowPort;
	break;
#endif				       /* I_dot1dPortPairLowPort */

#ifdef I_dot1dPortPairHighPort
      case I_dot1dPortPairHighPort:
	dp = &data->dot1dPortPairHighPort;
	break;
#endif				       /* I_dot1dPortPairHighPort */

#ifdef I_dot1dPortPairBridgeNum
      case I_dot1dPortPairBridgeNum:
	dp = &data->dot1dPortPairBridgeNum;
	break;
#endif				       /* I_dot1dPortPairBridgeNum */

#ifdef I_dot1dPortPairBridgeState
      case I_dot1dPortPairBridgeState:
	dp = &data->dot1dPortPairBridgeState;
	break;
#endif				       /* I_dot1dPortPairBridgeState */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}
#endif  /* NOT_YET */


#ifdef NOT_YET
/*----------------------------------------------------------------------
 * Free the dot1dPortPairEntry data object.
 *---------------------------------------------------------------------*/
static void
dot1dPortPairEntry_free(dot1dPortPairEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dot1dPortPairEntry set/undo
 *---------------------------------------------------------------------*/
static int
dot1dPortPairEntry_cleanup(doList_t *trash)
{
    dot1dPortPairEntry_free(trash->data);
#ifdef SR_SNMPv2
    dot1dPortPairEntry_free(trash->undodata);
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
dot1dPortPairEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    /*VarBind        *vb = (VarBind *) NULL;*/
    int             found;
    int             carry = 0;
    dot1dPortPairEntry_t *dot1dPortPairEntry;
    long            dot1dPortPairLowPort;
    long            dot1dPortPairHighPort;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 2) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dot1dPortPairLowPort, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dot1dPortPairHighPort, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dot1dPortPairEntry = k_dot1dPortPairEntry_get(-1, contextInfo, object->nominator, EXACT, dot1dPortPairLowPort, dot1dPortPairHighPort);

#ifndef dot1dPortPairEntry_READ_CREATE

    if (dot1dPortPairEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dot1dPortPairEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dot1dPortPairEntry_set) &&
	    (((dot1dPortPairEntry_t *) (dp->data)) != NULL) &&
	    (((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairLowPort == dot1dPortPairLowPort) &&
	    (((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairHighPort == dot1dPortPairHighPort)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dot1dPortPairEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dot1dPortPairEntry_t));

	dp->setMethod = dot1dPortPairEntry_set;
	dp->cleanupMethod = dot1dPortPairEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dot1dPortPairLowPort, ((dot1dPortPairEntry_t *) (dp->data))->valid);
	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairLowPort = dot1dPortPairLowPort;

	SET_VALID(I_dot1dPortPairHighPort, ((dot1dPortPairEntry_t *) (dp->data))->valid);
	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairHighPort = dot1dPortPairHighPort;

#ifdef dot1dPortPairEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dot1dPortPairEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dot1dPortPairEntry, sizeof(dot1dPortPairEntry_t));

	    SET_ALL_VALID(((dot1dPortPairEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dot1dPortPairEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dot1dPortPairLowPort
      case I_dot1dPortPairLowPort:

	SET_VALID(I_dot1dPortPairLowPort, ((dot1dPortPairEntry_t *) (dp->data))->valid);

	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairLowPort = value->sl_value;
	break;
#endif				       /* I_dot1dPortPairLowPort */

#ifdef I_dot1dPortPairHighPort
      case I_dot1dPortPairHighPort:

	SET_VALID(I_dot1dPortPairHighPort, ((dot1dPortPairEntry_t *) (dp->data))->valid);

	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairHighPort = value->sl_value;
	break;
#endif				       /* I_dot1dPortPairHighPort */

#ifdef I_dot1dPortPairBridgeNum
      case I_dot1dPortPairBridgeNum:

	SET_VALID(I_dot1dPortPairBridgeNum, ((dot1dPortPairEntry_t *) (dp->data))->valid);

	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairBridgeNum = value->sl_value;
	break;
#endif				       /* I_dot1dPortPairBridgeNum */

#ifdef I_dot1dPortPairBridgeState
      case I_dot1dPortPairBridgeState:

	SET_VALID(I_dot1dPortPairBridgeState, ((dot1dPortPairEntry_t *) (dp->data))->valid);

	((dot1dPortPairEntry_t *) (dp->data))->dot1dPortPairBridgeState = value->sl_value;
	break;
#endif				       /* I_dot1dPortPairBridgeState */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dot1dPortPairEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);
}

#endif  /* NOT_YET */

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
#ifdef NOT_YET
int
dot1dPortPairEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_dot1dPortPairEntry_set((dot1dPortPairEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

dot1dSr_t      *
k_dot1dSr_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static dot1dSr_t dot1dSrData;

    dot1dSrData.dot1dSrBridgeLfMode =;	/* unsupported */

    return (&dot1dSrData);
}

#ifdef SETS
int
k_dot1dSr_set(
    dot1dSr_t      *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

#endif				       /* SETS */
#endif				       /* NOT_YET */

dot1dSrPortEntry_t *
k_dot1dSrPortEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dot1dSrPort)
{
    static dot1dSrPortEntry_t dot1dSrPortEntryData;
    register hwidbtype *idb;
    srb_sb_t *srb_sb;

    idb = srb_dot1d_idb(&dot1dSrPort, searchType);
    if (idb == NULL)
	return(NULL);

    srb_sb = srb_get_sb_inline(idb->firstsw);
    if (!srb_sb) {
        return(NULL);
    }
    dot1dSrPortEntryData.dot1dSrPort = idb->firstsw->if_number;
    dot1dSrPortEntryData.dot1dSrPortHopCount = srb_sb->srb_max_hops;
    dot1dSrPortEntryData.dot1dSrPortLocalSegment = srb_sb->srb_thisring;
    dot1dSrPortEntryData.dot1dSrPortBridgeNum = srb_sb->srb_bridge_num;
    dot1dSrPortEntryData.dot1dSrPortTargetSegment = srb_sb->srb_targetring;
    dot1dSrPortEntryData.dot1dSrPortLargestFrame = RC_LF2052_VAL;

    if (!srb_sb->srb_spanning_explorer)
	dot1dSrPortEntryData.dot1dSrPortSTESpanMode = 
		D_dot1dSrPortSTESpanMode_disabled;
    else
	dot1dSrPortEntryData.dot1dSrPortSTESpanMode = 
		D_dot1dSrPortSTESpanMode_forced;

    dot1dSrPortEntryData.dot1dSrPortSpecInFrames = 
                srb_sb->srb_rx_count +
		idb->counters.inputs_fast[ACCT_PROTO_SRB] +
		idb->counters.inputs_auto[ACCT_PROTO_SRB];

    dot1dSrPortEntryData.dot1dSrPortSpecOutFrames = 
                srb_sb->srb_tx_nb_count +
		idb->counters.outputs_fast[ACCT_PROTO_SRB] +
		idb->counters.outputs_auto[ACCT_PROTO_SRB];

    dot1dSrPortEntryData.dot1dSrPortApeInFrames = srb_sb->srb_allrte_in;
    dot1dSrPortEntryData.dot1dSrPortApeOutFrames = srb_sb->srb_allrte_out;
    dot1dSrPortEntryData.dot1dSrPortSteInFrames = srb_sb->srb_span_in;
    dot1dSrPortEntryData.dot1dSrPortSteOutFrames = srb_sb->srb_span_out;
    dot1dSrPortEntryData.dot1dSrPortSegmentMismatchDiscards = 0;
    dot1dSrPortEntryData.dot1dSrPortDuplicateSegmentDiscards = 0;
    dot1dSrPortEntryData.dot1dSrPortHopCountExceededDiscards = 0;
    dot1dSrPortEntryData.dot1dSrPortDupLanIdOrTreeErrors = 0;
    dot1dSrPortEntryData.dot1dSrPortLanIdMismatches = 0;

    return (&dot1dSrPortEntryData);
}


#ifdef SETS
int
k_dot1dSrPortEntry_set(
    dot1dSrPortEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{
    register hwidbtype *idb;

    idb = srb_dot1d_idb(&data->dot1dSrPort, EXACT);
    if (!idb) {
	return(NULL);
    }

    srb_sb = srb_get_or_create_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return(NULL);
    }

    if (VALID(I_dot1dSrPortHopCount, data->valid)) {
	srb_sb->srb_max_hops = data->dot1dSrPortHopCount;
    }

    if (VALID(I_dot1dSrPortLocalSegment, data->valid)) {
	srb_sb->srb_thisring = data->dot1dSrPortLocalSegment;
    }

    if (VALID(I_dot1dSrPortBridgeNum, data->valid)) {
	srb_sb->srb_bridge_num = data->dot1dSrPortBridgeNum;
    }

    if (VALID(I_dot1dSrPortTargetSegment, data->valid)) {
	srb_sb->srb_targetring = data->dot1dSrPortTargetSegment;
    }

    if (VALID(I_dot1dSrPortLargestFrame, data->valid)) {
	/* unsupported */
    }

    if (VALID(I_dot1dSrPortSTESpanMode, data->valid)) {
	srb_sb->srb_spanning_explorer = data->dot1dSrPortSTESpanMode;
    }

    return (NO_ERROR);
}

#endif				       /* SETS */

#ifdef NOT_YET
dot1dPortPair_t *
k_dot1dPortPair_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static dot1dPortPair_t dot1dPortPairData;

    dot1dPortPairData.dot1dPortPairTableSize =;  /* unsupported */

    return (&dot1dPortPairData);
}
#endif				       /* NOT_YET */


#ifdef NOT_YET
dot1dPortPairEntry_t *
k_dot1dPortPairEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dot1dPortPairLowPort,
    long            dot1dPortPairHighPort)
{
	/* table unsupported */
    static dot1dPortPairEntry_t dot1dPortPairEntryData;

    dot1dPortPairEntryData.dot1dPortPairLowPort =;
    dot1dPortPairEntryData.dot1dPortPairHighPort =;
    dot1dPortPairEntryData.dot1dPortPairBridgeNum =;
    dot1dPortPairEntryData.dot1dPortPairBridgeState =;

    return (&dot1dPortPairEntryData);
}

#ifdef SETS
int
k_dot1dPortPairEntry_set(
    dot1dPortPairEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{
    return (GEN_ERROR);
}
#endif				       /* SETS */
#endif				       /* NOT_YET */


/*
 * get the idb for the token ring interface of interest.
 */
static hwidbtype * srb_dot1d_idb (long *port, int old_next)
{

    register hwidbtype *idb = NULL;
    boolean next;

    next = (old_next == NEXT);

    FOR_ALL_HWIDBS(idb) {
        idbtype *swidb = idb->firstsw;
        if (((*port == swidb->if_number) ||
             (next && *port <= swidb->if_number))
             && (idb->status & IDB_TR) && (srb_sb_get_thisring(swidb) != 0))
            break;
    }

    if (idb != NULL) {
        *port = idb->firstsw->if_number;
        return(idb);
    }
    return (NULL);
}

/*
 * srbmib subsystem header
 */

#define MAJVERSION_srbmib 1
#define MINVERSION_srbmib 0
#define EDITVERSION_srbmib 0

SUBSYS_HEADER(srbmib,
              MAJVERSION_srbmib,
              MINVERSION_srbmib,
              EDITVERSION_srbmib,
              init_srbmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: srb",
              NULL);
      
