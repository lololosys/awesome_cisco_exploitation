/* $Id: ncia_server_fsm_tbl.c,v 3.1.2.6 1996/07/19 22:45:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_server_fsm_tbl.c,v $
 *------------------------------------------------------------------
 * ncia_server_fsm_tbl.c - NCIA Server FSM Table
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_server_fsm_tbl.c,v $
 * Revision 3.1.2.6  1996/07/19  22:45:23  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/26  19:53:08  schiang
 * CSCdi61191:  incorrectly declare functions as inlines in
 * ncia_server_fsm.h
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/19  05:21:17  schiang
 * CSCdi60757:  HALT_DL_NO_ACK is not sent in some condition
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/18  19:01:52  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/12  06:51:56  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.4  1996/05/14  18:42:32  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/03  22:06:30  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/26  15:16:45  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:48  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:20:48  schiang
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

#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "../cls/ncia_debug.h"
#include "../cls/ncia_debug_flags.h"



/*
 * NCIA SERVER FSM 1 : State CLOSED.
 * The SAP has been activated.  No CEP and no Data Link Session.
 * We are waiting for either Req_OpnStn.Req from DLU or START_DL from Client.
 */
const static struct fsm_state ds_closed[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 00 admin_stop          */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 01 admin_error         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 02 admin_tcp_down      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 03 admin_tcp_busy      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 04 admin_tcp_unbusy    */

{NCIA_ACTION_9,  FSM_NOARG,  nciast_opening},       /* 05 cls_req_opn_stn_req */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 06 cls_close_stn_req   */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 07 cls_conn_req        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 08 cls_conn_rsp        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 09 cls_disc_req        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 10 cls_disc_rsp        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 11 cls_data_req        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 12 cls_id_req          */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 13 cls_id_rsp          */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 14 cls_test_req        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 15 cls_udata_req       */
 
{NCIA_ACTION_33, &NCIA_NISI, FSM_NOCHANGE},         /* 16 cls_id_stn_req      */
{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},         /* 17 cls_test_stn_req    */
{NCIA_ACTION_3,  FSM_NOARG,  FSM_NOCHANGE},         /* 18 cls_test_stn_rsp    */
{NCIA_ACTION_4,  FSM_NOARG,  FSM_NOCHANGE},         /* 19 cls_udata_stn_req   */
 
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 20 cls_flow_stop       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 21 cls_flow_start      */

{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},         /* 22 wan_canureach       */
{NCIA_ACTION_33, &NCIA_USI,  FSM_NOCHANGE},         /* 23 wan_dataframe       */
{NCIA_ACTION_32, &NCIA_TSI,  nciast_start_dl_rcvd}, /* 24 wan_start_dl        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 25 wan_dl_started      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 26 wan_start_dl_failed */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 27 wan_dgrmframe       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 28 wan_xidframe        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 29 wan_contact_stn     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 30 wan_stn_contacted   */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 31 wan_infoframe       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 32 wan_halt_dl         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 33 wan_halt_dl_noack   */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 34 wan_dl_halted       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 35 wan_imp             */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},         /* 36 timer_expired       */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 2 : State OPENING
 * Connect Out case.  Req_OpnStn.Req is received from DLU.  START_DL is sent to
 * clinet.  No CEP and no Data Link Session.
 */
const static struct fsm_state ds_opening[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_ROSCF, nciast_closed},      /* 00 admin_stop           */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 01 admin_error          */
{NCIA_ACTION_33, &NCIA_ROSCF, nciast_closed},      /* 02 admin_tcp_down       */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 03 admin_tcp_busy       */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 04 admin_tcp_unbusy     */

