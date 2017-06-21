/* $Id: ifmibapi.c,v 3.5.48.10 1996/09/03 23:38:44 ssankara Exp $
 * $Source: /release/112/cvs/Xsys/snmp/ifmibapi.c,v $
 *------------------------------------------------------------------
 * API functions for access to IF-MIB structures.
 *
 * March 1995, Sandra Durham
 *
 * Copyright (c) 1995 - 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ifmibapi.c,v $
 * Revision 3.5.48.10  1996/09/03  23:38:44  ssankara
 * CSCdi65581:  ifmib_if_deregister doesnt clear hwidb->snmp_if_index
 * Branch: California_branch :  Moved  snmpidb_init_common(snmpidb) and
 * free_snmpidb(snmpidb) after the switch to clear the
 * hwidb->snmp_if_index.
 *
 * Revision 3.5.48.9  1996/08/28  13:09:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.48.8  1996/08/13  15:01:14  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Add ifmib_swidb_get() for supporting swidb retrieval by given snmp
 *    ifIndex.
 * Branch: California_branch
 *
 * Revision 3.5.48.7  1996/07/24  18:09:22  sdurham
 * CSCdi61055:  snmp interface count incorrect for systems with sync/async
 * ints
 * Branch: California_branch - count ifNumber on the fly.
 *
 * Revision 3.5.48.6  1996/06/28  23:26:05  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.48.5  1996/06/26  03:08:29  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.5.48.4  1996/05/21  10:02:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.48.3  1996/05/21  06:38:38  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.48.2  1996/05/17  11:42:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.34.2  1996/05/05  23:46:51  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.34.1  1996/04/26  07:57:09  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.48.1  1996/04/30  23:17:31  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.5  1995/12/07  22:32:34  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/20  22:27:31  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:58:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:16:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:48:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * API Functions for IF-MIB Access -
 * The purpose of these functions is to provide external access to the
 * IF-MIB implementation, where needed.  This access provides the
 * ability to internally add and remove elements from each of the
 * tables defined by the IF-MIB, as well as to query the elements in
 * these tables.  These functions will be used by the sub-interface
 * media code to update the IF-MIB appropriately when changes occur to
 * snmp-registered sub-interfaces.
 *
 * NOTE: If your subsystem may be included in an image that does not
 * also contain the snmp subsystem, then you should not be calling
 * these routines directly.  Use the provided IF-MIB registry instead
 * (see snmp/ifmib_registry.reg)
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface.h"
#include "sys_registry.h"
#include "snmp_interface.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "ifmibapi.h"
#include "ifmib_registry.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "interface_private.h"

/*
 * globals
 */

queuetype	snmpifRcvAddrQ;
queuetype	snmpifStackQ;

static long next_snmp_if_index = 1;		/* next snmpidb number */

static queuetype  snmpifQ;		        /* queue of SNMP idbs */
/* static queuetype *snmpidbQ = &snmpifQ; */	        /* pointer to SNMP IDB queue */
/* used by FOR_ALL_SNMPIDBs macro in snmp_interface.h */
queuetype *snmpidbQ = &snmpifQ;
static queuetype  snmpfreeifQ;		        /* freelist of SNMP idbs */
static queuetype *snmpfreeidbQ = &snmpfreeifQ;	/* ptr to SNMP IDB freelist */

/*
 * internal support routines
 */

/*
 * ifmib_find_snmpidb
 *    returns the snmpidb associated with the given ifIndex, or NULL
 *    if no such snmpidb could be found
 */
static inline snmpidbtype *
ifmib_find_snmpidb (long ifIndex)
{
    snmpidbtype *snmpidb = NULL;

    FOR_ALL_SNMPIDBS(snmpidb) {
	if (snmpidb == NULL) {
	    continue;
	}
	if (ifIndex == snmpidb->snmp_if_index) {
	    break;
	}
    }
    return (snmpidb);
}

/*
 * ifStackQinsert:  insert a new entry into the ordered snmpifStackQ
 */
void
ifStackQinsert (ifStackQ     *newifStack)
{
    ifStackQ	       *prev = NULL;
    ifStackQ	       *test;
    uint		ifHighIndex;
    uint		ifLowIndex;

    if ((newifStack == NULL) || (newifStack->entry == NULL))
	return;

    ifHighIndex = newifStack->entry->ifStackHigherLayer;
    ifLowIndex = newifStack->entry->ifStackLowerLayer;

    test = snmpifStackQ.qhead;
    while (test) {
	if (ifHighIndex < test->entry->ifStackHigherLayer)
	    break;
	else if ((ifHighIndex == test->entry->ifStackHigherLayer) &&
		 (ifLowIndex < test->entry->ifStackLowerLayer)) 
	    break;
	prev = test;
	test = test->next;
    }

    insqueue(&snmpifStackQ, newifStack, prev);
    SET_ALL_VALID(newifStack->entry->valid);
    return;
}

/*
 * ifRcvAddrQinsert:  insert a new entry into the ordered snmpifRcvAddrQ
 */
