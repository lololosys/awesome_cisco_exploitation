/* $Id: vines_srtp_fsm.h,v 3.2.62.1 1996/07/19 22:47:12 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_srtp_fsm.h,v $
 *------------------------------------------------------------------
 * vines_srtp_fsm.h -- Vines Sequenced RTP State Machine
 *
 * June 1993, David R. Hampton
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_srtp_fsm.h,v $
 * Revision 3.2.62.1  1996/07/19  22:47:12  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:58:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VINES_SRTP_FSM_H__
#define __VINES_SRTP_FSM_H__

/*
 * Arguments:
 *
 * The Finite State Machine code allows for only one argument to the
 * function the it will call.  The vines SRTP code needs to pass more
 * than one argument, so it passes them in a structure.  This defines
 * what that structure looks like.
 */
typedef struct vsrtp_fsm_data_ {
    paktype *pak;
    vinespathtype *path;
} vsrtp_fsm_data;

/*
 * States:
 *
 * The allowed states for the vines SRTP state machine.
 */
typedef enum vsrtp_state_ {
    vsrtp_st_up	= 1,	/* neighbor reachable and in sunc */
    vsrtp_st_chgrqst	= 2,	/* neighbor reachable, but sequence lost */
    vsrtp_st_fullrqst	= 3,	/* neighbor reachable, but need full update */
    vsrtp_st_init	= 4,	/* neighbor is new to network */

    vsrtp_fsm_max	= vsrtp_st_init /* Max legal value of "state" */

} vsrtp_state;

/*
 * Major Categories of Input to vsrtp_fsm:
 */
typedef enum vsrtp_major_ {
    vsrtp_rx_request	= 1,
    vsrtp_rx_update	= 2,
    vsrtp_rx_response	= 3,
    vsrtp_rx_redirect	= 4,
    vsrtp_rx_reinit	= 5,
    vsrtp_timer		= 6,
    vsrtp_unknown	= 7,
} vsrtp_major;

/*
 *  Minor Categories of Input to vsrtp_fsm:
 */
typedef enum vsrtp_rx_req_minor_ {	/* index, desc */
    vsrtp_rx_req_specific	= 1,	/* Request specific routes        */
    vsrtp_rx_req_changes	= 2,	/* Request change update          */
    vsrtp_rx_req_full		= 3,	/* Request full update            */
    vsrtp_rx_req_response	= 4,	/* Request response only          */
    vsrtp_rx_req_max		= vsrtp_rx_req_response,
    vsrtp_rx_req_invalid	= 5
} vsrtp_rx_req_minor;

typedef enum vsrtp_rx_upd_minor_ {	/* index, desc */
    vsrtp_rx_upd_null_failed	= 1,	/* Null Update, reassembly failed    */
    vsrtp_rx_upd_null_less	= 2,	/* Null Update, new < prev           */
    vsrtp_rx_upd_null_equal	= 3,	/* Null Update, new == prev	     */
    vsrtp_rx_upd_null_onemore	= 4,	/* Null Update, new == prev + 1      */
    vsrtp_rx_upd_null_greater	= 5,	/* Null Update, new > prev	     */

    vsrtp_rx_upd_change_failed	= 6,	/* Change Update, reassembly failed  */
    vsrtp_rx_upd_change_less	= 7,	/* Change Update, new < prev	     */
    vsrtp_rx_upd_change_equal	= 8,	/* Change Update, new == prev        */
    vsrtp_rx_upd_change_onemore	= 9,	/* Change Update, new == prev + 1    */
    vsrtp_rx_upd_change_greater	= 10,	/* Change Update, new > prev	     */

    vsrtp_rx_upd_full_failed	= 11,	/* Full Update, reassembly failed    */
    vsrtp_rx_upd_full_less	= 12,	/* Full Update, new < prev	     */
    vsrtp_rx_upd_full_equal	= 13,	/* Full Update, new == prev	     */
    vsrtp_rx_upd_full_onemore	= 14,	/* Full Update, new == prev + 1      */
    vsrtp_rx_upd_full_greater	= 15,	/* Full Update, new > prev	     */

    vsrtp_rx_upd_sync_failed	= 16,	/* Sync Update, reassembly failed    */
    vsrtp_rx_upd_sync_less	= 17,	/* Sync Update, new < prev	     */
    vsrtp_rx_upd_sync_equal	= 18,	/* Sync Update, new == prev	     */
    vsrtp_rx_upd_sync_onemore	= 19,	/* Sync Update, new == prev + 1      */
    vsrtp_rx_upd_sync_greater	= 20,	/* Sync Update, new > prev	     */
    vsrtp_rx_upd_max		= vsrtp_rx_upd_sync_greater
} vsrtp_rx_upd_minor;

