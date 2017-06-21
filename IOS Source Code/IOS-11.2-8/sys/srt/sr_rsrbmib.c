/* $Id: sr_rsrbmib.c,v 3.6.12.1 1996/05/17 12:14:04 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/sr_rsrbmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code. One line description here.
 *
 * April 1995, Bob Clouston (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rsrbmib.c,v $
 * Revision 3.6.12.1  1996/05/17  12:14:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.1  1996/04/26  07:58:46  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/14  16:30:33  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/24  21:19:07  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.4  1995/12/14  08:28:02  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  18:56:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:34:28  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:31:32  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:01:58  hampton
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
#include "subsys.h"
#include "interface_generic.h"
#include "../snmp/snmp_api.h" 
#include "sr_rsrbmib.h"
#include "sr_rsrbmib-mib.h"
#include "../snmp/snmp_registry.h"
#include "srt_registry.h"
#include "../ibm/sdlc.h"
#include "rsrb.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_trap_fe.h"

static const char *peerStateChangeOid = "ciscoRsrbMIB.2.0.1";
extern ipaddrtype rsrb_fst_name;

static tr_vgrouptype* GetVrg (int searchType, long rsrbVirtRingIndex);
static tr_vpeertype* GetVrp (int searchType, long rsrbVirtRingIndex, long rsrbRemotePeerIndex);
static tr_ventrytype* GetVre (int searchType, long *rsrbVirtRingIndex, long rsrbRingIndex);

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the rsrbVirtRingEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsrbVirtRingEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rsrbVirtRingEntry_t *data=0;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            rsrbVirtRingIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rsrbVirtRingIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsrbVirtRingEntry_get(serialNum, contextInfo, arg ,searchType, rsrbVirtRingIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rsrbVirtRingIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsrbVirtRingIPAddr
    case I_rsrbVirtRingIPAddr:
       dp = (void *) (IPToOctetString(data->rsrbVirtRingIPAddr));
       break;
#endif /* I_rsrbVirtRingIPAddr */

#ifdef I_rsrbVirtRingMaxTcpQSize
    case I_rsrbVirtRingMaxTcpQSize:
       dp = (void *) (&data->rsrbVirtRingMaxTcpQSize);
       break;
