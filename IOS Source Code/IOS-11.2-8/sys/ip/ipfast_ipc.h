/* $Id: ipfast_ipc.h,v 3.4.10.8 1996/08/16 08:02:06 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_ipc.h,v $
 *------------------------------------------------------------------
 * IPC Distributed Flow-Switching Application defines for RSP/VIP.
 *
 * July 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_ipc.h,v $
 * Revision 3.4.10.8  1996/08/16  08:02:06  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.4.10.7  1996/08/12  06:12:18  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.4.10.6  1996/06/06  08:58:44  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.4.10.5  1996/06/05  05:08:54  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/05/12  23:09:31  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/04/19  15:41:47  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.7  1996/03/30  05:39:09  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.1.2.6  1996/03/30  05:19:06  carrel
 * cvs fixes
 *
 * Revision 3.1.2.5  1996/03/21  10:12:45  xliu
 * Branch: IosSec_branch
 * Modified comments.
 *
 * Revision 3.1.2.4  1996/03/21  10:04:13  xliu
 * Branch: IosSec_branch
 * Migrated to V111_1_3.
 *
 * Revision 3.1.2.3  1996/03/19  08:09:06  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.2  1996/01/31  04:45:25  xliu
 * Branch: IosSec_branch
 * Add RSP/VIP encryption feature code.
 *
 * Revision 3.1.2.1  1996/01/23  05:43:14  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.4.10.2  1996/04/11  00:32:20  dlobete
 * CSCdi50256:  DFS displays incorrect statistics for output MIP interface
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/04/10  03:38:25  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/24  02:14:22  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.3  1996/01/19  03:02:49  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:34:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:08  dlobete
 * Placeholders for VIP_branch development.
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MAX_IP_REWRITE 100

typedef struct ipc_ipfast_msg_hdr_ {
    ulong ipfast_type;
    ulong msg_size;
} ipc_ipfast_msg_hdr;

typedef struct ipc_dfs_add_msg_ {
    ipc_ipfast_msg_hdr	hdr;
    ulong             	hw_if_index;
    ulong		slot_and_unit;
    ulong		is_vip;
    ulong		idbtype;
    ulong		txq_ptr;
    ulong		tql_ptr;
    ulong               flags;
    ipaddrtype         	prefix;
    ipaddrtype         	mask;
    ipaddrtype         	nexthop;
    ulong		ip_mtu;
    ulong		output_acl;
    ulong		queue_algorithm;
    ulong               encaps_length;          /* encap size minus AC/FC */
    ulong		encaps_type;		/* FS type eg FS_ETHER */
    uchar  		mac_rewrite[0];
} ipc_ipfast_add_msg;

/*
 * Populating new txptr and txacc of each hwiidb to IP after EOIR
 */
typedef struct ipc_ipfast_update_idb_msg_ {
    ipc_ipfast_msg_hdr	hdr;
    ulong 		hw_if_index; 
    ulong		txq_ptr;
    ulong		tql_ptr;
} ipc_ipfast_update_idb_msg;

/*
 * Send down the RSP's notion of hw_if_index to VIPs after memd_recarves.
 */
typedef struct ipc_ipfast_hwindex_elt_ {
    ulong		hw_if_index;
    ulong		slot;
    ulong		slotunit;
    long   		vc;
}ipc_ipfast_hwindex_elt;

typedef struct ipc_ipfast_update_hwindex_msg_ {
    ipc_ipfast_msg_hdr      hdr;
    ulong 		    num_indices;
    ipc_ipfast_hwindex_elt  data[0];   /* bunch of ipc_ipfast_hwindex_elt */
} ipc_ipfast_update_hwindex_msg;

/*
 * This is used for figuring out size of pre-allocated msg.
 */
typedef struct {
    ipc_ipfast_add_msg add_msg;
    char               rewrite[MAX_IP_REWRITE];
} ipc_ipfast_max_add_msg;

typedef struct ipc_ipfast_inv_msg_ {
    ipc_ipfast_msg_hdr	hdr;
    ipaddrtype		prefix;
    ipaddrtype		mask;
} ipc_ipfast_inv_msg;

typedef struct ipc_ipfast_stat_msg_ {
    ipc_ipfast_msg_hdr	hdr;
    ulong num_stat_units; /* how many interfaces' stats in this message */
    uchar stats_data[0]; 
} ipc_ipfast_stat_msg;

typedef struct ipc_ipfast_dfs_cfgreq_msg_ {
    ipc_ipfast_msg_hdr  hdr;
    ulong              hw_if_index; 
} ipc_ipfast_dfs_cfgreq_msg;

/* 
 * All statistics about an interface.
 * A bunch of these will occupy the data portion of the ipc_ipfast_stats_msg.
 */
typedef struct ipc_ipfast_stat_unit_ {
    ulong if_index;
    ulong rx_bytes;
    ulong rx_packets;
    ulong tx_bytes;
    ulong tx_packets;
    ulong tx_drops;
    ulong throttled;
} ipc_ipfast_stat_unit;

