/* $Id: lack2fsm.c,v 3.2.62.2 1996/07/19 22:46:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/srt/lack2fsm.c,v $
 *------------------------------------------------------------------
 * lack2fsm.c -- Local Acknowledgement Finite State Machine
 *
 * March 4, 1993, Claude Cartee and Eric B. Decker
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lack2fsm.c,v $
 * Revision 3.2.62.2  1996/07/19  22:46:30  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/17  12:12:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:58:09  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  21:58:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  18:54:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:43:41  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.1  1995/06/07  23:00:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  I n c l u d e  F i l e s
 */

#include "master.h"
#include "interface_generic.h"
#include "packet.h"
#include "../util/fsm.h"
#include "../srt/rsrb.h"	/* lack_hash and older rsrb/lack definitions */
#include "../if/rif_inline.h"
#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"

typedef ushort	action_t;	/* return value for state action routines */


/*
 * l a k 2 _ f s m _ d e c o d e
 *
 * Given a major and minor, produce the majik index that drives all
 * the different array indicies.
 */

int lak2_fsm_decode(lak_t *lackie, int major, int minor)
{
    switch (major) {
      case l2ma_net:
	if (minor > l2mi_net_max)
	    return(FSM_FAULT);
	return(minor - 1);

      case l2ma_dlc:
	if (minor > l2mi_dlc_max)
	    return(FSM_FAULT);
	return(l2mi_net_max + minor - 1);

      case l2ma_timer:
	if (minor > l2mi_timer_max)
	    return(FSM_FAULT);
	return(l2mi_net_max + l2mi_dlc_max + minor - 1);

      default:
	return(FSM_FAULT);
    }
}


/*
 * Parameter blocks for common actions
 */

/*
 * abort parameter blocks
 */
