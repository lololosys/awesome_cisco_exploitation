/* $Id: sr_ibmappnmib.h,v 3.2.60.2 1996/07/17 06:52:34 bchan Exp $
 * $Source: /release/112/cvs/Xsys/appn/sr_ibmappnmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Mark Regan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_ibmappnmib.h,v $
 * Revision 3.2.60.2  1996/07/17  06:52:34  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:39:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/03/27  01:30:38  bchan
 * CSCdi50608:  makefile for mib incorrect
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/03/19  18:43:56  dmccowan
 * CSCdi51814:  Build of APPN MIB broken
 * Branch: IbuMod_Calif_branch
 * add #include sched.h
 *
 * Revision 3.2  1995/11/17  08:37:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/12  17:19:41  mregan
 * CSCdi29195:  APPN MIB values incorrect
 * update APPN MIB with latest SNMP changes.
 *
 * Revision 2.1  1995/06/07  20:03:52  hampton
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

#ifndef _ibmappnmib_h_
#define _ibmappnmib_h_

#define N_ibm6611          /* define OID parts for ibm appn  */

#include "sched.h"
#include "../snmp/snmp_api.h"
#include "sr_ibmappnmibdefs.h"
#include "sr_ibmappnmibsupp.h"
#include "sr_ibmappnmibpart.h"
#include "sr_ibmappnmibtype.h"

extern const ObjectInfo ibmappnmib_OidList[];
extern const uint       ibmappnmib_OidListNum;

extern void init_ibmappnmib(subsystype *subsys);

typedef struct AppnFunc_ {

ibmappnGeneralInfoAndCaps_t *
 (*t_ibmappnGeneralInfoAndCaps_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnUniqueInfoAndCaps_t *
 (*t_ibmappnNnUniqueInfoAndCaps_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnEnUniqueCaps_t *
 (*t_ibmappnEnUniqueCaps_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNodePortEntry_t *
 (*t_ibmappnNodePortEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortName);

ibmappnNodePortIpEntry_t *
 (*t_ibmappnNodePortIpEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortIpName);

ibmappnNodePortDlsEntry_t *
 (*t_ibmappnNodePortDlsEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortDlsName);
     
ibmappnNodePortTrEntry_t *
 (*t_ibmappnNodePortTrEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortTrName);

ibmappnNodePortDlcTraceEntry_t *
 (*t_ibmappnNodePortDlcTraceEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortDlcTracPortName, long ibmappnNodePortDlcTracIndex);

ibmappnNodeLsEntry_t *
 (*t_ibmappnNodeLsEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsName);

ibmappnNodeLsIpEntry_t *
 (*t_ibmappnNodeLsIpEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsIpName);

ibmappnNodeLsDlsEntry_t *
 (*t_ibmappnNodeLsDlsEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsDlsName);

ibmappnNodeLsTrEntry_t *
 (*t_ibmappnNodeLsTrEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsTrName);

ibmappnNodeLsStatusEntry_t *
 (*t_ibmappnNodeLsStatusEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNodeLsStatusIndex);

ibmappnSnmpInformation_t *
 (*t_ibmappnSnmpInformation_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnMemoryUse_t *
 (*t_ibmappnMemoryUse_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnXidInformation_t *
 (*t_ibmappnXidInformation_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnTopo_t *
 (*t_ibmappnNnTopo_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnTopoRouteEntry_t *
 (*t_ibmappnNnTopoRouteEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnTopoRouteCos);

ibmappnNnAdjNodeEntry_t *
 (*t_ibmappnNnAdjNodeEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnAdjNodeAdjName);

ibmappnNnTopologyEntry_t *
 (*t_ibmappnNnTopologyEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnNodeName);

ibmappnNnTgTopologyEntry_t *
 (*t_ibmappnNnTgTopologyEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnTgOwner, OctetString *ibmappnNnTgDest, long ibmappnNnTgNum);

ibmappnNnTopologyFREntry_t *
 (*t_ibmappnNnTopologyFREntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNnNodeFRFrsn, OctetString *ibmappnNnNodeFRName);

ibmappnNnTgTopologyFREntry_t *
 (*t_ibmappnNnTgTopologyFREntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNnTgFRFrsn, OctetString *ibmappnNnTgFROwner, OctetString *ibmappnNnTgFRDest, long ibmappnNnTgFRNum);

ibmappnLocalGeneral_t *
 (*t_ibmappnLocalGeneral_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnLocalNnSpecific_t *
 (*t_ibmappnLocalNnSpecific_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnLocalTgEntry_t *
 (*t_ibmappnLocalTgEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalTgDest, long ibmappnLocalTgNum);

ibmappnLocalEnEntry_t *
 (*t_ibmappnLocalEnEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalEnName);

ibmappnLocalEnTgEntry_t *
 (*t_ibmappnLocalEnTgEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalEnTgOrigin, OctetString *ibmappnLocalEnTgDest, long ibmappnLocalEnTgNum);

ibmappnDirPerf_t *
 (*t_ibmappnDirPerf_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnDirEntry_t *
 (*t_ibmappnDirEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnDirLuName);

ibmappnCosModeEntry_t *
 (*t_ibmappnCosModeEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosModeName);

ibmappnCosNameEntry_t *
 (*t_ibmappnCosNameEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosName);

ibmappnCosNodeRowEntry_t *
 (*t_ibmappnCosNodeRowEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosNodeRowName, long ibmappnCosNodeRowIndex);

ibmappnCosTgRowEntry_t *
 (*t_ibmappnCosTgRowEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosTgRowName, long ibmappnCosTgRowIndex);

ibmappnIsInGlobal_t *
 (*t_ibmappnIsInGlobal_get)(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnIsInEntry_t *
 (*t_ibmappnIsInEntry_get)(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnIsInFqLuName, OctetString *ibmappnIsInPcid);


} AppnFunc;

extern AppnFunc* appnRP;
extern AppnFunc* appnCip;

#endif  /* _ibmappnmib_h_ */
      