void
ifRcvAddrQinsert (ifRcvAddrQ     *newifRcvAddr)
{
    ifRcvAddrQ	       *prev;
    ifRcvAddrQ	       *test;
    uint	       ifIndex;
    OctetString        *rcvAddress;

    if ((newifRcvAddr == NULL) || ( newifRcvAddr->entry == NULL))
	return;
    ifIndex = newifRcvAddr->snmp_if_index;
    rcvAddress = CloneOctetString(newifRcvAddr->entry->ifRcvAddressAddress);
    prev = NULL;
    test = snmpifRcvAddrQ.qhead;
    while (test) {
	if (ifIndex < test->snmp_if_index) {
	    break;
	} else if ((ifIndex == test->snmp_if_index) &&
		    (rcvAddress->length < 
		    test->entry->ifRcvAddressAddress->length)) {
	    break;
	} else if ((ifIndex == test->snmp_if_index) &&
		   (rcvAddress->length == 
		    test->entry->ifRcvAddressAddress->length) &&
		   (CmpOctetStrings(rcvAddress,
				    test->entry->ifRcvAddressAddress) < 0)) {
	    break;
	}
	      
	prev = test;
	test = test->next;
    }
    insqueue(&snmpifRcvAddrQ, newifRcvAddr, prev);
    SET_ALL_VALID(newifRcvAddr->entry->valid);

    FreeOctetString(rcvAddress);
    return;
}

/*
 * ifRcvAddrQclean:  destroy all associated entries with given ifIndex
 */
static void
ifRcvAddrQclean (uint     ifIndex)
{
    ifRcvAddrQ	       *test, *next;
    ifRcvAddressEntry_t   *entry = NULL;

    for (test = snmpifRcvAddrQ.qhead; test; test = next) {

	entry = test->entry;
	next = test->next;

	/*
	 * the snmpifRcvAddrQ is sorted by ifIndex and
	 * ifRcvAddressAddress.
	 */
	if (test->snmp_if_index == ifIndex)
	    ifmib_destroy_rcvaddr(ifIndex,
		       entry->ifRcvAddressAddress->octet_ptr,
                       entry->ifRcvAddressAddress->length);

    }

    return;

}


/*
 * ifStackQclean:  destroy all stack entries associated with an ifIndex
 */
static void
ifStackQclean (uint     ifIndex)

{
    ifStackQ	       *test;
    ifStackQ	       *next;
    ifStackEntry_t     *entry;

    for (test = snmpifStackQ.qhead; test != NULL; test = next) {

	entry = test->entry;
	next = test->next;

	if ((entry->ifStackHigherLayer == ifIndex) ||
	    (entry->ifStackLowerLayer == ifIndex))
	    ifmib_destroy_stacklink(entry->ifStackHigherLayer,
				    entry->ifStackLowerLayer);

    } /* end of for loop */
    return;
}

/*
 * snmpidb_init_common
 * Initialize an snmpidb
 */
extern inline void snmpidb_init_common (snmpidbtype *snmpidb)
{
    snmpidb->snmp_if_index = 0;
    snmpidb->snmp_if_struct = SNMP_IF_NONE;
    snmpidb->snmp_if_ptr.ifptr.hwidb = NULL;
}

/*
 * create_snmpidb
 * Create an SNMP IDB (number, type, and pointers to real interface)
 */

extern inline snmpidbtype *create_snmpidb (void)
{
    snmpidbtype *snmpidb;

    snmpidb = malloc_named(sizeof(snmpidbtype), "SNMP IDB");
    if (!snmpidb)
	return(NULL);

    snmpidb_init_common(snmpidb);
    return(snmpidb);
}

/*
 * get_snmpidb
 * Get an SNMP IDB (number, type, and pointers to real interface)
 * Gets the SNMP IDB from freelist, if available, else calls 
 * create_snmpidb().
 * NB:  Assumes that any snmpidb on the freelist has already been
 * initialized before being put on the freelist.  
 * Also assumes that the snmpfreeidbQ pointer will *never* be NULL.
 * This should be okay, as it is initiallized to point at a declared
 * queue at compile time.
 */

extern inline snmpidbtype *get_snmpidb (void)
{
    snmpidbtype *snmpidb;

    if (snmpfreeidbQ->qhead != NULL) {
	snmpidb = snmpfreeidbQ->qhead;
	p_dequeue(snmpfreeidbQ);
	return(snmpidb);
    } else {
	if (onintstack()) {
	    return(NULL);
	} else {
	    return(create_snmpidb());
	}
    }
}

/*
 * enqueue_snmpidb
 * Enqueue a setup snmpidb IDB onto the snmpidbQ.
 */

extern inline void enqueue_snmpidb (snmpidbtype *snmpidb)
{
    if (snmpidb->snmp_if_index == 0) {
	errmsg(&msgsym(SNMP0IDX, SYS), "enqueue");
	return;
    }
    p_enqueue(snmpidbQ, snmpidb);
}

/*
 * unqueue_snmpidb
 * Unqueue an snmpidb IDB from the snmpidbQ.
 */

