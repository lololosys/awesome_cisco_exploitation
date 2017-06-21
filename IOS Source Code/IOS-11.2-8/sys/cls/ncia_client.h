/* $Id: ncia_client.h,v 3.1.2.5 1996/08/31 05:20:18 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_client.h,v $
 *------------------------------------------------------------------
 * ncia_client.h - Definitions for NCIA Client FSM
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_client.h,v $
 * Revision 3.1.2.5  1996/08/31  05:20:18  schiang
 * CSCdi67779:  debug ncia client message does not display all packet types
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/11  21:13:45  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/03  06:34:27  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/18  19:01:30  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.4  1996/05/14  18:42:14  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/14  04:31:51  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/05/03  22:06:23  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:42  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:16:13  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* NCIA client global variables */

#define NCIA_PROTOCOL_ID        0x80
#define NCIA_VERSION_NO         0x01
#define NCIA_FREED_CLIENT       0xffffffff

#define NCIA_MAX_SAPS             16
#define NCIA_MAX_CAP_XCHG          3

#define NCIA_CLIENT_CONFIGED    0x0100
#define NCIA_CLIENT_REGISTERED  0x0200
#define NCIA_CLIENT_NETBIOS     0x0400
#define NCIA_CLIENT_ACTIVE      0x0800

#define NCIA_MAX_TCP_QLEN          100
#define NCIA_TCP_HI_WATER_PERCENT   50
#define NCIA_TCP_LO_WATER_PERCENT   25
#define NCIA_OUT_MAC_RANGE      0xDFFF
#define NCIA_MAC_INDEX_USED     0xFFFF
#define NCIA_NOT_IN_HASH        0xFFFF

#define NCIA_CLIENT_TIMER_INT   10 * ONESEC


typedef enum _client_ret_t {
    CLIENT_UNREACHABLE,
    CLIENT_BUSY,
    CLIENT_SUCCESS,
    NO_MEMORY_FOR_PACKET
} client_ret_t;

typedef enum _cap_cmd_ret_t {
    CAP_CMD_MAC_OK,
    CAP_CMD_MAC_NO,
    CAP_CMD_MAC_OUT
} cap_cmd_ret_t;

typedef enum _ncia_client_eventtype {
    ACTIVE_OPEN,
    PASSIVE_OPEN,
    CLOSE_CONN,
    CONN_OPENED,
    CAP_RCVD,
    CAP_RSP_RCVD,
    KEEPALIVE_RCVD,
    KEEPALIVE_RSP_RCVD,
    TIME_OUT,
    BUSY_XPORT,
    UNBUSY_XPORT,
    NDLC_FSM_INPUT,
    NDLC_PRIMITIVES,
    CONN_CLOSED,
    CLOSE_PEER_REQ_RCVD,
    CLOSE_PEER_RSP_RCVD,
    CIRCUIT_FREED
} ncia_client_eventtype_t;

/*
 * Peer Transport Connection States
 */
typedef enum _nciapeerstatetype {
    NCIA_CLOSED,                /* read and write pipe closed */
    NCIA_OPEN_WAIT,             /* active open */
    NCIA_CAP_WAIT,              /* waiting for a cap exchg request */
    NCIA_CAP_NEG,               /* waiting for a cap exchg req/rsp */
    NCIA_OPENED,                /* both pipes opened */
    NCIA_BUSY,                  /* WAN transport is congested */
    NCIA_CLOSE_WAIT,            /* close connection */
    NCIA_SHUTDOWN_PENDING       /* TCP, HOST or Router shutdown */
} ncia_client_state_t;



typedef struct _ndlc_header {
    uchar proto_ver_id;         /* protocol id & version number */
    uchar message_type;         /* NCIA DLC message type        */
    ushort packet_length;       /* packet length, hdt + msg     */
} ndlc_header_t;


typedef struct _ndlc_cap_xchq_cv {
    uchar    ctl_vec_len;                /* ctl vec length, max is 18   */
    uchar    ctl_vec_typ;                /* 0x01, sap list              */
    uchar    sap_list[NCIA_MAX_SAPS];    /* up to 16 SAPs               */

    } ndlc_cap_xchg_cv_t;


