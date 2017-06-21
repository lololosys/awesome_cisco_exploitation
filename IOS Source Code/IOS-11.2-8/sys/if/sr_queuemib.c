/* $Id: sr_queuemib.c,v 3.5.8.5 1996/05/21 09:50:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/sr_queuemib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1995, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_queuemib.c,v $
 * Revision 3.5.8.5  1996/05/21  09:50:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.8.4  1996/05/21  06:31:21  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.8.3  1996/05/17  11:23:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.14.4  1996/05/05  23:10:12  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.14.3  1996/04/26  07:55:30  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.14.2  1996/04/03  14:39:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.14.1  1996/03/17  17:51:30  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5.8.2  1996/04/17  13:40:30  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.5.8.1  1996/03/18  20:14:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  06:56:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/13  01:35:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5  1996/02/29  16:17:08  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  23:01:02  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  09:31:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:03:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/10  21:04:49  fred
 * CSCdi40186:  queue mib crash on RSP
 *              return zeroes for interfaces that have WFQ as queue type
 *              (set at system startup) but no configuration that set up
 *              the queue head (such as 'no shutdown'
 *
 * Revision 2.4  1995/07/17  07:32:18  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.3  1995/06/28  09:26:26  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/23  16:39:20  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
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
 *
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_queuemib.h"
#include "sr_queuemib-mib.h"

#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "interface.h"
#include "packet.h"
#include "address.h"
#include "network.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "priority_private.h"
#include "../parser/parser_defs_priority.h"
#include "../if/network_debug.h"
#include "../ip/ip_registry.h"
#include "../snmp/sr_ifmib.h"
#include "../snmp/ifmibapi.h"

/*
 * Start of system-dependent routines
 */
static hwidbtype *hwidb = NULL;
static long old_ifIndex = -1;

/*
 * k_cQIfEntry_get
 *
 * obtain information for the general queuing algorithms in an interface
 */
cQIfEntry_t *
k_cQIfEntry_get(int serialNum, ContextInfo * contextInfo,
		 int nominator, int searchType, long ifIndex)
{
    static cQIfEntry_t cQIfEntryData;
    
    /* retrieve hw idb */
    if (!hwidb || old_ifIndex != ifIndex) {
	hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
				NULL, NULL);
	if (!hwidb) {
	    hwidb = NULL;
	    old_ifIndex = -1;
	    return(NULL);
	}
	old_ifIndex = ifIndex;
    }
    /* retrieve information */
    
    cQIfEntryData.cQIfTxLimit = hwidb->mci_txcount;
    cQIfEntryData.ifIndex = hwidb->snmp_if_index;
    switch (priority_type(hwidb)) {
      case QUEUE_FCFS:
      case QUEUE_RED:
	cQIfEntryData.cQIfQType = D_cQIfQType_fifo;
	cQIfEntryData.cQIfSubqueues = 1;
	break;
      case QUEUE_PRIORITY:
	cQIfEntryData.cQIfQType = D_cQIfQType_priority;
	cQIfEntryData.cQIfSubqueues = 4;
	break;
      case QUEUE_CUSTOM:
	cQIfEntryData.cQIfQType = D_cQIfQType_custom;
	cQIfEntryData.cQIfSubqueues = PRIORITY_MAXTYPE;
	break;
      case QUEUE_FAIR_DEF:	
      case QUEUE_FAIR:
	cQIfEntryData.cQIfQType = D_cQIfQType_weightedFair;
	cQIfEntryData.cQIfSubqueues = 
	    hwidb->fair_queue_head->fq_queue_count;
	break;
    } 
    return(&cQIfEntryData);
}

/*
 * get_next_idb_and_queue
 */
static long get_idb_and_queue (int serialNum, ContextInfo * contextInfo,
	   int nominator, int searchType,
	   boolean (*queue_number_test)(hwidbtype * hwidb, int queueNumber),
	   long ifIndex, long cQStatsQNumber)
{
    /* retrieve hw idb */
try_again:
    if (!hwidb || old_ifIndex != ifIndex) {
	/* need an hwidb because we haven't got one */
	hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex,
				NULL, NULL);
	if (!hwidb) {
	    hwidb = NULL;
	    old_ifIndex = -1;
	    return(0);
	}
	old_ifIndex = hwidb->snmp_if_index;
    }
    /* if NEXT hwidb is returned, reset queue number */
    if (ifIndex != hwidb->snmp_if_index)
	cQStatsQNumber = 0;
    /* having decided on an IDB, is the queue number valid? */
    if (!queue_number_test(hwidb, cQStatsQNumber)) {
	/* need another hwidb because we ran out of sub-queues */
	cQStatsQNumber = 0;
	ifIndex++;
	if (searchType == NEXT) 
	    goto try_again;	/* as opposed to recursive call */
	hwidb = NULL;
	old_ifIndex = -1;
    }
    return cQStatsQNumber;
}

