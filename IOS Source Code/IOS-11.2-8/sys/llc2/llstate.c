/* $Id: llstate.c,v 3.8.6.13 1996/09/09 17:17:27 asethi Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llstate.c,v $
 *------------------------------------------------------------------
 * llstate.c -- LLC2 event/state machine
 * 
 * January 1991, Rick Watson
 * 
 * llstate.c    v4.4    Copyright EUCS 1986,1987,1988
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llstate.c,v $
 * Revision 3.8.6.13  1996/09/09  17:17:27  asethi
 * CSCdi60435:  LLC2 crash when retransmitting
 * Branch: California_branch
 *
 * Revision 3.8.6.12  1996/08/27  04:53:19  bchan
 * CSCdi49447:  DSPU router does not resume after RNR/RR
 * Branch: California_branch
 *
 * Revision 3.8.6.11  1996/08/14  22:09:45  kpatel
 * CSCdi46918:  DLSW+ responds to SABME P with UA but no F bit
 * Branch: California_branch
 *
 * Revision 3.8.6.10  1996/08/10  01:04:33  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.8.6.9  1996/08/03  23:33:44  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.8.6.8  1996/07/31  06:48:46  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.8.6.7  1996/07/23  22:10:02  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 *              Using Registry stub.
 * Branch: California_branch
 *
 * Revision 3.8.6.6  1996/07/23  13:26:14  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.6.5  1996/07/23  06:30:10  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/06/21  22:31:51  syiu
 * CSCdi60893:  NETBIOS NBF (NBFCP) does not work to WFW or Win 95 clients
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/06/18  00:45:12  kpatel
 * CSCdi60102:  dlsw ethernet 80d5 bad frame after retransmission
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/05/17  11:27:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.6  1996/04/15  00:09:19  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.6.2.5  1996/04/05  01:38:51  kpatel
 * CSCdi52934:  dlsw ethernet bad frames after retransmission.
 *              bad saps and data
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.4  1996/04/03  20:04:01  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.2.3  1996/03/17  18:07:07  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.2.2  1996/03/01  16:31:18  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.6.2.1  1996/02/28  21:24:24  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.6.1  1996/03/18  21:11:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/16  07:13:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.1  1996/03/13  01:50:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8  1996/03/01  19:03:06  kmoberg
 * CSCdi44347:  SYS-2-SHAREDand tracebacks with DLSw ethernet. LLC2 over
 * tbridge causes a memory leak. Change retbuffer to datagram_done. Force
 * the linktype in DLSw. Don't try to retransmit a pak still in the box.
 *
 * Revision 3.7  1996/03/01  00:49:11  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.6  1996/02/16  18:09:29  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.5  1996/02/15  01:38:14  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.4  1996/01/06  03:28:48  lpereira
 * CSCdi46532:  LLC retransmissions over CLS RSRB crashes the router
 *
 * Revision 3.3  1995/11/17  17:44:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:15:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/25  00:11:27  lpereira
 * CSCdi37113:  appn stop link should send disconnect to end stations
 *
 * Revision 2.5  1995/09/02  07:37:02  pmorton
 * CSCdi36509:  CMNS broken - using wrong MAC address and spurious XID
 * sent on llc2
 * Endstations don't like getting XIDs.  Suppress them.
 *
 * Revision 2.4  1995/08/29  16:18:31  richl
 * CSCdi37525:  No netbios session possible with rsrb local-ack
 *              (window/workgoups). Spec says to send RR after SABME/UA.
 *
 * Revision 2.3  1995/08/03  21:09:19  hampton
 * Convert LLC2 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38203]
 *
 * Revision 2.2  1995/07/06 21:46:52  sberl
 * CSCdi36777:  WFQ should count llc2 sessions as conversations
 * Add logic to classify LINK_LLC2 packets into LLC2 conversations.
 *
 * Revision 2.1  1995/06/07  21:42:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "interface_generic.h"
#include "address.h"
#include "media_registry.h"
#include "../srt/srt_registry.h"
#include "ieee.h"
#include "packet.h"
#include "logger.h"
#include "../cls/clsi_msg.h"
#include "llc2.h"
#include "llstate.h"
#include "../if/network.h"
/*
 * imported from 9.1(8)
 * need to rethink to remove these interdependencies
 */
#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"	/* need to know about major/minor */
#include "../srt/lack2_externs.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"
#include "llc2_inline.h"
#include "lltimers.h"
#include "llc2_event.h"
#include "llc2_sb.h"


/* 
 * The values for the control PDUs - the offsets are defined in llc2.h 
 */
static const unsigned char llc2_cvals[] = {
    SABME,
    DISC,
    XID,
    UA,
    DM,
    XID,
    RR,
    RNR,
    REJ,
    RR,
    RNR,
    REJ,
    RR,
    RNR,
    REJ};

/*
 * Forward declarations
 */
static void llc2_goto_disconnect (llctype *llc);
static void llc2_flow_ctrl_ind (llctype *llc);

/*
 * Check to see if first I or S frame has been received.
 * Function is inserted in various runstates() locations where
 * LLC I & S frames are classified....
 */
static inline void clsContactedTest(llctype *llc)
{
    /*
     * llcRole is set on reception of SABME command.
     * v_r is count of received frames, it is incremented in
     * the post-runstates handlers.  therefore the first i-frame
     * will be indicated in runstates() with a _0_ v_r count.
     */
    if (llc->v_r == 0 && llc->llcRole == LLC2_SECONDARY)
    {
	llc2ClsNotify(llc, CLS_CONNECTED_IND, CLS_OK);
	llc->llcRole = LLC2_SECONDARY_CONTACTED;
    }
}    
	   

/*
 * llc2_flow_ctrl_ind
 *
 * Wrapper to get to the more general Cep Flow Control Indication
 */

static inline void llc2_flow_ctrl_ind (llctype *llc)
{
    CepFlowControlInd(llc->u_cep_id, &llc->fCep);
}

/*
 * d o D i s c C m d A c k ( )
 *
 * tired of always typing in the same code, over and over again!
 * then it's time to use a function, see K&R 2nd Edition, Section 1.7
 *
 * process incoming DISC command from LAN.  Used in various event-based
 * case statements.
 *
 * Inputs:
 *       llc     - ptr to current llc 'object'
 *       ctype   - control type of frame which will be sent in response to
 *                 incoming DISC command
 *
 */
static void doDiscCmdAck (llctype *llc, int ctype)
{
    if (LLC2isCls(llc)) /* CLS code responds after DLU responds */
    {
	switch (ctype)
	{
	case UA_IND:
	    llc->llcDiscAckRspType = LLC2_DISC_RSP_UA;
	    break;

	case DM_IND:
	    llc->llcDiscAckRspType = LLC2_DISC_RSP_DM;
	    break;

	default:
	    llc->llcDiscAckRspType = LLC2_DISC_RSP_NONE;
	    if (LLC2_DEBUG_ERR)
		buginf("\nLLC: Incorrect response type to doDiscCmdAck");
	}  /* endSwitch */
    }
    else			/* legacy code responds immediately */
	send_control(llc, ctype);
    
    llc2_goto_adm(llc, LC_DISC_LAN);
}

    

			    
			    



/*  ***************************************************
    **      event state functions                    **
    **                                               **
    **  the event to be actioned is stored within    **
    **  the link structure                           **
    **  the following values are relevant if the     **
    **  event was the reception of a frame           **
    **    p_f_val       poll/final bit               **
    **    cmdrspbit     1 if RSP frame               **
    **    need_f_bit    1 if poll bit set in CMD     **
    **    f_bit_set     1 if F bit set in RSP        **
    **    new_n_r       latest n_r value             **
    **    new_n_s       latest n_s value             **
    **    inval_n_r     1 if n_r value is invalid    **
    **    inval_n_s     1 if n_s value is invalid    **
    **    unexp_n_s     1 if n_s value is unexpected **
    ***************************************************  */

/*
 * admstate
 */
static void admstate (llctype *llc)
{
    switch (llc->event) {

    case CONN_REQ:
	send_control(llc, SABME_IND);

	llc->state = LLS_SETUP;
	llc->s_flag = 0;
	/*
	 * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
	 * require the setting of P-bit when Tx'ing a SABME frame...
	 * CLS controlled SABME's will also be sent with p-bit set, so start
	 * p-timer to keep track...
	 */
        if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM || 
             LLC2isNBFCP(llc) || LLC2isCls(llc))
	    start_p_timer(llc);
        else
	    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	return;

    case DISC_REQ:
	/*
	 * Disconnect request during initial setup, see ibm tkr ref 12-18
	 * Since no frame is sent, there is no UA 'trigger' for
	 * Disconnect indication.  Send disconnect response NOW
	 * _NOW_!
	 */
	llc->cause_flag = LLC2_Internal_Cause;
	llc2_goto_adm(llc, LC_DISC_CNF);
	return;

    case REC_SABME_CMD:
	/*
	 * Note that if rnr_disable is true we stay in ADM state.
	 * Is this right?  (cire)
	 */
	/*
	 * potentially notify CLS layer of incoming SABME, notify fn returns TRUE
	 * if llc is under CLS control.  Suppress UA for CLS LLC's, as UA
	 * will be sent on Connect.rsp.  Also suppress for legacy (rnr_disable)
	 * case...
	 */
        if (!llc2ClsNotify(llc, CLS_CONNECT_IND, CLS_OK) &&
	    !llc->rnr_disable) {
	    send_control(llc, UA_IND);
	    llc->state = LLS_NORMAL;
        }
	return;




    case TX_DM_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: %s TX_DM_RSP", llc2_prid(llc));

	/* 
	 * station component used to reply to REC_SABME_CMD when
	 * no resources available to accept connection 
	 */
    case REC_DISC_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: %s REC_DISC_CMD", llc2_prid(llc));

	send_control(llc, DM_IND);
	/* state remains ADM */
	return;

    case REC_I_CMD:
    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: %s %s", llc2_prid(llc), llc2_prevent(llc->event));

	if (llc->need_f_bit) 
	    send_control(llc, DM_IND);
#ifdef NOBODY_READS_ME
	else 
	    llc_stats_ignored++;
	/* state remains ADM */
#endif
	return;

    case REC_INV_LSAP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: %s REC_INV_LSAP", llc2_prid(llc));

	/* 
	 * station component used to ignore input frame for which
	 * no matching LSAP has been found 
	 */
    case REC_UA_RSP:
    case REC_DM_RSP:
    case REC_I_RSP:
    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
    case REC_FRMR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: %s %s", llc2_prid(llc), llc2_prevent(llc->event));

#ifdef NOBODY_READS_ME
	llc_stats_ignored++;
	/* state remains ADM */
#endif
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: admstate: state failure, %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	return;
    }
}

   
/*
 * This function releases UA after the end-to-end setup has gone through
 * This is required to implement rnr-disable functionality.
 * When we receive a SABME we hold the UA until the connection 
 * gets established all the way across the routers to the other side 
 * of the cloud.
 * Some equipments like 5494 for 5250 emulation
 * do require them since they are sensitive to 
 * RNR frames and to the BIND rep/resp timings.
 *
 * Object oriented ALERT! ;)  Code re-use in progress...  
 * within CLS/DLC interface code, which also needs to split
 * SABME/UA exchange...
 */
void llc2_release_UA(llctype *llc)
{
    /* 
     * Always set the Final bit for UA; kmoberg HACK alert! This
     * technically wrong but DLSw doesn't pass this info around in
     * some cases. Is there any situation where a SABME is received
     * without the Poll bit set? Don't think so! 
     */
    llc->need_f_bit = 1; 
    send_control(llc, UA_IND);
    llc->state = LLS_NORMAL;
}

