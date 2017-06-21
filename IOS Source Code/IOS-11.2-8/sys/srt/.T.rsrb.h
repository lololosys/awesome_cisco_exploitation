
rsrb.h,7102
#define __RSRB_H__207,8670
#define RSRB_INVALID_ADDRESS 213,8763
#define MAXDGRAM_RSRB 215,8813
#define FST_IP_HDR_SIZE 216,8881
#define RSRB_DROP 217,8908
#define RSRB_TCP_WINDOW_SIZE	218,8935
#define RSRB_MAX_STARTQ 219,9004
#define RSRB_MAXRINGS 222,9120
#define VRING_VISIBLE_BANDWIDTH 225,9190
#define RSRB_VERSION	233,9340
#define RSRB_CAP_VERSION	234,9364
#define RSRB_LACK_BC_VERSION	235,9431
#define RSRB_PREV_VERSION	236,9462
#define RSRB_MIN_VERSION	237,9490
#define RSRB_NO_VERSION	238,9517
#define RSRB_OUR_CAPS	240,9545
#define RSRB_CHECK_VER	248,9659
#define RSRB_DONT_CHECK_VER	249,9688
#define RSRB_RIFLF_KNOCKDOWN 251,9723
#define RSRB_RIFLF_LEAVEALONE 252,9760
#define ABORT_ON_FAIL	258,9825
#define DONT_ABORT_ON_FAIL	259,9853
typedef struct _tr_vhdrtype _tr_vhdrtype261,9887
} tr_vhdrtype;tr_vhdrtype269,10275
#define TR_VHDRTYPE_SIZE	271,10291
typedef struct _tr_fst_vhdrtype _tr_fst_vhdrtype277,10539
} tr_fst_vhdrtype;tr_fst_vhdrtype283,10764
#define TR_FST_VR_HDR_SIZE	285,10784
#define RSRB_OP_NONE	292,10903
#define RSRB_OP_FORWARD	293,10949
#define RSRB_OP_EXPLORER	294,11015
#define RSRB_OP_RINGXREQ	295,11057
#define RSRB_OP_RINGXREPLY 296,11124
#define RSRB_OP_NOP	297,11190
#define RSRB_OP_VERSIONREQ	298,11251
#define RSRB_OP_VERSIONREPLY	299,11313
#define RSRB_OP_VERSIONXREQ 300,11374
#define RSRB_OP_FORWARDCRC	301,11438
#define RSRB_OP_LINKUP_REQ 302,11504
#define RSRB_OP_LINKDOWN_REQ 303,11578
#define RSRB_OP_LLC2DATA 304,11653
#define RSRB_OP_LINKUP_PRSP 305,11727
#define RSRB_OP_LINKDOWN_PRSP 306,11800
#define RSRB_OP_SDLLC 307,11875
#define RSRB_OP_FORWARDFST 308,11934
#define RSRB_OP_FORWARDFSTCRC 309,12007
#define RSRB_OP_QUENCH_ON 310,12080
#define RSRB_OP_QUENCH_OFF 311,12151
#define RSRB_OP_ConnectRequest 317,12368
#define RSRB_OP_ConnectPollReq 318,12437
#define RSRB_OP_ConnectPollAck 319,12514
#define RSRB_OP_ConnectPollAckAndReq 320,12586
#define RSRB_OP_ConnectWaitAck 321,12670
#define RSRB_OP_DisconnectRequest 323,12781
#define RSRB_OP_DisconnectAck 324,12867
#define RSRB_OP_CAPS_REQ	325,12945
#define RSRB_OP_CAPS_REPLY	326,13004
#define RSRB_OP_LA2_QON	327,13063
#define RSRB_OP_LA2_QON_ACK 328,13126
#define RSRB_OP_LA2_QOFF 329,13208
#define RSRB_OP_LA2_QOFF_ACK 330,13274
#define RSRB_OP_SEND_XID	336,13389
#define RSRB_OP_BPDU	339,13501
#define RSRB_OP_SPAN 349,14017
#define RSRB_OP_QLLC	351,14052
#define RSRB_OP_TR_LANE	353,14122
#define RSRB_OP_IGNORE	359,14214
#define RSRB_OP_EMPTY	360,14266
#define RSRB_OP_ERR	368,14437
#define RSRB_OP_UNKVRN	369,14501
#define RSRB_OP_UNKPEER	370,14557
#define RSRB_OP_UNKTRN	371,14615
#define RSRB_OP_BADOP	372,14672
#define RSRB_OP_UNKVER	373,14723
#define RSRB_OP_UNKCAP	375,14789
#define RSRB_BN_MASK	379,14935
#define RSRB_BN_SDLLC	380,14998
#define RSRB_BN_QLLC	381,15056
#define RSRB_BN_TR_LANE	382,15112
#define RSRB_RN_DONE	383,15186
typedef struct _tr_vrxtype_1_ _tr_vrxtype_1_387,15337
} tr_vrxtype_1;tr_vrxtype_1390,15399
typedef struct _tr_vrxtype_2_ _tr_vrxtype_2_394,15490
} tr_vrxtype_2;tr_vrxtype_2398,15574
struct tr_vgrouptype_ tr_vgrouptype_423,16649
#define FOR_ALL_RING_GROUPS(FOR_ALL_RING_GROUPS445,17419
#define VRGF_GENUPDATE	448,17532
#define VRGF_FORCEXCHG	449,17590
typedef boolean (*vre_les_fastout_type)vre_les_fastout_type451,17654
typedef boolean (*vre_les_fastout_type)(hwidbtype *, hwidbtype *, paktype paktype451,17654
typedef boolean (*vre_hes_fastout_type)vre_hes_fastout_type453,17742
					ulong *, int, hwidbtype hwidbtype454,17822
struct tr_ventrytype_ tr_ventrytype_456,17856
typedef struct _srb_local_ring_type _srb_local_ring_type480,18614
} srb_local_ring_type;srb_local_ring_type483,18711
#define RSRB_ET_EITHER	486,18736
#define RSRB_ET_LOCAL	487,18796
#define RSRB_ET_REMOTE	488,18826
#define RSRB_ET_REMSTATIC	489,18857
#define RSRB_ET_SDLLC 490,18890
#define RSRB_ET_REMSDLLC	491,18929
#define RSRB_ET_QLLC 492,18961
#define RSRB_ET_REMQLLC	493,19000
#define RSRB_ET_LOCAL_VIRTUAL	494,19032
#define RSRB_ET_TR_LANE 495,19069
#define RSRB_ET_REMTR_LANE	496,19108
#define RSRB_ET_USESMAC	501,19208
#define RSRB_BN_DONTCARE	509,19399
typedef enum _tr_vpeer_statetype _tr_vpeer_statetype517,19498
} tr_vpeer_statetype;tr_vpeer_statetype527,19990
#define RSRB_CONNECT_WINDOW	537,20359
#define RSRB_INITIAL_START_WINDOW 538,20399
#define RSRB_SERIAL_MAXBYTES	550,20861
#define RSRB_FST_PACKET_SIZE 557,21073
#define RSRB_FST_FORWARD_PACKET_SIZE 560,21225
#define MAX_PEER_NAME 565,21384
typedef enum _tr_vpeer_typetype _tr_vpeer_typetype633,24191
} tr_vpeer_typetype;tr_vpeer_typetype640,24522
typedef enum _vpeer_connrtn _vpeer_connrtn642,24544
} vpeer_connrtn;vpeer_connrtn646,24743
#define RSRB_SEQNUM_RESTART_WINDOW 671,25759
#define RSRB_SEQNUM_OUT_OF_ORDER_WINDOW 672,25799
#define RSRB_SEQNUM_PROCESS_BASE 674,25845
#define RSRB_SEQNUM_PROCESS_TOP 675,25882
#define RSRB_SEQNUM_PROCESS_RESTART 676,25923
#define RSRB_SEQNUM_INTERRUPT_BASE 679,26027
#define RSRB_SEQNUM_INTERRUPT_TOP 680,26070
#define RSRB_SEQNUM_INTERRUPT_RESTART 681,26112
#define RSRB_FST_RESET_SEQUENCE_NUMBERS(RSRB_FST_RESET_SEQUENCE_NUMBERS692,26443
#define RSRB_FST_INCREMENT_SEND(RSRB_FST_INCREMENT_SEND704,26876
#define RSRB_FST_ACCEPTOR(RSRB_FST_ACCEPTOR733,27900
#define RSRB_PRIORITY_COUNT 801,30163
typedef int (*fastlanout_type)fastlanout_type803,30194
                      ushort, paktype paktype804,30271