typedef struct _ndlc_cap_xchq {
    uchar    peer_client_mac[IEEEBYTES]; /* client mac address      */
    uchar    cap_xchg_flag;              /* 0x01 NETBIOS, 0x02 TCP listen */
                                         /* 0x04 command, else response   */
    uchar    reserved;
    uchar    cap_xchg_vec[1];            /* len, cv_type, up to 16 SAP */
    } ndlc_cap_xchg_t;

typedef struct _ndlc_clo_peer_req {
    uchar    reason_code;                /* reason for disconnect   */
    uchar    reserved[3];                /* 3 bytes reserved field  */
    } ndlc_clo_peer_req_t;

typedef enum ncia_rd_state_t {
    RD_INIT,
    RD_HEADER,
    RD_DATA
} ncia_rd_state_t;

/*
 * NCIA client control block
 */
struct nciapeertype_ {
    nciapeertype        *fnext;         /* next in any queue of ncia types */
    ipaddrtype          ipaddr;         /* tcpdriver and udp */
    ushort              port;
    ushort              hash_index;     /* index to client hash queue */
    tcp_encaps          *t;             /* null, if tcp down after register*/
    pid_t               pid;
    ncia_rd_state_t     rd_state;
    ncia_client_state_t client_state;
    ncia_servertype     *ncia_server;
    uchar               Vmac[IEEEBYTES];
    ushort              VmacIndex;
    uchar               CapXchgFlags;
    ushort              NciaFlags;
    ndlc_header_t       ndlc_start;
    uchar               *target;
    ulong               sizeleft;
    int                 pkts_rx;
    int                 pkts_tx;
    int                 drops;
    ushort              keepalive_count;
    boolean             in_active_open;      /* tcp active open in prog */
    ulong               tcp_timeout;         /* tcp max retransmission time*/
    ushort              max_tcp_qlen;
    short               cur_cap_xchg_count;
    ndlc_cap_xchg_cv_t  cap_xchg_ctl_vec;         /* suport up to 16 saps  */
    paktype             *in_pak;        /* pak to be presented to server   */
    paktype             *out_pak;       /* TCP packets forwarded to client */
    NciaCepT            *current_pCepI;
    queuetype           OutputQ;        /* staging output when TCP is busy */
    queuetype           NciaCircuit;
    mgd_timer           timer;
};


/* NCIA ndlc protocol frame types */

typedef enum ndlc_msg_type {
    NDLC_CAN_U_REACH       = 0x01,
    NDLC_I_CAN_REACH       = 0x02,
    NDLC_I_CANNOT_REACH    = 0x03,
    NDLC_START_DL          = 0x04,
    NDLC_DL_STARTED        = 0x05,
    NDLC_START_DL_FAILED   = 0x06,
    NDLC_XID_FRAME         = 0x07,

    NDLC_CONTACT_STN       = 0x08,
    NDLC_STN_CONTACTED     = 0x09,
    NDLC_DATA_FRAME        = 0x0A,
    NDLC_INFO_FRAME        = 0x0B,
    NDLC_HALT_DL           = 0x0C,
    NDLC_HALT_DL_NOACK     = 0x0D,
    NDLC_DL_HALTED         = 0x0E,
    NDLC_FCM_FRAME         = 0x0F,
    NDLC_DGRM_FRAME        = 0x11,
    NDLC_CAP_XCHG          = 0x12,
    NDLC_CLOSE_PEER_REQ    = 0x13,
    NDLC_CLOSE_PEER_RSP    = 0x14,
    NDLC_PEER_TEST_REQ     = 0x1D,
    NDLC_PEER_TEST_RSP     = 0x1E,
    } ndlc_msg_type_t;

