
/* $Id: mlpvtgroup_connect.c,v 1.1.42.11 1996/08/28 13:00:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvtgroup_connect.c,v $
 *------------------------------------------------------------------
 * mlpvtgroup_connect.c -- MLPVT group connection setup
 *
 * January 1996, Andrew G. Harvey (SGBP/TCP)
 * Febuary 1997, Shoou Yiu (SGBP/UDP and State machine)
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvtgroup_connect.c,v $
 * Revision 1.1.42.11  1996/08/28  13:00:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.10  1996/07/30  18:20:33  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 *
 * Revision 1.1.42.9  1996/07/25  22:33:33  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 * SGBP may get 2 or more message on a closing tcb. Keep
 * track of free status of closing tcb in mlpvt_process_tcp_message()
 * Do not free an already freed tcb.
 *
 * Revision 1.1.42.8  1996/06/03  18:46:32  syiu
 * CSCdi59325:  SGBP messages should be debugged flagged
 * Branch: California_branch
 *
 * Revision 1.1.42.7  1996/06/03  18:02:50  syiu
 * CSCdi59320:  SGBP has too many errmsg facdefs
 * Branch: California_branch
 *
 * Revision 1.1.42.6  1996/05/22  03:41:53  perryl
 * CSCdi58388:  SGBP is missing an debug SGBP error option
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/05/21  18:18:18  perryl
 * CSCdi56888:  Bundle partitioning should reset and display warning
 * Branch: California_branch
 *
 * Revision 1.1.42.4  1996/05/19  23:32:25  perryl
 * CSCdi56888:  Bundle partitioning should reset and display warning
 * Branch: California_branch
 *
 * Revision 1.1.42.3  1996/05/18  22:35:06  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.2  1996/05/07  01:33:21  perryl
 * CSCdi56535:  MLPVT keeps its TCP pipe up even when peer is dead
 * Branch: California_branch
 * Add a 6 seconds keepalive ( three 2sec intervals)
 *
 * Revision 1.1.42.1  1996/04/27  06:34:49  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.8  1996/04/26  01:26:59  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.1.2.7  1996/04/22  00:31:22  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.5  1996/03/05  02:47:01  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.4  1996/03/01  22:58:12  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.3  1996/03/01  21:43:18  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.2  1996/02/13  18:03:32  agh
 * Branch: Dial1_branch
 * Clean up closing of TCP Pipes.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:48  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:56  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "msg_mlpvt.c"			/* Not a typo, see logger.h */
#include "login.h"
#include <ciscolib.h>
#include "../mlpvt/mlpvt.h"
#include "mlpvtgroup_connect.h"
#include "mlpvtgroup_discover.h"
#include "../wan/ppp_registry.h"

extern ipsocktype *mlpvt_get_socket(void);

/*
 * =================================================================
 * Name: mlpvt_close_pipe
 *
 * Description: Close the reliable pipe and clean up the member
 *		structure.
 *
 * Parameters:	mlpvtmember_t * - pointer to the member.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_close_pipe (mlpvtmember_t *memb)
{
    if (memb->status == MLPVTGROUP_MEMBER_IDLE)
        return;
    
    if (mlpvt_debug_events)
	errmsg(&msgsym(CLOSE, SGBP), memb->name);

    if (memb->status == MLPVTGROUP_MEMBER_ACTIVE) {
        if (mlpvt_debug_events | mlpvt_debug_errors)
           errmsg(&msgsym(LEAVING, SGBP), memb->name, memb->mlpvtgrp->name);
        reg_invoke_vpn_peer_event_occured(MLPVT_PEER_VANISH, memb->addr);
    }
    memb->retries = 0;
    memb->status = MLPVTGROUP_MEMBER_IDLE;
}

/*
 * =================================================================
 * Name: mlpvt_write_pipe
 *
 * Description: Send the message to the peer.  tcp_write may
 *		block 
 *
 * Parameters:	mlpvtmember_t * - pointer to the member.
 *		void * - pointer to the message.
 *		int - length of the message.
 *
 * Returns:	int - error number.
 *
 * =================================================================
 */

static int
mlpvt_write_pipe (mlpvtmember_t *memb, uchar *buffer, int len)
{
    paktype *pak;
    int offset;
    uchar *data;
    ipsocktype *soc;

    soc = mlpvt_get_socket();
    if (!soc) {
      if (mlpvt_debug_errors)
         buginf("\nSGBP write fail to %s, udp down", memb->name);
      return(ABORTED);  
    }

    /*
     * Bind Remote Address to this socket.  Pity this
     * is not hidden from the user.
     */
    soc->faddress.ip_addr = memb->addr;
    soc->faddress.type = ADDR_IP;

    pak = setup_ipsocket(soc, MLPVT_MAX_DGRAM_SIZE, &offset);
    if (pak == NULL) {
        if (mlpvt_debug_errors)
           buginf("\nSGBP write fail to %s, bad setup", memb->name);
        return(ABORTED);
    }

    if (len > (MLPVT_MAX_DGRAM_SIZE - offset))
       return(ABORTED);

    data = pak->data_area + offset;
    bcopy(buffer, data, len);

    if (len)
       write_ipsocket(soc, pak, len);
    else if (pak)
       retbuffer(pak);

    return(OK);
}

