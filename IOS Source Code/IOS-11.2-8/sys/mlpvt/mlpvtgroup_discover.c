/* $Id: mlpvtgroup_discover.c,v 1.1.42.11 1996/08/28 13:00:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvtgroup_discover.c,v $
 *------------------------------------------------------------------
 * mlpvtgroup_discover.c -- MLPVT group discovery via hellos
 *
 * January 1996, Andrew G. Harvey
 * Febuary 1997, Shoou Yiu (SGBP/UDP and State machine)
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvtgroup_discover.c,v $
 * Revision 1.1.42.11  1996/08/28  13:00:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.10  1996/06/04  00:22:26  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * misconfigurated -> misconfigured
 * authenication -> authentication
 * authenicated -> authenticated
 *
 * Revision 1.1.42.9  1996/06/03  18:02:52  syiu
 * CSCdi59320:  SGBP has too many errmsg facdefs
 * Branch: California_branch
 *
 * Revision 1.1.42.8  1996/05/23  06:35:53  perryl
 * CSCdi58477:  SGBP should send first Hello with minimal delay
 * Branch: California_branch
 * Send the 1st hello message out as soon as the system comes up.
 * For the 1st 20 seconds, send hello message in 4 seconds
 * intervals. After the initial 20 seconds, if it still can't connect
 * to its peer, send hellos message in 20 seconds interval.
 *
 * Revision 1.1.42.7  1996/05/22  03:41:54  perryl
 * CSCdi58388:  SGBP is missing an debug SGBP error option
 * Branch: California_branch
 *
 * Revision 1.1.42.6  1996/05/22  02:24:11  perryl
 * CSCdi56889:  MLPVT Hellos detection of misconfiguration
 * Branch: California_branch
 * Print the possible misconfigured member name in the
 * error message
 *
 * Revision 1.1.42.5  1996/05/21  09:57:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.42.4  1996/05/18  22:35:08  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/06  21:47:10  perryl
 * CSCdi56528:  MLPVT doesnt allow stack member to use more than 1 IP src
 * address.
 * Branch: California_branch
 * Add a secondary address list to the MLPVT member structure.
 * When a hello msg's source IP address different than the
 * configured member IP address, record it in the secondary
 * address list. When TCP pipe open request, it will than
 * able to recognize the address as the member's
 *
 * Revision 1.1.42.2  1996/05/01  02:06:53  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for L2F and MLPVT to do CHAP style authenication with the new AAA API.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:52  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.6  1996/04/26  01:27:01  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.1.2.5  1996/04/22  00:31:24  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.3  1996/03/05  02:47:04  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.2  1996/03/02  02:57:29  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:50  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:59  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../ip/ip_registry.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "msg_mlpvt.c"			/* Not a typo, see logger.h */
#include "aaa.h"
#include "login.h"
#include "../util/md5.h"
#include "../util/random.h"
#include "../if/network.h"
#include "../mlpvt/mlpvt.h"
#include "mlpvtgroup_connect.h"
#include "mlpvtgroup_discover.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include <ciscolib.h>

/*
 * Internal Prototypes required for forward reference
 */
static process mlpvt_hello_server(void);

static sys_timestamp mlpvt_hello_timer;

static void mlpvt_process_hello(ipsocktype *, paktype *, mlpvt_hellomsg_t *);
static void mlpvt_process_keepalive(ipsocktype *, paktype *, 
mlpvt_keepalive_t *);
  
static ipsocktype *sgbp_socket;
 
int rapidfire = 0;
  
extern mlpvtmember_t 
*mlpvt_alloc_member(mlpvtgroup_t *, char *, ipaddrtype, boolean);
extern void mlpvt_infomsg_listener(char *, int);
extern void mlpvt_mq_peer_msg_listener(char *, int);
extern void mlpvt_send_info_msg(ipsocktype *, mlpvtmember_t *);
extern boolean mlpvt_accept(paktype *);
extern void mlpvt_send_hello(ipsocktype *);

/*
 * SGBP over UDP overview:
 * SGBP consists over two components - the actual bidding component and
 * and the transport component. This section describes the transport 
 * component. Read overview in mlpvt_mq_protocol.c for the former.
 * 
 * The message handshake sequence is:
 *
 *   MLPVT_HELLO_CHALLENGE
 * ----------------------------->
 *
 *   MLPVT_HELLO_CHALLENGE_RESP
 * <----------------------------
 *
 *   MLPVT_HELLO_AUTHOK
 * ---------------------------->
 * 
 *   MLPVT_HELLO_INFO
 * <---------------------------
 *
 * After which, keepalive messages are exchanged
 *
 *   MLPVT_HELLO_KEPPALIVE
 * --------------------------->
 *
 *   MLPVT_HELLO_KEEPALIVE_ACK
 * <---------------------------
 *
 * The state transition goes from IDLE->AUTHOK->ACTIVE
 */

