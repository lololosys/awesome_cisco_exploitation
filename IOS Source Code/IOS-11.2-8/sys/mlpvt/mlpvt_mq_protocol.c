/* $Id: mlpvt_mq_protocol.c,v 1.1.42.14 1996/09/07 02:07:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_mq_protocol.c,v $
 *------------------------------------------------------------------
 * mlpvt_mq_protocol.c -- MLPVT mastership query protocol
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_mq_protocol.c,v $
 * Revision 1.1.42.14  1996/09/07  02:07:55  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 1.1.42.13  1996/08/28  13:00:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.12  1996/08/09  21:13:46  perryl
 * CSCdi65641:  SGBP offload server doesnt always win all the bundles
 * Branch: California_branch
 * For all platform, the default seed bid is now 50. The offload seed bid
 * is 50 plus the CPU calibrated value.
 * Remove helpout when bid requester send out a bid of zero.
 *
 * Revision 1.1.42.11  1996/06/03  18:46:30  syiu
 * CSCdi59325:  SGBP messages should be debugged flagged
 * Branch: California_branch
 *
 * Revision 1.1.42.10  1996/06/03  18:02:47  syiu
 * CSCdi59320:  SGBP has too many errmsg facdefs
 * Branch: California_branch
 *
 * Revision 1.1.42.9  1996/05/24  23:49:33  perryl
 * CSCdi57351:  MLPVT default seed bid may be different after each reboot
 * Branch: California_branch
 *
 * Revision 1.1.42.8  1996/05/22  03:41:51  perryl
 * CSCdi58388:  SGBP is missing an debug SGBP error option
 * Branch: California_branch
 *
 * Revision 1.1.42.7  1996/05/18  22:34:54  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.6  1996/05/07  23:34:40  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/05/07  19:13:29  dblair
 * CSCdi56726:  c1000: %SYS-3-SUPNONE: Registry 26 doesnt exist
 * mlpvt is not in all images that contain multilink.  remove
 * mlpvt_event_occured registry and use mlp_appoint_bundle instead.
 * Branch: California_branch
 *
 * Revision 1.1.42.4  1996/05/06  21:49:55  perryl
 * CSCdi56532:  MLPVT does not properly break tie on equal high bids
 * Branch: California_branch
 * Randomize the tie breaker
 *
 * Revision 1.1.42.3  1996/05/01  14:28:02  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Remove unused include of ppp_public.h.
 *
 * Revision 1.1.42.2  1996/04/29  09:40:17  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:32  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.13  1996/04/26  01:26:57  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.1.2.12  1996/04/22  00:31:18  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.11  1996/04/03  22:58:57  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.10  1996/03/09  00:47:58  abaindur
 * Branch: Dial1_branch
 *  Some state m/c fixes, cleanup, and documentation.
 *
 * Revision 1.1.2.9  1996/03/05  02:46:58  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.8  1996/03/02  02:57:22  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.7  1996/03/01  22:58:06  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.6  1996/03/01  21:43:12  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.5  1996/02/08  06:51:42  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.4  1996/02/07  20:23:36  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 1.1.2.3  1996/02/05  16:56:51  abaindur
 * Branch: Dial1_branch
 *  Plug a memory leak. Fix a corner case in the query state m/c.
 *  Remove unneeded simulation code.
 *
 * Revision 1.1.2.2  1996/01/31  18:35:24  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:42  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:42  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Some helpful hints (ie., read me first) :
 *
 * MQ  == Mastership Query (mastership of a Multi Link PPP bundle)
 * PB  == Peer Bid  (bids for mastership are sent to peers querying
 *                   for a master. May the best bid win)
 * pb  == lowercase for PB
 * P2P == Peer to Peer (as in communication)
 * p2p == lowercase for P2P
 *
 * Multi Link PPP virtual termination is a feature that allows multi
 * link PPP calls from a user to be terminated on different systems
 * that form a collaborative group called an MLPVT group. One of the
 * systems in this group takes mastership for managing the MLP bundle
 * when the first call from that user is received. 
 *
 * This protocol provides a mechanism for querying an existing master
 * within the MLPVT group for a user. If a master does not exist, the
 * protocol provides for resolving a master through a bidding process. 
 *
 * A master for a particular user may not necessarily be the system
 * receiving the first call from that user. In other words, a system
 * may bid for and take mastership for a user even if it is not
 * handling or has not received any call from that user.
 *
 * Bidding is for a user, not an individual connection. Once a system
 * has bid for and won mastership for a user, all subsequent MLP calls 
 * from that user will be handled by the master irrespective of which
 * system in the group those calls are received on. Once all calls
 * from the user are disconnected, the system owning the MLP bundle 
 * relinquishes mastership and a new master will get resolved when a
 * fresh call comes in from that user.
 *
 * Bidding for a user starts with the first call received from that
 * user. Once a master is selected, all subsequent bid requests can be
 * viewed as queries for a master. Group members not aware of who the
 * master is will keep on bidding.
 *
 * Bid values will have a range. The highest value in that range will
 * designate a special bid - a mastership claim. No system will bid
 * this value for a user unless it is a master for that user. Since
 * there can only be one master for a user, there can be atmost one
 * system that can bid this value. 
 *
 * Group members may boot up or go down while bidding is in
 * progress. Bid requests originated by any system must not assume
 * caching by other group members of any information related to any
 * previous bids.
 *
 * Once a system has replied to a bid request with a bid value that is
 * lower than the solicitor's value, it has declined mastership and
 * bowed out of the bidding process for that user. If a second bid
 * request is received from another system for the same user before
 * the first one has been resolved, the second bid value should not be
 * higher than the first one. This is to prevent an inconsistent state
 * within the group. Therefore, the bid value will be cached for a 
 * short while -- until the master is resolved. It also implies that
 * resolution of mastership needs to be conveyed to all members who
 * bid, irrespective of whether they bowed out or not from the bidding. 
 *
 * Two or more members may receive calls simultaneously from the same
 * caller. Hence they may start querying peers simultaneously. If a member
 * sees a peer querying for the same bundle, both will continue querying
 * all their peers. However, only one of the two (the one with the higher
 * bid) will decide who the master is. This member will be considered to
 * be "active". The other member will go into a "passive" mode, waiting
 * to hear who the master is or waiting to take over master selection in
 * case the "active" member goes down.
 *
 * If two members open queries to each other simultaneously with the same
 * bid value, the tie-breaker to decide who will go into the "active" mode
 * will be their reference ID that's based on their burnt-in MAC address.
 *
 *
 * Mastership query states : Overall states of a query
 * -----------------------
 * MQ_STATE_QUERY_TO_PEERS   : VPN/PPP has queried for mastership of
 *   a bundle. Since MLP does not already own this bundle, an active
 *   query to all peers is in progress.
 *
 * MQ_STATE_QUERY_DONE       : A query, either initiated locally by
 *   VPN/PPP, or by a peer, has been resolved. In the former case, we
 *   or a peer may be the master. In the latter case, we have been
 *   accepted as the master.
 *
 * MQ_STATE_QUERY_ABORTED    : A locally initiated query for a bundle
 *   master has been cancelled. Queries initiated from peers for that
 *   bundle may still be active.
 *
 * MQ_STATE_QUERY_FROM_PEERS : One or more queries for mastership of
 *   a bundle have been received from peers. There is no locally
 *   initiated query for that bundle.
 *
 *
 * Peer bidding states : States of a query with each peer
 * -------------------
 * PB_STATE_CREATED              : A state has been freshly created.
 *   No messages have been sent out to the peer. Transitionary state.
 *
 * PB_STATE_BID_OPENED_TO_PEER   : A query cum bidding session has
 *   been opened to the peer.
 *
 * PB_STATE_BID_OPENED_FROM_PEER : The peer has opened the query cum
 *   bidding session (a query has come in from the peer).
 *
 * PB_STATE_BID_SENT             : A bid for mastership has been sent
 *   in response to a query open from the peer.
 *
 * PB_STATE_BID_RCVD             : A bid has been received in response
 *   to a query open.
 *
 * PB_STATE_BID_XCHGD_PASSIVE    : A query open has been received in
 *   response to our open and the bid value in the peer's open exceeds
 *   our bid. We wait for a resolution from the peer, unless the peer
 *   aborts his query.
 *   Note : The query opens can cross if two or more members receive
 *   calls simultaneously from the same caller.
 *
 * PB_STATE_BID_XCHGD_ACTIVE     : A query open has been received in
 *   response to our open and our bid value exceeds the bid value in the
 *   peer's open. We are the active member in this bidding session and
 *   have to inform the peer of the outcome, unless we abort our query,
 *   in which case, the peer takes over.
 *
 * PB_STATE_BID_CLOSED		 : Query has been resolved or aborted; bid
 *   session with peer has been closed.
 *
 * PB_STATE_DELETED		 : Peer state has been logically deleted.
 *
 *
 * Events :
 * ------
 * 1. Locally initiated query from VPN/PPP for a bundle
 * 2. Cancellation of a locally initiated query from VPN/PPP
 * 3. Peer joining the MLPVT group
 * 4. Peer leaving the MLPVT group
 * 5. Messages (MQO/MB/BA/MQC/MQA) from a peer (see below)
 * 6. MLPVT feature shutdown
 *
 *
 * Message format :
 * --------------
 * All fields are in the TLV (Type-Length-Value) format.
 * +--------------------+
 * | MQ message header	|  MQ message ID and message length
 * +--------------------+
 * |    Message type	|  Type of MQ message (MQO/MB/MQC/MQA/BA)
 * +--------------------+
 * |      Sender ID	|  Unique ID of message sender
 * +--------------------+
 * |	Bundle name	|  Name of bundle (user) 
 * +--------------------+
 * | Sender's query ref#|  Serial number generated by sender for each query
 * +--------------------+
 * |   Peer's ref#	|  Peer's query serial number (if seen)
 * +--------------------+
 * | Bid rcvd from peer	|  (where applicable)
 * +--------------------+
 * | Sender's bid value	|  
 * +--------------------+
 * |    Master ID	|  Unique ID of master when closing query (MQC)
 * +--------------------+
 *
 * Message types :
 * -------------
 * MQO : Mastership query open. Starts a query for the specified bundle.
 *	 Sender opens query with own bid. 
 *
 * MB  : Mastership bid. A bid value in response to an MQO. A bid can lie
 *	 between 0 and a mastership-claim value. The mastership claim is
 *	 sent only if the member is already a master for the specified
 *	 bundle.
 *
 * BA  : Bid accept. Sent by the group member who initiated the query to
 *	 the peer whose bid was accepted because it was the best bid.
 *	 Sent even in response to a mastership claim. The receiving
 *	 member treats the BA as a query closure cum ACK for the mastership
 *	 claim.
 *       Note : More than one member may initiate a query simultaneously
 *	 for the same bundle. In this case, the member whose bid is higher
 *	 takes responsibility for selecting the master and sending the BA.
 *
 * MQC : Query close. Sent when a query has been resolved, ie., a master
 *	 found. Sent to every member except any member to whom a BA has
 *	 been sent.
 *	 Note : This is the only message that contains the 'Master ID'
 *	 field.
 *
 * MQA : Query abort. Sent to any peer with whom a query has been opened.
 *
 *
 * MQ state transitions :
 * --------------------
 * NONE
 * ->	MQ_STATE_QUERY_FROM_PEERS : MQO received from a peer.
 * ->   MQ_STATE_QUERY_TO_PEERS   : Local query from VPN/PPP
 *
 * MQ_STATE_QUERY_FROM_PEERS
 * ->	MQ_STATE_QUERY_TO_PEERS   : Local query from VPN/PPP
 * ->   MQ_STATE_QUERY_DONE	  : BA received from peer, or,
 *				    MQC received from peer specifying us
 *				    as master
 * ->   NONE			  : MQA received from last peer querying us;
 *				    MQC received from last peer querying us
 *				    and we are not the master
 *
 * MQ_STATE_QUERY_TO_PEERS
 * ->	MQ_STATE_QUERY_DONE	  : Query has been resolved (master found)
 * ->	MQ_STATE_QUERY_ABORTED	  : Local query from VPN/PPP aborted
 *
 * MQ_STATE_QUERY_ABORTED
 * ->	MQ_STATE_QUERY_FROM_PEERS : Query from one or more peers still
 *				    active
 * ->	MQ_STATE_QUERY_TO_PEERS   : Fresh local query from VPN/PPP
 * ->	NONE			  : All peer query states closed and deleted
 *
 * MQ_STATE_QUERY_DONE
 * ->	MQ_STATE_QUERY_ABORTED	  : Peer vanish event received for peer
 *				    just selected as master
 * ->	NONE			  : All peer query states closed and deleted
 *
 *
 * PB state transitions :
 * --------------------
 * NONE
 * ->	PB_STATE_CREATED	      : Transitionary state
 *
 * PB_STATE_CREATED
 * ->	PB_STATE_BID_OPENED_TO_PEER   : MQO sent to peer
 * ->	PB_STATE_BID_OPENED_FROM_PEER : MQO received from peer
 * ->	PB_STATE_DELETED	      : Query resolved or aborted, or,
 *					peer loss event received
 *
 * PB_STATE_BID_OPENED_TO_PEER
 * ->	PB_STATE_BID_RCVD	      : MB received
 * ->	PB_STATE_BID_XCHGD_ACTIVE     : MQO received with a lower bid
 * ->	PB_STATE_BID_XCHGD_PASSIVE    : MQO received with a higher bid
 * ->	PB_STATE_DELETED	      : Peer loss event received, or,
 *				        query resolved or aborted
 * ->	PB_STATE_BID_CLOSED	      : MQA sent on query abort
 *
 * PB_STATE_BID_OPENED_FROM_PEER
 * ->	PB_STATE_BID_SENT	      : MB sent in response to MQO
 * ->	PB_STATE_BID_XCHGD_ACTIVE     : MQO sent with a higher bid
 * ->	PB_STATE_BID_XCHGD_PASSIVE    : MQO sent with a lower bid
 * ->	PB_STATE_DELETED	      : Peer loss event received
 * ->	PB_STATE_BID_CLOSED	      : MQA/MQC received from peer; no
 *					local query active for same bundle
 * ->	PB_STATE_CREATED	      : MQA received from peer but local
 *					query active for same bundle
 *
 * PB_STATE_BID_SENT
 * ->	PB_STATE_BID_CLOSED	      : BA/MQC received from peer, or,
 *					MQA received from peer and no local
 *					query active for bundle
 * ->	PB_STATE_CREATED	      : MQA received from peer but local
 *					query active for same bundle
 * ->	PB_STATE_DELETED	      : Peer loss event received, or,
 *					retries for sending MB exceeded
 *
 * PB_STATE_BID_RCVD
 * ->	PB_STATE_BID_CLOSED	      : BA/MQC/MQA sent
 * ->	PB_STATE_DELETED	      : Peer loss event received, or,
 *					query resolved or aborted
 *
 * PB_STATE_BID_XCHGD_ACTIVE
 * ->	PB_STATE_BID_SENT	      : MQA sent on query abort
 * ->	PB_STATE_BID_RCVD	      : MQA received from peer
 * ->	PB_STATE_BID_CLOSED	      : BA/MQC sent to peer
 * ->	PB_STATE_DELETED	      : Peer loss event received
 *
 * PB_STATE_BID_XCHGD_PASSIVE
 * ->	PB_STATE_BID_SENT	      : MQA sent on query abort
 * ->	PB_STATE_BID_RCVD	      : MQA received from peer
 * ->	PB_STATE_BID_CLOSED	      : BA/MQC received from peer
 * ->	PB_STATE_DELETED	      : Peer loss event received
 *
 * PB_STATE_BID_CLOSED
 * ->	PB_STATE_DELETED	      : Transitionary, prior to deletion
 * ->	PB_STATE_BID_OPENED_FROM_PEER : MQO received from peer
 *
 * PB_STATE_DELETED
 * ->	NONE			      : Poof. Peer state gone
 * ->	PB_STATE_CREATED	      : Peer connect event received and a
 *				        local query is in progress
 * ->	PB_STATE_BID_OPENED_FROM_PEER : MQO received from peer
 *
 *
 * A simple example :
 * ----------------
 * 	System-A				System-B
 * ->VPN/PPP query
 *	BID_OPENED_TO_PEER 	 --MQO-->	BID_OPENED_FROM_PEER
 *	BID_RCVD		 <--MB---	BID_SENT
 *   Compare bids		 ---BA-->	BID_CLOSED
 * <-Dispose query
 *	BID_CLOSED				
 */


