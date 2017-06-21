/* $Id: nhrp.h,v 3.4.34.2 1996/04/25 23:24:27 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp.h,v $
 * Revision 3.4.34.2  1996/04/25  23:24:27  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.36.2  1996/04/09  21:09:40  rzagst
 * Syncing ATM_Cal_branch to the California mainline.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4.36.1  1996/03/24  04:05:51  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4.34.1  1996/03/28  03:31:28  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.4  1996/01/11  07:06:33  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Glean NHRP information from ATM ARP server.
 *
 * Revision 3.3  1995/12/08  02:01:52  bcole
 * CSCdi45234:  NHRP: Dont refresh unused cache entries
 *
 * Keep track of which cache entries are used, and avoid refreshing unused
 * entries.  Fix holddown time for implicit cache entries.
 *
 * Use IANA defined SNAP OUI for NHRP.
 *
 * Revision 3.2  1995/11/17  18:03:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:03  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Assigned address family numbers, from RFC1700
 */
#define AF_RESERVED	0
#define AF_IP		1	/* IP (IP version 4) */
#define AF_IP6		2	/* IP6 (IP version 6) */
#define AF_NSAP		3	/* NSAP */
#define AF_HDLC		4	/* HDLC (8-bit multidrop) */
#define AF_1822		5	/* BBN 1822 */
#define AF_802		6	/* 802 (all 802 media plus Ethernet
				   "canonical format") */
#define AF_E163		7	/* E.163 */
#define AF_E164		8	/* E.164 (SMDS, Frame Relay, ATM) */
#define AF_F69		9	/* F.69 (Telex) */
#define AF_X121		10	/* X.121 (X.25, Frame Relay) */
#define AF_IPX		11	/* IPX */
#define AF_ATALK	12	/* Appletalk */
#define AF_DECNET	13	/* Decnet IV */
#define AF_VINES	14	/* Banyan Vines */
#define AF_RESERVED1	65535

/*
 * Packet types
 */
#define NHRP_REQUEST	1		/* Request packet */
#define NHRP_REPLY	2		/* Reply packet */
#define NHRP_REG	3		/* Register packet */
#define NHRP_PURGE	4		/* Purge packet */
#define NHRP_ERR	5		/* Error packet */
#define NHRP_TYPES	5		/* # of types */

/*
 * Option types
 */
#define NHRP_OP_MASK	1		/* Destination mask */
#define NHRP_OP_NETID	2		/* Network ID */
#define NHRP_OP_RESP	3		/* Responder address */
#define NHRP_OP_RECORD	4		/* Forward record */
#define NHRP_OP_RRECORD	5		/* Reverse record */
#define NHRP_OP_QOS	6		/* QOS */
#define NHRP_OP_AUTH	7		/* Authentication */


#define NHRP_PFLAG 0x10		/* Set if reply is positive, clear if neg */
#define NHRP_AFLAG 0x20		/* Set if reply is authoritative */
#define NHRP_SFLAG 0x40		/* Set if advertized next hop is router ...*/
#define NHRP_QFLAG 0x80		/* Set if requestor is router, clear if host */
/*
 * Internal flags
 */
#define NHRP_IFLAG 0x01		/* Set if entry was learned implicitly */
#define NHRP_RFLAG 0x02		/* Set if entry was learned from reg pkt */
#define NHRP_GFLAG 0x04		/* Set if entry was dyn. generated locally */
#define NHRP_UFLAG 0x08		/* Set if entry has been used in last
				   cache invalidation interval */

#define NHRP_CACHED_FLAGS (NHRP_AFLAG|NHRP_PFLAG)	/* Cacheable flags */

/*
 * Fixed length section of packet
 */
#define NHRPFIXED \
    uchar version;		/* Proto version # */			\
    uchar hop;			/* Hop count */				\
    ushort cksum;		/* checksum */				\
    uchar pkt_type;		/* NHRP packet type */			\
    uchar unused0;		/* unused */				\
    ushort mand_len;		/* Length of mandatory part */

typedef struct nhrpfixedtype_ {
    NHRPFIXED
} nhrpfixedtype;

#define NHRP_FIXEDLEN 8			/* Size of fixed length field */