/*
 * =================================================================
 * Name: mlpvt_get_socket
 *
 * Description: Returns the socket that all messages will be sent and
 *		received on.
 *
 * Parameters:	void - None.
 *
 * Returns:	ipsocktype * - sgbp socket.
 *
 * =================================================================
 */

ipsocktype *mlpvt_get_socket (void)
{
    return(sgbp_socket);
}

/*
 * =================================================================
 * Name: mlpvt_set_socket
 *
 * Description:	Set the socket that all messages will be sent/received on
 *
 * Parameters:	ipsocktype * - sgbp socket.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_set_socket (ipsocktype *soc)
{
    sgbp_socket = soc;
}

static pid_t hello_pid = NO_PROCESS;

/*
 * =================================================================
 * Name: mlpvt_gethello_pid
 *
 * Description:	This wrapper returns the hello process pid.
 *
 * Parameters:	void - None.
 *
 * Returns:	pid_t - pid of hello process.
 *
 * =================================================================
 */

static pid_t
mlpvt_gethello_pid (void)
{
    return(hello_pid);
}

/*
 * =================================================================
 * Name: mlpvt_sethello_pid
 *
 * Description:	This wrapper sets the hello process pid.
 *
 * Parameters:	pid_t - pid of the hello process.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_sethello_pid (pid_t pid)
{
    hello_pid = pid;
}

/*
 * mlpvt_hello_timer_start
 * 
 * Starts the hello timer and resets rapidfire
 * This causes the hello timeout to happen and a slew of hellos sent out
 */
void mlpvt_hello_timer_start (void)
{
    TIMER_START(mlpvt_hello_timer, 0);
    rapidfire = 0;
}

/*
 * =================================================================
 * Name: mlpvt_if_state_change
 *
 * Description: If a LAN interface comes up, reset the rapidfire counter 
 * and wakeup the hello server to send a round of hellos to the
 * idle members.
 *
 * Parameters:	hwidb - hwidb of the changing state interface.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */
void mlpvt_if_state_change (idbtype *swidb, hwidbtype *hwidb)
{
    if (hwidb && (hwidb->status & (IDB_ETHER|IDB_TR|IDB_FDDI))) {
        mlpvt_hello_timer_start();
    }
}

/*
 * =================================================================
 * Name: mlpvt_hello_startup
 *
 * Description:	Start Hello Listener Process.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_hello_startup(void)
{
    pid_t pid;

    if (mlpvt_debug_events)
	errmsg(&msgsym(STARTUP, SGBP), "Hello Server process");    

    /*
     * Initiate hello process.
     */
    pid = process_create(mlpvt_hello_server,
			 "MLPVT Hello Server",
			 NORMAL_STACK, PRIO_NORMAL);
    if (pid == NO_PROCESS) {
        printf("\nCan't fork MLPVT Hello Server");
    }

    mlpvt_sethello_pid(pid);
}

/*
 * =================================================================
 * Name: mlpvt_build_authok_msg
 *
 * Description:	Build a authok message
 *
 * Parameter:	mlpvtmember_t * - pointer to member.
 *		ucahr * - pointer to message buffer.
 *
 * Returns:	int - length of message.
 *
 * =================================================================
 */
static int
mlpvt_build_authok_msg (mlpvtmember_t *memb, uchar *data)
{
    mlpvtgroup_t *grp;
    mlpvt_hellomsg_t *hello;
    uchar offset;

    grp = memb->mlpvtgrp;

    /*
     * Fill in header info
     */
    hello = (mlpvt_hellomsg_t *)data;
    hello->hdr.class = MLPVT_HELLO_CLASS;
    hello->hdr.version = MLPVT_VERSION;
    strcpy(hello->string, hostname);
    offset = strlen(hostname) + 1;

    hello->reference = grp->reference;
    hello->type = MLPVT_HELLO_AUTHOK;

    /*
     * Fill in the length of the whole hello packet
     */
    hello->hdr.length = offset + MLPVT_HELLOMSG_SIZE;
    return(hello->hdr.length);
}

