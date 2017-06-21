/* $Id: bgp_private.h,v 3.8.46.6 1996/08/21 08:03:55 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/bgp_private.h,v $
 *------------------------------------------------------------------
 * BGP private definitions
 *
 * October 1993, Paul Traina
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: bgp_private.h,v $
 * Revision 3.8.46.6  1996/08/21  08:03:55  rchandra
 * CSCdi66662:  BGP: Route Reflector not enabled with peer-groups
 * Branch: California_branch
 * - set the reflector-id when peer-group members are configured
 * - in bgp_send_update do not dereference path if there is no best path
 * - all 'show ip bgp community <community>' to work in priv user mode.
 *
 * Revision 3.8.46.5  1996/07/30  06:56:04  rchandra
 * CSCdi64489:  BGP: private-AS number cannot be removed from AS-path
 * Branch: California_branch
 * - add 'neighbor x.x.x.x remove-private-AS' command.
 * - add 'inbound' keyword to 'neighbor x.x.x.x soft-reconfiguration'
 *   command (CSCdi64487)
 *
 * Revision 3.8.46.4  1996/05/04  01:21:07  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.8.46.3  1996/04/16  19:01:18  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.8.46.2  1996/04/05  03:46:29  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.8.46.1  1996/03/28  22:28:18  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.8  1995/12/11  07:43:03  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.7  1995/12/02  00:57:49  rchandra
 * CSCdi44862:  BGP: accessing some fields of battrtype causes alignment
 * fixups
 * - rearrange the fields of battrtype structure to be long word aligned
 * - fix rn_nextleaf_version to check the childs version number during
 *   tree walk.
 *
 * Revision 3.6  1995/11/29  22:36:30  pst
 * CSCdi44665:  BGP: put more useful metric in IP forwarding table MIB
 *
 * Revision 3.5  1995/11/25  12:34:59  rchandra
 * CSCdi44014:  BGP: Inbound message processing is not always fair
 * - set an upper limit on the per neighbor input processing time
 *
 * Revision 3.4  1995/11/25  12:16:43  rchandra
 * CSCdi43886:  BGP: MED comparision & distance not correct with
 * confederation
 * - set the distance to be 'internal' for routes learned from EBGP
 *   confederation peers.
 * - while deciding if the MED of the paths are comparable, skip the
 *   confederation related portion of the AS path
 *
 * Revision 3.3  1995/11/18  01:25:07  thille
 * CSCdi38737:  bgp4PathAttrTable requests improperly handled
 * Fix it.
 *
 * Revision 3.2  1995/11/17  17:31:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/25  08:30:51  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.4  1995/07/10  18:55:05  rchandra
 * CSCdi36863:  BGP : Fix the bug introduced during code merge
 *  - use different value for BN_DEFAULT_ORIGINATE and BN_RR_CLIENT
 *
 * Revision 2.3  1995/07/06  23:12:24  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.2  1995/06/30  20:25:11  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.1  1995/06/07  21:05:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BGP_PRIVATE_H__
#define __BGP_PRIVATE_H__

#include "../util/radix.h"

/*
 * Fixed BGP message header.
 */

#define BGP_MARKERBYTES 16
#define BGP_HEADERBYTES 19
#define BGP_MARKER_VALUE 0xFF

typedef struct bgp_header_ {
    uchar marker[16];		/* sixteen octets, usually of all ones */
    uchar len[2];		/* total bytes in block, including header */
    uchar type;			/* one of BGP_TYPE_xxxx */
} bgp_header;

#define BGP_TYPE_OPEN		1
#define BGP_TYPE_UPDATE		2
#define BGP_TYPE_NOTIFICATION	3
#define BGP_TYPE_KEEPALIVE	4

/*
 * BGP_TYPE_OPEN
 * This is the first message sent after a TCP connection is opened.
 */

#define BGP_MIN_OPEN   10
#define BGP_MIN_OPEN3  10
#define BGP_MIN_OPEN2  6
#define BGP_AUTH_NONE  0

/*
 * BGP open message optional components
 */
#define	BGP_OPEN_OPT_AUTH	1

typedef struct bgp3_open_ {
    uchar version;              /* protocol version number */
    uchar my_as[2];		/* my AS number */
    uchar holdtime[2];          /* seconds of hold time */
    uchar identifier[4];	/* router identifier */
    uchar authcode;		/* authentication type code, 0 if none */
    uchar authdata[1];		/* authentication data, code dependent */
} bgp3_open;

typedef struct bgp2_open_ {
    uchar version;              /* protocol version number */
    uchar my_as[2];		/* my AS number */
    uchar holdtime[2];          /* seconds of hold time */
    uchar authcode;		/* authentication type code, 0 if none */
    uchar authdata[1];		/* authentication data, code dependent */
} bgp2_open;