#include "master.h"
#include "globs.h"
#include "sys_registry.h"
#include "../wan/ppp_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "logger.h"
#include "file.h"
#include "sched.h"
#include "login.h"
#include "../os/chunk.h"
#include "../os/timer_generic.h"
#include "../vpn/vpn.h"
#include "../mlpvt/mlpvt.h"
#include "../mlpvt/mlpvt_mq_protocol.h"
#include "../mlpvt/mlpvt_util.h"
#include "../wan/dialer.h"
#include "../wan/ppp.h"
#include "../wan/mlp.h"
#include <ciscolib.h>
#include "../util/random.h"
#include "msg_mlpvt.c"

BIDTYPE calibrate_bid;

/*
 * Queue for mastership query requests from VPN
 */
static queuetype MQ_reqQ;
static ushort	 Q_ref = 0;

/*
 * Chunkify(tm) some stuff - PB states, MQ states
 */
static chunk_type *pb_state_chunk  = NULL;
static chunk_type *mq_state_chunk  = NULL;

/*
 * Stuff related to our query protocol state machine process
 */
static watched_boolean *MQ_process_activate;
static sys_timestamp MQ_process_timer;
static pid_t MQ_pid = NO_PROCESS;

/*
 **************************Local defines *************************
 */
#define ADD_TO_MQ_MSG(field, ptr)  \
                      bcopy((char *)&field, ptr, sizeof(field)); \
                      ptr        += sizeof(field);

#define MLPVT_PEER_ADDR(peer)     mlpvt_get_peer_addr(NULL, peer)
#define MLPVT_OUR_ADDR
#define MLPVT_PEER_ID(addr)	  mlpvt_get_memberid_byaddr(NULL, addr)
#define MLPVT_OUR_ID              mlpvt_get_our_id(NULL)
#define MLPVT_FIRST_ACTIVE_PEERID mlpvt_getfirst_activememberid(NULL)
#define MLPVT_NEXT_ACTIVE_PEERID(peer) \
                                  mlpvt_getnext_activememberid(NULL, peer)
#define MLPVT_PEER_COUNT          mlpvt_get_connectedpeer_count(NULL)
#define MLPVT_SEED_BID		  mlpvt_get_seed_bid(NULL)
#define MLPVT_BIDTYPE    	  mlpvt_get_bidtype(NULL)

#ifdef MQ_SIMULATOR

#define VPNTYPE			vpntype_test
#define MLP_SEARCH_BUNDLE(b) 	mlp_search_bundle_test(b,MLPVT_MASTERSHIP_CLAIM)
#define VPN_SET_HANDLE(h,a,s)	vpn_put_ipaddress_test(h,a,s)

static pid_t sim_pid = NO_PROCESS;

int mlp_search_bundle_test(char *, BIDTYPE);
void mlp_appoint_bundle_test(char *);
void vpn_put_ipaddress_test(VPNTYPE *, ipaddrtype, int);

#else

#define VPNTYPE			vpntype
#define MLP_SEARCH_BUNDLE(b) 	reg_invoke_mlp_search_bundle(b,MLPVT_MASTERSHIP_CLAIM)
#define VPN_SET_HANDLE(h,a,s)	reg_invoke_vpn_put_ipaddress(h,a,s,0)

#endif

/*
 *=========================== Start ================================
 */
static inline void mlpvt_mq_process_wakeup (void)
{
    /*
     * Signal occurrence of event to our MQ process
     */
    process_set_boolean(MQ_process_activate, TRUE);
}

static ushort mlpvt_query_next_ref (ushort qref)
{
    return (++qref & MLPVT_REF_MASK);
}

static void mlpvt_master_print (MQ_state_t *mq, char *str)
{
    if (mq->master == MLPVT_NO_MASTER)
	strcpy(str, "no one");
    else if (mq->master == MLPVT_WE_ARE_MASTER)
	strcpy(str, "local");
    else
	sprintf(str, "%i", mq->master_addr);
}

static char *mlpvt_msgtype_print (int type)
{
    switch (type) {

    case MLPVT_P2P_MQO:
	return ("Open");
	
    case MLPVT_P2P_MB:
	return ("Bid");
	
    case MLPVT_P2P_BA:
	return ("Accept");
	
    case MLPVT_P2P_MQC:
	return ("Close");
	
    case MLPVT_P2P_MQA:
	return ("Abort");

    case MLPVT_P2P_MRR:
	return ("Resend_Req");

    default:
	return ("Unknown");
    }
}

static char *mlpvt_mqstate_print (int state)
{
    switch (state) {

    case MQ_STATE_UNKNOWN:
	return ("Unknown");
	
    case MQ_STATE_QUERY_TO_PEERS:
	return ("Query_to_peers");
	
    case MQ_STATE_QUERY_DONE:
	return ("Done");
	
    case MQ_STATE_QUERY_ABORTED:
	return ("Aborted");
	
    case MQ_STATE_QUERY_FROM_PEERS:
	return ("Query_from_peers");

    default:
	return ("Invalid");
    }
}

static char *mlpvt_pbstate_print (int state)
{
    switch (state) {

    case PB_STATE_CREATED:
	return ("Created");
	
    case PB_STATE_BID_OPENED_TO_PEER:
	return ("Open_to_peer");
	
    case PB_STATE_BID_OPENED_FROM_PEER:
	return ("Open_from_peer");
	
    case PB_STATE_BID_SENT:
	return ("Sent");
	
    case PB_STATE_BID_RCVD:
	return ("Rcvd");

    case PB_STATE_BID_XCHGD_PASSIVE:
	return ("Passive_xchg");
	
    case PB_STATE_BID_XCHGD_ACTIVE:
	return ("Active_xchg");
	
    case PB_STATE_BID_CLOSED:
	return ("Closed");

    case PB_STATE_DELETED:
	return ("Deleted");

    default:
	return ("Invalid");
    }
}

static char *mlpvt_pbsubstate_print (int state)
{
    switch (state) {

    case PB_SUBSTATE_NONE:
	return ("None");
	
    case PB_SUBSTATE_WE_WON:
	return ("Won");
	
    case PB_SUBSTATE_PEER_WON:
	return ("Lost");
	
    default:
	return ("Invalid");
    }
}

/*
 *================= "Show mlpvt queries" support ====================
 */
static void mlpvt_show_query_MQ_state (MQ_state_t *mq)
{
    char master[16];

    master[0] = '\0';
    mlpvt_master_print(mq, master);
    printf("\nQRef: %d  Bundle: %s  State: %s",
	   mq->ref, mq->bundle, mlpvt_mqstate_print(mq->state));
    printf("\n    Handles: %d  OurBid: %03d  Master: %s  MBid: %03d",
	   mq->handle_count, mq->our_bid, master, mq->master_bid);
}

static void mlpvt_show_query_PB_state (PB_state_t *pb)
{
    printf("\n  Peer: %i  QRef: %d  State: %s/%s  Bid: %03d  Retry: %d",
	   pb->addr, pb->ref, mlpvt_pbstate_print(pb->state),
	   mlpvt_pbsubstate_print(pb->sub_state),
	   pb->peer_bid, pb->retries);
}

void mlpvt_show_query_states (void)
{
    MQ_state_t *mq;
    PB_state_t *pb;

    /*
     * For each MQ state, display all the PB states
     * attached to it.
     */
    if (MQ_reqQ.qhead == NULL) {
	printf("\nNo query states found");
	flush();
	return;
    }

    automore_enable(NULL);
    for (mq = MQ_reqQ.qhead; mq; mq = mq->next) {

	mlpvt_show_query_MQ_state(mq);
	
	for (pb = mq->PB_states; pb; pb = pb->next) {

	    mlpvt_show_query_PB_state(pb);
	}
	printc('\n');
    }
    flush();
    automore_disable();
}