/*
 * Request/reply/purge packet format
 */

typedef struct nhrpreqreptype_ {
    NHRPFIXED
    uchar flags;		/* Flags, see above */
    uchar unused1;		/* unused */
    ushort protoid;		/* network layer proto id */
    ulong reqid;		/* Unique request id */
    uchar addrs[0];		/* Network layer addresses */
} nhrpreqreptype;

#define NHRP_REQLEN 8		/* Minimum size of mand. field of req. pkt */

/*
 * Structure of the next-hop records included in request/reply packets.
 */

typedef struct nhrpnhoptype_ {
    ushort hold;		/* Seconds entry is valid */
    ushort addr_type;		/* NBMA address type */
    uchar pref;			/* Preference or unused */
    uchar nbma_len;		/* NBMA address length in *bits* */
    uchar nbma[0];		/* Start of NBMA address */
} nhrpnhoptype;

#define NHRP_NHOPLEN 6		/* Minimum size of nhop record
				 (excluding actual NBMA address) */

/*
 * Purge flags
 */
#define NHRP_PURGE_AFLAG	0x80	/* Set if packet is acknowledgement */

/*
 * Register packet format
 */

typedef struct nhrpregtype_ {
    NHRPFIXED
    ushort unused;		/* unused */
    ushort protoid;		/* network layer proto id */
    uchar addr[0];		/* Network layer address */
} nhrpregtype;

#define NHRP_REGLEN 10		/* Minimum size of mand. field of reg. pkt */

/*
 * Error packet format
 */

typedef struct nhrperrtype_ {
    NHRPFIXED
    ushort errcode;		/* The error code */
    ushort offset;		/* Offset in octets of error in original pak*/
    uchar nhrp_pak[0];		/* The original packet with an error */
} nhrperrtype;

#define NHRP_ERRLEN 4		/* Minimum size of mand. field of err. pkt */

#define NHRP_ERR_OPT	1	/* Unknown option */
#define NHRP_ERR_NETID	2	/* Network ID mismatch */
#define NHRP_ERR_LOOP	3	/* Loop detected */
#define NHRP_ERR_CANT	4	/* Can't serve this address */
#define NHRP_ERR_FULL	5	/* Cache is full */
#define NHRP_ERR_NHS_REACH 6	/* NHS unreachable */
#define NHRP_ERR_PROTO	7	/* Protocol failure */
#define NHRP_ERR_FRAG	8	/* Fragmentation failure */

/*
 * Conglomerate of the various supported NBMA address types.
 */
typedef union nhrpaddrtype_ {
    uchar opaque[0];
    ipaddrtype ip_address;		/* 4 byte IP address */
    uchar smds_addr[STATIONLEN_SMDS64];	/* 8 byte SMDS address */
    uchar x121_address[ADDRLEN_X121];	/* 20 byte X25 address */
    uchar mac_addr[IEEEBYTES];		/* 6 byte IEEE 802.[35] mac addr */
    uchar nsap[STATIONLEN_ATMNSAP];	/* 20 byte NSAP */
} nhrpaddrtype;

/*
 * Various network layer address types.
 */
typedef union nhrpaddr_ {
    ulong opaque;
    ipaddrtype ip;
    struct ipx_address {
	ulong net;
	uchar host[IEEEBYTES];
    } ipx;
    ipaddrtype atalk;
} nhrpaddr;

typedef union nhrpnet_ {
    struct ip_network {
	ipaddrtype addr;
	ipaddrtype mask;
    } ip;
    ulong ipx;
    struct {
	ushort start;			/* start of cable-range */
	ushort end;			/* end of cable-range */
    } atalk;
} nhrpnet;

typedef union nhrpaddrnet_ {
    struct {
	uchar protoidx;
	uchar pad[2];
	uchar network_key;
	uchar opaque[0];
    } gen;
    struct {
	uchar protoidx;
	uchar pad[2];
	uchar network_key;		/* Aligned byte before net # */
	ipaddrtype target;
	uchar pad1[3];
	uchar mask_key;		/* Aligned byte before mask */
	ipaddrtype mask;		/* Netmask */
    } ip;
    struct {
	uchar protoidx;
	uchar pad[2];
	uchar network_key;		/* Aligned byte before net # */
	ulong net;
	uchar host[IEEEBYTES];
	uchar pad1;
	uchar mask_key;
	uchar mask[IEEEBYTES + sizeof(ulong)];
    } ipx;
    struct {
	uchar protoidx;
    } atalk;
} nhrpaddrnet;

