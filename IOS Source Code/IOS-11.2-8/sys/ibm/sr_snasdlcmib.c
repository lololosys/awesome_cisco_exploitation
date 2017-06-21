/* $Id: sr_snasdlcmib.c,v 3.5.10.4 1996/07/01 18:44:41 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sr_snasdlcmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1994, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snasdlcmib.c,v $
 * Revision 3.5.10.4  1996/07/01  18:44:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/05/21  09:48:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/05/21  06:30:20  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.10.1  1996/03/18  20:12:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  06:53:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/13  01:23:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.1  1996/02/20  14:21:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/26  21:08:38  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1995/12/14  08:24:32  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  09:24:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:02:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/31  04:12:55  szdravko
 * CSCdi37829:  Two sdlc mib fields not maintained
 *
 * Revision 2.3  1995/07/17  07:31:59  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:25:52  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:48:44  hampton
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
#include "snmp_interface.h"   
#include "../snmp/snmp_registry.h"
#include "../snmp/ifmibapi.h"
#include "interface_private.h"   
#include "sdlc.h"        
#include "sdlc_registry.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_trap_fe.h"
#include "../snmp/sr_ifmibsupp.h"
#include "sr_snasdlcmib.h"
#include "sr_snasdlcmib-mib.h"


static const OID sdlcEnterprise = {LNsdlc, (ulong *)IDsdlc};
static const char lsStatusChangeOid[] = "sdlc.3.2";
static sdlc_data_block* GetSdbFromIndex(int searchType, long ifIndex, long sdlc_address, hwidbtype** hwidbP);
static void GenerateSDLCLSStatusChangeTrap(sdlc_data_block *sdb);

static void
init_snasdlcmib (subsystype *subsys)
{
    load_mib(snasdlcmib_OidList, snasdlcmib_OidListNum);
    load_oid(snasdlcmib_oid_table);
    register_snmp_trap(TRAP_SDLC, lsStatusChangeOid);
    reg_add_sdlc_ls_status_change(GenerateSDLCLSStatusChangeTrap,
				  "GenerateSDLCLSStatusChangeTrap");
}

static boolean 
IsThisHwidbSdlc (hwidbtype *hwidb, long* index2)
{
    return is_sdlc(hwidb);
}

static long
IsPointToPointOrMultipoint(hwidbtype *hwidb)
{
    int i;
    int count = 0;  

    for (i=SDLC_MIN_ADDRESS; i<=SDLC_MAX_ADDRESS ; i++)
        if (hwidb->sdlc_data[i] != NULL) 
           count++;

    if (count == 0)
        return 0;          /* Not point-to-point or multipoint */
    else if (count == 1)
        return D_sdlcPortOperTopology_pointToPoint;
    else 
        return D_sdlcPortOperTopology_multipoint;
}

/*---------------------------------------------------------------------

 * Retrieve data from the sdlcPortAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcPortAdminEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                        ContextInfo *contextInfo, int serialNum)
{     
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcPortAdminEntry_t *data = 0;
    unsigned long buffer[1];
    OID inst;
    int carry;
    long ifIndex;


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
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcPortAdminEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcPortAdminName
    case I_sdlcPortAdminName:
	dp = MakeOctetString (data->sdlcPortAdminName->octet_ptr, data->sdlcPortAdminName->length);
	break;
#endif /* I_sdlcPortAdminName */

#ifdef I_sdlcPortAdminRole
    case I_sdlcPortAdminRole:
	dp = &data->sdlcPortAdminRole;
	break;
#endif /* I_sdlcPortAdminRole */

#ifdef I_sdlcPortAdminType
    case I_sdlcPortAdminType:
	dp = &data->sdlcPortAdminType;
	break;
#endif /* I_sdlcPortAdminType */

#ifdef I_sdlcPortAdminTopology
    case I_sdlcPortAdminTopology:
	dp = &data->sdlcPortAdminTopology;
	break;
#endif /* I_sdlcPortAdminTopology */

#ifdef I_sdlcPortAdminISTATUS
    case I_sdlcPortAdminISTATUS:
	dp = &data->sdlcPortAdminISTATUS;
	break;
#endif /* I_sdlcPortAdminISTATUS */

#ifdef I_sdlcPortAdminACTIVTO
    case I_sdlcPortAdminACTIVTO:
	dp = &data->sdlcPortAdminACTIVTO;
	break;
#endif /* I_sdlcPortAdminACTIVTO */

#ifdef I_sdlcPortAdminPAUSE
    case I_sdlcPortAdminPAUSE:
	dp = &data->sdlcPortAdminPAUSE;
	break;
#endif /* I_sdlcPortAdminPAUSE */

#ifdef I_sdlcPortAdminSERVLIM
    case I_sdlcPortAdminSERVLIM:
	dp = &data->sdlcPortAdminSERVLIM;
	break;
#endif /* I_sdlcPortAdminSERVLIM */

#ifdef I_sdlcPortAdminSlowPollTimer
    case I_sdlcPortAdminSlowPollTimer:
	dp = &data->sdlcPortAdminSlowPollTimer;
	break;
#endif /* I_sdlcPortAdminSlowPollTimer */

    default:
	return ((VarBind *) NULL);

    }	/* switch */ 
    return (MakeVarBind (object, &inst, dp));

}
#ifdef SETS
/*----------------------------------------------------------------------
 * Free the sdlcPortAdminEntry data object.
 *---------------------------------------------------------------------*/ 
static void
sdlcPortAdminEntry_free (sdlcPortAdminEntry_t *data)
{
    if (data != NULL) {
	if (data->sdlcPortAdminName != NULL) {
	    FreeOctetString (data->sdlcPortAdminName);
	}
	free ((char *) data);
    }
}
/*----------------------------------------------------------------------

 * cleanup after sdlcPortAdminEntry set/undo
 *---------------------------------------------------------------------*/ 
