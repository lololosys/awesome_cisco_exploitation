/* $Id: sr_ethermib.c,v 3.7.10.5 1996/05/21 10:02:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_ethermib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ethermib.c,v $
 * Revision 3.7.10.5  1996/05/21  10:02:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.4  1996/05/21  06:38:40  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.7.10.3  1996/04/22  03:35:18  rnardin
 * CSCdi53602:  SNMP nits on new platform
 * Branch: California_branch
 *
 * Revision 3.7.10.2  1996/04/04  05:35:29  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.7.10.1  1996/03/18  21:48:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/16  07:30:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.1  1996/03/13  01:59:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7  1996/02/26  19:55:03  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.6  1996/01/18  15:52:23  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.5  1995/12/20  23:46:30  gstovall
 * CSCdi45909:  Item->handled field tested while uninitialized
 * Make sure that item->handled is initialized.  Bad programmer, no pizza.
 *
 * Revision 3.4  1995/12/14  08:27:32  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  18:59:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:16:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/17  07:33:50  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.3  1995/06/28  10:21:53  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.2  1995/06/21  03:14:23  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:49:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmibapi.h"
#include "sr_ethermib.h"
#include "sr_ethermib-mib.h"
#include "../if/ether.h"
#include "../util/itemlist.h"

void
init_ethermib (subsystype *subsys)
{
    load_mib(ethermib_OidList, ethermib_OidListNum);
    load_oid(ethermib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the dot3StatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
dot3StatsEntry_get(
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    dot3StatsEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dot3StatsIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &dot3StatsIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if (arg != -1) {
        data = k_dot3StatsEntry_get(serialNum, contextInfo, arg,
                                    searchType, dot3StatsIndex);
        if (data == NULL) {
            arg = -1;
        }
    }

    if (arg != -1) {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->dot3StatsIndex;
       /*
        * Build the the variable binding for the variable that will be returned.
        */
        return (MakeVarBind(object, &inst, SR_OFFSET(data, arg)));
    }

    return ((VarBind *) NULL);

}

/*---------------------------------------------------------------------
 * Retrieve data from the dot3CollEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/

#ifdef NO_COLLTABLE

VarBind *
dot3CollEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    dot3CollEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dot3CollIndex;
    long            dot3CollCount;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &dot3CollCount, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &dot3CollIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if (arg != -1) { 
         data = k_dot3CollEntry_get(serialNum, contextInfo, arg, searchType, 
                                     dot3CollIndex, dot3CollCount);
         if (data  == NULL) {
             arg = -1;
         }
     }

     if (arg != -1) {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->dot3CollIndex;
        inst.oid_ptr[1] = data->dot3CollCount;
     }

     /*
      * Build the the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_dot3CollIndex
    case I_dot3CollIndex:
       dp = &data->dot3CollIndex;
       break;
#endif /* I_dot3CollIndex */

#ifdef I_dot3CollCount
    case I_dot3CollCount:
       dp = &data->dot3CollCount;
       break;
#endif /* I_dot3CollCount */

#ifdef I_dot3CollFrequencies
    case I_dot3CollFrequencies:
       dp = &data->dot3CollFrequencies;
       break;
#endif /* I_dot3CollFrequencies */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#endif




static boolean idb_ether_test(snmpidbtype *snmpidb, long* index2)
{
    hwidbtype *hwidb;
    iabtype *iab;

    if (snmpidb == NULL)
	return(FALSE);

    switch (snmpidb->snmp_if_struct) {
      case SNMP_IF_HWIDB:
	hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb;
	if (hwidb != NULL) {
	    if (hwidb->status & IDB_ETHER) {
		return(TRUE);
	    } 
	}
	return(FALSE);

      case SNMP_IF_IAB:
	iab = snmpidb->snmp_if_ptr.ifptr.iab;
	if (iab != NULL) {
	    if (iab->status & IDB_ETHER) {
		return(TRUE);
	    }
	} 
	return(FALSE);

      default:
	return(FALSE);
    }
}

/*
 * update_dot3entry_from_hwidb
 * Given snmpidb, hardware idb, and dot3StatsEntry pointers, fill out 
 * the dot3StatsEntry variables as appropriate based on the snmpidb 
 * and the hardware idb.
 */
static void update_dot3entry_from_hwidb(snmpidbtype      *snmpidb,
				 hwidbtype        *hwidb,
				 dot3StatsEntry_t *dot3StatsEntryPtr)
			    
