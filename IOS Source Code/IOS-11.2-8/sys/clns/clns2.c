/* $Id: clns2.c,v 3.4.22.5 1996/08/28 12:38:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns2.c,v $
 *------------------------------------------------------------------
 * clns2.c -- Support routines for CLNS packet switching
 *
 * 12-January-1989, Paulina Knibbe
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns2.c,v $
 * Revision 3.4.22.5  1996/08/28  12:38:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.22.4  1996/08/08  14:50:08  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.4.22.3  1996/08/03  23:20:17  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.4.22.2  1996/07/11  18:29:09  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.4.22.1  1996/03/18  19:02:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.3  1996/03/13  01:12:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.22.2  1996/03/07  08:39:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  00:20:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/23  02:38:04  asastry
 * CSCdi47315:  DECnet IV/V conversion makes bogus entries in ISO-IGRP L2
 * routing table
 *
 * Revision 3.3  1995/11/21  23:56:47  raj
 * CSCdi39109:  CLNS generates noise when interface is down
 * Added interface_up calls in various needed places and changed one
 * errmsg to a buginf instead.
 *
 * Revision 3.2  1995/11/17  08:51:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/11  23:57:27  dkatz
 * CSCdi41968:  CLNS ER PDUs all messed up
 * Calculate data length properly.  Copy the data from the right place.
 * Fix the debug routine so that it properly displays the packet.
 *
 * Revision 2.1  1995/06/07  20:16:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "ieee.h"
#include "../if/network.h"
#include "../x25/x25.h"
#include "../ip/ip.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../if/network.h"


/*
 * clns_domain_match
 *
 *    Returns true if the two domains are equal, else false
 */
boolean clns_domain_match (
    int length1,
    uchar *domain1,
    int length2,
    uchar *domain2)
{
    int i;

    if (length1 != length2){
	return(FALSE);
    }
    for (i = 0; i < length1; i++)
	if (*domain1++ != *domain2++)
	    return(FALSE);
    return(TRUE);
}

/*
 * area_match
 *    return true if the IGRP areas are equal
 */
boolean area_match (char *area1, char *area2)
{
    if ((*area1++ == *area2++) && (*area1 == *area2)) {
	return(TRUE);
     } else {
	return(FALSE);
    }
}

/* 
 * clns_find_pdb
 *
 * This routine is called when we are searching for the routing
 * process which owns the database for the specified area address.
 *
 * There is a restriction that two routing process cannot have the
 * same area address.
 */
clns_pdbtype *clns_find_pdb (NSAP_T nsap)
{
    clns_pdbtype *pdb;
    char         nsap_domain_length;
    boolean      match;
    int          i;

    match = FALSE;
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->typeflag & ISO_IGRPMASK) {
	    nsap_domain_length =
		nsap[0] - AREA_LENGTH - STATION_LENGTH - SELECTOR_LENGTH;
	    if (clns_domain_match(pdb->domain_length, pdb->domain,
		  nsap_domain_length, &nsap[1])) {
		match = TRUE;
		if (area_match((char *)get_area(pdb->net),
		       (char *)get_area(nsap)) && pdb->level == CLNS_LEVEL1){
		    return(pdb);
		} 
	    }
	} else if (pdb->typeflag & ISO_ISISMASK) {

	  /*
	   * Check both manual and active area addresses, in case we
	   * dropped one of the manuals due to area address overload.
	   */
	    nsap [0] -= (STATION_LENGTH + SELECTOR_LENGTH);
	    for (i = 0; i < pdb->num_active_areas; i++) {
		if (nsap_match (nsap, pdb->active_area_address [i])) {
		    nsap [0] += (STATION_LENGTH + SELECTOR_LENGTH);
		    return (pdb);
		}
	    }
	    for (i = 0; i < pdb->num_areas; i++) {
		if (nsap_match (nsap, pdb->area_address [i])) {
		    nsap [0] += (STATION_LENGTH + SELECTOR_LENGTH);
		    return (pdb);
		}
	    }
	    nsap [0] += (STATION_LENGTH + SELECTOR_LENGTH);
	}
    }

    if (match) pdb = clns_find_level2_pdb(nsap);
    return(pdb);
}


/*
 * clns_find_level2_pdb
 *
 *    Return the pdb for the level2 process which handles this nsap
 */
clns_pdbtype *clns_find_level2_pdb (NSAP_T nsap)
{
    clns_pdbtype *pdb;
    char nsap_domain_length;

    pdb = clns_protoQ.qhead;
    while (pdb) {
	if (pdb->level & CLNS_LEVEL2){
	    nsap_domain_length = nsap[0] - AREA_LENGTH - STATION_LENGTH - 1;
	    if (clns_domain_match(pdb->domain_length,
				  pdb->domain,
				  nsap_domain_length,
				  &nsap[1])){
		return(pdb);
	    }
	}
	pdb = pdb->next;
    }
    return(pdb);
}

