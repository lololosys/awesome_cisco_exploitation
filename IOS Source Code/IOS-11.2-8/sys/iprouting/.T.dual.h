
dual.h,8579
#define __DUAL_H__138,5375
#define EIGRP_REVISION_MAJOR_LEVEL 151,5770
#define EIGRP_REVISION_MINOR_LEVEL 152,5837
#define ACTIVE_METRIC_KLUDGE158,6030
#define DUAL_SPLIT_HORIZON_QUERIES163,6140
#define IGRP2_HELLOTIME	169,6286
#define IGRP2_HOLDTIME	170,6324
#define IGRP2_HELLOTIME_NBMA	171,6370
#define IGRP2_HOLDTIME_NBMA	172,6413
#define IGRP2_SLOW_VISIBLE_BANDWIDTH	173,6468
#define IGRP2_DEFAULT_LOG_ADJ_CHANGE 175,6520
#define IGRP2_PACKETIZATION_DELAY 184,6945
#define IGRP2_DEF_ACTIVE_TIME 186,7002
#define IGRP2_ACTIVE_SCAN_FREQUENCY 187,7045
#define IGRP2_JITTER_PCT	189,7091
#define IGRP2_MAX_QDEPTH	191,7154
#define IGRP2_ACL_CHANGE_DELAY 193,7218
#define IGRP2_DEFAULT_BANDWIDTH_PERCENT 195,7299
#define IGRP_TO_IGRP2_SHIFT	197,7380
#define IGRP2_INIT_RTO 202,7492
#define IGRP2_RTO_LOWER 203,7556
#define IGRP2_RTO_UPPER 204,7615
#define IGRP2_RTO_SRTT_MULTIPLIER 205,7680
#define IGRP2_VERSION 206,7750
#define IGRP2_RETRY_LIMIT 207,7779
#define IGRP2_RTO_PACING_MULTIPLIER 208,7809
#define IGRP2_FLOW_SRTT_MULTIPLIER 209,7880
#define IGRP2_MIN_MCAST_FLOW_DELAY 210,7955
#define IGRP2_MAX_PPS 211,8028
#define IGRP2_MIN_PACING_IVL 212,8094
#define IGRP2_NO_UNRELIABLE_DELAY_BW 217,8209
#define EIGRP_HOLDDOWN_DEFAULT 222,8340
#define IGRP2_PARAMETER 229,8596
#define IGRP2_AUTH 230,8630
#define IGRP2_SEQUENCE	231,8664
#define IGRP2_SW_VERSION 232,8697
#define IGRP2_NEXT_MCAST_SEQ 233,8731
typedef struct igrp2_sw_version_ igrp2_sw_version_238,8789
} igrp2_sw_version;igrp2_sw_version243,9032
#define IGRP2_SW_VERSION_SIZE 244,9052
#define IGRP2_MAX_SEQ_TLV 253,9456
#define HANDLE_MALLOC_SIZE(HANDLE_MALLOC_SIZE259,9628
#define BITSIZE_ULONG	260,9698
#define HANDLE_TO_CELL(HANDLE_TO_CELL261,9743
#define CELL_TO_HANDLE(CELL_TO_HANDLE262,9792
#define HANDLE_TO_BIT(HANDLE_TO_BIT263,9841
#define ULONG_HANDLE(ULONG_HANDLE264,9897
#define CLEAR_HANDLE(CLEAR_HANDLE265,9947
#define SET_HANDLE(SET_HANDLE266,10015
#define TEST_HANDLE(TEST_HANDLE267,10082
} igrp2_eventtype;igrp2_eventtype286,10702
#define DUAL_WORKQ_CHUNKSIZE 288,10722
enum {IGRP2_IIDB_PACING_TIMER,293,10815
enum {IGRP2_IIDB_PACING_TIMER, IGRP2_PEER_SEND_TIMER,293,10815
enum {IGRP2_IIDB_PACING_TIMER, IGRP2_PEER_SEND_TIMER, IGRP2_IIDB_HELLO_TIMER,293,10815
      IGRP2_PEER_HOLDING_TIMER,294,10893
      IGRP2_PEER_HOLDING_TIMER, IGRP2_FLOW_CONTROL_TIMER,294,10893
      IGRP2_PEER_HOLDING_TIMER, IGRP2_FLOW_CONTROL_TIMER, IGRP2_ACTIVE_TIMER,294,10893
      IGRP2_PACKETIZE_TIMER,295,10971
      IGRP2_PACKETIZE_TIMER, IGRP2_PDM_TIMER,295,10971
      IGRP2_PACKETIZE_TIMER, IGRP2_PDM_TIMER, IGRP2_IIDB_ACL_CHANGE_TIMER,295,10971
      IGRP2_DDB_ACL_CHANGE_TIMER}296,11046
#define IGRP_OPC_QUERY 302,11178
#define IGRP_OPC_REPLY 303,11207
#define IGRP_OPC_HELLO 304,11236
#define IGRP_OPC_IPXSAP 305,11265
#define IGRP_OPC_PROBE	306,11294
#define IGRP_OPC_ACK	307,11347
#define IGRP_NUM_OPCODES 308,11410
#define IGRP_OPC_ILLEGAL 309,11462
#define IGRP2TLMACRO 311,11497
#define IGRP2_TLV_HDR_LENGTH 314,11667
#define IGRP2METRICPAKMACRO 316,11721
typedef struct igrp2metric_ igrp2metric_328,12303
} igrp2metric;igrp2metric335,12445
typedef union igrp2nettype_ igrp2nettype_337,12461
} igrp2nettype;igrp2nettype347,12713
typedef union igrp2addrtype_ igrp2addrtype_349,12730
} igrp2addrtype;igrp2addrtype356,12925
typedef struct igrp2_xmit_anchor_ igrp2_xmit_anchor_365,13323
} igrp2_xmit_anchor;igrp2_xmit_anchor368,13471
#define IGRP2_MIN_ANCHOR 369,13492
typedef struct igrp2_xmit_thread_ igrp2_xmit_thread_398,15060
} igrp2_xmit_thread;igrp2_xmit_thread406,15534
#define DUAL_SERNO_MODULUS 407,15555
#define IGRP2_MIN_THREAD_DUMMIES 408,15605
    xmit_queue_type;xmit_queue_type420,16025