static void mlpvt_show_debug_query_states(void)
{
    MQ_state_t *mq;
    PB_state_t *pb;

    if (MQ_reqQ.qhead == NULL) {
	return;
    }

    for (mq = MQ_reqQ.qhead; mq; mq = mq->next) {
	errmsg(&msgsym(MQB, SGBP),  
	       mq->bundle, mlpvt_mqstate_print(mq->state),
	       mq->our_bid);
	
	for (pb = mq->PB_states; pb; pb = pb->next) {

	    errmsg(&msgsym(PB, SGBP), 
		   pb->addr, mlpvt_pbstate_print(pb->state),
		   pb->peer_bid, pb->retries); 
	}
    }
}
/*
 *====================== VPN query handles =========================
 */
/*
 * Find cached query handle in our list
 */
static int mlpvt_mq_handle_find (MQ_state_t *mq, void *handle)
{
    int i;

    if ((mq == NULL) || (handle == NULL))
	return (MLPVT_MAX_HANDLES); /* NULL handle */
	
    for (i = 0; i < mq->handle_count; i++) {
	if (mq->handles[i] == handle)
	    return (i);
    }
    return (MLPVT_MAX_HANDLES); /* Couldn't find handle */
}

/*
 * Add a query handle to our list
 */
static boolean mlpvt_mq_handle_add (MQ_state_t *mq, void *handle)
{
    if ((mq == NULL) || (handle == NULL))
	return (FALSE);
    
    /*
     * For now, use an array for caching the handles.
     * Visit later for a different storage mechanism.
     */
    if (mq->handle_count < MLPVT_MAX_HANDLES) {
	mq->handles[mq->handle_count++] = handle;
	return (TRUE);
    }
    return (FALSE);
}

/*
 * Remove a specific query handle from our list and
 * compress the list.
 * Done when a query is cancelled.
 */
static boolean mlpvt_mq_handle_delete (MQ_state_t *mq, int indx)
{
    int i;

    if (mq == NULL)
	return (FALSE);
    
    if ((mq->handle_count <= 0) || (indx >= mq->handle_count)) /* Error */
	return (FALSE);

    if ((indx < 0) || (indx >= MLPVT_MAX_HANDLES)) /* Error */
	return (FALSE);

    /*
     * Decrement count, then purge deleted handle from array.
     */
    mq->handle_count--;
    for (i = indx; i <= mq->handle_count; i++) {
	mq->handles[i] = mq->handles[i + 1];
    }

    return (TRUE);
}

/*
 * Dispose of all handles, for a specific query, from our list.
 * This is done when a query master is resolved or a query aborted.
 */
static void mlpvt_mq_all_handles_dispose (MQ_state_t *mq, int status)
{
    int i;
    VPNTYPE *handle;
    char master[16];
    
    if (mlpvt_debug_queries) {
	mlpvt_master_print(mq, master);
	errmsg(&msgsym(DONE, SGBP), mq->ref, mq->bundle,
	       mq->handle_count, master);
    }
    
    for (i = 0; i < mq->handle_count; i++) {
	handle = (VPNTYPE *)mq->handles[i];
	if (status == VPN_FORWARD) {
            VPN_SET_HANDLE(handle, mq->master_addr, status); 
	} else
            VPN_SET_HANDLE(handle, 0, status);

    }
    mq->handle_count = 0;
}

/*
 *========================== Peer bid states ===========================
 */
/*
 * Return TRUE if Peer Bidding states still left.
 * An MQ state can only be deleted when all PB states
 * under it have been deleted.
 */
static inline boolean mlpvt_pb_states_none (MQ_state_t *mq)
{
    return (mq->PB_states == NULL);
}

/*
 * Locate a PB state for a specific query with a specified peer
 */
static PB_state_t *mlpvt_pb_state_find (MQ_state_t *mq,
					  int peer_id)
{
    PB_state_t *pb;

    if (mq == NULL) /* Bad boy */
	return (NULL);

    pb = mq->PB_states;
    while (pb) {
	if (pb->peer_id == peer_id)
	    return (pb);
	pb = pb->next;
    }

    return (pb);
}

/*
 * Delete a PB state for a specific query with a specified peer
 */
static boolean mlpvt_pb_state_delete (PB_state_t *pb)
{
    MQ_state_t *mq;
    
    if (pb == NULL)   /* Bad boy */
	return (FALSE);

    if (pb->next != NULL) {
	pb->next->prev = pb->prev;
    }
    if (pb->prev != NULL) {
	pb->prev->next = pb->next;
    }
    else { /* first element */

	mq = pb->parent;
	if (mq)
	    mq->PB_states = pb->next;
    }
    chunk_free(pb_state_chunk, pb);

    return (TRUE);
}

/*
 * Add a PB state for a specific query for a specified
 * peer to our list
 */
static boolean mlpvt_pb_state_add (PB_state_t *pb)
{
    MQ_state_t *mq;
    
    if ((pb == NULL) || (pb->parent == NULL))
	return (FALSE);

    mq = pb->parent;
    pb->prev = NULL;
    pb->next = mq->PB_states;
    if (pb->next != NULL)
	pb->next->prev = pb;

    mq->PB_states = pb;

    return (TRUE);
}

/*
 * Reactivate a logically deleted peer state for a new query.
 */
static void mlpvt_pb_state_reactivate (PB_state_t *pb)
{
    if (pb) {

	pb->state     = PB_STATE_CREATED;
	pb->sub_state = PB_SUBSTATE_NONE;
	pb->retries   = 0;
	TIMER_STOP(pb->bid_timer);
    }
}

/*
 * Create a new PB state to represent the bidding with a
 * specified peer
 */
static PB_state_t *mlpvt_pb_state_create (MQ_state_t *mq,
					  int peer_id)
{
    PB_state_t *pb;
    
    pb = chunk_malloc(pb_state_chunk);
    if (!pb)
	return (NULL);

    pb->parent    = mq;
    pb->state     = PB_STATE_CREATED;
    pb->sub_state = PB_SUBSTATE_NONE;
    pb->peer_id   = peer_id;
    pb->peer_bid  = 0;
    pb->retries   = 0;
    pb->ref       = 0;
    TIMER_STOP(pb->bid_timer);
    pb->addr      = MLPVT_PEER_ADDR(peer_id);
    
    if (!mlpvt_pb_state_add(pb)) {
	chunk_free (pb_state_chunk, pb);
	return (NULL);
    }

    return (pb);
}

/*
 *========================= Query states ============================
 */
/*
 * Locate our record of a mastership query request for a bundle.
 * This record represents the state of the query.
 * This query may have been initiated internally or by a peer.
 */
static MQ_state_t *mlpvt_query_state_find (char *bundle)
{
    MQ_state_t *mq;

    if ((bundle == NULL) || (strlen(bundle) == 0))
	return (NULL);
    
    mq = MQ_reqQ.qhead;
    while (mq) {
	if (strcmp(bundle, mq->bundle) == 0)
	    break;
	mq = mq->next;
    }
    return (mq);
}

/*
 * Create a record for a mastership query request for a bundle.
 */
static MQ_state_t *mlpvt_query_state_create (char *bundle, BIDTYPE our_bid)
{
    MQ_state_t *mq;
    
    if ((bundle == NULL) || (strlen(bundle) == 0))
	return (NULL);
    
    /*
     * Create MQ_state_t structure and initialize it.
     * Add structure to MQ_reqQ.
     */
    mq = chunk_malloc(mq_state_chunk);
    if (!mq)
	return (NULL);

    mq->bundle = malloc(strlen(bundle) + 1);
    if (!mq->bundle) {
	chunk_free(mq_state_chunk, mq);
	return (NULL);
    }

    strcpy(mq->bundle, bundle);
    mq->our_id       = MLPVT_OUR_ID;
    Q_ref            = mlpvt_query_next_ref(Q_ref);
    mq->ref	     = Q_ref;
    mq->state        = MQ_STATE_UNKNOWN;
    mq->PB_states    = NULL;
    mq->handle_count = 0;
    mq->our_bid      = our_bid;
    mq->master       = MLPVT_NO_MASTER;
    mq->master_id    = 0;
    mq->master_bid   = 0;
    mq->master_addr  = 0;
    TIMER_STOP(mq->start);
    mq->elapsed      = 0;
    
    enqueue(&MQ_reqQ, mq);

    mlpvt_mq_process_wakeup();
    
    return (mq);
}

/*
 * Delete our record of a mastership query request for a bundle.
 */
static void mlpvt_query_state_delete (MQ_state_t *mq)
{
    if (mq) {

	if (!mlpvt_pb_states_none(mq))   /* PB states present */
	    return;                      /* Return without deleting */
	
	unqueue(&MQ_reqQ, mq); /* Dequeue query from queue */

	if (mq->bundle)
	    free(mq->bundle);
	chunk_free(mq_state_chunk, mq);
    }
}

/*
 *======================= VPN & MLP interface =======================
 */

/*
 * Validate a mastership query request received from another
 * internal module.
 */
static boolean mlpvt_query_validate (char *bundle,
				     VPNTYPE *handle)
{
    if (MQ_pid == NO_PROCESS) {		/* MQ not active */
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALIDB, SGBP), "Request", bundle,
		   "MQ process not active");
	handle->status = VPN_NOT_ACTIVE;
	return (FALSE);
    }
    
    if (handle == NULL) {		/* Bad evil */
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALIDB, SGBP), "Request", bundle,
		   "NULL handle");
	return (FALSE);
    }
    
    if (bundle == NULL) {		/* More bad evil */
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALIDB, SGBP), "Request", "",
		   "NULL bundle");
	handle->status = VPN_ERROR;
	return (FALSE);
    }

    if (strlen(bundle) == 0) {		/* Yet more bad evil */
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALIDB, SGBP), "Request", bundle,
		   "No bundle name");
	handle->status = VPN_ERROR;
	return (FALSE);
    }

    if (handle->status != VPN_PENDING) { /* That %$#@! does it */
	/* Someone's screwing around. Ignore request */
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALIDB, SGBP), "Request", bundle,
		   "Query not new/pending");
	return (FALSE);
    }

    return (TRUE);			/* Yippee */
}

/*
 * Duplicate internal query (from VPN) for a bundle
 */
static boolean mlpvt_query_lookup_duplicate (MQ_state_t *mq,
					     VPNTYPE *handle)
{
    if (mlpvt_debug_queries)
	errmsg(&msgsym(DUPL, SGBP), mq->ref, mq->bundle,
	       mq->handle_count+1, mq->our_bid);
    
    switch (mq->state) {

    default:
    case MQ_STATE_QUERY_ABORTED:
	/*
	 * Reactivate query. Fall down ...
	 */
    case MQ_STATE_QUERY_FROM_PEERS:
	/*
	 * Add request to Q and bump up count.
	 * Change query state. Return TRUE.
	 */
	if (!mlpvt_mq_handle_add(mq, handle)) {
	    handle->status = VPN_ERROR;
	    return (FALSE);
	}

        mq->state = MQ_STATE_QUERY_TO_PEERS;
        mlpvt_mq_process_wakeup();
        break;
   
    case MQ_STATE_QUERY_TO_PEERS:
	/*
	 * Add request to Q and bump up count.
	 * Return TRUE.
	 */
	if (!mlpvt_mq_handle_add(mq, handle)) {
	    handle->status = VPN_ERROR;
	    return (FALSE);
	}
        break;
    
    case MQ_STATE_QUERY_DONE:
	/*
	 * Master known. Return TRUE with appropriate
	 * status and info in handle.
	 */
	if (mq->master == MLPVT_PEER_IS_MASTER) {
            VPN_SET_HANDLE(handle, mq->master_addr, VPN_FORWARD);
	}
        else {
            VPN_SET_HANDLE(handle, 0, VPN_DONT_FORWARD);
	}
        break;
    }
    return (TRUE);  /* Success */
}

/*
 * Sparkling new query for a bundle from an internal
 * module (VPN).
 */