/*
 * clns_encaps
 * Apply the clns checksum.
 * Convert the snpa target type into a linktype, which the drivers know about.
 * If 'address' is not NULL, use that instead of the SNPA. This will be the
 * case for dialer interfaces, where we need the full NSAP address of the next
 * hop instead of just the SNPA. 
 */

boolean clns_encaps (
    paktype *pak,
    int length,
    idbtype *idb,
    hwaddrtype *snpa,
    uchar *address)
{
    pak->if_output = idb;
    pak->acct_proto = ACCT_PROTO_CLNS;
    
    switch (snpa->target_type) {
      case TARGET_UNICAST:
      default: 
	pak->linktype = LINK_CLNS;
	break;
      case TARGET_BROADCAST:
	pak->linktype = LINK_CLNS_BCAST;
	break;
      case TARGET_OSI_ALL_ES:
	pak->linktype = LINK_CLNS_ALL_ES;
	break;
      case TARGET_OSI_ALL_IS:
	pak->linktype = LINK_CLNS_ALL_IS;
	break;
    }
 
    if (is_x25(idb->hwptr))
	memset(snpa->addr+snpa->length, 0, X25_ADDRESS_LENGTH-snpa->length);

    /*
     * call interface encapsulator
     */
    pak->datagramstart = (uchar *)pak->network_start;
    pak->datagramsize = length;

    /*
     * Use 'address' if it is not NULL, instead of SNPA.
     */
    return((*idb->hwptr->vencap)(pak, 
				 address ? (long)address : (long)snpa->addr));
}


/*
 * clns_segment
 * Does packet segmentation and sends the segments to the physical
 * network.  Segmentation is performed by breaking the packet into two
 * packets: the front packet is of maximum allowable size; the second
 * packet is iteratively fed to ipsendnet again.  The original packet is
 * not given to the driver since we have no way of locking a packet twice,
 * that is, once at process level and once here.
 */

paktype *clns_segment (paktype *old_pak, clns_adjtype *adj)
{
    clnshdrtype *old_pak_clns;
    paktype *new1, *new2;
    clnshdrtype *new1_clns, *new2_clns;
    clns_segpart_t *segpart;
    int len1, len2, hdrlen, nfb;

    old_pak_clns = (clnshdrtype *)clnsheadstart(old_pak);

    /* 
     * Compute header size in bytes and determine number of segment blocks
     * we can fit into a maximum size packet.
     * Don't let the last segment be less than 8 bytes.  Why?  Who knows,
     * but its in the spec.
     */
    hdrlen = old_pak_clns->hlen;			/* count of header bytes */
    nfb = (old_pak->if_output->clns_mtu - hdrlen) / 8;
    len1 = hdrlen + nfb * 8;			/* length of first frag */
    len2 = GETSHORT(old_pak_clns->clns_seglen) - nfb*8;
    if ((len2 - hdrlen) < 8) {
	len2 += 8;
	len1 -= 8;
	nfb--;
    }
    new1 = clns_getbuffer(len1);		/* buffer for first frag */
    /* *** LATER  **** maybe don't use clns_getbuffer as it zeros buffer */
    if (!new1)
	return(NULL);
    new1_clns = (clnshdrtype *)clnsheadstart(new1);

    new2 = clns_getbuffer(len2);		/* buffer for second frag */
    if (!new2) {
	datagram_done(new1);
	return(NULL);
    }
    new2_clns = (clnshdrtype *)clnsheadstart(new2);

    clns_traffic[CLNS_SEGMENTED]++;		/* count segments */
    /*
     * Copy first part of original into new1 buffer, set the MS bit to
     * indicate more segments to follow, and compute the new checksum.
     */
    bcopy((char *)old_pak_clns, (char *)new1_clns, len1);
    new1_clns->ms = 1;
    PUTSHORT(new1_clns->clns_seglen, len1);

    /*
     * Compute checksum only if original datagram had a checksum
     * present.
     */
    if (GETSHORT(old_pak_clns->checksum)) {
	compute_iso_checksum((char *)new1_clns,
			     new1_clns->hlen,
			     new1_clns->checksum);
    }
    new1->flags |= (old_pak->flags & ~PAK_INPUTQ); /* Copy old flags, except
						      for input queue flag,
						      since we did not use
						      input_getbuffer() */
    new1->if_input = old_pak->if_input;		/* copy idb pointers */
    new1->if_output = old_pak->if_output;
    new1->clns_flags = old_pak->clns_flags;
    /*
     * fill in the nexthopaddr for the new packet
     */
    new1->clns_nexthopaddr = adj->key;
    new1->clns_dstaddr = PAK2ADDR(new1);
    new1->clns_srcaddr = new1->clns_dstaddr + new1->clns_dstaddr[0] + 1;
    new1->clns_segpart = new1->clns_srcaddr + new1->clns_srcaddr[0] + 1;
    /*
     * Send first new buffer to network driver, by recursively calling
     * clns_xs_sendnet.  We know that the recursion never is more than 2
     * deep because we made the packet so that it fits the mtu.
     */
    if (clns_xs_sendnet(new1, adj)) {
	clns_pbuginf("\nCLNS: Sending segment, size %d", new1->datagramsize);
	clns_pbuginf("\n      from %n \n      to %n", 
		     new1->clns_srcaddr, new1->clns_dstaddr);
    }
    /*
     * Copy header into new2 buffer, and fill in clns_nexthopaddr
     * Put the correct segment offset into the segmentation part,
     * and then generate the the checksum for the new header.
     */
    bcopy((char *)old_pak_clns, (char *)new2_clns, hdrlen);	/* copy header */
    PUTSHORT(new2_clns->clns_seglen, len2);		/* set new2 length */
                                                /* set frag. offset */
    new2->clns_nexthopaddr = adj->key;
    new2->clns_dstaddr = PAK2ADDR(new2);
    new2->clns_srcaddr = new2->clns_dstaddr + new2->clns_dstaddr[0] + 1;
    new2->clns_segpart = new2->clns_srcaddr + new2->clns_srcaddr[0] + 1;
    segpart = (clns_segpart_t *)new2->clns_segpart;
    PUTSHORT(segpart->segoffset, 
	     GETSHORT(segpart->segoffset) + nfb * 8);

    /*
     * Compute checksum only if original datagram had a checksum
     * present.
     */
    if (GETSHORT(old_pak_clns->checksum)) {
	compute_iso_checksum((char *)new2_clns,
			     new2_clns->hlen,
			     new2_clns->checksum);
    }

    /*
     * Copy remaining data into new2 buffer.  Copy relevant data fields.
     * Hand back pointer to packet containing the second segment we made.
     */
    bcopy((char *)old_pak_clns + len1,
	  (char *)new2_clns + hdrlen,
	  len2 - hdrlen);
    new2->flags |= (old_pak->flags & ~PAK_INPUTQ); /* Copy old flags, except
						      for input queue flag,
						      since we did not use
						      input_getbuffer() */
    new2->if_input = old_pak->if_input;
    new2->if_output = old_pak->if_output;		/* copy idb pointer */
    new2->clns_flags = old_pak->clns_flags;
    return(new2);
}

