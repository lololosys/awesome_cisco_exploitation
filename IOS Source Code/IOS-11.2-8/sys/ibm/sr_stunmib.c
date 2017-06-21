/* $Id: sr_stunmib.c,v 3.6.12.3 1996/05/21 09:48:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sr_stunmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code. One line description here.
 *
 * April 1995, Bob Clouston (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_stunmib.c,v $
 * Revision 3.6.12.3  1996/05/21  09:48:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.12.2  1996/05/21  06:30:22  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.12.1  1996/05/17  11:21:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.1  1996/04/26  07:54:38  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/14  20:56:19  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/24  22:03:47  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.4  1995/12/14  08:24:35  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  09:24:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:32:02  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:25:55  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:48:50  hampton
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
#include "logger.h"
#include "subsys.h"
#include "interface_generic.h"
#include "../snmp/snmp_api.h"
#include "sr_stunmib.h"
#include "sr_stunmib-mib.h"
/*#include "stun.h"*/
#include "ibm.h"
#include "sdlc.h"
#include "../snmp/snmp_registry.h"
#include "../ip/ip_registry.h"
#include "sys_registry.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_trap_fe.h"
#include "ibm_externs.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

static const char peerStateChangeOid[] = "ciscoStunMIB.2.0.1";

static boolean idb_tester(hwidbtype *, long* index2);
static hwidbtype *GetIdbFromStunIndex (int searchType, long stunGroupIndex);
static stunpeertype *GetStpFromAddr (int searchType, long stunGroupIndex, 
                      long stunRouteStationAddress);
/*
 * Start of system-independent routines
 */

/*---------------------------------------------------------------------
 * Retrieve data from the stunGlobal family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
stunGlobal_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    stunGlobal_t    *data=0;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_stunGlobal_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_stunIPAddr
    case I_stunIPAddr:
       dp = (void *) (IPToOctetString(data->stunIPAddr));
       break;
#endif /* I_stunIPAddr */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}