/*
 * setupstate
 */
static void setupstate ( llctype *llc)
{
    switch (llc->event) {

    case REC_SABME_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: setupstate: %s REC_SABME_CMD", llc2_prid(llc));

	/* clear sequence, zero retry_count and rem_busy and empty queues */
	zero_v_vals(llc);
	llc->s_flag = 1;	/* Indicates that SABME PDU has been received from
				 * the remote LLC while the local connection component
				 * is in RESET or SETUP state */

	/*
	 * potentially notify CLS layer of incoming SABME, fn returns TRUE
	 * if llc is under CLS control.  Suppress UA for CLS LLC's, as UA
	 * will be sent on Connect.rsp.
	 */
        if (!llc2ClsNotify(llc, CLS_CONNECT_IND, CLS_OK))
	{
	    send_control(llc, UA_IND);
        }
	/* state remains SETUP */
	return;

    case REC_UA_RSP:
	if (llc->p_f_val == llc->p_flag) {
	    llc2_stoptimer(llc, ACK_TIMER_M);

	    /* 
	     * clear sequence, zero retry_count and rem_busy 
	     * and empty queues 
	     */
	    zero_v_vals(llc);
	    update_p_flag(llc);  /* will stop p-timer */
	    llc->state = LLS_NORMAL;
	    /*
	     * According to CLS architecture, Primary link stations
	     * are supposed to open in a busy condition.  A Signal_stn.req
	     * will unbusy the link station
	     */
	    if (llc2ClsNotify(llc, CLS_CONNECT_CNF, CLS_OK))
	    {
		llc2_stateswitch(llc, SET_LOCAL_BUSY);
	    }
	    
	    ind_concnf(llc, LS_SUCCESS); /* IND_toNS to LC_CONCNF */
	    if (!llc->blocking) {
		llc->status_flags |= LLCSF_OPEN;
		llc2_starttimer(llc, STATUS_TIMER, 0);
	    }
            if (llc->rsrb_lack || LLC2isNBFCP(llc)) {
 		send_frame(llc, RR_CMD_P1); 
  		start_p_timer(llc);
	    }
	} 
#ifdef NOBODY_READS_ME
	else 
	    llc_stats_ignored++;
#endif
	return;

    case ACK_TIMER_EXP:
    case P_TIMER_EXP:
	if (llc->s_flag != 0) {

	    llc->p_flag = 0;
	    llc->rem_busy = 0;
	    llc->state = LLS_NORMAL;
	    ind_concnf(llc, LS_SUCCESS); /* IND_toNS to LC_CONCNF */
	    if (!llc->blocking) {
		llc->status_flags |= LLCSF_OPEN;
		llc2_starttimer(llc, STATUS_TIMER, 0);
	    }
	} else {
	    if (llc->retry_count < llc->llc2_n2) {
		send_control(llc, SABME_IND);
		llc->p_flag = 0;
		/*
		 * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
		 * require the setting of P-bit when Tx'ing a SABME frame...
         	 * CLS controlled SABME's will also be sent with p-bit set, so start
		 * p-timer to keep track...
		 */
		if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM ||
                    LLC2isNBFCP(llc) || LLC2isCls(llc))
		    start_p_timer(llc);
                else
		    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
		llc->retry_count++;

		/* state remains SETUP */

	    } else {
		llc2_goto_adm(llc, LC_TIMEOUT);
		ind_concnf(llc, LS_FAILED); /* IND_toNS to LC_CONCNF */
	    }
	}
	return;

    case DISC_REQ:
	/*
	 * Disconnect request during initial setup, see ibm tkr ref 12-18
	 * Since DM is unacknowledged frame, there is no UA 'trigger' for
	 * Disconnect indication.  Force llc2_goto_adm() to send indication
	 * _NOW_!
	 */
	send_control(llc, DM_IND);
	ind_concnf(llc, LS_DISCONNECT); /* IND_toNS to LC_DISC */
	llc2_goto_adm(llc, LC_DISC_CNF);
	return;


    case REC_DISC_CMD:
	doDiscCmdAck(llc, DM_IND);
	ind_concnf(llc, LS_DISCONNECT); /* IND_toNS to LC_DISC */
	return;

    case REC_DM_RSP:
	ind_concnf(llc, LS_DISCONNECT); /* IND_toNS to LC_DM */
	llc2_goto_adm(llc, LC_DM);
	return;

    case REC_I_CMD:
    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
    case REC_I_RSP:
    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
    case REC_FRMR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: setupstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
#ifdef NOBODY_READS_ME
	llc_stats_ignored++;
#endif
	/* state remains SETUP */
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: setupstate: state failure %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	return;
    }
}



/*
 * ind_concnf
 */
void ind_concnf (llctype *llc, int status)
{
    if (LLC2_DEBUG_STATE)
	buginf("\nLLC: Connection established: %s, %s",
	       llc2_prid(llc), llc2_prstatus(status));
    llc->open_block = status;	/* unblock waiting process */
}

/*
 * resetstate
 */
static void resetstate (llctype *llc)
{
    switch (llc->event) {

    case REC_SABME_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: resetstate: %s REC_SABME_CMD", llc2_prid(llc));

	/* clear sequence, zero retry_count and rem_busy and empty queues */
	zero_v_vals(llc);
	llc->s_flag = 1;	/* Indicates that SABME PDU has been received from
				 * the remote LLC while the local connection component
				 * is in RESET or SETUP state */
	/*
	 * potentially notify CLS layer of incoming SABME, fn returns TRUE
	 * if llc is under CLS control.  Suppress UA for CLS LLC's, as UA
	 * will be sent on Connect.rsp.  
	 */
        if (!llc2ClsNotify(llc, CLS_CONNECT_IND, CLS_OK))
	{
	send_control(llc, UA_IND);
        }
	/* state remains RESET */
	return;

    case REC_UA_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: resetstate: REC_UA_RSP");

	if (llc->p_f_val == llc->p_flag) {
	    llc2_stoptimer(llc, ACK_TIMER_M);
	    /* 
	     * clear sequence, zero retry_count and rem_busy 
	     * and empty queues 
	     */
	    zero_v_vals(llc);
	    update_p_flag(llc);
	    llc->state = LLS_NORMAL;
	}
#ifdef NOBODY_READS_ME
	else 
	    llc_stats_ignored++;
#endif
	return;

    case P_TIMER_EXP:
    case ACK_TIMER_EXP:
	if (llc2_debug_test)
	    buginf("\nLLC: resetstate: ack or p timer_exp");

	if (llc->s_flag != 0) {
	    llc->p_flag = 0;
	    llc->rem_busy = 0;
	    llc->state = LLS_NORMAL;
	}
	else {
	    if (llc->retry_count < llc->llc2_n2) {
		send_control(llc, SABME_IND);
		llc->p_flag = 0;
		/*
		 * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
		 * require the setting of P-bit when Tx'ing a SABME frame...
		 * CLS controlled SABME's will also be sent with p-bit set, so start
		 * p-timer to keep track...
		 */
		if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM || 
                    LLC2isNBFCP(llc) || LLC2isCls(llc))
		    start_p_timer(llc);
                else
		    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
		llc->retry_count++;
		/* state remains RESET */
	    } else
		llc2_goto_adm(llc, LC_TIMEOUT);
	}
	return;

    case DISC_REQ:
	/*
	 * Disconnect request during initial setup, see ibm tkr ref 12-18
	 * Since DM is unacknowledged frame, there is no UA 'trigger' for
	 * Disconnect indication.  Force llc2_goto_adm() to send indication
	 * _NOW_!
	 */
	send_control(llc, DM_IND);
	llc2_goto_adm(llc, LC_DISC_CNF);
	return;

    case REC_DISC_CMD:
	doDiscCmdAck(llc, DM_IND);
	return;

    case REC_DM_RSP:
	llc2_goto_adm(llc, LC_DM);
	return;

    case REC_I_CMD:
    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
    case REC_I_RSP:
    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
    case REC_FRMR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: resetstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
#ifdef NOBODY_READS_ME
	llc_stats_ignored++;
#endif
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: resetstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}




/*
 * d_connstate
 */
