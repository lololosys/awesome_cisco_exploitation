/* $Id: lecmib_util.c,v 3.3.42.5 1996/07/11 20:34:52 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecmib_util.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 ATMF LEC MIB support utilities
 * SNMP MIB method routines are in sr_lecmib.c
 * Only the SNMPR auto-generated functions are kept
 * This file contains support for:
 *       * get & get-next search
 *
 * April 1995, Andy Bierman
 *
 * Copyright (c) 1995-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lecmib_util.c,v $
 * Revision 3.3.42.5  1996/07/11  20:34:52  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.3.42.4  1996/05/30  21:33:20  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.3.42.3  1996/05/09  14:33:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.42.2.2.1  1996/04/27  07:06:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.42.2  1996/04/25  23:14:53  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.42.1  1996/04/04  05:35:09  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.3.60.2  1996/04/08  14:52:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.60.1  1996/04/08  01:57:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.48.1  1996/03/22  22:55:22  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.32.1  1996/03/05  06:43:28  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.22.1  1996/02/27  21:10:46  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.2.1  1996/01/11  01:46:45  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1996/01/05  04:56:21  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.2.10.1  1995/12/29  01:07:01  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/05  21:49:19  abierman
 * Adding placeholder files to support ATMF LEC MIB (atm94-0737R4)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "interface.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"

#include "../if/atm.h"
#include "../atm/ilmi_api.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"
#include "lane_registry.h"

#include "../snmp/snmp_api.h"
#include "sr_lecmib.h"
#include "lecmib_util.h"
#include "lecmib_if.h"


/*---------------------------------------------------------------------
 * CmpOctet2CString 
 *
 * special version of CmpOctetString that takes a Cstring instead
 * of an OctetString struct as the 2nd string to compare. Saves
 * having to make a bunch of temp structs
 * INPUTS:
 *    str1 == pointer to the OctetString
 *    str2 == pointer to the counted-string
 *    str2_len == byte count of str2
 * RETURNS:
 *    diff result (-1, 0, +1)
 *---------------------------------------------------------------------*/
static inline int CmpOctet2CString (const OctetString *str1, 
				    const uchar *str2,
				    uint  str2_len)
{
    if (str1==NULL || str2==NULL || str1->octet_ptr==NULL)   {
	return 0;     /* really internal error?? */
    }
    
    /* don't comapre if any of the strings zero-length */
    if (str1->length==0 && str2_len==0)
	return 0;
    else if (str1->length==0)
	return -1;
    else if (str2_len==0)
	return 1;
    
    /* strings non-zero length--compare byte by byte
     * this is a lexi-next compare, so length of strings can
     * be different
     */
    return memcmp(str1->octet_ptr, str2, min(str1->length, str2_len));
}  /* CmpOctet2CString */


/*---------------------------------------------------------------------
 * get_learp_info
 *
 * Retrieve the specified lec_le_arp_t record
 * INPUTS:
 *    lec_info == pointer to the lec_info struct for the specified LEC
 *    learp_mac_addr == LE-ARP MAcAddress to find
 * RETURNS:
 *    pointer to the lec_le_arp_t struct for the specified LE_ARP entry
 *---------------------------------------------------------------------*/
static inline lec_le_arp_t *
get_learp_info (lec_info_t  *lec_info, OctetString  *learp_mac_addr)
{
    lec_le_arp_t* ret_val;

    ret_val = NULL;
    if (learp_mac_addr->length != STATIONLEN_IEEE48)
	return NULL;
    
    ret_val = lec_lookup_le_arp(lec_info, learp_mac_addr->octet_ptr);

    /* only return completed entries */
    if ((ret_val) && (ret_val->state != LEC_MAC_FLOOD))
	return ret_val;
    else
	return NULL;
	
    
}   /* get_learp_info */


/*---------------------------------------------------------------------
 * get_lecInfo
 *
 * Retrieve the specified lec_info record
 * INPUTS:
 *    lecIndex == index into leInterfaces table
 * RETURNS:
 *    pointer to the lec_info struct for the specified LEC
 *---------------------------------------------------------------------*/
static inline lec_info_t *
get_lecInfo (long lecIndex)
{
    lec_info_t  *lec_info;
    
    if (lecIndex <= 0)
	return NULL_LEC;
    
    FOR_ALL_LEC(lec_info)  {
	if (get_lec_index(lec_info) == lecIndex)
	    return lec_info;
    }
    return NULL_LEC;
    
}   /* get_lecInfo */


/*---------------------------------------------------------------------
 * getnext_lecInfo
 *
 * Retrieve the lexi-next of a lec_info record from an lecIndex value
 * this will be the entry greater than or equal to the specified 
 * lecIndex
 * INPUTS:
 *    lecIndex == index into leInterfaces table, lecIndex for
 *                  lexi-next LEC
 *    filterFn   == address of filter-function for get-next sweep
 *                  of the LEC queue. See lec_gnFilterFn_t above
 *                  set to NULL if no filter function used
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 * NOTES:
 *    this function relies on the fact that the maximum lecIndex
 *    will be less than 2G -- implementation will not use the max
 *    index value of MAXLONG
 *    SNMP Research get-next is really get-lexi-greater-than-or-equal
 *    to the specified INDEX--not get-lexi-greater-than like epilogue
 *---------------------------------------------------------------------*/
static lec_info_t *
getnext_lecInfo (long lecIndex, lec_gnFilterFn_t filterFn, long
		 *retLecIndex)
{
    lec_info_t  *lec_info, *best_lec = NULL_LEC;
    long        best = MAXLONG, current = 0;
    
    FOR_ALL_LEC(lec_info)  {
	current = get_lec_index(lec_info);
	if (current==0)
	    continue;   /* internal problem--lecIndex should be set */
	
	/* check if user-supplied filter present. returns TRUE if the
	 * LEC should be skipped, FALSE if it should be processed 
	 */
	if (filterFn)  {
	    if ((*filterFn)(lec_info))
		continue;
	}
	
	if (current == lecIndex)  {
	    /* special case--found the exact lexi-next, won't find
	       better */
	    *retLecIndex = lecIndex;
	    return lec_info;
	}
	else if (current > lecIndex)  {
	    if (current < best)  {
		best = current;
		best_lec = lec_info;
	    }
	}
    }
    if (best_lec != NULL_LEC)
	*retLecIndex = best;
    return best_lec;
    
}   /* getnext_lecInfo */