static boolean mlpvt_query_lookup_new (char *bundle,
				       VPNTYPE *handle)
{
    BIDTYPE our_bid;
    MQ_state_t *mq;
    fsm *f;
    lcp_options *ho, *go;

    /*
     * If SGBP has set PPP forward, it means that both PPP and Multilink PPP
     * are to be forwarded (if possible). Otherwise (as below), only 
     * Multilink is to be forwarded. Straight PPP should not be forwarded.
     */
    if (!sgbp_forward_straight_ppp) {

       f = handle->link_hwidb->lcpfsm;
       ho = f->heroptions;
       go = f->gotoptions;

       /*
        * If this is not Multilink, do not continue
        */
       if (!(ho->neg_mlp_mrru && go->neg_mlp_mrru)) {
	 handle->status = VPN_DONT_FORWARD;
	 return (TRUE);
       }
    }

    /*
     * We don't know about this bundle.
     * Ask MLP if it does. If MLP does, return back with
     * info in the handle that says mastership is local.
     * If MLP does not, initiate query with peers. Create
     * our data structures to represent the state of the query.
     */
    our_bid = (BIDTYPE)MLP_SEARCH_BUNDLE(bundle);
    if (our_bid == MLPVT_MASTERSHIP_CLAIM) { 

	handle->status = VPN_DONT_FORWARD; /* Ta ta */
	return (TRUE);
    }
    /*
     * MLP has returned the count of the number of bundles it's
     * holding. Use this as our load and reduce it from our seed
     * bid. Zero is the lowest our bid can go to.
     */
    our_bid  = MLPVT_SEED_BID - our_bid;
    if (our_bid < 0)
	our_bid = 0;

    if (mlpvt_debug_queries)
	errmsg(&msgsym(NEWL, SGBP), mlpvt_query_next_ref(Q_ref),
	       bundle, 1, our_bid);
    
    /*
     * Bid not a mastership claim.
     * Initiate query with peers to find or resolve a master.
     */
    mq = mlpvt_query_state_create(bundle, our_bid);

    if (mq == NULL) {  /* Malloc failure ? */
	handle->status = VPN_ERROR;
	return (FALSE);
    }

    mq->state = MQ_STATE_QUERY_TO_PEERS;
    mlpvt_mq_handle_add(mq, handle);

    return (TRUE); /* Success */
}

/*
 * API routine for VPN to query mastership of a bundle.
 */
static boolean mlpvt_query_lookup (char *bundle, void *query)
{
    MQ_state_t *mq;
    VPNTYPE *handle = query;

    if (!mlpvt_query_validate(bundle, handle))
	return (FALSE);
    
    /*
     * Validations done. Check our database to see if any
     * earlier query for same bundle name is in progress.
     */
    mq = mlpvt_query_state_find(bundle);
    if (mq) /* Earlier query present */
	return (mlpvt_query_lookup_duplicate(mq, handle));
	
    /*
     * New query.
     */
    return (mlpvt_query_lookup_new(bundle, handle));
}

/*
 * API routine for VPN to cancel an earlier mastership
 * query request for a bundle.
 */
static void mlpvt_query_cancel (char *bundle, void *query)
{
    MQ_state_t *mq;
    int handle_indx;
    VPNTYPE *handle = query;

    if (!mlpvt_query_validate(bundle, handle)) {
	handle->status = VPN_CANCEL;
	return;
    }
    
    mq = mlpvt_query_state_find(bundle);
    if (mq) { /* Earlier query present */

	/*
	 * Locate this particular request by comparing handles.
	 */
	handle_indx = mlpvt_mq_handle_find(mq, handle);
	if (handle_indx < MLPVT_MAX_HANDLES) {
	    /*
	     * Found handle for specific request.
	     * Remove handle from our list and decrement count.
	     */
	    mlpvt_mq_handle_delete(mq, handle_indx);
	    
	    /*
	     * If no handles left, mark state as aborted,
	     * else leave state alone and let the mastership
	     * query proceed.
	     * 
	     */
	    if (mq->handle_count == 0) { /* Abort */
		mq->state = MQ_STATE_QUERY_ABORTED;
		mlpvt_mq_process_wakeup();
	    }

	    if (mlpvt_debug_queries)
		errmsg(&msgsym(CANCEL, SGBP), mq->ref,
		       handle_indx+1, bundle);
	} 
	else {
	    if (mlpvt_debug_errors)
		errmsg(&msgsym(INVALID, SGBP),
		       "Cancel", bundle, " -- Handle unknown");
	}
    }
    else {
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALID, SGBP),
		   "Cancel", bundle, " -- Query unknown");
    }

    /*
     * Now whatever be the case, cancel away.
     */
    handle->status = VPN_CANCEL; 
}

/*
 *================== MQ peer interface starts here ====================
 */
static void mlpvt_mq_make_us_master (MQ_state_t *mq)
{

    mlpvtgroup_t *grp = mlpvt_getgroup();

    mq->master          = MLPVT_WE_ARE_MASTER;
    mq->master_id       = mq->our_id;
    mq->master_bid      = mq->our_bid;
    mq->our_bid         = MLPVT_MASTERSHIP_CLAIM;
    mq->state           = MQ_STATE_QUERY_DONE;
    mq->elapsed         = ELAPSED_TIME(mq->start);

    /*
     * If configure as forward only, then tell VPN to hangup
     * the incoming call.
     */
    if (grp && (grp->seedbidtype == MLPVT_SEEDBID_FORWARD_ONLY)) {
	if (mlpvt_debug_errors || mlpvt_debug_events ||
	    mlpvt_debug_queries)
	    errmsg(&msgsym(HANGUP, SGBP), mq->bundle);
	mlpvt_mq_all_handles_dispose(mq, VPN_HANGUP);
	return;
    }
    /*
     * Inform MLP. Dispose of any handles hanging around.
     * Delete PB state for peer.
     */
    reg_invoke_mlp_appoint_bundle(mq->bundle);
    mlpvt_mq_all_handles_dispose(mq, VPN_DONT_FORWARD);
}

static void mlpvt_mq_make_peer_master (MQ_state_t *mq)
{

    mq->master    = MLPVT_PEER_IS_MASTER;
    mq->state     = MQ_STATE_QUERY_DONE;
    mq->elapsed   = ELAPSED_TIME(mq->start);
    mlpvt_mq_all_handles_dispose(mq, VPN_FORWARD);
}

/*
 * Functions to handle loss or entry of peers from/to
 * our MLPVT group.
 */
/*
 * Process to handle notification of a peer leaving the group
 */
static void mlpvt_mq_peer_vanish (ulong peer_id)
{
    MQ_state_t *mq, *tmpmq;
    PB_state_t *pb;
    
    /*
     * If peer going down is the master and we are in the DONE
     * state, flag all queries we are holding onto as failed and
     * purge the handles from our list.
     * If not, delete the PB state.
     */
    tmpmq = NULL;
    for (mq = MQ_reqQ.qhead; mq; mq = tmpmq) {

	/*
	 * Grab ahold of the next guy now. We may disappear.
	 */
	tmpmq = mq->next; 
	
	pb = mlpvt_pb_state_find(mq, peer_id);
	if (pb != NULL) /* Found */
	    pb->state = PB_STATE_DELETED;
	
	switch (mq->state) {

	case MQ_STATE_QUERY_TO_PEERS :
	    /*
	     * If there are no more peers connected and we
	     * lost the last one, become master.
	     */
	    if ((MLPVT_PEER_COUNT) == 0) {
		mlpvt_mq_make_us_master(mq);
	    }
	    mlpvt_mq_process_wakeup();
	    break;

	case MQ_STATE_QUERY_DONE :
	    if ((mq->master == MLPVT_PEER_IS_MASTER) &&
		(mq->master_id == peer_id)) {
		/*
		 * Master for this bundle has gone down.
		 * Treat query as aborted so we start querying
		 * afresh rather than forward to a peer that's
		 * gone down.
		 */
		mq->master     = MLPVT_NO_MASTER;
		mq->master_id  = 0;
		mq->state      = MQ_STATE_QUERY_ABORTED;
	    }

	    break;

	default :
	    break;
        }
    } /* For-loop thru MQ's */
}

/*
 * Process a notification of a peer joining the group
 */
static void mlpvt_mq_peer_connect (ulong peer_id)
{
    MQ_state_t *mq;
    PB_state_t *pb;
    
    /*
     * For all MQ's in the QUERY_TO_PEERS state, confirm there's
     * no PB_state_t struct for this peer, and then create
     * a struct and open bidding with the peer.
     */
    for (mq = MQ_reqQ.qhead; mq; mq = mq->next) {

	pb = mlpvt_pb_state_find(mq, peer_id);
	if (pb == NULL) { 

	    if (mq->state == MQ_STATE_QUERY_TO_PEERS) {
		pb = mlpvt_pb_state_create(mq, peer_id);
		mlpvt_mq_process_wakeup();
	    }
	}
	else {
	    switch (pb->state) {

	    case PB_STATE_DELETED:
		
		if (mq->state == MQ_STATE_QUERY_TO_PEERS)
		    mlpvt_pb_state_reactivate(pb); /* Reactivate */
		break;

	    default:
		/*
		 * Uh oh. Found and not deleted.
		 * How can that happen ? What do we do ? AB:
		 */
		if (mlpvt_debug_errors)
		    errmsg(&msgsym(PEERERROR, SGBP), pb->addr,
			   "connect", mlpvt_pbstate_print(pb->state),
			   mq->bundle);
		break;
	    }
	}
    }  /* For-loop thru MQ's */
}

/*
 * Process listening for peer events -- a peer joining or
 * leaving the group
 */
static void mlpvt_mq_peer_event_listener (int event, ipaddrtype addr)
{
    ulong peer_id;

    /*
     * If peer connection is lost, MLPVT module should not
     * blow away mlpvtmember_t structure, else we won't get
     * the peer id. Also if a member is deconfigured.
     */
    peer_id = MLPVT_PEER_ID(addr);
    
    if (peer_id > 0) {			/* Valid peer */

	if (event == MLPVT_PEER_VANISH) {
	    mlpvt_mq_peer_vanish(peer_id);
	}
	else if (event == MLPVT_NEW_PEER) {
	    mlpvt_mq_peer_connect(peer_id);
	}
	else {
	    if (mlpvt_debug_errors)
		errmsg(&msgsym(UNKNOWNEVENT, SGBP), event, addr);
	}
    }
    else {
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(UNKNOWNPEER, SGBP), event, addr);
    }
}

/*
 *========================= P2P messages ============================
 */
/*
 ********************* Messages from peers **************************
 */
/*
 * Find specified field in an incoming MQ message.
 * Return NULL if not found, starting point in the
 * message if found.
 */
static char *mlpvt_mq_msg_field_find (char *msg,
				      int len,
				      ushort type)
{
    int i;
    msgfield_t tlv;

    /*
     * Check for upper limit on len ?
     */
    for (i = 0; i < len; ) {
	bcopy(msg, (char *)&tlv, sizeof(msgfield_t));

	if (tlv.type == type)
	    return (msg);

	msg += tlv.len;
	i   += tlv.len;
    }
    return (NULL);
}

/*
 * Parse message from peer and extract specified field.
 * Return FALSE on any failure.
 */
static char *mlpvt_mq_msg_field_get (char *msg, int len,
				     void *fptr, int flen, ushort ftype)
{
    msgfield_t tlv;
    char *str;
    int vlen;
    
    if ((msg == NULL) || (fptr == NULL))
	return (NULL);

    if ((msg = mlpvt_mq_msg_field_find(msg, len, ftype)) != NULL) {

	bcopy(msg, (char *)&tlv, sizeof(msgfield_t));
	msg += sizeof(msgfield_t);
	vlen = tlv.len - sizeof(msgfield_t); /* Get value length */

	if (flen == 0) { /* string field of unknown length */
	    
	    str = malloc(vlen + 1); /* Don't forget that NULL */
	    if (str) {
		bcopy(msg, str, vlen);
		*(str + vlen)   = 0;
		*(char **)fptr  = str;
	    }
	    else return (NULL);
	}
	else { /* numeric field of specified length */

	    if (flen == vlen) { /* do the two lengths match ? */

		bcopy(msg, (char *)fptr, vlen);
	    }
	    else {
		if (mlpvt_debug_errors)
		    errmsg(&msgsym(INVFIELD, SGBP), "MQ message",
			   tlv.type, "length", vlen, flen);
		return (NULL);		/* Mismatch in field lengths - error */
	    }
	}

	msg += vlen;
    }
    return (msg);
}

