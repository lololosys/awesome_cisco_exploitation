/* $Id: slack_fsm.c,v 3.4.46.3 1996/07/19 22:46:04 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/slack_fsm.c,v $
 *------------------------------------------------------------------
 * slack_fsm.c -- SDLC Local Termination Finite State Machine
 *
 * March 1992, Wayne Clark
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Tables, finite state machine, action routines, and miscellaneous
 * functions for the SDLC Local Acknowledgement feature.
 *------------------------------------------------------------------
 * $Log: slack_fsm.c,v $
 * Revision 3.4.46.3  1996/07/19  22:46:04  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.4.46.2  1996/05/17  11:21:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.34.2  1996/04/12  07:19:08  athippes
 * CSCdi53276:  STUN w/local-ack remote serial reconnects and host
 * receives abort
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.34.1  1996/04/03  14:34:43  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.46.1  1996/03/18  20:11:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  09:42:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  14:21:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/06  23:46:33  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 3.3  1995/11/17  09:24:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/09  01:14:43  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/08/01 02:32:12  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.3  1995/07/27  03:11:07  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.2  1995/07/06  01:32:41  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.1  1995/06/07 20:48:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *  */

/*
 * slack_fsm.c -- SDLC Local Termination Finite State Machine
 *
 *      March 4, 1992 - Wayne Clark
 *
 */

/******************************************************************************
*
*  File Description:
*	This file contains the Finite State Machine logic for SDLC Local 
*	Termination.
*
*	See <os/fsm.c> and <os/fsm.h> for a definition of how a Finite
*	State Machine is represented in tabular form.
*
*  Structures in this file:
*   For SLACK_FSM:
*	slack_fsm_table		 the finite_state_machine
*	slack_disconnected	 fsm_state for Disconnected state 
*	slack_await_linkup_rsp	 fsm_state for AwaitLinkupResponse state 
*	slack_await_sdlc_open	 fsm_state for AwaitSdlcOpen state 
*	slack_active		 fsm_state for Active state 
*	slack_await_linkdown_req fsm_state for AwaitLinkdownRequest state 
*	slack_await_linkdown_rsp fsm_state for AwaitLinkdownResponse state 
*	slack_await_linkdown_ua	 fsm_state for AwaitLinkdownUa state 
*	slack_await_disc	 fsm_state for AwaitDisc state 
*	slack_await_abort_rsp	 fsm_state for AwaitAbortResponse state 
*
*  Functions in this file:
*   For SLACK_FSM:
*	slack_fsm		SDLC Local Termination FSM
*	slack_fsm_decode	decoder for slack_fsm
*	slack_action_a		action routine A
*	slack_action_b		action routine B
*	slack_action_c		action routine C
*	slack_action_d		action routine D
*	slack_action_e		action routine E
*	slack_action_f		action routine F
*	slack_action_g		action routine G
*	slack_action_h		action routine H
*	slack_action_i		action routine I
*	slack_action_j		action routine J
*	slack_action_k		action routine K
*	slack_action_l		action routine L
*	slack_action_m		action routine M
*	slack_action_n		action routine N
*	slack_action_o		action routine O
*	slack_action_p		action routine P
*	slack_action_q		action routine Q
*	slack_action_r		action routine R
*	slack_action_s		action routine S
*
*  Development Notes for SLACK_FSM:
*   (1)	Handling Abnormal Disonnections
*	Loss of an SDLC connection results in a STUN_OP_ABORT_REQ being
*	send to the STUN peer.  Since an SDLC connection can be lost in
*	any state, all states must be able to handle the following
*	inputs:
*	    Major		Minor			Action
*	    -----		----			------
*	    SLACK_SDLC		SDLC Session Loss	output STUN_OP_ABORT_REQ
*	    SLACK_NETWORK	STUN_OP_ABORT_REQ	output STUN_OP_ABORT_RSP
*
*   (2)	SDLC Connection Loss in AwaitLinkdownResponse
*	If either the primary or secondary STUN peers receive an SDLC
*	session loss after having received a DISC and having sent a
*	STUN_OP_LINKDOWN_REQ to its STUN peer (i.e. while the local
*	STUN peer is in AwaitLinkdownResponse state), merely accept the
*	SDLC session loss input and do nothing other than make the
*	transition to Disconnected.  (During development, a STUN_OP_ABORT_REQ 
*	was received ahead of the STUN_OP_LINKDOWN_REQ.  Since both sides
*	are on their way down to Disconnected, the results are the same.)
*
*   (3)	Appropriate Response to a DISC
*	While a UA is supposed to be the appropriate response to a DISC
*	sent from the primary SDLC link station, sometimes the 3174
*	will respond to a DISC with a DM.  As such, when a primary-side
*	of STUN is in AwaitLinkdownUa after having sent a DISC to the
*	secondary SDLC link station it is attached to, DM and UA are
*	both treated as adequate responses.
*
#ifdef RECONSIDERED
*   (4)	DISC while in Disconnected State
*	Receiving DISC while in Disconnected state means that we are
*	probably talking to a primary SDLC station that isn't playing
*	with a full deck.  Merely ignore the DISC and stay in
*	Disconnected state.
#endif
*   (5)	DISC while in AwaitLinkdownResponse State
*	If we receive a 2nd DISC while in AwaitLinkdownResponse state
*	(remember, we could only have gotten to AwaitLinkdownResponse
*	by having received a DISC), then consider it an Abnormal
*	Disconnect sequence and abort the STUN peer connection.  
*	We are justified in doing this since repeated DISCs after
*	the first DISC / DM exchange are violations of the SDLC protocol
*	(Notice that we were never officially in Normal Response Mode to
*	begin with since there was no valid SNRM / UA sequence.)
*	  Primary	secondary STUN		primary STUN	secondary
*	  -------	--------------		------------	---------
*								[dead]
*         SNRM -->
*                       LINKUP_REQ -->
*                       (AwaitLinkupResponse)
*                                               SNRM -->
*                                               (AwaitSdlcOpen)
*         DISC -->
*                   <-- DM
*                       (AwaitLinkdownResponse)
*                       LINKDOWN_REQ -->
*                                           <-- LINKDOWN_RSP
*                                               (Disconnected)
*				[Note (5a).]
*         DISC -->
*                       ABORT_REQ -->
*                       (Disconnected)
*                                           <-- ABORT_RSP
*
*  (5a)	Loss of STUN_OP_LINKDOWN_RSP
*	For some unexplainged reason, the STUN_OP_LINKDOWN_RSP sent from the
*	primary STUN peer to the secondary was never received.  Not worth
*	tracking down!
*
*****************************************************************************/

/*
**   I n c l u d e   F i l e s
*/
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ibm/ibm.h"
#include "../ibm/ibm_externs.h"
#include "../ibm/ibm_public.h"
#include "../ibm/slack.h"
#include "../util/fsm.h"
#include "../ibm/sdlc.h"
#include "../ibm/sdlc_public.h"
#include "../ibm/sdlc_debug.h"
#include "../ip/ip_registry.h"
#include <string.h>

/*
**   C o n s t a n t   D e f i n i t i o n s
*/

/*
**   L o c a l   D e f i n i t i o n s
*/
static char	print_buffer[30];
static int	output_major;
static int	output_minor;

/*
**   G l o b a l   D e f i n i t i o n s
*/

/*
**   E x t e r n a l   D e f i n i t i o n s
*/
extern  ulong   stun_slack_fsm_timeout;
extern  boolean quick_response;
/*
**   F o r w a r d   D e c l a r a t i o n s 
*/

/******************************************************************************
 *
 *                              S L A C K _ F S M 
 *
 * 		     	      S t a t e   T a b l e
 *
 *****************************************************************************/

/*
 * A few definitions to cast the action routines appropriately so that
 * the table stays nice and pretty:
 */