/*
 * =================================================================
 * Name: mlpvt_build_challenge_msg
 *
 * Description:	Build a challenge  message
 *
 * Parameter:	mlpvtmember_t * - pointer to member.
 *		ucahr * - pointer to message buffer.
 *
 * Returns:	int - length of message.
 *
 * =================================================================
 */
static int
mlpvt_build_challenge_msg (mlpvtmember_t *memb, uchar *data)
{
    mlpvtgroup_t *grp;
    mlpvt_hellomsg_t *hello;
    uchar offset;

    grp = memb->mlpvtgrp;

    /*
     * Fill in header info
     */
    hello = (mlpvt_hellomsg_t *)data;
    hello->hdr.class = MLPVT_HELLO_CLASS;
    hello->hdr.version = MLPVT_VERSION;
    strcpy(hello->string, hostname);
    offset = strlen(hostname) + 1;
    /*
     * Fill in the reference number, challenge id and 
     * hello message type
     */
    hello->reference = grp->reference;

    memb->chapid++;
    memb->chapid %= MLPVT_MAX_CHAPID;
    hello->chapid = memb->chapid;

    hello->type = MLPVT_HELLO_CHALLENGE;


    /*
     * Fill in our challenge, which should be unique (not repeated)
     * and random.  Note that random_gen() only generates 0..10000,
     * so we use something else (plus extra bits to ensure uniqueness)
     */
    PUTLONG(memb->chapval, create_unique_number());
    random_fill(&memb->chapval[sizeof(ulong)],
		MLPVT_CHALLENGE_SIZE-sizeof(ulong), TRUE);
    bcopy(memb->chapval, &hello->string[offset], MLPVT_CHALLENGE_SIZE);

    /*
     * Fill in the length of the whole hello packet
     */
    hello->hdr.length = offset + MLPVT_HELLOMSG_SIZE + MLPVT_CHALLENGE_SIZE;

    return(hello->hdr.length);
}

/*
 * =================================================================
 * Name: mlpvt_build_response_msg
 *
 * Description:	Build a responsemessage
 *
 * Parameter:	mlpvtmember_t * - pointer to member.
 *		uchar * - pointer to message buffer.
 *              mlpvt_hellomsg_t - point to the challenge buffer 
 *
 * Returns:	int - length of message.
 *
 * =================================================================
 */
static int 
mlpvt_build_response_msg( mlpvtmember_t *memb, uchar *data, mlpvt_hellomsg_t *challenge)
{
    mlpvtgroup_t *grp;
    mlpvt_hellomsg_t *hello;
    userstruct *ustruct;
    authenstruct astruct;
    uchar offset;
    boolean ret;

    grp = memb->mlpvtgrp;

    /*
     * Fill in the hello header, and reference number, challange id 
     * hello message type
     */
    hello = (mlpvt_hellomsg_t *)data;
    hello->hdr.class = MLPVT_HELLO_CLASS;
    hello->hdr.version = MLPVT_VERSION;
    strcpy(hello->string, hostname); 
    offset = strlen(hostname)+1;

    hello->reference = grp->reference;
    hello->chapid = challenge->chapid;
    hello->type = MLPVT_HELLO_CHALLENGE_RESP;

    ret = aaa_chap_rchallenge (&ustruct, &astruct, grp->name, NULL,
				 challenge->chapid,
				 &challenge->string[strlen(challenge->string)+1],
				 MLPVT_CHALLENGE_SIZE, "default");


    if (!ret) {
	aaa_cleanup_login(ustruct, &astruct);
	aaa_free_user(&ustruct);
	return 0;
    }

    bcopy(astruct.server_data, &hello->string[offset], MD5_LEN);
    aaa_cleanup_login(ustruct, &astruct);
    aaa_free_user(&ustruct);

    /*
     * Fill in the length
     */
    hello->hdr.length = offset + MLPVT_HELLOMSG_SIZE + MD5_LEN;
   
    return(hello->hdr.length);
    
}


/*
 * =================================================================
 * Name: mlpvt_build_keepalive_msg
 *
 * Description:	Build a keepalive message
 *
 * Parameter:	mlpvtmember_t * - pointer to member.
 *		ucahr * - pointer to message buffer.
 *
 * Returns:	int - length of message.
 *
 * =================================================================
 */