#define NHRP_RADIX_KEYLEN 4		/* # of bytes to store a radix key */

/*
 * Generic NBMA address data structure, used for parsing static map
 * entries.
 */

typedef struct nhrphwaddrtype_ {
    ushort addr_type;			/* Address type */
    uchar nbma_len;			/* Address length, in bytes */
    uchar flags;			/* Flags to cache */
    nhrpaddrtype nbma;			/* The address itself */
} nhrphwaddrtype;

typedef struct nhrpnbmatype_ {
    ushort addr_type;			/* Address type */
    uchar nbma_len;			/* Address length, in bytes */
    nhrpaddrtype *nbmaptr;		/* The address itself */
    ipaddrtype mask;			/* Mask to advertise in reply */
} nhrpnbmatype;

/*
 * Cache entry modes
 */
typedef enum {
    NHRP_STATIC = 1,		/* Statically configured */
    NHRP_DYNAMIC,		/* Dynamically generated */
    NHRP_INCOMPLETE		/* Dynamic, but resolution is incomplete */
} nhrp_cachemodetype;

/*
 * Cache entry data structure
 */

typedef struct nhrpcacherequestor_ {
    struct nhrpcacherequestor_ *next;
    nhrpaddr src;		/* Address of requestor */
    ulong reqid;		/* Request ID from requestor */
    uchar protoidx;
/*
 * For reliable purge notification:
 */
    ushort ret;			/* Retransmit delay */
    sys_timestamp expire;	/* Time when entry will expire */
    mgd_timer retrans;		/* Time to retransmit */
} nhrpcacherequestor;

typedef struct nhrpcachetype_ {
    rntype rt_nodes[2];		 /* radix pointer information */
    idbtype *idb;
    sys_timestamp created;	/* Time entry was added */
    sys_timestamp expire;	/* Time when entry will expire */
    sys_timestamp retrans;	/* Time to retransmit */
    nhrpaddrnet prefix;		/* Network layer prefix resolved via NHRP */
    nhrpaddr nhop;		/* Next hop IP address */
    nhrpaddrtype nbma;		/* The address itself */
    nhrp_cachemodetype mode;	/* Type of entry */
    ulong reqid;		/* Unique request id */
    nhrpcacherequestor *requests; /* Requests which we have answered */
    ushort addr_type;		/* NBMA address type */
    ushort hits;		/* # times references (for incomplete) */
    ushort ret;			/* Retransmit delay */
    uchar nbma_len;		/* NBMA address length in *bits* */
    uchar flags;		/* Flags */
} nhrpcachetype;

typedef struct nhrpmcasttype_ {
    struct nhrpmcasttype_ *next;
    nhrpaddrtype nbma;		/* The address itself */
    ushort addr_type;		/* NBMA address type */
    uchar nbma_len;		/* NBMA address length in *bits* */
} nhrpmcasttype;

typedef void * nhrp_mask;
typedef boolean (*nhrp_onnettype)(nhrpnet *, nhrpaddr *);
typedef void (*nhrp_copynettype)(nhrpnet *, nhrpnet *);
typedef boolean (*nhrp_specifictype)(nhrpnet *, nhrpnet *);
typedef boolean (*nhrp_specific2type)(uchar *, uchar *);
typedef boolean (*nhrp_cmpaddrtype)(nhrpaddr *, nhrpaddr *);
typedef boolean (*nhrp_cmpnettype)(nhrpnet *, nhrpnet *);
typedef idbtype * (*nhrp_nhoptype)(paktype *, nhrpaddr *, boolean);
typedef nhrpaddr *(*nhrp_ifaddrtype)(idbtype *);
typedef void (*nhrp_copymasktype)(nhrp_mask, nhrpcachetype *);
typedef boolean (*nhrp_isbroadtype)(nhrpaddr *, idbtype *);
typedef idbtype *(*nhrp_rtlookuptype)(nhrpaddr *, nhrp_mask *, void **,
				      boolean *);
