/* $Id: lec_fsm.h,v 3.3.6.1 1996/05/09 14:32:54 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_fsm.h,v $
 *------------------------------------------------------------------
 * lec_fsm.h -- LAN Emulation Client Finite State Machine
 *
 * September 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_fsm.h,v $
 * Revision 3.3.6.1  1996/05/09  14:32:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.32.1  1996/04/27  07:06:46  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.22.3  1996/04/16  02:44:48  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.22.2  1996/04/08  14:52:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.22.1  1996/04/08  01:57:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.12.1  1996/03/22  22:55:07  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3  1996/03/04  19:26:13  rbadri
 * CSCdi50636:  LANE client lacks a Flush timer
 * Add a flush timer to the LE_ARP entry. When it pops, switch to
 * data direct. If the flush response comes back before the timer pops,
 * then stop the timer and use the data direct VC.
 *
 * Revision 3.2.48.1  1996/03/05  06:43:16  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:34  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:35  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:52  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:12:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/30  00:50:53  bbenson
 * CSCdi39461:  LEC gets into an infinite loop trying to get LES addr
 * - Add an action function to handle resending of the config request to
 *   the LECS on control timeouts.
 *
 * Revision 2.5  1995/08/06  21:59:51  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.4  1995/07/12  07:28:33  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.3  1995/07/11  05:27:30  bbenson
 * CSCdi36864:  No need to display UPDOWN message periodically.
 * CSCdi36904:  LEC should retransmit JOIN requests C13_max_retry_count
 * times.
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes.
 *
 * Revision 2.2  1995/06/30  22:02:19  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.1  1995/06/07  21:20:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LEC_FSM_H__
#define __LEC_FSM_H__

/*
 * States for the LEC state machine.  Note that the enum starts at 1.
 * The fsm_execute() routine assumes that all module states are
 * normalized to 1.  Somebody was a pascal programmer...
 */
typedef enum lec_state_t_ { 
    LEC_ST_IDLE = 1,
    LEC_ST_REGISTER_ADDR,
    LEC_ST_POSTING_LISTEN,
    LEC_ST_GET_LECS_ADDR,
    LEC_ST_LECS_CONNECT,
    LEC_ST_GET_LES_ADDR,
    LEC_ST_LECS_RELEASE,
    LEC_ST_CTL_DIRECT_CONN,
    LEC_ST_JOIN_CTL_DIST_CONN,
    LEC_ST_CTL_DIST_CONN,
    LEC_ST_JOIN,
    LEC_ST_REGISTRATION,
    LEC_ST_GET_BUS_ADDR,
    LEC_ST_MCAST_SEND_FORWARD_CONN,
    LEC_ST_MCAST_SEND_CONN,
    LEC_ST_MCAST_FORWARD_CONN,
    LEC_ST_ACTIVE,
    LEC_ST_TERMINATING,
    MAX_LEC_FSM_STATE = LEC_ST_TERMINATING,
} lec_state_t;

/*
 * LEC predicate vector bits
 */
#define PRED_LEC_NSAP		0x0001
#define PRED_LECS_NSAP		0x0002
#define PRED_CTL_DIRECT_NSAP	0x0004
#define PRED_MCAST_SEND_NSAP	0x0008

/*
 * Minor Categories of Events to the LEC FSM.
 */
typedef enum lec_minor_local_t_ {	/* locally generated events	*/
    LEC_LOCAL_PREFIX_SET = 0,		/* ATM address prefix set	*/
    LEC_LOCAL_CONFIG_CHANGE,		/* Read new config		*/
    LEC_LOCAL_ACTIVATE,			/* Bring up the LEC		*/
    LEC_LOCAL_DEACTIVATE,		/* Bring it down		*/
    LEC_LOCAL_REGISTER_DONE,	/* registration complete - send bus arp */
    MAX_LEC_LOCAL_EVENT,
    LEC_LOCAL_EVENT_BASE = 0,
} lec_minor_local_t;

