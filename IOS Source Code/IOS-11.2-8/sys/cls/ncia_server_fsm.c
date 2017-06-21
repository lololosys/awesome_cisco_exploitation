/* $Id: ncia_server_fsm.c,v 3.1.2.10 1996/07/26 06:24:01 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_server_fsm.c,v $
 *------------------------------------------------------------------
 * ncia_server_fsm.c - NCIA Server FSM
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_server_fsm.c,v $
 * Revision 3.1.2.10  1996/07/26  06:24:01  schiang
 * CSCdi64000:  remove unused variable ncia_cisco_debug
 * Branch: California_branch
 *
 * Revision 3.1.2.9  1996/07/03  06:35:58  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.8  1996/06/27  06:05:26  schiang
 * CSCdi61391:  I_CAN_REACH packet is not properly setup
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/06/26  19:53:01  schiang
 * CSCdi61191:  incorrectly declare functions as inlines in
 * ncia_server_fsm.h
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/19  05:21:12  schiang
 * CSCdi60757:  HALT_DL_NO_ACK is not sent in some condition
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/18  19:01:45  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/12  06:51:52  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/04  05:58:11  schiang
 * CSCdi59005:  ncia client output queue full with duplicated pkts if
 * client is down
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/28  22:33:50  schiang
 * CSCdi58852:  router crashes when ncia server is un-configured
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.4  1996/05/14  18:42:23  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/03  22:06:27  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/26  15:16:44  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:47  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:19:35  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_generic.h"
#include "sys_registry.h"
#include "../cls/dlc_registry.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../cls/clsi_msg.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"

#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "../cls/ncia_debug.h"
#include "../cls/ncia_debug_flags.h"



static ushort ncia_send_dlu(NciaCepT *ncia_ptr, ncia_pb_t *action);
static char *ncia_print_pre_server_event(int event);
static void ncia_flow_control_in(NciaCepT *ncia_ptr, uchar flow_ctl, int ndlc);
static void ncia_send_i_can_reach(nciapeertype *ncia_client, paktype *pak);




/*
 *  Send STN_CONTACTED to clinet and stop timer.
 */
ushort ncia_action_1 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 1");

    ncia_send_ndlc(ncia_ptr, NDLC_STN_CONTACTED);
    mgd_timer_stop(&ncia_ptr->fTimer);
    return(FSM_OK);
}


/*
 * Send HALT_DL to clinet and restart timer.
 */
ushort ncia_action_2 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 2");

    ncia_send_ndlc(ncia_ptr, NDLC_HALT_DL);
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}


/*
 * Sned I_CAN_REACH to clinet.
 */
ushort ncia_action_3 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 3");

    ncia_send_ndlc(ncia_ptr, NDLC_I_CAN_REACH);
    return(FSM_OK);
}


/*
 * Send DATA_FRAME to client.
 */
ushort ncia_action_4 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 4");

    if (ncia_ptr->fNciaCepFlags & NciaClientBusy) {
        enqueue(&ncia_ptr->fOutputQ, ncia_ptr->fPacket);
        ncia_ptr->fPacket = NULL;
    } else
        ncia_send_ndlc(ncia_ptr, NDLC_DATA_FRAME);
    return(FSM_OK);
}


/*
 * Send DL_STARTED and CONTACT_STN to client and restart timer.
 */
ushort ncia_action_5 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 5");

    ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
    ncia_send_ndlc(ncia_ptr, NDLC_CONTACT_STN);
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}


/*
 * Send CONTACT_STN to clinet and restart timer.
 */
ushort ncia_action_6 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 6");

    ncia_send_ndlc(ncia_ptr, NDLC_CONTACT_STN);
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}


/*
 * Send DGRM_FRAME to client.
 */
ushort ncia_action_7 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 7");

    ncia_send_ndlc(ncia_ptr, NDLC_DGRM_FRAME);
    return(FSM_OK);
}


/*
 * Send INFO_FRAME to client.
 */
ushort ncia_action_8 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 8");

    ncia_send_ndlc(ncia_ptr, NDLC_INFO_FRAME);
    return(FSM_OK);
}


/*
 * Send START_DL to client and restart timer.
 */
ushort ncia_action_9 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 9");

    ncia_send_ndlc(ncia_ptr, NDLC_START_DL);
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}


/*
 * Send DL_STARTED and XID_FRAME to client and stop timer.
 */
ushort ncia_action_10 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 10");

    ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
    ncia_ptr->fPacket = ncia_ptr->fXid;
    ncia_ptr->fXid = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_XID_FRAME);
    mgd_timer_stop(&ncia_ptr->fTimer);
    return(FSM_OK);
}


/*
 * Send XID_FRAME to client and stop timer.
 */
ushort ncia_action_11 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 11");

    ncia_send_ndlc(ncia_ptr, NDLC_XID_FRAME);
    mgd_timer_stop(&ncia_ptr->fTimer);
    return(FSM_OK);
}


/*
 * Start timer.
 */
ushort ncia_action_12 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 12");

    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);

    return(FSM_OK);
}


/*
 * Start timer and send TestStn.Ind to DLU.
 */
ushort ncia_action_13 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 12");
    NciaServerDebugAll("\nNCIA: send CLS_TEST_STN_IND to DLU");

    ncia_cls_notify(ncia_ptr, CLS_TEST_STN_IND, CLS_OK, ncia_ptr->fPacket);
    ncia_ptr->fPacket = NULL;
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}


/*
 *  send CloseStn.Cnf to DLU.
 */
ushort ncia_action_14 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 14");
    NciaServerDebugAll("\nNCIA: send CLS_CLOSE_STN_CNF to DLU");

    ncia_ptr->fPacket = NULL;
    mgd_timer_stop(&ncia_ptr->fTimer);
    ncia_remove_cep(ncia_ptr);
    ncia_cls_notify(ncia_ptr, CLS_CLOSE_STN_CNF, CLS_OK, pak);
    return(FSM_OK);
}


/*
 * Send HALT_DL_NOACK to clinet, stop timer, and send Connect.Cnf(Fail) to DLU.
 */
ushort ncia_action_15 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 15");
    NciaServerDebugAll("\nNCIA: send CLS_CONNECT_CNF(BUSY) to DLU");

    ncia_ptr->fPacket = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_HALT_DL_NOACK);
    mgd_timer_stop(&ncia_ptr->fTimer);
    ncia_cls_notify(ncia_ptr, CLS_CONNECT_CNF, CLS_CONNECTION_BUSY, pak);
    return(FSM_OK);
}


/*
 * Send DL_STARTED to client, stop timer, and send ReqOpnStn.Cnf to DLU.
 */
ushort ncia_action_16 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 16");
    NciaServerDebugAll("\nNCIA: send CLS_REQ_OPNSTN_CNF to DLU");

    ncia_ptr->fPacket = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
    mgd_timer_stop(&ncia_ptr->fTimer);
    ncia_insert_cep(ncia_ptr);
    ncia_cls_notify(ncia_ptr, CLS_REQ_OPNSTN_CNF, CLS_OK, pak);
    return(FSM_OK);
}


