/* $Id: sr_ipmroutemib.c,v 3.6.8.8 1996/08/13 14:52:59 tylin Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/sr_ipmroutemib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Dino Farinacci (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ipmroutemib.c,v $
 * Revision 3.6.8.8  1996/08/13  14:52:59  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Incorporate with new ifmib_swidb_get() supporting function.
 * Branch: California_branch
 *
 * Revision 3.6.8.7  1996/08/07  23:50:55  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.6.8.6  1996/07/03  20:42:10  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.6.8.5  1996/07/01  18:44:55  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.8.4  1996/06/27  07:31:34  jjohnson
 * CSCdi61124:  Spurious Accesses Recorded in build_snmpv1_response
 * Fix definition and implementation of ciscoIpMRouteNextHopMacHdr
 *
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/05/21  09:52:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.8.2  1996/05/21  06:33:06  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.8.1  1996/03/18  20:19:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/16  06:55:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.1  1996/03/07  09:48:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/02/29  21:00:34  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/18  15:49:14  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/14  08:25:32  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/12/07  03:29:03  dino
 * CSCdi45160:  DVMRP radix tree is not matching on default route
 *
 * Revision 3.2  1995/11/17  17:30:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:02:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/06  07:06:33  dino
 * CSCdi39769:  Modularity: mroute.c has direct calls into mib subsystem
 *
 * Revision 2.7  1995/09/01  21:47:10  anke
 * CSCdi39742:  added ipmroute objects in cisco OID space
 *
 * Revision 2.6  1995/08/07  19:15:22  sgubbi
 * CSCdi38292:  Set ipMRouteProtocol now that we have multiple values
 * implemented.
 *
 * Revision 2.5  1995/08/05  00:54:20  dino
 * CSCdi38292:  Set ipMRouteProtocol now that we have multiple values
 *              implemented.
 *
 * Revision 2.4  1995/07/24  07:33:56  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.3  1995/07/17 07:32:54  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:27:15  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:04:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include <stdarg.h>
#include "subsys.h"
#include "packet.h"
#include "access.h"
#include "interface_private.h"
#include "../snmp/snmp_api.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaccess_private.h"
#include "../ip/tunnel.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/sr_ipmroutemib.h"
#include "sr_ipmroutemib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "../iprouting/route.h"

/*
 * This is a radix tree that corresponds to the multicast routing table.
 * There is an entry for each group which resides in the multicast routing
 * table. When a group is added or deleted, the SNMP equivalent add or
 * delete is performed for this data structure. This allows us to efficiently
 * get-next on multicast group entries.
 */
rnhtype *snmp_ip_group_cache;

void
init_ipmroutemib (subsystype *subsys)
{
    load_mib(ipmroutemib_OidList, ipmroutemib_OidListNum);
    load_oid(ipmroutemib_oid_table);
	
    /*	
     * Initialize SNMP IP radix tree.
     */
    rn_inithead((void **) &snmp_ip_group_cache, IPRADIXDATAOFFSET);

    /*
     * Register functions to the IP multicast routing table code.
     */
    reg_add_snmp_add_ip_group(snmp_add_ip_group, "snmp_add_ip_group");
    reg_add_snmp_delete_ip_group(snmp_delete_ip_group, "snmp_delete_ip_group");
}









/*---------------------------------------------------------------------
 * Retrieve data from the ipMRoute family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ipMRoute_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ipMRoute_t     *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ipMRoute_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ipMRouteEnable
      case I_ipMRouteEnable:
	dp = &data->ipMRouteEnable;
	break;
#endif				       /* I_ipMRouteEnable */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ipMRoute data object.
 *---------------------------------------------------------------------*/