/*---------------------------------------------------------------------
 * Retrieve data from the stunGroupEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
stunGroupEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    stunGroupEntry_t *data=0;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            stunGroupIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &stunGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        stunGroupIndex = MAX(1, stunGroupIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_stunGroupEntry_get(serialNum, contextInfo, arg ,searchType, stunGroupIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->stunGroupIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_stunProtocolType
    case I_stunProtocolType:
       dp = (void *) (&data->stunProtocolType);
       break;
#endif /* I_stunProtocolType */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the stunPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
stunPortEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    stunPortEntry_t *data=0;
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
     if ( (arg == -1) || (data = k_stunPortEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_stunPortGroupIndex
    case I_stunPortGroupIndex:
       dp = (void *) (&data->stunPortGroupIndex);
       break;
#endif /* I_stunPortGroupIndex */

#ifdef I_stunPortDefaultPeerType
    case I_stunPortDefaultPeerType:
       dp = (void *) (&data->stunPortDefaultPeerType);
       break;
#endif /* I_stunPortDefaultPeerType */

#ifdef I_stunPortDefaultPeerIP
    case I_stunPortDefaultPeerIP:
       dp = (void *) (IPToOctetString(data->stunPortDefaultPeerIP));
       break;
#endif /* I_stunPortDefaultPeerIP */

#ifdef I_stunPortDefaultPeerSerialInterface
    case I_stunPortDefaultPeerSerialInterface:
       dp = (void *) (&data->stunPortDefaultPeerSerialInterface);
       break;
#endif /* I_stunPortDefaultPeerSerialInterface */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*---------------------------------------------------------------------
 * Retrieve data from the stunRouteEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
stunRouteEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    stunRouteEntry_t *data=0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            stunGroupIndex;
    long            stunRouteStationAddress;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &stunRouteStationAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        stunRouteStationAddress = MAX(1, stunRouteStationAddress);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &stunGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        stunGroupIndex = MAX(1, stunGroupIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_stunRouteEntry_get(serialNum, contextInfo, arg ,searchType, stunGroupIndex, stunRouteStationAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->stunGroupIndex;
        inst.oid_ptr[1] = data->stunRouteStationAddress;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_stunRouteType
    case I_stunRouteType:
       dp = (void *) (&data->stunRouteType);
       break;
#endif /* I_stunRouteType */

#ifdef I_stunRouteRemoteIP
    case I_stunRouteRemoteIP:
       dp = (void *) (IPToOctetString(data->stunRouteRemoteIP));
       break;
#endif /* I_stunRouteRemoteIP */

#ifdef I_stunRouteSerialInterface
    case I_stunRouteSerialInterface:
       dp = (void *) (&data->stunRouteSerialInterface);
       break;
#endif /* I_stunRouteSerialInterface */

#ifdef I_stunRoutePriority
    case I_stunRoutePriority:
       dp = (void *) (&data->stunRoutePriority);
       break;
#endif /* I_stunRoutePriority */

#ifdef I_stunRoutePeerState
    case I_stunRoutePeerState:
       dp = (void *) (&data->stunRoutePeerState);
       break;
#endif /* I_stunRoutePeerState */

#ifdef I_stunRouteLocalAck
    case I_stunRouteLocalAck:
       dp = (void *) (&data->stunRouteLocalAck);
       break;
#endif /* I_stunRouteLocalAck */

#ifdef I_stunRouteRxPackets
    case I_stunRouteRxPackets:
       dp = (void *) (&data->stunRouteRxPackets);
       break;
#endif /* I_stunRouteRxPackets */

#ifdef I_stunRouteTxPackets
    case I_stunRouteTxPackets:
       dp = (void *) (&data->stunRouteTxPackets);
       break;
#endif /* I_stunRouteTxPackets */

#ifdef I_stunRouteRxBytes
    case I_stunRouteRxBytes:
       dp = (void *) (&data->stunRouteRxBytes);
       break;
#endif /* I_stunRouteRxBytes */

#ifdef I_stunRouteTxBytes
    case I_stunRouteTxBytes:
       dp = (void *) (&data->stunRouteTxBytes);
       break;
#endif /* I_stunRouteTxBytes */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}



/*
 * Start of system-dependent routines
 */




stunGlobal_t *
k_stunGlobal_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static stunGlobal_t stunGlobalData;

   stunGlobalData.stunIPAddr = get_stun_name();
   
   return(&stunGlobalData);
}


stunGroupEntry_t * k_stunGroupEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   stunGroupIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long stunGroupIndex;
{
   static stunGroupEntry_t stunGroupEntryData;
   char pname[NAMEDTHINGNAMELEN+1];
   hwidbtype *hwidb = GetIdbFromStunIndex(searchType, stunGroupIndex);
   if (hwidb==0)
     return 0;     
   stunGroupEntryData.stunGroupIndex = (long)hwidb->stun_group;
   
   get_stun_pname (hwidb->stun_group, pname);
   if (!strncmp(pname,"basic",NAMEDTHINGNAMELEN+1))
      stunGroupEntryData.stunProtocolType = D_stunProtocolType_basic;
   else if (!strncmp(pname,"sdlc",NAMEDTHINGNAMELEN+1))
      stunGroupEntryData.stunProtocolType = D_stunProtocolType_sdlc;
   else if (!strncmp(pname,"sdlc-tg",NAMEDTHINGNAMELEN+1))
      stunGroupEntryData.stunProtocolType = D_stunProtocolType_sdlctg;
   else
      stunGroupEntryData.stunProtocolType = D_stunProtocolType_custom;
      
   return(&stunGroupEntryData);
}
static hwidbtype* GetIdbFromStunIndex(int searchType, long stunGroupIndex)
{
   hwidbtype     *hwidb;
   hwidbtype     *best_idb=0;
   int best_so_far=MAXINT;
   
   FOR_ALL_HWIDBS(hwidb) {
       if (searchType == NEXT) {
          if ((hwidb->enctype == ET_STUN) && (hwidb->stun_group)) {
             if ((hwidb->stun_group >= stunGroupIndex) &&
                 (hwidb->stun_group < best_so_far)) {
                 best_idb = hwidb;
                 best_so_far = hwidb->stun_group;
             }    
          }
       } else {  /* EXACT */
          if ((hwidb->enctype == ET_STUN) && 
              (hwidb->stun_group == stunGroupIndex)) {
              return hwidb;
          }
       }       
   }                   
   return best_idb;
}  

stunPortEntry_t * k_stunPortEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
   stunpeertype *stp;
   static stunPortEntry_t stunPortEntryData;

   hwidbtype *hwidb = ifmib_hwidb_get(serialNum, searchType, 
				      ifIndex, idb_tester, NULL);
   if (hwidb==NULL)
     return NULL;     

   stunPortEntryData.ifIndex = hwidb->snmp_if_index;
   stunPortEntryData.stunPortGroupIndex = (long)hwidb->stun_group;
   for (stp = (stunpeertype *) hwidb->stun_Q.qhead; stp; stp = stp->next) {
     if (stp->all_addr) {  /* found default peer */
       switch (stp->type) {
         case STUN_PT_TCPD:
           stunPortEntryData.stunPortDefaultPeerType = 
               D_stunPortDefaultPeerType_ip;
           stunPortEntryData.stunPortDefaultPeerIP = stp->ipaddr;
           stunPortEntryData.stunPortDefaultPeerSerialInterface = 0;
           break;
         case STUN_PT_IF:
           stunPortEntryData.stunPortDefaultPeerType =
               D_stunPortDefaultPeerType_direct;
           stunPortEntryData.stunPortDefaultPeerIP = 0;
           stunPortEntryData.stunPortDefaultPeerSerialInterface = 
              stp->outidb->snmp_if_index;
           break;
         case STUN_PT_FR:
           stunPortEntryData.stunPortDefaultPeerType =
               D_stunPortDefaultPeerType_frameRelay;
           stunPortEntryData.stunPortDefaultPeerIP = 0;
           stunPortEntryData.stunPortDefaultPeerSerialInterface = 
              stp->outswidb->hwptr->snmp_if_index;
           break;
         case STUN_PT_ANY:
         case STUN_PT_NONE:
         default:
           stunPortEntryData.stunPortDefaultPeerType =
               D_stunPortDefaultPeerType_other;
           stunPortEntryData.stunPortDefaultPeerIP = stp->ipaddr;
           stunPortEntryData.stunPortDefaultPeerSerialInterface = 0;   
         }  
       break;
     } else {
       stunPortEntryData.stunPortDefaultPeerType =
               D_stunPortDefaultPeerType_other;
     }          
    }
   return(&stunPortEntryData);
}

stunRouteEntry_t * k_stunRouteEntry_get
  (serialNum, 
   contextInfo, 
   nominator, 
   searchType, 
   stunGroupIndex, 
   stunRouteStationAddress)
   
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long stunGroupIndex;
   long stunRouteStationAddress;
{
   static stunRouteEntry_t stunRouteEntryData;
   stunpeertype *stp = GetStpFromAddr(searchType, stunGroupIndex, 
                          stunRouteStationAddress);
   if (stp==0)
     return 0;

   stunRouteEntryData.stunGroupIndex = (long) stp->group;
   if (stp->all_addr)
     stunRouteEntryData.stunRouteStationAddress = 256;
   else
     stunRouteEntryData.stunRouteStationAddress = (long)stp->addr[0];
   switch (stp->type) {
       case STUN_PT_TCPD:
         stunRouteEntryData.stunRouteType = D_stunRouteType_ip;
         stunRouteEntryData.stunRouteRemoteIP = stp->ipaddr;
         stunRouteEntryData.stunRouteSerialInterface = 0;
         break;
       case STUN_PT_IF:
         stunRouteEntryData.stunRouteType = D_stunRouteType_direct;
         stunRouteEntryData.stunRouteRemoteIP = 0;
         stunRouteEntryData.stunRouteSerialInterface = 
              stp->outidb->snmp_if_index;
         break;
       case STUN_PT_FR:
         stunRouteEntryData.stunRouteType = D_stunRouteType_frameRelay;
         stunRouteEntryData.stunRouteRemoteIP = 0;
         stunRouteEntryData.stunRouteSerialInterface = 
              stp->outswidb->hwptr->snmp_if_index;
         break;
       case STUN_PT_ANY:
       case STUN_PT_NONE:
       default:
         stunRouteEntryData.stunRouteType = D_stunRouteType_other;
         stunRouteEntryData.stunRouteRemoteIP = 0;
         stunRouteEntryData.stunRouteSerialInterface = 0;
      }  
   
   if (!stp->priority_top) {
        stunRouteEntryData.stunRoutePriority = D_stunRoutePriority_high;
   } else {
	if (stp->priority_top->priority_peers[1] == stp)
	   stunRouteEntryData.stunRoutePriority = D_stunRoutePriority_medium;
	else if (stp->priority_top->priority_peers[2] == stp)
            stunRouteEntryData.stunRoutePriority = D_stunRoutePriority_normal;
	else if (stp->priority_top->priority_peers[3] == stp)
            stunRouteEntryData.stunRoutePriority = D_stunRoutePriority_low;
   } 
   switch (stp->state) {
     case STUN_PS_DEAD:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_dead;
       break;
     case STUN_PS_CLOSED:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_closed;
       break;
     case STUN_PS_OPENING:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_opening;
       break;
     case STUN_PS_OPENWAIT:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_openWait;
       break;
     case STUN_PS_CONNECTED:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_connected;
       break;
     case STUN_PS_DIRECT:
       stunRouteEntryData.stunRoutePeerState = D_stunRoutePeerState_direct;
       break;
     default:
     }
   stunRouteEntryData.stunRouteLocalAck = stp->local_ack_active;
   stunRouteEntryData.stunRouteRxPackets = stp->pkts_rx;
   stunRouteEntryData.stunRouteTxPackets = stp->pkts_tx;
   stunRouteEntryData.stunRouteRxBytes = stp->bytes_rx;
   stunRouteEntryData.stunRouteTxBytes = stp->bytes_tx;
   return(&stunRouteEntryData);
}

static stunpeertype *GetStpFromAddr (int searchType, long stunGroupIndex, long stunRouteStationAddress)
{
   hwidbtype     *hwidb;
   stunpeertype  *stp;
   stunpeertype  *best_stp=0;
   long best_so_far=MAXINT;
   /*-------------------------------------------------------------------   
    * Find the peer based on station address.  First find the idb
    * defined with the stun group.  Then use the stun_Q list to
    * find the address.  The stun_Q list is unordered except that
    * the 'all' entry is last.  'all' is also stored as address 0 but 
    * is returned in the mib as 256 so skip over it until no other
    * match found.   
    *------------------------------------------------------------------*/  
   hwidb = GetIdbFromStunIndex (searchType, stunGroupIndex);  
   if (searchType == NEXT) {
      while (hwidb != 0) {
        if (hwidb->stun_group != stunGroupIndex)
           stunRouteStationAddress = 0;
        for (stp = (stunpeertype *) hwidb->stun_Q.qhead; stp; stp = stp->next) {
            if ((stp->addr[0] >= stunRouteStationAddress) &&
                (stp->addr[0] < best_so_far) &&
                (stp->addr[0] != 0)) {
                best_stp = stp;
                best_so_far = stp->addr[0];
            }
        }    
        /* check last peer for 'all' */
        if (best_stp != 0) {
           return best_stp;
        } else {   
           stp = hwidb->stun_Q.qtail;
           if (stp->all_addr && (stunRouteStationAddress <= 256)) {
              return stp;
           } else {   
              /* No entries.  Increment to next stun group and start over */
              stunGroupIndex += 1;
              stunRouteStationAddress = 0;
              hwidb = GetIdbFromStunIndex (searchType, stunGroupIndex);  
           }    
        }
      }      
   } else { /* EXACT */       
      if (hwidb != 0) {
         for (stp = (stunpeertype *) hwidb->stun_Q.qhead; stp; stp = stp->next) {
             if (((stp->addr[0] == stunRouteStationAddress) &&
                  (stunRouteStationAddress != 0)) ||
                  (stp->all_addr && (stunRouteStationAddress == 256)))
                 return stp;
             else
                 best_stp = 0;  /* fall thru & return null */    
         }              
      }
   }    
   /* if we got this far we didn't find a match */      
   return 0;
}

/*
* stunPeerStateChangeNotification NOTIFICATION-TYPE
*      OBJECTS { stunRoutePeerState }
*      STATUS      current
*      DESCRIPTION
*         "This notification indicates that the state of a STUN route
*          has transitioned to active (connected) or inactive
*          (dead or closed)."
*/
static const OID stunPeerOID = {LNciscoStunMIB, (ulong *)IDciscoStunMIB};

#define STUN_TRAP_VBCOUNT  1
#define STUN_TRAP_NUM 1

static const OID stunPeerStateChange_varbinds[STUN_TRAP_VBCOUNT] = {
    OIDconversion(stunRoutePeerState)
};
void stun_trap (stunpeertype *stp)
{
    OID indexOID;
    ulong index[2];
    
    indexOID.length = 2;
    indexOID.oid_ptr = index;
    index[0] = stp->group;
    if (stp->all_addr)
      index[1] = 256;
    else
      index[1] = stp->addr[0];

    SnmpSendTrap_Generic(stunPeerStateChange_varbinds,
                         &stunPeerOID,
			 peerStateChangeOid,
                         &indexOID,
                         STUN_TRAP_VBCOUNT,
                         ENTERPRISE_TRAP, 
                         STUN_TRAP_NUM);
}

void
init_stunmib (subsystype *subsys)
{
    load_mib(stunmib_OidList, stunmib_OidListNum);
    load_oid(stunmib_oid_table);
    register_snmp_trap(TRAP_STUN, peerStateChangeOid);
    reg_add_stun_trap(stun_trap, "stun_trap");
}

static boolean idb_tester(hwidbtype *idb, long* index2)
{
   if ((idb->enctype == ET_STUN) && (idb->stun_group)) 
      return(TRUE);
   else
      return(FALSE);
}

/*
 * stunmib subsystem header
 */


#define MAJVERSION_stunmib 1
#define MINVERSION_stunmib 0
#define EDITVERSION_stunmib 0

SUBSYS_HEADER(stunmib,
              MAJVERSION_stunmib,
              MINVERSION_stunmib,
              EDITVERSION_stunmib,
              init_stunmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: stun", NULL);
      