static const l2_pb_t l2pb_DR_x_x =		{ RSRB_OP_DisconnectRequest,	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_DA_x_x =		{ RSRB_OP_DisconnectAck,	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_DR_x_abort =		{ RSRB_OP_DisconnectAck,	L2T_NONE,   L2DLC_ABORT };
static const l2_pb_t l2pb_DR_start_busy =	{ RSRB_OP_DisconnectRequest,	L2T_START,  L2DLC_BUSY };
static const l2_pb_t l2pb_DR_start_x    =	{ RSRB_OP_DisconnectRequest,	L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_DA_start_close =	{ RSRB_OP_DisconnectAck,	L2T_START,  L2DLC_CLOSE };

static const l2_pb_t l2pb_x_x_close =		{ RSRB_OP_NONE,			L2T_NONE,   L2DLC_CLOSE };
static const l2_pb_t l2pb_x_start_close =	{ RSRB_OP_NONE,			L2T_START,  L2DLC_CLOSE };
static const l2_pb_t l2pb_x_stop_abort =	{ RSRB_OP_NONE,			L2T_STOP,   L2DLC_ABORT };
static const l2_pb_t l2pb_x_start_abort =	{ RSRB_OP_NONE,			L2T_START,  L2DLC_ABORT };

static const l2_pb_t l2pb_LDR_stop_abort =	{ RSRB_OP_LINKDOWN_REQ,		L2T_STOP,   L2DLC_ABORT };
static const l2_pb_t l2pb_LDPR_stop_abort =	{ RSRB_OP_LINKDOWN_PRSP,	L2T_STOP,   L2DLC_ABORT };


/*
 * DISC state normal parameter blocks
 */
static const l2_pb_t l2pb_x_x_open =		{ RSRB_OP_NONE,			L2T_NONE,   L2DLC_OPEN };
static const l2_pb_t l2pb_CR_start_x =		{ RSRB_OP_ConnectRequest,	L2T_START,  L2DLC_NONE };



/*
 * RDW state normal parameter blocks
 */
static const l2_pb_t l2pb_DR_resume_x = 	{ RSRB_OP_DisconnectRequest,	L2T_RESUME, L2DLC_NONE };

/*
 * RW state normal parameter blocks
 */
static const l2_pb_t l2pb_CPR_start_x = 	{ RSRB_OP_ConnectPollReq,	L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_CPAR_start_x = 	{ RSRB_OP_ConnectPollAckAndReq, L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_CR_resume_x = 	{ RSRB_OP_ConnectRequest,	L2T_RESUME, L2DLC_NONE };
static const l2_pb_t l2pb_x_start_x =	 	{ RSRB_OP_NONE,			L2T_START,  L2DLC_NONE };

/*
 * LW state normal parameter blocks
 */
static const l2_pb_t l2pb_CWA_x_x = 		{ RSRB_OP_ConnectWaitAck,	L2T_NONE,   L2DLC_NONE };

/*
 * CP state normal parameter blocks
 */
static const l2_pb_t l2pb_x_stop_unbusy = 	{ RSRB_OP_NONE,			L2T_STOP,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_CPA_stop_unbusy = 	{ RSRB_OP_ConnectPollAck,	L2T_STOP,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_CPR_resume_x = 	{ RSRB_OP_ConnectPollReq,	L2T_RESUME, L2DLC_NONE };

/*
 * C state normal parameter blocks
 */
static const l2_pb_t l2pb_CPA_x_x = 		{ RSRB_OP_ConnectPollAck,	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_QONACK_x_busy = 	{ RSRB_OP_LA2_QON_ACK,		L2T_NONE,   L2DLC_BUSY };
static const l2_pb_t l2pb_QOFFACK_x_unbusy =	{ RSRB_OP_LA2_QOFF_ACK,		L2T_NONE,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_QON_start_x = 	{ RSRB_OP_LA2_QON,		L2T_START,  L2DLC_NONE };


/*
 * Qon_wait normal parameter blocks
 */
static const l2_pb_t l2pb_x_stop_x =		{ RSRB_OP_NONE,			L2T_STOP,   L2DLC_NONE };
static const l2_pb_t l2pb_QOFF_start_x = 	{ RSRB_OP_LA2_QOFF,		L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_QON_resume_x = 	{ RSRB_OP_LA2_QON,		L2T_RESUME, L2DLC_NONE };

/*
 * Qoff_wait normal parameter blocks
 */
static const l2_pb_t l2pb_QOFF_resume_x = 	{ RSRB_OP_LA2_QOFF,		L2T_RESUME, L2DLC_NONE };



/*
 * Bacward compatible FSM parameter blocks
 */
static const l2_pb_t l2pb_DR_stop_abort = 	{ RSRB_OP_DisconnectRequest,	L2T_STOP,   L2DLC_ABORT };
static const l2_pb_t l2pb_DA_stop_abort = 	{ RSRB_OP_DisconnectAck,	L2T_STOP,   L2DLC_ABORT };
static const l2_pb_t l2pb_LDR_x_x =      	{ RSRB_OP_LINKDOWN_REQ, 	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_LDPR_x_x =      	{ RSRB_OP_LINKDOWN_PRSP, 	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_LUR_start_x =      	{ RSRB_OP_LINKUP_REQ,   	L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_LDPR_start_close =	{ RSRB_OP_LINKDOWN_PRSP,	L2T_START,  L2DLC_CLOSE };
static const l2_pb_t l2pb_LDR_resume_x =      	{ RSRB_OP_LINKDOWN_REQ, 	L2T_RESUME, L2DLC_NONE };
static const l2_pb_t l2pb_LUPR_stop_unbusy = 	{ RSRB_OP_LINKUP_PRSP,  	L2T_STOP,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_LUPR_x_unbusy = 	{ RSRB_OP_LINKUP_PRSP,  	L2T_NONE,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_LDR_start_busy =	{ RSRB_OP_LINKDOWN_REQ, 	L2T_START,  L2DLC_BUSY };
static const l2_pb_t l2pb_LDR_start_x =      	{ RSRB_OP_LINKDOWN_REQ,   	L2T_START,  L2DLC_NONE };
static const l2_pb_t l2pb_LUR_resume_x =      	{ RSRB_OP_LINKUP_REQ,   	L2T_RESUME, L2DLC_NONE };
static const l2_pb_t l2pb_LUPR_x_x =      	{ RSRB_OP_LINKUP_PRSP,  	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_LDR_stop_x =      	{ RSRB_OP_LINKDOWN_REQ,   	L2T_STOP,   L2DLC_NONE };
static const l2_pb_t l2pb_x_x_busy =		{ RSRB_OP_NONE,			L2T_NONE,   L2DLC_BUSY };
static const l2_pb_t l2pb_x_x_unbusy =		{ RSRB_OP_NONE,			L2T_NONE,   L2DLC_UNBUSY };
static const l2_pb_t l2pb_BCQON_x_x =      	{ RSRB_OP_QUENCH_ON,    	L2T_NONE,   L2DLC_NONE };
static const l2_pb_t l2pb_BCQOFF_x_x =      	{ RSRB_OP_QUENCH_OFF,    	L2T_NONE,   L2DLC_NONE };


/*
 *****************************************************************************
 *                  L A K 2   A c t i o n s
 *****************************************************************************
 */

static action_t l2_action (lak_t *lakptr, const l2_pb_t *pb)
{
    dlc_rtn_t rc;

    if (!lakptr || !pb) {
	if (lack_debug_err)
	    buginf("\nLOCACK: l2_action, null lakptr or null param blk");
	return(0);
    }
    if (lack_debug_state)
	buginf("\nLOCACK: l2_action %d %d %d",
	       pb->net_op, pb->timer_op, pb->dlc_op);
    if (pb->net_op != RSRB_OP_NONE)
	lak2_send_netmsg(lakptr, pb->net_op);
    switch(pb->timer_op) {
      case L2T_NONE:
	break;
      case L2T_STOP:
	lak2_StopAckTimer(lakptr);
	break;
      case L2T_START:
	lak2_StartAckTimer(lakptr, LAK2_MSG_RETRY_COUNT);
	break;
      case L2T_RESUME:
	lak2_ResumeAckTimer(lakptr);
	break;
      default:
	buginf("\nLOCACK: l2_action, bad timer op %d", pb->timer_op);
	break;
    }
    if (pb->dlc_op != L2DLC_NONE) {
	if (!lakptr->dlc_handle) {
	    buginf("\nLOCACK: l2_action, no dlc_handle");
	    return(FSM_OK);
	}
	switch(pb->dlc_op) {
	  case L2DLC_OPEN:
	    if (!lakptr->dlc_handle->dlc_open) {
		buginf("\nLOCACK: l2_action, dlc_open, no proc ptr.");
		break;
	    }
	    rc = (*lakptr->dlc_handle->dlc_open)(&lakptr->dlc_db, lakptr);
	    switch(rc) {
	      case DLC_RS_OK:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_opened);
		break;
	      case DLC_RS_FAILED:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_failure);
		break;
	      case DLC_RS_PENDING:
		break;
	      default:
		buginf("\nLOCACK: l2_action, bad return from dlc_open - %d", rc);
		break;
	    }
	    break;

	  case L2DLC_CLOSE:
	    if (!lakptr->dlc_handle->dlc_close) {
		buginf("\nLOCACK: l2_action, dlc_close, no proc ptr.");
		return(FSM_OK);
	    }
	    rc = (*lakptr->dlc_handle->dlc_close)(&lakptr->dlc_db, lakptr);
	    switch(rc) {
	      case DLC_RS_OK:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_closed);
		break;
	      case DLC_RS_FAILED:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_failure);
		break;
	      case DLC_RS_PENDING:
		break;
	      default:
		buginf("\nLOCACK: l2_action, bad return from dlc_close - %d", rc);
		break;
	    }
	    break;

	  case L2DLC_BUSY:
	    if (!lakptr->dlc_handle->dlc_busy) {
		buginf("\nLOCACK: l2_action, dlc_busy, no proc ptr.");
		return(FSM_OK);
	    }
	    rc = (*lakptr->dlc_handle->dlc_busy)(&lakptr->dlc_db, TRUE);
	    switch(rc) {
	      case DLC_RS_OK:
	      case DLC_RS_PENDING:
		break;
	      case DLC_RS_FAILED:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_failure);
		break;
	      default:
		buginf("\nLOCACK: l2_action, bad return from dlc_busy - %d", rc);
		break;
	    }
	    break;

	  case L2DLC_UNBUSY:
	    if (!lakptr->dlc_handle->dlc_busy) {
		buginf("\nLOCACK: l2_action, dlc_unbusy, no proc ptr.");
		return(FSM_OK);
	    }
	    rc = (*lakptr->dlc_handle->dlc_busy)(&lakptr->dlc_db, FALSE);
	    switch(rc) {
	      case DLC_RS_OK:
	      case DLC_RS_PENDING:
		break;
	      case DLC_RS_FAILED:
		lak2_fsm(lakptr, l2ma_dlc, l2mi_dlc_ind_failure);
		break;
	      default:
		buginf("\nLOCACK: l2_action, bad return from dlc_unbusy - %d", rc);
		break;
	    }
	    break;

	  case L2DLC_ABORT:
	    if (!lakptr->dlc_handle->dlc_abort) {
		buginf("\nLOCACK: l2_action, dlc_abort, no proc ptr.");
		return(FSM_OK);
	    }
	    rc = (*lakptr->dlc_handle->dlc_abort)(&lakptr->dlc_db);
	    break;

	  default:
	    break;
	}
    }
    return(FSM_OK);
}


static action_t l2ac_bad_timer (lak_t *lakptr, const l2_pb_t *pb)
{
    if (lack_debug_err)
	buginf("\nLOCACK: bad timer pop for %s, new state %s",
	       lack_id(lakptr), lak2_prstate(lakptr, 0));
    return(FSM_RC_UNEXPECTED_INPUT);
}


/*
 * l 2 a c _ n e t _ c o n n
 *
 * state:	disconnect
 * event:	net_rx_connreq
 * action:	make permanent rif entry and call dlc for the open
 */

static action_t l2ac_net_conn (lak_t *lakptr, const l2_pb_t *pb)
{
    srbroute_t *rif;
    uchar *da, *sa;
    boolean flip = FALSE;

    if (!lakptr) {
	if (lack_debug_err)
	    buginf("\nLOCACK: l2ac_net_conn, null lakptr");
	return(0);
    }
    switch(lakptr->dlc_db.type) {
      case DLC_LLC2:
	da = (uchar *) &lakptr->LACK_LOCAL_ADDR;
	sa = (uchar *) &lakptr->LACK_REM_ADDR;
	flip = TRUE;
	break;
      case DLC_CLS:
      case DLC_SDLC:
      case DLC_QLLC:
	sa = (uchar *) &lakptr->LACK_LOCAL_ADDR;
	da = (uchar *) &lakptr->LACK_REM_ADDR;
	break;
      default:
	if (lack_debug_err)
	    buginf("\nLOCACK: l2ac_net_conn, unexpected dlc type %d", lakptr->dlc_db.type);
	return(0);
    }
    rif = (srbroute_t *) lakptr->rif.data;
    if (lakptr->rif.len) {
	if (flip)
	    rif->rc_dlf ^= RC_D;
	rif_update(NULL, 0, da, sa, rif, RIF_NOAGE);
	if (flip)
	    rif->rc_dlf ^= RC_D;
    } else
	rif_update(NULL, 0, da, sa, NULL, RIF_NOAGE);
    return(l2_action(lakptr, &l2pb_x_x_open));
}

/*
 * l 2 a c _ i n v l d _ s t
 *
 * state:	connectpending, remqonwait, remqoffwait
 * event:	all
 * action:	display an error message and then take appropriate action
 */

static action_t l2ac_invld_st (lak_t *lakptr, const l2_pb_t *pb)
{
    if (!lakptr || !pb) {
	if (lack_debug_err)
	    buginf("\nLOCACK: l2ac_invld_st, null lakptr or null param blk");
	return(0);
    }
    buginf("\nLACK finite state machine in invalid state");
    return(l2_action(lakptr, pb));
}

/******************************************************************************
 *
 *                               L A C K 2 _ F S M 
 *                              (The Wrath of Comm)
 *
 * 		     	      S t a t e   T a b l e
 *
 *****************************************************************************/

#define L2_ACTION	FSM_ACTION_ROUTINE(l2_action)
#define L2AC_NET_CONN	FSM_ACTION_ROUTINE(l2ac_net_conn)
#define L2AC_BAD_TIMER	FSM_ACTION_ROUTINE(l2ac_bad_timer)
#define L2AC_INVLD_ST	FSM_ACTION_ROUTINE(l2ac_invld_st)
#define LAK2_FSM_DECODE	FSM_DECODE_ROUTINE(lak2_fsm_decode)

/*
 *  LACK2_FSM State = "l2st_disconnect"
 */

const static struct fsm_state l2state_disconnect[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				     /* Network Events						  */
{ L2AC_NET_CONN,FSM_NOARG,		l2st_localwait },	/* 0, l2mi_net_rx_connreq         */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq     */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack     */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 4, l2mi_net_rx_connwaitack     */
{ L2_ACTION,	&l2pb_DA_x_x,		FSM_NOCHANGE },		/* 5, l2mi_net_rx_discreq         */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data       */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	FSM_NOCHANGE },		/* 13, l2mi_net_bc_linkupreq      */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	FSM_NOCHANGE },		/* 14, l2mi_net_bc_linkupprsp     */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	FSM_NOCHANGE },		/* 15, l2mi_net_bc_linkdnreq      */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	FSM_NOCHANGE },		/* 16, l2mi_net_bc_linkdnprsp     */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	FSM_NOCHANGE },		/* 17, l2mi_net_bc_qon            */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	FSM_NOCHANGE },		/* 18, l2mi_net_bc_qoff           */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_CR_start_x,	l2st_remwait },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened    	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired      	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_localdiscwait"
 */

const static struct fsm_state l2state_localdiscwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				     /* Network Events						  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_x_x,		FSM_NOCHANGE },		/* 5, l2mi_net_rx_discreq	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_x_abort,	FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ FSM_NOACTION,	FSM_NOARG,		l2st_disconnect },	/* 19, l2mi_dlc_ind_failure	  */
{ FSM_NOACTION,	FSM_NOARG,		l2st_disconnect },	/* 19, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_remdiscwait"
 */

const static struct fsm_state l2state_remdiscwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_x_start_close,	l2st_localdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_x_x,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_start_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 19, l2mi_dlc_ind_failure	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 20, l2mi_dlc_ind_closed	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_DR_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,      &l2pb_x_stop_abort,	l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_remwait"
 */

const static struct fsm_state l2state_remwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_CPR_start_x,	l2st_connectpending },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPAR_start_x,	l2st_connectpending },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_x_start_x,	FSM_NOCHANGE },		/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_CR_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,    &l2pb_x_stop_abort,     l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_localwait"
 */

const static struct fsm_state l2state_localwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_CWA_x_x,		FSM_NOCHANGE },		/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort   	  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_CPR_start_x,	l2st_connectpending },	/* 21, l2mi_dlc_ind_ext_conn	  */
/* rnr-disable feature changed it busy to x */
{ L2_ACTION,	&l2pb_CPR_start_x,	l2st_connectpending },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER, FSM_NOARG,            FSM_NOCHANGE },         /* 25, l2mi_timer_expired         */
{ L2AC_BAD_TIMER, FSM_NOARG,            FSM_NOCHANGE }          /* 26, l2mi_timer_abort           */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_connectpending"
 */