#endif /* I_rsrbVirtRingMaxTcpQSize */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rsrbRemotePeerEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsrbRemotePeerEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rsrbRemotePeerEntry_t *data=0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            rsrbVirtRingIndex;
    long            rsrbRemotePeerIndex;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &rsrbRemotePeerIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rsrbVirtRingIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsrbRemotePeerEntry_get(serialNum, contextInfo, arg ,searchType, rsrbVirtRingIndex, rsrbRemotePeerIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->rsrbVirtRingIndex;
        inst.oid_ptr[1] = data->rsrbRemotePeerIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsrbRemotePeerEncapsulation
    case I_rsrbRemotePeerEncapsulation:
       dp = (void *) (&data->rsrbRemotePeerEncapsulation);
       break;
#endif /* I_rsrbRemotePeerEncapsulation */

#ifdef I_rsrbRemotePeerIPAddr
    case I_rsrbRemotePeerIPAddr:
       dp = (void *) (IPToOctetString(data->rsrbRemotePeerIPAddr));
       break;
#endif /* I_rsrbRemotePeerIPAddr */

#ifdef I_rsrbRemotePeerLocalIfIndex
    case I_rsrbRemotePeerLocalIfIndex:
       dp = (void *) (&data->rsrbRemotePeerLocalIfIndex);
       break;
#endif /* I_rsrbRemotePeerLocalIfIndex */

#ifdef I_rsrbRemotePeerState
    case I_rsrbRemotePeerState:
       dp = (void *) (&data->rsrbRemotePeerState);
       break;
#endif /* I_rsrbRemotePeerState */

#ifdef I_rsrbRemotePeerPacketsRx
    case I_rsrbRemotePeerPacketsRx:
       dp = (void *) (&data->rsrbRemotePeerPacketsRx);
       break;
#endif /* I_rsrbRemotePeerPacketsRx */

#ifdef I_rsrbRemotePeerPacketsTx
    case I_rsrbRemotePeerPacketsTx:
       dp = (void *) (&data->rsrbRemotePeerPacketsTx);
       break;
#endif /* I_rsrbRemotePeerPacketsTx */

#ifdef I_rsrbRemotePeerBytesRx
    case I_rsrbRemotePeerBytesRx:
       dp = (void *) (&data->rsrbRemotePeerBytesRx);
       break;
#endif /* I_rsrbRemotePeerBytesRx */

#ifdef I_rsrbRemotePeerBytesTx
    case I_rsrbRemotePeerBytesTx:
       dp = (void *) (&data->rsrbRemotePeerBytesTx);
       break;
#endif /* I_rsrbRemotePeerBytesTx */

#ifdef I_rsrbRemotePeerExplorersRx
    case I_rsrbRemotePeerExplorersRx:
       dp = (void *) (&data->rsrbRemotePeerExplorersRx);
       break;
#endif /* I_rsrbRemotePeerExplorersRx */

#ifdef I_rsrbRemotePeerTcpQueue
    case I_rsrbRemotePeerTcpQueue:
       dp = (void *) (&data->rsrbRemotePeerTcpQueue);
       break;
#endif /* I_rsrbRemotePeerTcpQueue */

#ifdef I_rsrbRemotePeerDrops
    case I_rsrbRemotePeerDrops:
       dp = (void *) (&data->rsrbRemotePeerDrops);
       break;
#endif /* I_rsrbRemotePeerDrops */

#ifdef I_rsrbRemotePeerLocalAck
    case I_rsrbRemotePeerLocalAck:
       dp = (void *) (&data->rsrbRemotePeerLocalAck);
       break;
#endif /* I_rsrbRemotePeerLocalAck */

#ifdef I_rsrbRemotePeerVersion
    case I_rsrbRemotePeerVersion:
       dp = (void *) (&data->rsrbRemotePeerVersion);
       break;
#endif /* I_rsrbRemotePeerVersion */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rsrbRingEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsrbRingEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rsrbRingEntry_t *data=0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            rsrbVirtRingIndex;
    long            rsrbRingIndex;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &rsrbRingIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rsrbVirtRingIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsrbRingEntry_get(serialNum, contextInfo, arg ,searchType, rsrbVirtRingIndex, rsrbRingIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->rsrbVirtRingIndex;
        inst.oid_ptr[1] = data->rsrbRingIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsrbRingBridge
    case I_rsrbRingBridge:
       dp = (void *) (&data->rsrbRingBridge);
       break;
#endif /* I_rsrbRingBridge */

#ifdef I_rsrbRingLocal
    case I_rsrbRingLocal:
       dp = (void *) (&data->rsrbRingLocal);
       break;
#endif /* I_rsrbRingLocal */

#ifdef I_rsrbRingType
    case I_rsrbRingType:
       dp = (void *) (&data->rsrbRingType);
       break;
#endif /* I_rsrbRingType */

#ifdef I_rsrbRingMacAddr
    case I_rsrbRingMacAddr:
       dp = (void *) (CloneOctetString(data->rsrbRingMacAddr));
       break;
#endif /* I_rsrbRingMacAddr */

#ifdef I_rsrbRingLocalIfIndex
    case I_rsrbRingLocalIfIndex:
       dp = (void *) (&data->rsrbRingLocalIfIndex);
       break;
#endif /* I_rsrbRingLocalIfIndex */

#ifdef I_rsrbRingRemoteIpAddress
    case I_rsrbRingRemoteIpAddress:
       dp = (void *) (IPToOctetString(data->rsrbRingRemoteIpAddress));
       break;
#endif /* I_rsrbRingRemoteIpAddress */

#ifdef I_rsrbRingNbrPacketsFwd
    case I_rsrbRingNbrPacketsFwd:
       dp = (void *) (&data->rsrbRingNbrPacketsFwd);
       break;
#endif /* I_rsrbRingNbrPacketsFwd */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





rsrbVirtRingEntry_t * k_rsrbVirtRingEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   rsrbVirtRingIndex)
   
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long rsrbVirtRingIndex;
{
   static rsrbVirtRingEntry_t rsrbVirtRingEntryData;
   tr_vgrouptype *vrg = GetVrg(searchType, rsrbVirtRingIndex);
   ipaddrtype peername;
    
   if (vrg==0)
      return 0;


   rsrbVirtRingEntryData.rsrbVirtRingIndex = vrg->vrn;

   peername = vrtcp_peername(vrg);
   if (peername) {
       rsrbVirtRingEntryData.rsrbVirtRingIPAddr = peername;
   } else {
       rsrbVirtRingEntryData.rsrbVirtRingIPAddr = rsrb_fst_name;   
   }
   rsrbVirtRingEntryData.rsrbVirtRingMaxTcpQSize = RSRB_TCP_MAX_QUEUE_DEFAULT;
   return(&rsrbVirtRingEntryData);
}

rsrbRemotePeerEntry_t * k_rsrbRemotePeerEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   rsrbVirtRingIndex, 
   rsrbRemotePeerIndex)
   
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long rsrbVirtRingIndex;
   long rsrbRemotePeerIndex;
{
   static rsrbRemotePeerEntry_t rsrbRemotePeerEntryData;

   tr_vpeertype *vrp = GetVrp (searchType, rsrbVirtRingIndex, rsrbRemotePeerIndex);
   
   if (vrp==0)
      return 0;
      
   rsrbRemotePeerEntryData.rsrbRemotePeerIndex = vrp->snmp_index;
   rsrbRemotePeerEntryData.rsrbRemotePeerIPAddr = 0;
   rsrbRemotePeerEntryData.rsrbRemotePeerLocalIfIndex = 0;
   switch (vrp->type) {
     case RSRB_PT_TCPD: 
       rsrbRemotePeerEntryData.rsrbRemotePeerEncapsulation =
         D_rsrbRemotePeerEncapsulation_tcp;
       rsrbRemotePeerEntryData.rsrbRemotePeerIPAddr = vrp->ipaddr;
       break;
     case RSRB_PT_IF: 
       rsrbRemotePeerEntryData.rsrbRemotePeerEncapsulation =
         D_rsrbRemotePeerEncapsulation_serial;
       rsrbRemotePeerEntryData.rsrbRemotePeerLocalIfIndex = 
         vrp->outidb->snmp_if_index;
       break;
     case RSRB_PT_LAN: 
       rsrbRemotePeerEntryData.rsrbRemotePeerEncapsulation =
         D_rsrbRemotePeerEncapsulation_lan;
       rsrbRemotePeerEntryData.rsrbRemotePeerLocalIfIndex = 
         vrp->outidb->snmp_if_index;
       break;
     case RSRB_PT_FST: 
       rsrbRemotePeerEntryData.rsrbRemotePeerEncapsulation =
         D_rsrbRemotePeerEncapsulation_fst;
       rsrbRemotePeerEntryData.rsrbRemotePeerIPAddr = vrp->ipaddr;
       break;
     case RSRB_PT_FR: 
       rsrbRemotePeerEntryData.rsrbRemotePeerEncapsulation =
         D_rsrbRemotePeerEncapsulation_frameRelay;
       rsrbRemotePeerEntryData.rsrbRemotePeerLocalIfIndex = 
         vrp->outidb->snmp_if_index;
       break;
     case RSRB_PT_NONE:
     default:
       break;
  
     }
       
      
   switch (vrp->state) {
     case (RSRB_PS_DEAD):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_dead;
       break;
     case (RSRB_PS_CLOSED):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_closed;
       break;
     case (RSRB_PS_OPENING):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_opening;
       break;
     case (RSRB_PS_OPENWAIT):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_openWaitXport;
       break;
     case (RSRB_PS_REMWAIT):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_waitRemoteRsp;
       break;
     case (RSRB_PS_REMGO):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_remoteResponded;
       break;
     case (RSRB_PS_REMOPENED):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_remoteOpened;
       break;
     case (RSRB_PS_DRAINING):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_draining;
       break;
     case (RSRB_PS_CONNECTED):
       rsrbRemotePeerEntryData.rsrbRemotePeerState =
         D_rsrbRemotePeerState_connected;
       break;
     default:
       ;
     }
       
   rsrbRemotePeerEntryData.rsrbRemotePeerPacketsRx = vrp->pkts_rx;
   rsrbRemotePeerEntryData.rsrbRemotePeerPacketsTx = vrp->pkts_tx;
   rsrbRemotePeerEntryData.rsrbRemotePeerBytesRx = vrp->bytes_rx;
   rsrbRemotePeerEntryData.rsrbRemotePeerBytesTx = vrp->bytes_tx;
   rsrbRemotePeerEntryData.rsrbRemotePeerExplorersRx = vrp->explorers;
   if (vrp->t)
     rsrbRemotePeerEntryData.rsrbRemotePeerTcpQueue = 
       reg_invoke_ip_tcpdriver_oqlen(vrp->t);
   else    
     rsrbRemotePeerEntryData.rsrbRemotePeerTcpQueue = 0;
   rsrbRemotePeerEntryData.rsrbRemotePeerDrops = vrp->drops;
   rsrbRemotePeerEntryData.rsrbRemotePeerLocalAck = vrp->local_ack;
   rsrbRemotePeerEntryData.rsrbRemotePeerVersion = vrp->vrp_version;
   rsrbRemotePeerEntryData.rsrbVirtRingIndex = vrp->vrg->vrn;
   return(&rsrbRemotePeerEntryData);
}

