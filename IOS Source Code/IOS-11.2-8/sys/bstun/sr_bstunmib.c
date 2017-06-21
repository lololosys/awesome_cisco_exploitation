/* $Id: sr_bstunmib.c,v 3.6.8.4 1996/09/10 08:48:54 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/sr_bstunmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Silvana Zdravkovski (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bstunmib.c,v $
 * Revision 3.6.8.4  1996/09/10  08:48:54  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/05/21  09:43:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.8.2  1996/05/21  06:24:16  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.8.1  1996/03/18  19:01:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:29:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/13  01:12:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/02/20  00:20:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/29  20:34:52  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.5  1996/02/27  20:17:10  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/24  22:07:39  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1996/01/18  15:44:00  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.2  1995/11/17  08:49:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/31  08:49:20  arothwel
 * CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
 * same.
 *
 * Revision 2.3  1995/07/17  07:30:57  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/16  06:24:55  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:46  hampton
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
#include "subsys.h"
#include "../h/interface_private.h"
#include "../if/priority.h"

#include "../snmp/snmp_api.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

#include "sr_bstunmib.h"
#include "sr_bstunmib-mib.h"
#include "bstun.h"
#include "bsc.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the bstunGlobal family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bstunGlobal_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    bstunGlobal_t   *data = 0;


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
     if ( (arg == -1) || (data = k_bstunGlobal_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_bstunIPAddr
    case I_bstunIPAddr:
       dp = (void *) (IPToOctetString(data->bstunIPAddr));
       break;
#endif /* I_bstunIPAddr */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bstunGroupEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bstunGroupEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    bstunGroupEntry_t *data = 0;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            bstunGroupIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &bstunGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        bstunGroupIndex = MAX(1, bstunGroupIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_bstunGroupEntry_get(serialNum, contextInfo, arg ,searchType, bstunGroupIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->bstunGroupIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_bstunProtocolType
    case I_bstunProtocolType:
       dp = (void *) (&data->bstunProtocolType);
       break;
#endif /* I_bstunProtocolType */

#ifdef I_bstunLocalAck
    case I_bstunLocalAck:
       dp = (void *) (&data->bstunLocalAck);
       break;
#endif /* I_bstunLocalAck */

#ifdef I_bstunGroupUnroutableTransmit
    case I_bstunGroupUnroutableTransmit:
       dp = (void *) (&data->bstunGroupUnroutableTransmit);
       break;
#endif /* I_bstunGroupUnroutableTransmit */

#ifdef I_bstunGroupUnroutableReceive
    case I_bstunGroupUnroutableReceive:
       dp = (void *) (&data->bstunGroupUnroutableReceive);
       break;
#endif /* I_bstunGroupUnroutableReceive */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bstunPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bstunPortEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    bstunPortEntry_t *data = 0;
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
     if ( (arg == -1) || (data = k_bstunPortEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_bstunPortGroupNumber
    case I_bstunPortGroupNumber:
       dp = (void *) (&data->bstunPortGroupNumber);
       break;
#endif /* I_bstunPortGroupNumber */

#ifdef I_bstunPortDefaultPeerType
    case I_bstunPortDefaultPeerType:
       dp = (void *) (&data->bstunPortDefaultPeerType);
       break;
#endif /* I_bstunPortDefaultPeerType */

#ifdef I_bstunPortDefaultPeerIP
    case I_bstunPortDefaultPeerIP:
       dp = (void *) (IPToOctetString(data->bstunPortDefaultPeerIP));
       break;
#endif /* I_bstunPortDefaultPeerIP */

#ifdef I_bstunPortDefaultPeerSerial
    case I_bstunPortDefaultPeerSerial:
       dp = (void *) (&data->bstunPortDefaultPeerSerial);
       break;
#endif /* I_bstunPortDefaultPeerSerial */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bstunRouteEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bstunRouteEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    bstunRouteEntry_t *data = 0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            bstunRouteGroupIndex;
    long            bstunRouteStationAddress;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &bstunRouteStationAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        bstunRouteStationAddress = MAX(1, bstunRouteStationAddress);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &bstunRouteGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        bstunRouteGroupIndex = MAX(1, bstunRouteGroupIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_bstunRouteEntry_get(serialNum, contextInfo, arg ,searchType, bstunRouteGroupIndex, bstunRouteStationAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->bstunRouteGroupIndex;
        inst.oid_ptr[1] = data->bstunRouteStationAddress;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_bstunRouteType
    case I_bstunRouteType:
       dp = (void *) (&data->bstunRouteType);
       break;
#endif /* I_bstunRouteType */

#ifdef I_bstunRouteIP
    case I_bstunRouteIP:
       dp = (void *) (IPToOctetString(data->bstunRouteIP));
       break;
#endif /* I_bstunRouteIP */

#ifdef I_bstunRouteSerial
    case I_bstunRouteSerial:
       dp = (void *) (&data->bstunRouteSerial);
       break;
#endif /* I_bstunRouteSerial */

#ifdef I_bstunRoutePriority
    case I_bstunRoutePriority:
       dp = (void *) (&data->bstunRoutePriority);
       break;
#endif /* I_bstunRoutePriority */

#ifdef I_bstunRoutePeerState
    case I_bstunRoutePeerState:
       dp = (void *) (&data->bstunRoutePeerState);
       break;
#endif /* I_bstunRoutePeerState */

#ifdef I_bstunRouteRxPackets
    case I_bstunRouteRxPackets:
       dp = (void *) (&data->bstunRouteRxPackets);
       break;
#endif /* I_bstunRouteRxPackets */

#ifdef I_bstunRouteTxPackets
    case I_bstunRouteTxPackets:
       dp = (void *) (&data->bstunRouteTxPackets);
       break;
#endif /* I_bstunRouteTxPackets */

#ifdef I_bstunRouteRxBytes
    case I_bstunRouteRxBytes:
       dp = (void *) (&data->bstunRouteRxBytes);
       break;
#endif /* I_bstunRouteRxBytes */

#ifdef I_bstunRouteTxBytes
    case I_bstunRouteTxBytes:
       dp = (void *) (&data->bstunRouteTxBytes);
       break;
#endif /* I_bstunRouteTxBytes */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */

/*-------------------------------------------------------------*/ 
/* snmp_bstun_idb_tester: Is this hwidb configured with bstun? */
/*-------------------------------------------------------------*/
static boolean snmp_bstun_idb_tester(hwidbtype *idb, long* index2)
{
    return is_bstun(idb);
}

/*--------------------------------------------------------------------------------*/ 
/* find_stp_in_bstun_Q: Search bstun queue for stun peer type with given address. */
/*--------------------------------------------------------------------------------*/
static bstunpeertype * find_stp_in_bstun_Q(int searchType, hwidbtype *idb, long addr)
{
    bstunpeertype *stp;
    bstunpeertype *beststp=0;
    uchar bestaddr = BSTUN_MAX_GROUP;

    /*---- The bstun_Q.qhead is unordered except that the 'all' entry is stored
           at the end. 'all' is also stored as address 0 but is returned in 
           mib as 256 so skip over it until no other match found. */

    for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = stp->next) {
        if ((stp->type == BSTUN_PT_TCPD) && (stp->input_addr_inuse)) {
            if ((addr == stp->input_addr[0]) || ((addr <= stp->input_addr[0]) && (searchType == NEXT))) {
                if ((bestaddr > stp->input_addr[0]) && (stp->input_addr[0] != 0)){
                    bestaddr = stp->input_addr[0];
                    beststp = stp;
	        }
	    }
	}
        if ((addr == stp->addr[0]) || ((addr <= stp->addr[0]) && (searchType == NEXT))) {
            if ((bestaddr > stp->addr[0]) && (stp->input_addr[0] != 0)) {
                bestaddr = stp->addr[0];
                beststp = stp;
	    }
	}
    }
    if (beststp == 0){
        if ((beststp = idb->bstun_Q.qtail) != 0){
            if (beststp->all_addr && (addr <= 256))
                return beststp;
            else
                beststp = 0;
        }
    }
      
    return beststp;	
}

/*-----------------------------------------------------------------------------------------*/ 
/* snmp_stp_get: Retrieve the correct stun peer type given group, address and search type. */
/*-----------------------------------------------------------------------------------------*/
static bstunpeertype * snmp_stp_get(int searchType, 
                                    long bstunGroup, 
                                    long bstunAddress,
                                    hwidbtype **idbP)
{
   hwidbtype *idb;
   bstunpeertype *stp;
   bstunpeertype *beststp=0;
   bstun_protocol_info *gpi;
   int bestgroup = BSTUN_MAX_GROUP;

   /*---- Each time we find a best (or better) group then scan the bstun_Q looking for the
          best stp for that group.  */
   FOR_ALL_HWIDBS(idb) {
       if (is_bstun(idb)){
           if (!idb->bstun_Q.qhead)
               continue;
           if ((gpi = bstun_pinfo[idb->bstun_group]) == NULL)
               continue;
           if (((searchType == NEXT) && (bstunGroup <=  idb->bstun_group)) || (bstunGroup == idb->bstun_group)){
               if (bstunGroup < idb->bstun_group)
                   bstunAddress = 0; /*---- Don't skip any addrs < bstunAddress in next group */
               if (bestgroup > idb->bstun_group){
                   /*--- Yes, we have found a better group, now find the best addr in the group */
                   if ((stp = find_stp_in_bstun_Q(searchType, idb, bstunAddress)) != 0){
                       *idbP = idb;
                       beststp = stp;
                       bestgroup = idb->bstun_group;
                   }
	       }
           }
       }
   }
   return beststp;   
}

/*---------------------------------------------------------------------------*/ 
/*snmp_get_group: Retrieve the correct group protocol info given group index.*/
/*---------------------------------------------------------------------------*/
static bstun_protocol_info *snmp_get_group(int searchType, long *bstunGroupIndex)
{
    bstun_protocol_info *gpi;

    if (searchType == NEXT){
        while(((gpi = bstun_pinfo[*bstunGroupIndex]) == 0) && ((*bstunGroupIndex)++ < 255))
            ;
        return gpi;
    }
    else /*---- EXACT */
        return bstun_pinfo[*bstunGroupIndex];
    
    return 0;
}

/*---------------------------------------------------------------------*/ 
/*find_stp_all_addr: Find the bstun peer type with all addr configured.*/
/*---------------------------------------------------------------------*/
static bstunpeertype *find_stp_all_addr(hwidbtype *idb)
{
    bstun_protocol_info *gpi;
    bstunpeertype *stp;

    if (idb->bstun_Q.qhead  && ((gpi = bstun_pinfo[idb->bstun_group]) != 0))
        if ((stp = idb->bstun_Q.qtail) != 0) 
            if (stp->all_addr)
                return stp;

    return 0;
}

/*---------------------------------------------------------------*/ 
/*k_bstunGlobal_get:                                             */
/*---------------------------------------------------------------*/
extern ipaddrtype bstun_name;	
bstunGlobal_t *
k_bstunGlobal_get(int serialNum, ContextInfo *contextInfo, int nominator)
{
   static bstunGlobal_t bstunGlobalData;

   /*
    * put your code to retrieve the information here
    */
   bstunGlobalData.bstunIPAddr = bstun_name;  /*---- Global defined in bstun.c */
   return(&bstunGlobalData);
}

/*---------------------------------------------------------------*/ 
/*k_bstunGroupEntry_get:                                         */
/*---------------------------------------------------------------*/
bstunGroupEntry_t *
k_bstunGroupEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, int searchType, long bstunGroupIndex)
{
   static bstunGroupEntry_t bstunGroupEntryData;
   bstun_protocol_info *gpi;

   /*
    * put your code to retrieve the information here
    */
   memset (&bstunGroupEntryData, 0, sizeof (bstunGroupEntry_t)); 
   if ((gpi = snmp_get_group(searchType, &bstunGroupIndex)) == NULL) 
        return NULL;

   bstunGroupEntryData.bstunGroupIndex = bstunGroupIndex;

   if (!strncmp(gpi->name, BSC_PROTOCOL_NAME, NAMEDTHINGNAMELEN+1) ||
       !strncmp(gpi->name, BSC_LACK_PROTOCOL_NAME, NAMEDTHINGNAMELEN+1))
       bstunGroupEntryData.bstunProtocolType = D_bstunProtocolType_bsc;

   bstunGroupEntryData.bstunLocalAck = 
        (gpi->local_ack_capable ? D_bstunLocalAck_true : D_bstunLocalAck_false);
   bstunGroupEntryData.bstunGroupUnroutableTransmit = gpi->groupUnroutableTx;
   bstunGroupEntryData.bstunGroupUnroutableReceive = gpi->groupUnroutableRx;
   return(&bstunGroupEntryData);
}

/*---------------------------------------------------------------*/ 
/*k_bstunPortEntry_get:                                          */
/*---------------------------------------------------------------*/
bstunPortEntry_t *
k_bstunPortEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, int searchType, long ifIndex)
{
   static bstunPortEntry_t bstunPortEntryData;
   hwidbtype *hwidb; 
   bstunpeertype *stp;

   /*
    * put your code to retrieve the information here
    */
   memset (&bstunPortEntryData, 0, sizeof (bstunPortEntry_t));
   hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			   snmp_bstun_idb_tester, NULL);
   if (hwidb == NULL)
        return NULL;

   bstunPortEntryData.bstunPortGroupNumber = hwidb->bstun_group;

   if ((stp = find_stp_all_addr(hwidb)) != 0) {
       /*---- route all ..  must have been configured */
       switch (stp->type) {
           case BSTUN_PT_IF: 
               bstunPortEntryData.bstunPortDefaultPeerType = 
                                                   D_bstunPortDefaultPeerType_serial;
               bstunPortEntryData.bstunPortDefaultPeerSerial= stp->outidb->snmp_if_index;
               break;
           case BSTUN_PT_NONE: 
               bstunPortEntryData.bstunPortDefaultPeerType = D_bstunPortDefaultPeerType_none;
               break;
           case BSTUN_PT_TCPD:
           default:
               bstunPortEntryData.bstunPortDefaultPeerType = D_bstunPortDefaultPeerType_ip;
               bstunPortEntryData.bstunPortDefaultPeerIP = stp->ipaddr;
               break;
       }
   }
   else
       bstunPortEntryData.bstunPortDefaultPeerType = D_bstunPortDefaultPeerType_none;

   bstunPortEntryData.ifIndex = hwidb->snmp_if_index;
   return(&bstunPortEntryData);
}

/*---------------------------------------------------------------*/ 
/*k_bstunRouteEntry_get:                                         */
/*---------------------------------------------------------------*/
bstunRouteEntry_t *
k_bstunRouteEntry_get(int serialNum, 
                      ContextInfo *contextInfo, 
                      int nominator, 
                      int searchType, 
                      long bstunRouteGroupIndex, 
                      long bstunRouteStationAddress)
{
   static bstunRouteEntry_t bstunRouteEntryData;
   hwidbtype *hwidb;
   bstunpeertype *stp;
   /*
    * put your code to retrieve the information here
    */
   memset (&bstunRouteEntryData, 0, sizeof (bstunRouteEntry_t));
   if ((stp = snmp_stp_get(searchType, bstunRouteGroupIndex, bstunRouteStationAddress, &hwidb)) == NULL) 
        return NULL;

   bstunRouteEntryData.bstunRouteGroupIndex = hwidb->bstun_group;  

   if (stp->all_addr)
       /*---- All unrecognized addresses - default address */
       bstunRouteEntryData.bstunRouteStationAddress = 256; 
   else 
       /*---- BSC protocol has address length of 1. */
       bstunRouteEntryData.bstunRouteStationAddress = (long)stp->addr[0];
     
   switch (stp->type) {
       case BSTUN_PT_IF: 
           bstunRouteEntryData.bstunRouteType = D_bstunRouteType_serial;
           bstunRouteEntryData.bstunRouteSerial = stp->outidb->snmp_if_index;
           break;
       case BSTUN_PT_NONE: 
           bstunRouteEntryData.bstunRouteType = D_bstunRouteType_none;
           break;
       case BSTUN_PT_TCPD:
       default:
           bstunRouteEntryData.bstunRouteType = D_bstunRouteType_ip;
           bstunRouteEntryData.bstunRouteIP = stp->ipaddr;
           break;
   }

   if (!stp->priority_top) { 
        bstunRouteEntryData.bstunRoutePriority = D_bstunRoutePriority_high; /*---- Port 0 */
   } else {
        if (stp->priority){
	    if (stp == stp->priority_top->priority_peers[1])                /*---- Port 1 */
                bstunRouteEntryData.bstunRoutePriority = D_bstunRoutePriority_medium;
            else if (stp == stp->priority_top->priority_peers[2])           /*---- Port 2 */
                bstunRouteEntryData.bstunRoutePriority = D_bstunRoutePriority_normal;
	    else if (stp == stp->priority_top->priority_peers[3])           /*---- Port 3 */
                bstunRouteEntryData.bstunRoutePriority = D_bstunRoutePriority_low;
        }
   }

   switch (stp->state) {
       case BSTUN_PS_DEAD:
           bstunRouteEntryData.bstunRoutePeerState = D_bstunRoutePeerState_dead;
           break;
       case BSTUN_PS_CLOSED:
           bstunRouteEntryData.bstunRoutePeerState = D_bstunRoutePeerState_closed;
           break;
       case BSTUN_PS_OPENING:
           bstunRouteEntryData.bstunRoutePeerState = D_bstunRoutePeerState_opening;
           break;
       case BSTUN_PS_OPENWAIT:
           bstunRouteEntryData.bstunRoutePeerState = D_bstunRoutePeerState_openWait;
           break;
       case BSTUN_PS_CONNECTED:
           bstunRouteEntryData.bstunRoutePeerState = D_bstunRoutePeerState_connected;
           break;
   }

   bstunRouteEntryData.bstunRouteRxPackets = stp->pkts_rx;
   bstunRouteEntryData.bstunRouteTxPackets = stp->pkts_tx;
   bstunRouteEntryData.bstunRouteRxBytes = stp->bytes_rx;
   bstunRouteEntryData.bstunRouteTxBytes = stp->bytes_tx;
   return(&bstunRouteEntryData);

}



static void
init_bstunmib (subsystype* subsys)
{
    load_mib(bstunmib_OidList, bstunmib_OidListNum);
    load_oid(bstunmib_oid_table);
}

/*
 * bstunmib subsystem header
 */


#define MAJVERSION_bstunmib 1
#define MINVERSION_bstunmib 0
#define EDITVERSION_bstunmib 1

SUBSYS_HEADER(bstunmib,
              MAJVERSION_bstunmib,
              MINVERSION_bstunmib,
              EDITVERSION_bstunmib,
              init_bstunmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);






      