const static struct fsm_state l2state_connectpending[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_CPR_start_x,	FSM_NOCHANGE },		/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPAR_start_x,	FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_x_stop_unbusy,	l2st_connect },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_CPA_stop_unbusy,	l2st_connect },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_x_stop_unbusy,	l2st_connect },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_CPR_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,      &l2pb_x_stop_abort,	l2st_disconnect }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_connect
 */

const static struct fsm_state l2state_connect[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_QOFFACK_x_unbusy,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_QONACK_x_busy,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_QON_start_x,	l2st_remqonwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_remqonwait"
 */

const static struct fsm_state l2state_remqonwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_QOFFACK_x_unbusy,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_QONACK_x_busy,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_x_stop_x,		l2st_quench },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_QOFF_start_x,	l2st_remqoffwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_QON_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,    &l2pb_x_stop_abort,     l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_remqoffwait"
 */

const static struct fsm_state l2state_remqoffwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_QOFFACK_x_unbusy,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_QONACK_x_busy,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_x_stop_x,		l2st_connect },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_QON_start_x,	l2st_remqonwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ FSM_NOACTION,	FSM_NOACTION,		FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_QOFF_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,    &l2pb_x_stop_abort,     l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2st_quench"
 */

const static struct fsm_state l2state_quench[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq	  */
{ FSM_NOACTION,	FSM_NOACTION,		FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_CPA_x_x,		FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_start_close,	l2st_localdiscwait },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_QOFFACK_x_unbusy,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_QONACK_x_busy,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_DR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_DR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_QOFF_start_x,	l2st_remqoffwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


const fsm_t lak2_fsm_table[l2st_fsm_max] = 
{
    /*  State                   Decoder			    State Value		*/
      { l2state_disconnect,	LAK2_FSM_DECODE },	/*  l2st_disconnect     */
      { l2state_localdiscwait,	LAK2_FSM_DECODE },	/*  l2st_localdiscwait    */
      { l2state_remdiscwait,	LAK2_FSM_DECODE },	/*  l2st_remdiscwait    */
      { l2state_remwait,	LAK2_FSM_DECODE },	/*  l2st_remwait        */
      { l2state_localwait,	LAK2_FSM_DECODE },	/*  l2st_localwait      */
      { l2state_connectpending,	LAK2_FSM_DECODE },	/*  l2st_connectpending */
      { l2state_connect,	LAK2_FSM_DECODE },	/*  l2st_connect        */
      { l2state_remqonwait,     LAK2_FSM_DECODE },	/*  l2st_remqonwait	*/
      { l2state_remqoffwait,    LAK2_FSM_DECODE },	/*  l2st_remqoffwait	*/
      { l2state_quench,         LAK2_FSM_DECODE }	/*  l2st_Quench         */
};


/******************************************************************************
 *
 *                               L A C K _ B C _ F S M 
 *
 * 		      	         S t a t e   T a b l e
 *
 * Backward Compatible Old LACK State Machine
 *
 *****************************************************************************/


/*
 *  LACK2_FSM State = "l2bcstate_disconnect"
 */

const static struct fsm_state l2bcstate_disconnect[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 0, l2mi_net_rx_connreq         */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 1, l2mi_net_rx_connpollreq     */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 2, l2mi_net_rx_connpollack     */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 4, l2mi_net_rx_connwaitack     */
{ L2_ACTION,	&l2pb_DA_stop_abort,	FSM_NOCHANGE },		/* 5, l2mi_net_rx_discreq         */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	FSM_NOCHANGE },		/* 11, l2mi_net_rx_llc2data       */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2AC_NET_CONN,FSM_NOARG,		l2st_localwait },	/* 13, l2mi_net_bc_linkupreq      */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 14, l2mi_net_bc_linkupprsp     */
{ L2_ACTION,	&l2pb_LDPR_x_x,  	FSM_NOCHANGE },		/* 15, l2mi_net_bc_linkdnreq      */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 16, l2mi_net_bc_linkdnprsp     */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 17, l2mi_net_bc_qon            */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 18, l2mi_net_bc_qoff           */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_LUR_start_x,	l2st_remwait },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened    	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired      	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2bcstate_localdiscwait"
 */