static int
mlpvt_build_keepalive_msg (mlpvtmember_t *memb, uchar *data, int type)
{
    mlpvtgroup_t *grp;
    mlpvt_keepalive_t *keepalive;
    uchar offset;

    grp = memb->mlpvtgrp;

    /*
     * Fill in header info
     */
    keepalive = (mlpvt_keepalive_t *)data;
    keepalive->hdr.class = MLPVT_KEEPALIVE_CLASS;
    keepalive->hdr.version = MLPVT_VERSION;

    keepalive->reference = grp->reference;
    keepalive->type = type;
 
    strcpy(keepalive->groupname, grp->name);
    strcpy(keepalive->membername, memb->name);
    offset = strlen(memb->name) + 1;

    /*
     * Fill in the length of the whole keepalive packet
     */
    keepalive->hdr.length = offset + MLPVT_KEEPALIVEMSG_SIZE;

    return(keepalive->hdr.length);
}

/*
 * =================================================================
 * Name: mlpvt_check_challenge_response
 *
 * Description:	Check the response to our challenge
 *
 * Parameter:	mlpvtmember_t * - pointer to member.
 *		mlpvt_hellomsg_t * - pointer to the response.
 *
 * Returns:	True - if response matches the challenge
 *
 * =================================================================
 */
static boolean
mlpvt_check_challenge_response ( mlpvtmember_t *memb, mlpvt_hellomsg_t *response)
{
    mlpvtgroup_t *grp;
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret;
    grp = memb->mlpvtgrp;
    

    ret = aaa_chap_rresponse (&ustruct, &astruct, grp->name, NULL, 
				   &response->string[strlen(response->string)+1],
				   memb->chapid, memb->chapval,
				   MLPVT_CHALLENGE_SIZE, "default");

    aaa_cleanup_login(ustruct, &astruct);
    aaa_free_user(&ustruct);
    return (ret);
}

/*
 * =================================================================
 * Name: mlpvt_send_authok_msg
 *
 * Description:	Send an authOK message to a peer.
 *
 * Parameters:	ipsocktype * - socket to send the message on.
 *		mlpvtmember_t * - member to send the message to.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void 
mlpvt_send_authok_msg (ipsocktype *soc, mlpvtmember_t *memb)
{
    paktype *pak;
    int offset;
    uchar *data;
    int len=0;

    /*
     * Bind Remote Address to this socket.  Pity this
     * is not hidden from the user.
     */
    soc->faddress.ip_addr = memb->addr;
    soc->faddress.type = ADDR_IP;
    
    pak = setup_ipsocket(soc, MLPVT_MAX_DGRAM_SIZE, &offset);
    if (pak == NULL)
        return;

    data = pak->data_area + offset;

    len = mlpvt_build_authok_msg(memb, data);
    if (len)
	write_ipsocket(soc, pak, len);
    else if (pak)
        retbuffer(pak);
}

/*
 * =================================================================
 * Name: mlpvt_send_hello_msg
 *
 * Description:	Send an hello message to a peer.
 *
 * Parameters:	ipsocktype * - socket to send the message on.
 *		mlpvtmember_t * - member to send the message to.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_send_hello_msg (ipsocktype *soc, mlpvtmember_t *memb,  mlpvt_hellomsg_t *challenge)
{
    paktype *pak;
    int offset;
    uchar *data;
    int len=0;

    /*
     * Bind Remote Address to this socket.  Pity this
     * is not hidden from the user.
     */
    soc->faddress.ip_addr = memb->addr;
    soc->faddress.type = ADDR_IP;
    
    pak = setup_ipsocket(soc, MLPVT_MAX_DGRAM_SIZE, &offset);
    if (pak == NULL)
        return;

    data = pak->data_area + offset;

    /*
     * If we receive a challenge, send a response to the challenge
     * Otherwise send a challenge to the peer
     */
    if (!challenge) {
       len = mlpvt_build_challenge_msg(memb, data);
    } else if (challenge->type == MLPVT_HELLO_CHALLENGE) {
	  len = mlpvt_build_response_msg(memb, data, challenge);

	  if (!len && (mlpvt_debug_hellos || mlpvt_debug_errors)) 
	     errmsg(&msgsym(NORESP, SGBP), memb->name,memb->mlpvtgrp->name);
    }

    if (len)
	write_ipsocket(soc, pak, len);
    else if (pak)
        retbuffer(pak);
}

