/* $Id: pim.h,v 3.8.10.13 1996/09/13 22:52:53 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim.h,v $
 *------------------------------------------------------------------
 * pim.h - Structure and Constant defintions for PIM.
 *
 * November, 1993
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Structure and Constant defintions for the IP Protocol Independennt
 * Multicast protocol. This protocol provides IP multicast routing support
 * for dense and sparsely populated groups.
 *------------------------------------------------------------------
 * $Log: pim.h,v $
 * Revision 3.8.10.13  1996/09/13  22:52:53  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.8.10.12  1996/09/12  21:09:34  chwhite
 * CSCdi61325:  (*,G) JOINS received by the RP do not trigger (S,G) JOINS
 * to source
 * Branch: California_branch
 *
 * Revision 3.8.10.11  1996/09/10  05:32:01  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.8.10.10  1996/08/28  12:51:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.10.9  1996/08/22  01:37:39  lwei
 * CSCdi66702:  PIM-1-FAST_RP_REACHABLE log message should be removed
 * Branch: California_branch
 *
 * Revision 3.8.10.8  1996/08/21  01:21:32  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.8.10.7  1996/08/14  05:03:46  lwei
 * CSCdi64491:  Check (*,G) OIF list when new PIM neighbor comes up
 * Branch: California_branch
 *
 * Revision 3.8.10.6  1996/08/09  03:33:39  mleelani
 * CSCdi65587:  (S, G) state gratuitously created on receiving prunes.
 * Branch: California_branch
 * Create (S, G) state only on receiveing (S, G, RP-bit) prunes.
 *
 * Revision 3.8.10.5  1996/08/09  03:19:14  mleelani
 * CSCdi61876:  (*,G) mcache entries do not work
 * Branch: California_branch
 * o Correct SPT threshold behaviour.
 * o Allow (*, G) fastswitching if on the shared tree.
 *
 * Revision 3.8.10.4  1996/08/07  23:50:53  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/08/03  01:20:44  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.8.10.2  1996/06/11  02:50:31  lwei
 * CSCdi59607:  RP-reachability messages need to be rate-limited
 * Branch: California_branch
 *
 * Revision 3.8.10.1  1996/04/03  18:39:55  dino
 * CSCdi51024:  PIM nbma-mode adds too many interfaces to olist
 * Branch: California_branch
 *
 * Revision 3.8  1996/02/22  09:03:19  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.7  1996/02/21  17:47:42  dino
 * CSCdi43762:  Register flags get set to 0xffffffff in DVMRP/PIM border
 *              router.
 *
 * Revision 3.6  1996/02/08  03:39:30  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.5  1995/12/17  19:24:28  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.4  1995/12/06  20:23:48  mikel
 * CSCdi33290:  HSRP + PIM on low-end fddi if = packet replication
 * disable PIM and HSRP config on low end fddi due to the non ability
 * to program CAM.  Disallow for BSI chipset only.
 *
 * Revision 3.3  1995/11/21  23:12:15  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:30:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/25  19:18:51  dino
 * CSCdi37594:  Delay pruning on LAN is expiring just before Join override
 *
 * Revision 2.3  1995/07/24  07:33:52  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.2  1995/07/01 07:46:19  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.1  1995/06/07  21:04:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PIM_H__
#define __PIM_H__

/*
 * Constant definitions.
 */
#define PIM_VERSION1    0x10000000
#define PIM_DENSE_MODE  0
#define PIM_SPARSE_MODE 1
#define PIM_SPARSE_DENSE_MODE 2

#define PIMV1 0x1
#define PIMV2 0x2

