/* $Id: if_tn3270s.c,v 3.1.4.9 1996/09/02 08:19:23 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_tn3270s.c,v $
 *------------------------------------------------------------------
 * TN3270 server network management
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_tn3270s.c,v $
 * Revision 3.1.4.9  1996/09/02  08:19:23  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/08/07  08:59:09  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/08/07  07:35:38  bchan
 * CSCdi64701:  crash when doing getmany tn3270sGlobalTable
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/08/05  07:24:20  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/07/17  06:52:07  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/07/15  07:28:19  bchan
 * CSCdi62509:  Change tn3270 server mib
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/06/03  07:34:36  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/21  06:29:27  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1.4.1  1996/05/17  11:18:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.4  1996/05/12  08:10:06  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.3  1996/04/03  14:30:41  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.2.2  1996/03/28  11:28:41  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:21:39  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:00:31  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include "interface_private.h"
#include "sysmgt_tn3270s.h"
#include "sysmgt_snanaumib.h"
#include "if_cip_sysmgt_rpc.h"
#include "if_tn3270s.h"
#include "../h/ieee.h"
#include "../dspu/tn3270s_types.h"
#include "logger.h"
#include "../os/free.h"
#include "../ui/debug.h"
#include "../if/channel_debug.h"
#include "../snmp/ifmib_registry.h"
#include "../snmp/snmp_api.h"
#include "sysmgt_cipappn.h"
#include "sysmgt_dlurmib.h"
#include "sysmgt_tn3270s.h"
#include "if_cip.h"
#include "registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "../hes/cbus_registry.h"
#include "../entity/entity_api.h"
#include "subsys.h"
#include "tnlogscat.h"          /* log-entry scatter functions   */


typedef struct Tn3270sLen {
   ushort index_length;
   ushort data_length;
} Tn3270sLen;

static const Tn3270sLen tn3270sLen[] = {
    {0                          , sizeof(Tn3270sGblEntry_t)     },
    {2*sizeof(long)             , sizeof(Tn3270sStatsEntry_t)   },
    {sizeof(long)               , sizeof(Tn3270sPuEntry_t)      },
    {2*sizeof(long)             , sizeof(Tn3270sIpEntry_t)      },
    {sizeof(LuControlCommon)    , sizeof(Tn3270sLuEntry_t)      },
    {2*sizeof(long)             , sizeof(Tn3270sLogEntry_t)     },
    {sizeof(long)               , sizeof(SnaNodeAdminEntry_t)   },
    {sizeof(long)               , sizeof(SnaNodeOperEntry_t)    },
    {sizeof(long)               , sizeof(SnaPu20StatsEntry_t)   },
    {sizeof(long)               , sizeof(SnaNodeLinkAdminEntry_t)},
    {sizeof(long)               , sizeof(SnaNodeLinkOperEntry_t)},
    {sizeof(LuControlCommon)    , sizeof(SnaLuAdminEntry_t)     },
    {sizeof(LuControlCommon)    , sizeof(SnaLuOperEntry_t)      },
    {sizeof(LuControlCommon)    , sizeof(SnaLuSessnEntry_t)     },
    {sizeof(LuControlCommon)    , sizeof(SnaLuSessnStatsEntry_t)},
    {2*sizeof(long)             , sizeof(Tn3270sLogDefEntry_t)  }
   ,{2*sizeof(long)             , sizeof(Tn3270sExtra_t)  }


};

static const Tn3270sLen appnLen[] = {
{ 0,  sizeof(ibmappnGeneralInfoAndCaps_T)},
{ 0,  sizeof(ibmappnNnUniqueInfoAndCaps_T)},
{ 0,  sizeof(ibmappnEnUniqueCaps_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodePortEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodePortIpEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodePortDlsEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodePortTrEntry_T)},
{ STRING_SIZE_8 * sizeof(char) + sizeof(long),  sizeof(ibmappnNodePortDlcTraceEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodeLsEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodeLsIpEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodeLsDlsEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnNodeLsTrEntry_T)},
{ sizeof(long),  sizeof(ibmappnNodeLsStatusEntry_T)},
{ 0,  sizeof(ibmappnSnmpInformation_T)},
{ 0,  sizeof(ibmappnMemoryUse_T)},
{ 0,  sizeof(ibmappnXidInformation_T)},
{ 0,  sizeof(ibmappnNnTopo_T)},
{ STRING_SIZE_UNDEF * sizeof(char),  sizeof(ibmappnNnTopoRouteEntry_T)},
{ STRING_SIZE_17 * sizeof(char),  sizeof(ibmappnNnAdjNodeEntry_T)},
{ STRING_SIZE_17 * sizeof(char),  sizeof(ibmappnNnTopologyEntry_T)},
{ STRING_SIZE_17 * sizeof(char) + STRING_SIZE_17 * sizeof(char) + sizeof(long),  sizeof(ibmappnNnTgTopologyEntry_T)},
{ STRING_SIZE_17 * sizeof(char) + sizeof(long),  sizeof(ibmappnNnTopologyFREntry_T)},
{ STRING_SIZE_17 * sizeof(char) + STRING_SIZE_17 * sizeof(char) + 2 * sizeof(long),  sizeof(ibmappnNnTgTopologyFREntry_T)},
{ 0,  sizeof(ibmappnLocalGeneral_T)},
{ 0,  sizeof(ibmappnLocalNnSpecific_T)},
{ STRING_SIZE_17 * sizeof(char) + sizeof(long),  sizeof(ibmappnLocalTgEntry_T)},
{ STRING_SIZE_17 * sizeof(char),  sizeof(ibmappnLocalEnEntry_T)},
{ STRING_SIZE_17 * sizeof(char) + STRING_SIZE_17 * sizeof(char) + sizeof(long),  sizeof(ibmappnLocalEnTgEntry_T)},
{ 0,  sizeof(ibmappnDirPerf_T)},
{ STRING_SIZE_17 * sizeof(char),  sizeof(ibmappnDirEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnCosModeEntry_T)},
{ STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnCosNameEntry_T)},
{ STRING_SIZE_8 * sizeof(char) + sizeof(long),  sizeof(ibmappnCosNodeRowEntry_T)},
{ STRING_SIZE_8 * sizeof(char) + sizeof(long),  sizeof(ibmappnCosTgRowEntry_T)},
{ 0,  sizeof(ibmappnIsInGlobal_T)},
{ STRING_SIZE_17 * sizeof(char) + STRING_SIZE_8 * sizeof(char),  sizeof(ibmappnIsInEntry_T)}

};

static const Tn3270sLen dlurLen[] = {
    {0,              sizeof(dlurNodeCapabilities_T)},
    {sizeof(long),   sizeof(dlurPuEntry_T)},
    {sizeof(long),   sizeof(dlurDlusEntry_T)}
};

static char *const netmgt_tn3270s_table[] = {
    "SYSMGT_TN3270S_GLOBAL",
    "SYSMGT_TN3270S_STATS",
    "SYSMGT_TN3270S_PU",
    "SYSMGT_TN3270S_IP",
    "SYSMGT_TN3270S_LU",
    "SYSMGT_TN3270S_LOG",
    "SYSMGT_TN3270S_NODE_ADMIN",
    "SYSMGT_TN3270S_NODE_OPER",
    "SYSMGT_TN3270S_PU_STAT",
    "SYSMGT_TN3270S_LINK_ADMIN",
    "SYSMGT_TN3270S_LINK_OPER",
    "SYSMGT_TN3270S_LU_ADMIN",
    "SYSMGT_TN3270S_LU_OPER",
    "SYSMGT_TN3270S_LU_SESSN",
    "SYSMGT_TN3270S_LU_SESSN_STAT",
    "SYSMGT_TN3270S_LOG_DEF"
   ,"SYSMGT_TN3270S_EXTRA"
};