static int
sdlcPortAdminEntry_cleanup (doList_t *trash)
{
    sdlcPortAdminEntry_free (trash->data);
#ifdef SR_SNMPv2
    sdlcPortAdminEntry_free (trash->undodata);
#endif /* SR_SNMPv2 */

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
sdlcPortAdminEntry_test (OID *incoming, ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    int instLength = incoming->length - object->oid.length;
    doList_t *dp;
/*    VarBind *vb = (VarBind *) NULL;
*/
    int found;
    int carry = 0;
    sdlcPortAdminEntry_t *sdlcPortAdminEntry;
    long ifIndex;

    /*
     * Validate the object instance
     *
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    sdlcPortAdminEntry = k_sdlcPortAdminEntry_get (-1, contextInfo, object->nominator, EXACT, ifIndex);

#ifndef sdlcPortAdminEntry_READ_CREATE

    if (sdlcPortAdminEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif 
    /* sdlcPortAdminEntry_READ_CREATE */
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */ 
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == sdlcPortAdminEntry_set) &&
	    (((sdlcPortAdminEntry_t *) (dp->data)) != NULL) &&
	    (((sdlcPortAdminEntry_t *) (dp->data))->ifIndex == ifIndex)) {

	    found = 1;
	    break;
	}
    } 
    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof (sdlcPortAdminEntry_t))) == NULL) {
	    DPRINTF ((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset (dp->data, 0, sizeof (sdlcPortAdminEntry_t));

	dp->setMethod = sdlcPortAdminEntry_set;
	dp->cleanupMethod = sdlcPortAdminEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID (I_ifIndex, ((sdlcPortAdminEntry_t *) (dp->data))->valid);
	((sdlcPortAdminEntry_t *) (dp->data))->ifIndex = ifIndex;

#ifdef sdlcPortAdminEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (sdlcPortAdminEntry != NULL) {
	    /* use existing values as default values */
	    memcpy (dp->data, sdlcPortAdminEntry, sizeof(sdlcPortAdminEntry_t));

	    SET_ALL_VALID (((sdlcPortAdminEntry_t *) (dp->data))->valid);

	    ((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminName =
		CloneOctetString (sdlcPortAdminEntry->sdlcPortAdminName);

	} else {
	    /* Fill in default values here */
        }
#endif /* sdlcPortAdminEntry_READ_CREATE */ 
    }
    switch (object->nominator) {

#ifdef I_sdlcPortAdminName
    case I_sdlcPortAdminName:

	SET_VALID (I_sdlcPortAdminName, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	if (((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminName != NULL) {
	    FreeOctetString (((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminName);
	}
	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminName =
	    MakeOctetString (value->os_value->octet_ptr, value->os_value->length);

	break;
#endif /* I_sdlcPortAdminName */

#ifdef I_sdlcPortAdminRole
    case I_sdlcPortAdminRole:

	SET_VALID (I_sdlcPortAdminRole, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminRole = value->sl_value;
	break;
#endif /* I_sdlcPortAdminRole */

#ifdef I_sdlcPortAdminType
    case I_sdlcPortAdminType:

	SET_VALID (I_sdlcPortAdminType, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminType = value->sl_value;
	break;
#endif /* I_sdlcPortAdminType */

#ifdef I_sdlcPortAdminTopology
    case I_sdlcPortAdminTopology:

	SET_VALID (I_sdlcPortAdminTopology, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminTopology = value->sl_value;
	break;
#endif /* I_sdlcPortAdminTopology */

#ifdef I_sdlcPortAdminISTATUS
    case I_sdlcPortAdminISTATUS:

	SET_VALID (I_sdlcPortAdminISTATUS, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminISTATUS = value->sl_value;
	break;
#endif /* I_sdlcPortAdminISTATUS */

#ifdef I_sdlcPortAdminACTIVTO
    case I_sdlcPortAdminACTIVTO:

	SET_VALID (I_sdlcPortAdminACTIVTO, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminACTIVTO = value->sl_value;
	break;
#endif /* I_sdlcPortAdminACTIVTO */

#ifdef I_sdlcPortAdminPAUSE
    case I_sdlcPortAdminPAUSE:

	SET_VALID (I_sdlcPortAdminPAUSE, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminPAUSE = value->sl_value;
	break;
#endif /* I_sdlcPortAdminPAUSE */

#ifdef I_sdlcPortAdminSERVLIM
    case I_sdlcPortAdminSERVLIM:

	SET_VALID (I_sdlcPortAdminSERVLIM, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminSERVLIM = value->sl_value;
	break;
#endif /* I_sdlcPortAdminSERVLIM */

#ifdef I_sdlcPortAdminSlowPollTimer
    case I_sdlcPortAdminSlowPollTimer:

	SET_VALID (I_sdlcPortAdminSlowPollTimer, ((sdlcPortAdminEntry_t *) (dp->data))->valid);

	((sdlcPortAdminEntry_t *) (dp->data))->sdlcPortAdminSlowPollTimer = value->sl_value;
	break;
#endif /* I_sdlcPortAdminSlowPollTimer */

    default:
	DPRINTF ((APALWAYS, "snmpd: Internal error (invalid nominator in sdlcPortAdminEntry_test)\n"));
	return (GEN_ERROR);

    }  /* switch */ 
    dp->state = ADD_MODIFY;

    return (NO_ERROR);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/ 
int
sdlcPortAdminEntry_set (doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return (k_sdlcPortAdminEntry_set ((sdlcPortAdminEntry_t *) (doCur->data),
				      contextInfo, doCur->state));
}
#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the sdlcPortOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcPortOperEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                       ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcPortOperEntry_t *data = 0;
    unsigned long buffer[1];
    OID inst;
    int carry;
    long ifIndex;

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
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcPortOperEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcPortOperName
    case I_sdlcPortOperName:
	dp = MakeOctetString (data->sdlcPortOperName->octet_ptr, data->sdlcPortOperName->length);
	break;
#endif /* I_sdlcPortOperName */

#ifdef I_sdlcPortOperRole
    case I_sdlcPortOperRole:
	dp = &data->sdlcPortOperRole;
	break;
#endif /* I_sdlcPortOperRole */

#ifdef I_sdlcPortOperType
    case I_sdlcPortOperType:
	dp = &data->sdlcPortOperType;
	break;
#endif /* I_sdlcPortOperType */

#ifdef I_sdlcPortOperTopology
    case I_sdlcPortOperTopology:
	dp = &data->sdlcPortOperTopology;
	break;
#endif /* I_sdlcPortOperTopology */

#ifdef I_sdlcPortOperISTATUS
    case I_sdlcPortOperISTATUS:
	dp = &data->sdlcPortOperISTATUS;
	break;
#endif /* I_sdlcPortOperISTATUS */

#ifdef I_sdlcPortOperACTIVTO
    case I_sdlcPortOperACTIVTO:
	dp = &data->sdlcPortOperACTIVTO;
	break;
#endif /* I_sdlcPortOperACTIVTO */

#ifdef I_sdlcPortOperPAUSE
    case I_sdlcPortOperPAUSE:
	dp = &data->sdlcPortOperPAUSE;
	break;
#endif /* I_sdlcPortOperPAUSE */

#ifdef I_sdlcPortOperSlowPollMethod
    case I_sdlcPortOperSlowPollMethod:
	dp = &data->sdlcPortOperSlowPollMethod;
	break;
#endif /* I_sdlcPortOperSlowPollMethod */

#ifdef I_sdlcPortOperSERVLIM
    case I_sdlcPortOperSERVLIM:
	dp = &data->sdlcPortOperSERVLIM;
	break;
#endif /* I_sdlcPortOperSERVLIM */

#ifdef I_sdlcPortOperSlowPollTimer
    case I_sdlcPortOperSlowPollTimer:
	dp = &data->sdlcPortOperSlowPollTimer;
	break;
#endif /* I_sdlcPortOperSlowPollTimer */

#ifdef I_sdlcPortOperLastModifyTime
    case I_sdlcPortOperLastModifyTime:
	dp = &data->sdlcPortOperLastModifyTime;
	break;
#endif /* I_sdlcPortOperLastModifyTime */

#ifdef I_sdlcPortOperLastFailTime
    case I_sdlcPortOperLastFailTime:
	dp = &data->sdlcPortOperLastFailTime;
	break;
#endif /* I_sdlcPortOperLastFailTime */

#ifdef I_sdlcPortOperLastFailCause
    case I_sdlcPortOperLastFailCause:
	dp = &data->sdlcPortOperLastFailCause;
	break;
#endif /* I_sdlcPortOperLastFailCause */

    default:
	return ((VarBind *) NULL);

    }  /* switch */ 
    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the sdlcPortStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcPortStatsEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                        ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcPortStatsEntry_t *data = 0;
    unsigned long buffer[1];
    OID inst;
    int carry;
    long ifIndex;


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
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcPortStatsEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcPortStatsPhysicalFailures
    case I_sdlcPortStatsPhysicalFailures:
	dp = &data->sdlcPortStatsPhysicalFailures;
	break;
#endif /* I_sdlcPortStatsPhysicalFailures */

#ifdef I_sdlcPortStatsInvalidAddresses
    case I_sdlcPortStatsInvalidAddresses:
	dp = &data->sdlcPortStatsInvalidAddresses;
	break;
#endif /* I_sdlcPortStatsInvalidAddresses */

#ifdef I_sdlcPortStatsDwarfFrames
    case I_sdlcPortStatsDwarfFrames:
	dp = &data->sdlcPortStatsDwarfFrames;
	break;
#endif /* I_sdlcPortStatsDwarfFrames */

#ifdef I_sdlcPortStatsPollsIn
    case I_sdlcPortStatsPollsIn:
	dp = &data->sdlcPortStatsPollsIn;
	break;
#endif /* I_sdlcPortStatsPollsIn */

#ifdef I_sdlcPortStatsPollsOut
    case I_sdlcPortStatsPollsOut:
	dp = &data->sdlcPortStatsPollsOut;
	break;
#endif /* I_sdlcPortStatsPollsOut */

#ifdef I_sdlcPortStatsPollRspsIn
    case I_sdlcPortStatsPollRspsIn:
	dp = &data->sdlcPortStatsPollRspsIn;
	break;
#endif /* I_sdlcPortStatsPollRspsIn */

#ifdef I_sdlcPortStatsPollRspsOut
    case I_sdlcPortStatsPollRspsOut:
	dp = &data->sdlcPortStatsPollRspsOut;
	break;
#endif /* I_sdlcPortStatsPollRspsOut */

#ifdef I_sdlcPortStatsLocalBusies
    case I_sdlcPortStatsLocalBusies:
	dp = &data->sdlcPortStatsLocalBusies;
	break;
#endif /* I_sdlcPortStatsLocalBusies */

#ifdef I_sdlcPortStatsRemoteBusies
    case I_sdlcPortStatsRemoteBusies:
	dp = &data->sdlcPortStatsRemoteBusies;
	break;
#endif /* I_sdlcPortStatsRemoteBusies */

#ifdef I_sdlcPortStatsIFramesIn
    case I_sdlcPortStatsIFramesIn:
	dp = &data->sdlcPortStatsIFramesIn;
	break;
#endif /* I_sdlcPortStatsIFramesIn */

#ifdef I_sdlcPortStatsIFramesOut
    case I_sdlcPortStatsIFramesOut:
	dp = &data->sdlcPortStatsIFramesOut;
	break;
#endif /* I_sdlcPortStatsIFramesOut */

#ifdef I_sdlcPortStatsOctetsIn
    case I_sdlcPortStatsOctetsIn:
	dp = &data->sdlcPortStatsOctetsIn;
	break;
#endif /* I_sdlcPortStatsOctetsIn */

#ifdef I_sdlcPortStatsOctetsOut
    case I_sdlcPortStatsOctetsOut:
	dp = &data->sdlcPortStatsOctetsOut;
	break;
#endif /* I_sdlcPortStatsOctetsOut */

#ifdef I_sdlcPortStatsProtocolErrs
    case I_sdlcPortStatsProtocolErrs:
	dp = &data->sdlcPortStatsProtocolErrs;
	break;
#endif /* I_sdlcPortStatsProtocolErrs */

#ifdef I_sdlcPortStatsActivityTOs
    case I_sdlcPortStatsActivityTOs:
	dp = &data->sdlcPortStatsActivityTOs;
	break;
#endif /* I_sdlcPortStatsActivityTOs */

#ifdef I_sdlcPortStatsRNRLIMITs
    case I_sdlcPortStatsRNRLIMITs:
	dp = &data->sdlcPortStatsRNRLIMITs;
	break;
#endif /* I_sdlcPortStatsRNRLIMITs */

#ifdef I_sdlcPortStatsRetriesExps
    case I_sdlcPortStatsRetriesExps:
	dp = &data->sdlcPortStatsRetriesExps;
	break;
#endif /* I_sdlcPortStatsRetriesExps */

#ifdef I_sdlcPortStatsRetransmitsIn
    case I_sdlcPortStatsRetransmitsIn:
	dp = &data->sdlcPortStatsRetransmitsIn;
	break;
#endif /* I_sdlcPortStatsRetransmitsIn */

#ifdef I_sdlcPortStatsRetransmitsOut
    case I_sdlcPortStatsRetransmitsOut:
	dp = &data->sdlcPortStatsRetransmitsOut;
	break;
#endif /* I_sdlcPortStatsRetransmitsOut */

    default:
	return ((VarBind *) NULL);

    }  /* switch */ 
    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the sdlcLSAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcLSAdminEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                      ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcLSAdminEntry_t *data = 0;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long ifIndex;
    long sdlcLSAddress;


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
    if ((InstToInt (incoming, 1 + object->oid.length, &sdlcLSAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcLSAdminEntry_get (serialNum, contextInfo, arg, searchType, ifIndex, sdlcLSAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->ifIndex;
	inst.oid_ptr[1] = data->sdlcLSAddress;
    }				       
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcLSAddress
    case I_sdlcLSAddress:
	dp = &data->sdlcLSAddress;
	break;
#endif /* I_sdlcLSAddress */

#ifdef I_sdlcLSAdminName
    case I_sdlcLSAdminName:
	dp = MakeOctetString (data->sdlcLSAdminName->octet_ptr, data->sdlcLSAdminName->length);
	break;
#endif /* I_sdlcLSAdminName */

#ifdef I_sdlcLSAdminState
    case I_sdlcLSAdminState:
	dp = &data->sdlcLSAdminState;
	break;
#endif /* I_sdlcLSAdminState */

#ifdef I_sdlcLSAdminISTATUS
    case I_sdlcLSAdminISTATUS:
	dp = &data->sdlcLSAdminISTATUS;
	break;
#endif /* I_sdlcLSAdminISTATUS */

#ifdef I_sdlcLSAdminMAXDATASend
    case I_sdlcLSAdminMAXDATASend:
	dp = &data->sdlcLSAdminMAXDATASend;
	break;
#endif /* I_sdlcLSAdminMAXDATASend */

#ifdef I_sdlcLSAdminMAXDATARcv
    case I_sdlcLSAdminMAXDATARcv:
	dp = &data->sdlcLSAdminMAXDATARcv;
	break;
#endif /* I_sdlcLSAdminMAXDATARcv */

#ifdef I_sdlcLSAdminREPLYTO
    case I_sdlcLSAdminREPLYTO:
	dp = &data->sdlcLSAdminREPLYTO;
	break;
#endif /* I_sdlcLSAdminREPLYTO */

#ifdef I_sdlcLSAdminMAXIN
    case I_sdlcLSAdminMAXIN:
	dp = &data->sdlcLSAdminMAXIN;
	break;
#endif /* I_sdlcLSAdminMAXIN */

#ifdef I_sdlcLSAdminMAXOUT
    case I_sdlcLSAdminMAXOUT:
	dp = &data->sdlcLSAdminMAXOUT;
	break;
#endif /* I_sdlcLSAdminMAXOUT */

#ifdef I_sdlcLSAdminMODULO
    case I_sdlcLSAdminMODULO:
	dp = &data->sdlcLSAdminMODULO;
	break;
#endif /* I_sdlcLSAdminMODULO */

#ifdef I_sdlcLSAdminRETRIESm
    case I_sdlcLSAdminRETRIESm:
	dp = &data->sdlcLSAdminRETRIESm;
	break;
#endif /* I_sdlcLSAdminRETRIESm */

#ifdef I_sdlcLSAdminRETRIESt
    case I_sdlcLSAdminRETRIESt:
	dp = &data->sdlcLSAdminRETRIESt;
	break;
#endif /* I_sdlcLSAdminRETRIESt */

#ifdef I_sdlcLSAdminRETRIESn
    case I_sdlcLSAdminRETRIESn:
	dp = &data->sdlcLSAdminRETRIESn;
	break;
#endif /* I_sdlcLSAdminRETRIESn */

#ifdef I_sdlcLSAdminRNRLIMIT
    case I_sdlcLSAdminRNRLIMIT:
	dp = &data->sdlcLSAdminRNRLIMIT;
	break;
#endif /* I_sdlcLSAdminRNRLIMIT */

#ifdef I_sdlcLSAdminDATMODE
    case I_sdlcLSAdminDATMODE:
	dp = &data->sdlcLSAdminDATMODE;
	break;
#endif /* I_sdlcLSAdminDATMODE */

#ifdef I_sdlcLSAdminGPoll
    case I_sdlcLSAdminGPoll:
	dp = &data->sdlcLSAdminGPoll;
	break;
#endif /* I_sdlcLSAdminGPoll */

#ifdef I_sdlcLSAdminSimRim
    case I_sdlcLSAdminSimRim:
	dp = &data->sdlcLSAdminSimRim;
	break;
#endif /* I_sdlcLSAdminSimRim */

#ifdef I_sdlcLSAdminXmitRcvCap
    case I_sdlcLSAdminXmitRcvCap:
	dp = &data->sdlcLSAdminXmitRcvCap;
	break;
#endif /* I_sdlcLSAdminXmitRcvCap */

#ifdef I_sdlcLSAdminRowStatus
    case I_sdlcLSAdminRowStatus:
	dp = &data->sdlcLSAdminRowStatus;
	break;
#endif /* I_sdlcLSAdminRowStatus */

    default:
	return ((VarBind *) NULL);

    }  /* switch */ 
    return (MakeVarBind (object, &inst, dp));

}
#ifdef SETS
/*----------------------------------------------------------------------
 * Free the sdlcLSAdminEntry data object.
 *---------------------------------------------------------------------*/ 
static void
sdlcLSAdminEntry_free (sdlcLSAdminEntry_t *data)
{
    if (data != NULL) {
	if (data->sdlcLSAdminName != NULL) {
	    FreeOctetString (data->sdlcLSAdminName);
	}
	free ((char *) data);
    }
}
/*----------------------------------------------------------------------

 * cleanup after sdlcLSAdminEntry set/undo
 *---------------------------------------------------------------------*/ 
static int
sdlcLSAdminEntry_cleanup (doList_t *trash)
{
    sdlcLSAdminEntry_free (trash->data);
#ifdef SR_SNMPv2
    sdlcLSAdminEntry_free (trash->undodata);
#endif /* SR_SNMPv2 */
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
sdlcLSAdminEntry_test (OID *incoming, ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    int instLength = incoming->length - object->oid.length;
    doList_t *dp;
/*    VarBind *vb = (VarBind *) NULL;
*/
    int found;
    int carry = 0;
    sdlcLSAdminEntry_t *sdlcLSAdminEntry;
    long ifIndex;
    long sdlcLSAddress;

    /*
     * Validate the object instance
     *
     */
    if (instLength != 2) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt (incoming, 1 + object->oid.length, &sdlcLSAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    sdlcLSAdminEntry = k_sdlcLSAdminEntry_get (-1, contextInfo, object->nominator, EXACT, ifIndex, sdlcLSAddress);

#ifndef sdlcLSAdminEntry_READ_CREATE

    if (sdlcLSAdminEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif /* sdlcLSAdminEntry_READ_CREATE */  
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */ 
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == sdlcLSAdminEntry_set) &&
	    (((sdlcLSAdminEntry_t *) (dp->data)) != NULL) &&
	    (((sdlcLSAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
	    (((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAddress == sdlcLSAddress)) {

	    found = 1;
	    break;
	}
    } 
    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof (sdlcLSAdminEntry_t))) == NULL) {
	    DPRINTF ((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset (dp->data, 0, sizeof (sdlcLSAdminEntry_t));

	dp->setMethod = sdlcLSAdminEntry_set;
	dp->cleanupMethod = sdlcLSAdminEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID (I_ifIndex, ((sdlcLSAdminEntry_t *) (dp->data))->valid);
	((sdlcLSAdminEntry_t *) (dp->data))->ifIndex = ifIndex;

	SET_VALID (I_sdlcLSAddress, ((sdlcLSAdminEntry_t *) (dp->data))->valid);
	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAddress = sdlcLSAddress;

#ifdef sdlcLSAdminEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (sdlcLSAdminEntry != NULL) {
	    /* use existing values as default values */
	    memcpy (dp->data, sdlcLSAdminEntry, sizeof(sdlcLSAdminEntry_t));

	    SET_ALL_VALID (((sdlcLSAdminEntry_t *) (dp->data))->valid);

	    ((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminName =
		CloneOctetString (sdlcLSAdminEntry->sdlcLSAdminName);

	} else {


	    /* Fill in default values here */


	}
#endif /* sdlcLSAdminEntry_READ_CREATE */ 
    }
    switch (object->nominator) {

#ifdef I_sdlcLSAddress
    case I_sdlcLSAddress:

	SET_VALID (I_sdlcLSAddress, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAddress = value->sl_value;
	break;
#endif /* I_sdlcLSAddress */

#ifdef I_sdlcLSAdminName
    case I_sdlcLSAdminName:

	SET_VALID (I_sdlcLSAdminName, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	if (((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminName != NULL) {
	    FreeOctetString (((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminName);
	}
	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminName =
	    MakeOctetString (value->os_value->octet_ptr, value->os_value->length);

	break;
#endif /* I_sdlcLSAdminName */

#ifdef I_sdlcLSAdminState
    case I_sdlcLSAdminState:

	SET_VALID (I_sdlcLSAdminState, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminState = value->sl_value;
	break;
#endif /* I_sdlcLSAdminState */

#ifdef I_sdlcLSAdminISTATUS
    case I_sdlcLSAdminISTATUS:

	SET_VALID (I_sdlcLSAdminISTATUS, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminISTATUS = value->sl_value;
	break;
#endif /* I_sdlcLSAdminISTATUS */

#ifdef I_sdlcLSAdminMAXDATASend
    case I_sdlcLSAdminMAXDATASend:

	SET_VALID (I_sdlcLSAdminMAXDATASend, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminMAXDATASend = value->sl_value;
	break;
#endif /* I_sdlcLSAdminMAXDATASend */

#ifdef I_sdlcLSAdminMAXDATARcv
    case I_sdlcLSAdminMAXDATARcv:

	SET_VALID (I_sdlcLSAdminMAXDATARcv, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminMAXDATARcv = value->sl_value;
	break;
#endif /* I_sdlcLSAdminMAXDATARcv */

#ifdef I_sdlcLSAdminREPLYTO
    case I_sdlcLSAdminREPLYTO:

	SET_VALID (I_sdlcLSAdminREPLYTO, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminREPLYTO = value->sl_value;
	break;
#endif /* I_sdlcLSAdminREPLYTO */

#ifdef I_sdlcLSAdminMAXIN
    case I_sdlcLSAdminMAXIN:

	SET_VALID (I_sdlcLSAdminMAXIN, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminMAXIN = value->sl_value;
	break;
#endif /* I_sdlcLSAdminMAXIN */

#ifdef I_sdlcLSAdminMAXOUT
    case I_sdlcLSAdminMAXOUT:

	SET_VALID (I_sdlcLSAdminMAXOUT, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminMAXOUT = value->sl_value;
	break;
#endif /* I_sdlcLSAdminMAXOUT */

#ifdef I_sdlcLSAdminMODULO
    case I_sdlcLSAdminMODULO:

	SET_VALID (I_sdlcLSAdminMODULO, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminMODULO = value->sl_value;
	break;
#endif /* I_sdlcLSAdminMODULO */

#ifdef I_sdlcLSAdminRETRIESm
    case I_sdlcLSAdminRETRIESm:

	SET_VALID (I_sdlcLSAdminRETRIESm, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminRETRIESm = value->sl_value;
	break;
#endif /* I_sdlcLSAdminRETRIESm */

#ifdef I_sdlcLSAdminRETRIESt
    case I_sdlcLSAdminRETRIESt:

	SET_VALID (I_sdlcLSAdminRETRIESt, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminRETRIESt = value->sl_value;
	break;
#endif /* I_sdlcLSAdminRETRIESt */

#ifdef I_sdlcLSAdminRETRIESn
    case I_sdlcLSAdminRETRIESn:

	SET_VALID (I_sdlcLSAdminRETRIESn, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminRETRIESn = value->sl_value;
	break;
#endif /* I_sdlcLSAdminRETRIESn */

#ifdef I_sdlcLSAdminRNRLIMIT
    case I_sdlcLSAdminRNRLIMIT:

	SET_VALID (I_sdlcLSAdminRNRLIMIT, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminRNRLIMIT = value->sl_value;
	break;
#endif /* I_sdlcLSAdminRNRLIMIT */

#ifdef I_sdlcLSAdminDATMODE
    case I_sdlcLSAdminDATMODE:

	SET_VALID (I_sdlcLSAdminDATMODE, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminDATMODE = value->sl_value;
	break;
#endif /* I_sdlcLSAdminDATMODE */

#ifdef I_sdlcLSAdminGPoll
    case I_sdlcLSAdminGPoll:

	SET_VALID (I_sdlcLSAdminGPoll, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminGPoll = value->sl_value;
	break;
#endif /* I_sdlcLSAdminGPoll */

#ifdef I_sdlcLSAdminSimRim
    case I_sdlcLSAdminSimRim:

	SET_VALID (I_sdlcLSAdminSimRim, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminSimRim = value->sl_value;
	break;
#endif /* I_sdlcLSAdminSimRim */

#ifdef I_sdlcLSAdminXmitRcvCap
    case I_sdlcLSAdminXmitRcvCap:

	SET_VALID (I_sdlcLSAdminXmitRcvCap, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminXmitRcvCap = value->sl_value;
	break;
#endif /* I_sdlcLSAdminXmitRcvCap */

#ifdef I_sdlcLSAdminRowStatus
    case I_sdlcLSAdminRowStatus:

	SET_VALID (I_sdlcLSAdminRowStatus, ((sdlcLSAdminEntry_t *) (dp->data))->valid);

	((sdlcLSAdminEntry_t *) (dp->data))->sdlcLSAdminRowStatus = value->sl_value;
	break;
#endif /* I_sdlcLSAdminRowStatus */

    default:
	DPRINTF ((APALWAYS, "snmpd: Internal error (invalid nominator in sdlcLSAdminEntry_test)\n"));
	return (GEN_ERROR);

    }	/* switch */ 
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}
/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/ 
int
sdlcLSAdminEntry_set (doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return (k_sdlcLSAdminEntry_set ((sdlcLSAdminEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}
#endif 
/* SETS */



/*---------------------------------------------------------------------
 * Retrieve data from the sdlcLSOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcLSOperEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                     ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcLSOperEntry_t *data = 0;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long ifIndex;
    long sdlcLSAddress;


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
    if ((InstToInt (incoming, 1 + object->oid.length, &sdlcLSAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcLSOperEntry_get (serialNum, contextInfo, arg, searchType, ifIndex, sdlcLSAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->ifIndex;
	inst.oid_ptr[1] = data->sdlcLSAddress;
    }	
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcLSOperName
    case I_sdlcLSOperName:
	dp = MakeOctetString (data->sdlcLSOperName->octet_ptr, data->sdlcLSOperName->length);
	break;
#endif /* I_sdlcLSOperName */

#ifdef I_sdlcLSOperRole
    case I_sdlcLSOperRole:
	dp = &data->sdlcLSOperRole;
	break;
#endif /* I_sdlcLSOperRole */

#ifdef I_sdlcLSOperState
    case I_sdlcLSOperState:
	dp = &data->sdlcLSOperState;
	break;
#endif /* I_sdlcLSOperState */

#ifdef I_sdlcLSOperMAXDATASend
    case I_sdlcLSOperMAXDATASend:
	dp = &data->sdlcLSOperMAXDATASend;
	break;
#endif /* I_sdlcLSOperMAXDATASend */

#ifdef I_sdlcLSOperREPLYTO
    case I_sdlcLSOperREPLYTO:
	dp = &data->sdlcLSOperREPLYTO;
	break;
#endif /* I_sdlcLSOperREPLYTO */

#ifdef I_sdlcLSOperMAXIN
    case I_sdlcLSOperMAXIN:
	dp = &data->sdlcLSOperMAXIN;
	break;
#endif /* I_sdlcLSOperMAXIN */

#ifdef I_sdlcLSOperMAXOUT
    case I_sdlcLSOperMAXOUT:
	dp = &data->sdlcLSOperMAXOUT;
	break;
#endif /* I_sdlcLSOperMAXOUT */

#ifdef I_sdlcLSOperMODULO
    case I_sdlcLSOperMODULO:
	dp = &data->sdlcLSOperMODULO;
	break;
#endif /* I_sdlcLSOperMODULO */

#ifdef I_sdlcLSOperRETRIESm
    case I_sdlcLSOperRETRIESm:
	dp = &data->sdlcLSOperRETRIESm;
	break;
#endif /* I_sdlcLSOperRETRIESm */

#ifdef I_sdlcLSOperRETRIESt
    case I_sdlcLSOperRETRIESt:
	dp = &data->sdlcLSOperRETRIESt;
	break;
#endif /* I_sdlcLSOperRETRIESt */

#ifdef I_sdlcLSOperRETRIESn
    case I_sdlcLSOperRETRIESn:
	dp = &data->sdlcLSOperRETRIESn;
	break;
#endif /* I_sdlcLSOperRETRIESn */

#ifdef I_sdlcLSOperRNRLIMIT
    case I_sdlcLSOperRNRLIMIT:
	dp = &data->sdlcLSOperRNRLIMIT;
	break;
#endif /* I_sdlcLSOperRNRLIMIT */

#ifdef I_sdlcLSOperDATMODE
    case I_sdlcLSOperDATMODE:
	dp = &data->sdlcLSOperDATMODE;
	break;
#endif /* I_sdlcLSOperDATMODE */

#ifdef I_sdlcLSOperLastModifyTime
    case I_sdlcLSOperLastModifyTime:
	dp = &data->sdlcLSOperLastModifyTime;
	break;
#endif /* I_sdlcLSOperLastModifyTime */

#ifdef I_sdlcLSOperLastFailTime
    case I_sdlcLSOperLastFailTime:
	dp = &data->sdlcLSOperLastFailTime;
	break;
#endif /* I_sdlcLSOperLastFailTime */

#ifdef I_sdlcLSOperLastFailCause
    case I_sdlcLSOperLastFailCause:
	dp = &data->sdlcLSOperLastFailCause;
	break;
#endif /* I_sdlcLSOperLastFailCause */

#ifdef I_sdlcLSOperLastFailCtrlIn
    case I_sdlcLSOperLastFailCtrlIn:
	dp = MakeOctetString (data->sdlcLSOperLastFailCtrlIn->octet_ptr, data->sdlcLSOperLastFailCtrlIn->length);
	break;
#endif /* I_sdlcLSOperLastFailCtrlIn */

#ifdef I_sdlcLSOperLastFailCtrlOut
    case I_sdlcLSOperLastFailCtrlOut:
	dp = MakeOctetString (data->sdlcLSOperLastFailCtrlOut->octet_ptr, data->sdlcLSOperLastFailCtrlOut->length);
	break;
#endif /* I_sdlcLSOperLastFailCtrlOut */

#ifdef I_sdlcLSOperLastFailFRMRInfo
    case I_sdlcLSOperLastFailFRMRInfo:
	dp = MakeOctetString (data->sdlcLSOperLastFailFRMRInfo->octet_ptr, data->sdlcLSOperLastFailFRMRInfo->length);
	break;
#endif /* I_sdlcLSOperLastFailFRMRInfo */

#ifdef I_sdlcLSOperLastFailREPLYTOs
    case I_sdlcLSOperLastFailREPLYTOs:
	dp = &data->sdlcLSOperLastFailREPLYTOs;
	break;
#endif /* I_sdlcLSOperLastFailREPLYTOs */

#ifdef I_sdlcLSOperEcho
    case I_sdlcLSOperEcho:
	dp = &data->sdlcLSOperEcho;
	break;
#endif /* I_sdlcLSOperEcho */

#ifdef I_sdlcLSOperGPoll
    case I_sdlcLSOperGPoll:
	dp = &data->sdlcLSOperGPoll;
	break;
#endif /* I_sdlcLSOperGPoll */

#ifdef I_sdlcLSOperSimRim
    case I_sdlcLSOperSimRim:
	dp = &data->sdlcLSOperSimRim;
	break;
#endif /* I_sdlcLSOperSimRim */

#ifdef I_sdlcLSOperXmitRcvCap
    case I_sdlcLSOperXmitRcvCap:
	dp = &data->sdlcLSOperXmitRcvCap;
	break;
#endif /* I_sdlcLSOperXmitRcvCap */

    default:
	return ((VarBind *) NULL);

    }	/* switch */ 
    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the sdlcLSStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
sdlcLSStatsEntry_get (OID *incoming, ObjectInfo *object, int searchType, 
                      ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    sdlcLSStatsEntry_t *data = 0;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long ifIndex;
    long sdlcLSAddress;


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
    if ((InstToInt (incoming, 1 + object->oid.length, &sdlcLSAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ 
    if ((arg == -1) || (data = k_sdlcLSStatsEntry_get (serialNum, contextInfo, arg, searchType, ifIndex, sdlcLSAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->ifIndex;
	inst.oid_ptr[1] = data->sdlcLSAddress;
    }				       
    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_sdlcLSStatsBLUsIn
    case I_sdlcLSStatsBLUsIn:
	dp = &data->sdlcLSStatsBLUsIn;
	break;
#endif /* I_sdlcLSStatsBLUsIn */

#ifdef I_sdlcLSStatsBLUsOut
    case I_sdlcLSStatsBLUsOut:
	dp = &data->sdlcLSStatsBLUsOut;
	break;
#endif /* I_sdlcLSStatsBLUsOut */

#ifdef I_sdlcLSStatsOctetsIn
    case I_sdlcLSStatsOctetsIn:
	dp = &data->sdlcLSStatsOctetsIn;
	break;
#endif /* I_sdlcLSStatsOctetsIn */

#ifdef I_sdlcLSStatsOctetsOut
    case I_sdlcLSStatsOctetsOut:
	dp = &data->sdlcLSStatsOctetsOut;
	break;
#endif /* I_sdlcLSStatsOctetsOut */

#ifdef I_sdlcLSStatsPollsIn
    case I_sdlcLSStatsPollsIn:
	dp = &data->sdlcLSStatsPollsIn;
	break;
#endif /* I_sdlcLSStatsPollsIn */

#ifdef I_sdlcLSStatsPollsOut
    case I_sdlcLSStatsPollsOut:
	dp = &data->sdlcLSStatsPollsOut;
	break;
#endif /* I_sdlcLSStatsPollsOut */

#ifdef I_sdlcLSStatsPollRspsOut
    case I_sdlcLSStatsPollRspsOut:
	dp = &data->sdlcLSStatsPollRspsOut;
	break;
#endif /* I_sdlcLSStatsPollRspsOut */

#ifdef I_sdlcLSStatsPollRspsIn
    case I_sdlcLSStatsPollRspsIn:
	dp = &data->sdlcLSStatsPollRspsIn;
	break;
#endif /* I_sdlcLSStatsPollRspsIn */

#ifdef I_sdlcLSStatsLocalBusies
    case I_sdlcLSStatsLocalBusies:
	dp = &data->sdlcLSStatsLocalBusies;
	break;
#endif /* I_sdlcLSStatsLocalBusies */

#ifdef I_sdlcLSStatsRemoteBusies
    case I_sdlcLSStatsRemoteBusies:
	dp = &data->sdlcLSStatsRemoteBusies;
	break;
#endif /* I_sdlcLSStatsRemoteBusies */

#ifdef I_sdlcLSStatsIFramesIn
    case I_sdlcLSStatsIFramesIn:
	dp = &data->sdlcLSStatsIFramesIn;
	break;
#endif /* I_sdlcLSStatsIFramesIn */

#ifdef I_sdlcLSStatsIFramesOut
    case I_sdlcLSStatsIFramesOut:
	dp = &data->sdlcLSStatsIFramesOut;
	break;
#endif /* I_sdlcLSStatsIFramesOut */

#ifdef I_sdlcLSStatsUIFramesIn
    case I_sdlcLSStatsUIFramesIn:
	dp = &data->sdlcLSStatsUIFramesIn;
	break;
#endif /* I_sdlcLSStatsUIFramesIn */

#ifdef I_sdlcLSStatsUIFramesOut
    case I_sdlcLSStatsUIFramesOut:
	dp = &data->sdlcLSStatsUIFramesOut;
	break;
#endif /* I_sdlcLSStatsUIFramesOut */

#ifdef I_sdlcLSStatsXIDsIn
    case I_sdlcLSStatsXIDsIn:
	dp = &data->sdlcLSStatsXIDsIn;
	break;
#endif /* I_sdlcLSStatsXIDsIn */

#ifdef I_sdlcLSStatsXIDsOut
    case I_sdlcLSStatsXIDsOut:
	dp = &data->sdlcLSStatsXIDsOut;
	break;
#endif /* I_sdlcLSStatsXIDsOut */

#ifdef I_sdlcLSStatsTESTsIn
    case I_sdlcLSStatsTESTsIn:
	dp = &data->sdlcLSStatsTESTsIn;
	break;
#endif /* I_sdlcLSStatsTESTsIn */

#ifdef I_sdlcLSStatsTESTsOut
    case I_sdlcLSStatsTESTsOut:
	dp = &data->sdlcLSStatsTESTsOut;
	break;
#endif /* I_sdlcLSStatsTESTsOut */

#ifdef I_sdlcLSStatsREJsIn
    case I_sdlcLSStatsREJsIn:
	dp = &data->sdlcLSStatsREJsIn;
	break;
#endif /* I_sdlcLSStatsREJsIn */

#ifdef I_sdlcLSStatsREJsOut
    case I_sdlcLSStatsREJsOut:
	dp = &data->sdlcLSStatsREJsOut;
	break;
#endif /* I_sdlcLSStatsREJsOut */

#ifdef I_sdlcLSStatsFRMRsIn
    case I_sdlcLSStatsFRMRsIn:
	dp = &data->sdlcLSStatsFRMRsIn;
	break;
#endif /* I_sdlcLSStatsFRMRsIn */

#ifdef I_sdlcLSStatsFRMRsOut
    case I_sdlcLSStatsFRMRsOut:
	dp = &data->sdlcLSStatsFRMRsOut;
	break;
#endif /* I_sdlcLSStatsFRMRsOut */

#ifdef I_sdlcLSStatsSIMsIn
    case I_sdlcLSStatsSIMsIn:
	dp = &data->sdlcLSStatsSIMsIn;
	break;
#endif /* I_sdlcLSStatsSIMsIn */

#ifdef I_sdlcLSStatsSIMsOut
    case I_sdlcLSStatsSIMsOut:
	dp = &data->sdlcLSStatsSIMsOut;
	break;
#endif /* I_sdlcLSStatsSIMsOut */

#ifdef I_sdlcLSStatsRIMsIn
    case I_sdlcLSStatsRIMsIn:
	dp = &data->sdlcLSStatsRIMsIn;
	break;
#endif /* I_sdlcLSStatsRIMsIn */

#ifdef I_sdlcLSStatsRIMsOut
    case I_sdlcLSStatsRIMsOut:
	dp = &data->sdlcLSStatsRIMsOut;
	break;
#endif /* I_sdlcLSStatsRIMsOut */

#ifdef I_sdlcLSStatsDISCIn
    case I_sdlcLSStatsDISCIn:
	dp = &data->sdlcLSStatsDISCIn;
	break;
#endif /* I_sdlcLSStatsDISCIn */

#ifdef I_sdlcLSStatsDISCOut
    case I_sdlcLSStatsDISCOut:
	dp = &data->sdlcLSStatsDISCOut;
	break;
#endif /* I_sdlcLSStatsDISCOut */

#ifdef I_sdlcLSStatsUAIn
    case I_sdlcLSStatsUAIn:
	dp = &data->sdlcLSStatsUAIn;
	break;
#endif /* I_sdlcLSStatsUAIn */

#ifdef I_sdlcLSStatsUAOut
    case I_sdlcLSStatsUAOut:
	dp = &data->sdlcLSStatsUAOut;
	break;
#endif /* I_sdlcLSStatsUAOut */

#ifdef I_sdlcLSStatsDMIn
    case I_sdlcLSStatsDMIn:
	dp = &data->sdlcLSStatsDMIn;
	break;
#endif /* I_sdlcLSStatsDMIn */

#ifdef I_sdlcLSStatsDMOut
    case I_sdlcLSStatsDMOut:
	dp = &data->sdlcLSStatsDMOut;
	break;
#endif /* I_sdlcLSStatsDMOut */

#ifdef I_sdlcLSStatsSNRMIn
    case I_sdlcLSStatsSNRMIn:
	dp = &data->sdlcLSStatsSNRMIn;
	break;
#endif /* I_sdlcLSStatsSNRMIn */

#ifdef I_sdlcLSStatsSNRMOut
    case I_sdlcLSStatsSNRMOut:
	dp = &data->sdlcLSStatsSNRMOut;
	break;
#endif /* I_sdlcLSStatsSNRMOut */

#ifdef I_sdlcLSStatsProtocolErrs
    case I_sdlcLSStatsProtocolErrs:
	dp = &data->sdlcLSStatsProtocolErrs;
	break;
#endif /* I_sdlcLSStatsProtocolErrs */

#ifdef I_sdlcLSStatsActivityTOs
    case I_sdlcLSStatsActivityTOs:
	dp = &data->sdlcLSStatsActivityTOs;
	break;
#endif /* I_sdlcLSStatsActivityTOs */

#ifdef I_sdlcLSStatsRNRLIMITs
    case I_sdlcLSStatsRNRLIMITs:
	dp = &data->sdlcLSStatsRNRLIMITs;
	break;
#endif /* I_sdlcLSStatsRNRLIMITs */

#ifdef I_sdlcLSStatsRetriesExps
    case I_sdlcLSStatsRetriesExps:
	dp = &data->sdlcLSStatsRetriesExps;
	break;
#endif /* I_sdlcLSStatsRetriesExps */

#ifdef I_sdlcLSStatsRetransmitsIn
    case I_sdlcLSStatsRetransmitsIn:
	dp = &data->sdlcLSStatsRetransmitsIn;
	break;
#endif /* I_sdlcLSStatsRetransmitsIn */

#ifdef I_sdlcLSStatsRetransmitsOut
    case I_sdlcLSStatsRetransmitsOut:
	dp = &data->sdlcLSStatsRetransmitsOut;
	break;
#endif /* I_sdlcLSStatsRetransmitsOut */

    default:
	return ((VarBind *) NULL);

    } /* switch */ 
    return (MakeVarBind (object, &inst, dp));

} 

sdlcPortAdminEntry_t *
k_sdlcPortAdminEntry_get (int serialNum, ContextInfo *contextInfo, int nominator, 
                          int searchType, long ifIndex)
{
#ifdef NOT_YET
    static sdlcPortAdminEntry_t sdlcPortAdminEntryData;

    sdlcPortAdminEntryData.sdlcPortAdminName =;
    sdlcPortAdminEntryData.sdlcPortAdminRole =;
    sdlcPortAdminEntryData.sdlcPortAdminType =;
    sdlcPortAdminEntryData.sdlcPortAdminTopology =;
    sdlcPortAdminEntryData.sdlcPortAdminISTATUS =;
    sdlcPortAdminEntryData.sdlcPortAdminACTIVTO =;
    sdlcPortAdminEntryData.sdlcPortAdminPAUSE =;
    sdlcPortAdminEntryData.sdlcPortAdminSERVLIM =;
    sdlcPortAdminEntryData.sdlcPortAdminSlowPollTimer =;

    return (&sdlcPortAdminEntryData);
#else /* NOT_YET */
    return (NULL);
#endif /* NOT_YET */
}
#ifdef SETS 
int
k_sdlcPortAdminEntry_set (sdlcPortAdminEntry_t *data, ContextInfo *contextInfo, 
                          int function)
{

    return (GEN_ERROR);
}
#endif /* SETS */ 


sdlcPortOperEntry_t *
k_sdlcPortOperEntry_get (int serialNum, ContextInfo *contextInfo, int nominator, 
                         int searchType, long ifIndex)
{
    hwidbtype                  *hwidb;
    static sdlcPortOperEntry_t  sdlcPortOperEntryData;
    static unsigned char portName[] = "Serial  ";
    static OctetString portStr = {portName, 8};

    if ((hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
				 IsThisHwidbSdlc, NULL)) == NULL)
        return NULL;

    portName[6] = hwidb->unit + '0';
    sdlcPortOperEntryData.sdlcPortOperName = &portStr;

    sdlcPortOperEntryData.sdlcPortOperTopology = IsPointToPointOrMultipoint(hwidb);

    if (ISPSDLC(hwidb)) {
        sdlcPortOperEntryData.sdlcPortOperRole = D_sdlcPortOperRole_primary;
        sdlcPortOperEntryData.sdlcPortOperPAUSE = hwidb->sdlc_poll_pause_timer;
        if (sdlcPortOperEntryData.sdlcPortOperTopology == D_sdlcPortAdminTopology_multipoint){
            sdlcPortOperEntryData.sdlcPortOperSlowPollMethod = D_sdlcPortOperSlowPollMethod_pollpause;
            sdlcPortOperEntryData.sdlcPortOperSlowPollTimer = hwidb->sdlc_slow_poll_interval;
	}
        else { 
            sdlcPortOperEntryData.sdlcPortOperSlowPollMethod = 0;
            sdlcPortOperEntryData.sdlcPortOperSlowPollTimer = 0;
	}
    }
    else {
        if (ISSSDLC(hwidb))  
            sdlcPortOperEntryData.sdlcPortOperRole = D_sdlcPortOperRole_secondary;
        else  
            sdlcPortOperEntryData.sdlcPortOperRole = D_sdlcPortOperRole_undefined;
        sdlcPortOperEntryData.sdlcPortOperPAUSE = 0;   
        sdlcPortOperEntryData.sdlcPortOperSlowPollMethod = 0;
        sdlcPortOperEntryData.sdlcPortOperSlowPollTimer = 0;
    }
    sdlcPortOperEntryData.sdlcPortOperType = D_sdlcPortOperType_leased;
    
    /* The following is not implemented */
    sdlcPortOperEntryData.sdlcPortOperISTATUS = 0;
    sdlcPortOperEntryData.sdlcPortOperACTIVTO = 0;
    sdlcPortOperEntryData.sdlcPortOperSERVLIM = 0;
    sdlcPortOperEntryData.sdlcPortOperLastModifyTime = 0;
    sdlcPortOperEntryData.sdlcPortOperLastFailTime = 0;
    sdlcPortOperEntryData.sdlcPortOperLastFailCause = 0;

    sdlcPortOperEntryData.ifIndex = hwidb->snmp_if_index;
    return (&sdlcPortOperEntryData);
} 

sdlcPortStatsEntry_t *
k_sdlcPortStatsEntry_get (int serialNum, 
                          ContextInfo *contextInfo, 
                          int nominator, 
                          int searchType, 
                          long ifIndex)
{

    static sdlcPortStatsEntry_t sdlcPortStatsEntryData;
    hwidbtype       *hwidb;
    sdlc_data_block *sdb;
    int i;

    if ((hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
				 IsThisHwidbSdlc, NULL)) == NULL)
        return NULL;


    memset (&sdlcPortStatsEntryData, 0, sizeof (sdlcPortStatsEntry_t));

    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
	if ((sdb = hwidb->sdlc_data[i]) != 0){
            sdlcPortStatsEntryData.sdlcPortStatsPollsIn += sdb->sdlc_pollsIn;
            sdlcPortStatsEntryData.sdlcPortStatsPollsOut += sdb->sdlc_pollsOut;
            sdlcPortStatsEntryData.sdlcPortStatsPollRspsIn += sdb->sdlc_pollRspsIn;
            sdlcPortStatsEntryData.sdlcPortStatsPollRspsOut += sdb->sdlc_pollRspsOut;
            sdlcPortStatsEntryData.sdlcPortStatsLocalBusies += sdb->sdlc_rnrssent;
            sdlcPortStatsEntryData.sdlcPortStatsRemoteBusies += sdb->sdlc_rnrsrcvd ;
            sdlcPortStatsEntryData.sdlcPortStatsIFramesIn += sdb->sdlc_iframesrcvd;
            sdlcPortStatsEntryData.sdlcPortStatsIFramesOut += sdb->sdlc_iframessent;
            sdlcPortStatsEntryData.sdlcPortStatsOctetsIn += sdb->sdlc_octetsrcvd;
            sdlcPortStatsEntryData.sdlcPortStatsOctetsOut += sdb->sdlc_octetssent;
            sdlcPortStatsEntryData.sdlcPortStatsProtocolErrs += sdb->sdlc_protocolErrs;
            sdlcPortStatsEntryData.sdlcPortStatsRNRLIMITs += sdb->sdlc_rnrLimitCount;
            sdlcPortStatsEntryData.sdlcPortStatsActivityTOs += sdb->sdlc_activityTOs;
            sdlcPortStatsEntryData.sdlcPortStatsRetriesExps += sdb->sdlc_retriesexps;
            sdlcPortStatsEntryData.sdlcPortStatsRetransmitsOut += sdb->sdlc_retransmitsFrameCount;
        }
    }

    /* The following is not implemented */
    sdlcPortStatsEntryData.sdlcPortStatsPhysicalFailures = 0;
    sdlcPortStatsEntryData.sdlcPortStatsInvalidAddresses = 0;
    sdlcPortStatsEntryData.sdlcPortStatsDwarfFrames = 0;
    sdlcPortStatsEntryData.sdlcPortStatsRetransmitsIn = 0;
    

    sdlcPortStatsEntryData.ifIndex = hwidb->snmp_if_index;   

    return (&sdlcPortStatsEntryData);
} 

sdlcLSAdminEntry_t *
k_sdlcLSAdminEntry_get (int serialNum, 
                        ContextInfo *contextInfo, 
                        int nominator, 
                        int searchType, 
                        long ifIndex, 
                        long sdlcLSAddress)
{
  
    hwidbtype                *hwidb;
    sdlc_data_block          *sdb;
    static sdlcLSAdminEntry_t sdlcLSAdminEntryData;
    static unsigned char nullStr[] = "";
    static OctetString adminName = {nullStr, 0}; 

    if ((sdb = GetSdbFromIndex(searchType, ifIndex, sdlcLSAddress, &hwidb)) == NULL)
        return NULL;

    sdlcLSAdminEntryData.sdlcLSAddress = sdb->sdlc_address;
    sdlcLSAdminEntryData.ifIndex = hwidb->snmp_if_index;
    sdlcLSAdminEntryData.sdlcLSAdminName = &adminName;
    sdlcLSAdminEntryData.sdlcLSAdminState = D_sdlcLSAdminState_active;
    sdlcLSAdminEntryData.sdlcLSAdminREPLYTO = hwidb->sdlc_t1;
    sdlcLSAdminEntryData.sdlcLSAdminMAXIN = hwidb->sdlc_k;
    sdlcLSAdminEntryData.sdlcLSAdminMAXOUT = hwidb->sdlc_k;
    sdlcLSAdminEntryData.sdlcLSAdminMODULO = D_sdlcLSOperMODULO_eight;
    sdlcLSAdminEntryData.sdlcLSAdminRETRIESm = hwidb->sdlc_n2;
    sdlcLSAdminEntryData.sdlcLSAdminRETRIESt = 0;
    sdlcLSAdminEntryData.sdlcLSAdminRETRIESn = 1;

    /* The following is not supported */
    sdlcLSAdminEntryData.sdlcLSAdminISTATUS = 0;
    sdlcLSAdminEntryData.sdlcLSAdminMAXDATASend = 0;
    sdlcLSAdminEntryData.sdlcLSAdminMAXDATARcv = 0;
    sdlcLSAdminEntryData.sdlcLSAdminRNRLIMIT = 0;
    sdlcLSAdminEntryData.sdlcLSAdminDATMODE = 0;
    sdlcLSAdminEntryData.sdlcLSAdminGPoll = 0;
    sdlcLSAdminEntryData.sdlcLSAdminSimRim = 0;
    sdlcLSAdminEntryData.sdlcLSAdminXmitRcvCap = 0;
    sdlcLSAdminEntryData.sdlcLSAdminRowStatus = 0;

    return (&sdlcLSAdminEntryData);

}
#ifdef SETS
int
k_sdlcLSAdminEntry_set (sdlcLSAdminEntry_t *data, ContextInfo *contextInfo, 
                        int function)
{

    return (GEN_ERROR);
}
#endif /* SETS */ 


sdlcLSOperEntry_t *
k_sdlcLSOperEntry_get (int          serialNum, 
                       ContextInfo *contextInfo, 
                       int          nominator, 
                       int          searchType, 
                       long         ifIndex, 
                       long         sdlcLSAddress)
{
    hwidbtype                *hwidb;
    sdlc_data_block          *sdb;
    static sdlcLSOperEntry_t  sdlcLSOperEntryData;
    static unsigned char instr[3];
    static unsigned char outstr[3];
    static unsigned char frmrstr[7];
    static OctetString lastFailCtrlIn = {instr, 2}; /* Statically allocate OctetStrings before use */
    static OctetString lastFailCtrlOut = {outstr, 2};
    static OctetString sdlcLSOperLastFailFRMRInfo = {frmrstr, 6};
    static OctetString sdlcLSOperName;   

    if ((sdb = GetSdbFromIndex(searchType, ifIndex, sdlcLSAddress, &hwidb)) == NULL)
        return NULL;


    sdlcLSOperEntryData.sdlcLSOperName = &sdlcLSOperName;
    sdlcLSOperEntryData.sdlcLSOperName->octet_ptr = sdb->sdlc_operName; 
    sdlcLSOperEntryData.sdlcLSOperName->length    = strlen(sdb->sdlc_operName);

    switch (sdb->sdlc_state) {
        case SDLC_DISCONNECT :
        case SDLC_ERROR      : sdlcLSOperEntryData.sdlcLSOperState = D_sdlcLSOperState_discontacted;
                               break;
        case SDLC_DISCSENT   : sdlcLSOperEntryData.sdlcLSOperState = D_sdlcLSOperState_discontactPending;
                               break;
        case SDLC_SNRMSEEN   : 
        case SDLC_SNRMSENT   : sdlcLSOperEntryData.sdlcLSOperState = D_sdlcLSOperState_contactPending;
                               break;
        case SDLC_CONNECT    :
        case SDLC_THEMBUSY   :
        case SDLC_USBUSY     :
        case SDLC_BOTHBUSY   : sdlcLSOperEntryData.sdlcLSOperState = D_sdlcLSOperState_contacted;
                               break;
        default              : sdlcLSOperEntryData.sdlcLSOperState = 0;  /* Unknown state */
                               break;
    }  /*switch*/

    if (ISPSDLC(hwidb)) {
        sdlcLSOperEntryData.sdlcLSOperRole = D_sdlcLSOperRole_primary;
        sdlcLSOperEntryData.sdlcLSOperREPLYTO = hwidb->sdlc_t1;
    }
    else if (ISSSDLC(hwidb))  {
        sdlcLSOperEntryData.sdlcLSOperRole = D_sdlcLSOperRole_secondary;
        sdlcLSOperEntryData.sdlcLSOperREPLYTO = 0;
    }
    else { 
        sdlcLSOperEntryData.sdlcLSOperRole = D_sdlcLSOperRole_undefined;
        sdlcLSOperEntryData.sdlcLSOperREPLYTO = 0;
    }

    sdlcLSOperEntryData.sdlcLSOperMAXDATASend = hwidb->sdlc_n1/8 - SDLCHEADERBYTES;
    sdlcLSOperEntryData.sdlcLSOperMAXIN = hwidb->sdlc_k;
    sdlcLSOperEntryData.sdlcLSOperMAXOUT = hwidb->sdlc_k;
    sdlcLSOperEntryData.sdlcLSOperMODULO = D_sdlcLSOperMODULO_eight;
    sdlcLSOperEntryData.sdlcLSOperRETRIESm = hwidb->sdlc_n2;
    sdlcLSOperEntryData.sdlcLSOperRETRIESt = 0;
    sdlcLSOperEntryData.sdlcLSOperRETRIESn = 1;

    sdlcLSOperEntryData.sdlcLSOperRNRLIMIT = hwidb->sdlc_rnrLimit;

    sdlcLSOperEntryData.ifIndex = hwidb->snmp_if_index;
    sdlcLSOperEntryData.sdlcLSAddress = sdb->sdlc_address;

    if (hwidb->sdlc_simultaneous) {
        if (hwidb->sdlc_flags & SDLC_FLAG_TWS_FULL)
            sdlcLSOperEntryData.sdlcLSOperDATMODE = D_sdlcLSOperDATMODE_full;
        else
            sdlcLSOperEntryData.sdlcLSOperDATMODE = D_sdlcLSOperDATMODE_half;
    }
    else
        sdlcLSOperEntryData.sdlcLSOperDATMODE = D_sdlcLSOperDATMODE_half;         
        
    sdlcLSOperEntryData.sdlcLSOperLastModifyTime =
	SNMP_ABSOLUTE_TIME(hwidb->sdlc_lastModifyTime);

    switch (sdb->lastFailCause) {
        case RX_FRMR        : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_rxFRMR;
                              break;
        case TX_FRMR        : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_txFRMR;
                              break;
        case NO_RESPONSE    : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_noResponse;
                              break;
        case PROTOCOL_ERR   : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_protocolErr;
                              break;
        case NO_ACTIVITY    : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_noActivity;
                              break;
        case RNR_LIMIT      : sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_rnrLimit;
                              break;
        case RETRIES_EXPIRED: sdlcLSOperEntryData.sdlcLSOperLastFailCause =
                                D_sdlcLSOperLastFailCause_retriesExpired;
                              break;
        default             : sdlcLSOperEntryData.sdlcLSOperLastFailCause = 
                                D_sdlcLSOperLastFailCause_undefined;
                              break;
    }
    if (sdlcLSOperEntryData.sdlcLSOperLastFailCause == D_sdlcLSOperLastFailCause_undefined)
        sdlcLSOperEntryData.sdlcLSOperLastFailTime = 0L;
    else
        sdlcLSOperEntryData.sdlcLSOperLastFailTime =
	    SNMP_ABSOLUTE_TIME(sdb->sdlc_lastFailTime);

    
    sdlcLSOperEntryData.sdlcLSOperLastFailCtrlIn = &lastFailCtrlIn;
    if (sdb->sdlc_lastFailCtrlIn == SDLC_NOFRAME){
        /* No meaning if link has not failed */
        instr[0] = 0;
        lastFailCtrlIn.length = 0;
    } 
    else {
        sprintf(instr, "%02x", sdb->sdlc_lastFailCtrlIn);
        lastFailCtrlIn.length = 2;
    }

    
    sdlcLSOperEntryData.sdlcLSOperLastFailCtrlOut = &lastFailCtrlOut;
    if (sdb->sdlc_lastFailCtrlOut == SDLC_NOFRAME){
        /* No meaning if link has not failed*/ 
        outstr[0] = 0;
        lastFailCtrlOut.length = 0;
    } 
    else {
        sprintf(outstr, "%02x", sdb->sdlc_lastFailCtrlOut);
        lastFailCtrlOut.length = 2;
    }

        
    sdlcLSOperEntryData.sdlcLSOperLastFailFRMRInfo = &sdlcLSOperLastFailFRMRInfo;
    if ((sdb->lastFailCause == RX_FRMR) || (sdb->lastFailCause == TX_FRMR)){
        sprintf(frmrstr, "%02x%02x%02x",sdb->sdlc_lastFailFRMRInfo[0], 
                                        sdb->sdlc_lastFailFRMRInfo[1],
                                        sdb->sdlc_lastFailFRMRInfo[2]);
        sdlcLSOperEntryData.sdlcLSOperLastFailFRMRInfo->length = 6;
    }
    else {
        sdlcLSOperEntryData.sdlcLSOperLastFailFRMRInfo->length = 0;
        frmrstr[0] = 0;
    }

    sdlcLSOperEntryData.sdlcLSOperLastFailREPLYTOs = sdb->sdlc_lastFailReplyTO;    

    sdlcLSOperEntryData.sdlcLSOperEcho = 
        ((sdb->stun_addr_echo) ? D_sdlcLSOperEcho_yes : D_sdlcLSOperEcho_no);      
 
    sdlcLSOperEntryData.sdlcLSOperGPoll = hwidb->sdlc_group_addr;                  

    sdlcLSOperEntryData.sdlcLSOperSimRim =                                         
        ((sdb->sdlc_simRim == 0) ? D_sdlcLSOperSimRim_no : D_sdlcLSOperSimRim_yes); 

    sdlcLSOperEntryData.sdlcLSOperXmitRcvCap =                                     
        ((sdb->sdlc_xmitRcvCap == 0) ? D_sdlcLSOperXmitRcvCap_twa : D_sdlcLSOperXmitRcvCap_tws);

    return (&sdlcLSOperEntryData);
} 

sdlcLSStatsEntry_t *
k_sdlcLSStatsEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, 
                       int searchType, long ifIndex, long sdlcLSAddress)
{

    static sdlcLSStatsEntry_t sdlcLSStatsEntryData;
    hwidbtype       *hwidb;
    sdlc_data_block *sdb;

    if ((sdb = GetSdbFromIndex(searchType, ifIndex, sdlcLSAddress, &hwidb))== NULL)
        return NULL;

    sdlcLSStatsEntryData.sdlcLSStatsOctetsIn = sdb->sdlc_octetsrcvd;          
    sdlcLSStatsEntryData.sdlcLSStatsOctetsOut = sdb->sdlc_octetssent;         
    sdlcLSStatsEntryData.sdlcLSStatsUIFramesIn = sdb->sdlc_uiframesrcvd;      
    sdlcLSStatsEntryData.sdlcLSStatsUIFramesOut = sdb->sdlc_uiframessent;     
    sdlcLSStatsEntryData.sdlcLSStatsXIDsIn = sdb->sdlc_xidsrcvd;              
    sdlcLSStatsEntryData.sdlcLSStatsXIDsOut = sdb->sdlc_xidssent;             
    sdlcLSStatsEntryData.sdlcLSStatsTESTsIn = sdb->sdlc_testsrcvd;            
    sdlcLSStatsEntryData.sdlcLSStatsTESTsOut = sdb->sdlc_testssent;           
    sdlcLSStatsEntryData.sdlcLSStatsSIMsIn = sdb->sdlc_simsrcvd;              
    sdlcLSStatsEntryData.sdlcLSStatsSIMsOut = sdb->sdlc_simssent;             
    sdlcLSStatsEntryData.sdlcLSStatsRIMsIn = sdb->sdlc_rimsrcvd;              
    sdlcLSStatsEntryData.sdlcLSStatsRIMsOut = sdb->sdlc_rimssent;             
    sdlcLSStatsEntryData.sdlcLSStatsUAIn = sdb->sdlc_uasrcvd;                 
    sdlcLSStatsEntryData.sdlcLSStatsUAOut = sdb->sdlc_uassent;                
    sdlcLSStatsEntryData.sdlcLSStatsDMIn = sdb->sdlc_dmsrcvd;                 
    sdlcLSStatsEntryData.sdlcLSStatsDMOut = sdb->sdlc_dmssent;                
    sdlcLSStatsEntryData.sdlcLSStatsPollsIn = sdb->sdlc_pollsIn;
    sdlcLSStatsEntryData.sdlcLSStatsPollsOut = sdb->sdlc_pollsOut;
    sdlcLSStatsEntryData.sdlcLSStatsPollRspsIn = sdb->sdlc_pollRspsIn;
    sdlcLSStatsEntryData.sdlcLSStatsPollRspsOut = sdb->sdlc_pollRspsOut;
    sdlcLSStatsEntryData.sdlcLSStatsSNRMIn = sdb->sdlc_snrmsrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsSNRMOut = sdb->sdlc_snrmssent;
    sdlcLSStatsEntryData.sdlcLSStatsDISCIn = sdb->sdlc_discsrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsDISCOut = sdb->sdlc_discssent;
    sdlcLSStatsEntryData.sdlcLSStatsFRMRsIn = sdb->sdlc_frmrsrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsFRMRsOut = sdb->sdlc_frmrssent;
    sdlcLSStatsEntryData.sdlcLSStatsREJsIn = sdb->sdlc_rejsrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsREJsOut = sdb->sdlc_rejssent;             
    sdlcLSStatsEntryData.sdlcLSStatsIFramesIn = sdb->sdlc_iframesrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsIFramesOut = sdb->sdlc_iframessent;
    sdlcLSStatsEntryData.sdlcLSStatsRemoteBusies = sdb->sdlc_rnrsrcvd;
    sdlcLSStatsEntryData.sdlcLSStatsLocalBusies = sdb->sdlc_rnrssent;
    sdlcLSStatsEntryData.sdlcLSStatsProtocolErrs = sdb->sdlc_protocolErrs;
    sdlcLSStatsEntryData.sdlcLSStatsRNRLIMITs = sdb->sdlc_rnrLimitCount;      
    sdlcLSStatsEntryData.sdlcLSStatsActivityTOs = sdb->sdlc_activityTOs;
    sdlcLSStatsEntryData.sdlcLSStatsRetriesExps = sdb->sdlc_retriesexps;      
    sdlcLSStatsEntryData.sdlcLSStatsRetransmitsOut = sdb->sdlc_retransmitsFrameCount;
    sdlcLSStatsEntryData.ifIndex = hwidb->snmp_if_index;
    sdlcLSStatsEntryData.sdlcLSAddress = sdb->sdlc_address;

    /* The following is not supported */
    sdlcLSStatsEntryData.sdlcLSStatsBLUsIn = 0;
    sdlcLSStatsEntryData.sdlcLSStatsBLUsOut = 0;

    sdlcLSStatsEntryData.sdlcLSStatsRetransmitsIn = 0;

    return (&sdlcLSStatsEntryData);
}


/*
 *     sdlcLSStatusChange    NOTIFICATION-TYPE
 *            VARIABLES { ifIndex,
 *	                  sdlcLSAddress,
 *                        sdlcLSOperState,
 *                        sdlcLSAdminState,
 *                        sdlcLSOperLastFailTime,
 *                        sdlcLSOperLastFailCause,
 *                        sdlcLSOperLastFailCtrlIn,
 *                        sdlcLSOperLastFailCtrlOut,
 *                        sdlcLSOperLastFailREPLYTOs }
 *	      DESCRIPTION
 *                        "This trap indicates that the state of an SDLC 
 *                         link station has transitioned to contacted or
 *                         discontacted."
 *             ::= { sdlcTraps 2 }
 */

#define LS_STATUS_CHANGE_VBCOUNT  7
#define LS_STATUS_CHANGE_TRAP_NUM 2


static const OID sdlcLSStatusChange_varbinds[LS_STATUS_CHANGE_VBCOUNT] = {
    OIDconversion(sdlcLSOperState),
    OIDconversion(sdlcLSAdminState),
    OIDconversion(sdlcLSOperLastFailTime),
    OIDconversion(sdlcLSOperLastFailCause),
    OIDconversion(sdlcLSOperLastFailCtrlIn),
    OIDconversion(sdlcLSOperLastFailCtrlOut),
    OIDconversion(sdlcLSOperLastFailREPLYTOs)
};
 
void GenerateSDLCLSStatusChangeTrap(sdlc_data_block *sdb)
{
    OID indexOID;
    ulong index[2];
    indexOID.length = 2;
    indexOID.oid_ptr = index;
    index[0] = sdb->sdlc_idb->snmp_if_index;
    index[1] = sdb->sdlc_address;

    SnmpSendTrap_Generic(sdlcLSStatusChange_varbinds,
                         &sdlcEnterprise,
                         lsStatusChangeOid,
                         &indexOID,
                         LS_STATUS_CHANGE_VBCOUNT,
                         ENTERPRISE_TRAP, 
                         LS_STATUS_CHANGE_TRAP_NUM);
}

/* return TRUE if there is an sdlc configured */
static boolean snmp_sdlc_idb_tester (hwidbtype *idb, long* indexP)
{
   sdlc_data_block  *sdb;
   if (is_sdlc(idb)){
        int sdlc_address = *indexP;
        while ((sdb = idb->sdlc_data[sdlc_address]) == 0 && sdlc_address++ < 255)
            ;
       if (sdb != NULL){
          *indexP = sdlc_address;
          return TRUE;
      }
   }
   return FALSE;      /* no secondary index defined */
}

static sdlc_data_block* GetSdbFromIndex(int searchType, long ifIndex, long sdlc_address, hwidbtype** hwidbP)
{
   hwidbtype* hwidb = 
       ifmib_2indices_get(0, searchType, ifIndex,
			  snmp_sdlc_idb_tester, &sdlc_address);
   *hwidbP = hwidb;
   return (hwidb != NULL ? hwidb->sdlc_data[sdlc_address] : NULL);

}





/*
 * snasdlcmib subsystem header
 */

#define MAJVERSION_snasdlcmib 1
#define MINVERSION_snasdlcmib 0
#define EDITVERSION_snasdlcmib 0

SUBSYS_HEADER(snasdlcmib,
              MAJVERSION_snasdlcmib,
              MINVERSION_snasdlcmib,
              EDITVERSION_snasdlcmib,
              init_snasdlcmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: sdlc", NULL);
      