typedef boolean (*nhrp_ouraddtype)(nhrpaddr *);
typedef boolean (*nhrp_ourifaddtype)(idbtype *, nhrpaddr *);
typedef boolean (*nhrp_lookuptype)(paktype *, void *, uchar const **);
typedef void (*nhrp_checkcachetype)(nhrpaddr *);
typedef nhrp_mask (*nhrp_netsmasktype)(nhrpaddr *, boolean *);
typedef struct nhrpfuns_ {
    nhrp_onnettype nhrp_onnet;
    nhrp_specifictype nhrp_specific;
    nhrp_specific2type nhrp_specific2;
    nhrp_cmpaddrtype nhrp_cmpaddr;
    nhrp_cmpnettype nhrp_cmpnet;
    nhrp_nhoptype nhrp_nhop;
    nhrp_rtlookuptype nhrp_rtlookup;
    nhrp_ifaddrtype nhrp_ifaddr;
    nhrp_copymasktype nhrp_copymask;
    nhrp_isbroadtype nhrp_isbroad;
    nhrp_ouraddtype nhrp_ouradd;
    nhrp_ourifaddtype nhrp_ourifadd;
    nhrp_lookuptype nhrp_lookup;
    nhrp_checkcachetype nhrp_checkcache;
    nhrp_netsmasktype nhrp_netsmask;
    uchar nhrp_addlen, nhrp_netlen;
    uchar addrtype;
    ushort linktype;
    ushort protoid;
    uchar protoidx;
    boolean aggregate;
} nhrpfuns;

typedef boolean (*nhrp_cacheupdatefunc)(nhrpcachetype *, idbtype *,
					nhrpaddr *, nhrp_mask,
					void *, nhrpfuns *);

/*
 * Data structure for storing any configured NHSes.
 */

typedef struct nhrpnhstype_ {
    struct nhrpnhstype_ *next;
    nhrpnet net;		/* Network prefix for serviced destination */
    nhrpaddr nhs;		/* Address of NHS */
} nhrpnhstype;

/*
 * Convenient macros to hide some type casting
 */
#define nhrpopstart(req, len) ((uchar *)req + NHRP_FIXEDLEN + len)

#define NHRP_HASHLEN NETHASHLEN		/* NHRP hash table size */
#define NHRP_DEF_HOLDTIME (2*60*60)	/* 2 hours */
#define NHRP_VERSION 1			/* Protocol version # */
#define NHRP_MAXHOPS 255		/* max # hops for NHRP packets */
#define NHRP_BYTESIZE 8			/* # of bits in byte */
/*
 * Maximum NBMA address size, in bits
 */
#define NHRP_NBMA_MAXLEN (STATIONLEN_MAX * NHRP_BYTESIZE)
#define NHRP_NBMA_BYTES(bits) ((bits + 1) / NHRP_BYTESIZE)


typedef struct nhrpstattype_ {
    ulong pkts[NHRP_TYPES*2];		/* Totals for each individual type */
} nhrpstattype;

typedef struct nhrpopgen_ {
    ushort type, len;
    uchar opaque[0];
} nhrpopgentype;

typedef struct nhrpopmask_ {
    ushort type, len;
    ulong mask;
} nhrpopmasktype;

typedef struct nhrpopnetid_ {
    ushort type, len;
    ulong netid;
} nhrpopnetidtype;

typedef struct nhrpopres_ {
    ushort type, len;
    ulong resp;
} nhrpoprestype;

typedef struct nhrpoprec_ {
    ushort type, len;
    uchar opaque[0];
} nhrpoprectype;

typedef struct nhrpopauth_ {
    ulong type;			/* Authentication type */
    uchar data[0];		/* Authentication data */
} nhrpopauthtype;

#define NHRP_AUTH_CLEAR	0x1
#define NHRP_AUTH_MD5	0x2