static char *const netmgt_appn_table[] = {

  "SYSMGT_appnGeneralInfoAndCapsData",
  "SYSMGT_appnNnUniqueInfoAndCapsData",
  "SYSMGT_appnEnUniqueCapsData",
  "SYSMGT_appnNodePortEntryData",
  "SYSMGT_appnNodePortIpEntryData",
  "SYSMGT_appnNodePortDlsEntryData",
  "SYSMGT_appnNodePortTrEntryData", 
  "SYSMGT_appnNodePortDlcTraceEntryData",
  "SYSMGT_appnNodeLsEntryData",
  "SYSMGT_appnNodeLsIpEntryData",
  "SYSMGT_appnNodeLsDlsEntryData",
  "SYSMGT_appnNodeLsTrEntryData",
  "SYSMGT_appnNodeLsStatusEntryData",
  "SYSMGT_appnSnmpInformationData",
  "SYSMGT_appnMemoryUseData",
  "SYSMGT_appnXidInformationData",
  "SYSMGT_appnNnTopoData",
  "SYSMGT_appnNnTopoRouteEntryData",
  "SYSMGT_appnNnAdjNodeEntryData",
  "SYSMGT_appnNnTopologyEntryData",
  "SYSMGT_appnNnTgTopologyEntryData",
  "SYSMGT_appnNnTopologyFREntryData",
  "SYSMGT_appnNnTgTopologyFREntryData",
  "SYSMGT_appnLocalGeneralData",
  "SYSMGT_appnLocalNnSpecificData",
  "SYSMGT_appnLocalTgEntryData",
  "SYSMGT_appnLocalEnEntryData",
  "SYSMGT_appnLocalEnTgEntryData",
  "SYSMGT_appnDirPerfData",
  "SYSMGT_appnDirEntryData",
  "SYSMGT_appnCosModeEntryData",
  "SYSMGT_appnCosNameEntryData",
  "SYSMGT_appnCosNodeRowEntryData",
  "SYSMGT_appnCosTgRowEntryData",
  "SYSMGT_appnIsInGlobalData",
  "SYSMGT_appnIsInEntryData"
};

static char *const netmgt_dlur_table[] = {
    "SYSMGT_DLUR_NODE_CAP",
    "SYSMGT_DLUR_PU_ENTRY",
    "SYSMGT_DLUR_DLUS_ENTRY"
};