/*
 * Process a mastership bid received from a peer.
 */
static boolean mlpvt_mq_peer_bid_process (PB_state_t *pb, BIDTYPE peer_bid)
{
    MQ_state_t *mq;
    
    TIMER_STOP(pb->bid_timer);
    pb->retries  = 0;
    
    pb->peer_bid = peer_bid;
    mq           = pb->parent;
    
    if (mq->our_bid > peer_bid) {
	/*
	 * We bid higher
	 */
	pb->sub_state = PB_SUBSTATE_WE_WON;
    }
    else if (mq->our_bid < peer_bid) {
	/*
	 * We lost
	 */
	pb->sub_state = PB_SUBSTATE_PEER_WON;
    }
    else { /* compare ID's */

	if (mq->our_id > pb->peer_id) {
	    /*
	     * Our ID is higher
	     */
	    pb->sub_state = PB_SUBSTATE_WE_WON;
	}
	else
	    pb->sub_state = PB_SUBSTATE_PEER_WON;
    }
    return (TRUE);
}

/*
 * Process a query open from a peer
 */
static void mlpvt_mq_p2p_msg_MQO_process (PB_state_t *pb,
					  BIDTYPE peer_bid)
{
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(RCVD, SGBP), mlpvt_msgtype_print(MLPVT_P2P_MQO),
	       pb->addr, pb->parent->ref, pb->ref,
	       pb->parent->bundle, peer_bid);

    if (mlpvt_debug_queries) {
	MQ_state_t *mq;
	mq = pb->parent;
	errmsg(&msgsym(NEWP, SGBP), pb->ref, mq->bundle,
	       mq->handle_count+1, peer_bid, mq->our_bid);
    }
    
    switch (pb->state) {

    case PB_STATE_CREATED:
    case PB_STATE_BID_OPENED_FROM_PEER: /* Must be a retry */
	pb->peer_bid = peer_bid;
	pb->state    = PB_STATE_BID_OPENED_FROM_PEER;
	mlpvt_mq_process_wakeup();
	break;
			
    case PB_STATE_BID_CLOSED:
    case PB_STATE_DELETED:
	mlpvt_pb_state_reactivate(pb);
	pb->peer_bid = peer_bid;
	pb->state    = PB_STATE_BID_OPENED_FROM_PEER;
	mlpvt_mq_process_wakeup();
	break;
	
    case PB_STATE_BID_OPENED_TO_PEER:
	mlpvt_mq_peer_bid_process(pb, peer_bid);
	/*
	 * If MQO's crossed and we lost, go into
	 * passive state with this peer.
	 */
	if (pb->sub_state == PB_SUBSTATE_PEER_WON)
	    pb->state     = PB_STATE_BID_XCHGD_PASSIVE;
	else
	    pb->state     = PB_STATE_BID_XCHGD_ACTIVE;

	mlpvt_mq_process_wakeup();
	break;

    case PB_STATE_BID_SENT:
	break;  /* Ignore. Must be a retry */
	
    default:
	if (mlpvt_debug_errors) { 
	    errmsg(&msgsym(UNEXP, SGBP),
		   mlpvt_msgtype_print(MLPVT_P2P_MQO),
		   pb->addr, pb->ref, pb->parent->bundle,
		   peer_bid, mlpvt_pbstate_print(pb->state));
	}
	break;	
    }
}

/*
 * Process a mastership bid from a peer
 */
static void mlpvt_mq_p2p_msg_MB_process (PB_state_t *pb,
					 BIDTYPE peer_bid)
{
    /*
     * We've received a bid.
     * If an open PB state does not exist, discard.
     */
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(RCVD, SGBP), mlpvt_msgtype_print(MLPVT_P2P_MB),
	       pb->addr, pb->parent->ref, pb->ref,
	       pb->parent->bundle, peer_bid);

    if ((pb->state == PB_STATE_BID_OPENED_TO_PEER) ||
	(pb->state == PB_STATE_BID_RCVD)) {
	mlpvt_mq_peer_bid_process(pb, peer_bid);
	pb->state  = PB_STATE_BID_RCVD;
	mlpvt_mq_process_wakeup();
    }
    else {
	if (mlpvt_debug_errors) {
	    errmsg(&msgsym(UNEXP, SGBP),
		   mlpvt_msgtype_print(MLPVT_P2P_MB),
		   pb->addr, pb->ref, pb->parent->bundle,
		   peer_bid, mlpvt_pbstate_print(pb->state));
	}
    }
}

/*
 * Process a bid accept in response to our bid
 */
static void mlpvt_mq_p2p_msg_BA_process (PB_state_t *pb)
{
    MQ_state_t *mq;
    
    /*
     * We've received a bid accept.
     * If a PB state in SENT state does not exist, discard.
     */
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(RCVD, SGBP), mlpvt_msgtype_print(MLPVT_P2P_BA),
	       pb->addr, pb->parent->ref, pb->ref,
	       pb->parent->bundle, pb->parent->our_bid);

    if ((pb->state == PB_STATE_BID_SENT) ||
	(pb->state == PB_STATE_BID_XCHGD_PASSIVE)) {

	mq = pb->parent;	     /* save what we need */

	if (mq->state != MQ_STATE_QUERY_DONE) {
	    mlpvt_mq_make_us_master(mq); /* do the good stuff */
	}
	pb->state = PB_STATE_BID_CLOSED;
	return;
    }
	
    if (mlpvt_debug_errors) {
	errmsg(&msgsym(UNEXP, SGBP),
	       mlpvt_msgtype_print(MLPVT_P2P_BA),
	       pb->addr, pb->ref, pb->parent->bundle,
	       pb->parent->our_bid,
	       mlpvt_pbstate_print(pb->state));
    }
}

/*
 * Process a query close.
 */
static void mlpvt_mq_p2p_msg_MQC_process (PB_state_t *pb)
{
    MQ_state_t *mq;
    
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(RCVD, SGBP), mlpvt_msgtype_print(MLPVT_P2P_MQC),
	       pb->addr, pb->parent->ref, pb->ref,
	       pb->parent->bundle, pb->peer_bid);

    mq    = pb->parent;
    
    switch (pb->state) {

    case PB_STATE_BID_OPENED_FROM_PEER:
	/*
	 * Peer closed before we could respond.
	 * Close it now and see what's to be done later based
	 * on the MQ state.
	 */
	pb->state = PB_STATE_BID_CLOSED;
	break;

    case PB_STATE_BID_SENT:
    case PB_STATE_BID_XCHGD_PASSIVE:

	if (mq->master_id == 0) {
	    /*
	     * Err. Bad. Protocol violation.
	     */
	    if (mlpvt_debug_errors) {
		errmsg(&msgsym(UNEXP, SGBP),
		       "close with no master ID",
		       pb->addr, mq->ref, mq->bundle, mq->our_bid,
		       mlpvt_pbstate_print(pb->state));
	    }
	}
	else {
	    if (mq->master_id == mq->our_id) {
		/*
		 * We are the master
		 */
		mlpvt_mq_make_us_master(mq);
	    }
	    else {
		
		if (mq->state == MQ_STATE_QUERY_TO_PEERS) {
		    /*
		     * We are waiting to know who the master is.
		     * It's some peer.
		     */
		    mq->master_addr = MLPVT_PEER_ADDR(mq->master_id);
		    mlpvt_mq_make_peer_master(mq);
		}
	    }
	}

	pb->state = PB_STATE_BID_CLOSED;
	break;
		
    default :
	if (mlpvt_debug_errors) {
	    errmsg(&msgsym(UNEXP, SGBP),
		   mlpvt_msgtype_print(MLPVT_P2P_MQC),
		   pb->addr, pb->ref, pb->parent->bundle,
		   pb->peer_bid,
		   mlpvt_pbstate_print(pb->state));
	}
	break;
    }

    mlpvt_mq_process_wakeup();
}

/*
 * Process a query abort from a peer
 */
static void mlpvt_mq_p2p_msg_MQA_process (PB_state_t *pb)
{

    /*
     * We've received a bid abort.
     * If a PB state in the SENT state does not exist,
     * discard.
     */
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(RCVD, SGBP), mlpvt_msgtype_print(MLPVT_P2P_MQA),
	       pb->addr, pb->parent->ref, pb->ref,
	       pb->parent->bundle, pb->peer_bid);

    switch (pb->state) {

    case PB_STATE_BID_OPENED_FROM_PEER:
    case PB_STATE_BID_SENT:
	
	if (pb->parent->state == MQ_STATE_QUERY_TO_PEERS) {
	    /*
	     * We are waiting to know who the master is.
	     * Peer aborted his query. If we are also
	     * querying, reset state so that an MQO will be sent
	     * to reopen the query.
	     */
	    mlpvt_pb_state_reactivate(pb);
	    mlpvt_mq_process_wakeup();
	}
	else
	    pb->state = PB_STATE_BID_CLOSED; /* Ta ta */
		
	break;

    case PB_STATE_BID_XCHGD_PASSIVE:
    case PB_STATE_BID_XCHGD_ACTIVE:
	/*
	 * Other side aborted, so our open prevails.
	 * Convert peer's MQO to an MB. Don't delete anything.
	 */
	pb->state = PB_STATE_BID_RCVD;
	mlpvt_mq_process_wakeup();
	break;
	
    default :
	if (mlpvt_debug_errors) {
	    errmsg(&msgsym(UNEXP, SGBP),
		   mlpvt_msgtype_print(MLPVT_P2P_MQA),
		   pb->addr, pb->parent->ref, pb->ref,
		   pb->parent->bundle, pb->peer_bid,
		   mlpvt_pbstate_print(pb->state));
	}
	break;
    }
}

/*
 * Listen for and process messages from peers.
 */
