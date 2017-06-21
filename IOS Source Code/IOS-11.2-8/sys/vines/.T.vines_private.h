
vines_private.h,6889
#define NO_INLINES97,3568
#define STATIC 100,3607
#define INLINE 101,3629
#define STATIC103,3657
#define INLINE104,3672
#define ONETICK 155,4900
#define VRTP_LAST 156,4952
#define VRTP55_LAST 157,4972
#define VINES_PORT_MAXKNOWN	158,4994
#define VERBOSE_DEBUG	159,5029
#define VINESBUFLEN 160,5056
#define VST_ITEMLEN 162,5080
#define VST_DOMAINLEN 163,5107
#define VST_ORGLEN 164,5134
#define VST_TOTALLEN 165,5161
#define VST_DESCRIPTION 166,5228
#define VST_PASSWORD 167,5255
typedef struct vinesstringtype_ vinesstringtype_170,5284
} vinesstring;vinesstring173,5357
typedef struct vinessmallstringtype_ vinessmallstringtype_175,5373
} vinessmallstring;vinessmallstring178,5451
typedef struct vinesnametype_ vinesnametype_180,5472
} vinesnametype;vinesnametype185,5590
#define RUN_DQUEUE_SELF(RUN_DQUEUE_SELF188,5609
#define RUN_DQUEUE(RUN_DQUEUE192,5760
#define RUN_DQUEUE_W_PREV(RUN_DQUEUE_W_PREV196,5905
#define RUN_DQUEUE_W_NEXT(RUN_DQUEUE_W_NEXT200,6105
#define DQUEUE_EMPTY(DQUEUE_EMPTY204,6311
#define DQUEUE_HEAD(DQUEUE_HEAD206,6373
#define DQUEUE_ENTRY(DQUEUE_ENTRY208,6431
enum {VT_ARP,211,6501
      VT_CHANGED,212,6515
      VT_WAKEUP,213,6533
      VT_QUERY,214,6550
      VT_SYNC,215,6566
      VT_REASSEMBLY,216,6581
      VT_RETRY,217,6602
      VT_RC,218,6618
      VT_SRTP_START,219,6631
      VT_PERIODIC,220,6652
      VT_IPC_ACK,221,6671
      VT_IPC_IDLE,222,6689
      VT_IPC_RETRANS,223,6708
      VT_SVC_SVC,224,6730
      VT_SVC_TREE}225,6748
#define VMSG_STATECHANGE	227,6769
typedef struct vinesmacechotype_ vinesmacechotype_237,6916
} vinesmacechotype;vinesmacechotype240,7012
typedef struct simplevinesaddr_ simplevinesaddr_248,7210
} simplevinesaddr;simplevinesaddr251,7276
#define VINES_NONCE_LEN 253,7296
typedef uchar vinesmacaddress[vinesmacaddress254,7323
typedef uchar vinesnonce[vinesnonce255,7357
typedef struct vinesaddress_ vinesaddress_257,7401
} vinesaddress;vinesaddress292,7919
#define VINES_ADDR_NATIVE 297,7955
#define VINES_ADDR_DODIP 298,7983
#define VINES_ADDR_ATALK 299,8011
#define VINES_NET_MFG	304,8067
#define VINES_NET_SEQ	305,8126
#define VINES_NET_BANYAN 306,8188
#define VINES_NET_CISCO	307,8266
#define VINES_NON_BANYAN 308,8333
#define VINES_LAST_BANYAN 309,8400
#define VINES_SERVER	314,8489
#define VINES_1ST_CLIENT 315,8519
#define VINES_LAST_CLIENT 316,8552
#define VINES_ALLNETS 317,8585
#define VINES_ALLHOSTS 318,8622
#define VINES_UNKNOWNNET 319,8655
#define VINES_UNKNOWNHOST 320,8692
#define VINES_NOCHECKSUM 335,8997
#define VINES_IPC	342,9228
#define VINES_SPP	343,9248
#define VINES_ARP	344,9268
#define VINES_RTP	345,9288
#define VINES_ICP	346,9308
#define NVINESIPTYPES	347,9328
#define VINES_BA	353,9565
#define VINES_HDLC	354,9623
#define VINES_X25	355,9683
#define VINES_SMDS	356,9733
#define VINES_FRAME	357,9787
#define VINES_CLASS	363,9892
#define VINES_HOPS	364,9950
#define VINES_TTL0	365,10007
#define VINES_B_SERVERS	367,10036
#define VINES_B_LANS	368,10107
#define VINES_B_GT4800	369,10158
#define VINES_B_NOCHARG	370,10231
#define VINES_B_ALL	371,10303
#define VINES_REDIRECT	373,10360
#define VINES_METRIC	374,10424
#define VINES_ERROR	375,10490
#define VINES_NOFLAGS	376,10558
#define	VINES_MINSIZE	381,10621
#define	VINES_MAXSIZE	382,10647
#define VINES_FORWMAX	383,10680
typedef struct varp_header_ varp_header_409,11364
} varp_header;varp_header413,11479
typedef struct vsarp_header_ vsarp_header_415,11495
} vsarp_header;vsarp_header422,11748
#define VARP_VER_NSARP 424,11765
#define VARP_VER_SARP 425,11790
#define VARP_SVC_REQ 427,11816
#define VARP_SVC_RESP 428,11840
#define VARP_ASS_REQ 429,11864
#define VARP_ASS_RESP 430,11888
#define VARP_LAST 431,11912
#define VARP_LEARNING_TIME 433,11937
typedef enum varp_states_ varp_states_434,11977
} varp_states;varp_states440,12114
typedef enum varp_type_ varp_type_442,12130
} varp_type;varp_type446,12221
typedef struct vinesicptype_ vinesicptype_454,12409
} vinesicptype;vinesicptype458,12496
#define VICP_EXCEPTION 460,12513
#define VICP_METRIC	461,12543
#define VICP_LAST	462,12569
#define V_ICP_COPIEDBYTES 464,12594
#define VINES_ERR_NO_PROCEDURE	472,12803
#define VINES_ERR_HOST_UNR	473,12869
#define VINES_ERR_NO_SOCKET	474,12927
#define VINES_ERR_NO_PROTOCOL	475,12984
typedef struct vinesechotype_ vinesechotype_484,13206
} vinesechotype;vinesechotype493,13505
typedef struct vinestracetype_ vinestracetype_495,13523
  } vinestracetype;vinestracetype509,14157