static void
ipMRoute_free(ipMRoute_t *data)
{
    if (data != NULL) {
	free(data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ipMRoute set/undo
 *---------------------------------------------------------------------*/
static int
ipMRoute_cleanup(doList_t *trash)
{
    ipMRoute_free(trash->data);
#ifdef SR_SNMPv2
    ipMRoute_free(trash->undodata);
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
ipMRoute_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             error_status = NO_ERROR;

    /*
     * Validate the object instance: 1) It must be of length 1  2) and the
     * instance must be 0.
     */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
	return (NO_CREATION_ERROR);
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == ipMRoute_set) &&
            (((ipMRoute_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(ipMRoute_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(ipMRoute_t));

	dp->setMethod = ipMRoute_set;
	dp->cleanupMethod = ipMRoute_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ipMRoute_UNDO
        dp->undoMethod = ipMRoute_undo;
#else /* SR_ipMRoute_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_ipMRoute_UNDO */
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

    }

    switch (object->nominator) {

#ifdef I_ipMRouteEnable
      case I_ipMRouteEnable:

        if ((value->sl_value < 1) || (value->sl_value > 2)) {
           return WRONG_VALUE_ERROR;
        }

	((ipMRoute_t *) (dp->data))->ipMRouteEnable = value->sl_value;
	break;
#endif				       /* I_ipMRouteEnable */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ipMRoute_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */

   /* Do system dependent testing in k_ipMRoute_test */
   error_status = k_ipMRoute_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ipMRoute_t *) (dp->data))->valid);
        error_status = k_ipMRoute_ready(object, value, doHead, dp);
   }

   return (error_status);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
ipMRoute_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_ipMRoute_set((ipMRoute_t *) (doCur->data),
			   contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ipMRouteEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ipMRouteEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ipMRouteEntry_t *data = NULL;
    unsigned long   buffer[12];
    OID             inst;
    int             carry;
    unsigned long   ipMRouteGroup;
    unsigned long   ipMRouteSource;
    unsigned long   ipMRouteSourceMask;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 12
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 12) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 8 + object->oid.length, &ipMRouteSourceMask, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 4 + object->oid.length, &ipMRouteSource, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ipMRouteGroup, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ipMRouteEntry_get(serialNum, contextInfo, arg, searchType, ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 12;
	inst.oid_ptr[0] = (data->ipMRouteGroup >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ipMRouteGroup >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ipMRouteGroup >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ipMRouteGroup >> 0) & 0xff;
	inst.oid_ptr[4] = (data->ipMRouteSource >> 24) & 0xff;
	inst.oid_ptr[5] = (data->ipMRouteSource >> 16) & 0xff;
	inst.oid_ptr[6] = (data->ipMRouteSource >> 8) & 0xff;
	inst.oid_ptr[7] = (data->ipMRouteSource >> 0) & 0xff;
	inst.oid_ptr[8] = (data->ipMRouteSourceMask >> 24) & 0xff;
	inst.oid_ptr[9] = (data->ipMRouteSourceMask >> 16) & 0xff;
	inst.oid_ptr[10] = (data->ipMRouteSourceMask >> 8) & 0xff;
	inst.oid_ptr[11] = (data->ipMRouteSourceMask >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ipMRouteUpstreamNeighbor
      case I_ipMRouteUpstreamNeighbor:
	dp = IPToOctetString(data->ipMRouteUpstreamNeighbor);
	break;
#endif				       /* I_ipMRouteUpstreamNeighbor */

#ifdef I_ipMRouteInIfIndex
      case I_ipMRouteInIfIndex:
	dp = &data->ipMRouteInIfIndex;
	break;
#endif				       /* I_ipMRouteInIfIndex */

#ifdef I_ipMRouteUpTime
      case I_ipMRouteUpTime:
	dp = &data->ipMRouteUpTime;
	break;
#endif				       /* I_ipMRouteUpTime */

#ifdef I_ipMRouteExpiryTime
      case I_ipMRouteExpiryTime:
	dp = &data->ipMRouteExpiryTime;
	break;
#endif				       /* I_ipMRouteExpiryTime */

#ifdef I_ipMRoutePkts
      case I_ipMRoutePkts:
	dp = &data->ipMRoutePkts;
	break;
#endif				       /* I_ipMRoutePkts */

#ifdef I_ipMRouteDifferentInIfPackets
      case I_ipMRouteDifferentInIfPackets:
	dp = &data->ipMRouteDifferentInIfPackets;
	break;
#endif				       /* I_ipMRouteDifferentInIfPackets */

#ifdef I_ipMRouteOctets
      case I_ipMRouteOctets:
	dp = &data->ipMRouteOctets;
	break;
#endif				       /* I_ipMRouteOctets */

#ifdef I_ipMRouteProtocol
      case I_ipMRouteProtocol:
	dp = &data->ipMRouteProtocol;
	break;
#endif				       /* I_ipMRouteProtocol */

#ifdef I_ciscoIpMRoutePruneFlag
    case I_ciscoIpMRoutePruneFlag:
       dp = &data->ciscoIpMRoutePruneFlag;
       break;
#endif /* I_ciscoIpMRoutePruneFlag */

#ifdef I_ciscoIpMRouteSparseFlag
    case I_ciscoIpMRouteSparseFlag:
       dp = &data->ciscoIpMRouteSparseFlag;
       break;
#endif /* I_ciscoIpMRouteSparseFlag */

#ifdef I_ciscoIpMRouteConnectedFlag
    case I_ciscoIpMRouteConnectedFlag:
       dp = &data->ciscoIpMRouteConnectedFlag;
       break;
#endif /* I_ciscoIpMRouteConnectedFlag */

#ifdef I_ciscoIpMRouteLocalFlag
    case I_ciscoIpMRouteLocalFlag:
       dp = &data->ciscoIpMRouteLocalFlag;
       break;
#endif /* I_ciscoIpMRouteLocalFlag */

#ifdef I_ciscoIpMRouteRegisterFlag
    case I_ciscoIpMRouteRegisterFlag:
       dp = &data->ciscoIpMRouteRegisterFlag;
       break;
#endif /* I_ciscoIpMRouteRegisterFlag */

#ifdef I_ciscoIpMRouteRpFlag
    case I_ciscoIpMRouteRpFlag:
       dp = &data->ciscoIpMRouteRpFlag;
       break;
#endif /* I_ciscoIpMRouteRpFlag */

#ifdef I_ciscoIpMRouteSptFlag
    case I_ciscoIpMRouteSptFlag:
       dp = &data->ciscoIpMRouteSptFlag;
       break;
#endif /* I_ciscoIpMRouteSptFlag */

#ifdef I_ciscoIpMRouteBps
    case I_ciscoIpMRouteBps:
       dp = &data->ciscoIpMRouteBps;
       break;
#endif /* I_ciscoIpMRouteBps */

#ifdef I_ciscoIpMRouteMetric
    case I_ciscoIpMRouteMetric:
       dp = &data->ciscoIpMRouteMetric;
       break;
#endif /* I_ciscoIpMRouteMetric */

#ifdef I_ciscoIpMRouteMetricPreference
    case I_ciscoIpMRouteMetricPreference:
       dp = &data->ciscoIpMRouteMetricPreference;
       break;
#endif /* I_ciscoIpMRouteMetricPreference */

#ifdef I_ciscoIpMRouteInLimit
    case I_ciscoIpMRouteInLimit:
       dp = &data->ciscoIpMRouteInLimit;
       break;
#endif /* I_ciscoIpMRouteInLimit */

#ifdef I_ciscoIpMRouteLastUsed
    case I_ciscoIpMRouteLastUsed:
       dp = &data->ciscoIpMRouteLastUsed;
       break;
#endif /* I_ciscoIpMRouteLastUsed */

#ifdef I_ciscoIpMRouteInLimit2
    case I_ciscoIpMRouteInLimit2:
       dp = &data->ciscoIpMRouteInLimit2;
       break;
#endif /* I_ciscoIpMRouteInLimit2 */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*---------------------------------------------------------------------
 * Retrieve data from the ipMRouteNextHopEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ipMRouteNextHopEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ipMRouteNextHopEntry_t *data = NULL;
    unsigned long   buffer[17];
    OID             inst;
    int             carry;
    unsigned long   ipMRouteNextHopGroup;
    unsigned long   ipMRouteNextHopSource;
    unsigned long   ipMRouteNextHopSourceMask;
    long            ipMRouteNextHopIfIndex;
    unsigned long   ipMRouteNextHopAddress;

    dp = NULL;

    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 17
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 17) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 13 + object->oid.length, &ipMRouteNextHopAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 12 + object->oid.length, &ipMRouteNextHopIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 8 + object->oid.length, &ipMRouteNextHopSourceMask, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 4 + object->oid.length, &ipMRouteNextHopSource, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &ipMRouteNextHopGroup, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ipMRouteNextHopEntry_get(serialNum, contextInfo, arg, searchType, ipMRouteNextHopGroup, ipMRouteNextHopSource, ipMRouteNextHopSourceMask, ipMRouteNextHopIfIndex, ipMRouteNextHopAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 17;
	inst.oid_ptr[0] = (data->ipMRouteNextHopGroup >> 24) & 0xff;
	inst.oid_ptr[1] = (data->ipMRouteNextHopGroup >> 16) & 0xff;
	inst.oid_ptr[2] = (data->ipMRouteNextHopGroup >> 8) & 0xff;
	inst.oid_ptr[3] = (data->ipMRouteNextHopGroup >> 0) & 0xff;
	inst.oid_ptr[4] = (data->ipMRouteNextHopSource >> 24) & 0xff;
	inst.oid_ptr[5] = (data->ipMRouteNextHopSource >> 16) & 0xff;
	inst.oid_ptr[6] = (data->ipMRouteNextHopSource >> 8) & 0xff;
	inst.oid_ptr[7] = (data->ipMRouteNextHopSource >> 0) & 0xff;
	inst.oid_ptr[8] = (data->ipMRouteNextHopSourceMask >> 24) & 0xff;
	inst.oid_ptr[9] = (data->ipMRouteNextHopSourceMask >> 16) & 0xff;
	inst.oid_ptr[10] = (data->ipMRouteNextHopSourceMask >> 8) & 0xff;
	inst.oid_ptr[11] = (data->ipMRouteNextHopSourceMask >> 0) & 0xff;
	inst.oid_ptr[12] = data->ipMRouteNextHopIfIndex;
	inst.oid_ptr[13] = (data->ipMRouteNextHopAddress >> 24) & 0xff;
	inst.oid_ptr[14] = (data->ipMRouteNextHopAddress >> 16) & 0xff;
	inst.oid_ptr[15] = (data->ipMRouteNextHopAddress >> 8) & 0xff;
	inst.oid_ptr[16] = (data->ipMRouteNextHopAddress >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ipMRouteNextHopState
      case I_ipMRouteNextHopState:
	dp = &data->ipMRouteNextHopState;
	break;
#endif				       /* I_ipMRouteNextHopState */

#ifdef I_ipMRouteNextHopUpTime
      case I_ipMRouteNextHopUpTime:
	dp = &data->ipMRouteNextHopUpTime;
	break;
#endif				       /* I_ipMRouteNextHopUpTime */

#ifdef I_ipMRouteNextHopExpiryTime
      case I_ipMRouteNextHopExpiryTime:
	dp = &data->ipMRouteNextHopExpiryTime;
	break;
#endif				       /* I_ipMRouteNextHopExpiryTime */

#ifdef I_ipMRouteNextHopClosestMemberHops
    case I_ipMRouteNextHopClosestMemberHops:
	dp = &data->ipMRouteNextHopClosestMemberHops;
	break;
#endif				       /* I_ipMRouteNextHopClosestMemberHops */

#ifdef I_ipMRouteNextHopProtocol
    case I_ipMRouteNextHopProtocol:
	dp = &data->ipMRouteNextHopProtocol;
	break;
#endif				       /* I_ipMRouteNextHopProtocol */

#ifdef I_ipMRouteNextHopPkts
    case I_ipMRouteNextHopPkts:
	dp = &data->ipMRouteNextHopPkts;
	break;
#endif				       /* I_ipMRouteNextHopPkts */

#ifdef I_ciscoIpMRouteNextHopOutLimit
    case I_ciscoIpMRouteNextHopOutLimit:
       dp = &data->ciscoIpMRouteNextHopOutLimit;
       break;
#endif /* I_ciscoIpMRouteNextHopOutLimit */

#ifdef I_ciscoIpMRouteNextHopMacHdr
    case I_ciscoIpMRouteNextHopMacHdr:
	dp = CloneOctetString(data->ciscoIpMRouteNextHopMacHdr);
	break;
#endif /* I_ciscoIpMRouteNextHopMacHdr */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}



/*---------------------------------------------------------------------
 * Retrieve data from the ipMRouteInterfaceEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ipMRouteInterfaceEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ipMRouteInterfaceEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ipMRouteInterfaceIfIndex;


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

    if ((InstToInt(incoming, 0 + object->oid.length, &ipMRouteInterfaceIfIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_ipMRouteInterfaceEntry_get(serialNum, contextInfo, arg, searchType, ipMRouteInterfaceIfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ipMRouteInterfaceIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ipMRouteInterfaceTtl
      case I_ipMRouteInterfaceTtl:
	dp = &data->ipMRouteInterfaceTtl;
	break;
#endif				       /* I_ipMRouteInterfaceTtl */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the ipMRouteInterfaceEntry data object.
 *---------------------------------------------------------------------*/
static void
ipMRouteInterfaceEntry_free(ipMRouteInterfaceEntry_t *data)
{
    if (data != NULL) {
	free(data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after ipMRouteInterfaceEntry set/undo
 *---------------------------------------------------------------------*/
static int
ipMRouteInterfaceEntry_cleanup(doList_t *trash)
{
    ipMRouteInterfaceEntry_free(trash->data);
#ifdef SR_SNMPv2
    ipMRouteInterfaceEntry_free(trash->undodata);
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
ipMRouteInterfaceEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int             error_status = NO_ERROR;
    int             carry = 0;
    ipMRouteInterfaceEntry_t *ipMRouteInterfaceEntry;
    long            ipMRouteInterfaceIfIndex;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &ipMRouteInterfaceIfIndex, EXACT, &carry)) < 0) {
	return NO_CREATION_ERROR;
    }

        ipMRouteInterfaceEntry = k_ipMRouteInterfaceEntry_get(-1, contextInfo, object->nominator, EXACT, ipMRouteInterfaceIfIndex);

#ifndef ipMRouteInterfaceEntry_READ_CREATE

    if (ipMRouteInterfaceEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* ipMRouteInterfaceEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == ipMRouteInterfaceEntry_set) &&
	    (((ipMRouteInterfaceEntry_t *) (dp->data)) != NULL) &&
	    (((ipMRouteInterfaceEntry_t *) (dp->data))->ipMRouteInterfaceIfIndex == ipMRouteInterfaceIfIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;

	dp->setMethod = ipMRouteInterfaceEntry_set;
	dp->cleanupMethod = ipMRouteInterfaceEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ipMRouteInterfaceEntry_UNDO
        dp->undoMethod = ipMRouteInterfaceEntry_undo;
#else                                  /* SR_ipMRouteInterfaceEntry_UNDO */
        dp->undoMethod = NULL;
#endif                                 /* SR_ipMRouteInterfaceEntry_UNDO */
	dp->state = UNKNOWN;

        if (ipMRouteInterfaceEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_ipMRouteInterfaceEntry(ipMRouteInterfaceEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_ipMRouteInterfaceEntry(ipMRouteInterfaceEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef ipMRouteInterfaceEntry_READ_CREATE
            if ( (dp->data = malloc(sizeof(ipMRouteInterfaceEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(ipMRouteInterfaceEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

                ((ipMRouteInterfaceEntry_t *) (dp->data))->ipMRouteInterfaceIfIndex = (ipMRouteInterfaceIfIndex);

	        SET_VALID(I_ipMRouteInterfaceIfIndex, ((ipMRouteInterfaceEntry_t *) (dp->data))->valid);

                error_status = k_ipMRouteInterfaceEntry_set_defaults(dp);

	    }
#else /* ipMRouteInterfaceEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* ipMRouteInterfaceEntry_READ_CREATE */
	}
#endif /* SR_SNMPv2 */
    }

    if (error_status != NO_ERROR) {
	return error_status;
    }

    switch (object->nominator) {

#ifdef I_ipMRouteInterfaceTtl
      case I_ipMRouteInterfaceTtl:
	((ipMRouteInterfaceEntry_t *) (dp->data))->ipMRouteInterfaceTtl = value->sl_value;
	break;
#endif				       /* I_ipMRouteInterfaceTtl */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ipMRouteInterfaceEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    /* Do system dependent testing in k_ipMRouteInterfaceEntry_test */
    error_status = k_ipMRouteInterfaceEntry_test(object, value, dp, contextInfo);
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ipMRouteInterfaceEntry_t *) (dp->data))->valid);
        error_status = k_ipMRouteInterfaceEntry_ready(object, value, doHead, dp);
    }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
ipMRouteInterfaceEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return (k_ipMRouteInterfaceEntry_set((ipMRouteInterfaceEntry_t *) (doCur->data),
					 contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ipMRouteBoundaryEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ipMRouteBoundaryEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ipMRouteBoundaryEntry_t *data = NULL;
    unsigned long   buffer[9];
    OID             inst;
    int             carry;
    long	    ipMRouteBoundaryIfIndex;
    unsigned long   ipMRouteBoundaryAddress;
    unsigned long   ipMRouteBoundaryAddressMask;

    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 9 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 9) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ((InstToIP(incoming, 5 + object->oid.length, &ipMRouteBoundaryAddressMask, searchType, &carry)) < 0) {
       arg = -1;
    }
    if ((InstToIP(incoming, 1 + object->oid.length, &ipMRouteBoundaryAddress, searchType, &carry)) < 0) {
       arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &ipMRouteBoundaryIfIndex, searchType, &carry)) < 0) {
       arg = -1;
    }
    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ((arg == -1) || (data = k_ipMRouteBoundaryEntry_get(serialNum, contextInfo, arg ,searchType, ipMRouteBoundaryIfIndex, ipMRouteBoundaryAddress, ipMRouteBoundaryAddressMask)) == NULL) {
        arg = -1;
     } else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 9;
        inst.oid_ptr[0] = data->ipMRouteBoundaryIfIndex;
        inst.oid_ptr[1] = (data->ipMRouteBoundaryAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->ipMRouteBoundaryAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->ipMRouteBoundaryAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->ipMRouteBoundaryAddress >> 0) & 0xff;
        inst.oid_ptr[5] = (data->ipMRouteBoundaryAddressMask >> 24) & 0xff;
        inst.oid_ptr[6] = (data->ipMRouteBoundaryAddressMask >> 16) & 0xff;
        inst.oid_ptr[7] = (data->ipMRouteBoundaryAddressMask >> 8) & 0xff;
        inst.oid_ptr[8] = (data->ipMRouteBoundaryAddressMask >> 0) & 0xff;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ipMRouteBoundaryStatus
    case I_ipMRouteBoundaryStatus:
       dp = &data->ipMRouteBoundaryStatus;
       break;
#endif					/* I_ipMRouteBoundaryStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the ipMRouteBoundaryEntry data object.
 *---------------------------------------------------------------------*/
static void
ipMRouteBoundaryEntry_free(ipMRouteBoundaryEntry_t *data)
{
   if (data != NULL) {
       free (data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after ipMRouteBoundaryEntry set/undo
 *---------------------------------------------------------------------*/
static int
ipMRouteBoundaryEntry_cleanup (doList_t *trash)
{
   ipMRouteBoundaryEntry_free(trash->data);
#ifdef SR_SNMPv2
   ipMRouteBoundaryEntry_free(trash->undodata);
#endif				     /* SR_SNMPv2 */
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
ipMRouteBoundaryEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int		    instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int		    found;
    int		    error_status = NO_ERROR;
    int		    carry = 0;
    ipMRouteBoundaryEntry_t *ipMRouteBoundaryEntry;
    long	    ipMRouteBoundaryIfIndex;
    unsigned long   ipMRouteBoundaryAddress;
    unsigned long   ipMRouteBoundaryAddressMask;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 9) {
        return NO_CREATION_ERROR;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &ipMRouteBoundaryIfIndex, EXACT, &carry)) < 0) {
        return NO_CREATION_ERROR;
    }

    if ((InstToIP(incoming, 1 + object->oid.length, &ipMRouteBoundaryAddress, EXACT, &carry)) < 0) {
        return NO_CREATION_ERROR;
    }

    if ((InstToIP(incoming, 5 + object->oid.length, &ipMRouteBoundaryAddressMask, EXACT, &carry)) < 0) {
        return NO_CREATION_ERROR;
    }

    ipMRouteBoundaryEntry = k_ipMRouteBoundaryEntry_get(-1, contextInfo, object->nominator, EXACT, ipMRouteBoundaryIfIndex, ipMRouteBoundaryAddress, ipMRouteBoundaryAddressMask);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ((dp->setMethod == ipMRouteBoundaryEntry_set) &&
             (((ipMRouteBoundaryEntry_t *) (dp->data)) != NULL) &&
             (((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryIfIndex == ipMRouteBoundaryIfIndex) &&
             (((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryAddress == ipMRouteBoundaryAddress) &&
             (((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryAddressMask == ipMRouteBoundaryAddressMask)) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = ipMRouteBoundaryEntry_set;
        dp->cleanupMethod = ipMRouteBoundaryEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ipMRouteBoundaryEntry_UNDO
        dp->undoMethod = ipMRouteBoundaryEntry_undo;
#else				    /* SR_ipMRouteBoundaryEntry_UNDO */
        dp->undoMethod = NULL;
#endif				    /* SR_ipMRouteBoundaryEntry_UNDO */
        dp->state = UNKNOWN;

        if (ipMRouteBoundaryEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_ipMRouteBoundaryEntry(ipMRouteBoundaryEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2
            if ((dp->undodata = (void *) Clone_ipMRouteBoundaryEntry(ipMRouteBoundaryEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef ipMRouteBoundaryEntry_READ_CREATE
	    if ((dp->data = malloc(sizeof(ipMRouteBoundaryEntry_t))) == NULL) {
		DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		error_status = RESOURCE_UNAVAILABLE_ERROR;
	    } else {
		memset(dp->data, 0, sizeof(ipMRouteBoundaryEntry_t));
#ifdef SR_SNMPv2
		dp_undodata = NULL;
#endif /* SR_SNMPv2 */		


                /* Fill in reasonable default values for this new entry */
                ((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryIfIndex = (ipMRouteBoundaryIfIndex);
                SET_VALID(I_ipMRouteBoundaryIfIndex, ((ipMRouteBoundaryEntry_t *) (dp->data))->valid);

                ((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryAddress = (ipMRouteBoundaryAddress);
                SET_VALID(I_ipMRouteBoundaryAddress, ((ipMRouteBoundaryEntry_t *) (dp->data))->valid);

                ((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryAddressMask = (ipMRouteBoundaryAddressMask);
                SET_VALID(I_ipMRouteBoundaryAddressMask, ((ipMRouteBoundaryEntry_t *) (dp->data))->valid);

                error_status = k_ipMRouteBoundaryEntry_set_defaults(dp);
            }
#else /* ipMRouteBoundaryEntry_READ_CREATE */
	    error_status = NO_ACCESS_ERROR;
#endif /* ipMRouteBoundaryEntry_READ_CREATE */
	}
#endif /* SR_SNMPv2 */
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_ipMRouteBoundaryStatus
   case I_ipMRouteBoundaryStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_ipMRouteBoundaryStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (ipMRouteBoundaryEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_ipMRouteBoundaryStatus_notInService) || (value->sl_value == D_ipMRouteBoundaryStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_ipMRouteBoundaryStatus_createAndGo) || (value->sl_value == D_ipMRouteBoundaryStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((ipMRouteBoundaryEntry_t *) (dp->data))->ipMRouteBoundaryStatus = value->sl_value;
     break;
#endif /* I_ipMRouteBoundaryStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ipMRouteBoundaryEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_ipMRouteBoundaryEntry_test */
   error_status = k_ipMRouteBoundaryEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ipMRouteBoundaryEntry_t *) (dp->data))->valid);
        error_status = k_ipMRouteBoundaryEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ipMRouteBoundaryEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_ipMRouteBoundaryEntry_set((ipMRouteBoundaryEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


ipMRoute_t     *
k_ipMRoute_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    static ipMRoute_t ipMRouteData;

    ipMRouteData.ipMRouteEnable = (ip_mrouter_running) ? 
	D_ipMRouteEnable_enabled : D_ipMRouteEnable_disabled;

    return (&ipMRouteData);
}

#ifdef SETS
int
k_ipMRoute_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_ipMRoute_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRoute_set(data, contextInfo, function)
    ipMRoute_t     *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_ipMRoute_UNDO
/* add #define SR_ipMRoute_UNDO in ipmroutemib.h to
 * include the undo routine for the ipMRoute family.
 */
int
ipMRoute_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRoute_UNDO */
#endif /* SR_SNMPv2 */

ipMRouteEntry_t *
k_ipMRouteEntry_get(serialNum, contextInfo, nominator, searchType, ipMRouteGroup, ipMRouteSource, ipMRouteSourceMask)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    unsigned long   ipMRouteGroup;
    unsigned long   ipMRouteSource;
    unsigned long   ipMRouteSourceMask;
{
    static ipMRouteEntry_t ipMRouteEntryData;
    mdbtype *mdb = NULL;
    gdbtype *gdb = NULL;

    mdb = snmp_best_ip_mroute(searchType, ipMRouteGroup, ipMRouteSource,
			      ipMRouteSourceMask, &gdb);
    if (!mdb) return(NULL);

    ipMRouteEntryData.ipMRouteGroup = mdb->group;
    if (mdb == &gdb->mdb) {
	ipMRouteEntryData.ipMRouteSource = 0;
	ipMRouteEntryData.ipMRouteSourceMask = MASK_HOST;
    } else {
	ipMRouteEntryData.ipMRouteSource = ip_radix2addr(mdb->source);
	ipMRouteEntryData.ipMRouteSourceMask = ip_radix2addr(mdb->mask);
    }
    ipMRouteEntryData.ipMRouteUpstreamNeighbor = mdb->rpf_nbr;
    if (mdb->if_input) {
	ipMRouteEntryData.ipMRouteInIfIndex = mdb->if_input->snmp_if_index ?
	    mdb->if_input->snmp_if_index : mdb->if_input->hwptr->snmp_if_index;
    } else {
	ipMRouteEntryData.ipMRouteInIfIndex = 0;
    }
    ipMRouteEntryData.ipMRouteUpTime = ELAPSED_TIME(mdb->uptime) / 10;
    ipMRouteEntryData.ipMRouteExpiryTime = 
	TIME_LEFT_SLEEPING(mdb->expires) / 10;
    ipMRouteEntryData.ciscoIpMRoutePruneFlag = 
	(MDB_OUTPUTQ_NULL(mdb)) ? D_ciscoIpMRoutePruneFlag_true :
	    D_ciscoIpMRoutePruneFlag_false;
    ipMRouteEntryData.ciscoIpMRouteSparseFlag = 
	IS_SPARSE_GROUP(gdb) ? D_ciscoIpMRouteSparseFlag_true :
	    D_ciscoIpMRouteSparseFlag_false;
    ipMRouteEntryData.ciscoIpMRouteConnectedFlag = 
	(mdb->flags & CONNECTED_FLAG) ? D_ciscoIpMRouteConnectedFlag_true :
	    D_ciscoIpMRouteConnectedFlag_false;
    ipMRouteEntryData.ciscoIpMRouteLocalFlag = 
	(mdb->flags & LOCAL_FLAG) ? D_ciscoIpMRouteLocalFlag_true :
	    D_ciscoIpMRouteLocalFlag_false;
    ipMRouteEntryData.ciscoIpMRouteRegisterFlag = 
	(mdb->flags & SET_REG_FLAG) ? D_ciscoIpMRouteRegisterFlag_true :
	    D_ciscoIpMRouteRegisterFlag_false;
    ipMRouteEntryData.ciscoIpMRouteRpFlag = 
	(mdb->flags & RP_BIT_FLAG) ? D_ciscoIpMRouteRpFlag_true :
	    D_ciscoIpMRouteRpFlag_false;
    ipMRouteEntryData.ciscoIpMRouteSptFlag = 
	(mdb->flags & SPT_FLAG) ? D_ciscoIpMRouteSptFlag_true :
	    D_ciscoIpMRouteSptFlag_false;
    ipMRouteEntryData.ciscoIpMRouteBps = mdb->bits_sec;
    ipMRouteEntryData.ciscoIpMRouteMetric = mdb->metric;
    ipMRouteEntryData.ciscoIpMRouteMetricPreference = mdb->metric_pref;
    ipMRouteEntryData.ciscoIpMRouteInLimit = mdb->in_limit;
    ipMRouteEntryData.ciscoIpMRouteLastUsed = 
	ELAPSED_TIME(mdb->lastinput_fs) / 10;
    ipMRouteEntryData.ciscoIpMRouteInLimit2 = mdb->in_limit;

    ipMRouteEntryData.ipMRoutePkts = mdb->uses;
    ipMRouteEntryData.ipMRouteDifferentInIfPackets = mdb->rpf_failed;
    ipMRouteEntryData.ipMRouteOctets = mdb->bytes;
    switch (mdb->rpf_type) {
    case RPF_STATIC:
	ipMRouteEntryData.ipMRouteProtocol = D_ipMRouteProtocol_local;
	break;
    case RPF_DVMRP:
	ipMRouteEntryData.ipMRouteProtocol = D_ipMRouteProtocol_dvmrp;
	break;
    case RPF_UNICAST:
	ipMRouteEntryData.ipMRouteProtocol = D_ipMRouteProtocol_pim;
	break;
    default:
	ipMRouteEntryData.ipMRouteProtocol = D_ipMRouteProtocol_other;
    }

    return (&ipMRouteEntryData);
}

ipMRouteNextHopEntry_t *
k_ipMRouteNextHopEntry_get(serialNum, contextInfo, nominator, searchType, ipMRouteNextHopGroup, ipMRouteNextHopSource, ipMRouteNextHopSourceMask, ipMRouteNextHopIfIndex, ipMRouteNextHopAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    unsigned long   ipMRouteNextHopGroup;
    unsigned long   ipMRouteNextHopSource;
    unsigned long   ipMRouteNextHopSourceMask;
    long            ipMRouteNextHopIfIndex;
    unsigned long   ipMRouteNextHopAddress;
{
    static ipMRouteNextHopEntry_t ipMRouteNextHopEntryData;
    static OctetString ciscoIpMRouteNextHopMacHdr;
    mdbtype *mdb;
    midbtype *best_midb = NULL;
    gdbtype *gdb = NULL;
    uint best_ifIndex = ipMRouteNextHopIfIndex;
    ndbtype *ndb;

    mdb = snmp_best_ip_mroute(EXACT, ipMRouteNextHopGroup, 
		ipMRouteNextHopSource, ipMRouteNextHopSourceMask, &gdb);
    if (mdb) {
	best_midb = snmp_best_ip_midb(searchType, mdb, &best_ifIndex,
				      (ipaddrtype)ipMRouteNextHopAddress);
    }

    if (!best_midb && searchType == NEXT) {
	/*
	 * Try next lexicographically larger route entry of the same group
	 * or the first route entry of the next lexicographically larger
	 * group entry.
	 */
	mdb = snmp_best_ip_mroute(NEXT, ipMRouteNextHopGroup,
		    ipMRouteNextHopSource, ipMRouteNextHopSourceMask, &gdb);

	while (gdb) {
	    while (mdb) {
		best_ifIndex = 0;
		best_midb = snmp_best_ip_midb(NEXT, mdb, &best_ifIndex, 0);
		if (best_midb) break;
		mdb = snmp_best_ip_mroute(NEXT, mdb->group,
			ip_radix2addr(mdb->source), ip_radix2addr(mdb->mask),
			&gdb);
	    }

	    if (best_midb) break;
	    if (!gdb) break;
	    gdb = snmp_get_ip_group(NEXT, gdb->mdb.group);
	    if (gdb) {
		mdb = &gdb->mdb;
	    }
	}
    }

    if (!best_midb) return(NULL);

    /*
     * Fill in fields from mdb and midb.
     */
    ipMRouteNextHopEntryData.ipMRouteNextHopGroup = mdb->group;
    if (mdb == &gdb->mdb) {
	ipMRouteNextHopEntryData.ipMRouteNextHopSource = 0;
	ipMRouteNextHopEntryData.ipMRouteNextHopSourceMask = MASK_HOST;
    } else {
	ipMRouteNextHopEntryData.ipMRouteNextHopSource =
	    ip_radix2addr(mdb->source);
	ipMRouteNextHopEntryData.ipMRouteNextHopSourceMask =
	    ip_radix2addr(mdb->mask);
    }
    ipMRouteNextHopEntryData.ipMRouteNextHopIfIndex = best_ifIndex;
    ipMRouteNextHopEntryData.ipMRouteNextHopAddress = best_midb->next_hop;
    ipMRouteNextHopEntryData.ipMRouteNextHopState = 
	(best_midb->state == MIDB_PRUNE_STATE) ? 
	    D_ipMRouteNextHopState_pruned : D_ipMRouteNextHopState_forwarding;
    ipMRouteNextHopEntryData.ipMRouteNextHopUpTime = 
	ELAPSED_TIME(best_midb->uptime) / 10;
    ipMRouteNextHopEntryData.ipMRouteNextHopExpiryTime =
	TIMER_RUNNING(best_midb->expires) ?
	    TIME_LEFT_SLEEPING(best_midb->expires) / 10 : 0;
    ipMRouteNextHopEntryData.ipMRouteNextHopClosestMemberHops = 1;
    ipMRouteNextHopEntryData.ipMRouteNextHopProtocol =
	D_ipMRouteNextHopProtocol_other;
    if (best_midb->next_hop && !IPMULTICAST(best_midb->next_hop)) {
	ndb = ip_rtlookup(best_midb->next_hop, MASK_HOST);
	if (ndb && ndb->routecount) {
	    ipMRouteNextHopEntryData.ipMRouteNextHopClosestMemberHops =
		ndb->rdb[0]->hopcount;
	    if (DVMRP_TUNNEL(best_midb->idb)) {
		ipMRouteNextHopEntryData.ipMRouteNextHopProtocol =
		    D_ipMRouteNextHopProtocol_dvmrp;
	    } else if (NDB2PROCTYPE(ndb) & PROC_STATIC) {
		ipMRouteNextHopEntryData.ipMRouteNextHopProtocol =
		    D_ipMRouteNextHopProtocol_local;
	    }
	}
    }
    ipMRouteNextHopEntryData.ipMRouteNextHopPkts = best_midb->uses;
    ipMRouteNextHopEntryData.ciscoIpMRouteNextHopOutLimit = 
	best_midb->out_limit;
    ipMRouteNextHopEntryData.ciscoIpMRouteNextHopMacHdr =
	&ciscoIpMRouteNextHopMacHdr;
    if (best_midb->mac_header != NULL) {
        ciscoIpMRouteNextHopMacHdr.length = best_midb->mac_length;
        ciscoIpMRouteNextHopMacHdr.octet_ptr = best_midb->mac_header;
    } else {
        ciscoIpMRouteNextHopMacHdr.length = 0;
	/* gotta point at something */
        ciscoIpMRouteNextHopMacHdr.octet_ptr =
	    (unsigned char *)&ciscoIpMRouteNextHopMacHdr;
      }

    return (&ipMRouteNextHopEntryData);
}

ipMRouteInterfaceEntry_t *
k_ipMRouteInterfaceEntry_get(serialNum, contextInfo, nominator, searchType, ipMRouteInterfaceIfIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            ipMRouteInterfaceIfIndex;
{
    static ipMRouteInterfaceEntry_t ipMRouteInterfaceEntryData;
    idbtype *swidb;

    swidb = ifmib_swidb_get(serialNum, searchType,
			    ipMRouteInterfaceIfIndex,
			    ip_multicast_enabled, NULL);
    if (!swidb) return(NULL);

    ipMRouteInterfaceEntryData.ipMRouteInterfaceIfIndex = swidb->snmp_if_index ?
	swidb->snmp_if_index : swidb->hwptr->snmp_if_index;
    ipMRouteInterfaceEntryData.ipMRouteInterfaceTtl = 
	swidb->ip_multicast_threshold;

    return (&ipMRouteInterfaceEntryData);
}

#ifdef SETS
int
k_ipMRouteInterfaceEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_ipMRouteInterfaceEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRouteInterfaceEntry_set(data, contextInfo, function)
    ipMRouteInterfaceEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    return (GEN_ERROR);
}

#endif				       /* SETS */


#ifdef SR_SNMPv2
#ifdef SR_ipMRouteInterfaceEntry_UNDO
/* add #define SR_ipMRouteInterfaceEntry_UNDO in ipmroutemib.h to
 * include the undo routine for the ipMRouteInterfaceEntry family.
 */
int
ipMRouteInterfaceEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ipMRouteInterfaceEntry_UNDO */
#endif /* SR_SNMPv2 */


ipMRouteBoundaryEntry_t *
k_ipMRouteBoundaryEntry_get(serialNum, contextInfo, nominator, searchType, ipMRouteBoundaryIfIndex, ipMRouteBoundaryAddress, ipMRouteBoundaryAddressMask)
    int		    serialNum;
    ContextInfo	   *contextInfo;
    int		    nominator;
    int		    searchType;
    long	    ipMRouteBoundaryIfIndex;
    unsigned long   ipMRouteBoundaryAddress;
    unsigned long   ipMRouteBoundaryAddressMask;
{
    static ipMRouteBoundaryEntry_t ipMRouteBoundaryEntryData;
    long index = ipMRouteBoundaryIfIndex;
    idbtype *swidb;
    ipaddrtype src = ipMRouteBoundaryAddress;
    ipaddrtype mask = ipMRouteBoundaryAddressMask;
    boolean found;

    swidb = ifmib_swidb_get(serialNum, EXACT, index,
			    ip_multicast_enabled, NULL);

    if (searchType == EXACT) {
	found = swidb && snmp_exact_ip_boundary(swidb, src, mask);
    } else {
	found = swidb && snmp_best_ip_boundary(swidb, &src, &mask, FALSE);
	index++;
	while (!found &&
	       (swidb = ifmib_swidb_get(serialNum, NEXT, index,
					ip_multicast_enabled, NULL))
	       != NULL) {
	    found = snmp_best_ip_boundary(swidb, &src, &mask, TRUE);
	    index = swidb->snmp_if_index ? swidb->snmp_if_index + 1:
					   swidb->hwptr->snmp_if_index + 1;
	}
    }

    if (!found) return(NULL);

   /*
    * put your code to retrieve the information here
    */

   ipMRouteBoundaryEntryData.ipMRouteBoundaryIfIndex = swidb->snmp_if_index ?
	swidb->snmp_if_index : swidb->hwptr->snmp_if_index;
   ipMRouteBoundaryEntryData.ipMRouteBoundaryAddress = src;
   ipMRouteBoundaryEntryData.ipMRouteBoundaryAddressMask = mask;
   ipMRouteBoundaryEntryData.ipMRouteBoundaryStatus =
	D_ipMRouteBoundaryStatus_active;
   return(&ipMRouteBoundaryEntryData);
}

#ifdef SETS
int
k_ipMRouteBoundaryEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_ipMRouteBoundaryEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipMRouteBoundaryEntry_set(data, contextInfo, function)
   ipMRouteBoundaryEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}

#endif /* SETS */


/*
 * snmp_first_node_get
 *
 * Called from rn_walktree to get first node in radix tree (smallest 
 * lexiographically) and then exit tree walk.
 */
static int snmp_first_node_get (rntype *node, va_list args)
{

    rntype **rn_ptr;

    rn_ptr = (rntype **) va_arg(args, rntype **);
    *rn_ptr = node;
    return(1);
}

/*
 * snmp_first_node
 *
 * Get first node in SNMP radix tree.
 */
static rntype *snmp_first_node (rnhtype *head)
{
    rntype *rn;

    rn = NULL;
    rn_walktree(head->rnh_treetop, snmp_first_node_get, &rn);
    return(rn);
}

/*
 * snmp_get_ip_group
 *
 * Retrieve next group from SNMP multicast radix tree.
 */
gdbtype *snmp_get_ip_group (int searchType, ipaddrtype group)
{

    rntype         *rn;
    gdbtype        *gdb;
    snmp_radixtype *snmp;
    ipradixtype    address;

    /*
     * Do exact match.
     */
    if (searchType == EXACT) {
	if (!group) return(NULL);
	ip_addr2radix(group, address);
	rn = rn_match(address, snmp_ip_group_cache);
    } else {

	/*
	 * If a get-next is requested, look for node in radix tree immediately 
	 * following this one. If address is 0.0.0.0, get first one in tree.
	 */
	if (!group) {
	    rn = snmp_first_node(snmp_ip_group_cache);
	} else {
	    rn = ip_radix_nextnode(group, snmp_ip_group_cache->rnh_treetop);
	}
    }
    if (!rn) return(NULL);

    /*
     * Group address might change if doing a get-next.
     */
    snmp = (snmp_radixtype *) rn;
    group = ip_radix2addr(snmp->group);

    /*
     * Want to return data from multicast routing table, look it up now. If
     * NULL, something is inconsistent between multicast routing table and 
     * SNMP radix cache.
     */
    gdb = ip_get_gdb(group);
    return(gdb);
}

/*
 * snmp_add_ip_group
 *
 * Add a group address to the SNMP radix tree. This is called by the IP
 * multicast routing table routines to keep it in sync with the SNMP table.
 */
void snmp_add_ip_group (ipaddrtype group)
{

    snmp_radixtype *snmp;

    /*
     * If it already exists, don't add it.
     */
    if (snmp_get_ip_group(EXACT, group)) return;

    snmp = malloc(sizeof(snmp_radixtype));
    if (!snmp) return;

    ip_addr2radix(group, snmp->group);
    ip_addr2radix(MASK_HOST, snmp->mask);
    if (!rn_addroute(snmp->group, snmp->mask, snmp_ip_group_cache, 
		     snmp->radix)) {
	free(snmp);
    }
}

/*
 * snmp_delete_ip_group
 *
 * Delete a group address from the SNMP radix tree. This is called by the IP
 * multicast routing table routines to keep it in sync with the SNMP table.
 */
void snmp_delete_ip_group (ipaddrtype group)
{

    rntype      *rn;
    ipradixtype address, mask;

    /*
     * If it doesn't exist, we can't delete it.
     */
    if (!snmp_get_ip_group(EXACT, group)) return;
    
    /*
     * Convert IP address to radix tree key format.
     */
    ip_addr2radix(group, address);
    ip_addr2radix(MASK_HOST, mask);

    /*
     * Delete from radix tree and free memory.
     */
    rn = rn_delete(address, mask, snmp_ip_group_cache);
    if (rn) free(rn);
}

/*
 * snmp_best_ip_mroute
 *
 * Get best multicast routing entry. First get best group, then get best
 * source in group.
 */
mdbtype *snmp_best_ip_mroute (int searchType, ulong group, ulong source,
			      ulong mask, gdbtype **gdb_ptr)
{
    gdbtype *gdb;
    mdbtype *mdb = NULL;

    /*
     * Get the group entry.
     */
    gdb = *gdb_ptr ? *gdb_ptr : snmp_get_ip_group(EXACT, group);
    if (searchType == EXACT) {
	/*
	 * EXACT match is just a mdb lookup or (*, G) entry.
	 */
	if (gdb) {
	    mdb = (!source && mask == MASK_HOST) ?
		  &gdb->mdb : ip_get_mdb(gdb, source, mask);
	}
    } else if (searchType == NEXT) {
	/*
	 * Get-next match is the next node in the multicast routing radix
	 * tree. If we got to an end of a group, go to next group and get
	 * (*, G) entry.
	 * Unfortunatelly, ip_next_mdb() may not return the first leaf node
	 * if source is 'less' than any routing entry. So, call the
	 * snmp_first_node() to retrieve the first node and compare it with
	 * source, if it is 'greater' than source then the first node is the
	 * candidate, otherwise call the ip_next_mdb() to get the next
	 * 'larger' route entry.
	 */
	if (gdb) {
	    mdb = (mdbtype *)snmp_first_node(gdb->radix_head);
	    if (mdb && source >= ip_radix2addr(mdb->source)) {
		mdb = ip_next_mdb(gdb, source);
	    }
	}

	if (!mdb) {
	    gdb = snmp_get_ip_group(NEXT, group);
	    if (gdb) {
		mdb = &gdb->mdb;
	    }
	}
    }

    *gdb_ptr = gdb;
    return(mdb);
}


/*
 * snmp_best_ip_midb
 *
 * Get best outgoing interface associated with the route entry.
 */
midbtype *snmp_best_ip_midb(int searchType, mdbtype *mdb, uint *ifIndex,
			    ipaddrtype next_hop)
{
    midbtype *midb, *best_midb, *best_midb2;
    uint index, best_ifIndex, best_ifIndex2;
    ipaddrtype best_next_hop, best_next_hop2;

    best_midb = best_midb2 = NULL;
    best_ifIndex = best_ifIndex2 = 0xFFFFFFFF;
    best_next_hop = best_next_hop2 = 0xFFFFFFFF;

    for (midb = mdb->if_outputQ.qhead; midb; midb = midb->next) {
	index = midb->idb->snmp_if_index ?
		midb->idb->snmp_if_index : midb->idb->hwptr->snmp_if_index;
	if (searchType == EXACT) {
	    if (index == *ifIndex && midb->next_hop == next_hop)
		return(midb);
	} else if (searchType == NEXT) {
	    if (index == *ifIndex && midb->next_hop > next_hop &&
		midb->next_hop < best_next_hop) {
		best_midb = midb;
		best_ifIndex = index;
		best_next_hop = midb->next_hop;

	    } else if (index > *ifIndex &&
		       ((index < best_ifIndex2) ||
		        (index == best_ifIndex2 && midb->next_hop < best_next_hop2))) {
		best_midb2 = midb;
		best_ifIndex2 = index;
		best_next_hop2 = midb->next_hop;
	    }
	}
    }

    *ifIndex = best_midb ? best_ifIndex : best_ifIndex2;
    return(best_midb ? best_midb : best_midb2);
}

/*
 * snmp_exact_ip_boundary
 *
 * Check if the given multicast boundary (src, mask) pair exists.
 */
boolean snmp_exact_ip_boundary (idbtype *swidb, ipaddrtype src, ipaddrtype mask)
{
    acl_headertype *acl = swidb->ipmulticast_boundary;
    fasttype *grp;
    ipaddrtype wildmask = ~mask;
    fasthash *acc;
    fastitem *item;
    int i;

    /*
     * Return if no boundary list.
     */
    if (!acl || !acl->list) return(FALSE);

    grp = (fasttype *) acl->list;
    if (wildmask == IPADDR_ZERO) {
	for (i = 0; i < NETHASHLEN; i++) {
	    acc = grp->hashlist[i];
	    while (acc) {
		/*
		 * Found if source address is the same and the
		 * action is deny.
		 */
		if (src == acc->address && !acc->grant) return(TRUE);
		acc = acc->next;
	    }
	}
    } else {
	item = grp->items;
	while (item) {
	    /*
	     * SNMP requires exact match, do not mask with src before
	     * comparing with permitmask.
	     */
	    if (item->permitmask == src && item->wildmask == wildmask &&
		!item->grant)
		return(TRUE);
	    item = item->next;
	}
    }

    return(FALSE);
}

/*
 * snmp_best_ip_boundary
 *
 * Get best boundary address pair.
 */
boolean snmp_best_ip_boundary(idbtype *swidb, ipaddrtype *src, ipaddrtype *mask,
			      boolean first)
{
    acl_headertype *acl = swidb->ipmulticast_boundary;
    fasttype *grp;
    ipaddrtype wildmask = ~*mask;
    fasthash *acc;
    fastitem *item;
    ipaddrtype best_src1, best_src2, best_src3;
    ipaddrtype best_wildmask1, best_wildmask2, best_wildmask3;
    boolean found1, found2, found3;
    int i;

    /*
     * Return if no boundary list.
     */
    if (!acl || !acl->list) return(FALSE);

    found1 = found2 = found3 = FALSE;
    best_src1 = best_src2 = best_src3 = 0xFFFFFFFF;
    best_wildmask1 = best_wildmask2 = best_wildmask3 = 0;

    /*
     * First round, search individual address list with deny action.
     */
    grp = (fasttype *) acl->list;
    for (i = 0; i < NETHASHLEN; i++) {
	acc = grp->hashlist[i];
	while (acc) {
	    if (!acc->grant && (first || acc->address > *src) &&
		acc->address < best_src1) {
		best_src1 = acc->address;
		found1 = TRUE; 
	    }
	    acc = acc->next;
	}
    }

    /*
     * Second round, search network address list with deny action.
     */
    item = grp->items;
    while (item) {
	if (!item->grant) {
	    if ((first ||
		 (item->permitmask == *src &&
		  item->wildmask < wildmask)) &&
		item->wildmask > best_wildmask2) {
		best_src2 = item->permitmask;
		best_wildmask2 = item->wildmask;
		found2 = TRUE;
	    } else if (item->permitmask > *src &&
		       ((item->permitmask < best_src3) ||
			(item->permitmask == best_src3 &&
			 item->wildmask > best_wildmask3))) {
		best_src3 = item->permitmask;
		best_wildmask3 = item->wildmask;
		found3 = TRUE;
	    }
	}
	item = item->next;
    }

    /*
     * Compare first and second round's result, return the 'smaller' one.
     */
    *src = found2 ? best_src2 : best_src3;
    *mask = found2 ? ~best_wildmask2 : ~best_wildmask3;
    if (best_src1 < *src) {
	*src = best_src1;
	*mask = ~best_wildmask1;
    }

    return(found1 || found2 || found3);
}

/*
 * ipmroutemib subsystem header
 */

#define MAJVERSION_ipmroutemib 1
#define MINVERSION_ipmroutemib 0
#define EDITVERSION_ipmroutemib 0

SUBSYS_HEADER(ipmroutemib,
              MAJVERSION_ipmroutemib,
              MINVERSION_ipmroutemib,
              EDITVERSION_ipmroutemib,
              init_ipmroutemib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: ipmulticast",
              NULL);
      
