/* $Id: ipfast_flow.h,v 3.5.4.3 1996/09/07 02:16:54 kao Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_flow.h,v $
 *------------------------------------------------------------------
 * IP fastswitching flow cache support
 *
 * Oct 1995, Darren Kerr
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_flow.h,v $
 * Revision 3.5.4.3  1996/09/07  02:16:54  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase
 * the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.5.4.2  1996/05/07  00:14:12  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.5.4.1  1996/04/19  15:41:45  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.9  1996/04/11  23:14:54  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 3.1.2.8  1996/03/30  05:39:05  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.1.2.7  1996/03/21  10:04:12  xliu
 * Branch: IosSec_branch
 * Migrated after CA branch.
 *
 * Revision 3.1.2.6  1996/03/19  08:09:02  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.5  1996/01/09  01:20:22  xliu
 * Branch: IosSec_branch
 * Add RSP flow switch encryption support.
 *
 * Revision 3.1.2.4  1996/01/04  19:02:45  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 3.1.2.3  1995/12/17  01:40:23  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.1.2.2  1995/12/10  09:39:15  xliu
 * Branch: IosSec_branch
 * Moved encryption code to ipsendnet, after fragmentation.
 *
 * Revision 3.1.2.1  1995/11/30  06:06:50  xliu
 * Branch: IosSec_branch
 *
 * Add encryption support.
 *
 * Revision 3.5  1996/03/07  12:46:11  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Fix alignment errors in access list checks for inbound tokenring.
 *
 * Revision 3.4  1996/03/04  15:31:04  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.3  1995/11/30  04:41:43  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.2  1995/11/17  09:34:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/07  00:02:02  dkerr
 * Initial checkin
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define TCP_FIN_TIMEOUT (3*ONESEC)  /* time for fin ack's ack to return */
#define IDLE_TIMEOUT    (30*ONESEC)
#define ACTIVE_TIMEOUT  (30*ONEMIN) /* for periodic logging */
#define IPFLOWSHOWMAX   32
#define FLOWS_PER_PAK   24
#define MAX_SECONDS_TO_KEEP_PARTIAL_FLOWSTAT 10

#define RSP_IPFLOWTBL_SIZE   (1 << 16)               /* 64K entries */

#define IPFLOWTBL_SIZE       (1 << 14)               /* 16K buckets */
#define BKTS_PER_PERIOD      (IPFLOWTBL_SIZE >> 3)   /* 1/8th, or 2K bkts */
#define BKTS_PER_SETLEVEL    (BKTS_PER_PERIOD >> 6)  /* 32 buckets */

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
#define IPFLOWSTAT_TCP_BGP       6
#define IPFLOWSTAT_TCP_FRAG      7
#define IPFLOWSTAT_TCP_OTHER     8
#define IPFLOWSTAT_UDP_DNS       9
#define IPFLOWSTAT_UDP_NTP      10
#define IPFLOWSTAT_UDP_TFTP     11
#define IPFLOWSTAT_UDP_FRAG     12
#define IPFLOWSTAT_UDP_OTHER    13
#define IPFLOWSTAT_ICMP         14 
#define IPFLOWSTAT_IGMP         15
#define IPFLOWSTAT_IPINIP       16
#define IPFLOWSTAT_GRE          17
#define IPFLOWSTAT_IP_OTHER     18
#define IPFLOWSTAT_TOTAL        19
#define X_PORT                  6000

#define TCP_FLAGS_FIN 0x01
#define TCP_FLAGS_SYN 0x02
#define TCP_FLAGS_RST 0x04
#define TCP_FLAGS_PSH 0x08
#define TCP_FLAGS_ACK 0x10
#define TCP_FLAGS_URG 0x20
#define FLAGS_DNS     0x40  /* hack to expire DNS flows early */
#define FLAGS_LRU     0x80  /* hack to expire DNS flows early */

#define TCP_FLAGS_EST (TCP_FLAGS_ACK | TCP_FLAGS_RST)
#define TCP_FLAGS_MSK 0x3F
#define FLAGS_AGE_OUT (TCP_FLAGS_FIN | TCP_FLAGS_RST | FLAGS_DNS)

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
 * Flow Switching return values.
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
#define IPFLOWERR_RESOURCE 0x000A

