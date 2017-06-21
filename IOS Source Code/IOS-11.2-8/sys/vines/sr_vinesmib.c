/* $Id: sr_vinesmib.c,v 3.6.18.4 1996/07/03 20:44:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/sr_vinesmib.c,v $
 *------------------------------------------------------------------
 * S R _ V I N E S M I B . C
 *
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * September 1994, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_vinesmib.c,v $
 * Revision 3.6.18.4  1996/07/03  20:44:13  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.6.18.3  1996/05/21  10:08:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.18.2  1996/05/21  06:42:53  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.18.1  1996/03/18  22:31:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/13  02:04:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  11:04:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:46:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/07  21:52:57  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/18  15:55:05  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/14  08:28:36  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/12/01  21:38:50  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.2  1995/11/17  17:57:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:34:55  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:32:27  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:13:53  hampton
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
#include <string.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../snmp/snmp_api.h"
#include "sr_vinesmib.h"
#include "sr_vinesmib-mib.h"
#include "sr_old_lvinesmib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
  
#include "vines_private.h"

#define VINES_MAXHOST 0xFFFF
#define VINES_MAXNET 0x7FFFFFFF

void
init_vinesmib (subsystype *subsys)
{
    load_mib(vinesmib_OidList, vinesmib_OidListNum);
    load_oid(vinesmib_oid_table);
    init_old_lvinesmib();
}
/*---------------------------------------------------------------------

 * Retrieve data from the cvBasic family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cvBasic_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int arg;
    void *dp;
    cvBasic_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cvBasic_get (serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */ switch (arg) {

#ifdef I_cvBasicNetwork
    case I_cvBasicNetwork:
	dp = &data->cvBasicNetwork;
	break;
#endif /* I_cvBasicNetwork */

#ifdef I_cvBasicHost
    case I_cvBasicHost:
	dp = &data->cvBasicHost;
	break;
#endif /* I_cvBasicHost */

#ifdef I_cvBasicNextClient
    case I_cvBasicNextClient:
	dp = &data->cvBasicNextClient;
	break;
#endif /* I_cvBasicNextClient */

    default:
	return ((VarBind *) NULL);

    }				       /*
				          switch 
				        */
    return (MakeVarBind (object, &ZeroOid, dp));

}
/*---------------------------------------------------------------------

 * Retrieve data from the cvForwarding family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cvForwarding_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int arg;
    void *dp;
    cvForwarding_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cvForwarding_get (serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */
    switch (arg) {

#ifdef I_cvForwNeighborNeighborCount
    case I_cvForwNeighborNeighborCount:
	dp = &data->cvForwNeighborNeighborCount;
	break;
#endif /* I_cvForwNeighborNeighborCount */

#ifdef I_cvForwNeighborPathCount
    case I_cvForwNeighborPathCount:
	dp = &data->cvForwNeighborPathCount;
	break;
#endif /* I_cvForwNeighborPathCount */

#ifdef I_cvForwNeighborVersion
    case I_cvForwNeighborVersion:
	dp = &data->cvForwNeighborVersion;
	break;
#endif /* I_cvForwNeighborVersion */

#ifdef I_cvForwRouteRouterCount
    case I_cvForwRouteRouterCount:
	dp = &data->cvForwRouteRouterCount;
	break;
#endif /* I_cvForwRouteRouterCount */

#ifdef I_cvForwRouteRouteCount
    case I_cvForwRouteRouteCount:
	dp = &data->cvForwRouteRouteCount;
	break;
#endif /* I_cvForwRouteRouteCount */

#ifdef I_cvForwRouteVersion
    case I_cvForwRouteVersion:
	dp = &data->cvForwRouteVersion;
	break;
#endif /* I_cvForwRouteVersion */

#ifdef I_cvForwRouteUpdateCountdown
    case I_cvForwRouteUpdateCountdown:
	dp = &data->cvForwRouteUpdateCountdown;
	break;
#endif /* I_cvForwRouteUpdateCountdown */

    default:
	return ((VarBind *) NULL);

    } /* switch */

    return (MakeVarBind (object, &ZeroOid, dp));
}