/*
 * Send DL_STARTED to client, stop timer.
 */
ushort ncia_action_17 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 17");

    ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
    mgd_timer_stop(&ncia_ptr->fTimer);
    return(FSM_OK);
}


/*
 * Send DL_STARTED to client, stop timer, and send Disconnect.Cnf to DLU.
 */
ushort ncia_action_18 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 18");
    NciaServerDebugAll("\nNCIA: send CLS_DISC_CNF to DLU");

    ncia_ptr->fPacket = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
    mgd_timer_stop(&ncia_ptr->fTimer);
    ncia_cls_notify(ncia_ptr, CLS_DISC_CNF, CLS_OK, pak);
    return(FSM_OK);
}


/*
 * Send START_DL_FAILED to client
 */
ushort ncia_action_19 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 19");

    ncia_send_ndlc(ncia_ptr, NDLC_START_DL_FAILED);
    return(FSM_OK);
}


/*
 * Send HALT_DL_NOACK to client
 */
ushort ncia_action_20 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 20");

    ncia_send_ndlc(ncia_ptr, NDLC_HALT_DL_NOACK);
    return(FSM_OK);
}



/*
 * Save XID, send START_DL to client, and restart timer.
 */
ushort ncia_action_21 (NciaCepT *ncia_ptr)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 21");

    if (ncia_ptr->fXid)
        datagram_done(ncia_ptr->fXid);
    ncia_ptr->fXid = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_START_DL);
    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(FSM_OK);
}



/*
 * the Flow.Ind (DLC_CONGESTION_HIGH) will be sent to DLU for two reasons:
 * 1. TCP Congestion: client_fsm sends BUSY_XPORT.
 * 2. Flow Control: windows is reduced to 0.
 * When either one occurs, we need to check if the Flow.Ind has been sent
 * to DLU already.
 * Should we send them in non-connected state ?
 */
/*
 * TCP Busy.   send flow.req to DLU (busy) 
 */
ushort ncia_action_22 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 22");

    ncia_ptr->fNciaCepFlags |= NciaTcpBusy;

    /* make sure the Flow.Ind hasn't been sent */
    if (!(ncia_ptr->fNciaCepFlags & NciaClientBusy)) {
        NciaServerDebugAll("\nNCIA: send CLS_FLOW_IND to DLU");
        ncia_cls_notify(ncia_ptr, CLS_FLOW_IND, CLS_OK, pak);
    }

    return(FSM_OK);
}

/*
 * TCP Unbusy.  send flow.req to DLU (unbusy)
 */
ushort ncia_action_23 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 23");

    ncia_ptr->fNciaCepFlags &= ~NciaTcpBusy;

    /* check if all flow control condition is cleared */
    if (!(ncia_ptr->fNciaCepFlags & NciaClientBusy)) {
        NciaServerDebugAll("\nNCIA: send CLS_FLOW_IND to DLU");
        ncia_cls_notify(ncia_ptr, CLS_FLOW_IND, CLS_OK, pak);
        /* packets in circuit queue can't be drain until the flow control
           condition is cleared */
        ncia_drain_circuit_outQ(ncia_ptr);
    }

    return(FSM_OK);
}



/*
 * DLU Busy.
 */
ushort ncia_action_24 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 24");

    NciaCircuitDebugState(" --> set NciaClsBusy");
    ncia_ptr->fNciaCepFlags |= NciaClsBusy;
    datagram_done(pak);

    /* reduce window */
    ncia_ptr->fFCFlag = NciaHalveWindowOp;
    return(FSM_OK);
}

/*
 * DLU Unbusy.
 */
ushort ncia_action_25 (NciaCepT *ncia_ptr)
{
    paktype *pak;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 25");

    if (ncia_ptr->fPacket) {
        datagram_done(ncia_ptr->fPacket);
        ncia_ptr->fPacket = NULL;
    }
    ncia_ptr->fNciaCepFlags &= ~NciaClsBusy;
    NciaCircuitDebugState(" --> unset NciaClsBusy");

    /* increase window */
    if (ncia_ptr->fClientGrantPacket == 0) {
        /* flow control bit will be set in the outgoing flow control routine */
        ncia_ptr->fFCFlag = NciaIncrementWindowOp;
        ncia_send_ndlc(ncia_ptr, NDLC_FCM_FRAME);
    }

    if (QUEUEEMPTY(&(ncia_ptr->fInputQ)))
        return(FSM_OK);

    while ((pak = dequeue(&ncia_ptr->fInputQ))) {
        NciaServerDebugAll("\nNCIA: send CLS_DATA_IND to DLU");
        ncia_cls_notify(ncia_ptr, CLS_DATA_IND, CLS_OK, pak);
    }

    return(FSM_OK);
}



/*
 * XID Retry.
 */
ushort ncia_action_26 (NciaCepT *ncia_ptr)
{
    boolean timer = TRUE;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 26");

    if (ncia_ptr->fPacket) {
        if (ncia_ptr->fXid)
            datagram_done(ncia_ptr->fXid);
        ncia_ptr->fXid = pak_duplicate(ncia_ptr->fPacket);
        if (!ncia_ptr->fXid)
            timer = FALSE;
    }
    if (timer) {
        NciaCircuitDebugState(" start timer for 15 seconds");
        mgd_timer_stop(&ncia_ptr->fTimer);
        mgd_timer_start(&ncia_ptr->fTimer, (15 * ONESEC));
    } else
        NciaCircuitDebugState(" start timer is not started");

    /* packet gets freed when circuit is freed. */

    /* only retry once */
    return(ncia_send_dlu(ncia_ptr, (ncia_pb_t *) &NCIA_ISI));
}




/*
 * Send HALT_DL_NOACK to clinet, stop timer, and send CloseStn.Cnf to DLU.
 */
ushort ncia_action_27 (NciaCepT *ncia_ptr)
{
    paktype *pak = ncia_ptr->fPacket;

    NciaCircuitDebugState("\nNCIA: ncia server fsm action 27");
    NciaServerDebugAll("\nNCIA: send CLS_CLOSE_STN_CNF to DLU");

    ncia_ptr->fPacket = NULL;
    ncia_send_ndlc(ncia_ptr, NDLC_HALT_DL_NOACK);
    mgd_timer_stop(&ncia_ptr->fTimer);
    ncia_remove_cep(ncia_ptr);
    ncia_cls_notify(ncia_ptr, CLS_CLOSE_STN_CNF, CLS_OK, pak);
    return(FSM_OK);
}



/*
 * Send HALT_DL_NOACK to client, stop timer, and send the given input to DLU.
 */