/*
 * clns_dispose
 * Dispose of a discarded CLNS PDU.
 * this routine should generate Error PDU's, unless:
 *	- its from a broadcast or multicast 
 *	- its generated locally
 *	- ER PDU's are disabled on this interface
 *	- we've send an ER PDU too recently on this interface
 *	- the packet specified not to send error reports
 * To maintain consistency with IP and other routing protocols
 * this routing does not actually get rid of the packet. The caller
 * must call datagram_done.
 */
void clns_dispose (
    paktype *pak,
    int code,		/* reason for discard */
    int spot)		/* offset in header of error, if known */
{
    clnshdrtype *clns, *erpak_clns;
    hwaddrtype dst_snpa;
    hwaddrtype src_snpa;
    idbtype *idb;
    paktype *erpak;
    uchar *ptr;
    int header_len;
    int nl_len;		/* network layer length of errored packet */
    int er_data_len;	/* ER PDU Data length */
    int er_pdu_len;	/* length of ER PDU */
    NSAP_T *my_nsap;

    idb = pak->if_input;
    if (!idb) {
	return;
    }

    clns = (clnshdrtype *)clnsheadstart(pak);

    /* 
     * Don't bother with further processing if we received an ER PDU
     */
    if (clns->type == CLNS_TYPE_ERROR) {
	clns_ebuginf("\nCLNS: received an ER PDU - no ER PDU sent");
        return;
    }

    (*idb->hwptr->extract_snpa)(pak, &dst_snpa, DESTINATION);
    (*idb->hwptr->extract_snpa)(pak, &src_snpa, SOURCE);
    if (dst_snpa.target_type != TARGET_UNICAST) {
	/*
         * CLNS: clns_dispose called on non-unicast packet, discarded,
	 *       from <src-nsap> @ <interface-name>:<src-snpa> to <snpa>
	 */
	clns_ebuginf("\nCLNS: clns_dispose called on non-unicast packet");
	clns_ebuginf("\n      from %n @%s:%s, to %s",
		     pak->clns_srcaddr, pak->if_input->namestring,
		     snpa_string(&src_snpa), snpa_string(&dst_snpa));
	return;
    }
    /*
     * CLNS: PDU from <src-nsap> discarded - <reason> (<err-code> @ <offset>)
     */
    clns_ebuginf("\nCLNS: PDU from %n discarded - %s (%#x @ %d)\n  dest: %n",
		 pak->clns_srcaddr, discard_string(code),
		 code, spot, pak->clns_dstaddr);

    /*
     * Should we rate-limit ER PDU generation.
     */
    if (clns_rate_limit_erpdu(pak, idb)) {
	clns_ebuginf("\nCLNS: no ER PDU sent (rate-limited or disabled)");
	return;
    }

    erpak = clns_getbuffer(MEDDATA);
    if (!erpak) {
	clns_ebuginf("\nCLNS: no ER PDU sent (no buffer)");
	return;
    }

    erpak_clns = (clnshdrtype *)clnsheadstart(erpak);

    erpak_clns->pid = NLP_ISO8473;
    erpak_clns->version = CLNS_VERSION;
    erpak_clns->sp = erpak_clns->ms = erpak_clns->er = 0;
    erpak_clns->lifetime = clns_packet_lifetime;
    erpak_clns->type = CLNS_TYPE_ERROR;
    ptr = PAK2ADDR(erpak);
    /* dst addr */
    if (pak->clns_srcaddr) {
	erpak->clns_dstaddr = *(NSAP_T *)ptr;
	nsap_copy(*(NSAP_T *)pak->clns_srcaddr, *(NSAP_T *)ptr);
	ptr += ptr[0] + 1;
    } else {
	/* for now, return */
	clns_ebuginf("\nCLNS: no src addr to send ER to");
	retbuffer(erpak);
	return;
    }
    /* src addr */
    my_nsap = clns_get_local_npai(clns_find_pdb(erpak->clns_dstaddr));
    nsap_copy(*my_nsap, *(NSAP_T *)ptr);
    erpak->clns_srcaddr = *(NSAP_T *)ptr;
    ptr += ptr[0] + 1;
    *ptr++ = CLNS_OPT_DISCARD_REASON;
    *ptr++ = CLNS_OPT_DISCARD_REASON_LEN;
    *ptr++ = code;
    *ptr++ = spot;
    /* other options  -- LATER */
    header_len = ptr - clnsheadstart(erpak);
    erpak_clns->hlen =  header_len;
    nl_len = pak->datagramsize - (clnsheadstart(pak) - pak->datagramstart);
    er_data_len = min(nl_len, (CLNS_MINMTU - header_len));
    bcopy(clns, ptr, er_data_len);
    ptr += er_data_len;
    er_pdu_len = ptr - clnsheadstart(erpak);
    PUTSHORT(erpak_clns->clns_seglen, er_pdu_len);
    /*
     * Send the error packet out using the standard routing stuff
     * This is our best bet of actually getting the error packet through
     */
    erpak->if_input = erpak->if_output = NULL;
    if (clns_write(erpak)){
	clns_traffic[CLNS_OUTPUT]++;
	clns_traffic[CLNS_ERSSENT]++;
	clns_ebuginf("\nCLNS: ER PDU sent to %n", erpak->clns_dstaddr);
    } else {
	clns_ebuginf(" write of ER PDU failed");
    }
    GET_TIMESTAMP(idb->clns_er_timer);
}

