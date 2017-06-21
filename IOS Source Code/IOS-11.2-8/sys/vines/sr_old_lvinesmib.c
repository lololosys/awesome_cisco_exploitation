/* $Id: sr_old_lvinesmib.c,v 3.5.10.4 1996/07/03 20:44:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/sr_old_lvinesmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Chifei W. Cheng (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lvinesmib.c,v $
 * Revision 3.5.10.4  1996/07/03  20:44:10  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.10.3  1996/05/21  10:08:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/05/21  06:42:51  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.10.1  1996/03/18  22:31:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.4  1996/03/16  07:52:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.3  1996/03/13  02:04:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.2  1996/03/07  11:04:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  21:46:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/21  21:23:46  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1995/12/14  08:28:35  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/12/01  21:38:48  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.2  1995/11/17  17:57:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  07:34:53  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.1  1995/06/07  23:13:49  hampton
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
#include "interface_private.h"
#include "packet.h"
#include "vines_private.h"
#include "../snmp/snmp_api.h"
#include "sr_old_lvinesmib.h"
#include "sr_old_lvinesmib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"


void
init_old_lvinesmib(void)
{
    load_mib(old_lvinesmib_OidList, old_lvinesmib_OidListNum);
    load_oid(old_lvinesmib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the tmpvines family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
tmpvines_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    tmpvines_t     *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_tmpvines_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_vinesInput
      case I_vinesInput:
	dp = &data->vinesInput;
	break;
#endif				       /* I_vinesInput */

#ifdef I_vinesOutput
      case I_vinesOutput:
	dp = &data->vinesOutput;
	break;
#endif				       /* I_vinesOutput */

#ifdef I_vinesLocaldest
      case I_vinesLocaldest:
	dp = &data->vinesLocaldest;
	break;
#endif				       /* I_vinesLocaldest */

#ifdef I_vinesForwarded
      case I_vinesForwarded:
	dp = &data->vinesForwarded;
	break;
#endif				       /* I_vinesForwarded */

#ifdef I_vinesBcastin
      case I_vinesBcastin:
	dp = &data->vinesBcastin;
	break;
#endif				       /* I_vinesBcastin */

#ifdef I_vinesBcastout
      case I_vinesBcastout:
	dp = &data->vinesBcastout;
	break;
#endif				       /* I_vinesBcastout */

#ifdef I_vinesBcastfwd
      case I_vinesBcastfwd:
	dp = &data->vinesBcastfwd;
	break;
#endif				       /* I_vinesBcastfwd */

#ifdef I_vinesNotlan
      case I_vinesNotlan:
	dp = &data->vinesNotlan;
	break;
#endif				       /* I_vinesNotlan */

#ifdef I_vinesNotgt4800
      case I_vinesNotgt4800:
	dp = &data->vinesNotgt4800;
	break;
#endif				       /* I_vinesNotgt4800 */

#ifdef I_vinesNocharges
      case I_vinesNocharges:
	dp = &data->vinesNocharges;
	break;
#endif				       /* I_vinesNocharges */

#ifdef I_vinesFormaterror
      case I_vinesFormaterror:
	dp = &data->vinesFormaterror;
	break;
#endif				       /* I_vinesFormaterror */

#ifdef I_vinesCksumerr
      case I_vinesCksumerr:
	dp = &data->vinesCksumerr;
	break;
#endif				       /* I_vinesCksumerr */

#ifdef I_vinesHopcount
      case I_vinesHopcount:
	dp = &data->vinesHopcount;
	break;
#endif				       /* I_vinesHopcount */

#ifdef I_vinesNoroute
      case I_vinesNoroute:
	dp = &data->vinesNoroute;
	break;
#endif				       /* I_vinesNoroute */

#ifdef I_vinesEncapsfailed
      case I_vinesEncapsfailed:
	dp = &data->vinesEncapsfailed;
	break;
#endif				       /* I_vinesEncapsfailed */

#ifdef I_vinesUnknown
      case I_vinesUnknown:
	dp = &data->vinesUnknown;
	break;
#endif				       /* I_vinesUnknown */

#ifdef I_vinesIcpIn
      case I_vinesIcpIn:
	dp = &data->vinesIcpIn;
	break;
#endif				       /* I_vinesIcpIn */

#ifdef I_vinesIcpOut
      case I_vinesIcpOut:
	dp = &data->vinesIcpOut;
	break;
#endif				       /* I_vinesIcpOut */

#ifdef I_vinesMetricOut
      case I_vinesMetricOut:
	dp = &data->vinesMetricOut;
	break;
#endif				       /* I_vinesMetricOut */

#ifdef I_vinesMacEchoIn
      case I_vinesMacEchoIn:
	dp = &data->vinesMacEchoIn;
	break;
#endif				       /* I_vinesMacEchoIn */