#define SLACK_ACTION_a FSM_ACTION_ROUTINE(slack_action_a)
#define SLACK_ACTION_b FSM_ACTION_ROUTINE(slack_action_b)
#define SLACK_ACTION_c FSM_ACTION_ROUTINE(slack_action_c)
#define SLACK_ACTION_d FSM_ACTION_ROUTINE(slack_action_d)
#define SLACK_ACTION_e FSM_ACTION_ROUTINE(slack_action_e)
#define SLACK_ACTION_f FSM_ACTION_ROUTINE(slack_action_f)
#define SLACK_ACTION_fn FSM_ACTION_ROUTINE(slack_action_fn)
#define SLACK_ACTION_g FSM_ACTION_ROUTINE(slack_action_g)
#define SLACK_ACTION_h FSM_ACTION_ROUTINE(slack_action_h)
#define SLACK_ACTION_i FSM_ACTION_ROUTINE(slack_action_i)
#define SLACK_ACTION_j FSM_ACTION_ROUTINE(slack_action_j)
#define SLACK_ACTION_k FSM_ACTION_ROUTINE(slack_action_k)
#define SLACK_ACTION_l FSM_ACTION_ROUTINE(slack_action_l)
#define SLACK_ACTION_m FSM_ACTION_ROUTINE(slack_action_m)
#define SLACK_ACTION_n FSM_ACTION_ROUTINE(slack_action_n)
#define SLACK_ACTION_o FSM_ACTION_ROUTINE(slack_action_o)
#define SLACK_ACTION_p FSM_ACTION_ROUTINE(slack_action_p)
#define SLACK_ACTION_q FSM_ACTION_ROUTINE(slack_action_q)
#define SLACK_ACTION_r FSM_ACTION_ROUTINE(slack_action_r)
#define SLACK_ACTION_s FSM_ACTION_ROUTINE(slack_action_s)
#define SLACK_ACTION_t FSM_ACTION_ROUTINE(slack_action_t)
#define SLACK_ACTION_u FSM_ACTION_ROUTINE(slack_action_u)
#define SLACK_ACTION_v FSM_ACTION_ROUTINE(slack_action_v)
#define SLACK_ACTION_w FSM_ACTION_ROUTINE(slack_action_w)
#define SLACK_ACTION_x FSM_ACTION_ROUTINE(slack_action_x)
#define SLACK_ACTION_y FSM_ACTION_ROUTINE(slack_action_y)
#define SLACK_ACTION_z FSM_ACTION_ROUTINE(slack_action_z)
#define SLACK_ACTION_aa FSM_ACTION_ROUTINE(slack_action_aa)
#define SLACK_ACTION_ab FSM_ACTION_ROUTINE(slack_action_ab)
#define SLACK_ACTION_ac FSM_ACTION_ROUTINE(slack_action_ac)
#define SLACK_ACTION_ad FSM_ACTION_ROUTINE(slack_action_ad)
#define SLACK_ACTION_ae FSM_ACTION_ROUTINE(slack_action_ae)
#define SLACK_ACTION_af FSM_ACTION_ROUTINE(slack_action_af)
#define SLACK_ACTION_ag FSM_ACTION_ROUTINE(slack_action_ag)
#define SLACK_ACTION_ah FSM_ACTION_ROUTINE(slack_action_ah)
/*
**  SLACK_FSM State = "Disconnected"
*/
const static struct fsm_state slack_disconnected[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_ag,FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME         */
  {SLACK_ACTION_ag,FSM_NOARG,    FSM_NOCHANGE},           /* 01  SDLC, SDLC_DM             */
  {SLACK_ACTION_ag,FSM_NOARG,    FSM_NOCHANGE},           /* 02  SDLC, SDLC_FRMR           */
  {SLACK_ACTION_aa,FSM_NOARG,    FSM_NOCHANGE},           /* 03  SDLC, SDLC_DISC           */
  {SLACK_ACTION_ad,FSM_NOARG,    FSM_NOCHANGE},           /* 04  SDLC, SDLC_UA             */
  {SLACK_ACTION_b, FSM_NOARG,    SLACK_AWAIT_LINKUP_RSP}, /* 05  SDLC, SDLC_SNRM           */
  {SLACK_ACTION_aa,FSM_NOARG,    FSM_NOCHANGE},           /* 06  SDLC, SDLC_RD             */
  {SLACK_ACTION_ag,FSM_NOARG,    FSM_NOCHANGE},           /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_fn,FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
  {SLACK_ACTION_a, FSM_NOARG,    SLACK_AWAIT_SDLC_OPEN},  /* 13  Network, ...LINKUP_REQ    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 14  Network, ...LINKUP_RSP    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 15  Network, ...LINKDOWN_REQ  */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 16  Network, ...LINKDOWN_RSP  */
  {SLACK_ACTION_p, FSM_NOARG,    FSM_NOCHANGE},           /* 17  Network, ...ABORT_REQ,sec */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 18  Network, ...ABORT_RSP     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 19  Network, ...RD_REQ        */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    FSM_NOCHANGE},           /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 23  SDLC, session loss        */
  {SLACK_ACTION_p, FSM_NOARG,    FSM_NOCHANGE},           /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_z, FSM_NOARG,    FSM_NOCHANGE},           /* 25  Network, ...TCP loss, sec */
  {SLACK_ACTION_z, FSM_NOARG,    FSM_NOCHANGE},           /* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitLinkupResponse"
*/