/*
 * clns_send_unreach
 */

void clns_send_unreach (paktype *pak)
{
    clns_traffic[CLNS_UNREACHDST]++,
    clns_dispose(pak, CLNS_DISC_UNREACHDST, 0);
}

#define CHECK_DUP(opt)	if (pak->clns_flags & opt) {		\
			    clns_traffic[CLNS_BADOPTION]++;	\
			    clns_dispose(pak, CLNS_DISC_DUPLICATE,len+1); \
			    datagram_done(pak);\
			    return(FALSE);			\
		        } else {				\
		            pak->clns_flags |= opt;		\
			    clns_traffic[CLNS_OPTIONS_TOTAL]++; \
			}					\

/*
 * clns_opt_input
 * handle the input side of options processing, which includes
 * scanning all the options for syntax, recording which ones
 * are present in pak.clns_flags, bouncing the packet if it
 * includes options we don't process.
 * Return FALSE if errors were encountered and the packet was disposed.
 */

boolean clns_opt_input (paktype *pak)
{
    clnshdrtype *clns;
    uchar *ptr;
    ulong len;
    ushort hdrlen;
    boolean seenone;

    clns = (clnshdrtype *)clnsheadstart(pak);

    seenone = FALSE;
    hdrlen = clns->hlen;
    ptr = pak->clns_optpart;
    len = ptr - (uchar *)clns;	/* length of header so far */
    while (len < hdrlen) {
	if (!seenone) {
	    seenone = TRUE;
	    clns_traffic[CLNS_OPTIONS_PACKETS]++;
	}
	switch (*ptr) {
	  case CLNS_OPT_DISCARD_REASON:
	    CHECK_DUP(CLNS_OPT_HASRFD);
	    break;		/* reason for discard is a no-op */
	  case CLNS_OPT_PADDING:
	    CHECK_DUP(CLNS_OPT_HASPAD);
	    break;		/* padding option is a no-op */
	  case CLNS_OPT_PRIORITY:
	    CHECK_DUP(CLNS_OPT_HASPRIOR);
	    break;		/* priority option is a no-op */
	  case CLNS_OPT_QOS:
	    CHECK_DUP(CLNS_OPT_HASQOS);
	    if ((ptr[2] & CLNS_QOS_GLOBAL) == CLNS_QOS_GLOBAL) {
		if (ptr[1] != CLNS_OPT_GQOS_LEN) {
		    clns_traffic[CLNS_BADOPTION]++;
		    clns_dispose(pak, CLNS_DISC_OPTION, len+3);
		    datagram_done(pak);
		    return(FALSE);
		}
		pak->clns_flags |= CLNS_OPT_HASGQOS;
		clns_traffic[CLNS_OPTIONS_GQOS]++;
	    }	/* non-global qos is no-op'ed */
	    pak->clns_qos = ptr;	/* save important option */
	    break;
	  case CLNS_OPT_RROUTE:
	    CHECK_DUP(CLNS_OPT_HASRRT);
	    if (ptr[1] == CLNS_OPT_CRROUTE) {
		pak->clns_flags |= CLNS_OPT_HASCRRT; /*for when we do it*/
		clns_traffic[CLNS_BADOPTION]++;
		clns_dispose(pak, CLNS_DISC_OPTION, len+3);
		datagram_done(pak);
		return(FALSE);
	    }
	    break;		/* Partial RR is a no-op now */
	  case CLNS_OPT_SROUTE:
	    CHECK_DUP(CLNS_OPT_HASSR);
	    if (ptr[1] == CLNS_OPT_CSROUTE) {
		pak->clns_flags |= CLNS_OPT_HASCSR; /*for when we do it*/
		clns_traffic[CLNS_BADOPTION]++;
		clns_dispose(pak, CLNS_DISC_OPTION, len+3);
		datagram_done(pak);
		return(FALSE);
	    }
	    break;		/* Partial SR is a no-op now */
	  case CLNS_OPT_SECURITY:
	    CHECK_DUP(CLNS_OPT_HASSEC);
	    if (!clns_security_passthru) {
	        clns_traffic[CLNS_BADOPTION]++;
	        clns_dispose(pak, CLNS_DISC_OPTION, len+3);
	        datagram_done(pak);
	        return(FALSE);
	    }
	    break;
	  default:
	    clns_traffic[CLNS_BADOPTION]++;
	    clns_dispose(pak, CLNS_DISC_OPTION, len+1);
	    datagram_done(pak);
	    return(FALSE);
	}
	len += ptr[1] + 2;
	ptr += ptr[1] + 2;
    }
    return(TRUE);
}
/*
 * clns_set_pointers
 *    This routine is called for clns and esis packets. It does the
 * common syntax checking.
 *
 * Step through the header, checking its syntax, and set up pointers to
 * the source address, segmentation part (if present), options part (if
 * present), and the start of the data.
 * 
 * If a syntax error is found, returns the byte number it was found at,
 * else returns 0.
 */