/*---------------------------------------------------------------------
 * getnext_lecInfo_2
 *
 * Retrieve the lexi-next AND the runner-up lexi-next. Tables
 * that have 2 index components will use either one for a get-next,
 * depending on the value of the 2nd index
 * Note: this will be the entry greater than or equal to the specified 
 * lecIndex SNMP Research style
 * INPUTS:
 *    lecIndex == index into leInterfaces table, lecIndex for
 *                  lexi-next LEC
 *    filterFn   == address of filter-function for get-next sweep
 *                  of the LEC queue. See lec_gnFilterFn_t above
 *                  set to NULL if no filter function used
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 *    *retLecIndex_2 contains the index of the runner-up LEC (may be 0)
 *    **retLecPtr    contains a pointer to the runner-up lec_info struct
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 * NOTES:
 *    this function relies on the fact that the maximum lecIndex
 *    will be less than 2G -- implementation will not use the max
 *    index value of MAXLONG
 *    SNMP Research get-next is really get-lexi-greater-than-or-equal
 *    to the specified INDEX--not get-lexi-greater-than like epilogue
 *---------------------------------------------------------------------*/
static lec_info_t *
getnext_lecInfo_2 (long lecIndex, lec_gnFilterFn_t filterFn, 
		   long *retLecIndex, long *retLecIndex_2,
		   lec_info_t **retLecPtr)
{
    lec_info_t  *lec_info, *best_lec = NULL_LEC, *next_best_lec = NULL;
    long        best = MAXLONG, next_best = 0, current = 0;
    
    FOR_ALL_LEC(lec_info)  {
	current = get_lec_index(lec_info);
	if (current==0)
	    continue;   /* internal problem--lecIndex should be set */

	/* check if user-supplied filter present. returns TRUE if the
	   LEC should be skipped, FALSE if it should be processed */
	if (filterFn)  {
	    if ((*filterFn)(lec_info))
		continue;
	}

	/* don't handle exact get-next match as a quick exit because
	 * the next-best may not be known yet 
	 */
	if (current >= lecIndex)  {
	    /* first check if the best is changing */
	    if (current < best)  {
		next_best = best;
		next_best_lec = best_lec;
		best = current;
		best_lec = lec_info;
	    }
	    else if (current < next_best)  {
		/* the next best is changing */
		next_best = current;
		next_best_lec = lec_info;
	    }
	}
	/* else current is not lexi-next to best choice so far */
    }  /* FOR_ALL_LEC */

    if (best_lec != NULL_LEC)  {
	*retLecIndex = best;
	*retLecIndex_2 = next_best;
	*retLecPtr = next_best_lec;
	*retLecIndex = best;
    }
    return best_lec;

}   /* getnext_lecInfo_2 */



/*---------------------------------------------------------------------
 * filter_invalid_nsap
 * callback -- can't be inline
 */
static boolean 
filter_invalid_nsap (lec_info_t *lec_info)
{
    if (lec_info)
	return !verify_nsap_hwaddr(&lec_info->nsap);
    else 
	return FALSE;

}   /* filter_invalid_nsap */


/*---------------------------------------------------------------------
 * filter_invalid_nsap_mac
 * callback -- can't be inline
 */
static boolean
filter_invalid_nsap_mac (lec_info_t *lec_info)
{
    if (lec_info)
	return (!verify_nsap_hwaddr(&lec_info->nsap));
    else 
	return FALSE;

}   /* filter_invalid_nsap_mac */


/*---------------------------------------------------------------------
 * filter_empty_learp_cache
 * callback -- can't be inline
 */
static boolean
filter_empty_learp_cache (lec_info_t *lec_info)
{
    int    index;

    /* sweep through the LE-ARP table hash-chain headers, the first
     * non-NULL header indicates at least one entry in the cache,
     * hopefully, a more efficient means will be provided later 
     */
    for (index=0; index < LANE_NETHASHLEN; index++)  {
	if (lec_info->le_arp_table[index])
	    return FALSE;
    }
    return TRUE;

}   /* filter_empty_learp_cache */



/*---------------------------------------------------------------------
 * getnext_lecInfo_by_atm_addr
 *
 * Retrieve the lexi-next of a lec_info record from an lecIndex value
 * and NSAP.
 * this will be the entry greater than or equal to the specified 
 * lecIndex and NSAP
 * INPUTS:
 *    lecIndex == index into leInterfaces table, lecIndex for
 *                  lexi-next LEC
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 *---------------------------------------------------------------------*/
static lec_info_t *
getnext_lecInfo_by_atm_addr (long lecIndex, 
			     const OctetString  *atmAddr,
			     long *retLecIndex)
{
    lec_info_t  *lec_info, *lec_info_2 = NULL;
    long        lec_index_2;

    uchar       cmp_buffer[STATIONLEN_ATMNSAP];
    OctetString cmp_string = { cmp_buffer, STATIONLEN_ATMNSAP };

    /* first find a candidate LEC, may need 1st & 2nd best LEC.
     * the current code does not check for more then one NSAP per 
     * LEC...change getnext_lecInfo_2 if multiple NSAPs per LEC
     * ever implemented
     */
    if (atmAddr->length)  {
	lec_info = getnext_lecInfo_2(lecIndex, filter_invalid_nsap,
			       retLecIndex, &lec_index_2, &lec_info_2);
    }
    else  {
	lec_info = getnext_lecInfo(lecIndex, filter_invalid_nsap,
			       retLecIndex);
    }
    if (lec_info==NULL_LEC)
	return NULL_LEC;

    /* got some LEC, check if it different than the input INDEX */
    if (*retLecIndex > lecIndex)  {
	/* the major index rolled over, so there is no need to compare
	 * the ATM addresses--the found entry is the lexi-next  
	 */
	return lec_info;
    }

    if (atmAddr->length)  {
	/* lexi-next lecIndex is the same entry--have to
	 * compare the ATM address. 
	 * -- possibly zero-pad the compare-address
	 */
	(void)memset(cmp_buffer, 0x0, STATIONLEN_ATMNSAP);
	(void)memcpy(cmp_buffer, atmAddr->octet_ptr, 
		     min(atmAddr->length, STATIONLEN_ATMNSAP));

	/* check the NSAP passed in against the lec_2 NSAP */
	if (CmpOctet2CString(&cmp_string, lec_info->nsap.addr,
			     STATIONLEN_ATMNSAP) > 0)  {
	    /* have to use the next-best entry returned from the
	     * search ... may be NULL
	     */
	    *retLecIndex = lec_index_2;
	    return lec_info_2;
	}
	/* else the best LEC choice is still lexi-next to the
	 * passed-in NSAP...drop though and return the lec_info
	 */
    }
	
    /* lexi-next found in the best entry */
    return lec_info;

}   /* getnext_lecInfo_by_atm_addr */


