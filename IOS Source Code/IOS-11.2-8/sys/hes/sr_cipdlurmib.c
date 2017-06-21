/* $Id: sr_cipdlurmib.c,v 3.1.4.3 1996/09/02 08:19:31 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_cipdlurmib.c,v $
 *------------------------------------------------------------------
 * DLUR cip specific SNMP sub-agent code
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_cipdlurmib.c,v $
 * Revision 3.1.4.3  1996/09/02  08:19:31  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/08/05  07:24:27  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:19:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:42  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:00:59  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "../appn/sr_appndlurmib.h"
#include "if_tn3270s.h"
#include "sysmgt_dlurmib.h"
#include "../entity/entity_api.h"
#include "cbus_registry.h"


static boolean GetDlurControlBlock (int search, ContextInfo *contextInfo, sysmgt_dlur_table_t type, void* data)
{
   hwidbtype* hwidb;
   hwidb = GetCipIdbFromCommunity(contextInfo);
   if (hwidb != NULL) {
      return get_dlur(search == EXACT, hwidb, type, data);
   }
   return FALSE;

}

static dlurNodeCapabilities_t *
cip_dlurNodeCapabilities_get (
   int serialNum, 
   ContextInfo *contextInfo, 
   int nominator)
{
   static dlurNodeCapabilities_t dlurNodeCapabilitiesData;
   static dlurNodeCapabilities_T dlurNodeCap;
   
   static int lastSerialNum = MAXINT;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum)
      return(&dlurNodeCapabilitiesData);

   if (GetDlurControlBlock(EXACT, contextInfo, SYSMGT_DLUR_NODE_CAP, &dlurNodeCap)){

      static OctetString release;

      release.octet_ptr = dlurNodeCap.dlurReleaseLevel;
      release.length = 4;

      dlurNodeCapabilitiesData.dlurReleaseLevel = &release;
      dlurNodeCapabilitiesData.dlurANSsupport = dlurNodeCap.dlurANSsupport;
      dlurNodeCapabilitiesData.dlurMultiSubnetSupport = dlurNodeCap.dlurMultiSubnetSupport;
       
      lastSerialNum = serialNum;
      return(&dlurNodeCapabilitiesData);
   }
   else {
      return(NULL);
   }
}

static dlurPuEntry_t *
cip_dlurPuEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long dlurPuIndex)
{
   static dlurPuEntry_t dlurPuEntryData;
   static dlurPuEntry_T dlurPuEntry;

   static int lastSerialNum = MAXINT;
   static long lastdlurPuIndex = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum && lastdlurPuIndex == dlurPuIndex)
      return(&dlurPuEntryData);

   dlurPuEntry.dlurPuIndex = dlurPuIndex;
   
   if (GetDlurControlBlock(searchType, contextInfo, SYSMGT_DLUR_PU_ENTRY, &dlurPuEntry)){

      static OctetString puName;
      static OctetString activeDlsName;
      static OctetString primDlusName;
      static OctetString backDlusName;
        
      puName.octet_ptr = dlurPuEntry.dlurPuName;
      puName.length = strlen(dlurPuEntry.dlurPuName);
      activeDlsName.octet_ptr = dlurPuEntry.dlurPuActiveDlusName;
      activeDlsName.length = strlen(dlurPuEntry.dlurPuActiveDlusName);
      primDlusName.octet_ptr = dlurPuEntry.dlurPuDefPrimDlusName;
      primDlusName.length = strlen(dlurPuEntry.dlurPuDefPrimDlusName);
      backDlusName.octet_ptr = dlurPuEntry.dlurPuDefBackDlusName;
      backDlusName.length = strlen(dlurPuEntry.dlurPuDefBackDlusName);

      dlurPuEntryData.dlurPuIndex = dlurPuEntry.dlurPuIndex;
      dlurPuEntryData.dlurPuName = &puName;
      dlurPuEntryData.dlurPuStatus = dlurPuEntry.dlurPuStatus;
      dlurPuEntryData.dlurPuANSsupport = dlurPuEntry.dlurPuANSsupport;
      dlurPuEntryData.dlurPuLocation = dlurPuEntry.dlurPuLocation;
      dlurPuEntryData.dlurPuDlusSessnStatus = dlurPuEntry.dlurPuDlusSessnStatus;
      dlurPuEntryData.dlurPuActiveDlusName = &activeDlsName;
      dlurPuEntryData.dlurPuDefPrimDlusName = &primDlusName;
      dlurPuEntryData.dlurPuDefBackDlusName = &backDlusName;

      lastSerialNum = serialNum;
      lastdlurPuIndex = dlurPuIndex;
      return(&dlurPuEntryData);
   }
   else{
      return(NULL);
   }
}

static dlurDlusEntry_t *
cip_dlurDlusEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long dlurDlusIndex)
{
   static dlurDlusEntry_t dlurDlusEntryData;
   static dlurDlusEntry_T dlusEntryData;

   static int lastSerialNum = MAXINT;
   static long lastdlurDlusIndex = MAXLONG;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if (lastSerialNum == serialNum && lastdlurDlusIndex == dlurDlusIndex)
      return(&dlurDlusEntryData);

   dlusEntryData.dlurDlusIndex = dlurDlusIndex;

   if (GetDlurControlBlock(searchType, contextInfo, SYSMGT_DLUR_DLUS_ENTRY, &dlusEntryData)){

      static OctetString dlurDlusName;

      dlurDlusName.octet_ptr = dlusEntryData.dlurDlusName;
      dlurDlusName.length = strlen(dlusEntryData.dlurDlusName);
      dlurDlusEntryData.dlurDlusIndex = dlusEntryData.dlurDlusIndex;
      dlurDlusEntryData.dlurDlusName = &dlurDlusName;
      dlurDlusEntryData.dlurDlusSessnStatus = dlusEntryData.dlurDlusSessnStatus;

      lastSerialNum = serialNum;
      lastdlurDlusIndex = dlurDlusIndex;
      return(&dlurDlusEntryData);
   }
   else{
      return(NULL);
   }
}

static void snmp_add_dlur_entity (const char* entityName, const char* entitydescr)
{
static const OID   dlurMIBoid = {LNappnDlur, (ulong *)IDappnDlur};
    snmp_add_logical_entity(entityName,entitydescr, &dlurMIBoid);
}


static SubAgentFunc dlurFunc = {
   cip_dlurNodeCapabilities_get,
   cip_dlurPuEntry_get,
   cip_dlurDlusEntry_get
};


/* enable subagent */
void init_cipdlurmib ()
{
   reg_add_snmp_add_dlur_entity(snmp_add_dlur_entity, "snmp_add_dlur_entity");
   dlurCip = &dlurFunc;     /* add hook to the sub-agent */
}

      