extern inline void unqueue_snmpidb (snmpidbtype *snmpidb)
{
    p_unqueue(snmpidbQ, snmpidb);
    if (snmpidb->snmp_if_index == 0) {
	errmsg(&msgsym(SNMP0IDX, SYS), "unqueue");
	return;
    }
}

/*
 * free_snmpidb
 * Stick an unused snmpidb on our freelist.  Ignores NULL pointers.
 */

static void free_snmpidb (snmpidbtype *snmpidb)
{
    if (snmpidb == NULL)
	return;
    p_enqueue(snmpfreeidbQ, snmpidb);
}

/*
 * can_create_more_snmpidb
 * Do we have more snmp if index values available?
 */

extern inline boolean can_create_more_snmpidbs (void)

{
    return(next_snmp_if_index < MAX_SNMP_INTERFACES);
}

/*
 * get_next_snmp_if_index
 * Return the next free snmp if index value available.
 */

extern inline uint get_next_snmp_if_index (void)
{
    return(next_snmp_if_index++);
}


/*
 * instrumentation for ifmib registry services
 */

long ifmib_get_ifNumber (void)
{
   /*
    * since some hwidbs just get marked DELETED, and not dereg'd in
    * snmp, must tally ifNumber on the fly.  Also works with
    * master/subagent scheme for cat5k...
    */

    int snmp_if_number;
    hwidbtype *hwidb;
    snmpidbtype *snmpidb;
 
    snmp_if_number = 0;
    hwidb = NULL;

    FOR_ALL_SNMPIDBS(snmpidb) {
	if (snmpidb->snmp_if_struct == SNMP_IF_HWIDB) {
	    hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb;
            if ((hwidb == NULL) || (hwidb->status & IDB_DELETED)) {
		continue;
	    }
	}
	snmp_if_number++;
    }
    return snmp_if_number;

} 

/* ifmib_hwidb_get
 * retrieve the correct idb given an ifIndex and searchtype
 *
 * NOTE:  The index2 parameter is a second index to pass into your
 * test routine.  For most cases, it will be ignored by your test
 * routine and you can simply pass 0.  This is the same idea as the
 * subfunc parameter on some of the parser macros.
 *
 * Note that snmpidbtype is in asscending order of snmp_if_index.
 */

hwidbtype *
ifmib_hwidb_get(int serialNum,
		int searchType,
		long ifIndex,
		ProcTest test,
		long* index2)
{
    snmpidbtype  *snmpidb;
    hwidbtype    *hwidb;

    FOR_ALL_SNMPIDBS(snmpidb) {

	if ((snmpidb->snmp_if_struct != SNMP_IF_HWIDB) ||
	    ((hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb) == NULL) ||
            (hwidb->status & IDB_DELETED) ||
            (ifIndex > snmpidb->snmp_if_index)){
	    continue;
	}
	if ((ifIndex == snmpidb->snmp_if_index) || (searchType == NEXT)) {
	    if ((test == NULL) || ((*test)(hwidb, index2))){
                return hwidb;
	    }
	}
        else
           return NULL;
    }
    return NULL;
}
/* ifmib_2indices_get
 * This is the same as ifmib_hwidb_get except that there are 2 indices. The
 * test routine and index2 must be present. It differs from ifmib_hwidb_get:
 * 1. It changes the second index to 0 when the ifIndex is different.
 * 2. It returns NULL if the second index does not match for EXACT.
 *
 * Note that index2 and test routine must be present
 */

hwidbtype *
ifmib_2indices_get (int serialNum,
		    int searchType,
		    long ifIndex,
		    ProcTest test,
		    long* index2)
{
    snmpidbtype  *snmpidb;
    hwidbtype    *hwidb;
    uint         inputIndex2 = *index2;

    if (test == NULL || index2 == NULL){
       buginf("ifmib_2indices_get illegal input");
       return NULL;
    }

    FOR_ALL_SNMPIDBS(snmpidb) {

	if ((snmpidb->snmp_if_struct != SNMP_IF_HWIDB) ||
	    ((hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb) == NULL) ||
            (hwidb->status & IDB_DELETED) ||
            (ifIndex > snmpidb->snmp_if_index)) {
	    continue;
	}
	if ((ifIndex == snmpidb->snmp_if_index) || (searchType == NEXT)) {
            if (ifIndex != snmpidb->snmp_if_index)
                  *index2 = 0;
	    if (((*test)(hwidb, index2))){
                if (searchType == EXACT && inputIndex2 != *index2)
                   return NULL;
                return hwidb;
	    }
	}
        else
           return NULL;
    }
   return NULL;
}

/* ifmib_swidb_get
 * retrieve the correct software idb given an ifIndex and searchtype
 *
 * NOTE:  The index2 parameter is a second index to pass into your
 * test routine.  For most cases, it will be ignored by your test
 * routine and you can simply pass 0.  This is the same idea as the
 * subfunc parameter on some of the parser macros.
 *
 * Note that snmpidbtype is in asscending order of snmp_if_index.
 */