int clns_set_pointers (paktype *pak)
{
    clnshdrtype *clns;
    int len;
    uchar *ptr;
    uchar *hptr;
    clns_segpart_t *segptr;
    int hdrlen;

    clns = (clnshdrtype *)clnsheadstart(pak);

    hptr = (uchar *)clns;
    hdrlen = clns->hlen;
    len = CLNS_FIXEDHDR_SIZE;

    /*
     * clns_dstaddr starts out pointing at the destination address in the
     * header, but if source routing is used, it is moved to point at the
     * the next address in the routing list.
     */
    ptr = PAK2ADDR(pak);

    /* dest address */
    pak->clns_dstaddr = ptr;
    len += ptr[0] + 1;
    if (!NSAP_LENGTH_CHECK (ptr) || len > hdrlen) {
	return(ptr - hptr);
    }
    ptr += ptr[0] + 1;

    /* source address */
    pak->clns_srcaddr = ptr;
    len += ptr[0] + 1;
    if (!NSAP_LENGTH_CHECK (ptr) || len > hdrlen) {
	return(ptr - hptr);
    }
    ptr += ptr[0] + 1;

    /* segmentation part */
    if (clns->sp == TRUE) {
	len += sizeof(clns_segpart_t);
	if (len > hdrlen) {
	    return(ptr - hptr);
	}
	
	/* Segment length can never be > total PDU length */
        segptr = (clns_segpart_t *)ptr;
        if (GETSHORT(clns->clns_seglen) > GETSHORT(segptr->length))
	    return(ptr - hptr);

	pak->clns_segpart = ptr;
	ptr += sizeof(clns_segpart_t);
    }
    /*
     * options part -- step through the options.  If at the end of the options
     * we're exactly at the end of the header, then its OK.  If the last
     * option claims to go beyond the end of the header then it's an error.
     */
    if (len < hdrlen) {
	pak->clns_optpart = ptr;
	while (len < hdrlen) {
	    len += ptr[1] + 2;
	    ptr += ptr[1] + 2;
	}
	if (hdrlen != len) {
	    pak->clns_optpart = NULL;
	    return(ptr - hptr);
	}
    }
    /*
     * If the packet is longer than the header then there is a data part
     */
    if (hdrlen < GETSHORT(clns->clns_seglen)) {
	pak->clns_datapart = hptr + hdrlen;
    }
    return(0);
}