#ifdef I_vinesMacEchoOut
      case I_vinesMacEchoOut:
	dp = &data->vinesMacEchoOut;
	break;
#endif				       /* I_vinesMacEchoOut */

#ifdef I_vinesEchoIn
      case I_vinesEchoIn:
	dp = &data->vinesEchoIn;
	break;
#endif				       /* I_vinesEchoIn */

#ifdef I_vinesEchoOut
      case I_vinesEchoOut:
	dp = &data->vinesEchoOut;
	break;
#endif				       /* I_vinesEchoOut */

#ifdef I_vinesProxyCnt
      case I_vinesProxyCnt:
	dp = &data->vinesProxyCnt;
	break;
#endif				       /* I_vinesProxyCnt */

#ifdef I_vinesProxyReplyCnt
      case I_vinesProxyReplyCnt:
	dp = &data->vinesProxyReplyCnt;
	break;
#endif				       /* I_vinesProxyReplyCnt */

#ifdef I_vinesNet
      case I_vinesNet:
	dp = &data->vinesNet;
	break;
#endif				       /* I_vinesNet */

#ifdef I_vinesSubNet
      case I_vinesSubNet:
	dp = &data->vinesSubNet;
	break;
#endif				       /* I_vinesSubNet */

#ifdef I_vinesClient
      case I_vinesClient:
	dp = &data->vinesClient;
	break;
#endif				       /* I_vinesClient */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the vinesIfTableEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
vinesIfTableEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    vinesIfTableEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ifIndex;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_vinesIfTableEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
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

#ifdef I_vinesIfMetric
      case I_vinesIfMetric:
	dp = &data->vinesIfMetric;
	break;
#endif				       /* I_vinesIfMetric */

#ifdef I_vinesIfEnctype
      case I_vinesIfEnctype:
	dp = &data->vinesIfEnctype;
	break;
#endif				       /* I_vinesIfEnctype */

#ifdef I_vinesIfAccesslist
      case I_vinesIfAccesslist:
	dp = &data->vinesIfAccesslist;
	break;
#endif				       /* I_vinesIfAccesslist */

#ifdef I_vinesIfPropagate
      case I_vinesIfPropagate:
	dp = &data->vinesIfPropagate;
	break;
#endif				       /* I_vinesIfPropagate */

#ifdef I_vinesIfArpEnabled
      case I_vinesIfArpEnabled:
	dp = &data->vinesIfArpEnabled;
	break;
#endif				       /* I_vinesIfArpEnabled */

#ifdef I_vinesIfServerless
      case I_vinesIfServerless:
	dp = &data->vinesIfServerless;
	break;
#endif				       /* I_vinesIfServerless */

#ifdef I_vinesIfRedirectInterval
      case I_vinesIfRedirectInterval:
	dp = &data->vinesIfRedirectInterval;
	break;
#endif				       /* I_vinesIfRedirectInterval */

#ifdef I_vinesIfSplitDisabled
      case I_vinesIfSplitDisabled:
	dp = &data->vinesIfSplitDisabled;
	break;
#endif				       /* I_vinesIfSplitDisabled */

#ifdef I_vinesIfLineup
      case I_vinesIfLineup:
	dp = &data->vinesIfLineup;
	break;
#endif				       /* I_vinesIfLineup */

#ifdef I_vinesIfFastokay
      case I_vinesIfFastokay:
	dp = &data->vinesIfFastokay;
	break;
#endif				       /* I_vinesIfFastokay */

#ifdef I_vinesIfRouteCache
      case I_vinesIfRouteCache:
	dp = &data->vinesIfRouteCache;
	break;
#endif				       /* I_vinesIfRouteCache */

#ifdef I_vinesIfRxNotEnabledCnt
      case I_vinesIfRxNotEnabledCnt:
	dp = &data->vinesIfRxNotEnabledCnt;
	break;
#endif				       /* I_vinesIfRxNotEnabledCnt */

#ifdef I_vinesIfRxFormatErrorCnt
      case I_vinesIfRxFormatErrorCnt:
	dp = &data->vinesIfRxFormatErrorCnt;
	break;
#endif				       /* I_vinesIfRxFormatErrorCnt */

#ifdef I_vinesIfRxLocalDestCnt
      case I_vinesIfRxLocalDestCnt:
	dp = &data->vinesIfRxLocalDestCnt;
	break;
#endif				       /* I_vinesIfRxLocalDestCnt */

#ifdef I_vinesIfRxBcastinCnt
      case I_vinesIfRxBcastinCnt:
	dp = &data->vinesIfRxBcastinCnt;
	break;
#endif				       /* I_vinesIfRxBcastinCnt */

#ifdef I_vinesIfRxForwardedCnt
      case I_vinesIfRxForwardedCnt:
	dp = &data->vinesIfRxForwardedCnt;
	break;