idbtype *
ifmib_swidb_get(int serialNum,
		int searchType,
		long ifIndex,
		ProcTestSwidb test,
		long* index2)
{
    snmpidbtype *snmpidb;
    idbtype *swidb = NULL;

    FOR_ALL_SNMPIDBS(snmpidb) {
	/* 
	 * This test is on hwidb, but this is a union of pointers,
	 * so checking any member checks all members.
	 */
	if ((snmpidb->snmp_if_ptr.ifptr.hwidb == NULL) ||
            (ifIndex > snmpidb->snmp_if_index)) {
	    continue;
	}
	switch (snmpidb->snmp_if_struct) {

	  case SNMP_IF_HWIDB:
	    /*
	     * NOTE: Remove this case statement once swidb-snmpidb mapping
	     *       is supported.
	     */
	    if (snmpidb->snmp_if_ptr.ifptr.hwidb->status & IDB_DELETED) {
		continue;
	    }
	    swidb = snmpidb->snmp_if_ptr.ifptr.hwidb->firstsw;
	    break;

	  case SNMP_IF_SWIDB:
	    swidb = snmpidb->snmp_if_ptr.ifptr.swidb;
	    break;

	  case SNMP_IF_SUBIAB:
	    if (snmpidb->snmp_if_ptr.ifptr.subiab->master_type != SNMP_IF_SWIDB)
		continue;
	    swidb = snmpidb->snmp_if_ptr.ifptr.subiab->master_ptr.ifptr.swidb;
	    break;

	  default:
	    continue;
	}
	if ((ifIndex == snmpidb->snmp_if_index) || (searchType == NEXT)) {
	    if ((test == NULL) || ((*test)(swidb, index2))){
                return swidb;
	    }
	} else {
           return NULL;
        }

    }
    return NULL;
}

/* ifmib_snmpidb_get_by_type
 * retrieves the correct idb given serialNum, searchType, ifIndex,
 * test(), and index2.  Returns NULL if no interesting idb found.
 *
 * NOTE: if the typeFilter is non-null, the search will be limited
 * to the idb types included in the filter.  the filter is one or more
 * of the enum SNMP_IF_DATA_TYPE values ORed together.
 *
 * NOTE:  The test routine is called by ifmib_snmpidb_get_by_type to
 * validate that the interface being tested is one that the calling
 * function is _interested in_.  ifmib_snmpidb_get_by_type() doesn't
 * care what the _interesting_ criteria is, only whether or not the
 * interface being tested is interesting to the calling routine.  You
 * write your test routine to make that determination.
 *
 * NOTE 2:  Index2 is *NOT* a second index in the SNMP sense.  Index2
 * is simply another parameter that you pass through
 * ifmib_snmpidb_get_by_type() to your test routine to help it make up
 * its mind whether or not it is interested in any interfaces that
 * ifmib_snmpidb_get_by_type might find and ask it to test.
 */

snmpidbtype *ifmib_snmpidb_get_by_type(snmpidbtype *start,
				       int serialNum,
				       int searchType,
				       long ifIndex,
				       uint typeFilter,
				       ProcTestSnmp test,
				       void* index2)

{
    snmpidbtype  *snmpidb;
    hwidbtype    *hwidb;

    if (start == NULL)
        start = (snmpidbtype *) snmpidbQ->qhead;

    for (snmpidb = start; snmpidb; snmpidb = snmpidb->next) {

	/* 
	 * This test is on hwidb, but this is a union of pointers,
	 * so checking any member checks all members.
	 */
	if ((snmpidb->snmp_if_ptr.ifptr.hwidb == NULL) ||
            (ifIndex > snmpidb->snmp_if_index)) {
	    continue;
	}
	if ((typeFilter) && (!ifmib_snmpidb_is_type(snmpidb, typeFilter)))
	    continue;
	switch (snmpidb->snmp_if_struct) {

	  case SNMP_IF_HWIDB:
	    hwidb = snmpidb->snmp_if_ptr.ifptr.hwidb;
	    if (hwidb->status & IDB_DELETED) {
		continue;
	    }
	    break;

	  case SNMP_IF_IAB:
	  case SNMP_IF_SUBIAB:
	    break;

  	  /* 
 	   * Currently, only hwidbs, iabs and subiabs are supported.  For
 	   * all others, skip to next snmpidb.
  	   */
	  default:
	    continue;
	}
	if ((ifIndex == snmpidb->snmp_if_index) || (searchType == NEXT)) {
	    if ((test == NULL) || ((*test)(snmpidb, index2))){
                return snmpidb;
	    }
	}
        else
           return NULL;

    }
    return NULL;
}

/* ifmib_snmpidb_get
 * same as ifmib_snmpidb_get_by_type, except no type filter
 */

snmpidbtype *ifmib_snmpidb_get(snmpidbtype *start,
                               int serialNum,
			       int searchType,
			       long ifIndex,
			       ProcTestSnmp test,
			       void* index2)

{
    return ifmib_snmpidb_get_by_type(start, serialNum, searchType, ifIndex,
				     0, test, index2);
}