typedef struct ipc_ipfast_en_dis_msg_ {
    ipc_ipfast_msg_hdr hdr;
    ulong hw_if_index;
    ulong ip_routecache;
} ipc_ipfast_en_dis_msg;

typedef struct ipc_ipfast_acc_fasthash_ {
    ipc_ipfast_msg_hdr hdr;
    ulong list_number;
    ipaddrtype address;             /* address to match exactly */
    ulong grant;                    /* TRUE if permit, FALSE if deny */
} ipc_ipfast_acc_fasthash;
 
typedef struct  ipc_ipfast_acc_fastitem_ {
    ipc_ipfast_msg_hdr hdr;
    ulong list_number;
    ipaddrtype permitmask;          /* IP address mask */
    ipaddrtype wildmask;            /* mask of wildcarded address bits */
    ulong grant;                  /* TRUE if permit, FALSE if deny */
} ipc_ipfast_acc_fastitem;
 
typedef struct ipc_ipfast_acc_slowtype_ {
    ipc_ipfast_msg_hdr hdr;
    ulong list_number;
    ipaddrtype source;      /* source IP address */
    ipaddrtype srcmask;     /* mask of wildcarded address bits */
    ipaddrtype destination; /* destination IP address */
    ipaddrtype dstmask;     /* mask of wildcarded address bits */
    ulong hits;             /* number of times ACE has matched */
    ulong si_hits;          /* number of times ACE matched at last clear */
    ulong grant;          /* TRUE if permit, FALSE if deny */
    ulong tos;            /* TRUE if the tos value is set */
    ulong precedence;     /* TRUE if the precedence value is set */
    ulong established;    /* TRUE if filtering on established */
    ulong fancy;          /* TRUE if we need to do more than usual */
    ulong log;            /* TRUE if we need to log this ACE's use */
    ulong protocol;       /* protocol type we are filtering */
    ulong srcopr;         /* operator name */
    ulong sport;          /* [TCP, UDP] source port */
    ulong sport2;        /* [TCP, UDP] upper port in range */
    ulong dstopr;         /* operator name */
    ulong dport;          /* [TCP, UDP] destination port */
                            /* ICMP message type */
    ulong dport2;          /* [TCP, UDP] upper port in range */
    ulong tos_value;       /* TOS value to filter on */
    ulong precedence_value;/* matching precedence value */
} ipc_ipfast_acc_slowtype;

typedef struct ipc_ipfast_cfgreq_msg_ {
    ipc_ipfast_msg_hdr hdr;
    ulong num_indices;
    ulong index_data[0];
} ipc_ipfast_cfgreq_msg;

typedef struct ipc_ipfast_acl_ena_msg_ {
    ipc_ipfast_msg_hdr hdr;
    ulong num_interfaces;
    ulong acl_data[0];
} ipc_ipfast_acl_ena_msg;

typedef struct ipc_ipfast_acl_inout_ {
    ulong hw_if_index;
    ulong ip_out_accesslist;
    ulong ip_in_accesslist;
    ulong in_not_out;
    ulong no_inacls;
} ipc_ipfast_acl_inout;

/*
 * IPFAST message codes.
 */
#define	IPFAST_UPDATE_PREFIX			1
#define	IPFAST_INVALIDATE_PREFIX_IF		2
#define	IPFAST_INVALIDATE_PREFIX_PR		3
#define	IPFAST_INVALIDATE_PREFIX_ALL		4
#define	IPFAST_STATS_UPLOAD			5
#define IPFAST_ENABLE_RC_DIST                   6
#define IPFAST_DISABLE_RC_DIST                  7
#define IPFAST_ACCESS_ADD_FASTHASH		8
#define IPFAST_ACCESS_DEL_FASTHASH		9
#define IPFAST_ACCESS_ADD_FASTITEM		10
#define IPFAST_ACCESS_DEL_FASTITEM		11
#define IPFAST_ACCESS_ADD_SLOWITEM              12
#define IPFAST_ACCESS_DEL_SLOWITEM              13
#define IPFAST_CFG_DFS_REQUEST			14
#define IPFAST_CFG_ACL_REQUEST			15
#define IPFAST_IP_ACL_INTFACE			16
#define IPFAST_CRYPTO_ENGINE_VIP_UPD_IDB        17
#define IPFAST_CRYPTO_ENGINE_VIP_KEY_REQUEST    18
#define IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP        19
#define IPFAST_UPDATE_IDB			20
#define IPFAST_UPDATE_INDEX			21


/*
 * Distributed Switching Port Names.
 */

#define IPFAST_RSP_STATISTICS_NAME	"IPC Master: DFS.statistics"
#define IPFAST_RSP_CONFIG_NAME		"IPC Master: DFS.config"

/*
 * Interrupt level port-names.
 */