const static struct fsm_state l2bcstate_localdiscwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },   	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },   	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_x_x, 	FSM_NOCHANGE },   	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },   	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },   	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },   	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ FSM_NOACTION,	FSM_NOARG,		l2st_disconnect },	/* 19, l2mi_dlc_ind_failure	  */
{ FSM_NOACTION,	FSM_NOARG,		l2st_disconnect },	/* 19, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_x_x_close, 	FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_x_x_close, 	FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_x_x_close,	FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2bcstate_remdiscwait"
 */

const static struct fsm_state l2bcstate_remdiscwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_start_close,	l2st_localdiscwait },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_x_start_close,	l2st_localdiscwait },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_x_x,  	FSM_NOCHANGE },		/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 19, l2mi_dlc_ind_failure	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 20, l2mi_dlc_ind_closed	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 21, l2mi_dlc_ind_ext_conn	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 22, l2mi_dlc_ind_opened	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_LDR_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,    &l2pb_x_stop_abort,     l2st_disconnect }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2bcstate_remwait"
 */

const static struct fsm_state l2bcstate_remwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LUPR_stop_unbusy,	l2st_connect },   	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_x_stop_unbusy,	l2st_connect },   	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_start_close,	l2st_localdiscwait },   	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2_ACTION,	&l2pb_LUR_resume_x,	FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2bcstate_localwait"
 */