/*---------------------------------------------------------------------
 * getnext_lecInfo_by_mac_addr
 *
 * Retrieve the lexi-next of a lec_info record from an lecIndex value
 * and MAC addr.
 * this will be the entry greater than or equal to the specified 
 * lecIndex and MAC addr
 * INPUTS:
 *    lecIndex == index into leInterfaces table, lecIndex for
 *                  lexi-next LEC
 *    macAddr == MAC address index
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 *---------------------------------------------------------------------*/
static lec_info_t *
getnext_lecInfo_by_mac_addr (long lecIndex, 
			     const OctetString  *macAddr,
			     long *retLecIndex)
{
    lec_info_t  *lec_info, *lec_info_2 = NULL;
    long        lec_index_2;
    uchar       cmp_buffer[STATIONLEN_IEEE48];
    OctetString cmp_string = { cmp_buffer, STATIONLEN_IEEE48 };

    /* first find a candidate LEC */
    if (macAddr->length)  {
	lec_info = getnext_lecInfo_2(lecIndex, filter_invalid_nsap_mac,
				   retLecIndex, &lec_index_2, &lec_info_2);
    }
    else  {
	lec_info = getnext_lecInfo(lecIndex, filter_invalid_nsap_mac,
				   retLecIndex);
    }
    if (lec_info==NULL_LEC)
	return NULL_LEC;

    /* got some LEC, check if it different than the input INDEX */
    if (*retLecIndex > lecIndex)  {
	/* the major index rolled over, so there is no need to compare
	 * the MAC addresses--the found entry is the lexi-next  
	 */
	return lec_info;
    }

    if (macAddr->length)  {
	/* lexi-next lecIndex is the same entry--have to
	 * compare the MAC address. 
	 * -- possibly zero-pad the compare-address 
	 */
	(void)memset(cmp_buffer, 0x0, STATIONLEN_IEEE48);
	(void)memcpy(cmp_buffer, macAddr->octet_ptr, 
		     min(macAddr->length, STATIONLEN_IEEE48));
	if (CmpOctet2CString(&cmp_string, 
			     lec_info->normalMac,
			     STATIONLEN_IEEE48) > 0)  {
	    /* have to use the next-best entry -- may be NULL */
	    *retLecIndex = lec_index_2;
	    return lec_info_2;
	}
	/* else drop through and return 1st choice */
    }  /* macAddr not empty */
	
    /* else first lexi-next found is okay -- cmp_mac less-than or
     * equal to found mac 
     */
    return lec_info;

}   /* getnext_lecInfo_by_mac_addr */


/*---------------------------------------------------------------------
 * getnext_learp_info
 *
 * Retrieve the lexi-next of a lec_le_arp record from an OctetString
 * value and the specified lec_info
 * This will be the entry greater than or equal to the specified 
 * lecIndex and LE-ARP MAC addr
 * INPUTS:
 *    lec_info == pointer to lec_info struct 
 *    learpMacAddr == LE-ARP MAC address index
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 *    *retLeArp == pointer to the lec_le_arp_t struct for this entry
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 * NOTES:
 *   this function will check the LE_ARP cache of the specified
 *   LEC. If tno get-0next is found, the caller might want to
 *   roll-over to the lexi-next lec_info
 *---------------------------------------------------------------------*/
static lec_le_arp_t *
getnext_learp_info (lec_info_t   *lec_info, 
		    OctetString  *learpMacAddr)

{
    lec_le_arp_t  *tmp_learp, *learp_hdr, *best_learp = NULL;
    int           index, cmp_result;

    for (index=0; index < LANE_NETHASHLEN; index++)  {
	learp_hdr = lec_info->le_arp_table[index];

	/* check this hash bucket chain */
	for (tmp_learp = learp_hdr; (tmp_learp); tmp_learp =
	     tmp_learp->next)  {
	    /*??? SHOULD THE LEARP ENTRY STATE BE FILTERED -- NO FOR
	      NOW */

	    /* 
	     * check valid MAC--this should always be the case 
	     * That's whay I am deleting this line:-) BR
	     */
	    
	    /* compare found MAC to specified MAC */
	    cmp_result = memcmp(learpMacAddr->octet_ptr,
				tmp_learp->mac,
				STATIONLEN_IEEE48);
	    if (cmp_result == 0)   {
		/* won't find better lexi-next than this entry */
		return tmp_learp;
	    }
	    else if (cmp_result < 0)  {
		if (best_learp)   {
		    /* the best has been set, check for new best */
		    if (memcmp(tmp_learp->mac,
			       best_learp->mac,
			       STATIONLEN_IEEE48)  < 0)  {
			best_learp = tmp_learp;
		    }
		}
		else
		    best_learp = tmp_learp;   /* 1st time set */
	    }
	}  /* for each entry in this hash bucket chain */
    }   /* for all hash bucket chains */
    return best_learp;
    
}   /* getnext_learp_info */