#define TN3270S_DEBUG TRUE
static void
tn3270s_dump_buf (char  *pDesc,
                  void  *buf,
		  ushort len)
{
    char *p;
    p = buf;
    
    if (TN3270S_DEBUG && channel_debug) {
	if (pDesc)
	    buginf("\n%s", pDesc);
	memory_dump(p, &p[len-1],
		    FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
    }
}

static const char tn3270s_entity_name[] = "tn3270Server";

/* we will maintain tn3270 server idb. This is used to speed up the scanning
 * of hwidb and also used for the entity mib. 
 */
static hwidbtype* tn3270serverIdb[MAX_SLOTS];


/* save tn3270 idb */
/* returns true if this is the first time this slot is used */
static boolean SaveTn3270Idb (hwidbtype  *hwidb)
{
   int i;
   hwidbtype* idb;
   uint slot;

   slot = hwidb->slot;
   for (i = 0; i < MAX_SLOTS; i++) {
      if (((idb = tn3270serverIdb[i]) != NULL) && idb->slot == slot) {
          tn3270serverIdb[i] = hwidb;   /* ensure new idb is used */
          return FALSE;            /* slot already defined */
      }
   }
   for (i = 0; i < MAX_SLOTS; i++) {
      if (tn3270serverIdb[i] == NULL) {
          tn3270serverIdb[i] = hwidb;
          return TRUE;             /* put slot in */
      }
   }
   return FALSE;      /* 12 cip card slot? impossible */
}

static void InitTn3270Idb (void)
{
   int i;
   for (i = 0; i < MAX_SLOTS; i++) {
      tn3270serverIdb[i] = NULL;
   }
}

/*
 * Get the hardware idb from the entity. The entity must be
 * in the format of tn3270s_entity_name<n>, where n is the
 * slot number. 
 */

hwidbtype* GetCipIdbFromCommunity (ContextInfo *contextInfo)
{
#define tn3270s_entity_name_len  (sizeof(tn3270s_entity_name) - 1)

   const char* entity;
   entity = snmp_extract_logical_entity(contextInfo);
   if (memcmp(entity, tn3270s_entity_name, tn3270s_entity_name_len) == 0){
      uint slot;
      int  i;
      hwidbtype* hwidb;
      boolean okflag;
      entity += tn3270s_entity_name_len;
      slot = parse_decimal_long((char*)entity, &okflag);
      if (!okflag) {
          return NULL;
      }
      for (i = 0; i < MAX_SLOTS; i++) {
          if (((hwidb = tn3270serverIdb[i]) != NULL) && hwidb->slot == slot) {
              return hwidb;
	  }
      }
   }
   return NULL;

}

/* Get cip idb from slot */
hwidbtype* GetCipIdbFromSlot (int searchType, uint slot)
{
   int i;
   hwidbtype* idb;
   hwidbtype* bestIdb = NULL;
   for (i = 0; i < MAX_SLOTS; i++) {
      idb = tn3270serverIdb[i];
      if (idb != NULL) {
         if (idb->type == IDBTYPE_CIP && is_cip_virtual(idb) && 
            (idb->status & IDB_DELETED) == 0 && slot <= idb->slot) {
            if (slot == idb->slot){
                return idb;
	 }
            if (searchType == NEXT) {
               if (bestIdb == NULL || bestIdb->slot > idb->slot) {
                   bestIdb = idb;
	        }
	    }
	}
      }
   }
   return bestIdb;
}

/*
 */
boolean get_tn3270s (boolean exact, hwidbtype* hwidb, sysmgt_snatn3270s_table_t type, void* data)
{
    boolean success;
    
   const Tn3270sLen* len = &tn3270sLen[type];
   netmgt_header_t nm_hdr;
   nm_hdr.class           = SYSMGT_TN3270S;
   nm_hdr.type            = (ushort)type;
   nm_hdr.index_length    = len->index_length;
   nm_hdr.data_length     = len->data_length;
   nm_hdr.slot            = hwidb->slot;
   nm_hdr.flags.operation = (exact) ?  NETMGT_GET_MATCH : NETMGT_GET_NEXT;

    success = sysmgt_rpc(hwidb, &nm_hdr, data) != NULL && 
	nm_hdr.flags.ret_code == NETMGT_FOUND;

    if (channel_sysmgt_debug) {
	buginf("\n%s %s %d ret_code = %d",
	       netmgt_operations[nm_hdr.flags.operation],
	       netmgt_tn3270s_table[type], nm_hdr.slot,
	       nm_hdr.flags.ret_code);
	tn3270s_dump_buf(NULL, data,
			 len->data_length);
    }
    return success;
}


/* dummy union routine, so we can get maximum size */
typedef union {
    Tn3270sGblEntry_t gbl;
    Tn3270sPuEntry_t  pu;
    Tn3270sLuEntry_t  lu;
    SnaPu20StatsEntry_t puStats;
    SnaNodeOperEntry_t  node;
    SnaLuAdminEntry_t   luAdmin;
    SnaLuOperEntry_t  luOper;
    SnaLuSessnEntry_t sess;
    SnaLuSessnStatsEntry_t sessStats;
} CommonData;

static void* GetCommonData (boolean exact, hwidbtype* hwidb, sysmgt_snatn3270s_table_t type, 
                 ulong index1,
                 ulong index2)
{
    static CommonData data;
    data.lu.tn3270sLuPuIndex = index1;
    data.lu.tn3270sLuIndex = index2;
    if (get_tn3270s(exact, hwidb, type, &data)) {
       return (void*) &data;
   }
    return NULL;
}
                  


/* Get log data */
Tn3270sLogEntry_t*
get_tn3270_log (boolean exact, hwidbtype* hwidb, long newest_index)
{
   static Tn3270sLogEntry_t logEntry;
   netmgt_header_t nm_hdr;
   boolean         success;
   logEntry.tn3270sLogIndexNewest = newest_index;

   nm_hdr.class           = SYSMGT_TN3270S;
   nm_hdr.type            = SYSMGT_TN3270S_LOG;
   nm_hdr.index_length    = 8;
   nm_hdr.data_length     = sizeof(Tn3270sLogEntry_t);
   nm_hdr.slot            = hwidb->slot;
   nm_hdr.flags.operation = (exact) ?  NETMGT_GET_MATCH : NETMGT_GET_NEXT;

   success = (sysmgt_rpc(hwidb, &nm_hdr, &logEntry) != NULL)
     && (nm_hdr.flags.ret_code == NETMGT_FOUND);
     
   if (channel_sysmgt_debug) {
     buginf("\n%s %s %d ret_code = %d",
            netmgt_operations[nm_hdr.flags.operation], "SYSMGT_TN3270S_LOG",
            nm_hdr.slot, nm_hdr.flags.ret_code);
     if (success) {
       buginf("\nnewest-index=%x", newest_index);
       tn3270s_dump_buf(NULL, &logEntry, sizeof(Tn3270sLogEntry_t));
     }
   }
   return success ? &logEntry : NULL;
}

/* Get log data */
Tn3270sLogDefEntry_t*
get_tn3270_log_def (boolean exact, hwidbtype* hwidb, long logdef_index)
{
   static Tn3270sLogDefEntry_t  log_def_entry;
   boolean                      success;
   netmgt_header_t              nm_hdr;

   log_def_entry.tn3270sLogDefEntryEventIndex = logdef_index;
   log_def_entry.tn3270sLogDefEntryChannelSlot = hwidb->slot;

   nm_hdr.class           = SYSMGT_TN3270S;
   nm_hdr.type            = SYSMGT_TN3270S_LOG_DEF;
   nm_hdr.index_length    = 8;
   nm_hdr.data_length     = sizeof(Tn3270sLogDefEntry_t);
   nm_hdr.slot            = hwidb->slot;
   nm_hdr.flags.operation = (exact) ?  NETMGT_GET_MATCH : NETMGT_GET_NEXT;

   success = (sysmgt_rpc(hwidb, &nm_hdr, &log_def_entry) != NULL)
     && (nm_hdr.flags.ret_code == NETMGT_FOUND);
     
   if (channel_sysmgt_debug) {
     buginf("\n%s %s %d ret_code = %d",
            netmgt_operations[nm_hdr.flags.operation], "log def table",
            nm_hdr.slot, nm_hdr.flags.ret_code);
     tn3270s_dump_buf(NULL, &log_def_entry, sizeof(Tn3270sLogDefEntry_t));
   }
   return success ? &log_def_entry : NULL;
}

/*
 */


boolean get_appn (boolean exact, hwidbtype* hwidb, sysmgt_appn_table_t type, void* data)
{
    boolean success;
    
   const Tn3270sLen* len = &appnLen[type];
   netmgt_header_t nm_hdr;
   nm_hdr.class           = SYSMGT_APPN;
   nm_hdr.type            = (ushort)type;
   nm_hdr.index_length    = len->index_length;
   nm_hdr.data_length     = len->data_length;
   nm_hdr.slot            = hwidb->slot;
   nm_hdr.flags.operation = (exact) ?  NETMGT_GET_MATCH : NETMGT_GET_NEXT;

   success = sysmgt_rpc(hwidb, &nm_hdr, data) != NULL && 
           nm_hdr.flags.ret_code == NETMGT_FOUND;

   if (channel_sysmgt_debug) {
            buginf("\n%s %s %d ret_code = %d",
                   netmgt_operations[nm_hdr.flags.operation],
                   netmgt_appn_table[type], nm_hdr.slot,
		   nm_hdr.flags.ret_code);
	tn3270s_dump_buf(NULL, data,
			 len->data_length);

   }
   return success;
}
/*
 */


boolean get_dlur (boolean exact, hwidbtype* hwidb, sysmgt_dlur_table_t type, void* data)
{
    boolean success;
    
   const Tn3270sLen* len = &dlurLen[type];
   netmgt_header_t nm_hdr;
   nm_hdr.class           = SYSMGT_DLUR;
   nm_hdr.type            = (ushort)type;
   nm_hdr.index_length    = len->index_length;
   nm_hdr.data_length     = len->data_length;
   nm_hdr.slot            = hwidb->slot;
   nm_hdr.flags.operation = (exact) ?  NETMGT_GET_MATCH : NETMGT_GET_NEXT;

   success = sysmgt_rpc(hwidb, &nm_hdr, data) != NULL && 
           nm_hdr.flags.ret_code == NETMGT_FOUND;

   if (channel_sysmgt_debug) {
            buginf("\n%s %s %d ret_code = %d",
                   netmgt_operations[nm_hdr.flags.operation],
                   netmgt_dlur_table[type], nm_hdr.slot,
		   nm_hdr.flags.ret_code);
	tn3270s_dump_buf(NULL, data,
			 len->data_length);
   }
   return success;
}
/*
 * CIP snmpidb test function
 */
static boolean cip_hwidb_test ( hwidbtype *hwidb, long* user_data) {
    return (hwidb->type == IDBTYPE_CIP && 
            (hwidb->firstsw && cip_interface_is_virtual(hwidb)));
}
hwidbtype*
cip_get_virtual_hwidb(int searchType, long ifIndex)
{
   return reg_invoke_ifmib_hwidb_get(MAXINT, searchType, ifIndex,
				     cip_hwidb_test, 0);
}

hwidbtype*
UpdatePuConfig (int exact, sysmgt_snatn3270s_table_t type, 
                  LuControlCommon* puEntry)
{
   hwidbtype* idb;
   tn3270s_pu_t* pPu;
   ulong index = puEntry->puIndex;

   idb = tn3270s_hwidb_from_pu_index(index, exact == EXACT,&pPu);
   if (idb != NULL){
      if (IsPuShut(pPu, &index)){
          if (!UpdateLocalPuConfig(pPu, type, puEntry)) {
              return NULL;     /* no local configuration */
          }  
      } else{
          puEntry->puIndex = index;
          get_tn3270s(EXACT, idb, type, puEntry);
      }
   }
   return idb;
}

boolean UpdatePuCardConfig (int exact, hwidbtype* hwidb, LuControlCommon* puEntry)
{
   tn3270s_pu_t* pPu;
   ulong index = puEntry->puIndex;

   pPu = tn3270s_pu_from_index(hwidb, exact == EXACT, &index);
   if (pPu) {
      if (IsPuShut(pPu, &index)){
          if (!UpdateLocalPuConfig(pPu, SYSMGT_TN3270S_PU, puEntry)) {
              return FALSE;     /* no local configuration */
          }  
      } else{
          puEntry->puIndex = index;
          get_tn3270s(EXACT, hwidb, SYSMGT_TN3270S_PU, puEntry);
      }
      return TRUE;
      
   }
   return FALSE;
}

/* This subroutine gets and updates the control block */

hwidbtype* GetLuControlBlock (int exact, sysmgt_snatn3270s_table_t type, 
                  LuControlCommon* luEntryData)    /* The first index must be PU */
{
   hwidbtype* idb;
   tn3270s_pu_t     *pPU;
   ulong index = luEntryData->puIndex;

   while ((idb = tn3270s_hwidb_from_pu_index(index, exact == EXACT,&pPU)) != NULL){
      /* a cip hwidb is found, go and get the control block */
      if (IsPuShut(pPU, &index)){
          index++;
          continue;
      }
      if (luEntryData->puIndex != index) {
          luEntryData->luIndex = 0;
      }
      luEntryData->puIndex = index;
      if (get_tn3270s(exact == EXACT, idb, type, luEntryData)){
          return idb;;
      }
      else {
         if (exact == EXACT)
            return NULL;
         else {
            index++;       /* search next puIndex */
            luEntryData->luIndex = 0;
         }
      }
   }
   return NULL;
}

/*
** - TN3270 Alert Classes: these distinguish between different
** alert-generation functions which can get called to field an alert.
** */

enum Tn3270_AlertClass {
  ACLASS_TN3270S
};

/*
** - TN3270 event profile: pulls together information about a TN3270
** event.  Does it generate an SNMP trap? what SNA alert code (or -1)
** should be used? what "class" of alert is going to be generated
** (tn3270,dlc,appn,etc)?
** */

struct tn3270_event_profile {
  char*                 eventDescriptor;
  unsigned short        doSnmpTrapP : 1; /* predicate: do SNMP trap?     */
  short                 alertCode; /* alert code to generate             */
  short                 alertClass; /* type of alert class--tn3270, etc  */
};

/*
** - Note: when adding a code to `sysmgt_tn3270s_event_code'
** enumeration list, also add the description here.  Otherwise you
** will NOT receive the event.  The INDEX of this vector must be the
** same as the sysmgt-tn3270s-event-code.
** */
static const struct tn3270_event_profile ev_profile[] = {
  /* description                snmp-trap?      alert-code      alert-class */
  { "no generic pool depleted",        FALSE,   Tn3270sAlert1,  ACLASS_TN3270S  },
  { "no memory for LU allocation",     FALSE,   Tn3270sAlert2,  ACLASS_TN3270S  },/*
  { "system up",                       FALSE,   32767,          ACLASS_TN3270S  },
  { "DLUS protocol error",             FALSE,   Tn3270sCPDL001, ACLASS_TN3270S  },
  { "Configured partner not a DLUS",   FALSE,   Tn3270sCPDL002, ACLASS_TN3270S  },
  { "DLUS across subnets",             FALSE,   Tn3270sCPDL005, ACLASS_TN3270S  },
  { "Rejecting Bind",                  FALSE,   Tn3270sCPAM001, ACLASS_TN3270S  },
  { "XID rejected",                    FALSE,   Tn3270sCPCS004, ACLASS_TN3270S  },
  { "Bad PIU",                         FALSE,   Tn3270sCPPL001, ACLASS_TN3270S  }*/
};


static  void tn3270s_bring_logdefs_uptodate(hwidbtype* idb)
{
  /*
  ** - note: when the router ends up allowing different CIPs to run
  ** different versions of the microcode, then we'll need to change
  ** this algorithm.  This is because we only asking the CIP from
  ** which this event came about fields.  If different CIP microcodes
  ** are running simultaneously, then we'll need to keep info
  ** associated with each IDB.
  ** */
  static init_done = 0;

  if (!init_done) {
    /*
    ** go thru all the log definitions, interpreting the entry
    ** definitions as we go.  The get-next operation works well for this. 
    ** */
    int logdef_index, sanity_lmt = 1000;
    for (logdef_index = 0; sanity_lmt; --sanity_lmt, ++logdef_index) {
      const Tn3270sLogDefEntry_t* logdef = get_tn3270_log_def(FALSE, idb, logdef_index);
      if (!logdef)
        break;
      logdef_index = logdef->tn3270sLogDefEntryEventIndex;
      if (InitScatBindingsTn(logdef_index, logdef->tn3270sLogDefEntryFldString,
                             logdef->tn3270sLogDefEntryFldSize)) {
        if (channel_sysmgt_debug)
          buginf("\nTN3270S: bad get of scat-bindings for logdef_index %d\n", logdef_index);
      }
      else if (channel_sysmgt_debug)
        buginf("\nTN3270S: good get of scat-bindings for logdef_index %d\n", logdef_index);
    }
    if (sanity_lmt)
      ++init_done;
    else if (channel_sysmgt_debug)
      buginf("TN3270S: looped insanely trying to  get log-bindings");
  }
}


static boolean setup_tn3270s_event(tn3270s_data* tn3270s_info,
                                   const sysmgt_tn3270s_gen_event* cip_event_blk,
                                   hwidbtype *hwidb)
{

  tn3270s_bring_logdefs_uptodate(hwidb);

  tn3270s_info->idb = hwidb;
  tn3270s_info->alertVector = cip_event_blk->eventFields;
  tn3270s_info->alertLength = cip_event_blk->eventLength;
  return TRUE;
}

static void cip_tn3270_netmgt_event_handler (ushort slot, ushort code,
                                      ushort length, void *ev_data_from_cip)
{
  hwidbtype *hwidb;
  int i;

  for (i = 0; i < MAX_SLOTS; i++) {
     if ((hwidb = tn3270serverIdb[i]) == NULL){
         continue;
     }
     if ((hwidb->type == IDBTYPE_CIP) && 
        (hwidb->firstsw && cip_interface_is_virtual(hwidb)) &&
        (slot == hwidb->slot)) {
      break;
    }
  }
  
  if (hwidb == NULL) {
    if (channel_sysmgt_debug) {
      buginf("SYSMGT TN3270S: no CIP in slot %d", slot);
    }
    return;
  }


  if (code >= sizeof(ev_profile)/sizeof(ev_profile[0])) {
    if (channel_sysmgt_debug) {
      buginf("SYSMGT TN3270S: unknown event code on CIP%d: %d", hwidb->slot, code);
    }
    return;
  }
  /*
  ** - the event profile gives the specifics of each event.  Here, we
  ** use the 'alertType' part of the event profile to choose what
  ** alert-function-registry to use to generate an alert (plus, we use
  ** it as a hint as to what info we need to gather to support that
  ** alert): */
  {
    const struct tn3270_event_profile* code_ev_profile = ev_profile + code;
    tn3270s_data tn3270s_info;
    if (channel_event_sna_debug)
      buginf("SYSMGT TN3270S: code = %d, len=%d\n", code, length);
    if (code_ev_profile->alertCode >= 0) {
      switch (code_ev_profile->alertClass) {
      case ACLASS_TN3270S:
        if (!setup_tn3270s_event(&tn3270s_info, (sysmgt_tn3270s_gen_event*) ev_data_from_cip, hwidb))
          return;
        reg_invoke_Tn3270s_Alert(code_ev_profile->alertCode, code_ev_profile->alertCode, &tn3270s_info);
        break;
      }
    }

    /*
    ** - issue an SNMP trap if our profile for the event code says we
    ** should: */
    if (code_ev_profile->doSnmpTrapP)
      reg_invoke_cip_tn3270s_trap(code, hwidb->slot, &tn3270s_info);

  }
}

/*
 * ----------------------------------------------------------------------------
 * Show commands
 * ----------------------------------------------------------------------------
 */
/* DLUR Link NodeLsDlcType 
 * Reference: IBM-6611-APPN-MIB.my
 */
#define TN3270S_NODE_LS_DLC_TYPE_DLS  3
#define TN3270S_NODE_LS_DLC_TYPE_TR   6

/* DLUR Link NodeLsDynamic
 * Reference: IBM-6611-APPN-MIB.my
 */
#define TN3270S_NODE_LS_DYNAMIC_YES  1
#define TN3270S_NODE_LS_DYNAMIC_NO   2

static char *const tn3270s_query_string[] = {
    "TN3270S_CIP_QUERY_SVR",
    "TN3270S_CIP_QUERY_PU",
    "TN3270S_CIP_QUERY_LU",
    "TN3270S_CIP_QUERY_CLIENT_LU",
    "TN3270S_CIP_QUERY_DLUR",
    "TN3270S_CIP_QUERY_DLUR_PU",
    "TN3270S_CIP_QUERY_DLUR_SAP",
    "TN3270S_CIP_QUERY_DLUR_LINK",
    "TN3270S_CIP_QUERY_HUH"
};

/*
 * ----------------------------------------------------------------------------
 * o The siftdown for IdleTime and KeepAlive are returned in units of 
 *   seconds, rather than minutes... 
 * ----------------------------------------------------------------------------
 */

static void
tn3270s_copy_siftdown (show_siftdown_t *pSift,
		       long             tn3270sTcpPort,
		       long             tn3270sIdleTime,
		       long             tn3270sKeepAlive,
		       long             tn3270sUnbindAction,
		       long             tn3270sGenericPool)
{
    pSift->tcp_port.val = tn3270sTcpPort;
    pSift->idle_time.val = tn3270sIdleTime ;
    pSift->keepalive.val = tn3270sKeepAlive;
    pSift->unbind_act.val = tn3270sUnbindAction;
    pSift->generic_pool.val = tn3270sGenericPool;
}	

/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 *   lu_rev_dns_fqdn
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_svr (hwidbtype          *hwidb,
		       boolean             exact,
		       boolean             detail,
		       void               *pData)
{
    tn3270s_show_svr_t      *pShowSvr;
    Tn3270sGblEntry_t    *pGlobalEntry;
    boolean                  success;
    
    success = FALSE;
    pShowSvr = pData;
    pGlobalEntry = (Tn3270sGblEntry_t*) GetCommonData (TN3270S_FIND_NEXT, hwidb,
			  SYSMGT_TN3270S_GLOBAL, 0, 0);
    
    if (pGlobalEntry) {
	pShowSvr->max_lu = pGlobalEntry->tn3270sMaxLus;
        pShowSvr->max_lu_used = pGlobalEntry->tn3270sLusInUse;
        pShowSvr->timingMarkSupported = pGlobalEntry->tn3270sTimingMarkSupported;
	tn3270s_copy_siftdown(&pShowSvr->siftdown,
			      pGlobalEntry->tn3270sGblTcpPort,
			      pGlobalEntry->tn3270sGblIdleTimeout,
			      pGlobalEntry->tn3270sGblKeepAlive,
			      pGlobalEntry->tn3270sGblUnbindAction,
			      pGlobalEntry->tn3270sGblGenericPool);
	sstrncpy(pShowSvr->lu_dns_fqdn,
		 pGlobalEntry->tn3270sLuDomainStem, MAX_FQDN_SIZE+1);
	/* Unretrievable             */
	/* pShowSvr->lu_rev_dns_fqdn */
        return TRUE;
    }
    return FALSE;
}

/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 *   pShowPU->lu_seed
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_pu (hwidbtype          *hwidb,
		      boolean             exact,
		      boolean             detail,
		      void               *pData)
{
    tn3270s_show_pu_t       *pShowPU;
    Tn3270sPuEntry_t        *pPuEntry;
    SnaPu20StatsEntry_t     *pStatsEntry;
    boolean                  success;
    
    pShowPU = pData;
    pPuEntry = GetCommonData(exact, hwidb,
			  SYSMGT_TN3270S_PU, pShowPU->index, 0);
    if (pPuEntry) {
	pShowPU->index = pPuEntry->tn3270sPuIndex;
	tn3270s_copy_siftdown(&pShowPU->siftdown,
			      pPuEntry->tn3270sPuTcpPort,
			      pPuEntry->tn3270sPuIdleTimeout,
			      pPuEntry->tn3270sPuKeepAlive,
			      pPuEntry->tn3270sPuUnbindAction,
			      pPuEntry->tn3270sPuGenericPool);
	pShowPU->ipaddr = pPuEntry->tn3270sPuIpAddr;
	pShowPU->lsap = pPuEntry->tn3270sLocalSapAddress;
	ieee_copy(pPuEntry->tn3270sRemoteMacAddress, pShowPU->rmac);
	pShowPU->rsap = pPuEntry->tn3270sRemoteSapAddress;
	pShowPU->status = pPuEntry->tn3270sPuState;
        pShowPU->xid = pPuEntry->xid;
	/* Unretrievables */
	/* pShowPU->lu_seed */
	
	success = FALSE;
	pStatsEntry = GetCommonData(TN3270S_FIND_MATCH, hwidb,
			      SYSMGT_TN3270S_PU_STAT, pShowPU->index, 0);

	if (pStatsEntry) {
            Tn3270sExtra_t* pExtra;

            pShowPU->sentBytes.w[1] = pStatsEntry->snaPu20StatsSentBytes;
            pShowPU->receivedBytes.w[1] = pStatsEntry->snaPu20StatsReceivedBytes;
            pShowPU->sentPius = pStatsEntry->snaPu20StatsSentPius;
            pShowPU->receivedPius = pStatsEntry->snaPu20StatsReceivedPius;
            pShowPU->sentNegativeResps = pStatsEntry->snaPu20StatsSentNegativeResps;
            pShowPU->receivedNegativeResps = pStatsEntry->snaPu20StatsReceivedNegativeResps;
            pShowPU->actLus = pStatsEntry->snaPu20StatsActLus;
            pShowPU->dactLus = pStatsEntry->snaPu20StatsInActLus;
            pShowPU->bindLus = pStatsEntry->snaPu20StatsBindLus;

	    pExtra = GetCommonData(TN3270S_FIND_MATCH, hwidb,
			      SYSMGT_TN3270S_EXTRA, pShowPU->index, 0);
            if (pExtra) {     /* CIP supports this version ? */
	       /* Since NAU MIB only uses 32 bits statistics, this can be 
                * easily overflow. We can't change the nau mib since it is
                * not CISCO mib. So we sent the top 32 bits via new codepoint. 
                * The two 32 bits are concatenate to give 64 bits.
                */

               pShowPU->receivedBytes.w[0] = pExtra->puReceivedBytesHi; 
               pShowPU->sentBytes.w[0] = pExtra->puSentBytesHi;
	    }
            else {
               pShowPU->receivedBytes.w[0] = 0;   /* version mismatched */
               pShowPU->sentBytes.w[0] = 0;
	    }
            return TRUE;
	}
    }
    return FALSE;
}
/*
 * ----------------------------------------------------------------------------
 * o Query statatistics 
 * ----------------------------------------------------------------------------
 */
boolean
tn3270s_cip_query_stats (hwidbtype          *hwidb, 
		      tn3270s_show_stat_t*  pShowStat)
{
    Tn3270sStatsEntry_t*     statsEntry;

    statsEntry = GetCommonData(TN3270S_FIND_NEXT, hwidb,
			  SYSMGT_TN3270S_STATS, pShowStat->ipAddr, pShowStat->tcpPort);
    if (statsEntry) {
        pShowStat->ipAddr = statsEntry->tn3270sStatsServerAddr;
        pShowStat->tcpPort = statsEntry->tn3270sStatsServerTcpPort ;
        pShowStat->maxSess = statsEntry->tn3270sStatsMaxSess ;
        pShowStat->spareSess = statsEntry->tn3270sStatsSpareSess ;
        pShowStat->connectsIn = statsEntry->tn3270sStatsConnectsIn ;
        pShowStat->disconnects = statsEntry->tn3270sStatsDisconnects ;
        pShowStat->connectsFailed = statsEntry->tn3270sStatsTN3270ConnectsFailed ;
        pShowStat->iTransactions = statsEntry->tn3270sStatsInboundTransactions ;
        pShowStat->oTransactions = statsEntry->tn3270sStatsOutboundTransactions ;
        pShowStat->siTransactions = statsEntry->tn3270sStatsSampledInboundTransactions ;
        pShowStat->siTransactionsTime = statsEntry->tn3270sStatsNetSampledInboundTransactionRespTime ;
        pShowStat->soTransactions = statsEntry->tn3270sStatsSampledOutboundTransactions ;
        pShowStat->soTransactionsTime = statsEntry->tn3270sStatsNetSampledOutboundTransactionRespTime ;
        return TRUE;
    }
    return FALSE;
}



/*
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_dlur_pu (hwidbtype          *hwidb,
			   boolean             exact,
			   boolean             detail,
			   void               *pData)
{
    tn3270s_show_pu_t       *pShowPU;
    dlurPuEntry_T           *pDlurPuEntry;
    boolean                  success;
    
    success = FALSE;
    pShowPU = pData;
    
    /* First query the standard PU fields */
    success = tn3270s_cip_query_pu(hwidb, exact,
				   detail, pData);
    if (success) {
	success = FALSE;
	
	pDlurPuEntry = malloc(sizeof(*pDlurPuEntry));
	if (!pDlurPuEntry) {
	    return success;
	}
	pDlurPuEntry->dlurPuIndex = pShowPU->index;
	success = get_dlur(exact, hwidb,
			   SYSMGT_DLUR_PU_ENTRY, pDlurPuEntry);
	if (success) {
	    pShowPU->dlurStatus = pDlurPuEntry->dlurPuStatus;
	    sstrncpy(pShowPU->name_at_host, pDlurPuEntry->dlurPuName,
		     MAX_FQNN_SIZE+1);
	}
	else {
	    pShowPU->name_at_host[0] = 0;
	}
	free(pDlurPuEntry);
    }
    return success;
}

