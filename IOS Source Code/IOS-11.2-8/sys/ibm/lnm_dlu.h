/* $Id: lnm_dlu.h,v 3.2.58.1 1996/09/10 03:14:16 ioakley Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lnm_dlu.h,v $
 *------------------------------------------------------------------
 * lnm_dlu.h -- CLSI Lan Manager Data Link User Definitions
 *
 * August 1995, Ivan P. Oakley
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lnm_dlu.h,v $
 * Revision 3.2.58.1  1996/09/10  03:14:16  ioakley
 * CSCdi60079:  Correct clsi compliant LNM dlu clean-up problems
 *              that result in LNM clsi related error messages when
 *              "no source-bridge"/"source-bridge" commands are
 *              entered on token ring interfaces.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:23:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  02:38:38  ioakley
 * Placeholder for new file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct str_lnm_buf
{
    int len;      /* data length */
    char *buf;    /* pointer to buffer */ 
};

struct str_lnm_rcvctl
{
    int prim_type;
    int msg_type;
    int flags;
    int fsm_event;
    int result;
    int resv;
    int len;
};

struct str_lnm_sndctl
{
    int prim_type;
    int msg_type;
    int flags;
    int resv;
};    

void Lnm_to_Lan(struct str_lnm_buf *ctlbuf, struct str_lnm_buf *databuf, int *flags);
#define LFAIL 0
#define LPASS 1
#define LNM_DEF_Q_DEPTH 100

/* Supported primitives are as follows */
#define LNM_INIT_LAN              1
#define LNM_CONFIG_LAN            2
#define LNM_FSM_LAN               3

/* supported Msg types for the LNM sub-modules are as follows */
/* For the INIT sub-module */
#define I_START 1
#define I_STOP  2

/* For the FSM sub-module */
#define LFSM_1 1
#define LFSM_2 2
#define LFSM_3 3

void Lan_to_Lnm(struct str_lnm_buf *ctlbuf, struct str_lnm_buf *databuf, int *flags);
/* Supported primitives are as follows: */
#define LNM_LAN_INIT              1
#define LNM_LAN_FSM               2

/*
 * Calls coming from LNM to the DISP.
 */

/* CLSEntityManagerHandleMsg */
#define  L_ENABLE_REQ         0
#define  L_DISABLE_REQ        1
#define  L_ACTIVATESAP_REQ    2

/* CLSSapHandleMsg */
#define  L_DEACTIVATESAP_REQ  3
#define  L_REQ_OPNSTN_REQ     4

/*CLSCepHandleMsg */
#define  L_REQ_OPN_REQ        5
#define  L_CLOSESTN_REQ       6
#define  L_CONNECT_REQ        7
#define  L_SIGNALSTN_REQ      8
#define  L_DATA_REQ           9
#define  L_UDATA_REQ          10
#define  L_DISCONNECT_REQ     11
#define  L_DISCONNECT_RSP     12
#define  L_UDATASTN_REQ       13

/* Miscellaneous */
#define  L_INIT_1             20
#define  L_INIT_2             21
#define  L_INIT_3             22
#define  L_INIT_4             23

struct L_enable_req
{
    int           prim_type;
    idbtype       *swidb;
    hwidbtype     *hwidb;
    word          corr;
    CLSDLCType_e  port_type;
};

struct L_disable_req
{
    int           prim_type;
    idbtype       *swidb;
    CLSDLCType_e  port_type;
};

struct L_activatesap_req
{
    int           prim_type;
    idbtype       *swidb;
    CLSDLCType_e  port_type;
    byte          sap_value;
    word          p_sap_id;
    word          u_sap_id;
};

struct L_deactivatesap_req
{
    int           prim_type;
    idbtype       *swidb;
    CLSDLCType_e  port_type;
    byte          sap_value;
    word          p_sap_id;
    word          u_sap_id;
};