/*---------------------------------------------------------------------
 * getnext_lecInfo_by_learp_addr
 *
 * Retrieve the lexi-next of a lec_info record from an lecIndex value
 * and LE-ARP MAC addr.
 * this will be the entry greater than or equal to the specified 
 * lecIndex and LE-ARP MAC addr
 * INPUTS:
 *    lecIndex == index into leInterfaces table, lecIndex for
 *                  lexi-next LEC
 *    learpMacAddr == LE-ARP MAC address index
 * OUTPUTS:
 *    *retLecIndex contains the index of the returned LEC, only valid
 *     if return value is not NULL
 *    *retLeArp == pointer to the lec_le_arp_t struct for this entry
 * RETURNS:
 *    pointer to the lec_info struct for the lexi-next LEC
 *---------------------------------------------------------------------*/
static lec_info_t *
getnext_lecInfo_by_learp_addr (long               lecIndex, 
			       const OctetString  *learpMacAddr,
			       long               *retLecIndex,
			       lec_le_arp_t       **retLeArp)
{
    lec_info_t  *lec_info, *lec_info_2 = NULL_LEC;
    long        lec_index_2;
    uchar       cmp_buffer[STATIONLEN_IEEE48];
    OctetString cmp_string = { cmp_buffer, STATIONLEN_IEEE48 };

    /* first find a candidate LEC */
    lec_info = getnext_lecInfo_2(lecIndex, filter_empty_learp_cache,
				 retLecIndex, &lec_index_2, &lec_info_2);
    if (lec_info==NULL_LEC)
	return NULL_LEC;     /* no entry found */

    /* got some LEC, check if it is on the same LEC that we started
       with */
    if (*retLecIndex == lecIndex)  {
	/* try to find a lexi-next LE ARP entry on this LEC */
	(void)memset(cmp_buffer, 0x0, STATIONLEN_IEEE48);
	(void)memcpy(cmp_buffer, learpMacAddr->octet_ptr, 
		     min(learpMacAddr->length, STATIONLEN_IEEE48));
	*retLeArp = getnext_learp_info(lec_info, &cmp_string);
	if (*retLeArp != NULL)   {
	    /* found the lexi-next */
	    return lec_info;
	}
	/* have to look for 1st LE-ARP entry on lec_2 */
	if (lec_info_2 == NULL_LEC)
	    return NULL_LEC;
	(void)memset(cmp_buffer, 0x0, STATIONLEN_IEEE48);
	*retLeArp = getnext_learp_info(lec_info_2, &cmp_string);
	if (*retLeArp != NULL)  {
	    *retLecIndex = lec_index_2;
	    return lec_info_2;
	}
	else
	    return NULL_LEC;
    }

    /* the major index rolled over, so there is no need to compare
     * the LE-ARP MAC addresses--the found entry is the lexi-next
     * --now find the first LE-ARP entry 
     */
    (void)memset(cmp_buffer, 0x0, STATIONLEN_IEEE48);
    *retLeArp = getnext_learp_info(lec_info, &cmp_string);
    if (*retLeArp == NULL)  {
	/* should not happen, because this entry was just screened
	 *  in the filter_empty_learp_cache callback 
	 */
	return NULL;
    }
    else  
	return lec_info;

}   /* getnext_lecInfo_by_learp_addr */


/*---------------------------------------------------------------------
 * find_lecInfo
 * check the cache for the specified table or try to find the lec_info
 * struct if the cached struct does not contain the specified data
 * INPUTS:
 *    searchType == SR searchType (e.g. EXACT)
 *    serialNum == SR serial number of PDU, must increment every PDU
 *    index == LEC table index
 * INPUT/OUTPUT:
 *    cache == lecmib cache control block for this table
 * OUTPUTS:
 *    *useCache == true if the cached entry can be used. If false
 *      then foundIndex and the return value should be checked
 * RETURNS:
 *    ptr to lec_info struct, or NULL if not found or needed
 *---------------------------------------------------------------------*/
lec_info_t * find_lecInfo (long             searchType, 
	      int              serialNum,
	      long             index,
	      lecmib_cache_t  *cache,
	      boolean         *useCache)
{
    lec_info_t  *lec_info = NULL;
    long        found_index;

    /* find the lec_info struct */
    if (searchType==EXACT)  {
	*useCache = (cache->mode==LECMIB_CACHE_GET &&
	    serialNum != -1 && serialNum==cache->serial_num &&
	    index==cache->lec_index);
	if  (*useCache)  
	    return NULL_LEC;   /* not needed */

	/* else cache invalid -- look for the specified entry */
	lec_info = get_lecInfo(index);
	if (lec_info==NULL || lec_info->lec_snmp==NULL)
	    return NULL_LEC;   /* LEC not found or SNMP malloc failed */
	/* found the entry--update the cache indicators, the
	 * static entry will get filled in by the caller, 
	 * don't do this for a get_entry during a set_request 
	 */
	if (serialNum != SR_GET_ON_SET_FLAG)  {
	    cache->mode = LECMIB_CACHE_GET;
	    cache->serial_num = serialNum;
	    cache->lec_index = index;
	}
	else  {
	    /* have to zap the cache for this set request */
	    cache->mode = LECMIB_CACHE_INVALID;
	}
	return lec_info;
    }  /* END searchtype is EXACT */

    /* else lexi-next request..check the cache */
    *useCache = (cache->mode==LECMIB_CACHE_GETNEXT && 
	    serialNum != -1 && serialNum==cache->serial_num &&
	    index==cache->gn_lec_index);

    if (*useCache)
	return NULL_LEC;  /* lec_info not needed */

    /* else cache is invalid */
    lec_info = getnext_lecInfo(index, NULL_FILTER_FN, &found_index);
    if ((lec_info==NULL) || (lec_info->lec_snmp==NULL) ||
	(lec_info->lane_info == NULL))
	return NULL_LEC;     /* entry not found */

    /* else found the entry--update the cache indicators, the
     * static entry will get filled in by the caller, 
     * don't do this for a get_entry during a set_request 
     */
    if (serialNum != SR_GET_ON_SET_FLAG)  {
	cache->mode = LECMIB_CACHE_GETNEXT;
	cache->serial_num = serialNum;
	cache->lec_index = found_index;
	cache->gn_lec_index = index;
    } else  {
	/* have to zap the cache for this set request */
	cache->mode = LECMIB_CACHE_INVALID;
    }
    return lec_info;

}   /* END find_lecInfo */