#define IPFAST_VIP_SLOT0_CTRL_NAME	"Slot 0: DFS.control.IL"
#define IPFAST_VIP_SLOT1_CTRL_NAME	"Slot 1: DFS.control.IL"
#define IPFAST_VIP_SLOT2_CTRL_NAME	"Slot 2: DFS.control.IL"
#define IPFAST_VIP_SLOT3_CTRL_NAME	"Slot 3: DFS.control.IL"
#define IPFAST_VIP_SLOT4_CTRL_NAME	"Slot 4: DFS.control.IL"
#define IPFAST_VIP_SLOT5_CTRL_NAME	"Slot 5: DFS.control.IL"
#define IPFAST_VIP_SLOT6_CTRL_NAME	"Slot 6: DFS.control.IL"
#define IPFAST_VIP_SLOT7_CTRL_NAME	"Slot 7: DFS.control.IL"
#define IPFAST_VIP_SLOT8_CTRL_NAME	"Slot 8: DFS.control.IL"
#define IPFAST_VIP_SLOT9_CTRL_NAME	"Slot 9: DFS.control.IL"
#define IPFAST_VIP_SLOT10_CTRL_NAME     "Slot 10: DFS.control.IL"
#define IPFAST_VIP_SLOT11_CTRL_NAME     "Slot 11: DFS.control.IL"
#define IPFAST_VIP_SLOT12_CTRL_NAME     "Slot 12: DFS.control.IL"

/*
 * Process level port-names.
 */
#define IPFAST_VIP_SLOT0_CTRL_PNAME     "Slot 0: DFS.control.PL"
#define IPFAST_VIP_SLOT1_CTRL_PNAME     "Slot 1: DFS.control.PL"
#define IPFAST_VIP_SLOT2_CTRL_PNAME     "Slot 2: DFS.control.PL"
#define IPFAST_VIP_SLOT3_CTRL_PNAME     "Slot 3: DFS.control.PL"
#define IPFAST_VIP_SLOT4_CTRL_PNAME     "Slot 4: DFS.control.PL"
#define IPFAST_VIP_SLOT5_CTRL_PNAME     "Slot 5: DFS.control.PL"
#define IPFAST_VIP_SLOT6_CTRL_PNAME     "Slot 6: DFS.control.PL"
#define IPFAST_VIP_SLOT7_CTRL_PNAME     "Slot 7: DFS.control.PL"
#define IPFAST_VIP_SLOT8_CTRL_PNAME     "Slot 8: DFS.control.PL"
#define IPFAST_VIP_SLOT9_CTRL_PNAME     "Slot 9: DFS.control.PL"
#define IPFAST_VIP_SLOT10_CTRL_PNAME    "Slot 10: DFS.control.PL"
#define IPFAST_VIP_SLOT11_CTRL_PNAME    "Slot 11: DFS.control.PL"
#define IPFAST_VIP_SLOT12_CTRL_PNAME    "Slot 12: DFS.control.PL"

#define DFS_MAX_PORT_NAME	35
#ifndef MAX_SLOTS
#define	MAX_SLOTS		13
#endif
static uchar dfs_proc_level_name_table [MAX_SLOTS][DFS_MAX_PORT_NAME] = {
{IPFAST_VIP_SLOT0_CTRL_PNAME},
{IPFAST_VIP_SLOT1_CTRL_PNAME},
{IPFAST_VIP_SLOT2_CTRL_PNAME},
{IPFAST_VIP_SLOT3_CTRL_PNAME},
{IPFAST_VIP_SLOT4_CTRL_PNAME},
{IPFAST_VIP_SLOT5_CTRL_PNAME},
{IPFAST_VIP_SLOT6_CTRL_PNAME},
{IPFAST_VIP_SLOT7_CTRL_PNAME},
{IPFAST_VIP_SLOT8_CTRL_PNAME},
{IPFAST_VIP_SLOT9_CTRL_PNAME},
{IPFAST_VIP_SLOT10_CTRL_PNAME},
{IPFAST_VIP_SLOT11_CTRL_PNAME},
{IPFAST_VIP_SLOT12_CTRL_PNAME}
};

static uchar dfs_int_level_name_table [MAX_SLOTS][DFS_MAX_PORT_NAME]={
{IPFAST_VIP_SLOT0_CTRL_NAME},
{IPFAST_VIP_SLOT1_CTRL_NAME},
{IPFAST_VIP_SLOT2_CTRL_NAME},
{IPFAST_VIP_SLOT3_CTRL_NAME},
{IPFAST_VIP_SLOT4_CTRL_NAME},
{IPFAST_VIP_SLOT5_CTRL_NAME},
{IPFAST_VIP_SLOT6_CTRL_NAME},
{IPFAST_VIP_SLOT7_CTRL_NAME},
{IPFAST_VIP_SLOT8_CTRL_NAME},
{IPFAST_VIP_SLOT9_CTRL_NAME},
{IPFAST_VIP_SLOT10_CTRL_NAME},
{IPFAST_VIP_SLOT11_CTRL_NAME},
{IPFAST_VIP_SLOT12_CTRL_NAME}
};

/*
 * Convert slot number to DFS control port name.
 */
static inline uchar * generate_ds_ctrl_port_name (ushort slot, 
						  boolean int_level_port)
{
    if (slot >= MAX_SLOTS)
	return(NULL);

    if (int_level_port) {
	return &dfs_int_level_name_table[slot][0];
    } else {
	return &dfs_proc_level_name_table[slot][0];
    }
}