{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 05 cls_req_opn_stn_req  */
{NCIA_ACTION_14, FSM_NOARG,   nciast_closed},      /* 06 cls_close_stn_req    */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 07 cls_conn_req         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 08 cls_conn_rsp         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 09 cls_disc_req         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 10 cls_disc_rsp         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 11 cls_data_req         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 12 cls_id_req           */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 13 cls_id_rsp           */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 14 cls_test_req         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 15 cls_udata_req        */

{NCIA_ACTION_33, &NCIA_NISI,  FSM_NOCHANGE},       /* 16 cls_id_stn_req       */
{NCIA_ACTION_33, &NCIA_TSI,   FSM_NOCHANGE},       /* 17 cls_test_stn_req     */
{NCIA_ACTION_3,  FSM_NOARG,   FSM_NOCHANGE},       /* 18 cls_test_stn_rsp     */
{NCIA_ACTION_4,  FSM_NOARG,   FSM_NOCHANGE},       /* 19 cls_udata_stn_req    */

{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 20 cls_flow_stop        */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 21 cls_flow_start       */

{NCIA_ACTION_3,  FSM_NOARG,   FSM_NOCHANGE},       /* 22 wan_canureach        */
{NCIA_ACTION_33, &NCIA_USI,   FSM_NOCHANGE},       /* 23 wan_dataframe        */
{NCIA_ACTION_16, FSM_NOARG,   nciast_opened},      /* 24 wan_start_dl         */
{NCIA_ACTION_33, &NCIA_ROSC,  nciast_opened},      /* 25 wan_dl_started       */
{NCIA_ACTION_33, &NCIA_ROSCF, nciast_closed},      /* 26 wan_start_dl_failed  */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 27 wan_dgrmframe        */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 28 wan_xidframe         */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 29 wan_contact_stn      */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 30 wan_stn_contacted    */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 31 wan_infoframe        */
{NCIA_ACTION_31, &NCIA_ROSCF, nciast_closed},      /* 32 wan_halt_dl          */
{NCIA_ACTION_33, &NCIA_ROSCF, nciast_closed},      /* 33 wan_halt_dl_noack    */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 34 wan_dl_halted        */
{FSM_NOACTION,   FSM_NOARG,   FSM_NOCHANGE},       /* 35 wan_imp              */

{NCIA_ACTION_33, &NCIA_ROSCF, nciast_closed},      /* 36 timer_expired        */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 3 : State OPENED
 * Received START_DL or DL_STARTED from client.  Req_OpnStn.Cnf is sent to DLU.
 * We have CEP and Data Link Session now.
 * 
 */
const static struct fsm_state ds_opened[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,  nciast_opened_nodl},   /* 00 admin_stop           */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 01 admin_error          */
{FSM_NOACTION,   FSM_NOARG, nciast_opened_nodl},   /* 02 admin_tcp_down       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 03 admin_tcp_busy       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 04 admin_tcp_unbusy     */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 05 cls_req_opn_stn_req  */
{NCIA_ACTION_27, FSM_NOARG, nciast_closed},        /* 06 cls_close_stn_req    */
{NCIA_ACTION_6,  FSM_NOARG, nciast_conn_pending},  /* 07 cls_conn_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 08 cls_conn_rsp         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 09 cls_disc_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 10 cls_disc_rsp         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 11 cls_data_req         */
{NCIA_ACTION_11, FSM_NOARG, FSM_NOCHANGE},         /* 12 cls_id_req           */
{NCIA_ACTION_11, FSM_NOARG, FSM_NOCHANGE},         /* 13 cls_id_rsp           */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},         /* 14 cls_test_req         */
{NCIA_ACTION_7,  FSM_NOARG, FSM_NOCHANGE},         /* 15 cls_udata_req        */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 16 cls_id_stn_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 17 cls_test_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 18 cls_test_stn_rsp     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 19 cls_udata_stn_req    */
  
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 20 cls_flow_stop        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 21 cls_flow_start       */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},         /* 22 wan_canureach        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 23 wan_dataframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 24 wan_start_dl         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 25 wan_dl_started       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 26 wan_start_dl_failed  */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},         /* 27 wan_dgrmframe        */
{NCIA_ACTION_33, &NCIA_XID, FSM_NOCHANGE},         /* 28 wan_xidframe         */
{NCIA_ACTION_32, &NCIA_CI,  nciast_connect_wait},  /* 29 wan_contact_stn      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 30 wan_stn_contacted    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 31 wan_infoframe        */
{NCIA_ACTION_31, &NCIA_NI,  nciast_opened_nodl},   /* 32 wan_halt_dl          */
{FSM_NOACTION,   FSM_NOARG, nciast_opened_nodl},   /* 33 wan_halt_dl_noack    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 34 wan_dl_halted        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 35 wan_imp              */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 36 timer_expired        */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 4 : State CONNECT_PENDING
 * Received Connect.Req in OPENED state.  CONTACT_STN is sent to client.
 * We have CEP and Data Link Session.
 */
const static struct fsm_state ds_connect_pending[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,  nciast_opened_nodl},   /* 00 admin_stop           */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 01 admin_error          */
{NCIA_ACTION_33, &NCIA_NI,  nciast_opened_nodl},   /* 02 admin_tcp_down       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 03 admin_tcp_busy       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 04 admin_tcp_unbusy     */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 05 cls_req_opn_stn_req  */
{NCIA_ACTION_27, FSM_NOARG, nciast_closed},        /* 06 cls_close_stn_req    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 07 cls_conn_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 08 cls_conn_rsp         */
{NCIA_ACTION_2,  FSM_NOARG, nciast_disc_pending},  /* 09 cls_disc_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 10 cls_disc_rsp         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 11 cls_data_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 12 cls_id_req           */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 13 cls_id_rsp           */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},         /* 14 cls_test_req         */
{NCIA_ACTION_7,  FSM_NOARG, FSM_NOCHANGE},         /* 15 cls_udata_req        */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 16 cls_id_stn_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 17 cls_test_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 18 cls_test_stn_rsp     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 19 cls_udata_stn_req    */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 20 cls_flow_stop        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 21 cls_flow_start       */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},         /* 22 wan_canureach        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 23 wan_dataframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 24 wan_start_dl         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 25 wan_dl_started       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 26 wan_start_dl_failed  */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},         /* 27 wan_dgrmframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 28 wan_xidframe         */
{NCIA_ACTION_15, FSM_NOARG, nciast_opened_nodl},   /* 29 wan_contact_stn      */
{NCIA_ACTION_33, &NCIA_CC,  nciast_connected},     /* 30 wan_stn_contacted    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 31 wan_infoframe        */
{NCIA_ACTION_31, &NCIA_CCF, nciast_opened_nodl},   /* 32 wan_halt_dl          */
{NCIA_ACTION_33, &NCIA_CCF, nciast_opened_nodl},   /* 33 wan_halt_dl_noack    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 34 wan_dl_halted        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 35 wan_imp              */

{NCIA_ACTION_33, &NCIA_CCF, nciast_opened},        /* 36 timer_expired        */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 5 : State CONNECTED
 * End to End session is established.  I frame can be sent in this state only.
 * We have both CEP and Data Link Session.
 */
const static struct fsm_state ds_connected[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_DI,   nciast_disc_wait_noack},/* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_DI,   nciast_disc_wait_noack},/* 02 admin_tcp_down     */
{NCIA_ACTION_22, FSM_NOARG,  FSM_NOCHANGE},          /* 03 admin_tcp_busy     */
{NCIA_ACTION_23, FSM_NOARG,  FSM_NOCHANGE},          /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_27, FSM_NOARG,  nciast_closed},         /* 06 cls_close_stn_req  */
{NCIA_ACTION_2,  FSM_NOARG,  nciast_disc_pending},   /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 08 cls_conn_rsp       */
{NCIA_ACTION_2,  FSM_NOARG,  nciast_disc_pending},   /* 09 cls_disc_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 10 cls_disc_rsp       */
{NCIA_ACTION_8,  FSM_NOARG,  FSM_NOCHANGE},          /* 11 cls_data_req       */
{NCIA_ACTION_11, FSM_NOARG,  FSM_NOCHANGE},          /* 12 cls_id_req         */
{NCIA_ACTION_11, FSM_NOARG,  FSM_NOCHANGE},          /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,   FSM_NOCHANGE},          /* 14 cls_test_req       */
{NCIA_ACTION_7,  FSM_NOARG,  FSM_NOCHANGE},          /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 19 cls_udata_stn_req  */
 
