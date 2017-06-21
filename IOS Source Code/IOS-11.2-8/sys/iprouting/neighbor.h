/* $Id: neighbor.h,v 3.2.60.2 1996/05/04 01:21:52 wilber Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/neighbor.h,v $
 *------------------------------------------------------------------
 * neighbor.h -- Neighbor Definitions
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: neighbor.h,v $
 * Revision 3.2.60.2  1996/05/04  01:21:52  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.2.60.1  1996/04/16  19:01:34  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:35:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/28  21:14:36  rchandra
 * CSCdi39436:  BGP : Allow conditional unsuppression of suppressed routes
 * - add 'neighbor x.x.x.x unsuppress-map <map>' command
 *
 * Revision 2.5  1995/07/06  23:12:27  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.4  1995/06/30  20:25:15  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.3  1995/06/28  03:21:18  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/15 22:06:54  hampton
 * Fix 24.5 day bugs in EGP support.  While there, eliminate
 * direct references to msclock.  [CSCdi35916]
 *
 * Revision 2.1  1995/06/07 21:08:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NEIGHBOR_H__
#define __NEIGHBOR_H__

#define DATLEN 20

typedef struct thirdparty_ {
    struct thirdparty_ *next;	/* pointer to next 3rd paty host */
    ipaddrtype address;		/* address of 3rd party router */
    char flags;			/* internal/external */

    /*
     * Per 3rd party data needed when building updates.
     */
    uchar *buffer;		/* temporary buffer pointer */
    int  bytesleft;		/* bytes remeining in buffer */
    uchar *p;			/* working pointer into buffer */
    uchar *distances;		/* pointer to distances in buffer */
    uchar *netcount;		/* pointer to networks in buffer */
    int  nnets;			/* total nets. Determines if included */
} thirdpartytype;

#define TP_INTERNAL 1
#define TP_EXTERNAL 2


struct neighbortype_ {
    struct neighbortype_ *next;	/* pointer to next neighbor */
    idbtype *idb;		/* interface of last input */
    ipaddrtype address;		/* gateway (neighbor) address */
    char neighbor_name[24];	/* ascii version of address or text tag */
    ipaddrtype myaddr;		/* our local ip address */
    pdbtype *pdb;		/* pointer to protocol block */
    ushort rcvseq;		/* receive sequence number */
    ushort sndseq;		/* send sequence number */
    short hello_int;		/* seconds between Hello retransmissions */
    short poll_int;		/* seconds between Poll retransmissions */
    short reachcount;		/* count of reachability indications */
    short reachshift;		/* the reachability "shift register" */
    short state;		/* current state */
    ushort yoursystem;		/* autonomous system number of our peer */
    ushort mysystem;   		/* a.s. number of our interface */
    uchar dummy2[2];
    ulong flags;		/* random flags */
    sys_timestamp hello_timer;	/* t1 - Request, Hello, Cease rexmit timer */
    sys_timestamp poll_timer;	/* t2 - Poll command retransmit timer */
    sys_timestamp abort_timer;	/* t3 - abort timer */ 
    sys_timestamp retrytime;	/* leave dead entry alone this long */
    sys_timestamp started;	/* when this neighbor was reached */
    int mode;			/* EGP_NAS_ACTIVE or EGP_NAS_PASSIVE */
    thirdpartytype *thirdp;	/* Third party for these hosts */
    int protocol_version;	/* protocol version possibly negotiated */

    /*
     * BGP specific fields
     */
    struct neighbortype_ *peergroup;   	/* ptr to the peer group leader */
    struct neighbortype_ *peerhead;	/* ptr to peers. Used by grp leader */
    struct neighbortype_ *nextpeer;	/* ptr to next peer in our group */
    ushort cluster_id;			/* cluster id of the peer */
    uchar  dummy[2];			/* for allignment */
    acl_headertype     *pfilter;	/* promiscuous filter */
    int	     ebgp_ttl;			/* ttl for ebgp connections */
    idbtype *bgp_update_source;
    struct neighbortype_ *creator_template; /* used by anonynous neighbors */
    int	     open_pid;			/* pid of active open process or -1 */
    tcbtype *tcb;
    tcbtype *secondary_tcb;		/* used only in connection collision */
    tcbtype *bgp_listener;
    boolean bgp_internal;
    int bgp_state;
    acl_headertype *bgp_distlistin;
    acl_headertype *bgp_distlistout;
    unsigned bgp_weight;
    int bgp_inlist;
    int bgp_outlist;
    queuetype bgp_weightdata;
    queuetype bgp_asdata;
    int	bgp_default_state;              /* state of the default to this peer */
    route_map_nametype *bgp_default_map;/* route-map to generate defaults */
    route_map_nametype *bgp_routemapin;
    route_map_nametype *bgp_routemapout;
    route_map_nametype *bgp_unsuppress_map;
    struct bgp_cond_adv_maptype_ *bgp_cond_adv; /* list of conditional
						   advertising maps */
    ulong bgp_probetime;
    ulong bgp_holdtime;
    sys_timestamp bgp_ktimer;
    sys_timestamp bgp_lastread;
    ulong bgp_minadverttime;
    ulong bgp_defminadverttime;
    sys_timestamp bgp_last_upd_sent;
    sys_timestamp bgp_last_upd_check;	/* last check to send update */
    ulong version;			/* table version */
    int bgp_configed_version;		/* protocol version */
    ipaddrtype bgp_router_id;		/* router id */
    queuetype bgp_writeQ;
    queuetype bgp_secondary_writeQ;
    queuetype bgp_readQ;
    uchar *readptr;
    int readcount;
    int readstate;
    int readlimit;
    uchar *bgp_netptr;
    int bgp_netcount;
    bmsgtype *bgp_netpak;
    uchar *bgp_lastnetptr;		/* debugging variables */
    int bgp_lastnetcount;
    bmsgtype *bgp_lastnetpak;		/* UNUSED */
    int bgp_lastreadstate;
    uint bgp_upd_version;		/* version to update to */
    uint bgp_soft_reconfig_version;	/* version to soft reconfigure */
    ipaddrtype bgp_upd_checkpoint;	/* last network attempted to send */
    uchar readdata[DATLEN];
    password_struct bgp_password;
    ulong neighbor_index;	   	/* EBGP neighbor index */
    ulong neighbor_index_mask;   	/* mask to modify neighbor's bit */
    uchar neighbor_index_offset;	/* Offset for this neighbor's bit*/
    uchar bgp_last_error[2];		/* a copy of the last notification */

    /*
     * SNMP required fields. Should be usable by both EGP and BGP.
     */
    ulong inmsgs;			/* input EGP msgs from neighbor */
    ulong inupdates;			/* input BGP updates to neighbor */
    sys_timestamp inupdatetime;		/* clock at time of last update */
    ulong inerrs;			/* input EGP msgs in error */
    ulong outmsgs;			/* output EGP msgs to neighbor */
    ulong outupdates;			/* output BGP updates to neighbor */
    ulong outerrs;			/* output EGP msgs in error */
    ulong errsrcvd;			/* input error msgs from neighbor */
    ulong errssent;			/* output error msgs to neighbor */
    ulong cameup;			/* number of times peer came up */
    ulong wentdn;			/* number of times peer went down */
};

#endif __NEIGHBOR_H__