ushort ncia_action_30 (NciaCepT *ncia_ptr, ncia_pb_t *action)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 30");

    ncia_send_ndlc(ncia_ptr, NDLC_HALT_DL_NOACK);
    mgd_timer_stop(&ncia_ptr->fTimer);

    if (action->input == CLS_REQ_OPNSTN_CNF_FAIL)
        ncia_ptr->fNciaCepFlags |= NciaFreeCkt;

    return(ncia_send_dlu(ncia_ptr, action));
}



/*
 * Send DL_HALTED to client, stop timer, and send the given input to DLU.
 */
ushort ncia_action_31 (NciaCepT *ncia_ptr, ncia_pb_t *action)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 31");

    ncia_send_ndlc(ncia_ptr, NDLC_DL_HALTED);
    mgd_timer_stop(&ncia_ptr->fTimer);

    return(ncia_send_dlu(ncia_ptr, action));
}


/*
 * Restart timer and send the given input to DLU.
 */
ushort ncia_action_32 (NciaCepT *ncia_ptr, ncia_pb_t *action)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 32");

    mgd_timer_start(&ncia_ptr->fTimer, NCIA_GENERAL_TIMEOUT);
    return(ncia_send_dlu(ncia_ptr, action));
}



/*
 * Stop timer and send the given input to DLU.
 */
ushort ncia_action_33 (NciaCepT *ncia_ptr, ncia_pb_t *action)
{
    NciaCircuitDebugState("\nNCIA: ncia server fsm action 33");

    mgd_timer_stop(&ncia_ptr->fTimer);

    if (action->input == CLS_REQ_OPNSTN_CNF_FAIL)
        ncia_ptr->fNciaCepFlags |= NciaFreeCkt;

    return(ncia_send_dlu(ncia_ptr, action));
}



static
ushort ncia_send_dlu (NciaCepT *ncia_ptr, ncia_pb_t *action)
{
    paktype *pak = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;

    NciaServerDebugAll("\nNCIA: send %s to DLU", 
                       ncia_print_action(action->input));

    switch(action->input){
    case CLS_NO_INPUT:
         if (pak)
             datagram_done(pak);
         break;

    case CLS_REQ_OPNSTN_CNF_AND_CONNECT_IND:
         ncia_insert_cep(ncia_ptr);
         ncia_cls_notify(ncia_ptr, CLS_REQ_OPNSTN_CNF, CLS_OK, pak);
         ncia_cls_notify(ncia_ptr, CLS_CONNECT_IND, CLS_OK, NULL);
         break;

    case CLS_REQ_OPNSTN_CNF_FAIL:
         if (!ncia_ptr->fNciaClient->NciaCircuit.qhead)
             ncia_client_fsm(ncia_ptr->fNciaClient, CIRCUIT_FREED);
         ncia_cls_notify(ncia_ptr, CLS_REQ_OPNSTN_CNF, CLS_TIMEOUT, pak);
         break;

    case CLS_NULL_ID_STN_IND:
         pak->datagramsize = 0;
         ncia_cls_notify(ncia_ptr, CLS_ID_STN_IND, CLS_OK, pak);
         break;

    case CLS_CONNECT_CNF_FAIL:
         ncia_cls_notify(ncia_ptr, CLS_CONNECT_CNF, CLS_TIMEOUT, pak);
         break;

    case CLS_DISC_CNF_OR_CONN_CNF_FAIL:
         if (ncia_ptr->fNciaCepFlags & NCIA_DISC_CNF_PENDING) {
             ncia_ptr->fNciaCepFlags &= ~NCIA_DISC_CNF_PENDING;
             ncia_cls_notify(ncia_ptr, CLS_DISC_CNF, CLS_OK, pak);
         } else
             ncia_cls_notify(ncia_ptr, CLS_CONNECT_CNF, CLS_TIMEOUT, pak);
         break;

    case CLS_FREE_XID:
         if (ncia_ptr->fXid) {
             datagram_done(ncia_ptr->fXid);
             ncia_ptr->fXid = NULL;
         }
         break;

    case CLS_CONNECT_CNF:
         ncia_ptr->fNciaCepFlags |= NciaClsBusy;
         NciaCircuitDebugState(" --> CLS_CONNECT_CNF sets NciaClsBusy");
         ncia_cls_notify(ncia_ptr, CLS_CONNECT_CNF, CLS_OK, pak);
         break;

    case CLS_DATA_IND:
         /* only DATA_IND is subject flow control in CLS */
         if (ncia_ptr->fNciaCepFlags & NciaClsBusy) {
             NciaCircuitDebugState(" enqueue DATA due to flow control");
             enqueue(&ncia_ptr->fInputQ, pak);
         } else
             ncia_cls_notify(ncia_ptr, action->input, CLS_OK, pak);
         break;

    case CLS_ID_STN_IND:
    case CLS_TEST_STN_IND:
    case CLS_CONNECT_STN_IND:
    case CLS_UDATA_STN_IND:
         ncia_cls_notify(ncia_ptr, action->input, CLS_OK, pak);
         break;

    case CLS_CLOSE_STN_CNF:
         if (!ncia_ptr->fNciaClient->NciaCircuit.qhead)
             ncia_client_fsm(ncia_ptr->fNciaClient, CIRCUIT_FREED);
         ncia_cls_notify(ncia_ptr, action->input, CLS_OK, pak);
         break;

    case CLS_REQ_OPNSTN_CNF:
         ncia_insert_cep(ncia_ptr);
         /* fall through */

    default:
         ncia_cls_notify(ncia_ptr, action->input, CLS_OK, pak);
         break;
    }
    return(FSM_OK);
}


static
char *ncia_print_pre_server_event (int event)
{
    switch(event) {
    case BUSY_XPORT:       return("BUSY_XPORT");
    case UNBUSY_XPORT:     return("UNBUSY_XPORT");
    case CONN_OPENED:      return("CONN_OPENED");
    case NDLC_TCP_DOWN:    return("NDLC_TCP_DOWN");
    case NDLC_PRIMITIVES:  return("NDLC_PRIMITIVES");
    default:               return("Unknown event");
    }
}



void ncia_record_ckt_info (ndlc_msg_type_t ndlc, ndlc_start_dl_t *start_dl, 
           NciaCepT *ncia_ptr)
{

    if (ndlc == NDLC_START_DL) {
        ncia_ptr->fClientCktId = start_dl->origin_ckt_id;
    } else if (ndlc == NDLC_DL_STARTED) {
        ncia_ptr->fClientCktId = start_dl->target_ckt_id;
    } else
        return;

    ncia_ptr->fInitWindowSize = start_dl->window_size;
    ncia_ptr->fCurrentWindow = start_dl->window_size;
}





/*
 * return TRUE if under flow control
 * return FALSE if allow to send
 */