void mlpvt_mq_peer_msg_listener (char *msg, int len)
{
    char *bundle, *ptr;
    MQ_state_t *mq;
    PB_state_t *pb;
    BIDTYPE our_bid, peer_bid;
    ulong peer_id;
    int msg_type;
    ushort our_ref, peer_ref;
    
    /*
     * Listen for messages from peers. (When a MQ message comes in,
     * this function should get called via a registry service).
     * From each message, get
     *  - bundle id    : to get MQ_state
     *  - peer id      : to get PB_state
     *  - message type : for obvious reasons
     * Process based on message type and MQ & PB states.
     */

    msg_type = MLPVT_P2P_UNKNOWN;
    peer_id  = 0;
    bundle   = NULL;
    peer_bid = 0;
    our_ref  = 0;
    peer_ref = 0;
    
    msg += sizeof(msgfield_t); /* Skip message header */
    len -= sizeof(msgfield_t);
    
    ptr = mlpvt_mq_msg_field_get(msg, len, &msg_type,
				  sizeof(msg_type), MLPVT_MQ_MSG_TYPE);
    if (ptr == NULL) {
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALID, SGBP), "no message type");
	return;				/* Unknown message -- no type. Discard */
    }

    ptr = mlpvt_mq_msg_field_get(msg, len, &peer_id,
				  sizeof(peer_id), MLPVT_MQ_PEER_ID);
    if (ptr == NULL) {
	if (mlpvt_debug_errors)	
	    errmsg(&msgsym(INVALID, SGBP), "no peer ID");
	return;				/* No peer id. Discard. */
    }

    ptr = mlpvt_mq_msg_field_get(msg, len, &bundle, 0, MLPVT_MQ_BUNDLE); 
    if (ptr == NULL) {
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(INVALID, SGBP), "no MLP bundle name");
	return;
    }

    mlpvt_mq_msg_field_get(msg, len, &peer_ref, sizeof(peer_ref),
			    MLPVT_MQ_OUR_REFERENCE);

    mlpvt_mq_msg_field_get(msg, len, &our_ref, sizeof(our_ref),
			    MLPVT_MQ_PEER_REFERENCE);

    mlpvt_mq_msg_field_get(msg, len, &peer_bid,
			    sizeof(BIDTYPE), MLPVT_MQ_NEW_BID);
    
    mq = mlpvt_query_state_find(bundle);

    if (mlpvt_debug_msgs)
	errmsg(&msgsym(MQ, SGBP), mlpvt_msgtype_print(msg_type),
	       msg_type, our_ref, peer_ref, bundle, peer_bid, len);
	       
    if (mq == NULL) { /* New query ?? */

	if (msg_type == MLPVT_P2P_MQO) {
	    /*
	     * Initial query for mastership. This opens the bidding.
	     * If an MQ_state does not exist, ask MLP for its bid &
	     * create an MQ state. Then create a PB_state.
	     * If an MQ_state exists, determine action based on
	     * the query state.
	     */
	    our_bid = (BIDTYPE)MLP_SEARCH_BUNDLE(bundle);

	    /*
	     * MLP has returned either the count of the number of
	     * bundles it's holding, or a mastership claim.
	     * If it's not a mastership claim,
	     *  - if we're not configured to bid for queries from
	     *    peers, bid zero.
	     *  - if seedbid type is default and peer has bid 
	     *    something, bid zero.
	     *  - if sedbid type is offload or num, use
	     *    this as our load and reduce it from our seed bid.
	     *    Zero is the lowest our bid can go to.
	     */
	    if (our_bid != MLPVT_MASTERSHIP_CLAIM) {

		if (MLPVT_BIDTYPE == MLPVT_SEEDBID_DEFAULT) 
		    our_bid = MLPVT_MASTERSHIP_DECLINE;
		else {
		    our_bid  = MLPVT_SEED_BID - our_bid;
		    if (our_bid < 0)
			our_bid = 0;
		}
	    }
	    mq = mlpvt_query_state_create(bundle, our_bid);
	    free(bundle); /* We're done with bundle */
	    
	    if (mq == NULL) { /* bad */
		/*
		 * What do we do ? We may be the master.
		 * Let the other side timeout and retransmit ?
		 */
		return;
	    }

	    pb = mlpvt_pb_state_create(mq, peer_id);
	    if (pb == NULL) { /* bad */
		/*
		 * Free up mq. What else do we do ?
		 * Note that we may be the master...
		 * Let the other side timeout and retransmit ?
		 */
		mlpvt_query_state_delete(mq);
		return;
	    }
	    pb->ref      = peer_ref;
	    mq->state    = MQ_STATE_QUERY_FROM_PEERS;
	}
	else {
	    if (bundle)
		free(bundle);
	    return; /* Pitch anything other than MQO */
	}
    }
    else {  /* Not the first query for this bundle */

	if (bundle)   
	    free(bundle);  /* We don't need this now */
	
	/*
	 * See if we already have a bid state with this peer.
	 * If not, create one.
	 */
	pb = mlpvt_pb_state_find(mq, peer_id);
	if (pb == NULL) {
	    if (msg_type == MLPVT_P2P_MQO) { /* Query open */
		pb = mlpvt_pb_state_create(mq, peer_id);
		if (pb)
		    pb->ref = peer_ref;
		else
		    return;
	    }
	    else return; /* Pitch anything other than a MQO */
	}
	/*
	 * Else check query reference number ??
	 */
    }

    switch (msg_type) {

    case MLPVT_P2P_MQO:
	mlpvt_mq_p2p_msg_MQO_process(pb, peer_bid);
	break;

    case MLPVT_P2P_MB:
	mlpvt_mq_p2p_msg_MB_process(pb, peer_bid);
	break;
	
    case MLPVT_P2P_BA:
	mlpvt_mq_p2p_msg_BA_process(pb);
	break;
	
    case MLPVT_P2P_MQC:
	/*
	 * We've received a bid close. Someone is the master.
	 * Get that master's id from message.
	 */
	mq->master_id = 0;
	mlpvt_mq_msg_field_get(msg, len, &mq->master_id,
			       sizeof(mq->master_id),
			       MLPVT_MQ_MASTER_ID);

	mlpvt_mq_p2p_msg_MQC_process(pb);
	break;
	
    case MLPVT_P2P_MQA:
	mlpvt_mq_p2p_msg_MQA_process(pb);
	break;
	
    case MLPVT_P2P_MRR:
	/* Punt */
	break;
    default:
	if (mlpvt_debug_errors) {
	    errmsg(&msgsym(UNEXP, SGBP), "unknown", pb->addr,
		   peer_ref, mq->bundle, peer_bid);
	}
	return;
    }
}

/*
 ******************* Messages to peers **************************
 */
/*
 * Create and send specified MQ message to peer
 */
static void mlpvt_mq_p2p_msg_send (PB_state_t *pb, int msg_type)
{
    char *buf, *ptr;
    msgfield_t field;
    MQ_state_t *mq;
    int blen;

    if (pb == NULL) /* Bad evil */
	return;
    
    /*
     * Malloc a buffer.
     * Build message header.
     * Add fields in TLV form based on message type.
     * If no errors, ship it.
     */
    buf = malloc(MLPVT_MQ_MSG_MAXSIZE);
    if (buf == NULL) {
	return;
    }

    mq          = pb->parent;
    
    /*
     * Put common fields first.
     */
    ptr         = buf + sizeof(msgfield_t); /* Skip message header */

    /*
     * Message type : MQO, MB, BA, MQC, MQA, ...
     */
    field.type  = MLPVT_MQ_MSG_TYPE;
    field.len   = sizeof(msgfield_t) + sizeof(msg_type);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(msg_type, ptr)

    /*
     * Our ID in the group
     */
    field.type  = MLPVT_MQ_MEMBER_ID;
    field.len   = sizeof(msgfield_t) + sizeof(mq->our_id);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(mq->our_id, ptr)

    /*
     * Bundle name
     */
    blen        = strlen(mq->bundle);
    field.type  = MLPVT_MQ_BUNDLE;
    field.len   = sizeof(msgfield_t) + blen;
    ADD_TO_MQ_MSG(field, ptr)
    bcopy(mq->bundle, ptr, blen);
    ptr        += blen;

    /*
     * Our reference number for this query
     */
    field.type  = MLPVT_MQ_OUR_REFERENCE;
    field.len   = sizeof(msgfield_t) + sizeof(mq->ref);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(mq->ref, ptr)

    /*
     * Peer's reference number for this query
     */
    field.type  = MLPVT_MQ_PEER_REFERENCE;
    field.len   = sizeof(msgfield_t) + sizeof(pb->ref);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(pb->ref, ptr)

    /*
     * Bid seen, if any, from peer.
     */
    field.type  = MLPVT_MQ_ECHO_OF_BID_SEEN;
    field.len   = sizeof(msgfield_t) + sizeof(pb->peer_bid);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(pb->peer_bid, ptr)

    /*
     * Our bid
     */
    field.type  = MLPVT_MQ_NEW_BID;
    field.len   = sizeof(msgfield_t) + sizeof(mq->our_bid);
    ADD_TO_MQ_MSG(field, ptr)
    ADD_TO_MQ_MSG(mq->our_bid, ptr)

    /*
     * ID of the master. For a query close (MQC) only
     */
    if (msg_type == MLPVT_P2P_MQC) {
	field.type  = MLPVT_MQ_MASTER_ID;
	field.len   = sizeof(msgfield_t) + sizeof(mq->master_id);
	ADD_TO_MQ_MSG(field, ptr)
	ADD_TO_MQ_MSG(mq->master_id, ptr)
    }

    /*
     * Now go back and set the overall message header field
     */
    field.type  = MLPVT_MQ_SELECT;
    field.len   = (ushort)(ptr - buf);
    ptr         = buf;
    ADD_TO_MQ_MSG(field, ptr)

    /*
     * Now ship it !!
     */
    mlpvt_p2p_msg_output(pb->peer_id, buf, (int)field.len);
    if (mlpvt_debug_msgs)
	errmsg(&msgsym(SENT, SGBP), mlpvt_msgtype_print(msg_type),
	       pb->addr, mq->ref, pb->ref, mq->bundle, mq->our_bid,
	       field.len);

    free(buf); 
}

/*
 * Send message for opening the query
 */
static void mlpvt_mq_p2p_msg_MQO_send (PB_state_t *pb)
{
    mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQO);
    TIMER_START(pb->bid_timer, MLPVT_MQO_TIMER);
    pb->retries++;
}

static void mlpvt_mq_p2p_msg_MQO_send_resend (PB_state_t *pb)
{
    if (pb->retries == 0)
	mlpvt_mq_p2p_msg_MQO_send(pb);
    else if (pb->retries < MLPVT_MQO_RETRIES) {
	if (TIMER_RUNNING_AND_AWAKE(pb->bid_timer))
	    mlpvt_mq_p2p_msg_MQO_send(pb);
    }
    else {
	pb->state = PB_STATE_DELETED; /* OK to delete ? */
    }
}

/*
 * Send our bid for mastership in response to an opening query
 */
static void mlpvt_mq_p2p_msg_MB_send (PB_state_t *pb)
{
    mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MB);
    pb->state = PB_STATE_BID_SENT;
    TIMER_START(pb->bid_timer, MLPVT_MB_TIMER);
    pb->retries++;
}

static void mlpvt_mq_p2p_msg_MB_send_resend (PB_state_t *pb)
{
    if (pb->retries == 0) /* First time */
	mlpvt_mq_p2p_msg_MB_send(pb);
    else if (pb->retries < MLPVT_MB_RETRIES) {
	if (TIMER_RUNNING_AND_AWAKE(pb->bid_timer))
	    mlpvt_mq_p2p_msg_MB_send(pb);
    }
    else {
	pb->state = PB_STATE_DELETED; /* OK to delete ? */
    }
}

static void mlpvt_mq_peer_bid_accept (PB_state_t *pb)
{
    MQ_state_t *mq;
    
    mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_BA);
    if (pb->state != PB_STATE_DELETED) {
	mq              = pb->parent;
	mq->master_id   = pb->peer_id;
	mq->master_bid  = pb->peer_bid;
	mq->master_addr = MLPVT_PEER_ADDR(pb->peer_id);
	mlpvt_mq_make_peer_master(mq);
    }
}

/*
 ***************** MQ Process state machines *********************
 */
/*
 * Our mastership query has either been DONE or ABORTED.
 * Close bidding in cases where we opened the bidding and
 * were active. Clean up whatever peer bidding states we can.
 * If all peer states are gone, blow away the query state.
 */
static void mlpvt_mq_process_query_close (MQ_state_t *mq)
{
    PB_state_t *pb, *tmppb;
    int mq_in;
    
    /*
     * For each peer with whom we opened bidding, close
     * bidding session and blow away PB_state_t. When all
     * done, blow away MQ state. If not done, we are
     * waiting for peer to close.
     */
    mq_in = 0;
    tmppb = NULL;
    for (pb = mq->PB_states; pb; pb = tmppb) {
	tmppb = pb->next;  /* Save the next sucker */

	switch (pb->state) {

	case PB_STATE_BID_OPENED_TO_PEER :
	case PB_STATE_BID_RCVD :
	    /*
	     * Close bidding by sending an MQC or MQA
	     */
	    if (mq->state == MQ_STATE_QUERY_ABORTED)
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQA);
	    else
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQC);

	    /*
	     * Delete the PB state.
	     */
	    pb->state = PB_STATE_DELETED;
	    break;

	case PB_STATE_CREATED:
	case PB_STATE_BID_CLOSED:
	case PB_STATE_DELETED:
	default : /* Wait for peer to close or check timeouts */
	    /*
	     * Delete the PB state.
	     */
	    pb->state = PB_STATE_DELETED;
	    break;

	case PB_STATE_BID_XCHGD_PASSIVE:
	    /*
	     * If we've aborted, send an MQA to let the other
	     * side know. Then change our state.
	     * We can't send a close in this state.
	     */
	    if (mq->state == MQ_STATE_QUERY_ABORTED) {
		pb->state = PB_STATE_BID_SENT;
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQA);
	    }
	    break;
	    
	case PB_STATE_BID_XCHGD_ACTIVE:
	    /*
	     * If we've aborted, send an MQA to let the other
	     * side know. Then change our state.
	     * If we're done, send a close.
	     */
	    if (mq->state == MQ_STATE_QUERY_ABORTED) {
		pb->state = PB_STATE_BID_SENT;
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQA);
	    }
	    else {
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQC);
		pb->state = PB_STATE_BID_CLOSED;  /* Or delete ? */
	    }
	    break;

	case PB_STATE_BID_OPENED_FROM_PEER:
	case PB_STATE_BID_SENT:
	    /*
	     * Wait for peer to close. In the meantime, see if
	     * we need to retry if timer's expired. If we've sent
	     * too many retries, give up by deleting the state.
	     */
	    mlpvt_mq_p2p_msg_MB_send_resend(pb);
	    if (pb->state != PB_STATE_DELETED) {
		if ((pb->retries >= MLPVT_MB_RETRIES) &&
		    (TIMER_RUNNING_AND_AWAKE(pb->bid_timer))) {
                    if (mlpvt_debug_events)
		       errmsg(&msgsym(TIMEOUT, SGBP), pb->addr,
			   mlpvt_pbstate_print(pb->state));
		    pb->state = PB_STATE_DELETED;
		}
	    }
	    break;
	}

	if (pb->state == PB_STATE_DELETED)
	    mlpvt_pb_state_delete(pb);
	else if (pb->state == PB_STATE_BID_SENT)
	    mq_in++;
	
    }   /* end for loop */

    if (mlpvt_pb_states_none(mq)) { /* All peer states gone */
	mlpvt_query_state_delete(mq);
    }
    else if (mq_in && (mq->state == MQ_STATE_QUERY_ABORTED)) {
	/* Incoming queries left */
	mq->state = MQ_STATE_QUERY_FROM_PEERS;
    }
}