/*
 * BGP_TYPE_UPDATE message bits and flags
 */

#define UPF_OPTIONAL 0x80
#define UPF_TRANSIT  0x40
#define UPF_PARTIAL  0x20
#define UPF_EXTENDED 0x10
#define UPF_USED     0xF0

#define ATT3_ORIGIN	 1
#define ATT3_PATH	 2
#define ATT3_NEXTHOP	 3
#define ATT3_UNREACHABLE 4
#define ATT3_METRIC	 5
#define ATT3_COMMUNITY	 8

#define ATT4_ORIGIN	 1
#define ATT4_PATH	 2
#define ATT4_NEXTHOP	 3
#define ATT4_EXITDISC	 4
#define ATT4_LOCALPREF   5
#define ATT4_ATOMICAGG   6
#define ATT4_AGGREGATOR  7
#define ATT4_COMMUNITY   8
#define ATT4_ORIGINATOR  9
#define ATT4_CLUSTLIST   10
#define ATT4_DESTPREF	 11

#define	AS_SET		 1
#define	AS_SEQUENCE	 2
#define	ASENTRY_SEQUENCE 3
#define	ASENTRY_SET	 4

/*
 * the maximum size of of the AS path we can handle (in terms of ASs)
 */
#define	BGP_PATHBUFSIZE   255
#define PATHSIZE          2
#define BGP_PATHBUFLEN    (BGP_PATHBUFSIZE*PATHSIZE)
#define ASCII_PATHSIZE    6		/* up to six chars */
#define	ASCII_PATHBUFSIZE (ASCII_PATHSIZE*BGP_PATHBUFSIZE)


/*
 * Max communities allowed
 */
#define BGP_MAXCOMMUNITY	255
#define BGP_COMMUNITY_BUFSIZE	BGP_MAXCOMMUNITY << 2

/*
 * Each community in ascii format consumes maximum of 15 chars to
 * incorporate "no-advertise" community string.
 */
#define ASCII_COMMUNITY_SIZE    15
#define BGP_COMMUNITY_ASCIISIZE (ASCII_COMMUNITY_SIZE * BGP_MAXCOMMUNITY)

/*
 * Size of origin AS buffer
 */
#define BGP_ORIGIN_BUFLEN  20


/*
 * BGP_TYPE_NOTIFICATION
 * The notification message has a byte of code and a byte of subcode followed
 * by a variable amount of data.  The codes and subcodes are defined below.
 */

#define BNOT_HEADER    1
#define  BNOT_HEADER_SYNC   1
#define  BNOT_HEADER_LENGTH 2
#define  BNOT_HEADER_TYPE   3

#define BNOT_OPEN      2
#define  BNOT_OPEN_VERSION  1
#define  BNOT_OPEN_PEERAS   2
#define  BNOT_OPEN_BADID    3
#define  BNOT_OPEN_AUTHCODE 4
#define  BNOT_OPEN_AUTHFAIL 5


#define BNOT_UPDATE    3
#define  BNOT_UPDATE_MALFORMED 1
#define  BNOT_UPDATE_UNRECOG 2
#define  BNOT_UPDATE_MISSING 3
#define  BNOT_UPDATE_FLAGS   4
#define  BNOT_UPDATE_LENGTH  5
#define  BNOT_UPDATE_ORIGIN  6
#define  BNOT_UPDATE_LOOP    7
#define  BNOT_UPDATE_NEXTHOP 8
#define  BNOT_UPDATE_OPTION  9
#define  BNOT_UPDATE_NETWORK 10
#define	 BNOT_UPDATE_BADPATH 11	/* BGP 4 only */

#define BNOT_HOLDTIME  4
#define BNOT_FSMERROR  5
#define BNOT_CEASE     6

struct bmsgtype_ {
    struct bmsgtype_ *next;	 /* link to next */
    ulong	length;		 /* length of buffer */
    uchar	*dataptr;	 /* ptr to work */
    uchar 	*datagramstart;	 /* start */
    uchar	*unfeasable;	 /* BGP4: pointer to unfeasable counter */
    uchar	*attributes;	 /* BGP4: pointer to attributes counter */
    ushort 	datagramsize;	 /* size */
    boolean	emergency;	 /* TRUE if this is the emergency buffer */
    struct bmsgtype_ *msgheadptr;/* ptr to the msg containing data part */
    struct battrtype_ *attr;	 /* for updates, ptr to attributes used */
    ipaddrtype  gateway;	 /* for updates, the next hop used */
    route_maptype *map;		 /* route-map member that created this msg */
    ulong       metric;
};