#define NHRP_OPLEN_MASK		4
#define NHRP_OPLEN_NETID	4
#define NHRP_OPLEN_RESP		4
#define NHRP_OPLEN_REC		6	/* ... plus network layer */

#define NHRP_OPDEF_MASK 0xffffffff
#define NHRP_OPLEN_MAXTOTAL	200
#define NHRP_TL_LEN		4	/* Number of bytes taken TL in TLV */

/*
 * Default option length based upon use of MASK, NETID, RESP, RECORD, RRECORD,
 * and authentication
 */
#define NHRP_OPLEN_DEF	(8 + 8 + 8 + 4 + 4 + 12)

#define NHRP_ALIGN(x) (((x) + sizeof(long) - 1) & ~(sizeof(long) - 1))
#define NHRP_BYTE_LEN(x) ((x + 7) / 8)
#define NHRP_TLV_OPTBIT 0x8000

#define NHRP_AUTHLEN_MAX 8		/* Max length of an auth key */
#define NHRP_NBMA_MAXSTR 52		/* Max ascii len of NBMA addr */
#define NHRP_NETID_UNDEF	0

#define NHRP_CFG_HOLD	0x1
#define NHRP_CFG_NOREC	0x2
#define NHRP_IF_INIT	0x4		/* Interface has been initialized */

/*
 * Constants for NHRP management
 */
#define NHRP_PERIODIC 10		/* Periodic timer interval */
#define NHRP_IF_RATE_IVL 10		/* Interval for throttling interface */
#define NHRP_MAX_PKT 100		/* Max packets per interval */
#define NHRP_USE_DEF 1			/* # of requests before actually send*/
#define NHRP_RETRANS_RATE 2		/* Retransmit requests this often */
#define NHRP_RETRANS_EXP_TIME (3 * ONEMIN)  /* Stop retrans after 3 min */
#define NHRP_MAPNAME "_nhrp"
#define NHRP_MAPNAME_LEN 32
#define NHRP_MAX_RET_DELAY 60		/* Maximum retransmit delay, in sec. */

#define NHRP_NETPROTOCNT 3		/* Support 3 network layer protocols */
enum {
 NHRP_NETIP = 0,
 NHRP_NETAT,
 NHRP_NETIPX
};

typedef enum {
    NHRP_EV_REQ
} nhrp_eventcode;

typedef struct nhrp_event_ {
    struct nhrp_event_ *next;
    nhrp_eventcode type;
    nhrpfuns *fun;
    union {
	struct {
	    idbtype *if_out;
	    nhrpaddr dest;
	    boolean refresh;
	} req;
    } ev;
} nhrp_eventtype;

typedef enum {
    NHRP_TIM_PERIODIC,
    NHRP_TIM_PURGE
} nhrp_timertype;



typedef struct {
    /*
     * NHRP
     */
    ulong nhrp_flags;
    ulong nhrp_net_id;			/* NBMA network identifier */
    nhrpnhstype *nhrp_nhstable;		/* Table of NHSes */
    nhrpstattype *nhrp_stats;		/* Traffic statistics */
    nhrpmcasttype *nhrp_mcast;		/* Multicast NBMA address list */
    idbtype *nhrp_respif;		/* Responder if */
    uchar *nhrp_auth;			/* Opaque authentication info */ 
    rnhtype *nhrp_head;			/* Ptr to head of cache table tree */
    ushort nhrp_req_acl;		/* ACL for sending of NHRP requests */
    ushort nhrp_hold_pos;		/* Advertised hold time for pos. rsp */
    ushort nhrp_hold_neg;		/* Advertised hold time for neg. rsp */
    ushort nhrp_reg_hold;		/* Time to send periodic registration */

    /*
     * NHRP throttling
     */
    ushort nhrp_pkt_cnt;		/* NHRP max pkt cnt per interval */
    ushort nhrp_interval;		/* Rate limit interval, in seconds */
    ushort nhrp_quota;			/* Current remaining pkt cnt quota */
    ushort nhrp_secs;			/* Intf. perodic event timer */
    ushort nhrp_use;			/* # of requests before actually send*/
} nhrpproinfotype;

struct nhrpinfotype_ {
    nhrpproinfotype pro[NHRP_NETPROTOCNT];
};