typedef struct vtrace_info_ vtrace_info_511,14178
} vtrace_info;vtrace_info520,14534
#define VTRACE_ABORT	522,14550
#define VTRACE_TIMEOUT	523,14577
#define VTRACE_INTERRUPTED	524,14606
#define VTRACE_MAXWAITTIME	526,14639
typedef struct vinesacctype_ vinesacctype_532,14733
} vinesacctype;vinesacctype548,15054
typedef enum vprop_type_ vprop_type_556,15273
} vprop_type;vprop_type560,15368
typedef enum vsvless_type_ vsvless_type_562,15383
} vsvless_type;vsvless_type567,15531
enum VINES_SIGNALS VINES_SIGNALS569,15548
    VINES_SIGNAL_IDB_UP 570,15569
    VINES_SIGNAL_IDB_UP = 0x80000000,570,15569
    VINES_SIGNAL_IDB_DOWN,571,15607
    VINES_SIGNAL_VC_ADDED,572,15634
    VINES_SIGNAL_VC_REMOVED,573,15661
    VINES_SIGNAL_IDB_DELETION_PENDING574,15690
struct vinesidbtype_ vinesidbtype_580,15770
#define IDB_VINES_BCAST 793,26260
static inline boolean do_vines_pkt_debug 803,26547
static inline boolean do_vines_rtp_debug 809,26710
static inline boolean do_vines_rtp_v_debug 815,26873
static inline boolean do_vines_table_debug 821,27056
#define FOR_ALL_VIDBS(FOR_ALL_VIDBS829,27249
vines_getidb 833,27372
static inline boolean vines_is_lan 838,27459
static inline boolean vines_is_wan 843,27563
static inline boolean vines_valid_net 848,27655
static inline boolean vines_valid_host 853,27775
static inline boolean vines_is_this_addr 858,27902
static inline boolean vines_is_bcast_addr 863,28033
static inline boolean server_is_banyan 868,28169
static inline paktype *vip_getbuffer 873,28288
static inline boolean vines_no_updates 888,28578
static inline void vines_flush_queue 894,28751
static inline ulong vines_frames_input 902,28895
static inline ulong vines_frames_output 909,29119
static inline ulong vines_frames_deltain 916,29346
static inline ulong vines_frames_deltaout 926,29694
static inline ulong vines_frames_fastin 936,30045
static inline ulong vines_frames_fastout 942,30220
static inline ulong vines_bytes_input 948,30398
static inline ulong vines_bytes_output 955,30630
static inline ulong vines_bytes_deltain 962,30863
static inline ulong vines_bytes_deltaout 972,31218
static inline char *vtimer_pretty_value 990,31775
static inline char *vtimer_pretty_print 999,31995
static inline vinesidbtype *vip_send_pak 1015,32405