boolean ncia_flow_control_out (NciaCepT *ncia_ptr, ndlc_packet_t *ndlc_packet, 
                               int ndlc)
{
    uchar *flow_ctl = 0;

    if (!ncia_ptr) {
        NciaCircuitDebugFlow(
            "\nNCIA: can't do output flow control with circuit CB");
        return (FALSE);
    }

    switch(ndlc) {
    case NDLC_INFO_FRAME:
    case NDLC_DGRM_FRAME:
         if (ncia_ptr->fGrantPacket == 0) { 
             NciaCircuitDebugFlow("\nNCIA: Granted packet is zero");
             return(TRUE);
         }
         ncia_ptr->fGrantPacket--;
         /* fall through */

    case NDLC_XID_FRAME:
    case NDLC_CONTACT_STN:
    case NDLC_STN_CONTACTED:
    case NDLC_FCM_FRAME:
         flow_ctl = &ndlc_packet->ndlc_data.xid_frame.flow_control;
         break;

    default:
         NciaCircuitDebugFlow("\nNCIA: no flow control in %s frame", 
                              ncia_print_ndlc(ndlc));
         /* no flow control on other frame */
         return (FALSE);
    }

    if (*flow_ctl)
       *flow_ctl = 0;

    /*
     * for receiver
     */
    if (!ncia_ptr->fFCACKOwed) {
      if (ncia_ptr->fFCFlag) {
        switch (ncia_ptr->fFCFlag) {
        case NciaRepeatWindowOp:
             NciaCircuitDebugFlow("\nNCIA: send Repeat Window Op for %x", 
                                  ncia_ptr);
             ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
             ncia_ptr->fFCACKOwed = 1;
             *flow_ctl |= ncia_ptr->fFCFlag | NCIA_FCI;
             break;
 
        case NciaIncrementWindowOp:
             NciaCircuitDebugFlow("\nNCIA: send Increment Window Op for %x",
                                  ncia_ptr);
             ncia_ptr->fClientCurrentWindow++;
             ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
             ncia_ptr->fFCACKOwed = 1;
             *flow_ctl |= ncia_ptr->fFCFlag | NCIA_FCI;
             break;
 
        case NciaDecrementWindowOp:
             NciaCircuitDebugFlow("\nNCIA: send Decrement Window Op for %x",
                                  ncia_ptr);
             if (ncia_ptr->fClientCurrentWindow > 1)
                 ncia_ptr->fClientCurrentWindow--;
             ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
             ncia_ptr->fFCACKOwed = 1;
             *flow_ctl |= ncia_ptr->fFCFlag | NCIA_FCI;
             break;
 
        case NciaResetWindowOp:
             NciaCircuitDebugFlow("\nNCIA: send Reset Window Op for %x", 
                                  ncia_ptr);
             ncia_ptr->fClientGrantPacket = 0;
             ncia_ptr->fClientCurrentWindow = 0;
             ncia_ptr->fFCACKOwed = 1;
             *flow_ctl |= ncia_ptr->fFCFlag | NCIA_FCI;
             break;
 
        case NciaHalveWindowOp:
             NciaCircuitDebugFlow("\nNCIA: send Halve Window Op for %x", 
                                  ncia_ptr);
             if (ncia_ptr->fClientCurrentWindow > 1)
                 ncia_ptr->fClientCurrentWindow /= 2;
             ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
             ncia_ptr->fFCACKOwed = 1;
             *flow_ctl |= ncia_ptr->fFCFlag | NCIA_FCI;
             break;
 
        default:
             NciaCircuitDebugFlow(
                 "\nNCIA: Invalid outgoing flow control operator %x",
                 ncia_ptr->fFCFlag);
             break;
        }
        ncia_ptr->fFCFlag = 0;
      } else {
        /*
         *  check if client has enough tokens to send
         */
        if (ncia_ptr->fClientGrantPacket < NCIA_MAX_GRANTED_PACKET_SIZE) {
            NciaCircuitDebugFlow(
                "\nNCIA: grant client more packet by sending Repeat Window Op");
            if (ncia_ptr->fClientCurrentWindow < NCIA_MAX_WINDOW) {
                *flow_ctl |= NciaIncrementWindowOp | NCIA_FCI;
                ncia_ptr->fClientCurrentWindow++;
                ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
            } else {
                *flow_ctl |= NciaRepeatWindowOp | NCIA_FCI;
                ncia_ptr->fClientGrantPacket += ncia_ptr->fClientCurrentWindow;
            }
            ncia_ptr->fFCACKOwed = 1;
        } else 
            NciaCircuitDebugFlow("\nNCIA: Flow Control is not set");
      }
    } else
      NciaCircuitDebugFlow("\nNCIA: Client owes server FCA");

    /*
     * for sender
     */
    if (ncia_ptr->fNciaCepFlags & NciaSendFCA) {
        ncia_ptr->fNciaCepFlags &= ~NciaSendFCA;
        *flow_ctl |= NCIA_FCA;
    }

    NciaCircuitDebugFlow(
"\nNCIA: flow_control_out FC: 0x%x, IW %d GP %d CW %d, Client IW %d GP %d CW %d"
                , *flow_ctl, ncia_ptr->fInitWindowSize, ncia_ptr->fGrantPacket,
                ncia_ptr->fCurrentWindow, ncia_ptr->fClientInitWindowSize,
                ncia_ptr->fClientGrantPacket, ncia_ptr->fClientCurrentWindow);

    return(FALSE);
}




