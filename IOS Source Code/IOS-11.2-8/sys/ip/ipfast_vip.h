/* $Id: ipfast_vip.h,v 3.5.10.10 1996/09/05 02:52:21 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_vip.h,v $
 *------------------------------------------------------------------
 * VIP resident IP fastswitching structs and defines
 *
 * August 1995, Darren Kerr and Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_vip.h,v $
 * Revision 3.5.10.10  1996/09/05  02:52:21  dlobete
 * CSCdi67904:  Small hole in ACL check for distributed flow switching
 * Branch: California_branch
 *
 * Revision 3.5.10.9  1996/08/30  16:41:22  drowell
 * CSCdi67055:  When VIP1 or VIP2 have 8MB DRAM dont run DFS
 * Branch: California_branch
 * Do not pre-allocate DFS arrays on a 8Meg VIP.
 *
 * Revision 3.5.10.8  1996/08/12  06:12:25  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.5.10.7  1996/06/06  09:24:55  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.5.10.6  1996/04/29  04:46:41  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.5.10.5  1996/04/19  15:42:19  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.5.18.2  1996/04/09  20:38:15  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.5.18.1  1996/03/30  05:39:31  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.5.10.4  1996/04/10  03:38:30  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/04/08  13:44:45  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/03/21  22:50:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.5.10.1  1996/03/18  20:17:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:26:49  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  17:28:26  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  07:48:16  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.5  1996/02/24  02:14:39  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.4  1996/01/24  20:23:55  dlobete
 * CSCdi47211:  IP DFS cache operations stall with traffic load on RVIP
 * Use the right callback routine configuration to truly run IPC
 * at interrupt level (and thus DFS cache updates). Now the DFS cache
 * will be updated in a snappier way when the RSP IP cache changes.
 *
 * Revision 3.3  1996/01/19  03:02:57  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:35:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:05  dlobete
 * Placeholders for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Flow Switching return values.
 * Redefined from src-rsp/rsp_fast.h to here to avoid including rsp stuff.
 */
#define IPFLOWOK           0x0000
#define IPFLOWERR_NOTCONF  0x0001
#define IPFLOWERR_CKS      0x0002
#define IPFLOWERR_NOCACHE  0x0003
#define IPFLOWERR_MTU      0x0004
#define IPFLOWERR_TQL      0x0005
#define IPFLOWERR_UNSUP    0x0006
#define IPFLOWERR_ACCESS   0x0007
#define IPFLOWERR_ACCT     0x0008

#define IPFLOWTBL_SIZE (1 << 13)                  /* 16K */
#define IPFLOWTBL_MASK (IPFLOWTBL_SIZE - 1)
#define IPFLOW_FREE_RESERVE 512
#define IPFLOW_ALLOC_MAX    (64*1024)
#define IPFLOW_IDLE_TIMEOUT (64*ONESEC)
#define IPFLOWSHOWMAX 32
#define IPFLOW_ALLOC_MAX_HUGE         	IPFLOW_ALLOC_MAX
#define IPFLOW_ALLOC_MAX_LARGE 	      	(IPFLOW_ALLOC_MAX/2)
#define IPFLOW_ALLOC_MAX_MEDIUM 	(IPFLOW_ALLOC_MAX/32)
#define IPFLOW_ALLOC_MAX_SMALL 		0

/*
 * Break up flow statistics into some well known and aggregated
 * ports and protocols.
 */
#define IPFLOWSTAT_TCP_TELNET    0
#define IPFLOWSTAT_TCP_FTP       1
#define IPFLOWSTAT_TCP_FTPD      2
#define IPFLOWSTAT_TCP_WWW       3
#define IPFLOWSTAT_TCP_SMTP      4
#define IPFLOWSTAT_TCP_X         5
#define IPFLOWSTAT_TCP_OTHER     6
#define IPFLOWSTAT_UDP_TFTP      7
#define IPFLOWSTAT_UDP_OTHER     8
#define IPFLOWSTAT_ICMP          9 
#define IPFLOWSTAT_IGMP         10
#define IPFLOWSTAT_IPINIP       11
#define IPFLOWSTAT_GRE          12
#define IPFLOWSTAT_IP_OTHER     13
#define IPFLOWSTAT_TOTAL        14