#define NDLC_TCP_DOWN           0xFF
#define NDLC_HDR_SZ             sizeof(ndlc_header_t)
#define NDLC_CAP_XCHG_SZ        sizeof(ndlc_cap_xchg_t) - 1
#define NDLC_CAP_XCHG_CV_SZ     sizeof(ndlc_cap_xchg_cv_t)
#define NDLC_CLOSE_PEER_REQ_SZ  sizeof(ndlc_clo_peer_req_t)
#define NDLC_CLOSE_PEER_RSP_SZ  0    
#define NDLC_KEEP_ALIVE_REQ_SZ  0    
#define NDLC_KEEP_ALIVE_RSP_SZ  0    


#define NCIA_CAP_XCHG_CMD         0x04
#define NCIA_CAP_XCHG_RSP         0x00

#define NCIA_NETBIOS_IND          0x01
#define NCIA_TCP_LISTEN_MODE      0x02

#define NCIA_CAP_CONST_TWO        0x02
#define NCIA_CAP_MIN_LEN          0x0c
#define NCIA_CAP_ONE_SAP_LEN      0x0f
#define NCIA_CAP_MAX_LEN          0x1e
#define NCIA_CLO_SYS_DOWN         0x01
#define NCIA_CLO_SUSPEND          0x02
#define NCIA_CLO_NO_MAC           0x03

#define NCIA_KEEP_CNT_THOLD          3


typedef struct _ndlc_can_u_reach {
    uchar target_mac[IEEEBYTES];
    uchar client_sap;
    uchar reserve[1];
} ndlc_can_u_reach_t;

typedef ndlc_can_u_reach_t ndlc_i_can_reach_t;
typedef ndlc_can_u_reach_t ndlc_i_cannot_reach_t;

typedef struct _ndlc_start_dl {
    uchar target_mac[IEEEBYTES];
    uchar target_sap;
    uchar client_sap;
    ulong origin_ckt_id;
    ulong target_ckt_id;
    uchar largest_frame_size;
    uchar window_size;
    uchar reserve[2];
} ndlc_start_dl_t;

typedef ndlc_start_dl_t ndlc_dl_started_t;
typedef ndlc_start_dl_t ndlc_start_dl_failed_t;


typedef struct _ndlc_halt_dl_t {
    ulong sender_ckt_id;
    ulong receiver_ckt_id;
    uchar reserve[4];
} ndlc_halt_dl_t;


typedef ndlc_halt_dl_t ndlc_halt_dl_noack_t;
typedef ndlc_halt_dl_t ndlc_dl_halted_t;


typedef struct _ndlc_xid_t {
    ulong session_id;
    uchar flow_control;
    uchar reserve[3];
} ndlc_xid_t;

typedef ndlc_xid_t ndlc_contact_stn_t;
typedef ndlc_xid_t ndlc_stn_contacted_t;
typedef ndlc_xid_t ndlc_info_frame_t;
typedef ndlc_xid_t ndlc_fcm_frame_t;
typedef ndlc_xid_t ndlc_dgrm_frame_t;



typedef struct _ndlc_data_frame {
    uchar target_mac[IEEEBYTES];
    uchar target_sap;
    uchar client_sap;
    uchar broadcast_type;
    uchar reserve[3];
} ndlc_data_frame_t;


typedef struct ndlc_packet {
    ndlc_header_t ndlc_header;
    union {
        ndlc_can_u_reach_t      can_u_reach;
        ndlc_i_can_reach_t      i_can_reach;
        ndlc_i_cannot_reach_t   i_cannot_reach;
        ndlc_start_dl_t         start_dl;
        ndlc_dl_started_t       dl_started;
        ndlc_start_dl_failed_t  start_dl_failed;
        ndlc_xid_t              xid_frame;
        ndlc_contact_stn_t      contact_stn;
        ndlc_stn_contacted_t    stn_contacted;
        ndlc_info_frame_t       info_frame;
        ndlc_halt_dl_t          halt_dl;
        ndlc_halt_dl_noack_t    halt_dl_noack;
        ndlc_dl_halted_t        dl_halted;
        ndlc_fcm_frame_t        fcm_frame;
        ndlc_dgrm_frame_t       dgrm_frame;
        ndlc_data_frame_t       data_frame;
        ndlc_cap_xchg_t         cap_xchg;
    } ndlc_data;
} ndlc_packet_t;



