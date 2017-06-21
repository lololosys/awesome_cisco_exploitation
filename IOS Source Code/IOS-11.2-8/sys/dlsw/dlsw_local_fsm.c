/* $Id: dlsw_local_fsm.c,v 3.8.20.3 1996/07/19 22:45:38 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_local_fsm.c,v $
 *------------------------------------------------------------------
 * DLSw Local Switching Finite State Machine
 *
 * August 1995, Frank Bordonaro
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_local_fsm.c,v $
 * Revision 3.8.20.3  1996/07/19  22:45:38  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.8.20.2  1996/05/17  10:45:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.6.3  1996/04/26  07:34:25  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.6.2  1996/04/11  14:37:16  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.6.1  1996/04/03  14:01:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.20.1  1996/03/18  19:32:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.2  1996/03/07  08:44:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  00:46:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/05  21:56:18  fbordona
 * CSCdi47942:  dlsw sdlc abm bit not turned off in 1st xid sent to
 *              sdlc station. Use abm default for media until xid3
 *              is received from a device.
 *
 * Revision 3.7  1996/01/31  02:14:16  snyder
 * CSCdi47864:  spellink errors
 *              non-activaton -> non-activation
 *
 * Revision 3.6  1995/12/31  18:17:01  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.5  1995/12/23  00:53:49  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.4  1995/12/15  17:18:28  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.3  1995/12/09  16:49:09  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.2  1995/11/17  09:03:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  18:48:58  fbordona
 * Placeholder for new file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "address.h"
#include "mgd_timers.h"
#include "ieee.h"
#include "../util/fsm.h"
#include "../if/rif.h"
#include "../ibm/sna_util.h"
#include "../cls/clsi_msg.h"
#include "../ui/debug.h"
#include "../ibm/libibm_externs.h"

#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_dlu.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_local_private.h"
#include "../dlsw/dlsw_core_externs.h"
/*
 * File Scope Variables
 */
static char print_buffer[30];

/*
 * File Scope Function Prototypes
 */
static void dl_send_seg_to_disp (void *, paktype *);
static void dl_restart_fsm(void *d, int old_state, int major, int minor);

/*
 * DLSw Local Switching FSM : State Disconnect
 */
const static struct fsm_state dlstate_disconnected[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DL_ACTION_a,  FSM_NOARG, dlst_ros_sent},	/* 00 dle_admin_start */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Request Open Station Sent
 */
const static struct fsm_state dlstate_ros_sent[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_y,  FSM_NOARG, dlst_disconnected},	/* 04 dle_admin_dlc_err */
{DL_ACTION_b,  FSM_NOARG, dlst_ckt_established},/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Circuit Established
 */