static
void ncia_flow_control_in (NciaCepT *ncia_ptr, uchar flow_ctl, int ndlc)
{
    if (!ncia_ptr) {
        NciaCircuitDebugFlow(
            "\nNCIA: can't do input flow control with circuit CB");
        return;
    }

    if (ndlc == NDLC_INFO_FRAME || ndlc == NDLC_DGRM_FRAME)
        if (ncia_ptr->fClientGrantPacket > 0) {
            ncia_ptr->fClientGrantPacket--;
            NciaCircuitDebugFlow(
                "\nNCIA: reduce ClientGrantPacket by 1 (Granted: %d)",
                ncia_ptr->fClientGrantPacket);
        } else
            NciaCircuitDebugFlow(
                "\nNCIA: client shouldn't send packet: Grant Packet is %d", 
                ncia_ptr->fClientGrantPacket);

    /* 
     * for receiver 
     */
    if (ncia_ptr->fFCACKOwed) {
        if (flow_ctl & NCIA_FCA) {
            NciaCircuitDebugFlow("\nNCIA: receive FCA for circuit %x",ncia_ptr);
            ncia_ptr->fFCACKOwed = 0;
         } else
            NciaCircuitDebugFlow("\nNCIA: Invalid flow control NCIA_FCA");
    }

    /* 
     * for sender 
     */
    if (flow_ctl & NCIA_FCI) {
        switch(flow_ctl & NCIA_FCO_MASK) {
        case NciaRepeatWindowOp:
             NciaCircuitDebugFlow(
                 "\nNCIA: receive Repeat Window Op for circuit %x", ncia_ptr);
             ncia_ptr->fGrantPacket += ncia_ptr->fCurrentWindow;
             ncia_ptr->fNciaCepFlags |= NciaSendFCA;
             break;

        case NciaIncrementWindowOp:
             NciaCircuitDebugFlow(
                "\nNCIA: receive Increment Window Op for circuit %x", ncia_ptr);
             ncia_ptr->fCurrentWindow++;
             ncia_ptr->fGrantPacket += ncia_ptr->fCurrentWindow;
             ncia_ptr->fNciaCepFlags |= NciaSendFCA;
             break;

        case NciaDecrementWindowOp:
             NciaCircuitDebugFlow(
                "\nNCIA: receive Decrement Window Op for circuit %x", ncia_ptr);
             if (ncia_ptr->fCurrentWindow > 1)
                 ncia_ptr->fCurrentWindow--;
             ncia_ptr->fGrantPacket += ncia_ptr->fCurrentWindow;
             ncia_ptr->fNciaCepFlags |= NciaSendFCA;
             break;

        case NciaResetWindowOp:
             NciaCircuitDebugFlow(
                 "\nNCIA: receive Reset Window Op for circuit %x", ncia_ptr);
             if (ndlc == NDLC_FCM_FRAME) {
                 ncia_ptr->fGrantPacket = 0;
                 ncia_ptr->fCurrentWindow = 0;
                 ncia_ptr->fNciaCepFlags |= NciaSendFCA;
                 ncia_send_ndlc(ncia_ptr, NDLC_FCM_FRAME);
             } else
                 NciaCircuitDebugFlow(
                     "\nNCIA: Invalid Reset Window Operator in %s",
                     ncia_print_ndlc(ndlc));
             break;

        case NciaHalveWindowOp:
             NciaCircuitDebugFlow(
                 "\nNCIA: receive Halve Window Op for circuit %x", ncia_ptr);
             if (ncia_ptr->fCurrentWindow > 1)
                 ncia_ptr->fCurrentWindow /= 2;
             ncia_ptr->fGrantPacket += ncia_ptr->fCurrentWindow;
             ncia_ptr->fNciaCepFlags |= NciaSendFCA;
             break;

        default:
             NciaCircuitDebugFlow("\nNCIA: Invalid flow control operator %x", 
                                       flow_ctl & NCIA_FCI);
             break;
        }

        /* we have something in queue */
        if (!QUEUEEMPTY(&(ncia_ptr->fOutputQ))) {
            if (ncia_ptr->fNciaClient->client_state == NCIA_OPENED)
                ncia_drain_circuit_outQ(ncia_ptr);
            else
                NciaCircuitDebugFlow(
                    "\nNCIA: some packets in queue but can't be sent");
        }
    }

    if (!(ncia_ptr->fNciaCepFlags & NciaClsBusy)) {
        /* CLS flow control is not on */
        paktype *pak;

        if (ncia_ptr->fClientGrantPacket == 0 && 
          !(ndlc == NDLC_XID_FRAME && ncia_ptr->fState != nciast_connected)){
            /*
             * no need to increase the window size during XID negotiation
             */
            pak = ncia_ptr->fPacket;
            ncia_ptr->fPacket = NULL;
            /* the flow control bit will be set in the outgoing 
               flow control routine */
            ncia_ptr->fFCFlag = NciaIncrementWindowOp;
            ncia_send_ndlc(ncia_ptr, NDLC_FCM_FRAME);
            ncia_ptr->fPacket = pak;
        }
    }

    NciaCircuitDebugFlow(
"\nNCIA: flow_control_in FC 0x%x, IW %d GP %d CW %d, Client IW %d GP %d CW %d",
                flow_ctl, ncia_ptr->fInitWindowSize, ncia_ptr->fGrantPacket,
                ncia_ptr->fCurrentWindow, ncia_ptr->fClientInitWindowSize,
                ncia_ptr->fClientGrantPacket, ncia_ptr->fClientCurrentWindow);
}


void ncia_print_ndlc_header (int ndlc, ndlc_packet_t *ndlc_packet, char *dir)
{
    buginf("\nNCIA(%s): Ver_Id: 0x%x, MsgType: %s, Len: %d",
        dir, ndlc_packet->ndlc_header.proto_ver_id,
        ncia_print_ndlc(ndlc_packet->ndlc_header.message_type),
        ndlc_packet->ndlc_header.packet_length);

    switch(ndlc) {
    case NDLC_CAN_U_REACH: 
    case NDLC_I_CAN_REACH:
    case NDLC_I_CANNOT_REACH:
         buginf(", tmac: %e, ssap: %d", 
                &ndlc_packet->ndlc_data.can_u_reach.target_mac[0],
                ndlc_packet->ndlc_data.can_u_reach.client_sap);
         break;

    case NDLC_START_DL:
    case NDLC_DL_STARTED:
    case NDLC_START_DL_FAILED:
         buginf(
      ", tmac: %e\n          tsap: %d, csap %d, oid: %x, tid %x, lfs %d, ws %d",
                &ndlc_packet->ndlc_data.start_dl.target_mac[0],
                ndlc_packet->ndlc_data.start_dl.target_sap,
                ndlc_packet->ndlc_data.start_dl.client_sap,
                ndlc_packet->ndlc_data.start_dl.origin_ckt_id,
                ndlc_packet->ndlc_data.start_dl.target_ckt_id,
                ndlc_packet->ndlc_data.start_dl.largest_frame_size,
                ndlc_packet->ndlc_data.start_dl.window_size);
         break;

    case NDLC_XID_FRAME:
    case NDLC_CONTACT_STN:
    case NDLC_STN_CONTACTED:
    case NDLC_INFO_FRAME:
    case NDLC_FCM_FRAME:
    case NDLC_DGRM_FRAME:
         buginf(", sid: %x, FC 0x%x",
                ndlc_packet->ndlc_data.xid_frame.session_id,
                ndlc_packet->ndlc_data.xid_frame.flow_control);
         break;

    case NDLC_HALT_DL:
    case NDLC_HALT_DL_NOACK:
         buginf(", s_id: %x, r_id %x",
                ndlc_packet->ndlc_data.halt_dl.sender_ckt_id,
                ndlc_packet->ndlc_data.halt_dl.receiver_ckt_id);
         break;

    case NDLC_DL_HALTED:
         buginf(", s_id: %x, r_id %x",
                ndlc_packet->ndlc_data.halt_dl.receiver_ckt_id,
                ndlc_packet->ndlc_data.halt_dl.sender_ckt_id);
         break;

    case NDLC_DATA_FRAME:
         buginf("\n          tmac: %e, tsap %d, csap %d, btype 0x%x",
                &ndlc_packet->ndlc_data.data_frame.target_mac[0],
                ndlc_packet->ndlc_data.data_frame.target_sap,
                ndlc_packet->ndlc_data.data_frame.client_sap,
                ndlc_packet->ndlc_data.data_frame.broadcast_type);
         break;
    }
}




/*
 * check if the destination mac is valid or not.
 */
