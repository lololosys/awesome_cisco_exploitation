/* $Id: sr_pimmib.c,v 3.6.6.8 1996/08/13 14:53:02 tylin Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/sr_pimmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Dino Farinacci (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_pimmib.c,v $
 * Revision 3.6.6.8  1996/08/13  14:53:02  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Incorporate with new ifmib_swidb_get() supporting function.
 * Branch: California_branch
 *
 * Revision 3.6.6.7  1996/08/07  23:50:58  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.6.6.6  1996/08/03  01:20:46  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/07/03  20:42:13  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.6.6.4  1996/07/01  18:44:57  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/05/21  09:52:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.6.2  1996/05/21  06:33:07  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.6.1  1996/05/04  01:09:02  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.6  1996/03/02  02:25:47  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/02/22  09:03:21  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.4  1996/01/18  15:49:16  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:25:33  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  17:30:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:02:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/24  07:33:58  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.3  1995/07/17 07:32:55  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:27:18  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:04:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "logger.h"
#include "packet.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/tunnel.h"
#include "../iprouting/route.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/pim.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/sr_ipmroutemib.h"
#include "../ipmulticast/sr_pimmib-mib.h"

#include "../snmp/snmp_api.h"
#include "sr_pimmib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

void
init_pimmib (subsystype *subsys)
{
    load_mib(pimmib_OidList, pimmib_OidListNum);
    load_oid(pimmib_oid_table);
}










/*---------------------------------------------------------------------
 * Retrieve data from the pim family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
pim_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    pim_t          *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_pim_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_pimMessageInterval
      case I_pimMessageInterval:
	dp = &data->pimMessageInterval;
	break;
#endif				       /* I_pimMessageInterval */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the pim data object.
 *---------------------------------------------------------------------*/