rsrbRingEntry_t * k_rsrbRingEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   rsrbVirtRingIndex, 
   rsrbRingIndex)

   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long rsrbVirtRingIndex;
   long rsrbRingIndex;
{
   static rsrbRingEntry_t rsrbRingEntryData;
   static OctetString macAddr;
   tr_ventrytype *vre = 
        GetVre(searchType, &rsrbVirtRingIndex, rsrbRingIndex);
   
   if (vre==0)
      return 0;
        
   rsrbRingEntryData.rsrbRingIndex = vre->rn;
   rsrbRingEntryData.rsrbRingBridge = vre->bn;
   rsrbRingEntryData.rsrbRingMacAddr = &macAddr;
   macAddr.length = IEEEBYTES;
   macAddr.octet_ptr = vre->macaddr;
   rsrbRingEntryData.rsrbRingRemoteIpAddress = 0;
   rsrbRingEntryData.rsrbRingLocalIfIndex = 0;
   switch (vre->type) {
     case (RSRB_ET_LOCAL):
       rsrbRingEntryData.rsrbRingLocal = TRUE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_static;   
       rsrbRingEntryData.rsrbRingLocalIfIndex = vre->outhole.idb->snmp_if_index;
       break;
     case (RSRB_ET_REMOTE):
       rsrbRingEntryData.rsrbRingLocal = FALSE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_dynamic;   
       rsrbRingEntryData.rsrbRingRemoteIpAddress = vre->outhole.vp->ipaddr;
       break;
     case (RSRB_ET_REMSTATIC):
       rsrbRingEntryData.rsrbRingLocal = FALSE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_static; 
       rsrbRingEntryData.rsrbRingRemoteIpAddress = vre->outhole.vp->ipaddr;
       break;
     case (RSRB_ET_SDLLC):
       rsrbRingEntryData.rsrbRingLocal = TRUE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_sdllc;  
       rsrbRingEntryData.rsrbRingLocalIfIndex = vre->outhole.idb->snmp_if_index;
       break;
     case (RSRB_ET_REMSDLLC):
       rsrbRingEntryData.rsrbRingLocal = FALSE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_sdllc;   
       rsrbRingEntryData.rsrbRingRemoteIpAddress = vre->outhole.vp->ipaddr;
       break;
     case (RSRB_ET_QLLC):
       rsrbRingEntryData.rsrbRingLocal = TRUE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_qllc;   
       rsrbRingEntryData.rsrbRingLocalIfIndex = vre->outhole.idb->snmp_if_index;
       break;
     case (RSRB_ET_REMQLLC):
       rsrbRingEntryData.rsrbRingLocal = FALSE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_qllc;   
       rsrbRingEntryData.rsrbRingRemoteIpAddress  = vre->outhole.vp->ipaddr;
       break;
     case (RSRB_ET_LOCAL_VIRTUAL):
       rsrbRingEntryData.rsrbRingLocal = TRUE;
       rsrbRingEntryData.rsrbRingType = D_rsrbRingType_virtual;   
       rsrbRingEntryData.rsrbRingLocalIfIndex = vre->outhole.idb->snmp_if_index;
       break;
     default:
     }   
   rsrbRingEntryData.rsrbRingNbrPacketsFwd = vre->forwards;
   rsrbRingEntryData.rsrbVirtRingIndex = rsrbVirtRingIndex;
   return(&rsrbRingEntryData);
}

