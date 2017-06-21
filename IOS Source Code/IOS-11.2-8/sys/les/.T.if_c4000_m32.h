
if_c4000_m32.h,7565
#define __IF_C4000_M32_H__82,3310
#define M32_NUM_CHANNELS 93,3496
#define M32_NUM_TIMESLOTS 94,3524
#define M32_MAX_FRAME_SIZE 95,3553
#define M32_LONG_WORD_ALIGN 96,3586
#define M32_IDLE_CHNL_UNASSIGNED 97,3616
enum m32_channel_state_t m32_channel_state_t102,3718
    M32_CHANNEL_ILLEGAL 103,3745
    M32_CHANNEL_UNUSED,105,3887
    M32_CHANNEL_IDLE,106,3946
    M32_CHANNEL_DOWN,108,4082
    M32_CHANNEL_STANDBY,110,4200
    M32_CHANNEL_UP,112,4347
    M32_CHANNEL_THROTTLED 113,4416
#define M32_AR_ALL_CHANNELS 120,4560
#define M32_AR_SPIN_WAIT_ITERATIONS 125,4702
#define M32_AR_SPIN_WAIT_USEC 126,4743
#define M32_AR_PCM_T1 130,4833
#define M32_AR_PCM_E1 131,4869
#define M32_AR_IN 132,4905
#define M32_AR_ICO 133,4941
#define M32_AR_RES 134,4977
#define M32_AR_EXTERNAL_LINE_LOOP 135,5013
#define M32_AR_EXTERNAL_CHANNEL_LOOP 136,5060
#define M32_AR_INTERNAL_LINE_LOOP 137,5107
#define M32_AR_INTERNAL_CHANNEL_LOOP 138,5154
#define M32_AR_IA 139,5201
#define M32_AR_MFL_SHIFT 140,5237
#define M32_AR_MFL 141,5265
#define M32_AR_CHANNEL_SHIFT 142,5325
#define M32_AR_BASE_MASK 146,5412
#define M32_AR_ONE_CHNL_MASK 147,5451
#define M32_AR_ALL_CHNL_MASK 148,5512
#define M32_AR_STARTUP_MASK 149,5574
#define M32_REG_INT_DISABLE 154,5705
#define M32_REG_INT_ENABLE 155,5737
#define M32_REG_INT_CLEAR 156,5769
#define M32_REG_INT_PENDING 157,5801
#define M32_INTQ_BLOCKS 164,6005
#define M32_INTQ_ELEMENTS 165,6035
#define M32_INTS_TO_PROCESS 171,6173
#define M32_INT_INT 176,6274
#define M32_INT_VERSION 177,6305
#define M32_INT_ARACK 178,6340
#define M32_INT_ARF 179,6371
#define M32_INT_HI 180,6402
#define M32_INT_FI 181,6433
#define M32_INT_IFC 182,6464
#define M32_INT_SF 183,6495
#define M32_INT_ERR 184,6526
#define M32_INT_FO 185,6557
#define M32_INT_RT 186,6588
#define M32_INT_CHANNEL 187,6619
#define M32_INT_VER_SHIFT 189,6651
#define M32_INT_VER_21 190,6680
#define M32_INT_VER_22 191,6708
#define M32_INT_VER_32 192,6736
#define M32_INT_VER_33 193,6764
#define M32_INT_RT_XMIT 197,6846
#define M32_INT_RT_RECV 198,6875
#define M32_BITRATE_DIVISOR 202,6958
#define M32_MAX_FILLMASK 203,6988
#define M32_TIMESLOT_USE_MSB 210,7267
#define M32_TIMESLOT_USE_LSB 211,7298
#define M32_TIMESLOT_CHANNEL 215,7380
#define M32_TIMESLOT_BITRATE 216,7416
#define M32_TIMESLOT_INHIBIT 217,7452
#define M32_TIMESLOT_CHNL_SHIFT 218,7488
#define M32_TIMESLOT_BITRATE_64KBPS 219,7522
#define M32_CHANNEL_INT_MASK 225,7724
#define M32_CHANNEL_NITBS 226,7764
#define M32_CHANNEL_CS 227,7804
#define M32_CHANNEL_CRC 228,7844
#define M32_CHANNEL_FA 229,7884
#define M32_CHANNEL_MODE 230,7924
#define M32_CHANNEL_IFTF 231,7964
#define M32_CHANNEL_TMA_MASK 232,8004
#define M32_CHANNEL_TX_RX_CMD_MASK 236,8119
#define M32_CHANNEL_ABORT_CMD 237,8188
#define M32_CHANNEL_CLEAR_CMD 238,8253
#define M32_CHANNEL_INIT_CMD 239,8318
#define M32_CHANNEL_OFF_CMD 240,8382
#define M32_CHANNEL_BASE_MASK 245,8521
#define M32_CHANNEL_INIT_MASK 246,8593
#define M32_CHANNEL_ABORT_MASK 247,8690
#define M32_CHANNEL_CLEAR_MASK 248,8769
#define M32_CHANNEL_DOWN_MASK 249,8848
#define M32_RECV_DESC_HOLD 254,8975
#define M32_RECV_DESC_HI 255,9015
#define M32_RECV_DESC_NO 256,9055
#define M32_RECV_DESC_NO_SHIFT 257,9095
#define M32_RECV_DESC_FE 261,9179
#define M32_RECV_DESC_C 262,9219
#define M32_RECV_DESC_BNO 263,9259
#define M32_RECV_DESC_STATUS 264,9299
#define M32_RECV_DESC_BNO_SHIFT 265,9339
#define M32_RECV_STATUS_SF 271,9502
#define M32_RECV_STATUS_LOSS 272,9538
#define M32_RECV_STATUS_CRC 273,9574
#define M32_RECV_STATUS_NOB 274,9610
#define M32_RECV_STATUS_LFD 275,9646
#define M32_RECV_STATUS_RA 276,9682
#define M32_RECV_STATUS_ROF 277,9718
#define M32_RECV_PARAMS_INITIAL 279,9755
#define M32_RECV_STATUS_INITIAL 280,9804
#define M32_XMIT_DESC_FE 284,9886
#define M32_XMIT_DESC_HOLD 285,9927
#define M32_XMIT_DESC_HI 286,9968
#define M32_XMIT_DESC_NO 287,10009
#define M32_XMIT_DESC_CSM 288,10050
#define M32_XMIT_DESC_FNUM 289,10091
#define M32_XMIT_DESC_FNUM_MAX 290,10132
#define M32_XMIT_DESC_NO_SHIFT 291,10173
#define M32_MAX_INTERFRAME_FLAGS 292,10207
#define M32_XMIT_DESC_INITIAL 294,10244
#define M32_HOST_INT 302,10581
#define M32_AR_INT 303,10606
#define M32_CLEAR_HOST_INT 304,10631
#define M32_CLEAR_AR_INT 305,10674
#define M32_RUNT_THRESHOLD 308,10717
#define M32_ERR_HIGH_MARK 310,10857
#define M32_INTERVAL 312,10991
#define CHAN_INTERVAL 314,11138
#define M32_ERR_LOW_MARK 316,11288
#define REM_LOOP_PATTERN_MATCHED(REM_LOOP_PATTERN_MATCHED322,11505
#define REM_LOOP_TIP_BYTES 323,11568
#define REM_LOOP_SELECT 324,11606
#define REM_LOOP_LBE_BYTES 325,11644
#define REM_LOOP_FEV_BYTES 326,11682
#define REM_LOOPUP_TIP_CNT 328,11722
#define REM_LOOPUP_SELECT_CNT 329,11756
#define REM_LOOPUP_LBE_CNT 330,11790
#define REM_LOOPUP_FEV_COUNT 331,11824
#define REM_LOOPDOWN_TIP_CNT 332,11896
#define REM_LOOPUP_PREAMBLE_CNT 333,11968
#define REM_LOOP_FRAME_CNT 335,12064
enum m32_loopback_t m32_loopback_t340,12147
    M32_NO_LOOPBACK 341,12169
    M32_LOCAL_LOOPBACK,342,12194
    M32_REMOTE_LOOPBACK_WAIT_UP,343,12218
    M32_REMOTE_LOOPBACK_ACTIVE,344,12251
    M32_REMOTE_LOOPBACK_WAIT_DOWN,345,12283