#define MAX_UNRELIABLE_PACKETS 427,16237
#define MAX_UNICAST_PACKETS 434,16445
struct dual_peertype_ dual_peertype_445,16760
} igrp2origintype;igrp2origintype489,19157
typedef struct dual_rdbtype_ dual_rdbtype_509,19943
} dual_rdbtype;dual_rdbtype529,21161
#define NO_PEER_HANDLE 531,21178
typedef struct igrp2_handle_type_ igrp2_handle_type_539,21481
} igrp2_handle_type;igrp2_handle_type543,21694
} dual_qo_t;dual_qo_t554,22100
typedef struct dual_ndbtype_ dual_ndbtype_568,22592
} dual_ndbtype;dual_ndbtype595,24220
#define DUAL_PASSIVE 600,24270
#define DUAL_ACTIVE 601,24294
typedef enum dual_rtev_type_ dual_rtev_type_606,24357
} dual_rtev_type;dual_rtev_type611,24583
#define DUAL_QOMASK 616,24664
#define DUAL_SENDREPLY 622,24836
#define DUAL_MULTQUERY 623,24895
#define DUAL_MULTUPDATE 624,24956
#define DUAL_MULTMASK 626,25019
#define DUAL_URCOUNT 628,25080
#define DUAL_MQCOUNT 629,25139
#define DUAL_MUCOUNT 630,25200
#define DUAL_MAXFLAG 631,25262
typedef struct igrp2type_ igrp2type_636,25331
} igrp2type;igrp2type644,25488
#define IGRP2_INIT_FLAG 651,25684
#define IGRP2_CR_FLAG 661,26068
typedef struct igrp2_idbtype_ igrp2_idbtype_678,26751
} igrp2_idbtype;igrp2_idbtype725,29841
typedef struct dual_newroute_ dual_newroute_733,30027
} dual_newroute;dual_newroute747,30747
typedef struct igrp2_pakdesc_ igrp2_pakdesc_774,31924
} igrp2_pakdesc;igrp2_pakdesc788,32685
#define IGRP2_MIN_PAKDESC 790,32703
typedef struct igrp2_pakdesc_qelm_ igrp2_pakdesc_qelm_799,33004
} igrp2_pakdesc_qelm;igrp2_pakdesc_qelm804,33297
#define IGRP2_MIN_PAKDESC_QELM 806,33320
typedef struct igrp2_input_packet_ igrp2_input_packet_816,33704
} igrp2_input_packet;igrp2_input_packet820,33903
#define IGRP2_MIN_INPAK_CHUNKS 822,33926
typedef struct igrp2_mibtype_ igrp2_mibtype_828,34135
} igrp2_mibtype;igrp2_mibtype839,34399
} dual_event_code;dual_event_code949,36693
#define DUAL_DEFAULT_MAX_EVENTS 951,36713
#define DUAL_EVENTS_BUFFER_SIZE 952,36781
#define DUAL_LOG_ALL(DUAL_LOG_ALL953,36816
#define DUAL_LOG(DUAL_LOG955,36902
} dual_etype_enum;dual_etype_enum972,37265
#define DUAL_LOGGING_DUAL_DEFAULT 976,37309
#define DUAL_LOGGING_XPORT_DEFAULT 977,37348
#define DUAL_LOGGING_XMIT_DEFAULT 978,37389
typedef struct dual_event_type_ dual_event_type_984,37559
} dual_event_type;dual_event_type990,37927
#define DUAL_SHOW_EVENT_FRAG_SIZE 999,38264
#define DUAL_SHOW_EVENT_ENTRY_COUNT 1000,38342
typedef struct dual_show_event_block_ dual_show_event_block_1002,38443
} dual_show_event_block;dual_show_event_block1005,38628
typedef struct dual_event_decode_ dual_event_decode_1008,38655
} dual_event_decode;dual_event_decode1011,38745
typedef struct dual_iidb_scratch_ dual_iidb_scratch_1018,38983
} dual_iidb_scratch;dual_iidb_scratch1021,39129
typedef enum advertise_request_type_ advertise_request_type_1026,39198
} advertise_request_type;advertise_request_type1030,39385
struct ddbtype_ ddbtype_1138,46048
#define DUAL_EXTDATA_CHUNKSIZE 1256,52937
#define DUAL_UPDATE	1261,53031
#define DUAL_QUERY	1262,53073
#define DUAL_REPLY	1263,53113
#define DUAL_RTABLE	1264,53159
#define DUAL_LINKDOWN	1265,53229
struct igrp2_idblink_ igrp2_idblink_1273,53461
typedef struct igrp2_gentlvtype_ igrp2_gentlvtype_1282,53725
} igrp2_gentlvtype;igrp2_gentlvtype1284,53777
#define IGRP2_EXTERNAL 1289,53831
#define IGRP2_CD 1290,53907
typedef struct igrp2_paramtype_ igrp2_paramtype_1292,53970
} igrp2_paramtype;igrp2_paramtype1301,54434
#define IGRP2_PARAM_TLV_LENGTH 1302,54453
typedef struct igrp2_versiontype_ igrp2_versiontype_1304,54512
} igrp2_versiontype;igrp2_versiontype1307,54595
#define IGRP2_VERSION_TLV_LENGTH 1308,54616
typedef struct igrp2_next_multi_seq_type_ igrp2_next_multi_seq_type_1318,55017
} igrp2_next_multi_seq_type;igrp2_next_multi_seq_type1321,55141
#define IGRP2_MULTISEQ_TLV_LENGTH 1322,55170
typedef struct igrp2_mdecode_ igrp2_mdecode_1329,55316
} igrp2_mdecode;igrp2_mdecode1331,55372
typedef struct igrp2_probe_type_ igrp2_probe_type_1336,55451
} igrp2_probe_type;igrp2_probe_type1338,55535
#define IGRP2_PROBE_SIZE 1339,55555
#define IGRP2HEADERBYTES 1342,55606
#define MAXIGRP2BYTES 1343,55649
#define IGRP2HELLOHDRBYTES 1344,55679
#define IGRP2HELLOHOOKBYTEKLUDGE 1349,55870
#define IGRP2_KBYTES 1350,55948
#define IGRP2_OOS_MAX 1357,56199
#define IGRP2_INIT_HELLO_DELAY 1364,56319
typedef void  (*igrp2_peerfunction)igrp2_peerfunction1376,56632
typedef void  (*igrp2_peerfunction)(struct ddbtype_ *, dual_peertype dual_peertype1376,56632
#define IGRP2_FBUGINF 1498,62616
#define IGRP2_FTARGET(IGRP2_FTARGET1499,62667
#define IGRP2_FABUGINF(IGRP2_FABUGINF1500,62805
#define IGRP2_XBUGINF(IGRP2_XBUGINF1501,62870