/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 * o These missing fields are configured values
 *   and are initialized prior to calling this routine:
 *
 *     pShowDLUR-> dlur_fq_cpname[MAX_FQNN_SIZE+1];
 *     pShowDLUR-> dlur_fq_dlusname[MAX_FQNN_SIZE+1];
 *
 *     pShowDLUR-> dlur_dlus_backup_name[MAX_FQNN_SIZE+1];
 *     pShowDLUR-> dlur_nn_svr[MAX_FQNN_SIZE+1];
 *
 * o Now retrieving this via SYSMGT_appnDirEntryData
 *   but the entry returns NOT_FOUND:
 *
 *   pShowDLUR-> current_nn_svr[MAX_FQNN_SIZE+1];
 *
 * o No sysmgt field corresponding to this:
 *     pShowDLUR->cp_cp_status;
 *
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_dlur (hwidbtype          *hwidb,
			boolean             exact,
			boolean             detail,
			void               *pData)
{
    tn3270s_show_dlur_t     *pShowDLUR;
    dlurDlusEntry_T         *pDlurDlus;
    boolean                  success;
    
    success = FALSE;
    pShowDLUR = pData;

    pDlurDlus = malloc(sizeof(*pDlurDlus));
    
    if (!pDlurDlus) {
	return success;
    }
    pDlurDlus->dlurDlusIndex = pShowDLUR->index;
    success = get_dlur(TN3270S_FIND_MATCH, hwidb,
		       SYSMGT_DLUR_DLUS_ENTRY, pDlurDlus);
    if (success) {
	pShowDLUR->dlur_dlus_status = pDlurDlus->dlurDlusSessnStatus;
	sstrncpy(pShowDLUR->current_dlusname,
		 pDlurDlus->dlurDlusName, MAX_FQNN_SIZE+1);
    }
    free(pDlurDlus);
    
    if (detail) {

	/* Unretrievables */
	sstrncpy(pShowDLUR->current_nn_svr,
	         TN3270S_STATUS_STR_NOTKNOWN, MAX_FQNN_SIZE+1);

        pShowDLUR->cp_cp_status = TN3270S_STATUS_NOTKNOWN;
    }

   return success;
}


