
ncia_client.h,3976
#define NCIA_PROTOCOL_ID 57,1868
#define NCIA_VERSION_NO 58,1905
#define NCIA_FREED_CLIENT 59,1942
#define NCIA_MAX_SAPS 61,1986
#define NCIA_MAX_CAP_XCHG 62,2023
#define NCIA_CLIENT_CONFIGED 64,2061
#define NCIA_CLIENT_REGISTERED 65,2100
#define NCIA_CLIENT_NETBIOS 66,2139
#define NCIA_CLIENT_ACTIVE 67,2178
#define NCIA_MAX_TCP_QLEN 69,2218
#define NCIA_TCP_HI_WATER_PERCENT 70,2257
#define NCIA_TCP_LO_WATER_PERCENT 71,2296
#define NCIA_OUT_MAC_RANGE 72,2335
#define NCIA_MAC_INDEX_USED 73,2374
#define NCIA_NOT_IN_HASH 74,2413
#define NCIA_CLIENT_TIMER_INT 76,2453
typedef enum _client_ret_t _client_ret_t79,2499
} client_ret_t;client_ret_t84,2614
typedef enum _cap_cmd_ret_t _cap_cmd_ret_t86,2631
} cap_cmd_ret_t;cap_cmd_ret_t90,2721
typedef enum _ncia_client_eventtype _ncia_client_eventtype92,2739
} ncia_client_eventtype_t;ncia_client_eventtype_t110,3095
typedef enum _nciapeerstatetype _nciapeerstatetype115,3166
} ncia_client_state_t;ncia_client_state_t124,3698
typedef struct _ndlc_header _ndlc_header128,3724
} ndlc_header_t;ndlc_header_t132,3955
typedef struct _ndlc_cap_xchq_cv _ndlc_cap_xchq_cv135,3974
    } ndlc_cap_xchg_cv_t;ndlc_cap_xchg_cv_t140,4235
typedef struct _ndlc_cap_xchq _ndlc_cap_xchq143,4263
    } ndlc_cap_xchg_t;ndlc_cap_xchg_t149,4617
typedef struct _ndlc_clo_peer_req _ndlc_clo_peer_req151,4641
    } ndlc_clo_peer_req_t;ndlc_clo_peer_req_t154,4819
typedef enum ncia_rd_state_t ncia_rd_state_t156,4847
} ncia_rd_state_t;ncia_rd_state_t160,4918
struct nciapeertype_ nciapeertype_165,4974
typedef enum ndlc_msg_type ndlc_msg_type202,6547
    } ndlc_msg_type_t;ndlc_msg_type_t225,7312
#define NDLC_TCP_DOWN 227,7336
#define NDLC_HDR_SZ 228,7373
#define NDLC_CAP_XCHG_SZ 229,7427
#define NDLC_CAP_XCHG_CV_SZ 230,7487
#define NDLC_CLOSE_PEER_REQ_SZ 231,7546
#define NDLC_CLOSE_PEER_RSP_SZ 232,7606
#define NDLC_KEEP_ALIVE_REQ_SZ 233,7644
#define NDLC_KEEP_ALIVE_RSP_SZ 234,7682
#define NCIA_CAP_XCHG_CMD 237,7722
#define NCIA_CAP_XCHG_RSP 238,7761
#define NCIA_NETBIOS_IND 240,7801
#define NCIA_TCP_LISTEN_MODE 241,7840
#define NCIA_CAP_CONST_TWO 243,7880
#define NCIA_CAP_MIN_LEN 244,7919
#define NCIA_CAP_ONE_SAP_LEN 245,7958
#define NCIA_CAP_MAX_LEN 246,7997
#define NCIA_CLO_SYS_DOWN 247,8036
#define NCIA_CLO_SUSPEND 248,8075
#define NCIA_CLO_NO_MAC 249,8114
#define NCIA_KEEP_CNT_THOLD 251,8154
typedef struct _ndlc_can_u_reach _ndlc_can_u_reach254,8195
} ndlc_can_u_reach_t;ndlc_can_u_reach_t258,8307
typedef ndlc_can_u_reach_t ndlc_i_can_reach_t;ndlc_i_can_reach_t260,8330
typedef ndlc_can_u_reach_t ndlc_i_cannot_reach_t;ndlc_i_cannot_reach_t261,8377
typedef struct _ndlc_start_dl _ndlc_start_dl263,8428
} ndlc_start_dl_t;ndlc_start_dl_t272,8662
typedef ndlc_start_dl_t ndlc_dl_started_t;ndlc_dl_started_t274,8682
typedef ndlc_start_dl_t ndlc_start_dl_failed_t;ndlc_start_dl_failed_t275,8725
typedef struct _ndlc_halt_dl_t _ndlc_halt_dl_t278,8775
} ndlc_halt_dl_t;ndlc_halt_dl_t282,8882
typedef ndlc_halt_dl_t ndlc_halt_dl_noack_t;ndlc_halt_dl_noack_t285,8902
typedef ndlc_halt_dl_t ndlc_dl_halted_t;ndlc_dl_halted_t286,8947
typedef struct _ndlc_xid_t _ndlc_xid_t289,8990
} ndlc_xid_t;ndlc_xid_t293,9087
typedef ndlc_xid_t ndlc_contact_stn_t;ndlc_contact_stn_t295,9102
typedef ndlc_xid_t ndlc_stn_contacted_t;ndlc_stn_contacted_t296,9141
typedef ndlc_xid_t ndlc_info_frame_t;ndlc_info_frame_t297,9182
typedef ndlc_xid_t ndlc_fcm_frame_t;ndlc_fcm_frame_t298,9220
typedef ndlc_xid_t ndlc_dgrm_frame_t;ndlc_dgrm_frame_t299,9257
typedef struct _ndlc_data_frame _ndlc_data_frame303,9298
} ndlc_data_frame_t;ndlc_data_frame_t309,9457
typedef struct ndlc_packet ndlc_packet312,9480
} ndlc_packet_t;ndlc_packet_t333,10325
#define NciaClientDebugMSGS 339,10379
#define NciaClientDebugEVENT 340,10417
#define NciaClientDebugERROR 341,10455
#define NciaClientDebugALL 342,10493
inline int ncia_get_ndlc_size 346,10601