/*---------------------------------------------------------------------
 * Retrieve data from the cvForwNeighborEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cvForwNeighborEntry_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
/*    int instLength = incoming->length - object->oid.length; */
    int arg = object->nominator;
    void *dp;
    cvForwNeighborEntry_t *data = NULL;
    unsigned long buffer[4+STATIONLEN_MAX];
    OID inst;
    int carry;
    int i;
    long cvForwNeighborNetwork;
    int cvForwNeighborNetwork_offset;
    long cvForwNeighborHost;
    int cvForwNeighborHost_offset;
    long ifIndex;
    int ifIndex_offset;
    OctetString *cvForwNeighborPhysAddress;
    int cvForwNeighborPhysAddress_offset;
    int index;
    int final_index;

    cvForwNeighborNetwork_offset = object->oid.length;
    cvForwNeighborHost_offset = cvForwNeighborNetwork_offset + 1;
    ifIndex_offset = cvForwNeighborHost_offset + 1;
    cvForwNeighborPhysAddress_offset = ifIndex_offset + 1;
    final_index = cvForwNeighborPhysAddress_offset + GetVariableIndexLength (incoming, cvForwNeighborPhysAddress_offset);

    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }
    if ((InstToVariableOctetString (incoming, cvForwNeighborPhysAddress_offset, &cvForwNeighborPhysAddress, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, cvForwNeighborHost_offset, &cvForwNeighborHost, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, cvForwNeighborNetwork_offset, &cvForwNeighborNetwork, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cvForwNeighborEntry_get (serialNum, contextInfo, arg, searchType, cvForwNeighborNetwork, cvForwNeighborHost, ifIndex, cvForwNeighborPhysAddress)) == NULL) {
	arg = -1;
    } else {
	/*
	   * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
	inst.oid_ptr[index++] = (unsigned long) data->cvForwNeighborNetwork;
	inst.oid_ptr[index++] = (unsigned long) data->cvForwNeighborHost;
	inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
	inst.oid_ptr[index++] = (unsigned long) data->cvForwNeighborPhysAddress->length;
	for (i = 0; i < data->cvForwNeighborPhysAddress->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->cvForwNeighborPhysAddress->octet_ptr[i];
	} inst.length = index;
    } if (cvForwNeighborPhysAddress != NULL) {
	FreeOctetString (cvForwNeighborPhysAddress);
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */
    switch (arg) {

#ifdef I_cvForwNeighborSource
    case I_cvForwNeighborSource:
	dp = &data->cvForwNeighborSource;
	break;
#endif /* I_cvForwNeighborSource */

#ifdef I_cvForwNeighborRtpVersion
    case I_cvForwNeighborRtpVersion:
	dp = &data->cvForwNeighborRtpVersion;
	break;
#endif /* I_cvForwNeighborRtpVersion */

#ifdef I_cvForwNeighborUsageType
    case I_cvForwNeighborUsageType:
	dp = &data->cvForwNeighborUsageType;
	break;
#endif /* I_cvForwNeighborUsageType */

#ifdef I_cvForwNeighborAge
    case I_cvForwNeighborAge:
	dp = &data->cvForwNeighborAge;
	break;
#endif /* I_cvForwNeighborAge */

#ifdef I_cvForwNeighborMetric
    case I_cvForwNeighborMetric:
	dp = &data->cvForwNeighborMetric;
	break;
#endif /* I_cvForwNeighborMetric */

#ifdef I_cvForwNeighborUses
    case I_cvForwNeighborUses:
	dp = &data->cvForwNeighborUses;
	break;
#endif /* I_cvForwNeighborUses */

    default:
	return ((VarBind *) NULL);

    } /* switch */

    return (MakeVarBind (object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cvForwRouteEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cvForwRouteEntry_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cvForwRouteEntry_t *data = NULL;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long cvForwRouteNetworkNumber;
    long cvForwRouteNeighborNetwork;


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
    if ((InstToInt (incoming, 1 + object->oid.length, &cvForwRouteNeighborNetwork, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt (incoming, 0 + object->oid.length, &cvForwRouteNetworkNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_cvForwRouteEntry_get (serialNum, contextInfo, arg, searchType, cvForwRouteNetworkNumber, cvForwRouteNeighborNetwork)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->cvForwRouteNetworkNumber;
	inst.oid_ptr[1] = data->cvForwRouteNeighborNetwork;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */
    switch (arg) {

#ifdef I_cvForwRouteSource
    case I_cvForwRouteSource:
	dp = &data->cvForwRouteSource;
	break;
#endif /* I_cvForwRouteSource */

#ifdef I_cvForwRouteRtpVersion
    case I_cvForwRouteRtpVersion:
	dp = &data->cvForwRouteRtpVersion;
	break;
#endif /* I_cvForwRouteRtpVersion */

#ifdef I_cvForwRouteUseNext
    case I_cvForwRouteUseNext:
	dp = &data->cvForwRouteUseNext;
	break;
#endif /* I_cvForwRouteUseNext */

#ifdef I_cvForwRouteForwardBroadcast
    case I_cvForwRouteForwardBroadcast:
	dp = &data->cvForwRouteForwardBroadcast;
	break;
#endif /* I_cvForwRouteForwardBroadcast */

#ifdef I_cvForwRouteSuppress
    case I_cvForwRouteSuppress:
	dp = &data->cvForwRouteSuppress;
	break;
#endif /* I_cvForwRouteSuppress */

#ifdef I_cvForwRouteLoadShareEligible
    case I_cvForwRouteLoadShareEligible:
	dp = &data->cvForwRouteLoadShareEligible;
	break;
#endif /* I_cvForwRouteLoadShareEligible */

#ifdef I_cvForwRouteAge
    case I_cvForwRouteAge:
	dp = &data->cvForwRouteAge;
	break;
#endif /* I_cvForwRouteAge */

#ifdef I_cvForwRouteMetric
    case I_cvForwRouteMetric:
	dp = &data->cvForwRouteMetric;
	break;
#endif /* I_cvForwRouteMetric */

#ifdef I_cvForwRouteUses
    case I_cvForwRouteUses:
	dp = &data->cvForwRouteUses;
	break;
#endif /* I_cvForwRouteUses */

    default:
	return ((VarBind *) NULL);

    } /* switch */

    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the cvTotal family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ VarBind *
cvTotal_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int arg;
    void *dp;
    cvTotal_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */ 

    if ((arg == -1) || 
	(data = k_cvTotal_get (serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */ 
    switch (arg) {

#ifdef I_cvTotalInputPackets
    case I_cvTotalInputPackets:
	dp = &data->cvTotalInputPackets;
	break;
#endif /*
          I_cvTotalInputPackets 
        */

#ifdef I_cvTotalOutputPackets
    case I_cvTotalOutputPackets:
	dp = &data->cvTotalOutputPackets;
	break;
#endif /*
          I_cvTotalOutputPackets 
        */

#ifdef I_cvTotalLocalDestPackets
    case I_cvTotalLocalDestPackets:
	dp = &data->cvTotalLocalDestPackets;
	break;
#endif /*
          I_cvTotalLocalDestPackets 
        */

#ifdef I_cvTotalForwardedPackets
    case I_cvTotalForwardedPackets:
	dp = &data->cvTotalForwardedPackets;
	break;
#endif /*
          I_cvTotalForwardedPackets 
        */

#ifdef I_cvTotalBroadcastInPackets
    case I_cvTotalBroadcastInPackets:
	dp = &data->cvTotalBroadcastInPackets;
	break;
#endif /*
          I_cvTotalBroadcastInPackets 
        */

#ifdef I_cvTotalBroadcastOutPackets
    case I_cvTotalBroadcastOutPackets:
	dp = &data->cvTotalBroadcastOutPackets;
	break;
#endif /*
          I_cvTotalBroadcastOutPackets 
        */

#ifdef I_cvTotalBroadcastForwardPackets
    case I_cvTotalBroadcastForwardPackets:
	dp = &data->cvTotalBroadcastForwardPackets;
	break;
#endif /*
          I_cvTotalBroadcastForwardPackets 
        */

#ifdef I_cvTotalLanOnlyPackets
    case I_cvTotalLanOnlyPackets:
	dp = &data->cvTotalLanOnlyPackets;
	break;
#endif /*
          I_cvTotalLanOnlyPackets 
        */

#ifdef I_cvTotalNotOver4800Packets
    case I_cvTotalNotOver4800Packets:
	dp = &data->cvTotalNotOver4800Packets;
	break;
#endif /*
          I_cvTotalNotOver4800Packets 
        */

#ifdef I_cvTotalNoChargesPackets
    case I_cvTotalNoChargesPackets:
	dp = &data->cvTotalNoChargesPackets;
	break;
#endif /*
          I_cvTotalNoChargesPackets 
        */

#ifdef I_cvTotalFormatErrors
    case I_cvTotalFormatErrors:
	dp = &data->cvTotalFormatErrors;
	break;
#endif /*
          I_cvTotalFormatErrors 
        */

#ifdef I_cvTotalChecksumErrors
    case I_cvTotalChecksumErrors:
	dp = &data->cvTotalChecksumErrors;
	break;
#endif /*
          I_cvTotalChecksumErrors 
        */

#ifdef I_cvTotalHopCountsExceeded
    case I_cvTotalHopCountsExceeded:
	dp = &data->cvTotalHopCountsExceeded;
	break;
#endif /*
          I_cvTotalHopCountsExceeded 
        */

#ifdef I_cvTotalNoRouteDrops
    case I_cvTotalNoRouteDrops:
	dp = &data->cvTotalNoRouteDrops;
	break;
#endif /*
          I_cvTotalNoRouteDrops 
        */

#ifdef I_cvTotalEncapsFailedDrops
    case I_cvTotalEncapsFailedDrops:
	dp = &data->cvTotalEncapsFailedDrops;
	break;
#endif /*
          I_cvTotalEncapsFailedDrops 
        */

#ifdef I_cvTotalUnknownPackets
    case I_cvTotalUnknownPackets:
	dp = &data->cvTotalUnknownPackets;
	break;
#endif /*
          I_cvTotalUnknownPackets 
        */

#ifdef I_cvTotalIcpInPackets
    case I_cvTotalIcpInPackets:
	dp = &data->cvTotalIcpInPackets;
	break;
#endif /*
          I_cvTotalIcpInPackets 
        */

#ifdef I_cvTotalIcpOutPackets
    case I_cvTotalIcpOutPackets:
	dp = &data->cvTotalIcpOutPackets;
	break;
#endif /*
          I_cvTotalIcpOutPackets 
        */

#ifdef I_cvTotalMetricOutPackets
    case I_cvTotalMetricOutPackets:
	dp = &data->cvTotalMetricOutPackets;
	break;
#endif /*
          I_cvTotalMetricOutPackets 
        */

#ifdef I_cvTotalMacEchoInPackets
    case I_cvTotalMacEchoInPackets:
	dp = &data->cvTotalMacEchoInPackets;
	break;
#endif /*
          I_cvTotalMacEchoInPackets 
        */

#ifdef I_cvTotalMacEchoOutPackets
    case I_cvTotalMacEchoOutPackets:
	dp = &data->cvTotalMacEchoOutPackets;
	break;
#endif /*
          I_cvTotalMacEchoOutPackets 
        */

#ifdef I_cvTotalEchoInPackets
    case I_cvTotalEchoInPackets:
	dp = &data->cvTotalEchoInPackets;
	break;
#endif /*
          I_cvTotalEchoInPackets 
        */

#ifdef I_cvTotalEchoOutPackets
    case I_cvTotalEchoOutPackets:
	dp = &data->cvTotalEchoOutPackets;
	break;
#endif /*
          I_cvTotalEchoOutPackets 
        */

#ifdef I_cvTotalProxyOutPackets
    case I_cvTotalProxyOutPackets:
	dp = &data->cvTotalProxyOutPackets;
	break;
#endif /*
          I_cvTotalProxyOutPackets 
        */

#ifdef I_cvTotalProxyReplyOutPackets
    case I_cvTotalProxyReplyOutPackets:
	dp = &data->cvTotalProxyReplyOutPackets;
	break;
#endif /*
          I_cvTotalProxyReplyOutPackets 
        */

    default:
	return ((VarBind *) NULL);

    }				       /*
				          switch 
				        */
    return (MakeVarBind (object, &ZeroOid, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the cvIfConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ VarBind *
cvIfConfigEntry_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cvIfConfigEntry_t *data = NULL;
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
    } if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ if ((arg == -1) || (data = k_cvIfConfigEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	   * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       /*
				          * Build the the variable binding for the variable that will be returned.
				        */ switch (arg) {

#ifdef I_cvIfConfigMetric
    case I_cvIfConfigMetric:
	dp = &data->cvIfConfigMetric;
	break;
#endif /* I_cvIfConfigMetric */

#ifdef I_cvIfConfigEncapsulation
    case I_cvIfConfigEncapsulation:
	dp = &data->cvIfConfigEncapsulation;
	break;
#endif /* I_cvIfConfigEncapsulation */

#ifdef I_cvIfConfigAccesslist
    case I_cvIfConfigAccesslist:
	dp = &data->cvIfConfigAccesslist;
	break;
#endif /* I_cvIfConfigAccesslist */

#ifdef I_cvIfConfigPropagate
    case I_cvIfConfigPropagate:
	dp = &data->cvIfConfigPropagate;
	break;
#endif /* I_cvIfConfigPropagate */

#ifdef I_cvIfConfigArpEnabled
    case I_cvIfConfigArpEnabled:
	dp = &data->cvIfConfigArpEnabled;
	break;
#endif /* I_cvIfConfigArpEnabled */

#ifdef I_cvIfConfigServerless
    case I_cvIfConfigServerless:
	dp = &data->cvIfConfigServerless;
	break;
#endif /* I_cvIfConfigServerless */

#ifdef I_cvIfConfigRedirectInterval
    case I_cvIfConfigRedirectInterval:
	dp = &data->cvIfConfigRedirectInterval;
	break;
#endif /* I_cvIfConfigRedirectInterval */

#ifdef I_cvIfConfigSplitDisabled
    case I_cvIfConfigSplitDisabled:
	dp = &data->cvIfConfigSplitDisabled;
	break;
#endif /* I_cvIfConfigSplitDisabled */

#ifdef I_cvIfConfigLineup
    case I_cvIfConfigLineup:
	dp = &data->cvIfConfigLineup;
	break;
#endif /* I_cvIfConfigLineup */

#ifdef I_cvIfConfigFastokay
    case I_cvIfConfigFastokay:
	dp = &data->cvIfConfigFastokay;
	break;
#endif /* I_cvIfConfigFastokay */

#ifdef I_cvIfConfigRouteCache
    case I_cvIfConfigRouteCache:
	dp = &data->cvIfConfigRouteCache;
	break;
#endif /* I_cvIfConfigRouteCache */

#ifdef I_cvIfConfigInputRouterFilter
    case I_cvIfConfigInputRouterFilter:
	dp = &data->cvIfConfigInputRouterFilter;
	break;
#endif /* I_cvIfConfigInputRouterFilter */

#ifdef I_cvIfConfigInputNetworkFilter
    case I_cvIfConfigInputNetworkFilter:
	dp = &data->cvIfConfigInputNetworkFilter;
	break;
#endif /* I_cvIfConfigInputNetworkFilter */

#ifdef I_cvIfConfigOutputNetworkFilter
    case I_cvIfConfigOutputNetworkFilter:
	dp = &data->cvIfConfigOutputNetworkFilter;
	break;
#endif /* I_cvIfConfigOutputNetworkFilter */

    default:
	return ((VarBind *) NULL);

    }				       /*
				          switch 
				        */
    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the cvIfCountInEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ VarBind *
cvIfCountInEntry_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cvIfCountInEntry_t *data = NULL;
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
    } if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ if ((arg == -1) || (data = k_cvIfCountInEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	   * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       /*
				          * Build the the variable binding for the variable that will be returned.
				        */ switch (arg) {

#ifdef I_cvIfCountInNotEnabledDrops
    case I_cvIfCountInNotEnabledDrops:
	dp = &data->cvIfCountInNotEnabledDrops;
	break;
#endif /*
          I_cvIfCountInNotEnabledDrops 
        */

#ifdef I_cvIfCountInFormatErrors
    case I_cvIfCountInFormatErrors:
	dp = &data->cvIfCountInFormatErrors;
	break;
#endif /*
          I_cvIfCountInFormatErrors 
        */

#ifdef I_cvIfCountInLocalDestPackets
    case I_cvIfCountInLocalDestPackets:
	dp = &data->cvIfCountInLocalDestPackets;
	break;
#endif /*
          I_cvIfCountInLocalDestPackets 
        */

#ifdef I_cvIfCountInBroadcastPackets
    case I_cvIfCountInBroadcastPackets:
	dp = &data->cvIfCountInBroadcastPackets;
	break;
#endif /*
          I_cvIfCountInBroadcastPackets 
        */

#ifdef I_cvIfCountInForwardedPackets
    case I_cvIfCountInForwardedPackets:
	dp = &data->cvIfCountInForwardedPackets;
	break;
#endif /*
          I_cvIfCountInForwardedPackets 
        */

#ifdef I_cvIfCountInNoRouteDrops
    case I_cvIfCountInNoRouteDrops:
	dp = &data->cvIfCountInNoRouteDrops;
	break;
#endif /*
          I_cvIfCountInNoRouteDrops 
        */

#ifdef I_cvIfCountInZeroHopCountDrops
    case I_cvIfCountInZeroHopCountDrops:
	dp = &data->cvIfCountInZeroHopCountDrops;
	break;
#endif /*
          I_cvIfCountInZeroHopCountDrops 
        */

#ifdef I_cvIfCountInChecksumErrors
    case I_cvIfCountInChecksumErrors:
	dp = &data->cvIfCountInChecksumErrors;
	break;
#endif /*
          I_cvIfCountInChecksumErrors 
        */

#ifdef I_cvIfCountInArpQueryRequests
    case I_cvIfCountInArpQueryRequests:
	dp = &data->cvIfCountInArpQueryRequests;
	break;
#endif /*
          I_cvIfCountInArpQueryRequests 
        */

#ifdef I_cvIfCountInArpQueryResponses
    case I_cvIfCountInArpQueryResponses:
	dp = &data->cvIfCountInArpQueryResponses;
	break;
#endif /*
          I_cvIfCountInArpQueryResponses 
        */

#ifdef I_cvIfCountInArpAssignmentRequests
    case I_cvIfCountInArpAssignmentRequests:
	dp = &data->cvIfCountInArpAssignmentRequests;
	break;
#endif /*
          I_cvIfCountInArpAssignmentRequests 
        */

#ifdef I_cvIfCountInArpAssignmentResponses
    case I_cvIfCountInArpAssignmentResponses:
	dp = &data->cvIfCountInArpAssignmentResponses;
	break;
#endif /*
          I_cvIfCountInArpAssignmentResponses 
        */

#ifdef I_cvIfCountInArpIllegalMessages
    case I_cvIfCountInArpIllegalMessages:
	dp = &data->cvIfCountInArpIllegalMessages;
	break;
#endif /*
          I_cvIfCountInArpIllegalMessages 
        */

#ifdef I_cvIfCountInIcpErrorMessages
    case I_cvIfCountInIcpErrorMessages:
	dp = &data->cvIfCountInIcpErrorMessages;
	break;
#endif /*
          I_cvIfCountInIcpErrorMessages 
        */

#ifdef I_cvIfCountInIcpMetricMessages
    case I_cvIfCountInIcpMetricMessages:
	dp = &data->cvIfCountInIcpMetricMessages;
	break;
#endif /*
          I_cvIfCountInIcpMetricMessages 
        */

#ifdef I_cvIfCountInIcpIllegalMessages
    case I_cvIfCountInIcpIllegalMessages:
	dp = &data->cvIfCountInIcpIllegalMessages;
	break;
#endif /*
          I_cvIfCountInIcpIllegalMessages 
        */

#ifdef I_cvIfCountInIpcMessages
    case I_cvIfCountInIpcMessages:
	dp = &data->cvIfCountInIpcMessages;
	break;
#endif /*
          I_cvIfCountInIpcMessages 
        */

#ifdef I_cvIfCountInRtp0Messages
    case I_cvIfCountInRtp0Messages:
	dp = &data->cvIfCountInRtp0Messages;
	break;
#endif /*
          I_cvIfCountInRtp0Messages 
        */

#ifdef I_cvIfCountInRtp1Messages
    case I_cvIfCountInRtp1Messages:
	dp = &data->cvIfCountInRtp1Messages;
	break;
#endif /*
          I_cvIfCountInRtp1Messages 
        */

#ifdef I_cvIfCountInRtp2Messages
    case I_cvIfCountInRtp2Messages:
	dp = &data->cvIfCountInRtp2Messages;
	break;
#endif /*
          I_cvIfCountInRtp2Messages 
        */

#ifdef I_cvIfCountInRtp3Messages
    case I_cvIfCountInRtp3Messages:
	dp = &data->cvIfCountInRtp3Messages;
	break;
#endif /*
          I_cvIfCountInRtp3Messages 
        */

#ifdef I_cvIfCountInRtpUpdateMessages
    case I_cvIfCountInRtpUpdateMessages:
	dp = &data->cvIfCountInRtpUpdateMessages;
	break;
#endif /*
          I_cvIfCountInRtpUpdateMessages 
        */

#ifdef I_cvIfCountInRtpResponseMessages
    case I_cvIfCountInRtpResponseMessages:
	dp = &data->cvIfCountInRtpResponseMessages;
	break;
#endif /*
          I_cvIfCountInRtpResponseMessages 
        */

#ifdef I_cvIfCountInRtpRedirectMessages
    case I_cvIfCountInRtpRedirectMessages:
	dp = &data->cvIfCountInRtpRedirectMessages;
	break;
#endif /*
          I_cvIfCountInRtpRedirectMessages 
        */

#ifdef I_cvIfCountInRtpIllegalMessages
    case I_cvIfCountInRtpIllegalMessages:
	dp = &data->cvIfCountInRtpIllegalMessages;
	break;
#endif /*
          I_cvIfCountInRtpIllegalMessages 
        */

#ifdef I_cvIfCountInSppMessages
    case I_cvIfCountInSppMessages:
	dp = &data->cvIfCountInSppMessages;
	break;
#endif /*
          I_cvIfCountInSppMessages 
        */

#ifdef I_cvIfCountInIpUnknownProtocols
    case I_cvIfCountInIpUnknownProtocols:
	dp = &data->cvIfCountInIpUnknownProtocols;
	break;
#endif /*
          I_cvIfCountInIpUnknownProtocols 
        */

#ifdef I_cvIfCountInIpcUnknownPorts
    case I_cvIfCountInIpcUnknownPorts:
	dp = &data->cvIfCountInIpcUnknownPorts;
	break;
#endif /*
          I_cvIfCountInIpcUnknownPorts 
        */

#ifdef I_cvIfCountInBroadcastsHelpered
    case I_cvIfCountInBroadcastsHelpered:
	dp = &data->cvIfCountInBroadcastsHelpered;
	break;
#endif /*
          I_cvIfCountInBroadcastsHelpered 
        */

#ifdef I_cvIfCountInBroadcastsForwarded
    case I_cvIfCountInBroadcastsForwarded:
	dp = &data->cvIfCountInBroadcastsForwarded;
	break;
#endif /*
          I_cvIfCountInBroadcastsForwarded 
        */

#ifdef I_cvIfCountInBroadcastDuplicates
    case I_cvIfCountInBroadcastDuplicates:
	dp = &data->cvIfCountInBroadcastDuplicates;
	break;
#endif /*
          I_cvIfCountInBroadcastDuplicates 
        */

#ifdef I_cvIfCountInEchoPackets
    case I_cvIfCountInEchoPackets:
	dp = &data->cvIfCountInEchoPackets;
	break;
#endif /*
          I_cvIfCountInEchoPackets 
        */

#ifdef I_cvIfCountInMacEchoPackets
    case I_cvIfCountInMacEchoPackets:
	dp = &data->cvIfCountInMacEchoPackets;
	break;
#endif /*
          I_cvIfCountInMacEchoPackets 
        */

#ifdef I_cvIfCountInProxyReplyPackets
    case I_cvIfCountInProxyReplyPackets:
	dp = &data->cvIfCountInProxyReplyPackets;
	break;
#endif /*
          I_cvIfCountInProxyReplyPackets 
        */

    default:
	return ((VarBind *) NULL);

    }				       /*
				          switch 
				        */
    return (MakeVarBind (object, &inst, dp));

}
/*---------------------------------------------------------------------
 * Retrieve data from the cvIfCountOutEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ VarBind *
cvIfCountOutEntry_get (incoming, object, searchType, contextInfo, serialNum)
     OID *incoming;
     ObjectInfo *object;
     int searchType;
     ContextInfo *contextInfo;
     int serialNum;
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cvIfCountOutEntry_t *data = NULL;
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
    } if ((InstToInt (incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */ if ((arg == -1) || (data = k_cvIfCountOutEntry_get (serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	   * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }				       /*
				          * Build the the variable binding for the variable that will be returned.
				        */ switch (arg) {

#ifdef I_cvIfCountOutUnicastPackets
    case I_cvIfCountOutUnicastPackets:
	dp = &data->cvIfCountOutUnicastPackets;
	break;
#endif /*
          I_cvIfCountOutUnicastPackets 
        */

#ifdef I_cvIfCountOutBroadcastPackets
    case I_cvIfCountOutBroadcastPackets:
	dp = &data->cvIfCountOutBroadcastPackets;
	break;
#endif /*
          I_cvIfCountOutBroadcastPackets 
        */

#ifdef I_cvIfCountOutForwardedPackets
    case I_cvIfCountOutForwardedPackets:
	dp = &data->cvIfCountOutForwardedPackets;
	break;
#endif /*
          I_cvIfCountOutForwardedPackets 
        */

#ifdef I_cvIfCountOutEncapsulationFailures
    case I_cvIfCountOutEncapsulationFailures:
	dp = &data->cvIfCountOutEncapsulationFailures;
	break;
#endif /*
          I_cvIfCountOutEncapsulationFailures 
        */

#ifdef I_cvIfCountOutAccessFailures
    case I_cvIfCountOutAccessFailures:
	dp = &data->cvIfCountOutAccessFailures;
	break;
#endif /*
          I_cvIfCountOutAccessFailures 
        */

#ifdef I_cvIfCountOutDownFailures
    case I_cvIfCountOutDownFailures:
	dp = &data->cvIfCountOutDownFailures;
	break;
#endif /*
          I_cvIfCountOutDownFailures 
        */

#ifdef I_cvIfCountOutPacketsNotBroadcastToSource
    case I_cvIfCountOutPacketsNotBroadcastToSource:
	dp = &data->cvIfCountOutPacketsNotBroadcastToSource;
	break;
#endif /*
          I_cvIfCountOutPacketsNotBroadcastToSource 
        */

#ifdef I_cvIfCountOutPacketsNotBroadcastLanOnly
    case I_cvIfCountOutPacketsNotBroadcastLanOnly:
	dp = &data->cvIfCountOutPacketsNotBroadcastLanOnly;
	break;
#endif /*
          I_cvIfCountOutPacketsNotBroadcastLanOnly 
        */

#ifdef I_cvIfCountOutPacketsNotBroadcastNotOver4800
    case I_cvIfCountOutPacketsNotBroadcastNotOver4800:
	dp = &data->cvIfCountOutPacketsNotBroadcastNotOver4800;
	break;
#endif /*
          I_cvIfCountOutPacketsNotBroadcastNotOver4800 
        */

#ifdef I_cvIfCountOutPacketsNotBroadcastNoCharge
    case I_cvIfCountOutPacketsNotBroadcastNoCharge:
	dp = &data->cvIfCountOutPacketsNotBroadcastNoCharge;
	break;
#endif /*
          I_cvIfCountOutPacketsNotBroadcastNoCharge 
        */

#ifdef I_cvIfCountOutBroadcastsForwarded
    case I_cvIfCountOutBroadcastsForwarded:
	dp = &data->cvIfCountOutBroadcastsForwarded;
	break;
#endif /*
          I_cvIfCountOutBroadcastsForwarded 
        */

#ifdef I_cvIfCountOutBroadcastsHelpered
    case I_cvIfCountOutBroadcastsHelpered:
	dp = &data->cvIfCountOutBroadcastsHelpered;
	break;
#endif /*
          I_cvIfCountOutBroadcastsHelpered 
        */

#ifdef I_cvIfCountOutArpQueryRequests
    case I_cvIfCountOutArpQueryRequests:
	dp = &data->cvIfCountOutArpQueryRequests;
	break;
#endif /*
          I_cvIfCountOutArpQueryRequests 
        */

#ifdef I_cvIfCountOutArpQueryResponses
    case I_cvIfCountOutArpQueryResponses:
	dp = &data->cvIfCountOutArpQueryResponses;
	break;
#endif /*
          I_cvIfCountOutArpQueryResponses 
        */

#ifdef I_cvIfCountOutArpAssignmentRequests
    case I_cvIfCountOutArpAssignmentRequests:
	dp = &data->cvIfCountOutArpAssignmentRequests;
	break;
#endif /*
          I_cvIfCountOutArpAssignmentRequests 
        */

#ifdef I_cvIfCountOutArpAssignmentResponses
    case I_cvIfCountOutArpAssignmentResponses:
	dp = &data->cvIfCountOutArpAssignmentResponses;
	break;
#endif /*
          I_cvIfCountOutArpAssignmentResponses 
        */

#ifdef I_cvIfCountOutIcpErrorMessages
    case I_cvIfCountOutIcpErrorMessages:
	dp = &data->cvIfCountOutIcpErrorMessages;
	break;
#endif /*
          I_cvIfCountOutIcpErrorMessages 
        */

#ifdef I_cvIfCountOutIcpMetricMessages
    case I_cvIfCountOutIcpMetricMessages:
	dp = &data->cvIfCountOutIcpMetricMessages;
	break;
#endif /*
          I_cvIfCountOutIcpMetricMessages 
        */

#ifdef I_cvIfCountOutIpcMessages
    case I_cvIfCountOutIpcMessages:
	dp = &data->cvIfCountOutIpcMessages;
	break;
#endif /*
          I_cvIfCountOutIpcMessages 
        */

#ifdef I_cvIfCountOutRtp0Messages
    case I_cvIfCountOutRtp0Messages:
	dp = &data->cvIfCountOutRtp0Messages;
	break;
#endif /*
          I_cvIfCountOutRtp0Messages 
        */

#ifdef I_cvIfCountOutRtpRequestMessages
    case I_cvIfCountOutRtpRequestMessages:
	dp = &data->cvIfCountOutRtpRequestMessages;
	break;
#endif /*
          I_cvIfCountOutRtpRequestMessages 
        */

#ifdef I_cvIfCountOutRtp2Messages
    case I_cvIfCountOutRtp2Messages:
	dp = &data->cvIfCountOutRtp2Messages;
	break;
#endif /*
          I_cvIfCountOutRtp2Messages 
        */

#ifdef I_cvIfCountOutRtp3Messages
    case I_cvIfCountOutRtp3Messages:
	dp = &data->cvIfCountOutRtp3Messages;
	break;
#endif /*
          I_cvIfCountOutRtp3Messages 
        */

#ifdef I_cvIfCountOutRtpUpdateMessages
    case I_cvIfCountOutRtpUpdateMessages:
	dp = &data->cvIfCountOutRtpUpdateMessages;
	break;
#endif /*
          I_cvIfCountOutRtpUpdateMessages 
        */

#ifdef I_cvIfCountOutRtpResponseMessages
    case I_cvIfCountOutRtpResponseMessages:
	dp = &data->cvIfCountOutRtpResponseMessages;
	break;
#endif /*
          I_cvIfCountOutRtpResponseMessages 
        */

#ifdef I_cvIfCountOutRtpRedirectMessages
    case I_cvIfCountOutRtpRedirectMessages:
	dp = &data->cvIfCountOutRtpRedirectMessages;
	break;
#endif /*
          I_cvIfCountOutRtpRedirectMessages 
        */

#ifdef I_cvIfCountOutSppMessages
    case I_cvIfCountOutSppMessages:
	dp = &data->cvIfCountOutSppMessages;
	break;
#endif /*
          I_cvIfCountOutSppMessages 
        */

#ifdef I_cvIfCountOutEchoPackets
    case I_cvIfCountOutEchoPackets:
	dp = &data->cvIfCountOutEchoPackets;
	break;
#endif /*
          I_cvIfCountOutEchoPackets 
        */

#ifdef I_cvIfCountOutMacEchoPackets
    case I_cvIfCountOutMacEchoPackets:
	dp = &data->cvIfCountOutMacEchoPackets;
	break;
#endif /*
          I_cvIfCountOutMacEchoPackets 
        */

#ifdef I_cvIfCountOutProxyPackets
    case I_cvIfCountOutProxyPackets:
	dp = &data->cvIfCountOutProxyPackets;
	break;
#endif /*
          I_cvIfCountOutProxyPackets 
        */

    default:
	return ((VarBind *) NULL);

    }				       /*
				          switch 
				        */
    return (MakeVarBind (object, &inst, dp));

}

/*
 * Below are the kernel routines called by the SNMP research code.
 *
 * All executable, hand-written code should be here or below.
 */

/*
 * Convert metric to straight milleseconds.
 */

static ulong metric_to_mill (ulong metric)
{
    ulong result;

    /* low 4 bits * 12.5 ms each, round up */
    result = ((((metric & 0xF) * 125) + 5) / 10);

    /* merge high 12 bits * 200 ms each */
    result += ((metric / 16) * 200);

    return (result);
}

cvBasic_t *
k_cvBasic_get (serialNum, contextInfo, nominator)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
{
    static cvBasic_t cvBasicData;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    cvBasicData.cvBasicNetwork = (long) vines_network;

    cvBasicData.cvBasicHost = (long) vines_host;

    cvBasicData.cvBasicNextClient = (long) vines_client;

    return (&cvBasicData);
}

cvForwarding_t *
k_cvForwarding_get (serialNum, contextInfo, nominator)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
{
    static cvForwarding_t cvForwardingData;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    cvForwardingData.cvForwNeighborNeighborCount = 
	vines_neighbor_tree->activeNodes;
    cvForwardingData.cvForwNeighborPathCount = vines_paths;
    cvForwardingData.cvForwNeighborVersion = vines_neighbor_version;
    cvForwardingData.cvForwRouteRouterCount = vines_network_tree->activeNodes;
    cvForwardingData.cvForwRouteRouteCount = vines_routes;
    cvForwardingData.cvForwRouteVersion = vines_net_version;
    cvForwardingData.cvForwRouteUpdateCountdown =
	mgd_timer_left_sleeping(&vines_idb_timer) / ONESEC;

    return (&cvForwardingData);
}

static int
compare_phys (int len1, unsigned char *str1, int len2, unsigned char *str2)
{
    /* this is a lexical, octet-string compare, so lengths count first */
    if (len1 < len2)
	return (-1);
    if (len1 > len2)
	return (1);

    return memcmp(str1, str2, len1);

/*    for (i=0; (i < len1) && (str1[i] == str2[i]); i++);

    if (i>=len1)
	return (0);
    if (str1[i] < str2[i])
	return (-1);
    return (1); */
}

cvForwNeighborEntry_t *
k_cvForwNeighborEntry_get (serialNum, contextInfo, nominator, searchType, cvForwNeighborNetwork, cvForwNeighborHost, ifIndex, cvForwNeighborPhysAddress)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
     int searchType;
     long cvForwNeighborNetwork;
     long cvForwNeighborHost;
     long ifIndex;
     OctetString *cvForwNeighborPhysAddress;
{
    static cvForwNeighborEntry_t cvForwNeighborEntryData;
    static OctetString physAddress;
    static uchar address_bytes[STATIONLEN_MAX];
    vinesneighbortype *neighbor;
    vinespathtype *path, *p;
    int match;
    long NeighborNetwork;
    long NeighborHost;
    long NeighborIfIndex;
    OctetString NeighborPhysAddress;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    NeighborNetwork = cvForwNeighborNetwork;
    NeighborHost = cvForwNeighborHost;
    NeighborIfIndex = ifIndex;
    NeighborPhysAddress.length = cvForwNeighborPhysAddress->length;
    NeighborPhysAddress.octet_ptr = cvForwNeighborPhysAddress->octet_ptr;
    path = NULL;
    while (!path) {
	/* find best neighbor */
	neighbor = vneigh_GetBestNeighbor
	    (vneigh_EntryKey (NeighborNetwork, NeighborHost));

	if (neighbor == NULL
	    || ((searchType == EXACT)
		&& ((neighbor->net != cvForwNeighborNetwork)
		    || (neighbor->host != cvForwNeighborHost))))
	    return (NULL);

	/* find path with best interface and physical address */
	FOR_ALL_PATHS(neighbor, p) {
	    if (p->idb->phys_idb->hwptr->snmp_if_index < NeighborIfIndex)
		continue;
	    if (p->idb->phys_idb->hwptr->snmp_if_index == NeighborIfIndex) {
		match = compare_phys (p->dlc_addr.length,
				      p->dlc_addr.addr,
				      NeighborPhysAddress.length,
				      NeighborPhysAddress.octet_ptr);
		if (match < 0)
		    continue;
		if (match == 0) {
		    /* got an exact match, all done searching */
		    path = p;
		    break;
		}
	    }

	    /* allow for best non-match */
	    if (searchType != EXACT) {
		if (path) {
		    if (p->idb->phys_idb->hwptr->snmp_if_index
			> path->idb->phys_idb->hwptr->snmp_if_index)
			continue;
		    if (p->idb->phys_idb->hwptr->snmp_if_index
			== path->idb->phys_idb->hwptr->snmp_if_index) {
			match = compare_phys (p->dlc_addr.length,
					      p->dlc_addr.addr,
					      path->dlc_addr.length,
					      path->dlc_addr.addr);
			if (match > 0)
			    continue;
		    }
		}
		path = p;
	    }
	}

	if ((path == NULL && NeighborHost != cvForwNeighborHost)
	    || ((searchType == EXACT) && (path != p)))
	    return (NULL);

	/* try next, but only once */
	if (NeighborHost < VINES_MAXHOST) {
	    NeighborHost++;
	} else {
	    if (NeighborNetwork < VINES_MAXNET) {
		NeighborNetwork++;
		NeighborHost = 0;
	    } else {
		return (NULL);
	    }
	}
	NeighborIfIndex = 0;
	NeighborPhysAddress.length = 0;
	NeighborPhysAddress.octet_ptr = NULL;
    }

    /* extract return values */

    cvForwNeighborEntryData.cvForwNeighborNetwork = neighbor->net;

    cvForwNeighborEntryData.cvForwNeighborHost = neighbor->host;

    cvForwNeighborEntryData.ifIndex =
	path->idb->phys_idb->hwptr->snmp_if_index;

    physAddress.length = min(path->dlc_addr.length, STATIONLEN_MAX);
    bcopy(path->dlc_addr.addr, &address_bytes, physAddress.length);
    physAddress.octet_ptr = address_bytes;
    cvForwNeighborEntryData.cvForwNeighborPhysAddress = &physAddress;

    if (path->flags & V_N_SELF)
	cvForwNeighborEntryData.cvForwNeighborSource = 
	    D_cvForwNeighborSource_self;
    else if (path->flags & V_N_REDIRECT)
	cvForwNeighborEntryData.cvForwNeighborSource = 
	    D_cvForwNeighborSource_rtpRedirect;
    else if (path->flags & V_N_DYNAMIC)
	cvForwNeighborEntryData.cvForwNeighborSource =
	    D_cvForwNeighborSource_rtpUpdate;
    else if (path->flags & V_N_STATIC)
	cvForwNeighborEntryData.cvForwNeighborSource =
	    D_cvForwNeighborSource_manualRoute;
    else if (path->flags & V_N_IGRP)
	cvForwNeighborEntryData.cvForwNeighborSource =
	    D_cvForwNeighborSource_igrp;
    else if (path->flags & V_N_TEST)
	cvForwNeighborEntryData.cvForwNeighborSource =
	    D_cvForwNeighborSource_test;
    else if (path->flags & V_N_PLACEHOLDER)
	cvForwNeighborEntryData.cvForwNeighborSource =
	    D_cvForwNeighborSource_manualNeighbor;
    else
	cvForwNeighborEntryData.cvForwNeighborSource = 
	    D_cvForwNeighborSource_unrecognized;

    cvForwNeighborEntryData.cvForwNeighborRtpVersion = neighbor->version;

    if (path == neighbor->best_path) {
	cvForwNeighborEntryData.cvForwNeighborUsageType = 
	    D_cvForwNeighborUsageType_next;
    } else {
	cvForwNeighborEntryData.cvForwNeighborUsageType =
	    (path->metric == neighbor->metric) ? 
		D_cvForwNeighborUsageType_roundRobin :
		    D_cvForwNeighborUsageType_backup;
    }

    if ((path->flags & V_N_STATIC)
	|| ((neighbor->version == VRTP_VERSION)
	&& vines_no_updates(path->idb)))
	cvForwNeighborEntryData.cvForwNeighborAge = -1;
    else
	cvForwNeighborEntryData.cvForwNeighborAge =
	    min(ELAPSED_TIME(path->timer)/ONESEC,999);

    cvForwNeighborEntryData.cvForwNeighborMetric =
	metric_to_mill (path->metric);

    cvForwNeighborEntryData.cvForwNeighborUses = path->uses;

    return (&cvForwNeighborEntryData);
}

cvForwRouteEntry_t *
k_cvForwRouteEntry_get (serialNum, contextInfo, nominator, searchType, cvForwRouteNetworkNumber, cvForwRouteNeighborNetwork)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
     int searchType;
     long cvForwRouteNetworkNumber;
     long cvForwRouteNeighborNetwork;
{
    static cvForwRouteEntry_t cvForwRouteEntryData;
    vinesnettype *net;
    vinesroutetype *route;
    long NetworkNumber;
    long NeighborNetwork;
    ushort metric;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    NetworkNumber = cvForwRouteNetworkNumber;
    NeighborNetwork = cvForwRouteNeighborNetwork;
    route = NULL;
    while (!route) {
	/* find best network */
	net = vroute_GetBestNetwork
	    (vroute_EntryKey (NetworkNumber));

	if (net == NULL
	    || ((searchType == EXACT) 
		&& (net->net != cvForwRouteNetworkNumber)))
	    return (NULL);

	/* find best neighbor network */
	FOR_ALL_ROUTES(net, route) {
	    if (route->gw->net >= NeighborNetwork) {
		break;
	    }
	}

	if ((route == NULL && NetworkNumber != cvForwRouteNetworkNumber)
	    || ((searchType == EXACT)
		&& (route->gw->net != cvForwRouteNeighborNetwork)))
	    return (NULL);

	/* try next, but only once */
	if (NetworkNumber < VINES_MAXNET) {
	    NetworkNumber++;
	    NeighborNetwork = 0;
	} else {
	    return (NULL);
	}
    }

    /* extract return values */

    cvForwRouteEntryData.cvForwRouteNetworkNumber = net->net;
    cvForwRouteEntryData.cvForwRouteNeighborNetwork = route->gw->net;

    if (route->flags & V_NET_ME)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_self;
    else if (route->flags & V_NET_REDIRECT)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_rtpRedirect;
    else if (route->flags & V_NET_RTP)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_rtpUpdate;
    else if (route->flags & V_NET_STATIC)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_manualRoute;
    else if (route->flags & V_NET_IGRP)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_igrp;
    else if (route->flags & V_NET_TEST)
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_test;
    else
	cvForwRouteEntryData.cvForwRouteSource =
	    D_cvForwRouteSource_unrecognized;

    cvForwRouteEntryData.cvForwRouteRtpVersion = route->gw->version;

    cvForwRouteEntryData.cvForwRouteUseNext =
	(route == net->best_route) ? 
	    D_cvForwRouteUseNext_true : D_cvForwRouteUseNext_false;

    cvForwRouteEntryData.cvForwRouteForwardBroadcast =
	(net->net == vines_nearest_server_net ()) ?
	    D_cvForwRouteForwardBroadcast_true :
		D_cvForwRouteForwardBroadcast_false;

    cvForwRouteEntryData.cvForwRouteSuppress = 
	(TIMER_RUNNING(net->suppresstime)) ?
	    D_cvForwRouteSuppress_true : D_cvForwRouteSuppress_false;

    metric = min(VINES_INFINITY, route->metric + route->gw->metric);
    cvForwRouteEntryData.cvForwRouteMetric = metric_to_mill (metric);

    cvForwRouteEntryData.cvForwRouteLoadShareEligible =
	(metric == net->actual_metric) ?
	    D_cvForwRouteLoadShareEligible_true :
		D_cvForwRouteLoadShareEligible_false;

    if ((route->flags & V_NET_STATIC)
	|| (route->gw->version == VSRTP_VERSION)
	|| vines_best_path_from_delta_or_snapshot(route))
	cvForwRouteEntryData.cvForwRouteAge = -1;
    else
	cvForwRouteEntryData.cvForwRouteAge = 
	    min(ELAPSED_TIME(route->timer)/ONESEC,999);

    cvForwRouteEntryData.cvForwRouteUses = route->uses;

    return (&cvForwRouteEntryData);
}

cvTotal_t *
k_cvTotal_get (serialNum, contextInfo, nominator)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
{
    static cvTotal_t cvTotalData;
    vinesidbtype *vidb;
    hwidbtype *idb;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    memset(&cvTotalData, 0, sizeof(cvTotalData));
    idb = NULL;

    FOR_ALL_HWIDBS(idb) {
	cvTotalData.cvTotalInputPackets += idb->counters.rx_frames[ACCT_PROTO_VINES];
	cvTotalData.cvTotalOutputPackets += idb->counters.tx_frames[ACCT_PROTO_VINES];
    }

    FOR_ALL_VIDBS(vidb) {
	cvTotalData.cvTotalLocalDestPackets += (long) vidb->rx_local_dest;
	cvTotalData.cvTotalForwardedPackets += (long) vidb->rx_forwarded;
	cvTotalData.cvTotalBroadcastInPackets += (long) vidb->rx_bcast_in;
	cvTotalData.cvTotalBroadcastOutPackets += (long) vidb->tx_bcast;
	cvTotalData.cvTotalBroadcastForwardPackets +=
	    (long) vidb->rx_bcast_forwarded;
	cvTotalData.cvTotalLanOnlyPackets += (long) vidb->tx_notbcast_notlan;
	cvTotalData.cvTotalNotOver4800Packets +=
	    (long) vidb->tx_notbcast_notgt4800;
	cvTotalData.cvTotalNoChargesPackets +=
	    (long) vidb->tx_notbcast_ppcharge;
	cvTotalData.cvTotalFormatErrors += (long) (vidb->rx_too_small
						   + vidb->rx_too_large 
						   + vidb->rx_bad_size);
	cvTotalData.cvTotalChecksumErrors += (long) vidb->rx_checksum_error;
	cvTotalData.cvTotalHopCountsExceeded += (long) vidb->rx_zero_hop_count;
	cvTotalData.cvTotalNoRouteDrops += (long) vidb->rx_no_route;
	cvTotalData.cvTotalEncapsFailedDrops += (long) vidb->tx_failed_encaps;
	cvTotalData.cvTotalUnknownPackets +=
	    (long) (vidb->rx_ip_unknown + vidb->rx_ipc_unknown);
	cvTotalData.cvTotalIcpInPackets += 
	    (long) (vidb->rx_icp_error + vidb->rx_icp_metric);
	cvTotalData.cvTotalIcpOutPackets += (long) vidb->tx_icp_error;
	cvTotalData.cvTotalMetricOutPackets += (long) vidb->tx_icp_metric;
	cvTotalData.cvTotalMacEchoInPackets += (long) vidb->rx_echo;
	cvTotalData.cvTotalMacEchoOutPackets += (long) vidb->tx_mac_echo;
	cvTotalData.cvTotalEchoInPackets += (long) vidb->rx_echo;
	cvTotalData.cvTotalEchoOutPackets += (long) vidb->tx_echo;
	cvTotalData.cvTotalProxyOutPackets += vidb->tx_proxy;
	cvTotalData.cvTotalProxyReplyOutPackets += vidb->rx_proxy_reply;
    }

    return (&cvTotalData);
}

static boolean idb_tester (hwidbtype* idb, long* dummy)
{
   return (idb->firstsw != NULL && 
	   vines_getidb(idb->firstsw) != NULL);
}

cvIfConfigEntry_t *
k_cvIfConfigEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
     int searchType;
     long ifIndex;
{
    static cvIfConfigEntry_t cvIfConfigEntryData;
    hwidbtype *idb;
    vinesidbtype *vidb;

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    idb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			  idb_tester, NULL);
    if (idb == NULL) 
	return(NULL);

    vidb = vines_getidb(idb->firstsw);
    if (!vidb)
	return(NULL);

    /* extract return values */

    cvIfConfigEntryData.ifIndex = idb->snmp_if_index;

    cvIfConfigEntryData.cvIfConfigMetric = 
	metric_to_mill ((long) vidb->vines_metric);

    if (vidb->vines_enctype == ET_ARPA) {
	cvIfConfigEntryData.cvIfConfigEncapsulation = 
	    D_cvIfConfigEncapsulation_arpa;
    } else if (vidb->vines_enctype == ET_VINES_TR) {
	cvIfConfigEntryData.cvIfConfigEncapsulation = 
	    D_cvIfConfigEncapsulation_tokenRing;
    } else {
	cvIfConfigEntryData.cvIfConfigEncapsulation = 
	    D_cvIfConfigEncapsulation_snap;
    }
   
    cvIfConfigEntryData.cvIfConfigAccesslist = 
	(long) vidb->vines_accesslist;

    if (vidb->propagate == VPROP_DYNAMIC) {
	cvIfConfigEntryData.cvIfConfigPropagate = 
	    D_cvIfConfigPropagate_dynamic;
    } else if (vidb->propagate == VPROP_ALWAYS) {
	cvIfConfigEntryData.cvIfConfigPropagate = 
	    D_cvIfConfigPropagate_always;
    } else {
	cvIfConfigEntryData.cvIfConfigPropagate = 
	    D_cvIfConfigPropagate_never;
    }

    if (vidb->arp_enabled == VARP_DYNAMIC) {
	cvIfConfigEntryData.cvIfConfigArpEnabled = 
	    D_cvIfConfigArpEnabled_dynamic;
    } else if (vidb->arp_enabled == VARP_ALWAYS) {
	cvIfConfigEntryData.cvIfConfigArpEnabled = 
	    D_cvIfConfigArpEnabled_always;
    } else {
	cvIfConfigEntryData.cvIfConfigArpEnabled = 
	    D_cvIfConfigArpEnabled_never;
    }

    if (vidb->serverless == VSVLESS_ALWAYS_BCAST) {
	cvIfConfigEntryData.cvIfConfigServerless = 
	    D_cvIfConfigServerless_alwaysBroadcast;
    } else if (vidb->serverless == VSVLESS_DYNAMIC) {
	cvIfConfigEntryData.cvIfConfigServerless = 
	    D_cvIfConfigServerless_dynamic;
    } else if (vidb->serverless == VSVLESS_ALWAYS) {
	cvIfConfigEntryData.cvIfConfigServerless = 
	    D_cvIfConfigServerless_always;
    } else {
	cvIfConfigEntryData.cvIfConfigServerless = 
	    D_cvIfConfigServerless_never;
    }

    cvIfConfigEntryData.cvIfConfigRedirectInterval = 
	(long) vidb->vines_redirect_interval;

    cvIfConfigEntryData.cvIfConfigSplitDisabled = 
	(vidb->vines_split_disabled) ?
	    D_cvIfConfigSplitDisabled_true :
		D_cvIfConfigSplitDisabled_false;

    cvIfConfigEntryData.cvIfConfigLineup = 
	(vidb->vines_lineup) ?
	    D_cvIfConfigLineup_true :
		D_cvIfConfigLineup_false;

    cvIfConfigEntryData.cvIfConfigFastokay = 
	(idb->vines_fastokay) ?
	    D_cvIfConfigFastokay_true :
		D_cvIfConfigFastokay_false;

    cvIfConfigEntryData.cvIfConfigRouteCache = 
	(idb->vines_routecache) ?
	    D_cvIfConfigRouteCache_true :
		D_cvIfConfigRouteCache_false;

    cvIfConfigEntryData.cvIfConfigInputRouterFilter = 
	vidb->input_router_filter;

    cvIfConfigEntryData.cvIfConfigInputNetworkFilter = 
	vidb->input_network_filter;

    cvIfConfigEntryData.cvIfConfigOutputNetworkFilter = 
	vidb->output_network_filter;

    return (&cvIfConfigEntryData);
}

cvIfCountInEntry_t *
k_cvIfCountInEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
     int searchType;
     long ifIndex;
{
    static cvIfCountInEntry_t cvIfCountInEntryData;
    hwidbtype *idb;
    vinesidbtype *vidb;
#define ARP0CNT_IDX 0
#define ARP1CNT_IDX 1
#define ARP2CNT_IDX 2
#define ARP3CNT_IDX 3
#define ARP4CNT_IDX 4
#define ARP5CNT_IDX 5
#define ARP6CNT_IDX 6

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    idb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			  idb_tester, NULL);
    if (idb == NULL) 
	return(NULL);

    vidb = vines_getidb(idb->firstsw);
    if (!vidb)
	return(NULL);

    /* extract return values */

    cvIfCountInEntryData.ifIndex = idb->snmp_if_index;

    cvIfCountInEntryData.cvIfCountInNotEnabledDrops = vidb->rx_not_enabled;
    cvIfCountInEntryData.cvIfCountInFormatErrors = (vidb->rx_too_small
						    + vidb->rx_too_large 
						    + vidb->rx_bad_size);
    cvIfCountInEntryData.cvIfCountInLocalDestPackets = vidb->rx_local_dest;
    cvIfCountInEntryData.cvIfCountInBroadcastPackets = vidb->rx_bcast_in;
    cvIfCountInEntryData.cvIfCountInForwardedPackets = vidb->rx_forwarded;
    cvIfCountInEntryData.cvIfCountInNoRouteDrops = vidb->rx_no_route;
    cvIfCountInEntryData.cvIfCountInZeroHopCountDrops = vidb->rx_zero_hop_count;
    cvIfCountInEntryData.cvIfCountInChecksumErrors = 
	vidb->rx_checksum_error;
    cvIfCountInEntryData.cvIfCountInArpQueryRequests = 
	vidb->rx_arpv0_type[ARP0CNT_IDX] + vidb->rx_arpv1_type[ARP0CNT_IDX];
    cvIfCountInEntryData.cvIfCountInArpQueryResponses = 
	vidb->rx_arpv0_type[ARP1CNT_IDX] + vidb->rx_arpv1_type[ARP1CNT_IDX];
    cvIfCountInEntryData.cvIfCountInArpAssignmentRequests = 
	vidb->rx_arpv0_type[ARP2CNT_IDX] + vidb->rx_arpv1_type[ARP2CNT_IDX];
    cvIfCountInEntryData.cvIfCountInArpAssignmentResponses = 
	vidb->rx_arpv0_type[ARP3CNT_IDX] + vidb->rx_arpv1_type[ARP3CNT_IDX];
    cvIfCountInEntryData.cvIfCountInArpIllegalMessages = 
	vidb->rx_arpv0_illegal + vidb->rx_arpv1_illegal;
    cvIfCountInEntryData.cvIfCountInIcpErrorMessages = vidb->rx_icp_error;
    cvIfCountInEntryData.cvIfCountInIcpMetricMessages = vidb->rx_icp_metric;
    cvIfCountInEntryData.cvIfCountInIcpIllegalMessages = vidb->rx_icp_illegal;
    cvIfCountInEntryData.cvIfCountInIpcMessages = vidb->rx_ipc;
    cvIfCountInEntryData.cvIfCountInRtp0Messages =
	vidb->rx_rtpv0_type[ARP0CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtp1Messages =
	vidb->rx_rtpv0_type[ARP1CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtp2Messages =
	vidb->rx_rtpv0_type[ARP2CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtp3Messages =
	vidb->rx_rtpv0_type[ARP3CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtpUpdateMessages = 
	vidb->rx_rtpv0_type[ARP4CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtpResponseMessages =
	vidb->rx_rtpv0_type[ARP5CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtpRedirectMessages =
	vidb->rx_rtpv0_type[ARP6CNT_IDX];
    cvIfCountInEntryData.cvIfCountInRtpIllegalMessages = vidb->rx_rtpv0_illegal;
    cvIfCountInEntryData.cvIfCountInSppMessages = vidb->rx_spp;
    cvIfCountInEntryData.cvIfCountInIpUnknownProtocols =
	vidb->rx_ip_unknown;
    cvIfCountInEntryData.cvIfCountInIpcUnknownPorts = vidb->rx_ipc_unknown;
    cvIfCountInEntryData.cvIfCountInBroadcastsHelpered = 
	vidb->rx_bcast_helpered;
    cvIfCountInEntryData.cvIfCountInBroadcastsForwarded = 
	vidb->rx_bcast_forwarded;
    cvIfCountInEntryData.cvIfCountInBroadcastDuplicates = 
	vidb->rx_bcast_duplicate;
    cvIfCountInEntryData.cvIfCountInEchoPackets = vidb->rx_echo;
    cvIfCountInEntryData.cvIfCountInMacEchoPackets = vidb->rx_mac_echo;
    cvIfCountInEntryData.cvIfCountInProxyReplyPackets = vidb->rx_proxy_reply;

    return (&cvIfCountInEntryData);
}

cvIfCountOutEntry_t *
k_cvIfCountOutEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex)
     int serialNum;
     ContextInfo *contextInfo;
     int nominator;
     int searchType;
     long ifIndex;
{
    static cvIfCountOutEntry_t cvIfCountOutEntryData;
    hwidbtype *idb;
    vinesidbtype *vidb;
#define ARP0CNT_IDX 0
#define ARP1CNT_IDX 1
#define ARP2CNT_IDX 2
#define ARP3CNT_IDX 3
#define ARP4CNT_IDX 4
#define ARP5CNT_IDX 5
#define ARP6CNT_IDX 6

    /* if VINES isn't running, pretend nothing's here */
    if (!vines_running)
	return (NULL);

    idb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			  idb_tester, NULL);
    if (idb == NULL) 
	return(NULL);

    vidb = vines_getidb(idb->firstsw);
    if (!vidb)
	return(NULL);

    /* extract return values */

    cvIfCountOutEntryData.ifIndex = idb->snmp_if_index;

    cvIfCountOutEntryData.cvIfCountOutUnicastPackets = vidb->tx_unicast;
    cvIfCountOutEntryData.cvIfCountOutBroadcastPackets = vidb->tx_bcast;
    cvIfCountOutEntryData.cvIfCountOutForwardedPackets = vidb->tx_forwarded;
    cvIfCountOutEntryData.cvIfCountOutEncapsulationFailures = 
	vidb->tx_failed_encaps;
    cvIfCountOutEntryData.cvIfCountOutAccessFailures = 
	vidb->tx_failed_access;
    cvIfCountOutEntryData.cvIfCountOutDownFailures = 
	vidb->tx_failed_down;
    cvIfCountOutEntryData.cvIfCountOutPacketsNotBroadcastToSource = 
	vidb->tx_notbcast_toward_source;
    cvIfCountOutEntryData.cvIfCountOutPacketsNotBroadcastLanOnly = 
	vidb->tx_notbcast_notlan;
    cvIfCountOutEntryData.cvIfCountOutPacketsNotBroadcastNotOver4800 = 
	vidb->tx_notbcast_notgt4800;
    cvIfCountOutEntryData.cvIfCountOutPacketsNotBroadcastNoCharge = 
	vidb->tx_notbcast_ppcharge;
    cvIfCountOutEntryData.cvIfCountOutBroadcastsForwarded = 
	vidb->tx_bcast_forwarded;
    cvIfCountOutEntryData.cvIfCountOutBroadcastsHelpered = 
	vidb->tx_bcast_helpered;
    cvIfCountOutEntryData.cvIfCountOutArpQueryRequests =
	vidb->tx_arpv0_type[ARP0CNT_IDX] + vidb->tx_arpv1_type[ARP0CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutArpQueryResponses =
	vidb->tx_arpv0_type[ARP1CNT_IDX] + vidb->tx_arpv1_type[ARP1CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutArpAssignmentRequests =
	vidb->tx_arpv0_type[ARP2CNT_IDX] + vidb->tx_arpv1_type[ARP2CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutArpAssignmentResponses =
	vidb->tx_arpv0_type[ARP3CNT_IDX] + vidb->tx_arpv1_type[ARP3CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutIcpErrorMessages = vidb->tx_icp_error;
    cvIfCountOutEntryData.cvIfCountOutIcpMetricMessages = vidb->tx_icp_metric;
    cvIfCountOutEntryData.cvIfCountOutIpcMessages = vidb->tx_ipc;
    cvIfCountOutEntryData.cvIfCountOutRtp0Messages =
	vidb->tx_rtpv0_type[ARP0CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtpRequestMessages =
	vidb->tx_rtpv0_type[ARP1CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtp2Messages =
	vidb->tx_rtpv0_type[ARP2CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtp3Messages =
	vidb->tx_rtpv0_type[ARP3CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtpUpdateMessages =
	vidb->tx_rtpv0_type[ARP4CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtpResponseMessages =
	vidb->tx_rtpv0_type[ARP5CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutRtpRedirectMessages =
	vidb->tx_rtpv0_type[ARP6CNT_IDX];
    cvIfCountOutEntryData.cvIfCountOutSppMessages = vidb->tx_spp;
    cvIfCountOutEntryData.cvIfCountOutEchoPackets = vidb->tx_echo;
    cvIfCountOutEntryData.cvIfCountOutMacEchoPackets = vidb->tx_mac_echo;
    cvIfCountOutEntryData.cvIfCountOutProxyPackets = vidb->tx_proxy;

    return (&cvIfCountOutEntryData);
}


/*
 * vinesmib subsystem header
 */

#define MAJVERSION_vinesmib 1
#define MINVERSION_vinesmib 0
#define EDITVERSION_vinesmib 0

SUBSYS_HEADER(vinesmib,
              MAJVERSION_vinesmib,
              MINVERSION_vinesmib,
              EDITVERSION_vinesmib,
              init_vinesmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: vines",
              NULL);
      