/*
 * =================================================================
 * Name: mlpvt_send_keepalive_msg
 *
 * Description:	Send an keepalive message to a peer.
 *
 * Parameters:	ipsocktype * - socket to send the message on.
 *		mlpvtmember_t * - member to send the message to.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_send_keepalive_msg (ipsocktype *soc, mlpvtmember_t *memb, int type)
{
    paktype *pak;
    int offset;
    uchar *data;
    int len=0;

    /*
     * Bind Remote Address to this socket.  Pity this
     * is not hidden from the user.
     */
    soc->faddress.ip_addr = memb->addr;
    soc->faddress.type = ADDR_IP;
    
    pak = setup_ipsocket(soc, MLPVT_MAX_DGRAM_SIZE, &offset);
    if (pak == NULL)
        return;

    data = pak->data_area + offset;

    len = mlpvt_build_keepalive_msg(memb, data, type);

    if (len)
	write_ipsocket(soc, pak, len);
    else if (pak)
        retbuffer(pak);
}

/*
 * =================================================================
 * Name: mlpvt_send_keepalive
 *
 * Description:	Driving process that scans all active members
 *		and initiates the sending of a keepalive message.
 *
 * Parameters:	ipsocktype * - socket to send the message via.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */
static void 
mlpvt_send_keepalive (ipsocktype *soc, mlpvtmember_t *memb)
{
    /*
     * If Retry max not reached, send a Keepalive to the peer 
     * Otherwise move member to IDLE. Induce rapidfire sequence
     * for Hello messages for faster recovery (if any)
     */ 
    if (memb->retries < MLPVT_KEEPALIVE_RETRIES ) {
       if (mlpvt_debug_hellos)
          errmsg(&msgsym(KEEPALIVE, SGBP), memb->name, memb->retries);
       memb->retries++;
       mlpvt_send_keepalive_msg(soc, memb, MLPVT_KEEPALIVE_TICKLE);
     } else {
       if (mlpvt_debug_hellos)
          errmsg(&msgsym(KEEPALIVE_TIMEOUT, SGBP), memb->name);
       mlpvt_hello_timer_start();
       mlpvt_close_pipe(memb);
     }
}

/*
 * =================================================================
 * Name: mlpvt_send_messages
 *
 * Description:	Send SGBP messages.
 *
 * Parameters:	ipsocktype * - socket that receives the hello msg.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_send_messages (ipsocktype *soc, boolean rapid)
{
    mlpvtmember_t *memb;
    mlpvtgroup_t *grp;

    grp = mlpvt_getgroup();
    if (grp != NULL) {
        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {

	  switch (memb->status) { 
	      case MLPVTGROUP_MEMBER_IDLE:
		   /*
                    * If rapid flag set, we want the Hello Challenge to be 
                    * sent in this tick;
                    */
                   if (rapid)
                      memb->ticks = MLPVT_HELLO_TICKS;

		   /*
                    * Send Hello Challenges on IDLE members when timer 
                    * tick is reached. 
                    */
                   if (memb->ticks++ < MLPVT_HELLO_TICKS)
                      break;
                   memb->ticks = 0;
                   if (mlpvt_debug_hellos) 
		       errmsg(&msgsym(CHALLENGE, SGBP), memb->name, 
                              memb->mlpvtgrp->name);    
                   mlpvt_send_hello_msg(soc, memb, NULL);
                   break;

               case MLPVTGROUP_MEMBER_AUTHOK:
                   /*
                    * (Re)send AuthOK. Will timeout on max retry and move 
                    * member back to IDLE. Induce rapidfire sequence
                    * for Hello messages for faster recovery (if any) 
                    */
	           if (memb->retries < MLPVT_AUTHOK_RETRIES) {
	              if (mlpvt_debug_hellos)
	                 errmsg(&msgsym(SENDAUTHOK, SGBP), memb->name, 
                                memb->addr);
                      memb->retries++;
                      mlpvt_send_authok_msg(soc, memb);
                   } else {
                      mlpvt_hello_timer_start();
                      mlpvt_close_pipe(memb);
                   }
                   break;

	       case MLPVTGROUP_MEMBER_ACTIVE:
		   /*
                    * Send the Keepalive message only if we have not heard 
                    * a Keepalive from this member. Otherwise, don't send 
                    * a Keepalive. This lessens by half the Keepalives and 
                    * Keepalive Acks sent between two ACTIVE members.   
                    */
		   if (AWAKE(memb->peer_timeout)) {
                      /*
                       * (Re)send out Keepalive on ACTIVE members. Will timeout
                       * and move back to IDLE 
                       */
                      mlpvt_send_keepalive(soc, memb);
                   }
                   break;

		default:
                   break;
	  }
          memb = mlpvt_getnext_member(grp, memb);
        }
     }
}