const static struct fsm_state dlstate_ckt_established[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{DL_ACTION_d,  FSM_NOARG, dlst_conn_req_sent},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_2,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{DL_ACTION_c,  FSM_NOARG, dlst_conn_ind_rcvd},	/* 07 dle_conn_ind */
{DL_ACTION_x,  FSM_NOARG, FSM_NOCHANGE},        /* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{DL_ACTION_q,  FSM_NOARG, dlst_close_pending},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_2,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Connect Request Sent
 */
const static struct fsm_state dlstate_conn_req_sent[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{DL_ACTION_e,  FSM_NOARG, dlst_connected},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{DL_ACTION_q,  FSM_NOARG, dlst_close_pending},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Connect Request Sent
 */
const static struct fsm_state dlstate_conn_ind_rcvd[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{DL_ACTION_f,  FSM_NOARG, dlst_connected},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{DL_ACTION_q,  FSM_NOARG, dlst_close_pending},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Connected
 */
const static struct fsm_state dlstate_connected[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_w,  FSM_NOARG, dlst_disc_req_sent},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{DL_ACTION_w,  FSM_NOARG, dlst_disc_req_sent},	/* 03 dle_admin_disc */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 07 dle_conn_ind */
{DL_ACTION_1,  FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{DL_ACTION_g,  FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{DL_ACTION_u,  FSM_NOARG, dlst_disc_ind_rcvd},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Disconnect Indication Received
 */
const static struct fsm_state dlstate_disc_ind_rcvd[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_s,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{DL_ACTION_s,  FSM_NOARG, dlst_close_pending},	/* 03 dle_admin_disc */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_q,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};


/*
 * DLSw Local Switching FSM : State Disconnect Request Sent
 */
const static struct fsm_state dlstate_disc_req_sent[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{DL_ACTION_z,  FSM_NOARG, dlst_close_pending},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_r,  FSM_NOARG, dlst_close_pending},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{DL_ACTION_t,  FSM_NOARG, dlst_close_pending},	/* 11 dle_disc_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dle_disc_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dle_close_stn_cnf */
{DL_ACTION_t,  FSM_NOARG, dlst_close_pending},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};

/*
 * DLSw Local Switching FSM : State Disconnect
 */
const static struct fsm_state dlstate_close_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dle_admin_start */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dle_admin_stop */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dle_admin_conn */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dle_admin_disc */
{DL_ACTION_y,  FSM_NOARG, dlst_disconnected},	/* 04 dle_admin_dlc_err */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dle_req_opn_stn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dle_conn_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dle_conn_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dle_id_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dle_data_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dle_flow_ind */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dle_disc_cnf */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dle_disc_ind */
{DL_ACTION_y,  FSM_NOARG, dlst_disconnected},	/* 13 dle_close_stn_cnf */
{DL_ACTION_y,  FSM_NOARG, dlst_disconnected},	/* 14 dle_timer_expired */
/* ---------------------------------------------------------------------- */
};

ushort dl_action_a (dl_cep_t *dc)
{
    union D_primitives dp;

    LDEBUG("\nDLSw: LFSM-A: Opening DLC station");
    mgd_timer_start(&dc->timer, DL_ROS_TIMEOUT);

    dp.type = D_REQ_OPNSTN_REQ;
    dp.disp_reqopenstn_req.uCepId = (uint) dc;
    ieee_copy(dc->mac, dp.disp_reqopenstn_req.remoteAddr);
    ieee_copy(dc->partner->mac, dp.disp_reqopenstn_req.localAddr);
    dp.disp_reqopenstn_req.lSap = dc->partner->sap;
    dp.disp_reqopenstn_req.rSap = dc->sap;
    dp.disp_reqopenstn_req.rifLength = RIF_LENGTH(dc->rif[0]);
    dp.disp_reqopenstn_req.rif = dc->rif;
    dp.disp_reqopenstn_req.options = EO_RIF_SPECIFIED;
    dp.disp_reqopenstn_req.p_sap_id = dc->port_id->p_sap_id;
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_b()
 *
 * Received a request open station confirm.
 */
ushort dl_action_b (dl_cep_t *dc)
{
    ushort cls_op;

    LDEBUG("\nDLSw: LFSM-B: DLC station opened");
    mgd_timer_start(&dc->timer, DL_XID_TIMEOUT);
    dc->p_cep_id = ((ReqOpnStnCnf_t *)dc->clsi_msg)->fCLSIHdr.fPCepId;

    /*
     * Free ReqOpnStnCnf_t clsi_msg for this cep.
     * Makes logic below more readable.
     */
    CLSIMsgDestroy(dc->clsi_msg);
    dc->clsi_msg = NULL;

    /*
     * Can't process any messages until both partner dlu ceps
     * have received ReqOpnStn.Cfm and have p_cep_id's.
     * The last partner's ReqOpnStn.Cfm to come back will
     * process any pending_clsi_msg's for either partner.
     *
     * Hold your nose and re-call the fsm.
     * The other alternatives are worse.
     */
    if (dc->partner->p_cep_id != 0) {
        if (dc->pending_clsi_msg) {
            LDEBUG("\nDLSw: processing saved clsi message");
            dc->clsi_msg = dc->pending_clsi_msg;
            dc->pending_clsi_msg = NULL;
            cls_op = CLSIMsgGetPrimitiveCode(dc->clsi_msg);
            dl_fsm(dc, dle_dlc, cls_op);
            if (dc->clsi_msg) {
                CLSIMsgDestroy(dc->clsi_msg);
                dc->clsi_msg = NULL;
            }
        } else if (dc->partner->pending_clsi_msg) {
            LDEBUG("\nDLSw: processing saved clsi message");
            dc->partner->clsi_msg = dc->partner->pending_clsi_msg;
            dc->partner->pending_clsi_msg = NULL;
            cls_op = CLSIMsgGetPrimitiveCode(dc->partner->clsi_msg);
            dl_fsm(dc->partner, dle_dlc, cls_op);
            if (dc->partner->clsi_msg) {
                CLSIMsgDestroy(dc->partner->clsi_msg);
                dc->partner->clsi_msg = NULL;
            }
        }
    }
    return(FSM_OK);
}


/*
 * dl_action_c()
 *
 * Received a connect indication.
 */
ushort dl_action_c (dl_cep_t *dc)
{
    LDEBUG("\nDLSw: LFSM-C: starting local partner");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    /*
     * Drive partner cep's fsm to send connect.req.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_conn);

    return(FSM_OK);
}


/*
 * dl_action_d()
 *
 * Send a connect.req.
 */
ushort dl_action_d (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-D: sending connect request to station"); 
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    dp.type = D_CONNECT_REQ;
    dp.disp_connect_req.pCepId = dc->p_cep_id;
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_e()
 *
 * Received a connect confirm.
 */
ushort dl_action_e (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-E: station accepted the connection");
    mgd_timer_stop(&dc->timer);
    dc->xid_state = dl_xid_reset;

    /*
     * Drive partner cep's fsm to send connect.rsp.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_conn);

    /*
     * Release the CEP from the BUSY state.
     */
    dp.type = D_FLOW_REQ;
    dp.disp_flow_req.pCepId = dc->p_cep_id;
    dp.disp_flow_req.action = CLS_ACTION_RESUME_FLOW;
    fsm_to_disp(&dp);

    return(FSM_OK);
}


/*
 * dl_action_f()
 *
 * Send a connect.rsp.
 */
ushort dl_action_f (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-F: accept incoming connection");
    mgd_timer_stop(&dc->timer);

    dp.type = D_CONNECT_RSP;
    dp.disp_connect_rsp.pCepId = dc->p_cep_id;
    fsm_to_disp(&dp);

    return(FSM_OK);
}


/*
 * dl_action_g()
 *
 * Send a Data.Req
 */
ushort dl_action_g (dl_cep_t *dc)
{
    DataInd_t *data_ind;
    ulong data_len;
    paktype *pak;

    LDEBUG("\nDLSw: LFSM-G: sending data");
    data_ind = (DataInd_t*) dc->clsi_msg;
    data_len = data_ind->fDataLength;
    if (data_len == 0) {
        LDEBUG("\nDLSw: L-ckt: received data.ind of length zero");
        return(FSM_OK);
    }

    pak = CLSIMsgGetPak(dc->clsi_msg);
    pak->datagramstart = data_ind->fData + data_ind->fReserveLength;
    pak->datagramsize = data_len;

    /*
     * If the current i-field size (payload_len) exceeds the max
     * receievable i-field of the destination lan station then
     * segment it.
     *
     * Note: do not have to consider the media header since the
     *       XID3 max i-field len does not include the media header.
     */
    if ((dlsw_sna_spoof_and_seg()) && (dc->partner->max_i_field_len) &&
        (data_len > dc->partner->max_i_field_len)) {
        queuetype q;

	if (sna_th_frag(pak, dc->partner->max_i_field_len, &q)) {
	    sendqueue(&q, (SendFragProc)dl_send_seg_to_disp, dc->partner);
        } else {
	    datagram_done(pak);    /* no buffer, drop packet to ground */
	    dlsw_delayed_event(dl_restart_fsm, 
			       dc, dc->state,
			       dle_admin, dle_admin_stop);
	}
    } else {
        /* disp consumes pak */
        dc->clsi_msg = NULL;
        dl_send_seg_to_disp(dc->partner, pak);
    }
    return(FSM_OK);
}


/*
 * dl_action_q()
 */
ushort dl_action_q (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-Q: acknowledge disconnect");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    dp.type = D_DISCONNECT_RSP;
    dp.disp_disconnect_rsp.pCepId = dc->p_cep_id;
    fsm_to_disp(&dp);

    /*
     * Drive partner cep's fsm to send closest.req.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_stop);

    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_r()
 *
 * Used to close down both circuits before they're connected.
 * Called for timeouts and other error conditions.
 */
ushort dl_action_r (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-R: closing dlc station");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    /*
     * Drive partner cep's fsm to send closest.req.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_stop);

    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_s()
 *
 * Time to send Disconnect Response
 */
ushort dl_action_s (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-S: acknowledge disconnect"); 
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    dp.type = D_DISCONNECT_RSP;
    dp.disp_disconnect_rsp.pCepId = dc->p_cep_id;
    fsm_to_disp(&dp);

    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_t()
 *
 * Received a disconnect confirm.
 */
ushort dl_action_t (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-T: disconnect confirmed");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    /*
     * Drive partner cep's fsm to send disconnect.rsp.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_disc);

    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_u()
 *
 * Received a disconnect indication.
 */
ushort dl_action_u (dl_cep_t *dc)
{
    LDEBUG("\nDLSw: LFSM-U: disconnecting partner station");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    /*
     * Drive partner cep's fsm to send disconnect.req.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_disc);
    return(FSM_OK);
}


static void dl_send_seg_to_disp (void *controlblock, paktype *pak)
{
    union D_primitives dp;
    dl_cep_t *dc = (dl_cep_t *) controlblock;

    dp.type = D_DATA_REQ;
    dp.disp_data_req.pCepId = dc->p_cep_id;
    dp.disp_data_req.theData = pak->datagramstart;
    dp.disp_data_req.dataLength = pak->datagramsize;
    dp.disp_data_req.pak = pak;
    fsm_to_disp(&dp); /* disp consumes pak */
}

/*
 * dl_action_w()
 *
 * Used to close down a circuit after it's connected.
 */
ushort dl_action_w (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-W: request station disconnect");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    dp.type = D_DISCONNECT_REQ;
    dp.disp_disconnect_req.pCepId = dc->p_cep_id;
    fsm_to_disp(&dp);
    return(FSM_OK);
}


/*
 * dl_action_x()
 *
 * XID processing
 */
ushort dl_action_x (dl_cep_t *dc)
{
    XID3type *xid;
    ulong xid_len; 
    ushort cls_op;
    IdStnInd_t *id_stn;
    IdInd_t *id_ind;
    IdCnf_t *id_cnf;
    union D_primitives dp;

    LDEBUG("\nDLSw: LFSM-X: forward XID to partner");

    if (!mgd_timer_running(&dc->timer))
        mgd_timer_start(&dc->timer, DL_XID_TIMEOUT);

    if (dc->partner->p_cep_id == 0) {
	LDEBUG("\nDLSw: partner cep not ready for xid");
	return(FSM_OK);
    }
    cls_op = CLSIMsgGetPrimitiveCode(dc->clsi_msg);
    if (cls_op == (CLS_ID_STN | CLS_INDICATION)) {
        id_stn = (IdStnInd_t*) dc->clsi_msg;
        xid_len = id_stn->fXIDLength;
        xid = (XID3type *) (((uchar *) id_stn) + id_stn->fXIDOffset);
        dc->xid_state = dl_xid_ind_rcvd;
    } else if (cls_op == (CLS_ID | CLS_INDICATION)) {
        id_ind = (IdInd_t *) dc->clsi_msg;
        xid_len = id_ind->fXIDLength;
        xid = (XID3type *) (((uchar *) id_ind) + id_ind->fXIDOffset);
        dc->xid_state = dl_xid_ind_rcvd;
    } else if (cls_op == (CLS_ID | CLS_CONFIRM)) {
        id_cnf = (IdCnf_t *) dc->clsi_msg;
        xid_len = id_cnf->fXIDLength;
        xid = (XID3type *) (((uchar *) id_cnf) + id_cnf->fXIDOffset);
        dc->xid_state = dl_xid_reset;
    } else {
        LDEBUG("\nDLSw: dl_action_x(): bogus cls op %x", cls_op);
        return(FSM_OK);
    }
        
    /*
     * Learn the max recievable i-field, max i-frame (DLC modulo)
     * without acknowledgement, and ABM capabilities so we can echo
     * them back to the lan station when we "pass through" XID3s
     * end to end.
     */
    if (xid_len >= sizeof(XID3type)) {
        dlsw_learn_xid_parms(xid, &dc->abm_state,
                             &dc->max_i_field_len, &dc->window_size);

        if (dc->partner->port_id->port_type == CLS_SDLC) {
            dlsw_spoof_xid_parms(xid, dc->partner->abm_state,
                                 DLSW_ABM_INCAPABLE,
                                 dc->partner->max_i_field_len,
                                 dc->partner->window_size);
        } else {
            dlsw_spoof_xid_parms(xid, dc->partner->abm_state,
                                 DLSW_ABM_CAPABLE,
                                 dc->partner->max_i_field_len,
                                 dc->partner->window_size);
        }
    }

    /*
     * Finally, forward the XID onto the target lan.
     */
    if ((dc->partner->xid_state == dl_xid_reset) ||
        (dc->partner->xid_state == dl_xid_req_sent)) {
        dp.type = D_ID_REQ;
        dc->partner->xid_state = dl_xid_req_sent;
    } else { /* (dc->partner->xid_state == dl_xid_ind_rcvd) */
        dp.type = D_ID_RSP;
        dc->partner->xid_state = dl_xid_reset;
    }
    dp.disp_id_req.pCepId = dc->partner->p_cep_id;
    dp.disp_id_req.xidData = (uchar *) xid;
    dp.disp_id_req.xidLength = xid_len;
    fsm_to_disp(&dp);

    return(FSM_OK);
}


/*
 * dl_action_y()
 *
 * Takes care of admin cleanup after receiving CloseStn.cfn
 */
ushort dl_action_y (dl_cep_t *dc)
{
    dc->id = 0;
    dc->p_cep_id = 0;
    mgd_timer_stop(&dc->timer);

    /*
     * If our partner cep is ready to go away, then move the
     * circuit to the dead queue. If partner is awaiting 
     * ReqOpnStn.cnf then we can't kill it or we will go out
     * of sync with CLSI (the cep will never be destroyed).
     */
    if ((dc->partner->id == 0) && (dc->partner->state != dlst_ros_sent)) {
        LDEBUG("\nDLSw: LFSM-Y: removing local switch entity");
        p_unqueue(&dl_hashQ[((dl_ckt_t *) dc->parent)->bucket],
                  dc->parent);
        p_enqueue(&dl_deadQ, dc->parent);
    } else {
        /*
         * To be safe, Drive partner cep's fsm to send closest.req.
         */
        LDEBUG("\nDLSw: LFSM-Y: driving partner to close circuit");
        dl_fsm(dc->partner, dle_admin, dle_admin_stop);
    }
    return(FSM_OK);
}


/*
 * dl_action_z()
 *
 * Used to close down a circuit before it's connected.
 */
ushort dl_action_z (dl_cep_t *dc)
{
    union D_primitives  dp;

    LDEBUG("\nDLSw: LFSM-Z: close dlc station request");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);

    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}




/*******************************************************************
 *
 *            FSM Administation Structures/Routines
 *
 ********************************************************************/
 
#define DL_FSM_DECODE FSM_DECODE_ROUTINE(dl_fsm_decode)

const static struct finite_state_machine dl_fsm_table[] =
{
/*  State                       Decoder        */
 {   dlstate_disconnected,     	DL_FSM_DECODE},
 {   dlstate_ros_sent,      	DL_FSM_DECODE},
 {   dlstate_ckt_established,	DL_FSM_DECODE},
 {   dlstate_conn_ind_rcvd,	DL_FSM_DECODE},
 {   dlstate_conn_req_sent,    	DL_FSM_DECODE},
 {   dlstate_connected,      	DL_FSM_DECODE},
 {   dlstate_disc_ind_rcvd,    	DL_FSM_DECODE},
 {   dlstate_disc_req_sent,    	DL_FSM_DECODE},
 {   dlstate_close_pending,    	DL_FSM_DECODE},
};


int dl_fsm_decode (dl_cep_t *dc, dle_major_t major, int minor)
{
    int index;

    switch (major) {

      case dle_admin:

        switch (minor) {
          case dle_admin_start:
            index = 0;
            break;

          case dle_admin_stop:
            index = 1;
            break;

          case dle_admin_conn:
            index = 2;
            break;

          case dle_admin_disc:
            index = 3;
            break;

          case dle_admin_dlc_err:
            index = 4;
            break;

          default:
            index = FSM_FAULT;
            break;
        }
        break;

      case dle_dlc:

        switch (minor) {
          case CLS_REQ_OPNSTN | CLS_CONFIRM:
	    index = 5;		 
	    break;

	  case CLS_CONNECT | CLS_CONFIRM:
          case CLS_CONNECTED | CLS_INDICATION:
	    index = 6;		 
	    break;

	  case CLS_CONNECT | CLS_INDICATION:
      	  case CLS_CONNECT_STN | CLS_INDICATION:
	    index = 7;		 
	    break;

	  case CLS_ID_STN | CLS_INDICATION:
	  case CLS_ID | CLS_CONFIRM:
	  case CLS_ID | CLS_INDICATION:
	    index = 8;		 
	    break;

	  case CLS_DATA | CLS_INDICATION:
	    index = 9;		 
	    break;

	  case CLS_FLOW | CLS_INDICATION:
	    index = 10;		 
	    break;

	  case CLS_DISCONNECT | CLS_CONFIRM:
	    index = 11;		 
	    break;

	  case CLS_DISCONNECT | CLS_INDICATION:
	    index = 12;		 
	    break;

	  case CLS_CLOSE_STN | CLS_CONFIRM:
	    index = 13;		 
	    break;

          default:
            index = FSM_FAULT;
            break;
        }
        break;

      case dle_timer:

        switch (minor) {
          case dle_timer_expired:
            index = 14;
            break;

          default:
            index = FSM_FAULT;
            break;

        }
	break;

      default:
        index = FSM_FAULT;
        break;
    }
    return (index);
}


/*
 * d l _ f s m
 */

ushort dl_fsm (dl_cep_t *dc, dle_major_t major, int minor)
{
    int         fsm_rc;
    int         old_state;

    old_state = dc->state;

    if (dc->debug) {
        dl_print_event(major, minor, print_buffer);
        buginf("\nDLSw: START-LFSM %s (%e->%e) event:%s",
               dc->port_id->swidb->hwptr->hw_namestring,
               dc->mac, dc->partner->mac, print_buffer);
    }

    fsm_rc = (int) fsm_execute ("DLSW-LOCAL", dl_fsm_table,
                                ((int *) &(dc->state)), dlst_fsm_max,
                                dc, major, minor, FALSE);
    if (dc->debug) {
        buginf("\nDLSw: END-LFSM (%e->%e): state:%s->%s\n",
               dc->mac, dc->partner->mac, dl_print_state(old_state),
               dl_print_state(dc->state));
    }

    if (fsm_rc < 0) {
      switch (fsm_rc) {
      case FSM_RC_ERROR_INPUT:
      case FSM_RC_INVALID_INPUT:
        LDEBUG("\nInvalid input"); break;
      case FSM_RC_UNKNOWN_STATE:
        LDEBUG("\nIllegal state"); break;
      case FSM_RC_UNKNOWN_INPUT:
        LDEBUG("\nIllegal input"); break;
      case FSM_RC_UNEXPECTED_INPUT:
        LDEBUG("\nUnexpected input"); break;
      default:
        LDEBUG("\nunknown error rtn code"); break;
      }
    }
    return (fsm_rc);
}

static void dl_restart_fsm (void *d, int old_state, int major, int minor)
{
    dl_cep_t	*dc;

    dc = d;
    dl_fsm (dc, major, minor);
}

/*
 * dl_action_1()
 *
 * XID processing in connected state - Check for non-activation XID
 */
ushort dl_action_1 (dl_cep_t *dc)
{
    XID3type *xid;
    ulong xid_len;
    ushort cls_op;
    IdInd_t *id_ind;
    IdCnf_t *id_cnf;
    union D_primitives dp;

    LDEBUG("\nDLSw: L-ckt: dl_action_1()");

    cls_op = CLSIMsgGetPrimitiveCode(dc->clsi_msg);
    if (cls_op == (CLS_ID | CLS_INDICATION)) {
        id_ind = (IdInd_t *) dc->clsi_msg;
        xid_len = id_ind->fXIDLength;
        xid = (XID3type *) (((uchar *) id_ind) + id_ind->fXIDOffset);
    } else if (cls_op == (CLS_ID | CLS_CONFIRM)) {
        id_cnf = (IdCnf_t *) dc->clsi_msg;
        xid_len = id_cnf->fXIDLength;
        xid = (XID3type *) (((uchar *) id_cnf) + id_cnf->fXIDOffset);
    } else {
        LDEBUG("\nDLSw: dl_action_1(): bogus cls op %x", cls_op);
        return(FSM_OK);
    }

    if (!xid_is_nonactivation((uchar *)xid)) {
      LDEBUG("\nDLSw: dl_action_1(): Not a non-activation XID");
      return(FSM_OK);
    }

    if (cls_op == (CLS_ID | CLS_INDICATION))
        dc->xid_state = dl_xid_ind_rcvd;
    else
        dc->xid_state = dl_xid_reset;

    /*
     * Finally, forward the XID onto the target lan.
     */
    if ((dc->partner->xid_state == dl_xid_reset) ||
        (dc->partner->xid_state == dl_xid_req_sent)) {
        dp.type = D_ID_REQ;
        dc->partner->xid_state = dl_xid_req_sent;
    } else { /* (dc->partner->xid_state == dl_xid_ind_rcvd) */
        dp.type = D_ID_RSP;
        dc->partner->xid_state = dl_xid_reset;
    }
    dp.disp_id_req.pCepId = dc->partner->p_cep_id;
    dp.disp_id_req.xidData = (uchar *) xid;
    dp.disp_id_req.xidLength = xid_len;
    fsm_to_disp(&dp);

    return(FSM_OK);
  }


/*
 * dl_action_2()
 *
 * Used to close down both circuits due to disconnects and timeouts
 * in the circuit established state.
 */
ushort dl_action_2 (dl_cep_t *dc)
{
    union D_primitives  dp;
    u_portid_type_t prp_id;
 
    LDEBUG("\nDLSw: LFSM-2: closing dlc station");
    mgd_timer_start(&dc->timer, DL_GENERAL_TIMEOUT);
 
    prp_id.port_id = dc->port_id;
    delete_xidfailure_mac_entry(dc->mac, &prp_id);

    prp_id.port_id = dc->partner->port_id;
    delete_xidfailure_mac_entry(dc->partner->mac, &prp_id);

    /*
     * Drive partner cep's fsm to send closest.req.
     */
    dl_fsm(dc->partner, dle_admin, dle_admin_stop);
 
    dp.type = D_CLOSESTN_REQ;
    if (dc->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dc->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = (uint) dc;
    }
    fsm_to_disp(&dp);
    return(FSM_OK);
}
 
 

