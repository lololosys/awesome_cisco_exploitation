/* $Id: ipnat_private.h,v 3.1.6.12 1996/09/07 01:07:18 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat_private.h,v $
 *------------------------------------------------------------------
 * IP Network Address Translation internals
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat_private.h,v $
 * Revision 3.1.6.12  1996/09/07  01:07:18  ahh
 * CSCdi68015:  NAT: multi-homing not quite right
 * Route-map-based translation needs a slightly different address
 * allocation strategy -- each inside host gets one address per
 * route-map.  Do a little cleanup.
 * Branch: California_branch
 *
 * Revision 3.1.6.11  1996/08/28  22:57:53  ahh
 * CSCdi65898:  NAT: rsh not working when overloading
 * rsh wants rshd to connect back to it for stderr, so need to allow this
 * flow from a random privileged port in.
 * Branch: California_branch
 *
 * Revision 3.1.6.10  1996/08/22  00:27:52  ahh
 * CSCdi66753:  NAT: portlists not reaped when a mapping is deleted
 * As ye sow, so shall ye reap (or something like that).
 * Branch: California_branch
 *
 * Revision 3.1.6.9  1996/08/20  18:55:34  ahh
 * CSCdi66560:  NAT: cannot clear statistics
 * Branch: California_branch
 *
 * Revision 3.1.6.8  1996/08/14  05:37:22  ahh
 * CSCdi65971:  NAT: need static translations with ports
 * Allow user to configure protocol and ports in a static translation.
 * Branch: California_branch
 *
 * Revision 3.1.6.7  1996/08/13  21:27:42  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.6  1996/07/30  23:43:02  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.1.6.5  1996/06/21  00:22:26  ahh
 * CSCdi60792:  NAT: traceroute not working with overload
 * Extract flow information out of the embedded transport headers.
 * Branch: California_branch
 *
 * Revision 3.1.6.4  1996/06/03  23:23:20  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/13  23:21:10  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:06:38  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:26  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:50  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IP_NAT_PRIVATE_H__
#define __IP_NAT_PRIVATE_H__

#define IPNAT_NO_MAX			0

#define IPNAT_TIMEOUT_DEFAULT		(24 * ONEHOUR)	/* time is in ms */
#define IPNAT_FINRST_TIMEOUT_DEFAULT	(1 * ONEMIN)	/* time is in ms */
#define IPNAT_TCP_TIMEOUT_DEFAULT	(24 * ONEHOUR)	/* time is in ms */
#define IPNAT_UDP_TIMEOUT_DEFAULT	(5 * ONEMIN)	/* time is in ms */
#define IPNAT_DNS_TIMEOUT_DEFAULT	(1 * ONEMIN)	/* time is in ms */
#define IPNAT_ICMP_TIMEOUT_DEFAULT	(1 * ONEMIN)	/* time is in ms */
#define IPNAT_SYN_TIMEOUT_DEFAULT	(1 * ONEMIN)	/* time is in ms */
#define IPNAT_MAX_ENTRIES_DEFAULT	IPNAT_NO_MAX	/* no max # */

#define IPNAT_HASHLEN			2048
#define IPNAT_ENTRY_CHUNK_MAX		200
#define IPNAT_PORT_RANGE_CHUNK_MAX	200

#define IPNAT_NO_PORT	0xffffffff

STRUCTURE_REF(ipnat_entry);
STRUCTURE_REF(ipnat_mapping);
STRUCTURE_REF(ipnat_edit_context);
STRUCTURE_REF(ipnat_seq_delta_pair);
STRUCTURE_REF(ipnat_statistics);
STRUCTURE_REF(ipnat_door);
STRUCTURE_REF(ipnat_fragment);

/*
 * Type of translation entry lookup
 */

enum ipnat_lookup {
    IPNAT_LOCAL, IPNAT_GLOBAL, IPNAT_LOOKUPS
};