{
    N_ITEM_LIST(tmp, 5);

    item_desc_init(&tmp_item[0], ETHER_HW_OUTPUT_ONE_COLLISION, sizeof(ulong));

    item_desc_init(&tmp_item[1], ETHER_HW_OUTPUT_MORE_COLLISION,
		   sizeof(ulong));

    item_desc_init(&tmp_item[2], ETHER_HW_OUTPUT_DEFERRED, sizeof(ulong));

    item_desc_init(&tmp_item[3], ETHER_HW_OUTPUT_LATE_COLLISION,
		   sizeof(ulong));

    item_desc_init(&tmp_item[4], ETHER_HW_OUTPUT_EXCESSIVE_COLLISION,
		   sizeof(ulong)); 

    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, &tmp_list);

    dot3StatsEntryPtr->dot3StatsIndex = hwidb->snmp_if_index;
    dot3StatsEntryPtr->dot3StatsAlignmentErrors = hwidb->counters.input_frame;
    dot3StatsEntryPtr->dot3StatsFCSErrors = hwidb->counters.input_crc;
    if ((hwidb->type == IDBTYPE_LANCE) || 
	(hwidb->type == IDBTYPE_QUICC_ETHER) ||
	(!is_vip(hwidb) && (hwidb->type == IDBTYPE_AMDP2)) ||
	(!is_vip(hwidb) && (hwidb->type == IDBTYPE_FEIP)))
	dot3StatsEntryPtr->dot3StatsSingleCollisionFrames = 
	    tmp_item[0].u.l_value;
    else
	dot3StatsEntryPtr->dot3StatsSingleCollisionFrames = 
	    hwidb->counters.output_collision;
    dot3StatsEntryPtr->dot3StatsMultipleCollisionFrames =
	tmp_item[1].u.l_value;
    dot3StatsEntryPtr->dot3StatsSQETestErrors = 0;
    dot3StatsEntryPtr->dot3StatsDeferredTransmissions = tmp_item[2].u.l_value;
    dot3StatsEntryPtr->dot3StatsLateCollisions = tmp_item[3].u.l_value;
    dot3StatsEntryPtr->dot3StatsExcessiveCollisions = tmp_item[4].u.l_value;
    dot3StatsEntryPtr->dot3StatsInternalMacTransmitErrors = 0;
    dot3StatsEntryPtr->dot3StatsCarrierSenseErrors
	= hwidb->counters.transitions;
    dot3StatsEntryPtr->dot3StatsFrameTooLongs = hwidb->counters.giants;
    dot3StatsEntryPtr->dot3StatsInternalMacReceiveErrors = 
	hwidb->counters.input_overrun;
}

/*
 * update_dot3entry_from_iab
 * Given snmpidb, iab, and dot3StatsEntry pointers, fill out the
 * dot3StatsEntry variables as appropriate based on the snmpidb and
 * the iab.  Returns TRUE if successful, FALSE on failure.
 */
static boolean update_dot3entry_from_iab(snmpidbtype      *snmpidb,
				  iabtype          *iab,
				  dot3StatsEntry_t *dot3StatsEntryPtr)
			    
{
    /*
     * If this particular iab needs to do something to update the
     * stats in the iab (like a FLEX), then call the routine and let
     * them do it before copying the stats.
     */
    if (!(*iab->get_stats)(iab))
	return(FALSE);

    /* 
     * Okay, iab is up to date, fill out the ifEntry.
     */
    dot3StatsEntryPtr->dot3StatsIndex = snmpidb->snmp_if_index;
    dot3StatsEntryPtr->dot3StatsAlignmentErrors = iab->input_frame;
    dot3StatsEntryPtr->dot3StatsFCSErrors = iab->input_crc;
#if 0 /* This didn't work right anyway */
    if (iab->status == IDB_LANCE)
	dot3StatsEntryPtr->dot3StatsSingleCollisionFrames = 
	    iab->output_one_collision;
    else
#endif
	dot3StatsEntryPtr->dot3StatsSingleCollisionFrames = 
	    iab->output_collision;
    dot3StatsEntryPtr->dot3StatsMultipleCollisionFrames = 
	iab->output_more_collision;
    dot3StatsEntryPtr->dot3StatsSQETestErrors = 0;
    dot3StatsEntryPtr->dot3StatsDeferredTransmissions = 
	iab->output_deferred;
    dot3StatsEntryPtr->dot3StatsLateCollisions = 
	iab->output_late_collision;
    dot3StatsEntryPtr->dot3StatsExcessiveCollisions = 
	iab->output_excessive_collision;
    dot3StatsEntryPtr->dot3StatsInternalMacTransmitErrors = 0;
    dot3StatsEntryPtr->dot3StatsCarrierSenseErrors = iab->transitions;
    dot3StatsEntryPtr->dot3StatsFrameTooLongs = iab->giants;
    dot3StatsEntryPtr->dot3StatsInternalMacReceiveErrors = 
	iab->input_overrun;

    return(TRUE);
}