{NCIA_ACTION_24, FSM_NOARG,  FSM_NOCHANGE},          /* 20 cls_flow_stop      */
{NCIA_ACTION_25, FSM_NOARG,  FSM_NOCHANGE},          /* 21 cls_flow_start     */
 
{NCIA_ACTION_3,  FSM_NOARG,  FSM_NOCHANGE},          /* 22 wan_canureach      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 26 wan_start_dl_failed*/
{NCIA_ACTION_33, &NCIA_UI,   FSM_NOCHANGE},          /* 27 wan_dgrmframe      */
{NCIA_ACTION_33, &NCIA_XID,  FSM_NOCHANGE},          /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 30 wan_stn_contacted  */
{NCIA_ACTION_33, &NCIA_DATA, FSM_NOCHANGE},          /* 31 wan_infoframe      */
{NCIA_ACTION_32, &NCIA_DI,   nciast_disc_wait},      /* 32 wan_halt_dl        */
{NCIA_ACTION_32, &NCIA_DI,   nciast_disc_wait_noack},/* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 35 wan_imp            */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 6 : State DISCONNECT_PENDING
 * Received Disconnect.Req in CONNECTED state.  HALT_DL is sent to client.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_disc_pending[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_14, FSM_NOARG,     nciast_closed},      /* 06 cls_close_stn_req  */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 08 cls_conn_rsp       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 09 cls_disc_req       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 11 cls_data_req       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,      FSM_NOCHANGE},       /* 14 cls_test_req       */
{NCIA_ACTION_4,  FSM_NOARG,     FSM_NOCHANGE},       /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 21 cls_flow_start     */

{NCIA_ACTION_3,  FSM_NOARG,     FSM_NOCHANGE},       /* 22 wan_canureach      */
{NCIA_ACTION_33, &NCIA_UI,      FSM_NOCHANGE},       /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 26 wan_start_dl_failed*/
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 31 wan_infoframe      */
{NCIA_ACTION_31, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 32 wan_halt_dl        */
{NCIA_ACTION_33, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 33 wan_halt_dl_noack  */
{NCIA_ACTION_33, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 35 wan_imp            */

{NCIA_ACTION_33, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 7 : State OPENED_BUT_NO_DATA_LINK
 * Received DL_HALTED or HALT_DL in DISCONNECT PENDING state.
 * Disconnect.Cnf or Connect.Cnf (Fail) is sent to DLU.
 * This should be a transient state because DLU will sent CloseStn.Req now.
 * If we get in not from the DISCONNECT PENDING state then it's not a transient
 * state.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_opened_nodl[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,  FSM_NOCHANGE},           /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 01 admin_error        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_14, FSM_NOARG, nciast_closed},          /* 06 cls_close_stn_req  */
{NCIA_ACTION_9,  FSM_NOARG, nciast_dls_wait_open},   /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 08 cls_conn_rsp       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 09 cls_disc_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 11 cls_data_req       */
{NCIA_ACTION_21, FSM_NOARG, nciast_dl_started_wait}, /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},           /* 14 cls_test_req       */
{NCIA_ACTION_4,  FSM_NOARG, FSM_NOCHANGE},           /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 21 cls_flow_start     */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},           /* 22 wan_canureach      */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},           /* 23 wan_dataframe      */
{NCIA_ACTION_17, FSM_NOARG, nciast_opened},          /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 26 wan_start_dl_failed*/
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 31 wan_infoframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 32 wan_halt_dl        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 35 wan_imp            */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 8 : State DATA_LINK_STARTED_WAIT
 * Comes from OPENED_NO_DL or OPENING_HALT state.
 * Id.Req is received from DLU.  START_DL is sent to client.
 * We have CEP but no Data Link Session
 */
const static struct fsm_state ds_dl_started_wait[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_FI,  nciast_opened_nodl},   /* 00 admin_stop           */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 01 admin_error          */
{NCIA_ACTION_33, &NCIA_FI,  nciast_opened_nodl},   /* 02 admin_tcp_down       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 03 admin_tcp_busy       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 04 admin_tcp_unbusy     */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 05 cls_req_opn_stn_req  */
{NCIA_ACTION_14, FSM_NOARG, nciast_closed},        /* 06 cls_close_stn_req    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 07 cls_conn_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 08 cls_conn_rsp         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 09 cls_disc_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 10 cls_disc_rsp         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 11 cls_data_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 12 cls_id_req           */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 13 cls_id_rsp           */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},         /* 14 cls_test_req         */
{NCIA_ACTION_4,  FSM_NOARG, FSM_NOCHANGE},         /* 15 cls_udata_req        */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 16 cls_id_stn_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 17 cls_test_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 18 cls_test_stn_rsp     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 19 cls_udata_stn_req    */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 20 cls_flow_stop        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 21 cls_flow_start       */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},         /* 22 wan_canureach        */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},         /* 23 wan_dataframe        */
{NCIA_ACTION_10, FSM_NOARG, nciast_opened},        /* 24 wan_start_dl         */
{NCIA_ACTION_11, FSM_NOARG, nciast_opened},        /* 25 wan_dl_started       */
{NCIA_ACTION_33, &NCIA_FI,  nciast_opened_nodl},   /* 26 wan_start_dl_failed  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 27 wan_dgrmframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 28 wan_xidframe         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 29 wan_contact_stn      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 30 wan_stn_contacted    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 31 wan_infoframe        */
{NCIA_ACTION_31, &NCIA_NI,  nciast_opened_nodl},   /* 32 wan_halt_dl          */
{NCIA_ACTION_33, &NCIA_NI,  nciast_opened_nodl},   /* 33 wan_halt_dl_noack    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 34 wan_dl_halted        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},         /* 35 wan_imp              */
 