#define TCP_FLAGS_FIN 0x01
#define TCP_FLAGS_RST 0x04
#define TCP_FLAGS_ACK 0x10
#define TCP_FLAGS_EST (TCP_FLAGS_ACK | TCP_FLAGS_RST)

/*
 * Move this to ip.h if this is really a well-known port.
 */
#define X_PORT                  6000


/*
 * If any of unsupported flag values is on return FALSE.
 */
#define DFS_SUPPORTED_OUTPUT_FLAGS(flags) (!(flags &           \
			                 (IP_FAST_POLICY |     \
					  IP_FAST_ACCOUNTING | \
					  IP_FAST_RSP_DDR)))  

/*
 * Returns TRUE if output interface has CRYPTO off or if CRYPTO on and is_vip.
 * FALSE otherwise.
 */
#define DFS_SUPPORTED_CRYPTO(flags, is_vip) (!(flags & IP_FAST_CRYPTO_MAP) \
					     || is_vip)


/*
 * crypto tag
 *
 * Contains cryptoflags, which specifies the encryption policy, and
 * the crypto connection id.
 */
typedef struct {
    ushort    cryptoflags;
    short     crypto_conn_id;
} crypto_tag_t;


/*
 * IP Flow Key
 *
 * Contains all information to uniquely identify a tcp/ip session.
 * Additionally, it contains some other IP or TCP info used for
 * access list checking.
 */
typedef struct {
    ipaddrtype fl_src;            /* source IP network host address */
    ipaddrtype fl_dst;            /* destination IP network host address */
    union {
	struct {
	    uchar prot;           /* IP protocol, e.g., 6=TCP, 17=UDP, ... */
	    uchar tos;            /* IP type-of-service byte */
	    ushort input_index;   /* inbound interface tag */
	} i;
	ulong all;                /* summarize above for 32-bit compare */
    } misc;
    union {
	struct {
	    ushort dport;         /* tcp or udp destination port number */
	    ushort sport;         /* tcp or udp source port number */
	} direction;
	ulong both;
    } ports;                      /* summarize above for 32-bit compare */
} ipflow_key_t;

#define fl_ports ports.both
#define fl_dport ports.direction.dport
#define fl_sport ports.direction.sport
#define fl_misc  misc.all
#define fl_prot  misc.i.prot
#define fl_tos   misc.i.tos
#define fl_input misc.i.input_index


/*
 * IP Flow Cache Entry
 *
 * This includes
 *    _ overhead for managing lists of flows on hash chain
 *    _ an IP flow key in order to uniquely identify a flow.
 *    _ IP bytes and packets since last statistic flush to idb counters
 *    _ total IP bytes and packets since flow was created
 *    _ timestamp of last packet switched through this flow
 *    _ timestamp of flow creation
 *    _ pointer to parent network prefix cache entry for verifying that
 *      the route has not been invalidated since flow creation
 *    _ pointer to mac encapsulation rewrite info
 */
typedef struct ipflow_t_ {
    struct ipflow_t_ *next;         /* link to next flow in hash chain */
    ipflow_key_t key;               /* net/port info needed to identify flow */
    ipcachetype *cptr;              /* parent prefix entry for inval check */
    encapinfotype *eptr;            /* mac encapsulation rewrite info */
    uchar tcp_flags_needed;         /* maybe use for encryption? */
    uchar tcp_flags_last;           /* if TCP_FIN, then ripe for replacement */
    ulong bytes;                    /* Total IP bytes switched by flow */
    ulong pkts;                     /* Total IP pkts switched by flow */
    sys_timestamp last_switched;    /* timestamp of last use of this flow */
    sys_timestamp first_switched;   /* timestamp of first use of this flow */
    crypto_tag_t  crypto_tag;
} ipflow_t;


/*
 * Temporary flow structs used during "show ipcache".
 */