static void d_connstate (llctype *llc)
{
    switch (llc->event) {

    case REC_SABME_CMD:
	  /*
	   * field controls what type of acknowledgement will be sent
	   * when Disconnect.rsp is received from DLU/CLS.  According to
	   * IEEE 802.2-1985, Table 7-1, no response to SABME is required.
	   */
	send_control(llc, DM_IND);
	llc2_goto_adm(llc, LC_DISC_CNF);
	return;

    case REC_UA_RSP:
	if (llc->p_f_val != llc->p_flag) {
#ifdef NOBODY_READS_ME
	    llc_stats_ignored++;
#endif
	    return;
	}
	llc2_goto_adm(llc, LC_DISC_CNF); /* confirm disconnect is actually complete */
	return;

    case REC_DM_RSP:
	llc->llcDiscAckRspType = LLC2_DISC_RSP_NONE; /* see comment for case REC_SABME_CMD */
	llc2_goto_adm(llc, LC_DM);
	return;

    case DISC_REQ:			/* silently ignore, we will get there */
	break;

    case REC_DISC_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: d_connstate: %s REC_DISC_CMD", llc2_prid(llc));
	doDiscCmdAck(llc, UA_IND);
	/* state remains D_CONN, except for SDLLC */
	return;

    case REC_I_CMD:
    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
    case REC_I_RSP:
    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
    case REC_FRMR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: d_connstate: %s %s (%d", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
#ifdef NOBODY_READS_ME
	llc_stats_ignored++;
#endif
	return;

    case P_TIMER_EXP:
    case ACK_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	      llc2_goto_disconnect(llc);
	    /* state remains D_CONN */
	} else
	    /*
	     * Use seperate 'timeout' number to prevent infinite
	     * timeout loop for cls controlled interfaces
	     *  prevent the following...
	     *     1.  n2 time-out signals llc2_goto_adm(LC_TIMEOUT)
	     *     2.  in goto_adm(), setstate = LLS_D_CONN
	     *     3.  in LLS_D_CONN, Tx of DISC_CMD times out,
	     *     go back to #1, in infinite loop...
	     * therefore, use different error code in #3, (not LC_TIMEOUT)
	     * and check in llc2_goto_adm...
	     */
	    llc2_goto_adm(llc, LC_DISC_TIMEOUT);
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: d_connstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}


/*
 * errorstate
 */
static void errorstate (llctype *llc)
{
    switch (llc->event) {

    case REC_SABME_CMD:
	/*
	 * clear sequence, zero retry_count and rem_busy and empty queues
	 */
	zero_v_vals(llc);
	send_control(llc, UA_IND);
	llc->p_flag = 0;
	llc2_stoptimer(llc, ACK_TIMER_M);
	llc->state = LLS_NORMAL;
	return;

    case DISC_REQ:
	send_control(llc, DM_IND);
	llc2_goto_adm(llc, LC_DISC_CNF);
	return;

    case REC_DISC_CMD:
	doDiscCmdAck(llc, UA_IND);
	return;

    case REC_DM_RSP:
	llc2_goto_adm(llc, LC_DM);
	return;

    case REC_FRMR_RSP:
	send_control(llc, SABME_IND);
	llc->s_flag = 0;
	llc->p_flag = 0;
	llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	llc->retry_count = 0;
	llc->cause_flag = LLC2_External_Cause;
	llc->state = LLS_RESET;
	llc->status_flags |= LLCSF_RESET;
	llc2_starttimer(llc, STATUS_TIMER, 0);
	return;

    case REC_I_CMD:
    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: errorstate: %s %s (%d), sending frmr",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	send_FRMR(llc);
	/* state remains ERROR */
	return;

    case REC_UA_RSP:
    case REC_I_RSP:
    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: errorstate: %s %s (%d), ignored", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
#ifdef NOBODY_READS_ME
	llc_stats_ignored++;
#endif
	return;

    case P_TIMER_EXP:
    case ACK_TIMER_EXP:
	if (llc2_debug_test)
	    buginf("\nLLC: errorstate: %s ack or p timer_exp", llc2_prid(llc));
	if (llc->retry_count < llc->llc2_n2) {
	    send_FRMR(llc);
	    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	    llc->retry_count++;
	    /* state remains ERROR */
        } else {
	    send_control(llc, SABME_IND);
	    llc->s_flag = 0;
	    llc->p_flag = 0;
	    /*
	     * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
	     * require the setting of P-bit when Tx'ing a SABME frame...
	     * CLS controlled SABME's will also be sent with p-bit set, so start
	     * p-timer to keep track...
	     */
	    if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM || 
                LLC2isNBFCP(llc) || LLC2isCls(llc))
		start_p_timer(llc);
            else
		llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	    llc->retry_count = 0;
	    llc->cause_flag = LLC2_External_Cause;
	    llc->status_flags |= LLCSF_RESET;
	    llc2_starttimer(llc, STATUS_TIMER, 0);
	    llc->state = LLS_RESET;
	}
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: errorstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}


/*
 * runstates
 * returns 0 if the event has been actioned
 * returns 1 if event not yet recognised
 */
static int runstates (llctype *llc)
{
    switch (llc->event) {

    case DISC_REQ:
	  llc->cause_flag = LLC2_Internal_Cause;
	  llc2_goto_disconnect(llc);
	return(0);

    case RESET_REQ:
	send_control(llc, SABME_IND);
	llc->s_flag = 0;    /* must be cleared before entry to reset state */
	llc->p_flag = 0;
	stop_other_timers(llc);
	/*
	 * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
	 * require the setting of P-bit when Tx'ing a SABME frame...
	 * CLS controlled SABME's will also be sent with p-bit set, so start
	 * p-timer to keep track...
	 */
        if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM || 
            LLC2isNBFCP(llc) || LLC2isCls(llc))
	    start_p_timer(llc);
        else
	    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	llc->retry_count = 0;
	llc->cause_flag = LLC2_Internal_Cause;
	llc->status_flags |= LLCSF_RESET;
	llc2_starttimer(llc, STATUS_TIMER, 0);
	llc->state = LLS_RESET;
	return(0);

    case REC_SABME_CMD:
        /* NmEvent Hook 1 */        
        LLC2Event(llc, SABMEReceivedWhileConnected);

	llc2ClsNotify(llc, CLS_CONNECT_IND, CLS_OK);
	/*
	 * clear sequence, zero retry_count and rem_busy and empty queues
	 */
	zero_v_vals(llc);
	send_control(llc, UA_IND);
	llc->p_flag = 0;
	stop_all_timers(llc);
	llc->state = LLS_NORMAL;
	return(0);

    case REC_DISC_CMD:
	  doDiscCmdAck(llc, UA_IND);
	return(0);

    case REC_DM_RSP:
	llc2_goto_adm(llc, LC_DM);

        /* NmEvent Hook 4 */
        LLC2Event(llc, DMReceived); 
	return(0);

    case P_TIMER_EXP:
    case ACK_TIMER_EXP:
    case REJ_TIMER_EXP:
    case BUSY_TIMER_EXP:
	llc->cause_flag = LLC2_External_Cause;
	if (llc2_debug_test && !llc2_debug_packet)
	    buginf("\nLLC (rs): runstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->retry_count < llc->llc2_n2) 
	    return(1);				/* no action yet */
	/*
	 * If N2 is exceeded then drop into next case
	 */
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC (rs): n2 exceeded! %s %s  (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);

	  /*
	   * Handle 'delicate' situation of link station timeouts
	   * for CLS LLCs...
	   * If in D_CONN, and receive a timeout, it is because no
	   *    response to repeated DISC commands.  Indicate to DLU,
	   *    we're DISC'd because of timeout...
	   *
	   * Otherwise, this the link station is indicating an N2 timeout,
	   *    for example in normal data transfer mode...
	   */
	  /* (general n2 timeout error)
	   * if cls interface, also need to send DISC command out to wire
	   * ibm tkr ref 12-23
	   * legacy code simply goes to adm states and dies...
	   *
	   * On n2 timeout LLC sends DISC on it's own and generates a
	   * Disconnect.ind. (which is a bit different from 'normal' behavior
	   * when processing a DISC_REQ on a Disconnect.req).  After another
	   * n2 timeout on the DISC's, we signal LC_DISC_TIMEOUT to
	   * determione whether a Disconnect.cnf is required.
	   * So for external Disconnects (timeouts) we refrain from
	   * generating Disconnect.cnf's on things like timeouts, and
	   * acknowledgements (i.e. DM or UA).  For internal disconnects
	   * (disconnect.ind) we use these timeout and ack events to
	   * trigger confirmation messages...
	   *
	   */

          /* NmEvent Hook 5 - link has been lost for all LLC's*/
          LLC2Event(llc, AckTimerExpired);
	  if (LLC2isCls(llc))
	  {
	      /* CLS LLC's will handle timeout for 'pre-connect states'
	       * (adm, reset, d_conn and setup) in state handlers
	       * other 'runtime' states are handled here
	       */
	      if (llc->state < LLS_ERROR)
		  return(1);	/* event has been not been actioned... */
	      else
	      {
		  llc->llcDiscAckRspType = LLC2_DISC_RSP_NONE;
		  llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR);
		  llc->cause_flag = LLC2_External_Cause;
		  llc2_goto_disconnect(llc);
	      }
	      return(0);	/* event has been actioned... */
	  }

	if (llc->l2_lakptr || llc->sdllc_active || llc->lnx_active) {
	    llc2_goto_adm (llc, LC_TIMEOUT);
	    return(0);
	}

	  /*
	   * intentional fall-through...  IEEE 802.2-1985 specifies that timer
	   * expiry -or- reception of FRMR could send SABME and restart connection.
	   * See Normal-Busy-Reject... state entry in Table 7-1...
	   * Exception code above (in timer expiry section) modifies this behavior so
	   * SDLLC and LACK connections are terminated and not restarted.
	   * The fall through would only occur for non-[SDLLC, LACK, CLS] LLC's.  At
	   * this time (05/23/94), only other LLC client is LAN Net Manager...
	   */
    case REC_FRMR_RSP:
          /* NmEvent Hook 2 - Link has been lost for all LLC's */
          if (llc->event == REC_FRMR_RSP) {
              llc->frmrchars[FRMR_WXYZ] = 
                              *((llc->inbuf)->datagramstart + 7);
              LLC2Event(llc, FRMRreceived);
	   }
	  if (llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR))
	  {	    
	      /*
	       * Rx of FRMR signals disconnect, ibm tkr ref 12-64
	       */
	      llc2_goto_disconnect(llc);
	      return(0);		/* event has been actioned */
	  }	    

	/*
	 * Local ACK and SDLLC don't want to restart the session, for sure.
	 * Are we really sure about this?  (cire)
	 */
	if (llc->l2_lakptr || llc->sdllc_active || llc->lnx_active) {
	    llc2_goto_adm (llc, LC_FRMR);
	    return(0);
	}
	send_control(llc, SABME_IND);
	llc->s_flag = 0;/* must be cleared before entry to RESET state */
	llc->p_flag = 0;
	stop_other_timers(llc);
	/*
	 * Remote Source-Route bridging and IBM Network Management (CSCdi07429)
	 * require the setting of P-bit when Tx'ing a SABME frame...
	 * CLS controlled SABME's will also be sent with p-bit set, so start
	 * p-timer to keep track...
	 */
        if (llc->rsrb_lack || llc2_get_rsap(llc) == SAP_IBMNM || 
            LLC2isNBFCP(llc) || LLC2isCls(llc))
	    start_p_timer(llc);
        else
	    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	llc->retry_count = 0;
	llc->cause_flag = LLC2_External_Cause;
	llc->status_flags |= LLCSF_RESET;
	llc2_starttimer(llc, STATUS_TIMER, 0);
	llc->state = LLS_RESET;
	return(0);

    case REC_UA_RSP:
    case REC_BAD_PDU:
    case REC_BAD_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC (rs): %s %s (%d), inval_recv", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	inval_recv(llc, FRMR_WBIT);
	return(0);

    case REC_INV_ILEN:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC (rs): %s rec_inv_ilen", llc2_prid(llc));
	inval_recv(llc, FRMR_YBIT);
	return(0);

    case REC_INV_IFIELD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC (rs): %s rec_inv_ifield", llc2_prid(llc));
	inval_recv(llc, FRMR_WBIT+FRMR_XBIT);
	return(0);

    case REC_I_CMD:
	clsContactedTest(llc);	/* send CLS CLS_CONNECTED_IND msg if necessary */
	if (LLC2_DEBUG_PACKET)
	    buginf("\nLLC (rs): %s %s N(S)=%d V(R)=%d", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->new_n_s>>1, llc->v_r>>1);
	if (llc->inval_n_s) {
	    inval_recv(llc, FRMR_VBIT+FRMR_WBIT);
	    return(0);
	}

	/* drop down into next cases */

    case REC_RR_CMD:
    case REC_RNR_CMD:
    case REC_REJ_CMD:
	clsContactedTest(llc);	/* send CLS CLS_CONNECTED_IND msg if necessary */
	if (LLC2_DEBUG_PACKET)
	    buginf("\nLLC (rs): %s %s N(R)=%d",  llc2_prid(llc),
		   llc2_prevent(llc->event), llc->new_n_r);
	if (llc->inval_n_r) {
	    inval_recv(llc, FRMR_ZBIT);
	    return(0);
	}
	return(1);			/* further examination required */

    case REC_I_RSP:
	clsContactedTest(llc);	/* send CLS CLS_CONNECTED_IND msg if necessary */
	if (LLC2_DEBUG_PACKET)
	    buginf("\nLLC (rs): %s REC_I_RSP N(S)=%d V(R)=%d", llc2_prid(llc), 
		   llc->new_n_s>>1, llc->v_r>>1);
	if (llc->inval_n_s) {
	    inval_recv(llc, FRMR_VBIT+FRMR_WBIT);
	    return(0);
	}

	/* drop down into next cases */

    case REC_RR_RSP:
    case REC_RNR_RSP:
    case REC_REJ_RSP:
	clsContactedTest(llc);	/* send CLS CLS_CONNECTED_IND msg if necessary */
	if (llc->inval_n_r) {
	    inval_recv(llc, FRMR_ZBIT);
	    return(0);
	}
	/* Do a timer sanity check here. */
	if ((llc->p_f_val != 0) && (llc->p_flag == 0)) {
	    if (llc->llc2_T1_time < 100) {
		llc->llc2_T1_time = T1_DEFAULT;
	    }
	    if (llc->llc2_trej_time < 100) {
		llc->llc2_trej_time = TREJ_DEFAULT;
	    }
	    if (llc->llc2_tpf_time < 100) {
		llc->llc2_tpf_time = TPF_DEFAULT;
	    }
	    if (llc->llc2_tbusy_time < 100) {
		llc->llc2_tbusy_time = TBUSY_DEFAULT;
	    }
	    if (llc->llc2_idle_time < 100) {
		llc->llc2_idle_time = IDLE_DEFAULT;
	    }
	    if (llc->llc2_xid_rtry_time < 100) {
		llc->llc2_xid_rtry_time = XID_RTRY_TIME;
	    }
	    if (llc->llc2_xid_neg_val_time < 100) {
		llc->llc2_xid_neg_val_time = XID_NEG_TIME;
	    }
	    /* Toss the frame... */
	    return(0);
	}
	return(1);			/* further examination required */
    }

    /* defaults will be examined later if return value was 1 */

    return(1);
}


