/* $Id: clns_misc.c,v 3.3.60.6 1996/08/08 14:50:24 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_misc.c,v $
 *------------------------------------------------------------------
 * clns_misc.c -- support routines for clns
 *  
 * Feb-8-1989 by Jim Forster
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_misc.c,v $
 * Revision 3.3.60.6  1996/08/08  14:50:24  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.3.60.5  1996/08/07  08:57:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/06/25  00:27:17  mschaef
 * CSCdi37797:  NLSP uses different next hop address than expected on WANs
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/05/22  20:54:09  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.3.60.2  1996/04/25  23:03:24  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.60.1.2.1  1996/03/24  03:46:29  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:02:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:13:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  13:40:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:52:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/21  19:02:52  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.3  1995/10/03  22:58:15  rchandra
 * CSCdi39069:  Route entries all hash to same bucket
 * - Use the last 4 bytes of the NSAP for hashing
 * - include the code-review changes for CSCdi38153
 * - rebuild both L1 and L2 LSPs when redistribute command is entered
 *
 * Revision 2.2  1995/06/30  23:39:47  dbath
 * CSCdi35958:  show clns routes command fields not wide enough for slots
 * 10,11...
 * Make show 'clns route', 'clns neigh', 'clns is-neigh',
 * 'clns es-neigh' and 'isis route' all use short names for interfaces
 * to make room for 2 digit slot numbers (should also help for FEIP).
 *
 * Revision 2.1  1995/06/07  20:17:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_registry.h"
#include "../ip/tunnel.h"



/*
 * Local Storage
 */

#define NSTRINGBUFS 5
static char string_buf[NSTRINGBUFS][3*CLNS_MAX_NSAP_LENGTH];
#define NLONGSTRINGBUFS 2
static char long_string_buf[NLONGSTRINGBUFS][10*MAX_CLNS_ADDR_LEN+10];
static int which_buf = 0;
static int which_long_buf = 0;

/*
 * next_buf
 */

char *next_buf (void)
{
    which_buf = (++which_buf) % NSTRINGBUFS;
    return(string_buf[which_buf]);
}

/*
 * next_long_buf
 */

static char *next_long_buf (void)
{
    which_long_buf = (++which_long_buf) % NLONGSTRINGBUFS;
    return(long_string_buf[which_long_buf]);
}

/*
 * clear_nsap
 * clear out the nsap that is passed in
 */

void clear_nsap (NSAP_T nsap)
{
    int i;

    for (i = 0; i < CLNS_MAX_NSAP_LENGTH; i++) {
        *nsap++ = (uchar) 0;
    }
}

/*
 * nsap_copy
 */
void nsap_copy (NSAP_T ns_src, NSAP_T ns_dst)
{
    int i;
    register int len;

    if ((!ns_src) || (!ns_dst)) {
	return;
    }
    len = ns_src[0] + 1;
    if (len > CLNS_MAX_NSAP_LENGTH) {
	return;
    }
    for (i=0; (i < len); i++) {
       *ns_dst++ = *ns_src++;
   }
}

/* 
 * prefix_copy
 *
 * Copy an IS-IS encoded prefix to an NSAP encoded entry. The prefix
 * has the length encoded in semi-octets. The NSAP_T has an NSAP address
 * encoded with an octet length. 
 *
 * For prefixes when an odd number of semi-octets, a pad nibble of 0 is
 * inserted.
 */
void prefix_copy (NSAP_T prefix, NSAP_T nsap)
{
    int byte_len;

    /*
     * Make length in unit of octets.
     */
    byte_len = *prefix;
    byte_len >>= 1;

    bcopy(&prefix[1], &nsap[1], byte_len);
    nsap[0] = byte_len;

    /*
     * Odd number of semi-octets, pad last nibble with 0.
     */
    if (*prefix & 1) {
	nsap[0]++;
	nsap[nsap[0]] = (prefix[byte_len+1] & 0xf0);
    }
}

/*
 * station_copy
 */