/* 
 * clns_check_packet
 *
 *    This routine checks the clns specific parts of the header
 *    It returns true if everything is okay, else false
 *    If an error is discovered, an error pdu may be sent.
 */
boolean clns_check_packet(paktype *pak)
{
    clnshdrtype *clns;
    ushort len;
    int nl_len;

    clns = (clnshdrtype *)clnsheadstart(pak);

    if (clns->version != CLNS_VERSION) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	clns_dispose(pak,CLNS_DISC_HDRSYNTAX,3);
	datagram_done(pak);
	return(FALSE);
    }
    nl_len = pak->datagramsize - ((uchar *)clns - pak->datagramstart);
    if ((clns->hlen < CLNS_MINHEADERBYTES) ||
	(clns->hlen > CLNS_MAXHEADERBYTES) ||
	(clns->hlen > nl_len)){
	clns_traffic[CLNS_HDRSYNTAX]++;
	clns_dispose(pak,CLNS_HDRSYNTAX,2);
	datagram_done(pak);
	return(FALSE);
    }
    len = GETSHORT(clns->clns_seglen);
    if ((len < clns->hlen) ||
	(len > pak->datagramsize)) {
	clns_traffic[CLNS_HDRSYNTAX]++;
	clns_dispose(pak,CLNS_DISC_HDRSYNTAX,6);
	datagram_done(pak);
	return(FALSE);
    }

    /*
     * Verify checksum only if supplied.
     */
    if (GETSHORT(clns->checksum) &&
	!verify_iso_checksum((uchar *)clns, clns->hlen, clns->checksum)){
	clns_traffic[CLNS_CHECKSUM]++;
	clns_dispose(pak,CLNS_DISC_CHECKSUM, 0);
	datagram_done(pak);
	return(FALSE);
    }
    if (pak->clns_optpart && (!clns_opt_input(pak))){
	return(FALSE);
    }
    pak->clns_flags = 0;
    return(TRUE);
}


/*
 * clns_enqueue
 * Called from interrupt level to enqueue an ISO CLNS datagram to the 
 * clns input process
 */

void clns_enqueue (paktype *pak)
{
    if (pak->if_input->clns_enabled){
	process_enqueue_pak(clnsQ, pak);	
    } else {
	protocol_discard(pak, clns_running);
    }
}


/*
 * clns_error_pdu(pak)
 *    find the embedded original packet and decipher it's contents
 *    check if it matches an outstanding traceroute packet
 */

void clns_error_pdu (paktype *pak)
{
    clnshdrtype    *clns, *clns_embed;
    clns_echo_cb_t *tp;
    uchar *ptr;

    clns = (clnshdrtype *)clnsheadstart(pak);
    clns_embed = (clnshdrtype *)(clnsheadstart(pak) + clns->hlen);

    /*
     * display the original packet if appropriate
     */
    if (clns_packet_debug || clns_event_debug) {
	buginf("\nOriginal packet header");
	clns_print(clns_embed);
    }
    /*
     * does this packet correspond to a traceroute packet
     */
    if (!QUEUEEMPTY(&clns_echoQ)) {
	for (tp = clns_echoQ.qhead; tp; tp=tp->next) {
	    if  (1) { /* (tp->echo_id == origPak->duid) */
		tp->deltatime = ELAPSED_TIME(tp->xmittime);
		nsap_copy (pak->clns_srcaddr ,tp->v.trace.who);
		tp->active = FALSE;
		p_unqueue(&clns_echoQ, tp);
		/*
		 * find the reason for this error pdu
		 */
 		ptr = pak->clns_srcaddr + pak->clns_srcaddr[0] + 1;
		if ((*ptr++ == CLNS_OPT_DISCARD_REASON) &&
		    (*ptr++ == CLNS_OPT_DISCARD_REASON_LEN))
		  switch (*ptr) {
		    case CLNS_DISC_HOPS:
		    case CLNS_DISC_REASSEM:
		      tp->how = '&';
		      break;
		    case CLNS_DISC_UNREACHDST:
		    case CLNS_DISC_UNKNOWNDST:
		      tp->how = 'U';
		      break;
		    case CLNS_DISC_CONGEST:
		      tp->how = 'C';
		      break;
		    default:
		      tp->how = *ptr;
		      break;
		  }
		break;
	    } 
	} /* end of for loop */
    }
}
/* 
 * clns_opt_postroute
 * Process the options that need to be done after routing.  
 * Return true if they were
 * processed successfully, otherwise send any appropriate error pdu, 
 * dispose of the packet, and return false.
 * Note that we assume thaat the packet has global qos options, and that
 * therefore pak->clns_qos is valid.
 */

