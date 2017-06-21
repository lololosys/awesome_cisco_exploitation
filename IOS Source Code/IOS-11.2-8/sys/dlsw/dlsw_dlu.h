/* $Id: dlsw_dlu.h,v 3.5.6.1 1996/05/17 10:45:48 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_dlu.h,v $
 *------------------------------------------------------------------
 * Interface between DLSw and CLSI structure definitions
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Includes all the function prototypes for 
 * core/fsm/csm to CLSI interface
 *------------------------------------------------------------------
 * $Log: dlsw_dlu.h,v $
 * Revision 3.5.6.1  1996/05/17  10:45:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.30.3  1996/04/25  04:28:06  ioakley
 * CSCdi55673:  Reduce number of calls on DLSw LAN side and remove
 *              functions that are no longer useful or necessary.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.30.2  1996/04/24  06:30:20  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.4.30.1  1996/03/17  17:38:04  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5  1996/03/05  23:56:29  kpatel
 * CSCdi50720:  SNA session hangs under heavy traffic
 *
 * Revision 3.4  1995/12/15  17:18:26  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.3  1995/12/09  16:49:07  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.2  1995/11/17  09:03:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:55:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/07  04:37:55  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.3  1995/08/07  05:31:23  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.2  1995/07/31  18:58:55  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.1  1995/06/07  20:27:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _DLSW_DLU_H_
#define _DLSW_DLU_H_

#include "../cls/clsi_queue.h"

#define FAIL 0
#define PASS 1

#define NUM_OF_RINGS 16
#define NUM_OF_SLOTS 10
#define NUM_OF_TYPES  5

#define DLSW_DEF_EXPLQ_DEPTH 100 /* same as CSM_DEF_EXPLQ_DEPTH */

extern dlsw_ring_t *dlsw_glob_ring_access_check(hword, hword);

/* Used for both data and control portion of message */

struct strbuf
 {
  int len;         /* data length */
  char *buf;       /* pointer to buffer */
 };

struct str_core_rcvctl 
 {
  int prim_type;
  int msg_type;
  int flags;
  int fsm_event;
  int result;
  int resv;
  int len;
 };

struct str_core_sndctl 
 {
  int prim_type;
  int msg_type;
  int flags;
  int resv;
 };

/* Supported primitive are as follows */
#define  DLSW_INIT_LAN           1
#define  DLSW_CONFIG_LAN         2
#define  DLSW_FSM_LAN            3
#define  DLSW_CSTM_LAN           4


/* supported Msg types from each sub-modules are as follows */
/* FOR INIT    sub-module */
#define INIT_START   1
#define INIT_STOP    2

/* FOR CONFIG  sub-module */
#define CONFIG_FILT_1  1
#define CONFIG_FILT_2  2
#define CONFIG_ADDR_1  3
#define CONFIG_ADDR_2  4

/* FOR FSM     sub-module */
#define FSM_1             1
#define FSM_2             2
#define FSM_3             3

/* FOR CSTM    sub-module */
#define CSTM_NETBIOS_WILD_FILT 1
#define CSTM_NETBIOS_FILT      2
#define CSTM_SNA_FILT          3

/* Supported primitive are as follows */
#define  DLSW_LAN_INIT            1
#define  DLSW_LAN_CONFIG          2
#define  DLSW_LAN_FSM             3
#define  DLSW_LAN_CSTM            4 


/*
 * Calls coming from the DLSw CORE to the DISP.
 */

/* CLSEntityManagerHandleMsg */
#define  D_ENABLE_REQ               0
#define  D_DISABLE_REQ              1
#define  D_ACTIVATESAP_REQ          2
#define  D_ACTIVATERING_REQ         3

/* CLSSapHandleMsg */
#define  D_DEACTIVATESAP_REQ        4
#define  D_DEACTIVATERING_REQ       5
#define  D_REQ_OPNSTN_REQ           6

/* CLSCepHandleMsg */
#define  D_REQ_OPN_REQ              7
#define  D_CLOSESTN_REQ             8
#define  D_ID_REQ                   9
#define  D_ID_RSP                   10
#define  D_TEST_REQ                 11
#define  D_TEST_RSP                 12
#define  D_CONNECT_REQ              13
#define  D_CONNECT_RSP              14
#define  D_SIGNALSTN_REQ            15
#define  D_DATA_REQ                 16
#define  D_UDATA_REQ                17
#define  D_FLOW_REQ                 18
#define  D_DISCONNECT_REQ           19
#define  D_DISCONNECT_RSP           20
#define  D_OPENSTN_REQ              21
#define  D_IDSTN_REQ                22
#define  D_IDSTN_RSP                23
#define  D_TESTSTN_REQ              24
#define  D_TESTSTN_RSP              25
#define  D_UDATASTN_REQ             26