#define PIM_QUERY_INTERVAL    (30 * ONESEC)
#define PIM_MESSAGE_INTERVAL  (ONEMIN)
#define PIM_STACK_SIZE        1500
#define PIM_PERIODIC_INTVL    ONESEC
#define PIM_MULTIPLIER        3
#define PIM_PRUNE_DELAY       (4 * ONESEC + ONESEC / 2)
#define PIM_JOIN_DELAY_MAX    3
#define PIM_GRAFT_FREQUENCY   (3 * ONESEC)
#define PIM_ASSERT_FREQUENCY  (1 * ONESEC)
#define PIM_RP_REACH_INTERVAL (90 * ONESEC)
#define PIM_RP_REACH_HOLDTIME (3 * PIM_RP_REACH_INTERVAL)
#define PIM_RP_EXPIRES        (3 * ONEMIN)
#define PIM_BRING_UP_RP       (90 * ONESEC)
#define PIM_NULL_REG_INTERVAL (60 * ONESEC)
#define PIM_JITTER_PCT        3
#define PRM_JITTER_PCT	      3
#define PIM_INFINITE_THRESHOLD -1
#define PIM_RP_ANNOUNCE_HOLDTIME  (3 * PIM_RP_ANNOUNCE_INTVL / ONESEC)
#define PIM_RP_DISCOVERY_HOLDTIME (3 * PIM_RP_MAPPING_INTVL / ONESEC)
#define PIM_RP_ANNOUNCE_INTVL (ONEMIN)
#define PIM_RP_MAPPING_INTVL  (ONEMIN)

#define PIM_QUERY_CODE         0
#define PIM_REGISTER_CODE      1
#define PIM_REGISTER_STOP_CODE 2
#define PIM_JOIN_PRUNE_CODE    3
#define PIM_RP_REACHABLE_CODE  4
#define PIM_ASSERT_CODE        5
#define PIM_GRAFT_CODE         6
#define PIM_GRAFT_ACK_CODE     7

/*
 * PIM version 2 message types
 */
#define PIMV2_HELLO_TYPE       0
#define PIMV2_REGISTER_TYPE    1
#define PIMV2_REG_STOP_TYPE    2
#define PIMV2_JOIN_PRUNE_TYPE  3
#define PIMV2_BOOTSTRAP_TYPE   4
#define PIMV2_ASSERT_TYPE      5
#define PIMV2_GRAFT_TYPE       6
#define PIMV2_GRAFT_ACK_TYPE   7
#define PIMV2_CAND_RP_ADV_TYPE 8

#define PIM_AUTORP_V1	       1
#define PIM_RP_ANNOUNCE_TYPE   1
#define PIM_RP_DISCOVERY_TYPE  2

/*
 * Packet format definitions.
 */
#define PIM_QUERY_SIZE         4
#define PIM_REGISTER_STOP_SIZE 8

#define PIMV2_REGISTER_SIZE    4      /* Size not including pim header */
#define PIMV2_REGISTER_STOP_SIZE 14   /* Size not including pim header */
 
#define PIMV2_NULL_REGISTER_BIT 0x40000000    /* Null register bit in header */
#define HOST_MASK_LEN 32  /* Number of bit-1's in a host mask */

typedef struct pimtype_ {
    ipaddrtype address;       /* Target upstream neighbor address */
    ushort     reserved1;     /* Sent as 0, ignored on receipt */
    ushort     holdtime;      /* Time-out receiver should use */
    uchar      reserved2;     /* Sent as 0, ignored on receipt */
    uchar      maddr_length;  /* Length of group addresses in packet, bytes */
    uchar      addr_length;   /* Length of source addresses in packet, bytes */
    uchar      num_groups;    /* Number of groups present in packet */
    char       data[0];       /* Variable length fields begin here */
} pimtype;

typedef struct pim_grouptype_ {
    ipaddrtype group;             /* IP multicast group address */
    ipaddrtype group_mask;        /* Wildcard mask to aggregate group info */
    ushort     num_join_sources;  /* Number of sources to join in packet */
    ushort     num_prune_sources; /* Number of sources to prune in packet */
} pim_grouptype;

typedef struct pim_addrtype_ {
    uchar pad: 7;                      /* Sent as 0, ignored on receipt */
    uchar sparse_bit: 1;               /* This group is in sparse-mode */
    uchar wc_bit: 1;                   /* This is applied to (*,G) */
    uchar rp_bit: 1;                   /* Should be sent towards RP */
    uchar mask_length: 6;              /* Length of netmask in bits */
    uchar address[sizeof(ipaddrtype)]; /* Source IP address */
} pim_addrtype;

/*
 * Need this for the R4000 since sizeof() returns byte count in multiple of
 * longwords.
 */
#define PIM_ADDRTYPE_SIZE 6

/*
 * RP-reachable message packet format.
 */
typedef struct pim_rpreach_type_ {
    ipaddrtype group;
    ipaddrtype group_mask;
    ipaddrtype rp_address;
    ushort     reserved;
    ushort     holdtime;
} pim_rpreach_type;

/*
 * Assert packet message packet format.
 */