/*
 * Go through all the peer bidding states and see if
 * we can resolve a master.
 */
static void mlpvt_mq_select_master (MQ_state_t *mq)
{
    PB_state_t *pb, *selpb, *tmppb;
    uchar pending, equal_high_bid, winner;
    BIDTYPE high_bid;

    selpb    = NULL;
    high_bid = mq->our_bid;
    equal_high_bid = 1;
    pending    = 0;
    tmppb    = NULL;

    /*
     * First check do we have all the bids. The peer state
     * must be in BID_RCVD or BID_XCHGD_ACTIVE to qualify
     * as recieve bids. 
     * In addition keep track of the highest bid and 
     * quantity of highest bid.
     */
    for (pb = mq->PB_states; pb; pb = tmppb) {

	tmppb = pb->next; /* Save the next sucker first */

	switch (pb->state) {
	case PB_STATE_DELETED:
	    mlpvt_pb_state_delete(pb);
	    break;

	case PB_STATE_BID_RCVD:
	case PB_STATE_BID_XCHGD_ACTIVE:
	    /*
	     * Make note of first peer with a higher bid.
	     * If no one has a higher bid, we'll prevail.
	     */
  	    if (pb->peer_bid > high_bid) {
  		high_bid = pb->peer_bid;
  		selpb    = pb;
		equal_high_bid= 1;
  	    }
	    else if (pb->peer_bid == high_bid)
		equal_high_bid++;

	    break;

	default:
	    /*
	     * Count cases where we are waiting for
	     * a BA/MQC/MQA from a peer.
	     */
	    pending++;
	    break;
	}
    }

    /*
     * Not ready yet, there are more pending bids
     */
    if (pending)
	return;

    /*
     * If we are exhuasted and no one wants to bid
     * we will be the master
     */
    if (high_bid == 0) {
	    mlpvt_mq_make_us_master(mq);
	    return;
	}

    if (equal_high_bid == 1) {
	/* only one highest bid */
	if (selpb != NULL) {		/* highest bid is a  peer */
	    mlpvt_mq_peer_bid_accept(selpb);
	    mlpvt_pb_state_delete(selpb);
	}
	else {				/* we have the highest bid*/
	    mlpvt_mq_make_us_master(mq);
	}
    }
    else {
	/*
	 * we have multi highest bidders. 
	 * If we are one of the highest bidders, we win.
	 * Else we will randomly select a winner from other bidders.
	 */
	if (mq->our_bid == high_bid)
	    mlpvt_mq_make_us_master(mq);
	else {
	    winner = random_gen()%equal_high_bid;
	    
	    if (mlpvt_debug_queries)
		errmsg(&msgsym(EQUAL, SGBP), equal_high_bid, winner);

	    for (pb = mq->PB_states; pb; pb = tmppb) {

		tmppb = pb->next;	/* Save the next sucker first */

		switch (pb->state) {

		case PB_STATE_BID_RCVD:
		case PB_STATE_BID_XCHGD_ACTIVE:
		    if (pb->peer_bid ==  high_bid) {
			if (winner == 0){
			    mlpvt_mq_peer_bid_accept(pb);
			    mlpvt_pb_state_delete(pb);
			    return;
			} 
			winner--;
		    }
		    break;

		default:
		    break;
		}
	    }
	}
    }

    return;
}

/*
 * Process the QUERY_TO_PEERS state where we are querying
 * peers for mastership. Peers may also be querying us for
 * mastership simultaneously. If a peer queries us with a
 * higher bid, we go passive and let that peer figure out
 * the master. Note that a peer may query us with a higher
 * bid and then drop out by aborting his query.
 */
static void mlpvt_mq_process_query_to_peers (MQ_state_t *mq)
{
    PB_state_t *pb;
    ulong peer_id;
    
    /*
     * Get list of connected peers from lower layer.
     * Create PB_state_t where needed for each such peer.
     * Open bidding where not done. If bidding already open
     * and bid received, evaluate bid. If timeout for any
     * reply expected, resend message.
     */
    peer_id = MLPVT_FIRST_ACTIVE_PEERID;
    while ((peer_id != 0) && (mq->state == MQ_STATE_QUERY_TO_PEERS)) { 

	pb = mlpvt_pb_state_find(mq, peer_id);
	if (pb == NULL) {
	    pb = mlpvt_pb_state_create(mq, peer_id);
	}

	if (pb != NULL) {
	    switch (pb->state) {

	    case PB_STATE_CREATED :
		/*
		 * Newly created state. Open bidding.
		 */
		pb->state = PB_STATE_BID_OPENED_TO_PEER;
		mlpvt_mq_p2p_msg_MQO_send_resend(pb);
		break;

	    case PB_STATE_BID_OPENED_FROM_PEER :
		/*
		 * Peer beat us to it. Compare bids and
		 * see if we should be active or passive
		 * with this peer. Then send a query open
		 * to let peer know we are also querying.
		 */
		mlpvt_mq_peer_bid_process(pb, pb->peer_bid);

		if (pb->sub_state == PB_SUBSTATE_WE_WON) {
		    pb->state = PB_STATE_BID_XCHGD_ACTIVE;
		}
		else {
		    pb->state = PB_STATE_BID_XCHGD_PASSIVE;
		}
		mlpvt_mq_p2p_msg_MQO_send_resend(pb);

		break;
			    
	    case PB_STATE_BID_OPENED_TO_PEER :
	    case PB_STATE_BID_XCHGD_PASSIVE :
		/*
		 * See if timer's expired. If so, retransmit.
		 */
		mlpvt_mq_p2p_msg_MQO_send_resend(pb);
		break;
			    
	    case PB_STATE_BID_SENT :
		/*
		 * Nothing to do but wait for peer to
		 * send us something. Retry if timer expired.
		 */
		mlpvt_mq_p2p_msg_MB_send_resend(pb);
		break;
			    
	    case PB_STATE_BID_RCVD :
	    case PB_STATE_BID_XCHGD_ACTIVE :
		/*
		 * Is this a mastership claim or a simple
		 * bid ? If claim, then resolve bid.
		 */
		if (pb->peer_bid == MLPVT_MASTERSHIP_CLAIM) {
		    
		    mlpvt_mq_peer_bid_accept(pb);
		    pb->state = PB_STATE_BID_CLOSED; /* Or delete ? */
		}
		break;

	    default : /* Do nothing. Shouldn't happen */
		break;
	    }

	    if (pb->state == PB_STATE_DELETED)
		mlpvt_pb_state_delete(pb);
	    
	}   /* pb != NULL */

	peer_id = MLPVT_NEXT_ACTIVE_PEERID(peer_id);
		    
    }   /* end while all peers not done */

    /*
     * See if we still need to and can resolve the master.
     */
    if (mq->state == MQ_STATE_QUERY_TO_PEERS)
	mlpvt_mq_select_master(mq);
}

/*
 * Process the QUERY_FROM_PEERS state where peers are querying
 * us for mastership. We do not send queries to peers in this
 * state, but only our bids, after consulting MLP ofcourse.
 */
static void mlpvt_mq_process_query_from_peers (MQ_state_t *mq)
{
    PB_state_t *pb, *tmppb;

    /*
     * If we need to send our bid, do it.
     */
    tmppb = NULL;
    for (pb = mq->PB_states; pb; pb = tmppb) {

	tmppb = pb->next;
	switch (pb->state) {
	case PB_STATE_BID_OPENED_FROM_PEER:
	case PB_STATE_BID_SENT:
	    /*
	     * Send or resend our bid.
	     */
	    mlpvt_mq_p2p_msg_MB_send_resend(pb);
	    break;

	case PB_STATE_BID_CLOSED:

	    pb->state = PB_STATE_DELETED;
	    break;

	default:
	    break;
	}
	
	if (pb->state == PB_STATE_DELETED)
	    mlpvt_pb_state_delete(pb);
	
    }   /* end for loop */

    if (mlpvt_pb_states_none(mq))
	mlpvt_query_state_delete(mq);
}

/*
 * For each query in our list, see what state it is in
 * and call the appropriate worker routine.
 */
static void mlpvt_MQ_process_work (void)
{
    MQ_state_t *mq, *tmpmq;

    /*
     * Process state changes in MQ_state
     * (due to events from VPN or peers)
     */
    tmpmq = NULL;
    for (mq = MQ_reqQ.qhead; mq; mq = tmpmq) {
	tmpmq = mq->next;
	
	if (mq->state == MQ_STATE_QUERY_TO_PEERS) {
	    mlpvt_mq_process_query_to_peers(mq);
	}
	
	if (mq->state == MQ_STATE_QUERY_FROM_PEERS) {
	    mlpvt_mq_process_query_from_peers(mq);
	}  
	
	if ((mq->state == MQ_STATE_QUERY_DONE) ||
	    (mq->state == MQ_STATE_QUERY_ABORTED)) {
	    if (mlpvt_debug_queries) 
		mlpvt_show_debug_query_states();
	    mlpvt_mq_process_query_close(mq);
	}
    }
}

/*
 * Main mastership query process. Looks at the state machines
 * to see what's to be done. Either sends messages to peers,
 * or evaluates bids to resolve the master.
 */
static process mlpvt_MQ_process (void)
{
    ulong major, minor;

    MQ_process_activate = create_watched_boolean("MQ Activity", 0);
    process_watch_boolean(MQ_process_activate, ENABLE, RECURRING);

    TIMER_START(MQ_process_timer, MLPVT_MQ_PROCESS_WAKEUP);
    process_watch_timer(&MQ_process_timer, ENABLE);
    
    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case BOOLEAN_EVENT:
	    case TIMER_EVENT:

		if (mlpvt_debug_queries) {
		    if (process_get_boolean(MQ_process_activate))
			    mlpvt_show_debug_query_states();
		}	
		mlpvt_MQ_process_work();

		if (mlpvt_debug_queries) {
		    if (process_get_boolean(MQ_process_activate))
			    mlpvt_show_debug_query_states();
		}	
		process_set_boolean(MQ_process_activate, FALSE);
		TIMER_START(MQ_process_timer, MLPVT_MQ_PROCESS_WAKEUP);
		break;
	    }
	}   /* end while(wakeup) */
    }   /* end while(TRUE) */
}

/*
 * Shutdown Mastership Query process, probably because MLPVT
 * was deconfigured.
 * Clean up all our state machine data structures.
 * Kill the MQ process.
 */