struct L_reqopenstn_req
{
    int           prim_type;
    word          p_sap_id;
    word          uCepId;
    byte          remoteAddr[IEEEBYTES];
    byte          rSap;
    byte          localAddr[IEEEBYTES];
    byte          lSap;
    byte          options;
    byte          rifLength;
    byte          *rif;
    ushort        fr_dlci;
};

struct L_closestn_req
{
    int           prim_type;
    word          pCepId;
};

struct L_connect_req
{
    int           prim_type;
    word          pCepId;
};

struct L_signalstn_req
{
    int           prim_type;
    word          pCepId;
};

struct L_data_req
{
    int           prim_type;
    word          pCepId;
    byte          *theData;
    size_t        dataLength;
    paktype       *pak;
};

struct L_udata_req
{
    int           prim_type;
    word          pCepId;
    byte          *theUData;
    size_t        udataLength;
};

struct L_disconnect_req
{
    int           prim_type;
    word          pCepId;
};

struct L_disconnect_rsp
{
    int           prim_type;
    word          pCepId;
};

struct L_udatastn_req
{
    int           prim_type;
    word          p_sap_id;
    byte          *theUData;
    size_t        udataLength;
    byte          remoteAddr[IEEEBYTES];
    byte          rSap;
    byte          localAddr[IEEEBYTES];
    byte          lSap;
    byte          options;
    byte          rifLength;
    byte          *rif;
};

struct L_init_1
{
    int           primtype;
};

struct L_init_2
{
    int           primtype;
};

TCLSIMsg* LCreateEnableReq(struct L_enable_req *);
TCLSIMsg* LCreateDisableReq(struct L_disable_req *);
TCLSIMsg* LCreateActSapReq(struct L_activatesap_req *);
TCLSIMsg* LCreateDeActSapReq(struct L_deactivatesap_req *);
TCLSIMsg* LCreateReqOpnStnReq(struct L_reqopenstn_req *);
TCLSIMsg* LCreateConnectReq(struct L_connect_req *);
TCLSIMsg* LCreateSignalStnReq(struct L_signalstn_req *);
TCLSIMsg* LCreateDataReq(struct L_data_req *);
TCLSIMsg* LCreateDiscReq(struct L_disconnect_req *);
TCLSIMsg* LCreateDiscRsp(struct L_disconnect_rsp *);
TCLSIMsg* LCreateCloseStnReq(struct L_closestn_req *);
TCLSIMsg* LCreateUDataReq(struct L_udata_req *);
TCLSIMsg* LCreateUDataStnReq(struct L_udatastn_req *);

union L_primitives
{
    int    type;
    struct L_enable_req ldisp_enable_req;
    struct L_disable_req ldisp_disable_req;
    struct L_activatesap_req ldisp_activatesap_req;
    struct L_deactivatesap_req ldisp_deactivatesap_req;
    struct L_reqopenstn_req ldisp_reqopenstn_req;
    struct L_closestn_req ldisp_closestn_req;
    struct L_connect_req ldisp_connect_req;
    struct L_signalstn_req ldisp_signalstn_req;
    struct L_data_req ldisp_data_req;
    struct L_udata_req ldisp_udata_req;
    struct L_udatastn_req ldisp_udatastn_req;
    struct L_disconnect_req ldisp_disconnect_req;
    struct L_disconnect_rsp ldisp_disconnect_rsp;
    struct L_init_1 ldisp_init_1;
    struct L_init_2 ldisp_init_2;
};
    
/*
 * LNM (LLC) packet types
 */
#define LNM_TEST  1
#define LNM_SABME 2
#define LNM_UDATA 3
#define LNM_OTHER 4
#define LNM_ERROR 5

int update_llan_config(int msg_type, struct str_lnm_buf *databuf, int *flags);
int lnm_dlu_clean(void);
int lnm_dlu_init(void);
void ldisp_to_init(int result);
void ldisp_to_lfsm(int msg_type, TCLSIMsg *msg);
void lnm_work(paktype *pak, hwidbtype *idb, int riflen);
void lfsm_to_ldisp(union L_primitives *dp);
void init_to_ldisp(union L_primitives *dp);