/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 *
 *     pShowLU->assoc_pu_name[MAX_NAME_SIZE+1];
 *     pShowLU->assoc_locaddr;
 *
 * Note:
 *     pLu->tn3270sLuTermModel (18 char field) is truncated to
 *     pShowLU->display_mod_name (8 char field)
 * 
 * If this is a TN3270S_FIND_NEXT operation, 
 * return the next lu for this pu.  Do not increment
 * beyond the pu_index
 *
 * ----------------------------------------------------------------------------
 */ 
static boolean
tn3270s_cip_query_lu (hwidbtype          *hwidb,
		      boolean             exact,
		      boolean             detail,
		      void               *pData)
{
    tn3270s_show_lu_t   *pShowLU;
    Tn3270sLuEntry_t    *pLu;
    SnaLuOperEntry_t    *pSnaLuOper;
    SnaLuSessnStatsEntry_t *pStat;
    SnaLuSessnEntry_t   *pSess;
    boolean             success;

    success = FALSE;
    
    pShowLU = pData;
    pLu = GetCommonData(exact, hwidb,
			  SYSMGT_TN3270S_LU, pShowLU->pu_index, pShowLU->lu_index);

    if (pLu) {    
	/*
	 * Verify the FIND_NEXT didn't increment beyond 
	 * this PU
	 */
        pShowLU->display_mod_name[0] = 0;
        pShowLU->luEvents[0] = 0;
        pShowLU->trace[0][0] = 0;
        pShowLU->trace[0][1] = 0;
        pShowLU->appnLinkName[0] = 0;
        pShowLU->lfsid = 0;
	if (pLu->tn3270sLuPuIndex == pShowLU->pu_index) {
	    
	    pShowLU->lu_index = pLu->tn3270sLuIndex;
	    pShowLU->client_port = pLu->tn3270sLuClientTcpPort;
	    pShowLU->client_ipaddr = pLu->tn3270sLuClientAddr;
            pShowLU->lu_state = pLu->tn3270sLuState ;
            pShowLU->telnetType = pLu->tn3270sLuTelnetType ;
            memcpy(pShowLU->appnLinkName, pLu->tn3270sLuAppnLinkName, 
                         sizeof(pLu->tn3270sLuAppnLinkName));
            pShowLU->appnLinkName[8] = 0;         /* make sure null termination */
            pShowLU->lfsid = pLu->tn3270sLuLfsid ;
            pShowLU->curInbPacing = pLu->tn3270sLuCurInbPacing ;
            pShowLU->curInbQsize = pLu->tn3270sLuCurInbQsize ;
            pShowLU->curOutQsize = pLu->tn3270sLuCurOutQsize ;
            pShowLU->dynamicType = pLu->tn3270sLuType ;
            pShowLU->luType = pLu->lu_type ;
            pShowLU->idleTime = pLu->tn3270sLuIdleTime ;


	    sstrncpy(pShowLU->display_mod_name,
		     pLu->tn3270sLuTermModel, MAX_DISPLAYMOD_SIZE+1);
	    
            memcpy(pShowLU->luEvents, pLu->tn3270sLuEvents, MaxEvents);
            memcpy(pShowLU->trace, pLu->trace, NumberOfTraceBuffer*TraceBufferLength);
	    
            pSess = GetCommonData(TN3270S_FIND_MATCH, hwidb,
			  SYSMGT_TN3270S_LU_SESSN, pShowLU->pu_index, pShowLU->lu_index);
            if (pSess) {
               pShowLU->maxSndRuSize = pSess->snaLuSessnMaxSndRuSize ;
               pShowLU->maxRcvRuSize = pSess->snaLuSessnMaxRcvRuSize ;
               pShowLU->sndPacingSize = pSess->snaLuSessnSndPacingSize ;
               pShowLU->rcvPacingSize = pSess->snaLuSessnRcvPacingSize ;
	    }

            pStat = GetCommonData(TN3270S_FIND_MATCH, hwidb,
			  SYSMGT_TN3270S_LU_SESSN_STAT, pShowLU->pu_index, pShowLU->lu_index);
            if (pStat) {
               pShowLU->sentBytes = pStat->snaLuSessnStatsSentBytes ;
               pShowLU->receivedBytes = pStat->snaLuSessnStatsReceivedBytes ;
               pShowLU->sentRus = pStat->snaLuSessnStatsSentRus ;
               pShowLU->receivedRus = pStat->snaLuSessnStatsReceivedRus ;
               pShowLU->sentNegativeResps = pStat->snaLuSessnStatsSentNegativeResps ;
               pShowLU->receivedNegativeResps = pStat->snaLuSessnStatsReceivedNegativeResps ;
	   }
                
	    pSnaLuOper = GetCommonData(TN3270S_FIND_MATCH, hwidb,
				  SYSMGT_TN3270S_LU_OPER, pShowLU->pu_index, pShowLU->lu_index);
	    if (pSnaLuOper) {
		sstrncpy(pShowLU->lu_name,
			 pSnaLuOper->snaLuOperSnaName, MAX_FQNN_SIZE+1);
	    }
	    else {
	       pShowLU->lu_name[0] = 0;
	    }
            return TRUE;
	}
    }
    return FALSE;
}

