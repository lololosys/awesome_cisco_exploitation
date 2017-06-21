/* $Id: mlpvt.h,v 1.1.42.7 1996/08/28 13:00:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt.h,v $
 *------------------------------------------------------------------
 * mlpvt.h -- MLPVT common include file
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt.h,v $
 * Revision 1.1.42.7  1996/08/28  13:00:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.6  1996/08/09  21:13:38  perryl
 * CSCdi65641:  SGBP offload server doesnt always win all the bundles
 * Branch: California_branch
 * For all platform, the default seed bid is now 50. The offload seed bid
 * is 50 plus the CPU calibrated value.
 * Remove helpout when bid requester send out a bid of zero.
 *
 * Revision 1.1.42.5  1996/05/23  06:35:52  perryl
 * CSCdi58477:  SGBP should send first Hello with minimal delay
 * Branch: California_branch
 * Send the 1st hello message out as soon as the system comes up.
 * For the 1st 20 seconds, send hello message in 4 seconds
 * intervals. After the initial 20 seconds, if it still can't connect
 * to its peer, send hellos message in 20 seconds interval.
 *
 * Revision 1.1.42.4  1996/05/18  22:34:46  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/07  23:34:35  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.2  1996/05/06  23:41:31  perryl
 * CSCdi56533:  Redundant MLPVT parameter, connect retry interval
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/04/27  06:34:21  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.8  1996/04/22  00:31:16  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.7  1996/04/03  22:58:53  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.6  1996/03/09  07:18:05  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.5  1996/03/05  02:46:54  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.4  1996/03/02  02:57:19  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.3  1996/03/01  22:58:03  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.2  1996/02/08  06:51:41  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:38  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:33  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_H__
#define __MLPVT_H__

#define MLPVT_MAX_NAME 40

#include "../mlpvt/mlpvt_types.h"
#include "../mlpvt/mlpvt_util.h"
#include "../mlpvt/mlpvt_registry.h"
#include "../mlpvt/mlpvt_debug.h"

/*
 * MLPVT subsystem header
 */
#define MLPVT_MAJVERSION 1
#define MLPVT_MINVERSION 0
#define MLPVT_EDITVERSION 0

void mlpvt_process_shutdown(void);
void mlpvt_process_init(void);

ulong mlpvt_getpackage_state(void);
void mlpvt_setpackage_state(ulong);

void mlpvt_hello_startup(void);
void mlpvt_hello_shutdown(void);

#define MLPVT_MASTERSHIP_CLAIM		10000
#define MLPVT_MASTERSHIP_DECLINE	0

enum mlpvtgroup_config {
    MLPVTGROUP_NONE = 0,
    MLPVTGROUP_NAME,
    MLPVTGROUP_MEMBER,
    MLPVTGROUP_QUERYBID_DEFAULT,
    MLPVTGROUP_QUERYBID_OFFLOAD,
    MLPVTGROUP_QUERYBID_NUM,
    MLPVTGROUP_QUERYBID_FORWARD_ONLY,
    MLPVTGROUP_PPP_FWD,
    MLPVTGROUP_SOURCE_IP
};

#define MLPVTGROUP_FAST_CRETRY_INTERVAL 5
#define MLPVTGROUP_FAST_CRETRY          (1 * ONESEC)
#define MLPVTGROUP_DEFAULT_CRETRY	(2 * ONESEC)
#define MLPVTGROUP_SEED_BID		50
#define MLPVTGROUP_CALIBRATE_BID        calibrate_bid

#define MLPVTGROUP_AUTH_NONE	0
#define MLPVTGROUP_AUTH_PAP	1
#define MLPVTGROUP_AUTH_CHAP	2

/*
 * States
 */
#define MLPVTGROUP_STATUS_CREATED	0
#define MLPVTGROUP_STATUS_ACTIVE	1

#define MLPVTGROUP_MEMBER_ILLEGAL	0
#define MLPVTGROUP_MEMBER_IDLE		1
#define MLPVTGROUP_MEMBER_WAIT_INFO	2
#define MLPVTGROUP_MEMBER_ACTIVE	3
#define MLPVTGROUP_MEMBER_AUTHOK        4
  
#define SHOW_MLPVT			1

#define MLPVT_NOTINIT		0
#define MLPVT_IDLE		1
#define MLPVT_ACTIVE		2

/*
 * Class numbers.
 */
#define MLPVT_ILLEGAL_CLASS	0
#define MLPVT_HELLO_CLASS	1
#define MLPVT_MQ_CLASS		3
#define MLPVT_KEEPALIVE_CLASS   4
#define MLPVT_MAX_CLASS		5


/*
 * Hello message type
 */

#define MLPVT_HELLO_CHALLENGE         1
#define MLPVT_HELLO_CHALLENGE_RESP    2
#define MLPVT_HELLO_AUTHOK            3
#define MLPVT_HELLO_INFO              4

/* 
 * Keepalive Message type
 */
#define MLPVT_KEEPALIVE_TICKLE 1
#define MLPVT_KEEPALIVE_ACK 2


/*
 * Max number of challenge ID
 */
#define MLPVT_MAX_CHAPID        256

/*
 * Version Fields for all protocols.  The format is
 * version_major:4 bits
 * version_minor:4 bits
 */

#define MLPVT_VERSION	0x20

#define MLPVT_MQ_SELECT		((MLPVT_MQ_CLASS << 8) | MLPVT_VERSION)

#define MLPVT_PEER_VANISH		1
#define MLPVT_NEW_PEER			2

/*
 * Externs
 */
extern BIDTYPE calibrate_bid;
extern boolean sgbp_forward_straight_ppp;

/*
 * Seed bid types
 */
#define MLPVT_SEEDBID_DEFAULT   1
#define MLPVT_SEEDBID_OFFLOAD   2
#define MLPVT_SEEDBID_NUM       3
#define MLPVT_SEEDBID_FORWARD_ONLY 4

/* 
 * Retries
 */
#define MLPVT_AUTHOK_RETRIES  3
#define MLPVT_WAITINFO_RETRIES  3

/*
 * Hello Challenge timer ticks
 */
#define MLPVT_HELLO_TICKS 10

#endif