/*---------------------------------------------------------------------
 * find_lecInfo_by_atm_addr
 * find the lec_info struct
 * the specified ATM address is compared to the primary NSAP for
 * each LEC
 * INPUTS:
 *    searchType == SR searchType (e.g. EXACT)
 *    serialNum == SR serial number of PDU
 *    index == LEC table index
 *    lecAtmAddress == pointer to ATM address to use in get/getnext
 *                     search
 * INPUT/OUTPUT:
 *     cache == ptr to cache control block
 * OUTPUTS:
 *    *useCache == true if the cached entry can be used. If false
 *      then return value should be checked
 * RETURNS:
 *    ptr to lec_info struct, or NULL if not found or needed
 *---------------------------------------------------------------------*/
lec_info_t * find_lecInfo_by_atm_addr (
                          long           searchType, 
			  int            serialNum,
			  long           index,
			  OctetString    *lecAtmAddress,
			  lecmib_cache_t *cache,
			  boolean        *useCache)
{
    lec_info_t  *lec_info = NULL;
    long        found_index;

    /* find the lec_info struct */
    if (searchType==EXACT)  {
	*useCache = (cache->mode==LECMIB_CACHE_GET &&
		     serialNum != -1 &&
		     serialNum==cache->serial_num &&
		     index==cache->lec_index &&
		     cache->buff_len==STATIONLEN_ATMNSAP &&
		     CmpOctet2CString(lecAtmAddress, cache->buff,
				      STATIONLEN_ATMNSAP)==0);
	if (*useCache)  
	    return NULL_LEC;   /* entry not needed */

	/* else cache invalid -- setup new entry */
	lec_info = get_lecInfo(index);
	if (lec_info==NULL || lec_info->lec_snmp==NULL)
	    return NULL_LEC;	/* no entry found */

	/* else found the entry--check if the ATM address is valid
	 * and if it matches the one specified -- only one nsap
	 * per LEC supported at this time 
	 */
	if (!verify_nsap_hwaddr(&lec_info->nsap)) 
	    return NULL_LEC;
	if (CmpOctet2CString(lecAtmAddress, lec_info->nsap.addr,
			    STATIONLEN_ATMNSAP))  {
	    /* nsap does not match, entry not found */
	    return NULL_LEC;   
	}

	/* else entry found and ATM address matches--update cache info
	   and exit */
	cache->mode = LECMIB_CACHE_GET;
	cache->serial_num = serialNum;
	cache->lec_index = index;
	cache->buff_len = STATIONLEN_ATMNSAP;
	(void)memcpy(cache->buff, lec_info->nsap.addr,
		     STATIONLEN_ATMNSAP);
	return lec_info;

    }  /* END searchtype is EXACT */

    /* else need to search for lexi-next */
    *useCache = (cache->mode==LECMIB_CACHE_GETNEXT && 
		 serialNum != -1 &&
		 serialNum==cache->serial_num &&
		 index==cache->gn_lec_index &&
		 CmpOctet2CString(lecAtmAddress,
				  cache->gn_buff,
				  cache->gn_buff_len)==0);
    if (*useCache)
	return NULL_LEC;     /* entry not needed */

    /* need to find the lexi-next entry */
    lec_info = getnext_lecInfo_by_atm_addr(index, 
				       lecAtmAddress,
				       &found_index);
    if (lec_info==NULL || lec_info->lec_snmp==NULL)
	return NULL_LEC;     /* entry not found */

    /* lexi-next entry found...fill in cache params and exit 
       don't do this for a get_entry during a set_request */
    if (serialNum != SR_GET_ON_SET_FLAG)  {
	cache->mode = LECMIB_CACHE_GETNEXT;
	cache->serial_num = serialNum;
	cache->lec_index = found_index;
	cache->buff_len = STATIONLEN_ATMNSAP;
	(void)memcpy(cache->buff, lec_info->nsap.addr,
		     STATIONLEN_ATMNSAP);
	cache->gn_lec_index = index;
	cache->gn_buff_len = min(STATIONLEN_ATMNSAP,
				 lecAtmAddress->length);
	(void)memcpy(cache->gn_buff, lecAtmAddress->octet_ptr,
		     cache->gn_buff_len);
    }
    else  {
	/* have to zap the cache for this set request */
	cache->mode = LECMIB_CACHE_INVALID;
    }
    return lec_info;
   
}   /* END find_lecInfo_by_atm_addr */



/*---------------------------------------------------------------------
 * find_lecInfo_by_mac_addr
 * try to find the lec_info struct
 * the specified MAC address is compared to the primary MAC for
 * each LEC
 * INPUTS:
 *    searchType == SR searchType (e.g. EXACT)
 *    serialNum == SR serial number of PDU
 *    index == LEC table index
 *    lecMacAddress == pointer to MAC address to use in get/getnext
 *                     search
 * INPUT/OUTPUT:
 *     cache == ptr to cache control block
 * OUTPUTS:
 *    *useCache == true if the cached entry can be used. If false
 *      then return value should be checked
 * OUTPUTS:
 *    *foundIndex == the index of the entry that was found to use,
 *       this is only set if an entry was found or needed
 * RETURNS:
 *    ptr to lec_info struct, or NULL if not found or needed
 *---------------------------------------------------------------------*/