/*
 * inval_recv
 */
static void inval_recv ( llctype *llc, int vwxyz)
{
    uchar *frameptr;
    paktype *pak;

    /* 
     * set up the FRMR information field 
     */
    pak = llc->inbuf;
    frameptr = pak->datagramstart + pak->llc2_sapoffset;
    llc->frmrchars[0] = *(frameptr+2);      /* 0 first control character */
    if ((*(frameptr+2) & 3) == 3) 		    /* if unnumbered */
	llc->frmrchars[1] = 0;					/* 1 */
    else 
	llc->frmrchars[1] = *(frameptr+3);			/* 1 */
    llc->frmrchars[FRMR_V_S] = llc->v_s;			/* 2 */
    llc->frmrchars[FRMR_V_R] = llc->v_r + llc->cmdrspbit;       /* 3 */
    llc->frmrchars[FRMR_WXYZ] = vwxyz;                          /* 4 */

    /* NmEvent Hook 3 */
    LLC2Event(llc, FRMRsent);
    send_FRMR(llc);

    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
    stop_other_timers(llc);
    llc->retry_count = 0;
    llc->state = LLS_ERROR;
}



/*
 * normalstate
 */
static void normalstate (llctype *llc)
{
    boolean rem_busy;

    switch (llc->event) {

    case SET_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: normalstate: set_local_busy %s", llc2_prid(llc));

        if (!(llc->fFlags & LLC_NO_RNR))
            send_frame(llc, RNR_CMD_P0);
	llc->data_flag = 0;
	llc->state = LLS_BUSY;
	return;

	/*
	 * case CLEAR_LOCAL_BUSY:
	 *
	 * Do we want to make this harmless?  Note a Rem Wait node will
	 * go into local busy and then when going from RW to CP will clear
	 * local busy.  It will be in BUSY state because of the set so we
	 * won't be here.  But the local wait side never goes busy so it
	 * is normal vs. busy.  but the connect pending to connect lack2
	 * transition (which also happens on the Rem Wait side) has to
	 * clear local busy.  This causes an erroneous debugging statement
	 * about a state failure.
	 */

    case REC_I_CMD:
    case REC_I_RSP:
	if (llc->f_bit_set)
	    clear_rem_busy(llc);
	if (llc->unexp_n_s) {
	    if (llc->event == REC_I_CMD)
		send_frame(llc, REJ_RSP);
	    else
		send_frame(llc, REJ_CMD_P0);
	    update_n_r(llc);
	    update_p_flag(llc);
	    llc2_starttimer(llc, REJ_TIMER, llc->llc2_trej_time);
	    llc->state = LLS_REJECT;
	} else {
	    /* incrementing V(R) done in indinfo */
	    indinfo(llc);
	    update_p_flag(llc);
	    update_n_r(llc);
	    /*
	     * update_n_r and clear_rem_busy are executed before
	     * send_ack_xxx to ensure that any I PDU eligible for
	     * transmission will be sent instead of RR when acknowledging   
	     */ 
	    send_ack_xxx(llc);

	    /* state remains NORMAL */

	}
	return;

    case REC_RR_CMD:
    case REC_RR_RSP:
	update_p_flag(llc);
	update_n_r(llc);
        rem_busy = llc->rem_busy;
	clear_rem_busy(llc);
	/*
	 * update_n_r and clear_rem_busy are executed before send_ack_xxx to
	 * ensure that any I PDU eligible for transmission will be sent
	 * instead of RR when acknowledging
	 */ 
	if (llc->need_f_bit) 
	    send_ack_xxx(llc);

        if (rem_busy && llc->v_s != llc->new_n_r) {
           /* remote was busy and not all frames acknowledged, send a poll
            * to find out.
            */
	    send_frame(llc, RR_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;
	    llc->state = LLS_AWAIT;
        }
	/* else state remains NORMAL */
	return;

    case REC_RNR_CMD:
    case REC_RNR_RSP:
	if (llc->need_f_bit) 
	    send_frame(llc, RR_RSP);
	update_p_flag(llc);
	update_n_r(llc);
	set_rem_busy(llc);

	/* state remains NORMAL */

	return;

    case REC_REJ_CMD:
    case REC_REJ_RSP:
	/* v_s set to new_n_r by resend_i_xxx calling re_q_akchain */
	update_n_r(llc);
	update_p_flag(llc);
	clear_rem_busy(llc);
	/*
	 * clear_rem_busy called before resend_i_xxx because send_i_xxx may be
	 * called which examines rem_busy
	 */
	resend_i_xxx(llc);
	/* state remains NORMAL */
	return;

    case INIT_PF_CYCLE:
	if (llc->p_flag == 0) {
	    send_frame(llc, RR_CMD_P1);
	    start_p_timer(llc);
	    /* state remains NORMAL */
	}
	return;

    case ACK_TIMER_EXP:
    case BUSY_TIMER_EXP:
	if (llc->p_flag != 0) 
	    return;

	/* else drop down into next case */

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    send_frame(llc, RR_CMD_P1);
	    /* reset_v_s(llc); */
	    start_p_timer(llc);
	    llc->retry_count++;
	    llc->state = LLS_AWAIT;
	}
	/* else retry_count >= llc->llc2_n2 dealt with in runstates */
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: normalstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}

/*
 * busystate
 */
static void busystate (llctype *llc)
{
    switch (llc->event) {

    case SET_LOCAL_BUSY:
	/*
	 * we are already busy.  just ignore silently.
	 */
	return;

    case CLEAR_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: busystate: %s local busy cleared",
		   llc2_prid(llc));
	if (llc->data_flag == 1) {
	    send_frame(llc, REJ_CMD_P0);
	    llc2_starttimer(llc, REJ_TIMER, llc->llc2_trej_time);
	    llc->state = LLS_REJECT;
	} else {
	    send_frame(llc, RR_CMD_P0);
	    if (llc->data_flag == 0)
		llc->state = LLS_NORMAL;
	    else		/* llc->data_flag == 2 */
		llc->state = LLS_REJECT;
	}
	return;

    case REC_I_CMD:
    case REC_I_RSP:
	/*
	 * case where (llc->p_f_val != 0) && (llc->p_flag == 0)
	 * on a RSP frame has already been dealt with
	 */
	if (llc->event == REC_I_CMD) 
	    send_frame(llc, RNR_RSP);
	else 
	    send_frame(llc, RNR_CMD_P0);
	update_p_flag(llc);
	update_n_r(llc);
	if (llc->unexp_n_s) {
	    if (llc->data_flag == 0) 
		llc->data_flag = 1;
	} else {
	    if (llc->data_flag == 2) 
		llc2_stoptimer(llc, REJ_TIMER_M);
	    llc->data_flag = 1;
	}
	if (llc->f_bit_set)
	    clear_rem_busy(llc);
	return;				/* remain BUSY */

    case REC_RR_CMD:
    case REC_RR_RSP:
	if (llc->need_f_bit) 
	    send_frame(llc, RNR_RSP);
	update_p_flag(llc);
	update_n_r(llc);
        if (llc->rem_busy && llc->v_s != llc->new_n_r) {
           /* remote was busy and not all frames acknowledged, send a poll
            * to find out.
            */
	    send_frame(llc, RNR_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;
	    llc->state = LLS_AWAIT_BUSY;
	}
	clear_rem_busy(llc);
	return;				/* remain BUSY */

    case REC_RNR_CMD:
    case REC_RNR_RSP:
	if (llc->need_f_bit) 
	    send_frame(llc, RNR_RSP);
	update_p_flag(llc);
	update_n_r(llc);
	set_rem_busy(llc);
	return;				/* remain BUSY */

    case REC_REJ_CMD:
    case REC_REJ_RSP:
	/* v_s set to new_n_r by resend_i_xxx calling re_q_akchain */
	update_n_r(llc);
	update_p_flag(llc);
	if (llc->need_f_bit) {
	    send_frame(llc, RNR_RSP);
	    /* N.B.  need_f_bit now set to zero */
	}
	clear_rem_busy(llc);
	/*
	 * clear_rem_busy called before resend_i_xxx because send_i_xxx may be
	 * called which examines rem_busy
	 */
	resend_i_xxx(llc);
	return;				/* remain BUSY */

    case INIT_PF_CYCLE:
	if (llc->p_flag == 0) {
	    send_frame(llc, RNR_CMD_P1);
	    start_p_timer(llc);
	}
	return;				/* remain BUSY */

    case REJ_TIMER_EXP:
	llc->data_flag = 1;

	/* drop down into next case */

    case ACK_TIMER_EXP:
    case BUSY_TIMER_EXP:
	if (llc->p_flag != 0) 
	    return;

	/* else drop down into next case */

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    send_frame(llc, RNR_CMD_P1);
	    /* reset_v_s(llc); */
	    start_p_timer(llc);
	    llc->retry_count++;
	    llc->state = LLS_AWAIT_BUSY;
	}

	/* else retry_count >= llc->llc2_n2 dealt with in runstates */

	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: busystate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}

/*
 * rejectstate
 */
static void rejectstate (llctype *llc)
{
    boolean rem_busy;
    switch (llc->event) {

    case SET_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: rejectstate: %s local busy set",
		   llc2_prid(llc));
	send_frame(llc, RNR_CMD_P0);
	llc->data_flag = 2;
	llc->state = LLS_BUSY;
	return;

    case REC_I_CMD:
    case REC_I_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: rejectstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	update_n_r(llc);
	update_p_flag(llc);
	if (llc->f_bit_set)
	    clear_rem_busy(llc);

	/* 
	 * update_n_r and clear_rem_busy are executed before
	 * send_ack_xxx to ensure that any I PDU eligible for
	 * transmission will be sent instead of RR when acknowledging
	 */ 
	if (llc->unexp_n_s) {
	    if (llc->need_f_bit) send_frame(llc, RR_RSP);
	    /* state remains REJECT */
	} else {
	    /* incrementing V(R) done in indinfo */
	    indinfo(llc);
	    send_ack_xxx(llc);
	    llc2_stoptimer(llc, REJ_TIMER_M);
	    llc->state = LLS_NORMAL;
	}
	return;

    case REC_RR_CMD:
    case REC_RR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: rejectstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	update_p_flag(llc);
	update_n_r(llc);
        rem_busy = llc->rem_busy;
	clear_rem_busy(llc);
	/* 
	 * update_n_r and clear_rem_busy are executed before
	 * send_ack_xxx to ensure that any I PDU eligible for
	 * transmission will be sent instead of RR when acknowledging   
	 */ 
	if (llc->need_f_bit) 
	    send_ack_xxx(llc);
        if (rem_busy && llc->v_s != llc->new_n_r) {
           /* remote was busy and not all frames acknowledged, send a poll
            * to find out.
            */
	    send_frame(llc, RR_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;
	    llc->state = LLS_AWAIT_REJ;
	}
	/* else state remains REJECT */
	return;

    case REC_RNR_CMD:
    case REC_RNR_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: rejectstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->need_f_bit) 
	    send_frame(llc, RR_RSP);
	update_p_flag(llc);
	update_n_r(llc);
	set_rem_busy(llc);
	/* state remains REJECT */
	return;

    case REC_REJ_CMD:
    case REC_REJ_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: rejectstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);

	/* v_s set to new_n_r by resend_i_xxx calling re_q_akchain */
	update_n_r(llc);
	update_p_flag(llc);
	clear_rem_busy(llc);
	/* 
	 * clear_rem_busy called before resend_i_xxx because
	 * send_i_xxx may be called which examines rem_busy 
	 */
	resend_i_xxx(llc);
	/* state remains REJECT */
	return;

    case INIT_PF_CYCLE:
	if (llc->p_flag == 0) {
	    send_frame(llc, RR_CMD_P1);
	    start_p_timer(llc);
	    /* state remains REJECT */
	}
	return;

    case REJ_TIMER_EXP:
    case ACK_TIMER_EXP:
    case BUSY_TIMER_EXP:
	if (llc->p_flag != 0) 
	    return;

	/* drop down into next case */

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    if (llc->event == REJ_TIMER_EXP) {
		send_frame(llc, REJ_CMD_P1);
		/* state remains REJECT */
	    } else {
		send_frame(llc, RR_CMD_P1);
		/* reset_v_s(llc); */
		llc->state = LLS_AWAIT_REJ;
	    }
	    start_p_timer(llc);
	    llc2_starttimer(llc, REJ_TIMER, llc->llc2_trej_time);
	    llc->retry_count++;

	    /* state remains REJECT */

	}
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: rejectstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}