typedef struct pim_asserttype_ {
    ipaddrtype group;
    ipaddrtype group_mask;
    ipaddrtype host_address;
    ulong      metric_preference;
    ulong      metric;
} pim_asserttype;

#define PIM_ASSERT_RP_BIT   0x80000000
#define PIM_ASSERT_INFINITY 0x7fffffff

/*
 * PIM version 2 packet header structure.
 */
typedef struct pimv2_header_ {
    uchar ver:4;      /* PIM version */
    uchar type:4;     /* PIM message type */
    uchar reserved;   /* Address length */
    ushort checksum;  /* Checksum */
} pimv2_header;

/*
 * PIM neighbor definition.
 */
struct pim_nbrtype_ {
    struct pim_nbrtype_ *next;
    ipaddrtype          nbr_address;
    sys_timestamp       uptime;
    sys_timestamp       expires;
    ulong               mode;
};

/*
 * PIM parameter block for parsing Join/Prune packets.
 */
typedef struct pim_parmtype_ {
    pimtype       *pim;          /* Input */
    ulong         paklen;        /* Input */
    pim_grouptype *group;        /* Output */
    ulong         groups_left;   /* Internal use */
    pim_addrtype  *source;       /* Output */
    ulong         sources_left;  /* Internal use */
    boolean       join_list;     /* Output */
} pim_parmtype;

/*
 * PIM spt threshold structure definition.
 */
typedef struct pim_spttype_ {
    struct pim_spttype_ *next; /* Next item in linked list */
    ulong spt_threshold;       /* In bits per second */
    acl_headertype *group_acl; /* Simple access-list, NULL means all groups */
} pim_spttype;

/*
 * PIM periodic selection entry type.
 */
typedef struct pim_entrytype_ {
    tinybool send_join_toward_rp;     /* Source listed in join packet to RP */
    tinybool send_join_toward_source; /* Source listed in join to source's 
					 RPF nbr which is different from RP*/
    tinybool send_rpbit_prune;        /* (S, G, RP) bit prune */
} pim_entrytype;

/*
 * RP accept filter structure defintion.
 */
typedef struct pim_rpfilter_ {
    struct pim_rpfilter_ *next;
    ipaddrtype           address;
    acl_headertype       *group_acl;
    boolean              autorp;
} pim_rpfilter;

/*
 * PIM Auto-RP RP announcement structure
 */
typedef struct pim_rp_announce_type_ {
    struct pim_rp_announce_type_ *next;  /* Next RP to be announced */
    ipaddrtype      rp_address;     /* IP address of announced RP */
    acl_headertype *group_acl;      /* Access-list for group prefix */
    ulong           ttl;            /* Scope of this announcement */
    idbtype         *idb;           /* Interface with address IP */
    ulong           holdtime;       /* Holdtime to be advertised */
} pim_rp_announce_type;

typedef struct pim_rp_afilter_type_ {
    struct pim_rp_afilter_type_ *next;
    acl_headertype *rp_acl;
    acl_headertype *group_acl;
} pim_rp_afilter_type;


/*
 * PIM auto-rp announcement/discovery packet header structure
 */
typedef struct pim_autorp_header_ {
     uchar    version:4;      /* Version of Auto-RP */
     uchar    type:4;         /* Type of message */
     uchar    rp_count;       /* Number of announced RPs in this packet */
     ushort   holdtime;       /* Number of seconds to keep the state */
     ulong    reserved;       /* Sent as zero, ignored when received */
     uchar    data[0];        /* Beginning of variable part */
} pim_autorp_header;

/*
 * PIM auto-rp RP/group-prefix block in announcement packets
 */
typedef struct pim_autorp_block_ {
     uchar rp[ADDRLEN_IP];   /* RP's address, not aligned on 4 byte boundary */
     uchar reserved_and_ver; /* 6-bits reserved, then 2 bits PIM version */
     uchar grp_count;        /* Number of group prefixes for this RP */
     uchar grp_prefix[PIM_ADDRTYPE_SIZE]; /* 1st encoded group address */
} pim_autorp_block;

/*
 * Macro extracting the RP version from the reserved_and_ver field
 */
#define AUTORP_GET_VER(byte) (byte & 0x3)

/*
 * PIM encoded group address, used in autorp packets
 */