/*
 * Magic numbers
 *
 * WINDOW	- Increase the TCP receive window from its tiny default to
 *		  a larger value so that we can get some bulk throughput.
 * WRITELIMIT 	- By limiting the number of messages that are in the
 *		  outbound queue for a neighbor, we can prevent
 *		  overcommitting memory that will not be used in the near
 *		  future.  Note that this is not an absolute limit on the
 *		  size of the queue, but rather we won't start computing
 *		  more updates if the queue has not drained to at least this
 *		  point.
 * READLIMIT 	- Limit the number of messages that can be processed from the
 *		  input queue at any one time.
 */
#define BGP_WINDOW	16384	 /* TCP receive window */
#define BGP_MAXBYTES	4096	 /* maximum size of a message buffer */
#define BGP_UPDATESIZE	1024	 /* maximum size of an update buffer */
#define	BGP_WRITELIMIT  300	 /* don't queue more updates if 300 waiting */
#define BGP_READLIMIT	200	 /* max num of messages to read in one cycle */
#define BGP_DISPATCH	100	 /* num of ms we can run before dispatch */

#define BGP_INITPATHS	3	 /* num of paths preallocated */
#define	BGP_ADDPATHS	2	 /* num of paths to malloc per call to
				    add_path */
#define BGP_MAXPATHS	254	 /* num of paths that we can have */
#define BGP_NULL_INDEX	255	 /* marks the end of path index chain */
#define BGP_NO_BEST	255	 /* no best path */

#define BGP_MAX_ACCEPTERS	20	/* max number of accepter processes */

/*
 * Data Structure Definitions
 */

typedef struct bpathtype_ {
    ipaddrtype neighbor;	 /* neighbor who gave us path */
    ipaddrtype router_id;	 /* BGP identifier of said neighbor */
    ipaddrtype gateway;		 /* well-known and mandatory */
    struct battrtype_ *attr;	 /* pointer to path attributes */
    struct bdampinfotype_ *dampinfo;/* pointer to damp information */
    ulong flags;		 /* BFLAG_xxx flags */
    ushort weight;		 /* weight of path */
    uchar next;			 /* index of next path in the sorted chain */
    uchar prev;			 /* index of prev path in the sorted chain */
} bpathtype;

/*
 * NOTE: there are byte alignment constraints on this structure.
 * network_key and mask_key must be the bytes BGPRADIXKEYOFFSET bits
 * immediately preceeding the network and mask structure... they are glue for
 * the generic radix processing code.
 */
typedef struct bnettype_ {
    rntype rt_nodes[2];		 /* radix pointer information */
    uchar best;			 /* index of best path, 0xFF for none */
    uchar firstpath;		 /* first paths' index in the sorted order */
    uchar pathcount;		 /* number of paths */
    uchar network_key;		 /* aligned byte before network */
    ipaddrtype network;		 /* network number */
    ushort flags;		 /* BNET_xxx flags */
    uchar pathsmalloced;	 /* number of paths allocated */
    uchar mask_key;		 /* aligned byte before mask */
    ipaddrtype mask;		 /* network mask */
    ulong version;		 /* table version number */
    struct bpathtype_ paths[0];	 /* array of paths */
} bnettype;

typedef struct baggtype_ {
    ipaddrtype ip;		 /* IP address of aggregator */
    ushort as;			 /* AS number of aggregator */
} baggtype;

typedef struct rrinfotype_ {
    ipaddrtype originator;      /* Originator of this route in the local AS */
    int clusterlength;		/* len of clust list. Usually max 2*/
    ulong clusterlist[0];	/* cluster list attribute */
}rrinfotype;

typedef struct battrtype_ {
    struct battrtype_ *next;	 /* next attribute record */
    struct attrlist_entrytype_ *alist;/* ptr to the parent attribute list */
    ulong localpref;		 /* optional, localpref */
    ulong metric;		 /* optional, non-transitive */
    baggtype aggregator;	 /* optional, trasitive (6 bytes long) */
    uchar origin;		 /* well-known and mandatory */
    uchar aslength;		 /* length of AS path attribute */
    ulong *community;		 /* pointer to the communities attribute */
    uchar *transptr;		 /* pointer to transit attributes */
    rrinfotype *rrinfo;          /* Route Reflector related information */
    uchar translen;		 /* length of transit attributes */
    uchar comcount;	 	 /* count of communities attribute */
    uchar hashbucket;		 /* hash bucket in the attr hash table */
    uchar pathhops;		 /* computed pseudo-length of AS path */
    ulong destpref;		 /* destination preference */
    ushort destpref_as;		 /* AS that set the destination preference */
    ushort neighboras;		 /* neighbor AS number */
    uchar as_path[0];		 /* well-known and mandatory */
} battrtype;