enum m32_error_t m32_error_t351,12373
    M32_NOERR 352,12392
    M32_ERR_NO_TIMESLOT,353,12411
    M32_ERR_TIMESLOT_IN_USE,354,12436
    M32_ERR_ILLEGAL_CHANNEL,355,12465
    M32_ERR_AR_TIMEOUT,356,12494
    M32_ERR_AR_FAILURE,357,12518
    M32_ERR_BAD_STATE,358,12542
    M32_ERR_BAD_BITRATE,359,12565
    M32_ERR_BAD_INTQ,360,12590
    M32_ERR_MEM_ALLOC361,12612
typedef struct m32_intq m32_intq_t;m32_intq_t383,13159
struct m32_intq m32_intq384,13195
typedef struct m32_timeslot m32_timeslot_t;m32_timeslot_t395,13457
struct m32_timeslot m32_timeslot396,13501
typedef struct m32_recv_desc m32_recv_desc_t;m32_recv_desc_t404,13633
struct m32_recv_desc m32_recv_desc405,13679
typedef struct m32_xmit_desc m32_xmit_desc_t;m32_xmit_desc_t412,13816
struct m32_xmit_desc m32_xmit_desc413,13862
typedef struct m32_channel_ctrl m32_channel_ctrl_t;m32_channel_ctrl_t422,14036
struct m32_channel_ctrl m32_channel_ctrl423,14088
typedef struct m32_control m32_control_t;m32_control_t435,14457
struct m32_control m32_control436,14499
typedef struct m32_pak_ring m32_pak_ring_t;m32_pak_ring_t473,15571
struct m32_pak_ring m32_pak_ring474,15615
typedef void (*m32_recv_handler_t)m32_recv_handler_t482,15739
typedef void (*m32_recv_handler_t)(hwidbtype *, paktype paktype482,15739
typedef void (*m32_xmit_handler_t)m32_xmit_handler_t483,15799
typedef void (*m32_xmit_handler_t)(hwidbtype *, paktype paktype483,15799
typedef void (*m32_disable_adaptor_t)m32_disable_adaptor_t484,15859
typedef void (*m32_disable_adaptor_t)(hwidbtype hwidbtype484,15859
typedef void (*m32_enable_adaptor_t)m32_enable_adaptor_t485,15911
typedef void (*m32_enable_adaptor_t)(hwidbtype hwidbtype485,15911
typedef void (*m32_down_connection_t)m32_down_connection_t486,15962
typedef void (*m32_down_connection_t)(hwidbtype hwidbtype486,15962
typedef struct m32_channel_params m32_channel_params_t;m32_channel_params_t492,16070
struct m32_channel_params m32_channel_params493,16126
typedef struct m32_channel_init_info m32_channel_init_info_t;m32_channel_init_info_t597,19645
struct m32_channel_init_info m32_channel_init_info598,19707
typedef struct m32_regs m32_regs_t;m32_regs_t623,20839
struct m32_regs m32_regs624,20875
struct m32_instance m32_instance636,21261