{NCIA_ACTION_33, &NCIA_FI,  nciast_opened_nodl},   /* 36 timer_expired        */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 9 : State START_DL_RECEIVED
 * START_DL is received in CLOSED State.  TestStn.Ind is sent to DLU.
 * No CEP and Data Link Session.
 */
const static struct fsm_state ds_start_dl_rcvd[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,   nciast_closed},         /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_NI,   nciast_closed},         /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 04 admin_tcp_unbusy   */

{NCIA_ACTION_16, FSM_NOARG,  nciast_opened},         /* 05 cls_req_opn_stn_req*/
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 06 cls_close_stn_req  */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 08 cls_conn_rsp       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 09 cls_disc_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 11 cls_data_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 13 cls_id_rsp         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 14 cls_test_req       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 15 cls_udata_req      */

{NCIA_ACTION_33, &NCIA_NISI, FSM_NOCHANGE},          /* 16 cls_id_stn_req     */
{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},          /* 17 cls_test_stn_req   */
{NCIA_ACTION_17, FSM_NOARG,  nciast_dl_started_snd}, /* 18 cls_test_stn_rsp   */
{NCIA_ACTION_4,  FSM_NOARG,  FSM_NOCHANGE},          /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 21 cls_flow_start     */

{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},          /* 22 wan_canureach      */
{NCIA_ACTION_33, &NCIA_USI,  FSM_NOCHANGE},          /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 26 wan_start_dl_failed*/
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 31 wan_infoframe      */
{NCIA_ACTION_31, &NCIA_NI,   nciast_closed},         /* 32 wan_halt_dl        */
{NCIA_ACTION_33, &NCIA_NI,   nciast_closed},         /* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},          /* 35 wan_imp            */

{NCIA_ACTION_19, FSM_NOARG,  nciast_closed},         /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 10 : State DL_STARTED_SND
 * TestStn.Rsp is received in START_DL_RECEIVED state or
 * HALT_DL(_NOACK) is received in OPEN_WAIT state.
 * DL_STARTED is sent.  * No CEP but have Data Link Session.
 */
const static struct fsm_state ds_dl_started_snd[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, FSM_NOARG,  nciast_closed},       /* 00 admin_stop           */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 01 admin_error          */
{FSM_NOACTION,   FSM_NOARG,  nciast_closed},       /* 02 admin_tcp_down       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 03 admin_tcp_busy       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 04 admin_tcp_unbusy     */

{NCIA_ACTION_33, &NCIA_ROSC, nciast_opened},       /* 05 cls_req_opn_stn_req  */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 06 cls_close_stn_req    */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 07 cls_conn_req         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 08 cls_conn_rsp         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 09 cls_disc_req         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 10 cls_disc_rsp         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 11 cls_data_req         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 12 cls_id_req           */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 13 cls_id_rsp           */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 14 cls_test_req         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 15 cls_udata_req        */

{NCIA_ACTION_33, &NCIA_NISI, FSM_NOCHANGE},        /* 16 cls_id_stn_req       */
{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},        /* 17 cls_test_stn_req     */
{NCIA_ACTION_3,  FSM_NOARG,  FSM_NOCHANGE},        /* 18 cls_test_stn_rsp     */
{NCIA_ACTION_7,  FSM_NOARG,  FSM_NOCHANGE},        /* 19 cls_udata_stn_req    */

