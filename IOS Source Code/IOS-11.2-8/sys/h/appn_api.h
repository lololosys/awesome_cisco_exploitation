/* $Id: appn_api.h,v 3.2.60.1 1996/05/17 11:15:18 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/h/appn_api.h,v $
 *------------------------------------------------------------------
 * Header file for functions that reside in APPN subsystem.
 *
 * April 1995, Mark Regan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_api.h,v $
 * Revision 3.2.60.1  1996/05/17  11:15:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/01  08:34:40  vtseng
 * Branch: IbuMod_Calif_branch
 * Add DLUR MIB support.
 *
 * Revision 3.2.48.1  1996/03/27  01:30:51  bchan
 * CSCdi50608:  makefile for mib incorrect
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  09:11:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __APPN_API_H__
#define __APPN_API_H__


ibmappnGeneralInfoAndCaps_t *
api_k_ibmappnGeneralInfoAndCaps_get (int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnUniqueInfoAndCaps_t *
api_k_ibmappnNnUniqueInfoAndCaps_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnEnUniqueCaps_t *
api_k_ibmappnEnUniqueCaps_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNodePortEntry_t *
api_k_ibmappnNodePortEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortName);

ibmappnNodePortIpEntry_t *
api_k_ibmappnNodePortIpEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortIpName);

ibmappnNodePortDlsEntry_t *
api_k_ibmappnNodePortDlsEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortDlsName);
     
ibmappnNodePortTrEntry_t *
api_k_ibmappnNodePortTrEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortTrName);

ibmappnNodePortDlcTraceEntry_t *
api_k_ibmappnNodePortDlcTraceEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodePortDlcTracPortName, long ibmappnNodePortDlcTracIndex);

ibmappnNodeLsEntry_t *
api_k_ibmappnNodeLsEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsName);

ibmappnNodeLsIpEntry_t *
api_k_ibmappnNodeLsIpEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsIpName);

ibmappnNodeLsDlsEntry_t *
api_k_ibmappnNodeLsDlsEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsDlsName);

ibmappnNodeLsTrEntry_t *
api_k_ibmappnNodeLsTrEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNodeLsTrName);

ibmappnNodeLsStatusEntry_t *
api_k_ibmappnNodeLsStatusEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNodeLsStatusIndex);

ibmappnSnmpInformation_t *
api_k_ibmappnSnmpInformation_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnMemoryUse_t *
api_k_ibmappnMemoryUse_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnXidInformation_t *
api_k_ibmappnXidInformation_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnTopo_t *
api_k_ibmappnNnTopo_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnNnTopoRouteEntry_t *
api_k_ibmappnNnTopoRouteEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnTopoRouteCos);

ibmappnNnAdjNodeEntry_t *
api_k_ibmappnNnAdjNodeEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnAdjNodeAdjName);

ibmappnNnTopologyEntry_t *
api_k_ibmappnNnTopologyEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnNodeName);

ibmappnNnTgTopologyEntry_t *
api_k_ibmappnNnTgTopologyEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnNnTgOwner, OctetString *ibmappnNnTgDest, long ibmappnNnTgNum);

ibmappnNnTopologyFREntry_t *
api_k_ibmappnNnTopologyFREntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNnNodeFRFrsn, OctetString *ibmappnNnNodeFRName);

ibmappnNnTgTopologyFREntry_t *
api_k_ibmappnNnTgTopologyFREntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, long ibmappnNnTgFRFrsn, OctetString *ibmappnNnTgFROwner, OctetString *ibmappnNnTgFRDest, long ibmappnNnTgFRNum);

ibmappnLocalGeneral_t *
api_k_ibmappnLocalGeneral_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnLocalNnSpecific_t *
api_k_ibmappnLocalNnSpecific_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnLocalTgEntry_t *
api_k_ibmappnLocalTgEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalTgDest, long ibmappnLocalTgNum);

ibmappnLocalEnEntry_t *
api_k_ibmappnLocalEnEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalEnName);

ibmappnLocalEnTgEntry_t *
api_k_ibmappnLocalEnTgEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnLocalEnTgOrigin, OctetString *ibmappnLocalEnTgDest, long ibmappnLocalEnTgNum);

ibmappnDirPerf_t *
api_k_ibmappnDirPerf_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnDirEntry_t *
api_k_ibmappnDirEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnDirLuName);

ibmappnCosModeEntry_t *
api_k_ibmappnCosModeEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosModeName);

ibmappnCosNameEntry_t *
api_k_ibmappnCosNameEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosName);

ibmappnCosNodeRowEntry_t *
api_k_ibmappnCosNodeRowEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosNodeRowName, long ibmappnCosNodeRowIndex);

ibmappnCosTgRowEntry_t *
api_k_ibmappnCosTgRowEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnCosTgRowName, long ibmappnCosTgRowIndex);

ibmappnIsInGlobal_t *
api_k_ibmappnIsInGlobal_get(int SerialNum, ContextInfo *contextInfo, int nominator);

ibmappnIsInEntry_t *
api_k_ibmappnIsInEntry_get(int SerialNum, ContextInfo *contextInfo, int nominator, int searchType, OctetString *ibmappnIsInFqLuName, OctetString *ibmappnIsInPcid);

#endif __APPN_API_H__

dlurNodeCapabilities_t *
api_k_dlurNodeCapabilities_get(int serialNum, ContextInfo *contextInfo, 
int nominator);

dlurPuEntry_t *
api_k_dlurPuEntry_get(int serialNum, ContextInfo *contextInfo, int nominator,
int searchType, long dlurPuIndex);

dlurDlusEntry_t *
api_k_dlurDlusEntry_get(int serialNum, ContextInfo *contextInfo, int nominator,
int searchType, long dlurDlusIndex);
