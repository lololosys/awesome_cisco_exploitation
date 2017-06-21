/* $Id: latcb.h,v 3.2 1995/11/17 17:33:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latcb.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latcb.h,v $
 * Revision 3.2  1995/11/17  17:33:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latcb		define lat engine circuit block
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATCB_H
#define LATCB_H         0		/* Indicate file has been read  */

/*----------------------------------------------------------------------
    Define the circuit types. Note, these definitions follow the
    master bit flag used in circuit messages.
 -----------------------------------------------------------------------*/
#define CB_SERVER	0x02		/* Server circuit (master)	*/
#define CB_HOST		0x00		/* Host   circuit (slave)	*/
#define CB_RRF          0x01            /* Response Requested Flag */

/*----------------------------------------------------------------------
    Define the circuit states.
 -----------------------------------------------------------------------*/
#define CB_RUN		0		/* (S)   Circuit sending RUN	*/
#define CB_RUN_W 	1		/* (S,H) Circuit waiting RUN	*/
#define CB_START	2		/* (S)   Circuit sending START	*/
#define CB_START_W	3		/* (S)   Circuit waiting START	*/
#define CB_IDLE		4		/* (S)   Host    balanced	*/
#define CB_BALANCE	5		/* (S,H) Circuit balanced	*/
#define CB_BALANCE_R	6		/* (H)   Host balanced, to run	*/

/*----------------------------------------------------------------------
    Define the circuit block.
 -----------------------------------------------------------------------*/
    typedef struct cb
    {
	struct sb SB_VIEW(cb_sb[SB_HASHES][2]);/* Hash headers for sb	*/
	struct sb SB_VIEW(cb_slot[2]);	/* Pointer to sessions (by slot)*/
	struct nd ND_VIEW(cb_nd);	/* Pointer to node structure	*/
	of_ptr	cb_xmit[2];		/* Pointer to transmited frames	*/
	int	cb_tics;		/* Remaining tics to trigger	*/
	word    cb_rmt_id;              /* Remote ID, sequence		*/
	word    cb_lcl_id;              /* Local  ID, sequence		*/
	byte	cb_type;		/* Circuit type			*/
	byte	cb_state;		/* Circuit state		*/
	byte	cb_session;		/* Next session ID to assign	*/
	byte	cb_max_ses;		/* Maximum session/circuit	*/
	byte	cb_vc_tics;		/* Circuit interval    (tics)	*/
	byte	cb_rx_tics;		/* Circuit retransmit  (tics)	*/
	byte	cb_hd_tics;		/* Circuit host delay  (tics)	*/
	byte	cb_ka_secs;		/* Circuit keep-alive  (secs)	*/
	byte	cb_tics_sec;		/* Circuit tics/second (tics)	*/
	byte	cb_retries;		/* Remaining retransmissions	*/
	byte	cb_halt_reason;		/* Circuit halt reason code	*/
	byte	cb_msg_seq;		/* Next message sequence number	*/
	byte	cb_msg_rcv;		/* Last message received	*/
	byte	cb_msg_ack;		/* Last message acknowledged	*/
	byte	cb_msg_tic;		/* Messages received in tic	*/
    	byte	cb_extra;		/* Extra receives available	*/
	byte	cb_ses_count;		/* Current number of sessions   */
    } cb_node, CB_VIEW(cb_ptr);
#endif
