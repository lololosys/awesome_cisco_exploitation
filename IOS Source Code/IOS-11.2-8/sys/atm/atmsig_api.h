/* $Id: atmsig_api.h,v 3.2.52.5 1996/07/18 17:36:48 grobelch Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_api.h,v $
 *------------------------------------------------------------------
 * A T M S I G _ A P I . H 
 *
 * January 1995,  Percy P. Khabardar 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_api.h,v $
 * Revision 3.2.52.5  1996/07/18  17:36:48  grobelch
 * - QOS Unavail is 0x31, not 0x33
 * CSCdi63341:  ATMSIG: API release cause code QOS Unavailable incorrect
 * Branch: California_branch
 *
 * Revision 3.2.52.4  1996/06/08  20:13:13  cakyol
 * CSCdi59964:  a sig routine needs to return a vcd_t instead of an int
 * Branch: California_branch
 *
 * Revision 3.2.52.3  1996/05/31  01:08:03  shj
 * CSCdi46584:  LANE signaling needs to be smarter about ADD_PARTY
 * Branch: California_branch
 * Add cause code for "too many add_party's".
 *
 * Revision 3.2.52.2  1996/05/09  14:11:03  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.52.1.2.1  1996/04/27  06:33:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.52.1  1996/04/25  23:02:11  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/03/30  03:34:30  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.2.68.1  1996/04/08  01:44:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.58.1  1996/03/22  09:35:03  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.44.1  1996/03/05  05:48:55  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  20:40:34  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.5  1996/02/24  01:39:20  cakyol
 * added the "atmSig_printOpcode" user api function for
 * user debugging support.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.4  1996/01/30  23:56:02  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.3  1996/01/29  22:05:44  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_callId2CallingPartyAddress   and
 *     atmSig_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.2  1996/01/26  02:01:55  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:44:36  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:05:27  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  08:46:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/01  17:48:24  percyk
 * CSCdi39722:  set sdu sizes per connection - part 2
 *
 * Revision 2.2  1995/08/30  23:56:40  percyk
 * CSCdi39584:  set sdu sizes per connection - part 1
 *
 * Revision 2.1  1995/06/07  20:11:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMSIG_API_H__
#define __ATMSIG_API_H__


extern watched_queue *atmSig_outputQ;


#ifndef ATM_NSAP_ADDR_LNGTH
#define ATM_NSAP_ADDR_LNGTH      20
#endif

/* declare a size big enough to hold any ATM address */
#define	ATM_MAX_ADDR_LENGTH	ATM_NSAP_ADDR_LNGTH
#define ATM_MAX_SUBADDR_LENGTH	20		/* check this +++ */


/* 
 * For printing NSAPs in ASCII format.
 * Requires two bytes per byte of address, plus 10 '.'s, plus a
 * trailing null
 */
#define MAX_ATM_NSAP_ASCII_LEN	(ATM_NSAP_ADDR_LNGTH * 2 + 10 + 1)

#include "../atm/atmsig_ie.h"

typedef enum sig_api_opcode_ {
   ATM_LISTEN_NSAP,
   ATM_CANCEL_NSAP,
   ATM_SETUP,
   ATM_CONNECT,
   ATM_CONNECT_ACK,
   ATM_RELEASE,
   ATM_RELEASE_COMPLETE,
   ATM_STATUS_ENQUIRY,
   ATM_STATUS,
   ATM_ADD_PARTY,
   ATM_ADD_PARTY_ACK,
   ATM_ADD_PARTY_REJ,
   ATM_DROP_PARTY,
   ATM_DROP_PARTY_ACK,
} sig_api_opcode;