/*
 * =================================================================
 * Name: mlpvt_send_info_msg
 *
 * Description:	Send the info message to the peer.
 *
 * Parameters:	mlpvtmember_t * - pointer to member.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void mlpvt_send_info_msg (ipsocktype *soc, mlpvtmember_t *memb)
{
    mlpvt_hellomsg_t *info = NULL;
    int bundle_cnt = 0;
    char *bundle_list = NULL;
    int list_len = 0;
    int hostname_offset;

    hostname_offset = strlen(hostname) + 1;

    /*
     * Retrieve the bundle list from MLP
     * bundle_cnt has the number of bundle
     * bundle_list is a NULL term. list of bundle names
     * list_len is the length of the bundle list
     */
    reg_invoke_mlp_get_bundle_list(&bundle_cnt, &bundle_list, &list_len);

    info = malloc_named(list_len+hostname_offset+MLPVT_HELLOMSG_SIZE, 
			   "MLPVT info msg");
    if (!info) {
       return;
    }

    /*
     * Fill in the info msg header 
     */
    info->hdr.class = MLPVT_HELLO_CLASS;
    info->hdr.version = MLPVT_VERSION;
    info->peercnt = 0;
    info->addr = soc->laddress.addr.ip_address;
    info->reference = memb->mlpvtgrp->reference;   
    info->hdr.length = MLPVT_HELLOMSG_SIZE;
    info->type = MLPVT_HELLO_INFO;
    strcpy(info->string, hostname);
    info->hdr.length = MLPVT_HELLOMSG_SIZE + hostname_offset;

    /*
     * We have the bundle list, copy it to the end of the info msg
     * Delineate the hostname from bundle list with 0
     */
    if (list_len) {
        if (mlpvt_debug_hellos)
           buginf("\nSGBP Send Info, count=%d size=%d", bundle_cnt, list_len); 

        info->string[hostname_offset] = 0;
	info->peercnt = bundle_cnt;
	bcopy(bundle_list, &info->string[hostname_offset], list_len);
	info->hdr.length += list_len;
    }

    /*
     * ship it
     */
    mlpvt_write_pipe(memb, (uchar *)info , info->hdr.length);

    if (info)
       free(info);

    if (bundle_list)
	free(bundle_list);

}

/*
 * =================================================================
 * Name: mlpvt_accept
 *
 * Description:	Accept a SGBP connection.  
 *
 * Parameters:	paktype *
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

boolean mlpvt_accept (paktype *pak)
{
    iphdrtype *ip;
    mlpvtmember_t *memb;

    ip = (iphdrtype *)ipheadstart(pak);   
    memb = mlpvt_getmemberbyaddr(NULL, ip->srcadr);
    if (memb == NULL) {
	if (mlpvt_debug_errors)
	    errmsg(&msgsym(UNKNOWN, SGBP), ip->srcadr);
        return(FALSE);
    }
    return(TRUE);
}

/*
 * =================================================================
 * Name: mlpvt_infomsg_listener
 *
 * Description:	Process an incomming information message.  If the
 *		member is found then populate the member structure
 *		with the matric and reference value.
 *
 * Parameters:	char * - pointer to info message.
 *
 * Returns:	int - length of message.
 *
 * =================================================================
 */
#define MLP_SEARCH_BUNDLE(b) 	reg_invoke_mlp_search_bundle(b,MLPVT_MASTERSHIP_CLAIM)
void
mlpvt_infomsg_listener (char *data, int len)
{
    mlpvtmember_t *memb;
    mlpvt_hellomsg_t *info;
    int i;
    char *peerlist;

    info = (mlpvt_hellomsg_t *)data;

    if (mlpvt_debug_hellos)
        errmsg(&msgsym(RCVINFO, SGBP),
               info->addr, info->reference);

    memb = mlpvt_getmemberbyaddr(NULL, info->addr);

    if (memb == NULL) {
	if (mlpvt_debug_hellos || mlpvt_debug_errors)
	    buginf("\nSGBP Can't find member after receiving info message");
	return;
    }

    /*
     * if the peer is not myself then do the bundle duplication check
     * check the peer's address against all our addresses.
     */
    if (!ip_ouraddress(info->addr)) {
        memb->reference = info->reference;

	peerlist = (char *) &info->string;

        /*
         * Advance past the hostname of the sender
         */
        peerlist += strlen(peerlist)+1;

	for (i=0; i<info->peercnt; i++) {
            if (mlpvt_debug_hellos)
               buginf("\nSGBP remote bundle %s", peerlist);
 
	    if (MLPVT_MASTERSHIP_CLAIM == MLP_SEARCH_BUNDLE(peerlist))
		{
		    if (mlpvt_debug_hellos || mlpvt_debug_errors)
			errmsg(&msgsym(DUPLICATE, SGBP), peerlist, memb->name);
		    reg_invoke_mlp_reset_bundle(peerlist);
		}
	    peerlist += strlen(peerlist)+1;
	}
    }
    if (mlpvt_debug_events)
       errmsg(&msgsym(ARRIVING, SGBP), memb->name, memb->mlpvtgrp->name);
    reg_invoke_vpn_peer_event_occured(MLPVT_NEW_PEER, memb->addr);
}

/*
 * =================================================================
 * Name: mlpvt_p2p_msg_output
 *
 * Description: Wrapper that will send messages to each peer via its
 *		reliable pipe.
 *
 * Parameters:	ulong - reference number for the peer.
 *		char * - pointer to the message.
 *		int - length of the message.
 *
 * Returns:	int - error code.
 *
 * =================================================================
 */

int
mlpvt_p2p_msg_output (ulong ref, char *buffer, int len)
{
    mlpvtmember_t *memb;

    memb = mlpvt_getmemberbyref(NULL, ref);
    if (memb) {
	return (mlpvt_write_pipe(memb, buffer, len));
    }
    return (ABORTED);
}

