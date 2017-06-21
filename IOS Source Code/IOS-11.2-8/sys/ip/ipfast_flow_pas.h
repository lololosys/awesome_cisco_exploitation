/* $Id: ipfast_flow_pas.h,v 3.1.4.2 1996/06/18 01:45:03 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_flow_pas.h,v $
 *------------------------------------------------------------------
 * ipfast_flow_pas.h : IP Flow Switching for Particle based boxes
 *
 * March 1996, Michael Beesley
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_flow_pas.h,v $
 * Revision 3.1.4.2  1996/06/18  01:45:03  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/07  00:14:14  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/29  18:42:29  mbeesley
 * Add placeholder files for new project
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MISC_TRAILER_BYTES 		11 /* smds trailer + 7 bytes 
					      pad for dword copy */
#define IPCACHE_HARDCODED_FC_BYTES   	 1 /* needed for ipcache_rntype */
#define IPCACHE_HARDCODED_ACFC_BYTES 	 2 /* needed for ipcache_rntype */

/*
 * ipflow_pas_hdr_sum_unaligned:
 * Sum IP header using 16-bit reads to avoid alignment issues for
 * packets with RIFs or wierd ATM encaps.
 */
static inline ushort ipflow_pas_hdr_sum_unaligned (iphdrtype *ip)
{
    ushort *ptr = (ushort *)ip;
    ushort ssum;
    long lsum;

    lsum  = ptr[0];
    lsum += ptr[1];
    lsum += ptr[2];
    lsum += ptr[3];
    lsum += ptr[4];
    lsum += ptr[5];
    lsum += ptr[6];
    lsum += ptr[7];
    lsum += ptr[8];
    lsum += ptr[9];
    ssum = lsum + (lsum >> 16);
    return(~ssum);
}

/*
 * ipflow_pas_ttl_adjust:
 * This routine just decrements the IP TTL and adjusts the CKS field
 * accordingly.
 */
static inline void ipflow_pas_ttl_adjust (iphdrtype *ip)
{
    ushort *ptr = (ushort *)ip;
    ushort cks, ttl;

    cks = ptr[5];
    ttl = ptr[4];
    if (cks >= 0xFEFF)
	cks++;
    ptr[5] = cks + 0x100;
    ptr[4] = ttl - 0x100;;
}

/*
 * ipflow_pas_update_rx_counters:
 * Update the rx counters
 */
static inline void ipflow_pas_update_rx_counters (hwidbtype *in, 
						  ulong bytes, ulong pkts)
{
    ip_traffic.inputs += pkts;
    in->counters.inputs_fast[ACCT_PROTO_IP] += pkts;
    in->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
}

/*
 * ipflow_pas_update_tx_counters:
 * Update the tx counters
 */
static inline void ipflow_pas_update_tx_counters (hwidbtype *out, 
						  ulong bytes, ulong pkts)
{
    ip_traffic.forwarded += pkts;
}

/* end of file */