typedef struct ipflowshow_t_ {
    ipflow_key_t key;
    ulong bytes;
    ulong pkts;
    hwidbtype *hwidb;
    char *instr;
    int input_index;
    char *outstr;
    ulong output_index;
    ulong duration;
    ulong idle;
} ipflowshow_t;

/*
 * This struct contains the flow hash table, along
 * with free queues and statistics information.
 * Keep track of statistics on a 10-second interval.
 */
typedef struct ipflowtbl_t_ {
    ipflow_t *bucket[IPFLOWTBL_SIZE];
    ipflow_t *flow_pool;
    int active_flows;
    int alloced_flows;
    ulong added;
    ulong timed_out;
    ulong invalidated;
    ulong tcp_fin_seen;
    ulong create_failures;
    ulong switched; /* temporary */
    ulong idle_timeout;
} ipflowtbl_t;

/*
 * Cumulative stats for all flows of a given protocol
 */
typedef struct ipflow_stats_t_ {
    ulonglong pkts;
    ulonglong bytes;
    ulonglong active;
    ulonglong idle;
    ulonglong flows;
    char  *proto;
} ipflow_stats_t;

extern ipflowtbl_t ipflowtbl;

static inline void update_rx_ip_cntrs (hwidbtype *in, ulong bytes, ulong pkts)
{
    if (in == NULL)
      return;

    in->counters.inputs_auto[ACCT_PROTO_IP] += pkts;
    in->counters.rx_cumbytes_auto[ACCT_PROTO_IP] += bytes;

}

static inline void update_tx_ip_cntrs (dfsidbtype *out, ulong bytes, 
				       ulong pkts)
{
    if (out == NULL)
	return;

    out->outputs_auto[ACCT_PROTO_IP] += pkts;
    out->tx_cumbytes_auto[ACCT_PROTO_IP] += bytes;

}

/*
 * Prepend mac encapsulation rewrite from encapinfo structure to
 * a network header.
 *
 * NB: Since this routine copies in chunks of 64 bits (e.g., 8 bytes,
 * 16 bytes, 24 bytes, ...), the caller must make sure that there is
 * (macsize % 8) bytes of extra space in the destination buffer.
 */
static inline void encap_rewrite_generic (encapinfotype *eptr,
					  uchar *network_start,
					  int macsize)
{
    uchar *src = &eptr->m.macstring[macsize];
    uchar *dst = network_start;
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
	macsize -= 8;

	/*
	 * write out 8 bytes of data to a potentially unaligned
	 * destination.
	 */
	asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		      : "=&r" (data) : "r" (dst) );

	asm volatile (".set reorder");
	asm volatile (".set mips2");
	
    } while (macsize > 0);
}

extern boolean vip_ip_prefix_fs(hwidbtype *hwidb, paktype *pak);
extern boolean vip_ip_flow_fs(hwidbtype *hwidb, paktype *pak);
extern boolean vip_ip_encaps_generic(paktype *pak, encapinfotype *eptr,
				     hwidbtype *input, int input_encap_len,
				     ulong iplen, void *bhptr);


/*
 * Assign output encapsulation vector to encapsulation info struct.
 */
static inline encapvectortype get_ipencaps_vector (dfsidbtype *output)
{
    switch (output->fast_switch_type) {
      case FS_ETHER:
      case FS_FDDI:
      case FS_FR:
      case FS_HDLC:
      case FS_PPP:
      case FS_CHANNEL:
      case FS_TOKEN:
      case FS_AIP:
	return(vip_ip_encaps_generic);
      case FS_SMDS:  /* unresolved issues */
      case FS_SLIP:
      default:
	return(NULL);
    }
}

/*
 * If it hasn't been done before or has changed,
 * assign output encapsulation type and allocate a namestring
 * for a hardware idb.
 */