/* XXX, note: not all error codes from atmsig_ie.h are here! */
typedef enum sig_api_cause_code_ {
   ATM_UNALLOCATED         = 1,
   ATM_NO_ROUTE            = 2,
   ATM_NO_ROUTE_TO_DEST    = 3,
   ATM_VPCI_VCI_UNACCEPT   = 10,
   ATM_USER_BUSY           = 17,
   ATM_NO_USER_RESPOND     = 18,
   ATM_CALL_REJECTED       = 21,
   ATM_NUMBER_CHANGED      = 22,
   ATM_USER_REJECT         = 23,
   ATM_DEST_OUT_OF_ORDER   = 27,
   ATM_INVALID_NUMBER_FMT  = 28,
   ATM_STATUS_ENQ_RESP     = 30,
   ATM_NORMAL_UNSPECIFIED  = 31,
   ATM_VPCI_VCI_UNAVAIL    = 35,
   ATM_NET_OUT_OF_ORDER    = 38,
   ATM_TEMP_FAILURE        = 41,
   ATM_NO_VPCI	           = 45,
   ATM_RES_UNAVAIL         = 47,
   ATM_QOS_UNAVAIL         = 49,
   ATM_CELL_RATE_UNAVAIL   = 51,
   ATM_BC_UNAUTH           = 57,
   ATM_BC_UNAVAIL          = 58,
   ATM_BC_UNIMP            = 65,
   ATM_UNSUP_PARAM         = 73,
   ATM_INCOMPAT_DEST       = 88,
   ATM_INVALID_ENDPOINT    = 89,
   ATM_TOO_MANY_ADDS       = 92,
   ATM_AAL_PARAMS_NOT_SUPP = 93
} sig_api_cause_code;


/* internal error codes */
typedef enum sig_api_error_code_ {
   ATM_SIGAPI_OK,
   ATM_SIGAPI_FAIL
} sig_api_error_code;   


/*
 * Sig clients are responsible for formatting the entire BLL-IE contents
 * (except the IE header) in 'string' for outgoing Setup messages.  
 * 'length' indicates the number of bytes in 'string'. 
 * Likewise the Sig driver will empty the contents of the BLL-IE in
 * 'string' (except the IE header) for incoming Setup messages, set the 
 * 'length' and pass it onto the client.
 * In short, contents of BLL-IE fields are transparent to the Sig driver.
 */
typedef struct blli_type_ {
   int length;
   uchar string[ATM_BBAND_LOLAYER_MAXSIZE]; 
} blli_type;


/*
 * sig_api_listen
 *
 * This message is sent by a client to listen on a particular NSAP.
 * The client may listen on multiple NSAPs/interface pairs.  This
 * implies that the client may listen on multiple NSAPs on the same
 * interface or the same NSAP on multiple interfaces.
 */
typedef struct sig_api_listen_ {
   hwaddrtype nsap;
   void (*sig_user_callback)(void *);
   void *user_handle;
   sig_api_error_code result; 
} sig_api_listen;


/*
 * sig_api_cancel
 *
 * Sent by a client to cancel an outstanding listen request.
 */
typedef struct sig_api_cancel_ {
   hwaddrtype nsap;
   void *user_handle;
   sig_api_error_code result;
} sig_api_cancel;


/*
 * sig_api_setup
 *
 * Signalling clients sends this message to initiate a connection to a
 * peer.  The client must first call atmSig_api_allocate_call_id to
 * allocate a call_id which will be used in all succeeding messages
 * to identify the connection.  SigAPI will deliver this message to the
 * client who is listening on the called_party nsap.
 * The endpoint_id is only valid when the client receives a Setup 
 * message for a MTP call.
 * It should not be filled in when the client sends a Setup message.
 */
typedef struct sig_api_setup_ {
   void	*call_id;
   void *endpoint_id;
   void	(*sig_user_callback)(void *);
   hwaddrtype called_party;
   hwaddrtype calling_party;
   uint	flags;
   user_vc_type_t user_vc_type;
   atm_params_type params;
   blli_type blli[ATM_MAX_BLLI_NUM];
   vcd_t vcd;
   boolean multipoint;
   ushort fwd_sdu_size;
   ushort bwd_sdu_size;
} sig_api_setup;


/*
 * sig_api_connect
 *
 * This message is sent by a responding client to accept a setup message.
 * The endpoint_id is only valid when the client receives a Connect 
 * message for a MTP call.
 * It should not be filled in when the client sends a Connect message.
 */
typedef struct sig_api_connect_ {
   void *call_id;
   void *endpoint_id;
   vcd_t vcd;
   uint flags;
   user_vc_type_t user_vc_type;
   blli_type blli;
} sig_api_connect;


/*
 * sig_api_connect_ack
 *
 */
typedef struct sig_api_connect_ack_ {
   void	*call_id;
} sig_api_connect_ack;


/*
 * sig_api_release
 *   
 */
typedef struct sig_api_release_ {
   void *call_id;
   sig_api_cause_code cause;
} sig_api_release;


/*
 * sig_api_release_complete
 *
 */
