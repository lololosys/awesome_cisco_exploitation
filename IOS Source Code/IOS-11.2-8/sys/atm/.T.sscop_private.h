
sscop_private.h,5059
#define __SSCOP_PRIV_H__79,2791
typedef enum _sscop_statetype _sscop_statetype86,2843
} sscop_statetype;sscop_statetype98,3648
#define SSCOP_MT_BGN 104,3701
#define SSCOP_MT_BGAK 105,3756
#define SSCOP_MT_END 106,3823
#define SSCOP_MT_ENDAK 107,3876
#define SSCOP_MT_RS 108,3941
#define SSCOP_MT_RSAK 109,4008
#define SSCOP_MT_BGREJ 110,4090
#define SSCOP_MT_SD 111,4152
#define SSCOP_MT_SDP 112,4216
#define SSCOP_MT_POLL 113,4290
#define SSCOP_MT_STAT 114,4359
#define SSCOP_MT_USTAT 115,4434
#define SSCOP_MT_UD 116,4511
#define SSCOP_MT_MD 117,4576
#define SSCOP_MT_ER 118,4641
#define SSCOP_MT_ERAK 119,4721
#define MAA_ERROR_A 126,4843
#define MAA_ERROR_B 127,4871
#define MAA_ERROR_C 128,4899
#define MAA_ERROR_D 129,4927
#define MAA_ERROR_E 130,4955
#define MAA_ERROR_F 131,4983
#define MAA_ERROR_G 132,5011
#define MAA_ERROR_H 133,5039
#define MAA_ERROR_I 134,5067
#define MAA_ERROR_J 135,5095
#define MAA_ERROR_K 136,5124
#define MAA_ERROR_L 137,5153
#define MAA_ERROR_M 138,5182
#define MAA_ERROR_O 139,5211
#define MAA_ERROR_P 140,5240
#define MAA_ERROR_Q 141,5269
#define MAA_ERROR_R 142,5298
#define MAA_ERROR_S 143,5327
#define MAA_ERROR_T 144,5356
#define MAA_ERROR_U 145,5385
#define MAA_ERROR_V 146,5414
#define MAA_ERROR_W 147,5443
#define MAA_ERROR_X 148,5472
typedef enum _sscop_events _sscop_events154,5548
} sscop_events;sscop_events179,7072
#define SSCOP_POLLTIMER_DEFAULT 185,7129
#define SSCOP_KEEPALIVETIMER_DEFAULT 186,7182
#define SSCOP_CCTIMER_DEFAULT 187,7240
#define SSCOP_MAXCC_DEFAULT 188,7293
#define SSCOP_MAXRETRY_CNT_DEFAULT 189,7340
#define SSCOP_SENDWINDOW_DEFAULT 190,7387
#define SSCOP_RCVWINDOW_DEFAULT 191,7434
#define SSCOP_MAX_LIST_ELEMENTS 192,7481
#define SSCOP_IDLE_DEFAULT 193,7528
#define SSCOP_NORESPONSE_DEFAULT 194,7586
#define SSCOP_KEEPALIVE_V31_DEFAULT 195,7644
#define SSCOP_INIT_VRMR(SSCOP_INIT_VRMR199,7728
#define SSCOP_INC_VTSQ(SSCOP_INC_VTSQ200,7802
#define SSCOP_HEADER_START 205,7914
#define SSCOP_PADBYTES 207,8002
#define SSCOP_PADBYTE_MASK 208,8036
#define SSCOP_PDUTYPE_MASK 209,8079
#define SSCOP_SEQNUM_MASK 210,8122
#define SSCOP_RESERVED 211,8165
#define SSCOP_RESULT_OK 212,8208
#define SSCOP_NSQ_MASK 213,8245
#define SSCOP_SBIT_MASK 214,8288
#define SSCOP_SBIT_USER 215,8331
#define SSCOP_SBIT_SCP 216,8365
#define SSCOP_PDU_RETRANSMIT 217,8447
#define SSCOP_PDU_BAD 218,8482
#define SSCOP_PDU_NEW 219,8517
#define SSCOP_MAX_LIST_ELEMENTS 223,8603
#define SSCOP_STATS(SSCOP_STATS225,8643
#define SSCOP_INCR_CNTR(SSCOP_INCR_CNTR228,8704
typedef struct sscopStats_ sscopStats_234,8803
} sscopStats;sscopStats274,11485
struct sscopType_ sscopType_282,11612
#define SSCOP_CTB_NOT_INUSE	343,14974
#define SSCOP_CTB_INUSE 344,15040
#define SSCOP_BEGINREJ_MAX 345,15108
typedef struct sscop_beginPdu_ sscop_beginPdu_361,15409
} sscop_beginPdu;sscop_beginPdu364,15555
typedef struct sscop_beginAckPdu_ sscop_beginAckPdu_371,15600
} sscop_beginAckPdu;sscop_beginAckPdu374,15752
typedef struct sscop_beginRejPdu_ sscop_beginRejPdu_380,15799
} sscop_beginRejPdu;sscop_beginRejPdu383,15956
typedef struct sscop_sdPdu_ sscop_sdPdu_389,16008
} sscop_sdPdu;sscop_sdPdu391,16111
typedef struct sscop_sdpPdu_ sscop_sdpPdu_397,16167
} sscop_sdpPdu;sscop_sdpPdu400,16331
typedef struct sscop_pollPdu_ sscop_pollPdu_406,16368
} sscop_pollPdu;sscop_pollPdu409,16521
typedef struct sscop_resyncPdu_ sscop_resyncPdu_415,16561
} sscop_resyncPdu;sscop_resyncPdu418,16680
typedef struct sscop_resynsAckPdu_ sscop_resynsAckPdu_424,16737
} sscop_resyncAckPdu;sscop_resyncAckPdu426,16836
typedef struct sscop_resyncAck31Pdu_ sscop_resyncAck31Pdu_429,16860
} sscop_resyncAck31Pdu;sscop_resyncAck31Pdu432,16973
typedef struct sscop_statPdu_ sscop_statPdu_439,17020
} sscop_statPdu;sscop_statPdu443,17177
typedef struct sscop_ustatPdu_ sscop_ustatPdu_449,17227
} sscop_ustatPdu;sscop_ustatPdu454,17516
typedef struct sscop_endPdu_ sscop_endPdu_460,17555
} sscop_endPdu;sscop_endPdu463,17707
typedef struct sscop_endAckPdu_ sscop_endAckPdu_469,17758
} sscop_endAckPdu;sscop_endAckPdu471,17854
typedef struct sscop_endAck31Pdu_ sscop_endAck31Pdu_474,17875
} sscop_endAck31Pdu;sscop_endAck31Pdu477,18030
typedef struct sscop_errRecPdu_ sscop_errRecPdu_484,18118
} sscop_errRecPdu;sscop_errRecPdu487,18185
typedef struct sscop_errRecAckPdu_ sscop_errRecAckPdu_495,18276
} sscop_errRecAckPdu;sscop_errRecAckPdu498,18346
typedef struct sscop_mgmtPdu_ sscop_mgmtPdu_505,18396
} sscop_mgmtPdu;sscop_mgmtPdu507,18502
typedef struct sscop_udPdu_ sscop_udPdu_513,18557
} sscop_udPdu;sscop_udPdu515,18661
typedef struct sscop_list_ sscop_list_521,18712
} sscop_list;sscop_list523,18792
enum SSCOP_TIMERS SSCOP_TIMERS531,18841
  SSCOP_KEEPALIVE_TIMER,532,18861
  SSCOP_NO_RESPONSE_TIMER,533,18886
  SSCOP_IDLE_TIMER,534,18913
  SSCOP_POLL_TIMER,535,18933
  SSCOP_KEEPALIVE_TIMER_V31,536,18953
  SSCOP_ATMSIG_T309,537,18982
  SSCOP_RETRY_TIMER538,19046
