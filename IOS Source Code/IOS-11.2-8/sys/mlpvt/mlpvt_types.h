/* $Id: mlpvt_types.h,v 1.1.42.6 1996/07/25 22:33:29 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_types.h,v $
 *------------------------------------------------------------------
 * mlpvt_types.h -- MLPVT group setup definitions
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_types.h,v $
 * Revision 1.1.42.6  1996/07/25  22:33:29  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 * SGBP may get 2 or more message on a closing tcb. Keep
 * track of free status of closing tcb in mlpvt_process_tcp_message()
 * Do not free an already freed tcb.
 *
 * Revision 1.1.42.5  1996/05/18  22:34:58  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.4  1996/05/07  23:34:43  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.3  1996/05/06  23:41:30  perryl
 * CSCdi56533:  Redundant MLPVT parameter, connect retry interval
 * Branch: California_branch
 *
 * Revision 1.1.42.2  1996/05/06  21:47:08  perryl
 * CSCdi56528:  MLPVT doesnt allow stack member to use more than 1 IP src
 * address.
 * Branch: California_branch
 * Add a secondary address list to the MLPVT member structure.
 * When a hello msg's source IP address different than the
 * configured member IP address, record it in the secondary
 * address list. When TCP pipe open request, it will than
 * able to recognize the address as the member's
 *
 * Revision 1.1.42.1  1996/04/27  06:34:41  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.8  1996/04/22  00:31:19  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.7  1996/04/03  22:58:59  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.6  1996/03/09  07:18:08  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.5  1996/03/02  02:57:25  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.4  1996/03/01  22:58:08  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.3  1996/03/01  21:43:16  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.2  1996/02/08  06:51:44  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:46  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:52  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_TYPES_H__
#define __MLPVT_TYPES_H__

typedef int BIDTYPE;

typedef struct mlpvt_msgblock 
{
    char *buffer;
    ushort needed;
    ushort count;
} mlpvt_msgblock_t;

typedef struct mlpvt_msghdr
{
    uchar class;
    uchar version;
    ushort length;
} mlpvt_msghdr_t;

typedef struct mlpvtgroup {
    char		*name;	        /* Name of mlpvtgroup */
    list_header		*members;       /* List of members for this group */
    boolean		active;         /* Whether mlpvtgroup active */
    int			status;	        /* Mlpvt group status if active */
    BIDTYPE		queryseedbid;   /* Seed value for bidding */
    boolean		seedbidtype;    /* Seed bid type */
    ulong 		reference;      /* local reference */
} mlpvtgroup_t;

/* same size as CHAP_CHALLENGE_SIZE */
#define MLPVT_CHALLENGE_SIZE    16

typedef struct mlpvtmember {
    int retries;                        /* Retries (used in Keepalive etc) */
    sys_timestamp peer_timeout;             /* Timer used in Keepalive */
    char *name;				/* Member name */
    int id;	  			/* Serial number for convenience */
    int ticks;                          /* Timer ticks */
    ipaddrtype addr;			/* Member address */
    list_header *secaddr;               /* Secondary addresses */
    int status;				/* Member configuration status */
    struct mlpvtgroup *mlpvtgrp;	/* Pointer to parent */
    ulong reference;			/* Unique reference of this member */
    int chapid;                         /* chap style challenge id */
    uchar chapval[MLPVT_CHALLENGE_SIZE]; /* chap style challenge */
    mlpvt_msgblock_t *msg;		/* Temp message block */
} mlpvtmember_t;

#define MLPVT_MSGBLOCK_SIZE	sizeof(mlpvt_msgblock_t)
#define MLPVT_MSGHDR_SIZE	sizeof(mlpvt_msghdr_t)
#define MLPVTGROUP_SIZE		sizeof(mlpvtgroup_t)
#define MLPVTMEMBER_SIZE	sizeof(mlpvtmember_t)

/*
 * Message definition for on-the-wire messages.
 */
typedef struct mlpvt_hellomsg
{
    mlpvt_msghdr_t hdr;
    ulong reference;
    int type;
    int chapid;
    ipaddrtype addr;
    int peercnt;
    uchar string[0];
} mlpvt_hellomsg_t;

#define MLPVT_HELLOMSG_SIZE	sizeof(mlpvt_hellomsg_t)

typedef struct mlpvt_keepalivemsg
{
    mlpvt_msghdr_t hdr;
    ipaddrtype addr;
    ulong reference;
    int type;
    uchar groupname[MLPVT_MAX_NAME];
    uchar membername[0];
} mlpvt_keepalive_t;

#define MLPVT_KEEPALIVEMSG_SIZE	sizeof(mlpvt_keepalive_t)

#endif