/*
 * typed_queue_number_test
 *
 * used by snmp_idb_get on behalf of k_cQStatsEntry_get to compare the
 * queue number to the number of sub-queues available on an interface.
 */
static boolean 
typed_queue_number_test(hwidbtype * hwidb, int queueNumber)
{
    switch (priority_type(hwidb)) {
      case QUEUE_FCFS:
    case QUEUE_RED:
	return(queueNumber == 0 || queueNumber == PRIORITY_NORMAL);
      case QUEUE_PRIORITY:
	return(0 <= queueNumber && queueNumber < 4);
      case QUEUE_CUSTOM:
	return(0 <= queueNumber && queueNumber < PRIORITY_MAXTYPE);
      case QUEUE_FAIR_DEF:	
      case QUEUE_FAIR:
	return(0 <= queueNumber && 
	       queueNumber < hwidb->fair_queue_head->fq_queue_count);
    } 
    return(FALSE);
}				
/*
 * k_cQStatsEntry_get
 *
 * return information about individual sub-queues
 */
cQStatsEntry_t *
k_cQStatsEntry_get(int serialNum, ContextInfo * contextInfo,
	   int nominator, int searchType, long ifIndex, long cQStatsQNumber)
{
    fair_queue_head *fq;
    queuetype *queue;
    fair_queue *fqueue;
    static cQStatsEntry_t cQStatsEntryData;
    
    cQStatsQNumber =  get_idb_and_queue (serialNum, contextInfo, nominator,
					 searchType, typed_queue_number_test, 
					 ifIndex, cQStatsQNumber);
    if (!hwidb)
	return NULL;

    /* retrieve information */
    
    cQStatsEntryData.ifIndex = hwidb->snmp_if_index;
    switch (priority_type(hwidb)) {
    case QUEUE_FCFS:
    case QUEUE_RED:
	/* for FCFS, queue number is the "normal" queue */
	cQStatsQNumber = PRIORITY_NORMAL;
    case QUEUE_PRIORITY:
    case QUEUE_CUSTOM:
	cQStatsEntryData.cQStatsQNumber = cQStatsQNumber;
	queue = &hwidb->outputq[cQStatsQNumber];
	cQStatsEntryData.cQStatsDepth = queue->count;
	cQStatsEntryData.cQStatsMaxDepth = queue->maximum;
	cQStatsEntryData.cQStatsDiscards =
	    hwidb->counters.output_drops[cQStatsQNumber];
	break;
    case QUEUE_FAIR_DEF:
    case QUEUE_FAIR:
	fq = hwidb->fair_queue_head;
	if (!fq) {
	    /*
	     * uninitialized interface
	     */
	    cQStatsEntryData.cQStatsQNumber = cQStatsQNumber;
	    cQStatsEntryData.cQStatsDepth = 0;
	    cQStatsEntryData.cQStatsMaxDepth = 0;
	    cQStatsEntryData.cQStatsDiscards = 0;
	} else {
	    fqueue = fair_queue_address(fq,cQStatsQNumber);
	    /* return numbers */
	    cQStatsEntryData.cQStatsQNumber = cQStatsQNumber;
	    cQStatsEntryData.cQStatsDepth = pak_queue_depth(&fqueue->q);
	    cQStatsEntryData.cQStatsMaxDepth = pak_queue_max_depth(&fqueue->q);
	    cQStatsEntryData.cQStatsDiscards = fqueue->fq_discards;
	}
	break;
    } 
    return(&cQStatsEntryData);
}

/*
 * queue_custom_test
 *
 * used by snmp_hwidb_get on behalf of k_cQRotationEntry_get to select
 * interfaces with custom queuing enabled and a valid queue number
 */
static boolean
queue_custom_test(hwidbtype * hwidb, int queueNumber)
{
    if (priority_type(hwidb) == QUEUE_CUSTOM) {
	return(queueNumber < PRIORITY_MAXTYPE);
    } 
    return(FALSE);
}	
/*
 * k_cQRotationEntry_get
 *
 * return the octet rotation information for an interface
 */
cQRotationEntry_t *
k_cQRotationEntry_get(int serialNum, ContextInfo * contextInfo,
		       int nominator, int searchType, 
		       long ifIndex, long cQStatsQNumber)
{
    static cQRotationEntry_t cQRotationEntryData;
    
    cQStatsQNumber =  get_idb_and_queue (serialNum, contextInfo, nominator,
		searchType, queue_custom_test, ifIndex, cQStatsQNumber);
    if (!hwidb)
	return NULL;
    
    /* retrieve information */
    
    cQRotationEntryData.cQStatsQNumber = cQStatsQNumber;
    cQRotationEntryData.ifIndex = hwidb->snmp_if_index;
    
    /*
     * put your code to retrieve the information here
     */
    
    cQRotationEntryData.cQRotationOctets =
	custom_qbytecnt(hwidb, cQStatsQNumber);
    cQRotationEntryData.ifIndex = hwidb->snmp_if_index;
    cQRotationEntryData.cQStatsQNumber = cQStatsQNumber;
    return(&cQRotationEntryData);
}