const static struct fsm_state l2bcstate_localwait[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ FSM_NOACTION,	FSM_NOARG,      	FSM_NOCHANGE },   	/* 13, l2mi_net_bc_linkupreq	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_start_close,	l2st_localdiscwait },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_LUPR_x_unbusy,	l2st_connect },   	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_LUPR_x_unbusy, 	l2st_connect },   	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};



/*
 *  LACK2_FSM State = "l2bcstate_connect"
 */

const static struct fsm_state l2bcstate_connect[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LUPR_x_x, 	FSM_NOCHANGE },   	/* 13, l2mi_net_bc_linkupreq	  */
{ FSM_NOACTION,	FSM_NOARG,      	FSM_NOCHANGE },   	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_start_close,	l2st_localdiscwait },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_x_x_busy, 	FSM_NOCHANGE },   	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_x_x_unbusy,	FSM_NOCHANGE },   	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ L2_ACTION,	&l2pb_BCQON_x_x,  	l2st_quench },    	/* 23, l2mi_dlc_ind_txqhigh	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE },		/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,FSM_NOARG,		FSM_NOCHANGE }		/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};


/*
 *  LACK2_FSM State = "l2bcstate_quench"
 */

const static struct fsm_state l2bcstate_quench[LAK2_MAX_EVENTS] =
{
/*  Action	Param Blk		Next State		Index  Decoded Input		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Network Events						  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2_ACTION,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2_ACTION,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2_ACTION,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ---------------------------------------------------------------------------------------------- */
{ L2_ACTION,	&l2pb_LUPR_x_x, 	FSM_NOCHANGE },   	/* 13, l2mi_net_bc_linkupreq	  */
{ FSM_NOACTION,	FSM_NOARG,      	FSM_NOCHANGE },   	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2_ACTION,	&l2pb_LDPR_start_close,	l2st_localdiscwait },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2_ACTION,	&l2pb_x_x_busy, 	FSM_NOCHANGE },   	/* 17, l2mi_net_bc_qon		  */
{ L2_ACTION,	&l2pb_x_x_unbusy,	FSM_NOCHANGE },   	/* 18, l2mi_net_bc_qoff		  */
/* ---------------------------------------------------------------------------------------------- */
				/* Local DLC events						  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 19, l2mi_dlc_ind_failure	  */
{ L2_ACTION,	&l2pb_LDR_start_x,	l2st_remdiscwait },	/* 20, l2mi_dlc_ind_closed	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2_ACTION,	&l2pb_LDR_start_busy,	l2st_remdiscwait },	/* 22, l2mi_dlc_ind_opened	  */
{ FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE },		/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2_ACTION,	&l2pb_BCQOFF_x_x,  	l2st_connect },    	/* 24, l2mi_dlc_ind_txqlow	  */
/* ---------------------------------------------------------------------------------------------- */
				/* Timer events							  */
{ L2AC_BAD_TIMER,	FSM_NOARG,		FSM_NOCHANGE },	/* 25, l2mi_timer_expired	  */
{ L2AC_BAD_TIMER,	FSM_NOARG,		FSM_NOCHANGE }	/* 26, l2mi_timer_abort		  */
/* ---------------------------------------------------------------------------------------------- */
};