typedef int (*hes_fastlanout_type)hes_fastlanout_type805,30313
		      ushort, uchar uchar806,30394
typedef int (*les_fastlanout_type)les_fastlanout_type807,30425
		      ushort, paktype paktype808,30506
struct tr_vpeertype_ tr_vpeertype_810,30540
#define RSRB_BACKUP_GROUP_NONE	900,34347
#define BACKUP_GROUP_COUNT 901,34392
#define RSRB_MAX_OPEN_TRIES	903,34436
#define RSRB_REMOPEN_DEFTMO	904,34466
#define RSRB_KEEPALIVE_DEFAULT_INTERVAL	910,34549
#define RSRB_KEEPALIVE_TX_PERIOD	911,34603
#define RSRB_KEEPALIVE_MISSES	912,34650
#define RSRB_TCP_MAX_QUEUE_DEFAULT 921,35001
#define RSRB_TCP_MAX_UI_FRAME_OVERQUEUE 934,35616
#define RSRB_LACK_CONNECTION_TIMEOUT_DEFAULT 939,35778
typedef struct _tr_starttype _tr_starttype958,36573
} tr_starttype;tr_starttype961,36654
typedef struct _tr_conntype _tr_conntype974,37195
} tr_conntype;tr_conntype978,37363
#define DEFAULT_EXPLORERQ_DEPTH 985,37457
#define MAX_SRB_EXPLORERQ_TIME 988,37562
#define MAXRATE_PERIOD 991,37673
#define EXPLORER_BUF_SIZE 994,37751
#define DEFAULT_FS_MAXRATE_HES 999,37897
#define DEFAULT_FS_MAXRATE_LES 1000,37937
#define fs_clear_counts(fs_clear_counts1003,38040
#define LACK_TCPQ_THRESHOLD_HIGH 1010,38445
#define LACK_TCPQ_THRESHOLD_LOW 1011,38482
typedef struct rsrb_fr_ rsrb_fr_1016,38570
} rsrb_fr_t;rsrb_fr_t1019,38669
typedef union rsrb_cookie_ rsrb_cookie_1021,38683
} rsrb_cookie;rsrb_cookie1025,38820
enum { LLC_TRC_UFRAME,1027,38836
enum { LLC_TRC_UFRAME, LLC_TRC_SFRAME,1027,38836
enum { LLC_TRC_UFRAME, LLC_TRC_SFRAME, LLC_TRACE_IFRAME 1027,38836