#endif				       /* I_vinesIfRxForwardedCnt */

#ifdef I_vinesIfRxNoRouteCnt
      case I_vinesIfRxNoRouteCnt:
	dp = &data->vinesIfRxNoRouteCnt;
	break;
#endif				       /* I_vinesIfRxNoRouteCnt */

#ifdef I_vinesIfRxZeroHopCountCnt
      case I_vinesIfRxZeroHopCountCnt:
	dp = &data->vinesIfRxZeroHopCountCnt;
	break;
#endif				       /* I_vinesIfRxZeroHopCountCnt */

#ifdef I_vinesIfRxChecksumErrorCnt
      case I_vinesIfRxChecksumErrorCnt:
	dp = &data->vinesIfRxChecksumErrorCnt;
	break;
#endif				       /* I_vinesIfRxChecksumErrorCnt */

#ifdef I_vinesIfRxArp0Cnt
      case I_vinesIfRxArp0Cnt:
	dp = &data->vinesIfRxArp0Cnt;
	break;
#endif				       /* I_vinesIfRxArp0Cnt */

#ifdef I_vinesIfRxArp1Cnt
      case I_vinesIfRxArp1Cnt:
	dp = &data->vinesIfRxArp1Cnt;
	break;
#endif				       /* I_vinesIfRxArp1Cnt */

#ifdef I_vinesIfRxArp2Cnt
      case I_vinesIfRxArp2Cnt:
	dp = &data->vinesIfRxArp2Cnt;
	break;
#endif				       /* I_vinesIfRxArp2Cnt */

#ifdef I_vinesIfRxArp3Cnt
      case I_vinesIfRxArp3Cnt:
	dp = &data->vinesIfRxArp3Cnt;
	break;
#endif				       /* I_vinesIfRxArp3Cnt */

#ifdef I_vinesIfRxArpIllegalCnt
      case I_vinesIfRxArpIllegalCnt:
	dp = &data->vinesIfRxArpIllegalCnt;
	break;
#endif				       /* I_vinesIfRxArpIllegalCnt */

#ifdef I_vinesIfRxIcpErrorCnt
      case I_vinesIfRxIcpErrorCnt:
	dp = &data->vinesIfRxIcpErrorCnt;
	break;
#endif				       /* I_vinesIfRxIcpErrorCnt */

#ifdef I_vinesIfRxIcpMetricCnt
      case I_vinesIfRxIcpMetricCnt:
	dp = &data->vinesIfRxIcpMetricCnt;
	break;
#endif				       /* I_vinesIfRxIcpMetricCnt */

#ifdef I_vinesIfRxIcpIllegalCnt
      case I_vinesIfRxIcpIllegalCnt:
	dp = &data->vinesIfRxIcpIllegalCnt;
	break;
#endif				       /* I_vinesIfRxIcpIllegalCnt */

#ifdef I_vinesIfRxIpcCnt
      case I_vinesIfRxIpcCnt:
	dp = &data->vinesIfRxIpcCnt;
	break;
#endif				       /* I_vinesIfRxIpcCnt */

#ifdef I_vinesIfRxRtp0Cnt
      case I_vinesIfRxRtp0Cnt:
	dp = &data->vinesIfRxRtp0Cnt;
	break;
#endif				       /* I_vinesIfRxRtp0Cnt */

#ifdef I_vinesIfRxRtp1Cnt
      case I_vinesIfRxRtp1Cnt:
	dp = &data->vinesIfRxRtp1Cnt;
	break;
#endif				       /* I_vinesIfRxRtp1Cnt */

#ifdef I_vinesIfRxRtp2Cnt
      case I_vinesIfRxRtp2Cnt:
	dp = &data->vinesIfRxRtp2Cnt;
	break;
#endif				       /* I_vinesIfRxRtp2Cnt */

#ifdef I_vinesIfRxRtp3Cnt
      case I_vinesIfRxRtp3Cnt:
	dp = &data->vinesIfRxRtp3Cnt;
	break;
#endif				       /* I_vinesIfRxRtp3Cnt */

#ifdef I_vinesIfRxRtp4Cnt
      case I_vinesIfRxRtp4Cnt:
	dp = &data->vinesIfRxRtp4Cnt;
	break;
#endif				       /* I_vinesIfRxRtp4Cnt */

#ifdef I_vinesIfRxRtp5Cnt
      case I_vinesIfRxRtp5Cnt:
	dp = &data->vinesIfRxRtp5Cnt;
	break;
#endif				       /* I_vinesIfRxRtp5Cnt */

#ifdef I_vinesIfRxRtp6Cnt
      case I_vinesIfRxRtp6Cnt:
	dp = &data->vinesIfRxRtp6Cnt;
	break;
#endif				       /* I_vinesIfRxRtp6Cnt */

