/* $Id: sr_sdllcmib.c,v 3.5.12.6 1996/08/26 15:12:56 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/sr_sdllcmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code. One line description here.
 *
 * April 1995, Bob Clouston (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_sdllcmib.c,v $
 * Revision 3.5.12.6  1996/08/26  15:12:56  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.5.12.5  1996/07/09  06:05:55  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.5.12.4  1996/05/21  10:05:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.3  1996/05/21  06:41:09  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.12.2  1996/05/17  12:14:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.4  1996/04/26  15:23:35  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.3  1996/04/26  07:58:48  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.2.2  1996/04/11  14:38:31  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/04/03  21:59:37  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.12.1  1996/03/18  22:11:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:38:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/13  02:02:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.14.1  1996/02/20  18:49:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/14  17:36:24  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/24  21:19:08  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1995/12/14  08:28:03  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  18:56:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:30:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/07  05:44:07  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.3  1995/07/17  07:34:30  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:31:34  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:02:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
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
 */



#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_sdllcmib.h"
#include "sr_sdllcmib-mib.h"
#include "interface_generic.h"
#include "../snmp/snmp_registry.h"
#include "../ibm/sdlc.h"
#include "rsrb.h"
#include "srb_core.h"
#include "lack_externs.h"
#include "lack2fsm.h"
#include "../ip/ip_registry.h"
#include "../ibm/sdlc_registry.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_trap_fe.h"
#include "../snmp/sr_ifmib.h"
#include "../if/rif_util.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"


static const char peerStateChangeOid[] = "ciscoSnaSdllcMIB.2.0.1";


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the convSdllcPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
convSdllcPortEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp=0;
    convSdllcPortEntry_t *data=0;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ifIndex;


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
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_convSdllcPortEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ifIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_convSdllcPortVirtMacAddr
    case I_convSdllcPortVirtMacAddr:
       dp = (void *) (CloneOctetString(data->convSdllcPortVirtMacAddr));
       break;
#endif /* I_convSdllcPortVirtMacAddr */

#ifdef I_convSdllcPortVirtRing
    case I_convSdllcPortVirtRing:
       dp = (void *) (&data->convSdllcPortVirtRing);
       break;
#endif /* I_convSdllcPortVirtRing */

#ifdef I_convSdllcPortBridge
    case I_convSdllcPortBridge:
       dp = (void *) (&data->convSdllcPortBridge);
       break;
#endif /* I_convSdllcPortBridge */

#ifdef I_convSdllcPortLlc2Ring
    case I_convSdllcPortLlc2Ring:
       dp = (void *) (&data->convSdllcPortLlc2Ring);
       break;
#endif /* I_convSdllcPortLlc2Ring */

#ifdef I_convSdllcPortLocalAck
    case I_convSdllcPortLocalAck:
       dp = (void *) (&data->convSdllcPortLocalAck);
       break;
#endif /* I_convSdllcPortLocalAck */

#ifdef I_convSdllcPortLocalAckState
    case I_convSdllcPortLocalAckState:
       dp = (void *) (&data->convSdllcPortLocalAckState);
       break;
#endif /* I_convSdllcPortLocalAckState */

#ifdef I_convSdllcPortMaxLlc2FrameSize
    case I_convSdllcPortMaxLlc2FrameSize:
       dp = (void *) (&data->convSdllcPortMaxLlc2FrameSize);
       break;
#endif /* I_convSdllcPortMaxLlc2FrameSize */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the convSdllcAddrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
convSdllcAddrEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp=0;
    convSdllcAddrEntry_t *data=0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            convSdllcAddrSdlcAddr;


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
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &convSdllcAddrSdlcAddr, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        convSdllcAddrSdlcAddr = MAX(0, convSdllcAddrSdlcAddr);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_convSdllcAddrEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, convSdllcAddrSdlcAddr)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->convSdllcAddrSdlcAddr;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_convSdllcAddrPartnerMacAddr
    case I_convSdllcAddrPartnerMacAddr:
       dp = (void *) (CloneOctetString(data->convSdllcAddrPartnerMacAddr));
       break;
#endif /* I_convSdllcAddrPartnerMacAddr */

#ifdef I_convSdllcAddrXID
    case I_convSdllcAddrXID:
       dp = (void *) (CloneOctetString(data->convSdllcAddrXID));
       break;
#endif /* I_convSdllcAddrXID */

#ifdef I_convSdllcAddrState
    case I_convSdllcAddrState:
       dp = (void *) (&data->convSdllcAddrState);
       break;
