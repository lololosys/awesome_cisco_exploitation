/* $Id: lecmib_util.h,v 3.3.30.3 1996/07/11 20:34:54 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecmib_util.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 * ATMF LEC MIB utilities
 *
 * April 1995, Andy Bierman (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lecmib_util.h,v $
 * Revision 3.3.30.3  1996/07/11  20:34:54  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.3.30.2  1996/06/18  01:25:26  cyoung
 * Report correct lecIndex regardless of whether LEC is up or down
 * CSCdi59862:  LEC ifDescr appears confusing if interface shutdown
 * Branch: California_branch
 *
 * Revision 3.3.30.1  1996/04/25  23:14:56  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.34.1  1996/03/24  03:56:48  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3  1996/01/15  21:07:34  cyoung
 * Add check to see if lane_info->enabled in lec_info block
 * CSCdi46904:  LECs not being removed from LEC MIB after no lane client
 *
 * Revision 3.2  1995/11/17  17:51:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/05  21:49:20  abierman
 * Adding placeholder files to support ATMF LEC MIB (atm94-0737R4)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LECMIB_UTIL_H
#define LECMIB_UTIL_H

/* temp ...until registry done */
#include "../snmp/snmp_api.h"

/****************** CONSTANTS *******************/
#define NULL_LEC    ((lec_info_t *)0)
#define NULL_IDB    ((idbtype *)0)
#define NULL_FILTER_FN ((lec_gnFilterFn_t)0)

#define NOT_SET_FLAG             -2
#define SR_GET_ON_SET_FLAG       -1
#define ID_NOT_FOUND             -1

#define LECMIB_CACHE_INVALID      0
#define LECMIB_CACHE_GET          1
#define LECMIB_CACHE_GETNEXT      2      


#define NULL_CACHE   { NOT_SET_FLAG, NOT_SET_FLAG, NOT_SET_FLAG }

/* uchar *get_mac_addr(lane_dest_t *lane)
 */
#define get_mac_addr(P)      ((P)->addr.mac)

/* make sure the lane_dest_t really contains a MAC address
 * boolean verify_mac_addr(lane_dest_t *dest);
 */
#define verify_mac_addr(P)   ((P)->tag==LANE_TAG_MAC_ADDRESS)

/* verify fn for route descriptors--not used yet */
#define verify_rd(P)        ((P)->tag==LANE_TAG_ROUTE_DESC)

/* make sure the hwaddrtype actually contains an NSAP address 
 * boolean verify_nsap_hwaddr(hwaddrtype *P);
 */
#define verify_nsap_hwaddr(P)   \
    (snpa_get_type(P)==STATION_ATMNSAP && snpa_get_length(P)==STATIONLEN_ATMNSAP)

/* make sure the hwaddrtype actually contains a MAC address 
 * boolean verify_mac_hwaddr(hwaddrtype *P);
 */
#define verify_mac_hwaddr(P)   \
(snpa_get_type(P)==STATION_IEEE48 && snpa_get_length(P)==STATIONLEN_IEEE48)


/* callback function prototype for filtering the lec_info records
   during a get-next sweep 
   boolean filter_function (const lec_info_t *lec_info);
   returns TRUE if this LEC info should be skipped, FALSE
   if this LEC should be processed */
typedef boolean (*lec_gnFilterFn_t) (lec_info_t *lec_info);


/* struct to hold get and get-next caching information 
 * -- this caches based on PDU serial-id and the INDEX
 * -- since no direct coupling to the LEC code is done,
 * -- the LEC queue has to be scanned on each PDU
 * -- (may change later)
 * only one level of indexing beyond the ifIndex is supported.
 * this will have to change if the LEC MIB gets a 3-deep table
 * The gn_* vars contain the src id used in a get-next if the
 * current PDU is a get-next. the other vars represent the
 * entry currently in the cache
 */
typedef struct lecmib_cache_t_ {
    int       serial_num;
    long      lec_index;
    long      gn_lec_index;
    ushort    mode;     /* LECMIB_CACHE_GET, LECMIB_CACHE_GETNEXT */
    ushort    buff_len;       /* must be zero if buff not used */
    uchar     buff[STATIONLEN_ATMNSAP];
    ushort    gn_buff_len;       /* must be zero if gn_buff not used */
    uchar     gn_buff[STATIONLEN_ATMNSAP];
} lecmib_cache_t;


/*---------------------------------------------------------------------
 *  get_lec_index
 *
 * get the lec major index
 *---------------------------------------------------------------------*/
static inline long get_lec_index (lec_info_t *lec_info)
{
    if (!lec_info || !lec_info->lec_snmp || !lec_info->lane_info) 
	return ID_NOT_FOUND;
    else
	return (long)lec_info->lec_snmp->lec_snmp_index;
}

/*---------------------------------------------------------------------
 *  get_lec_ifIndex
 *
 * get the ifIndex assigned to this LEC
 *---------------------------------------------------------------------*/
static inline long get_lec_ifIndex (lec_info_t *lec_info)
{
    if (lec_info && lec_info->lec_snmp)
	return lec_info->lec_snmp->if_index;
    else
	return ID_NOT_FOUND;
}


/***************** E X T E R N A L    F U N C T I O N S *************/

/* get/getnext lec_info by lecIndex */
extern lec_info_t * find_lecInfo (
              long             searchType, 
	      int              serialNum,
	      long             index,
	      lecmib_cache_t  *cache,
	      boolean         *useCache);

/* get/getnext lec_info by lecIndex & NSAP */      
extern lec_info_t * find_lecInfo_by_atm_addr (
                          long           searchType, 
			  int            serialNum,
			  long           index,
			  OctetString    *lecAtmAddress,
			  lecmib_cache_t *cache,
			  boolean        *useCache);

/* get/getnext lec_info by lecIndex & MAC */      		  
extern lec_info_t * find_lecInfo_by_mac_addr (
		  long         searchType, 
		  int          serialNum,
		  long         index,
		  OctetString *lecMacAddress,
		  lecmib_cache_t *cache,
		  boolean        *useCache);

/* get/getnext lec_info by lecIndex & LE-ARP NSAP */      	  
extern lec_info_t * find_lecInfo_by_learp_addr (
		long         searchType, 
		int          serialNum,
		long         index,
		OctetString *lecLeArpAddress,
		lecmib_cache_t *cache,
		boolean        *useCache,
		lec_le_arp_t   **learp_info);

/* get/getnext lec_info by lecIfIndex */      	  
extern lec_info_t * find_lecInfo_by_ifIndex (
		long         searchType, 
		long         ifIndex);


/*** temp -- these will be in lecmib_registry.h ***/
extern void lecmib_register_lec_info(void *lec_info_mask);

extern void lecmib_complete_lec_info(void *lec_info_mask);

extern void lecmib_deregister_rcv_addr(void* lec_info_mask);

extern void lecmib_deregister_lec_info(void *lec_info_mask);



#endif  /* LECMIB_UTIL_H */

      