/*
 * ifmib_register_if
 * Register an interface to be managed by SNMP.
 * Accepts interface type and pointer to interface.
 * Returns SNMP's internal interface index for this interface if
 * successful, 0 if not. 
 *
 */

static long ifmib_register_if (enum SNMP_IF_DATA_TYPE if_struct_type, /* what type?*/
			snmpifptrtype if_ptr)  /* pointer union  */
{
    snmpidbtype *snmpidb;

    if (!can_create_more_snmpidbs())
	return(0);

    /* 
     * This test is on hwidb, but this is a union of pointers,
     * so checking any member checks all members.
     */
    if (if_ptr.ifptr.hwidb == NULL) {
	return(0);
    }

    if ((if_struct_type == SNMP_IF_HWIDB) &&
	(if_ptr.ifptr.hwidb->status & IDB_DELETED)) {
	return(0);
    }

    snmpidb = get_snmpidb();
    if (snmpidb == NULL)
	return(0);

    snmpidb->snmp_if_struct = if_struct_type;
    snmpidb->snmp_if_index = get_next_snmp_if_index();
    /* 
     * Using hardware idb, but is union of pointers, so could use any
     * one of them to assign all of them.
     */
    snmpidb->snmp_if_ptr.ifptr.hwidb = if_ptr.ifptr.hwidb;
    enqueue_snmpidb(snmpidb);
    return(snmpidb->snmp_if_index);
}

/*
 * ifmib_if_deregister
 * Unregister an interface so it will no longer be managed by SNMP.
 * Accepts interface type, snmp_if_index, and pointer to interface.
 * Returns TRUE if successful in unmanaging the interface, FALSE if
 * can't find snmpidb with that snmp_if_index or one of the sanity
 * checks fails.  Sanity checks are if_struct_type same and if_ptr
 * same. 
 */

static boolean ifmib_if_deregister (uint          if_index, /* Index of candidate */
			     enum SNMP_IF_DATA_TYPE if_struct_type,
			     snmpifptrtype if_ptr)   /* pointer union  */
{
    snmpidbtype *snmpidb;

    snmpidb = ifmib_find_snmpidb(if_index);
    if (snmpidb == NULL)
	return(FALSE);

    if (snmpidb->snmp_if_struct != if_struct_type)
	return(FALSE);

    /* 
     * Testing based on hwidb pointer, but applies to all because we
     * are dealing with a union of pointers.
     */
    if (snmpidb->snmp_if_ptr.ifptr.hwidb != if_ptr.ifptr.hwidb)
	    return(FALSE);
 
    unqueue_snmpidb(snmpidb);		/* removed from managed queue */

    /* Clear the snmp_if_index in structure */
    switch (snmpidb->snmp_if_struct) {
      case SNMP_IF_HWIDB:
	if_ptr.ifptr.hwidb->snmp_if_index = 0; 
	break;
      case SNMP_IF_SWIDB:
	if_ptr.ifptr.swidb->snmp_if_index = 0; 
	break;
      case SNMP_IF_IAB:
	if_ptr.ifptr.iab->snmp_if_index = 0; 
	break;
      case SNMP_IF_SUBIAB:
	if_ptr.ifptr.subiab->snmp_if_index = 0; 
	break;
      default:
	break;
    }
    snmpidb_init_common(snmpidb);	/* Re-initialize snmpidb */
    free_snmpidb(snmpidb);		/* Stick it on our free list */

    return(TRUE);
}

/*
 * Registration / deregistration routines for the various types of
 * interfaces supported.
 *
 * hardware idb support
 */

static long ifmib_register_hwidb (hwidbtype *hwidb)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.hwidb = hwidb;
    return(ifmib_register_if(SNMP_IF_HWIDB, if_ptr));
}

static boolean ifmib_deregister_hwidb (hwidbtype *hwidb)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.hwidb = hwidb;
    return(ifmib_if_deregister(hwidb->snmp_if_index, SNMP_IF_HWIDB,
			       if_ptr));
}

/* 
 * software idb support
 */

static long ifmib_register_swidb (idbtype *swidb)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.swidb = swidb;
    return(ifmib_register_if(SNMP_IF_SWIDB, if_ptr));
}

static boolean ifmib_deregister_swidb (idbtype *swidb)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.swidb = swidb;
    return(ifmib_if_deregister(swidb->snmp_if_index, SNMP_IF_SWIDB, if_ptr));
}

/* 
 * iab support 
 */

static long ifmib_register_iab (iabtype *iab)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.iab = iab;
    return(ifmib_register_if(SNMP_IF_IAB, if_ptr));
}

static boolean ifmib_deregister_iab (iabtype *iab)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.iab = iab;
    return(ifmib_if_deregister(iab->snmp_if_index, SNMP_IF_IAB, if_ptr));
}
/* 
 * subiab support 
 */

static long ifmib_register_subiab (subiabtype *subiab)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.subiab = subiab;
    GET_TIMESTAMP(subiab->last_change);
    return (ifmib_register_if(SNMP_IF_SUBIAB, if_ptr));
}