/*
 * awaitstate
 */
static void awaitstate (llctype *llc)
{
    switch (llc->event) {

    case SET_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: awaitstate: %s local busy set",
		   llc2_prid(llc));
	send_frame(llc, RNR_CMD_P0);
	llc->data_flag = 0;
	llc->state = LLS_AWAIT_BUSY;
	return;

    case REC_I_CMD:
    case REC_I_RSP:
	if (llc->unexp_n_s) {
	    if (llc->event == REC_I_CMD) 
		send_frame(llc, REJ_RSP);
	    else 
		send_frame(llc, REJ_CMD_P0);
	    update_n_r(llc);
	    /* update_v_s(llc); */
	    llc2_starttimer(llc, REJ_TIMER, llc->llc2_trej_time);
	    if (llc->p_f_val != 0) {
		if (llc->event != REC_I_CMD) {
		    stop_p_timer(llc);
		    clear_rem_busy(llc);
		    /*
		     * clear_rem_busy called before resend_i_xxx because
		     * send_i_xxx may be called which examines rem_busy.
		     */
		    resend_i_xxx(llc);
		    llc->state = LLS_REJECT;
		    return;
		}
	    }
	    llc->state = LLS_AWAIT_REJ;
	    return;
	}
	else {
	    /* incrementing V(R) done in indinfo */
	    indinfo(llc);
	    update_n_r(llc);
	    /* update_v_s(llc); */
	    if (llc->event == REC_I_CMD)
		send_frame(llc, RR_RSP);
	    else {
		if (llc->p_f_val != 0) {
		    stop_p_timer(llc);
		    clear_rem_busy(llc);
		    /*
		     * clear_rem_busy called before resend_i_xxx because
		     * send_i_xxx may be called which examines rem_busy
		     */
		    if (resend_i_xxx(llc) == 0) {
			/* unable to retransmit an I frame */
			send_frame(llc, RR_CMD_P0);
		    }
		    llc->state = LLS_NORMAL;
		} else 
		    send_frame(llc, RR_CMD_P0);
	    }
	    /* state remains LLS_AWAIT for CMD and RSP with F=0 */
	}
	return;

    case REC_RR_RSP:
    case REC_REJ_RSP:
    case REC_RR_CMD:
    case REC_REJ_CMD:
	if (llc->need_f_bit) 
	    send_frame(llc, RR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	clear_rem_busy(llc);
	/* clear_rem_busy called before resend_i_xxx because
	   send_i_xxx may be called which examines rem_busy */
	if (llc->f_bit_set) {
	    /* RSP frame with F bit set */
	    stop_p_timer(llc);
	    resend_i_xxx(llc);
	    llc->state = LLS_NORMAL;
	}

	/* else state remains AWAIT */

	return;

    case REC_RNR_CMD:
    case REC_RNR_RSP:
	if (llc->need_f_bit) send_frame(llc, RR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	set_rem_busy(llc);
	/*
	 * set_rem_busy called before re_q_akchain to ensure
	 * that I frames are not erroneously transmitted
	 */
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    re_q_akchain(llc);
	    llc->state = LLS_NORMAL;
	}

	/* state remains AWAIT for CMD and RSP with F=0 */

	return;

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    send_frame(llc, RR_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;

	    /* state remains AWAIT */

	}
	return;

    case INIT_PF_CYCLE:
	/*
	 * INIT_PF_CYCLE is launched from the llc2_ticks because it
	 * thinks we are idle.  While stictly speaking we aren't
	 * idle when we are in AWAIT but llc2_ticks doesn't know
	 * this.  This should get cleaned up.  (cire).
	 */
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: awaitstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}


/*
 * aw_busystate
 */
static void aw_busystate (llctype *llc)
{
    switch (llc->event) {

    case SET_LOCAL_BUSY:
	/*
	 * we are already busy.  just ignore silently.
	 */
	return;

    case CLEAR_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: aw_busystate: %s local busy cleared",
		   llc2_prid(llc));
	if (llc->data_flag == 1) {
	    send_frame(llc, REJ_CMD_P0);
	    llc2_starttimer(llc, REJ_TIMER, llc->llc2_trej_time);
	    llc->state = LLS_AWAIT_REJ;
	} else {
	    send_frame(llc, RR_CMD_P0);
	    if (llc->data_flag == 0) 
		llc->state = LLS_AWAIT;
	    else 
		llc->state = LLS_AWAIT_REJ;
	}
	return;

    case REC_I_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_busystate: %s REC_I_RSP", llc2_prid(llc));
	send_frame(llc, RNR_CMD_P0);
	update_n_r(llc);
	/* update_v_s(llc); */
	llc->data_flag = 1;
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    clear_rem_busy(llc);
	    resend_i_xxx(llc);
	    llc->state = LLS_BUSY;
	}

	/* else state remains LLS_AWAIT_BUSY */

	return;

    case REC_I_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_busystate: %s REC_I_CMD", llc2_prid(llc));
	send_frame(llc, RNR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	llc->data_flag = 1;

	/* state remains LLS_AWAIT_BUSY */

	return;

    case REC_RR_RSP:
    case REC_REJ_RSP:
    case REC_RR_CMD:
    case REC_REJ_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_busystate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->need_f_bit) 
	    send_frame(llc, RNR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	clear_rem_busy(llc);
	/* 
	 * clear_rem_busy called before resend_i_xxx because
	 * send_i_xxx may be called which examines rem_busy 
	 */
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    resend_i_xxx(llc);
	    llc->state = LLS_BUSY;
	}

	/* else state remains LLS_AWAIT_BUSY */

	return;

    case REC_RNR_RSP:
    case REC_RNR_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_busystate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->need_f_bit) 
	    send_frame(llc, RNR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	set_rem_busy(llc);
	/*
	 * set_rem_busy called before re_q_akchain to ensure
	 * that I frames are not erroneously transmitted
	 */
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    re_q_akchain(llc);	/* do resend when rem_busy cleared */
	    llc->state = LLS_BUSY;
	}
	/* else state remains LLS_AWAIT_BUSY */
	return;

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    send_frame(llc, RNR_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;

	    /* state remains LLS_AWAIT_BUSY */

	}
	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_busystate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}


/*
 * aw_rejstate
 */
static void aw_rejstate (llctype *llc)
{
    switch (llc->event) {

    case SET_LOCAL_BUSY:
	if (LLC2_DEBUG_STATE)
	    buginf("\nLLC: aw_rejstate: %s local busy set",
		   llc2_prid(llc));
	send_frame(llc, RNR_CMD_P0);
	llc->data_flag = 2;
	llc->state = LLS_AWAIT_BUSY;
	return;

    case REC_I_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_rejstate: %s REC_I_CMD", llc2_prid(llc));
	if (llc->unexp_n_s) {
	    if (llc->need_f_bit) 
		send_frame(llc, RR_RSP);
	    /* state remains LLS_AWAIT_REJ */
	} else {
	    /* incrementing V(R) done in indinfo */
	    indinfo(llc);
	    send_frame(llc, RR_RSP);
	    llc2_stoptimer(llc, REJ_TIMER_M);
	    llc->state = LLS_AWAIT;
	}
	update_n_r(llc);
	/* update_v_s(llc); */
	return;

    case REC_I_RSP:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_rejstate: %s REC_I_RSP", llc2_prid(llc));
	if (llc->unexp_n_s) {
	    update_n_r(llc);
	    /* update_v_s(llc); */
	    if (llc->f_bit_set) {
		stop_p_timer(llc);
		clear_rem_busy(llc);
		/* clear_rem_busy called before resend_i_xxx because
		   send_i_xxx may be called which examines rem_busy */
		resend_i_xxx(llc);
		llc->state = LLS_REJECT;
	    }

	    /* else state remains AWAIT_REJ */

	} else {
	    /* incrementing V(R) done in indinfo */
	    indinfo(llc);
	    llc2_stoptimer(llc, REJ_TIMER_M);
	    update_n_r(llc);
	    /* update_v_s(llc); */
	    if (llc->f_bit_set) {
		stop_p_timer(llc);
		clear_rem_busy(llc);
		/*
		 * clear_rem_busy called before resend_i_xxx because
		 * send_i_xxx may be called which examines rem_busy
		 */
		if (!resend_i_xxx(llc)) {
		    /*
		     * unable to retransmit an I frame
		     */
		    send_frame(llc, RR_CMD_P0);
		}
		llc->state = LLS_NORMAL;
	    } else {
		send_frame(llc, RR_CMD_P0);
		llc->state = LLS_AWAIT;
	    }
	}
	return;

    case REC_RR_RSP:
    case REC_REJ_RSP:
    case REC_RR_CMD:
    case REC_REJ_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_rejstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->need_f_bit)
	    send_frame(llc, RR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	clear_rem_busy(llc);
	/* clear_rem_busy called before resend_i_xxx because
	   send_i_xxx may be called which examines rem_busy */
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    resend_i_xxx(llc);
	    llc->state = LLS_REJECT;
	}
	/* else state remains AWAIT_REJ */
	return;

    case REC_RNR_RSP:
    case REC_RNR_CMD:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_rejstate: %s %s (%d)", llc2_prid(llc),
		   llc2_prevent(llc->event), llc->event);
	if (llc->need_f_bit) 
	    send_frame(llc, RR_RSP);
	update_n_r(llc);
	/* update_v_s(llc); */
	set_rem_busy(llc);
	/*
	 * set_rem_busy called before re_q_akchain to ensure
	 * that I frames are not erroneously transmitted
	 */
	if (llc->f_bit_set) {
	    stop_p_timer(llc);
	    re_q_akchain(llc);/* resend will be done when rem_busy cleared */
	    llc->state = LLS_REJECT;
	}
	return;

    case P_TIMER_EXP:
	if (llc->retry_count < llc->llc2_n2) {
	    send_frame(llc, REJ_CMD_P1);
	    start_p_timer(llc);
	    llc->retry_count++;
	}

	/* state remains AWAIT_REJ */

	return;

    default:
	if (LLC2_DEBUG_ERR)
	    buginf("\nLLC: aw_rejstate: state failure %s %s (%d)",
		   llc2_prid(llc), llc2_prevent(llc->event), llc->event);
	return;
    }
}


