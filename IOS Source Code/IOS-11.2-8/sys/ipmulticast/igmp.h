/* $Id: igmp.h,v 3.6.12.5 1996/08/21 01:21:06 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/igmp.h,v $
 *------------------------------------------------------------------
 * igmp.h - Structure and Constant defintions for IGMP.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Structure and Constant defintions for the Internet Group Management
 * Protocol. This protocol supports IP multicast group join/leave actions.
 *------------------------------------------------------------------
 * $Log: igmp.h,v $
 * Revision 3.6.12.5  1996/08/21  01:21:06  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/06/18  01:46:41  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/04  01:08:20  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.6.12.2  1996/04/03  23:19:09  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/03/27  23:29:56  mleelani
 * CSCdi52589:  UI misinformation of Igmp version
 * Branch: California_branch
 * Fix UI version bug.
 *
 * Revision 3.6  1996/02/13  22:32:28  dino
 * CSCdi48373:  When a member leaves a group, (S,G) are not triggered in
 *              Prunes
 *
 * Revision 3.5  1996/01/12  22:56:00  mleelani
 * CSCdi46802:  IGMP needs faster leave response
 * Miscellaneous v2 enhancements.
 *
 * Revision 3.4  1995/11/30  04:42:05  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.3  1995/11/21  23:12:01  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:29:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/03  01:12:59  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * Revision 2.2  1995/07/24  07:33:36  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.1  1995/06/07 21:03:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IGMP_H__
#define __IGMP_H__

/*
 * General constants.
 */
#define IGMP_QUERY_INTERVAL   ONEMIN /* The spec says 125 secs */
#define IGMP_STACK_SIZE       2000
#define IGMP_REPORT_DELAY_MAX 10
#define IGMPHEADERBYTES       8
#define IGMP_PERIODIC_INTVL   ONESEC

/*
 * Timers for Version2.
 */

/*
 * The following are multipliers of the query interval
 * for group membership interval, old router present interval,
 * querier present interval. All these values change if we
 * modify the query interval.
 */
#define IGMP_GROUP_MEM_MULT 	 3
#define IGMP_OLD_ROUTER_MULT 	 6
#define IGMP_QR_PRESENT_MULT     2

/*
 * New timers and defines.
 */
#define IGMP_MAX_QUERY_RESPONSE_INTERVAL 10 * ONESEC
#define IGMP_MAX_QUERY_RESPONSE_VALUE    100 /* 10 secs */
#define IGMP_UNSOLICITED_REPORT_INTERVAL 10 * ONESEC
#define IGMP_LAST_MEMBER_QUERY_INTERVAL  ONESEC
#define IGMP_LAST_MEMBER_QUERY_RESP_VALUE 10
#define IGMP_STARTUP_QUERY_INTERVAL      25 * ONESEC
#define IGMP_LAST_MEMBER_QUERY_COUNT     2
#define IGMP_STARTUP_QUERY_COUNT         2

#define IGMP_GROUP_MEM_INTERVAL   IGMP_GROUP_MEM_MULT * IGMP_QUERY_INTERVAL
#define IGMP_OLD_ROUTER_INTERVAL  IGMP_OLD_ROUTER_MULT * IGMP_QUERY_INTERVAL 
#define IGMP_QR_PRESENT_INTERVAL  IGMP_QR_PRESENT_MULT * IGMP_QUERY_INTERVAL 

/*
 * Since we have to mimick previous versions in case we
 * have old version router/hosts on the network, the following
 * version definition is necessary.
 */
enum igmp_versions {
    IGMP_VERSION1 = 1,
    IGMP_VERSION2 = 2,
    IGMP_VERSION3 = 3
};

/*
 * Timer types
 */
enum {
    IGMP_QUERY,
    IGMP_OTHER_QUERIER,
    IGMP_OLDER_ROUTER,
    CGMP_TIMER
};

/*
 * The Version field for compatibilty reasons.
 */
#define IGMP_VERSION1_OR_2_TYPE      0x10
#define IGMP_VERSION3_TYPE           0x20

/*
 * IGMP block per idb. Allocated only of we have IGMP
 * enabled on the interface. Already typedefed in types.h.
 */
struct igmpidbtype_ {
    ulong         query_interval; 	/* How often Queries are sent */
    acl_headertype *access_group;   	/* what groups can join */
    uchar         version;        	/* The version we are acting as */
    uchar         cfg_version;          /* The version we are configured as */
    boolean       multicast_qr;    	/* Are we the querier or not */
    boolean       enable_leave; 	/* should we process them */
    ipaddrtype    querier_address;      /* IP address of current querier */
    mgd_timer     master_timer;         /* master managed timer */
    mgd_timer     query_timer;    	/* igmp_query_timer */
    mgd_timer     querier_present_timer;/* another querier ? */
    mgd_timer     older_router_timer;   /* old version router ? */
    mgd_timer     cgmp_timer;           /* For sending self CGMP Joins */
    ulong         query_resp_value;     /* time to respond to query */
    ulong         querier_timeout;      /* For querier re-election */
    ulong         wrong_versions;	/* # of queries rcv w/ diff version */
    ulong         group_joins;		/* # of times a group added */
    ulong         group_leaves;		/* # of times a group removed */
    sys_timestamp query_resp_timer;     /* Sent query in last 10 seconds */
    paktype       *cgmp_pak;            /* For CGMP batch joins */
};

