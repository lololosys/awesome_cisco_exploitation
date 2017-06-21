/* $Id: ipfast_rsp.h,v 3.4.6.5 1996/09/07 02:17:17 kao Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp.h,v $
 *------------------------------------------------------------------
 * IP Flow Switching
 *
 * Feb 1995, Darren Kerr
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp.h,v $
 * Revision 3.4.6.5  1996/09/07  02:17:17  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase
 * the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.4.6.4  1996/07/22  18:52:15  hou
 * CSCdi59675:  Fast-switching is broken from isl to non-isl network.
 * Branch: California_branch
 * - Add an optimum switching vector for FastEthernet ISL packet.
 *
 * Revision 3.4.6.3  1996/06/18  01:45:30  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/04/30  21:18:07  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/03/18  20:17:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/16  06:58:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.1  1996/02/20  14:25:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/04  15:31:09  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.3  1995/11/30  04:41:44  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.2  1995/11/17  09:35:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:59:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MISC_TRAILER_BYTES 11 /* smds trailer + 7 bytes pad for dword copy */
#define IPCACHE_HARDCODED_FC_BYTES   1 /* needed for ipcache_rntype */
#define IPCACHE_HARDCODED_ACFC_BYTES 2 /* needed for ipcache_rntype */

/*
 * ip_hdr_sum_dstadr and ip_hdr_sum are the same except ip_hdr_sum_dstadr
 * returns ip->dstadr while ip_hdr_sum returns checksum
 * optimum switching calls ip_hdr_sum_dstadr; flow switching calls ip_hdr_sum
 */
static inline ushort ip_hdr_sum (iphdrtype *ip)
{
    if ((ulong)ip & 0x3) {
	/*
 	 * Sum IP header using 16-bit reads to avoid alignment issues for
 	 * packets with RIFs or wierd ATM encaps. Return ip->dstadr.
 	 */
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
    } else {
	/*
 	 * Sum IP header. Return ip->dstadr.
 	 */
	ulong ssum, tmp1, tmp2;

	asm volatile (
	              ".set noreorder\n"	
	              ".set noat\n"
	              ".set mips3");

	asm volatile (
	              "lwu    %0,0(%3)\n"
	              "lwu    %1,4(%3)\n"
	              "lwu    %2,8(%3)\n"
	              "daddu  %0,%0,%1\n"
	              "lwu    %1,12(%3)\n"
	              "daddu  %0,%0,%2\n"
	              "lwu    %2,16(%3)\n"
	              "daddu  %0,%0,%1\n"
	              "daddu  %0,%0,%2\n"
	              "dsra32 %1,%0,0\n"
	              "addu   %1,%1,%0\n"
	              "srl    %0,%1,16\n"
	              "addu   %0,%1,%0\n"
	              : "=&r" (ssum), "=&r" (tmp1), "=&r" (tmp2)
	              : "r" (ip) );
              
	asm volatile (
	              ".set at\n"
	              ".set reorder\n"
	              ".set mips2");

	return((ushort)~ssum);
    }
}

/*
 * ip_hdr_sum_dstadr and ip_hdr_sum are the same except ip_hdr_sum_dstadr
 * returns ip->dstadr while ip_hdr_sum returns checksum
 * optimum switching calls ip_hdr_sum_dstadr; flow switching calls ip_hdr_sum
 */
