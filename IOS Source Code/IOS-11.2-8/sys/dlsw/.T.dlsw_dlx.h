
dlsw_dlx.h,4642
#define __DLSW_DLX_H__29,814
enum dlx_op_type dlx_op_type32,860
    DLX_CAN_U_REACH 33,879
    DLX_CAN_U_REACH       = 0x01,33,879
    DLX_I_CAN_REACH 34,913
    DLX_I_CAN_REACH       = 0x02,34,913
    DLX_I_CANNOT_REACH 35,947
    DLX_I_CANNOT_REACH    = 0x03,35,947
    DLX_START_DL 36,981
    DLX_START_DL          = 0x04,36,981
    DLX_DL_STARTED 37,1015
    DLX_DL_STARTED        = 0x05,37,1015
    DLX_START_DL_FAILED 38,1049
    DLX_START_DL_FAILED   = 0x06,38,1049
    DLX_RESTART_DL 39,1083
    DLX_RESTART_DL        = 0x07,39,1083
    DLX_DL_RESTARTED 40,1117
    DLX_DL_RESTARTED      = 0x08,40,1117
    DLX_RESTART_DL_FAILED 41,1151
    DLX_RESTART_DL_FAILED = 0x09,41,1151
    DLX_XIDFRAME 42,1185
    DLX_XIDFRAME          = 0x0A,42,1185
    DLX_CONTACT_STN 43,1219
    DLX_CONTACT_STN       = 0x0B,43,1219
    DLX_STN_CONTACTED 44,1253
    DLX_STN_CONTACTED     = 0x0C,44,1253
    DLX_CL_DATA_FRAME 45,1287
    DLX_CL_DATA_FRAME     = 0x0D,45,1287
    DLX_INFO_FRAME 46,1321
    DLX_INFO_FRAME        = 0x0E,46,1321
    DLX_HALT_DL 47,1355
    DLX_HALT_DL           = 0x0F,47,1355
    DLX_DL_HALTED 48,1389
    DLX_DL_HALTED         = 0x10,48,1389
    DLX_SNA_DGRM 49,1423
    DLX_SNA_DGRM          = 0x11,49,1423
    DLX_NB_DGRM 50,1457
    DLX_NB_DGRM           = 0x12,50,1457
    DLX_QUERY_DL 51,1491
    DLX_QUERY_DL          = 0x13,51,1491
    DLX_QUERY_DL_RESPONSE 52,1525
    DLX_QUERY_DL_RESPONSE = 0x14,52,1525
    DLX_IPM_FRAME 53,1559
    DLX_IPM_FRAME         = 0x15,53,1559
    DLX_CAP_XCHANGE 54,1593
    DLX_CAP_XCHANGE       = 0x16,54,1593
    DLX_CLOSE_PEER 55,1627
    DLX_CLOSE_PEER        = 0x17,55,1627
    DLX_CLOSE_PEER_ACK 56,1661
    DLX_CLOSE_PEER_ACK    = 0x18,56,1661
    DLX_ENTER_PEER_BUSY 57,1695
    DLX_ENTER_PEER_BUSY   = 0x19,57,1695
    DLX_EXIT_PEER_BUSY 58,1729
    DLX_EXIT_PEER_BUSY    = 0x1A,58,1729
    DLX_XCAST_FRAME 59,1763
    DLX_XCAST_FRAME       = 0x1B,59,1763
    DLX_FRAME_REJ 60,1797
    DLX_FRAME_REJ         = 0x1C,60,1797
    DLX_PEER_TEST_REQ 61,1831
    DLX_PEER_TEST_REQ     = 0x1D,61,1831
    DLX_PEER_TEST_RSP 62,1865
    DLX_PEER_TEST_RSP     = 0x1E,62,1865
    DLX_FAST_PASS 63,1899
    DLX_FAST_PASS         = 0x3063,1899
#define WRENCH_BIT_MASK 67,1957
#define DLX_PROTO_VERSION 68,1991
#define DLX_MAX_RIF 69,2062
#define DLX_HDR_SZ 71,2112
#define DLX_CUR_SZ 72,2162
#define DLX_ICR_SZ 73,2214
#define DLX_ICNR_SZ 74,2266
#define DLX_STARTDL_SZ 75,2319
#define DLX_DL_STARTED_SZ 76,2375
#define DLX_STARTDLFAILED_SZ 77,2433
#define DLX_CMD_FAILED_SZ 78,2495
#define DLX_DGRM_SZ 79,2554
struct dlx_header_ dlx_header_83,2635
typedef struct dlx_header_ dlx_header_t;dlx_header_t92,3139
struct dlx_cur_data_ dlx_cur_data_94,3181
typedef struct dlx_cur_data_ dlx_cur_data_t;dlx_cur_data_t99,3375
struct dlx_icr_data_ dlx_icr_data_101,3421
typedef struct dlx_icr_data_ dlx_icr_data_t;dlx_icr_data_t106,3676
struct dlx_icnr_data_ dlx_icnr_data_108,3722
typedef struct dlx_icnr_data_ dlx_icnr_data_t;dlx_icnr_data_t112,3911
struct dlx_startdl_data_ dlx_startdl_data_114,3959
typedef struct dlx_startdl_data_ dlx_startdl_data_t;dlx_startdl_data_t128,4334
struct dlx_dlstarted_data_ dlx_dlstarted_data_130,4388
typedef struct dlx_dlstarted_data_ dlx_dlstarted_data_t;dlx_dlstarted_data_t144,4769
struct dlx_startdlfailed_data_ dlx_startdlfailed_data_146,4827
typedef struct dlx_startdlfailed_data_ dlx_startdlfailed_data_t;dlx_startdlfailed_data_t157,5108
struct dlx_dgrm_data_ dlx_dgrm_data_159,5174
typedef struct dlx_dgrm_data_ dlx_dgrm_data_t;dlx_dgrm_data_t167,5363
struct dlx_closepeer_data_ dlx_closepeer_data_169,5411
typedef struct dlx_closepeer_data_ dlx_closepeer_data_t;dlx_closepeer_data_t174,5515
struct dlx_fast_hdr_ dlx_fast_hdr_176,5573
typedef struct dlx_fast_hdr_ dlx_fast_hdr_t;dlx_fast_hdr_t183,5943
enum dlx_border_peer_op_t dlx_border_peer_op_t189,6032
    DLX_MEMBER_TO_BP 190,6060
    DLX_MEMBER_TO_BP      = 0xA0,190,6060
    DLX_BP_TO_MEMBER 191,6094
    DLX_BP_TO_MEMBER      = 0xA1,191,6094
    DLX_BP_TO_BP 192,6128
    DLX_BP_TO_BP          = 0xA2,192,6128
    DLX_RELAY_RSP 193,6162
    DLX_RELAY_RSP         = 0xA3193,6162
typedef struct _dlx_bp_hdr_t _dlx_bp_hdr_t196,6199
#define BP_FLAG_POD_FST BP_FLAG_POD_FST203,6394
} dlx_bp_hdr_t;dlx_bp_hdr_t208,6544
#define is_dlx_csm_op(is_dlx_csm_op211,6574
#define is_dlx_bp_op(is_dlx_bp_op219,6988
#define DLX_FST_PROTO_VERSION 228,7218
#define DLX_FST_PASS_OP 229,7262
#define DLX_FST_VERS_AND_OP 230,7306
#define DLX_FST_SHORT_VERS_AND_OP 231,7350