/*
 * =================================================================
 * Name: mlpvt_process_messages
 *
 * Description:	Receive and process all incomming messages.
 *		If the reference count is higher then open an
 *		reliable pipe.
 *
 * Parameters:	ipsocktype * - socket that receives the hello msg.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_process_messages (ipsocktype *soc)
{
    udptype *udp;
    paktype *pak;
    mlpvt_msghdr_t *msg;
    iphdrtype *ip;
    
    while ((pak = read_ipsocket(soc, FALSE, 0,
                                &udp, &msg)) != NULL) {

      if (msg->version < MLPVT_VERSION) {
         ip = (iphdrtype *)ipheadstart(pak);
         if (mlpvt_debug_hellos || mlpvt_debug_events || mlpvt_debug_errors)
            buginf("\nSGBP Received incompatible version from %i", ip->srcadr);
         datagram_done(pak);
         continue;
      }

      switch (msg->class) {
        case MLPVT_HELLO_CLASS:
             mlpvt_process_hello(soc, pak, (mlpvt_hellomsg_t *)msg);
             break;

        case MLPVT_MQ_CLASS:
             mlpvt_mq_peer_msg_listener((char *)msg, (int) msg->length);
             break;

	case MLPVT_KEEPALIVE_CLASS:
             mlpvt_process_keepalive(soc, pak, (mlpvt_keepalive_t *)msg);
             break;
      }
      retbuffer(pak);
    }
}

/*
 * =================================================================
 * Name: mlpvt_process_keepalive
 *
 * Description:	Receive and process all keepalive messages.
 *
 * Parameters:	ipsocktype * - socket that receives the hello msg.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */
static void
mlpvt_process_keepalive (ipsocktype *soc, paktype *pak, 
mlpvt_keepalive_t *keepalive)
{
     iphdrtype *ip;
     mlpvtmember_t *memb, *dup_memb;
     mlpvtgroup_t *grp;

     ip = (iphdrtype *)ipheadstart(pak);

     /*
      * If this is a Keepalive and the stack name matches, send an ACK back. 
      * Else if this is a Keepalive ACK, clear the retry count.
      */
     switch (keepalive->type) {

      case MLPVT_KEEPALIVE_TICKLE:
           /*
            * Make sure there is a Stack Group and it matches
            */ 
           grp = mlpvt_getgroupbyname(keepalive->groupname);
           if (grp) {
	        /*
                 * Send an ACK to sender. The ACK contains *this*
                 * host's name, which is in the keepalive message.
                 * Get the sender's name from the source IP address, 
                 * If the remote member is not defined, silently discard it.
                 * Otherwise, we've heard a Keepalive from the sender. 
                 * Reset the sender's peer_timeout so that we do not have
                 * to send a Keepalive *to* the same sender within this time
                 * since we know the sender is alive and well 
                 * (since it sent this message)
                 */
                memb = mlpvt_getmemberbyaddr(NULL, ip->srcadr);
                if (memb) {
                   if (mlpvt_debug_hellos)
                        buginf("\nSGBP Send Keepalive ACK to %s", memb->name);
                   TIMER_START(memb->peer_timeout, 
                              (2*MLPVTGROUP_DEFAULT_CRETRY));
                } else
                  return;
	        
                /*
                 * The keepalive->membername is the name of *this* host. 
                 * There isn't a member defined on this host obviously.
                 * Therefore, we need to create a dummy member of this host's 
                 * name for the sole purpose of sending a Keepalive ACK back
                 */         
                dup_memb = mlpvt_alloc_member(grp, keepalive->membername, 
                                   ip->srcadr, FALSE);
		if (dup_memb) {
                  mlpvt_send_keepalive_msg(soc, dup_memb, MLPVT_KEEPALIVE_ACK);
                  free(dup_memb->name);
                  free(dup_memb);
                } 
           }
           break;

      case MLPVT_KEEPALIVE_ACK:
           memb = mlpvt_getmemberbyname(NULL, keepalive->membername);
           if (memb == NULL) {
              if (mlpvt_debug_hellos || mlpvt_debug_errors)
                 buginf("\nSGBP Rcv unknown or invalid Keepalive ACK for %s", 
                     keepalive->membername);
              return;
           }
           if (mlpvt_debug_hellos)
              buginf("\nSGBP Keepalive ACK rcv from %s", memb->name);

           memb->retries = 0;
           break;
     }
}

