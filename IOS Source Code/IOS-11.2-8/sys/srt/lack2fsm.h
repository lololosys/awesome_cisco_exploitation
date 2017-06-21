/* $Id: lack2fsm.h,v 3.2.62.1 1996/07/19 22:46:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/srt/lack2fsm.h,v $
 *------------------------------------------------------------------
 * lack2fsm.h -- Local Acknowledgement Finite State Machine Header Shitola
 *
 * March 4, 1993, Claude Cartee and Eric B. Decker
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lack2fsm.h,v $
 * Revision 3.2.62.1  1996/07/19  22:46:34  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:54:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __LACK2FSM_H__
#define __LACK2FSM_H__

/*
 * States for new and improved Local Ack connection machine
 */

typedef enum {
    l2st_undefined	 = 0,		/* nobody home */
    l2st_disconnect	 = 1,		/* LAK not connected, but peers may be */
    l2st_localdiscwait	 = 2,		/* Waiting for other guy to disconnect  */
    l2st_remdiscwait	 = 3,		/* Waiting for other guy to disconnect  */
    l2st_remwait	 = 4,		/* Waiting for other guy to open */
    l2st_localwait	 = 5,		/* Waiting for my (local) Llc2 to open */
    l2st_connectpending	 = 6,		/* Syncronize us before final connect  */
    l2st_connect	 = 7,		/* We're passing data */
    l2st_remqonwait      = 8,	        /* Waiting for Quench on acknowledgement */
    l2st_remqoffwait     = 9,	        /* Waiting for Quench off acknowledgement */
    l2st_quench          = 10,	        /* We're quenched, expect no data */

    l2st_fsm_max	 = l2st_quench	/* Max legal value of "state" */

} l2st_t;


/*
 *  Major Categories of Input to Lak2_Fsm.
 */

typedef enum {
    l2ma_net		= 1,
    l2ma_dlc		= 2,
    l2ma_timer		= 3,
} l2ma_t;


/*
 *  Minor Categories of Input to Lak2_Fsm.
 */

typedef enum {				/* index, desc */
    l2mi_net_rx_connreq		= 1,	/* 0, Remote Connection Request */
    l2mi_net_rx_connpollreq	= 2,	/* 1, Remote in CP, have you seen it? */
    l2mi_net_rx_connpollack	= 3,	/* 2, Remote in CP, yes I have */
    l2mi_net_rx_connpollackreq	= 4,	/* 3, Remote in CP, have you seen me? */
    l2mi_net_rx_connwaitack	= 5,	/* 4, Remote in RW, sent CR, tell em we are here */
    l2mi_net_rx_discreq		= 6,	/* 5, Tear it down */
    l2mi_net_rx_discack		= 7,	/* 6, I'm torn down */
    l2mi_net_rx_la2_qoff	= 8,	/* 7, Saw a lack2 Quench off */
    l2mi_net_rx_la2_qon		= 9,	/* 8, Saw a lack2 Quench on */
    l2mi_net_rx_la2_qoff_ack	= 10,	/* 9, Received a Quench_Off acknowledgement */
    l2mi_net_rx_la2_qon_ack	= 11,	/* 10, Received a Quench_On acknowledgement */
    l2mi_net_rx_data		= 12,	/* 11, Saw Data */
    l2mi_net_abort		= 13,	/* 12, Network crashed */

    /*
     * Old Lack network message events.
     */
    l2mi_net_bc_linkupreq	= 14,	/* 13, old lack LINKUP_REQ */
    l2mi_net_bc_linkupprsp	= 15,	/* 14, old lack LINKUP_PRSP */
    l2mi_net_bc_linkdnreq	= 16,	/* 15, old lack LINKDOWN_REQ */
    l2mi_net_bc_linkdnprsp	= 17,	/* 16, old lack LINKDOWN_PRSP */
    l2mi_net_bc_qon		= 18,	/* 17, old lack QUENCH_ON */
    l2mi_net_bc_qoff		= 19,	/* 18, old lack QUENCH_OFF */

    l2mi_net_max		= l2mi_net_bc_qoff

} l2mi_net_t;


typedef enum {
    l2mi_dlc_ind_failure	= 1,	/* 19, DLC indicates somekind of failure */
    l2mi_dlc_ind_closed		= 2,	/* 20, DLC indicates it is closed */
    l2mi_dlc_ind_ext_conn	= 3,	/* 21, DLC got an external connection */
    l2mi_dlc_ind_opened		= 4,	/* 22, DLC has completed the connection */
    l2mi_dlc_ind_txqhigh	= 5,    /* 23, DLC's Tx queue at highwater mark */
    l2mi_dlc_ind_txqlow		= 6,    /* 24, DLC's Tx queue is low water mark */

#ifdef CLAUDE_DO_ME_LATER
    /*
     * while we're at it.  Get rid of the extraneous local shitola.
     */
    l2mi_dlc_data		= x,	/* xx, DLC local data */
#endif

    l2mi_dlc_max		= l2mi_dlc_ind_txqlow

} l2mi_dlt_t;
    

typedef enum {
    l2mi_timer_expired		= 1,	/* 25, Ack timer expired */
    l2mi_timer_abort		= 2,	/* 26, Ack timer exceeded retry count. */

    l2mi_timer_max		= l2mi_timer_abort

} l2mi_timer_t;


#define LAK2_MSG_RETRY_COUNT 3
#define LAK2_ACK_TIME 30	/* number of seconds between retries */

/*
 * Guard value for state definition tables in <../srt/lack2fsm.c>
 * Update value when adding new state events...
 */

#define LAK2_MAX_EVENTS		27
/*
 * Keep this entry hardcode to catch table entries mismatching
 * with number of events.
 */

/*
 * Param block definitions.
 * These parameters are encoded in the state tables to get passed
 * to the action routines.
 * This technique converges all the action routines to a single place
 * and does different actions based on the parameters being passed.
 */

typedef enum {
    L2T_NONE	= 0,
    L2T_STOP	= 1,
    L2T_START	= 2,
    L2T_RESUME	= 3
} l2pb_timer_op_t;

typedef enum {
    L2DLC_NONE		= 0,
    L2DLC_OPEN		= 1,
    L2DLC_CLOSE		= 2,
    L2DLC_BUSY		= 3,
    L2DLC_UNBUSY	= 4,
    L2DLC_ABORT		= 5
} l2pb_dlc_op_t;

typedef struct {
    int			net_op;
    l2pb_timer_op_t	timer_op;
    l2pb_dlc_op_t	dlc_op;
} l2_pb_t;


/*
 *  P r o t o t y p e   D e c l a r a t i o n s 
 */

/* -- In <srt/lack2fsm.c> -- */
extern	const fsm_t	lak2_fsm_table[];
extern	const fsm_t	lak2bc_fsm_table[];
extern	int	lak2_fsm_decode (lak_t *, int, int);

#endif /* __LACK2FSM_H__ */