#ifdef I_vinesIfRxRtpIllegalCnt
      case I_vinesIfRxRtpIllegalCnt:
	dp = &data->vinesIfRxRtpIllegalCnt;
	break;
#endif				       /* I_vinesIfRxRtpIllegalCnt */

#ifdef I_vinesIfRxSppCnt
      case I_vinesIfRxSppCnt:
	dp = &data->vinesIfRxSppCnt;
	break;
#endif				       /* I_vinesIfRxSppCnt */

#ifdef I_vinesIfRxIpUnknownCnt
      case I_vinesIfRxIpUnknownCnt:
	dp = &data->vinesIfRxIpUnknownCnt;
	break;
#endif				       /* I_vinesIfRxIpUnknownCnt */

#ifdef I_vinesIfRxIpcUnknownCnt
      case I_vinesIfRxIpcUnknownCnt:
	dp = &data->vinesIfRxIpcUnknownCnt;
	break;
#endif				       /* I_vinesIfRxIpcUnknownCnt */

#ifdef I_vinesIfRxBcastHelperedCnt
      case I_vinesIfRxBcastHelperedCnt:
	dp = &data->vinesIfRxBcastHelperedCnt;
	break;
#endif				       /* I_vinesIfRxBcastHelperedCnt */

#ifdef I_vinesIfRxBcastForwardedCnt
      case I_vinesIfRxBcastForwardedCnt:
	dp = &data->vinesIfRxBcastForwardedCnt;
	break;
#endif				       /* I_vinesIfRxBcastForwardedCnt */

#ifdef I_vinesIfRxBcastDuplicateCnt
      case I_vinesIfRxBcastDuplicateCnt:
	dp = &data->vinesIfRxBcastDuplicateCnt;
	break;
#endif				       /* I_vinesIfRxBcastDuplicateCnt */

#ifdef I_vinesIfRxEchoCnt
      case I_vinesIfRxEchoCnt:
	dp = &data->vinesIfRxEchoCnt;
	break;
#endif				       /* I_vinesIfRxEchoCnt */

#ifdef I_vinesIfRxMacEchoCnt
      case I_vinesIfRxMacEchoCnt:
	dp = &data->vinesIfRxMacEchoCnt;
	break;
#endif				       /* I_vinesIfRxMacEchoCnt */

#ifdef I_vinesIfRxProxyReplyCnt
      case I_vinesIfRxProxyReplyCnt:
	dp = &data->vinesIfRxProxyReplyCnt;
	break;
#endif				       /* I_vinesIfRxProxyReplyCnt */

#ifdef I_vinesIfTxUnicastCnt
      case I_vinesIfTxUnicastCnt:
	dp = &data->vinesIfTxUnicastCnt;
	break;
#endif				       /* I_vinesIfTxUnicastCnt */

#ifdef I_vinesIfTxBcastCnt
      case I_vinesIfTxBcastCnt:
	dp = &data->vinesIfTxBcastCnt;
	break;
#endif				       /* I_vinesIfTxBcastCnt */

#ifdef I_vinesIfTxForwardedCnt
      case I_vinesIfTxForwardedCnt:
	dp = &data->vinesIfTxForwardedCnt;
	break;
#endif				       /* I_vinesIfTxForwardedCnt */

#ifdef I_vinesIfTxFailedEncapsCnt
      case I_vinesIfTxFailedEncapsCnt:
	dp = &data->vinesIfTxFailedEncapsCnt;
	break;
#endif				       /* I_vinesIfTxFailedEncapsCnt */

#ifdef I_vinesIfTxFailedAccessCnt
      case I_vinesIfTxFailedAccessCnt:
	dp = &data->vinesIfTxFailedAccessCnt;
	break;
#endif				       /* I_vinesIfTxFailedAccessCnt */

#ifdef I_vinesIfTxFailedDownCnt
      case I_vinesIfTxFailedDownCnt:
	dp = &data->vinesIfTxFailedDownCnt;
	break;
#endif				       /* I_vinesIfTxFailedDownCnt */

#ifdef I_vinesIfTxNotBcastToSourceCnt
      case I_vinesIfTxNotBcastToSourceCnt:
	dp = &data->vinesIfTxNotBcastToSourceCnt;
	break;
#endif				       /* I_vinesIfTxNotBcastToSourceCnt */

#ifdef I_vinesIfTxNotBcastNotlanCnt
      case I_vinesIfTxNotBcastNotlanCnt:
	dp = &data->vinesIfTxNotBcastNotlanCnt;
	break;
#endif				       /* I_vinesIfTxNotBcastNotlanCnt */

#ifdef I_vinesIfTxNotBcastNotgt4800Cnt
      case I_vinesIfTxNotBcastNotgt4800Cnt:
	dp = &data->vinesIfTxNotBcastNotgt4800Cnt;
	break;