typedef struct pim_encoded_group_type_ {
     uchar reserved;  /* Sent as zero, ignored on receipt */
     uchar mask_len;  /* Length of mask */
     uchar address[sizeof(ipaddrtype)]; /* Multicast Group address prefix */
} pim_encoded_group;

/*
 * PIM rp_mapping structure definition
 */
typedef struct pim_rp_mapping_ {
     rntype             radix_nodes[2]; /* Radix tree pointers */
     ipradixtype        group_prefix;   /* Group address prefix */
     ipradixtype        group_mask;     /* Group address mask */
     ipaddrtype         rp_addr;        /* IP address of RP */
     ipaddrtype         info_source;    /* Source announced this mapping */
     sys_timestamp      expires;        /* When to expire */
     sys_timestamp	uptime;
     uchar              rp_version;     /* RP's PIM version */
} pim_rp_mapping;

/*
 * PIM RP structure definition. Contains RP information.
 */
typedef struct pim_rptype_ {
    struct pim_rptype_ *next;          /* Next RP */
    ipaddrtype         rp_address;     /* IP address of RP */
    acl_headertype     *group_acl;     /* What groups RP is responsible for */
    sys_timestamp      expires;        /* Global expiration timer */
    sys_timestamp      send_register;  /* When to send NULL Register */
    ulong              state;          /* RP state independent of group */
    ulong              send_null_reg;  /* Set when Reg sent to any group */
    pim_rp_mapping     *prm;           /* rp_mapping, used for learned RPs */
    uchar	       version;        /* Version of the RP, for register */
    tinybool           override;       /* "override" keyword in effect */
    tinybool	       null_register_ver; /* to probe RP's version */
    tinybool           configured;     /* Is this manually configured? */
} pim_rptype;

/*
 * PIM macro definitions.
 */
#define PIM_DEBUG(Group)      \
    (pim_debug && (!pim_group_debug || pim_group_debug == Group || !Group))

#define PIM_BUGINF(Group,Str) if (PIM_DEBUG(Group)) buginf Str

/*
 * Auto-RP macro
 */
#define AUTORP_BUGINF if (pim_autorp_debug) buginf

/*
 * Test if interface should be in dense mode or sparse mode for mroute 
 * entry mdb.
 */
#define IS_DENSE_MODE(idb, gdb) \
    (idb->pim_mode == PIM_DENSE_MODE || \
	(idb->pim_mode == PIM_SPARSE_DENSE_MODE && !IS_SPARSE_GROUP(gdb)))

/*
 * Testing for PIM version number for incoming packets.
 */
#define IS_PIM_VERSION1(Version) ((Version & 0xf0000000) == 0x10000000)

/*
 * Random PIM-DM backoff before sending joins and prunes.
 */
#define PIM_RANDOM_DELAY(Mdb) \
    ((((Mdb->if_input->ip_address ^ random_gen()) % PIM_JOIN_DELAY_MAX) + 1) \
     * ONESEC)

/*
 * Get mode and holdtime fields out of PIM Query packet.
 */
#define PIM_QUERY_GET_MODE(Field) (Field >> 28)
#define PIM_QUERY_GET_HT(Field)   (Field & 0xffff)

/*
 * Set Register flags bit field. This takes an RP count and sets all bits
 * in the flags field. Each RP has a handle which maps to a bit position in
 * the flags field.
 */
#define PIM_SET_REG_FLAGS(Count) (~(0xffffffff << Count))

/*
 * Take handle value (bit position), and shift so that bit position is a 1.
 * Used for testing that bit.
 */
#define PIM_HANDLE_TO_BIT(Handle) (1 << Handle)

/*
 * Test if the RP is capable of doing PIM version 2
 */
#define PIM_RP_IS_V2(ver) (ver & PIMV2)
#define PIM_RP_IS_V1(ver) (ver & PIMV1)

/*
 * PIM traffic statistics for protocol type PIM_PROT
 */
typedef struct pim_traffic_t_ {
    ulong inputs;
    ulong formaterr;
    ulong checksumerr;
    ulong input_registers;
    ulong input_register_stops;
    ulong output_registers;
    ulong output_register_stops;
    ulong input_hellos;
    ulong output_hellos;
    ulong input_join_prunes;
    ulong output_join_prunes;
    ulong input_bootstraps;
    ulong output_bootstraps;
    ulong input_asserts;
    ulong output_asserts;
    ulong input_grafts;
    ulong output_grafts;
    ulong input_graft_acks;
    ulong output_graft_acks;
    ulong input_cand_rp_advers;
    ulong output_cand_rp_advers;
    ulong outputs;
} pim_traffic_t;
extern pim_traffic_t pim_traffic;