/*
 * IP Version 4, with length 5 words (20 octets).
 * This is required first byte of fast-switchable IP header.
 */
#define IPV4_LEN5 0x45

/*
 * IP Flow Key
 *
 * Contains all information to uniquely identify a tcp/ip session.
 */
typedef struct {
    ipaddrtype fl_src;            /* source IP network host address */
    ipaddrtype fl_dst;            /* destination IP network host address */
    union {
	struct {
	    uchar prot;           /* IP protocol, e.g., 6=TCP, 17=UDP, ... */
	    uchar tos;            /* IP type-of-service byte */
	    ushort input;         /*  */
	} i;
	ulong all;                /* summarize above for 32-bit compare */
    } misc;
    union {
	struct {
	    ushort sport;         /* tcp or udp source port number */
	    ushort dport;         /* tcp or udp destination port number */
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
#define fl_index misc.i.input


/*
 * IP Flow Cache Entry
 *
 * This includes
 *    _ hash chain link
 *    _ an IP flow key in order to uniquely identify a flow.
 *    _ IP bytes and packets since flow created
 *    _ timestamp of last packet switched through this flow
 *    _ timestamp of flow creation
 *    _ input/output interfaces
 *    _ tcp crap for doing early invalidates and mis-sequencing stats
 *
 * XXX - LATER:
 *    _ maybe restore fast pointer to cache entry or fib to obviate
 *      need for route lookup.  For now, don't sweat the extra lookup...
 */
typedef struct ipflow_t_ {
    struct ipflow_t_ *hash_next;     /* link to next flow in hash chain */
    ulong            first_switched; /* timestamp of first use of this flow */
    ulong            last_switched;
    hwidbtype        *fl_input;      /* input interface */
    
    ipflow_key_t     key;            /* info needed to identify flow */

    uchar            tcp_flags;
    uchar            tcp_retx_cnt;
    uchar            tcp_retx_secs;
    uchar            tcp_misseq_cnt;
    ulong            next_sequence;
    ulong            bytes;          /* Total IP bytes switched by flow */
    ulong            pkts;           /* Total IP pkts switched by flow */
    
    hwidbtype        *fl_output;     /* output interface */
    ipaddrtype       nexthop;        /* Next hop router */
    ushort           drops;
    ushort           pad;
    ipcache_rntype   *cptr;
    ulong            reserved;
    crypto_tag_t     crypto_tag;     /* crypto information */
} ipflow_t;

typedef struct {
    ipflow_t *bucket[0];
} iphashtbl_t;

/*
 * This struct contains the flow hash table, along
 * with free queues and statistics information.
 */
typedef struct {
    /*
     * Control portion.
     */
    iphashtbl_t *hashtbl;
    ulong hash_mask;
    ulong hash_size;
    ipflow_t *free;
    ipflow_t *base;
    int free_flows;
    int active_flows;

    /*
     * Stat portion
     */
    int valid_buckets;
    ulong added;
    ulong cur_hash_max;
    ulong max_hash_max;
    ulong no_flow_pkts;
    ulong export_records;
    ulong export_no_pak;
    ulong export_paks;
} ipflowtbl_t;

typedef struct {
    ulonglong pkts;
    ulonglong bytes;
    ulonglong active;
    ulonglong idle;
    ulonglong flows;
    char      *proto;
} ipflow_stats_t;

typedef struct {
    ushort version;         /* 1 for now. */
    ushort count;           /* The number of records in PDU. */
    ulong  SysUptime;       /* Current time in millisecs since router booted */
    ulong  unix_secs;       /* Current seconds since 0000 UTC 1970 */
    ulong  unix_nsecs;      /* Residual nanoseconds since 0000 UTC 1970 */
} FlowStatHdr;

#define FLOWSTATHDR_VERSION 1

typedef struct {
    ipaddrtype srcaddr;    /* Source IP Address */
    ipaddrtype dstaddr;    /* Destination IP Address */
    ipaddrtype nexthop;    /* Next hop router's IP Address */
    ushort input;          /* Input interface index */
    ushort output;         /* Output interface index */
    
    ulong dPkts;           /* Packets sent in Duration */
    ulong dOctets;         /* Octets sent in Duration. */
    ulong First;           /* SysUptime at start of flow */
    ulong Last;            /* and of last packet of flow */
    
    ushort srcport;        /* TCP/UDP source port number or equivalent */
    ushort dstport;        /* TCP/UDP destination port number or equivalent */
    ushort pad;
    uchar  prot;           /* IP protocol, e.g., 6=TCP, 17=UDP, ... */
    uchar  tos;            /* IP Type-of-Service */
    
    uchar  flags;          /* Reason flow was discarded, etc...  */
    uchar  tcp_retx_cnt;   /* Number of mis-seq with delay > 1sec */
    uchar  tcp_retx_secs;  /* Cumulative seconds between mis-sequenced pkts */
    uchar  tcp_misseq_cnt; /* Number of mis-sequenced tcp pkts seen */
    ulong  reserved;
} IPFlowStat;

typedef struct {
    FlowStatHdr header;
    IPFlowStat  records[0];
} IPStatMsg;

#define FLOW_PAK_SIZE (sizeof(IPStatMsg) + sizeof(IPFlowStat) * FLOWS_PER_PAK)

#define FIRST_LINE_SLOTS     15
#define SECOND_LINE_SLOTS    11
#define TOTAL_LINE_SLOTS     (FIRST_LINE_SLOTS + SECOND_LINE_SLOTS)
#define SMALL_PKT_RANGE      32
#define SMALL_PKT_SHIFT       5 /* (32 log 2) */
#define SMALL_IP_MTU        576
#define SMALL_PKT_SLOTS     (SMALL_IP_MTU / SMALL_PKT_RANGE)
#define BIG_PKT_RANGE       512
#define BIG_PKT_SHIFT         9 /* (512 log 2) */
#define BIG_IP_MTU        32768
#define BIG_PKT_SLOTS     (BIG_IP_MTU / BIG_PKT_RANGE)
#define TOTAL_PKT_SLOTS   (SMALL_PKT_SLOTS + BIG_PKT_SLOTS)

extern ipflowtbl_t ipflowtbl;
extern ipflow_t bogon_flow;
extern paktype pak_access;
extern ipcache_debug;
extern ulonglong ip_total_pkts[TOTAL_PKT_SLOTS];

/*
 * Prototypes
 */
extern void 	ipflow_periodic(void);
extern void 	ipflow_check_free_resources(void);
extern void 	ipflow_show(ipaddrtype prefix, ipaddrtype mask, idbtype *);
extern boolean 	ipflow_table_init(void);
extern boolean 	ip_flow_cache_commands(parseinfo *csb);
extern boolean	flow_access_check(iphdrtype *ip, hwidbtype *hwin, 
				  idbtype *swout, ushort ipln,
				  crypto_tag_t *crypto_tag);
extern boolean 	pak_flow_access_check(paktype *pak, hwidbtype *hwin,
 				      idbtype *swout, ushort ipln,
				      crypto_tag_t *crypto_tag);
extern ipflow_t *ipflow_add(iphdrtype *ip, hwidbtype *hwin, hwidbtype *hwout, 
			    ipflow_key_t *, crypto_tag_t *);

static inline ulong clock_diff32 (ulong Co, ulong Cn)
{
    return((ulong)(Cn - Co));
}

/*
 * Increase the packet count in the appropriate packet size range counter.
 * Note that there are two different bucket sizes to get more granularity
 * on smaller sized packets.
 */
static inline void ipflow_log_size_distribution (ushort iplen)
{
    /*
     * Update packet size distribution.
     */
    if (iplen <= SMALL_IP_MTU)
      ip_total_pkts[(iplen-1)>>SMALL_PKT_SHIFT]++;
    else if (iplen <= BIG_IP_MTU)
      ip_total_pkts[(SMALL_PKT_SLOTS-1) + ((iplen-1)>>BIG_PKT_SHIFT)]++;
    else
      ip_total_pkts[TOTAL_PKT_SLOTS-1]++;
}

/*
 * Set all fields in flow key except for input interface.
 *
 * For TCP flows, returns tcp_flags in order that flow lookup and
 * managment code can determine whether further access checks are
 * needed, or whether flow is ready to terminate.
 */
static inline uchar ipflow_key_get (iphdrtype *ip, ipflow_key_t *flkey,
				    hwidbtype *input)
{
    uchar tcp_flags = TCP_FLAGS_ACK;
    uchar prot;
    ushort frag;
    
    /*
     * set flkey->fl_ports for TCP and UDP here
     * other protocols or IP fragments which are not performance sensitive 
     * will set appropriate value to flkey->fl_ports again later.
     */
    if ((ulong)ip & 3) {
	flkey->fl_dst = GETLONG(&ip->dstadr);
	flkey->fl_src = GETLONG(&ip->srcadr);
	frag = GETSHORT((uchar *)ip + 6) & 0x1FFF;
	flkey->fl_ports = GETLONG((ulong *)(ip + 1));
    } else {
	flkey->fl_dst = ip->dstadr;
	flkey->fl_src = ip->srcadr;
	frag = ip->fo;
	flkey->fl_ports = *(ulong *)(ip + 1);
    }

    flkey->fl_prot = prot = ip->prot;
    flkey->fl_tos = ip->tos;
    flkey->fl_index = input->hw_if_index;

    if (frag) {
	/* Fragmented IP datagram.
	 * If someone is trying to circumvent TCP-established acces
	 * checks by fragmenting the TCP header, then set flags to
	 * zero here to force later flow lookup code into falling through
	 * to full access checks.
	 */
	if (frag <= TCP_FLAGS_FRAGMENT)
	    tcp_flags = 0;
	flkey->fl_ports = 0;
    } else if (prot == TCP_PROT) {
	/*
	 * flkey->fl_ports is set already
	 */
	tcptype *tcp = (tcptype *)(ip + 1);
	uchar *tcp_flagp = (uchar *)(&tcp->window) - 1;
    
	tcp_flags = *tcp_flagp & TCP_FLAGS_MSK;
    } else if (prot == UDP_PROT) {
	/*
	 * flkey->fl_ports is set already
	 */
	if (flkey->fl_dport == DNS_PORT)
	    tcp_flags = FLAGS_DNS;
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

static inline ulong ipflow_hash (ulong dst, ulong src, ulong ports,
				 ulong misc, ulong mask)
{
    ulong lhash;
    
    lhash = dst ^ src ^ ports ^ misc;
    lhash ^= lhash >> 16;
    lhash &= mask;
    return(lhash);
}

/*
 * Lookup a flow using hashed IP source/destination addresses,
 * source/destination protocol port numbers, protocol, and input i/f idx.
 */
static inline ipflow_t *ipflow_hash_lookup (ipflow_key_t *flkey)
{
    ipflow_t *flow;
    ipaddrtype dst = flkey->fl_dst;
    ipaddrtype src = flkey->fl_src;
    ulong ports = flkey->fl_ports;
    ulong misc = flkey->fl_misc; /* protocol, tos, and input index */
    ulong hash;

    /*
     * Hash together the IP destination address, source address,
     * TCP/UDP port numbers, input interface descriptor, and protocol
     * type.  Use this to index into hash table.
     */
    hash = ipflow_hash(dst, src, ports, misc, ipflowtbl.hash_mask);
    flow = ipflowtbl.hashtbl->bucket[hash];
    
    while (flow) {
	/*
	 * Look at each flow descriptor hashed to this bucket until
	 * we find a match.  If our hash table is very wide compared
	 * to the maximum number of flow descriptors, then there
	 * shouldn't be more than a couple entries in any bucket most of
	 * the time.
	 *   
	 * Make sure that IP source address, IP destination address,
	 * destination port, source port, input interface, protocol,
	 * and type-of-service all match exactly.
	 *
	 * LATER: convert to dword comparisons.
	 */

	if (flow->key.fl_dst == dst && flow->key.fl_src == src &&
	    flow->key.fl_ports == ports && flow->key.fl_misc == misc) {

	    return(flow);
	}
	flow = flow->hash_next;
    }
    return(NULL);
}

/* end of file */