static void
pim_free(pim_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after pim set/undo
 *---------------------------------------------------------------------*/
static int
pim_cleanup(doList_t *trash)
{
    pim_free(trash->data);
#ifdef SR_SNMPv2
    pim_free(trash->undodata);
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
pim_test(incoming, object, value, doHead, doCur, contextInfo)
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
	if ((dp->data = malloc(sizeof(pim_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(pim_t));

	dp->setMethod = pim_set;
	dp->cleanupMethod = pim_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

    }
    switch (object->nominator) {

#ifdef I_pimMessageInterval
      case I_pimMessageInterval:

	SET_VALID(I_pimMessageInterval, ((pim_t *) (dp->data))->valid);

	((pim_t *) (dp->data))->pimMessageInterval = value->sl_value;
	break;
#endif				       /* I_pimMessageInterval */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in pim_test)\n"));
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
pim_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_pim_set((pim_t *) (doCur->data),
		      contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the pimInterfaceEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
pimInterfaceEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    pimInterfaceEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            pimInterfaceIfIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &pimInterfaceIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_pimInterfaceEntry_get(serialNum, contextInfo, arg, searchType, pimInterfaceIfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->pimInterfaceIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_pimInterfaceAddress
      case I_pimInterfaceAddress:
	dp = IPToOctetString(data->pimInterfaceAddress);
	break;
#endif				       /* I_pimInterfaceAddress */

#ifdef I_pimInterfaceNetMask
      case I_pimInterfaceNetMask:
	dp = IPToOctetString(data->pimInterfaceNetMask);
	break;
#endif				       /* I_pimInterfaceNetMask */

#ifdef I_pimInterfaceMode
      case I_pimInterfaceMode:
	dp = &data->pimInterfaceMode;
	break;
#endif				       /* I_pimInterfaceMode */

#ifdef I_pimInterfaceDR
      case I_pimInterfaceDR:
	dp = IPToOctetString(data->pimInterfaceDR);
	break;
#endif				       /* I_pimInterfaceDR */

#ifdef I_pimInterfaceQueryInterval
      case I_pimInterfaceQueryInterval:
	dp = &data->pimInterfaceQueryInterval;
	break;
#endif				       /* I_pimInterfaceQueryInterval */

#ifdef I_pimInterfaceStatus
      case I_pimInterfaceStatus:
	dp = &data->pimInterfaceStatus;
	break;
#endif				       /* I_pimInterfaceStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the pimInterfaceEntry data object.
 *---------------------------------------------------------------------*/
static void
pimInterfaceEntry_free(pimInterfaceEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after pimInterfaceEntry set/undo
 *---------------------------------------------------------------------*/
static int
pimInterfaceEntry_cleanup(doList_t *trash)
{
    pimInterfaceEntry_free(trash->data);
#ifdef SR_SNMPv2
    pimInterfaceEntry_free(trash->undodata);
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
pimInterfaceEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    pimInterfaceEntry_t *pimInterfaceEntry;
    long            pimInterfaceIfIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &pimInterfaceIfIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    pimInterfaceEntry = k_pimInterfaceEntry_get(-1, contextInfo, object->nominator, EXACT, pimInterfaceIfIndex);

#ifndef pimInterfaceEntry_READ_CREATE

    if (pimInterfaceEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* pimInterfaceEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == pimInterfaceEntry_set) &&
	    (((pimInterfaceEntry_t *) (dp->data)) != NULL) &&
	    (((pimInterfaceEntry_t *) (dp->data))->pimInterfaceIfIndex == pimInterfaceIfIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(pimInterfaceEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(pimInterfaceEntry_t));

	dp->setMethod = pimInterfaceEntry_set;
	dp->cleanupMethod = pimInterfaceEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_pimInterfaceIfIndex, ((pimInterfaceEntry_t *) (dp->data))->valid);
	((pimInterfaceEntry_t *) (dp->data))->pimInterfaceIfIndex = pimInterfaceIfIndex;

#ifdef pimInterfaceEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (pimInterfaceEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, pimInterfaceEntry, sizeof(pimInterfaceEntry_t));

	    SET_ALL_VALID(((pimInterfaceEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* pimInterfaceEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_pimInterfaceMode
      case I_pimInterfaceMode:

	SET_VALID(I_pimInterfaceMode, ((pimInterfaceEntry_t *) (dp->data))->valid);

	((pimInterfaceEntry_t *) (dp->data))->pimInterfaceMode = value->sl_value;
	break;
#endif				       /* I_pimInterfaceMode */

#ifdef I_pimInterfaceQueryInterval
      case I_pimInterfaceQueryInterval:

	SET_VALID(I_pimInterfaceQueryInterval, ((pimInterfaceEntry_t *) (dp->data))->valid);

	((pimInterfaceEntry_t *) (dp->data))->pimInterfaceQueryInterval = value->sl_value;
	break;
#endif				       /* I_pimInterfaceQueryInterval */

#ifdef I_pimInterfaceStatus
      case I_pimInterfaceStatus:

	SET_VALID(I_pimInterfaceStatus, ((pimInterfaceEntry_t *) (dp->data))->valid);

	((pimInterfaceEntry_t *) (dp->data))->pimInterfaceStatus = value->sl_value;
	break;
#endif				       /* I_pimInterfaceStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in pimInterfaceEntry_test)\n"));
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
pimInterfaceEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_pimInterfaceEntry_set((pimInterfaceEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the pimNeighborEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
pimNeighborEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    pimNeighborEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    unsigned long   pimNeighborAddress;


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

    if ((InstToIP(incoming, 0 + object->oid.length, &pimNeighborAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_pimNeighborEntry_get(serialNum, contextInfo, arg, searchType, pimNeighborAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = (data->pimNeighborAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->pimNeighborAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->pimNeighborAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->pimNeighborAddress >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_pimNeighborIfIndex
      case I_pimNeighborIfIndex:
	dp = &data->pimNeighborIfIndex;
	break;
#endif				       /* I_pimNeighborIfIndex */

#ifdef I_pimNeighborUpTime
      case I_pimNeighborUpTime:
	dp = &data->pimNeighborUpTime;
	break;
#endif				       /* I_pimNeighborUpTime */

#ifdef I_pimNeighborExpiryTime
      case I_pimNeighborExpiryTime:
	dp = &data->pimNeighborExpiryTime;
	break;
#endif				       /* I_pimNeighborExpiryTime */

#ifdef I_pimNeighborMode
      case I_pimNeighborMode:
	dp = &data->pimNeighborMode;
	break;
#endif				       /* I_pimNeighborMode */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the pimGroupEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
pimGroupEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    pimGroupEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    unsigned long   pimGroupAddress;


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

    if ((InstToIP(incoming, 0 + object->oid.length, &pimGroupAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_pimGroupEntry_get(serialNum, contextInfo, arg, searchType, pimGroupAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = (data->pimGroupAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->pimGroupAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->pimGroupAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->pimGroupAddress >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_pimGroupRPcount
      case I_pimGroupRPcount:
	dp = &data->pimGroupRPcount;
	break;
#endif				       /* I_pimGroupRPcount */

#ifdef I_pimGroupRPreach
      case I_pimGroupRPreach:
	dp = &data->pimGroupRPreach;
	break;
#endif				       /* I_pimGroupRPreach */

#ifdef I_pimGroupMode
      case I_pimGroupMode:
	dp = &data->pimGroupMode;
	break;
#endif				       /* I_pimGroupMode */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the pimRPEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
pimRPEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    pimRPEntry_t   *data = NULL;
    unsigned long   buffer[8];
    OID             inst;
    int             carry;
    unsigned long   pimRPGroupAddress;
    unsigned long   pimRPAddress;


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

    if ((InstToIP(incoming, 4 + object->oid.length, &pimRPAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &pimRPGroupAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_pimRPEntry_get(serialNum, contextInfo, arg, searchType, pimRPGroupAddress, pimRPAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 8;
	inst.oid_ptr[0] = (data->pimRPGroupAddress >> 24) & 0xff;
	inst.oid_ptr[1] = (data->pimRPGroupAddress >> 16) & 0xff;
	inst.oid_ptr[2] = (data->pimRPGroupAddress >> 8) & 0xff;
	inst.oid_ptr[3] = (data->pimRPGroupAddress >> 0) & 0xff;
	inst.oid_ptr[4] = (data->pimRPAddress >> 24) & 0xff;
	inst.oid_ptr[5] = (data->pimRPAddress >> 16) & 0xff;
	inst.oid_ptr[6] = (data->pimRPAddress >> 8) & 0xff;
	inst.oid_ptr[7] = (data->pimRPAddress >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_pimRPState
      case I_pimRPState:
	dp = &data->pimRPState;
	break;
#endif				       /* I_pimRPState */

#ifdef I_pimRPStateTimer
      case I_pimRPStateTimer:
	dp = &data->pimRPStateTimer;
	break;
#endif				       /* I_pimRPStateTimer */

#ifdef I_pimRPLastChange
      case I_pimRPLastChange:
	dp = &data->pimRPLastChange;
	break;
#endif				       /* I_pimRPLastChange */

#ifdef I_pimRPRowStatus
      case I_pimRPRowStatus:
	dp = &data->pimRPRowStatus;
	break;
#endif				       /* I_pimRPRowStatus */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the pimRPEntry data object.
 *---------------------------------------------------------------------*/
static void
pimRPEntry_free(pimRPEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after pimRPEntry set/undo
 *---------------------------------------------------------------------*/
static int
pimRPEntry_cleanup(doList_t *trash)
{
    pimRPEntry_free(trash->data);
#ifdef SR_SNMPv2
    pimRPEntry_free(trash->undodata);
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
pimRPEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    pimRPEntry_t   *pimRPEntry;
    unsigned long   pimRPGroupAddress;
    unsigned long   pimRPAddress;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 8) {
	return NO_CREATION_ERROR;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &pimRPGroupAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToIP(incoming, 4 + object->oid.length, &pimRPAddress, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    pimRPEntry = k_pimRPEntry_get(-1, contextInfo, object->nominator, EXACT, pimRPGroupAddress, pimRPAddress);

#ifndef pimRPEntry_READ_CREATE

    if (pimRPEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* pimRPEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == pimRPEntry_set) &&
	    (((pimRPEntry_t *) (dp->data)) != NULL) &&
	    (((pimRPEntry_t *) (dp->data))->pimRPGroupAddress == pimRPGroupAddress) &&
	    (((pimRPEntry_t *) (dp->data))->pimRPAddress == pimRPAddress)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(pimRPEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(pimRPEntry_t));

	dp->setMethod = pimRPEntry_set;
	dp->cleanupMethod = pimRPEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_pimRPGroupAddress, ((pimRPEntry_t *) (dp->data))->valid);
	((pimRPEntry_t *) (dp->data))->pimRPGroupAddress = pimRPGroupAddress;

	SET_VALID(I_pimRPAddress, ((pimRPEntry_t *) (dp->data))->valid);
	((pimRPEntry_t *) (dp->data))->pimRPAddress = pimRPAddress;

#ifdef pimRPEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (pimRPEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, pimRPEntry, sizeof(pimRPEntry_t));

	    SET_ALL_VALID(((pimRPEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* pimRPEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_pimRPRowStatus
      case I_pimRPRowStatus:

	SET_VALID(I_pimRPRowStatus, ((pimRPEntry_t *) (dp->data))->valid);

	((pimRPEntry_t *) (dp->data))->pimRPRowStatus = value->sl_value;
	break;
#endif				       /* I_pimRPRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in pimRPEntry_test)\n"));
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
pimRPEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_pimRPEntry_set((pimRPEntry_t *) (doCur->data),
			     contextInfo, doCur->state));
}

#endif				       /* SETS */






pim_t          *
k_pim_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static pim_t    pimData;

    pimData.pimMessageInterval = pim_message_interval / ONESEC;

    return (&pimData);
}

#ifdef SETS
int
k_pim_set(data, contextInfo, function)
    pim_t          *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

/*
 * ip_pim_enabled_test
 *
 * Called by snmp_swidb_get() to verify if pim enabled on interface.
 */
static boolean ip_pim_enabled_test (idbtype *swidb, long *index2)
{
    return(swidb->pim_enabled);
}

pimInterfaceEntry_t *
k_pimInterfaceEntry_get(serialNum, contextInfo, nominator, searchType, pimInterfaceIfIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            pimInterfaceIfIndex;
{
    static pimInterfaceEntry_t pimInterfaceEntryData;
    idbtype *swidb;

    swidb = ifmib_swidb_get(serialNum, searchType,
			    pimInterfaceIfIndex,
			    ip_pim_enabled_test, NULL);

    if (swidb == NULL)
        return(NULL);

    pimInterfaceEntryData.pimInterfaceIfIndex = swidb->snmp_if_index;
    if (swidb->ip_address) {
	pimInterfaceEntryData.pimInterfaceAddress = swidb->ip_address;
	pimInterfaceEntryData.pimInterfaceNetMask = swidb->ip_nets_mask;
    } else if (swidb->ip_unnumbered) {
	pimInterfaceEntryData.pimInterfaceAddress =
	    swidb->ip_unnumbered->ip_address;
	pimInterfaceEntryData.pimInterfaceNetMask =
	    swidb->ip_unnumbered->ip_nets_mask;
    } else {
	pimInterfaceEntryData.pimInterfaceAddress =
	pimInterfaceEntryData.pimInterfaceNetMask = 0;
    }
    pimInterfaceEntryData.pimInterfaceMode = 
	(swidb->pim_mode == PIM_DENSE_MODE) ? D_pimInterfaceMode_dense :
	    D_pimInterfaceMode_sparse;
    pimInterfaceEntryData.pimInterfaceDR = swidb->pim_dr;
    pimInterfaceEntryData.pimInterfaceQueryInterval = 
	swidb->pim_query_interval / ONESEC;
    pimInterfaceEntryData.pimInterfaceStatus =  (swidb->pim_enabled) ?
	D_pimInterfaceStatus_active : D_pimInterfaceStatus_notInService;

    return (&pimInterfaceEntryData);
}

#ifdef SETS
int
k_pimInterfaceEntry_set(data, contextInfo, function)
    pimInterfaceEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

/*
 * snmp_best_pim_nbr
 *
 * Get next lexigraphical PIM neighbor based on IP address.
 */
static pim_nbrtype *snmp_best_pim_nbr(int searchType, ulong pimNbrAddr, 
			       idbtype **idb_ptr)
{

    idbtype     *idb, *best_idb;
    pim_nbrtype *best_nbr, *pim;
    list_element *l_elt;
    
    best_nbr = NULL;
    best_idb = NULL;
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
        if (!idb->pim_enabled) continue;
	for (pim = idb->pim_nbrs; pim; pim = pim->next) {
	    if ((searchType == NEXT && pim->nbr_address >= pimNbrAddr) || 
		(pim->nbr_address == pimNbrAddr)) {
		if (!best_nbr || pim->nbr_address < best_nbr->nbr_address ||
		    searchType == EXACT) {
		    best_nbr = pim;
		    best_idb = idb;
		}
	    }
        }
    }
    *idb_ptr = best_idb;
    return(best_nbr);
}

pimNeighborEntry_t *
k_pimNeighborEntry_get(serialNum, contextInfo, nominator, searchType, pimNeighborAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    unsigned long   pimNeighborAddress;
{
    static pimNeighborEntry_t pimNeighborEntryData;
    pim_nbrtype *pim_nbr;
    idbtype *idb;

    pim_nbr = snmp_best_pim_nbr(searchType, pimNeighborAddress, &idb);
    if (!pim_nbr) return(NULL);

    pimNeighborEntryData.pimNeighborAddress = pim_nbr->nbr_address;
    pimNeighborEntryData.pimNeighborIfIndex = idb->snmp_if_index ?
	idb->snmp_if_index : idb->hwptr->snmp_if_index;
    pimNeighborEntryData.pimNeighborUpTime = ELAPSED_TIME(pim_nbr->uptime) / 10;
    pimNeighborEntryData.pimNeighborExpiryTime = TIME_LEFT_SLEEPING(pim_nbr->expires) / 10;
    pimNeighborEntryData.pimNeighborMode = (pim_nbr->mode == PIM_DENSE_MODE) ?
	D_pimNeighborMode_dense : D_pimNeighborMode_sparse;

    return (&pimNeighborEntryData);
}

pimGroupEntry_t *
k_pimGroupEntry_get(serialNum, contextInfo, nominator, searchType, pimGroupAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    unsigned long   pimGroupAddress;
{
    static pimGroupEntry_t pimGroupEntryData;
    gdbtype *gdb;

    gdb = snmp_get_ip_group(searchType, pimGroupAddress);
    if (gdb == NULL) 
	return(NULL);

    pimGroupEntryData.pimGroupAddress = gdb->mdb.group;
    pimGroupEntryData.pimGroupRPcount = gdb->rp.address ? 1 : 0;
    pimGroupEntryData.pimGroupRPreach = TIMER_RUNNING(gdb->send_rpreach) ?
	    TIME_LEFT_SLEEPING(gdb->send_rpreach) / 10 : 10;
    pimGroupEntryData.pimGroupMode = IS_SPARSE_GROUP(gdb) ? 
	D_pimGroupMode_sparse : D_pimGroupMode_dense;

    return (&pimGroupEntryData);
}

pimRPEntry_t   *
k_pimRPEntry_get(serialNum, contextInfo, nominator, searchType, pimRPGroupAddress, pimRPAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    unsigned long   pimRPGroupAddress;
    unsigned long   pimRPAddress;
{
    static pimRPEntry_t pimRPEntryData;
    gdbtype *gdb;
    mdb_rptype *rp, *best_rp;

    /*
     * Get group entry.
     */
    while (TRUE) {
	gdb = snmp_get_ip_group(searchType, pimRPGroupAddress);
	if (!gdb) return(NULL);

	/*
	 * Get RP entry in group entry.
	 */
	best_rp = NULL;
	pimRPGroupAddress = gdb->mdb.group;
	if (gdb->rp.address) {
	    rp = &gdb->rp;
	    if (searchType == EXACT) {
		if (rp->address == pimRPAddress) {
		    best_rp = rp;
		    break;
		}
	    } else if (searchType == NEXT) {
		if (!best_rp || (rp->address > pimRPAddress && 
				 rp->address < best_rp->address)) {
		    best_rp = rp;
		}
	    } else {
		return(NULL);
	    }
	}
	if (best_rp) break;
    }

    /*
     * Store the values.
     */
    pimRPEntryData.pimRPGroupAddress = gdb->mdb.group;
    pimRPEntryData.pimRPAddress = best_rp->address;
    pimRPEntryData.pimRPState = D_pimRPState_up ;
    pimRPEntryData.pimRPStateTimer = TIME_LEFT_SLEEPING(best_rp->expires) / 10;
    pimRPEntryData.pimRPLastChange = ELAPSED_TIME(best_rp->uptime) / 10;
    pimRPEntryData.pimRPRowStatus = D_pimRPRowStatus_active;

    return (&pimRPEntryData);
}

#ifdef SETS
int
k_pimRPEntry_set(data, contextInfo, function)
    pimRPEntry_t   *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */


/*
 * pimmib subsystem header
 */

#define MAJVERSION_pimmib 1
#define MINVERSION_pimmib 0
#define EDITVERSION_pimmib 0

SUBSYS_HEADER(pimmib,
              MAJVERSION_pimmib,
              MINVERSION_pimmib,
              EDITVERSION_pimmib,
              init_pimmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: ipmulticast",
              NULL);
      