static inline ulong ip_hdr_sum_dstadr (iphdrtype *ip)
{
    if ((ulong)ip & 0x3) {
	/*
 	 * Sum IP header using 16-bit reads to avoid alignment issues for
 	 * packets with RIFs or wierd ATM encaps. Return ip->dstadr.
 	 */
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
	if ((~ssum) & 0xFFFF)
	    return(0);
	else
	    return((ptr[8] << 16) | ptr[9]);
    } else {
	/*
 	 * Sum IP header. Return ip->dstadr.
 	 */
	ulong ssum, tmp1, tmp2;

	asm volatile (
	              ".set noreorder\n"	
	              ".set noat\n"
	              ".set mips3");

	asm volatile (
	              "lwu    %0,0(%3)\n"
	              "lwu    %1,4(%3)\n"
	              "lwu    %2,8(%3)\n"
	              "daddu  %0,%0,%1\n"
	              "lwu    %1,12(%3)\n"
	              "daddu  %0,%0,%2\n"
	              "lwu    %2,16(%3)\n"
	              "daddu  %0,%0,%1\n"
	              "daddu  %0,%0,%2\n"
	              "dsra32 %1,%0,0\n"
	              "addu   %1,%1,%0\n"
	              "srl    %0,%1,16\n"
	              "addu   %0,%1,%0\n"
	              : "=&r" (ssum), "=&r" (tmp1), "=&r" (tmp2)
	              : "r" (ip) );
              
	asm volatile (
	              ".set at\n"
	              ".set reorder\n"
	              ".set mips2");

	if ((~ssum) & 0xFFFF)
	    return(0);
	else
	    return(tmp2);
    }
}

/*
 * This routine just decrements the IP TTL and adjusts the CKS field
 * accordingly.
 *
 * This routine should run in about 80nS on an RSP (without TTL check).
 */
static inline void ip_ttl_adjust (iphdrtype *ip)
{
    ushort *ptr = (ushort *)ip;

    ptr[4] -= 0x100;
    if (ptr[5] >= 0xFEFF)
	ptr[5] += 0x101;
    else
	ptr[5] += 0x100;
}

/*
 * Set the MEMD buffer header's length and transmit start fields.
 */
static inline void setup_tx_bufptr (bufhdr_t *bufptr, hwidbtype *hwidb,
				    ulong txstart, ulong len)
{
    bufptr->bf_tx_ptr = ADDR_TO_MEMD_OFFSET_24(txstart);
    bufptr->bf_tx_bytes = len;
}

/*
 * Set the MEMD buffer header's length and transmit start fields
 * for ISL packet, here we have to set CRC bit for FEIP, this
 * bit would be ignored by ISL Port Adapter with the Altera MICE 
 * ASIC which does the double CRC if it sees the ISL multicast in 
 * the destination MAC addres.
 */
static inline void setup_tx_bufptr_isl (bufhdr_t *bufptr, hwidbtype *hwidb,
				    ulong txstart, ulong len)
{
    bufptr->bf_tx_ptr = (ADDR_TO_MEMD_OFFSET_24(txstart) |
			 TX_PTR_DO_ISL_INNER_CRC);
    bufptr->bf_tx_bytes = len;
} 

/*
 * Given a pointer to the IP header, encapsulation length, and a pointer
 * to the byte immediately following the last byte of the mac rewrite string,
 * copy the encaps in reverse order from the cache entry to packet memory.
 * Note that if the length is not an integral of 8, then some bogus
 * pad bytes will overwrite up to 7 bytes before the start of the packet;
 * this should not pose any problems for fastswitched packets.
 *
 * This routine assumes no particular alignment restrictions.
 */
static inline void rewrite_generic (ipcache_rntype *cptr, iphdrtype *ip,
				    int bytes)
{
    uchar *src = &cptr->m.macstring[bytes];
    uchar *dst = (uchar *)ip;
    ulong data;

    do {
	
	asm volatile (".set mips3");
	asm volatile (".set noreorder");

	src -= 8;
	dst -= 8;

	/*
	 * read 8 bytes of data from possibly unaligned source
	 * address into a 64-bit register.
	 */
	asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
		      : "=&r" (data) : "r" (src) );
	bytes -= 8;

	/*
	 * write out 8 bytes of data to a potentially unaligned
	 * destination.
	 */
	asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		      : "=&r" (data) : "r" (dst) );

	asm volatile (".set reorder");
	asm volatile (".set mips2");
	
    } while (bytes > 0);
}

/*
 * rewrite_fddi
 *
 * Copy the 20 bytes of DA/SA/SNAP from the IP cache entry to
 * packet memory.  Hard code in the FDDI Frame Control byte.
 * In order to avoid excessive write stalls on the Orion R4600
 * processor, we will prepend the FC with the first word of the
 * encapsulation and write out as a 64-bit value with 3 bytes
 * of pre-pended pad.
 *
 * It appears that if the destination addresses are aligned
 * to an 8-byte boundary, then an sdl/sdr doesn't introduce
 * any more I/O stalls than a single sd.  Apparently, the Orion
 * or RSP hardware is able to figure out that a zero length
 * write doesn't need to take a write slot.  Therefore, this
 * more general purpose rewrite routine is only 40nS slower
 * for aligned rewrites than a special purpose routine that
 * *requires* 8-byte alignment.
 */