/*
 * k_dot3StatsEntry_get
 * return the statistics for an ethernet interface.
 */
dot3StatsEntry_t *k_dot3StatsEntry_get (int serialNum,
					ContextInfo *contextInfo,
					int nominator,
					int searchType,
					long dot3StatsIndex)
{
   static snmpidbtype *snmpidb = NULL;
   static iabtype *iab = NULL;
   static hwidbtype *hwidb = NULL;
   static int lastserial = MAXINT;
   static int lastifIndex = MAXINT;
   static dot3StatsEntry_t dot3StatsEntryData;

   /*
    * Don't leave dangling data pointers.
    */
   if (snmpidb == NULL) {
       hwidb = NULL;
       iab = NULL;
   } else {
       if (snmpidb->snmp_if_struct != SNMP_IF_HWIDB) {
	   hwidb = NULL;
       }
       if (snmpidb->snmp_if_struct != SNMP_IF_IAB) {
	   iab = NULL;
       }
   }

   /*
    * See if we got a cache hit.
    */
   if ((serialNum != -1) && (serialNum == lastserial) &&
       (lastifIndex == dot3StatsIndex)) {
       /* hit cache */
       if (snmpidb) {
	   switch (snmpidb->snmp_if_struct) {
	     case SNMP_IF_HWIDB:
	       if (hwidb) {
		   return(&dot3StatsEntryData);
	       } else {
		   return(NULL);
	       }
	       break;
	     case SNMP_IF_IAB:
	       if (iab) {
		   return(&dot3StatsEntryData);
	       } else {
		   return(NULL);
	       }
	     default:
	       return(NULL);
	   }
       } else
           return(NULL);
   }
   
   /* missed cache, store info for next cache hit */
   lastserial = serialNum;
   lastifIndex = dot3StatsIndex;

   snmpidb = ifmib_snmpidb_get(NULL, serialNum, searchType,
			       dot3StatsIndex, idb_ether_test,NULL);
   if (snmpidb == NULL)
       return (NULL);

   switch (snmpidb->snmp_if_struct) {
     case SNMP_IF_HWIDB:
       hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb;
       if (hwidb == NULL) {
	   return(NULL);
       }
       update_dot3entry_from_hwidb(snmpidb, hwidb, &dot3StatsEntryData);
       break;

     case SNMP_IF_IAB:
       iab = snmpidb->snmp_if_ptr.ifptr.iab;
       if (iab == NULL) {
	   return(NULL);
       }
       update_dot3entry_from_iab(snmpidb, iab, &dot3StatsEntryData);
       break;

     default:
       return(NULL);
   } /* switch (snmpidb->snmp_if_struct) */

   return(&dot3StatsEntryData);
}

#ifdef NO_COLLTABLE
dot3CollEntry_t *
k_dot3CollEntry_get(serialNum, contextInfo, nominator, searchType, dot3CollIndex, dot3CollCount)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long dot3CollIndex;
   long dot3CollCount;
{
#ifdef NOT_YET
   static dot3CollEntry_t dot3CollEntryData;

   dot3CollEntryData.dot3CollIndex = ;
   dot3CollEntryData.dot3CollCount = ;
   dot3CollEntryData.dot3CollFrequencies = ;

   return(&dot3CollEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}
#endif



/*
 * ethermib subsystem header
 */


#define MAJVERSION_ethermib 1
#define MINVERSION_ethermib 0
#define EDITVERSION_ethermib 0

SUBSYS_HEADER(ethermib,
              MAJVERSION_ethermib,
              MINVERSION_ethermib,
              EDITVERSION_ethermib,
              init_ethermib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: ether",
              NULL);
      