/*
 * igmp_getidb
 *
 * Fetch the idb subblock with IGMP and CGMP fields.  Beware: The subblock
 * isn't created until igmp_enable_idb() is called and is destroyed when
 * igmp_disable_idb() succeeds.
 */
static inline igmpidbtype *igmp_getidb (struct idbtype_ const *idb)
{
    return idb_get_swsb(idb, SWIDB_SB_IGMP);
}

/*
 * IGMP fixed header.
 */
typedef struct igmptype_ {
    uchar      type;
    uchar      code;
    ushort     checksum;
    ipaddrtype address;
    char       data[0];
} igmptype;

/*
 * Type definitions. They reflect a combination of type + version
 * fields.
 */
#define IGMP_QUERY_TYPE  	0x11
#define IGMP_REPORT_TYPE 	0x12
#define IGMP_DVMRP_TYPE  	0x13
#define IGMP_PIM_TYPE    	0x14

/*
 * IGMP_TRACE_TYPE is no longer in use.
 */
#define IGMP_TRACE_TYPE  	0x15
#define IGMP_NEW_REPORT_TYPE 	0x16
#define IGMP_LEAVE_TYPE  	0x17
#define IGMP_MTRACE_RESP 	0x1e
#define IGMP_MTRACE_TYPE  	0x1f


/*
 * IGMP group cache. Identifies which multicast groups are directly connected
 * and what interfaces the group members are on.
 */
typedef struct igmp_cachetype_ {
    struct igmp_cachetype_ *next;    /* Next entry in igmp_cache */
    ipaddrtype    group;             /* IP group address */
    idbtype       *idb;              /* Interface group member is present */
    sys_timestamp expire_time;       /* 0 means not running */
    sys_timestamp uptime;            /* How long group has been in cache */
    sys_timestamp report_delay_time; /* When to report a group */
    ulong         flags;             /* See below */
    ipaddrtype    last_reporter;     /* Member that sent last group report */
    sys_timestamp older_host_timer;  /* old version host for this group */
    sys_timestamp last_member_query_time; /* For sending query for the
					     last member */
    uchar         last_member_query_count; /* # of times last member query
					      has been sent */
} igmp_cachetype;

/*
 * igmp_cachetype flag definitions.
 */
#define IGMP_LOCAL_FLAG     0x01
#define IGMP_USER_FLAG      0x02
#define IGMP_STATIC_FLAG    0x04

/*
 * IGMP macro definitions.
 */
#define IGMP_BUGINF if (igmp_debug) buginf
#define DVMRP_ACTIVE(Idb) (TIMER_RUNNING(Idb->dvmrp_nbr_timer))
#define IGMP_LEAVE_ENABLED(igmp) (!TIMER_RUNNING((igmp)->older_host_timer))

/*
 * IGMP traffic statistics
 */
typedef struct igmp_traffic_t_ {
    ulong inputs;
    ulong formaterr;
    ulong checksumerr;
    ulong input_queries;
    ulong input_reports;
    ulong input_leaves;
    ulong input_dvmrp;
    ulong input_pim;
    ulong output_queries;
    ulong output_reports;
    ulong output_leaves;
    ulong output_dvmrp;
    ulong output_pim;
    ulong outputs;
} igmp_traffic_t;
extern igmp_traffic_t igmp_traffic;

/*
 * ---------- External data declarations. ----------
 */
extern igmp_cachetype *igmp_cache[NETHASHLEN];
extern ipparamtype igmprouteparams;

/*
 * ---------- External function declarations. ----------
 */

/*
 * igmp.c
 */
extern void igmp_init(void);
extern void igmp_init_idb(idbtype *);
extern void igmp_input(paktype *);
extern void igmp_send_query(idbtype *, ipaddrtype );
extern void igmp_send_report(igmp_cachetype *);
extern void igmp_receive_query(igmptype *, ipaddrtype, idbtype *);
extern void igmp_receive_report(const igmptype *, ipaddrtype, ipaddrtype, idbtype *);
extern void igmp_process_timers(void);
extern void igmp_commands(parseinfo *);
extern void igmp_join_group_command(ipaddrtype, idbtype *, boolean);
extern igmp_cachetype *igmp_add_group(ipaddrtype, idbtype *, ulong, ulong,
				      ipaddrtype);
extern void igmp_delete_group(ipaddrtype, idbtype *);
extern igmp_cachetype *igmp_find_group(ipaddrtype, idbtype *);
extern void igmp_enable_idb(idbtype *);
extern void igmp_disable_idb(idbtype *);
extern void igmp_show_commands(parseinfo *);
extern void igmp_show_groups(parseinfo *);
extern void igmp_show_interface(parseinfo *);
extern void igmp_display_idb(idbtype *);
extern void igmp_show_traffic(void);
extern void igmp_add_mroute_idb(ipaddrtype, idbtype *, boolean, boolean, 
				ipaddrtype);
extern int igmp_add_one_mroute_idb(rntype *, va_list);
extern void igmp_delete_mroute_idb(ipaddrtype, idbtype *);
extern int igmp_delete_each_mroute_idb(rntype *, va_list);
extern void igmp_clear_cache(ipaddrtype, const idbtype *);
extern void igmp_clear_group_command(parseinfo *);
extern void igmp_send_leave(ipaddrtype group, idbtype *idb);
extern void igmp_check_new_rp(ipaddrtype, const idbtype *);

/*
 * igmp_chain.c
 */
extern void igmp_parser_init(void);

#endif __IGMP_H__