/* Client related DEBUG flags */

#define NciaClientDebugMSGS    0x1000
#define NciaClientDebugEVENT   0x2000
#define NciaClientDebugERROR   0x4000
#define NciaClientDebugALL     (NciaClientDebugMSGS | NciaClientDebugEVENT | NciaClientDebugERROR)


static
inline int ncia_get_ndlc_size (ndlc_msg_type_t ndlc)
{
    switch(ndlc) {
    case NDLC_CAN_U_REACH:      
    case NDLC_I_CAN_REACH:      
    case NDLC_I_CANNOT_REACH:   
             return(sizeof(ndlc_can_u_reach_t)+NDLC_HDR_SZ); 

    case NDLC_START_DL:         
    case NDLC_DL_STARTED:       
    case NDLC_START_DL_FAILED:  
             return(sizeof(ndlc_start_dl_t)+NDLC_HDR_SZ); 

    case NDLC_HALT_DL:          
    case NDLC_HALT_DL_NOACK:    
    case NDLC_DL_HALTED:        
             return(sizeof(ndlc_halt_dl_t)+NDLC_HDR_SZ);

    case NDLC_CONTACT_STN:      
    case NDLC_STN_CONTACTED:    
    case NDLC_FCM_FRAME:        
    case NDLC_XID_FRAME:        
    case NDLC_INFO_FRAME:       
    case NDLC_DGRM_FRAME:       
             return(sizeof(ndlc_xid_t)+NDLC_HDR_SZ); 

    case NDLC_DATA_FRAME:       
             return(sizeof(ndlc_data_frame_t)+NDLC_HDR_SZ); 
    default:                    
             return(0);
    }
}

/* ncia_client.c */

extern boolean ncia_tcpd_connect(nciapeertype *ncia_client);
extern void ncia_tcpd_listen(tcbtype *tcb);
extern void ncia_client_destroy(nciapeertype *ncia_client);
extern nciapeertype *ncia_find_client_by_ipaddr(ncia_servertype *ncia_server,
                                                ipaddrtype ipaddr);
extern void ncia_terminate_client(nciapeertype *ncia_client);
extern void ncia_shutdown_client(nciapeertype *ncia_client, boolean slam_flag);
extern client_ret_t ncia_send_from_out_pak(nciapeertype *ncia_client);
extern boolean ncia_client_active(ulong sport, ulong dport, ipaddrtype fhost);
extern void ncia_pre_client_fsm(nciapeertype *ncia_client, 
                                ncia_client_eventtype_t event);
extern void ncia_init_tcp_value_in_client_cb(nciapeertype *ncia_client);
extern boolean ncia_mac_in_range_but_taken(nciapeertype *ncia_client);
extern boolean ncia_client_mac_in_range(nciapeertype *ncia_client);
extern void ncia_add_to_free_mac_index_chain(nciapeertype *ncia_client);
extern void ncia_remove_from_free_mac_index_chain(nciapeertype *ncia_client);
extern ushort ncia_subtract_macaddr_for_vmacindex(uchar *addr1, uchar *addr2, 
                                                  ushort mac_range);
extern void ncia_chain_client_to_hash_tbl(nciapeertype *ncia_client);
extern void ncia_unchain_client_from_hash_tbl(nciapeertype *ncia_client);
extern void ncia_tcpd_close(tcp_encaps *t, paktype *pak);
extern boolean ncia_compare_macaddr(uchar *addr1, uchar *addr2, 
                                    ushort mac_range);

/* ncia_client_fsm.c */

extern char *ncia_state_to_string(ncia_client_state_t state);
extern char *ncia_event_to_string(ncia_client_eventtype_t event);
extern void ncia_client_fsm(nciapeertype *ncia_client, 
                            ncia_client_eventtype_t event);

/* ncia.c */

extern void ncia_send_ndlc(NciaCepT *ncia_ptr, ndlc_msg_type_t ndlc);