enum ipnat_domain {
    IPNAT_INSIDE, IPNAT_OUTSIDE, IPNAT_DOMAINS
};


/*
 * ipnat_entry
 *
 * This structure describes an active translation.  It is threaded
 * on two hash-chains: one hashed on the local address; the other
 * hashed on the global address.  Static entries are also linked together
 * with the other source- or destination-based entries.
 */

struct ipnat_entry_ {
    /* Address translation */
    ipaddrtype address[IPNAT_DOMAINS][IPNAT_LOOKUPS];
    /* Port translation */
    ushort port[IPNAT_DOMAINS][IPNAT_LOOKUPS];

    uchar proto;				/* IP protocol number */

    uchar flags;				/* flags (see below) */
    uchar more_flags;				/* flags used internally */

    ushort refcount;				/* number of accessors */

    ipnat_mapping *mapping[IPNAT_DOMAINS];	/* dynamic mappings used */
    ipnat_edit_context *edit_context;		/* stream editing context */

    /* Times */
    sys_timestamp create_time;			/* when created */
    sys_timestamp used_time;			/* when last used */

    mgd_timer inactivity_timer;			/* for expiring entries */

    ipnat_entry *static_next;			/* linkage */
    ipnat_entry *hash_next[IPNAT_LOOKUPS];	/* linkage */
    ipnat_entry *map_hash_next[IPNAT_LOOKUPS];	/* yet more linkage */
};

/*
 * Translation entry flags
 * Update ipnat_flagnames array when adding flags.
 */

#define IPNF_STATIC	0x01		/* Translation entry is static */
#define IPNF_EXTENDED	0x02		/* Extended entry (has flow info) */
#define IPNF_DEST	0x04		/* Created via dynamic dest trans */
#define IPNF_OUTSIDE	0x08		/* Contains outside mapping */
#define IPNF_TIMING_OUT	0x10		/* Entry is dying */
#define IPNF_EXTENDABLE	0x20		/* Entry is extendable (static) */
#define IPNF_NOALIAS	0x40		/* Entry should not create alias (s) */

/* Internal flags (not displayed) */
#define IPNF2_FIN_IN		0x01	/* saw a fin in */
#define IPNF2_FIN_OUT		0x02	/* saw a fin out */
#define IPNF2_PORT_STOLEN	0x04	/* port stolen, not allocated */
#define IPNF2_PROCESS_DONE	0x08	/* no processing needed anymore */

#define IPNAT_BOTH_FINS(flags)	(((flags) & (IPNF2_FIN_IN|IPNF2_FIN_OUT)) == \
				 (IPNF2_FIN_IN|IPNF2_FIN_OUT))

/*
 * ipnat_mapping
 *
 * Structure to describe address->address dynamic mapping.
 *
 * This auxiliary structure is created when a dynamic translation is
 * configured.  An address matching the access-list should cause an
 * address to be allocated from the pool named by poolname.
 *
 * When a translation is created using one of these mappings, the
 * translation entry will be made to point to it and the mapping's
 * refcount will be incremented.
 *
 * Mappings in use (refcount > 0) cannot be deleted or changed.
 * (Deleting is bad without killing any translation entries that
 * may be referring to the mapping.  Changing is bad because a
 * translation may have allocated an address from the pool.  If
 * the pool name is changed, the translation entry will not be
 * able to return its address to the right pool.)
 *
 * A mapping is linked onto one of two global lists so it can be
 * found and changed or deleted easily.
 */

enum ipnat_match_type {
    IPNAT_MATCH_ACL,
    IPNAT_MATCH_ROUTE_MAP
};

enum ipnat_source_type {
    IPNAT_SOURCE_POOL,
    IPNAT_SOURCE_INTERFACE
};

struct ipnat_mapping_ {
    /* Matching function */
    enum ipnat_match_type match_type;	/* type of matching function */
    acl_headertype *acl;		/* access-list for address */
    char *mapname;			/* route-map name */