void mlpvt_mq_shutdown (void)
{
    MQ_state_t *mq, *tmpmq;
    PB_state_t *pb, *tmppb;

    if (MQ_pid == NO_PROCESS)
	return;

    /*
     * For each MQ state, delete all the PB states
     * attached to it, then delete the MQ state.
     */
    tmpmq = NULL;
    for (mq = MQ_reqQ.qhead; mq; mq = tmpmq) {
	tmpmq = mq->next;

	tmppb = NULL;
	for (pb = mq->PB_states; pb; pb = tmppb) {
	    tmppb = pb->next;

	    switch (pb->state) {

	    case PB_STATE_BID_OPENED_TO_PEER:
	    case PB_STATE_BID_RCVD:
	    case PB_STATE_BID_XCHGD_PASSIVE:
	    case PB_STATE_BID_XCHGD_ACTIVE:
		mlpvt_mq_p2p_msg_send(pb, MLPVT_P2P_MQA);
		break;

	    default:
		break;
	    }
	    
	    mlpvt_pb_state_delete(pb);
	}

	/*
	 * AB: Should we abort all handles or assume we are
	 * master and set dont_forward ?
	 */
	mq->master = MLPVT_NO_MASTER;
	mlpvt_mq_all_handles_dispose(mq, VPN_ABORT);
	mlpvt_query_state_delete(mq);
    }

#ifdef MQ_SIMULATOR
    if (sim_pid != NO_PROCESS) {
	process_kill(sim_pid);
	sim_pid = NO_PROCESS;
	errmsg(&msgsym(SHUTDOWN, SGBP), "MQ simulation process");
    }
#endif /* MQ_SIMULATOR */

    process_kill(MQ_pid);
    MQ_pid = NO_PROCESS;
    if (mlpvt_debug_events)
	errmsg(&msgsym(SHUTDOWN, SGBP), "Mastership query process");

    reg_delete_mlpvt_peer_msg_listener(MLPVT_MQ_SELECT);
    reg_delete_vpn_peer_event_occured(mlpvt_mq_peer_event_listener);

    if (pb_state_chunk) {
	chunk_destroy(pb_state_chunk);
	pb_state_chunk = NULL;
    }
    if (mq_state_chunk) {
	chunk_destroy(mq_state_chunk);
	mq_state_chunk = NULL;
    }
}

#ifdef MQ_SIMULATOR
/*
 *================ Temporary stuff - Simulator =====================
 */
#define MAX_BUNDLES	8

static char *const bundles[] =
{
    "John",
    "Jim",
    "Peter",
    "Satish",
    "Suresh",
    "Andrew",
    "Dana",
    "Shoou",
    ""
};
static sys_timestamp sim_timer;
static VPNTYPE handle[MAX_BUNDLES];

void vpn_put_ipaddress_test (VPNTYPE *handle, ipaddrtype addr, int status)
{
    handle->status = status;
    handle->addr   = addr;
}

int mlp_search_bundle_test (char *bundle, BIDTYPE max)
{
    int i, bcnt;
    
    for (bcnt = 0, i = 0; i < MAX_BUNDLES; i++) {
	if (handle[i].status == VPN_DONT_FORWARD)
	    bcnt++;
    }

    for (i = 0; i < MAX_BUNDLES; i++) {
	if (strcmp(bundle, bundles[i]) == 0) {
	    if (handle[i].status == VPN_DONT_FORWARD)
		return (MLPVT_MASTERSHIP_CLAIM);
	    else if (handle[i].status == VPN_FORWARD)
		return (MLPVT_MASTERSHIP_DECLINE);
	    else
		return (bcnt);
	}
    }
    return ((sim_timer.u.value & 0x7f) &
	    ((sim_timer.u.value & 0x7f00) >> 8));
}

void mlp_appoint_bundle_test (char *bundle)
{
}

static process mlpvt_MQ_sim_process (void)
{
    ulong major, minor, id;
    int i, j;
    
    TIMER_START(sim_timer, 2*ONESEC);
    process_watch_timer(&sim_timer, ENABLE);

    for (i = 0; i < MAX_BUNDLES; i++)
	handle[i].status = VPN_NOT_ACTIVE;
    i = j = 0;
    while (TRUE) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
            case TIMER_EVENT:

		if (!mlpvt_debug_queries)
		    break;
		
		id = MLPVT_OUR_ID;
		if (handle[j].status == VPN_PENDING) {
		    mlpvt_query_cancel(bundles[j], &handle[j]);
		}
		handle[j].status  = VPN_NOT_ACTIVE;
		handle[j].addr    = 0;
		if (handle[j].status == VPN_NOT_ACTIVE) {
		    handle[j].status = VPN_PENDING;
		    handle[j].bundle = bundles[j];
		    mlpvt_query_lookup(bundles[j], &handle[j]);
		    j++;
		    if (j >= MAX_BUNDLES)
			j = 0;
		}
		break;

	    default:
		break;
	    }
	}

	TIMER_START(sim_timer, 2*ONESEC);
    }
}
#endif /* MQ_SIMULATOR */


/*
 * mlpvt_calibrate_seed_bid
 *
 * Seed bid will be the starting bid value when bidding for
 * mastership of any bundle/user. As a system takes up mastership
 * of bundles, the actual bid value will go lower than the seed
 * bid based on the number of bundles being handled.
 *     
 * For simplicity, each bundle will correspond to a bid value of 1.
 * Therefore, the seed bid can be viewed as the max number of bundles
 * a system desires to handle.
 *
 * On the basis of some investigation on different systems, an auto
 * calibration scheme for the seed bid has been derived. This
 * scheme is based on the timer calibration routines and results
 * in a seed bid based on the CPU power -- higher the power, higher
 * the seed bid. The 20usec factor is an outcome of the investigation
 * and considers the min seed value desired on the bottom rung of
 * the access server product line.
 *
 * So, for eg., a seed bid of 50 would imply that a system would bid
 * a non-zero value until it is handling 50 bundles. Note that this
 * does not imply that a system will not handle more than 'seed-bid'
 * bundles. After 'seed-bid' bundles, a system would handle bundles
 * thrust upon it by peers or by default (everyone else in the group
 * has stopped bidding a non-zero value).
 *
 * We round off the returned value to the nearest tens unit to account
 * for jitter in the calibration.
 */

static BIDTYPE mlpvt_calibrate_seed_bid(void) {

    BIDTYPE value;

    value = (BIDTYPE)timer_calibrate(20, FALSE);
    value += 4;
    value = value - (value % 10);
    if (value < 0)
	value = 0;
    else if (value >= MLPVT_MASTERSHIP_CLAIM)
	value = MLPVT_MASTERSHIP_CLAIM - 1;

    return (value);
}

/*
 * mlpvt_sample_seed_bid()
 *
 * With mlpvt_calibrate_seed_bid() we have a pretty consistent auto
 * generated seed bid value. However, there still is a chance for 
 * some jitters, less than 5%. Since we use the auto generated seed
 * value as the DEFAULT seed bid, so it must be the same every time
 * Hence, we need to remove that less than 5% jitter.
 * To do that we use a sample method. We generated a 
 * MLPVT_SAMPLE_ITERATION number of samples and pick the highest 
 * frequent generated sample value.
 */

/*
 * MLPVT_SAMPLE_ITERATION: Make 10 samples
 * EMPTY: use to indicate a seed[] slot is empty, since
 * mlpvt_calibrate_seed_bid only generate number in 10s
 * so  a 1 con uniquely identifies a slot is empty.
 */

#define MLPVT_SAMPLE_ITERATION 10 
#define EMPTY 1 

static BIDTYPE mlpvt_sample_seed_bid(void)
{
    int i, slot, high_count, high_count_seed;
    int count[MLPVT_SAMPLE_ITERATION];
    BIDTYPE value, seed[MLPVT_SAMPLE_ITERATION];

    /*
     * Initialize the count and seed array
     * seed[] collects unique generated seed value
     * count[] records the frequeny of a generated seed value
     */
    for (i=0 ; i<MLPVT_SAMPLE_ITERATION; i++) {
	count[i] = 0;
	seed[i] = EMPTY;
    }

    /*
     * Sample the seed value
     * put the seed value into seed[] 
     */
    for (i=0 ; i<MLPVT_SAMPLE_ITERATION; i++) {	

	/*
	 * Obtain the generated seed value
	 */
	value = mlpvt_calibrate_seed_bid();

	/*
	 * Search seed[], see if the same value has been
	 * recorded. If yes, increment the corresponding
	 * count[] entry
	 * If the seed[] slot is empty, it implies we have
	 * check all the entries in the seed[]. This is
	 * a new seed value, add this to the seed[].
	 */
	for (slot=0 ; slot<MLPVT_SAMPLE_ITERATION; slot++) {	
	    if (seed[slot] == value) {
		count[slot]++;		
		break;
	    }	
	    else {
		/*
		 * If this slot is EMPTY, this implies there
		 * are no more value in the seed[]
		 * Put it in the current slot
		 */
		if (seed[slot] == EMPTY) {
		    seed[slot] = value;
		    count[slot]++;
		    break;
		}
	    }
	}
    }

    /*
     * Find the most frequent seed value
     * If there is a tie between 2 seed value
     * the higher seed value wins
     */
    high_count= 0;
    high_count_seed = -1;
    for (i=0 ; i<MLPVT_SAMPLE_ITERATION; i++) {

	/*
	 * I have visit all the entries in the seed[]
	 * we can exit now
	 */
	if (seed[i] == EMPTY)
	    break;
	/*
	 * New most frequent seed value
	 */
	if (count[i] > high_count ) {
	    high_count_seed = i;
	    high_count = count[i];
	}
	else if (count[i] == high_count) {
	    /*
	     * Two equal highest frequency seed value
	     * use the seed value to break the tie
	     */
	    if (seed[i] > seed[high_count_seed]) {
		high_count_seed = i;
	    }
	}
    }

    /*
     * if high_count_seed is still -1, it implies
     * we did not get any generated seed value. This
     * shouldn't happen, return 0 in this case.
     */
    if (high_count_seed == -1)
	return 0;
    else
	return seed[high_count_seed];
}

/*
 * Mastership query module init function.
 * Initializes the queue for mastership query requests.
 * Sets up registry services.
 * Forks off MQ process.
 */
void mlpvt_mq_init (void)
{

    if (MQ_pid != NO_PROCESS) /* Process already running */
	return;
    
    queue_init(&MQ_reqQ, 0);
    mlpvt_debug_init();

    if (pb_state_chunk == NULL) {
	pb_state_chunk = chunk_create(sizeof(PB_state_t), 200, 0,
				      NULL, 0, "MLPVT PB state chunks");
	if (pb_state_chunk == NULL) /* Hopeless. Can't operate */
	    crashdump(0);
    }
    if (mq_state_chunk == NULL) {
	mq_state_chunk = chunk_create(sizeof(MQ_state_t), 50, 0,
				      NULL, 0, "MLPVT MQ state chunks");
	if (mq_state_chunk == NULL) /* Crash & burn */
	    crashdump(0);
    }
    
    reg_add_vpn_peer_event_occured(mlpvt_mq_peer_event_listener,
				     "mlpvt_mq_peer_event_listener");
    reg_add_mlpvt_peer_msg_listener(MLPVT_MQ_SELECT,
				    mlpvt_mq_peer_msg_listener,
				    "mlpvt_mq_peer_msg_listener");
    reg_add_mlpvt_query_lookup(mlpvt_query_lookup, "mlpvt_query_lookup");
    reg_add_mlpvt_query_cancel(mlpvt_query_cancel, "mlpvt_query_cancel");

    MQ_pid = process_create(mlpvt_MQ_process, "MLPVT MQ Process",
			    NORMAL_STACK, PRIO_NORMAL);
    if (MQ_pid == NO_PROCESS) {
	printf("\nMLPVT: Couldn't fork MQ process");
    }
    flush();

#ifdef MQ_SIMULATOR
    sim_pid = process_create(mlpvt_MQ_sim_process,
			     "MLPVT MQ Sim Process",
			     NORMAL_STACK, PRIO_NORMAL);
#endif

    /*
     * Get the sampled calibrated default seed bid for this box
     */
    calibrate_bid = mlpvt_sample_seed_bid();
}    