static boolean ncia_validate_dmac(nciapeertype *ncia_client, char *dmac)
{
    ncia_servertype *ncia_server;

    ncia_server = ncia_client->ncia_server;

    if (ncia_compare_macaddr(dmac, ncia_server->vmac_start,
                                ncia_server->vmac_range) == TRUE)
	/* the destination address is in the virtual mac range. */
	return(FALSE);

    if (ncia_find_client_by_mac(ncia_client->ncia_server->ncia_swidb, dmac)
        != NULL)
	/* the destination mac is one of the clients */
	return(FALSE);

    return(TRUE);
}


/*
 * server fsm pre-preocessor
 * When a packet is sent to fsm, the datagram_start is moved to the beginning
 * of the raw data.  The datagram_size is also changed.
 */
void ncia_pre_server_fsm (nciapeertype *ncia_client, int event)
{
    ndlc_packet_t *ndlc_packet;
    int           ndlc;
    NciaCepT      *ncia_ptr;
    int           major;
    int           minor;
    paktype       *pak;
    ncia_servertype *ncia_server;

    pak = ncia_client->in_pak;
    ncia_client->in_pak = NULL;
    ncia_ptr = NULL;
    ncia_server = ncia_client->ncia_server;

    NciaCircuitDebugState("\nNCIA: pre-server fsm: event %s", 
                          ncia_print_pre_server_event(event));

    switch(event) {
    case BUSY_XPORT:
         if (pak)
             datagram_done(pak);
         ForEachNciaCircuit(ncia_client, ncia_ptr)
             ncia_server_fsm(ncia_ptr, nciama_admin, nciami_admin_tcp_busy);
         return;

    case UNBUSY_XPORT:
    case CONN_OPENED:
         if (pak)
             datagram_done(pak);

         /* send packets in client queue first */
         ncia_drain_client_outputq(ncia_client);
         ForEachNciaCircuit(ncia_client, ncia_ptr)
             ncia_server_fsm(ncia_ptr, nciama_admin, nciami_admin_tcp_unbusy);
         return;

    case NDLC_TCP_DOWN:
         {
         /*
          * we have to put the circuit control block into a tmp Q in case
          * it can't be freed.  Once we go through the whole queue, we put
          * put everything back if there is any.
          */
         queuetype tmpQ;

         queue_init(&tmpQ, 0);

         if (pak)
             datagram_done(pak);

         while ((ncia_ptr = dequeue(&ncia_client->NciaCircuit))) {
             switch(ncia_ptr->fState) {
             /* with CEP */
             /* can't do anything for these two! */
             case nciast_opened:
             case nciast_opened_nodl:
                  /* put it back */
                  enqueue(&tmpQ, ncia_ptr);
                  break;

             /* no CEP */
             case nciast_start_dl_rcvd:
             case nciast_dl_started_snd:
             case nciast_closed:

             case nciast_open_wait:
             {
                 nciapeertype *ncia_client = ncia_ptr->fNciaClient;

                 if (ncia_ptr->fPacket)
                     datagram_done(ncia_ptr->fPacket);
                 if (ncia_ptr->fXid)
                     datagram_done(ncia_ptr->fXid);

                 NciaCircuitDebugState(", ckt id (%x) is freed", ncia_ptr);
                 free(ncia_ptr);

                 if (!ncia_client->NciaCircuit.qhead && !tmpQ.qhead) {
                     ncia_client_fsm(ncia_client, CIRCUIT_FREED);
                     return;
                 }
                 break;
             }

             default:
                 /* put it back */
                 enqueue(&tmpQ, ncia_ptr);
                 ncia_server_fsm(ncia_ptr, nciama_admin, nciami_admin_tcp_down);
             }
         }
         ncia_client->NciaCircuit = tmpQ;
         }
         return;

    case NDLC_PRIMITIVES:
         break;

    default:
         NciaCircuitDebugState("\nNCIA: pre-server fsm invalid event: %x ", 
                               event);
         return;
    }

    CLSASSERT_FATAL(pak);

    ndlc_packet = (ndlc_packet_t *) pak->datagramstart;
    ndlc = ndlc_packet->ndlc_header.message_type;

    if (ncia_circuit_debug & NciaCircuitDebugEVENT)
        ncia_print_ndlc_header(ndlc, ndlc_packet, "IN");

    ncia_ptr = ncia_get_cep_from_ndlc(ncia_client, pak);

    /* adjust datagramstart pointer */
    pak_delta_adjust_start(pak, ncia_get_ndlc_size(ndlc));

    if (!ncia_ptr) {
        NciaSapT *sap_registry;

        /* 
         * find all activated SAPs and send TestStn.Ind to them 
         * should we let the client to specify with DLU should be used?
         */
        sap_registry = 
          (NciaSapT *) &(ncia_client->ncia_server->sap_registry[NCIA_DLSW_SAP]);

        switch (ndlc) {
        case NDLC_DATA_FRAME: {
             /* only send UDATA to DLSw+ */
             ClsDlcParamBlockT  clsDlcParamBlock;
             ndlc_data_frame_t  *data_frame;
 
             if (!sap_registry->fUSapId) {
                  NciaCircuitDebugState(
             "\nNCIA: pre-server fsm: local switch is not up, free data_frame");
                  datagram_done(pak);
                  return;
              }
 
             data_frame = (ndlc_data_frame_t *) 
                           &(ndlc_packet->ndlc_data.data_frame);
 
             NciaSetupUControlStnInd(data_frame->target_sap, 
                             data_frame->client_sap,
                             &data_frame->target_mac[0],
                             &ncia_client->Vmac[0], LLC1_UI, pak);
 
             NciaCircuitDebugState("\nNCIA: send UdataStn.Ind to Local Switch");
             NciaSendUControlStnInd(&clsDlcParamBlock, pak, 
                             (ClsUSapIdT) (sap_registry->fUSapId));
             }
             return;

        case NDLC_CAN_U_REACH: {
             ClsDlcParamBlockT   clsDlcParamBlock;
             ndlc_can_u_reach_t *can_u_reach;

             can_u_reach = (ndlc_can_u_reach_t *) 
                           &(ndlc_packet->ndlc_data.can_u_reach);
 
             if (ncia_validate_dmac(ncia_client, &can_u_reach->target_mac[0])
                 == FALSE) {
                 NciaCircuitDebugState(
                     "\nNCIA: pre-server fsm: invalid dmac %e", 
                     &can_u_reach->target_mac[0]);
                 datagram_done(pak);
                 return;
             }
 
             if (ieee_equal(&(can_u_reach->target_mac[0]), 
                            &(ncia_server->server_mac[0]))) {
                 /* reply to client */
                 ncia_send_i_can_reach(ncia_client, pak);
 
                 return;
             }

             if (ncia_find_circuit(ncia_client, &can_u_reach->target_mac[0], 
                                   can_u_reach->client_sap, NCIA_DONT_CARE)) {
                 ncia_send_i_can_reach(ncia_client, pak);
                 return;
             }
 
             if (!sap_registry->fUSapId) {
                  NciaCircuitDebugState(
             "\nNCIA: pre-server fsm: local switch is not up, free data_frame");
                  datagram_done(pak);
                  return;
             }
 
             /*
              * send to DLSw, fsm is not involved. 
              * because DLSw+ doesn't support non-zero DSAP for test frame,
              * set DSAP to zero.  Set SSAP to zero, too.
              */
             NciaSetupUControlStnInd(0, can_u_reach->client_sap,
                             &can_u_reach->target_mac[0],
                             &ncia_client->Vmac[0], LLC1_TEST, pak);
 
             NciaServerDebugAll("\nNCIA: send TestStn.Ind to DLU");
             DlcSendUControlStnInd(&clsDlcParamBlock, pak, 
                             LLC1_TEST, FALSE,
                             (ClsUSapIdT) (sap_registry->fUSapId), 
                             (DlcPCepIdT) CLS_NO_CEP);
             }
             return;

        case NDLC_START_DL: {
             ndlc_start_dl_t *start_dl;
             NciaSapT *Sap;
             int ii = 0;
             int local = FALSE;

             start_dl = &ndlc_packet->ndlc_data.start_dl;
 
             if (ncia_validate_dmac(ncia_client, &start_dl->target_mac[0])
                 == FALSE) {
                 NciaCircuitDebugState(
                     "\nNCIA: pre-server fsm: invalid dmac %e", 
                     &start_dl->target_mac[0]);
                 datagram_done(pak);
                 return;
             }

             if (ieee_equal(&(start_dl->target_mac[0]), 
                            &(ncia_server->server_mac[0]))) {
                 /*
                  * to local DSPU or APPN
                  */
                 for (ii=1; ii<NCIA_MAX_SAP; ii++)
                     if (sap_registry[ii].fSapValue == start_dl->target_sap)
                         break;

                 if (ii == NCIA_MAX_SAP) {
                     NciaCircuitDebugState(
              "\nNCIA: pre-server fsm: local sap is not up, free can_u_reach");
                     datagram_done(pak);
                     return;
                 }
                 local = TRUE;
             } else
             if (!sap_registry->fUSapId) {
                  NciaCircuitDebugState(
             "\nNCIA: pre-server fsm: local switch is not up, free data_frame");
                  datagram_done(pak);
                  return;
             }

             Sap = &sap_registry[ii];

             ncia_ptr = ncia_circuit_create(ncia_client, start_dl->target_mac,
                      start_dl->target_sap, start_dl->client_sap, Sap);
             if (!ncia_ptr) {
                 datagram_done(pak);
                 return;
             }

             ncia_ptr->fPacket = pak;
             ncia_record_ckt_info(ndlc, start_dl, ncia_ptr);

             if (local) {
                 /*
                  * we have to respond to this because non-DLSw DLU
                  * doesn't handle Test.Req.
                  */
                 if (ncia_ptr->fState == nciast_closed) {
                     NciaCircuitDebugState(
                         "\nNCIA: pre-server fsm: send DL_STARTED back");
                     ncia_ptr->fState = nciast_dl_started_snd;
                     ncia_send_ndlc(ncia_ptr, NDLC_DL_STARTED);
                 } else {
                     datagram_done(ncia_ptr->fPacket);
                     ncia_ptr->fPacket = NULL;
                 }
                 return;
             }
             }
             break;

        default:
             NciaCircuitDebugState("\nNCIA: pre-server fsm: free packet");
             datagram_done(pak);
             return;
        }
    } else {
        boolean  found = FALSE;
        NciaCepT *ncia_circuit;

        ForEachNciaCircuit(ncia_client, ncia_circuit)
            if ((ulong) ncia_circuit == (ulong) ncia_ptr) {
                found = TRUE;
                break;
            }

        if (!found) {
            NciaCircuitDebugError("\nNCIA: Invalid circuit id %x", ncia_ptr);
            datagram_done(pak);
            return;
        }
        ncia_ptr->fPacket = pak;

        if (ndlc == NDLC_DL_STARTED)
             ncia_record_ckt_info(ndlc, &ndlc_packet->ndlc_data.dl_started,
                                  ncia_ptr);
    }


    switch(ndlc){
    case NDLC_CAN_U_REACH:
         minor = nciami_wan_canureach;
         break;
    case NDLC_START_DL:
         minor = nciami_wan_start_dl;
         break;
    case NDLC_DATA_FRAME:
         minor = nciami_wan_dataframe;
         break;
    case NDLC_DL_STARTED:
         minor = nciami_wan_dl_started;
         break;
    case NDLC_START_DL_FAILED:
         minor = nciami_wan_start_dl_failed;
         break;
    case NDLC_XID_FRAME:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.xid_frame.flow_control, ndlc);
         minor = nciami_wan_xidframe;
         break;
    case NDLC_CONTACT_STN:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.contact_stn.flow_control, ndlc);
         minor = nciami_wan_contact_stn;
         break;
    case NDLC_STN_CONTACTED:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.stn_contacted.flow_control, ndlc);
         minor = nciami_wan_stn_contacted;
         break;
    case NDLC_INFO_FRAME:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.info_frame.flow_control, ndlc);
         minor = nciami_wan_infoframe;
         break;
    case NDLC_HALT_DL:
         minor = nciami_wan_halt_dl;
         break;
    case NDLC_HALT_DL_NOACK:
         minor = nciami_wan_halt_dl_noack;
         break;
    case NDLC_DL_HALTED:
         minor = nciami_wan_dl_halted;
         break;
    case NDLC_FCM_FRAME:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.fcm_frame.flow_control, ndlc);
         minor = nciami_wan_imp;
         break;
    case NDLC_DGRM_FRAME:
         ncia_flow_control_in(ncia_ptr, 
                    ndlc_packet->ndlc_data.dgrm_frame.flow_control, ndlc);
         minor = nciami_wan_dgrmframe;
         break;
    default:
         NciaCircuitDebugError(
             "\nNCIA: Invalid message type %x in pre-server fsm", ndlc);
         return;
    }

    major = nciama_wan;
    ncia_server_fsm(ncia_ptr, major, minor);
}