/* 
 * ----------------------------------------------------------------------------
 * By iterating through all the TcpPorts at the
 * specified IpAddress, all the LUs will be returned.
 *
 * Note: the FIND_NEXT will not increment the ipaddress
 *       This is a find all TcpPort on a specific ipaddress
 *
 *   the PuAddr and LuAddr fields are actually
 *       pu_index and lu_index
 *
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_client_lu (hwidbtype          *hwidb,
			     boolean             exact,
			     boolean             detail,
			     void               *pData)
{
    tn3270s_show_lu_t   *pShowLU; 
    Tn3270sIpEntry_t    *pIpEntry;
    boolean              success = FALSE;
    
    pShowLU = pData;
    
    pIpEntry = GetCommonData(exact, hwidb,
			  SYSMGT_TN3270S_IP, pShowLU->client_ipaddr, pShowLU->client_port);
    if (pIpEntry) {
	/* Assure the element returned is still for this ipaddr */
        success = TRUE;
	if (pIpEntry->tn3270sIpClientAddr == pShowLU->client_ipaddr) {
	    pShowLU->client_port = pIpEntry->tn3270sIpClientTcpPort;
	    pShowLU->pu_index = pIpEntry->tn3270sIpPuIndex;
	    pShowLU->lu_index = pIpEntry->tn3270sIpLuIndex;
	    /* Go retrieve the lu goodies */
	    success = tn3270s_cip_query_lu(hwidb, TN3270S_FIND_MATCH,
					   detail, pShowLU);
	}
    }
    return success;
}

