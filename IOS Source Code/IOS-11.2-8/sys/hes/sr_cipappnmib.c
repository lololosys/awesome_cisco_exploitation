/* $Id: sr_cipappnmib.c,v 3.1.4.3 1996/09/02 08:19:28 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_cipappnmib.c,v $
 *------------------------------------------------------------------
 * cip specific appn SNMP sub-agent code 
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_cipappnmib.c,v $
 * Revision 3.1.4.3  1996/09/02  08:19:28  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/08/05  07:24:25  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:18:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/04/03  14:31:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.2.1  1996/03/21  03:21:41  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:01:33  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "../appn/sr_ibmappnmib.h"
#include "if_tn3270s.h"
#include "sysmgt_cipappn.h"
#include "../entity/entity_api.h"
#include "cbus_registry.h"
#include <ciscolib.h>


static boolean GetAppnControlBlock (int search, ContextInfo *contextInfo, sysmgt_appn_table_t type, void* data)
{
   hwidbtype* hwidb;
   hwidb = GetCipIdbFromCommunity(contextInfo);
   if (hwidb != NULL) {
      return get_appn(search == EXACT, hwidb, type, data);
   }
   return FALSE;

}


static ibmappnGeneralInfoAndCaps_t *
cip_ibmappnGeneralInfoAndCaps_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnGeneralInfoAndCaps_t ibmappnGeneralInfoAndCapsData;
   static ibmappnGeneralInfoAndCaps_T appnGeneralInfoAndCapsData;

   static int lastSerialNum = MAXINT;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnGeneralInfoAndCapsData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnGeneralInfoAndCapsData, &appnGeneralInfoAndCapsData)) {

      static OctetString ibmappnNodeCpName;
      static OctetString ibmappnNodeNetid;
      static OctetString ibmappnNodeBlockNum;
      static OctetString ibmappnNodeIdNum;

      ibmappnNodeCpName.octet_ptr = appnGeneralInfoAndCapsData.ibmappnNodeCpName;
      ibmappnNodeCpName.length = strlen(appnGeneralInfoAndCapsData.ibmappnNodeCpName);
      ibmappnNodeNetid.octet_ptr = appnGeneralInfoAndCapsData.ibmappnNodeNetid;
      ibmappnNodeNetid.length = strlen(appnGeneralInfoAndCapsData.ibmappnNodeNetid);
      ibmappnNodeBlockNum.octet_ptr = appnGeneralInfoAndCapsData.ibmappnNodeBlockNum;
      ibmappnNodeBlockNum.length = 3;
      ibmappnNodeIdNum.octet_ptr = appnGeneralInfoAndCapsData.ibmappnNodeIdNum;
      ibmappnNodeIdNum.length = 5;
      
      ibmappnGeneralInfoAndCapsData.ibmappnNodeCpName = &ibmappnNodeCpName;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeNetid = &ibmappnNodeNetid;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeBlockNum = &ibmappnNodeBlockNum;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeIdNum = &ibmappnNodeIdNum;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeType = appnGeneralInfoAndCapsData.ibmappnNodeType;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeUpTime = appnGeneralInfoAndCapsData.ibmappnNodeUpTime;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeNegotLs = appnGeneralInfoAndCapsData.ibmappnNodeNegotLs;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeSegReasm = appnGeneralInfoAndCapsData.ibmappnNodeSegReasm;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeBindReasm = appnGeneralInfoAndCapsData.ibmappnNodeBindReasm;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeParallelTg = appnGeneralInfoAndCapsData.ibmappnNodeParallelTg;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeService = appnGeneralInfoAndCapsData.ibmappnNodeService;
      ibmappnGeneralInfoAndCapsData.ibmappnNodeAdaptiveBindPacing = appnGeneralInfoAndCapsData.ibmappnNodeAdaptiveBindPacing;
      
      lastSerialNum = serialNum;
      return(&ibmappnGeneralInfoAndCapsData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnUniqueInfoAndCaps_t *
cip_ibmappnNnUniqueInfoAndCaps_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnNnUniqueInfoAndCaps_t ibmappnNnUniqueInfoAndCapsData;
   static ibmappnNnUniqueInfoAndCaps_T appnNnUniqueInfoAndCapsData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnNnUniqueInfoAndCapsData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnNnUniqueInfoAndCapsData, &appnNnUniqueInfoAndCapsData)) {

      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnRcvRegChar = appnNnUniqueInfoAndCapsData.ibmappnNodeNnRcvRegChar ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnGateway = appnNnUniqueInfoAndCapsData.ibmappnNodeNnGateway ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnCentralDirectory = appnNnUniqueInfoAndCapsData.ibmappnNodeNnCentralDirectory ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnTreeCache = appnNnUniqueInfoAndCapsData.ibmappnNodeNnTreeCache ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnTreeUpdate = appnNnUniqueInfoAndCapsData.ibmappnNodeNnTreeUpdate ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnRouteAddResist = appnNnUniqueInfoAndCapsData.ibmappnNodeNnRouteAddResist ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnIsr = appnNnUniqueInfoAndCapsData.ibmappnNodeNnIsr ;
      ibmappnNnUniqueInfoAndCapsData.ibmappnNodeNnFrsn = appnNnUniqueInfoAndCapsData.ibmappnNodeNnFrsn ;

      lastSerialNum = serialNum;
      return(&ibmappnNnUniqueInfoAndCapsData);
   }
   else {
      return(NULL);
   }
}

static ibmappnEnUniqueCaps_t *
cip_ibmappnEnUniqueCaps_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnEnUniqueCaps_t ibmappnEnUniqueCapsData;
   static ibmappnEnUniqueCaps_T appnEnUniqueCapsData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnEnUniqueCapsData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnEnUniqueCapsData, &appnEnUniqueCapsData)) {

      ibmappnEnUniqueCapsData.ibmappnNodeEnSegGen = appnEnUniqueCapsData.ibmappnNodeEnSegGen ;
      ibmappnEnUniqueCapsData.ibmappnNodeEnModeCosMap = appnEnUniqueCapsData.ibmappnNodeEnModeCosMap ;
      ibmappnEnUniqueCapsData.ibmappnNodeEnLocateCdinit = appnEnUniqueCapsData.ibmappnNodeEnLocateCdinit ;
      ibmappnEnUniqueCapsData.ibmappnNodeEnSendRegNames = appnEnUniqueCapsData.ibmappnNodeEnSendRegNames ;
      ibmappnEnUniqueCapsData.ibmappnNodeEnSendRegChar = appnEnUniqueCapsData.ibmappnNodeEnSendRegChar ;

      lastSerialNum = serialNum;
      return(&ibmappnEnUniqueCapsData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodePortEntry_t *
cip_ibmappnNodePortEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodePortName)
{
   static ibmappnNodePortEntry_t ibmappnNodePortEntryData;
   static ibmappnNodePortEntry_T appnNodePortEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodePortName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodePortName,
                   ibmappnNodePortName->octet_ptr,
                   ibmappnNodePortName->length) == 0))
   {
      return(&ibmappnNodePortEntryData);
   }

   sstrncpy(appnNodePortEntryData.ibmappnNodePortName,
            ibmappnNodePortName->octet_ptr,
            ibmappnNodePortName->length+1);

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodePortEntryData, &appnNodePortEntryData)) {
   
      static OctetString nodePortName;

      nodePortName.octet_ptr = appnNodePortEntryData.ibmappnNodePortName;
      nodePortName.length = strlen(appnNodePortEntryData.ibmappnNodePortName);
      
      ibmappnNodePortEntryData.ibmappnNodePortName = &nodePortName;
      ibmappnNodePortEntryData.ibmappnNodePortState = appnNodePortEntryData.ibmappnNodePortState;
      ibmappnNodePortEntryData.ibmappnNodePortDlcType = appnNodePortEntryData.ibmappnNodePortDlcType;
      ibmappnNodePortEntryData.ibmappnNodePortPortType = appnNodePortEntryData.ibmappnNodePortPortType;
      ibmappnNodePortEntryData.ibmappnNodePortSIMRIM = appnNodePortEntryData.ibmappnNodePortSIMRIM;
      ibmappnNodePortEntryData.ibmappnNodePortLsRole = appnNodePortEntryData.ibmappnNodePortLsRole;
      ibmappnNodePortEntryData.ibmappnNodePortMaxRcvBtuSize = appnNodePortEntryData.ibmappnNodePortMaxRcvBtuSize;
      ibmappnNodePortEntryData.ibmappnNodePortMaxIframeWindow = appnNodePortEntryData.ibmappnNodePortMaxIframeWindow;
      ibmappnNodePortEntryData.ibmappnNodePortDefLsGoodXids = appnNodePortEntryData.ibmappnNodePortDefLsGoodXids;
      ibmappnNodePortEntryData.ibmappnNodePortDefLsBadXids = appnNodePortEntryData.ibmappnNodePortDefLsBadXids;
      ibmappnNodePortEntryData.ibmappnNodePortDynLsGoodXids = appnNodePortEntryData.ibmappnNodePortDynLsGoodXids;
      ibmappnNodePortEntryData.ibmappnNodePortDynLsBadXids = appnNodePortEntryData.ibmappnNodePortDynLsBadXids;
 
      ibmappnNodePortEntryData.ibmappnNodePortSpecific = &nullOID;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNodePortName,
               ibmappnNodePortName->octet_ptr,
               ibmappnNodePortName->length+1);
      return(&ibmappnNodePortEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodePortIpEntry_t *
cip_ibmappnNodePortIpEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodePortIpName)
{
   static ibmappnNodePortIpEntry_t ibmappnNodePortIpEntryData;
   static ibmappnNodePortIpEntry_T appnNodePortIpEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodePortIpName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodePortIpName,
                   ibmappnNodePortIpName->octet_ptr,
                   ibmappnNodePortIpName->length) == 0))
   {
      return(&ibmappnNodePortIpEntryData);
   }
   
   sstrncpy(appnNodePortIpEntryData.ibmappnNodePortIpName,
            ibmappnNodePortIpName->octet_ptr,
            ibmappnNodePortIpName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodePortIpEntryData, &appnNodePortIpEntryData)) {
   
      static OctetString nodePortIpName;
   
      nodePortIpName.octet_ptr = appnNodePortIpEntryData.ibmappnNodePortIpName;
      nodePortIpName.length = strlen(appnNodePortIpEntryData.ibmappnNodePortIpName);
      
      ibmappnNodePortIpEntryData.ibmappnNodePortIpName = &nodePortIpName;
      ibmappnNodePortIpEntryData.ibmappnNodePortIpPortNum = appnNodePortIpEntryData.ibmappnNodePortIpPortNum;

      lastSerialNum = serialNum;
      sstrncpy(lastNodePortIpName,
               ibmappnNodePortIpName->octet_ptr,
               ibmappnNodePortIpName->length+1);
      return(&ibmappnNodePortIpEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodePortDlsEntry_t *
cip_ibmappnNodePortDlsEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodePortDlsName)
{
   static ibmappnNodePortDlsEntry_t ibmappnNodePortDlsEntryData;
   static ibmappnNodePortDlsEntry_T appnNodePortDlsEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodePortDlsName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodePortDlsName,
                   ibmappnNodePortDlsName->octet_ptr,
                   ibmappnNodePortDlsName->length) == 0))
   {
      return(&ibmappnNodePortDlsEntryData);
   }
   
   sstrncpy(appnNodePortDlsEntryData.ibmappnNodePortDlsName,
            ibmappnNodePortDlsName->octet_ptr,
            ibmappnNodePortDlsName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodePortDlsEntryData, &appnNodePortDlsEntryData)) {
   
      static OctetString nodePortDlsName;
      static OctetString ibmappnNodePortDlsMac;
      static OctetString ibmappnNodePortDlsSap;
      
      nodePortDlsName.octet_ptr = appnNodePortDlsEntryData.ibmappnNodePortDlsName;
      nodePortDlsName.length = strlen(appnNodePortDlsEntryData.ibmappnNodePortDlsName);
      ibmappnNodePortDlsMac.octet_ptr = appnNodePortDlsEntryData.ibmappnNodePortDlsMac;
      ibmappnNodePortDlsMac.length = 6;
      ibmappnNodePortDlsSap.octet_ptr = &appnNodePortDlsEntryData.ibmappnNodePortDlsSap;
      ibmappnNodePortDlsSap.length = 1;

      ibmappnNodePortDlsEntryData.ibmappnNodePortDlsName = &nodePortDlsName;
      ibmappnNodePortDlsEntryData.ibmappnNodePortDlsMac = &ibmappnNodePortDlsMac;
      ibmappnNodePortDlsEntryData.ibmappnNodePortDlsSap = &ibmappnNodePortDlsSap;

      lastSerialNum = serialNum;
      sstrncpy(lastNodePortDlsName,
               ibmappnNodePortDlsName->octet_ptr,
               ibmappnNodePortDlsName->length+1);
      return(&ibmappnNodePortDlsEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodePortTrEntry_t *
cip_ibmappnNodePortTrEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodePortTrName)
{
   static ibmappnNodePortTrEntry_t ibmappnNodePortTrEntryData;
   static ibmappnNodePortTrEntry_T appnNodePortTrEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodePortTrName[STRING_SIZE_8];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodePortTrName,
                   ibmappnNodePortTrName->octet_ptr,
                   ibmappnNodePortTrName->length) == 0))
   {
      return(&ibmappnNodePortTrEntryData);
   }
   
   sstrncpy(appnNodePortTrEntryData.ibmappnNodePortTrName,
            ibmappnNodePortTrName->octet_ptr,
            ibmappnNodePortTrName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodePortTrEntryData, &appnNodePortTrEntryData)) {
   
      static OctetString nodePortTrName;
      static OctetString ibmappnNodePortTrMac;
      static OctetString ibmappnNodePortTrSap;
  
      nodePortTrName.octet_ptr = appnNodePortTrEntryData.ibmappnNodePortTrName;
      nodePortTrName.length = strlen(appnNodePortTrEntryData.ibmappnNodePortTrName);
      ibmappnNodePortTrMac.octet_ptr = appnNodePortTrEntryData.ibmappnNodePortTrMac;
      ibmappnNodePortTrMac.length = 6;
      ibmappnNodePortTrSap.octet_ptr = &appnNodePortTrEntryData.ibmappnNodePortTrSap;
      ibmappnNodePortTrSap.length = 1;

      ibmappnNodePortTrEntryData.ibmappnNodePortTrName = &nodePortTrName;
      ibmappnNodePortTrEntryData.ibmappnNodePortTrMac = &ibmappnNodePortTrMac;
      ibmappnNodePortTrEntryData.ibmappnNodePortTrSap = &ibmappnNodePortTrSap;

      lastSerialNum = serialNum;
      sstrncpy(lastNodePortTrName,
               ibmappnNodePortTrName->octet_ptr,
               ibmappnNodePortTrName->length+1);
      return(&ibmappnNodePortTrEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodePortDlcTraceEntry_t *
cip_ibmappnNodePortDlcTraceEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodePortDlcTracPortName,
   long ibmappnNodePortDlcTracIndex)
{
   static ibmappnNodePortDlcTraceEntry_t ibmappnNodePortDlcTraceEntryData;
   static ibmappnNodePortDlcTraceEntry_T appnNodePortDlcTraceEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodePortDlcTracPortName[STRING_SIZE_UNDEF];
   static long lastIbmappnNodePortDlcTracIndex = MAXLONG;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (lastIbmappnNodePortDlcTracIndex == ibmappnNodePortDlcTracIndex)
       && (strncmp(lastNodePortDlcTracPortName,
                   ibmappnNodePortDlcTracPortName->octet_ptr,
                   ibmappnNodePortDlcTracPortName->length) == 0))
   {
      return(&ibmappnNodePortDlcTraceEntryData);
   }
   
   sstrncpy(appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracPortName,
            ibmappnNodePortDlcTracPortName->octet_ptr,
            ibmappnNodePortDlcTracPortName->length+1);
   appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracIndex = ibmappnNodePortDlcTracIndex;

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodePortDlcTraceEntryData, &appnNodePortDlcTraceEntryData)) {
   
      static OctetString nodePortDlcTracPortName;
      static OctetString ibmappnNodePortDlcTracLocalAddr;
      static OctetString ibmappnNodePortDlcTracRemoteAddr;
   
      nodePortDlcTracPortName.octet_ptr = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracPortName;
      nodePortDlcTracPortName.length = strlen(appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracPortName);
      ibmappnNodePortDlcTracLocalAddr.octet_ptr = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracLocalAddr;
      ibmappnNodePortDlcTracLocalAddr.length = strlen(appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracLocalAddr);
      ibmappnNodePortDlcTracRemoteAddr.octet_ptr = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracRemoteAddr;
      ibmappnNodePortDlcTracRemoteAddr.length = strlen(appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracRemoteAddr);

      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracPortName = &nodePortDlcTracPortName;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracIndex = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracIndex;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracDlcType = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracDlcType;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracLocalAddr = &ibmappnNodePortDlcTracLocalAddr;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracRemoteAddr = &ibmappnNodePortDlcTracRemoteAddr;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracMsgType = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracMsgType;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracCmdType = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracCmdType;
      ibmappnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracUseWan = appnNodePortDlcTraceEntryData.ibmappnNodePortDlcTracUseWan;
    
      lastSerialNum = serialNum;
      sstrncpy(lastNodePortDlcTracPortName,
               ibmappnNodePortDlcTracPortName->octet_ptr,
               ibmappnNodePortDlcTracPortName->length+1);
      lastIbmappnNodePortDlcTracIndex = ibmappnNodePortDlcTracIndex;
      return(&ibmappnNodePortDlcTraceEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodeLsEntry_t *
cip_ibmappnNodeLsEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodeLsName)
{
   static ibmappnNodeLsEntry_t ibmappnNodeLsEntryData;
   static ibmappnNodeLsEntry_T appnNodeLsEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodeLsName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodeLsName,
                   ibmappnNodeLsName->octet_ptr,
                   ibmappnNodeLsName->length) == 0))
   {
      return(&ibmappnNodeLsEntryData);
   }

   sstrncpy(appnNodeLsEntryData.ibmappnNodeLsName,
            ibmappnNodeLsName->octet_ptr,
            ibmappnNodeLsName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodeLsEntryData, &appnNodeLsEntryData)) {
   
      static OctetString nodeLsName;
      static OctetString ibmappnNodeLsPortName;
      static OctetString ibmappnNodeLsCpName;
      static OctetString ibmappnNodeLsBlockNum;
      static OctetString ibmappnNodeLsIdNum;
      
      nodeLsName.octet_ptr = appnNodeLsEntryData.ibmappnNodeLsName;
      nodeLsName.length = strlen(appnNodeLsEntryData.ibmappnNodeLsName);
      ibmappnNodeLsPortName.octet_ptr = appnNodeLsEntryData.ibmappnNodeLsPortName;
      ibmappnNodeLsPortName.length = strlen(appnNodeLsEntryData.ibmappnNodeLsPortName);
      ibmappnNodeLsCpName.octet_ptr = appnNodeLsEntryData.ibmappnNodeLsCpName;
      ibmappnNodeLsCpName.length = strlen(appnNodeLsEntryData.ibmappnNodeLsCpName);
      ibmappnNodeLsBlockNum.octet_ptr = appnNodeLsEntryData.ibmappnNodeLsBlockNum;
      ibmappnNodeLsBlockNum.length = strlen(appnNodeLsEntryData.ibmappnNodeLsBlockNum);
      ibmappnNodeLsIdNum.octet_ptr = appnNodeLsEntryData.ibmappnNodeLsIdNum;
      ibmappnNodeLsIdNum.length = strlen(appnNodeLsEntryData.ibmappnNodeLsIdNum);
      
      ibmappnNodeLsEntryData.ibmappnNodeLsName = &nodeLsName;
      ibmappnNodeLsEntryData.ibmappnNodeLsPortName = &ibmappnNodeLsPortName;
      ibmappnNodeLsEntryData.ibmappnNodeLsDlcType = appnNodeLsEntryData.ibmappnNodeLsDlcType;
      ibmappnNodeLsEntryData.ibmappnNodeLsDynamic = appnNodeLsEntryData.ibmappnNodeLsDynamic ;
      ibmappnNodeLsEntryData.ibmappnNodeLsState = appnNodeLsEntryData.ibmappnNodeLsState;
      ibmappnNodeLsEntryData.ibmappnNodeLsCpName = &ibmappnNodeLsCpName;
      ibmappnNodeLsEntryData.ibmappnNodeLsTgNum = appnNodeLsEntryData.ibmappnNodeLsTgNum ;
      ibmappnNodeLsEntryData.ibmappnNodeLsLimResource = appnNodeLsEntryData.ibmappnNodeLsLimResource;
      ibmappnNodeLsEntryData.ibmappnNodeLsMigration = appnNodeLsEntryData.ibmappnNodeLsMigration;
      ibmappnNodeLsEntryData.ibmappnNodeLsBlockNum = &ibmappnNodeLsBlockNum;
      ibmappnNodeLsEntryData.ibmappnNodeLsIdNum = &ibmappnNodeLsIdNum;
      ibmappnNodeLsEntryData.ibmappnNodeLsCpCpSession = appnNodeLsEntryData.ibmappnNodeLsCpCpSession;
      ibmappnNodeLsEntryData.ibmappnNodeLsTargetPacingCount = appnNodeLsEntryData.ibmappnNodeLsTargetPacingCount;
      ibmappnNodeLsEntryData.ibmappnNodeLsMaxSendBtuSize = appnNodeLsEntryData.ibmappnNodeLsMaxSendBtuSize;
      ibmappnNodeLsEntryData.ibmappnNodeLsEffCap = appnNodeLsEntryData.ibmappnNodeLsEffCap;
      ibmappnNodeLsEntryData.ibmappnNodeLsConnCost = appnNodeLsEntryData.ibmappnNodeLsConnCost;
      ibmappnNodeLsEntryData.ibmappnNodeLsByteCost = appnNodeLsEntryData.ibmappnNodeLsByteCost;
      ibmappnNodeLsEntryData.ibmappnNodeLsSecurity = appnNodeLsEntryData.ibmappnNodeLsSecurity;
      ibmappnNodeLsEntryData.ibmappnNodeLsDelay = appnNodeLsEntryData.ibmappnNodeLsDelay;
      ibmappnNodeLsEntryData.ibmappnNodeLsUsr1 = appnNodeLsEntryData.ibmappnNodeLsUsr1;
      ibmappnNodeLsEntryData.ibmappnNodeLsUsr2 = appnNodeLsEntryData.ibmappnNodeLsUsr2;
      ibmappnNodeLsEntryData.ibmappnNodeLsUsr3 = appnNodeLsEntryData.ibmappnNodeLsUsr3;
      ibmappnNodeLsEntryData.ibmappnNodeLsInXidBytes = appnNodeLsEntryData.ibmappnNodeLsInXidBytes;
      ibmappnNodeLsEntryData.ibmappnNodeLsInMsgBytes = appnNodeLsEntryData.ibmappnNodeLsInMsgBytes;
      ibmappnNodeLsEntryData.ibmappnNodeLsInXidFrames = appnNodeLsEntryData.ibmappnNodeLsInXidFrames;
      ibmappnNodeLsEntryData.ibmappnNodeLsInMsgFrames = appnNodeLsEntryData.ibmappnNodeLsInMsgFrames;
      ibmappnNodeLsEntryData.ibmappnNodeLsOutXidBytes = appnNodeLsEntryData.ibmappnNodeLsOutXidBytes;
      ibmappnNodeLsEntryData.ibmappnNodeLsOutMsgBytes = appnNodeLsEntryData.ibmappnNodeLsOutMsgBytes;
      ibmappnNodeLsEntryData.ibmappnNodeLsOutXidFrames = appnNodeLsEntryData.ibmappnNodeLsOutXidFrames;
      ibmappnNodeLsEntryData.ibmappnNodeLsOutMsgFrames = appnNodeLsEntryData.ibmappnNodeLsOutMsgFrames;
      ibmappnNodeLsEntryData.ibmappnNodeLsEchoRsps = appnNodeLsEntryData.ibmappnNodeLsEchoRsps;
      ibmappnNodeLsEntryData.ibmappnNodeLsCurrentDelay = appnNodeLsEntryData.ibmappnNodeLsCurrentDelay;
      ibmappnNodeLsEntryData.ibmappnNodeLsMaxDelay = appnNodeLsEntryData.ibmappnNodeLsMaxDelay;
      ibmappnNodeLsEntryData.ibmappnNodeLsMinDelay = appnNodeLsEntryData.ibmappnNodeLsMinDelay;
      ibmappnNodeLsEntryData.ibmappnNodeLsMaxDelayTime = appnNodeLsEntryData.ibmappnNodeLsMaxDelayTime;
      ibmappnNodeLsEntryData.ibmappnNodeLsGoodXids = appnNodeLsEntryData.ibmappnNodeLsGoodXids;
      ibmappnNodeLsEntryData.ibmappnNodeLsBadXids = appnNodeLsEntryData.ibmappnNodeLsBadXids;
      
      ibmappnNodeLsEntryData.ibmappnNodeLsSpecific = &nullOID;
      
      ibmappnNodeLsEntryData.ibmappnNodeLsSubState = appnNodeLsEntryData.ibmappnNodeLsSubState;
      ibmappnNodeLsEntryData.ibmappnNodeLsStartTime = appnNodeLsEntryData.ibmappnNodeLsStartTime;
      ibmappnNodeLsEntryData.ibmappnNodeLsActiveTime = appnNodeLsEntryData.ibmappnNodeLsActiveTime;
      ibmappnNodeLsEntryData.ibmappnNodeLsCurrentStateTime = appnNodeLsEntryData.ibmappnNodeLsCurrentStateTime;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNodeLsName,
               ibmappnNodeLsName->octet_ptr,
               ibmappnNodeLsName->length+1);
      return(&ibmappnNodeLsEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodeLsIpEntry_t *
cip_ibmappnNodeLsIpEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodeLsIpName)
{
   static ibmappnNodeLsIpEntry_t ibmappnNodeLsIpEntryData;
   static ibmappnNodeLsIpEntry_T appnNodeLsIpEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodeLsIpName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodeLsIpName,
                   ibmappnNodeLsIpName->octet_ptr,
                   ibmappnNodeLsIpName->length) == 0))
   {
      return(&ibmappnNodeLsIpEntryData);
   }
   
   sstrncpy(appnNodeLsIpEntryData.ibmappnNodeLsIpName,
            ibmappnNodeLsIpName->octet_ptr,
            ibmappnNodeLsIpName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodeLsIpEntryData, &appnNodeLsIpEntryData)) {
   
      static OctetString nodeLsIpName;

      nodeLsIpName.octet_ptr = appnNodeLsIpEntryData.ibmappnNodeLsIpName;
      nodeLsIpName.length = strlen(appnNodeLsIpEntryData.ibmappnNodeLsIpName);

      ibmappnNodeLsIpEntryData.ibmappnNodeLsIpName = &nodeLsIpName;
      ibmappnNodeLsIpEntryData.ibmappnNodeLsIpState = appnNodeLsIpEntryData.ibmappnNodeLsIpState;
      ibmappnNodeLsIpEntryData.ibmappnNodeLsLocalIpAddr = appnNodeLsIpEntryData.ibmappnNodeLsLocalIpAddr;
      ibmappnNodeLsIpEntryData.ibmappnNodeLsLocalIpPortNum = appnNodeLsIpEntryData.ibmappnNodeLsLocalIpPortNum;
      ibmappnNodeLsIpEntryData.ibmappnNodeLsRemoteIpAddr = appnNodeLsIpEntryData.ibmappnNodeLsRemoteIpAddr;
      ibmappnNodeLsIpEntryData.ibmappnNodeLsRemoteIpPortNum = appnNodeLsIpEntryData.ibmappnNodeLsRemoteIpPortNum;
     
      lastSerialNum = serialNum;
      sstrncpy(lastNodeLsIpName,
               ibmappnNodeLsIpName->octet_ptr,
               ibmappnNodeLsIpName->length+1);
      return(&ibmappnNodeLsIpEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodeLsDlsEntry_t *
cip_ibmappnNodeLsDlsEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodeLsDlsName)
{
   static ibmappnNodeLsDlsEntry_t ibmappnNodeLsDlsEntryData;
   static ibmappnNodeLsDlsEntry_T appnNodeLsDlsEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodeLsDlsName[STRING_SIZE_8];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodeLsDlsName,
                   ibmappnNodeLsDlsName->octet_ptr,
                   ibmappnNodeLsDlsName->length) == 0))
   {
      return(&ibmappnNodeLsDlsEntryData);
   }
   
   sstrncpy(appnNodeLsDlsEntryData.ibmappnNodeLsDlsName,
            ibmappnNodeLsDlsName->octet_ptr,
            ibmappnNodeLsDlsName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodeLsDlsEntryData, &appnNodeLsDlsEntryData)) {
   
      static OctetString nodeLsDlsName;
      static OctetString ibmappnNodeLsLocalDlsMac;
      static OctetString ibmappnNodeLsLocalDlsSap;
      static OctetString ibmappnNodeLsRemoteDlsMac;
      static OctetString ibmappnNodeLsRemoteDlsSap;
  
      nodeLsDlsName.octet_ptr = appnNodeLsDlsEntryData.ibmappnNodeLsDlsName;
      nodeLsDlsName.length = strlen(appnNodeLsDlsEntryData.ibmappnNodeLsDlsName);
      ibmappnNodeLsLocalDlsMac.octet_ptr = appnNodeLsDlsEntryData.ibmappnNodeLsLocalDlsMac;
      ibmappnNodeLsLocalDlsMac.length = 6;
      ibmappnNodeLsLocalDlsSap.octet_ptr = &appnNodeLsDlsEntryData.ibmappnNodeLsLocalDlsSap;
      ibmappnNodeLsLocalDlsSap.length = 1;
      ibmappnNodeLsRemoteDlsMac.octet_ptr = appnNodeLsDlsEntryData.ibmappnNodeLsRemoteDlsMac;
      ibmappnNodeLsRemoteDlsMac.length = 6;
      ibmappnNodeLsRemoteDlsSap.octet_ptr = &appnNodeLsDlsEntryData.ibmappnNodeLsRemoteDlsSap;
      ibmappnNodeLsRemoteDlsSap.length = 1;
      
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsDlsName = &nodeLsDlsName;
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsDlsState = appnNodeLsDlsEntryData.ibmappnNodeLsDlsState;
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsLocalDlsMac = &ibmappnNodeLsLocalDlsMac;
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsLocalDlsSap = &ibmappnNodeLsLocalDlsSap;
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsRemoteDlsMac = &ibmappnNodeLsRemoteDlsMac;
      ibmappnNodeLsDlsEntryData.ibmappnNodeLsRemoteDlsSap = &ibmappnNodeLsRemoteDlsSap;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNodeLsDlsName,
               ibmappnNodeLsDlsName->octet_ptr,
               ibmappnNodeLsDlsName->length+1);
      return(&ibmappnNodeLsDlsEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodeLsTrEntry_t *
cip_ibmappnNodeLsTrEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNodeLsTrName)
{
   static ibmappnNodeLsTrEntry_t ibmappnNodeLsTrEntryData;
   static ibmappnNodeLsTrEntry_T appnNodeLsTrEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNodeLsTrName[STRING_SIZE_8];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNodeLsTrName,
                   ibmappnNodeLsTrName->octet_ptr,
                   ibmappnNodeLsTrName->length) == 0))
   {
      return(&ibmappnNodeLsTrEntryData);
   }
   
   sstrncpy(appnNodeLsTrEntryData.ibmappnNodeLsTrName,
            ibmappnNodeLsTrName->octet_ptr,
            ibmappnNodeLsTrName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodeLsTrEntryData, &appnNodeLsTrEntryData)) {
   
      static OctetString nodeLsTrName;
      static OctetString ibmappnNodeLsLocalTrMac;
      static OctetString ibmappnNodeLsLocalTrSap;
      static OctetString ibmappnNodeLsRemoteTrMac;
      static OctetString ibmappnNodeLsRemoteTrSap;
  
      nodeLsTrName.octet_ptr = appnNodeLsTrEntryData.ibmappnNodeLsTrName;
      nodeLsTrName.length = strlen(appnNodeLsTrEntryData.ibmappnNodeLsTrName);
      ibmappnNodeLsLocalTrMac.octet_ptr = appnNodeLsTrEntryData.ibmappnNodeLsLocalTrMac;
      ibmappnNodeLsLocalTrMac.length = 6;
      ibmappnNodeLsLocalTrSap.octet_ptr = &appnNodeLsTrEntryData.ibmappnNodeLsLocalTrSap;
      ibmappnNodeLsLocalTrSap.length = 1;
      ibmappnNodeLsRemoteTrMac.octet_ptr = appnNodeLsTrEntryData.ibmappnNodeLsRemoteTrMac;
      ibmappnNodeLsRemoteTrMac.length = 6;
      ibmappnNodeLsRemoteTrSap.octet_ptr =&appnNodeLsTrEntryData.ibmappnNodeLsRemoteTrSap;
      ibmappnNodeLsRemoteTrSap.length = 1;
      
      ibmappnNodeLsTrEntryData.ibmappnNodeLsTrName = &nodeLsTrName;
      ibmappnNodeLsTrEntryData.ibmappnNodeLsTrState = appnNodeLsTrEntryData.ibmappnNodeLsTrState;
      ibmappnNodeLsTrEntryData.ibmappnNodeLsLocalTrMac = &ibmappnNodeLsLocalTrMac;
      ibmappnNodeLsTrEntryData.ibmappnNodeLsLocalTrSap = &ibmappnNodeLsLocalTrSap;
      ibmappnNodeLsTrEntryData.ibmappnNodeLsRemoteTrMac = &ibmappnNodeLsRemoteTrMac;
      ibmappnNodeLsTrEntryData.ibmappnNodeLsRemoteTrSap = &ibmappnNodeLsRemoteTrSap;
     
      lastSerialNum = serialNum;
      sstrncpy(lastNodeLsTrName,
               ibmappnNodeLsTrName->octet_ptr,
               ibmappnNodeLsTrName->length+1);
      return(&ibmappnNodeLsTrEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNodeLsStatusEntry_t *
cip_ibmappnNodeLsStatusEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long ibmappnNodeLsStatusIndex)
{
   static ibmappnNodeLsStatusEntry_t ibmappnNodeLsStatusEntryData;
   static ibmappnNodeLsStatusEntry_T appnNodeLsStatusEntryData;

   static int lastSerialNum = MAXINT;
   static long lastIbmappnNodeLsStatusIndex;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (lastIbmappnNodeLsStatusIndex == ibmappnNodeLsStatusIndex))
   {
      return(&ibmappnNodeLsStatusEntryData);
   }
   
   appnNodeLsStatusEntryData.ibmappnNodeLsStatusIndex = ibmappnNodeLsStatusIndex;

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNodeLsStatusEntryData, &appnNodeLsStatusEntryData)) {
   
      static OctetString ibmappnNodeLsStatusLsName;
      static OctetString ibmappnNodeLsStatusCpName;
      static OctetString ibmappnNodeLsStatusNodeId;
      static OctetString ibmappnNodeLsStatusGeneralSense;
      static OctetString ibmappnNodeLsStatusEndSense;
      static OctetString ibmappnNodeLsStatusXidLocalSense;
      static OctetString ibmappnNodeLsStatusXidRemoteSense;
      static OctetString ibmappnNodeLsStatusLocalAddr;
      static OctetString ibmappnNodeLsStatusRemoteAddr;
      
      ibmappnNodeLsStatusLsName.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusLsName;
      ibmappnNodeLsStatusLsName.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusLsName);
      ibmappnNodeLsStatusCpName.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusCpName;
      ibmappnNodeLsStatusCpName.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusCpName);
      ibmappnNodeLsStatusNodeId.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusNodeId;
      ibmappnNodeLsStatusNodeId.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusNodeId);
      ibmappnNodeLsStatusGeneralSense.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusGeneralSense;
      ibmappnNodeLsStatusGeneralSense.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusGeneralSense);
      ibmappnNodeLsStatusEndSense.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusEndSense;
      ibmappnNodeLsStatusEndSense.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusEndSense);
      ibmappnNodeLsStatusXidLocalSense.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidLocalSense;
      ibmappnNodeLsStatusXidLocalSense.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidLocalSense);
      ibmappnNodeLsStatusXidRemoteSense.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidRemoteSense;
      ibmappnNodeLsStatusXidRemoteSense.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidRemoteSense);
      ibmappnNodeLsStatusLocalAddr.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusLocalAddr;
      ibmappnNodeLsStatusLocalAddr.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusLocalAddr);
      ibmappnNodeLsStatusRemoteAddr.octet_ptr = appnNodeLsStatusEntryData.ibmappnNodeLsStatusRemoteAddr;
      ibmappnNodeLsStatusRemoteAddr.length = strlen(appnNodeLsStatusEntryData.ibmappnNodeLsStatusRemoteAddr);
      
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusIndex = appnNodeLsStatusEntryData.ibmappnNodeLsStatusIndex;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusTime = appnNodeLsStatusEntryData.ibmappnNodeLsStatusTime;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusLsName = &ibmappnNodeLsStatusLsName;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusCpName = &ibmappnNodeLsStatusCpName;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusNodeId = &ibmappnNodeLsStatusNodeId;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusTgNum = appnNodeLsStatusEntryData.ibmappnNodeLsStatusTgNum;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusGeneralSense = &ibmappnNodeLsStatusGeneralSense;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusNofRetry = appnNodeLsStatusEntryData.ibmappnNodeLsStatusNofRetry;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusEndSense = &ibmappnNodeLsStatusEndSense;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusXidLocalSense = &ibmappnNodeLsStatusXidLocalSense;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusXidRemoteSense = &ibmappnNodeLsStatusXidRemoteSense;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusXidByteInError = appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidByteInError;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusXidBitInError = appnNodeLsStatusEntryData.ibmappnNodeLsStatusXidBitInError;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusDlcType = appnNodeLsStatusEntryData.ibmappnNodeLsStatusDlcType;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusLocalAddr = &ibmappnNodeLsStatusLocalAddr;
      ibmappnNodeLsStatusEntryData.ibmappnNodeLsStatusRemoteAddr = &ibmappnNodeLsStatusRemoteAddr;
      
      lastSerialNum = serialNum;
      lastIbmappnNodeLsStatusIndex = ibmappnNodeLsStatusIndex;
      return(&ibmappnNodeLsStatusEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnSnmpInformation_t *
cip_ibmappnSnmpInformation_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnSnmpInformation_t ibmappnSnmpInformationData;
   static ibmappnSnmpInformation_T appnSnmpInformationData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnSnmpInformationData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnSnmpInformationData, &appnSnmpInformationData)) {

      ibmappnSnmpInformationData.ibmappnSnmpInPkts = appnSnmpInformationData.ibmappnSnmpInPkts;
      ibmappnSnmpInformationData.ibmappnSnmpInGetRequests = appnSnmpInformationData.ibmappnSnmpInGetRequests;
      ibmappnSnmpInformationData.ibmappnSnmpInGetNexts = appnSnmpInformationData.ibmappnSnmpInGetNexts;
      ibmappnSnmpInformationData.ibmappnSnmpInSetRequests = appnSnmpInformationData.ibmappnSnmpInSetRequests;
      ibmappnSnmpInformationData.ibmappnSnmpInTotalVars = appnSnmpInformationData.ibmappnSnmpInTotalVars;
      ibmappnSnmpInformationData.ibmappnSnmpInGetVars = appnSnmpInformationData.ibmappnSnmpInGetVars;
      ibmappnSnmpInformationData.ibmappnSnmpInGetNextVars = appnSnmpInformationData.ibmappnSnmpInGetNextVars;
      ibmappnSnmpInformationData.ibmappnSnmpInSetVars = appnSnmpInformationData.ibmappnSnmpInSetVars;
      ibmappnSnmpInformationData.ibmappnSnmpOutNoSuchNames = appnSnmpInformationData.ibmappnSnmpOutNoSuchNames;
      ibmappnSnmpInformationData.ibmappnSnmpOutGenErrs = appnSnmpInformationData.ibmappnSnmpOutGenErrs;
    
      lastSerialNum = serialNum;
      return(&ibmappnSnmpInformationData);
   }
   else {
      return(NULL);
   }
}

static ibmappnMemoryUse_t *
cip_ibmappnMemoryUse_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnMemoryUse_t ibmappnMemoryUseData;
   static ibmappnMemoryUse_T appnMemoryUseData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnMemoryUseData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnMemoryUseData, &appnMemoryUseData)) {

      ibmappnMemoryUseData.ibmappnMemorySize = appnMemoryUseData.ibmappnMemorySize;
      ibmappnMemoryUseData.ibmappnMemoryUsed = appnMemoryUseData.ibmappnMemoryUsed;
      ibmappnMemoryUseData.ibmappnMemoryWarnThresh = appnMemoryUseData.ibmappnMemoryWarnThresh;
      ibmappnMemoryUseData.ibmappnMemoryCritThresh = appnMemoryUseData.ibmappnMemoryCritThresh;
      
      lastSerialNum = serialNum;
      return(&ibmappnMemoryUseData);
   }
   else {
      return(NULL);
   }
}

static ibmappnXidInformation_t *
cip_ibmappnXidInformation_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnXidInformation_t ibmappnXidInformationData;
   static ibmappnXidInformation_T appnXidInformationData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnXidInformationData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnXidInformationData, &appnXidInformationData)) {

      ibmappnXidInformationData.ibmappnNodeDefLsGoodXids = appnXidInformationData.ibmappnNodeDefLsGoodXids;
      ibmappnXidInformationData.ibmappnNodeDefLsBadXids = appnXidInformationData.ibmappnNodeDefLsBadXids;
      ibmappnXidInformationData.ibmappnNodeDynLsGoodXids = appnXidInformationData.ibmappnNodeDynLsGoodXids;
      ibmappnXidInformationData.ibmappnNodeDynLsBadXids = appnXidInformationData.ibmappnNodeDynLsBadXids;
      
      lastSerialNum = serialNum;
      return(&ibmappnXidInformationData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTopo_t *
cip_ibmappnNnTopo_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnNnTopo_t ibmappnNnTopoData;
   static ibmappnNnTopo_T appnNnTopoData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnNnTopoData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnNnTopoData, &appnNnTopoData)) {
   
      ibmappnNnTopoData.ibmappnNnTopoMaxNodes = appnNnTopoData.ibmappnNnTopoMaxNodes;
      ibmappnNnTopoData.ibmappnNnTopoCurNumNodes = appnNnTopoData.ibmappnNnTopoCurNumNodes;
      ibmappnNnTopoData.ibmappnNnTopoInTdus = appnNnTopoData.ibmappnNnTopoInTdus;
      ibmappnNnTopoData.ibmappnNnTopoOutTdus = appnNnTopoData.ibmappnNnTopoOutTdus;
      ibmappnNnTopoData.ibmappnNnTopoNodeLowRsns = appnNnTopoData.ibmappnNnTopoNodeLowRsns;
      ibmappnNnTopoData.ibmappnNnTopoNodeEqualRsns = appnNnTopoData.ibmappnNnTopoNodeEqualRsns;
      ibmappnNnTopoData.ibmappnNnTopoNodeGoodHighRsns = appnNnTopoData.ibmappnNnTopoNodeGoodHighRsns;
      ibmappnNnTopoData.ibmappnNnTopoNodeBadHighRsns = appnNnTopoData.ibmappnNnTopoNodeBadHighRsns;
      ibmappnNnTopoData.ibmappnNnTopoNodeStateUpdates = appnNnTopoData.ibmappnNnTopoNodeStateUpdates;
      ibmappnNnTopoData.ibmappnNnTopoNodeErrors = appnNnTopoData.ibmappnNnTopoNodeErrors;
      ibmappnNnTopoData.ibmappnNnTopoNodeTimerUpdates = appnNnTopoData.ibmappnNnTopoNodeTimerUpdates;
      ibmappnNnTopoData.ibmappnNnTopoNodePurges = appnNnTopoData.ibmappnNnTopoNodePurges;
      ibmappnNnTopoData.ibmappnNnTopoTgLowRsns = appnNnTopoData.ibmappnNnTopoTgLowRsns;
      ibmappnNnTopoData.ibmappnNnTopoTgEqualRsns = appnNnTopoData.ibmappnNnTopoTgEqualRsns;
      ibmappnNnTopoData.ibmappnNnTopoTgGoodHighRsns = appnNnTopoData.ibmappnNnTopoTgGoodHighRsns;
      ibmappnNnTopoData.ibmappnNnTopoTgBadHighRsns = appnNnTopoData.ibmappnNnTopoTgBadHighRsns;
      ibmappnNnTopoData.ibmappnNnTopoTgStateUpdates = appnNnTopoData.ibmappnNnTopoTgStateUpdates;
      ibmappnNnTopoData.ibmappnNnTopoTgErrors = appnNnTopoData.ibmappnNnTopoTgErrors;
      ibmappnNnTopoData.ibmappnNnTopoTgTimerUpdates = appnNnTopoData.ibmappnNnTopoTgTimerUpdates;
      ibmappnNnTopoData.ibmappnNnTopoTgPurges = appnNnTopoData.ibmappnNnTopoTgPurges;
      ibmappnNnTopoData.ibmappnNnTopoTotalRouteCalcs = appnNnTopoData.ibmappnNnTopoTotalRouteCalcs;
      ibmappnNnTopoData.ibmappnNnTopoTotalRouteRejs = appnNnTopoData.ibmappnNnTopoTotalRouteRejs;

      lastSerialNum = serialNum;
      return(&ibmappnNnTopoData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTopoRouteEntry_t *
cip_ibmappnNnTopoRouteEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNnTopoRouteCos)
{
   static ibmappnNnTopoRouteEntry_t ibmappnNnTopoRouteEntryData;
   static ibmappnNnTopoRouteEntry_T appnNnTopoRouteEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNnTopoRouteCos[STRING_SIZE_UNDEF];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNnTopoRouteCos,
                   ibmappnNnTopoRouteCos->octet_ptr,
                   ibmappnNnTopoRouteCos->length) == 0))
   {
      return(&ibmappnNnTopoRouteEntryData);
   }
   
   sstrncpy(appnNnTopoRouteEntryData.ibmappnNnTopoRouteCos,
            ibmappnNnTopoRouteCos->octet_ptr,
            ibmappnNnTopoRouteCos->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnTopoRouteEntryData, &appnNnTopoRouteEntryData)) {
   
      static OctetString nnTopoRouteCos;

      nnTopoRouteCos.octet_ptr = appnNnTopoRouteEntryData.ibmappnNnTopoRouteCos;
      nnTopoRouteCos.length = strlen(appnNnTopoRouteEntryData.ibmappnNnTopoRouteCos);
      
      ibmappnNnTopoRouteEntryData.ibmappnNnTopoRouteCos = &nnTopoRouteCos;
      ibmappnNnTopoRouteEntryData.ibmappnNnTopoRouteTrees = appnNnTopoRouteEntryData.ibmappnNnTopoRouteTrees;
      ibmappnNnTopoRouteEntryData.ibmappnNnTopoRouteCalcs = appnNnTopoRouteEntryData.ibmappnNnTopoRouteCalcs;
      ibmappnNnTopoRouteEntryData.ibmappnNnTopoRouteRejs = appnNnTopoRouteEntryData.ibmappnNnTopoRouteRejs;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNnTopoRouteCos,
               ibmappnNnTopoRouteCos->octet_ptr,
               ibmappnNnTopoRouteCos->length+1);
      return(&ibmappnNnTopoRouteEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnAdjNodeEntry_t *
cip_ibmappnNnAdjNodeEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNnAdjNodeAdjName)
{
   static ibmappnNnAdjNodeEntry_t ibmappnNnAdjNodeEntryData;
   static ibmappnNnAdjNodeEntry_T appnNnAdjNodeEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNnAdjNodeAdjName[STRING_SIZE_17];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNnAdjNodeAdjName,
                   ibmappnNnAdjNodeAdjName->octet_ptr,
                   ibmappnNnAdjNodeAdjName->length) == 0))
   {
      return(&ibmappnNnAdjNodeEntryData);
   }
   
   sstrncpy(appnNnAdjNodeEntryData.ibmappnNnAdjNodeAdjName,
            ibmappnNnAdjNodeAdjName->octet_ptr,
            ibmappnNnAdjNodeAdjName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnAdjNodeEntryData, &appnNnAdjNodeEntryData)) {
   
      static OctetString nnAdjNodeAdjName;

      nnAdjNodeAdjName.octet_ptr = appnNnAdjNodeEntryData.ibmappnNnAdjNodeAdjName;
      nnAdjNodeAdjName.length = strlen(appnNnAdjNodeEntryData.ibmappnNnAdjNodeAdjName);
      
      ibmappnNnAdjNodeEntryData.ibmappnNnAdjNodeAdjName = &nnAdjNodeAdjName;
      ibmappnNnAdjNodeEntryData.ibmappnNnAdjNodeCpCpSessStatus = appnNnAdjNodeEntryData.ibmappnNnAdjNodeCpCpSessStatus;
      ibmappnNnAdjNodeEntryData.ibmappnNnAdjNodeOutOfSeqTdus = appnNnAdjNodeEntryData.ibmappnNnAdjNodeOutOfSeqTdus;
      ibmappnNnAdjNodeEntryData.ibmappnNnAdjNodeLastFrsnSent = appnNnAdjNodeEntryData.ibmappnNnAdjNodeLastFrsnSent;
      ibmappnNnAdjNodeEntryData.ibmappnNnAdjNodeLastFrsnRcvd = appnNnAdjNodeEntryData.ibmappnNnAdjNodeLastFrsnRcvd;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNnAdjNodeAdjName,
               ibmappnNnAdjNodeAdjName->octet_ptr,
               ibmappnNnAdjNodeAdjName->length+1);
      return(&ibmappnNnAdjNodeEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTopologyEntry_t *
cip_ibmappnNnTopologyEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNnNodeName)
{
   static ibmappnNnTopologyEntry_t ibmappnNnTopologyEntryData;
   static ibmappnNnTopologyEntry_T appnNnTopologyEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNnNodeName[STRING_SIZE_17];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNnNodeName,
                   ibmappnNnNodeName->octet_ptr,
                   ibmappnNnNodeName->length) == 0))
   {
      return(&ibmappnNnTopologyEntryData);
   }
   
   sstrncpy(appnNnTopologyEntryData.ibmappnNnNodeName,
            ibmappnNnNodeName->octet_ptr,
            ibmappnNnNodeName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnTopologyEntryData, &appnNnTopologyEntryData)) {
   
      static OctetString nnNodeName;
      
      nnNodeName.octet_ptr = appnNnTopologyEntryData.ibmappnNnNodeName;
      nnNodeName.length = strlen(appnNnTopologyEntryData.ibmappnNnNodeName);

      ibmappnNnTopologyEntryData.ibmappnNnNodeName = &nnNodeName;
      ibmappnNnTopologyEntryData.ibmappnNnNodeFrsn = appnNnTopologyEntryData.ibmappnNnNodeFrsn;
      ibmappnNnTopologyEntryData.ibmappnNnNodeEntryTimeLeft = appnNnTopologyEntryData.ibmappnNnNodeEntryTimeLeft;
      ibmappnNnTopologyEntryData.ibmappnNnNodeType = appnNnTopologyEntryData.ibmappnNnNodeType;
      ibmappnNnTopologyEntryData.ibmappnNnNodeRsn = appnNnTopologyEntryData.ibmappnNnNodeRsn;
      ibmappnNnTopologyEntryData.ibmappnNnNodeRouteAddResist = appnNnTopologyEntryData.ibmappnNnNodeRouteAddResist;
      ibmappnNnTopologyEntryData.ibmappnNnNodeCongested = appnNnTopologyEntryData.ibmappnNnNodeCongested;
      ibmappnNnTopologyEntryData.ibmappnNnNodeIsrDepleted = appnNnTopologyEntryData.ibmappnNnNodeIsrDepleted;
      ibmappnNnTopologyEntryData.ibmappnNnNodeEndptDepleted = appnNnTopologyEntryData.ibmappnNnNodeEndptDepleted;
      ibmappnNnTopologyEntryData.ibmappnNnNodeQuiescing = appnNnTopologyEntryData.ibmappnNnNodeQuiescing;
      ibmappnNnTopologyEntryData.ibmappnNnNodeGateway = appnNnTopologyEntryData.ibmappnNnNodeGateway;
      ibmappnNnTopologyEntryData.ibmappnNnNodeCentralDirectory = appnNnTopologyEntryData.ibmappnNnNodeCentralDirectory;
      ibmappnNnTopologyEntryData.ibmappnNnNodeIsr = appnNnTopologyEntryData.ibmappnNnNodeIsr;
      ibmappnNnTopologyEntryData.ibmappnNnNodeChainSupport = appnNnTopologyEntryData.ibmappnNnNodeChainSupport;

      lastSerialNum = serialNum;
      sstrncpy(lastNnNodeName,
               ibmappnNnNodeName->octet_ptr,
               ibmappnNnNodeName->length+1);
      return(&ibmappnNnTopologyEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTgTopologyEntry_t *
cip_ibmappnNnTgTopologyEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnNnTgOwner,
   OctetString * ibmappnNnTgDest,
   long ibmappnNnTgNum)
{
   static ibmappnNnTgTopologyEntry_t ibmappnNnTgTopologyEntryData;
   static ibmappnNnTgTopologyEntry_T appnNnTgTopologyEntryData;

   static int lastSerialNum = MAXINT;
   static char lastNnTgOwner[STRING_SIZE_17];
   static char lastNnTgDest[STRING_SIZE_17];
   static long lastIbmappnNnTgNum = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastNnTgOwner,
                   ibmappnNnTgOwner->octet_ptr,
                   ibmappnNnTgOwner->length) == 0)
       && (strncmp(lastNnTgDest,
                   ibmappnNnTgDest->octet_ptr,
                   ibmappnNnTgDest->length) == 0)
       && (lastIbmappnNnTgNum == ibmappnNnTgNum))
   {
      return(&ibmappnNnTgTopologyEntryData);
   }
   
   sstrncpy(appnNnTgTopologyEntryData.ibmappnNnTgOwner,
            ibmappnNnTgOwner->octet_ptr,
            ibmappnNnTgOwner->length+1);
   sstrncpy(appnNnTgTopologyEntryData.ibmappnNnTgDest,
            ibmappnNnTgDest->octet_ptr,
            ibmappnNnTgDest->length+1);
   appnNnTgTopologyEntryData.ibmappnNnTgNum = ibmappnNnTgNum;

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnTgTopologyEntryData, &appnNnTgTopologyEntryData)) {
   
      static OctetString nnTgOwner;
      static OctetString nnTgDest;
      static OctetString ibmappnNnTgDlcData;
      
      nnTgOwner.octet_ptr = appnNnTgTopologyEntryData.ibmappnNnTgOwner;
      nnTgOwner.length = strlen(appnNnTgTopologyEntryData.ibmappnNnTgOwner);
      nnTgDest.octet_ptr = appnNnTgTopologyEntryData.ibmappnNnTgDest;
      nnTgDest.length = strlen(appnNnTgTopologyEntryData.ibmappnNnTgDest);
      ibmappnNnTgDlcData.octet_ptr = appnNnTgTopologyEntryData.ibmappnNnTgDlcData;
      ibmappnNnTgDlcData.length = 9;

      ibmappnNnTgTopologyEntryData.ibmappnNnTgOwner = &nnTgOwner;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgDest = &nnTgDest;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgNum = appnNnTgTopologyEntryData.ibmappnNnTgNum;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgFrsn = appnNnTgTopologyEntryData.ibmappnNnTgFrsn;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgEntryTimeLeft = appnNnTgTopologyEntryData.ibmappnNnTgEntryTimeLeft;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgDestVirtual = appnNnTgTopologyEntryData.ibmappnNnTgDestVirtual;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgDlcData = &ibmappnNnTgDlcData;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgRsn = appnNnTgTopologyEntryData.ibmappnNnTgRsn;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgOperational = appnNnTgTopologyEntryData.ibmappnNnTgOperational;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgQuiescing = appnNnTgTopologyEntryData.ibmappnNnTgQuiescing;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgCpCpSession = appnNnTgTopologyEntryData.ibmappnNnTgCpCpSession;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgEffCap = appnNnTgTopologyEntryData.ibmappnNnTgEffCap;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgConnCost = appnNnTgTopologyEntryData.ibmappnNnTgConnCost;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgByteCost = appnNnTgTopologyEntryData.ibmappnNnTgByteCost;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgSecurity = appnNnTgTopologyEntryData.ibmappnNnTgSecurity;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgDelay = appnNnTgTopologyEntryData.ibmappnNnTgDelay;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgModemClass = appnNnTgTopologyEntryData.ibmappnNnTgModemClass;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgUsr1 = appnNnTgTopologyEntryData.ibmappnNnTgUsr1;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgUsr2 = appnNnTgTopologyEntryData.ibmappnNnTgUsr2;
      ibmappnNnTgTopologyEntryData.ibmappnNnTgUsr3 = appnNnTgTopologyEntryData.ibmappnNnTgUsr3;
      
      lastSerialNum = serialNum;
      sstrncpy(lastNnTgOwner,
               ibmappnNnTgOwner->octet_ptr,
               ibmappnNnTgOwner->length+1);
      sstrncpy(lastNnTgDest,
               ibmappnNnTgDest->octet_ptr,
               ibmappnNnTgDest->length+1);
      lastIbmappnNnTgNum = ibmappnNnTgNum;
      return(&ibmappnNnTgTopologyEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTopologyFREntry_t *
cip_ibmappnNnTopologyFREntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long ibmappnNnNodeFRFrsn,
   OctetString * ibmappnNnNodeFRName)
{
   static ibmappnNnTopologyFREntry_t ibmappnNnTopologyFREntryData;
   static ibmappnNnTopologyFREntry_T appnNnTopologyFREntryData;

   static int lastSerialNum = MAXINT;
   static long lastIbmappnNnNodeFRFrsn = MAXLONG;
   static char lastNnNodeFRName[STRING_SIZE_17];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (lastIbmappnNnNodeFRFrsn == ibmappnNnNodeFRFrsn)
       && (strncmp(lastNnNodeFRName,
                   ibmappnNnNodeFRName->octet_ptr,
                   ibmappnNnNodeFRName->length) == 0))
   {
      return(&ibmappnNnTopologyFREntryData);
   }
   
   appnNnTopologyFREntryData.ibmappnNnNodeFRFrsn = ibmappnNnNodeFRFrsn;
   sstrncpy(appnNnTopologyFREntryData.ibmappnNnNodeFRName,
            ibmappnNnNodeFRName->octet_ptr,
            ibmappnNnNodeFRName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnTopologyFREntryData, &appnNnTopologyFREntryData)) {
   
      static OctetString nnNodeFRName;

      nnNodeFRName.octet_ptr = appnNnTopologyFREntryData.ibmappnNnNodeFRName;
      nnNodeFRName.length = strlen(appnNnTopologyFREntryData.ibmappnNnNodeFRName);

      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRName = &nnNodeFRName;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRFrsn = appnNnTopologyFREntryData.ibmappnNnNodeFRFrsn;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFREntryTimeLeft = appnNnTopologyFREntryData.ibmappnNnNodeFREntryTimeLeft;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRType = appnNnTopologyFREntryData.ibmappnNnNodeFRType;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRRsn = appnNnTopologyFREntryData.ibmappnNnNodeFRRsn;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRRouteAddResist = appnNnTopologyFREntryData.ibmappnNnNodeFRRouteAddResist;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRCongested = appnNnTopologyFREntryData.ibmappnNnNodeFRCongested;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRIsrDepleted = appnNnTopologyFREntryData.ibmappnNnNodeFRIsrDepleted;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFREndptDepleted = appnNnTopologyFREntryData.ibmappnNnNodeFREndptDepleted;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRQuiescing = appnNnTopologyFREntryData.ibmappnNnNodeFRQuiescing;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRGateway = appnNnTopologyFREntryData.ibmappnNnNodeFRGateway;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRCentralDirectory = appnNnTopologyFREntryData.ibmappnNnNodeFRCentralDirectory;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRIsr = appnNnTopologyFREntryData.ibmappnNnNodeFRIsr;
      ibmappnNnTopologyFREntryData.ibmappnNnNodeFRChainSupport = appnNnTopologyFREntryData.ibmappnNnNodeFRChainSupport;
     
      lastSerialNum = serialNum;
      lastIbmappnNnNodeFRFrsn = ibmappnNnNodeFRFrsn;
      sstrncpy(lastNnNodeFRName,
               ibmappnNnNodeFRName->octet_ptr,
               ibmappnNnNodeFRName->length+1);
      return(&ibmappnNnTopologyFREntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnNnTgTopologyFREntry_t *
cip_ibmappnNnTgTopologyFREntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long ibmappnNnTgFRFrsn,
   OctetString * ibmappnNnTgFROwner,
   OctetString * ibmappnNnTgFRDest,
   long ibmappnNnTgFRNum)
{
   static ibmappnNnTgTopologyFREntry_t ibmappnNnTgTopologyFREntryData;
   static ibmappnNnTgTopologyFREntry_T appnNnTgTopologyFREntryData;

   static int lastSerialNum = MAXINT;
   static long lastIbmappnNnTgFRFrsn = MAXLONG;
   static char lastNnTgFROwner[STRING_SIZE_17];
   static char lastNnTgFRDest[STRING_SIZE_17];
   static long lastIbmappnNnTgFRNum  = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (lastIbmappnNnTgFRFrsn == ibmappnNnTgFRFrsn)
       && (strncmp(lastNnTgFROwner,
                   ibmappnNnTgFROwner->octet_ptr,
                   ibmappnNnTgFROwner->length) == 0)
       && (strncmp(lastNnTgFRDest,
                   ibmappnNnTgFRDest->octet_ptr,
                   ibmappnNnTgFRDest->length) == 0)
       && (lastIbmappnNnTgFRNum == ibmappnNnTgFRNum))
   {
      return(&ibmappnNnTgTopologyFREntryData);
   }
   
   appnNnTgTopologyFREntryData.ibmappnNnTgFRFrsn = ibmappnNnTgFRFrsn;
   sstrncpy(appnNnTgTopologyFREntryData.ibmappnNnTgFROwner,
            ibmappnNnTgFROwner->octet_ptr,
            ibmappnNnTgFROwner->length+1);
   sstrncpy(appnNnTgTopologyFREntryData.ibmappnNnTgFRDest,
            ibmappnNnTgFRDest->octet_ptr,
            ibmappnNnTgFRDest->length+1);
   appnNnTgTopologyFREntryData.ibmappnNnTgFRNum = ibmappnNnTgFRNum;
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnNnTgTopologyFREntryData, &appnNnTgTopologyFREntryData)) {
   
      static OctetString nnTgFROwner;
      static OctetString nnTgFRDest;
      static OctetString ibmappnNnTgFRDlcData;
      
      nnTgFROwner.octet_ptr = appnNnTgTopologyFREntryData.ibmappnNnTgFROwner;
      nnTgFROwner.length = strlen(appnNnTgTopologyFREntryData.ibmappnNnTgFROwner);
      nnTgFRDest.octet_ptr = appnNnTgTopologyFREntryData.ibmappnNnTgFRDest;;
      nnTgFRDest.length = strlen(appnNnTgTopologyFREntryData.ibmappnNnTgFRDest);
      ibmappnNnTgFRDlcData.octet_ptr = appnNnTgTopologyFREntryData.ibmappnNnTgFRDlcData;
      ibmappnNnTgFRDlcData.length = 9;
      
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFROwner = &nnTgFROwner;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRDest = &nnTgFRDest;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRNum = appnNnTgTopologyFREntryData.ibmappnNnTgFRNum;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRFrsn = appnNnTgTopologyFREntryData.ibmappnNnTgFRFrsn;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFREntryTimeLeft = appnNnTgTopologyFREntryData.ibmappnNnTgFREntryTimeLeft;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRDestVirtual = appnNnTgTopologyFREntryData.ibmappnNnTgFRDestVirtual;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRDlcData = &ibmappnNnTgFRDlcData;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRRsn = appnNnTgTopologyFREntryData.ibmappnNnTgFRRsn;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFROperational = appnNnTgTopologyFREntryData.ibmappnNnTgFROperational;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRQuiescing = appnNnTgTopologyFREntryData.ibmappnNnTgFRQuiescing;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRCpCpSession = appnNnTgTopologyFREntryData.ibmappnNnTgFRCpCpSession;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFREffCap = appnNnTgTopologyFREntryData.ibmappnNnTgFREffCap;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRConnCost = appnNnTgTopologyFREntryData.ibmappnNnTgFRConnCost;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRByteCost = appnNnTgTopologyFREntryData.ibmappnNnTgFRByteCost ;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRSecurity = appnNnTgTopologyFREntryData.ibmappnNnTgFRSecurity ;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRDelay = appnNnTgTopologyFREntryData.ibmappnNnTgFRDelay;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRModemClass = appnNnTgTopologyFREntryData.ibmappnNnTgFRModemClass ;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRUsr1 = appnNnTgTopologyFREntryData.ibmappnNnTgFRUsr1;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRUsr2 = appnNnTgTopologyFREntryData.ibmappnNnTgFRUsr2;
      ibmappnNnTgTopologyFREntryData.ibmappnNnTgFRUsr3 = appnNnTgTopologyFREntryData.ibmappnNnTgFRUsr3;

      lastSerialNum = serialNum;
      lastIbmappnNnTgFRFrsn = ibmappnNnTgFRFrsn;
      sstrncpy(lastNnTgFROwner,
               ibmappnNnTgFROwner->octet_ptr,
               ibmappnNnTgFROwner->length+1);
      sstrncpy(lastNnTgFRDest,
               ibmappnNnTgFRDest->octet_ptr,
               ibmappnNnTgFRDest->length+1);
      lastIbmappnNnTgFRNum = ibmappnNnTgFRNum;
      return(&ibmappnNnTgTopologyFREntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnLocalGeneral_t *
cip_ibmappnLocalGeneral_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnLocalGeneral_t ibmappnLocalGeneralData;
   static ibmappnLocalGeneral_T appnLocalGeneralData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnLocalGeneralData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnLocalGeneralData, &appnLocalGeneralData)) {
      
      static OctetString ibmappnLocalNodeName;

      ibmappnLocalNodeName.octet_ptr = appnLocalGeneralData.ibmappnLocalNodeName;
      ibmappnLocalNodeName.length = strlen(appnLocalGeneralData.ibmappnLocalNodeName);

      ibmappnLocalGeneralData.ibmappnLocalNodeName = &ibmappnLocalNodeName;
      ibmappnLocalGeneralData.ibmappnLocalNodeType = ibmappnLocalGeneralData.ibmappnLocalNodeType;

      lastSerialNum = serialNum;
      return(&ibmappnLocalGeneralData);
   }
   else {
      return(NULL);
   }
}

static ibmappnLocalNnSpecific_t *
cip_ibmappnLocalNnSpecific_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnLocalNnSpecific_t ibmappnLocalNnSpecificData;
   static ibmappnLocalNnSpecific_T appnLocalNnSpecificData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnLocalNnSpecificData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnLocalNnSpecificData, &appnLocalNnSpecificData)) {
   
      ibmappnLocalNnSpecificData.ibmappnLocalNnRsn = appnLocalNnSpecificData.ibmappnLocalNnRsn;
      ibmappnLocalNnSpecificData.ibmappnLocalNnRouteAddResist = appnLocalNnSpecificData.ibmappnLocalNnRouteAddResist;
      ibmappnLocalNnSpecificData.ibmappnLocalNnCongested = appnLocalNnSpecificData.ibmappnLocalNnCongested;
      ibmappnLocalNnSpecificData.ibmappnLocalNnIsrDepleted = appnLocalNnSpecificData.ibmappnLocalNnIsrDepleted;
      ibmappnLocalNnSpecificData.ibmappnLocalNnEndptDepleted = appnLocalNnSpecificData.ibmappnLocalNnEndptDepleted;
      ibmappnLocalNnSpecificData.ibmappnLocalNnQuiescing = appnLocalNnSpecificData.ibmappnLocalNnQuiescing;
      ibmappnLocalNnSpecificData.ibmappnLocalNnGateway = appnLocalNnSpecificData.ibmappnLocalNnGateway;
      ibmappnLocalNnSpecificData.ibmappnLocalNnCentralDirectory = appnLocalNnSpecificData.ibmappnLocalNnCentralDirectory;
      ibmappnLocalNnSpecificData.ibmappnLocalNnIsr = appnLocalNnSpecificData.ibmappnLocalNnIsr;
      ibmappnLocalNnSpecificData.ibmappnLocalNnChainSupport = appnLocalNnSpecificData.ibmappnLocalNnChainSupport;
      ibmappnLocalNnSpecificData.ibmappnLocalNnFrsn = appnLocalNnSpecificData.ibmappnLocalNnFrsn;
      
      lastSerialNum = serialNum;
      return(&ibmappnLocalNnSpecificData);
   }
   else {
      return(NULL);
   }
}

static ibmappnLocalTgEntry_t *
cip_ibmappnLocalTgEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnLocalTgDest,
   long ibmappnLocalTgNum)
{
   static ibmappnLocalTgEntry_t ibmappnLocalTgEntryData;
   static ibmappnLocalTgEntry_T appnLocalTgEntryData;

   static int lastSerialNum = MAXINT;
   static char lastLocalTgDest[STRING_SIZE_17];
   static long lastIbmappnLocalTgNum = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastLocalTgDest,
                   ibmappnLocalTgDest->octet_ptr,
                   ibmappnLocalTgDest->length) == 0)
       && (lastIbmappnLocalTgNum == ibmappnLocalTgNum))
   {
      return(&ibmappnLocalTgEntryData);
   }
   
   sstrncpy(appnLocalTgEntryData.ibmappnLocalTgDest,
            ibmappnLocalTgDest->octet_ptr,
            ibmappnLocalTgDest->length+1);
   appnLocalTgEntryData.ibmappnLocalTgNum = ibmappnLocalTgNum;
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnLocalTgEntryData, &appnLocalTgEntryData)) {
   
      static OctetString localTgDest;
      static OctetString ibmappnLocalTgDlcData;
      
      localTgDest.octet_ptr = appnLocalTgEntryData.ibmappnLocalTgDest;
      localTgDest.length = strlen(appnLocalTgEntryData.ibmappnLocalTgDest);
      ibmappnLocalTgDlcData.octet_ptr = appnLocalTgEntryData.ibmappnLocalTgDlcData;
      ibmappnLocalTgDlcData.length = 9;

      ibmappnLocalTgEntryData.ibmappnLocalTgDest = &localTgDest;
      ibmappnLocalTgEntryData.ibmappnLocalTgNum = appnLocalTgEntryData.ibmappnLocalTgNum;
      ibmappnLocalTgEntryData.ibmappnLocalTgDestVirtual = appnLocalTgEntryData.ibmappnLocalTgDestVirtual;
      ibmappnLocalTgEntryData.ibmappnLocalTgDlcData = &ibmappnLocalTgDlcData;
      ibmappnLocalTgEntryData.ibmappnLocalTgRsn = appnLocalTgEntryData.ibmappnLocalTgRsn;
      ibmappnLocalTgEntryData.ibmappnLocalTgQuiescing = appnLocalTgEntryData.ibmappnLocalTgQuiescing;
      ibmappnLocalTgEntryData.ibmappnLocalTgOperational = appnLocalTgEntryData.ibmappnLocalTgOperational;
      ibmappnLocalTgEntryData.ibmappnLocalTgCpCpSession = appnLocalTgEntryData.ibmappnLocalTgCpCpSession;
      ibmappnLocalTgEntryData.ibmappnLocalTgEffCap = appnLocalTgEntryData.ibmappnLocalTgEffCap;
      ibmappnLocalTgEntryData.ibmappnLocalTgConnCost = appnLocalTgEntryData.ibmappnLocalTgConnCost;
      ibmappnLocalTgEntryData.ibmappnLocalTgByteCost = appnLocalTgEntryData.ibmappnLocalTgByteCost;
      ibmappnLocalTgEntryData.ibmappnLocalTgSecurity = appnLocalTgEntryData.ibmappnLocalTgSecurity;
      ibmappnLocalTgEntryData.ibmappnLocalTgDelay = appnLocalTgEntryData.ibmappnLocalTgDelay;
      ibmappnLocalTgEntryData.ibmappnLocalTgModemClass = appnLocalTgEntryData.ibmappnLocalTgModemClass;
      ibmappnLocalTgEntryData.ibmappnLocalTgUsr1 = appnLocalTgEntryData.ibmappnLocalTgUsr1;
      ibmappnLocalTgEntryData.ibmappnLocalTgUsr2 = appnLocalTgEntryData.ibmappnLocalTgUsr2;
      ibmappnLocalTgEntryData.ibmappnLocalTgUsr3 = appnLocalTgEntryData.ibmappnLocalTgUsr3;

      lastSerialNum = serialNum;
      sstrncpy(lastLocalTgDest,
               ibmappnLocalTgDest->octet_ptr,
               ibmappnLocalTgDest->length+1);
      lastIbmappnLocalTgNum = ibmappnLocalTgNum;
      return(&ibmappnLocalTgEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnLocalEnEntry_t *
cip_ibmappnLocalEnEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnLocalEnName)
{
   static ibmappnLocalEnEntry_t ibmappnLocalEnEntryData;
   static ibmappnLocalEnEntry_T appnLocalEnEntryData;

   static int lastSerialNum = MAXINT;
   static char lastLocalEnName[STRING_SIZE_17];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastLocalEnName,
                   ibmappnLocalEnName->octet_ptr,
                   ibmappnLocalEnName->length) == 0))
   {
      return(&ibmappnLocalEnEntryData);
   }
   
   sstrncpy(appnLocalEnEntryData.ibmappnLocalEnName,
            ibmappnLocalEnName->octet_ptr,
            ibmappnLocalEnName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnLocalEnEntryData, &appnLocalEnEntryData)) {
   
      static OctetString localEnName;
      
      localEnName.octet_ptr = appnLocalEnEntryData.ibmappnLocalEnName;
      localEnName.length = strlen(appnLocalEnEntryData.ibmappnLocalEnName);

      ibmappnLocalEnEntryData.ibmappnLocalEnName = &localEnName;
      ibmappnLocalEnEntryData.ibmappnLocalEnEntryTimeLeft = appnLocalEnEntryData.ibmappnLocalEnEntryTimeLeft;
      ibmappnLocalEnEntryData.ibmappnLocalEnType = appnLocalEnEntryData.ibmappnLocalEnType;

      lastSerialNum = serialNum;
      sstrncpy(lastLocalEnName,
               ibmappnLocalEnName->octet_ptr,
               ibmappnLocalEnName->length+1);
      return(&ibmappnLocalEnEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnLocalEnTgEntry_t *
cip_ibmappnLocalEnTgEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnLocalEnTgOrigin,
   OctetString * ibmappnLocalEnTgDest,
   long ibmappnLocalEnTgNum)
{
   static ibmappnLocalEnTgEntry_t ibmappnLocalEnTgEntryData;
   static ibmappnLocalEnTgEntry_T appnLocalEnTgEntryData;

   static int lastSerialNum = MAXINT;
   static char lastLocalEnTgOrigin[STRING_SIZE_17];
   static char lastLocalEnTgDest[STRING_SIZE_17];
   static long lastIbmappnLocalEnTgNum = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastLocalEnTgOrigin,
                   ibmappnLocalEnTgOrigin->octet_ptr,
                   ibmappnLocalEnTgOrigin->length) == 0)
       && (strncmp(lastLocalEnTgDest,
                   ibmappnLocalEnTgDest->octet_ptr,
                   ibmappnLocalEnTgDest->length) == 0)
       && (lastIbmappnLocalEnTgNum == ibmappnLocalEnTgNum))
   {
      return(&ibmappnLocalEnTgEntryData);
   }
   
   sstrncpy(appnLocalEnTgEntryData.ibmappnLocalEnTgOrigin,
            ibmappnLocalEnTgOrigin->octet_ptr,
            ibmappnLocalEnTgOrigin->length+1);
   sstrncpy(appnLocalEnTgEntryData.ibmappnLocalEnTgDest,
            ibmappnLocalEnTgDest->octet_ptr,
            ibmappnLocalEnTgDest->length+1);
   appnLocalEnTgEntryData.ibmappnLocalEnTgNum = ibmappnLocalEnTgNum;   
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnLocalEnTgEntryData, &appnLocalEnTgEntryData)) {
   
      static OctetString localEnTgOrigin;
      static OctetString localEnTgDest;
      static OctetString ibmappnLocalEnTgDlcData;
      
      localEnTgOrigin.octet_ptr = appnLocalEnTgEntryData.ibmappnLocalEnTgOrigin;
      localEnTgOrigin.length = strlen(appnLocalEnTgEntryData.ibmappnLocalEnTgOrigin);
      localEnTgDest.octet_ptr = appnLocalEnTgEntryData.ibmappnLocalEnTgDest;
      localEnTgDest.length = strlen(appnLocalEnTgEntryData.ibmappnLocalEnTgDest);
      ibmappnLocalEnTgDlcData.octet_ptr = appnLocalEnTgEntryData.ibmappnLocalEnTgDlcData;
      ibmappnLocalEnTgDlcData.length = 9;
  
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgOrigin = &localEnTgOrigin;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgDest = &localEnTgDest;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgNum = appnLocalEnTgEntryData.ibmappnLocalEnTgNum;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgEntryTimeLeft = appnLocalEnTgEntryData.ibmappnLocalEnTgEntryTimeLeft;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgDestVirtual = appnLocalEnTgEntryData.ibmappnLocalEnTgDestVirtual;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgDlcData = &ibmappnLocalEnTgDlcData;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgOperational = appnLocalEnTgEntryData.ibmappnLocalEnTgOperational;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgCpCpSession = appnLocalEnTgEntryData.ibmappnLocalEnTgCpCpSession;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgEffCap = appnLocalEnTgEntryData.ibmappnLocalEnTgEffCap;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgConnCost = appnLocalEnTgEntryData.ibmappnLocalEnTgConnCost;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgByteCost = appnLocalEnTgEntryData.ibmappnLocalEnTgByteCost;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgSecurity = appnLocalEnTgEntryData.ibmappnLocalEnTgSecurity;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgDelay = appnLocalEnTgEntryData.ibmappnLocalEnTgDelay;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgModemClass = appnLocalEnTgEntryData.ibmappnLocalEnTgModemClass;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgUsr1 = appnLocalEnTgEntryData.ibmappnLocalEnTgUsr1;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgUsr2 = appnLocalEnTgEntryData.ibmappnLocalEnTgUsr2;
      ibmappnLocalEnTgEntryData.ibmappnLocalEnTgUsr3 = appnLocalEnTgEntryData.ibmappnLocalEnTgUsr3;

      lastSerialNum = serialNum;
      sstrncpy(lastLocalEnTgOrigin,
               ibmappnLocalEnTgOrigin->octet_ptr,
               ibmappnLocalEnTgOrigin->length+1);
      sstrncpy(lastLocalEnTgDest,
               ibmappnLocalEnTgDest->octet_ptr,
               ibmappnLocalEnTgDest->length+1);
      lastIbmappnLocalEnTgNum = ibmappnLocalEnTgNum;
      return(&ibmappnLocalEnTgEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnDirPerf_t *
cip_ibmappnDirPerf_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnDirPerf_t ibmappnDirPerfData;
   static ibmappnDirPerf_T appnDirPerfData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnDirPerfData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnDirPerfData, &appnDirPerfData)) {
   
      ibmappnDirPerfData.ibmappnDirMaxCaches = appnDirPerfData.ibmappnDirMaxCaches;
      ibmappnDirPerfData.ibmappnDirCurCaches = appnDirPerfData.ibmappnDirCurCaches;
      ibmappnDirPerfData.ibmappnDirCurHomeEntries = appnDirPerfData.ibmappnDirCurHomeEntries;
      ibmappnDirPerfData.ibmappnDirRegEntries = appnDirPerfData.ibmappnDirRegEntries;
      ibmappnDirPerfData.ibmappnDirInLocates = appnDirPerfData.ibmappnDirInLocates;
      ibmappnDirPerfData.ibmappnDirInBcastLocates = appnDirPerfData.ibmappnDirInBcastLocates;
      ibmappnDirPerfData.ibmappnDirOutLocates = appnDirPerfData.ibmappnDirOutLocates;
      ibmappnDirPerfData.ibmappnDirOutBcastLocates = appnDirPerfData.ibmappnDirOutBcastLocates;
      ibmappnDirPerfData.ibmappnDirNotFoundLocates = appnDirPerfData.ibmappnDirNotFoundLocates;
      ibmappnDirPerfData.ibmappnDirNotFoundBcastLocates = appnDirPerfData.ibmappnDirNotFoundBcastLocates;
      ibmappnDirPerfData.ibmappnDirLocateOutstands = appnDirPerfData.ibmappnDirLocateOutstands;

      lastSerialNum = serialNum;
      return(&ibmappnDirPerfData);
   }
   else {
      return(NULL);
   }
}

static ibmappnDirEntry_t *
cip_ibmappnDirEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnDirLuName)
{
   static ibmappnDirEntry_t ibmappnDirEntryData;
   static ibmappnDirEntry_T appnDirEntryData;

   static int lastSerialNum = MAXINT;
   static char lastDirLuName[STRING_SIZE_17];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastDirLuName,
                   ibmappnDirLuName->octet_ptr,
                   ibmappnDirLuName->length) == 0))
   {
      return(&ibmappnDirEntryData);
   }
   
   sstrncpy(appnDirEntryData.ibmappnDirLuName,
            ibmappnDirLuName->octet_ptr,
            ibmappnDirLuName->length+1);

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnDirEntryData, &appnDirEntryData)) {
   
      static OctetString dirLuName;
      static OctetString ibmappnDirServerName;
      static OctetString ibmappnDirLuOwnerName;
      
      dirLuName.octet_ptr = appnDirEntryData.ibmappnDirLuName;
      dirLuName.length = strlen(appnDirEntryData.ibmappnDirLuName);;
      ibmappnDirServerName.octet_ptr = appnDirEntryData.ibmappnDirServerName;
      ibmappnDirServerName.length = strlen(appnDirEntryData.ibmappnDirServerName);
      ibmappnDirLuOwnerName.octet_ptr = appnDirEntryData.ibmappnDirLuOwnerName;
      ibmappnDirLuOwnerName.length = strlen(appnDirEntryData.ibmappnDirLuOwnerName);
  
      ibmappnDirEntryData.ibmappnDirLuName = &dirLuName;
      ibmappnDirEntryData.ibmappnDirServerName = &ibmappnDirServerName;
      ibmappnDirEntryData.ibmappnDirLuOwnerName = &ibmappnDirLuOwnerName;
      ibmappnDirEntryData.ibmappnDirLuLocation = appnDirEntryData.ibmappnDirLuLocation;
      ibmappnDirEntryData.ibmappnDirType = appnDirEntryData.ibmappnDirType;
      ibmappnDirEntryData.ibmappnDirWildCard = appnDirEntryData.ibmappnDirWildCard;

      lastSerialNum = serialNum;
      sstrncpy(lastDirLuName,
               ibmappnDirLuName->octet_ptr,
               ibmappnDirLuName->length+1);
      return(&ibmappnDirEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnCosModeEntry_t *
cip_ibmappnCosModeEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnCosModeName)
{
   static ibmappnCosModeEntry_t ibmappnCosModeEntryData;
   static ibmappnCosModeEntry_T appnCosModeEntryData;

   static int lastSerialNum = MAXINT;
   static char lastCosModeName[STRING_SIZE_8];
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastCosModeName,
                   ibmappnCosModeName->octet_ptr,
                   ibmappnCosModeName->length) == 0))
   {
      return(&ibmappnCosModeEntryData);
   }
   
   sstrncpy(appnCosModeEntryData.ibmappnCosModeName,
            ibmappnCosModeName->octet_ptr,
            ibmappnCosModeName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnCosModeEntryData, &appnCosModeEntryData)) {
   
      static OctetString cosModeName;
      static OctetString ibmappnCosModeCosName;
      
      cosModeName.octet_ptr = appnCosModeEntryData.ibmappnCosModeName;
      cosModeName.length = strlen(appnCosModeEntryData.ibmappnCosModeName);
      ibmappnCosModeCosName.octet_ptr = appnCosModeEntryData.ibmappnCosModeCosName;
      ibmappnCosModeCosName.length = strlen(appnCosModeEntryData.ibmappnCosModeCosName);

      ibmappnCosModeEntryData.ibmappnCosModeName = &cosModeName;
      ibmappnCosModeEntryData.ibmappnCosModeCosName = &ibmappnCosModeCosName;

      lastSerialNum = serialNum;
      sstrncpy(lastCosModeName,
               ibmappnCosModeName->octet_ptr,
               ibmappnCosModeName->length+1);
      return(&ibmappnCosModeEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnCosNameEntry_t *
cip_ibmappnCosNameEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnCosName)
{
   static ibmappnCosNameEntry_t ibmappnCosNameEntryData;
   static ibmappnCosNameEntry_T appnCosNameEntryData;

   static int lastSerialNum = MAXINT;
   static char lastCosName[STRING_SIZE_8];
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastCosName,
                   ibmappnCosName->octet_ptr,
                   ibmappnCosName->length) == 0))
   {
      return(&ibmappnCosNameEntryData);
   }
   
   sstrncpy(appnCosNameEntryData.ibmappnCosName,
            ibmappnCosName->octet_ptr,
            ibmappnCosName->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnCosNameEntryData, &appnCosNameEntryData)) {
   
      static OctetString cosName;
      
      cosName.octet_ptr = appnCosNameEntryData.ibmappnCosName;
      cosName.length = strlen(appnCosNameEntryData.ibmappnCosName);

      ibmappnCosNameEntryData.ibmappnCosName = &cosName;
      ibmappnCosNameEntryData.ibmappnCosTransPriority = appnCosNameEntryData.ibmappnCosTransPriority;

      lastSerialNum = serialNum;
      sstrncpy(lastCosName,
               ibmappnCosName->octet_ptr,
               ibmappnCosName->length+1);
      return(&ibmappnCosNameEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnCosNodeRowEntry_t *
cip_ibmappnCosNodeRowEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnCosNodeRowName,
   long ibmappnCosNodeRowIndex)
{
   static ibmappnCosNodeRowEntry_t ibmappnCosNodeRowEntryData;
   static ibmappnCosNodeRowEntry_T appnCosNodeRowEntryData;

   static int lastSerialNum = MAXINT;
   static char lastCosNodeRowName[STRING_SIZE_8];
   static long lastIbmappnCosNodeRowIndex;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastCosNodeRowName,
                   ibmappnCosNodeRowName->octet_ptr,
                   ibmappnCosNodeRowName->length) == 0)
       && (lastIbmappnCosNodeRowIndex == ibmappnCosNodeRowIndex))
   {
      return(&ibmappnCosNodeRowEntryData);
   }
   
   sstrncpy(appnCosNodeRowEntryData.ibmappnCosNodeRowName,
            ibmappnCosNodeRowName->octet_ptr,
            ibmappnCosNodeRowName->length+1);
   appnCosNodeRowEntryData.ibmappnCosNodeRowIndex = ibmappnCosNodeRowIndex;
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnCosNodeRowEntryData, &appnCosNodeRowEntryData)) {
   
      static OctetString cosNodeRowName;
      static OctetString ibmappnCosNodeRowWgt;
      
      cosNodeRowName.octet_ptr = appnCosNodeRowEntryData.ibmappnCosNodeRowName;
      cosNodeRowName.length = strlen(appnCosNodeRowEntryData.ibmappnCosNodeRowName);
      ibmappnCosNodeRowWgt.octet_ptr = appnCosNodeRowEntryData.ibmappnCosNodeRowWgt;
      ibmappnCosNodeRowWgt.length = strlen(appnCosNodeRowEntryData.ibmappnCosNodeRowWgt);
      
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowName = &cosNodeRowName;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowIndex = appnCosNodeRowEntryData.ibmappnCosNodeRowIndex;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowWgt = &ibmappnCosNodeRowWgt;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowResistMin = appnCosNodeRowEntryData.ibmappnCosNodeRowResistMin;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowResistMax = appnCosNodeRowEntryData.ibmappnCosNodeRowResistMax;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowMinCongestAllow = appnCosNodeRowEntryData.ibmappnCosNodeRowMinCongestAllow;
      ibmappnCosNodeRowEntryData.ibmappnCosNodeRowMaxCongestAllow = appnCosNodeRowEntryData.ibmappnCosNodeRowMaxCongestAllow;

      lastSerialNum = serialNum;
      sstrncpy(lastCosNodeRowName,
               ibmappnCosNodeRowName->octet_ptr,
               ibmappnCosNodeRowName->length+1);
      lastIbmappnCosNodeRowIndex = ibmappnCosNodeRowIndex;
      return(&ibmappnCosNodeRowEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnCosTgRowEntry_t *
cip_ibmappnCosTgRowEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnCosTgRowName,
   long ibmappnCosTgRowIndex)
{
   static ibmappnCosTgRowEntry_t ibmappnCosTgRowEntryData;
   static ibmappnCosTgRowEntry_T appnCosTgRowEntryData;

   static int lastSerialNum = MAXINT;
   static char lastCosTgRowName[STRING_SIZE_8];
   static long lastIbmappnCosTgRowIndex;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastCosTgRowName,
                   ibmappnCosTgRowName->octet_ptr,
                   ibmappnCosTgRowName->length) == 0)
       && (lastIbmappnCosTgRowIndex == ibmappnCosTgRowIndex))
   {
      return(&ibmappnCosTgRowEntryData);
   }
   
   sstrncpy(appnCosTgRowEntryData.ibmappnCosTgRowName,
            ibmappnCosTgRowName->octet_ptr,
            ibmappnCosTgRowName->length+1);
   appnCosTgRowEntryData.ibmappnCosTgRowIndex = ibmappnCosTgRowIndex;

   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnCosTgRowEntryData, &appnCosTgRowEntryData)) {
   
      static OctetString cosTgRowName;
      static OctetString ibmappnCosTgRowWgt;
      
      cosTgRowName.octet_ptr = appnCosTgRowEntryData.ibmappnCosTgRowName;
      cosTgRowName.length = strlen(appnCosTgRowEntryData.ibmappnCosTgRowName);
      ibmappnCosTgRowWgt.octet_ptr = appnCosTgRowEntryData.ibmappnCosTgRowWgt;
      ibmappnCosTgRowWgt.length = strlen(appnCosTgRowEntryData.ibmappnCosTgRowWgt);

      ibmappnCosTgRowEntryData.ibmappnCosTgRowName = &cosTgRowName;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowIndex = appnCosTgRowEntryData.ibmappnCosTgRowIndex;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowWgt = &ibmappnCosTgRowWgt;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowEffCapMin = appnCosTgRowEntryData.ibmappnCosTgRowEffCapMin;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowEffCapMax = appnCosTgRowEntryData.ibmappnCosTgRowEffCapMax;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowConnCostMin = appnCosTgRowEntryData.ibmappnCosTgRowConnCostMin;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowConnCostMax = appnCosTgRowEntryData.ibmappnCosTgRowConnCostMax;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowByteCostMin = appnCosTgRowEntryData.ibmappnCosTgRowByteCostMin;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowByteCostMax = appnCosTgRowEntryData.ibmappnCosTgRowByteCostMax;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowSecurityMin = appnCosTgRowEntryData.ibmappnCosTgRowSecurityMin;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowSecurityMax = appnCosTgRowEntryData.ibmappnCosTgRowSecurityMax;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowDelayMin = appnCosTgRowEntryData.ibmappnCosTgRowDelayMin;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowDelayMax = appnCosTgRowEntryData.ibmappnCosTgRowDelayMax;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr1Min = appnCosTgRowEntryData.ibmappnCosTgRowUsr1Min;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr1Max = appnCosTgRowEntryData.ibmappnCosTgRowUsr1Max;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr2Min = appnCosTgRowEntryData.ibmappnCosTgRowUsr2Min;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr2Max = appnCosTgRowEntryData.ibmappnCosTgRowUsr2Max;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr3Min = appnCosTgRowEntryData.ibmappnCosTgRowUsr3Min;
      ibmappnCosTgRowEntryData.ibmappnCosTgRowUsr3Max = appnCosTgRowEntryData.ibmappnCosTgRowUsr3Max;

      lastSerialNum = serialNum;
      sstrncpy(lastCosTgRowName,
               ibmappnCosTgRowName->octet_ptr,
               ibmappnCosTgRowName->length+1);
      lastIbmappnCosTgRowIndex = ibmappnCosTgRowIndex;
      return(&ibmappnCosTgRowEntryData);
   }
   else {
      return(NULL);
   }
}

static ibmappnIsInGlobal_t *
cip_ibmappnIsInGlobal_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   static ibmappnIsInGlobal_t ibmappnIsInGlobalData;
   static ibmappnIsInGlobal_T appnIsInGlobalData;

   static int lastSerialNum = MAXINT;
  
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&ibmappnIsInGlobalData);
   
   if (GetAppnControlBlock(EXACT, contextInfo, SYSMGT_appnIsInGlobalData, &appnIsInGlobalData)) {
   
      ibmappnIsInGlobalData.ibmappnIsInGlobeStatus = appnIsInGlobalData.ibmappnIsInGlobeStatus;
      ibmappnIsInGlobalData.ibmappnIsInGlobeRscv = appnIsInGlobalData.ibmappnIsInGlobeRscv;
      ibmappnIsInGlobalData.ibmappnIsInGlobeRscvTime = appnIsInGlobalData.ibmappnIsInGlobeRscvTime;
      ibmappnIsInGlobalData.ibmappnIsInGlobeCtrStatus = appnIsInGlobalData.ibmappnIsInGlobeCtrStatus;
      ibmappnIsInGlobalData.ibmappnIsInGlobeCtrStatusTime = appnIsInGlobalData.ibmappnIsInGlobeCtrStatusTime;

      lastSerialNum = serialNum;
      return(&ibmappnIsInGlobalData);
   }
   else {
      return(NULL);
   }
}

static ibmappnIsInEntry_t *
cip_ibmappnIsInEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   OctetString * ibmappnIsInFqLuName,
   OctetString * ibmappnIsInPcid)
{
   static ibmappnIsInEntry_t ibmappnIsInEntryData;
   static ibmappnIsInEntry_T appnIsInEntryData;

   static int lastSerialNum = MAXINT;
   static char lastIsInFqLuName[STRING_SIZE_17];
   static char lastIsInPcid[STRING_SIZE_8];

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((lastSerialNum == serialNum)
       && (strncmp(lastIsInFqLuName,
                   ibmappnIsInFqLuName->octet_ptr,
                   ibmappnIsInFqLuName->length) == 0)
       && (strncmp(lastIsInPcid,
                   ibmappnIsInPcid->octet_ptr,
                   ibmappnIsInPcid->length) == 0))
   {
      return(&ibmappnIsInEntryData);
   }
   
   sstrncpy(appnIsInEntryData.ibmappnIsInFqLuName,
            ibmappnIsInFqLuName->octet_ptr,
            ibmappnIsInFqLuName->length+1);
   sstrncpy(appnIsInEntryData.ibmappnIsInPcid,
            ibmappnIsInPcid->octet_ptr,
            ibmappnIsInPcid->length+1);
   
   if (GetAppnControlBlock(searchType, contextInfo, SYSMGT_appnIsInEntryData, &appnIsInEntryData)) {
   
      static OctetString isInFqLuName;
      static OctetString isInPcid;
      static OctetString ibmappnIsInPriLuName;
      static OctetString ibmappnIsInSecLuName;
      static OctetString ibmappnIsInModeName;
      static OctetString ibmappnIsInCosName;
      static OctetString ibmappnIsInPsAdjCpName;
      static OctetString ibmappnIsInSsAdjCpName;
      static OctetString ibmappnIsInRouteInfo;
      
      isInFqLuName.octet_ptr = appnIsInEntryData.ibmappnIsInFqLuName;
      isInFqLuName.length = strlen(appnIsInEntryData.ibmappnIsInFqLuName);
      isInPcid.octet_ptr = appnIsInEntryData.ibmappnIsInPcid;
      isInPcid.length = strlen(appnIsInEntryData.ibmappnIsInPcid);
      ibmappnIsInPriLuName.octet_ptr = appnIsInEntryData.ibmappnIsInPriLuName;
      ibmappnIsInPriLuName.length = strlen(appnIsInEntryData.ibmappnIsInPriLuName);
      ibmappnIsInSecLuName.octet_ptr = appnIsInEntryData.ibmappnIsInSecLuName;
      ibmappnIsInSecLuName.length = strlen(appnIsInEntryData.ibmappnIsInSecLuName);
      ibmappnIsInModeName.octet_ptr = appnIsInEntryData.ibmappnIsInModeName;
      ibmappnIsInModeName.length = strlen(appnIsInEntryData.ibmappnIsInModeName);
      ibmappnIsInCosName.octet_ptr = appnIsInEntryData.ibmappnIsInCosName;
      ibmappnIsInCosName.length = strlen(appnIsInEntryData.ibmappnIsInCosName);
      ibmappnIsInPsAdjCpName.octet_ptr = appnIsInEntryData.ibmappnIsInPsAdjCpName;
      ibmappnIsInPsAdjCpName.length = strlen(appnIsInEntryData.ibmappnIsInPsAdjCpName);
      ibmappnIsInSsAdjCpName.octet_ptr = appnIsInEntryData.ibmappnIsInSsAdjCpName;
      ibmappnIsInSsAdjCpName.length = strlen(appnIsInEntryData.ibmappnIsInSsAdjCpName);
      ibmappnIsInRouteInfo.octet_ptr = appnIsInEntryData.ibmappnIsInRouteInfo;
      ibmappnIsInRouteInfo.length = strlen(appnIsInEntryData.ibmappnIsInRouteInfo);
      
      ibmappnIsInEntryData.ibmappnIsInFqLuName = &isInFqLuName;
      ibmappnIsInEntryData.ibmappnIsInPcid = &isInPcid;
      ibmappnIsInEntryData.ibmappnIsInPriLuName = &ibmappnIsInPriLuName;
      ibmappnIsInEntryData.ibmappnIsInSecLuName = &ibmappnIsInSecLuName;
      ibmappnIsInEntryData.ibmappnIsInModeName = &ibmappnIsInModeName;
      ibmappnIsInEntryData.ibmappnIsInCosName = &ibmappnIsInCosName;
      ibmappnIsInEntryData.ibmappnIsInTransPriority = appnIsInEntryData.ibmappnIsInTransPriority;
      ibmappnIsInEntryData.ibmappnIsInSessType = appnIsInEntryData.ibmappnIsInSessType;
      ibmappnIsInEntryData.ibmappnIsInSessState = appnIsInEntryData.ibmappnIsInSessState;
      ibmappnIsInEntryData.ibmappnIsInSessStartTime = appnIsInEntryData.ibmappnIsInSessStartTime;
      ibmappnIsInEntryData.ibmappnIsInSessUpTime = appnIsInEntryData.ibmappnIsInSessUpTime;
      ibmappnIsInEntryData.ibmappnIsInCtrUpTime = appnIsInEntryData.ibmappnIsInCtrUpTime;
      ibmappnIsInEntryData.ibmappnIsInP2SFmdPius = appnIsInEntryData.ibmappnIsInP2SFmdPius;
      ibmappnIsInEntryData.ibmappnIsInS2PFmdPius = appnIsInEntryData.ibmappnIsInS2PFmdPius;
      ibmappnIsInEntryData.ibmappnIsInP2SNonFmdPius = appnIsInEntryData.ibmappnIsInP2SNonFmdPius;
      ibmappnIsInEntryData.ibmappnIsInS2PNonFmdPius = appnIsInEntryData.ibmappnIsInS2PNonFmdPius;
      ibmappnIsInEntryData.ibmappnIsInP2SFmdBytes = appnIsInEntryData.ibmappnIsInP2SFmdBytes;
      ibmappnIsInEntryData.ibmappnIsInS2PFmdBytes = appnIsInEntryData.ibmappnIsInS2PFmdBytes;
      ibmappnIsInEntryData.ibmappnIsInP2SNonFmdBytes = appnIsInEntryData.ibmappnIsInP2SNonFmdBytes;
      ibmappnIsInEntryData.ibmappnIsInS2PNonFmdBytes = appnIsInEntryData.ibmappnIsInS2PNonFmdBytes;
      ibmappnIsInEntryData.ibmappnIsInPsAdjCpName = &ibmappnIsInPsAdjCpName;
      ibmappnIsInEntryData.ibmappnIsInPsAdjTgNum = appnIsInEntryData.ibmappnIsInPsAdjTgNum;
      ibmappnIsInEntryData.ibmappnIsInPsSendMaxBtuSize = appnIsInEntryData.ibmappnIsInPsSendMaxBtuSize;
      ibmappnIsInEntryData.ibmappnIsInPsSendPacingType = appnIsInEntryData.ibmappnIsInPsSendPacingType;
      ibmappnIsInEntryData.ibmappnIsInPsSendRpc = appnIsInEntryData.ibmappnIsInPsSendRpc;
      ibmappnIsInEntryData.ibmappnIsInPsSendNxWndwSize = appnIsInEntryData.ibmappnIsInPsSendNxWndwSize;
      ibmappnIsInEntryData.ibmappnIsInPsRecvPacingType = appnIsInEntryData.ibmappnIsInPsRecvPacingType;
      ibmappnIsInEntryData.ibmappnIsInPsRecvRpc = appnIsInEntryData.ibmappnIsInPsRecvRpc;
      ibmappnIsInEntryData.ibmappnIsInPsRecvNxWndwSize = appnIsInEntryData.ibmappnIsInPsRecvNxWndwSize;
      ibmappnIsInEntryData.ibmappnIsInSsAdjCpName = &ibmappnIsInSsAdjCpName;
      ibmappnIsInEntryData.ibmappnIsInSsAdjTgNum = appnIsInEntryData.ibmappnIsInSsAdjTgNum;
      ibmappnIsInEntryData.ibmappnIsInSsSendMaxBtuSize = appnIsInEntryData.ibmappnIsInSsSendMaxBtuSize;
      ibmappnIsInEntryData.ibmappnIsInSsSendPacingType = appnIsInEntryData.ibmappnIsInSsSendPacingType;
      ibmappnIsInEntryData.ibmappnIsInSsSendRpc = appnIsInEntryData.ibmappnIsInSsSendRpc;
      ibmappnIsInEntryData.ibmappnIsInSsSendNxWndwSize = appnIsInEntryData.ibmappnIsInSsSendNxWndwSize;
      ibmappnIsInEntryData.ibmappnIsInSsRecvPacingType = appnIsInEntryData.ibmappnIsInSsRecvPacingType;
      ibmappnIsInEntryData.ibmappnIsInSsRecvRpc = appnIsInEntryData.ibmappnIsInSsRecvRpc;
      ibmappnIsInEntryData.ibmappnIsInSsRecvNxWndwSize = appnIsInEntryData.ibmappnIsInSsRecvNxWndwSize;
      ibmappnIsInEntryData.ibmappnIsInRouteInfo = &ibmappnIsInRouteInfo;

      lastSerialNum = serialNum;
      sstrncpy(lastIsInFqLuName,
               ibmappnIsInFqLuName->octet_ptr,
               ibmappnIsInFqLuName->length+1);
      sstrncpy(lastIsInPcid,
               ibmappnIsInPcid->octet_ptr,
               ibmappnIsInPcid->length+1);
      return(&ibmappnIsInEntryData);
   }
   else {
      return(NULL);
   }
}

static void snmp_add_appn_entity (const char* entityName, const char* entitydescr)
{
static   const OID   appnMIBoid = {LNibm6611, (ulong *)IDibm6611};
   snmp_add_logical_entity(entityName,entitydescr, &appnMIBoid);
}


static AppnFunc cipAppnFunc = {
  cip_ibmappnGeneralInfoAndCaps_get,
  cip_ibmappnNnUniqueInfoAndCaps_get,
  cip_ibmappnEnUniqueCaps_get,
  cip_ibmappnNodePortEntry_get,
  cip_ibmappnNodePortIpEntry_get,
  cip_ibmappnNodePortDlsEntry_get,
  cip_ibmappnNodePortTrEntry_get,
  cip_ibmappnNodePortDlcTraceEntry_get,
  cip_ibmappnNodeLsEntry_get,
  cip_ibmappnNodeLsIpEntry_get,
  cip_ibmappnNodeLsDlsEntry_get,
  cip_ibmappnNodeLsTrEntry_get,
  cip_ibmappnNodeLsStatusEntry_get,
  cip_ibmappnSnmpInformation_get,
  cip_ibmappnMemoryUse_get,
  cip_ibmappnXidInformation_get,
  cip_ibmappnNnTopo_get,
  cip_ibmappnNnTopoRouteEntry_get,
  cip_ibmappnNnAdjNodeEntry_get,
  cip_ibmappnNnTopologyEntry_get,
  cip_ibmappnNnTgTopologyEntry_get,
  cip_ibmappnNnTopologyFREntry_get,
  cip_ibmappnNnTgTopologyFREntry_get,
  cip_ibmappnLocalGeneral_get,
  cip_ibmappnLocalNnSpecific_get,
  cip_ibmappnLocalTgEntry_get,
  cip_ibmappnLocalEnEntry_get,
  cip_ibmappnLocalEnTgEntry_get,
  cip_ibmappnDirPerf_get,
  cip_ibmappnDirEntry_get,
  cip_ibmappnCosModeEntry_get,
  cip_ibmappnCosNameEntry_get,
  cip_ibmappnCosNodeRowEntry_get,
  cip_ibmappnCosTgRowEntry_get,
  cip_ibmappnIsInGlobal_get,
  cip_ibmappnIsInEntry_get
  };

void
init_cipappnmib (void)
{
  reg_add_snmp_add_appn_entity(snmp_add_appn_entity, "snmp_add_appn_entity");
  appnCip = &cipAppnFunc; /* add hook to sub agent */
}

      