typedef struct sig_api_release_comp_ {
   void *call_id;
   sig_api_cause_code cause;
} sig_api_release_comp;


/*
 * sig_api_status_enq
 *
 */
typedef struct sig_api_status_enq_ {
   void *call_id;
   void *endpoint_id;
} sig_api_status_enq;


/*
 * sig_api_status
 *
 */
typedef struct sig_api_status_ {
   void *call_id;
   void *endpoint_id;
   int state;
   sig_api_cause_code cause;
   int end_point_state;
} sig_api_status;


/*
 * sig_api_add_party
 *
 * Adds a call to an already existing connection which had been setup 
 * either with a SETUP or CONNECTed.  The initiating user must allocate
 * a fresh endpoint_id before calling.  The flags, params and blli
 * will be retained from the original connection that this call is
 * being added to.
 */
typedef struct sig_api_add_party_ {
   void *call_id;
   void *endpoint_id;
   hwaddrtype called_party;
} sig_api_add_party;


/*
 * sig_api_add_party_ack
 */
typedef struct sig_api_add_party_ack_ {
   void *call_id;
   void *endpoint_id;
} sig_api_add_party_ack;
 
 
/*
 * sig_api_add_party_rej
 */
typedef struct sig_api_add_party_rej_ {
   void *call_id;
   void *endpoint_id;
   sig_api_cause_code  cause;
} sig_api_add_party_rej;
 
 
/*
 * sig_api_drop_party
 */
typedef struct sig_api_drop_party_ {
   void *call_id;
   void *endpoint_id;
   sig_api_cause_code  cause;
} sig_api_drop_party;


/*
 * sig_api_drop_party_ack
 */
typedef struct sig_api_drop_party_ack_ {
   void *call_id;
   void *endpoint_id;
   sig_api_cause_code  cause;
} sig_api_drop_party_ack;


/*
 * sig_api_msg
 */
typedef struct sig_api_msg_ {
   struct sig_api_msg_       *link;  /* required by queue mang. */
   sig_api_opcode            opcode;
   idbtype                   *idb;
   union msg_body {
      sig_api_listen         listen;
      sig_api_cancel         cancel;
      sig_api_setup          setup;
      sig_api_connect        connect;
      sig_api_connect_ack    connect_ack;
      sig_api_release        release;
      sig_api_release_comp   release_comp;
      sig_api_status_enq     status_enq;
      sig_api_status         status;
      sig_api_add_party      add_party;
      sig_api_add_party_ack  add_party_ack;
      sig_api_add_party_rej  add_party_rej;
      sig_api_drop_party     drop_party;
      sig_api_drop_party_ack drop_party_ack;
   } u;
} sig_api_msg;

/*
** given the atm signalling opcode, returns
** a printable string form of it.
*/
extern char *atmSig_printOpcode (sig_api_opcode opcode);

/*
 * These utility functions are called by the user to allocate either a
 * call_id or an endpoint_id before calling setup or add_party.
 * Internal to signalling, this results in an svc control block or a 
 * party block getting allocated.
 */
extern void *atmSig_api_allocate_call_id(void);
extern void *atmSig_api_allocate_endpoint_id(void *call_id);

/*
 * this is the procedural interface to the signalling
 * proper.  The user passes a message and signalling performs
 * the action.  The user could pass this on the stack, since
 * the signalling will internally malloc a message block and
 * deal with it.  Returns TRUE if no trouble occured, FALSE
 * if malloc did not succeed.
 *
 * If the user passes the message on the heap, then he/she is
 * responsible for clearing it up after the command.  Since the
 * command makes a permanent copy internally, the storage can
 * be released immediately after returning from this call.
 */
extern boolean atmSig_handleUserRequest (sig_api_msg *userRequest);

/* 
** given the magic cookie "callId", returns the vcd
*/
extern vcd_t atmSig_api_callId2vcd (void *callId);

/*
** given the magic cookie "callId", copies the corresponding 
** calling party address into the user supplied hwaddrtype.
*/
extern void atmSig_api_callId2CallingPartyAddress (void *callId, 
    hwaddrtype *callingPartyAddress);

/*
** compares a user specified calling party address with
** the calling party address of a given call id.  Returns
** TRUE if same, FALSE otherwise.
*/
extern boolean atmSig_api_sameCallingPartyAddress (void *callId,
    hwaddrtype *callingPartyAddress);

#endif __ATMSIG_API_H__