{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 20 cls_flow_stop        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 21 cls_flow_start       */

{NCIA_ACTION_33, &NCIA_TSI,  FSM_NOCHANGE},        /* 22 wan_canureach        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 23 wan_dataframe        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 24 wan_start_dl         */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 25 wan_dl_started       */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 26 wan_start_dl_failed  */
{NCIA_ACTION_33, &NCIA_USI,  FSM_NOCHANGE},        /* 27 wan_dgrmframe        */
#ifdef XID_RETRY
{NCIA_ACTION_26, FSM_NOARG,  FSM_NOCHANGE},        /* 28 wan_xidframe         */
#else
{NCIA_ACTION_33, &NCIA_ISI,  FSM_NOCHANGE},        /* 28 wan_xidframe         */
#endif
{NCIA_ACTION_32, &NCIA_CSI,  nciast_open_wait},    /* 29 wan_contact_stn      */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 30 wan_stn_contacted    */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 31 wan_infoframe        */
{NCIA_ACTION_31, &NCIA_NI,   nciast_closed},       /* 32 wan_halt_dl          */
{FSM_NOACTION,   FSM_NOARG,  nciast_closed},       /* 33 wan_halt_dl_noack    */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 34 wan_dl_halted        */
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 35 wan_imp              */

#ifdef XID_RETRY
{NCIA_ACTION_33, &NCIA_ISI,  FSM_NOCHANGE},        /* 36 timer_expired        */
#else
{FSM_NOACTION,   FSM_NOARG,  FSM_NOCHANGE},        /* 36 timer_expired        */
#endif
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 11 : State DL_STARTED_AND_WAIT_OPEN
 * Connect.Req is received in OPENED_BUT_NO_DATA_LINK and OPENING_HALT state.
 * START_DL is sent and we are waiting for DL_STARTED.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_dls_wait_open[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_CCF, nciast_opened_nodl},    /* 00 admin_stop          */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 01 admin_error         */
{NCIA_ACTION_33, &NCIA_CCF, nciast_opened_nodl},    /* 02 admin_tcp_down      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 03 admin_tcp_busy      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 04 admin_tcp_unbusy    */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 05 cls_req_opn_stn_req */
{NCIA_ACTION_14, FSM_NOARG, nciast_closed},         /* 06 cls_close_stn_req   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 07 cls_conn_req        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 08 cls_conn_rsp        */
{NCIA_ACTION_32, &NCIA_NI,  nciast_opening_halt},   /* 09 cls_disc_req        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 10 cls_disc_rsp        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 11 cls_data_req        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 12 cls_id_req          */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 13 cls_id_rsp          */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},          /* 14 cls_test_req        */
{NCIA_ACTION_4,  FSM_NOARG, FSM_NOCHANGE},          /* 15 cls_udata_req       */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 16 cls_id_stn_req      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 17 cls_test_stn_req    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 18 cls_test_stn_rsp    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 19 cls_udata_stn_req   */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 20 cls_flow_stop       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 21 cls_flow_start      */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},          /* 22 wan_canureach       */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},          /* 23 wan_dataframe       */
{NCIA_ACTION_5,  FSM_NOARG, nciast_conn_pending},   /* 24 wan_start_dl        */
{NCIA_ACTION_6,  FSM_NOARG, nciast_conn_pending},   /* 25 wan_dl_started      */
{NCIA_ACTION_33, &NCIA_CCF, nciast_opened_nodl},    /* 26 wan_start_dl_failed */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 27 wan_dgrmframe       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 28 wan_xidframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 29 wan_contact_stn     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 30 wan_stn_contacted   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 31 wan_infoframe       */
{NCIA_ACTION_31, &NCIA_CCF, nciast_opened_nodl},    /* 32 wan_halt_dl         */
{NCIA_ACTION_33, &NCIA_CCF, nciast_opened_nodl},    /* 33 wan_halt_dl_noack   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 34 wan_dl_halted       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 35 wan_imp             */

{NCIA_ACTION_33, &NCIA_CCF, nciast_opened_nodl},    /* 36 timer_expired       */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 12 : State CONNECT_WAIT
 * Req_OpnStn.Req is received in OPEN_WAIT state and Req_OpnStn.Cnf and
 * Connect.Ind is sent to DLU or
 * CONTACT_STN is received in OPENED state and Connect.Ind is sent to DLU.
 * We are waiting for Connect.Rsp.
 * We have both CEP and Data Link Session.
 */
const static struct fsm_state ds_connect_wait[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_DI,  nciast_disc_wait_noack}, /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_DI,  nciast_disc_wait_noack}, /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_27, FSM_NOARG, nciast_closed},          /* 06 cls_close_stn_req  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 07 cls_conn_req       */
{NCIA_ACTION_1,  &NCIA_CEDI,nciast_connected},       /* 08 cls_conn_rsp       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 09 cls_disc_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 11 cls_data_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},           /* 14 cls_test_req       */
{NCIA_ACTION_7,  FSM_NOARG, FSM_NOCHANGE},           /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 21 cls_flow_start     */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},           /* 22 wan_canureach      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 26 wan_start_dl_failed*/
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},           /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 31 wan_infoframe      */
{NCIA_ACTION_32, &NCIA_DI,  nciast_disc_wait},       /* 32 wan_halt_dl        */
{NCIA_ACTION_32, &NCIA_DI,  nciast_disc_wait_noack}, /* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 35 wan_imp            */

{NCIA_ACTION_20, FSM_NOARG, nciast_opened_nodl},     /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 13 : State OPEN_WAIT
 * CONTACT_STN is received in DL_STARTED_SND state and ConnectStn.Ind is sent
 * to DLU.  No CEP but have Data Link Session.
 */
const static struct fsm_state ds_open_wait[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,      nciast_closed},       /* 00 admin_stop        */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 01 admin_error       */
{FSM_NOACTION,   FSM_NOARG,     nciast_closed},       /* 02 admin_tcp_down    */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 03 admin_tcp_busy    */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 04 admin_tcp_unbusy  */

{NCIA_ACTION_32, &NCIA_ROSC_CI, nciast_connect_wait}, /* 05cls_req_opn_stn_req*/
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 06 cls_close_stn_req */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 07 cls_conn_req      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 08 cls_conn_rsp      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 09 cls_disc_req      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 10 cls_disc_rsp      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 11 cls_data_req      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 12 cls_id_req        */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 13 cls_id_rsp        */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 14 cls_test_req      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 15 cls_udata_req     */

{NCIA_ACTION_33, &NCIA_NISI,    FSM_NOCHANGE},        /* 16 cls_id_stn_req    */
{NCIA_ACTION_33, &NCIA_TSI,     FSM_NOCHANGE},        /* 17 cls_test_stn_req  */
{NCIA_ACTION_3,  FSM_NOARG,     FSM_NOCHANGE},        /* 18 cls_test_stn_rsp  */
{NCIA_ACTION_7,  FSM_NOARG,     FSM_NOCHANGE},        /* 19 cls_udata_stn_req */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 20 cls_flow_stop     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 21 cls_flow_start    */

{NCIA_ACTION_3,  FSM_NOARG,     FSM_NOCHANGE},        /* 22 wan_canureach     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 23 wan_dataframe     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 24 wan_start_dl      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 25 wan_dl_started    */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 26wan_start_dl_failed*/
{NCIA_ACTION_33, &NCIA_USI,     FSM_NOCHANGE},        /* 27 wan_dgrmframe     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 28 wan_xidframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 29 wan_contact_stn   */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 30 wan_stn_contacted */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 31 wan_infoframe     */
{NCIA_ACTION_31, &NCIA_NI,      nciast_closed},       /* 32 wan_halt_dl       */
{NCIA_ACTION_33, &NCIA_NI,      nciast_closed},       /* 33 wan_halt_dl_noack */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 34 wan_dl_halted     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},        /* 35 wan_imp           */