/* ***************************************************
   **      action functions in the order described  **
   **      in the IEEE802.2 specification           **
   *************************************************** */

/*
 * clear_rem_busy
 */
static void clear_rem_busy (llctype *llc)
{
    if (llc->rem_busy != 0) {
	llc->retry_count = 0;
	llc->rem_busy = 0;
	llc2_stoptimer(llc, BUSY_TIMER_M);
	llc->txaction |= TXDATA_R_BIT; /* attempt data transmission */
    }
}


/*
 * indinfo
 */
static void indinfo (llctype *llc)
{
    paktype *pak;

    if ((pak = llc->inbuf) == NULL)
	return;

    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: INFO: %s v(r) %d", llc2_prid(llc), llc->v_r>>1);

    /* 
     * increment V(R) 
     */
    llc->v_r += 2;
    
    /*
     * Prevent '76/75' errors by clearing input statistics for those
     * packets destined for CLS.  Clear here, because we 'release
     * packet' within the notify to the higher layers.  And who knows
     * what will happen to it then? ;) Note that now we need to be
     * aware of possible memory or buffer leaks which may occur if
     * DLU's don't properly dispose of packets...
     */
    if (LLC2isCls(llc))
        clear_if_input(pak);

    /*
     * If llc is under CLSI control, pass up data packet
     * must notify CLS before NULL'ing out llc->inbuf!! */
       if (llc2ClsNotify(llc, CLS_DATA_IND, CLS_OK))
       {
	   llc->inbuf = NULL;	/* if !NULL packet gets 'eaten' in llc2_input(), */
	   return;		/* for CLS LLC's packet is eaten by application... */
				/* prevent processing by rsrb/sdllc/qllc functionality */
       }
       llc->inbuf = NULL;
    /*
     * pass to higher layer if this vc is not operating in local ack
     * or sdllc or qllc mode.
     */
    if (llc->sdllc_active || llc->lnx_active) {
        pak->llc2_sap = pak->datagramstart[pak->llc2_sapoffset];
        pak->datagramstart += pak->llc2_sapoffset + 4;
        pak->datagramsize -= pak->llc2_sapoffset + 4;
        pak->flags |= PAK_LACK_OUT;
    }
    else {
       if (llc->rsrb_lack)
          pak->flags |= PAK_LACK_OUT;
       else {
          /*
           * De-encapsulate MAC and LLC.  An I frame always has 4 bytes
           * of control. Pass to higher level.  Save sap for 
           * get10mblink().
           */
           pak->llc2_sap = pak->datagramstart[pak->llc2_sapoffset];
           pak->datagramstart += pak->llc2_sapoffset + 4;
           pak->datagramsize -= pak->llc2_sapoffset + 4;
           pak->llc2_cb = llc;  /* save cookie for higher layer */
           raw_enqueue(llc->idb->hwptr, pak);
       }
    }
}

/*
 * send_control
 * Send an unnumbered control frame
 */
void send_control (llctype *llc, int ctype)
{
    uchar *frameptr;
    paktype *pak;

    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: %s txmt %s", llc2_prid(llc), llc2_prctlchr(ctype));
    
    /* 
     * flush out any delayed AKs 
     */
    if (llc->notaknum) {
	if (ctype <= XID_IND) 
	    send_frame(llc, RR_CMD_P0);
	else {
	    /* cannot send RR when remote end expecting UA or DM */
	    llc->notaknum = 0;
	}
    }
    /* 
     * get a buffer to send control frame whose maximum LLC2 size is 6 
     * initialize offset and fill in appropriate m_type value
     */
    pak = getbuffer(0);
    if (!pak) {
	return;
    }

    pak->enc_flags = llc->enc_flags;
    if (ctype == XID_IND) {
	pak->datagramstart -= (SAP_HDRBYTES + XID_FMT_STD_LEN);
	pak->datagramsize = XID_FMT_STD_LEN;

    } else {
	pak->datagramstart -= SAP_HDRBYTES;
	pak->datagramsize = 0;
    }

    frameptr = pak->datagramstart;
    *frameptr = llc2_get_rsap(llc);
    if (ctype <= XID_IND) {
	/* SABME, DISC or XID command being sent */
	*(frameptr+1) = llc2_get_lsap(llc);
	*(frameptr+2) = llc2_cvals[ctype];
	if ((llc->rsrb_lack) ||		/* Are we running remote source-route bridging, 
 					 * local acknowledgement?
 					 */
            LLC2isNBFCP(llc) ||   /* NetBEUI/PPP */
 	    (llc2_get_rsap(llc) == SAP_IBMNM) ||	/* LAN Network Manager session? CSCdi07429 */
	    LLC2isCls(llc)		/* CLS-controlled LLCs will always set p-bit */
	    )
	    *(frameptr + 2) |= 0x10;      /* set p-bit */
    } else {
	/* UA or DM response being sent */
	*(frameptr+1) = llc2_get_lsap(llc) + RSPBIT;
	*(frameptr+2) = llc2_cvals[ctype] + (llc->need_f_bit << 4);
	if (llc->need_f_bit) 
	    llc->need_f_bit = 0;
    }

    if (ctype == XID_IND) {
	*(frameptr+3) = XID_FMT_STD;
	*(frameptr+4) = (XID_CLASS_TYPE1 | XID_CLASS_TYPE2);
	*(frameptr+5) = llc->loc_wind;
    }

    llc2_transmit(llc, pak, 0, LLC_UNNUMBERED);
}



/*
 * send_frame
 * Send a supervisory frame
 */
static void send_frame (llctype *llc, int ctype)
{
    uchar *frameptr;
    paktype *pak;

    /* 
     * get a buffer to send RR, RNR or REJ, max LLC2 size is 4 
     */
    pak = getbuffer(0);
    if (!pak) {
	  return;
      }

    pak->enc_flags = llc->enc_flags;
    pak->datagramsize = 0;
    pak->datagramstart = pak->network_start - 4;

    frameptr = pak->datagramstart;
    *frameptr = llc2_get_rsap(llc);
    *(frameptr+2) = llc2_cvals[ctype];
    if (ctype >= RR_RSP) {
	/* RSP PDU */
	*(frameptr+1) = llc2_get_lsap(llc) + RSPBIT;
	*(frameptr+3) = llc->v_r + llc->need_f_bit;
	if (llc->need_f_bit) 
	    llc->need_f_bit = 0;
    }
    else {
	/* CMD PDU */
	*(frameptr+1) = llc2_get_lsap(llc);
	if (ctype >= RR_CMD_P1) {
	    /* P flag must be set */
	    *(frameptr+3) = llc->v_r + 1;
	}
	else {
	    *(frameptr+3) = llc->v_r;
	}
    }

    /*
     * Display the packet
     */
    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: %s txmt %s N(R)=%d p/f=%d", llc2_prid(llc),
	       llc2_prctlchr(ctype), llc->v_r>>1, (*(frameptr+3)&1));
    /* 
     * Sending RR, REJ or RNR all of which transmit current V(R)
     * Therefore set notaknum to zero  
     */

    llc2_transmit(llc, pak, 0, LLC_SUPERVISORY);

    llc->notaknum = 0;
    llc2_stoptimer(llc, AKDELAY_TIMER_M);
}


/*
 * send_FRMR
 */
static void send_FRMR (llctype *llc)
{
    uchar   *frameptr;
    paktype *pak;
    
    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: %s txmt FRMR %02x %02x %02x %02x %02x ",
	       llc2_prid(llc), llc->frmrchars[0],
	       llc->frmrchars[1], llc->frmrchars[2], 
	       llc->frmrchars[3], llc->frmrchars[4]);
    /* 
     * get a buffer to send FRMR, max LLC2 size is 8 
     */
    pak = getbuffer(0);
    if (!pak) {
	  return;
      }

    pak->datagramsize = 0;
    pak->datagramstart = pak->network_start - 8;

    frameptr = pak->datagramstart;
    *frameptr = llc2_get_rsap(llc);
    *(frameptr+1) = llc2_get_lsap(llc) + RSPBIT;
    *(frameptr+2) = FRMR + (llc->need_f_bit<<4);
    llc->need_f_bit = 0;	/* why test?  (as is done everywhere else
				   just set to zero for next usage... */

    *(frameptr+3) = llc->frmrchars[0];
    *(frameptr+4) = llc->frmrchars[1];
    *(frameptr+5) = llc->frmrchars[2];
    *(frameptr+6) = llc->frmrchars[3];
    *(frameptr+7) = llc->frmrchars[4];

    llc2_transmit(llc, pak, FRMR_INFO_LENGTH, LLC_UNNUMBERED);
}

/*
 * send_i_xxx.
 * Returns FALSE if unable to transmit.
 */
static int send_i_xxx (llctype *llc)
{
    uchar    *frameptr;
    int      v_s_copy, last_n_r_copy;
    paktype  *pak;
    lak_t    *lackie;

    llc->txaction &= ~TXDATA_R_BIT;
    if (llc->rem_busy) 
	return(FALSE);
    if (llc->txaction & REQ_AK_BIT) {
	return(FALSE);
    }
    /* check that transmission lies within window */
    v_s_copy = llc->v_s;
    last_n_r_copy = llc->last_n_r;
    if (last_n_r_copy > v_s_copy) 
	v_s_copy += 256;
    if ((v_s_copy - last_n_r_copy) >= llc->loc_wind)
	return(FALSE);

    /* then check that there is a cell to transmit */
    if ((pak = p_dequeue (&llc->txQ)) == NULL)
	return(FALSE);
    llc->txqnum--;

    frameptr = pak->datagramstart + pak->llc2_sapoffset;

    *frameptr = llc2_get_rsap(llc);
    *(frameptr+1) = llc2_get_lsap(llc);
    pak_lock(pak);
    if (pak_enqueue(&llc->akQ, pak) == NULL) {
	datagram_done(pak);
	p_requeue(&llc->txQ, pak);
	return (FALSE);
    }
 
    *(frameptr+2) = llc->v_s;
    *(frameptr+3) = llc->v_r;
    if (llc->need_f_bit) {
	(*(frameptr+1))++;		/* add response bit */
	(*(frameptr+3))++;		/* add F bit */
	llc->need_f_bit = 0;	/* ensure that F bit only set once */
    }
    llc->v_s += 2;			/* update sequence number */
    
    /*
     * store if_input and if_output so that packet travels down same river
     * transmit routines will modify these fields, so store them to insure
     * re-transmitted packets take same path through the idb chaining...
     *
     * store other fields which are modified by virtual idb driver.
     * Fields are restored on re-transmission...
     */
    pak->flags |= PAK_DLQ;
    pak->vidb_input = pak->if_input; 
    pak->vidb_output = pak->if_output;
    
    pak->vidb_flags = pak->flags; 
    pak->vidb_datagramsize = pak->datagramsize;
    pak->vidb_datagramstart = pak->datagramstart;
    pak->vidb_linktype = pak->linktype; 
    pak->vidb_enctype = pak->enctype; 
    
    llc_sendframe(pak);
    llc->akqnum++;

    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: %s txmt I N(S)=%d N(R)=%d p/f=%d size=%d",
	       llc2_prid(llc), (*(frameptr+2))>>1,
	       (*(frameptr+3))>>1, (*(frameptr+3)&1),
	       pak->datagramsize);

    if (!(llc->timer_flags & ACK_TIMER_M))
	llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
    llc->notaknum = 0;
    llc2_stoptimer(llc, AKDELAY_TIMER_M);
    if (llc->txQ.qhead != NULL)		/* if tx queue not empty */
	llc->txaction |= TXDATA_R_BIT;
    /*
     * check if low water mark has been reached for this LOCACK entity
     * only check if valid lack entity (i.e. non-NULL ptr from get_lackie())
     * then, only check if we're in a "quenched" or "about to be quenched"
     * state...
     */
    lackie = llc2_get_lackie(llc);
    if (lackie &&
	  (lackie->lak2_state == l2st_quench ||
	   lackie->lak2_state == l2st_remqonwait)) {
	if (llc_get_oqueue_status(llc) < DLC_OQUEUE_NORMAL) {
	    /* signal we've reached low mark */
	    reg_invoke_srt_lackfsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqlow);
	}
    }
    return(TRUE);
}