static inline void assign_encaps_and_hwstring (dfsidbtype *hwidb,
					       ulong encaps_type, 
					       boolean is_vip)
{
    char *mediastr, buf[32];
    int slot, slotunit, bay, intfc;

    if (!hwidb->hw_namestring || hwidb->fast_switch_type != encaps_type) {
	switch (hwidb->fast_switch_type = encaps_type) {
	  case FS_ETHER:
	    mediastr = "Ethernet";
	    break;
	  case FS_FDDI:
	    mediastr = "Fddi";
	    break;
	  case FS_FR:
	  case FS_HDLC:
	  case FS_PPP:
	    mediastr = "Serial";
	    break;
	  case FS_CHANNEL:
	    mediastr = "Channel";
	    break;
	  case FS_TOKEN:
	    mediastr = "Token";
	    break;
	  case FS_AIP:
	    mediastr = "Atm";
	    break;
	  case FS_SMDS:  /* unresolved issues */
	  case FS_SLIP:

	  default:
	    mediastr = NULL;
	    break;
	}
	slot = (hwidb->slot_and_unit & 0xFF000000) >> 24;
	slotunit  = hwidb->slot_and_unit & 0x00FFFFFF;
	if (is_vip) {
	    bay = (slotunit >= PA_VIRTUAL_SLOT_BOUNDARY) ? 1 : 0;
	    intfc = bay ? (slotunit - PA_VIRTUAL_SLOT_BOUNDARY) : slotunit;
	    sprintf(buf, "%s%d/%d/%d", mediastr, slot, bay, intfc);
	} else {
	    sprintf(buf, "%s%d/%d", mediastr, slot, slotunit);
	}
	strcpy(hwidb->hw_namestring, buf);
    }
}


/*
 * Set all fields in flow key except for input interface.
 *
 * For TCP flows, returns tcp_flags in order that flow lookup and
 * managment code can determine whether further access checks are
 * needed, or whether flow is ready to terminate.
 */
static inline uchar ipflow_key_get (iphdrtype *ip, ipflow_key_t *flkey)
{
    ulong data;
    uchar tcp_flags = TCP_FLAGS_EST;
    uchar prot;
    
    asm volatile (".set mips3");
    asm volatile (".set noreorder");

    /*
     * Read IP source and destination network addresses from
     * IP header into flowkey structure.
     */
    asm volatile ("ldl %0, 12(%1); ldr %0, 19(%1);" 
		  : "=&r" (data) : "r" (ip) );
    asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		  :  : "r" (data), "r" (flkey) );

    flkey->fl_prot = prot = ip->prot;
    flkey->fl_tos = ip->tos;
    
    if (prot == TCP_PROT) {
	tcptype *tcp = (tcptype *)(ip + 1);
	uchar *tcp_flagp = (uchar *)(&tcp->window) - 1;
    
	tcp_flags = *tcp_flagp;
	flkey->fl_ports = GETLONG((ulong *)(ip + 1));
    } else if (prot == UDP_PROT) {
	flkey->fl_ports = GETLONG((ulong *)(ip + 1));
    } else if (prot == ICMP_PROT) {
	icmptype *icmp;
	
	icmp = (icmptype *) (ip + 1);
	flkey->fl_ports = (icmp->type << 8) | icmp->code;
    } else if (prot == IGMP_PROT) {
	igmptype *igmp;
	
	igmp = (igmptype *) (ip + 1);
	flkey->fl_ports = igmp->type;
    } else {
	flkey->fl_ports = 0;
    }
    return(tcp_flags);
}

static inline ulong ipflow_hash (ulong dst, ulong src, ulong ports, ulong misc)
{
    ulong lhash;
    
    lhash = dst + src + ports + misc;
    lhash += lhash >> 16;
    lhash &= IPFLOWTBL_MASK;
    return(lhash);
}

/*
 * Lookup a flow using hashed IP source/destination addresses,
 * source/destination protocol port numbers, protocol, and input hwidb.
 */
