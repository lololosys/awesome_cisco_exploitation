
ncia.h,3354
#define NCIA_PROTO_VERSION 69,2272
#define NCIA_HASH_SIZE 70,2310
#define NCIA_HASH_MASK 71,2378
#define NCIA_TCP_WINDOW_SIZE 72,2435
#define NCIA_TCP_MAX_QUEUE_DEFAULT 73,2475
#define NCIA_LOCAL_RING 74,2515
#define NCIA_BRIDGE 75,2554
#define NCIA_TARGET_RING 76,2592
#define NCIA_SAP_IN_USE_BY_BRIDGE 77,2631
#define NCIA_GENERAL_TIMEOUT 79,2673
#define NCIA_SERVER_CONFIGURED 82,2737
#define NCIA_SERVER_ACTIVE 83,2818
#define NCIA_SERVER_STOP 84,2899
#define NCIA_INBOUND_ONLY 85,2980
#define MAX_TIMEOUT 88,3063
#define NCIA_MAX_SAP 89,3093
#define NCIA_DLSW_SAP 90,3123
#define NCIA_DEFAULT_NDLC_KEEPALIVE 92,3154
#define NCIA_DEFAULT_TCP_KEEPALIVE 93,3210
#define  NCIA_SNA_ONLY 95,3267
#define  NCIA_SNA_NETBIOS 96,3297
struct NciaSapT_ NciaSapT_104,3382
struct NciaCepT_ NciaCepT_114,3564
struct ncia_servertype_ ncia_servertype_154,4741
#define NCIA_RSRB_UP 176,5523
typedef struct ncia_rsrb_ ncia_rsrb_179,5572
} ncia_rsrb_t;ncia_rsrb_t188,5883
typedef enum nciaOpenFailureCode_nciaOpenFailureCode_192,5901
} nciaOpenFailureCode;nciaOpenFailureCode197,6030
enum NciaTimerType NciaTimerType200,6055
    NCIA_CIRCUIT_TIMER,201,6076
    NCIA_CLIENT_TIMER202,6100
#define NciaCircuitDebugEVENT 209,6156
#define NciaCircuitDebugSTATE 210,6199
#define NciaCircuitDebugFLOW 211,6242
#define NciaCircuitDebugERROR 212,6285
#define NciaCircuitDebugALL 214,6329
#define NciaUsedBySAP 217,6492
#define NciaTestRspPending 218,6535
#define NciaXidRspPending 220,6579
#define NciaClientBusy 221,6622
#define NciaClsBusy 223,6776
#define NciaTcpBusy 224,6856
#define NciaSendFCA 226,7011
#define NciaFreeCkt 227,7092
#define NEXT_SERVER 235,7382
#define NEXT_CLIENT 236,7440
#define NEXT_CIRCUIT 237,7497
#define ForEachNciaServer(ForEachNciaServer240,7550
#define ForEachNciaClientHashQueue 247,7797
#define ForEachNciaClient(ForEachNciaClient251,7943
#define ForEachNciaCircuit(ForEachNciaCircuit259,8253
#define NCIA_FCI 266,8528
#define NCIA_FCA 267,8555
#define NCIA_FCO_MASK 268,8582
#define NciaRepeatWindowOp 271,8611
#define NciaIncrementWindowOp 272,8646
#define NciaDecrementWindowOp 273,8681
#define NciaResetWindowOp 274,8716
#define NciaHalveWindowOp 275,8751
#define NCIA_MAX_GRANTED_PACKET_SIZE 278,8840
#define NCIA_INIT_WINDOW_SIZE 279,8880
#define NCIA_MAX_WINDOW 280,8919
#define IEEE_LFMASK 283,8961
#define IEEE_LF516 284,8992
#define IEEE_LF1470 285,9023
#define IEEE_LF2052 286,9054
#define IEEE_LF4399 287,9085
#define IEEE_LF8130 288,9116
#define IEEE_LF11407 289,9147
#define IEEE_LF17749 290,9178
#define IEEE_LFINITIAL 291,9209
#define IEEE_LF516_VAL 293,9242
#define IEEE_LF1470_VAL 294,9272
#define IEEE_LF2052_VAL 295,9303
#define IEEE_LF4399_VAL 296,9334
#define IEEE_LF8130_VAL 297,9365
#define IEEE_LF11407_VAL 298,9396
#define IEEE_LF17749_VAL 299,9428
#define IEEE_LF_INFINITY 300,9460
#define NCIA_MTU 305,9574
#define NCIA_RSRB 310,9634
#define CONFIG_NCIA_SERVER 311,9668
#define CONFIG_NCIA_CLIENT 312,9702
#define DEBUG_NCIA 316,9764
#define EXEC_NCIA_STOP 319,9800
#define EXEC_NCIA_START 320,9834
#define NDLC_DATA 323,9870
#define NCIA_DONT_CARE 325,9912
#define NCIA_FREE_PAK(NCIA_FREE_PAK328,9948
#define NCIA_CHECK_SERVER_IS_UP(NCIA_CHECK_SERVER_IS_UP335,10208
inline int ncia_print_frame_size 344,10529
