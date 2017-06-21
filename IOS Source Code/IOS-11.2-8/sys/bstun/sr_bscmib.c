/* $Id: sr_bscmib.c,v 3.5.8.4 1996/06/17 08:30:16 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/bstun/sr_bscmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Silvana Zdravkovski (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bscmib.c,v $
 * Revision 3.5.8.4  1996/06/17  08:30:16  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.5.8.3  1996/05/21  09:43:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.8.2  1996/05/21  06:24:14  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.8.1  1996/03/18  19:01:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:29:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/13  01:12:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/02/20  00:20:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/27  20:05:34  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/24  22:07:38  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1996/01/18  15:43:58  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.2  1995/11/17  08:49:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/11  08:22:01  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.4  1995/07/31  08:49:19  arothwel
 * CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
 * same.
 *
 * Revision 2.3  1995/07/17  07:30:56  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/16  06:24:53  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../h/interface_private.h"

#include "../snmp/snmp_api.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "sr_bscmib.h"
#include "sr_bscmib-mib.h"
#include "bsc.h"
#include "bsc_lcb.h"
#include "bsc_lack_support.h"
#include "bsc_lack_fsm.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the bscPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bscPortEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    bscPortEntry_t  *data=0;
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
     if ( (arg == -1) || (data = k_bscPortEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_bscPortRole
    case I_bscPortRole:
       dp = (void *) (&data->bscPortRole);
       break;
#endif /* I_bscPortRole */

#ifdef I_bscPortCodeSet
    case I_bscPortCodeSet:
       dp = (void *) (&data->bscPortCodeSet);
       break;
#endif /* I_bscPortCodeSet */

#ifdef I_bscPortPause
    case I_bscPortPause:
       dp = (void *) (&data->bscPortPause);
       break;
#endif /* I_bscPortPause */

#ifdef I_bscPortServlim
    case I_bscPortServlim:
       dp = (void *) (&data->bscPortServlim);
       break;
#endif /* I_bscPortServlim */

#ifdef I_bscPortPollTimeout
    case I_bscPortPollTimeout:
       dp = (void *) (&data->bscPortPollTimeout);
       break;
#endif /* I_bscPortPollTimeout */

#ifdef I_bscPortRecoveryRetries
    case I_bscPortRecoveryRetries:
       dp = (void *) (&data->bscPortRecoveryRetries);
       break;
#endif /* I_bscPortRecoveryRetries */

#ifdef I_bscPortUnknownControlUnitsReceived
    case I_bscPortUnknownControlUnitsReceived:
       dp = (void *) (&data->bscPortUnknownControlUnitsReceived);
       break;
#endif /* I_bscPortUnknownControlUnitsReceived */

#ifdef I_bscPortSoftErrors
    case I_bscPortSoftErrors:
       dp = (void *) (&data->bscPortSoftErrors);
       break;
#endif /* I_bscPortSoftErrors */

#ifdef I_bscPortHardErrors
    case I_bscPortHardErrors:
       dp = (void *) (&data->bscPortHardErrors);
       break;
#endif /* I_bscPortHardErrors */

#ifdef I_bscPortProtocolViolations
    case I_bscPortProtocolViolations:
       dp = (void *) (&data->bscPortProtocolViolations);
       break;