#define BFLAG_VALID      0x000001  /* usable set of path attributes */
#define BFLAG_INTERNAL   0x000002  /* received over an internal link */
#define BFLAG_METRIC     0x000004  /* we have a metric */
#define BFLAG_NEXTHOP    0x000008  /* we have a next hop address */
#define BFLAG_ORIGIN     0x000010  /* we have a path origin */
#define BFLAG_ASPATH     0x000020  /* we have an AS path */
#define BFLAG_SOURCED    0x000040  /* we are sourcing this path */
#define BFLAG_LOCAL      0x000080  /* we used a network command for this */
#define BFLAG_SYNCED     0x000100  /* set if internal and IGP has route */
#define BFLAG_AGGREGATE  0x000200  /* this is a locally created aggregate */
#define BFLAG_LOCALPREF  0x000400  /* local preference (BGP4) */
#define BFLAG_ATOMICAGG  0x000800  /* atomic aggregate (BGP4) */
#define BFLAG_AGGREGATOR 0x001000  /* aggregator (BGP4) */
#define BFLAG_COMMUNITY  0x002000  /* we have a community attribute */
#define BFLAG_NOEXPORT   0x004000  /* donot send to external peer */
#define BFLAG_NOADV      0x008000  /* donot send to any peer */
#define BFLAG_ORIGINATOR 0x010000  /* we have the originator attribute */
#define BFLAG_CLUSTLIST  0x020000  /* we have the cluster list attribute */
#define BFLAG_CLIENT     0x040000  /* received from a RR client */
#define BFLAG_CONFED     0x080000  /* received from a confederation peer */
#define BFLAG_SUPPRESSED 0x100000  /* path is suppressed due to dampening */
#define BFLAG_HISTORY	 0x200000  /* path entry held to remember history */
#define BFLAG_LOCALAS    0x400000  /* donot send out of local sub-AS */
#define BFLAG_DESTPREF	 0x800000  /* we have destination preference */
#define BFLAG_RCVD_ONLY 0x1000000  /* just received path */
#define BFLAG_RCVD_USED 0x2000000  /* received path used without modification*/
#define BFLAG_MULTIPATH 0x4000000  /* path is one of multipath best routes */
#define BFLAG_MEDLOSER  0x8000000  /* there is other path from the same AS
 				      with a better MED */
#define BFLAG_ILLEGAL  0x10000000  /* indicates that path has wrong values
                                       and the update is not rejected */

#define BFLAG_REQUIRED  	(BFLAG_NEXTHOP+BFLAG_ORIGIN+BFLAG_ASPATH)
#define BGP_BEST_GROUPMASK	(BFLAG_INTERNAL+BFLAG_CLIENT)
#define BFLAG_NOTEXTERNAL	(BFLAG_INTERNAL+BFLAG_SOURCED)
#define BFLAG_CLEAR_RCVD	(BFLAG_SYNCED+BFLAG_NOEXPORT+BFLAG_NOADV+ \
				 BFLAG_LOCALAS+BFLAG_MULTIPATH+\
				 BFLAG_RCVD_ONLY+BFLAG_RCVD_USED)

#define BNET_SENT_IBGP   0x01	 /* we've advertised a path via IBGP */
#define BNET_SENT_EBGP   0x02	 /* we've advertised a path via EBGP */
#define BNET_SENT_CLIENT 0x04	 /* advertised normal IBGP route to client */
#define BNET_SUPPRESSED  0x08	 /* this network should be suppressed because
				    it has been aggregated */
#define BNET_BACKDOOR	 0x10	 /* backdoor network */
#define BNET_MULTIPATH   0x20	 /* we've added multiple path to IP table */

#define BNET_PRESERVE	(BNET_SENT_IBGP+BNET_SENT_EBGP+BNET_SENT_CLIENT)


#define BGP_DEFAULT_MULTIPATH   1	/* default number of ip route */
#define	BGP_DEFAULT_LOCALPREF 100	/* default value for localpref if
					   not specified */

/*
 * The dampening control block structure holds information about
 * different set of dampening parameters operational at any given
 * time. 
 */
typedef struct dampctrlblk_ {
    struct dampctrlblk_	*next;
    ulong	       	halflife_time;		/* in ticks */
    ushort 		suppress_penalty;	/* penalty to start supp. */
    ushort		reuse_penalty;		/* penalty to reuse */
    ulong		maximum_stime;		/* in ticks */
    ushort		maximum_penalty;	/* max. penalty for this set */
    ushort		decay_array_len;
    ulong	        *decay_array;	
    ulong		*reuse_helper;		/* helper to calculate reuse */
    ushort		reuse_helper_len;
}dampctrlblk;

/*
 * Structure to hold stability history per path.
 * In this structure, instead of a ptr to the path we have a ptr to
 * the net and the index of the path. We do it this way so that we
 * could safely lock the net and suspend while walking the reuse
 * array.
 */