#endif				       /* I_vinesIfTxNotBcastNotgt4800Cnt */

#ifdef I_vinesIfTxNotBcastPpchargeCnt
      case I_vinesIfTxNotBcastPpchargeCnt:
	dp = &data->vinesIfTxNotBcastPpchargeCnt;
	break;
#endif				       /* I_vinesIfTxNotBcastPpchargeCnt */

#ifdef I_vinesIfTxBcastForwardedCnt
      case I_vinesIfTxBcastForwardedCnt:
	dp = &data->vinesIfTxBcastForwardedCnt;
	break;
#endif				       /* I_vinesIfTxBcastForwardedCnt */

#ifdef I_vinesIfTxBcastHelperedCnt
      case I_vinesIfTxBcastHelperedCnt:
	dp = &data->vinesIfTxBcastHelperedCnt;
	break;
#endif				       /* I_vinesIfTxBcastHelperedCnt */

#ifdef I_vinesIfTxArp0Cnt
      case I_vinesIfTxArp0Cnt:
	dp = &data->vinesIfTxArp0Cnt;
	break;
#endif				       /* I_vinesIfTxArp0Cnt */

#ifdef I_vinesIfTxArp1Cnt
      case I_vinesIfTxArp1Cnt:
	dp = &data->vinesIfTxArp1Cnt;
	break;
#endif				       /* I_vinesIfTxArp1Cnt */

#ifdef I_vinesIfTxArp2Cnt
      case I_vinesIfTxArp2Cnt:
	dp = &data->vinesIfTxArp2Cnt;
	break;
#endif				       /* I_vinesIfTxArp2Cnt */

#ifdef I_vinesIfTxArp3Cnt
      case I_vinesIfTxArp3Cnt:
	dp = &data->vinesIfTxArp3Cnt;
	break;
#endif				       /* I_vinesIfTxArp3Cnt */

#ifdef I_vinesIfTxIcpErrorCnt
      case I_vinesIfTxIcpErrorCnt:
	dp = &data->vinesIfTxIcpErrorCnt;
	break;
#endif				       /* I_vinesIfTxIcpErrorCnt */

#ifdef I_vinesIfTxIcpMetricCnt
      case I_vinesIfTxIcpMetricCnt:
	dp = &data->vinesIfTxIcpMetricCnt;
	break;
#endif				       /* I_vinesIfTxIcpMetricCnt */

#ifdef I_vinesIfTxIpcCnt
      case I_vinesIfTxIpcCnt:
	dp = &data->vinesIfTxIpcCnt;
	break;
#endif				       /* I_vinesIfTxIpcCnt */

#ifdef I_vinesIfTxRtp0Cnt
      case I_vinesIfTxRtp0Cnt:
	dp = &data->vinesIfTxRtp0Cnt;
	break;
#endif				       /* I_vinesIfTxRtp0Cnt */

#ifdef I_vinesIfTxRtp1Cnt
      case I_vinesIfTxRtp1Cnt:
	dp = &data->vinesIfTxRtp1Cnt;
	break;
#endif				       /* I_vinesIfTxRtp1Cnt */

#ifdef I_vinesIfTxRtp2Cnt
      case I_vinesIfTxRtp2Cnt:
	dp = &data->vinesIfTxRtp2Cnt;
	break;
#endif				       /* I_vinesIfTxRtp2Cnt */

#ifdef I_vinesIfTxRtp3Cnt
      case I_vinesIfTxRtp3Cnt:
	dp = &data->vinesIfTxRtp3Cnt;
	break;
#endif				       /* I_vinesIfTxRtp3Cnt */

#ifdef I_vinesIfTxRtp4Cnt
      case I_vinesIfTxRtp4Cnt:
	dp = &data->vinesIfTxRtp4Cnt;
	break;
#endif				       /* I_vinesIfTxRtp4Cnt */

#ifdef I_vinesIfTxRtp5Cnt
      case I_vinesIfTxRtp5Cnt:
	dp = &data->vinesIfTxRtp5Cnt;
	break;
#endif				       /* I_vinesIfTxRtp5Cnt */

#ifdef I_vinesIfTxRtp6Cnt
      case I_vinesIfTxRtp6Cnt:
	dp = &data->vinesIfTxRtp6Cnt;
	break;
#endif				       /* I_vinesIfTxRtp6Cnt */

#ifdef I_vinesIfTxSppCnt
      case I_vinesIfTxSppCnt:
	dp = &data->vinesIfTxSppCnt;
	break;
#endif				       /* I_vinesIfTxSppCnt */

#ifdef I_vinesIfTxEchoCnt
      case I_vinesIfTxEchoCnt:
	dp = &data->vinesIfTxEchoCnt;
	break;