lec_info_t *
find_lecInfo_by_mac_addr (long         searchType, 
			  int          serialNum,
			  long         index,
			  OctetString *lecMacAddress,
			  lecmib_cache_t *cache,
			  boolean        *useCache)
{
    lec_info_t  *lec_info = NULL;
    long        found_index;

    /* find the lec_info struct */
    if (searchType==EXACT)  {
	*useCache = (cache->mode==LECMIB_CACHE_GET &&
		     serialNum != -1 &&
		     serialNum==cache->serial_num &&
		     index==cache->lec_index &&
		     cache->buff_len==STATIONLEN_IEEE48 &&
		     CmpOctet2CString(lecMacAddress, cache->buff,
				      STATIONLEN_IEEE48)==0);
	if (*useCache)  
	    return NULL_LEC;   /* entry not needed */

	/* else cache invalid -- find new entry */
	lec_info = get_lecInfo(index);
	if (lec_info==NULL || lec_info->lec_snmp==NULL)
	    return NULL_LEC;	/* no entry found */

	/* else found the entry--check the NSAP first */
	if (!verify_nsap_hwaddr(&lec_info->nsap))
	    return NULL_LEC;    /* no entry found */

	/* check if the MAC address is valid
	   and if it matches the one specified -- only one nsap
	   per LEC supported at this time */
	/* always ok
	if (!verify_mac_addr(&lec_info->mac)) 
	    return NULL_LEC;
	*/

	/* compare the specified MAC to th valid MAC */
	if (CmpOctet2CString(lecMacAddress, lec_info->normalMac,
			    STATIONLEN_IEEE48))  {
	    /* MAC does not match, entry not found */
	    return NULL_LEC;   
	}

	/* else entry found and MAC address matches--update cache info
	   and exit */
	cache->mode = LECMIB_CACHE_GET;
	cache->serial_num = serialNum;
	cache->lec_index = index;
	cache->buff_len = STATIONLEN_IEEE48;
	(void)memcpy(cache->buff, lec_info->normalMac,
		     STATIONLEN_IEEE48);
	return lec_info;

    }  /* END searchtype is EXACT */

    /* else need to search for lexi-next */
    *useCache = (cache->mode==LECMIB_CACHE_GETNEXT && 
		 serialNum != -1 &&
		 serialNum==cache->serial_num &&
		 index==cache->gn_lec_index &&
		 CmpOctet2CString(lecMacAddress,
				  cache->gn_buff,
				  cache->gn_buff_len)==0);
    if (*useCache)
	return NULL_LEC;     /* entry not needed */

    /* need to find the lexi-next entry */
    lec_info = getnext_lecInfo_by_mac_addr(index, 
				       lecMacAddress,
				       &found_index);
    if (lec_info==NULL || lec_info->lec_snmp==NULL)
	return NULL_LEC;     /* entry not found */

    /* lexi-next entry found...fill in cache params and exit 
       don't do this for a get_entry during a set_request */
    if (serialNum != SR_GET_ON_SET_FLAG)  {
	cache->mode = LECMIB_CACHE_GETNEXT;
	cache->serial_num = serialNum;
	cache->lec_index = found_index;
	cache->buff_len = STATIONLEN_IEEE48;
	(void)memcpy(cache->buff, lec_info->normalMac,
		     STATIONLEN_IEEE48);
	cache->gn_lec_index = index;
	cache->gn_buff_len = min(STATIONLEN_IEEE48,
				 lecMacAddress->length);
	(void)memcpy(cache->gn_buff, lecMacAddress->octet_ptr,
		     cache->gn_buff_len);
    }
    else  {
	/* have to zap the cache for this set request */
	cache->mode = LECMIB_CACHE_INVALID;
    }
    return lec_info;

}   /* END find_lecInfo_by_mac_addr */


/*---------------------------------------------------------------------
 * find_lecInfo_by_learp_addr
 * try to find the lec_info struct
 * the specified LE-ARP MAC address is compared to the addresses in
 * the LE-ARP cache for the identified LEC
 * INPUTS:
 *    searchType == SR searchType (e.g. EXACT)
 *    serialNum == SR serial number of PDU
 *    index == LEC table index
 *    lecLeArpAddress == pointer to LE-ARP MAC address to use in get/getnext
 *                     search
 * INPUT/OUTPUT:
 *     cache == ptr to cache control block
 * OUTPUTS:
 *    *useCache == true if the cached entry can be used. If false
 *      then return value should be checked
 *    *learp_info == address of le_arp entry if one was found
 * RETURNS:
 *    ptr to lec_info struct, or NULL if not found or needed
 *---------------------------------------------------------------------*/
lec_info_t *
find_lecInfo_by_learp_addr (long         searchType, 
			  int          serialNum,
			  long         index,
			  OctetString *lecLeArpAddress,
			  lecmib_cache_t *cache,
			  boolean        *useCache,
			  lec_le_arp_t   **learp_info)
{
    lec_info_t  *lec_info = NULL;
    long        found_index;

    /* find the lec_info struct */
    if (searchType==EXACT)  {
	*useCache = (cache->mode==LECMIB_CACHE_GET &&
		     serialNum != -1 &&
		     serialNum==cache->serial_num &&
		     index==cache->lec_index &&
		     cache->buff_len==STATIONLEN_IEEE48 &&
		     CmpOctet2CString(lecLeArpAddress, cache->buff,
				      STATIONLEN_IEEE48)==0);
	if (*useCache)  
	    return NULL_LEC;   /* entry not needed */

	/* else cache invalid -- find new entry */
	lec_info = get_lecInfo(index);
	if (lec_info==NULL || lec_info->lec_snmp==NULL)
	    return NULL_LEC;	/* no entry found */

	/* else found the entry--check if this LE-ARP entry is present */
	*learp_info = get_learp_info(lec_info, lecLeArpAddress);
	if (*learp_info == NULL)
	    return NULL_LEC;    /* no entry found */

	/* found the entry in the LE-ARP cache */
	cache->mode = LECMIB_CACHE_GET;
	cache->serial_num = serialNum;
	cache->lec_index = index;
	cache->buff_len = STATIONLEN_IEEE48;
	(void)memcpy(cache->buff, (*learp_info)->mac,
		     STATIONLEN_IEEE48);
	return lec_info;

    }  /* END searchtype is EXACT */

    /* else need to search for lexi-next */
    *useCache = (cache->mode==LECMIB_CACHE_GETNEXT && 
		 serialNum != -1 &&
		 serialNum==cache->serial_num &&
		 index==cache->gn_lec_index &&
		 CmpOctet2CString(lecLeArpAddress,
				  cache->gn_buff,
				  cache->gn_buff_len)==0);
    if (*useCache)
	return NULL_LEC;     /* entry not needed */

    /* need to find the lexi-next entry */
    lec_info = getnext_lecInfo_by_learp_addr(index, 
				       lecLeArpAddress,
				       &found_index,
				       learp_info);
    if (lec_info==NULL || lec_info->lec_snmp==NULL)
	return NULL_LEC;     /* entry not found */

    /* lexi-next entry found...fill in cache params and exit 
     * don't do this for a get_entry during a set_request 
     */
    if (serialNum != SR_GET_ON_SET_FLAG)  {
	cache->mode = LECMIB_CACHE_GETNEXT;
	cache->serial_num = serialNum;
	cache->lec_index = found_index;
	cache->buff_len = STATIONLEN_IEEE48;
	(void)memcpy(cache->buff, (*learp_info)->mac,
		     STATIONLEN_IEEE48);
	cache->gn_lec_index = index;
	cache->gn_buff_len = min(STATIONLEN_IEEE48,
				 lecLeArpAddress->length);
	(void)memcpy(cache->gn_buff, lecLeArpAddress->octet_ptr,
		     cache->gn_buff_len);
    }
    else  {
	/* have to zap the cache for this set request */
	cache->mode = LECMIB_CACHE_INVALID;

    }
    return lec_info;

}   /* END find_lecInfo_by_learp_addr */