/* GetVrg - get virtual ring group by unsorted vrn */
static tr_vgrouptype* GetVrg (int searchType, long rsrbVirtRingNbr)
{
   tr_vgrouptype * vrg;
   tr_vgrouptype * best_vrg=0;
   long best_so_far=MAXLONG;

   for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
      /* whether GET or GETNEXT, an exact match is as good as we can get */
      if (vrg->vrn == rsrbVirtRingNbr ) {
         return vrg;
      } 
      /* only if GETNEXT, save best lexigraphically so far */
      if (searchType == NEXT) {
         if ((vrg->vrn > rsrbVirtRingNbr)  &&
             (vrg->vrn < best_so_far)) {
             best_vrg = vrg;
             best_so_far = vrg->vrn;
         }
      } 
   }   
   return best_vrg;
}

/* GetVrp -- get remote peer, sorted by remote peer index */
static tr_vpeertype* GetVrp (int searchType, long rsrbVirtRingNbr, long rsrbRemotePeerIndex)
{
   tr_vpeertype *vrp=0;
   /* Get the vrg first */
   tr_vgrouptype *vrg = GetVrg (searchType, rsrbVirtRingNbr);
   if (searchType == NEXT) {
     /*------------------------------------------------------------------*/
     /* loop thru this vrg's peers to find the next peer.  If not found  */
     /* get the first vrp from the next peer.  Put it in a loop to go    */
     /* thru the rest of the vrgs until match found in case we have      */
     /* ring groups with no peers for some reason, although that should  */
     /* be a rare case.                                                  */
     /*------------------------------------------------------------------*/
     while (vrg != 0) {   
       if (vrg->vrn != rsrbVirtRingNbr)
         rsrbRemotePeerIndex = 0;
       for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
           if ((vrp->snmp_index >= rsrbRemotePeerIndex) &&
               (vrp->ipaddr != vrtcp_peername(vrg)))
              /* found a good match that wasn't the local peer */
              return vrp;
       }
       /*----------------------------------------------------------*/
       /* if we got here, there are no more vrps on this vrg.  so  */
       /* go to the next one.                                      */ 
       /*----------------------------------------------------------*/
       rsrbVirtRingNbr += 1;    
       rsrbRemotePeerIndex = 0;
       vrg = GetVrg (searchType, rsrbVirtRingNbr);
     }  
   } else {  /* EXACT */  
     /*----------------------------------------------------------------*/
     /* virt ring group must match, and don't return the local peer,   */
     /* which has vrp index of 0.                                      */
     /*----------------------------------------------------------------*/
     if (vrg != 0) {
       for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
           if (vrp->snmp_index == rsrbRemotePeerIndex)
              /* don't return the local peer as a remote peer */
              if (vrp->ipaddr != vrtcp_peername(vrg))
                 return vrp;
              else
                 return 0; 
       } 
     }        
   }   
   return vrp;
}