#endif				       /* I_vinesIfTxEchoCnt */

#ifdef I_vinesIfTxMacEchoCnt
      case I_vinesIfTxMacEchoCnt:
	dp = &data->vinesIfTxMacEchoCnt;
	break;
#endif				       /* I_vinesIfTxMacEchoCnt */

#ifdef I_vinesIfTxProxyCnt
      case I_vinesIfTxProxyCnt:
	dp = &data->vinesIfTxProxyCnt;
	break;
#endif				       /* I_vinesIfTxProxyCnt */

#ifdef I_vinesIfInputRouterFilter
      case I_vinesIfInputRouterFilter:
	dp = &data->vinesIfInputRouterFilter;
	break;
#endif				       /* I_vinesIfInputRouterFilter */

#ifdef I_vinesIfInputNetworkFilter
      case I_vinesIfInputNetworkFilter:
	dp = &data->vinesIfInputNetworkFilter;
	break;
#endif				       /* I_vinesIfInputNetworkFilter */

#ifdef I_vinesIfOutputNetworkFilter
      case I_vinesIfOutputNetworkFilter:
	dp = &data->vinesIfOutputNetworkFilter;
	break;
#endif				       /* I_vinesIfOutputNetworkFilter */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}





tmpvines_t     *
k_tmpvines_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static tmpvines_t tmpvinesData;
    vinesidbtype *vidb;
    hwidbtype *idb;

    if (!vines_running)
	return(NULL);
    memset(&tmpvinesData, 0, sizeof(tmpvinesData));
    idb = NULL;
    FOR_ALL_HWIDBS(idb) {
	tmpvinesData.vinesInput += idb->counters.rx_frames[ACCT_PROTO_VINES];
	tmpvinesData.vinesOutput += idb->counters.tx_frames[ACCT_PROTO_VINES];
    }
    FOR_ALL_VIDBS(vidb) {
	tmpvinesData.vinesLocaldest += (long) vidb->rx_local_dest;
	tmpvinesData.vinesForwarded += (long) vidb->rx_forwarded;
	tmpvinesData.vinesBcastin += (long) vidb->rx_bcast_in;
	tmpvinesData.vinesBcastout += (long) vidb->tx_bcast;
	tmpvinesData.vinesBcastfwd += (long) vidb->rx_bcast_forwarded;
	tmpvinesData.vinesNotlan += (long) vidb->tx_notbcast_notlan;
	tmpvinesData.vinesNotgt4800 += (long) vidb->tx_notbcast_notgt4800;
	tmpvinesData.vinesNocharges += (long) vidb->tx_notbcast_ppcharge;
	tmpvinesData.vinesFormaterror += (long) (vidb->rx_too_small +
						 vidb->rx_too_large +
						 vidb->rx_bad_size);
	tmpvinesData.vinesCksumerr += (long) vidb->rx_checksum_error;
	tmpvinesData.vinesHopcount += (long) vidb->rx_zero_hop_count;
	tmpvinesData.vinesNoroute += (long) vidb->rx_no_route;
	tmpvinesData.vinesEncapsfailed += (long) vidb->tx_failed_encaps;
	tmpvinesData.vinesUnknown += (long) (vidb->rx_ip_unknown + vidb->rx_ipc_unknown);
	tmpvinesData.vinesIcpIn += (long) (vidb->rx_icp_error + vidb->rx_icp_metric);
	tmpvinesData.vinesIcpOut += (long) vidb->tx_icp_error;
	tmpvinesData.vinesMetricOut += (long) vidb->tx_icp_metric;
	tmpvinesData.vinesMacEchoIn += (long) vidb->rx_echo;
	tmpvinesData.vinesMacEchoOut += (long) vidb->tx_mac_echo;
	tmpvinesData.vinesEchoIn += (long) vidb->rx_echo;
	tmpvinesData.vinesEchoOut += (long) vidb->tx_echo;
	tmpvinesData.vinesProxyCnt += vidb->tx_proxy;
	tmpvinesData.vinesProxyReplyCnt += vidb->rx_proxy_reply;
    }
    tmpvinesData.vinesNet = (long) vines_network;
    tmpvinesData.vinesSubNet = (long) vines_host;
    tmpvinesData.vinesClient = (long) vines_client;
    return (&tmpvinesData);
}

