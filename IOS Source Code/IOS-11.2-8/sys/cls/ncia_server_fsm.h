/* $Id: ncia_server_fsm.h,v 3.1.2.6 1996/06/26 19:53:05 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_server_fsm.h,v $
 *------------------------------------------------------------------
 * ncia_server_fsm.h - Definitions for NCIA Server FSM
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_server_fsm.h,v $
 * Revision 3.1.2.6  1996/06/26  19:53:05  schiang
 * CSCdi61191:  incorrectly declare functions as inlines in
 * ncia_server_fsm.h
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/19  05:21:14  schiang
 * CSCdi60757:  HALT_DL_NO_ACK is not sent in some condition
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/18  19:01:49  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/12  06:51:54  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/21  17:50:42  schiang
 * CSCdi58302:  ncia_ndlc_classify does not have a default return
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.4  1996/05/14  18:42:26  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/03  22:06:28  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/26  07:33:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1.4.1  1996/04/05  06:05:47  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:20:27  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include"master.h"
#include"globs.h"
#include"packet.h"
#include"logger.h"
#include"config.h"
#include"address.h"
#include"mgd_timers.h"
#include"ieee.h"
#include"types.h"

#include"../util/fsm.h"

#include"../ibm/sna_util.h"
#include"../ibm/libibm_externs.h"
#include"../ibm/netbios.h"
#include"../cls/clsi_msg.h"
#include"../ui/debug.h"

#define NCIA_DISC_CNF_PENDING 0x01


/*
 * ################################################################
 *                      STATES
 * ################################################################
 */

typedef enum {
    nciast_undefined = 0,
    nciast_closed,          /* Activate_Sap.Cnf or Activate_Ring.Cnf sent */
    nciast_opening,         /* Req_OpnStn.Req received, START_DL sent */
    nciast_opened,          /* DL_STARTED received, Req_OpnStn.Cnf sent */
    nciast_conn_pending,    /* Connect.Req received, CONTACT_STN sent */
    nciast_connected,       /* STN_CONTACTED received, Connect.Cnf sent */
    nciast_disc_pending,    /* Disconnect.Req received, HALT_DL sent */
    nciast_opened_nodl,     /* DL_HALTED received, Disconnect.Cnf sent */
    nciast_dl_started_wait, /* Xid.Req received at opened_nodl, START_DL sent */
    nciast_start_dl_rcvd,   /* START_DL received at Activated, 
                               Test_Stn.Ind sent */
    nciast_dl_started_snd,  /* Test_Stn.Rsp received, DL_STARTED sent */
    nciast_dls_wait_open,   /* in nciast_opened_nodl, Connect.Req received, 
                               START_DL sent */
    nciast_connect_wait,    /* StnOpened. CONTACT_STN received, 
                               Connect.Ind sent */
    nciast_open_wait,       /* SapActed. CONTACT_STN received, 
                               Connect_Stn.Ind sent */
    nciast_disc_wait,       /* in nciast_conn_pending, nciast_connected,
                               nciast_connect_wait, or nciast_open_wait,
                               HALT_DL received, Disconnect.Ind sent */
    nciast_disc_wait_noack, /* in nciast_conn_pending, nciast_connected,
                               nciast_connect_wait, nciast_open_wait, 
                               or nciast_disc_wait
                               HALT_DL_NOACK received, Disconnect.Ind sent */
    nciast_opening_halt,    /* in dl_started_wait_open, START_DL sent */
    nciast_fsm_max = nciast_opening_halt
} nciast_t;



/*
 * ################################################################
 *                      EVENTS - MAJOR CATEGORIES
 * ################################################################
 */
 
typedef enum {
    nciama_admin = 1,
    nciama_wan,
    nciama_dlu,
    nciama_timer,
    nciama_max = nciama_timer
} nciama_t;
 
 


typedef enum {
    nciami_admin_stop = 1,        /* clear circuit command */
    nciami_admin_error,
    nciami_admin_tcp_down,
    nciami_admin_tcp_busy,
    nciami_admin_tcp_unbusy,
    nciami_admin_max = nciami_admin_tcp_unbusy
} nciami_admin_t;