static inline void rewrite_fddi (ipcache_rntype *cptr, iphdrtype *ip)
{
    ulong data1, data2;
    uchar *src;
    uchar *dst;

    /*
     * Point src and dst to the byte following the end of the rewrite.
     */
    src = &cptr->m.macstring[20];
    dst = (uchar *)ip;

    asm volatile (".set mips3");
    asm volatile (".set noreorder");

    /*
     * Copy macstring bytes 4 through 19 (encaps bytes 5 through 20)
     * to MEMD, starting 16 bytes before start of IP header.
     * Use two 64-bit loads and stores.
     * Since we no longer require the macstring to be 64-bit aligned,
     * we must use the ldl/ldr instructions to handle misalignment.
     */
    asm volatile ("ldl %0, -8(%1); ldr %0, -1(%1);" 
		  : "=&r" (data2) : "r" (src) );
    asm volatile ("ldl %0, -16(%1); ldr %0, -9(%1);"  
		  : "=&r" (data1) : "r" (src) );

    asm volatile ("sdl %0, -8(%1); sdr %0, -1(%1);" 
		  :  : "r" (data2), "r" (dst) );
    asm volatile ("sdl %0, -16(%1); sdr %0, -9(%1);" 
		  :  : "r" (data1), "r" (dst) );
    
    /*
     * Prepend fddi FC byte to macstring bytes 0 through 3, and write
     * out to MEMD as a 64-bit write (with 3 bytes of pad in front).
     */
    asm volatile ("lwu %0, -20(%1);"
		  : "=&r" (data1) : "r" (src) );
    data2 = FDDI_LLC_FC_BYTE;
    asm volatile ("dsll %0, %1, 32;"  
		  : "=&r" (data2) : "r" (data2) );
    asm volatile ("or %0, %1, %2;"  
		  : "=&r" (data1) : "0" (data1), "r" (data2) );
    asm volatile ("sdl %0, -24(%1); sdr %0, -17(%1);" 
		  : : "r" (data1), "r" (dst) );


    asm volatile (".set reorder");
    asm volatile (".set mips2");
}

/*
 * Ripoff of Shaker and Schaefer's IRSP function.
 */
#define HIT_INVALIDATE_DATA	"17"

static inline void fast_line_inval (ulong addr)
{
    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)" :
	/* no output */ : "r" (addr));

    asm volatile(".set mips2");
    asm volatile(".set reorder");
}
/*
 * Invalidate 1 cache line if 'acl' is 0 indicating only IP hdr is accessed.
 * Invalidate 2 cache lines if 'acl' is 1 indicating caller checks
 * access list and may have referenced further into protocol header.
 */
static inline ushort dec_tql_and_inval (uchar *ptr, qa_acc_t *tqlptr, int acl)
{
    leveltype status;
    ushort tql;
    ulong line;

    /*
     * Disable all interrupts to make sure that timer interrupt
     * doesn't separate the two ops and sabotage the workaround.
     */
    line = (ulong)ptr & ~3;
    GLOBAL_INTERRUPT_BLOCK(status);
    tql = hwacc_read_dec_inline(tqlptr);
    fast_line_inval(line);
    if (acl)
	fast_line_inval(line + 32);
    GLOBAL_INTERRUPT_UNBLOCK(status);
    return(tql);
}

static inline void update_global_ip_counts_cache (ulong pkts)
{
    global_ip_counts_cache += pkts;
}

static inline void update_rx_counters (hwidbtype *in, ulong bytes, ulong pkts)
{
    in->counters.inputs_fast[ACCT_PROTO_IP] += pkts;
    in->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
}

static inline void update_tx_counters (hwidbtype *out, ulong bytes, ulong pkts)
{
    out->counters.outputs_fast[ACCT_PROTO_IP] += pkts;
    out->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
}