const static struct fsm_state slack_await_linkup_rsp[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_q, FSM_NOARG,    SLACK_ACTIVE},           /* 00  SDLC, SDLC_SFRAME         */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
  {SLACK_ACTION_j, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_RSP},/* 03 SDLC, SDLC_DISC           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
  {SLACK_ACTION_b, FSM_NOARG,    FSM_NOCHANGE},           /* 05  SDLC, SDLC_SNRM           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
  {SLACK_ACTION_c, FSM_NOARG,    SLACK_ACTIVE},           /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 13  Network, ...LINKUP_REQ    */
  {SLACK_ACTION_g, FSM_NOARG,    FSM_NOCHANGE},           /* 14  Network, ...LINKUP_RSP    */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 15  Network, ...LINKDOWN_REQ  */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_n, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_ah,FSM_NOARG,    SLACK_DISCONNECTED},     /* 25  Network, ...TCP loss, sec */
  {SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitSdlcOpen"
*/

const static struct fsm_state slack_await_sdlc_open[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 00  SDLC, SDLC_SFRAME         */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 03  SDLC, SDLC_DISC           */
  {SLACK_ACTION_d, FSM_NOARG,    SLACK_ACTIVE},           /* 04  SDLC, SDLC_UA             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 05  SDLC, SDLC_SNRM           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
  {SLACK_ACTION_a, FSM_NOARG,    FSM_NOCHANGE},           /* 13  Network, ...LINKUP_REQ    */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
  {SLACK_ACTION_h, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_UA},/* 15  Network, ...LINKDOWN_REQ  */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 25  Network, ...TCP loss, sec */
  {SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "Active"
*/

const static struct fsm_state slack_active[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_q, FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME         */
  {SLACK_ACTION_k, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_REQ},/* 01  SDLC, SDLC_DM            */
  {SLACK_ACTION_u, FSM_NOARG,    SLACK_AWAIT_ABORT_RSP},  /* 02  SDLC, SDLC_FRMR           */
  {SLACK_ACTION_j, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_RSP},/* 03  SDLC, SDLC_DISC          */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
  {SLACK_ACTION_n, FSM_NOARG,    SLACK_DISCONNECTED},     /* 05  SDLC, SDLC_SNRM           */
  {SLACK_ACTION_k, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_REQ},/* 06  SDLC, SDLC_RD            */
  {SLACK_ACTION_c, FSM_NOARG,    FSM_NOCHANGE},           /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12 Network, ...PASS           */
  {SLACK_ACTION_a, FSM_NOARG,    SLACK_AWAIT_SDLC_OPEN},  /* 13  Network, ...LINKUP_REQ    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 14  Network, ...LINKUP_RSP    */
  {SLACK_ACTION_h, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_UA},/* 15  Network, ...LINKDOWN_REQ  */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
  {SLACK_ACTION_i, FSM_NOARG,    SLACK_AWAIT_ABEND_DISC}, /* 17  Network, ...ABORT_REQ,sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
  {SLACK_ACTION_i, FSM_NOARG,    SLACK_AWAIT_NORM_DISC},  /* 19  Network, ...RD_REQ        */
  {SLACK_ACTION_m, FSM_NOARG,    FSM_NOCHANGE},           /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_n, FSM_NOARG,    SLACK_AWAIT_ABORT_RSP},  /* 23  SDLC, session loss        */
  {SLACK_ACTION_h, FSM_NOARG,    SLACK_AWAIT_ABEND_DISC}, /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED}, /* 25  Network, ...TCP loss, sec */
  {SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},/* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_x, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_y, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_ae,FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_ae,FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitLinkdownRequest"
*/

const static struct fsm_state slack_await_linkdown_req[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME        */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 03  SDLC, SDLC_DISC           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 05  SDLC, SDLC_SNRM           */
  {SLACK_ACTION_k, FSM_NOARG,    FSM_NOCHANGE},           /* 06  SDLC, SDLC_RD             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
  {SLACK_ACTION_a, FSM_NOARG,    SLACK_AWAIT_SDLC_OPEN},  /* 13  Network, ...LINKUP_REQ    */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
  {SLACK_ACTION_h, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_UA},/* 15  Network, ...LINKDOWN_REQ  */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 17  Network, ...ABORT_REQ,sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_n, FSM_NOARG,    SLACK_AWAIT_ABORT_RSP},  /* 23  SDLC, session loss        */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 25  Network, ...TCP loss, sec */
  {SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_k, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitLinkdownResponse"
*/

const static struct fsm_state slack_await_linkdown_rsp[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 00  SDLC, SDLC_SFRAME         */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
  {SLACK_ACTION_j, FSM_NOARG,    FSM_NOCHANGE},           /* 03  SDLC, SDLC_DISC           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
  {SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 05  SDLC, SDLC_SNRM           */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
  {SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
  {SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
  {SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 13  Network, ...LINKUP_REQ    */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 15  Network, ...LINKDOWN_REQ  */
  {SLACK_ACTION_s, FSM_NOARG,    SLACK_DISCONNECTED},     /* 16  Network, ...LINKDOWN_RSP  */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 19  Network, ...RD_REQ        */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
  {SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
  {SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
  {SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 25  Network, ...TCP loss, sec */
  {FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 26  Network, ...TCP loss, pri */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
  {SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitLinkdownUa"
*/

const static struct fsm_state slack_await_linkdown_ua[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME         */
 { SLACK_ACTION_l, FSM_NOARG,    SLACK_DISCONNECTED},     /* 01  SDLC, SDLC_DM             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 03  SDLC, SDLC_DISC           */
 { SLACK_ACTION_l, FSM_NOARG,    SLACK_DISCONNECTED},     /* 04  SDLC, SDLC_UA             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 05  SDLC, SDLC_SNRM           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
 { SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
 { SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
 { SLACK_ACTION_a, FSM_NOARG,    SLACK_AWAIT_SDLC_OPEN},  /* 13  Network, ...LINKUP_REQ    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
 { SLACK_ACTION_h, FSM_NOARG,    FSM_NOCHANGE},           /* 15  Network, ...LINKDOWN_REQ  */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
 { SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_t, FSM_NOARG,    SLACK_DISCONNECTED},     /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_n, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 25  Network, ...TCP loss, sec */
 { SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 26  Network, ...TCP loss, pri */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitAbendDisc"
*/

const static struct fsm_state slack_await_abend_disc[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_q, FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME         */
 { SLACK_ACTION_o, FSM_NOARG,    SLACK_DISCONNECTED},     /* 01  SDLC, SDLC_DM             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
 { SLACK_ACTION_o, FSM_NOARG,    SLACK_DISCONNECTED},     /* 03  SDLC, SDLC_DISC           */
 { SLACK_ACTION_o, FSM_NOARG,    SLACK_DISCONNECTED},     /* 04  SDLC, SDLC_UA             */
 { SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 05  SDLC, SDLC_SNRM           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
 { SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
 { SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 13  Network, ...LINKUP_REQ    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 15  Network, ...LINKDOWN_REQ  */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
 { SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_t, FSM_NOARG,    SLACK_DISCONNECTED},     /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 25  Network, ...TCP loss, sec */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 26  Network, ...TCP loss, pri */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitAbortResponse"
*/

const static struct fsm_state slack_await_abort_rsp[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 00  SDLC, SDLC_SFRAME         */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 03  SDLC, SDLC_DISC           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
 { SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 05  SDLC, SDLC_SNRM           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
 { SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
 { SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
 { SLACK_ACTION_a, FSM_NOARG,    SLACK_AWAIT_SDLC_OPEN},  /* 13  Network, ...LINKUP_REQ    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 15  Network, ...LINKDOWN_REQ  */
 { SLACK_ACTION_v, FSM_NOARG,    SLACK_DISCONNECTED},     /* 16  Network, ...LINKDOWN_RSP  */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
 { SLACK_ACTION_v, FSM_NOARG,    SLACK_DISCONNECTED},     /* 18  Network, ...ABORT_RSP     */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 19  Network, ...RD_REQ        */
 { SLACK_ACTION_af,FSM_NOARG,    FSM_NOCHANGE},           /* 20  Network, ...DATA          */
 { SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_t, FSM_NOARG,    SLACK_DISCONNECTED},     /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 23  SDLC, session loss        */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 25  Network, ...TCP loss, sec */
 { SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 26  Network, ...TCP loss, pri */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
 { SLACK_ACTION_n, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


/*
**  SLACK_FSM State = "AwaitNormDisc"
*/

const static struct fsm_state slack_await_norm_disc[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_q, FSM_NOARG,    FSM_NOCHANGE},           /* 00  SDLC, SDLC_SFRAME         */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 01  SDLC, SDLC_DM             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 02  SDLC, SDLC_FRMR           */
 { SLACK_ACTION_j, FSM_NOARG,    SLACK_AWAIT_LINKDOWN_RSP},/* 03  SDLC, SDLC_DISC          */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 04  SDLC, SDLC_UA             */
 { SLACK_ACTION_r, FSM_NOARG,    SLACK_DISCONNECTED},     /* 05  SDLC, SDLC_SNRM           */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 06  SDLC, SDLC_RD             */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 07  SDLC, SDLC_IFRAME         */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 08  SDLC, SDLC_XID_TEST       */
 { SLACK_ACTION_ab,FSM_NOARG,    SLACK_DISCONNECTED},     /* 09  SDLC, SDLC_SIM            */
 { SLACK_ACTION_ab,FSM_NOARG,    FSM_NOCHANGE},           /* 10  SDLC, SDLC_RIM            */
 { SLACK_ACTION_f, FSM_NOARG,    FSM_NOCHANGE},           /* 11  SDLC, SDLC_UFRAME         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_e, FSM_NOARG,    FSM_NOCHANGE},           /* 12  Network, ...PASS          */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 13  Network, ...LINKUP_REQ    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 14  Network, ...LINKUP_RSP    */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 15  Network, ...LINKDOWN_REQ  */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 16  Network, ...LINKDOWN_RSP  */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 17  Network, ...ABORT_REQ,sec */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 18  Network, ...ABORT_RSP     */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 19  Network, ...RD_REQ        */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 20  Network, ...DATA          */
 { SLACK_ACTION_ac,FSM_NOARG,    SLACK_DISCONNECTED},     /* 21  Network, ...SIM           */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_t, FSM_NOARG,    SLACK_DISCONNECTED},     /* 22  Timer, await DISC         */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_n, FSM_NOARG,    SLACK_DISCONNECTED},     /* 23  SDLC, session loss        */
 { SLACK_ACTION_p, FSM_NOARG,    SLACK_DISCONNECTED},     /* 24  Network, ...ABORT_REQ,pri */
/* ------------------------------------------------------------------------------------- */
 { SLACK_ACTION_z, FSM_NOARG,    SLACK_DISCONNECTED},     /* 25  Network, ...TCP loss, sec */
 { FSM_NOACTION,   FSM_NOARG,    FSM_INVALID},            /* 26  Network, ...TCP loss, pri */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 27  Network, ...QUENCH ON     */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 28  Network, ...QUENCH OFF    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE},           /* 29  Network, ...LINK_ESTAB    */
 { SLACK_ACTION_r, FSM_NOARG,    FSM_NOCHANGE}            /* 30  Retransmit Timer Expired  */
/* ------------------------------------------------------------------------------------- */
};


#define SLACK_FSM_DECODE FSM_DECODE_ROUTINE(slack_fsm_decode)
/*
**  Finite State Machine table for SLACK_FSM
*/

const static struct finite_state_machine slack_fsm_table[] =
{
/*  State                       Decoder              State Value              */
 {   slack_disconnected,         SLACK_FSM_DECODE}, /* SLACK_DISCONNECTED       */
 {   slack_await_linkup_rsp,     SLACK_FSM_DECODE}, /* SLACK_AWAIT_LINKUP_RSP   */
 {   slack_await_sdlc_open,      SLACK_FSM_DECODE}, /* SLACK_AWAIT_SDLC_OPEN    */
 {   slack_active,               SLACK_FSM_DECODE}, /* SLACK_ACTIVE             */
 {   slack_await_linkdown_req,   SLACK_FSM_DECODE}, /* SLACK_AWAIT_LINKDOWN_REQ */
 {   slack_await_linkdown_rsp,   SLACK_FSM_DECODE}, /* SLACK_AWAIT_LINKDOWN_RSP */
 {   slack_await_linkdown_ua,    SLACK_FSM_DECODE}, /* SLACK_AWAIT_LINKDOWN_UA  */
 {   slack_await_abend_disc,     SLACK_FSM_DECODE}, /* SLACK_AWAIT_ABEND_DISC   */
 {   slack_await_abort_rsp,      SLACK_FSM_DECODE}, /* SLACK_AWAIT_ABORT_RSP    */
 {   slack_await_norm_disc,      SLACK_FSM_DECODE}  /* SLACK_AWAIT_NORM_DISC    */
};


int
slack_fsm_decode (stunpeertype *peer, int major, int minor)
{
    int  	index = 0;

    switch (major)
    {
	case SLACK_SDLC:
	    switch (minor)
	    {
		case SLACK_SDLC_SFRAME:
		    index = 0;		 /* SDLC side, rcv RR with P-bit*/
		    break;
		case SLACK_SDLC_DM:
		    index = 1;		 /* SDLC side, rcv DM           */
		    break;
		case SLACK_SDLC_FRMR:
		    index = 2;		 /* SDLC side, rcv FRMR		*/
		    break;
		case SLACK_SDLC_DISC:
		    index = 3;		 /* SDLC side, rcv DISC		*/
		    break;
		case SLACK_SDLC_UA:
		    index = 4;		 /* SDLC side, rcv UA		*/
		    break;
		case SLACK_SDLC_SNRM:
		    index = 5;		 /* SDLC side, rcv SNRM		*/
		    break;
		case SLACK_SDLC_RD:
		    index = 6;		 /* SDLC side, rcv RD		*/
		    break;
		case SLACK_SDLC_IFRAME:
		    index = 7;		 /* SDLC side, rcv I-frame	*/
		    break;
		case SLACK_SDLC_XID:
		    index = 8;		 /* SDLC side, rcv XID or TEST	*/
		    break;
                case SLACK_SDLC_SIM:
                    index = 9;           /* SDLC side, rcv SIM          */
                    break;
                case SLACK_SDLC_RIM:
                    index = 10;          /* SDLC side, rcv RIM          */
                    break;
                case SLACK_SDLC_UI:
                    index = 11;          /* SDLC side, rcv UI           */
                    break;
                case SLACK_SDLC_SESSION_LOSS:
                    index = 23;          /* SDLC side, session loss     */
                    break;
                default:
                    index = FSM_FAULT;
            }
            break;
        case SLACK_NETWORK:
            switch (minor)
            {
                case SLACK_STUN_OP_PASS:
                    index = 12;         /* Network side, rcv PASS   */
                    break;
                case SLACK_STUN_OP_LINKUP_REQ:
                    index = 13;         /* Network side, rcv LINKUP_REQ   */
                    break;
                case SLACK_STUN_OP_LINKUP_RSP:
                    index = 14;         /* Network side, rcv LINKUP_RSP   */
                    break;
                case SLACK_STUN_OP_LINKDOWN_REQ:
                        index = 15;     /* Network side, rcv LINKDOWN_REQ,pri */
                    break;
                case SLACK_STUN_OP_LINKDOWN_RSP:
                    index = 16;         /* Network side, rcv LINKDOWN_RSP */
                    break;
                case SLACK_STUN_OP_ABORT_REQ:
                    if (ISSSDLC(peer->stun_idb))
                        index = 17;     /* Network side, rcv ABORT_REQ,sec */
                    else
                        index = 24;     /* Network side, rcv ABORT_REQ,pri */
                    break;
                case SLACK_STUN_OP_ABORT_RSP:
                    index = 18;         /* Network side, rcv ABORT_RSP    */
                    break;
                case SLACK_STUN_OP_RD_REQ:
                    index = 19;         /* Network side, rcv RD_REQ       */
                    break;
                case SLACK_STUN_OP_DATA:
                    index = 20;         /* Network side, rcv DATA         */
                    break;
                case SLACK_NETWORK_SESSION_LOSS:
                    if (ISSSDLC(peer->stun_idb))
                        index = 25;     /* Network side, TCP loss, sec     */
                    else
                        index = 26;     /* Network side, TCP loss, pri     */
                    break;
                case SLACK_STUN_OP_QUENCH_ON:
                    index = 27;
                    break;
                case SLACK_STUN_OP_QUENCH_OFF:
                    index = 28;
                    break;
                case SLACK_STUN_OP_SIM:
                    index = 21;
                    break;
                case SLACK_STUN_OP_LINK_ESTAB:
                    index = 29;
                    break;
                default:
                    index = FSM_FAULT;
            }
            break;
        case SLACK_TIMER:
            switch (minor)
            {
                case SLACK_TIMER_DISC:
                    index = 22;         /* Timer, awaiting DISC           */
                    break;
                case SLACK_RETRY:
                    index = 30;         /* retransmit STUN protocol packet */
                    break;
                default:
                    index = FSM_FAULT;
            }
            break;
        default:
            index = FSM_FAULT;
    }

    return (index);
}

/*
**  s l a c k _ a c t i o n _ a ( )  - SLACK Action Routine A
**
**	Send a SNRM to the SDLC port.
*/
ushort
slack_action_a (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine A");
    sdb = SDB_FROM_PEER(peer);

   /*
    * Send SNRM only if the interface is up.
    */
    if (peer->stun_idb->state == IDBS_UP) {
        level = raise_interrupt_level(NETS_DISABLE);
        sdlc_setstate(peer->stun_idb, SDLC_SNRMSENT, sdb);
        if (!SLEEPING(peer->stun_idb->sdlc_top_timer)) {
	     sdlc_settimer(peer->stun_idb, 0, sdb);
        }
        reset_interrupt_level(level);
    }
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_SNRM;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ b ( )  - SLACK Action Routine B
**
**	Send a STUN_OP_LINKUP_REQ to the STUN peer.
**
**	Note: Some SNA devices sent SNRMs more than once per second.
**	      This is not enough time to establish the end-to-end
**	      connection for SDLC locally-terminated sessions.  This
**	      action routine throttles the flow of SNRMs.
*/
ushort
slack_action_b (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    paktype *pak;
    leveltype level;
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine B");
    sdb = SDB_FROM_PEER(peer);


    /** if we already got a LINKUP_RESPONSE and sent out an UA
    ** just reply with UA agian.
    **/
    if (sdb->sdlc_state == SDLC_CONNECT) {
        pak = getsdlc(peer->stun_idb, "UA");
	if(!pak) {
	    output_major = 0;
	    output_minor = 0;
	    return(STUN_OP_IGNORE);
	}
	sdlc_start(peer->stun_idb, pak, TRUE, TRUE, sdb); 
	output_major = SLACK_SDLC;
	output_minor = SLACK_SDLC_UA;
	return(STUN_OP_IGNORE);
    }

    peer->slack_retry_count = SLACK_RETRY_OFF;

    if (peer->stun_idb->sdlc_group_addr || (quick_response &&
                                            (!peer->now_xid_passthru))) {
                 
        if (peer->slack_ua_rcvd) {
	    peer->slack_ua_rcvd = FALSE;
	    output_major = SLACK_SDLC;
	    output_minor = SLACK_SDLC_UA;
	    return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
	}
	else {
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak = getsdlc(peer->stun_idb, "Send DM");
	    sdlc_sendframe(SDLC_DM, TRUE, peer->stun_idb, pak, sdb);
	    reset_interrupt_level(level);
	}
    }

    sdb->stun_init_busy = TRUE;
    if (!TIMER_RUNNING_AND_SLEEPING(sdb->time_linkup_sent))
    {
	/*
	**  This is either the first SNRM or the SNRM throttle timer
	**  has expired.  Send a SNRM and start the SNRM throttle timer.
        **  Put SDLC in DISCONNECT state
	*/

      if (sdb->sdlc_state != SDLC_DISCONNECT) {
          sdlc_setstate(peer->stun_idb, SDLC_DISCONNECT, sdb);
	}

	TIMER_START(sdb->time_linkup_sent, SLACK_SNRM_THROTTLE * ONESEC);

        peer->peers_open = 0;

	output_major = SLACK_NETWORK;
	output_minor = SLACK_STUN_OP_LINKUP_REQ;
	return(STUN_OP_LINKUP_REQ);
    }
    else
    {
	/*
	**  We've received a SNRM from the attached SDLC device but the
	**  SNRM throttle timer hasn't expired.  Just eat it.
	*/
	output_major = 0;
	output_minor = 0;
	return(STUN_OP_IGNORE);
    }
}


/*
**  s l a c k _ a c t i o n _ c ( )  - SLACK Action Routine C
**
**	Send a STUN_OP_DATA to the STUN peer.
*/
ushort slack_action_c (stunpeertype *peer)
{
    sdlc_data_block     *sdb;
    paktype  *pak;

    sdb = SDB_FROM_PEER(peer);
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine C");

    if (ISPSDLC(peer->stun_idb) && (sdb->stun_init_busy == TRUE)) {
	/*
	 * corner case for if endstation sends us I-frame before we come
	 * out of initialisation busy state .
	 */
	if(slack_debug)
	    buginf("\nReceived I-frame before RR(P) - Frame discarded\n");
	output_major = 0;
	output_minor = 0;
	return(STUN_OP_IGNORE);
    }
    peer->slack_stats.i_frames_rcvd++;
    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    if (ISSSDLC(peer->stun_idb) && (sdb->stun_init_busy == TRUE)) {
        if((pak = getsdlc(peer->stun_idb, "LINK_ESTAB"))) {
	    stun_to_network(peer->stun_idb, pak, peer,
			    (char *)&(peer->addr[0]), STUN_OP_LINK_ESTAB);
	    sdb->stun_init_busy = FALSE;
	    peer->slack_retry_count = SLACK_RETRY_ON;
	}
    }
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_DATA;
    
    return(STUN_OP_DATA | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ d ( )  - SLACK Action Routine D
**
**	Send a STUN_OP_LINKUP_RSP to the STUN peer.
*/
ushort
slack_action_d (stunpeertype *peer)
{
    sdlc_data_block  *sdb;
    leveltype level;
 
#ifdef WAYNES_WORLD
    /*
    **  Need to get this working by Release 9.1 FCS.
    **				Wayne Clark, 08 May 92
    */
    /*
    **  Now that we've received a UA to our prior SNRM, we know that the
    **  SDLC link station in this router (i.e.  this stunpeertype) is a
    **  primary link station.
    */
    peer->stun_idb->stun_sdlc_encap = ET_SDLCP;
#endif

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine D");
 
    if (peer->tg_active)
       peer->local_ack_tg_load = FALSE;

    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * make sure the peer comes up in busy state and not
     * accept any I-frames immediately.
     * The flag is either reset by a DATA or LINK_ESTAB message.
     *
     */
    sdb->stun_init_busy = TRUE;
    reset_interrupt_level(level);

    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_LINKUP_RSP;
    return(STUN_OP_LINKUP_RSP | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ e ( )  - SLACK Action Routine E
**
**	Send a XID or TEST (whichever is present) to the SDLC port.
*/
ushort
slack_action_e (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine E");

    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_PASS;
    /*
    **  ++++ I don't believe this return code is correct. +++
    */
    return(STUN_OP_PASS);
}


/*
**  s l a c k _ a c t i o n _ f ( )  - SLACK Action Routine F
**
**	Send a STUN_OP_PASS to the STUN peer.
*/
ushort
slack_action_f (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine F");
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_PASS;
    return(STUN_OP_PASS);
}

/*
**  s l a c k _ a c t i o n _ f n ( )  - SLACK Action Routine FN
**   
**      XID received on the SDLC side in DISCONNECTED state. 
**	Will handle the case of quick_response. 
*/
ushort
slack_action_fn (stunpeertype *peer)
{
    sdlc_data_block  *sdb;
    paktype *pak;
    leveltype level; 

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine F");
    sdb = SDB_FROM_PEER(peer);

    if ((!quick_response) ||
        ( peer->now_xid_passthru))  { 
 
        output_major = SLACK_NETWORK;
        output_minor = SLACK_STUN_OP_PASS;
        return(STUN_OP_PASS);
    }
    else { 
      if (peer->slack_xid_rsp_received) { 
           /* send the stored value of XID response */
           level = raise_interrupt_level(NETS_DISABLE);
           sdlc_sendframe(SDLC_XID,TRUE,peer->stun_idb,peer->stored_xid_response,sdb);
           peer->now_xid_passthru= TRUE; /* here onwards behave normally */
           peer->slack_xid_rsp_received = FALSE;
	   reset_interrupt_level(level);
	}
        else { 
            level = raise_interrupt_level(NETS_DISABLE);
	    pak = getsdlc(peer->stun_idb, "Send DM");
	    sdlc_sendframe(SDLC_DM, TRUE, peer->stun_idb, pak, sdb);
	    reset_interrupt_level(level);
        
	    if (!TIMER_RUNNING_AND_SLEEPING(sdb->time_linkup_sent))
	      {
		/*
		 **  This is either the first XID  or the XID throttle timer
		 **  has expired.  Send XID and start the XID throttle timer.
		 */
		TIMER_START(sdb->time_linkup_sent, SLACK_SNRM_THROTTLE * ONESEC);
		output_major = SLACK_NETWORK;
		output_minor = SLACK_STUN_OP_LINKUP_REQ;/* hope these values won't create trouble*/
		return(STUN_OP_PASS);
	      }
	}

        output_major = 0;
        output_minor = 0;
        return(STUN_OP_IGNORE);

    }
}
/*
**  s l a c k _ a c t i o n _ g ( )  - SLACK Action Routine G
**
**	Send a SNRM to the SDLC code, so it can reply with a UA.
*/
ushort
slack_action_g (stunpeertype *peer)
{
    leveltype	level;
    paktype	*pak;
    sdlc_data_block	*sdb;
    sdb = SDB_FROM_PEER(peer);

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine G");


    /*
     * If priority is turned on we need to wait till all the priority
     * peers are up.
     */
    if ((!peer->priority) ||
        (peer->priority && (peer->peers_open == STUN_PRIORITY_COUNT))) {

        peer->peers_open = 0;

        /*
        **  Take the SNRM that has been held on the stunpeertype and
        **  present it to the SDLC level.
        */
        level = raise_interrupt_level(NETS_DISABLE);

        if (peer->tg_active)
           peer->local_ack_tg_load = FALSE;

        if ((peer->stun_idb->sdlc_group_addr) || (quick_response && 
                                             (!peer->now_xid_passthru)))
	    peer->slack_ua_rcvd = TRUE;

        if (peer->slack_snrm_pak) 
        {
	    sdb = SDB_FROM_PEER(peer);
            pak = peer->slack_snrm_pak;
	    peer->slack_snrm_pak = NULL;
	    sdlc_input(peer->stun_idb, pak);
	    /* give the primary sdlc station a chance to get the UA */
	    TIMER_START(sdb->time_linkup_sent, SLACK_SNRM_THROTTLE * ONESEC);
        } 

	reset_interrupt_level(level);

#ifdef WAYNES_WORLD
    /*
    **  Need to get this working by Release 9.1 FCS.
    **				Wayne Clark, 08 May 92
    */
    /*
    **  Now that we've given SDLC the SNRM we've been holding onto,
    **  we know that the SDLC link station in this router (i.e.
    **  this stunpeertype) is a secondary link station.
    */
    peer->stun_idb->stun_sdlc_encap = ET_SDLCS;
#endif
    }
    else
        peer->peers_open++;
    /* 
     * If we are Secondary(adjacent to the FEP) then set init_busy as we need
     * to  send remote STUN_OP_LINK_ESTAB to take it out of busy
     * on receipt of rr(p) from the FEP  .
     */
    if(ISSSDLC(peer->stun_idb)) {
	if(slack_debug)
	    buginf("\nSLACK:Action G - setting stun busy  \n");

	sdb->stun_init_busy = TRUE;   
    }
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_UA;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ h ( )  - SLACK Action Routine H
**
**	Send a DISC to the SDLC port.
*/
ushort
slack_action_h (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype           level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine H");
    sdb = SDB_FROM_PEER(peer);
    /* for the primary sdlc router, the is the same as presenting
       a received RD to sdlc, forcing out the DISC */
    level = raise_interrupt_level(NETS_DISABLE);
    if (sdb->sdlc_state != SDLC_DISCSENT) {
	/* if sdlc is already in disconnected state, don't force the DISC */
	if (sdb->sdlc_state != SDLC_DISCONNECT) {
	    sdlc_setstate(peer->stun_idb, SDLC_DISCSENT, sdb);
	    if (!SLEEPING(peer->stun_idb->sdlc_top_timer))
		sdlc_settimer(peer->stun_idb, 0, sdb);
	}
    }
    peer->slack_retry_count = SLACK_RETRY_OFF;
    reset_interrupt_level(level);
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_DISC;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ i ( )  - SLACK Action Routine I
**
**	Send a RD to the SDLC port. 
*/
ushort
slack_action_i (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype		level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine I");
    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
   /*
    *  send out an RD when it is polled.
    */
    sdlc_setstate(peer->stun_idb, SDLC_DISCSENT, sdb);

    peer->slack_retry_count = SLACK_RETRY_OFF;
    reset_interrupt_level(level);
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_RD;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ j ( )  - SLACK Action Routine J
**
**	Send a STUN_OP_LINKDOWN_REQ to the STUN peer.
*/
ushort
slack_action_j (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine J");
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_LINKDOWN_REQ;
    /* don't present the DISC to sdlc, wait until a STUN_OP_LINKDOWN_RSP   
       has been received, then send UA */
    return(STUN_OP_LINKDOWN_REQ);
}

/*
**  s l a c k _ a c t i o n _ k ( )  - SLACK Action Routine K
**
**	Send a STUN_OP_RD_REQ to the STUN peer.
*/
ushort
slack_action_k (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine K");
    if (peer->slack_retry_count == SLACK_RETRY_OFF)
	peer->slack_retry_count = SLACK_RETRY_ON;
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_RD_REQ;
    /* don't present the RD to sdlc until OP_LINKDOWN_REQ is received */
    return(STUN_OP_RD_REQ);
}

/*
**  s l a c k _ a c t i o n _ l ( )  - SLACK Action Routine L
**
**	Send a STUN_OP_LINKDOWN_RSP to the STUN peer.
*/
ushort
slack_action_l (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine L");
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_LINKDOWN_RSP;
    return(STUN_OP_LINKDOWN_RSP | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ m ( )  - SLACK Action Routine M
**
**	Enqueue an I-frame onto the sdlc_holdq.
*/
ushort
slack_action_m (stunpeertype *peer)
{
    sdlc_data_block *sdb;
    paktype   *pak;
    leveltype		level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine M");

    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;
    peer->slack_stats.i_frames_sent++;

   /*
    * Check to see if the sdlc is getting full,
    * If yes, send QUENCH_ON message to remote peer.
    */
    if (((100 * sdb->sdlc_holdq.count)/sdb->sdlc_holdq.maximum) >=
	SDLC_TXQ_HIWATER) {
        if((pak = getsdlc(peer->stun_idb, "QUENCH ON"))) {
	    stun_to_network(peer->stun_idb, pak, peer,
			    (char *)&(peer->addr[0]),
			    STUN_OP_QUENCH_ON);
	    sdb->stun_local_busy = TRUE;
        }
    }
    if (sdb->stun_init_busy == TRUE) {
        sdb->stun_init_busy = FALSE;
        sdlc_set_clear_busy(peer->stun_idb, (unsigned)sdb->sdlc_address, FALSE);
    }
    reset_interrupt_level(level);

    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_IFRAME;

    return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ n ( )  - SLACK Action Routine N
**
**	Send a STUN_OP_ABORT_REQ to the STUN peer.
*/
ushort
slack_action_n (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    paktype	*pak;
    leveltype level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine N");

    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    /*
    **  Get a packet for the STUN_OP_ABORT_REQ.
    */
    pak = getsdlc(peer->stun_idb, "Abort Req");
    if (!pak)
    {
	output_major = 0;
	output_minor = 0;
	return(STUN_OP_IGNORE);
    }

    /*
    **  Force SDLC into disconnect state.
    */
    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
    sdlc_setstate(peer->stun_idb, SDLC_DISCONNECT, sdb);

    /*
    **  Send the STUN_OP_ABORT_REQ.
    */
    stun_to_network(peer->stun_idb, pak, peer, (char *) &(peer->addr[0]),
		    STUN_OP_ABORT_REQ);
    if (peer->slack_retry_count == SLACK_RETRY_OFF)
	peer->slack_retry_count = SLACK_RETRY_ON;
    reset_interrupt_level(level);
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_ABORT_REQ;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ o ( )  - SLACK Action Routine O
**
**	Send a UA to the SDLC port.
**	Send a STUN_OP_ABORT_RSP to the STUN peer.
*/
ushort
slack_action_o (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine O");
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_UA;
#ifdef NOTUSED
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_ABORT_RSP;
#endif
    return(STUN_OP_ABORT_RSP | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ p ( )  - SLACK Action Routine P
**
**      Send a STUN_OP_ABORT_RSP to the STUN peer.
*/
ushort
slack_action_p (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype           level;
    paktype             *pak;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine P");
    /*
    **  Force SDLC into disconnect state.
    */

    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);

    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    if (sdb->sdlc_state != SDLC_DISCSENT) {
	/* if sdlc is already in disconnected state, don't force the DISC */
	if (sdb->sdlc_state != SDLC_DISCONNECT) {
	    sdlc_setstate(peer->stun_idb, SDLC_DISCSENT, sdb);
	    if (!SLEEPING(peer->stun_idb->sdlc_top_timer))
		sdlc_settimer(peer->stun_idb, 0, sdb);
	} 
    }

    peer->slack_retry_count = SLACK_RETRY_OFF;
    reset_interrupt_level(level);

    /*
    **  Get a packet for the STUN_OP_ABORT_RSP.
    */
    pak = getsdlc(peer->stun_idb, "Abort Rsp");
    if (!pak)
    {
        output_major = 0;
        output_minor = 0;
        return(STUN_OP_IGNORE);
    }

    stun_to_network(peer->stun_idb, pak, peer, (char *) &(peer->addr[0]),
                    STUN_OP_ABORT_RSP);

    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_ABORT_RSP;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ q ( )  - SLACK Action Routine Q
**
**	Merely send this S-frame to SDLC.
*/
ushort
slack_action_q (stunpeertype *peer)
{
    int         current_len, i, peer_lo_water;
    char	debug_output[5];
    stunpeertype *current_peer;
    sdlc_data_block	*sdb;
    paktype    *pak;

#ifdef NOTUSED
    /*
    **  This buginf() creates a lot of output ... leave it disabled.
    */
    buginf("\nSLACK: Action Routine Q");
#endif
    sdb = SDB_FROM_PEER(peer);
    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    /*
    **  SDLC Flow Control
    **  -----------------
    **  If the remote sdlc is not busy, 
    **
    **  If we are in a flow control state, see if we have drained the TCP/IP
    **  queues below the low water mark.  If multiple TCP connections
    **  are opened for prioritization purposes, we must make sure
    **  all TCP queues for this stun connection are below the lo water
    **  mark before we do the set clear busy.
    **
    */
    if ((peer->slack_flow_control)  && (sdb->stun_peer_busy == FALSE)) {
        current_len = reg_invoke_ip_tcpdriver_oqlen(peer->primary_t);
        peer_lo_water = (peer->user_def_max_tcp_qlen*SLACK_LO_PERCENT) / 100;
	
	/* check to make sure none of the TCP queues are getting full */
        if ((peer->priority) && (current_len <= peer_lo_water)) {
	    for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
		current_peer = peer->priority_peers[i]; 
		current_len = reg_invoke_ip_tcpdriver_oqlen(
						      current_peer->primary_t);
                if (current_len >= peer_lo_water)
		    break;
	    }
        }
        if (current_len <= peer_lo_water) {
             sdb->stun_peer_busy = FALSE;
	     strcpy (debug_output, "RR");
             sdlc_set_clear_busy(peer->stun_idb, (unsigned)sdb->sdlc_address,
				 FALSE);
             peer->slack_flow_control = FALSE;
	} else  {
	    sprintf(debug_output, "%d", current_len);
	}

	if (slack_debug & SLACK_DBG_FLOW_CONTROL) {
	    buginf(".%s", debug_output);
	}
    }
    /*
     * Check to see if local sdlc_holdq was full. If yes,
     * check to see if sdlc_holdq can accecpt more data.
     * If yes, tell remote peer, we can now accept more data.
     */
     if (sdb->stun_local_busy) {
         if (((100 * sdb->sdlc_holdq.count)/sdb->sdlc_holdq.maximum) <=
                SDLC_TXQ_LOWATER) {
            if((pak = getsdlc(peer->stun_idb, "QUENCH OFF"))) {
		 stun_to_network(peer->stun_idb, pak, peer,
				 (char *)&(peer->addr[0]), STUN_OP_QUENCH_OFF);
		 sdb->stun_local_busy = FALSE;
            } 
	}
     } 
    if (ISSSDLC(peer->stun_idb) && (sdb->stun_init_busy == TRUE)) {
        if((pak = getsdlc(peer->stun_idb, "LINK_ESTAB"))) {
	    stun_to_network(peer->stun_idb, pak, peer,
			    (char *)&(peer->addr[0]), STUN_OP_LINK_ESTAB);
	    sdb->stun_init_busy = FALSE;
	    peer->slack_retry_count = SLACK_RETRY_ON;
	}
    }

    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_SFRAME;
    return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ r ( )  - SLACK Action Routine R
**
**	Do nothing (this includes not sending any STUN_OP_xxx).
*/
ushort
slack_action_r (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine R");
    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    output_major = 0;
    output_minor = 0;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ s ( )  - SLACK Action Routine S
**
**	Send a UA to the SDLC port.
*/
ushort
slack_action_s (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype		level;
    paktype		*pak;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine S");
    peer->slack_retry_count = SLACK_RETRY_OFF;
    pak = getsdlc(peer->stun_idb, "LinkDown");
    if (!pak)
    {
	output_major = 0;
	output_minor = 0;
	return(STUN_OP_IGNORE);
    }
    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
    sdlc_sendframe(SDLC_UA, TRUE, peer->stun_idb, pak, sdb);
    sdlc_setstate(peer->stun_idb, SDLC_DISCONNECT, sdb);
    reset_interrupt_level(level);

    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_UA;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ t ( )  - SLACK Action Routine T
**
**	Reset the SDLC link.
*/
ushort
slack_action_t (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    hwidbtype		*idb;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine T");
    sdb = SDB_FROM_PEER(peer);
    idb = peer->stun_idb;
/*
    sdlc_reset(idb, "SLACK hung awaiting Disconnect", sdb);
*/
   /* NASDAQ
    * assume TCP dropped a STUN protocol packet.
    * bring down the tcp connection to make sure
    * both sides are in sync.
    */
    if (peer->abort)
	(*peer->abort)(peer);
 
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = 0;
    output_minor = 0;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ u ( )  - SLACK Action Routine U
**
**	Received a FRMR - reset the SDLC link.
*/
ushort
slack_action_u (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine U");
 
    if (peer->slack_retry_count == SLACK_RETRY_OFF)
        peer->slack_retry_count = SLACK_RETRY_ON;
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_ABORT_REQ;
    return(STUN_OP_ABORT_REQ | STUN_OP_TO_SDLC);
}

/*
**  s l a c k _ a c t i o n _ v ( )  - SLACK Action Routine V
**
**	Reset the SDLC link.
*/
ushort
slack_action_v (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    hwidbtype		*idb;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine V");
    sdb = SDB_FROM_PEER(peer);
    idb = peer->stun_idb;
    sdlc_setstate(peer->stun_idb, SDLC_DISCONNECT, sdb);
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = 0;
    output_minor = 0;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ w ( )  - SLACK Action Routine W
**
**	Received a SNRM while in Active state, Send a STUN_OP_ABORT_REQ to
**      the STUN peer.
*/
ushort
slack_action_w (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    paktype	*pak;
    leveltype   level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine W");

    peer->slack_retry_count = SLACK_RETRY_OFF;
    /*
    **  If we're still sleeping on the SNRM throttle timer, then merely
    **  ignore this SNRM.
    */
    sdb = SDB_FROM_PEER(peer);
    if (SLEEPING(sdb->time_linkup_sent))
	return (STUN_OP_IGNORE);

    /*
    **  Get a packet for the STUN_OP_ABORT_REQ.
    */
    pak = getsdlc(peer->stun_idb, "Abort Req");
    if (!pak)
    {
	output_major = 0;
	output_minor = 0;
	return(STUN_OP_IGNORE);
    }

    /*
    **  Force SDLC into disconnect state.
    */
    sdb = SDB_FROM_PEER(peer);
    level = raise_interrupt_level(NETS_DISABLE);
    if (sdb->sdlc_state != SDLC_DISCSENT) {
	sdlc_setstate(peer->stun_idb, SDLC_DISCSENT, sdb);
        if (!SLEEPING(peer->stun_idb->sdlc_top_timer))
	    sdlc_settimer(peer->stun_idb, 0, sdb);
    }
    reset_interrupt_level(level);

    /*
    **  Send the STUN_OP_ABORT_REQ.
    */
    stun_to_network(peer->stun_idb, pak, peer, (char *) &(peer->addr[0]),
		    STUN_OP_ABORT_REQ);
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_ABORT_REQ;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ x ( )  - SLACK Action Routine X
**
**      Received a QUENCH_ON from remote peer
**      
*/
ushort
slack_action_x (stunpeertype *peer)
{
    sdlc_data_block	*sdb;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine X");

    sdb = SDB_FROM_PEER(peer);

    sdb->stun_peer_busy = TRUE;
    sdlc_set_clear_busy(peer->stun_idb, (unsigned) sdb->sdlc_address, TRUE);
    output_major = SLACK_NETWORK;
    output_major = SLACK_STUN_OP_QUENCH_ON;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ y ( )  - SLACK Action Routine Y
**
**      Received a QUENCH_OFF from remote peer
**      
*/
ushort
slack_action_y (stunpeertype *peer)
{
    sdlc_data_block	*sdb;
    leveltype		level;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
	buginf("\nSLACK: Action Routine Y");

    sdb = SDB_FROM_PEER(peer);

    level = raise_interrupt_level(NETS_DISABLE);
    sdb->stun_peer_busy = FALSE;
    sdlc_set_clear_busy(peer->stun_idb, (unsigned)sdb->sdlc_address, FALSE);
    reset_interrupt_level(level);
 
    output_major = SLACK_NETWORK;
    output_major = SLACK_STUN_OP_QUENCH_OFF;
    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ z ( )  - SLACK Action Routine Z
**
**      The TCP session has gone down, disconnect the SDLC session.
*/
ushort
slack_action_z (stunpeertype *peer)
{
    sdlc_data_block     *sdb;
    leveltype           level;
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine Z");
    sdb = SDB_FROM_PEER(peer);
    /* for the primary sdlc router, the is the same as presenting
       a received RD to sdlc, forcing out the DISC */
    level = raise_interrupt_level(NETS_DISABLE);
    peer->now_xid_passthru = FALSE;
    peer->slack_xid_rsp_received=FALSE;

    if (ISSSDLC(peer->stun_idb)) {
        if (sdb->sdlc_state != SDLC_DISCONNECT)
            sdlc_setstate(peer->stun_idb, SDLC_DISCSENT,sdb);
    } else {
	if (sdb->sdlc_state != SDLC_DISCSENT) {
	    /* if sdlc is already in disconnected state, don't force the DISC */
	    if (sdb->sdlc_state != SDLC_DISCONNECT) {
		sdlc_setstate(peer->stun_idb, SDLC_DISCSENT, sdb);
		if (!SLEEPING(peer->stun_idb->sdlc_top_timer))
		    sdlc_settimer(peer->stun_idb, 0, sdb);
	    }
	}
    }
    reset_interrupt_level(level);
 
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_SDLC;
    output_minor = SLACK_SDLC_DISC;
    return(STUN_OP_IGNORE);
}
 
/*
**  s l a c k _ a c t i o n _ aa ( )  - SLACK Action Routine AA
**
**      Simply pass the packet to SDLC.
*/
ushort
slack_action_aa (stunpeertype *peer)
{
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AA");
    output_major = SLACK_SDLC;
    output_minor = 0;
    return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
}
 
/*
**  s l a c k _ a c t i o n _ ab ( )  - SLACK Action Routine AB
**
**      Send a STUN_OP_SIM to the STUN peer, if secondary.
**      Send a STUN_OP_PASS to the STUN peer, if primary.
**
*/
ushort
slack_action_ab (stunpeertype *peer)
{
    sdlc_data_block *sdb;
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AB");
 
    peer->slack_retry_count = SLACK_RETRY_OFF;
    output_major = SLACK_NETWORK;
    /*
     * Sending NCP is always Primary, since the NCPs have not yet
     * negotiated their sdlc roles. Router should behave as secondary.
     */
    if ((ISSSDLC(peer->stun_idb)) || (peer->tg_active)) {
        sdb = SDB_FROM_PEER(peer);
        if (sdb->sdlc_state != SDLC_DISCONNECT)
            sdlc_reset(peer->stun_idb, "Set Initialization Mode", sdb);
        output_minor = SLACK_STUN_OP_SIM;
        return(STUN_OP_SIM);
    } else {
        output_minor = SLACK_STUN_OP_PASS;
        return(STUN_OP_PASS);
    }
}

/*
**  s l a c k _ a c t i o n _ ac ( )  - SLACK Action Routine AC
**
**      Received  a STUN_OP_SIM from the STUN peer.
**
*/
ushort
slack_action_ac (stunpeertype *peer)
{
    sdlc_data_block *sdb;
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AC");
 
    peer->slack_retry_count = SLACK_RETRY_OFF;
    sdb = SDB_FROM_PEER(peer);
    if (sdb->sdlc_state != SDLC_DISCONNECT)
	sdlc_reset(peer->stun_idb, "Set Initialization Mode", sdb);
    if (peer->tg_active)
       peer->local_ack_tg_load = TRUE;
    peer->local_ack_sim_sent = TRUE;
    output_major = SLACK_NETWORK;
    output_minor = SLACK_STUN_OP_PASS;
    return(STUN_OP_PASS);
}
 
/*
**  s l a c k _ a c t i o n _ ad ( )  - SLACK Action Routine AD
**
**      Got a UA from sdlc.
**      Simply pass the packet to SDLC.
**      OR if we are in sim_sent state, pass it to STUN peer.
*/
ushort
slack_action_ad (stunpeertype *peer)
{
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AD");
 
    peer->slack_retry_count = SLACK_RETRY_OFF;
    if (peer->local_ack_sim_sent) {
        peer->local_ack_sim_sent = FALSE;
        output_major = SLACK_NETWORK;
        output_minor = SLACK_STUN_OP_PASS;
        return(STUN_OP_PASS);
    } else {
       output_major = SLACK_SDLC;
       output_minor = 0;
       return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
    }
}
 
/*
**  s l a c k _ a c t i o n _ ae ( )  - SLACK Action Routine AE
**
**      Received a LINK_ESTAB message.
**      If secondary, stop retransmission of the LINK_ESTAB.
**      If primary, respond with a LINK_ESTAB.
*/
ushort
slack_action_ae (stunpeertype *peer)
{
 
    sdlc_data_block     *sdb;
    paktype *pak;
 
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AE");
 
    sdb = SDB_FROM_PEER(peer);
 
    if (ISPSDLC(peer->stun_idb)) {
        if((pak = getsdlc(peer->stun_idb, "LINK_ESTAB"))) {
	    stun_to_network(peer->stun_idb, pak, peer,
			    (char *)&(peer->addr[0]), STUN_OP_LINK_ESTAB);
	    sdb->stun_init_busy = FALSE;
	    sdlc_set_clear_busy(peer->stun_idb, (unsigned)sdb->sdlc_address, FALSE);
	}
    }
    peer->slack_retry_count = SLACK_RETRY_OFF;
 
    return(STUN_OP_IGNORE);
}
 
/*
**  s l a c k _ a c t i o n _ af ( )  - SLACK Action Routine AF
**
**      If this peer is part of a transmission group reroute the
**      I-frame to the sdlc_holdq of another active peer in the
**      transmission group.
*/
ushort
slack_action_af (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AF");

    peer->slack_retry_count = SLACK_RETRY_OFF;

    if (peer->tg_active) {
       if (stun_tg_debug)
          buginf("\naction_af: returning OP_REROUTE");
       return (STUN_OP_REROUTE);
    }

    return(STUN_OP_IGNORE);
}

/*
**  s l a c k _ a c t i o n _ ag ( )  - SLACK Action Routine AG
**
**      Send a STUN_OP_PASS to the STUN peer if TG active.
**      Used for remote loading NCP's before Normal Response Mode.
*/
ushort
slack_action_ag (stunpeertype *peer)
{
    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AG");

    peer->slack_retry_count = SLACK_RETRY_OFF;

    if (peer->local_ack_tg_load) {
       output_major = SLACK_NETWORK;
       output_minor = SLACK_STUN_OP_PASS;
       return(STUN_OP_PASS);
    }
    else
       return(STUN_OP_IGNORE | STUN_OP_TO_SDLC);
}


/*
**  s l a c k _ a c t i o n _ ah ( )  - SLACK Action Routine AH
**
**      If we receive a Network Session Loss when we are in
**      AwaitLinkUpRsp state we need to reset the ua_rcvd flag.
*/
ushort
slack_action_ah (stunpeertype *peer)
{
    sdlc_data_block     *sdb;

    if (slack_debug & SLACK_DBG_FSM_ACTION)
        buginf("\nSLACK: Action Routine AH");

    peer->slack_retry_count = SLACK_RETRY_OFF;

    peer->slack_ua_rcvd = FALSE;
    sdb = SDB_FROM_PEER(peer);
    TIMER_STOP(sdb->time_linkup_sent);

    output_major = 0;
    output_minor = 0;
    return(STUN_OP_IGNORE);
}


/*
**  s l a c k _ f s m ( ) 
*/
ushort
slack_fsm (stunpeertype *peer, slackmajor major, int minor)
{
    int     		fsm_rc;
    int     		old_value;		
    char		debug_prefix[20];
    char		*old_state, *new_state, *current_input, *output;

    /*
    **  Execute the SDLC Local Termination Finite State Machine.
    */
    old_value = peer->slack_state;
    fsm_rc = (int) fsm_execute("SLACK", slack_fsm_table,
			       ((int *) &(peer->slack_state)), SLACK_FSM_MAX,
			       peer, major, minor, FALSE);

    /*
     * If going into shutdown mode, start the dead timer. This will kick us out
     * of shutdown mode some time later.
     */
    if ((peer->slack_state == SLACK_AWAIT_LINKDOWN_UA) ||
	(peer->slack_state == SLACK_AWAIT_ABORT_RSP)   ||
	(peer->slack_state == SLACK_AWAIT_ABEND_DISC)  ||
	(peer->slack_state == SLACK_AWAIT_NORM_DISC)) {
        TIMER_START(peer->dead_timer, stun_slack_fsm_timeout * ONESEC);
    } else {
	TIMER_STOP(peer->dead_timer);
    }

    if (fsm_rc < 0) {
	sprintf(debug_prefix, "SLACK (Serial%d)", peer->stun_idb->unit);
	switch (fsm_rc) {
	    case FSM_RC_INVALID_INPUT:
		current_input = slack_prinput(major, minor);
		old_state     = slack_prstate(old_value);
		if (slack_debug) {
		    buginf("\n%s: Invalid Input: state = %s, ", 
			   debug_prefix, old_state);
		    buginf("major, minor = %s \n",
			   current_input);
                } 
		break;
	    case FSM_RC_ERROR_INPUT:
		current_input = slack_prinput(major, minor);
		old_state     = slack_prstate(old_value);
		if (slack_debug) {
		    buginf("\n%s: Invalid Input: state = %s, ", 
			   debug_prefix, old_state);
		    buginf("major, minor = %s \n",
			   current_input);
                }
		break;
	    case FSM_RC_UNKNOWN_STATE:
		if (slack_debug) {
		    buginf("\n%s: Current \"state\" is invalid: state = 0x%x", 
			   debug_prefix, peer->slack_state);
                }
		break;
	    case FSM_RC_UNKNOWN_INPUT:
		old_state = slack_prstate(old_value);
		if (slack_debug) {
		    buginf("\n%s: Unknown Input: state = %s, ",
			   debug_prefix, old_state);
		    buginf("major = 0x%08x, minor = 0x%08x\n",
			   major, minor);
                }
		break;
	}
        return(STUN_OP_ERR);
    }
    if (slack_debug & SLACK_DBG_FSM_ALL) {
	sprintf (debug_prefix, "SLACK (Serial%d)", peer->stun_idb->unit);
	if (((slack_debug & SLACK_DBG_FSM_SFRAME) == 0)	&&
	    (((major == SLACK_SDLC)		&&
	     (minor == SLACK_SDLC_SFRAME))		||
	    ((output_major == SLACK_SDLC)	&&
	     (output_minor == SLACK_SDLC_SFRAME)))) {
	    /*  S-Frame Sent or Received - eat it. */
	} else if (((slack_debug & SLACK_DBG_FSM_IFRAME) == 0)	&&
		   (((major == SLACK_SDLC)			&&
		     (minor == SLACK_SDLC_IFRAME))			||
		    ((major == SLACK_NETWORK)		&&
		     (minor == SLACK_STUN_OP_DATA)))) {
	    /*  I-Frame Received, SDLC or Network - eat it. */
	} else {
	    /*  Debug slack_fsm input event and state transition. */
	    current_input = slack_prinput(major, minor);
	    old_state     = slack_prstate(old_value);
	    new_state     = slack_prstate(peer->slack_state);
	    buginf("\n%s: Input     = %s\n", debug_prefix, current_input);
	    buginf("%s: Old State = %23s  New State = %23s \n",
		   debug_prefix, old_state, new_state);
	    if (output_major != 0) {
		output = slack_prinput(output_major, output_minor);
		buginf("%s: Output    = %s\n", debug_prefix, output);
	    }
	}
    }

    /*
    **  Accumulate connection/disconnect statistics.
    */
    if (old_value != peer->slack_state) {
	if (peer->slack_state == SLACK_ACTIVE) {
	    peer->slack_stats.num_connects++;
	} else if (peer->slack_state == SLACK_DISCONNECTED) {
            peer->slack_stats.num_disconnects++;
            peer->slack_retry_count = SLACK_RETRY_OFF;
        }
    }

    return(fsm_rc);
}

/*
** s l a c k _ s d l c _ c a t e g o r i z e ( )
*/
slackminor_sdlc slack_sdlc_categorize (paktype *pak, stunpeertype *peer)
{
    sdlc_frametype *sdlc = (sdlc_frametype *) pak->datagramstart;
    uchar conch;			/* Control field */

    if (SDLC_ISIFRAME(sdlc)) {
	return(SLACK_SDLC_IFRAME);
    } else if (SDLC_ISSFRAME(sdlc)) {
	return(SLACK_SDLC_SFRAME);
    }  else {
	/* We assume a U-frame here... */
	conch = sdlc->control & ~SDLC_PF; /* Remove p/f bit... */
	switch (conch) {
	  case SDLC_SNRM:
	    return(SLACK_SDLC_SNRM);
	  case SDLC_XID:
	    return(SLACK_SDLC_XID);
	  case SDLC_TEST:
	    return(SLACK_SDLC_TEST);
	  case SDLC_DISC: /* and SDLC_RD */
	    if (peer->stun_idb->stun_sdlc_encap == ET_SDLCP)
		return(SLACK_SDLC_RD);
	    else
		return(SLACK_SDLC_DISC);
	  case SDLC_UA:
	    return(SLACK_SDLC_UA);
	  case SDLC_FRMR:
	    return(SLACK_SDLC_FRMR);
	  case SDLC_DM:
	    return(SLACK_SDLC_DM);
          case SDLC_SIM:
	    if(ISSSDLC(peer->stun_idb))
		return(SLACK_SDLC_SIM);
	    else
		return(SLACK_SDLC_RIM);
          case SDLC_UFRAME:
	    return(SLACK_SDLC_UI);
	  default:
	    buginf ("\nSLACK: Unrecognized SDLC Control Frame = 0x%02x\n", 
		  conch);
	      return(0);
	}
    }
}

/*
** s l a c k _ n e t w o r k  _ c a t e g o r i z e ( )
*/
slackminor_network slack_network_categorize (int opcode)
{
    switch (opcode) {
      case STUN_OP_PASS:
	return(SLACK_STUN_OP_PASS);
      case STUN_OP_LINKUP_REQ:
	return(SLACK_STUN_OP_LINKUP_REQ);
      case STUN_OP_LINKUP_RSP:
	return(SLACK_STUN_OP_LINKUP_RSP);
      case STUN_OP_LINKDOWN_REQ:
	return(SLACK_STUN_OP_LINKDOWN_REQ);
      case STUN_OP_LINKDOWN_RSP:
	return(SLACK_STUN_OP_LINKDOWN_RSP);
      case STUN_OP_ABORT_REQ:
	return(SLACK_STUN_OP_ABORT_REQ);
      case STUN_OP_ABORT_RSP:
	return(SLACK_STUN_OP_ABORT_RSP);
      case STUN_OP_RD_REQ:
	return(SLACK_STUN_OP_RD_REQ);
      case STUN_OP_DATA:
	return(SLACK_STUN_OP_DATA);
      case STUN_OP_QUENCH_ON:
	return(SLACK_STUN_OP_QUENCH_ON);
      case STUN_OP_QUENCH_OFF:
	return(SLACK_STUN_OP_QUENCH_OFF);
      case STUN_OP_SIM:
	return(SLACK_STUN_OP_SIM);
      case STUN_OP_LINK_ESTAB:
	return(SLACK_STUN_OP_LINK_ESTAB);
      default:
	return(0);
    }
}


/*
**  s l a c k _ p r s t a t e ( ) 
*/
char *slack_prstate (int state)
{
    char	*printed;

    switch (state) {
      case SLACK_DISCONNECTED:
	printed = "Disconnected";
	break;
      case SLACK_AWAIT_LINKUP_RSP:
	printed = "AwaitLinkupResponse";
	break;
      case SLACK_AWAIT_SDLC_OPEN:
	printed = "AwaitSdlcOpen";
	break;
      case SLACK_ACTIVE:
	printed = "Active";
	break;
	case SLACK_AWAIT_LINKDOWN_REQ:
	printed = "AwaitLinkdownRequest";
	break;
      case SLACK_AWAIT_LINKDOWN_RSP:
	printed = "AwaitLinkdownResponse";
	break;
      case SLACK_AWAIT_LINKDOWN_UA:
	printed = "AwaitLinkdownUa";
	break;
      case SLACK_AWAIT_ABEND_DISC:
	printed = "AwaitAbendDisc";
	break;
      case SLACK_AWAIT_ABORT_RSP:
	printed = "AwaitAbortResponse";
	break;
      case SLACK_AWAIT_NORM_DISC:
	printed = "AwaitNormDisc";
	break;
      default:
	printed = "UNKNOWN";
    }
    return(printed);
}

/*
**  s l a c k _ p r i n p u t ( ) 
*/
char *slack_prinput (int major, int minor)
{
    char	*printed, *prmajor, *prminor;

    printed = print_buffer;
    switch (major) {
      case SLACK_SDLC:
	prmajor = "SDLC";
	switch (minor) {
	  case SLACK_SDLC_SFRAME:
	    prminor = "S-Frame";
	    break;
	  case SLACK_SDLC_DM:
	    prminor = "DM";
	    break;
	  case SLACK_SDLC_FRMR:
	    prminor = "FRMR";
	    break;
	  case SLACK_SDLC_DISC:
	    prminor = "DISC";
	    break;
	  case SLACK_SDLC_UA:
	    prminor = "UA";
	    break;
	  case SLACK_SDLC_SNRM:
	    prminor = "SNRM";
	    break;
	  case SLACK_SDLC_RD:
	    prminor = "RD";
	    break;
	  case SLACK_SDLC_IFRAME:
	    prminor = "I-frame";
	    break;
	  case SLACK_SDLC_XID:
	    prminor = "XID";
	    break;
	  case SLACK_SDLC_SESSION_LOSS:
	    prminor = "SDLC session loss";
	    break;
	  case SLACK_SDLC_SIM:
	    prminor = "SIM";
	    break;
	  case SLACK_SDLC_RIM:
	    prminor = "RIM";
	    break;
	  case SLACK_SDLC_UI:
	    prminor = "UI";
	    break;
	  case SLACK_SDLC_TEST:
	    prminor = "TEST";
	    break;
	  default:
	    prminor = "UNKNOWN";
	}
	break;
      case SLACK_NETWORK:
	prmajor = "Network";
	switch (minor) {
	  case SLACK_STUN_OP_PASS:
	    prminor = "PassThrough";
	    break;
	  case SLACK_STUN_OP_LINKUP_REQ:
	    prminor = "LinkupRequest";
	    break;
	  case SLACK_STUN_OP_LINKUP_RSP:
	    prminor = "LinkupResponse";
	    break;
	  case SLACK_STUN_OP_LINKDOWN_REQ:
	    prminor = "LinkdownRequest";
	    break;
	  case SLACK_STUN_OP_LINKDOWN_RSP:
	    prminor = "LinkdownResponse";
	    break;
	  case SLACK_STUN_OP_ABORT_REQ:
	    prminor = "AbortRequest";
	    break;
	  case SLACK_STUN_OP_ABORT_RSP:
	    prminor = "AbortResponse";
	    break;
	  case SLACK_STUN_OP_RD_REQ:
	    prminor = "RequestDisconnectRequest";
	    break;
	  case SLACK_STUN_OP_DATA	:
	      prminor = "Data";
	    break;
	  case SLACK_NETWORK_SESSION_LOSS	:
	      prminor = "TCP session loss";
	    break;
	  case SLACK_STUN_OP_QUENCH_ON:
	    prminor = "Remote Flow Control On Request";
	    break;
	  case SLACK_STUN_OP_QUENCH_OFF:
	    prminor = "Remote Flow Control Off Request";
	    break;
	  case SLACK_STUN_OP_SIM:
	    prminor = "SIM/RIM";
	    break;
	  case SLACK_STUN_OP_LINK_ESTAB:
	    prminor = "Link Established";
	    break;
	  default:
	    prminor = "UNKNOWN";
	}
	break;
      case SLACK_TIMER:
	prmajor = "Timer";
	switch (minor) {
	  case SLACK_TIMER_DISC:
	    prminor = "Disconnect";
	    break;
	  case SLACK_RETRY:
	    prminor = "Slack Retransmission";
	    break;
	  default:
	    prminor = "UNKNOWN";
	}
	break;
      default:
	prmajor = "UNKNOWN";
	prminor = "";
    }
    strcpy(printed, prmajor);
    strcat(printed, ", ");
    strcat(printed , prminor);

    return(printed);
}


/*
**
** s l a c k _ b u r n _ a n d _ d i e
**
** SDLC has said this session has died, for a number of reasons. Abort the
** other end...
*/
void slack_burn_and_die (
     hwidbtype *idb,
     sdlc_data_block *sdb)
{
    stunpeertype *stp;

    stp = stun_addr_to_peer(idb, ((char *) &(sdb->sdlc_address)));
    if (!stp)
	return;
    (void) slack_fsm(stp, SLACK_SDLC, SLACK_SDLC_SESSION_LOSS);
}