typedef enum {
    nciami_cls_req_opn_stn_req = 1,
    nciami_cls_close_stn_req,
    nciami_cls_conn_req,
    nciami_cls_conn_rsp,
    nciami_cls_disc_req,
    nciami_cls_disc_rsp,
    nciami_cls_data_req,
    nciami_cls_id_req,
    nciami_cls_id_rsp,
    nciami_cls_test_req,
    nciami_cls_udata_req,

    nciami_cls_id_stn_req,
    nciami_cls_test_stn_req,
    nciami_cls_test_stn_rsp,
    nciami_cls_udata_stn_req,

    nciami_cls_flow_stop,
    nciami_cls_flow_start,
    nciami_dlc_max = nciami_cls_flow_start
} nciami_dlu_t;



typedef enum {
    nciami_wan_unknown,
    nciami_wan_canureach = 1,
    nciami_wan_dataframe,
    nciami_wan_start_dl,
    nciami_wan_dl_started,
    nciami_wan_start_dl_failed,
    nciami_wan_dgrmframe,
    nciami_wan_xidframe,
    nciami_wan_contact_stn,
    nciami_wan_stn_contacted,
    nciami_wan_infoframe,
    nciami_wan_halt_dl,
    nciami_wan_halt_dl_noack,
    nciami_wan_dl_halted,
    nciami_wan_imp,
    nciami_wan_max = nciami_wan_imp
} nciami_wan_t;



typedef enum {
    nciami_timer_expired,
    nciami_timer_max = nciami_timer_expired
} nciami_timer_t;





static inline nciami_wan_t ncia_ndlc_classify (ndlc_msg_type_t opcode)
{
    switch (opcode) {
        case NDLC_CAN_U_REACH:        return(nciami_wan_canureach);
        case NDLC_START_DL:           return(nciami_wan_start_dl);
        case NDLC_DL_STARTED:         return(nciami_wan_dl_started);
        case NDLC_START_DL_FAILED:    return(nciami_wan_start_dl_failed);
        case NDLC_DGRM_FRAME:         return(nciami_wan_dgrmframe);
        case NDLC_XID_FRAME:          return(nciami_wan_xidframe);
        case NDLC_CONTACT_STN:        return(nciami_wan_contact_stn);
        case NDLC_STN_CONTACTED:      return(nciami_wan_stn_contacted);
        case NDLC_DATA_FRAME:         return(nciami_wan_dataframe);
        case NDLC_INFO_FRAME:         return(nciami_wan_infoframe);
        case NDLC_HALT_DL:            return(nciami_wan_halt_dl);
        case NDLC_HALT_DL_NOACK:      return(nciami_wan_halt_dl_noack);
        case NDLC_DL_HALTED:          return(nciami_wan_dl_halted);
        case NDLC_FCM_FRAME:          return(nciami_wan_imp);
        default:
    };
    return(nciami_wan_unknown);
}





typedef enum {
    NCIA_ACTION_INPUT_MIN = CLS_MAX_OP,
    CLS_REQ_OPNSTN_CNF_AND_CONNECT_IND,
    CLS_REQ_OPNSTN_CNF_FAIL,
    CLS_NO_INPUT,
    CLS_NULL_ID_STN_IND,
    CLS_CONNECT_CNF_FAIL,
    CLS_DISC_CNF_OR_CONN_CNF_FAIL,
    CLS_FREE_XID,
} ncia_action_input_t;

typedef struct {
    int input;
} ncia_pb_t;

static const ncia_pb_t NCIA_CSC     = { CLS_CLOSE_STN_CNF };   /* NCloStnCnf  */
static const ncia_pb_t NCIA_CC      = { CLS_CONNECT_CNF };     /* NConnCnf    */
static const ncia_pb_t NCIA_CCF     = { CLS_CONNECT_CNF_FAIL };
                                                              /* NConnCnfFail */
static const ncia_pb_t NCIA_CI      = { CLS_CONNECT_IND };     /* NConnInd    */
static const ncia_pb_t NCIA_CSI     = { CLS_CONNECT_STN_IND }; /* NConnStnInd */
static const ncia_pb_t NCIA_DATA    = { CLS_DATA_IND };        /* NDataInd    */
static const ncia_pb_t NCIA_DC      = { CLS_DISC_CNF };        /* NDisConnCnf */
static const ncia_pb_t NCIA_DI      = { CLS_DISC_IND };        /* NDisConnInd */
static const ncia_pb_t NCIA_DC_CCF  = { CLS_DISC_CNF_OR_CONN_CNF_FAIL }; 
                                                          /* NDiscC_ConnCFail */