    /* Address source */
    enum ipnat_source_type source_type;	/* type of source */
    char *poolname;			/* pool to allocate from */
    idbtype *idb;			/* interface whose address to use */

    /* Housekeeping */
    ushort refcount;			/* count of users of this mapping */
    ushort flags;			/* misc flags */
    list_element element;		/* queueing on ipnat_*_mappings */
};

#define IPNMF_OVERLOAD	0x0001	/* doing source port translation */

/*
 * Information kept for each edit_context.
 */

enum local_type {
    NAT_CLIENT,		/* client is having source translated */
    NAT_SERVER		/* server is having source translated */
};

#define TOKEN_LEN	32

struct ipnat_edit_context_ {
    enum local_type source_type;	/* per-protocol type */
    ushort state;			/* per-protocol state */
    ushort flags;

    ipnat_entry *nat;			/* pointer back to nat entry */

    /*
     * Lexing state.
     */
    uchar token_string[TOKEN_LEN];
    uchar *token_end;
    enum { LEX_UNKNOWN, LEX_NUMBER, LEX_KEYWORD } lex_state;
    long value;

    /*
     * TCP stream information.
     */
    ipnat_seq_delta_pair *sd_pairs;	/* list of deltas */
    ulong sd_count;			/* number of deltas */

    /*
     * FTP edit_context state.
     */
    ipaddrtype port_cmd_addr;		/* IP address for PORT or PASV resp. */
    ushort port_cmd_port;		/* port num for PORT or PASV resp. */
};

/*
 * A sequence number/delta pair.  The sequence number changes
 * by some amount at this point in the stream.
 */

struct ipnat_seq_delta_pair_ {
    ipnat_seq_delta_pair *next;		/* linkage */
    ulong seq;				/* starting sequence number */
    enum ipnat_domain domain;		/* who's sequence number */
    int delta;				/* amount to adjust by */
};

/*
 * Maximum number of pairs per connection.
 * It's not good to have a limit, but we don't want to starve the box
 * of memory.  If we really get this number of pairs, the choice of a
 * linked list to keep track of them will start to hurt.
 */
#define IPNAT_MAX_SEQ_DELTA_PAIRS	100

/*
 * Conversations involving these ports require special treatment
 * by the translator, so cannot be handled at interrupt level.
 */

static inline boolean
ipnat_special_tcpport (ushort port)
{
    return(port == FTP_PORT);
}

static inline boolean
ipnat_special_udpport (ushort port)
{
    return(port == DNS_PORT || port == TFTP_PORT || port == CUSEEME_PORT);
}

static inline boolean
ipnat_special_icmptype (uchar type)
{
    return(type == ICMP_UNREACHABLE || type == ICMP_QUENCH ||
	   type == ICMP_REDIRECT || type == ICMP_TIMEEXCEEDED ||
	   type == ICMP_PARAMETER);
}

static inline enum ipnat_lookup
ipnat_opposite_lookup (enum ipnat_lookup lookup)
{
    return((lookup == IPNAT_LOCAL) ? IPNAT_GLOBAL : IPNAT_LOCAL);
}

static inline enum ipnat_domain
ipnat_opposite_domain (enum ipnat_domain domain)
{
    return((domain == IPNAT_INSIDE) ? IPNAT_OUTSIDE : IPNAT_INSIDE);
}

/*
 * Some statistics
 */

struct ipnat_statistics_ {
    tinybool initialized;	/* Structures allocated, etc. */
    ulong entries;		/* Total translations */
    ulong statics;		/* Total static translations */
    ulong flows;		/* Total flows */
    ushort insides;		/* Number of inside interfaces */
    ushort outsides;		/* Number of outside interfaces */

    /*
     * Clearable counters start here.
     * If you add a counter here, you must add code to zero it
     * in the clear routine.
     */
    ulong entry_timeouts;	/* Number of entries which timed out */
    ulong hits;			/* Successful searches */
    ulong misses;		/* Unsuccessful searches */
};