/* GetVre - get virtual ring entry by unsorted vrn and unsorted ring nbr  */ 
static tr_ventrytype* GetVre (int searchType, long *PrsrbVirtRingNbr, long rsrbRingNbr)
{
   long best_so_far=MAXLONG;
   tr_ventrytype * best_vre=0;
   tr_ventrytype * vre=0;
   
   /* Get the vrg first */
   tr_vgrouptype *vrg= GetVrg (searchType,*PrsrbVirtRingNbr);
   if (searchType == NEXT) {
     /*------------------------------------------------------------------*/
     /* loop thru this vrg's peers to find the next peer.  If not found  */
     /* get the first vrp from the next peer.  Put it in a loop to go    */
     /* thru the rest of the vrgs until match found in case we have      */
     /* ring groups with no peers for some reason, although that should  */
     /* be a rare exception.                                             */
     /*------------------------------------------------------------------*/
     while (vrg != 0) { 
       if (vrg->vrn != *PrsrbVirtRingNbr)
          rsrbRingNbr = 0;    
       *PrsrbVirtRingNbr = vrg->vrn;  
       for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
         if ((vre->rn >= rsrbRingNbr) &&
             (vre->rn < best_so_far)) {
             best_vre = vre;
             best_so_far = vre->rn;
         }
       }  
       if (best_vre != 0) {
          return best_vre;
        /*----------------------------------------------------------*/
        /* else no more vres on this vrg. go to the next one.       */
        /*----------------------------------------------------------*/
       } else {
         *PrsrbVirtRingNbr += 1;    
         vrg = GetVrg (searchType, *PrsrbVirtRingNbr);
       }
     }
   } else {  /* EXACT */   
     if (vrg != 0) {
       for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
           if (vre->rn == rsrbRingNbr)
              return vre;
       }       
     }       
   }   
   return vre;
}