static const ncia_pb_t NCIA_FI      = { CLS_FREE_XID };        /* NFreeXid    */
static const ncia_pb_t NCIA_ISI     = { CLS_ID_STN_IND };      /* NIdStnInd   */
static const ncia_pb_t NCIA_NI      = { CLS_NO_INPUT };        /* NNoInput    */
static const ncia_pb_t NCIA_NISI    = { CLS_NULL_ID_STN_IND }; /*NNullIdStnInd*/
static const ncia_pb_t NCIA_ROSC    = { CLS_REQ_OPNSTN_CNF };/* NReqOpnStnCnf */
static const ncia_pb_t NCIA_ROSC_CI = { CLS_REQ_OPNSTN_CNF_AND_CONNECT_IND };
                                                         /* NReqOpnStnC_ConnI */
static const ncia_pb_t NCIA_ROSCF   = { CLS_REQ_OPNSTN_CNF_FAIL };
                                                         /* NReqOpnStnCnfFail */
static const ncia_pb_t NCIA_TI      = { CLS_TEST_IND };        /* NTestCnf    */
static const ncia_pb_t NCIA_TSI     = { CLS_TEST_STN_IND };    /* NTestStnInd */
static const ncia_pb_t NCIA_UI      = { CLS_UDATA_IND };       /* NUdataInd   */
static const ncia_pb_t NCIA_USI     = { CLS_UDATA_STN_IND };  /* NUdataStnInd */
static const ncia_pb_t NCIA_XID     = { CLS_ID_IND };          /* NXid        */
static const ncia_pb_t NCIA_CEDI    = { CLS_CONNECTED_IND }; /* NConnectedInd */

extern ushort ncia_action_1 (NciaCepT *ncia_ptr);
extern ushort ncia_action_2 (NciaCepT *ncia_ptr);
extern ushort ncia_action_3 (NciaCepT *ncia_ptr);
extern ushort ncia_action_4 (NciaCepT *ncia_ptr);
extern ushort ncia_action_5 (NciaCepT *ncia_ptr);
extern ushort ncia_action_6 (NciaCepT *ncia_ptr);
extern ushort ncia_action_7 (NciaCepT *ncia_ptr);
extern ushort ncia_action_8 (NciaCepT *ncia_ptr);
extern ushort ncia_action_9 (NciaCepT *ncia_ptr);
extern ushort ncia_action_10 (NciaCepT *ncia_ptr);
extern ushort ncia_action_11 (NciaCepT *ncia_ptr);
extern ushort ncia_action_12 (NciaCepT *ncia_ptr);
extern ushort ncia_action_13 (NciaCepT *ncia_ptr);
extern ushort ncia_action_14 (NciaCepT *ncia_ptr);
extern ushort ncia_action_15 (NciaCepT *ncia_ptr);
extern ushort ncia_action_16 (NciaCepT *ncia_ptr);
extern ushort ncia_action_17 (NciaCepT *ncia_ptr);
extern ushort ncia_action_18 (NciaCepT *ncia_ptr);
extern ushort ncia_action_19 (NciaCepT *ncia_ptr);
extern ushort ncia_action_20 (NciaCepT *ncia_ptr);
extern ushort ncia_action_21 (NciaCepT *ncia_ptr);
extern ushort ncia_action_22 (NciaCepT *ncia_ptr);
extern ushort ncia_action_23 (NciaCepT *ncia_ptr);
extern ushort ncia_action_24 (NciaCepT *ncia_ptr);
extern ushort ncia_action_25 (NciaCepT *ncia_ptr);
extern ushort ncia_action_26 (NciaCepT *ncia_ptr);
extern ushort ncia_action_27 (NciaCepT *ncia_ptr);