void station_copy (uchar *ns_src, uchar *ns_dst)
{
    int i;
    
    for (i = 0; i < STATION_LENGTH; i++) {
	*ns_dst++ = *ns_src++;
    }
}

/*
 * systemid_copy
 */
void systemid_copy (uchar *src, uchar *dst)
{
    int i;

    for (i = 0; i < SYSTEMID_LENGTH; i++) {
	*dst++ = *src++;
    }
}

/*
 * area_copy
 *
 * Copy an IS-IS area address to another memory location. Accepts
 * input as nsap arrays and copies all but last 7 bytes. The 
 * length of the dest parameter will reflect the number of octets
 * in area address.
 *
 */
void area_copy (NSAP_T src, NSAP_T dest)
{

    int len, i;

    len = src[0] - SYSTEMID_LENGTH;
    
    for (i = 1; i < len+1; i++) {
	dest[i] = src[i];
    }
    dest[0] = len;
}

/*
 * Format a single nibble of a masked NSAP.  Returns the updated
 * character pointer.
 */

#define CVBTOH(x) ((x) + ((x) > 9 ? 'a'-10 : '0'))
static boolean in_bits = FALSE;

static char *format_masked_nibble (
    register int len,
    register uchar nibble,
    register uchar mask,
    register char *outptr,
    boolean last)
{
    register int i;

    if (mask == 0xf && len == 4) {
	if (in_bits) {
	    *outptr++ = ')';
	    in_bits = FALSE;
	}
	*outptr++ = CVBTOH(nibble);
    } else if (mask == 0 && len == 4) {
	if (in_bits) {
	    *outptr++ = ')';
	    in_bits = FALSE;
	}
	*outptr++ = '*';
    } else {
	if (in_bits) {
	    *outptr++ = '.';
	} else {
	    *outptr++ = '(';
	    in_bits = TRUE;
	}
	for (i=0; i<len; i++) {
	    register int bitmask = 1 << (3 - i);
	    if (mask & bitmask) {
		*outptr++ = '0' + ((nibble & bitmask) != 0);
	    } else {
		*outptr++ = '*';
	    }
	}
    }
    if (last && in_bits) {
	in_bits = FALSE;
	*outptr++ = ')';
    }
    return(outptr);
}



/*
 * Format a substring of a masked NSAP.  The length is in bits.
 */
static char *masked_nsap_substring (
    register int len,
    register uchar *addr,
    register const uchar *mask,
    register char *outptr,
    boolean odd_dots)
{
    register uchar nibble;
    register uchar masknibble;
    register int i;

    for (i=0; i<len; i += 8) {
	register boolean lastnibble;
	register int bitsleft;

	if (!in_bits && (
			 (odd_dots && (i & 8)) || 
			 (!odd_dots && i && !(i & 8))))
	    *outptr++ = '.';

	bitsleft = len - i;
	if (bitsleft > 4) {
	    bitsleft = 4;
	    lastnibble = FALSE;
	} else {
	    lastnibble = TRUE;
	}

	nibble = *addr >> 4;
	masknibble = *mask >> 4;
	outptr = format_masked_nibble(bitsleft, nibble, masknibble,
				      outptr, lastnibble);

	if (!lastnibble) {
	    bitsleft = len - i - 4;
	    if (bitsleft > 4) {
		bitsleft = 4;
		lastnibble = FALSE;
	    } else {
		lastnibble = TRUE;
	    }
	    
	    nibble = *addr++ & 0x0f;
	    masknibble = *mask++ & 0x0f;
	    outptr = format_masked_nibble(bitsleft, nibble, masknibble,
					  outptr, lastnibble);
	}
    }
    return(outptr);
}
/*
 * Format an NSAP address.
 */
static const uchar dummy_mask[20] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
				      0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
				      0xff,0xff,0xff,0xff };

