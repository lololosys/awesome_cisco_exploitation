/* $Id: mlpvt_util.h,v 1.1.42.6 1996/07/25 22:33:32 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_util.h,v $
 *------------------------------------------------------------------
 * mlpvt_util.h -- MLPVT group setup and maintenance prototypes
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_util.h,v $
 * Revision 1.1.42.6  1996/07/25  22:33:32  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 * SGBP may get 2 or more message on a closing tcb. Keep
 * track of free status of closing tcb in mlpvt_process_tcp_message()
 * Do not free an already freed tcb.
 *
 * Revision 1.1.42.5  1996/05/18  22:35:04  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.4  1996/05/07  23:34:45  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.3  1996/05/06  21:47:13  perryl
 * CSCdi56528:  MLPVT doesnt allow stack member to use more than 1 IP src
 * address.
 * Branch: California_branch
 * Add a secondary address list to the MLPVT member structure.
 * When a hello msg's source IP address different than the
 * configured member IP address, record it in the secondary
 * address list. When TCP pipe open request, it will than
 * able to recognize the address as the member's
 *
 * Revision 1.1.42.2  1996/05/06  18:09:02  syiu
 * CSCdi56520:  Show ppp multilink not showing member links
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/04/27  06:34:47  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.9  1996/04/22  00:31:21  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.8  1996/04/03  22:59:00  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.7  1996/03/09  07:18:09  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.6  1996/03/05  02:47:00  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.5  1996/03/02  02:57:28  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.4  1996/03/01  22:58:11  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.3  1996/02/08  23:47:18  syiu
 * Branch: Dial1_branch
 * Support for CHAP challenge by stackname if locally defined
 *
 * Revision 1.1.2.2  1996/02/08  06:51:45  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:47  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:55  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_UTIL_H__
#define __MLPVT_UTIL_H__

/*
 * Member search routines.
 */
mlpvtmember_t *mlpvt_getmemberbyname(mlpvtgroup_t *, char *);
mlpvtmember_t *mlpvt_getmemberbyref(mlpvtgroup_t *, ulong);
mlpvtmember_t *mlpvt_getmemberbyaddr(mlpvtgroup_t *, ipaddrtype);
mlpvtmember_t *mlpvt_getfirst_member(mlpvtgroup_t *);
mlpvtmember_t *mlpvt_getnext_member(mlpvtgroup_t *, mlpvtmember_t *);
mlpvtmember_t *mlpvt_getnext_memberbyref(mlpvtgroup_t *, ulong);
mlpvtmember_t *mlpvt_getfirst_activemember(mlpvtgroup_t *);
mlpvtmember_t *mlpvt_getnext_activememberbyref(mlpvtgroup_t *, ulong);

/*
 * New functions for MQ module *****************************
 */
static inline ulong
mlpvt_getfirst_memberid (mlpvtgroup_t *grp)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getfirst_member(grp)) ?
	   memb->reference : 0);
}

static inline ulong
mlpvt_getnext_memberid (mlpvtgroup_t *grp, ulong ref)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getnext_memberbyref(grp, ref)) ?
	   memb->reference : 0);
}

static inline ulong
mlpvt_get_memberid_byaddr (mlpvtgroup_t *grp, ipaddrtype addr)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getmemberbyaddr(grp, addr)) ?
	   memb->reference : 0);
}

static inline ipaddrtype
mlpvt_get_peer_addr (mlpvtgroup_t *grp, ulong ref)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getmemberbyref(grp, ref)) ?
           memb->addr : 0);
}

static inline ulong
mlpvt_getfirst_activememberid (mlpvtgroup_t *grp)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getfirst_activemember(grp)) ?
	   memb->reference : 0);
}

static inline ulong
mlpvt_getnext_activememberid (mlpvtgroup_t *grp, ulong ref)
{
    mlpvtmember_t *memb;
    
    return((memb = mlpvt_getnext_activememberbyref(grp, ref)) ?
	   memb->reference : 0);
}

/*
 * Group search routines.
 */
boolean mlpvt_find_stackname (char *, int, ipaddrtype *);
boolean mlpvt_find_membername (char *, int, ipaddrtype);
mlpvtgroup_t *mlpvt_getgroup(void);
mlpvtgroup_t *mlpvt_getgroupbyname(char *);

/*
 * New functions for MQ module *****************************
 */
static inline ulong
mlpvt_get_our_id (mlpvtgroup_t *grp)
{
    return((grp = mlpvt_getgroup()) ?
	   grp->reference : 0);
}

static inline BIDTYPE
mlpvt_get_seed_bid (mlpvtgroup_t *grp)
{
    return((grp = mlpvt_getgroup()) ?
	   grp->queryseedbid : 0);
}

/*
 * Initaliztion routines.
 */
void mlpvt_init_groups(void);
boolean mlpvt_addgroup(mlpvtgroup_t *);
void mlpvt_removegroup(mlpvtgroup_t *);
boolean mlpvt_addmember(mlpvtgroup_t *, mlpvtmember_t *);
void mlpvt_removemember(mlpvtgroup_t *, mlpvtmember_t *);

/*
 * Integration functions
 */
#ifdef POST_INTEGRATION
ulong mlpvt_getfirst_memberid(mlpvtgroup_t *grp);
ulong mlpvt_getnext_memberid(mlpvtgroup_t *grp, ulong peer_id);
ulong mlpvt_get_our_id(mlpvtgroup_t *grp);
ipaddrtype mlpvt_get_peer_addr(mlpvtgroup_t *grp, ulong peer_id);
#endif

ulong mlpvt_getfirst_memberid(mlpvtgroup_t *);
ulong mlpvt_getnext_memberid(mlpvtgroup_t *, ulong);
ulong mlpvt_get_connectedpeer_count(mlpvtgroup_t *);
ipaddrtype mlpvt_get_peer_addr(mlpvtgroup_t *, ulong);

int mlpvt_p2p_msg_output(ulong peer_id, char *buf, int len);
void mlpvt_mq_init(void);
void mlpvt_mq_shutdown(void);
void mlpvt_show_query_states(void);

void mlpvtgroup_member_delete(mlpvtgroup_t *, mlpvtmember_t *);
void mlpvtgroup_delete (mlpvtgroup_t *);

boolean mlpvt_add_secaddr(mlpvtmember_t *, ipaddrtype);
uchar mlpvt_get_bidtype(mlpvtgroup_t *);

#endif