extern ushort ncia_action_30 (NciaCepT *ncia_ptr, ncia_pb_t *input);
extern ushort ncia_action_31 (NciaCepT *ncia_ptr, ncia_pb_t *input);
extern ushort ncia_action_32 (NciaCepT *ncia_ptr, ncia_pb_t *input);
extern ushort ncia_action_33 (NciaCepT *ncia_ptr, ncia_pb_t *input);

#define NCIA_ACTION_1 FSM_ACTION_ROUTINE(ncia_action_1)
#define NCIA_ACTION_2 FSM_ACTION_ROUTINE(ncia_action_2)
#define NCIA_ACTION_3 FSM_ACTION_ROUTINE(ncia_action_3)
#define NCIA_ACTION_4 FSM_ACTION_ROUTINE(ncia_action_4)
#define NCIA_ACTION_5 FSM_ACTION_ROUTINE(ncia_action_5)
#define NCIA_ACTION_6 FSM_ACTION_ROUTINE(ncia_action_6)
#define NCIA_ACTION_7 FSM_ACTION_ROUTINE(ncia_action_7)
#define NCIA_ACTION_8 FSM_ACTION_ROUTINE(ncia_action_8)
#define NCIA_ACTION_9 FSM_ACTION_ROUTINE(ncia_action_9)
#define NCIA_ACTION_10 FSM_ACTION_ROUTINE(ncia_action_10)
#define NCIA_ACTION_11 FSM_ACTION_ROUTINE(ncia_action_11)
#define NCIA_ACTION_12 FSM_ACTION_ROUTINE(ncia_action_12)
#define NCIA_ACTION_13 FSM_ACTION_ROUTINE(ncia_action_13)
#define NCIA_ACTION_14 FSM_ACTION_ROUTINE(ncia_action_14)
#define NCIA_ACTION_15 FSM_ACTION_ROUTINE(ncia_action_15)
#define NCIA_ACTION_16 FSM_ACTION_ROUTINE(ncia_action_16)
#define NCIA_ACTION_17 FSM_ACTION_ROUTINE(ncia_action_17)
#define NCIA_ACTION_18 FSM_ACTION_ROUTINE(ncia_action_18)
#define NCIA_ACTION_19 FSM_ACTION_ROUTINE(ncia_action_19)
#define NCIA_ACTION_20 FSM_ACTION_ROUTINE(ncia_action_20)
#define NCIA_ACTION_21 FSM_ACTION_ROUTINE(ncia_action_21)
#define NCIA_ACTION_22 FSM_ACTION_ROUTINE(ncia_action_22)
#define NCIA_ACTION_23 FSM_ACTION_ROUTINE(ncia_action_23)
#define NCIA_ACTION_24 FSM_ACTION_ROUTINE(ncia_action_24)
#define NCIA_ACTION_25 FSM_ACTION_ROUTINE(ncia_action_25)
#define NCIA_ACTION_26 FSM_ACTION_ROUTINE(ncia_action_26)
#define NCIA_ACTION_27 FSM_ACTION_ROUTINE(ncia_action_27)

#define NCIA_ACTION_30 FSM_ACTION_ROUTINE(ncia_action_30)
#define NCIA_ACTION_31 FSM_ACTION_ROUTINE(ncia_action_31)
#define NCIA_ACTION_32 FSM_ACTION_ROUTINE(ncia_action_32)
#define NCIA_ACTION_33 FSM_ACTION_ROUTINE(ncia_action_33)



/* ncia_serevr_fsm.c */

extern void ncia_record_ckt_info(ndlc_msg_type_t ndlc, 
                                 ndlc_start_dl_t *start_dl, NciaCepT *ncia_ptr);
extern void ncia_print_ndlc_header(int ndlc, ndlc_packet_t *ndlc_packet, 
                                   char *dir);
extern void ncia_pre_server_fsm(nciapeertype *ncia_client, int event);
extern boolean ncia_flow_control_out(NciaCepT *ncia_ptr, 
                                     ndlc_packet_t *ndlc_packet, int ndlc);
extern char *ncia_print_circuit_state(nciast_t state);
extern char *ncia_print_action(int input);
extern char *ncia_print_ndlc(ndlc_msg_type_t ndlc);

/* ncia_server_fsm_tbl.c */

extern short ncia_server_fsm(NciaCepT *ncia_ptr, nciama_t major, int minor);