/*
 * =================================================================
 * Name: mlpvt_process_hello
 *
 * Description:	Receive and process all incoming hello messages.
 *              These include Hello Challenge, Resposne, AuthOK and Info
 *              messages
 *
 * Parameters:	ipsocktype * - socket that receives msg
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_process_hello (ipsocktype *soc, paktype *pak, mlpvt_hellomsg_t *msg)
{
    iphdrtype *ip;
    mlpvtmember_t *memb;
    
    ip = (iphdrtype *)ipheadstart(pak);
    memb = mlpvt_getmemberbyname(NULL, msg->string);
    if (memb == NULL) {
       if (mlpvt_debug_hellos || mlpvt_debug_errors) {
	   /*
	    * If this is a miss configured member.
	    * we should have a NULL term. name in the 
	    * name field
	    * Since this packet may be totally junk
	    * do not take any assumption fo the hello
	    * data structure.
	    * Assume 40 characters to be the max of a 
	    * reasonable hostname.
	    */
	   int i; 
	   char *name = (char *)msg->string;
	   char *ptr = name;
	   boolean found = FALSE;
		
	   for (i =0; i<40; i++, ptr++) {
	       if (*ptr == 0) {
                  found = TRUE;
		  break;
	       }
	   }
	   if (found && (i > 0))
	       errmsg(&msgsym(MISSCONF, SGBP), name, ip->srcadr);
	   else
	       errmsg(&msgsym(UNKNOWNHELLO, SGBP), ip->srcadr);
	   }
	   return;
    }

    if (ip->srcadr != memb->addr) {
       if (mlpvt_debug_hellos || mlpvt_debug_errors) 
	  errmsg(&msgsym(DIFFERENT, SGBP), memb->name, memb->addr, ip->srcadr);
	  if (!mlpvt_add_secaddr(memb, ip->srcadr)){             
	     if (mlpvt_debug_hellos || mlpvt_debug_errors)
		errmsg(&msgsym(MISSCONF, SGBP), memb->name, ip->srcadr);
	     return;
	  }
    }

    if (mlpvt_debug_hellos || mlpvt_debug_errors) {
       if (ip->srcadr != memb->addr)
	  errmsg(&msgsym(DIFFERENT, SGBP), memb->name, memb->addr, ip->srcadr);
    }

    /*
     * Depending on the Message received, 
     */
    switch (msg->type) {
	case MLPVT_HELLO_CHALLENGE:
	    /*
             * Action: Send Hello Response
             * Move to State: Stay in current state
             */
	    if (mlpvt_debug_hellos) {
               errmsg(&msgsym(CHALLENGED, SGBP), memb->name, ip->srcadr);
               errmsg(&msgsym(RESPONSE, SGBP), memb->name, 
                      memb->mlpvtgrp->name); 
            }              
	    mlpvt_send_hello_msg(soc, memb, msg);    
	    break;

	case MLPVT_HELLO_CHALLENGE_RESP:
            if (mlpvt_debug_hellos)
               errmsg(&msgsym(RESPONSED, SGBP), memb->name, ip->srcadr);
	    if (!(mlpvt_check_challenge_response(memb , msg))) {
		if (mlpvt_debug_hellos || mlpvt_debug_errors) 
		    errmsg(&msgsym(AUTHFAILED, SGBP), memb->name);
		return;
	    } else {
		/*
                 * Action: Send AuthOK message
                 * Move to State: Move state from IDLE to AUTHOK
		 */
	        if (memb->status == MLPVTGROUP_MEMBER_IDLE) {
	           if (mlpvt_debug_errors || mlpvt_debug_events)
	              errmsg(&msgsym(AUTHOK, SGBP), memb->name);
		   memb->status = MLPVTGROUP_MEMBER_AUTHOK;
                   memb->retries = 0;
                }  
	        if (mlpvt_debug_hellos)
	           errmsg(&msgsym(SENDAUTHOK, SGBP), memb->name, 
                          memb->addr);
                mlpvt_send_authok_msg(soc, memb);
	    }
	    break;

        case MLPVT_HELLO_AUTHOK:
	    /*
             * Action: Send Info message
             * Move to State: Stay in current state
             */
	    if (mlpvt_debug_hellos)
	       errmsg(&msgsym(SENDINFO, SGBP), memb->name);
            mlpvt_send_info_msg(soc, memb);
            break;

	case MLPVT_HELLO_INFO:
	    /*
             * Action: None
             * Move to State: Move state from AUTHOK to ACTIVE
             * Set the peer timeout so that a Keepalive can be sent at 
             * the next tick 
	     */
	    if (mlpvt_accept(pak)) {
	        if (memb->status == MLPVTGROUP_MEMBER_AUTHOK) {
	          if (mlpvt_debug_errors || mlpvt_debug_events)
	             errmsg(&msgsym(ACTIVE, SGBP), memb->name);
                  memb->status = MLPVTGROUP_MEMBER_ACTIVE;
                  TIMER_START(memb->peer_timeout, 0);  
                  memb->retries = 0;               
	       }
               mlpvt_infomsg_listener((char *)msg, (int) msg->hdr.length);
            }
	    break;

	default:
	    break;
     }
}