/*
 * PIMv2 encoded group address type
 */
typedef struct pimv2_encoded_group_ {
    uchar addr_family;	   /* Address family */
    uchar encode_type;     /* Encoding type */
    uchar reserved;        /* Sent as 0, ignored on receipt */
    uchar mask_len;        /* Mask length */
    ipaddrtype group_addr; /* Group address */
} pimv2_encoded_group;

/*
 * PIMv2 "unicast address" encoding, e.g. in register-stop messages
 */
typedef struct pimv2_unicast_address_ {
    uchar addr_family;     /* Address family */
    uchar encode_type;     /* Encoding type */
    uchar address[4];      /* IP address, compiler aligns uchar correctly */
} pimv2_unicast_address;

/*
 * ---------- External data declarations. ----------
 */
extern watched_queue *pimQ;
extern queuetype pim_configured_rp;
extern queuetype pim_learned_rp;
extern queuetype pim_announced_rp;
extern queuetype pim_rp_announce_filter;
extern ulong pim_message_interval;
extern pim_spttype *pim_spt_thresholds;
extern sys_timestamp pim_rp_announce_timer;
extern sys_timestamp pim_rp_mapping_timer;
extern ipsocktype *rp_agent_socket;
extern ipsocktype *pim_rp_discovery;
extern int pim_autorp_min_mtu;
extern int pim_rp_mapping_ttl;
extern rnhtype *pim_autorp_cache;
extern boolean pim_has_new_rp;
extern boolean pim_immediate_register_stop;
extern ipparamtype pimrouteparams;
extern ulong pim_version;

/*
 * ---------- External function declarations. ----------
 */

/*
 * pim.c
 */
extern void pim_init(void);
extern void pim_init_idb(idbtype *);
extern uchar *pim_code_string(ulong);
extern boolean pim_accept_rp(ipaddrtype, ipaddrtype);
extern void pim_accept_rp_command(parseinfo *);
extern void pim_commands(parseinfo *);
extern void pim_enable_idb(idbtype *);
extern void pim_disable_idb(idbtype *);
extern void pim_idb_comingup(idbtype *);
extern void pim_idb_goingdown(idbtype *);
extern void pim_show_commands(parseinfo *);
extern void pim_process_timers(void);
extern void pim_send_query(idbtype *);
extern void pim_receive_query(igmptype *, ipaddrtype, idbtype *, ulong);
extern pim_nbrtype *pim_find_nbr(ipaddrtype, idbtype *);
extern void pim_expire_nbrs(idbtype *);
extern void pim_show_neighbors(parseinfo *);
extern void pim_show_interface(parseinfo *);
extern void pim_spt_threshold_command(parseinfo *);
extern void pim_init_parm(pim_parmtype *, pimtype *, ulong);
extern boolean pim_validate_header(pimtype *, int);
extern boolean pim_parse_packet(pim_parmtype *);
extern boolean pim_parse_getnext_group(pim_parmtype *);
extern boolean pim_parse_goto_prunes(pim_parmtype *);
extern void pim_send_join_prune(mdbtype *, idbtype *, ipaddrtype, boolean, 
				boolean, boolean, boolean);
extern void pim_receive_join_prune(igmptype *, idbtype *, ipaddrtype, ulong);
extern void pim_receive_prune(pim_parmtype *, idbtype *, ipaddrtype, 
			      ipaddrtype, ulong);
extern void pim_receive_join(pim_parmtype *, idbtype *, ipaddrtype, 
			     ipaddrtype, ulong);
extern void pim_prune_interface(gdbtype *, mdbtype *, midbtype *, ulong);
extern void pim_add_mroute_idb(idbtype *, ipaddrtype, boolean);
extern int pim_add_one_mroute_idb(rntype *, va_list);
extern void pim_schedule_join(mdbtype *);
extern void pim_add_mroute(gdbtype **, mdbtype **, ipaddrtype, ipaddrtype, 
                           ipaddrtype, idbtype *, ipaddrtype, ulong,  
			   ulong, boolean);