/*---------------------------------------------------------------------
 * find_lecInfo_by_ifIndex
 * try to find the lec_info struct ordered by
 * the specified lecIfIndex value -- MIB row cache not used since the
 * the mapping table contains only the lecIndex
 *
 * INPUTS:
 *    searchType == SR searchType (e.g. EXACT)
 *    ifIndex == lecIfIndex
 * RETURNS:
 *    ptr to lec_info struct, or NULL if not found
 *---------------------------------------------------------------------*/
lec_info_t * find_lecInfo_by_ifIndex (
		long         searchType, 
                long         ifIndex )
{
    lec_info_t    *lec_info, *best_lec_info;
    long          best, current = 0;

    /* linear search of all LECs since the snmp index isn't
     *  ordered--snmp access routine in sys/snmp does a linear
     *  search of all IDBs; + no support yet for sub-itfs 
     */
    switch (searchType)  {
    case EXACT:
	if (ifIndex < 1)
	    return NULL;
	FOR_ALL_LEC(lec_info)   {
	    if (lec_info->lec_snmp==NULL)
	    	continue;
	    if (get_lec_ifIndex(lec_info) == ifIndex)
		return lec_info;
	}
	return NULL;
    case NEXT:
	/* SNMPR get-next is >= parameter value */
	best = MAXLONG;
	best_lec_info = NULL;
	FOR_ALL_LEC(lec_info)   {
	    if (lec_info->lec_snmp==NULL)
		continue;
	    current = get_lec_ifIndex(lec_info);
	    if (current == ifIndex)  
		    return lec_info;   /* won't find better next */
	    else if (current > ifIndex && current < best )  {
		best = current;
		best_lec_info = lec_info;
	    }  /* END update best */
	}  /* END for all lec */
	return best_lec_info;
    default:    
	return NULL;     /* should not happen */
    }
    /*NOTREACHED*/
}  /* find_lecInfo_by_ifIndex */



/*---------------------------------------------------------------------
 * free_lec_snmp_info
 *
 * release all lec_snmp resources
 * INPUTS:
 *    lec_info == LEC containing the lec_snmp ptr
 *---------------------------------------------------------------------*/
static inline void free_lec_snmp_info (lec_info_t  *lec_info)
{
    if (lec_info && lec_info->lec_snmp)  {
	free(lec_info->lec_snmp);
	lec_info->lec_snmp = NULL;
    }
}

/*---------------------------------------------------------------------
 * alloc_lec_snmp_info
 *
 * malloc the lec_snmp resources
 * INPUTS:
 *    lec_info == LEC containing the lec_snmp ptr
 * RETURNS:
 *    lec_snmp ptr will be non-NULL if the malloc succeeded
 *        NULL otherwise
 *---------------------------------------------------------------------*/