boolean clns_opt_postroute (paktype *pak)
{
    idbtype *idb;
    int hqsize;

    clns_pbuginf("\nCLNS: opt_postroute called");
    idb = pak->if_output;
    if (pak->clns_flags & CLNS_OPT_HASGQOS) {
	switch (idb->hwptr->enctype) {
	  case ET_HDLC:
	  case ET_ARPA:
	  case ET_SNAP:
	  case ET_SAP:
	  default:
	    hqsize = idb->hwptr->output_qcount;
	    break;
	  case ET_LAPB:
	  case ET_X25:
	    hqsize = 0;		/* no congestion bit set for LAPB/X.25 yet */
	    break;
	}

	/* 
	 * Do not set the congest bit if the threshold is zero.
	 */
	if (((idb->hwptr->clns_congestion != 0) && 
	     (hqsize > idb->hwptr->clns_congestion)) || 
	    (pak->fr_flags & PAK_FR_FECN)) {
	    pak->clns_qos[2] |= CLNS_QOS_CONGESTION;
	    clns_traffic[CLNS_OUTPUTCNGSTN_EXPRNCED]++;
	}
    }
    return(TRUE);
}

/*
 * clns_send_redirect
 * Send an ES-IS redirect packet to the sender of this packet.
 */

paktype *clns_send_redirect (paktype *pak,clns_adjtype *adj)
{
    NSAP_T       *addrmask;
    paktype      *rdpak;
    clnshdrtype  *rdpak_clns;
    idbtype      *idb;
    uchar        *ptr;
    hwaddrtype   pak_snpa;
    NSAP_T       net;

    addrmask = NULL;
    idb = pak->if_input;

    if(!interface_up(idb))
	return(NULL);

    memset((uchar *)&pak_snpa, 0, sizeof(hwaddrtype));

    /*
     * Check if the interface allows a redirect to go out
     */
    if (!idb->clns_send_rdpdus ||
	(TIMER_RUNNING(idb->clns_rd_timer) &&
	 CLOCK_IN_INTERVAL(idb->clns_rd_timer, idb->clns_rd_interval))) {
	clns_ebuginf("\nCLNS: Redirect to %n not sent: interval limited",
		     pak->clns_srcaddr);
	return(NULL);
    }
    rdpak = clns_getbuffer(MEDDATA);
    if (!rdpak) {
	clns_ebuginf("\nCLNS: Redirect to %n not sent: no buffer available",
		     pak->clns_srcaddr);
	return(NULL);
    }

    rdpak_clns = (clnshdrtype *)clnsheadstart(rdpak);

    rdpak_clns->pid = NLP_ESIS;
    rdpak_clns->version = ESIS_VERSION;
    rdpak_clns->lifetime = 0;
    rdpak_clns->sp = 0;
    rdpak_clns->ms = 0;
    rdpak_clns->er = 0;
    rdpak_clns->type = ESIS_TYPE_RD;
    PUTSHORT(rdpak_clns->esis_holding_time, clns_holding_timer/ONESEC);
    ptr = PAK2ADDR(rdpak);

    /*
     * Dest addr from original packet 
     */
    nsap_copy(*(NSAP_T *)pak->clns_dstaddr, *(NSAP_T *) ptr);
    ptr += ptr[0] + 1;

    /*
     * Better SNPA
     */
    switch (idb->hwptr->enctype) {
    case ET_X25:
	ptr[0] = (adj->snpa.length + 2) / 2;

    case ET_SMDS:
	if (ptr[0])
	    chartobcd(adj->snpa.addr, &ptr[1], adj->snpa.length);
	else {
	    datagram_done(rdpak);
	    return(NULL);
	}
	break;

    case ET_HDLC:
    case ET_ARPA:
    case ET_SNAP:
    case ET_SAP:
	ptr[0] = adj->snpa.length;
	if (ptr[0]) {
	    bcopy(adj->snpa.addr, &ptr[1], ptr[0]);
	    if (idb->hwptr->status & IDB_TR) {
		ieee_swap(&ptr[1], &ptr[1]);
	    }
	} else {
	    datagram_done(rdpak);
	    return(NULL);
	}
	break;

    case ET_FRAME_RELAY:

	/*
	 * Can't send Redirects on Frame Relay, because there's no
	 * guarantee that the sender has a PVC configured to the
	 * better SNPA. 
	 *
	 * Logic should flow to default case.
	 */
	clns_ebuginf("\nCLNS: Redirect to %n not sent: Frame Relay",
		     pak->clns_srcaddr);

    default:
	datagram_done(rdpak);
	return(NULL);
    }
    ptr += ptr[0] + 1;

    /*
     * If we are saying to send to a different IS, insert the NET for
     * that IS. Build NET by concatenating the area address and the system id.
     */
    if (adj->adjacencyType != CLNS_ES_ADJ_TYPE && adj->num_areas) {
	BUILD_NSAP (adj, net);
	nsap_copy(net, *(NSAP_T *)ptr);
	ptr += ptr[0] + 1;
    } else {
	net [0] = 0;
	*ptr++ = 0;
    }

    /*
     * Now check if we should send an address mask option.
     * If we learned this route through a 'route' command, send the
     * prefix in the redirect.
     */

    /*
     * Now calculate the header length NET & Address Mask, then calculate 
     * header length.
     */
    rdpak_clns->hlen = ptr - (uchar *)rdpak_clns;

    /*
     * Extract source snpa address from original packet, and use it
     * to send redirect.
     * CLNS: RD Sent on Ethernet0 to <src-nsap> @ <src-snpa>,
     *       redirecting <dst-nsap> to [<net> @] <bsnpa> [for all <addrmask>]
     */
    (*idb->hwptr->extract_snpa)(pak, &pak_snpa, SOURCE);
    if (clns_encaps(rdpak, rdpak_clns->hlen, idb, &pak_snpa, NULL)) {
        clns_pbuginf("\nCLNS: RD Sent on %s to %n @ %s,",
		     idb->namestring, pak->clns_srcaddr,
		     snpa_string(&pak_snpa));
	clns_pbuginf("\n      redirecting %n to %s%s%s%s%s",
		     pak->clns_dstaddr,
		     (net[0]) ? nsap_string(net) : "", 
		     (net[0]) ? " @ " : "",
		     snpa_string(&adj->snpa),
		     (addrmask) ? " for all " : "",
		     (addrmask) ? nsap_string(*addrmask) : "");
	GET_TIMESTAMP(idb->clns_rd_timer);
	return(rdpak);
    } else {
	clns_pbuginf("\nCLNS: encaps of RD failed for %s, len=%d",
		     idb->namestring, rdpak_clns->hlen);
	datagram_done(rdpak);
	return(NULL);
    }

}