typedef enum vsrtp_rx_resp_minor_ {	/* index, desc */
    vsrtp_rx_resp_null_failed	= 1,	/* Null Update, reassembly failed    */
    vsrtp_rx_resp_null_less	= 2,	/* Null Update, new < prev           */
    vsrtp_rx_resp_null_equal	= 3,	/* Null Update, new == prev	     */
    vsrtp_rx_resp_null_onemore	= 4,	/* Null Update, new == prev + 1      */
    vsrtp_rx_resp_null_greater	= 5,	/* Null Update, new > prev	     */

    vsrtp_rx_resp_change_failed	= 6,	/* Change Update, reassembly failed  */
    vsrtp_rx_resp_change_less	= 7,	/* Change Update, new < prev	     */
    vsrtp_rx_resp_change_equal	= 8,	/* Change Update, new == prev        */
    vsrtp_rx_resp_change_onemore = 9,	/* Change Update, new == prev + 1    */
    vsrtp_rx_resp_change_greater = 10,	/* Change Update, new > prev	     */

    vsrtp_rx_resp_full_failed	= 11,	/* Full Update, reassembly failed    */
    vsrtp_rx_resp_full_less	= 12,	/* Full Update, new < prev	     */
    vsrtp_rx_resp_full_equal	= 13,	/* Full Update, new == prev	     */
    vsrtp_rx_resp_full_onemore	= 14,	/* Full Update, new == prev + 1      */
    vsrtp_rx_resp_full_greater	= 15,	/* Full Update, new > prev	     */

    vsrtp_rx_resp_sync_failed	= 16,	/* Sync Update, reassembly failed    */
    vsrtp_rx_resp_sync_less	= 17,	/* Sync Update, new < prev	     */
    vsrtp_rx_resp_sync_equal	= 18,	/* Sync Update, new == prev	     */
    vsrtp_rx_resp_sync_onemore	= 19,	/* Sync Update, new == prev + 1      */
    vsrtp_rx_resp_sync_greater	= 20,	/* Sync Update, new > prev	     */
    vsrtp_rx_resp_max		= vsrtp_rx_resp_sync_greater
} vsrtp_rx_resp_minor;

typedef enum _vsrtp_timer_minor_ {
    vsrtp_timer_retry		= 1,	/* Request retry timer fired	     */
    vsrtp_timer_retry_final	= 2,	/* Request retry timer final attempt */
    vsrtp_timer_max		= vsrtp_timer_retry_final
} vsrtp_timer_minor;

/*
 * Guard value for state definition tables in <../vines/vines_srtp_fsm.c>
 * Update value when adding new state events...
 */

#define vsrtp_rx_req_base	(0)
#define vsrtp_rx_upd_base	(vsrtp_rx_req_max)
#define vsrtp_rx_resp_base	(vsrtp_rx_upd_base + vsrtp_rx_upd_max)
#define vsrtp_rx_redir_base	(vsrtp_rx_resp_base + vsrtp_rx_resp_max)
#define vsrtp_rx_reinit_base	(vsrtp_rx_redir_base + 1)
#define vsrtp_rx_timer_base	(vsrtp_rx_reinit_base + 1)
#define VSRTP_MAX_EVENT_NUM	(vsrtp_rx_timer_base + vsrtp_timer_max)


/**********************************************************************
 *
 *			      PROTOTYPES
 *
 **********************************************************************/

/* -- In <vines/vines_srtp_fsm.c> -- */
extern const struct	finite_state_machine vsrtp_fsm_table[];

/* vines_srtp_fsm.c */
extern int vsrtp_fsm_decode(vsrtp_fsm_data *, int, int);
extern int vsrtp_decode_update_major(vsrtp_update *);
extern int vsrtp_decode_update_minor(uchar, int);
extern int vsrtp_decode_request_minor(uchar);
extern char *vsrtp_prstate(vsrtp_state);
extern char *vsrtp_prinput(vsrtp_major, int);
extern ushort vsrtp_fsm(paktype *, vinespathtype *, vsrtp_major, int);

#endif /* __VINES_SRTP_FSM_H__ */