static boolean ifmib_deregister_subiab (subiabtype *subiab)

{
    snmpifptrtype if_ptr;

    if_ptr.ifptr.subiab = subiab;
    return(ifmib_if_deregister(subiab->snmp_if_index, SNMP_IF_SUBIAB, if_ptr));
}

static boolean ifmib_register_subif (
        snmpifptrtype *subif_ptr,
	enum SNMP_IF_DATA_TYPE  subif_type,
        enum SNMP_ADMIN_STATE   initAdminStatus)

/* 
 * Create an ifEntry, ifXEntry, and possibly ifRcvAddressEntry
 * INPUTS: 
 *	subif_ptr == structure to union of possible structures which can be
 *                  registered by this function.  (ie hwidb, subiab, etc).
 *      subif_type == the type of structure contained in subif_ptr
 *	initAdminStatus == initial value of ifAdminStatus. If set
 *		to IF_DOWN, then an NMS must explicitly set 
 *		ifAdminStatus to IF_UP before the interface 'can' be used
 *   NOTE: if registering a subiab, the following parts of the subiab must
 *   be filled in correctly:
 *	if_descrstring == interface description string or NULL if not used
 *	if_name == interface name or NULL if not used
 *	ifPhysAddr == primary physical address or NULL if not used
 *		this address (if non-NULL) will be used to create a 
 *		single corresponding entry in the ifRcvAddressTable
 *	ifPhysAddrLen == number of bytes in ifPhysAddr, 
 *		0 if ifPhysAddr is NULL
 *	ifType == IANAType enumeration for the interface type
 *      maxmtu == largest possible packet size for IP (and similiarly
 *              encapsulated protocols)
 *      master_ptr == pointer to the controlling interface (typically a
 *              hwidb or swidb), if there is one
 *      master_type == the type of structure contained in master_ptr.  if
 *              there is no controlling interface, this should be
 *              SNMP_IF_NONE.
 *      cleaner == callback for performing cleanup when this interface
 *              is deregistered, or NULL if no cleanup is necessary
 *      data_ptr == handle to any interface-specific data you wish to
 *              associate with this interface.
 *   The following subiab items will be set by ifmib_register_subif:
 *      snmp_if_index == ifIndex value for this interface
 *      last_change == timestamp of last interface status change
 *
 * RETURNS:
 *	TRUE if parameters valid and entry created, FALSE otherwise
 * NOTES:
 *	The following initial parameters are hard-wired for
 *      sub-interfaces:
 *		ifLinkUpDownTrapEnable == disabled(2)
 *		ifPromiscuousMode == false(2)
 *		ifConnectorPresent == true(1) for cdb sub-interfaces
 *		ifConnectorPresent == false(2) for all other sub-interfaces
 */	 

{
    idbtype *swidb = NULL;
    hwidbtype *hwidb = NULL;
    subiabtype *subiab = NULL;
    long ifIndex = 0;

    if (subif_ptr == NULL)
	return (FALSE);

    if (!subif_type)
	return (FALSE);

    switch (subif_type) {
    case SNMP_IF_SWIDB:
	swidb = subif_ptr->ifptr.swidb;
	ifIndex = ifmib_register_swidb(swidb);
	swidb->snmp_if_index = ifIndex;
	break;

    case SNMP_IF_SUBIAB:
	subiab = subif_ptr->ifptr.subiab;
	ifIndex = ifmib_register_subiab(subiab);
	subiab->snmp_if_index = ifIndex;

	/*
	 * if this is the only snmp interface associated with the
	 * controlling interface, assign the ifindex value to the
	 * controlling interface
	 */
	switch (subiab->master_type) {
	case SNMP_IF_HWIDB:
	    hwidb = subiab->master_ptr.ifptr.hwidb;
	    if (hwidb->snmp_if_index == 0)
		hwidb->snmp_if_index = ifIndex;
	    break;
	case SNMP_IF_SWIDB:
	    swidb = subiab->master_ptr.ifptr.swidb;
	    if (swidb->snmp_if_index == 0)
		swidb->snmp_if_index = ifIndex;
	    break;
	case SNMP_IF_CDB:
	    /*
	     * can't include platform dependent code here, so must add
	     * cdb->snmp_if_index update in a service point
	     */
	    reg_invoke_ifmib_add_cdb_index(subiab->master_ptr.ifptr.cdb,
					   ifIndex);
	    break;
	default:
	    break;
	}
	break;

    default:
	return (FALSE);
	break;
    }

    if (!ifIndex)
	return (FALSE);

    /*
     * add endpoints to the ifStackTable for this entry
     */
    ifmib_create_stacklink(ifIndex, 0);
    ifmib_create_stacklink(0,ifIndex);
    
    return (TRUE);
}