/*
 * doesn't have circuit control block, can't use ncia_send_ndlc
 */
static
void ncia_send_i_can_reach (nciapeertype *ncia_client, paktype *pak)
{
    ndlc_packet_t      *ndlc_ptr;

    pak_delta_adjust_start(pak, -1 * ncia_get_ndlc_size(NDLC_I_CAN_REACH));

    ndlc_ptr = (ndlc_packet_t *) pak->datagramstart;

    ndlc_ptr->ndlc_header.message_type = NDLC_I_CAN_REACH;

    if (ncia_circuit_debug & NciaCircuitDebugEVENT)
        ncia_print_ndlc_header(NDLC_I_CAN_REACH, ndlc_ptr, "OUT");

    if (ncia_client->out_pak) {
        if (ncia_client->NciaFlags & NciaClientDebugERROR)
            buginf("\nNCIA:no outpak found");
        datagram_done(ncia_client->out_pak);
    } if (ncia_client->client_state == BUSY_XPORT)
        enqueue(&ncia_client->OutputQ, pak);
    else {
        ncia_client->out_pak = pak;
        ncia_pre_client_fsm(ncia_client, NDLC_FSM_INPUT);
    }
}


char *ncia_print_circuit_state (nciast_t state)
{
    switch (state) {
      case nciast_closed:          return("CLSOED");
      case nciast_opening:         return("OPENING");
      case nciast_opened:          return("OPENED");
      case nciast_conn_pending:    return("CONNECT_PENDING");
      case nciast_connected:       return("CONNECTED");
      case nciast_disc_pending:    return("DISC_PENDING");
      case nciast_opened_nodl:     return("OPENED_NODL");
      case nciast_dl_started_wait: return("START_WAIT");
      case nciast_start_dl_rcvd:   return("START_DL_RCVD");
      case nciast_dl_started_snd:  return("DL_STARTED_SND");
      case nciast_dls_wait_open:   return("DL_STARTED_WAIT_OPEN");
      case nciast_connect_wait:    return("CONNECT_WAIT");
      case nciast_open_wait:       return("OPEN_WAIT");
      case nciast_disc_wait:       return("DISC_WAIT");
      case nciast_disc_wait_noack: return("DISC_WAIT_NOACK");
      case nciast_opening_halt:    return("OPENING_HALT");
      default:                     return("UNKNOWN");
    }
}


