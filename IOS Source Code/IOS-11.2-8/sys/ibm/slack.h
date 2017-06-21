/* $Id: slack.h,v 3.2 1995/11/17 09:24:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/slack.h,v $
 *------------------------------------------------------------------
 * slack.h -- SDLC Local Termination Finite State Machine
 *
 * March 1992, Wayne Clark
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines, structures, and prototypes for SDLC Local Termination feature.
 *------------------------------------------------------------------
 * $Log: slack.h,v $
 * Revision 3.2  1995/11/17  09:24:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/01  02:32:09  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.2  1995/07/27  03:11:04  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.1  1995/06/07  20:48:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *  */

/***********************************************************************
*
*  File Description:
*	This include file defines all constants and structures unique
*	to the SDLC Local Termination feature.
*
*  Structures in this file:
*	sssss		ddddddddddddddddd
*
* This file contains the definition of "state" values for the SDLC Local
* Termination Finite State Machine.
*	SLACK_FSM	maintain state of SDLC Local Termination
*
**********************************************************************/

#ifndef SLACK_H
#define SLACK_H

/*
**   D e p e n d e n t   I n c l u d e   F i l e s
*/

/*
**   C o n s t a n t   D e f i n i t i o n s
*/
/*  No. seconds to wait between SNRMs  */
#define	SLACK_SNRM_THROTTLE	5

/*
**  SLACK flow control parameters:
**	SLACK_HI_PERCENT	percent full TCP/IP queue before RNR
**	SLACK_LO_PERCENT	after RNR, percent full TCP/IP queue
**				before RR
*/
#define	SLACK_HI_PERCENT	90
#define	SLACK_LO_PERCENT	80

/*
**  Values for slack_debug:
*/
#define	SLACK_DBG_FSM_ALL	0x07	/* slack_fsm tracing		*/
#define	SLACK_DBG_FSM_UFRAME	0x01	/*   - U-Frame events		*/
#define	SLACK_DBG_FSM_IFRAME	0x02	/*   - I-Frame events		*/
#define	SLACK_DBG_FSM_SFRAME	0x04	/*   - S-Frame events		*/
#define	SLACK_DBG_FSM_ACTION	0x10	/* action routine calls		*/
#define	SLACK_DBG_FLOW_CONTROL	0x20	/* slack flow control  		*/

/*
**
*/
#define SLACK_RETRY_OFF         -1      /* turn off retransmission      */
#define SLACK_RETRY_ON           0      /* turn on retransmission      */
 

/*
**   E n u m   D e f i n i t i o n s
*/

/*
**  Major Categories of Input to SLACK_FSM:
*/
typedef enum _slackmajor
{
    SLACK_SDLC    = 1,			/* from SDLC side of world	*/
    SLACK_NETWORK = 2,			/* from network side of world	*/
    SLACK_TIMER   = 3 			/* from timer expiration	*/
} slackmajor;

/*
**  Minor Categories of Input to SLACK_FSM:
*/
typedef enum _slackminor_sdlc
{
    SLACK_SDLC_SFRAME		= 1,	/* S-Frame (RR/RNR)		*/
    SLACK_SDLC_DM  		= 2,	/* DM           		*/
    SLACK_SDLC_FRMR		= 3,	/* FRMR 			*/
    SLACK_SDLC_DISC		= 4,	/* DISC				*/
    SLACK_SDLC_UA		= 5,	/* UA				*/
    SLACK_SDLC_SNRM		= 6,	/* SNRM				*/
    SLACK_SDLC_RD		= 7,	/* RD				*/
    SLACK_SDLC_IFRAME		= 8,	/* I-frame			*/
    SLACK_SDLC_XID		= 9,	/* XID  			*/
    SLACK_SDLC_SESSION_LOSS	= 10,	/* SDLC session loss		*/
    SLACK_SDLC_SIM              = 11,   /* SIM                          */
    SLACK_SDLC_RIM              = 12,   /* RIM                          */
    SLACK_SDLC_UI               = 13,   /* UI                           */
    SLACK_SDLC_TEST                     /* TEST                         */
} slackminor_sdlc;