static void ifmib_deregister_subif ( long ifIndex )
/*
 * Destroy the specified ifEntry, ifXEntry, ifStack, 
 *	 and ifRcvAddressEntries
 * INPUTS:
 *	ifIndex == specific interface to deregister
 */
{
    snmpidbtype *snmpidb;
    snmpifptrtype * if_ptr = NULL;
    subiabtype *subiab;
    idbtype *swidb = NULL;
    hwidbtype *hwidb = NULL;

    /* 
     * first - find the snmpidb associated with this ifIndex
     */
    snmpidb = ifmib_find_snmpidb(ifIndex);
    if (snmpidb == NULL)
	return;

    if (!ifmib_snmpidb_is_type(snmpidb, SNMP_IF_SWIDB|SNMP_IF_SUBIAB))
	return;

    ifRcvAddrQclean(ifIndex);
    ifStackQclean(ifIndex);

    /* 
     * Testing based on hwidb pointer, but applies to all because we
     * are dealing with a union of pointers.
     */
    if_ptr = &snmpidb->snmp_if_ptr;
    if (if_ptr->ifptr.hwidb == NULL)
	return;
 
    switch (snmpidb->snmp_if_struct) {
    case SNMP_IF_SWIDB:
	ifmib_deregister_swidb(if_ptr->ifptr.swidb);
	break;

    case SNMP_IF_SUBIAB:
	subiab = if_ptr->ifptr.subiab;
	ifmib_deregister_subiab(subiab);

	/* clear out the controlling interface's snmp_if_index */
	switch (subiab->master_type) {
	case SNMP_IF_HWIDB:
	    hwidb = subiab->master_ptr.ifptr.hwidb;
	    if (hwidb->snmp_if_index == ifIndex)
		hwidb->snmp_if_index = 0;
	    break;
	case SNMP_IF_SWIDB:
	    swidb = subiab->master_ptr.ifptr.swidb;
	    if (swidb->snmp_if_index == ifIndex)
		swidb->snmp_if_index = 0;
	    break;
	case SNMP_IF_CDB:
	    reg_invoke_ifmib_add_cdb_index(subiab->master_ptr.ifptr.cdb, 0);
	    break;
	default:
	    break;
	}

	if (subiab->cleaner)
	    (subiab->cleaner)(subiab);
	free(subiab);
	break;

    /* shut up the compiler */
    default:
	return;
	break;
    }
}

static void ifmib_delete_subif_default (idbtype *idb)
{
    reg_invoke_ifmib_deregister_subif(idb->snmp_if_index);
}

ifRcvAddressEntry_t * ifmib_create_rcvaddr (
	long		ifIndex,
	const uchar	*ifRcvAddr,
	long		ifRcvAddrLen)

/* 
 * Create an ifRcvAddressEntry for the specified interface.
 * INPUTS:
 *	ifIndex == interface requesting to add the new address
 *	ifRcvAddr == pointer to address string. Must not be NULL.
 *	ifRcvAddrLen == length of ifRcvAddr string. If ifPhysAddr
 *		was not NULL in the ifmib_createSubIfEntry call
 *		then ifRcvAddrLen must == ifPhysAddrLen.
  * RETURNS:
 *	entry pointerif the rcv address was added successfully
 *	NULL if not added due to resources or bad params
 * NOTES:
 *	All ifRcvAddress entries are created with
 *	ifRcvAddressType == other(1)
 *      Also - ifRcvAddrFn has already approved this addition prior to
 *     calling this function, including verifying rcvAddrLen is correct.
 */ 	 

{
   ifRcvAddrQ  * rcvEntry;

   if ((ifRcvAddrLen == IEEEBYTES) && !(strncmp(ifRcvAddr,"000000",IEEEBYTES)))
       return NULL;

    rcvEntry = malloc(sizeof(ifRcvAddrQ));
    if ((!rcvEntry) || (ifIndex == 0))
	return(NULL);
    rcvEntry->snmp_if_index = ifIndex;

   rcvEntry->entry = malloc(sizeof(ifRcvAddressEntry_t));
   if ((rcvEntry->entry == NULL) || (ifRcvAddr == NULL)) {
       free(rcvEntry);
       return (NULL);
   }
   rcvEntry->entry->ifIndex = ifIndex;
   rcvEntry->entry->ifRcvAddressAddress = 
       MakeOctetString((uchar *)ifRcvAddr,ifRcvAddrLen);
   rcvEntry->entry->ifRcvAddressType = 
       D_ifRcvAddressType_other;
   rcvEntry->entry->ifRcvAddressStatus = D_ifRcvAddressStatus_active;
   rcvEntry->next = NULL;

   ifRcvAddrQinsert (rcvEntry);

   return (rcvEntry->entry);
}



void ifmib_destroy_rcvaddr (
	long		ifIndex,
	const uchar	*ifRcvAddr,
	uint		ifRcvAddrLen)
/* 
 * Destroy an ifRcvAddressEntry for the specified interface.
 * INPUTS:
 *	ifIndex == interface of entry to delete
 *	ifRcvAddr == pointer to address string. Must not be NULL.
 *	ifRcvAddrLen == length of ifRcvAddr string. 
 * RETURNS:
 *	none
 */ 	 


