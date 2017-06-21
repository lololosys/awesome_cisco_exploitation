/* $Id: ospf_pak.h,v 3.2.60.3 1996/05/23 12:06:53 klmj Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_pak.h,v $
 *------------------------------------------------------------------
 * ospf_pak.h  -- ospf packet header definitions
 *
 * 13-Sept-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_pak.h,v $
 * Revision 3.2.60.3  1996/05/23  12:06:53  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/04  01:22:06  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.2.60.1  1996/04/16  19:01:58  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:36:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:09:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* packet data structures
 * definitions.
 */

/* 
 * Link State Advertisement Header
 */
typedef struct link_state_header {
    ushort	lsh_age;	/* This advertisemet age in seconds */
    uchar	lsh_options;	/* options */
    uchar	lsh_type;	/* Type of link state advertisement */
    ulong	lsh_id;		/* Link State Id */
    ulong	lsh_adv_rtr;	/* advertising router id */
    long	lsh_seq;	/* */
    ushort	lsh_chksum;	/* checksum for this advertisement part */
    ushort	lsh_length;	/* length of this advertisement */
    uchar	lsh_data[0];	/* Type-specific data */
} lsatype;

#define LINK_STATE_HEADER_SZ (sizeof(lsatype))

/* 
 * Link state advertisements (lsh_type)
 */
#define LSA_TYPE_STUB		0   
#define LSA_TYPE_RTR		1   
#define LSA_TYPE_NET		2
#define LSA_TYPE_SUM_NET	3
#define LSA_TYPE_SUM_ASB	4
#define LSA_TYPE_ASE		5
#define LSA_TYPE_IGNORE		6
#define LSA_TYPE_7_ASE		7

#define RTR_LS_MAX_COST		0xFFFF
#define SUM_LS_INFINITY		0xFFFFFF
#define INTRA_PATH_COST_MAX	(SUM_LS_INFINITY - 1)

#define SPF_SCHD_RTR		(1 << LSA_TYPE_RTR)
#define SPF_SCHD_NET		(1 << LSA_TYPE_NET)
#define SPF_SCHD_SNET		(1 << LSA_TYPE_SUM_NET)
#define SPF_SCHD_ASB		(1 << LSA_TYPE_SUM_ASB)
#define SPF_SCHD_ASE		(1 << LSA_TYPE_ASE)
#define SPF_SCHD_7_ASE		(1 << LSA_TYPE_7_ASE)

#define SPF_SCHD_BIT(type)	(1 << (type))

#define SPF_SCHD_INTRA		(SPF_SCHD_RTR|SPF_SCHD_NET)
#define SPF_SCHD_INTER		(SPF_SCHD_SNET|SPF_SCHD_ASB)
#define SPF_SCHD_EX		(SPF_SCHD_ASE|SPF_SCHD_7_ASE)

/* 
 * Hello packet related structures 
 */
typedef struct hello_header {
    ulong	h_netmask;	/* net mask */
    ushort	h_interval;	/* seconds between this rtr's Hello packets */
    uchar	h_rtr_options;	/* this router's priority */
    uchar	h_rtr_priority;	/* this router's priority */
    ulong	h_deadint;	/* seconds before declaring this router down */
    ulong	h_deg_rtr;	/* ID of DR for this net */
    ulong	h_bdeg_rtr;	/* ID of the backup DR for this net */
    ulong	h_nbrs;	/* list of neighbours (not part of header) */ 
} hellotype;
	
#define HELLO_HDR_SIZE		20
#define NBR_ID_SZ		4
#define OPTION_T_BIT   0x01
#define OPTION_E_BIT   0x02
#define OPTION_MC_BIT  0x04
#define OPTION_N_P_BIT 0x08
#define OPTION_DC_BIT  0x20

/* macros to access E and T bit in options */
#define GET_E_BIT(options) ((options) & OPTION_E_BIT)
#define GET_T_BIT(options) ((options) & OPTION_T_BIT)
#define GET_TE_BITS(options) ((options) & (OPTION_E_BIT | OPTION_T_BIT))
#define GET_MC_BIT(options) (options & OPTION_MC_BIT)
#define GET_N_P_BIT(options) ((options) & OPTION_N_P_BIT)
#define GET_DC_BIT(options) ((options) & OPTION_DC_BIT)

/*
 * convenient macros to verify DC interface type
 */