char *ncia_print_action (int input)
{
    switch(input) {
    case CLS_NO_INPUT:                       return("CLS_NO_INPUT");
    case CLS_REQ_OPNSTN_CNF_AND_CONNECT_IND: return(
                                          "CLS_REQ_OPNSTN_CNF_AND_CONNECT_IND");
    case CLS_REQ_OPNSTN_CNF_FAIL:            return("CLS_REQ_OPNSTN_CNF_FAIL");
    case CLS_NULL_ID_STN_IND:                return("CLS_NULL_ID_STN_IND");
    case CLS_CONNECT_CNF_FAIL:               return("CLS_CONNECT_CNF_FAIL");
    case CLS_DISC_CNF_OR_CONN_CNF_FAIL:      return(
                                               "CLS_DISC_CNF_OR_CONN_CNF_FAIL");
    case CLS_FREE_XID:                       return("CLS_FREE_XID");
    case CLS_ENABLE_CNF:                     return("CLS_ENABLE_CNF");
    case CLS_DISABLE_CNF:                    return("CLS_DISABLE_CNF");
    case CLS_ACTIVATE_SAP_CNF:               return("CLS_ACTIVATE_SAP_CNF");
    case CLS_MODIFY_SAP_CNF:                 return("CLS_MODIFY_SAP_CNF");
    case CLS_DEACTIVATE_SAP_CNF:             return("CLS_DEACTIVATE_SAP_CNF");
    case CLS_REQ_OPNSTN_CNF:                 return("CLS_REQ_OPNSTN_CNF");
    case CLS_CLOSE_STN_CNF:                  return("CLS_CLOSE_STN_CNF");
    case CLS_MODIFY_STN_CNF:                 return("CLS_MODIFY_STN_CNF");
    case CLS_TEST_IND:                       return("CLS_TEST_IND");
    case CLS_ID_IND:                         return("CLS_ID_IND");
    case CLS_TEST_STN_IND:                   return("CLS_TEST_STN_IND");
    case CLS_UDATA_STN_IND:                  return("CLS_UDATA_STN_IND");
    case CLS_UDATA_IND:                      return("CLS_UDATA_IND");
    case CLS_CONNECT_STN_IND:                return("CLS_CONNECT_STN_IND");
    case CLS_ID_STN_IND:                     return("CLS_ID_STN_IND");
    case CLS_ACTIVATE_RING_CNF:              return("CLS_ACTIVATE_RING_CNF");
    case CLS_DEACTIVATE_RING_CNF:            return("CLS_DEACTIVATE_RING_CNF");
    case CLS_CONNECT_IND:                    return("CLS_CONNECT_IND");
    case CLS_CONNECT_CNF:                    return("CLS_CONNECT_CNF");
    case CLS_CONNECTED_IND:                  return("CLS_CONNECTED_IND");
    case CLS_DISC_IND:                       return("CLS_DISC_IND");
    case CLS_DISC_CNF:                       return("CLS_DISC_CNF");
    case CLS_DATA_IND:                       return("CLS_DATA_IND");
    case CLS_DATA_CNF:                       return("CLS_DATA_CNF");
    case CLS_PONG_CNF:                       return("CLS_PONG_CNF");
    case CLS_FLOW_IND:                       return("CLS_FLOW_IND");
    default:                                 return("Unknown");
    }
}


char *ncia_print_ndlc (ndlc_msg_type_t ndlc)
{
    switch(ndlc) {
    case NDLC_CAN_U_REACH:     return("NDLC_CAN_U_REACH");
    case NDLC_I_CAN_REACH:     return("NDLC_I_CAN_REACH");
    case NDLC_I_CANNOT_REACH:  return("NDLC_I_CANNOT_REACH");
    case NDLC_START_DL:        return("NDLC_START_DL");
    case NDLC_DL_STARTED:      return("NDLC_DL_STARTED");
    case NDLC_START_DL_FAILED: return("NDLC_START_DL_FAILED");
    case NDLC_XID_FRAME:       return("NDLC_XID_FRAME");
    case NDLC_CONTACT_STN:     return("NDLC_CONTACT_STN");
    case NDLC_STN_CONTACTED:   return("NDLC_STN_CONTACTED");
    case NDLC_DATA_FRAME:      return("NDLC_DATA_FRAME");
    case NDLC_INFO_FRAME:      return("NDLC_INFO_FRAME");
    case NDLC_HALT_DL:         return("NDLC_HALT_DL");
    case NDLC_HALT_DL_NOACK:   return("NDLC_HALT_DL_NOACK");
    case NDLC_DL_HALTED:       return("NDLC_DL_HALTED");
    case NDLC_FCM_FRAME:       return("NDLC_FCM_FRAME");
    case NDLC_DGRM_FRAME:      return("NDLC_DGRM_FRAME");
    case NDLC_CAP_XCHG:        return("NDLC_CAP_XCHG");
    case NDLC_CLOSE_PEER_REQ:  return("NDLC_CLOSE_PEER_REQ");
    case NDLC_CLOSE_PEER_RSP:  return("NDLC_CLOSE_PEER_RSP");
    case NDLC_PEER_TEST_REQ:   return("NDLC_PEER_TEST_REQ");
    case NDLC_PEER_TEST_RSP:   return("NDLC_PEER_TEST_RSP");
    default:                   return("Invalid ndlc op");
    }
}
