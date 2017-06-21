/* $Id: ifmibapi.h,v 3.3.54.2 1996/08/13 15:01:16 tylin Exp $
 * $Source: /release/112/cvs/Xsys/snmp/ifmibapi.h,v $
 *------------------------------------------------------------------
 * Header file for api functions to IF-MIB
 *
 * March 1995, Sandra Durham
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ifmibapi.h,v $
 * Revision 3.3.54.2  1996/08/13  15:01:16  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Add ifmib_swidb_get() for supporting swidb retrieval by given snmp
 *    ifIndex.
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/05/21  06:38:39  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3  1995/11/20  22:27:32  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.2  1995/11/17  18:58:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:48:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * 		RFC 1573 (IF-MIB) Internal API
 * 
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
 *
 * This interface is used by modules implementing logical 
 * sub-interfaces that need to be represented in the IF-MIB. 
 * The 'calling' software entity is known as the LOG-IF-MIB,
 * and the software implementing this API is known as the IF-MIB
 *    	  
 * Some assumptions:
 * 1) only logical sub-interfaces need to be created. Physical 
 * 	interfaces are already being represented by the IF-MIB
 * 2) counter values are provided by the LOG-IF-MIB.
 * 3) ifStack links can only be built between existing interfaces.
 * 4) LOG-IF-MIB is responsible for creating and destroying sub-interfaces.
 * 5) The LOG-IF-MIB will request ifIndex values. The IF-MIB would reject
 *    requests for index values already in use.
 * 6) LOG-IF-MIB is responsible for presenting meaningful data in
 *  	the ifStackTable. Since many-to-many links are allowed
 *  	up and down the stack, depending on the specific protocols
 *  	involved, the IF-MIB cannot enforce any ifStack rules
 *  	except upper != lower and both ifIndexes exist. 
 * 7) the LOG-IF-MIB controls ifAdminStatus transitions
 * 8) the LOG-IF-MIB is responsible for its own ifOperStatus
 * 9) There is no association between the internal interface
 *  	numbering and ifIndex values enforced by this API.
 * 10) LOG-IF-MIB enforces the counter size rules from RFC 1573, 
 * 	section	3.2.6. A Counter64 function is required if
 * 	ifSpeed  * 20,000,000. The LOG-IF-MIB is responsible
 *         for providing the proper counter functions. The IF-MIB
 * 	must call the proper counter function, based on the
 * 	rules outined in RFC 1573.
 * 
 * Operation:
 * 1) Creating Interfaces:
 *   a) IF-MIB accepts registration of  hardware interfaces
 *   b) LOG-IF-MIB creates a sub-interface with a new function not in 
 *      the IF-MIB API
 *   c) LOG-IF-MIB registers the sub-interface with the IF-MIB
 *   c1) LOG-IF-MIB optionally creates an entry in the ifRcvAddressTable
 *       for additional supported receive addresses
 *   c2) NMS optionally creates an entry in the ifRcvAddressTable
 *       IF-MIB calls the rcvAddrFn callback to check the proposed
 *       rcvAddress addition
 *   [step c1 and c2 are repeated for each address the interface listens to]
 *   d1) LOG-IF-MIB links interface to ifStack with ifmib_createStackLink
 * 	  OR
 *   d2) NMS links interface to ifStack by setting ifStackStatus
 *       IF-MIB checks proposed NMS change with the ifStackFn callback
 * 	     
 * 2) Changing Status:
 *   a) NMS sets ifAdminStatus
 *   b) IF-MIB calls the statusFn callback, LOG-IF-MIB okays transition
 *      Admin status changes do not ripple to upper layers.
 *   c) LOG-IF-MIB handles any ifOperStatus change internally for all
 *      layers.
 * 
 * 3) Checking Status
 *   a) NMS gets ifAdminStatus, ifOperStatus
 *   b) IF-MIB knows ifAdminStatus, uses LOG-IF-MIB operStatus callback
 *      to get the ifOperStatus
 * 
 * 3) Destroying Interfaces
 *  a) LOG-IF-MIB de-registers the interface with IF-MIB
 *  b1) IF-MIB deletes the ifEntry, ifXEntry, and any ifRcvAddressEntries
 *      for the specified interface. Any ifStackEntry where the specified 
 *      ifIndex is used as the lower layer or upper layer must be removed.
 *  c) LOG-IF-MIB calls a destroy interface function not within the 
 *      IF-MIB API
 */
#ifndef	 __IFMIBAPI_H__ 
#define	 __IFMIBAPI_H__ 

#include "sr_ifmib.h"

extern queuetype	snmpifRcvAddrQ;
extern queuetype	snmpifStackQ;

#define IFHIGHSPEED_SCALE  1000000
#define MAX_IFTYPES   54            /* this is max in ianaIfType TC */

enum SNMP_ADMIN_STATE {
    SNMP_ADMIN_ERROR = 0,
    SNMP_ADMIN_UP,                   /* must match snmp enums in ifmib */
    SNMP_ADMIN_DOWN,
    SNMP_ADMIN_TESTING,
    SNMP_ADMIN_UNKNOWN,                    /* actually ifOperStatus   enum */
    SNMP_ADMIN_DORMANT                    /* actually ifOperStatus   enum */
};