/*
 * The Backward Compatible State Machine does NOT use the ConnectPending,
 * QonWait, or QoffWait states.  If the machine somehow gets into these
 * states we use the following state table.
 * 
 *  LACK2_FSM State = "l2bcstate_illegalstate"
 */

const static struct fsm_state l2bcstate_illegalstate[LAK2_MAX_EVENTS] =
{
/*  Action	        Param Blk		Next State		Index  Decoded Input		  */
/* ------------------------------------------------------------------------------------------------------ */
				             /* Network Events						  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 0, l2mi_net_rx_connreq	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 1, l2mi_net_rx_connpollreq	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 2, l2mi_net_rx_connpollack	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 3, l2mi_net_rx_connpollackreq  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 4, l2mi_net_rx_connwaitack	  */
{ L2AC_INVLD_ST,	&l2pb_DA_stop_abort,	l2st_disconnect },	/* 5, l2mi_net_rx_discreq	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 6, l2mi_net_rx_discack	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 7, l2mi_net_rx_la2_qoff	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 8, l2mi_net_rx_la2_qon	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 9, l2mi_net_rx_la2_qoff_ack	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 10, l2mi_net_rx_la2_qon_ack	  */
{ L2AC_INVLD_ST,	&l2pb_DR_stop_abort,	l2st_disconnect },	/* 11, l2mi_net_rx_llc2data	  */
{ L2AC_INVLD_ST,	&l2pb_x_stop_abort,	l2st_disconnect },	/* 12, l2mi_net_abort		  */
/* ------------------------------------------------------------------------------------------------------ */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 13, l2mi_net_bc_linkupreq	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 14, l2mi_net_bc_linkupprsp	  */
{ L2AC_INVLD_ST,	&l2pb_LDPR_stop_abort,	l2st_disconnect },	/* 15, l2mi_net_bc_linkdnreq	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 16, l2mi_net_bc_linkdnprsp	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 17, l2mi_net_bc_qon		  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 18, l2mi_net_bc_qoff		  */
/* ------------------------------------------------------------------------------------------------------ */
				             /* Local DLC events					  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_x,	l2st_disconnect },	/* 19, l2mi_dlc_ind_failure	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_x,	l2st_disconnect },	/* 20, l2mi_dlc_ind_closed	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 21, l2mi_dlc_ind_ext_conn	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 22, l2mi_dlc_ind_opened	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 23, l2mi_dlc_ind_txqhigh	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 24, l2mi_dlc_ind_txqlow	  */
/* ------------------------------------------------------------------------------------------------------ */
				             /* Timer events				       		  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect },	/* 25, l2mi_timer_expired	  */
{ L2AC_INVLD_ST,	&l2pb_LDR_stop_abort,	l2st_disconnect }	/* 26, l2mi_timer_abort		  */
/* ------------------------------------------------------------------------------------------------------ */
};