typedef struct bdampinfotype_ {
    struct bdampinfotype_ *next;
    struct bdampinfotype_ *prev;
    sys_timestamp	  decay_time;	/* time when penanly was decayed */
    bnettype         	  *net;		/* need ptr to net for locking */
    uchar		  pindex;	/* path index in the bnettype */
    uchar		  refcount;	/* opaque field. number of users */
    ushort		  penalty;
    sys_timestamp	  start_time;	/* time when flap counter started */
    ushort		  flap_count;	/* number of flaps */
    dampctrlblk		  *dcb;		/* ptr to dampening control block */
    char		  dummy[2];	/* for allignment */
}bdampinfotype;
    
/*
 * Structure used to create a attribute list
 * Used by "show ip bgp neighbor x.x.x.x paths" and battrtype
 */
typedef struct attrlist_entrytype_ {
    struct attrlist_entrytype_ *next;	/* link to next */
    battrtype *attr;			/* ptr to an attribute entry */
} attrlist_entrytype;

/*
 * Used for doing RD filtering for a particular peer
 */
typedef struct astype_ {
    struct astype_ *next;
    ushort number;
    unsigned weight;
    boolean grant;
} astype;

/*
 * Used for doing path weighting based on access lists.
 */
typedef struct bweightinfo_ {
    struct bweightinfo_ *next;
    int access_list;
    ushort weight;
} bweightinfo;

/*
 * Structure to hold work to be done.
 */
typedef struct bworktype_ {
    struct bworktype_ *next;	 /* link to next */
    int event;			 /* work to be done */
    ipaddrtype address;		 /* network number to work on */
    ipaddrtype mask;		 /* mask of network number */
} bworktype;

/*
 * Structure to hold next hop metric cache
 */
typedef struct bgpgateway_ {
    struct bgpgateway_ *next;
    ipaddrtype address;
    ulong      metric;
} bgpgateway;

/*
 * Structure to cache filter-list evaluation on an attribute.
 */
typedef struct bgp_filter_list_ctype_ {
    struct bgp_filter_list_ctype_ *next;
    int flist;				/* filter-list number */
    battrtype *attr;			/* bgp attribute */
    boolean grant;			/* result of flist on attr */
} bgp_filter_list_ctype;

/*
 * Structure to cache route-map evaluation on an attribute.
 */
typedef struct bgp_route_map_ctype_ {
    struct bgp_route_map_ctype_ *next;
    battrtype *attr;			/* bgp attribute */
    route_map_headtype *head;
    route_maptype *map;			/* successfull route-map */
} bgp_route_map_ctype;

/*
 * Conditional advertisement state
 */
enum {
    BGP_ADV_STATE_UNINITIALIZED = 0,
    BGP_ADV_STATE_ADVERTISED,
    BGP_ADV_STATE_NOT_ADVERTISED,
    BGP_ADV_STATE_DECONFIGURED
};

/*
 * Structure for conditional advertisement
 */
typedef struct bgp_cond_adv_maptype_ {
    struct bgp_cond_adv_maptype_ *next;     /* Link to other map of same nbr */
    struct bgp_cond_adv_maptype_ *link_next; /* Link to other map in pdb_BGP */
    neighbortype *nbr;                      /* ptr back to the peer */
    route_map_nametype *bgp_advertise_map;  /* point to the advertise-map */
    route_map_nametype *bgp_condition_map;  /* point to the condition-map */
    boolean cond_type;                      /* What kind of condition, e.g.
					       non-exist */
    boolean cond_change;                 /* The condition has changed from
					    last evaluation or not */
    int adv_state;                       /* The current state of the condition */
} bgp_cond_adv_maptype;

/*
 * Defines for the cache chunks.
 */
#define RCACHE_CHUNK_MAX	100	/* Max entries in the chunk */
#define FCACHE_CHUNK_MAX	100
#define DAMP_CHUNK_MAX		250

typedef struct bgp_nexthelper_ {
    struct bgp_nexthelper_ *next;
    uchar data[0];
}bgp_nexthelper;

/*
 * Structure to handle multipath nexthops
 */
typedef struct bnexthoptype_ {
    ipaddrtype gateway;
    ipaddrtype source;
    boolean    install;
}bnexthoptype;

/*
 * State machine definitions from RFC1163.
 */ 
#define BGPS_IDLE         1		/* nothing's happening */ 
#define BGPS_CONNECT      2		/* waiting for TCP to open passively*/
#define BGPS_ACTIVE       3		/* waiting for an active TCP open */
#define BGPS_OPENSENT     4		/* sent an OPEN packet */
#define BGPS_OPENCONFIRM  5		/* OPENed, need a keepalive */
#define BGPS_ESTAB        6		/* working */
#define BGPS_CLOSING	  7		/* unofficial - we're waiting to
					   close */

/*
 * Local Definitions
 */