/*
 * resend_i_xxx
 */
static int resend_i_xxx (llctype *llc)
{

    re_q_akchain(llc);
    if (send_i_xxx(llc)) {
	if (LLC2isCls(llc)) {
	    /* for CLS users, indicate congestion level on successful Tx */
	    llc2_flow_ctrl_ind(llc);
	}
	return(TRUE);
    } else {
	return(FALSE);
    }	
}


/* 
 * re_q_akchain
 */
static void re_q_akchain (llctype *llc)
{
    elementtype *element;
    paktype *pak;
    hwidbtype *old_if_output;
    uchar *savestart;

    /*
     * This is a test to see if the packet is still stuck in the
     * bowels of the TCP driver. Should also probably figure a way to
     * test if it is stuck on some interfaces output queue, but that is
     * not currently done here.
     *
     * Previous versions of this code also tested for pak->next !=
     * NULL in the loop. It was determined that the test of the
     * tcpdriver_flags was sufficient. [sberl 6/15/95]
     *
     * Setting REQ_AK_BIT makes this routine get called from the llc2_timer() routine.
     * Hopefully, the next time its called, the condition will have cleared itself.
     * Note that this condition can happen in lots of situations, because if the partner
     * goes into BUSY state, and sends an RNR, it may later send a REJ to get us to 
     * retransmit any dropped input. If we are in the process of sending a frame at that
     * very moment, this code will cause us to not send the data. It used to just return,
     * with the expectation that a T1 timeout will occur. However, DSPU uses 30 seconds for
     * this timer, making the wait extremely long. Good for congestion, bad for users...
     *    [bmonsen 3/6/97]
     */

    for (element = (elementtype *) llc->akQ.qhead; element; element = element->next) {
	pak = element->pakptr;
	if ((pak->next != NULL) || 
	    (pak->tcpdriver_flags & TCPD_PAK_INPIPE) ||
	    (pak->flags & PAK_DLQ)) {
	    llc->txaction |= REQ_AK_BIT;
	    return;
	}
    }

    llc->txaction &= ~REQ_AK_BIT;
    llc->txqnum += llc->akqnum;
    llc->akqnum = 0;
    llc->v_s = llc->new_n_r;

    while ((element = (elementtype *)llc->akQ.qtail)) {
	  /*
	   * place debugging message within loop so that it is only displayed if
	   * there is work to do.  Function can get called in conditions where there
	   * are no buffered frames to transmit.  Code does "nothing" properly...
	   */
	  if (LLC2_DEBUG_ERR)
	      buginf("\nLLC: re_q_akchain: V(S) reset from %d to %d",
		     llc->v_s>>1, llc->new_n_r>>1);
	  pak = pak_unqueue(&llc->akQ, element->pakptr);
	  if (!pak)
	      break;
	  if (llc2_debug_test)
	      buginf("\nLLC: retransmitting packet");

	  /*
	   * restore if_input and if_output so that packet travels down same "river"
	   * transmit routines will modify these fields, so store them to insure
	   * re-transmitted packets take same path through the idb chaining...
	   *
	   * Restore other fields in packet structure so "re-transmitted" state
	   * is same as "pre-transmitted state".
	   */
          old_if_output = hwidb_or_null(pak->if_output);
	  pak->if_input = pak->vidb_input; 
	  pak->if_output = pak->vidb_output; 	    
	  pak->flags = pak->vidb_flags; 
          if (pak->linktype == LINK_RSRB) {
              old_if_output = NULL;
          }
	  pak->linktype = pak->vidb_linktype; 
	  pak->enctype = pak->vidb_enctype; 

	  /*
	   * The joys of shared memory...  We have transmitted a frame
	   * out some 'bridge' interface which was nice enough to
	   * modify the frame as per requirements of output media.
	   * For Ethernet, it does things like flip the MAC addresses
	   * and sets proper encapsulation.
	   *
	   * This is a problem when we re-transmit, say because of a
	   * REJ frame, because the LLC code assumes that the packet
	   * in this akQ, hasn't been modified (whoops!).  So sending
	   * a packet through the bridge 'filters' twice is A Bad
	   * Thing(tm).
	   *
	   * The following code is an effort to regain a packet's
	   * virginity.  Find out where it has been so we can rip off
	   * the Bad Thing(tm) encapsulation, and let's start over
	   * from scratch.
	   */
          if ((pak->linktype == LINK_CLS) && (old_if_output != NULL))
	  {
	      int len;
	      /*
	       * Frame modification was made to support encapsulation
	       * type of output interface.  So use if_output as base
	       * to crack packet and remove encapsulation performed
	       * when packet went through bridging code.
	       */
              (*old_if_output->parse_packet)(old_if_output, pak, NULL);

	      /*
	       * Make a Virgin once more before the next sacrifice to
	       * the Transmit god.  Network start points to first data
	       * byte beyond llc2 header (skipping the 4 bytes of
	       * ss/ds/nr/ns).  Adjust pak->datagramstart
	       * (ala. llc2_send) before re-encapsulating to Virgin
	       * state.
	       */
              pak->llc2_cb = llc;
	      pak->datagramstart = pak->network_start - SAP_LONGHDRBYTES;
	    
	      len = pak->datagramsize - pak->encsize;
              savestart = pak->datagramstart;
	      llc_encapsulate(pak, LLC_IFRAME, len, llc);

              /* remember the offset to the first SAP */
              pak->llc2_sapoffset = savestart - pak->datagramstart;
	  } 
          else 
          {                   /* old style packet recycle */
              pak->datagramsize = pak->vidb_datagramsize; 
              pak->datagramstart = pak->vidb_datagramstart;
          }
	
	  p_requeue(&llc->txQ, pak); 
      }
    llc->txaction |= TXDATA_R_BIT;
}


/*
 * send_ack_xxx
*/
static void send_ack_xxx (llctype *llc)
{
    if (send_i_xxx(llc)) {
	if (LLC2isCls(llc)) {
            /* for CLS users, indicate congestion level on successful Tx */
	    llc2_flow_ctrl_ind(llc);
	}
	return;
    }

    /* no I PDUs eligible for transmission */

    if (llc->need_f_bit) {
	send_frame(llc, RR_RSP);
	return;
    }

    /*
     * CSCdi29047 & CSCdi27034
     * To delay the acknowledgement, store the number that haven't been
     * ack'ed in llc->notaknum. Cisco's ack-max is what IEEE calls N3.
     *
     * The IBM Token Ring Architecture Reference says:
     * Decrement N3 (llc2 ack-max nnn) each I-frame received.  When N3 
     * reaches 0, send the ack.  Note that llc2 ack-max 1 behaves the same
     * as llc2 ack-max 0.
     *
     * To implement, we simple count the unacknowledged frames and 
     * subtract from akmax, if akmax is configured.  Other parts of 
     * llc check to see if an acknowledgement is pending by examining
     * llc->notaknum for a non-zero value.
     */

    llc->notaknum++;

    /* 
     * If N(3) configured and N(3) not exhausted, start the 
     * the ack delay timer on receipt of the first frame.
     */

    if (llc->llc2_akmax &&
	(llc->notaknum < llc->llc2_akmax)) {
        if (llc->notaknum == 1) 
	    llc2_starttimer(llc, AKDELAY_TIMER, llc->llc2_akdelay_time);
    }
    else {
	if (llc->cmdrspbit) {
	    /* PDU was a response, reply with a CMD */
	    send_frame(llc, RR_CMD_P0);
	}
	else {
	    /* PDU was a command, reply with a RSP */
	    send_frame(llc, RR_RSP);
	}
    }
}


/*
 * set_rem_busy
 */
static void set_rem_busy (llctype *llc)
{
    if (llc->rem_busy == 0) {
	llc->rem_busy = 1;
	llc2_starttimer(llc, BUSY_TIMER, llc->llc2_tbusy_time);
    }
}


/*
 * update_n_r
 */
static void update_n_r (llctype *llc)
{
    int     newval = 0;
    paktype *pak;

    newval = 0;
    while (llc->last_n_r != llc->new_n_r) {
	if ((pak = pak_dequeue(&llc->akQ)) != NULL) {
	    newval = 1;
	    llc->last_n_r += 2;
	    llc->akqnum--;
	    datagram_done(pak);
	}
	else {
	    /* 
	     * this can only ever happen in the unlikely event 
	     * of receiving an acknowledgement before the MDC 
	     */
	    llc->new_n_r = llc->last_n_r;
	}
    }
    if (newval) {
	if (llc->txQ.qhead != NULL) {
	    llc->txaction |= TXDATA_R_BIT; /* attempt restart of data */
	}
	if (llc->akqnum) {
	    llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
	}
	else 
	    llc2_stoptimer(llc, ACK_TIMER_M);
	llc->retry_count = 0;
    }
}


/*
 * update_p_flag
 */
static void update_p_flag (llctype *llc)
{
    if (llc->f_bit_set) {
	/* frame was a response with F bit set */
	llc->p_flag = 0;
	llc2_stoptimer(llc, P_TIMER_M);
    }
}


/*
 * start_p_timer
 */
static void start_p_timer (llctype *llc)
{
    llc2_starttimer(llc, P_TIMER, llc->llc2_tpf_time);
    llc->p_flag = 1;
}


/*
 * stop_p_timer
 */
static void stop_p_timer (llctype *llc)
{
    llc2_stoptimer(llc, P_TIMER_M);
    llc->p_flag = 0;
}


/*
 * stop_all_timers
 */
static void stop_all_timers ( llctype *llc)
{
    llc->p_flag = 0;
    llc2_stoptimer(llc, P_TIMER_M);
    llc2_stoptimer(llc, ACK_TIMER_M);
    llc2_stoptimer(llc, REJ_TIMER_M);
    llc2_stoptimer(llc, BUSY_TIMER_M);
    llc2_stoptimer(llc, LLC1_TIMER_M);
}


/*
 * stop_other_timers
 */
static void stop_other_timers (llctype *llc)
{
    llc->p_flag = 0;
    llc2_stoptimer(llc, P_TIMER_M);
    llc2_stoptimer(llc, REJ_TIMER_M);
    llc2_stoptimer(llc, BUSY_TIMER_M);
}


/*
 * zero_v_vals
 */