#endif /* I_bscPortProtocolViolations */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bscCUEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
bscCUEntry_get(OID *incoming, ObjectInfo *object, int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    bscCUEntry_t    *data=0;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            bscCUAddress;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &bscCUAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        bscCUAddress = MAX(1, bscCUAddress);
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
     if ( (arg == -1) || (data = k_bscCUEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, bscCUAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->bscCUAddress;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_bscCUState
    case I_bscCUState:
       dp = (void *) (&data->bscCUState);
       break;
#endif /* I_bscCUState */

#ifdef I_bscCUBytesSent
    case I_bscCUBytesSent:
       dp = (void *) (&data->bscCUBytesSent);
       break;
#endif /* I_bscCUBytesSent */

#ifdef I_bscCUBytesReceived
    case I_bscCUBytesReceived:
       dp = (void *) (&data->bscCUBytesReceived);
       break;
#endif /* I_bscCUBytesReceived */

#ifdef I_bscCUTotalFramesSent
    case I_bscCUTotalFramesSent:
       dp = (void *) (&data->bscCUTotalFramesSent);
       break;
#endif /* I_bscCUTotalFramesSent */

#ifdef I_bscCUTotalFramesReceived
    case I_bscCUTotalFramesReceived:
       dp = (void *) (&data->bscCUTotalFramesReceived);
       break;
#endif /* I_bscCUTotalFramesReceived */

#ifdef I_bscCUDataFramesSent
    case I_bscCUDataFramesSent:
       dp = (void *) (&data->bscCUDataFramesSent);
       break;
#endif /* I_bscCUDataFramesSent */

#ifdef I_bscCUDataFramesReceived
    case I_bscCUDataFramesReceived:
       dp = (void *) (&data->bscCUDataFramesReceived);
       break;
#endif /* I_bscCUDataFramesReceived */

#ifdef I_bscCUSoftErrors
    case I_bscCUSoftErrors:
       dp = (void *) (&data->bscCUSoftErrors);
       break;
#endif /* I_bscCUSoftErrors */

#ifdef I_bscCUHardErrors
    case I_bscCUHardErrors:
       dp = (void *) (&data->bscCUHardErrors);
       break;
#endif /* I_bscCUHardErrors */

#ifdef I_bscCUProtocolViolations
    case I_bscCUProtocolViolations:
       dp = (void *) (&data->bscCUProtocolViolations);
       break;
#endif /* I_bscCUProtocolViolations */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */


/*---------------------------------------------------------------------*/
/* GetNextCucb:  */                       
/*---------------------------------------------------------------------*/
static cucb_t* GetNextCucb (hwidbtype *idb, int addr)
{
   cucb_t* bestcucb = 0;
   cucb_t* cucb = 0;
   long bestaddr = 255;   


   if (idb->lcb != 0){ 
       for (cucb = idb->lcb->cucb_list.head; cucb; cucb = cucb->next){
           if (addr <= cucb->address){
               if (bestaddr > cucb->address){
                   bestaddr = cucb->address;
                   bestcucb = cucb;
	       }
	    }
        }
    }
    return bestcucb;
}

/*---------------------------------------------------------------------*/
/* snmp_bsc_idb_tester: Is the BSC protocol configured on this hwidb ? */                       
/*---------------------------------------------------------------------*/
static boolean
snmp_bsc_idb_tester(hwidbtype* idb, long* index2)
{
    return (is_bstun(idb) && (idb->poll_protocol_flags & PP_ACTIVE));
}

/*---------------------------------------------------------------------*/
/* snmp_bsc_idb_and_cucb_tester:                                       */                       
/*---------------------------------------------------------------------*/
static boolean
snmp_bsc_idb_and_cucb_tester(hwidbtype* idb, long* index2)
{
    if (is_bstun(idb) && (idb->poll_protocol_flags & PP_ACTIVE)) {
        cucb_t *cucb = GetNextCucb(idb, *index2);
        if (cucb != NULL ){
            *index2 = cucb->address;
            return TRUE;
        }
    }
    return FALSE;
}

/*-------------------------------------------------------------------------*/ 
/* snmp_cucb_get: Retrieve the correct CUCB given ifIndex and bscCUAddress */
/*-------------------------------------------------------------------------*/ 

static cucb_t*
snmp_cucb_get(int searchType,
	      long ifIndex,
	      long bscCUAddress,
	      hwidbtype **hwidbP)
{
    hwidbtype* hwidb;

    hwidb = ifmib_2indices_get(0, searchType, ifIndex,
			       snmp_bsc_idb_and_cucb_tester,
			       &bscCUAddress);
    *hwidbP = hwidb;
    if (hwidb != NULL){
       return GetNextCucb(hwidb, bscCUAddress);
    }
    return NULL;

}  /* snmp_cucb_get. */



/*-----------------------------------------------------------------*/ 
/* TotalErrs: Find all errors on this port and associated CUCBS */
/*-----------------------------------------------------------------*/ 
static long TotalErrs(lcb_t *lcb, int err){
    
   cucb_t* cucb;
   long counter = 0;

   /*---- CUCB errors */
   cucb = lcb->cucb_list.head;
   while (cucb){
       counter += cucb->rx_frame_ct.ctr[err];
       cucb = cucb->next;
   }
   
   /*---- Port errors */
   counter += lcb->rx_frame_ct.ctr[err];

   return counter;
}  


/*----------------------------------------------*/ 
/* k_bscPortEntry_get:                          */
/*----------------------------------------------*/ 

bscPortEntry_t *
k_bscPortEntry_get(int serialNum,
		   ContextInfo *contextInfo,
		   int nominator,
		   int searchType,
		   long ifIndex)
{
   static bscPortEntry_t bscPortEntryData;
   hwidbtype* hwidb;

   /*
    * put your code to retrieve the information here
    */
   memset (&bscPortEntryData, 0, sizeof (bscPortEntry_t)); 
   hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
			   snmp_bsc_idb_tester, NULL);
   if (!hwidb)
      return NULL;
   
   switch (hwidb->lcb->char_set){
       case CS_EBCDIC:
           bscPortEntryData.bscPortCodeSet = D_bscPortCodeSet_ebcdic;
           break;
       case CS_ASCII:
           bscPortEntryData.bscPortCodeSet = D_bscPortCodeSet_ascii;
           break;
   }

   if (bsc_query_local_ack_capable(hwidb)) {
       if (hwidb->lcb->poll_role == PR_POLLER){
           bscPortEntryData.bscPortServlim = hwidb->lcb->servlim;                /*---- in tenths of a sec */
           bscPortEntryData.bscPortPollTimeout = hwidb->lcb->io_timer_table.timer[ToPoll] / 100; /*---- msec -> tenths of a sec */
       }

       bscPortEntryData.bscPortPause = hwidb->lcb->pause_timeout / 100;          /*---- msec -> tenths of a sec */ 
       bscPortEntryData.bscPortRecoveryRetries = hwidb->lcb->poll_retries; 

       /*---- CUCB (control unit control block) only exist for local ack */
       bscPortEntryData.bscPortUnknownControlUnitsReceived = hwidb->lcb->unknownCUCBAddrRx;
   }

   /*---- Note the following is not a mistake. When "bsc primary" is configured, the poll
              protocol role is SECONDARY and vice versa. See bsc_conf.c */
   switch (hwidb->lcb->poll_role){ 
       case PR_PRIMARY:
       case PR_POLLEE:
           bscPortEntryData.bscPortRole = D_bscPortRole_secondary;
           break;
       case PR_SECONDARY:
       case PR_POLLER:
           bscPortEntryData.bscPortRole = D_bscPortRole_primary;
           break;
       case PR_CONTENTION:
           bscPortEntryData.bscPortRole = D_bscPortRole_contention;
           break;
       default: break;
   }

   bscPortEntryData.bscPortSoftErrors = TotalErrs(hwidb->lcb, CtSoftErr);  
   bscPortEntryData.bscPortHardErrors = TotalErrs(hwidb->lcb, CtHardErr);
   bscPortEntryData.bscPortProtocolViolations = TotalErrs(hwidb->lcb, CtProtErr);
   bscPortEntryData.ifIndex = hwidb->snmp_if_index;
   return(&bscPortEntryData);

}

/*----------------------------------------------*/ 
/* k_bscCUEntry_get:                            */
/*----------------------------------------------*/ 
bscCUEntry_t *
k_bscCUEntry_get(int serialNum, ContextInfo *contextInfo, int nominator, int searchType, long ifIndex, long bscCUAddress)
{
   static bscCUEntry_t bscCUEntryData;
   hwidbtype *hwidb;
   cucb_t* cucb;

   /*
    * put your code to retrieve the information here
    */
   memset (&bscCUEntryData, 0, sizeof (bscCUEntry_t));
   if ((cucb = snmp_cucb_get(searchType, ifIndex, bscCUAddress, &hwidb)) == NULL) 
        return NULL;

   bscCUEntryData.bscCUAddress = cucb->address;
   if (cucb->state == CU_ACTIVE)
       bscCUEntryData.bscCUState = D_bscCUState_active;
   else
       bscCUEntryData.bscCUState = D_bscCUState_inactive;

   bscCUEntryData.bscCUBytesSent = cucb->tx_frame_ct.ctr[CtBytes];
   bscCUEntryData.bscCUBytesReceived = cucb->rx_frame_ct.ctr[CtBytes];
   
   bscCUEntryData.bscCUTotalFramesSent =
       cucb->tx_frame_ct.ctr[CtDataFrames] + cucb->tx_frame_ct.ctr[CtCtrlFrames];
   bscCUEntryData.bscCUTotalFramesReceived = 
       cucb->rx_frame_ct.ctr[CtDataFrames] + cucb->rx_frame_ct.ctr[CtCtrlFrames];

   bscCUEntryData.bscCUDataFramesSent = cucb->tx_frame_ct.ctr[CtDataFrames];
   bscCUEntryData.bscCUDataFramesReceived = cucb->rx_frame_ct.ctr[CtDataFrames];

   bscCUEntryData.bscCUSoftErrors = cucb->rx_frame_ct.ctr[CtSoftErr];
   bscCUEntryData.bscCUHardErrors = cucb->rx_frame_ct.ctr[CtHardErr];
   bscCUEntryData.bscCUProtocolViolations = cucb->rx_frame_ct.ctr[CtProtErr];
   bscCUEntryData.ifIndex = hwidb->snmp_if_index;
   return(&bscCUEntryData);
}



static void
init_bscmib (subsystype* subsys)
{
    load_mib(bscmib_OidList, bscmib_OidListNum);
    load_oid(bscmib_oid_table);
}

/*
 * bscmib subsystem header
 */


#define MAJVERSION_bscmib 1
#define MINVERSION_bscmib 0
#define EDITVERSION_bscmib 1

SUBSYS_HEADER(bscmib,
              MAJVERSION_bscmib,
              MINVERSION_bscmib,
              EDITVERSION_bscmib,
              init_bscmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