/*
 * clns_find_dest
 *
 *    This routine will return a pointer to the destination descriptor
 *    block for this id if it exists
 */
clns_ddbtype *clns_find_dest (clns_pdbtype *pdb, NSAP_T id)
{
    int bucket;
    clns_ddbtype *station;

    bucket = nsap_hash(id[0], &id[1]);
    station = pdb->dest_routing_table[bucket];
    while (station) {
	if (nsap_equ(station->destination, id))
	    return(station);
	station = station->next;
    }
    return(station);
}

/*
 * clns_rate_limit_erpdu
 * 
 * Determines if an ER PDU should be sent. Returns TRUE if we are to 
 * rate-limit (not send the ER PDU). Do ulong arithmetic to avoid timer
 * wrapping problems.
 */
boolean clns_rate_limit_erpdu (paktype *pak, idbtype *idb)
{
    clnshdrtype *clns;

    clns = (clnshdrtype *)clnsheadstart(pak);

    /*
     * Don't send ER PDU if the ER-bit is not set in the instigating packet or
     * ER PDUs are disabled for the interface.
     */
    if (!clns->er || !idb->clns_send_erpdus) return(TRUE);

    /*
     * Don't send ER PDU if we've done so recently.
     */

    return(CLOCK_IN_INTERVAL(idb->clns_er_timer, idb->clns_er_interval));
}

/*
 * clns_pak_duplicate
 *
 * Duplicate a packet and reset clns related pointer in the packet header.
 * If *addr is non-null, copy the address into the end of packet.
 */
void clns_pak_duplicate (paktype *old_pak, paktype **new_pak, long *addr)
{
    int len;
    paktype *pak;

    /*
     * Reserve a bigger buffer if need to copy address over.
     */
    len = *addr ? sizeof(union addresses) : 0;
    old_pak->datagramsize += len;
    pak = pak_duplicate(old_pak);
    old_pak->datagramsize -= len;

    if (pak) {
	/*
	 * Copy address to the end of packet.
	 */
	if (len) {
	    pak->datagramsize -= len;
	    bcopy((char *)(*addr), pak->datagramstart + pak->datagramsize, len);
	    *addr = (long)(pak->datagramstart + pak->datagramsize);
	}

	/*
	 * Reset clns_dstaddr, clns_srcaddr ...
	 */
	clns_set_pointers(pak);
    }

    *new_pak = pak;
}