/*
*  rsrbPeerStateChangeNotification NOTIFICATION-TYPE
*     VARIABLES { rsrbRemotePeerState }
*     DESCRIPTION
*        "This notification indicates that the state of an RSRB remote peer
*         has transitioned to active (psConnected) or inactive
*         (psDead or psClosed)."
*     ::= { rsrbTraps 1 }
*/     
static const OID rsrbPeerOID = {LNciscoRsrbMIB, (ulong *)IDciscoRsrbMIB};

#define RSRB_TRAP_VBCOUNT  1
#define RSRB_TRAP_NUM 1

static const OID rsrbPeerStateChange_varbinds[RSRB_TRAP_VBCOUNT] = {
    {LNrsrbRemotePeerState, (ulong *)IDrsrbRemotePeerState}
};
void generateRSRBtrap (tr_vpeertype *vrp)
{
       
    OID indexOID;
    ulong index[2];
    
    indexOID.length = 2;
    indexOID.oid_ptr = index;
    index[0] = vrp->vrg->vrn;
    index[1] = vrp->snmp_index;

    SnmpSendTrap_Generic(rsrbPeerStateChange_varbinds,
                         &rsrbPeerOID,
			 peerStateChangeOid,
                         &indexOID,
                         RSRB_TRAP_VBCOUNT,
                         ENTERPRISE_TRAP, 
                         RSRB_TRAP_NUM);
}

 
void
init_rsrbmib (subsystype *subsys)
 {
    load_mib(rsrbmib_OidList, rsrbmib_OidListNum);
    load_oid(rsrbmib_oid_table);
    register_snmp_trap(TRAP_RSRB,  peerStateChangeOid);
    reg_add_generateRSRBtrap(generateRSRBtrap, "generateRSRBtrap");
}

/*
 * rsrbmib subsystem header
 */

#define MAJVERSION_rsrbmib 1
#define MINVERSION_rsrbmib 0
#define EDITVERSION_rsrbmib 0

/*
 * Since this subsystem is in the net management class,
 * there's no point in putting in "seq: srb"
 * The class hierarchy takes care of that
 */

SUBSYS_HEADER(rsrbmib,
              MAJVERSION_rsrbmib,
              MINVERSION_rsrbmib,
              EDITVERSION_rsrbmib,
              init_rsrbmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: srb");
      