{
   ifRcvAddrQ  * rcvEntry;
   OctetString * ifRcvAddressAddress = NULL;

   ifRcvAddressAddress =  MakeOctetString((uchar *)ifRcvAddr,ifRcvAddrLen);
   rcvEntry = ifRcvAddrQget(EXACT,ifIndex,ifRcvAddressAddress);
   FreeOctetString(ifRcvAddressAddress);
   if (rcvEntry != NULL) {
       unqueue(&snmpifRcvAddrQ, rcvEntry);
       ifRcvAddressEntry_free(rcvEntry->entry);
       free(rcvEntry);
   }

}

ifStackEntry_t * ifmib_create_stacklink (
	long	higherIfIndex,
	long	lowerIfIndex )
/* 
 * Create an ifStackEntry with the specified interfaces
 * INPUTS:
 *	higherIfIndex == higher layer
 *	lowerIfIndex == lower layer
 * RETURNS:
 *	entry ptr if ifStackEntry added successfully, NULL otherwise
 */	 
{
   ifStackQ  * stackElement;

    stackElement = malloc_named(sizeof(ifStackQ), "IfMib Element");
    if (!stackElement)
	return(NULL);

   stackElement->entry = malloc_named(sizeof(ifStackEntry_t), "IfMib Elem Data");
   if (stackElement->entry == NULL) {
       free(stackElement);
       return (NULL);
   }
   stackElement->entry->ifStackHigherLayer = higherIfIndex;
   stackElement->entry->ifStackLowerLayer = lowerIfIndex;
   stackElement->entry->ifStackStatus = D_ifStackStatus_active;
   stackElement->next = NULL;

   ifStackQinsert (stackElement);
   return (stackElement->entry);
}


void ifmib_destroy_stacklink (
	long	higherIfIndex,
	long	lowerIfIndex )
/* 
 * Destroy an ifStackEntry with the specified interfaces
 * INPUTS:
 *	higherIfIndex == higher layer
 *	lowerIfIndex == lower layer
 * RETURNS:
 *	none
 */	 

{
   ifStackQ  * stackElement = NULL;

   stackElement = ifStackQget(EXACT,higherIfIndex,lowerIfIndex);
   if (!stackElement)
       return;
   unqueue(&snmpifStackQ, stackElement);
   ifStackEntry_free(stackElement->entry);
   free(stackElement);

}

/*
 * end of service instrumentation
 */

static void
init_ifmib_api (subsystype* subsys)
{
    queue_init(&snmpifRcvAddrQ, 0);
    queue_init(&snmpifStackQ, 0);

    queue_init(snmpidbQ, 0);
    queue_init(snmpfreeidbQ, 0);

    reg_add_ifmib_register_subif(ifmib_register_subif,"ifmib_register_subif");
    reg_add_ifmib_deregister_subif(ifmib_deregister_subif,"ifmib_deregister_subif");
    reg_add_default_ifmib_delete_subif(ifmib_delete_subif_default, "ifmib_delete_subif_default");

    reg_add_ifmib_create_rcvaddr(ifmib_create_rcvaddr,"ifmib_create_rcvaddr");
    reg_add_ifmib_destroy_rcvaddr(ifmib_destroy_rcvaddr,"ifmib_destroy_rcvaddr");
    reg_add_ifmib_create_stacklink(ifmib_create_stacklink,"ifmib_create_stacklink");
    reg_add_ifmib_destroy_stacklink(ifmib_destroy_stacklink,"ifmib_destroy_stacklink");
    
    reg_add_ifmib_hwidb_get(ifmib_hwidb_get, "ifmib_hwidb_get");
    reg_add_ifmib_2indices_get(ifmib_2indices_get, "ifmib_2indices_get");
    reg_add_ifmib_snmpidb_get(ifmib_snmpidb_get, "ifmib_snmpidb_get");
    reg_add_ifmib_snmpidb_get_by_type(ifmib_snmpidb_get_by_type,
				      "ifmib_snmpidb_get_by_type");

    reg_add_ifmib_register_hwidb(ifmib_register_hwidb, "ifmib_register_hwidb");
    reg_add_ifmib_deregister_hwidb(ifmib_deregister_hwidb, "ifmib_deregister_hwidb");
    reg_add_ifmib_register_iab(ifmib_register_iab, "ifmib_register_iab");
    reg_add_ifmib_deregister_iab(ifmib_deregister_iab, "ifmib_deregister_iab");
    reg_add_ifmib_get_ifNumber(ifmib_get_ifNumber, "ifmib_get_ifNumber");
}

/*
 * ifmib api subsystem header
 * this subsystems needs to be a kernel-class subsystem due to the fact
 * that various driver-class subsystems, when they initialize, use the
 * services to register interfaces
 */

#define MAJVERSION_ifmib_api 1
#define MINVERSION_ifmib_api 0
#define EDITVERSION_ifmib_api 0

SUBSYS_HEADER(ifmib_api,
              MAJVERSION_ifmib_api,
              MINVERSION_ifmib_api,
              EDITVERSION_ifmib_api,
              init_ifmib_api,
              SUBSYS_CLASS_KERNEL,
              NULL,
              NULL);