#define BGP_WAKEUPTIME  (2*ONESEC)	/* process wakes up this often */
#define BGP_SCANTIME	(ONEMIN)	/* scan routing table this often */
#define BGP_HOLDSECONDS	180		/* three minute holdtime */
#define	BGP_IBGP_MINADVERTINTERVAL 5
#define	BGP_EBGP_MINADVERTINTERVAL 30	/* generate EBGP updates no more than
					   once every 30 seconds */
#define BGP_GROUP_MINADVERTINTERVAL 5	/* for peer groups */
#define	BGP_GROUP_UPD_DELAY (30*ONESEC)	/* delay before initial group update */
#define BGP_PEER_MAXREADTIME (5*ONESEC) /* max per peer msg processing time */
#define BGP_MAXUPDATETIME    (30*ONESEC)/* maximum update generation time */
#define BGP_MAXRECONFIGTIME  (30*ONESEC)/* maximum soft reconfiguration time */
/*
 * Wait between 25-30 seconds before opening an IBGP connection.
 * Wait between 35-50 seconds before opening an EBGP connection.
 */
#define BGP_OPEN_DELAY      (20*ONESEC)	/* max delay before spawning open */
#define BGP_OPEN_NOROUTE_DELAY (20*ONESEC) /* delay when no route to peer */
#define IBGP_OPEN_DELAY     (10*ONESEC) /* add delay before IBGP open*/
#define EBGP_OPEN_DELAY     (30*ONESEC)	/* add delay before EBGP open*/
#define BGP_OPEN_JITTER	    50		/* How much to jitter open delay */ 
#define BGP_UPDATE_JITTER   50		/* How much to jitter updates */

/*
 * neighbortype flag bits
 */
#define BN_RESTART		0x0001 	/* restart as soon as possible */
#define BN_TEMPORARY		0x0002	/* neighbor is promiscuous temporary */
#define	BN_TEMPLATE		0x0004	/* this is a template entry */
#define	BN_PERMCFG		0x0008	/* anonymous neighbors get cfg'ed */
#define BN_NOHOPCALC		0x0010  /* next hop calculation disabled 
					   for this neighbor*/
#define	BN_BACKCOMPAT		0x0020	/* old broken peer on other side */
#define	BN_DELETED		0x0040	/* this entry may be GC'ed */
#define BN_COMMUNITYOK		0x0080	/* can send community attribute */
#define BN_COMMONADMIN		0x0100	/* EBGP peer with common admin ctrl*/
#define	BN_PEERGROUP		0x0200	/* peer group entry */
#define BN_DEFAULT_ORIGINATE	0x0400	/* originate default to this peer */
#define	BN_RR_CLIENT    	0x0800	/* route-reflector client */
#define BN_SOFT_RECONFIG_INOK	0x2000	/* inbound soft reconfig allowed */
#define BN_SOFT_RECONFIG_DOIN   0x4000	/* do inbound soft reconfiguration */
#define BN_SOFT_RECONFIG_DOOUT  0x8000	/* do outbound soft reconfiguration */
#define BN_SOFT_RECONFIG_OUT   0x10000	/* doing outbound reconfiguration */
#define BN_INBOUND_PATHPOLICY  0x20000	/* inbound path policy present */
#define BN_OUTBOUND_PATHPOLICY 0x40000  /* outbound path policy present */
#define BN_REMOVE_PRIVATE_AS   0x80000	/* remove private AS number */
#define BN_REPLICATE_UPDATE   0x100000	/* replicate upd to this pgrp member */
#define BN_ALLOCATE_INDEX     0x200000	/* allocate neighbor index for EBGP */
#define BN_RECLAIM_INDEX      0x400000	/* reclaim neighbor index */

#define PGRP2MEMBER_FLAGS	(BN_NOHOPCALC+BN_COMMUNITYOK+ \
				 BN_DEFAULT_ORIGINATE+BN_SOFT_RECONFIG_INOK+ \
				 BN_REMOVE_PRIVATE_AS)

#define BN_RESETCLEAR_FLAGS	(BN_RESTART+BN_SOFT_RECONFIG_DOIN+ \
				 BN_SOFT_RECONFIG_DOOUT+BN_SOFT_RECONFIG_OUT+ \
				 BN_REPLICATE_UPDATE)
/*
 * per neigbor defaults state
 */
#define BGP_DEFAULT_SENT	1
#define BGP_DEFAULT_DOWN	2
#define BGP_DEFAULT_WITHDRAW	3

/*
 * Definitions related to inbound-routemap prefix filtering
 */
#define BGP_ASPATH_CHANGED	0x01	/* AS-path changed */
#define BGP_ATTRINFO_CHANGED    0x02	/* attribute struct changed */
#define BGP_COMMUNITY_CHANGED   0x04	/* community changed */

/*
 * Definitions related to route flap dampening. 
 */