/***************   T Y P E S   ***************/
/* counter return code values */
typedef enum ifmib_err_t_ {
/* note - error condition must be 0, to take fallthru case on service point*/
	IF_CNTR_NOT_AVAIL,         
	IF_NO_ERR
} ifmib_err_t;

/* counters found in ifTable and ifXTable */
typedef enum ifmib_cntr_t_ {
	IN_OCTETS_CNTR,
	IN_UCAST_CNTR,
	IN_NUCAST_CNTR,
	IN_DISCARDS_CNTR,
	IN_ERRORS_CNTR,
	IN_UNK_PROTOS_CNTR,
	OUT_OCTETS_CNTR,
	OUT_UCAST_CNTR,
	OUT_NUCAST_CNTR,
	OUT_DISCARDS_CNTR,
	OUT_ERRORS_CNTR,
	OUT_QLEN_CNTR,
	IN_BCAST_CNTR,			/* ifXTable */
	IN_MCAST_CNTR,
	OUT_BCAST_CNTR,
	OUT_MCAST_CNTR
} ifmib_cntr_t;

 
 
extern ifRcvAddressEntry_t * ifmib_create_rcvaddr (
	long		ifIndex,
	const uchar	*ifRcvAddr,
	long		ifRcvAddrLen);
/* 
 * Create an ifRcvAddressEntry for the specified interface.
 * INPUTS:
 *	ifIndex == interface requesting to add the new address
 *	ifRcvAddr == pointer to address string. Must not be NULL.
 *	ifRcvAddrLen == length of ifRcvAddr string. If ifPhysAddr
 *		was not NULL in the ifmib_createSubIfEntry call
 *		then ifRcvAddrLen must == ifPhysAddrLen.
 * RETURNS:
 *	entry pointer if the rcv address was added successfully
 *	NULL if not added due to resources or bad params
 * NOTES:
 *	All ifRcvAddress entries are created with
 *	ifRcvAddressType == other(1)
 */ 	 


extern void ifmib_destroy_rcvaddr (
	long		ifIndex,
	const uchar	*ifRcvAddr,
	uint		ifRcvAddrLen);

/* 
 * Destroy an ifRcvAddressEntry for the specified interface.
 * INPUTS:
 *	ifIndex == interface of entry to delete
 *	ifRcvAddr == pointer to address string. Must not be NULL.
 *	ifRcvAddrLen == length of ifRcvAddr string. 
 * RETURNS:
 *	none
 */ 	 


extern ifStackEntry_t *  ifmib_create_stacklink (
	long	higherIfIndex,
	long	lowerIfIndex );
/* 
 * Create an ifStackEntry with the specified interfaces
 * INPUTS:
 *	higherIfIndex == higher layer
 *	lowerIfIndex == lower layer
 * RETURNS:
 *	entry pointer if ifStackEntry added successfully, NULL otherwise
 */	 


extern void ifmib_destroy_stacklink (
	long	higherIfIndex,
	long	lowerIfIndex );
/* 
 * Destroy an ifStackEntry with the specified interfaces
 * INPUTS:
 *	higherIfIndex == higher layer
 *	lowerIfIndex == lower layer
 * RETURNS:
 *	none
 */	 


/* ifmib_idb_get
 * retrieves the correct idb given serialNum, searchType, ifIndex,
 * test(), and index2.  Returns NULL if no interesting idb found.
 *
 * NOTE:  The test routine is called by ifmib_idb_get to validate that
 * the interface being tested is one that the calling function is
 * _interested in_.  ifmib_idb_get() doesn't care what the
 * _interesting_ criteria is, only whether or not the interface being
 * tested is interesting to the calling routine.  You write your test
 * routine to make that determination.
 *
 * NOTE 2:  Index2 is *NOT* a second index in the SNMP sense.  Index2
 * is simply another parameter that you pass through ifmib_idb_get() to
 * your test routine to help it make up its mind whether or not it is
 * interested in any interfaces that ifmib_idb_get might find and ask
 * it to test.
 */

extern hwidbtype *ifmib_hwidb_get(int serialNum,
				  int searchType,
				  long ifIndex,
				  ProcTest test,
				  long* index2);

extern hwidbtype *ifmib_2indices_get(int serialNum,
				     int searchType,
				     long ifIndex,
				     ProcTest test,
				     long* index2);

extern idbtype *ifmib_swidb_get(int serialNum,
				int searchType,
				long ifIndex,
				ProcTestSwidb test,
				long* index2);

extern snmpidbtype *ifmib_snmpidb_get(snmpidbtype *start,
                                      int serialNum,
				      int searchType,
				      long ifIndex,
				      ProcTestSnmp test,
				      void* index2);

extern snmpidbtype *ifmib_snmpidb_get_by_type(snmpidbtype *start,
					      int serialNum,
					      int searchType,
					      long ifIndex,
					      uint typeFilter,
					      ProcTestSnmp test,
					      void* index2);

extern long ifmib_get_ifNumber(void);

extern inline boolean
ifmib_snmpidb_is_type (snmpidbtype *snmpidb, uint filter)
{
    return (snmpidb->snmp_if_struct & filter) ? TRUE : FALSE;
}

#endif	 __IFMIBAPI_H__ 