/*
 * Private external/forward declarations
 */

extern nhrpfuns *nhrp_funtab[];

extern void nhrp_cache_age(void);
extern void nhrp_cache_clear(idbtype *, nhrpfuns *);
extern void nhrp_cache_clear_wrap(idbtype *);
extern uchar *nhrp_sprint_nbma(uchar *, uchar *, uchar, uchar);
extern nhrpcachetype *nhrp_cache_lookup(idbtype *, nhrpaddr *, nhrpfuns *);
extern nhrpcachetype *nhrp_cache_lookup_mask(idbtype *, nhrpaddr *, nhrp_mask,
					     nhrpfuns *);
extern nhrpcachetype *nhrp_cache_lookup_rnmask(idbtype *, nhrpaddrnet *,
					       nhrpfuns *, boolean);
extern nhrpcachetype *nhrp_cache_lookup_comp(idbtype *, nhrpaddr *,
					     nhrpfuns *);
extern nhrpcachetype *nhrp_cache_add(idbtype *, nhrpaddr *, nhrp_mask,
				     nhrp_cacheupdatefunc func, void *,
				     nhrpfuns *);
extern void nhrp_cache_delete(idbtype *, nhrpcachetype *);
extern void nhrp_cache_delete_subr(nhrpcachetype *);
extern void nhrp_req_enq(idbtype *, nhrpaddr *, boolean, nhrpfuns *);
extern ushort nhrp_nbma_type(idbtype *);
extern void nhrp_cache_if_delete(idbtype *);
extern void nhrp_process_start(void);
extern void nhrp_process_kill(void);
extern boolean nhrp_cache_nei(nhrpaddr *, ulong, nhrpcachetype *, nhrpfuns *);
extern void nhrp_send_purge(nhrpcacherequestor *);
extern void nhrp_send_purge_all(nhrpproinfotype *, nhrpfuns *);
extern void nhrp_queue_retrans(nhrpcacherequestor *, sys_timestamp);
extern void nhrp_cache_purge_id(idbtype *, ulong, nhrpfuns *);
extern void nhrp_cache_clear_addr(nhrpaddr *, nhrpfuns *);
extern void nhrp_cache_clear_prefix(nhrpaddrnet *, nhrpfuns *);
extern void nhrp_cache_init(rnhtype **);
extern int nhrp_cache_walk(rn_walk_function f, ...);
extern nhrpfuns *nhrp_funmap(uchar);
extern uchar nhrp_proto2idx(ushort);
extern int nhrp_cache_age_subr(rntype *, va_list);
extern void nhrp_update_nhs(idbtype *, nhrpaddr *, nhrpnet *, boolean,
			    nhrpfuns *);
extern void nhrp_map_mcast(idbtype *, nhrphwaddrtype *, boolean, nhrpfuns *);
extern void nhrp_map_command(idbtype *, nhrpaddr *, nhrp_mask,
			     nhrphwaddrtype *, boolean, nhrpfuns *);
extern uchar *nhrp_ptype_string(uchar);
extern boolean nhrp_resolve_nhop(idbtype *if_out, nhrpaddr *nhop,
				 uchar const **macaddr, ushort addr_type,
				 nhrpfuns *);
extern boolean nhrp_our_purge_pak(paktype *pak);
extern void nhrp_enqueue(paktype *);
extern void nhrp_show_purge(nhrpfuns *);
extern void nhrp_show_traffic(idbtype *, nhrpfuns *);
extern void nhrp_if_defaults(idbtype *, nhrpproinfotype *);
extern void nhrp_printaddr(uchar *, nhrpaddr *, nhrpfuns *);
extern void nhrp_printpre(uchar *, nhrpaddrnet *, nhrpfuns *);
extern ushort nhrp_link2idx(ushort);
extern void nhrp_bugnet(uchar *, nhrpaddrnet *, nhrpfuns *);
extern void nhrp_bugaddr(uchar *, nhrpaddr *, nhrpfuns *);
extern void nhrp_parser_init(void);
extern void nhrp_printnsap(uchar *, uchar *);
extern ulong nhrp_extractmask(nhrp_mask);