/*
 * =================================================================
 * Name: mlpvt_hello_shutdown
 *
 * Description:	Close the hello socket and shutdown the hello
 *		listen process.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_hello_shutdown (void)
{
    ipsocktype *soc;
    pid_t pid;

    TIMER_STOP(mlpvt_hello_timer);

    soc = mlpvt_get_socket();
    if (soc != NULL)
        close_ipsocket(soc);
    mlpvt_set_socket(NULL);
    
    if (mlpvt_debug_events)
	errmsg(&msgsym(STARTUP, SGBP), "Hello Server process");    
    pid = mlpvt_gethello_pid();
    if (pid != NO_PROCESS)
        process_kill(pid);
    mlpvt_sethello_pid(NO_PROCESS);
}

/*
 * =================================================================
 * Name: mlpvt_hello_server
 *
 * Description: Main hello process.  This will wait for incomming
 *		hello messages and then dispatch them to the
 *		appropriate routine.  This process will also handle
 *		all hello timers.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static process
mlpvt_hello_server (void)
{
    ipsocktype *soc;
    ulong major, minor; 
    
    process_wait_on_system_config();

    /*
     * Init the rapidfire counter
     */
    rapidfire = 0;

    /*
     * loop until we're able to open a listen port.
     */
    while (TRUE) {
	soc = open_ipsocket(UDP_PROT, 0L, 0L,
                            MLPVT_DISCOVER_PORT, MLPVT_DISCOVER_PORT,
                            SOCK_ANYSRC);
	if (soc == NULL) {
	    process_sleep_for(ONEMIN);
	    continue;
	}
	break;
    }

    mlpvt_set_socket(soc);
    
    process_watch_queue(soc->inq, ENABLE, RECURRING);
    TIMER_START(mlpvt_hello_timer, MLPVTGROUP_FAST_CRETRY);
    process_watch_timer(&mlpvt_hello_timer, ENABLE);
    
    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case DIRECT_EVENT:
            case TIMER_EVENT:

		/*
		 * For the 1st MLPVTGROUP_FAST_CRETRY_INTERVAL *
                 * MLPVTGROUP_FAST_CRETRY seconds, we have a more frequent
		 * retry interval to send all messages to peer by *this* 
                 * tick. We are trying to bringup the members to active ASAP.
                 * Once it settles down, the timer is set to 
                 * MLPVTGROUP_DEFAULT_CRETRY. All messages are this sent in 
                 * this timer tic. The exception is the sending of HELLO 
                 * challenges for IDLE members in greater-than-ticks 
                 * granularity. Hence not every timer tick results 
                 * in a Hello sent for IDLE members - they are more spread 
                 * out since IDLE members may be offline for an extended 
                 * period of time.  
		 */
                TIMER_STOP(mlpvt_hello_timer);
		if (rapidfire < MLPVTGROUP_FAST_CRETRY_INTERVAL) {
		    TIMER_START(mlpvt_hello_timer,MLPVTGROUP_FAST_CRETRY);
		    rapidfire++;
                    mlpvt_send_messages(soc, TRUE);
		}
		else {
		   TIMER_START(mlpvt_hello_timer, MLPVTGROUP_DEFAULT_CRETRY);
                   mlpvt_send_messages(soc, FALSE);
                }
                break;

            case QUEUE_EVENT:
                mlpvt_process_messages(soc);
                break;

            default:
		if (mlpvt_debug_errors)
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
            }
        }
    }
}