/*
 * Start of system-independent routines
 */

/*---------------------------------------------------------------------
 * Retrieve data from the cQIfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cQIfEntry_get(OID *incoming, ObjectInfo *object,
	       int searchType, ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cQIfEntry_t *data = NULL;
    unsigned long buffer[1];
    OID inst;
    int carry;
    long ifIndex;

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
	} carry = 0;
    } else {
	carry = 1;
    } 
    if ((InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) ||
	(data = k_cQIfEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
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

#ifdef I_cQIfQType
      case I_cQIfQType:
	dp = (void *) (&data->cQIfQType);
	break;
#endif /* I_cQIfQType */
	
#ifdef I_cQIfTxLimit
      case I_cQIfTxLimit:
	dp = (void *) (&data->cQIfTxLimit);
	break;
#endif /* I_cQIfTxLimit */
	
#ifdef I_cQIfSubqueues
      case I_cQIfSubqueues:
	dp = (void *) (&data->cQIfSubqueues);
	break;
#endif /* I_cQIfSubqueues */
	
      default:
	return((VarBind *) NULL);
	
    } /* switch */
    return(MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cQStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cQStatsEntry_get(OID *incoming, ObjectInfo *object,
     int searchType, ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cQStatsEntry_t *data = NULL;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long ifIndex;
    long cQStatsQNumber;
    
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
	} carry = 0;
    } else {
	carry = 1;
    } 
    if ((InstToInt(incoming, 1 + object->oid.length, &cQStatsQNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) ||
	(data = k_cQStatsEntry_get(serialNum, contextInfo, arg, searchType, ifIndex, cQStatsQNumber)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->ifIndex;
	inst.oid_ptr[1] = data->cQStatsQNumber;
    }				
    /*
     * Build the variable binding for the variable that will be returned.
     */
    switch (arg) {

#ifdef I_cQStatsDepth
      case I_cQStatsDepth:
	dp = (void *) (&data->cQStatsDepth);
	break;
#endif /* I_cQStatsDepth */
	
#ifdef I_cQStatsMaxDepth
      case I_cQStatsMaxDepth:
	dp = (void *) (&data->cQStatsMaxDepth);
	break;
#endif /* I_cQStatsMaxDepth */
	
#ifdef I_cQStatsDiscards
      case I_cQStatsDiscards:
	dp = (void *) (&data->cQStatsDiscards);
	break;
#endif /* I_cQStatsDiscards */
	
      default:
	return((VarBind *) NULL);
	
    } /* switch */
    return(MakeVarBind(object, &inst, dp));
}

/*---------------------------------------------------------------------
 * Retrieve data from the cQRotationEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/ 
VarBind *
cQRotationEntry_get(OID *incoming, ObjectInfo *object,
		     int searchType, ContextInfo *contextInfo, int serialNum)
{
    int instLength = incoming->length - object->oid.length;
    int arg = object->nominator;
    void *dp;
    cQRotationEntry_t *data = NULL;
    unsigned long buffer[2];
    OID inst;
    int carry;
    long ifIndex;
    long cQStatsQNumber;

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
	} carry = 0;
    }
    else {
	carry = 1;
    } 
    if ((InstToInt(incoming, 1 + object->oid.length, &cQStatsQNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) ||
	(data = k_cQRotationEntry_get(serialNum, contextInfo, arg, searchType, ifIndex, cQStatsQNumber)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->ifIndex;
	inst.oid_ptr[1] = data->cQStatsQNumber;
    }		
    /*
     * Build the variable binding for the variable that will be returned.
     */
    switch(arg) {

#ifdef I_cQRotationOctets
      case I_cQRotationOctets:
	dp = (void *) (&data->cQRotationOctets);
	break;
#endif /* I_cQRotationOctets */
	
      default:
	return((VarBind *) NULL);
	
    } /* switch */
    return(MakeVarBind(object, &inst, dp));
}

/*
 * initialize the queue object acquisition
 */ 
static void init_queuemib (subsystype *subsys)
{
    load_mib(queuemib_OidList, queuemib_OidListNum);
    load_oid(queuemib_oid_table);
}	
/*
 *
 * queuemib subsystem header
 */

/* WARNING: update "req" with the subsystem being monitored */

#define MAJVERSION_queuemib 1
#define MINVERSION_queuemib 0
#define EDITVERSION_queuemib 0 
SUBSYS_HEADER	(queuemib,
		MAJVERSION_queuemib,
		MINVERSION_queuemib,
		EDITVERSION_queuemib,
		init_queuemib,
		SUBSYS_CLASS_MANAGEMENT,
		NULL, NULL);
      