/*
 * Doors are instructions to the outside->inside translation code to
 * create a translation for a packet that will arrive in reply to an
 * inside->outside packet.  The outside port won't be known until the
 * packet comes back.
 */

struct ipnat_door_ {
    ipaddrtype outsidehost, insidehost;
    ushort outsideport;
    ushort insideport[IPNAT_LOOKUPS];
    ushort serverport;			/* original outside port */
    ushort clientport;			/* original inside port */
    uchar proto, trigger_proto;
    uchar flags;
    mgd_timer timeout_timer;		/* for expiring doors */
    list_element element;		/* queueing */
};

/* flags */
#define IPNDF_OUTSIDEHOST_ANY	0x01	/* allow any outside host address */
#define IPNDF_PORT_ALLOCATED	0x02	/* insideport[IPNAT_GLOBAL] alloced */
#define IPNDF_KNOWN_PORT	0x04	/* outsideport known */

#define IPNAT_DOGGY_DOOR_TIMEOUT	(1 * ONEMIN)

extern list_header ipnat_door_list;

static inline boolean
ipnat_pending_doors (void)
{
    return(!LIST_EMPTY(&ipnat_door_list));
}

struct ipnat_fragment_ {
    ipaddrtype srcadr, dstadr;
    ushort id;
    uchar prot;
    ushort sport, dport;
    mgd_timer timeout_timer;		/* for expiring fragments */
    list_element element;		/* queueing */
};

/*
 * This timeout is much too aggressive -- should be 1 to 2 minutes,
 * but we must prevent over-allocation of memory...
 */
#define IPNAT_FRAGMENT_TIMEOUT		(15 * ONESEC)

/*
 * Message types
 */

enum ipnat_msgtype {
    IPNAT_MSG_INSTALL_ALIAS
};

/*
 * Parsing
 */

#define IPNAT_OVERLOAD_SET	0x0001
#define IPNAT_EXTEND_SET	0x0002
#define IPNAT_NOALIAS_SET	0x0004

/*
 * Timer related information
 */

struct ipnat_timeout {
    int timeout;
    int enabled;
    int default_timeout;
};

/*
 * The types of timeouts we can do
 */

enum {
    IPNAT_TIMEOUT,
    IPNAT_TCP_TIMEOUT,
    IPNAT_UDP_TIMEOUT,
    IPNAT_FINRST_TIMEOUT,
    IPNAT_SYN_TIMEOUT,
    IPNAT_DNS_TIMEOUT,
    IPNAT_ICMP_TIMEOUT,
    IPNAT_NUM_TIMEOUTS
};

enum {
    IPNAT_TIMEOUT_VALUE,
    IPNAT_TIMEOUT_IMMEDIATE,
    IPNAT_TIMEOUT_NEVER
};

/*
 * Externs and prototypes
 * Private NAT stuff
 */

extern ipnat_entry *ipnat_statics[IPNAT_DOMAINS][IPNAT_LOOKUPS];
extern mgd_timer ipnat_entry_timer_master, ipnat_door_timer_master,
    ipnat_fragment_timer_master;
extern ipnat_statistics ipnat_stats;
extern struct ipnat_timeout ipnat_timeouts[IPNAT_NUM_TIMEOUTS];
extern list_header ipnat_mappings[IPNAT_DOMAINS][IPNAT_LOOKUPS];
extern pid_t ipnat_ager_pid;
extern int ipnat_max_entries;


/*
 * ipnat.c
 */
extern boolean ipnat_accesscheck(ipnat_entry *, int);
extern void ipnat_get_ports(iphdrtype *, ushort *, ushort *, uchar *,
			    ipaddrtype *, boolean *);


/*
 * ipnat_db.c
 */

extern void ipnat_free_all_hash (void);