extern void pim_delink_packet(paktype **, paktype *);
extern void pim_get_metric(ipaddrtype, uchar, ulong *, ulong *);
extern boolean pim_compare_metrics(ipaddrtype, ulong, ulong, ipaddrtype, ulong,
				   ulong);
extern boolean pim_check_hsrp(idbtype *);
extern void pim_send_rp_announce(void);
extern void pim_receive_rp_announce(void);
extern void pim_access_list_callback(acl_headertype *);
extern boolean pim_rp_needs_to_join(gdbtype *, mdbtype *);

/*
 * pim-dm.c
 */
extern void pim_send_assert(gdbtype *, mdbtype *, midbtype *, ipaddrtype);
extern void pim_receive_assert(igmptype *, ipaddrtype, idbtype *, int);
extern void pim_send_graft(gdbtype *);
extern int pim_build_graft_entry(rntype *, va_list);
extern void pim_send_graft_packet(paktype *, ipaddrtype);
extern void pim_receive_graft(igmptype *, ipaddrtype, idbtype *, ulong);
extern void pim_send_graft_ack(paktype *, ulong);
extern void pim_receive_graft_ack(igmptype *, ipaddrtype, idbtype *, ulong);
extern void pim_receive_rp_discovery(void);

/*
 * pim-sm.c
 */
extern void pim_rp_command(parseinfo *);
extern void pim_show_rp(parseinfo *);
extern void pim_display_rp(gdbtype *);
extern boolean pim_get_all_rps(gdbtype *, boolean, boolean);
extern pim_rptype *pim_find_rp(ipaddrtype, ipaddrtype);
extern void pim_store_rp_info(gdbtype *, ipaddrtype, ulong, boolean, boolean);
extern boolean pim_rp_rpf_lookup(gdbtype *, ipaddrtype);
extern void pim_age_rp(gdbtype *);
extern void pim_send_rp_reach(gdbtype *);
extern void pim_receive_rp_reach(paktype *, igmptype *, ulong);
extern void pim_send_periodic(gdbtype *);
extern void pim_send_batch_join(gdbtype *);
extern boolean pim_init_packet_buffer(gdbtype *, paktype **, paktype **, 
				      boolean);
extern int pim_build_entry(rntype *, va_list);
extern void pim_send_rp_packet(gdbtype *, paktype *, paktype *);
extern void pim_send_packet(paktype *, ipaddrtype, boolean, boolean);
extern void pim_send_sm_join(gdbtype *, mdbtype *);
extern void pim_send_register(gdbtype *, mdbtype *, paktype *);
extern void pim_send_register_packet(paktype *, ipaddrtype);
extern void pimv1_receive_register(paktype *, igmptype *, idbtype *);
extern void pim_send_register_stop(ipaddrtype, ipaddrtype, ipaddrtype, 
				   ipaddrtype, uchar);
extern void pimv1_receive_register_stop(igmptype *, ipaddrtype, ulong);
extern void pimv2_receive_register_stop(pimv2_header *, ipaddrtype, ulong);
extern void pim_send_null_register(void);
extern boolean pim_over_threshold(ulong, gdbtype *);
extern void pim_send_rp_mapping(void);
extern pim_rp_mapping *pim_get_best_rp_mapping(ipaddrtype);
extern void ip_pim_autorp_ager(void);
extern void pim_delete_sparse_dense_oif(gdbtype *);
extern void pimv2_receive_register(paktype *, pimv2_header *, idbtype *);
extern void pim_clear_rp_state(gdbtype *);

/*
 * pim_chain.c
 */
extern void pim_parser_init(void);

/*
 * pim-autorp.c
 */
extern boolean pim_add_prm(ipaddrtype, ipaddrtype, ipaddrtype, ipaddrtype,
			 ulong, boolean, uchar);
extern pim_rp_mapping *pim_get_prm(ipaddrtype, ipaddrtype);
extern void pim_delete_prm(pim_rp_mapping *);
extern void pim_check_rp_lists(acl_headertype *);
extern void pim_autorp_reset_known_groups(boolean, boolean);
extern void pim_receive_autorp_packet(paktype *, udptype *,
				      pim_autorp_header *);
extern boolean pim_enable_rp_discovery(idbtype *);
extern void ip_clear_autorp_cache(void);
extern void clear_autorp_command(parseinfo *);
extern void pim_set_rp_ann_holdtime(pim_rp_announce_type *, ulong);

#endif __PIM_H__