char *nsap_addr_string (nsap_addr *nsap)
{
    register char *outptr;
    register int len;
    char *string;

    if (!nsap) {
	return("");
    }
    len = nsap->length;

    string = outptr = next_buf();

    if (len == 0) {
	return("default");
    }

    if (len > MAX_CLNS_ADDR_LEN) {
	(void) sprintf(outptr, "** bad nsap length %d **", len);
	return(outptr);
    }
    outptr = masked_nsap_substring(len * 8, nsap->addr, dummy_mask,
				   outptr, TRUE);
    *outptr = '\0';
    return(string);
}


/*
 * nsap_pfx_string - form a string of an NSAP prefix.
 */
char *nsap_pfx_string (nsap_pfx *pfx, boolean print_dots)
{
    register char *outptr;
    char *string;
    register int len = pfx->length;

    if (!pfx) {
	return("");
    }

    /* Special-case the zero length prefix. */

    if (len == 0) {
	return("default");
    }

    string = outptr = next_buf();

    if (len > MAX_CLNS_ADDR_LEN * 8) {
	(void) sprintf(outptr, "** bad nsap prefix length %d **", len);
	return(outptr);
    }

    outptr = masked_nsap_substring(pfx->length, pfx->prefix, dummy_mask, 
				   outptr, TRUE);

    /* If the caller wants trailing dots, add them. */

    if (print_dots) {
	*outptr++ = '.';
	*outptr++ = '.';
	*outptr++ = '.';
    }
    *outptr = '\0';
    return(string);
}

/*
 * masked_nsap_string - form a string of a masked NSAP.
 */
char *masked_nsap_string (masked_nsap *mskd)
{
    register char *outptr;
    register int offset;
    char *string;

    if (!mskd) {
	return("");
    }

    if (!(mskd->leftlength + mskd->rightlength)) {
	return("default");
    }

    string = outptr = next_long_buf();

    if (mskd->leftlength + mskd->rightlength > MAX_CLNS_ADDR_LEN * 8) {
	(void) sprintf(outptr, "** bad masked nsap lengths (%d, %d) **",
		       mskd->leftlength, mskd->rightlength);
    }

    /* Start with the left side. */

    outptr = masked_nsap_substring(mskd->leftlength, mskd->addr, mskd->mask,
				   outptr, TRUE);

    /* If there's an inexact match, add a "..." */

    if (!mskd->exact) {
	*outptr++ = '.';
	*outptr++ = '.';
	*outptr++ = '.';
    }

    /*
     * Now do the right side.  It starts at the next octet boundary after
     * the left side.
     */
    offset = (mskd->leftlength + 7) / 8;
    outptr = masked_nsap_substring(mskd->rightlength * 8, mskd->addr + offset,
				   mskd->mask + offset, outptr, 
				   ((mskd->rightlength & 1) == 0));
    *outptr = '\0';
    return(string);
}


/*
 * nsap_string
 * convert an nsap to a string in the standard fashion
 *
 * Just call the new routine.
 */

char *nsap_string (NSAP_T nsap)
{
    return(nsap_addr_string(NSAP_T_to_nsap_addr(nsap)));
}


/*
 * prefix_string
 *
 * Prints an IS-IS encoded address prefix.
 *
 */
char *prefix_string (NSAP_T prefix)
{

    NSAP_T nsap;

    /*
     * Copy prefix formatted address into NSAP formatted entry.
     */
    prefix_copy(prefix, nsap);
    return((nsap[0]) ? nsap_string(nsap) : "Default Prefix");

}

/*
 * nsap_hash
 *
 * Compute the hash function on a clns nsap.  Hash function
 * is the ip hash function (xor the bytes), adapted to the
 * variable length nsap.  We only use the last four bytes
 * of the address. since they have the highest variability.  
 * For addresses of length four or less, we use all bytes of 
 * the address.
 *
 * Compute the hash function on a clns nsap.  Hash function
 * is the ip hash function (xor the bytes), adapted to the
 * variable length nsap.
 *
 * Note--this routine must match fast_nsap_hash in clns_inlines.h!!
 */