#define OSPF_DC_IF_TYPE(ospf_info) \
    (((ospf_info)->if_type == IF_POINT_TO_POINT) || \
     ((ospf_info)->if_type == IF_VIRTUAL_LINK) || \
     ((ospf_info)->if_type == IF_POINT_TO_M_POINT))
 
/* in hello_rtr_pri  the high order four
 * bits are used for options and lower four
 * bits for router priority. we extract them
 * using masks.
 */
#define HELLO_OPTION(hdr_ptr)		(((hdr_ptr->h_rtr_pri) & 0xF0)>>4)
#define HELLO_PRIORITY(hdr_ptr)		((hdr_ptr->h_rtr_pri) & 0x0F)
#define HELLO_NBR_START_PTR(hdr_ptr)	(&(hdr_ptr->h_nbrs))

/*
 * Database Description packet structures 
 */

typedef struct dbdtype {
    ushort	dbd_reserved;	/* reserved for future (fill with zero) */
    uchar	dbd_options;	/* options for this packet */
    uchar	dbd_i_m_ms;	/* higher order 5 bits are set to zeros */
    ulong	dbd_seq;	/* seq num of the database description pkts */
    lsatype	dbd_lsas[0];	/* List of LSA headers */
} dbdtype;

#define DBD_HDR_SIZE (sizeof(dbdtype))

#define I_BIT 	0x04
#define M_BIT 	0x02
#define MS_BIT 	0x01

#define IS_MS_BIT_SET(bits) (bits & MS_BIT)
#define IS_M_BIT_SET(bits)  (bits & M_BIT)
#define IS_I_BIT_SET(bits)  (bits & I_BIT)
#define IS_I_M_MS_SET(bits)  (bits == (I_BIT | M_BIT | MS_BIT))
#define IS_I_MS_OFF(bits)  ((bits & (I_BIT | MS_BIT)) == 0)

/*
 * Link State Request Packet structures
 */ 
typedef struct ls_req {
    ulong	lsr_type;	/* Type of the piece */
    ulong	lsr_id;		/* Link State Id */
    ulong	lsr_adv_rtr;	/* Advertising router */
} ls_req;

#define LS_REQ_SZ (sizeof(ls_req))

/*
 * Link state update packet strucures
 */

typedef struct ls_upd_ {
    ulong	lsu_adv_count;	/* Count of advertisements */
    lsatype	lsu_lsas[0];	/* The advertisements themselves */
} ls_upd;

#define LS_UPD_SZ (sizeof(ls_upd))

typedef struct tos_metric { 
    ushort	tos;		/* type of service; used with mask */
    ushort	metric;		/* cost of using this interface */
} tos_metric;

#define	LINK_TOS_MASK  0x0700
#define LINK_METRIC_SIZE (sizeof(tos_metric))

typedef struct link_info {
	ulong	lnk_id;	/* what it is connected to */
	ulong	lnk_data;	/* interface specific data */
	uchar	lnk_con_type;	/* what the interface connects to */
	uchar	lnk_tos_count;	/* number of tos specified */
	ushort	lnk_tos0_metric;/* metric for TOS 0 */
	tos_metric lnk_tos_metric[0]; /* Other TOS metrics */
} link_info;

/*
 * definitions for ( link_con_type)
 * which indicates type of link interface.
 */
#define	LINK_IF_RTR		1
#define	LINK_IF_TRANS_NET	2
#define	LINK_IF_STUB_NET	3
#define	LINK_IF_VIRTUAL_LINK	4

#define LINK_INFO_SIZE (sizeof(link_info))

#define HOST_NET_MASK		0xFFFFFFFF

 
typedef struct rtr_la_hdr {
	lsatype	la_hdr;
        uchar	la_E_B;
	uchar	la_zero;	/* external or border bits; used with mask */
	ushort	la_lnk_cnt;	/* number of links */
	link_info la_link;
} rtr_lsatype;

#define	AREA_BORDER_B_BIT	 0x1		/* border bit mask */
#define	AS_BORDER_E_BIT		 0x2		/* external bit mask */

#define AS_AREA_BORDER_MASK      (AREA_BORDER_B_BIT | AS_BORDER_E_BIT)

#define RTR_LSA_HDR_SIZE	(LINK_STATE_HEADER_SZ + 4)	

typedef struct net_lsatype{
	lsatype	la_hdr;
        ulong	la_mask;
	ulong	la_nbr;	/* external or border bits; used with mask */
} net_lsatype;