#endif /* I_convSdllcAddrState */

#ifdef I_convSdllcAddrMaxSdlcFrameSize
    case I_convSdllcAddrMaxSdlcFrameSize:
       dp = (void *) (&data->convSdllcAddrMaxSdlcFrameSize);
       break;
#endif /* I_convSdllcAddrMaxSdlcFrameSize */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */



/* return TRUE if there is an sdlc configured */
static boolean sdlc_tester (hwidbtype *idb, long* dummy)
{
   sdlc_data_block *sdb = NULL;
   int sdlc_address;
   if (is_sdlc(idb) && idb->sdllc_active){
       for (sdlc_address = 0; (sdb = idb->sdlc_data[sdlc_address]) == NULL && 
                                             sdlc_address++ < 255;)
            ;
   }
   return (sdb != NULL);
}


convSdllcPortEntry_t *
k_convSdllcPortEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
   static convSdllcPortEntry_t convSdllcPortEntryData;
   static OctetString macAddr;
   hwidbtype *hwidb;
   lak_t *lackie;
   long sdlc_address;
   sdlc_data_block *sdb;
   srb_triplet_t srb_triplet;

   hwidb = ifmib_hwidb_get(0, searchType, ifIndex,
			   sdlc_tester, NULL);
   if (hwidb == NULL)
      return NULL;

   /* get any sdb at this interface if any */

   for (sdlc_address = 0; (sdb = hwidb->sdlc_data[sdlc_address]) == NULL && sdlc_address++ < 255;)
            ;
   if (sdb == NULL) 
       return NULL;
     
    convSdllcPortEntryData.ifIndex = hwidb->snmp_if_index;
    convSdllcPortEntryData.convSdllcPortVirtMacAddr = &macAddr;
    macAddr.length = IEEEBYTES;
    macAddr.octet_ptr = hwidb->sdlc_vr_macaddr;
    

    srbV.srb_sb_get_triplet(firstsw_or_null(sdb->sdlc_idb), &srb_triplet);
    convSdllcPortEntryData.convSdllcPortVirtRing = srb_triplet.localRing;
    convSdllcPortEntryData.convSdllcPortBridge   = srb_triplet.bridgeNum;
    convSdllcPortEntryData.convSdllcPortLlc2Ring = srb_triplet.remoteRing;
   
    convSdllcPortEntryData.convSdllcPortLocalAck =
        sdb->sdllc_lak_enable;
    lackie = sdb2lak(hwidb, sdb);
    switch (lackie->lak2_state) {
      case l2st_disconnect:      
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_disconnected;
       break;
      case l2st_localdiscwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState =
          D_convSdllcPortLocalAckState_localDiscWait;
        break;
      case l2st_remdiscwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_remDiscWait;
       break;
      case l2st_remwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState =  
         D_convSdllcPortLocalAckState_remWait;
       break;
      case l2st_localwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState = 
         D_convSdllcPortLocalAckState_localWait;
       break;
      case l2st_connectpending:
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_connectPending;
       break;
      case l2st_connect: 
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_connected;
       break;
      case l2st_remqonwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_remQOnWait;
       break;
      case l2st_remqoffwait:
       convSdllcPortEntryData.convSdllcPortLocalAckState =
         D_convSdllcPortLocalAckState_remQOffWait;
       break;
      case l2st_quench:
       convSdllcPortEntryData.convSdllcPortLocalAckState = 
         D_convSdllcPortLocalAckState_quenched;
       break;
      case l2st_undefined: 
      default:
       convSdllcPortEntryData.convSdllcPortLocalAckState = 
         D_convSdllcPortLocalAckState_unknown;
      }
      
    convSdllcPortEntryData.convSdllcPortMaxLlc2FrameSize =
        rc_code_to_bytes(hwidb->sdllc_largest_token_frame);

   return(&convSdllcPortEntryData);
}

/* return TRUE if there is an sdlc configured */
static boolean snmp_sdlc_idb_tester (hwidbtype *idb, long* indexP)
{
   sdlc_data_block  *sdb;
   if (is_sdlc(idb) && idb->sdllc_active){
        long sdlc_address = *indexP;
        while ((sdb = idb->sdlc_data[sdlc_address]) == 0 && sdlc_address++ < 255)
            ;
       if (sdb != NULL){
          *indexP = sdlc_address;
          return TRUE;
      }
   }
   return FALSE;      /* no secondary index defined */
}