typedef enum _slackminor_network
{
    SLACK_STUN_OP_PASS		= 1,	/* PassThrough			*/
    SLACK_STUN_OP_LINKUP_REQ	= 2,	/* LinkupRequest		*/
    SLACK_STUN_OP_LINKUP_RSP	= 3,	/* LinkupResponse		*/
    SLACK_STUN_OP_LINKDOWN_REQ	= 4,	/* LinkdownRequest		*/
    SLACK_STUN_OP_LINKDOWN_RSP	= 5,	/* LinkdownResponse		*/
    SLACK_STUN_OP_ABORT_REQ	= 6,	/* AbortRequest			*/
    SLACK_STUN_OP_ABORT_RSP	= 7,	/* AbortResponse		*/
    SLACK_STUN_OP_RD_REQ	= 8,	/* RequestDisconnectRequest	*/
    SLACK_STUN_OP_DATA		= 9,	/* Data				*/
    SLACK_NETWORK_SESSION_LOSS	= 10,	/* TCP/IP session loss		*/
    SLACK_STUN_OP_QUENCH_ON     = 11,   /* Flow control on              */ 
    SLACK_STUN_OP_QUENCH_OFF    = 12,   /* Flow control off             */ 
    SLACK_STUN_OP_SIM           = 13,   /* Sim                          */
    SLACK_STUN_OP_LINK_ESTAB    = 14    /* Link Established             */
} slackminor_network;

typedef enum _slackminor_timer
{
    SLACK_TIMER_DISC		= 1, 	/* Waiting for Disconnect	*/
    SLACK_RETRY                 = 2     /* retransmit the last STUN protocol
                                           packet */
} slackminor_timer;

/*
**  T y p e d e f s
*/

/*
**  S t r u c t u r e s
*/

/*
**  P r o t o t y p e   D e c l a r a t i o n s 
*/
/* -- In <ibm/slack_fsm.c> -- */
extern  ushort	slack_fsm(stunpeertype *, slackmajor, int);
extern	int	slack_fsm_decode(stunpeertype *, int, int);
extern	ushort	slack_action_a(stunpeertype *);
extern	ushort	slack_action_b(stunpeertype *);
extern	ushort	slack_action_c(stunpeertype *);
extern	ushort	slack_action_d(stunpeertype *);
extern	ushort	slack_action_e(stunpeertype *);
extern	ushort	slack_action_f(stunpeertype *);
extern  ushort  slack_action_fn(stunpeertype *);
extern	ushort	slack_action_g(stunpeertype *);
extern	ushort	slack_action_h(stunpeertype *);
extern	ushort	slack_action_i(stunpeertype *);
extern	ushort	slack_action_j(stunpeertype *);
extern	ushort	slack_action_k(stunpeertype *);
extern	ushort	slack_action_l(stunpeertype *);
extern	ushort	slack_action_m(stunpeertype *);
extern	ushort	slack_action_n(stunpeertype *);
extern	ushort	slack_action_o(stunpeertype *);
extern	ushort	slack_action_p(stunpeertype *);
extern	ushort	slack_action_q(stunpeertype *);
extern	ushort	slack_action_r(stunpeertype *);
extern	ushort	slack_action_s(stunpeertype *);
extern	ushort	slack_action_t(stunpeertype *);
extern	ushort	slack_action_u(stunpeertype *);
extern	ushort	slack_action_v(stunpeertype *);
extern	ushort	slack_action_w(stunpeertype *);
extern	ushort	slack_action_x(stunpeertype *);
extern	ushort	slack_action_y(stunpeertype *);
extern  ushort  slack_action_z(stunpeertype *);
extern  ushort  slack_action_aa(stunpeertype *);
extern  ushort  slack_action_ab(stunpeertype *);
extern  ushort  slack_action_ac(stunpeertype *);
extern  ushort  slack_action_ad(stunpeertype *);
extern  ushort  slack_action_ae(stunpeertype *);
extern  ushort  slack_action_af (stunpeertype *);
extern  ushort  slack_action_ag (stunpeertype *);
extern  ushort  slack_action_ah (stunpeertype *);
extern	char   *slack_prstate(int);
extern	char   *slack_prinput(int, int);
extern  slackminor_sdlc slack_sdlc_categorize(paktype *, stunpeertype *);
extern  slackminor_network slack_network_categorize(int);

/*
**  M a c r o s
*/
#define SDB_FROM_PEER(peer) ((sdlc_data_block *) \
                             (peer)->stun_idb->sdlc_data[(peer)->addr[0]])
#endif		/* SLACK_H */