const fsm_t lak2bc_fsm_table[l2st_fsm_max] = 
{
    /*  State                   Decoder			    State Value		*/
      { l2bcstate_disconnect,	LAK2_FSM_DECODE },	/*  l2st_disconnect     */
      { l2bcstate_localdiscwait,LAK2_FSM_DECODE },	/*  l2st_localdiscwait  */
      { l2bcstate_remdiscwait,	LAK2_FSM_DECODE },	/*  l2st_remdiscwait    */
      { l2bcstate_remwait,	LAK2_FSM_DECODE },	/*  l2st_remwait        */
      { l2bcstate_localwait,	LAK2_FSM_DECODE },	/*  l2st_localwait      */
      { l2bcstate_illegalstate,	LAK2_FSM_DECODE },	/*  l2st_connectpending */
      { l2bcstate_connect,	LAK2_FSM_DECODE },	/*  l2st_connect        */
      { l2bcstate_illegalstate, LAK2_FSM_DECODE },	/*  l2st_remqonwait	*/
      { l2bcstate_illegalstate, LAK2_FSM_DECODE },	/*  l2st_remqoffwait	*/
      { l2bcstate_quench,       LAK2_FSM_DECODE }		/*  l2st_Quench         */
};

#ifdef documentationIsAGoodThing

1) When either an Ack_Timer_Expired or Ack_Timer_Abort condition occurs,
   the lak entity is removed from the active timer queue.  If we are done
   nothing else needs to happen.  If we want to continue counting down then
   lak2_resume_ack_timer should be called.  This reinserts the lak entity
   into the ack timer queue without modification to the retry_count.

   When a Ack_Timer_Abort occurs, Abort Peer is called if appropriate.
   No state change occures for this transition but the right thing happens
   anyway.  AbortPeer causes network aborts for all the lack entities
   associated with the peer including the current one.  The network
   abort causes the state transition.

2) In looking at the current Lack code, the following scenerio occurs.  We
   are in Connect and an incoming SABME is detected.  The current code
   sends a Link Up Request and goes into Awaiting Link Up Response.  When
   the other side gets this Link Up Request it closes the current llc
   session and reopens it.

   Currently, I will code this as an Llc2ConnectExt and will tear the llc2
   down and send a DR to the far side.  Is this how we want it to work?
   Its okay for now.

3) I think eventually we should add in behaviour for Local inputs too.  I
   think they can fit pretty reasonably.  But later.


** - means its been checked.

Current
State	Event			Actions		  Code	Next State
======================================================================
disc	Rx_ConnectRequest	Insert Network Rif (a)  lw	**
				llc2_req_open(busy)
	--------------------------------------------------------------
	Rx_ConnectPollReq	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollAck	Insert Network Rif (b)	disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectWaitAck	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectRequest	Insert Network Rif (c)  disc	**
				Tx_DisconnectAck
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	--			disc	**
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_ConnectRequest  (e)	rw	**
				StartAckTimer
				RetryCount = 3
	--------------------------------------------------------------
	LocalLlc2ConnectAck	Insert Network Rif (b)  disc	**
				Tx_DisconnectReq
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	AckTimerExpired		--			disc	**
	--------------------------------------------------------------
	AckTimerAbort		--			disc	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
rw	Rx_ConnectRequest	Tx_ConnectPollReq  (f)  cp	**
(timer)				StartAckTimer
	(parallel sabme race)	RetryCount = 3
	--------------------------------------------------------------
	Rx_ConnectPollReq	Tx_ConnPollAckReq  (g)  cp	**
				StartAckTimer
				RetryCount = 3
	--------------------------------------------------------------
	Rx_ConnectPollAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectWaitAck	StartAckTimer	   (i)  rw	**
				RetryCount = 3
	--------------------------------------------------------------
	Rx_DisconnectRequest	Tx_DisconnectAck   (t)  disc	**
				StopAckTimer
    				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Tx_DisconnectReq   (h)  rdw	AAA
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close

	This says that someone sent us another SABME.  Do we
	really want to rdw?  We should probably tell the other
	side and restart the connection.
	--------------------------------------------------------------
	LocalLlc2ConnectAck	--			rw	**

	eh?  We are in RW says we got a Llc2ConnectExt.  Why are we
	getting a Llc2ConnectAck from LLC2.  This sounds like an
	error to me.  I think this should generate a Tx_DisconnectReq
	and go to rdw.
	--------------------------------------------------------------
	AckTimerExpired		Tx_ConnectRequest  (j)  rw	**
				ResumeAckTimer
	--------------------------------------------------------------
	AckTimerAbort		AbortPeer	   (k)  --	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
lw	Rx_ConnectRequest	Tx_ConnectWaitAck  (l)  lw	**
	(signal remote "waiter" we are alive and waiting for local llc)
	--------------------------------------------------------------
	Rx_ConnectPollReq	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectWaitAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectRequest	Tx_DisconnectAck   (t)  disc	**
				StopAckTimer
    				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_ConnectPollReq  (f)  cp	**
				StartAckTimer
				RetryCount = 3
	--------------------------------------------------------------
	LocalLlc2ConnectAck	Tx_ConnectPollReq  (f)  cp	**
				StartAckTimer
				RetryCount = 3
	--------------------------------------------------------------
	AckTimerExpired		--			lw	**
	--------------------------------------------------------------
	AckTimerAbort		--			lw	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