typedef enum lec_minor_control_t_ {	/* LANE control protocol events	*/
    LEC_CTL_ILMI_SET_RSP_POS = 0,	/* ILMI rsp+ received		*/
    LEC_CTL_ILMI_SET_RSP_NEG,		/* ILMI rsp- received		*/
    LEC_CTL_CONFIG_RSP_POS,		/* LECS cfg rsp+ received	*/
    LEC_CTL_CONFIG_RSP_NEG,		/* LECS cfg rsp- received	*/
    LEC_CTL_JOIN_RSP_POS,		/* LES join rsp+ received	*/
    LEC_CTL_JOIN_RSP_NEG,		/* LES join rsp- received	*/
    LEC_CTL_ARP_REQ,			/* LE-ARP request received	*/
    LEC_CTL_ARP_RSP,			/* LE-ARP response received	*/
    LEC_CTL_NARP_REQ,			/* LE-NARP request received	*/
    LEC_CTL_READY_QUERY,		/* READY_QUERY received		*/
    LEC_CTL_READY_IND,			/* READY_IND received		*/
    LEC_CTL_FLUSH_REQ,			/* FLUSH_REQ received		*/
    LEC_CTL_FLUSH_RSP,			/* FLUSH_RSP received		*/
    LEC_CTL_TOPO_CHANGE,		/* TOPOLOGY change received	*/
    LEC_CTL_REG_RSP_POS,      /* LE_REGISTER rsp+ reeceived */
    LEC_CTL_REG_RSP_NEG,      /* LE_REGISTER rsp- reeceived */

    MAX_LEC_CTL_EVENT,
    LEC_CTL_EVENT_BASE = MAX_LEC_LOCAL_EVENT,
} lec_minor_control_t;

typedef enum lec_minor_signalling_t_ {
    LEC_SIG_SETUP = 0,			/* All of these events are	*/
    LEC_SIG_CONNECT,			/* from the signaling API	*/
    LEC_SIG_CONNECT_ACK,
    LEC_SIG_RELEASE,
    LEC_SIG_RELEASE_COMP,
    LEC_SIG_ADD_PARTY,
    LEC_SIG_DROP_PARTY,
    LEC_SIG_LISTEN_POS,
    LEC_SIG_LISTEN_NEG,
    LEC_SIG_CANCEL,
    MAX_LEC_SIG_EVENT,
    LEC_SIG_EVENT_BASE = MAX_LEC_LOCAL_EVENT +
	                 MAX_LEC_CTL_EVENT,
} lec_minor_network_t;

typedef enum lec_minor_timer_t_ {
    LEC_TIMER_IDLE = 0,			/* LEC idle timer		*/
    LEC_TIMER_CONFIG,			/* LECS config request 		*/
    LEC_TIMER_JOIN,			/* LES join request		*/
    LEC_TIMER_BUS_ARP,			/* BUS LE-ARP request		*/
    LEC_TIMER_READY,			/* READY_QUERY/IND timeout	*/
    LEC_TIMER_ARP_AGE,			/* LE-ARP cache timer		*/
    LEC_TIMER_ARP_CONTROL,		/* LE-ARP cache timer		*/
    LEC_TIMER_REG_REQUEST,              /* LE-REGISTER request timer     */
    LEC_TIMER_FLUSH,			/* Flush timer 			*/
    MAX_LEC_TIMER_EVENT,
    LEC_TIMER_EVENT_BASE = MAX_LEC_LOCAL_EVENT + 
	                   MAX_LEC_CTL_EVENT + 
			   MAX_LEC_SIG_EVENT,
} lec_minor_timer_t;

/*
 * Major Categories of Events to the LEC FSM.
 */
typedef enum lec_major_t_ {
    LEC_MAJOR_LOCAL,			/* Locally generated 		*/
    LEC_MAJOR_CONTROL,			/* LANE control PDUs		*/
    LEC_MAJOR_SIGNALLING,		/* Signalling primitives	*/
    LEC_MAJOR_TIMER,			/* All timer expirations	*/
    MAX_LEC_MAJOR_EVENT,
} lec_major_t;