{NCIA_ACTION_20, FSM_NOARG,     nciast_closed},       /* 36 timer_expired     */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 14 : State DISCONNECT_WAIT
 * HALT_DL is received in CONNECTED or CONNECT_WAIT state and Disconnect.Ind
 * is sent.  We are waiting for Disconnect.Rsp.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_disc_wait[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_33, &NCIA_NI,  nciast_opened_nodl},     /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_NI,  nciast_opened_nodl},     /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_31, &NCIA_CSC, nciast_closed},          /* 06 cls_close_stn_req  */
{NCIA_ACTION_31, &NCIA_CCF, nciast_opened_nodl},     /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 08 cls_conn_rsp       */
{NCIA_ACTION_31, &NCIA_DC,  nciast_opened_nodl},     /* 09 cls_disc_req       */
{NCIA_ACTION_31, &NCIA_NI,  nciast_opened_nodl},     /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 11 cls_data_req       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},           /* 14 cls_test_req       */
{NCIA_ACTION_4,  FSM_NOARG, FSM_NOCHANGE},           /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 21 cls_flow_start     */

{NCIA_ACTION_3,  FSM_NOARG, FSM_NOCHANGE},           /* 22 wan_canureach      */
{NCIA_ACTION_33, &NCIA_USI, FSM_NOCHANGE},           /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 26 wan_start_dl_failed*/
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 31 wan_infoframe      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 32 wan_halt_dl        */
{NCIA_ACTION_33, &NCIA_NI,  nciast_disc_wait_noack}, /* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},           /* 35 wan_imp            */

{FSM_NOACTION,   FSM_NOARG, nciast_opened_nodl},     /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 15 : State DISCONNECT_WAIT_NO_ACK
 * HALT_DL_NOACK is received in CONNECTED, CONNECT_WAIT, or DISCONNECT_WAIT
 * state and Disconnect.Ind is sent.  We are waiting for Disconnect.Rsp.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_disc_wait_noack[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_NI,      nciast_opened_nodl}, /* 00 admin_stop         */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 01 admin_error        */
{NCIA_ACTION_33, &NCIA_NI,      nciast_opened_nodl}, /* 02 admin_tcp_down     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 03 admin_tcp_busy     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 04 admin_tcp_unbusy   */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 05 cls_req_opn_stn_req*/
{NCIA_ACTION_33, &NCIA_CSC,     nciast_closed},      /* 06 cls_close_stn_req  */
{NCIA_ACTION_33, &NCIA_CCF,     nciast_opened_nodl}, /* 07 cls_conn_req       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 08 cls_conn_rsp       */
{NCIA_ACTION_33, &NCIA_DC_CCF,  nciast_opened_nodl}, /* 09 cls_disc_req       */
{NCIA_ACTION_33, &NCIA_NI,      nciast_opened_nodl}, /* 10 cls_disc_rsp       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 11 cls_data_req       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 12 cls_id_req         */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 13 cls_id_rsp         */
{NCIA_ACTION_33, &NCIA_TI,      FSM_NOCHANGE},       /* 14 cls_test_req       */
{NCIA_ACTION_4,  FSM_NOARG,     FSM_NOCHANGE},       /* 15 cls_udata_req      */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 16 cls_id_stn_req     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 17 cls_test_stn_req   */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 18 cls_test_stn_rsp   */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 19 cls_udata_stn_req  */

{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 20 cls_flow_stop      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 21 cls_flow_start     */

{NCIA_ACTION_3,  FSM_NOARG,     FSM_NOCHANGE},       /* 22 wan_canureach      */
{NCIA_ACTION_33, &NCIA_USI,     FSM_NOCHANGE},       /* 23 wan_dataframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 24 wan_start_dl       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 25 wan_dl_started     */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 26 wan_start_dl_failed*/
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 27 wan_dgrmframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 28 wan_xidframe       */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 29 wan_contact_stn    */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 30 wan_stn_contacted  */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 31 wan_infoframe      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 32 wan_halt_dl        */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 33 wan_halt_dl_noack  */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 34 wan_dl_halted      */
{FSM_NOACTION,   FSM_NOARG,     FSM_NOCHANGE},       /* 35 wan_imp            */

{FSM_NOACTION,   FSM_NOARG,     nciast_opened_nodl}, /* 36 timer_expired      */
/* -------------------------------------------------------------------------- */
};




/*
 * NCIA SERVER FSM 16 : State OPENING_HALT
 * Disconnect.Req is received in DL_STARTED_AND_WAIT_OPEN state.
 * We are waiting for DL_STARTED then proceed disconnect.
 * We have CEP but no Data Link Session.
 */