/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 *
 *    pShowLink->sessions;
 *    pShowLink->maxin;
 *    pShowLink->maxout;
 *
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_dlur_link (hwidbtype          *hwidb,
			     boolean             exact,
			     boolean             detail,
			     void               *pData)
{
    char                    *p;
    tn3270s_show_link_t     *pShowLink;
    ibmappnNodeLsEntry_T    *pNodeLs;
    ibmappnNodeLsDlsEntry_T *pNodeLsDls;
    ibmappnNodeLsTrEntry_T  *pNodeLsTr;
    boolean                  found;
    boolean                  success;
    
    success = FALSE;
    pShowLink = pData;
    pNodeLs = malloc(sizeof(*pNodeLs));
    
    if (!pNodeLs)
	return success;
    
    pNodeLsDls = malloc(sizeof(*pNodeLsDls));
    if (!pNodeLsDls) {
	free(pNodeLs);
	return success;
    }
    
    pNodeLsTr = malloc(sizeof(*pNodeLsTr));
    if (!pNodeLsTr) {
	free(pNodeLs);
	free(pNodeLsDls);
	return success;
    }
    
    found = FALSE;
    success = TRUE;
    while (success && !found) {
	if (exact == TN3270S_FIND_NEXT) {
	    if (strlen(pShowLink->name)) {
		/* Increment name for next search */
		p = &pShowLink->name[strlen(pShowLink->name)-1];
		*p = *p + 1;
	    }
	}
	/* Name is index */
	sstrncpy(pNodeLs->ibmappnNodeLsName,
		 pShowLink->name, MAX_NAME_SIZE+1);
	success = get_appn(exact, hwidb,
			   SYSMGT_appnNodeLsEntryData, pNodeLs);
	if (success) {
	    sstrncpy(pShowLink->name, 
		     pNodeLs->ibmappnNodeLsName, MAX_NAME_SIZE+1);
	    /* 
	     * Now retrieve the rmac, rsap
	     * and if this is a search next, determine if this is a
	     * link with the requested lmac.
	     */
	    switch (pNodeLs->ibmappnNodeLsDlcType) {
	    case TN3270S_NODE_LS_DLC_TYPE_DLS:
		sstrncpy(pNodeLsDls->ibmappnNodeLsDlsName,
			 pNodeLs->ibmappnNodeLsName, MAX_NAME_SIZE+1);
		success = get_appn(TN3270S_FIND_MATCH, hwidb,
				   SYSMGT_appnNodeLsDlsEntryData,
				   pNodeLsDls);
		if (success) {
		    found = TRUE;
		    if (exact == TN3270S_FIND_NEXT) {
			if (ieee_compare(pNodeLsDls->ibmappnNodeLsLocalDlsMac,
					 pShowLink->lmac))
			    found = FALSE;
		    }
		    if (found) {
			pShowLink->status = pNodeLsDls->ibmappnNodeLsDlsState;
			pShowLink->lsap = pNodeLsDls->ibmappnNodeLsLocalDlsSap;
			pShowLink->rsap = pNodeLsDls->ibmappnNodeLsRemoteDlsSap;
			ieee_copy(pNodeLsDls->ibmappnNodeLsLocalDlsMac,
				  pShowLink->lmac);
			ieee_copy(pNodeLsDls->ibmappnNodeLsRemoteDlsMac,
				  pShowLink->rmac);
		    }
		}
		break;
	    default :
	    case TN3270S_NODE_LS_DLC_TYPE_TR:
		sstrncpy(pNodeLsTr->ibmappnNodeLsTrName,
			 pNodeLs->ibmappnNodeLsName, MAX_NAME_SIZE+1);
		success = get_appn(TN3270S_FIND_MATCH, hwidb,
				   SYSMGT_appnNodeLsTrEntryData,
				   pNodeLsTr);
		if (success) {
		    found = TRUE;
		    if (exact == TN3270S_FIND_NEXT) {
			if (ieee_compare(pNodeLsTr->ibmappnNodeLsLocalTrMac,
					 pShowLink->lmac))
			    found = FALSE;
		    }
		    if (found) {
			pShowLink->status = pNodeLsTr->ibmappnNodeLsTrState;
			pShowLink->lsap = pNodeLsTr->ibmappnNodeLsLocalTrSap;
			pShowLink->rsap = pNodeLsTr->ibmappnNodeLsRemoteTrSap;
			ieee_copy(pNodeLsTr->ibmappnNodeLsLocalTrMac,
				  pShowLink->lmac);
			ieee_copy(pNodeLsTr->ibmappnNodeLsRemoteTrMac,
				  pShowLink->rmac);
		    }
		}
		break;
	    }
	}
	
	if (success) {
	    pShowLink->status = pNodeLs->ibmappnNodeLsState; /* ??? */
	    pShowLink->dynamic = (pNodeLs->ibmappnNodeLsDynamic == 
				  TN3270S_NODE_LS_DYNAMIC_YES);
	    sstrncpy(pShowLink->partner_name,
		     pNodeLs->ibmappnNodeLsCpName, MAX_FQNN_SIZE+1);
	    pShowLink->tgn = pNodeLs->ibmappnNodeLsTgNum;
	    pShowLink->maxdata = pNodeLs->ibmappnNodeLsMaxSendBtuSize; /* ??? */

	    /* Unretrievables */
	    pShowLink->sessions = 0;
	    pShowLink->maxin = 0;
	    pShowLink->maxout = 0;
	}
    }
    free(pNodeLsDls);
    free(pNodeLsTr);
    free(pNodeLs);
    return success;
}