/*
 * Function prototypes
 */
extern int	lec_fsm_decode(lec_info_t *lec_info, int major, int minor);
extern ushort	lec_fsm(lec_info_t *lec_info, lec_major_t major, int minor);
extern char	*lec_prpred(ulong predicate_vector);
extern char	*lec_pr_atmf_state(int state);

/*
 * LEC state machine action functions
 */
extern void	set_predicate (lec_info_t*, ulong predicate);
extern ushort	a_activate_lec(lec_info_t *lec_info);
extern ushort	a_alloc_lecs_addr(lec_info_t *lec_info);
extern ushort	a_post_listen(lec_info_t *lec_info);
extern ushort	a_post_listen_no_ilmi(lec_info_t *lec_info);
extern ushort	a_process_add_party(lec_info_t *lec_info);
extern ushort	a_process_arp_age_timer(lec_info_t *lec_info);
extern ushort	a_process_arp_control_timer(lec_info_t *lec_info);
extern ushort	a_process_flush_timer(lec_info_t *lec_info);
extern ushort	a_process_arp_rsp(lec_info_t *lec_info);
extern ushort	a_process_narp_req(lec_info_t *lec_info);
extern ushort	a_process_flush_rsp(lec_info_t *lec_info);
extern ushort	a_process_bus_connect(lec_info_t *lec_info);
extern ushort	a_process_cfg_rsp(lec_info_t *lec_info);
extern ushort	a_process_connect(lec_info_t *lec_info);
extern ushort	a_process_connect_ack(lec_info_t *lec_info);
extern ushort	a_process_ctl_dist_setup(lec_info_t *lec_info);
extern ushort  a_process_ctl_dist_setup_send_reg_req(lec_info_t *lec_info);
extern ushort	a_process_failed_listen(lec_info_t *lec_info);
extern ushort	a_process_idle_rel_comp(lec_info_t *lec_info);
extern ushort  a_process_join_rsp_send_req(lec_info_t *lec_info);
extern ushort	a_process_ready_ind(lec_info_t *lec_info);
extern ushort	a_process_ready_query(lec_info_t *lec_info);
extern ushort	a_process_ready_timer(lec_info_t *lec_info);
extern ushort  a_process_reg_rsp(lec_info_t *lec_info);
extern ushort	a_process_release(lec_info_t *lec_info);
extern ushort	a_process_rel_comp(lec_info_t *lec_info);
extern ushort	a_process_setup(lec_info_t *lec_info);
extern ushort	a_process_term_release(lec_info_t *lec_info);
extern ushort	a_process_term_rel_comp(lec_info_t *lec_info);
extern ushort	a_process_topo_change(lec_info_t *lec_info);
extern ushort	a_register_addr(lec_info_t *lec_info);
extern ushort	a_resend_cfg_req(lec_info_t *lec_info);
extern ushort	a_resend_join_req(lec_info_t *lec_info);
extern ushort  a_resend_reg_req(lec_info_t *lec_info);
extern ushort	a_send_arp_rsp(lec_info_t *lec_info);
extern ushort	a_send_bus_arp(lec_info_t *lec_info);
extern ushort	a_send_bus_connect(lec_info_t *lec_info);
extern ushort	a_send_bus_setup(lec_info_t *lec_info);
extern ushort	a_send_cfg_req(lec_info_t *lec_info);
extern ushort	a_send_flush_rsp(lec_info_t *lec_info);
extern ushort	a_send_join_req(lec_info_t *lec_info);
extern ushort	a_send_lecs_setup(lec_info_t *lec_info);
extern ushort	a_send_les_setup(lec_info_t *lec_info);
extern ushort	a_send_next_lecs_setup(lec_info_t *lec_info);
extern ushort	a_set_lec_active(lec_info_t *lec_info);
extern ushort	a_shutdown_lec(lec_info_t *lec_info);
extern ushort	a_teardown_lec(lec_info_t *lec_info);

#endif /* __LEC_FSM_H__ */