extern ipnat_entry *ipnat_create_entry(ipaddrtype, ipaddrtype, ushort, ushort,
				       ipaddrtype, ipaddrtype, ushort, ushort,
				       uchar, uchar);
extern void ipnat_delete_entry(ipnat_entry *);
extern ipnat_entry *ipnat_find_entry(ipaddrtype, ipaddrtype, ushort, ushort,
				     uchar, enum ipnat_lookup);
extern ipnat_mapping *ipnat_find_mapping(paktype *, ipaddrtype,
					 enum ipnat_lookup, enum ipnat_domain);

extern boolean ipnat_establish_alias(ipaddrtype, boolean);

extern ipnat_entry *ipnat_clone_entry(ipnat_entry *, ipnat_entry *,
				      ipnat_mapping *, ushort, uchar,
				      ipaddrtype, ushort);

extern void ipnat_portlist_init(void);
extern uint ipnat_allocate_port(ipnat_mapping *, uchar, ipaddrtype,
				  ipaddrtype, ushort, enum ipnat_lookup);
extern void ipnat_free_port(uchar, ipaddrtype, ushort, enum ipnat_lookup);
extern void ipnat_reap_portlists(ipnat_mapping *);

extern ipnat_entry *ipnat_new_inside_entry(ipnat_entry *, paktype *,
					   ipaddrtype, ipaddrtype, ushort,
					   ushort, uchar, boolean, boolean *);
extern ipnat_entry *ipnat_new_inside_dest_entry(ipnat_entry *, paktype *,
						ipaddrtype, ipaddrtype, ushort,
						ushort,	uchar, boolean *);
extern ipnat_entry *ipnat_new_outside_entry(paktype *, ipaddrtype, boolean *);

extern ipnat_edit_context *ipnat_create_edit_context(iphdrtype *,
						     ipnat_entry *,
						     enum local_type,
						     enum ipnat_lookup);
extern void ipnat_delete_edit_context(ipnat_edit_context *);

extern ipnat_door *ipnat_create_door(void);
extern void ipnat_add_door(ipnat_door *);
extern void ipnat_remove_door(ipnat_door *);
extern void ipnat_destroy_door(ipnat_door *);
extern boolean ipnat_setup_door(ipaddrtype, ipaddrtype, ushort, ushort,
				ushort, ushort, ushort, int, int, int);
extern ipnat_door *ipnat_find_door(uchar, ipaddrtype, ipaddrtype, ushort,
				   ushort, enum ipnat_lookup);
extern ipnat_entry *ipnat_open_door(uchar, ipaddrtype, ipaddrtype, ushort,
				    ushort, paktype *);

extern ipnat_fragment *ipnat_create_fragment(ipaddrtype, ipaddrtype, ushort,
					     uchar, ushort, ushort);
extern void ipnat_add_fragment(ipnat_fragment *);
extern void ipnat_remove_fragment(ipnat_fragment *);
extern void ipnat_destroy_fragment(ipnat_fragment *);
extern ipnat_fragment *ipnat_find_fragment(ipaddrtype, ipaddrtype, ushort,
					   uchar);

extern ipnat_seq_delta_pair *ipnat_create_seq_delta_pair(ipnat_edit_context *);
extern void ipnat_remove_seq_delta_pair(ipnat_seq_delta_pair *,
					ipnat_edit_context *);
extern void ipnat_destroy_seq_delta_pair(ipnat_seq_delta_pair *,
					 ipnat_edit_context *);
extern void ipnat_destroy_all_seq_delta_pairs(ipnat_edit_context *);

extern void ipnat_show_entries(boolean);
extern void ipnat_show_stats(void);
extern void ipnat_db_init(void);

/*
 * ipnat_fixup.c
 */
extern paktype *ipnat_l3_fixup(paktype *, iphdrtype *, ipnat_entry *,
			       enum ipnat_lookup);

#endif /* __IP_NAT_PRIVATE_H__ */