/*
 * ----------------------------------------------------------------------------
 * o Fields needed:
 *     pShowSAP->status
 *              (VRN also ?)
 * ----------------------------------------------------------------------------
 */
static boolean
tn3270s_cip_query_dlur_sap (hwidbtype          *hwidb,
			    boolean             exact,
			    boolean             detail,
			    void               *pData)
{
    tn3270s_show_dlur_sap_t *pShowSAP;
    ibmappnNodePortEntry_T  *pNodePort;
    boolean                  success;
    
    success = FALSE;
    pShowSAP = pData;

    pNodePort = malloc(sizeof(*pNodePort));
    
    if (!pNodePort)
	return success;
    
    /* Name is index */
    sstrncpy(pNodePort->ibmappnNodePortName, pShowSAP->name, MAX_NAME_SIZE+1);
    success = get_appn(exact, hwidb, SYSMGT_appnNodePortEntryData, pNodePort);

    if (success)
	pShowSAP->status = pNodePort->ibmappnNodePortState;

    free(pNodePort);
    return success;
}


/*
 * ----------------------------------------------------------------------------
 * Query CIP for data for show command output
 * ----------------------------------------------------------------------------
 */
boolean
tn3270s_cip_query (hwidbtype          *hwidb,
		   boolean             exact,
		   boolean             detail,
		   tn3270s_cip_query_t type,
		   void               *pData)
{
    boolean        success;
    
    if (TN3270S_DEBUG && channel_debug)
	buginf("\n%s", 
	       tn3270s_query_string[type]);

    success = FALSE;
    switch (type) {
    case TN3270S_CIP_QUERY_SVR:
	success = tn3270s_cip_query_svr(hwidb, exact,
					detail, pData);
	break;
    case TN3270S_CIP_QUERY_PU:
	success = tn3270s_cip_query_pu(hwidb, exact,
				       detail, pData);
	break;
    case TN3270S_CIP_QUERY_DLUR_PU: 
	success = tn3270s_cip_query_dlur_pu(hwidb, exact,
					    detail, pData);
	break;
    case TN3270S_CIP_QUERY_LU:
	success = tn3270s_cip_query_lu(hwidb, exact,
				       detail, pData);
	break;
    case TN3270S_CIP_QUERY_CLIENT_LU:
	success = tn3270s_cip_query_client_lu(hwidb, exact,
					      detail, pData);
	break;
    case TN3270S_CIP_QUERY_DLUR:
	success = tn3270s_cip_query_dlur(hwidb, exact,
					 detail, pData);
	break;
    case TN3270S_CIP_QUERY_DLUR_SAP:
	success = tn3270s_cip_query_dlur_sap(hwidb, exact,
					     detail, pData);
	break;
    case TN3270S_CIP_QUERY_DLUR_LINK: 
	success = tn3270s_cip_query_dlur_link(hwidb, exact,
					      detail, pData);
	break;
    case TN3270S_CIP_QUERY_MAX:
	return success;
    }   
    return success;	
}

/* Add tn3270 server entity.
 * The hardware idb is also saved here to check if the entity had
 * been added. It also saved the hwidb locally to speed up the
 * search of tn3270 server idb
 */
void add_tn3270s_entity (hwidbtype* hwidb)
{
/* must hold the prefix plus a unique value (<= 3 digits) plus /0 */
#define BUFFER_SIZE sizeof(tn3270s_entity_name)+4

   char entitydescr[80];
   char entityName[BUFFER_SIZE];
   int slot;
   if (SaveTn3270Idb(hwidb)) {
       slot = hwidb->slot;
       sprintf(entitydescr, "TN3270 server cip slot %d", slot);
       sprintf(entityName, "%s%u", tn3270s_entity_name, slot);
       reg_invoke_snmp_add_dlur_entity(entityName,entitydescr);
       reg_invoke_snmp_add_appn_entity(entityName,entitydescr);
   }
}



/*
 * ----------------------------------------------------------------------------
 */

/* TN3270s uses system-management RPC calls to the CIP for alerts
 * with RP
 */

static void init_tn3270s (subsystype *subsys)
{
  InitTn3270Idb();    /* clear tn3270 server hwidb array */
  reg_add_cip_netmgt_event(SYSMGT_TN3270S, cip_tn3270_netmgt_event_handler,
                           "cip_tn3270_netmgt_event_handler");
}

/*
 * tn3270smib subsystem header
 */

#define MAJVERSION_tn3270s 1
#define MINVERSION_tn3270s 0
#define EDITVERSION_tn3270s 0

SUBSYS_HEADER(tn3270s,
              MAJVERSION_tn3270s,
              MINVERSION_tn3270s,
              EDITVERSION_tn3270s,
              init_tn3270s,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cip_sysmgt_rpc",
              NULL);
      