static boolean idb_tester (hwidbtype* idb, long* dummy)
{
   return (idb->firstsw != NULL && 
	   vines_getidb(idb->firstsw) != NULL);
}
vinesIfTableEntry_t *
k_vinesIfTableEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            ifIndex)
{
    static vinesIfTableEntry_t vinesIfTableEntryData;
    hwidbtype *idb;
    vinesidbtype *vidb;
#define ARP0CNT_IDX 0
#define ARP1CNT_IDX 1
#define ARP2CNT_IDX 2
#define ARP3CNT_IDX 3
#define ARP4CNT_IDX 4
#define ARP5CNT_IDX 5
#define ARP6CNT_IDX 6

    if (!vines_running)
	return(NULL);
    idb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			  idb_tester, NULL);
    if (idb == NULL) 
	return(NULL);

    vidb = vines_getidb(idb->firstsw);
    if (!vidb)
	return(NULL);
    vinesIfTableEntryData.ifIndex = (long) idb->snmp_if_index;
    vinesIfTableEntryData.vinesIfMetric = (long) vidb->vines_metric;
    vinesIfTableEntryData.vinesIfEnctype = (long) vidb->vines_enctype;
    vinesIfTableEntryData.vinesIfAccesslist = (long) vidb->vines_accesslist;
    vinesIfTableEntryData.vinesIfPropagate = (long) vidb->propagate;
    vinesIfTableEntryData.vinesIfArpEnabled = (long) vidb->arp_enabled;
    vinesIfTableEntryData.vinesIfServerless = (long) vidb->serverless;
    vinesIfTableEntryData.vinesIfRedirectInterval = (long) vidb->vines_redirect_interval;
    vinesIfTableEntryData.vinesIfSplitDisabled = (long) vidb->vines_split_disabled;
    vinesIfTableEntryData.vinesIfLineup = (long) vidb->vines_lineup;
    vinesIfTableEntryData.vinesIfFastokay = (long) idb->vines_fastokay;
    vinesIfTableEntryData.vinesIfRouteCache = (long) idb->vines_routecache;
    vinesIfTableEntryData.vinesIfRxNotEnabledCnt = vidb->rx_not_enabled;
    vinesIfTableEntryData.vinesIfRxFormatErrorCnt = (vidb->rx_too_small +
						     vidb->rx_too_large +
						     vidb->rx_bad_size);
    vinesIfTableEntryData.vinesIfRxLocalDestCnt = vidb->rx_local_dest;
    vinesIfTableEntryData.vinesIfRxBcastinCnt = vidb->rx_bcast_in;
    vinesIfTableEntryData.vinesIfRxForwardedCnt = vidb->rx_forwarded;
    vinesIfTableEntryData.vinesIfRxNoRouteCnt = vidb->rx_no_route;
    vinesIfTableEntryData.vinesIfRxZeroHopCountCnt = vidb->rx_zero_hop_count;
    vinesIfTableEntryData.vinesIfRxChecksumErrorCnt = vidb->rx_checksum_error;
    vinesIfTableEntryData.vinesIfRxArp0Cnt = 
	vidb->rx_arpv0_type[ARP0CNT_IDX] + vidb->rx_arpv1_type[ARP0CNT_IDX];
    vinesIfTableEntryData.vinesIfRxArp1Cnt = 
	vidb->rx_arpv0_type[ARP1CNT_IDX] + vidb->rx_arpv1_type[ARP1CNT_IDX];
    vinesIfTableEntryData.vinesIfRxArp2Cnt = 
	vidb->rx_arpv0_type[ARP2CNT_IDX] + vidb->rx_arpv1_type[ARP2CNT_IDX];
    vinesIfTableEntryData.vinesIfRxArp3Cnt = 
	vidb->rx_arpv0_type[ARP3CNT_IDX] + vidb->rx_arpv1_type[ARP3CNT_IDX];
    vinesIfTableEntryData.vinesIfRxArpIllegalCnt = 
	vidb->rx_arpv0_illegal + vidb->rx_arpv1_illegal;
    vinesIfTableEntryData.vinesIfRxIcpErrorCnt = vidb->rx_icp_error;
    vinesIfTableEntryData.vinesIfRxIcpMetricCnt = vidb->rx_icp_metric;
    vinesIfTableEntryData.vinesIfRxIcpIllegalCnt = vidb->rx_icp_illegal;
    vinesIfTableEntryData.vinesIfRxIpcCnt = vidb->rx_ipc;
    vinesIfTableEntryData.vinesIfRxRtp0Cnt = vidb->rx_rtpv0_type[ARP0CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp1Cnt = vidb->rx_rtpv0_type[ARP1CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp2Cnt = vidb->rx_rtpv0_type[ARP2CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp3Cnt = vidb->rx_rtpv0_type[ARP3CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp4Cnt = vidb->rx_rtpv0_type[ARP4CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp5Cnt = vidb->rx_rtpv0_type[ARP5CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtp6Cnt = vidb->rx_rtpv0_type[ARP6CNT_IDX];
    vinesIfTableEntryData.vinesIfRxRtpIllegalCnt = vidb->rx_rtpv0_illegal;
    vinesIfTableEntryData.vinesIfRxSppCnt = vidb->rx_spp;
    vinesIfTableEntryData.vinesIfRxIpUnknownCnt = vidb->rx_ip_unknown;
    vinesIfTableEntryData.vinesIfRxIpcUnknownCnt = vidb->rx_ipc_unknown;
    vinesIfTableEntryData.vinesIfRxBcastHelperedCnt = vidb->rx_bcast_helpered;
    vinesIfTableEntryData.vinesIfRxBcastForwardedCnt = vidb->rx_bcast_forwarded;
    vinesIfTableEntryData.vinesIfRxBcastDuplicateCnt = vidb->rx_bcast_duplicate;
    vinesIfTableEntryData.vinesIfRxEchoCnt = vidb->rx_echo;
    vinesIfTableEntryData.vinesIfRxMacEchoCnt = vidb->rx_mac_echo;
    vinesIfTableEntryData.vinesIfRxProxyReplyCnt = vidb->rx_proxy_reply;
    vinesIfTableEntryData.vinesIfTxUnicastCnt = vidb->tx_unicast;
    vinesIfTableEntryData.vinesIfTxBcastCnt = vidb->tx_bcast;
    vinesIfTableEntryData.vinesIfTxForwardedCnt = vidb->tx_forwarded;
    vinesIfTableEntryData.vinesIfTxFailedEncapsCnt = vidb->tx_failed_encaps;
    vinesIfTableEntryData.vinesIfTxFailedAccessCnt = vidb->tx_failed_access;
    vinesIfTableEntryData.vinesIfTxFailedDownCnt = vidb->tx_failed_down;
    vinesIfTableEntryData.vinesIfTxNotBcastToSourceCnt = vidb->tx_notbcast_toward_source;
    vinesIfTableEntryData.vinesIfTxNotBcastNotlanCnt = vidb->tx_notbcast_notlan;
    vinesIfTableEntryData.vinesIfTxNotBcastNotgt4800Cnt = vidb->tx_notbcast_notgt4800;
    vinesIfTableEntryData.vinesIfTxNotBcastPpchargeCnt = vidb->tx_notbcast_ppcharge;
    vinesIfTableEntryData.vinesIfTxBcastForwardedCnt = vidb->tx_bcast_forwarded;
    vinesIfTableEntryData.vinesIfTxBcastHelperedCnt = vidb->tx_bcast_helpered;
    vinesIfTableEntryData.vinesIfTxArp0Cnt =
	vidb->tx_arpv0_type[ARP0CNT_IDX] + vidb->tx_arpv1_type[ARP0CNT_IDX];
    vinesIfTableEntryData.vinesIfTxArp1Cnt =
	vidb->tx_arpv0_type[ARP1CNT_IDX] + vidb->tx_arpv1_type[ARP1CNT_IDX];
    vinesIfTableEntryData.vinesIfTxArp2Cnt =
	vidb->tx_arpv0_type[ARP2CNT_IDX] + vidb->tx_arpv1_type[ARP2CNT_IDX];
    vinesIfTableEntryData.vinesIfTxArp3Cnt =
	vidb->tx_arpv0_type[ARP3CNT_IDX] + vidb->tx_arpv1_type[ARP3CNT_IDX];
    vinesIfTableEntryData.vinesIfTxIcpErrorCnt = vidb->tx_icp_error;
    vinesIfTableEntryData.vinesIfTxIcpMetricCnt = vidb->tx_icp_metric;
    vinesIfTableEntryData.vinesIfTxIpcCnt = vidb->tx_ipc;
    vinesIfTableEntryData.vinesIfTxRtp0Cnt = vidb->tx_rtpv0_type[ARP0CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp1Cnt = vidb->tx_rtpv0_type[ARP1CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp2Cnt = vidb->tx_rtpv0_type[ARP2CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp3Cnt = vidb->tx_rtpv0_type[ARP3CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp4Cnt = vidb->tx_rtpv0_type[ARP4CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp5Cnt = vidb->tx_rtpv0_type[ARP5CNT_IDX];
    vinesIfTableEntryData.vinesIfTxRtp6Cnt = vidb->tx_rtpv0_type[ARP6CNT_IDX];
    vinesIfTableEntryData.vinesIfTxSppCnt = vidb->tx_spp;
    vinesIfTableEntryData.vinesIfTxEchoCnt = vidb->tx_echo;
    vinesIfTableEntryData.vinesIfTxMacEchoCnt = vidb->tx_mac_echo;
    vinesIfTableEntryData.vinesIfTxProxyCnt = vidb->tx_proxy;
    vinesIfTableEntryData.vinesIfInputRouterFilter = vidb->input_router_filter;
    vinesIfTableEntryData.vinesIfInputNetworkFilter = vidb->input_network_filter;
    vinesIfTableEntryData.vinesIfOutputNetworkFilter = vidb->output_network_filter;

    return (&vinesIfTableEntryData);
}
      