static inline ipflow_t *ipflow_hash_lookup (ipflow_key_t *flkey, uchar flags)
{
    ipflow_t *flow;
    ipaddrtype dst = flkey->fl_dst;
    ipaddrtype src = flkey->fl_src;
    ulong ports = flkey->fl_ports;
    ulong misc = flkey->fl_misc;

    /*
     * Never lookup an unestablished tcp packet in the flow cache.
     * Otherwise hackers could circumvent tcp_established access check
     * by sending a bogus tcp packet with ACK set in order to trick to
     * the flow code into creating an entry, then following it with
     * an unestablished tcp packet to actually make the connection.
     *
     * Previously this was done by creating separate flows for
     * established and unestablished flows, but at a cost of 50% more
     * flows being created per TCP connection.
     *
     * Move this check to after the flow lookup.
     * This may be needed for special decryption check (tcp_flags
     * may come in encrypted, and you should then ignore them until
     * after decryption).
     */
    if ((flags & TCP_FLAGS_EST) == 0)
      return(NULL);

    /*
     * Hash together the IP destination address, source address,
     * TCP/UDP port numbers, input interface descriptor, and protocol
     * type.  Use this to index into hash table.
     */
    flow = ipflowtbl.bucket[ipflow_hash(dst, src, ports, misc)];
    
    while (flow) {
	/*
	 * Look at each flow descriptor hashed to this bucket until
	 * we find a match.  If our hash table is very wide compared
	 * to the maximum number of flow descriptors, then there
	 * shouldn't be more than one entry in any bucket most of
	 * the time.
	 *   
	 * Make sure that IP source address, IP destination address,
	 * destination port, source port, input interface, protocol,
	 * type-of-service, and tcp-established flag all match exactly.
	 *
	 * LATER: convert to dword comparisons.
	 */

	if (flow->key.fl_dst == dst && flow->key.fl_src == src &&
	    flow->key.fl_ports == ports && flow->key.fl_misc == misc &&
	    flow->cptr->flags & IPCACHE_FLAG_VALID)
	  return(flow);
	flow = flow->next;
    }
    return(NULL);
}


/*
 * Alloc a flow from global free pool.
 */
static inline ipflow_t *ipflow_alloc (void)
{
    ipflow_t *flow;

    flow = ipflowtbl.flow_pool;
    if (flow) {
	ipflowtbl.flow_pool = flow->next;
	ipflowtbl.active_flows++;
	ipflowtbl.added++;
    } else
	ipflowtbl.create_failures++;
      
    return(flow);
}

/*
 * Point flow at parent prefix cache entry.
 * We need to increment refcount in prefix cache entry so that
 * it won't get freed while some flow entry is still pointing at it.
 */
static inline void bind_ipcache_to_flow (ipcachetype *cptr,
					 ipflow_t *flow)
{
    flow->cptr = cptr;
    cptr->refcount++;
}

/*
 * Zero out cptr from flow, decrement refcount in flow, and
 * free the flow if it goes to zero.
 */
static inline void free_ipcache_from_flow (ipcachetype *cptr,
					   ipflow_t *flow)
{
    flow->cptr = NULL;
    cptr->refcount--;
    free_ipcache_entry(cptr);
}


/*
 * Return a flow to the global free pool.
 */
static inline void ipflow_free (ipflow_t *flow)
{
    flow->cptr = NULL;
    flow->next = ipflowtbl.flow_pool;
    ipflowtbl.flow_pool = flow;
    ipflowtbl.active_flows--;
}
	
/*
 * Insert flow into hash lookup table.
 */
static inline void ipflow_hash_insert (ipflow_t *flow)
{
    ulong hash;

    hash = ipflow_hash(flow->key.fl_dst, flow->key.fl_src,
		       flow->key.fl_ports, flow->key.fl_misc);
    
    flow->next = ipflowtbl.bucket[hash];
    ipflowtbl.bucket[hash] = flow;
}
    

extern void ipflow_tensec(void);
extern void vip_ipflow_show(ipaddrtype prefix, ipaddrtype mask);
extern boolean vip_ip_flow_fs(hwidbtype *input, paktype *pak);
extern void ipfast_vip_flow_init(ulong);
extern boolean vip_ip_access_check(paktype *pak, ulong flags,
                             acl_headertype *acl_in, acl_headertype *acl_out);