#define BD_DECAY_INTERVAL	(5*ONESEC)	/* ticks for decay calc */
#define BD_REUSE_INTERVAL	(10*ONESEC)	/* ticks for reuse calc */
#define BD_PRECISION		100000		/* five digit precision */
#define BD_PENALTY		1000		/* Penalty per flap */
#define BD_REUSE_PENALTY	750		/* start reuse at this value */
#define BD_SUPPRESS_PENALTY	2000		/* start suppress at this val*/
#define BD_MINIMUM_PENALTY	((BD_REUSE_PENALTY>>1) - 1)
#define BD_MAXIMUM_PENALTY	20000		/* max allowed penalty */
#define BD_DEFAULT_HALFLIFE	15		/* default halflife time */
#define BD_STIME_HALFLIFE_RATIO	4		/* max-sup-time/halflife-time*/
#define BD_CONFIG_DELAY		(3*ONESEC)	/* delay to reconf dampening */
#define BD_MAXIMUM_DAMPCTRLBLK  255		/* max routemap entries */

/*
 * consider a valid non-suppressed as a ok path for best path 
 * calculation. Do not consider the received-only paths.
 */
#define bgp_pathok(p) (((p)->flags & BFLAG_VALID) && \
		       (!((p)->flags & BFLAG_SUPPRESSED)) && \
		       (!((p)->flags & BFLAG_RCVD_ONLY)))

/*
 * check for Private AS number
 */
#define bgp_private_as(asnum) (((asnum) >= 64512) && \
			       ((asnum) <= 65535))
/*
 * Compare all the flags except synced and suppressed flag to compare
 * two paths. A valid path irrespective of whether it is suppressed 
 * or not suppressed is considered identical.
 */
#define BFLAG_IDENTICAL		(~(BFLAG_SYNCED+BFLAG_SUPPRESSED))

/*
 * Number of entries to be processed before calling check_suspend while
 * walking the reuse list.
 */
#define BGP_CHECK_REUSE_COUNT	20
#define BGP_INIT_BITFIELD_SIZE	1
#define BGP_INDEXING_DELAY	(10*ONESEC)
/*
 * Other defaults... 
 */
#define	BGP_DEFAULT_EBGP_TTL 1		/* EBGP connections must be local */
#define BGP_RR_BUFLEN        ltob(30)	/* buf for 30 clusters */

/*
 * bgp_netread state machine definitions
 */
#define RD_START	0
#define RD_SEARCH	1
#define RD_MALLOCING	2
#define RD_FILLING	3
/*
 * Different BGP users of route-map
 */
enum {
    ROUTE_MAP_BGP_INBOUND = 1,
    ROUTE_MAP_BGP_OUTBOUND,
    ROUTE_MAP_BGP_ATTRIBUTE,
    ROUTE_MAP_BGP_ADVERTISE,
    ROUTE_MAP_BGP_SUPPRESS,
    ROUTE_MAP_BGP_UNSUPPRESS,
    ROUTE_MAP_BGP_DEFAULT,
    ROUTE_MAP_BGP_DAMPENING,
    ROUTE_MAP_BGP_COND_ADVERTISE,
    ROUTE_MAP_BGP_CONDITION
};

/*
 * Match/Set supported by different BGP users of route-map
 */
#define ROUTE_MAP_BGP_INBOUND_MATCH \
     (ROUTE_MAP_MATCH_ASPATH + ROUTE_MAP_MATCH_COMMUNITY + \
      ROUTE_MAP_MATCH_EXACT_COMMUNITY + ROUTE_MAP_MATCH_IPADDRESS)

#define ROUTE_MAP_BGP_OUTBOUND_MATCH \
     (ROUTE_MAP_MATCH_ASPATH + ROUTE_MAP_MATCH_COMMUNITY + \
      ROUTE_MAP_MATCH_IPADDRESS + ROUTE_MAP_MATCH_EXACT_COMMUNITY + \
      ROUTE_MAP_MATCH_IPNEXTHOP + ROUTE_MAP_MATCH_IPROUTESOURCE + \
      ROUTE_MAP_MATCH_ROUTETYPE)

#define ROUTE_MAP_BGP_ATTRIBUTE_MATCH \
     (ROUTE_MAP_MATCH_ASPATH + ROUTE_MAP_MATCH_COMMUNITY + \
      ROUTE_MAP_MATCH_IPADDRESS + ROUTE_MAP_MATCH_IPNEXTHOP + \
      ROUTE_MAP_MATCH_IPROUTESOURCE + ROUTE_MAP_MATCH_ROUTETYPE)

#define ROUTE_MAP_BGP_ADVERTISE_MATCH 	ROUTE_MAP_BGP_ATTRIBUTE_MATCH
#define ROUTE_MAP_BGP_SUPPRESS_MATCH 	ROUTE_MAP_BGP_ATTRIBUTE_MATCH
#define ROUTE_MAP_BGP_UNSUPPRESS_MATCH 	ROUTE_MAP_BGP_OUTBOUND_MATCH