/* Misc */
#define  D_MODIFYSAP_REQ            29


#define  D_CONFIG_1                 30
#define  D_CONFIG_2                 31
#define  D_CONFIG_3                 32
#define  D_CONFIG_4                 33

#define  D_INIT_1                   50
#define  D_INIT_2                   51
#define  D_INIT_3                   52
#define  D_INIT_4                   53
#define  D_INIT_5                   54


 struct D_enable_req
  {
   int          prim_type;
   idbtype     *swidb;
   CLSDLCType_e port_type;
   ushort       span_index;
  };

 struct D_disable_req
  {
   int          prim_type;
   idbtype     *swidb;
   CLSDLCType_e port_type;
  };

 struct D_activatesap_req
  {
   int          prim_type;
   idbtype     *swidb;
   CLSDLCType_e port_type;
   byte         sap_value;
   word         p_sap_id;
   word		u_sap_id;
  };

 struct D_modifysap_req
  {
   int          prim_type;
   idbtype     *swidb;
   CLSDLCType_e port_type;
   byte         sap_value;
   word         p_sap_id;
  };

 struct D_activatering_req
  {
   int    	prim_type;
   idbtype     *swidb;
   ushort 	bridge_number;
   ushort 	source_ring;
   ushort 	target_ring;
   CLSDLCType_e port_type;
   word   	u_sap_id;
  };

 struct D_deactivatesap_req
  {
   int          prim_type;
   idbtype     *swidb;
   CLSDLCType_e port_type;
   byte         sap_value;
   word   	p_sap_id;
  };

 struct D_deactivatering_req
  {
   int    prim_type;
   word   p_sap_id;
   word   u_sap_id;
  };

 struct D_reqopenstn_req
  {
   int   prim_type;
   word p_sap_id;
   word  uCepId;
   byte  remoteAddr[IEEEBYTES]; 
   byte  rSap;
   byte  localAddr[IEEEBYTES]; 
   byte  lSap;
   byte  options;
   byte  rifLength;
   byte  *rif;
   ushort fr_dlci;
  };

/*
 * May need to issue a close stn request to cls before
 * the DLU gets a p_cep_id back on ReqOpnStn.cfm. In this
 * case, close station with u_cep_id.
 */
 struct D_closestn_req
  {
   int prim_type;
   byte identifierType; /* CLS_P_CEP_ID ot CLS_U_CEP_ID */
   word identifier;     /* pCepId or UCepId */
  };

 struct D_id_req
  {
   int prim_type;
   word   pCepId;
   byte   *xidData;
   size_t xidLength;

  };

 struct D_id_rsp
  {
   int    prim_type;
   word   pCepId;
   byte   *xidData;
   size_t xidLength;

  };

 struct D_test_req
  {
   int    prim_type;
   word   pCepId;
   byte   *testData;
   size_t testLength;

  };

 struct D_test_rsp
  {
   int    prim_type;
   word   pCepId;
   byte   *testData;
   size_t testLength;

  };

 struct D_generic_stn_msg
  {
   int    prim_type;
   word   p_sap_id;
   byte   *frame_Data;
   size_t frame_Length;
   byte  remoteAddr[IEEEBYTES];
   byte  rSap;
   byte  localAddr[IEEEBYTES];
   byte  lSap;
   byte  options;
   byte  rifLength;
   byte  *rif;

  };

 struct D_connect_req
  {
   int   prim_type;
   word  pCepId;
  };

 struct D_connect_rsp
  {
   int   prim_type;
   word  pCepId;
   byte  flag;

  };

 struct D_signalstn_req
  {
   int   prim_type;
   word  pCepId;

  };

 struct D_data_req
  {
   int    prim_type;
   word   pCepId;
   byte   *theData;
   size_t dataLength;
   paktype *pak;

  };

 struct D_udata_req
  {
   int    prim_type;
   word   pCepId;
   byte   *theUData;
   size_t udataLength;

  };

 struct D_flow_req
  {
   int   prim_type;
   word  pCepId;
   byte  action;

  };

 struct D_disconnect_req
  {
   int   prim_type;
   word  pCepId;

  };

 struct D_disconnect_rsp
  {
   int   prim_type;
   word  pCepId;

  };

 struct D_openstn_req
  {
   int   prim_type;
   word  pCepId;

  };

 struct D_config_1
  {
   int   prim_type;

  };

 struct D_config_2
  {
   int   prim_type;

  };

 struct D_init_1
  {
   int   prim_type;

  };

 struct D_init_2
  {
   int   prim_type;

  };