int nsap_hash (int length, uchar *nsap)
{
    int hash;
    if (length < CLNS_MAX_NSAP_LENGTH && length >= 4) {
	nsap += length - 4;
	length = 4;
    } else if (length >= CLNS_MAX_NSAP_LENGTH || length <= 0) {
	return(0);
    }
    hash = 0;
    while (length--) {
	hash ^= *nsap++;
    }
    return(hash);
}

/*
 * snpa_string
 * Convert an snpa to a string in standard form
 */

char *snpa_string (hwaddrtype *snpa)
{
    char *ptr;

    if (!snpa) {
	return("null snpa addr passed!");
    }
    ptr = next_buf();

    print_snpa_string(ptr, snpa);

    return(ptr);
}

/*
 * nsap_equ
 * Test two nsaps for equality
 */

boolean nsap_equ (NSAP_T nsap1, NSAP_T nsap2)
{
    int len;
    int i;

    if ((nsap1[0] >= CLNS_MAX_NSAP_LENGTH) ||
	(nsap2[0] >= CLNS_MAX_NSAP_LENGTH)) {
	return(FALSE);
    }
	
    len = (int) *nsap1 + 1;
    for (i = 0; i < len; i++) {
	if (*nsap1++ != *nsap2++) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * net_equ
 *
 *    Compare two nsaps and return true if the 'net's are equal
 *    (Don't pay attention to the selector byte).
 */
boolean net_equ (NSAP_T nsap1, NSAP_T nsap2)
{
    int len;
    
    if (!nsap1 || !nsap2 ||
	(nsap1[0] > MAX_CLNS_ADDR_LEN)) {
	return(FALSE);
    }

    len = *nsap1;
    while (len--) {
	if (*nsap1++ != *nsap2++) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * nsap_match
 * Compare an nsap-prefix and a destination nsap.
 * If all of the bytes of the nsap_prefix match the leading bytes of the
 * destination nsap, then return the length of the nsap-prefix.  Otherwise
 * return 0.
 */

int nsap_match (NSAP_T nsap_prefix, NSAP_T dst_nsap)
{
    int len;
    int i;

    len = *nsap_prefix++;
    if (len > *dst_nsap++) {
	return(0);
    }  else {
	for (i = 0; i < len; i++) {
	    if (*nsap_prefix++ != *dst_nsap++) {
		return(0);
	    }
	}
	return(len);
    }
}

/*
 * systemid_match
 *
 */
boolean systemid_match (uchar *one, uchar *two)
{
    int i;

    if (!one || !two) {
	return(FALSE);
    }

    for (i = 0; i < SYSTEMID_LENGTH; i++) {
	if (*one++ != *two++) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * clns_address_lookup
 * Given a clns host address,return pointer to a name cache entry.
 * No revalidation done here.
 * return NULL if name string for address not found.
 */
char *clns_address_lookup (NSAP_T nsap)
{
    nametype *ptr;
    addrtype addr;

    if (nsap[0] == '\0') {
	return(NULL);
    }

    addr.type = ADDR_CLNS;
    addr.length = ADDRLEN_CLNS;
    nsap_copy(nsap, addr.addr.clns_address);
    ptr = name_cache_lookup(NULL, &addr, NAM_CLNS);
    if (ptr) {
	return(ptr->name);
    }
    return(NULL);
}

/*
 * clns_stationid_lookup
 *
 * Given a 6-byte station id return a pointer to a name cache entry.
 * If the name is not found, return the stationid_string.
 *
 * If the pdb pointer is non-null and name lookups are disabled,
 * we only return the stationid string.
 */
char *clns_stationid_lookup (clns_pdbtype *pdb, uchar *stationid)
{
    nametype *name_entry;
    addrtype  addr;
	
    if (!pdb || !pdb->name_lookup_disabled) {
	addr.type = ADDR_CLNS;
	addr.length = STATION_LENGTH;
	
	station_copy(stationid, addr.addr.clns_address);
	name_entry = name_cache_lookup(NULL, &addr, NAM_CLNS);
	if (name_entry) {
	    return(name_entry->name);
	}
    }
    return(station_string(stationid));
}

/*
 * get_area
 *
 */
uchar *get_area (NSAP_T nsap)
{
    char area_spot;

    area_spot = nsap[0] - STATION_LENGTH - AREA_LENGTH;
    return(&nsap[area_spot]);
}

/*
 * get_station
 *
 */
uchar *get_station (NSAP_T nsap)
{
    char station_spot;

    station_spot = nsap[0] - STATION_LENGTH;
    return(&nsap[station_spot]);
}

/*
 * get_selector
 *
 *  Return the selector for the specified nsap.
 *  NOTE that this routine returns the actual selector. Not
 *  a pointer to the selector.
 */
uchar get_selector (NSAP_T nsap)
{
    return(nsap[nsap[0]]);
}

/*
 * area_string
 *
 */
char *area_string (uchar area_id[AREA_LENGTH])
{
    int i;
    char *ptr, *p2;
    uchar nibble;

    ptr = next_buf();
    p2 = ptr;
    for (i = 0; i < AREA_LENGTH; i++) {
	nibble = area_id[i] >> 4;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	nibble = area_id[i] & 0x0f;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
    }
    *ptr = '\0';
    return(p2);
}

/*
 * long_area_string
 *
 * Produce IS-IS area string.
 */
char *long_area_string (NSAP_T area_addr)
{
    char *string;

    area_addr[0] -= (STATION_LENGTH + SELECTOR_LENGTH);
    string = nsap_string(area_addr);
    area_addr[0] += (STATION_LENGTH + SELECTOR_LENGTH);
    return(string);
}

/*
 * station_string
 *
 */
char *station_string (uchar station_id[STATION_LENGTH])
{
    char  *ptr, *p2;
    int   i;
    uchar nibble;

    ptr = next_buf();
    p2 = ptr;
    for (i = 0; i < STATION_LENGTH; i++) {
	if (i && !(i % 2) && ((i + 1) < STATION_LENGTH)) {
	    *ptr++ = '.';
	}
	nibble = station_id[i] >> 4;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	nibble = station_id[i] & 0x0f;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
    }
    *ptr = '\0';
    return(p2);
}

/*
 * systemid_string
 *
 * Produces a string in the form xxxx.xxxx.xxxx.yy which correpsonds
 * to an IS-IS system id.
 *
 */
char *systemid_string (uchar system_id[SYSTEMID_LENGTH])
{
    char  *ptr, *p2;
    int   i;
    uchar nibble;

    ptr = next_buf();
    p2 = ptr;
    for (i = 0; i < SYSTEMID_LENGTH; i++) {
	if (i && !(i % 2)) {
	    *ptr++ = '.';
	}
	nibble = system_id[i] >> 4;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	nibble = system_id[i] & 0x0f;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
    }
    *ptr = '\0';
    return(p2);
}

/*
 * systemid_name_string
 *
 * If a system id exists in the name cache then the following string
 * is returned:
 *
 * cisco.yy     where yy is the pseudo id. 
 *
 * If the system id does not exist in the name cache, systemid_string ()
 * equivalent is returned:
 * 
 * xxxx.xxxx.xxxx.yy
 */
char *systemid_name_string (clns_pdbtype *pdb, uchar *systemid)
{
    char *str, *buf;
    clns_station_lookup_type lookup_proc;

    lookup_proc = NULL;

    /*
     * Get 6-byte station id name string, if exists.
     * Otherwise, return 6-byte address in ascii.
     *
     * If the PDB pointer is NULL, use the CLNS version.
     */

    if (pdb)
	lookup_proc = pdb->stationid_lookup;

    if (!lookup_proc)
	lookup_proc = clns_stationid_lookup;

    str = (*lookup_proc)(pdb, systemid);

    /*
     * Build pseudo id.
     */
    buf = next_buf();
    sstrncpy(buf, str, STATION_STRING_LENGTH+1);
    sprintf(buf + strlen(buf), ".%02x", systemid[SELECTOR_ID_OFFSET]);
    return(buf);
}


/*
 * station_match
 *
 *    return true if the stations are equal
 */
boolean station_match (uchar *station1, uchar *station2)
{
    int i;

    for (i = 0; i < STATION_LENGTH; i++) {
	if (*station1++ != *station2++) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 *  return true if the interface is in the specified domain/area
 */
boolean itf_in_pdb (idbtype *interface, clns_pdbtype *pdb)
{
    idbqueuetype *idbq;

    for (idbq = pdb->idbqueue; idbq; idbq = idbq->next) {
	if (interface == idbq->idb) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * clns_remove_idb_from_queue
 *
 *   Remove the specified interface from the idbqueue
 */
void clns_remove_idb_from_queue (
    clns_pdbtype *pdb,
    idbtype      *idb)
{
    boolean match;
    idbqueuetype *idbqueue,*prev;
    
    match = FALSE;
    idbqueue = pdb->idbqueue;
    prev = NULL;
    while (idbqueue) {
	if (idbqueue->idb == idb) {
	    match = TRUE;
	    break;
	}
	prev = idbqueue;
	idbqueue = idbqueue->next;
    }
    if (match){
	if (!prev){
	    pdb->idbqueue = idbqueue->next;
	} else {
	    prev->next = idbqueue->next;
	}
	free(idbqueue);
    }
}

/*
 * clns_add_if_to_pdbqueue
 *
 * Add an interface to the PDB queue.  Assumes that it's not already
 * there.
 *
 * Returns a pointer to the queue entry, or NULL if failure.
 */
idbqueuetype *clns_add_if_to_pdbqueue (idbtype *idb, clns_pdbtype *pdb)
{
    idbqueuetype *idbqueue;

    idbqueue = malloc(sizeof(idbqueuetype));
    if (idbqueue) {
	idbqueue->next = pdb->idbqueue;
	idbqueue->idb = idb;
	pdb->idbqueue = idbqueue;
    }
    return(idbqueue);
}

/*
 * build_nsap
 *
 */
void build_nsap (int domain_length, uchar *domain, uchar *area,
		 uchar *station, uchar *selector, NSAP_T *nsap)
{
    int len;

    len = 0;
    clear_nsap(*nsap);
    if (domain) {
	len = domain_length;
	bcopy(domain, &(*nsap)[1], domain_length);
    }
    if (area) {
	bcopy(area, &(*nsap)[len+1], AREA_LENGTH);
	len += AREA_LENGTH;
    }
    if (station) {
	bcopy(station, &(*nsap)[len+1], STATION_LENGTH);
	len += STATION_LENGTH;
    }
    if (selector) {
	bcopy(selector, &(*nsap)[len+1], SELECTOR_LENGTH);
	len += SELECTOR_LENGTH;
    }
    *nsap[0] = len;
}

/*
 * clns_show_one_neighbor
 *
 * Display clns adjacency database.
 *
 */
void clns_show_one_neighbor (clns_pdbtype *pdb, clns_adjtype *adj, char kind,
			     boolean long_display, boolean indent)
{

    char   *str;
    int    t, i;
    clns_adjtype *next_alias;
    clns_station_lookup_type lookup_proc;

    lookup_proc = NULL;

    /* If the PDB pointer is NULL, use the CLNS lookup routine. */

    if (pdb)
	lookup_proc = pdb->stationid_lookup;

    if (!lookup_proc)
	lookup_proc = clns_stationid_lookup;

    while (adj) {

	/*
	 * Don't display routes which reside in the adjacency table.
	 */
	if (adj->adjacencyType == CLNS_PREFIX_ADJ_TYPE)
	    return;

	mem_lock(adj);
	t = 0;
	if (TIMER_RUNNING(adj->expiration_time)) {
	    t = TIME_LEFT_SLEEPING(adj->expiration_time);
	}

	/* 
	 * Display Station ID. 
	 */
	str = (*lookup_proc) (pdb, &adj->key[1]);
	if (indent)
	    printf("\n   %15s",  str);
	else
	    printf("\n%15s",  str);

	if (kind == 'A') {

	    /* 
	     * Display SNPA. 
	     */
	    if (adj->idb->hwptr->tunnel) {
		printf("%20i", adj->idb->hwptr->tunnel->destination);
	    } else {
		printf("%20s", snpa_string(&adj->snpa));
	    }
	}

	printf("%12s", adj->idb->short_namestring);

	/* 
	 * Display adjacency state. 
	 */
	switch (adj->state) {
	case CLNS_ADJ_INIT_STATE:
	    printf("%7s", "Init");
	    break;

	case CLNS_ADJ_UP_STATE:
	    if (!interface_up(adj->idb)) {
		printf("%7s", "Down");
	    } else {
		printf("%7s", "Up");
	    }
	    break;

	case CLNS_ADJ_FAILED_STATE:
	    printf("%7s", "Failed");
	    break;
	default:
	    printf("%7s", "Unknown");
	}

	if (kind == 'A' || kind == 'N') {

	    /* 
	     * Display Holdtime. 
	     */
	    if (!(adj->protocol_source & ISO_STATIC)) {
		printf("%10d", t/ONESEC);
	    } else {
		printf("%10s", "--");
	    }
	}

	/* 
	 * Display system type - ES or IS. 
	 */
	if (kind != 'N') {
	    switch (adj->adjacencyType) {
	      case CLNS_ES_ADJ_TYPE:
		printf("%5s", "ES");
		break;
	      case CLNS_IS_ADJ_TYPE:
		if (adj->protocol_source & ISO_IGRPMASK) {
		    printf("%5s", (adj->L2only) ? "L2" : "L1L2");
		} else {
		    printf("%5s", "IS");
		}
		break;
	      case CLNS_L1_IS_ADJ_TYPE:
		printf("%5s", "L1");
		break;
	      case CLNS_L2_IS_ADJ_TYPE:
		printf("%5s", (adj->L2only) ? "L2" : "L1L2");
		break;
	      default:
		printf("%5s", "?");
		break;
	    }
	}

	if (kind == 'A') {

	    /* 
	     * Display protocol source. 
	     */
	    reg_invoke_clns_show_protocol_source(adj->protocol_source);
	}

	if (kind == 'I' || kind == 'N') {

	    /* 
	     * Display LAN Priority. 
	     */
	    if (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE && !adj->L2only) {
		printf("%2d/%7d", adj->L1_lan_priority, adj->L2_lan_priority);
	    } else {
		printf("%10d", (adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE) ?
		       adj->L2_lan_priority : adj->L1_lan_priority);
	    }

	    /* 
	     * Display circuit id (LAN Id). 
	     */
	    if (is_p2p(adj->idb)) {
		printf("%02x%17s", 
		(adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE && adj->L2only) ?
			adj->L2_circuit_id[0] : adj->L1_circuit_id[0], " ");
	    } else {
		printf("%19s", 
		(adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE && adj->L2only) ?
			systemid_name_string(pdb, adj->L2_circuit_id) :
			systemid_name_string(pdb, adj->L1_circuit_id));
	    }
	}

	if (kind != 'A' && kind != 'N') {

	    /* 
	     * Display DECNET Phase IV or Phase V. 
	     */
	    printf("%s", (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) ?
		   "Phase IV" : "Phase V");
	}

	if (long_display) {

	    /* 
	     * Display area addresses and IP addresses. 
	     */
	    for (i = 0; i < adj->num_areas; i++) {
		if (!i) printf("\n  Area Address(es):");
		printf(" %n", adj->area_address[i]);
	    }
	    for (i = 0; i < MAXROUTES && adj->ip_address[i]; i++) {
		if (!i) printf("\n  IP Address(es): ");
		printf(" %i%s", adj->ip_address[i],
		       (adj->ip_address[i] == adj->ip_nexthop) ? "*" : "");
	    }
	    /*
	     * Display IPX address and areas.
	     */
	    if (kind == 'N') {
	        printf("\n  IPX Address: %x.%e", adj->ipx_net, 
		       adj->ipx_node);
		if (adj->ipx_area_count)
		    printf("\n  IPX Areas:");
		for (i=0; i<adj->ipx_area_count; i++) {
		    printf("  %08x/%08x", adj->ipx_area_net[i], 
			   adj->ipx_area_mask[i]);
		}
	    }
	    /*
	     * Print info on how long this adjcacency has been up.
	     */
	    printf("\n  Uptime: %TE", adj->create_time);
	}
        next_alias = adj->next_alias;
        free (adj);
        adj = next_alias;
    }
}

/*
 * show_clns_neighbors
 *    
 * Process show neighbor command. The following commands are supported by
 * this function:
 * 
 * 	show clns neighbors [<interface name>] [Detail]
 *      show clns is-neighbors [<interface name>] [Detail]
 *      show clns es-neighbors [<interface name>] [Detail]
 *      show ipx nlsp neighbors [<interface name>] [Detail]
 */

const char nbr_hdr[] = { "\nSystem Id      SNPA                Interface   State  "
		      "Holdtime  Type Protocol" };
static const char is_nbr_hdr[] = { "\nSystem Id      Interface   State  Type "
				"Priority  Circuit Id         Format" };
static const char es_nbr_hdr[] = { "\nSystem Id      Interface   State  Type "
				"Format" };
static const char nlsp_nbr_hdr[] = { "\nSystem Id      Interface   State  Holdtime  "
				  "Priority  Circuit Id" };

void show_clns_neighbors (clns_pdbtype *pdb, clns_adjtype **adj_db, char kind,
			  idbtype *target, boolean long_display)
{
    clns_adjtype *adj, *next_adj;
    int          bucket;
    const char   *str = NULL;

    if (kind == 'A') {
	str = nbr_hdr;
    } else if (kind == 'E') {
	str = es_nbr_hdr;
    } else if (kind == 'I') {
	str = is_nbr_hdr;
    } else if (kind == 'N') {
	str = nlsp_nbr_hdr;
    }
    automore_enable(str);

    for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
	for (adj = adj_db[bucket]; adj; ) {
	    mem_lock(adj);
	    if (!target || target == adj->idb) {
		if ((kind == 'A' && 
		     (adj->adjacencyType & CLNS_ALL_ADJ_TYPE)) ||
		    (kind == 'E' && adj->adjacencyType == CLNS_ES_ADJ_TYPE) ||
		    (kind == 'N' && 
		     adj->adjacencyType == CLNS_NLSP_IS_ADJ_TYPE) ||
		    (kind == 'I' && 
		     (adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE || 
		      adj->adjacencyType == CLNS_IS_ADJ_TYPE || 
		      adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE))) {
		    clns_show_one_neighbor(pdb, adj, kind, long_display,
					   FALSE);
		}
	    }
	    next_adj = adj->next;
	    free(adj);
	    adj = next_adj;
	}
    }
    automore_disable();
}

/*
 * clns_set_pdb_running
 *
 * Manipulate the "running" flag in the PDB.  This is a wrapper routine
 * to allow a transition to the new scheduler.
 */
void clns_set_pdb_running (clns_pdbtype *pdb, boolean running_flag)
{
    if (pdb->watched_running) {		/* New scheduler? */
	process_set_boolean(pdb->watched_running, running_flag);
    } else {
	pdb->pdb_running = running_flag;
    }
}

/*
 * clns_pdb_running
 *
 * Returns TRUE if the PDB "running" flag is TRUE.  This is a wrapper routine
 * to allow transition to the new scheduler.
 */
boolean clns_pdb_running (clns_pdbtype *pdb)
{
    if (pdb->watched_running) {		/* New scheduler? */
	return(process_get_boolean(pdb->watched_running));
    } else {
	return(pdb->pdb_running);
    }
}