const static struct fsm_state ds_opening_halt[] =
{
/*  Action     Param Blk   Next State               index  Decoded Input      */
/* -------------------------------------------------------------------------- */
{NCIA_ACTION_30, &NCIA_DC,  nciast_opened_nodl},    /* 00 admin_stop          */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 01 admin_error         */
{NCIA_ACTION_33, &NCIA_DC,  nciast_opened_nodl},    /* 02 admin_tcp_down      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 03 admin_tcp_busy      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 04 admin_tcp_unbusy    */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 05 cls_req_opn_stn_req */
{NCIA_ACTION_14, FSM_NOARG, nciast_closed},         /* 06 cls_close_stn_req   */
{NCIA_ACTION_32, &NCIA_NI,  nciast_dls_wait_open},  /* 07 cls_conn_req        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 08 cls_conn_rsp        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 09 cls_disc_req        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 10 cls_disc_rsp        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 11 cls_data_req        */
{FSM_NOACTION,   FSM_NOARG, nciast_dl_started_wait},/* 12 cls_id_req          */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 13 cls_id_rsp          */
{NCIA_ACTION_33, &NCIA_TI,  FSM_NOCHANGE},          /* 14 cls_test_req        */
{NCIA_ACTION_4,  FSM_NOARG, FSM_NOCHANGE},          /* 15 cls_udata_req       */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 16 cls_id_stn_req      */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 17 cls_test_stn_req    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 18 cls_test_stn_rsp    */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 19 cls_udata_stn_req   */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 20 cls_flow_stop       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 21 cls_flow_start      */

{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 22 wan_canureach       */
{NCIA_ACTION_33, &NCIA_UI,  FSM_NOCHANGE},          /* 23 wan_dataframe       */
{NCIA_ACTION_18, FSM_NOARG, nciast_opened},         /* 24 wan_start_dl        */
{NCIA_ACTION_33, &NCIA_DC,  nciast_opened},         /* 25 wan_dl_started      */
{NCIA_ACTION_33, &NCIA_DC,  nciast_opened_nodl},    /* 26 wan_start_dl_failed */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 37 wan_dgrmframe       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 28 wan_xidframe        */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 29 wan_contact_stn     */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 20 wan_stn_contacted   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 31 wan_infoframe       */
{NCIA_ACTION_31, &NCIA_DC,  nciast_opened_nodl},    /* 32 wan_halt_dl         */
{NCIA_ACTION_33, &NCIA_DC,  nciast_opened_nodl},    /* 33 wan_halt_dl_noack   */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 34 wan_dl_halted       */
{FSM_NOACTION,   FSM_NOARG, FSM_NOCHANGE},          /* 35 wan_imp             */

{NCIA_ACTION_33, &NCIA_DC,  nciast_opened_nodl},    /* 36 timer_expired       */
/* -------------------------------------------------------------------------- */
};




static int ncia_server_fsm_decode(NciaCepT *ncia_ptr, int major, int minor);

/*******************************************************************
 *
 *            FSM Administation Structures/Routines
 *
 ********************************************************************/
 
#define NCIA_FSM_DECODE FSM_DECODE_ROUTINE(ncia_server_fsm_decode)

const static struct finite_state_machine ncia_fsm_table[] =
{
/*  State                   	Decoder     */
 { ds_closed,               NCIA_FSM_DECODE },
 { ds_opening,              NCIA_FSM_DECODE },
 { ds_opened,               NCIA_FSM_DECODE },
 { ds_connect_pending,      NCIA_FSM_DECODE },
 { ds_connected,            NCIA_FSM_DECODE },
 { ds_disc_pending,         NCIA_FSM_DECODE },
 { ds_opened_nodl,          NCIA_FSM_DECODE },
 { ds_dl_started_wait,      NCIA_FSM_DECODE },
 { ds_start_dl_rcvd,        NCIA_FSM_DECODE },
 { ds_dl_started_snd,       NCIA_FSM_DECODE },
 { ds_dls_wait_open,        NCIA_FSM_DECODE },
 { ds_connect_wait,         NCIA_FSM_DECODE },
 { ds_open_wait,            NCIA_FSM_DECODE },
 { ds_disc_wait,            NCIA_FSM_DECODE },
 { ds_disc_wait_noack,      NCIA_FSM_DECODE },
 { ds_opening_halt,         NCIA_FSM_DECODE }
};



static int ncia_server_fsm_decode (NciaCepT *ncia_ptr, int major, int minor)
{
    int index;

    switch (major) {
        case nciama_admin:
            switch(minor) {
            case nciami_admin_stop:
                index = 0;
                break;

            case nciami_admin_error:
                index = 1;
                break;

            case nciami_admin_tcp_down:
                index = 2;
                break;

            case nciami_admin_tcp_busy:
                index = 3;
                break;

            case nciami_admin_tcp_unbusy:
                index = 4;
                break;

            default:
                index = FSM_FAULT;
                break;
            }
            break;

        case nciama_dlu:
            switch(minor) {
            case nciami_cls_req_opn_stn_req:
                index = 5;
                break;

            case nciami_cls_close_stn_req:
                index = 6;
                break;

            case nciami_cls_conn_req:
                index = 7;
                break;

            case nciami_cls_conn_rsp:
                index = 8;
                break;

            case nciami_cls_disc_req:
                index = 9;
                break;

            case nciami_cls_disc_rsp:
                index = 10;
                break;

            case nciami_cls_data_req:
                index = 11;
                break;

            case nciami_cls_id_req:
                index = 12;
                break;

            case nciami_cls_id_rsp:
                index = 13;
                break;

            case nciami_cls_test_req:
                index = 14;
                break;

            case nciami_cls_udata_req:
                index = 15;
                break;

            case nciami_cls_id_stn_req:
                index = 16;
                break;

            case nciami_cls_test_stn_req:
                index = 17;
                break;

            case nciami_cls_test_stn_rsp:
                index = 18;
                break;

            case nciami_cls_udata_stn_req:
                index = 19;
                break;


            case nciami_cls_flow_stop:
                index = 20;
                break;

            case nciami_cls_flow_start:
                index = 21;
                break;

            default:
                index = FSM_FAULT;
                break;
            }
            break;

        case nciama_wan:
            switch(minor) {
            case nciami_wan_canureach:
                index = 22;
                break;

            case nciami_wan_dataframe:
                index = 23;
                break;

            case nciami_wan_start_dl:
                index = 24;
                break;

            case nciami_wan_dl_started:
                index = 25;
                break;

            case nciami_wan_start_dl_failed:
                index = 26;
                break;

            case nciami_wan_dgrmframe:
                index = 27;
                break;

            case nciami_wan_xidframe:
                index = 28;
                break;

            case nciami_wan_contact_stn:
                index = 29;
                break;

            case nciami_wan_stn_contacted:
                index = 30;
                break;

            case nciami_wan_infoframe:
                index = 31;
                break;

            case nciami_wan_halt_dl:
                index = 32;
                break;

            case nciami_wan_halt_dl_noack:
                index = 33;
                break;

            case nciami_wan_dl_halted:
                index = 34;
                break;

            case nciami_wan_imp:
                index = 35;
                break;

            default:
                index = FSM_FAULT;
                break;
            }
            break;

        case nciama_timer:
            switch(minor) {
            case nciami_timer_expired:
                index = 36;
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




static char *ncia_print_circuit_event_major(nciama_t major)
{
    switch (major) {
        case nciama_admin:
            return("ADMIN");

        case nciama_dlu:
            return("DLU");

        case nciama_wan:
            return("WAN");

        case nciama_timer:
            return("TIMER");

        default:
            return("Invalid Event");
    }
}

static char *ncia_print_circuit_event_minor(nciama_t major, uint minor)
{
    switch (major) {

      case nciama_admin:
        switch (minor) {
          case nciami_admin_stop:
            return("STOP");
          case nciami_admin_error:
            return("ERROR");
          case nciami_admin_tcp_down:
            return("TCP_DOWN");
          case nciami_admin_tcp_busy:
            return("TCP_BUSY");
          case nciami_admin_tcp_unbusy:
            return("TCP_UNBUSY");
        }
       

      case nciama_dlu:
        switch (minor) {
          case nciami_cls_req_opn_stn_req:
            return("ReqOpnStn.Req");

          case nciami_cls_close_stn_req:
            return("CloseStn.Req");

          case nciami_cls_conn_req:
            return("Connect.Req");

          case nciami_cls_conn_rsp:
            return("Connect.Rsp");

          case nciami_cls_disc_req:
            return("Disc.Req");

          case nciami_cls_disc_rsp:
            return("Disc.Rsp");

          case nciami_cls_data_req:
            return("Data.Rsp");

          case nciami_cls_id_req:
            return("Id.Req");

          case nciami_cls_id_rsp:
            return("Id.Rsp");

          case nciami_cls_test_req:
            return("Test.Req");

          case nciami_cls_udata_req:
            return("Udata.Req");

          case nciami_cls_id_stn_req:
            return("IdStn.Rsp");

          case nciami_cls_test_stn_req:
            return("TestStn.Req");

          case nciami_cls_test_stn_rsp:
            return("TestStn.Rsp");

          case nciami_cls_udata_stn_req:
            return("UdataStn.Req");

          case nciami_cls_flow_stop:
            return("Flow.Req (STOP)");

          case nciami_cls_flow_start:
            return("Flow.Req (START)");
        }
       

      case nciama_wan:
        switch (minor) {
          case nciami_wan_canureach:
            return("CUR");
          case nciami_wan_dataframe:
            return("DATA");
          case nciami_wan_start_dl:
            return("STDL");
          case nciami_wan_dl_started:
            return("DLST");
          case nciami_wan_start_dl_failed:
            return("SDLF");
          case nciami_wan_dgrmframe:
            return("DGRM");
          case nciami_wan_xidframe:
            return("XID");
          case nciami_wan_contact_stn:
            return("CONQ");
          case nciami_wan_stn_contacted:
            return("CONR");
          case nciami_wan_infoframe:
            return("INFO");
          case nciami_wan_halt_dl:
            return("HLTQ");
          case nciami_wan_halt_dl_noack:
            return("HLTN");
          case nciami_wan_dl_halted:
            return("HLTR");
          case nciami_wan_imp:
            return("IMP");
          default:
            return("UNKNOWN");
        }
       

      case nciama_timer:
        switch (minor) {
          case nciami_timer_expired:
            return("Expired");
        }

      default:
        return("UNKNOWN");
    }
}




/*
 * n c i a _ f s m
 */
short ncia_server_fsm (NciaCepT *ncia_ptr, nciama_t major, int minor)
{
    int fsm_rc;
    int old_state;

    old_state = ncia_ptr->fState;

    if (ncia_circuit_debug & NciaCircuitDebugSTATE)
        buginf("\nNCIA: server event: %s - %s state: %s",
            ncia_print_circuit_event_major(major),
            ncia_print_circuit_event_minor(major, minor),
            ncia_print_circuit_state(old_state));

    fsm_rc = (int) fsm_execute ("NCIA-SERVER", ncia_fsm_table,
                                ((int *) &(ncia_ptr->fState)), nciast_fsm_max,
                                ncia_ptr, major, minor, FALSE);

    NciaCircuitDebugState("\nNCIA: circuit state: %s -> %s",
           ncia_print_circuit_state(old_state), 
           ncia_print_circuit_state(ncia_ptr->fState));

    if (fsm_rc < 0) {
      switch (fsm_rc) {
      case FSM_RC_ERROR_INPUT:
      case FSM_RC_INVALID_INPUT:
        NciaCircuitDebugError("\nNCIA: ncia circuit fsm: Invalid input"); 
        break;
      case FSM_RC_UNKNOWN_STATE:
        NciaCircuitDebugError("\nNCIA: ncia circuit fsm: Illegal state"); 
        break;
      case FSM_RC_UNKNOWN_INPUT:
        NciaCircuitDebugError("\nNCIA: ncia circuit fsm: Illegal input"); 
        break;
      case FSM_RC_UNEXPECTED_INPUT:
        NciaCircuitDebugError("\nNCIA: ncia circuit fsm: Unexpected input"); 
        break;
      default:
        NciaCircuitDebugError(
            "\nNCIA: ncia circuit fsm: Unknown error rtn code"); 
        break;
      }
    }
    return (fsm_rc);
}