TCLSIMsg* CreateEnableReq(struct D_enable_req *);
TCLSIMsg* CreateDisableReq(struct D_disable_req *);
TCLSIMsg* CreateActSapReq(struct D_activatesap_req *);
TCLSIMsg* CreateDeActSapReq(struct D_deactivatesap_req *);
TCLSIMsg* CreateReqOpnStnReq(struct D_reqopenstn_req *);
TCLSIMsg* CreateIDReq(struct D_id_req *);
TCLSIMsg* CreateIDRsp(struct D_id_rsp *);
TCLSIMsg* CreateConnectReq(struct D_connect_req *);
TCLSIMsg* CreateConnectRsp(struct D_connect_rsp *);
TCLSIMsg* CreateSignalStnReq(struct D_signalstn_req *);
TCLSIMsg* CreateDataReq(struct D_data_req *);
TCLSIMsg* CreateDiscReq(struct D_disconnect_req *);
TCLSIMsg* CreateDiscRsp(struct D_disconnect_rsp *);
TCLSIMsg* CreateCloseStnReq(struct D_closestn_req *);
TCLSIMsg* CreateActRingReq(struct D_activatering_req *);
TCLSIMsg* CreateDeActRingReq(struct D_deactivatering_req *);
TCLSIMsg* CreateTestReq(struct D_test_req *);
TCLSIMsg* CreateTestRsp(struct D_test_rsp *);
TCLSIMsg* CreateUDataReq(struct D_udata_req *);
TCLSIMsg* CreateFlowReq(struct D_flow_req *);

TCLSIMsg* CreateTestStnReq(struct D_generic_stn_msg *);
TCLSIMsg* CreateTestStnRsp(struct D_generic_stn_msg *);
TCLSIMsg* CreateIDStnReq(struct D_generic_stn_msg *);
TCLSIMsg* CreateIDStnRsp(struct D_generic_stn_msg *);
TCLSIMsg* CreateUDataStnReq(struct D_generic_stn_msg *);


 union D_primitives
  {
   int type;
   struct D_generic_stn_msg disp_generic_stn_msg;
   struct D_enable_req disp_enable_req;
   struct D_disable_req disp_disable_req;
   struct D_activatesap_req disp_activatesap_req;
   struct D_activatering_req disp_activatering_req;
   struct D_deactivatesap_req disp_deactivatesap_req;
   struct D_deactivatering_req disp_deactivatering_req;
   struct D_modifysap_req disp_modifysap_req;
   struct D_reqopenstn_req disp_reqopenstn_req;
   struct D_closestn_req disp_closestn_req;
   struct D_id_req disp_id_req;
   struct D_id_rsp disp_id_rsp;
   struct D_test_req disp_test_req;
   struct D_test_rsp disp_test_rsp;
   struct D_connect_req disp_connect_req;
   struct D_connect_rsp disp_connect_rsp;
   struct D_signalstn_req disp_signalstn_req;
   struct D_data_req disp_data_req;
   struct D_udata_req disp_udata_req;
   struct D_flow_req disp_flow_req;
   struct D_disconnect_req disp_disconnect_req;
   struct D_disconnect_rsp disp_disconnect_rsp;
   struct D_openstn_req disp_openstn_req;

   struct D_config_1 disp_config_1;
   struct D_config_2 disp_config_2;

   struct D_init_1 disp_init_1;
   struct D_init_2 disp_init_2;

  };

/*
 * DEMUX values.
 */

#define  D_DLSW             1
#define  D_DLSW_AND_RSRB    2
#define  D_RSRB             3
#define  D_LNM              4
#define  D_APPN             5
#define  D_DSPU             6
#define  D_DLSW2            7

/*
 * DLSW packet types.
 */
#define  DLSW_TEST    1
#define  DLSW_XID     2
#define  DLSW_SABME   3
#define  DLSW_UDATA   4
#define  DLSW_OTHER   5
#define  DLSW_ERROR   6

/*
 * Exported Data from dlsw_dlu.c
 */
extern TCLSIQueue* gDLSW2CLSWriteQ;

int update_lan_filt_dbase(int msg_type, struct strbuf *databuf, int *flags);
int update_lan_config(int msg_type, struct strbuf *databuf, int *flags);
int dlsw_dlu_clean(void);
int dlsw_dlu_init(void);
int output_filter_check(struct strbuf *databuf);
int input_filter_check(TCLSIMsg *theMsg);
void disp_to_init(int result);
void disp_to_cstm(int msg_type, TCLSIMsg *msg);
void disp_to_fsm(int msg_type,  TCLSIMsg *msg);
void disp_to_config(int result);

void dlsw_work (paktype *pak, hwidbtype *idb, int riflen);
int dlsw_demux (paktype *pak, hwidbtype *idb);
void fsm_to_disp(union D_primitives *dp);
extern boolean dlsw_cantput(word UCepId,boolean queueempty);

#endif