static void alloc_lec_snmp_info (lec_info_t  *lec_info)
{
    lec_snmp_info_t   *lec_snmp;

    /* 
     * initialize SNMP parameters 
     */
    if (!lec_info->lec_snmp) {
	/* create a new LEC SNMP block */
	lec_snmp = malloc(sizeof(lec_snmp_info_t));
	if (!lec_snmp) {
	    return;
	}
	lec_info->lec_snmp = lec_snmp;
    }
    else
	lec_snmp = lec_info->lec_snmp;

    /* either we malloced a struct or we are re-initializing the
     * vars in a previously malloced struct...
     *
     * init lec_snmp vars:
     * the owner string needs to be saved in NVRAM and reset to the
     * saved value, if this LEC  is being recreated from the config
     * If created by VTP, then the owner should be "monitor-vtp"
     * If created by CLI, then the owner should be "monitor-cli"
     * If created by SNMP, then the owner should be the value of
     * lecOwner as it was set. This is an ASCIIZ string.
     * TEMP -- set owner to the CLI 
     */
    (void)strcpy(lec_snmp->lec_owner, "monitor-cli");

    /* this has to be filled in as the LEC is activated 
     * Same situation as owner string...the config source 
     * has to be saved as the LEC state machine runs...this
     * will be known by the time the LEC is active...
     * flag as 'did not use LECS' as starting enum
     */
    lec_snmp->lec_config_source = LEC_CONFIG_TYPE_NO_LECS;

    /* the config mode should also be saved in NVRAM if this entry was
     * created by SNMP, otherwise it will always be auto mode (DEFVAL)
     */
    lec_snmp->lec_config_mode_auto = TRUE;

    /* these failure codes need to be set by the LEC FSM. If there
     * is no error, then these default values will be valid later
     */
    lec_snmp->last_fail_resp_code = LANE_STATUS_SUCCESS;
    lec_snmp->last_fail_state = LEC_ST_IDLE;

    /*** temp -- need to save this in the config file when
     *** SNMP LEC creation supported there.
     *** need to tell this function if this LEC is created because:
     ***    it is being recreated from NVRAM
     ***    it is being created for the first time from CLI
     ***    it is being created because of VTP
     *** a write MEM should save the SNMP LEC index in all cases
     *** along with all the other SNMP vars
     */

    lec_snmp->lec_snmp_index = reg_invoke_lecmib_get_lec_index(lec_info); 

    /* Fall back in case index registry not provided. */
    if (!lec_snmp->lec_snmp_index) {
	lec_snmp->lec_snmp_index = next_snmp_lec_index++;      /* TEMP */
    }

    /* should get set to TRUE when IF-MIB register call is done */
    lec_snmp->ifmib_enabled = FALSE;

    lec_snmp->if_index = ID_NOT_FOUND;

    /* The LEC MIB requires that counters be maintained for all
     * VCs, since the join phase completed. As SVCs are deleted,
     * their IF-MIB counters are added to the deleted-ifmib totals
     */
    lec_snmp->deleted_in_octets = 0;            /* bits 0..31 */
    lec_snmp->deleted_in_octets_HC = 0;         /* bits 32..63 */
    lec_snmp->deleted_in_ucast_pkts = 0;
    lec_snmp->deleted_in_mcast_pkts = 0;
    lec_snmp->deleted_in_bcast_pkts = 0;
    lec_snmp->deleted_in_unknown_protos = 0;
    lec_snmp->deleted_in_errors = 0;
    lec_snmp->deleted_in_discards = 0;
    lec_snmp->deleted_out_octets = 0;            /* bits 0..31 */
    lec_snmp->deleted_out_octets_HC = 0;         /* bits 32..63 */
    lec_snmp->deleted_out_ucast_pkts = 0;
    lec_snmp->deleted_out_mcast_pkts = 0;
    lec_snmp->deleted_out_bcast_pkts = 0;
    lec_snmp->deleted_out_errors = 0;
    lec_snmp->deleted_out_discards = 0;

    /* a cache of IF-MIB counters is kept to improve performance
     * and help insure the counter values are consistent.
     * The entire set of IF-MIB counters is collected once per
     * PDU, by caching the PDU serial number
     */
    lecmib_clear_cur_if_cntrs(lec_snmp);

}  /* alloc_lec_snmp_info */


/*---------------------------------------------------------------------
 * lecmib_register_lec_info
 *
 * Registry callback to setup IF-MIB and LEC-MIB for a specified 
 * LEC
 * INPUTS:
 *    lec_info_mask == lec_info ptr cast as void *
 *---------------------------------------------------------------------*/
void lecmib_register_lec_info (void *lec_info_mask)
{
    lec_info_t   *lec_info = (lec_info_t *) lec_info_mask;

    /* first malloc the lec_snmp data */
    alloc_lec_snmp_info(lec_info);

    /* check if malloc worked...if it failed then
     * SNMP is disabled for this LEC; don't want to cause
     * the LEC to fail in any way, SNMP just not present
     * for this LEC
     */
    if (lec_info->lec_snmp==NULL)   {
	return;
    }

    /* now call the IF-MIB code and register the LEC interface
     * this function should be called before a CONFIGURE_REQUEST
     * is sent by the LEC
     * It is assumed (at this time) that the LEC is being created
     * by the CLI or VTP; not SNMP; so the default ifAdminStatus
     * is up; when SNMP LEC creation is done, the 2nd parm cannot
     * be hardwired below
     */
    lecmib_if_register(lec_info, TRUE);        /*???*/

    /* check if ifEntry exists for AAL-5 layer */
    /*???*/

    /* get the ATM interface info to build the stack table */
    /*???*/

} /* lecmib_register_lec_info */


/*---------------------------------------------------------------------
 * lecmib_complete_lec_info
 *
 * Registry callback to complete the IF-MIB and LEC-MIB for a 
 * specified LEC. Called after ok JOIN_RESP received.
 * INPUTS:
 *    lec_info_mask == lec_info ptr cast as void *
 *---------------------------------------------------------------------*/
void lecmib_complete_lec_info (void *lec_info_mask)
{
   lec_info_t   *lec_info = (lec_info_t *) lec_info_mask;

   /* check if first-pass create LEC failed */
   if (lec_info->lec_snmp == NULL)
       return;

    lecmib_if_complete_entry(lec_info);
}  /* lecmib_complete_lec_info */

/*---------------------------------------------------------------------
 * lecmib_deregister_rcv_addr 
 *
 * Registry callback to remove a registered receive address from the
 * IF-MIB. Called when LEC unregisters addresses (which at this time
 * is only as a result of termination).
 * INPUTS:
 *    lec_info_mask == lec_info ptr cast as void *
 *---------------------------------------------------------------------*/
void lecmib_deregister_rcv_addr (void* lec_info_mask) 
{
    lec_info_t *lec_info;

    lec_info = lec_info_mask;

    if (lec_info->lec_snmp == NULL)
	return;

    lecmib_if_deregister_rcv_addr(lec_info);
}

/*---------------------------------------------------------------------
 * lecmib_deregister_lec_info
 *
 * Registry callback to destroy the IF-MIB and LEC-MIB for a 
 * specified LEC. Called before lec_info is destroyed.
 * Not called if LEC just returns to initial state.
 * INPUTS:
 *    lec_info_mask == lec_info ptr cast as void *
 *---------------------------------------------------------------------*/
void lecmib_deregister_lec_info (void *lec_info_mask)
{
    lec_info_t   *lec_info = (lec_info_t *) lec_info_mask;

    if (lec_info->lec_snmp)   {
 	lecmib_if_deregister(lec_info);
	free_lec_snmp_info(lec_info);
    }
}  /* lecmib_deregister_lec_info */

/* END lecmib_util.c */


      