static void zero_v_vals (llctype *llc)
{
    paktype *pak;

    llc->v_s = 0;
    llc->v_r = 0;
    llc->last_n_r = 0;
    llc->retry_count = 0;
    llc->rem_busy = 0;
    /* 
     * No guidance is given concerning any unacknowledged or
     * untransmitted frames.  At present all queued buffers are discarded
     */
    while ((pak = pak_dequeue(&llc->akQ)) != NULL) {
	datagram_done(pak);
    }
    llc->akqnum = 0;

    while ((pak = p_dequeue(&llc->txQ)) != NULL)
	datagram_done(pak);
    llc->txqnum = 0;
}

/*
 * l l c 2 _ g o t o _ d i s c o n n e c t
 *
 * Code gratuitously lifted from runstates() disconnect.  Why copy
 * code when you can make it into a function...
 *
 * Modified to allow substitution in all places currently supporting
 * this and similar functionality.  Added ability to zero or increment
 * retry counter
 */
static void llc2_goto_disconnect (llctype *llc)
{
    send_control(llc, DISC_IND);
    llc->p_flag = 0;
    stop_other_timers(llc);
    /*
     * send_control() above will send a DISC CMD with p-bit == 1 for
     * LLC's under CLS.  Therefore we need to start p-timer to indicate
     * that we have an outstanding (well at least pretty good ;) P bit.
     * Now when p-timer goes off, code will 'do the right thing...'
     */
    if (LLC2isCls(llc) || (llc->rsrb_lack) || LLC2isNBFCP(llc) ) {
	start_p_timer(llc);
    } else {
	llc2_starttimer(llc, ACK_TIMER, llc->llc2_T1_time);
    }

    if (llc->state == LLS_D_CONN) /* if we're (still) in disconnect state... */
	llc->retry_count++;	/* ...increment counter  */
    else
	llc->retry_count = 0;   /* first time here, so get a clean slate */

    llc->state = LLS_D_CONN;
}


/*
 * llc2_goto_adm
 */
void llc2_goto_adm (llctype *llc, int reason)
{
    leveltype level;

    if (!llc) {
	buginf("\nLLC2: goto_adm called with null pointer.");
	return;
    }
    if (LLC2_DEBUG_ERR)
	buginf("\nLLC2: %s  %s -> ADM, event %s (%d), %s (%d)",
	       llc2_prid(llc), llc2_prstate(llc, 0),
	       llc2_prevent(llc->event), llc->event,
	       llc2_prreason(reason), reason);
    llc->adm_reason = reason;

    /*
     * If the user does a CloseStn before sending a disconnect then
     * send a DISC before cleaning up the llc session.
     */
    if ((llc->state == LLS_NORMAL) &&
	((LLC2isCls(llc)) && (reason == LC_USER_CLOSE)))
	send_control(llc, DISC_IND);

    llc->state = LLS_ADM;

    stop_all_timers(llc);		/* clear timers and p_flag */
    llc2_stoptimer(llc, IDLE_TIMER_M);
    llc2_stoptimer(llc, XID_RTRY_TIMER_M);
    llc2_stoptimer(llc, ADM_TIMER_M);
    zero_v_vals(llc);

    /*
     * If the LLC is under CLS control, then modify operation of goto_adm
     * so it only changes llc state and not disposition (i.e. save it from
     * the free  pool)
     * The exception that proves the rule is that LC_USER_CLOSE is used
     * by CLS controlled interfaces to destroy llctype structures...
     */
    if (LLC2isCls(llc) && reason != LC_USER_CLOSE)
    {
	switch (reason)
	{
	    /*
	     * currently used by CLS llc's to indicate timeout condition in reset
	     * setup states.  signals n2 failure on SABME
	     */
	case LC_TIMEOUT:
	    llc->llcDiscAckRspType = LLC2_DISC_RSP_NONE;
	    llc->cause_flag = LLC2_External_Cause;
	    llc2ClsNotify(llc, CLS_CONNECT_CNF, CLS_TIMEOUT);
	    break;
	    
	case LC_DISC_TIMEOUT:
	    /*
	     * LLC timed out sending DISC commands, just give up and
	     * signal error to DLU
	     */
	    if (llc->cause_flag == LLC2_Internal_Cause)
		llc2ClsNotify(llc, CLS_DISC_CNF, CLS_DISC_ERROR);
	    else
	    {
		/* silently do nothing... */
	    }
	    break;
	    
	case LC_INTDOWN:
	    /*
	     * Interface has been shut, no need to send DISC on lan...
	     */
	    llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR);
	    break;
	    
	case LC_FRMR:
	    /*
	     * Rx of FRMR signals disconnect, ibm tkr ref 12-64
	     */
	    llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR);
	    llc2_goto_disconnect(llc);
	    break;
	    
	    /*
	     * LC_RESET is used in d_conn state on reception of SABME,
	     * refer to Table 7-1 of IEEE 802.2-1985 and see that this
	     * input/state combination is effectively same as receiving
	     * a DISC from the LAN, except no UA response and next state
	     * is ADM...
	     */
	case LC_RESET:
	case LC_DISC_LAN:
	    llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR);
	    break;
	    
	case LC_DISC_CNF:
	    if (llc->cause_flag == LLC2_Internal_Cause)
		llc2ClsNotify(llc, CLS_DISC_CNF, CLS_OK);
	    else 
	    {
	    }
	    break;

	case LC_DM:
	    /*
	     * cause_flag tracks tracks source of input to RESET or D_CONN state
	     * External means LAN inputs, Internal is higher layer (DLU) action.
	     */
	    if (llc->cause_flag == LLC2_Internal_Cause)
		llc2ClsNotify(llc, CLS_DISC_CNF, CLS_DISC_ERROR);
	    else
	    {
		llc2ClsNotify(llc, CLS_DISC_IND, CLS_DISC_ERROR);
		llc->llcDiscAckRspType = LLC2_DISC_RSP_NONE;
	    }

	}	/* endSwitch */

	if (llc->llc2_adm_timer_value > 0) {
	    llc2_starttimer(llc, ADM_TIMER, llc->llc2_adm_timer_value);
	}

	return;			/* CLS llc's are kept until DLU's explicitly remove them
				   save the *llc, do not cross line of death below */
    }	/* endIf */
/******************************************************************************
 * For llctype, theese ees line of death, cross theese line and you die!
 *****************************************************************************/

    /*
     * Put this llc on the cleanup queue.
     */
    llc_remove_dte_hash(llc);

    /*
     * If on the timer queue, remove it before putting on the
     * cleanup queue.
     */
    
    level = raise_interrupt_level(NETS_DISABLE);

    if (llc->timer_queue != NULL || llc == llctQ.qtail) {
	if (!checkqueue(&llctQ, llc)) {
	    if (LLC2_DEBUG_ERR) {
		buginf("nllctype in llc_goto_adm() is on some other queue than timer queue");
	    }
	} else {
	    p_unqueue(&llctQ, llc);
	}
	llc->timer_queue = NULL;
	llc->timer_flags = 0;
    }
    
    reset_interrupt_level(level);

    /*
     * Current functions using the media_llc_cleanup vector:
     *        lack_cleanup_llc
     *        sdllc_cleanup_llc
     *        lnx_cleanup_llc
     *
     * Call the application llc cleanup routine for this llc.  The appropriate application
     * set the media_llc_cleaup vector (one of the three above) when it became involved
     * in this session.  If none of the above functions were involved in the
     * llc, then there's no special cleanup necessary.
     *
     */
     if (llc->media_llc_cleanup) {
       (*llc->media_llc_cleanup) (llc);
       llc->media_llc_cleanup = NULL;
     } 
    
     /* Don't do this if its either on the free queue or the cleanup queue */
     if (llc->q_elem.q_next == NULL
	 &&
	 llc2_cleanupQ.qtail != &llc->q_elem
	 &&
	 llc2_freelistQ.qtail != &llc->q_elem) {

	 p_enqueue(&llc2_cleanupQ, &llc->q_elem);
     }
}


/*
 * llc2_stateswitch
 */
void llc2_stateswitch (llctype *llc, int newevent)
{
    uchar curstate, prevstate, searchevent;

    llc->event = newevent;
    curstate = prevstate = llc->state;
    if (LLC2_DEBUG_PACKET)
	buginf("\nLLC: %s %s %s (%d) ", llc2_prid(llc),
	       llc2_prstate(llc, 0), llc2_prevent(llc->event), llc->event);
    if (curstate < LLS_ERROR) 
	searchevent = 1;
    else
	searchevent = runstates(llc);

    /*
     * Runstate may have changed our state.  Display any change.
     */
    if (LLC2_DEBUG_STATE && prevstate != llc->state)
	buginf("\nLLC(rs): %s, %s -> %s (%s)", llc2_prid(llc),
	       llc2_prstate(NULL, prevstate), llc2_prstate(llc, 0),
	       llc2_prevent(llc->event));

    prevstate = llc->state;
    if (searchevent) {
	switch (curstate) {
	case LLS_ADM:    
	    admstate(llc);
	    break;

	case LLS_SETUP:  
	    setupstate(llc);
	    break;

	case LLS_RESET:  
	    resetstate(llc);
	    break;

	case LLS_D_CONN: 
	    d_connstate(llc);
	    break;

	case LLS_ERROR:  
	    errorstate(llc);
	    break;

	case LLS_NORMAL: 
	    normalstate(llc);
	    break;

	case LLS_BUSY:   
	    busystate(llc);
	    break;

	case LLS_REJECT: 
	    rejectstate(llc);
	    break;

	case LLS_AWAIT:  
	    awaitstate(llc);
	    break;

	case LLS_AWAIT_BUSY: 
	    aw_busystate(llc);
	    break;

	case LLS_AWAIT_REJ: 
	    aw_rejstate(llc);
	    break;
	}
    }
    if (LLC2_DEBUG_STATE && prevstate != llc->state)
	buginf("\nLLC (stsw): %s, %s -> %s (%s)",
	       llc2_prid(llc), llc2_prstate(NULL, prevstate),
	       llc2_prstate(llc, 0), llc2_prevent(llc->event));

    if (llc->state != LLS_ADM) {
	llc2_stoptimer(llc, ADM_TIMER_M);
    }
}




/*
 * llc2_examaction
 */
void llc2_examaction (llctype *llc)
{
    int actionbyte;

    actionbyte = llc->txaction;

    if (actionbyte & ~TXDATA_R_BIT) {

	if (actionbyte & DISC_R_BIT) {
	    llc2_stateswitch(llc, DISC_REQ);
	    llc->txaction = 0;
	    return;
	}

	if (actionbyte & RESET_R_BIT) {
	    llc2_stateswitch(llc, RESET_REQ);
	    llc->txaction &= DISC_R_BIT;
	    return;
	}

	if (actionbyte & XID_R_BIT) {
	    llc->txaction &= (255 - XID_R_BIT - TXDATA_R_BIT);
	    send_control(llc, XID_IND);
	    return;
	}

	if (actionbyte & CONN_R_BIT) {
	    llc2_stateswitch(llc, CONN_REQ);
	    llc->txaction &= (DISC_R_BIT + RESET_R_BIT);
	    return;
	}

	if (actionbyte & REQ_AK_BIT) 
	    re_q_akchain(llc);
    }

    if ((llc->state >= LLS_NORMAL) && (llc->state <= LLS_REJECT)) {
	while (llc->txaction & TXDATA_R_BIT) {
	    send_i_xxx(llc);
	}
	/*
	 * For CLS users, indicate flow control congestion level.
	 */
	if (LLC2isCls(llc))
	    llc2_flow_ctrl_ind(llc);
    }
}


/*
 * llc2_sendRR
 */
void llc2_sendRR (llctype *llc)
{
    send_frame(llc, RR_CMD_P0);
}