cp	Rx_ConnectRequest	---			cp	**
(timer)
	There is a race condition here.  That is the remote side
	gets a Llc2ConnectAck and moves to cp but the local side
	(in RW) times out prior to seeing the CPR from the remote
	and retransmits the CR.
	--------------------------------------------------------------
	Rx_ConnectPollReq	Tx_ConnectPollAckReq (g) cp	**
				StartAckTimer
				RetryCount = 3
	--------------------------------------------------------------
	Rx_ConnectPollAck	StopAckTimer	   (n)  conn	**
				llc2_unbusy
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Tx_ConnectPollAck  (m)  conn	**
				StopAckTimer
				llc2_unbusy
	--------------------------------------------------------------
	Rx_ConnectWaitAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectRequest	Tx_DisconnectAck   (t)  disc	**
				StopAckTimer
    				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	StopAckTimer	   (n)  conn	**
				llc2_unbusy
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close

        This could be a resyncronization request.  Restart?
	--------------------------------------------------------------
	LocalLlc2ConnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	AckTimerExpired		Tx_ConnectPollReq  (r)  cp	**
				ResumeAckTimer
	--------------------------------------------------------------
	AckTimerAbort		AbortPeer	   (k)  --	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
rdw	Rx_ConnectRequest	Tx_DisconnectReq   (d)  rdw	**
(timer)	--------------------------------------------------------------
	Rx_ConnectPollReq	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	Rx_ConnectPollAck	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	Rx_ConnectWaitAck	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	Rx_DisconnectRequest	Tx_DisconnectAck   (t)  disc	**
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	StopAckTimer	   (q)  disc	**
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	LocalLlc2ConnectAck	Tx_DisconnectReq   (d)  rdw	**
	--------------------------------------------------------------
	AckTimerExpired		Tx_DisconnectReq   (o)  rdw	**
			 	ResumeAckTimer
	--------------------------------------------------------------
	AckTimerAbort		AbortPeer	   (k)  --	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
conn	Rx_ConnectRequest	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_ConnectPollReq	Tx_ConnectPollAck  (p)  conn	**
	--------------------------------------------------------------
	Rx_ConnectPollAck	--			conn	**
	--------------------------------------------------------------
	Rx_ConnectPollAckReq	Tx_ConnectPollAck  (p)  conn	**
	--------------------------------------------------------------
	Rx_ConnectWaitAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectRequest	Tx_DisconnectAck   (t)  disc	**
				StopAckTimer
				llc2_close
	--------------------------------------------------------------
	Rx_DisconnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	RSRB_OP_QUENCH_OFF	--			conn	**
	--------------------------------------------------------------
	RSRB_OP_QUENCH_ON	--			conn	**
	--------------------------------------------------------------
	RSRB_OP_LLC2DATA	--			conn	**
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
	--------------------------------------------------------------
	LocalLlc2Failure	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectExt	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	LocalLlc2ConnectAck	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	AckTimerExpired		--			conn	**
	--------------------------------------------------------------
	AckTimerAbort		--			conn	**
======================================================================

Current
State	Event			Actions		  Code  Next State
======================================================================
any	Rx_DisconnectReq	Tx_DisconnectReq   (h)  rdw	**
				StartAckTimer
				RetryCount = 3
				llc2_close
	--------------------------------------------------------------
	NetAbort		StopAckTimer	   (s)  disc	****
				llc2_close
======================================================================


States
------
l2st_Disconnect
l2st_remwait
l2st_localwait
l2st_connectpending
l2st_connect
l2st_remdiscwait


Network Events
---------------
Rx_ConnectRequest
Rx_ConnectPollReq
Rx_ConnectPollAck
Rx_ConnectPollAckReq
Rx_ConnectWaitAck
Rx_DisconnectRequest
Rx_DisconnectAck
RSRB_OP_QUENCH_OFF
RSRB_OP_QUENCH_ON
RSRB_OP_LLC2DATA (same as always...)
LackAbort			tcp dropped, or whatever.  nail this lack connection.


Local LLC Events
----------
llc2_localfailure
llc2_localconnext
llc2_localreqopenack


Timer Events
------------
timer_expired
timer_abort


Network Actions
----------------
Tx_ConnectRequest		lak2_send_netmsg(lackie, op)
Tx_ConnectPollReq
Tx_ConnectPollAck
Tx_ConnectPollAckReq
Tx_ConnectWaitAck
Tx_DisconnectRequest
Tx_DisconnectAck

Local LLC2 Control codes
------------------------
llc2_ctl_req_open		lak2_ctl_llc2(lackie, op)
llc2_close
llc2_unbusy

FSM Action Procs
----------------
lak2_butcher_piggie(lackie)
lak2_StartAckTimer(lackie, retry_count)
lak2_ResumeAckTimer(lackie)
lak2_StopAckTimer(lackie)

#endif