#define NET_LSA_HDR_SIZE	(LINK_STATE_HEADER_SZ + 4)

typedef struct sum_lsatype{
	lsatype	la_hdr;
        ulong	la_mask;
	ulong	la_tos_metric;	/* external or border bits; used with mask */
} sum_lsatype;

#define SUM_LSA_HDR_SIZE	(LINK_STATE_HEADER_SZ + 4)
#define SUM_METRIC(tos_metric)	((tos_metric) & 0x00ffffff)
#define SUM_TOS(tos_metric)	(((tos_metric) & 0xff000000)>>24)

typedef struct as_metric{
    ulong as_e_tos_metric;
    ipaddrtype as_forw_addr;
    ulong as_ex_tag;
} as_metric;

#define AS_METRIC_SZ	12

#define	TAG_AUTOMATIC	0x80000000
#define	TAG_COMPLETE	0x40000000

#define	TAG_PATHLEN_RSV	0x30000000
#define	TAG_PATHLEN_GE1	0x20000000
#define	TAG_PATHLEN_1	0x10000000
#define	TAG_PATHLEN_0	0x00000000

#define	TAG_PATHLENPART 0x30000000
#define	TAG_ARBPART	0x0fff0000
#define TAG_ASPART	0x0000ffff

#define	TAG_ARBSHIFT	16

typedef struct ex_lsatype{
	lsatype	la_hdr;
        ulong	la_mask;
	as_metric la_metric;
} ex_lsatype;

#define ASE_LSA_HDR_SIZE	(LINK_STATE_HEADER_SZ + 4)
#define	ASE_TYPE2_METRIC_BIT		0x80000000

#define ASE_METRIC(tos_metric)	((tos_metric) & 0x00ffffff)
#define  ASE_TOS(tos_metric)	(((tos_metric) & 0x7f000000)>>24)
#define  ASE_METRIC_TYPE(tos_metric)  ((tos_metric) & 0x80000000)

static inline ushort exlsa_to_route_type_inline (ex_lsatype *ex_lsa)
{
    boolean nssa, type2;
    type2 = ASE_METRIC_TYPE(ex_lsa->la_metric.as_e_tos_metric);
    nssa = (ex_lsa->la_hdr.lsh_type == LSA_TYPE_7_ASE);
    if (nssa)
	return (type2 ? RTYPE_NSSA2 : RTYPE_NSSA1);
    else
	return (type2 ? RTYPE_EXTERN2 : RTYPE_EXTERN1);
}
#define EXLSA_TO_ROUTE_TYPE(ex_lsa) exlsa_to_route_type_inline(ex_lsa)

#define EXLSA_TO_METRIC_TYPE(ex_lsa) ((ASE_METRIC_TYPE(  \
					ex_lsa->la_metric.as_e_tos_metric)) ? \
				      METRIC_TYPE2 : METRIC_TYPE1)

#define OSPF_AUTH_LEN 8			/* Byte length of auth data */

/*
 * Type 2 - message digest authentication
 */
typedef struct ospf_auth_type2_ {
    ushort	digest_offset;
    uchar	keyid;
    uchar	digest_len;
    ulong	seq_num;
} ospf_auth_type2;

typedef struct ospf_header {
    uchar	ospf_version;	/* ospf version number 1 or 2 */
    uchar	ospf_type;	/* packet type */
    ushort	ospf_length; 	/* length of entire packet in bytes */
    ulong	ospf_rtr_id;  	/* router ID of this packet source */
    ulong	ospf_area_id;	/* area ID to which this packet belongs*/
    ushort	ospf_checksum;  /* for entire packet except ospf_Auth field */
    ushort	ospf_autype; 	/* authentication scheme used in this area */
    union	{		/* authentication data in this network */
    	uchar	auth[OSPF_AUTH_LEN];
	ospf_auth_type2 auth2;
    } ospf_auth;
    uchar	ospf_data[0];	/* type-specific data */
} ospftype;

#define OSPF_HDR_SIZE (sizeof(ospftype))

/* 
 * ospf packet types (ospf_type)
 */
#define	OSPF_HELLO	1	/* hello to maintain neighbors */
#define	OSPF_DB_DES	2	/* database exchange to bringup adjacency */
#define	OSPF_LS_REQ	3	/* link state request */
#define	OSPF_LS_UPD	4	/* link state update */
#define	OSPF_LS_ACK	5	/* link state ack */