#define ROUTE_MAP_BGP_DEFAULT_MATCH \
     (ROUTE_MAP_MATCH_IPADDRESS + ROUTE_MAP_MATCH_IPNEXTHOP + \
      ROUTE_MAP_MATCH_IPROUTESOURCE + ROUTE_MAP_MATCH_METRIC + \
      ROUTE_MAP_MATCH_INTERFACE + ROUTE_MAP_MATCH_ROUTETYPE + \
      ROUTE_MAP_MATCH_TAG)

#define ROUTE_MAP_BGP_DAMPENING_MATCH \
     (ROUTE_MAP_MATCH_ASPATH + ROUTE_MAP_MATCH_COMMUNITY + \
      ROUTE_MAP_MATCH_EXACT_COMMUNITY + ROUTE_MAP_MATCH_IPADDRESS)

#define ROUTE_MAP_BGP_INBOUND_SET \
     (ROUTE_MAP_ASPATH_PND_SET + ROUTE_MAP_COMMUNITY_SET + \
      ROUTE_MAP_COMMUNITY_ADD + ROUTE_MAP_METRIC_SET + \
      ROUTE_MAP_LOCALPREF_SET + ROUTE_MAP_WEIGHT_SET + \
      ROUTE_MAP_NEXT_HOP_SET + ROUTE_MAP_ORIGIN_SET)

#define ROUTE_MAP_BGP_OUTBOUND_SET \
     (ROUTE_MAP_ASPATH_PND_SET + ROUTE_MAP_COMMUNITY_SET + \
      ROUTE_MAP_COMMUNITY_ADD + ROUTE_MAP_METRIC_SET + \
      ROUTE_MAP_METRICTYPE_SET + ROUTE_MAP_LOCALPREF_SET + \
      ROUTE_MAP_NEXT_HOP_SET + ROUTE_MAP_ORIGIN_SET)

#define ROUTE_MAP_BGP_ATTRIBUTE_SET \
     (ROUTE_MAP_ORIGIN_SET + ROUTE_MAP_LOCALPREF_SET + \
      ROUTE_MAP_METRIC_SET + ROUTE_MAP_WEIGHT_SET + \
      ROUTE_MAP_NEXT_HOP_SET + ROUTE_MAP_COMMUNITY_SET)

#define ROUTE_MAP_BGP_COND_ADVERTISE_MATCH   ROUTE_MAP_BGP_OUTBOUND_MATCH

#define ROUTE_MAP_BGP_CONDITION_MATCH \
     (ROUTE_MAP_MATCH_ASPATH + ROUTE_MAP_MATCH_COMMUNITY + \
      ROUTE_MAP_MATCH_EXACT_COMMUNITY + ROUTE_MAP_MATCH_IPADDRESS)

#define ROUTE_MAP_BGP_ADVERTISE_SET   	0
#define ROUTE_MAP_BGP_SUPPRESS_SET 	0
#define ROUTE_MAP_BGP_UNSUPPRESS_SET 	ROUTE_MAP_BGP_OUTBOUND_SET
#define ROUTE_MAP_BGP_DEFAULT_SET 	ROUTE_MAP_BGP_OUTBOUND_SET
#define ROUTE_MAP_BGP_DAMPENING_SET	0
#define ROUTE_MAP_BGP_COND_ADVERTISE_SET  ROUTE_MAP_BGP_OUTBOUND_SET
#define ROUTE_MAP_BGP_CONDITION_SET	0

static inline boolean bgpupdate_acl_debug (ipaddrtype net, ipaddrtype mask)
{
    acl_headertype *acl;

    if (!bgpupdate_debug)
	return(FALSE);

    if (bgpupdate_acl) {
	acl = acl_array[bgpupdate_acl];
	return(route_check(acl, net, mask));
    }

    return(TRUE);

}

static inline boolean bgpdamp_acl_debug (ipaddrtype net, ipaddrtype mask)
{
    acl_headertype *acl;

    if (!bgpdamp_debug)
	return(FALSE);

    if (bgpdamp_acl) {
	acl = acl_array[bgpdamp_acl];
	return(route_check(acl, net, mask));
    }

    return(TRUE);

}
extern pdbtype *pdb_BGP;

/* for snmp access */
extern ulong bgp_default_localpref;

extern bnettype *bgp4_find_pathattr(ipaddrtype prefix, ipaddrtype prefix_len, 
				    ipaddrtype peer_addr, int *path_index);
extern void bgp_community_list_callback(int);

/*
Local Variables:
comment-column: 33
End:
*/

#endif __BGP_PRIVATE_H_