convSdllcAddrEntry_t *
k_convSdllcAddrEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, convSdllcAddrSdlcAddr)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long convSdllcAddrSdlcAddr;
{
   static convSdllcAddrEntry_t convSdllcAddrEntryData;
   static OctetString macAddr;
   static OctetString xid;
   hwidbtype *hwidb;
   sdlc_data_block  *sdb;

   hwidb = ifmib_2indices_get(serialNum, searchType, ifIndex,
			      snmp_sdlc_idb_tester,
			      &convSdllcAddrSdlcAddr);
   
    if (hwidb == NULL)
        return NULL;

    /* if hwidb is not NULL, then sdb must be tested not NULL */

    sdb = hwidb->sdlc_data[convSdllcAddrSdlcAddr];
     
    convSdllcAddrEntryData.convSdllcAddrSdlcAddr =
        sdb->sdlc_address;
        
    convSdllcAddrEntryData.convSdllcAddrPartnerMacAddr = &macAddr;
    macAddr.length = IEEEBYTES;
    macAddr.octet_ptr = sdb->sdlc_prtnr;
    
    switch (sdb->sdllc_state) { 
      case SDLLC_DISCONNECT:
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_disconnected ; 
       break;
      case SDLLC_DW_SDLC: 
       convSdllcAddrEntryData.convSdllcAddrState =
         D_convSdllcAddrState_sdlcDisconnecting;
       break;
      case SDLLC_AW_SDLC_PRI: 
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_sdlcPriConnecting;
       break;
      case SDLLC_DW_NET: 
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_netDisconnecting;
       break;
      case SDLLC_AW_NET:
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_netConnecting;
       break;
      case SDLLC_AW_SDLC_SEC:
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_sdlcSecConnecting;
       break;
      case SDLLC_CONNECT: 
       convSdllcAddrEntryData.convSdllcAddrState = 
         D_convSdllcAddrState_connected;
       break;
      default: 
       ;
    }
     convSdllcAddrEntryData.convSdllcAddrMaxSdlcFrameSize =
        sdb->sdlc_largest_frame;

    convSdllcAddrEntryData.convSdllcAddrXID = &xid;
    xid.length = SDLC_XID_SIZE; 
    xid.octet_ptr = sdb->sdlc_xid;
     convSdllcAddrEntryData.ifIndex = hwidb->snmp_if_index;
   return(&convSdllcAddrEntryData);
}



/*
* convSdllcPeerStateChangeTrap NOTIFICATION-TYPE
*      OBJECTS { convSdllcPortState }
*      STATUS      current
*      DESCRIPTION
*         "This notification indicates that the state of an SDLLC peer
*          has transitioned to active (psConnect) or
*          inactive (psDisconnect)."
*      ::= { convSdllcTraps 1 }
*/
static const OID sdllcPeerOID = {LNciscoSnaSdllcMIB, (ulong *)IDciscoSnaSdllcMIB};

#define SDLLC_TRAP_VBCOUNT  1
#define SDLLC_TRAP_NUM 1

static const OID sdllcPeerStateChange_varbinds[SDLLC_TRAP_VBCOUNT] = {
    OIDconversion(convSdllcAddrState)
};
void sdllc_trap (sdlc_data_block *sdb)
{
    OID indexOID;
    ulong index[2];
    
    indexOID.length = 2;
    indexOID.oid_ptr = index;
    index[0] = sdb->sdlc_idb->snmp_if_index;
    index[1] = sdb->sdlc_address;

    SnmpSendTrap_Generic(sdllcPeerStateChange_varbinds,
                         &sdllcPeerOID,
			 peerStateChangeOid,
                         &indexOID,
                         SDLLC_TRAP_VBCOUNT,
                         ENTERPRISE_TRAP, 
                         SDLLC_TRAP_NUM);
}

void
init_sdllcmib (subsystype *subsys)
{ 
    load_mib(sdllcmib_OidList, sdllcmib_OidListNum);
    load_oid(sdllcmib_oid_table);
    register_snmp_trap(TRAP_SDLLC,  peerStateChangeOid);
    reg_add_sdllc_trap(sdllc_trap, "sdllc_trap");
}


/*
 * sdllcmib subsystem header
 */

#define MAJVERSION_sdllcmib 1
#define MINVERSION_sdllcmib 0
#define EDITVERSION_sdllcmib 0

SUBSYS_HEADER(sdllcmib,
              MAJVERSION_sdllcmib,
              MINVERSION_sdllcmib,
              EDITVERSION_sdllcmib,
              init_sdllcmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: sdlc");
      
