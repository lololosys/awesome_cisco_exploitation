
interface_private.h,16323
#define __INTERFACE_PRIVATE_H__659,25355
#define MAX_INTERFACES 821,32157
#define BANDWIDTH_SCALE 838,32990
#define SCALED_BANDWIDTH(SCALED_BANDWIDTH839,33060
#define BPS_TO_IGRP_BW(BPS_TO_IGRP_BW840,33140
#define INPUTQ_DEFAULT 842,33204
#define SNIFFLONGS 844,33291
typedef ulong encapstype;encapstype850,33430
#define IS_VIP(IS_VIP852,33457
#define IS_VIP4R(IS_VIP4R853,33506
#define IS_ISDN(IS_ISDN854,33577
# define IDB_MCI	880,34474
# define IDB_VIP 881,34530
# define IDB_ENCRYPT 882,34583
# define IDB_C5_ETHER	883,34661
# define IDB_PCBUS 884,34728
# define IDB_LEX	885,34801
# define IDB_ETHER	886,34857
# define IDB_SERIAL	887,34925
# define IDB_CHANNEL 888,34994
# define IDB_TR	889,35052
# define IDB_RESTART	890,35109
# define IDB_NO_NVGEN	891,35175
# define IDB_FDDI	892,35244
# define IDB_CBUS	893,35296
# define IDB_HSSI	894,35354
# define IDB_HEARSELF	895,35413
# define IDB_BCAST	896,35477
# define IDB_BVI	897,35541
# define IDB_POS 900,35656
# define IDB_DISABLED	901,35728
# define IDB_VIRTUAL	902,35786
# define IDB_BRIDGE	903,35855
# define IDB_ASYNC	904,35920
# define IDB_VTEMPLATE 905,35981
# define IDB_DELETED 907,36086
# define IDB_TUNNEL 909,36203
# define IDB_SYNC	910,36274
# define IDB_ISDN	911,36340
# define IDB_NOHARDWARE 912,36397
# define IDB_DIALER 914,36503
# define IDB_ATM	915,36571
# define IDB_VIRTUAL_HW	916,36631
# define IDB_EPHEMERAL 929,37306
# define IDB_DONTSETUP 934,37473
# define IDB_VIP_4R 936,37543
# define IDB_IEEEMASK	938,37581
# define IDB_802ONLY	939,37643
# define IDB_BRIDGERESET 946,37960
enum TBR_MEDIA TBR_MEDIA955,38193
   TBR_UNKNOWN,956,38210
   TBR_ETHER,957,38226
   TBR_FDDI,958,38240
   TBR_CTR,959,38253
   TBR_MTR,960,38292
   TBR_TR,961,38331
   TBR_FDDI_TRANSIT,962,38368
   TBR_HDLC_TRANSIT,963,38389
   TBR_ETHER_SDE,964,38410
   TBR_FDDI_SDE,965,38428
   TBR_CTR_SDE,966,38445
   TBR_TR_SDE,967,38461
   TBR_HDLC_TRANSIT_SDE,968,38476
   TBR_LEX_TRANSIT,969,38501
   TBR_VBRIDGE,970,38521
   TBR_ATM_TRANSIT,971,38537
   TBR_LANE_ETHER,972,38557
   TBR_FRAME_RELAY_TRANSIT,973,38576
   TBR_CPP_TRANSIT,974,38604
   TBR_CIP_VLAN,975,38624
   TBR_SMDS_TRANSIT,977,38712
   TBR_ETHER_ISL_VLAN,978,38733
   TBR_LAST,980,38865
#define IP_FAST_ACCESSLISTS	989,39080
#define IP_FAST_ACCOUNTING	990,39116
#define IP_FAST_INPUTACCESSLISTS	991,39151
#define IP_FAST_POLICY	992,39223
#define IP_FAST_RSP_DDR	993,39281
# define IP_FAST_ROUTECACHE	998,39417
# define IP_CBUS_ROUTECACHE	999,39457
# define IP_SSE_ROUTECACHE	1000,39497
# define IP_FLOW_ROUTECACHE	1001,39536
# define IP_OPTIMUM_ROUTECACHE	1002,39576
# define IP_DISTRIBUTED_ROUTECACHE	1003,39619
# define IP_DEFAULT_ROUTECACHE	1005,39666
#define PRIORITY_QUEUE_INC(PRIORITY_QUEUE_INC1013,39850
#define DEFAULT_QUEUE(DEFAULT_QUEUE1014,39925
enum FSTYPE FSTYPE1024,40163
    FS_UNKNOWN 1025,40177
    FS_ETHER 1026,40225
    FS_FDDI 1027,40273
    FS_TOKEN 1028,40316
    FS_HDLC 1029,40366
    FS_FR 1030,40411
    FS_SMDS 1031,40455
    FS_PPP 1032,40494
    FS_SLIP 1033,40531
    FS_AIP 1034,40591
    FS_CHANNEL 1035,40619
    FS_LEX 1036,40669
    FS_ATM_DXI 1037,40712
    FS_NULL 1038,40748
    FS_TUNNEL 1039,40780
    FS_DIALER 1040,40834
    FS_BVI 1041,40866
    FS_C5_ETHER 1042,40942
    FS_LAST 1043,41010
#define PP_ROLE_UNDEFINED 1049,41095
#define PP_ACTIVE 1050,41129
#define PP_ASCII 1051,41187
#define PP_CHARINT 1052,41243
#define SPAN_BR_PROC_PRIORITY_QUEUE	1060,41452
#define SPAN_BR_PROC_X25_BRIDGE	1061,41499
#define SPAN_BR_PROC_FR_BRIDGE	1062,41543
#define SPAN_BR_PROC_DIALER	1063,41586
#define SPAN_BR_PROC_VIRTUAL_IDB	1064,41626
#define	SPAN_BR_PROC_SMDS_BRIDGE	1065,41670
#define SPAN_BR_PROC_PPP_BRIDGE 1066,41714
#define SPAN_BR_PROC_ATM_BRIDGE	1067,41765
#define SPAN_BR_PROC_MLAPB_BRIDGE 1068,41809
#define	SPAN_BR_PROC_COMPRESSION	1069,41860
#define EOIR_RESET	1075,41951
#define IDB_NEEDS_INIT	1076,42024
typedef boolean (*notify_func_t)notify_func_t1088,42526
typedef struct idb_deletion_notify_ idb_deletion_notify_1090,42577
} idb_notify_t;idb_notify_t1095,42845
typedef struct idb_sb_ idb_sb_1107,43256
} idb_sb_t;idb_sb_t1113,43544
typedef struct idb_sb_dyn_ idb_sb_dyn_1118,43606
} idb_sb_dyn_t;idb_sb_dyn_t1121,43761
typedef boolean (*vencap_t)vencap_t1362,54436
typedef boolean (*vencap_t)(paktype paktype1362,54436
typedef boolean (*bencap_t)bencap_t1363,54482
typedef boolean (*bencap_t)(paktype *, hwidbtype hwidbtype1363,54482
typedef int (*cbencap_t)cbencap_t1365,54536
typedef int (*cbencap_t)(hwidbtype hwidbtype1365,54536
typedef uchar * (*bridge_vencap_t)bridge_vencap_t1366,54586
typedef uchar * (*bridge_vencap_t)(paktype paktype1366,54586
typedef void (*soutput_t)soutput_t1367,54639
typedef void (*soutput_t)(hwidbtype hwidbtype1367,54639
typedef void (*sinput_t)sinput_t1368,54679
typedef void (*sinput_t)(hwidbtype hwidbtype1368,54679
typedef boolean (*oqueue_t)oqueue_t1369,54718
typedef boolean (*oqueue_t)(hwidbtype *, paktype *, enum HEADTAIL)HEADTAIL1369,54718
typedef paktype * (*oqueue_dequeue_t)oqueue_dequeue_t1370,54786
typedef paktype * (*oqueue_dequeue_t)(hwidbtype hwidbtype1370,54786
typedef void (*iqueue_t)iqueue_t1371,54838
typedef void (*iqueue_t)(hwidbtype *, paktype paktype1371,54838
typedef iqueue_t (*parse_packet_t)parse_packet_t1372,54888
typedef iqueue_t (*parse_packet_t)(hwidbtype *, paktype *, uchar uchar1372,54888
typedef void (*reset_t)reset_t1374,54958
typedef void (*reset_t)(hwidbtype hwidbtype1374,54958
typedef void (*shutdown_t)shutdown_t1375,54996
typedef void (*shutdown_t)(hwidbtype hwidbtype1375,54996
typedef boolean (*extract_hwaddr_t)extract_hwaddr_t1376,55042
typedef boolean (*extract_hwaddr_t)(hwidbtype *, hwaddrtype hwaddrtype1376,55042
typedef boolean (*extract_snpa_t)extract_snpa_t1377,55106
typedef boolean (*extract_snpa_t)(paktype *, hwaddrtype *, enum SRCDST)SRCDST1377,55106
typedef void (*make_snpa_native_t)make_snpa_native_t1378,55179
typedef void (*make_snpa_native_t)(hwidbtype *, uchar uchar1378,55179
typedef fuzzy (*broadcast_t)broadcast_t1379,55237
typedef fuzzy (*broadcast_t)(hwidbtype *, paktype paktype1379,55237
typedef boolean (*can_mcast_t)can_mcast_t1380,55291
typedef boolean (*can_mcast_t)(hwidbtype *hwidb, enum LINK link)link1380,55291
typedef void (*setencap_t)setencap_t1381,55357
typedef void (*setencap_t)(parseinfo parseinfo1381,55357
typedef void (*deencap_t)deencap_t1382,55398
typedef void (*deencap_t)(hwidbtype hwidbtype1382,55398
typedef void (*lineaction_t)lineaction_t1383,55438
typedef void (*lineaction_t)(hwidbtype hwidbtype1383,55438
typedef boolean (*linestate_t)linestate_t1384,55481
typedef boolean (*linestate_t)(hwidbtype hwidbtype1384,55481
typedef void (*periodic_t)periodic_t1385,55526
typedef void (*periodic_t)(hwidbtype hwidbtype1385,55526
typedef void (*device_periodic_t)device_periodic_t1386,55567
typedef void (*device_periodic_t)(hwidbtype hwidbtype1386,55567
typedef void (*enable_t)enable_t1387,55615
typedef void (*enable_t)(hwidbtype hwidbtype1387,55615
typedef void (*listen_t)listen_t1388,55654
typedef void (*listen_t)(idbtype *, uchar *, uchar)uchar1388,55654
typedef void (*listenrange_t)listenrange_t1389,55707
typedef void (*listenrange_t)(idbtype *, uchar *, uchar uchar1389,55707
typedef boolean (*add_hwaddr_dynamic_t)add_hwaddr_dynamic_t1390,55767
typedef boolean (*add_hwaddr_dynamic_t)(idbtype *, uchar uchar1390,55767
typedef void (*setup_bridging_t)setup_bridging_t1391,55828
typedef void (*setup_bridging_t)(hwidbtype hwidbtype1391,55828
typedef boolean (*set_extra_addrs_t)set_extra_addrs_t1392,55875
typedef boolean (*set_extra_addrs_t)(hwidbtype hwidbtype1392,55875
typedef boolean (*set_maxdgram_t)set_maxdgram_t1393,55926
typedef boolean (*set_maxdgram_t)(hwidbtype hwidbtype1393,55926
typedef void (*show_support_t)show_support_t1394,55984
typedef void (*show_support_t)(hwidbtype hwidbtype1394,55984
typedef boolean (*show_controller_t)show_controller_t1395,56029
typedef boolean (*show_controller_t)(hwidbtype *, parseinfo parseinfo1395,56029
typedef int (*txqlength_t)txqlength_t1396,56093
typedef int (*txqlength_t)(hwidbtype hwidbtype1396,56093
typedef int (*max_proto_mtu_t)max_proto_mtu_t1397,56134
typedef int (*max_proto_mtu_t)(idbtype *, enum LINK, boolean)boolean1397,56134
typedef void (*clear_support_t)clear_support_t1398,56197
typedef void (*clear_support_t)(hwidbtype hwidbtype1398,56197
typedef void (*system_configured_t)system_configured_t1399,56243
typedef void (*system_configured_t)(hwidbtype hwidbtype1399,56243
typedef void (*fastsend_t)fastsend_t1400,56293
typedef void (*fastsend_t)(hwidbtype *, paktype paktype1400,56293
typedef	boolean (*tr_srb_fastswitch_type)tr_srb_fastswitch_type1401,56345
typedef	boolean (*tr_srb_fastswitch_type)(hwidbtype *, void *, hwidbtype hwidbtype1401,56345
typedef void (*dot5_void_type)dot5_void_type1402,56422
typedef void (*dot5_void_type)(hwidbtype hwidbtype1402,56422
typedef boolean (*dot5_boolean_type)dot5_boolean_type1403,56467
typedef boolean (*dot5_boolean_type)(hwidbtype hwidbtype1403,56467
typedef void (*show_serial_state_t)show_serial_state_t1404,56518
typedef void (*show_serial_state_t)(hwidbtype hwidbtype1404,56518
typedef void (*afilter_reset_t)afilter_reset_t1405,56576
typedef void (*afilter_reset_t)(hwidbtype hwidbtype1405,56576
typedef boolean (*fs_explorer_t)fs_explorer_t1407,56623
typedef boolean (*fs_explorer_t)(paktype *, fs_hdrtype *, hwidbtype hwidbtype1407,56623
typedef boolean (*fs_vbridge_t)fs_vbridge_t1408,56695
typedef boolean (*fs_vbridge_t)(paktype *, hwidbtype hwidbtype1408,56695
typedef int (*if_srb_fastout_type)if_srb_fastout_type1409,56752
typedef int (*if_srb_fastout_type)(hwidbtype *, paktype paktype1409,56752
typedef boolean (*if_srb_les_fastout_type)if_srb_les_fastout_type1410,56812
typedef boolean (*if_srb_les_fastout_type)(hwidbtype *, hwidbtype *, paktype paktype1410,56812
typedef	boolean (*if_srb_hes_fastout_type)if_srb_hes_fastout_type1412,56906
					   int, ulong *, int, hwidbtype hwidbtype1413,56984
typedef int (*rsrb_fst_fastout_type)rsrb_fst_fastout_type1414,57025
				     ipcache_rntype *, hwidbtype hwidbtype1416,57125
typedef int (*ip_turbo_fs_t)ip_turbo_fs_t1418,57167
typedef int (*ip_turbo_fs_t)(void *, void *, hwidbtype hwidbtype1418,57167
typedef void (*rx_char_int_t)rx_char_int_t1420,57227
typedef void (*rx_char_int_t)( hwidbtype *idb, uchar input, boolean overrun)overrun1420,57227
typedef void (*rx_framer_int_t)rx_framer_int_t1421,57305
typedef void (*rx_framer_int_t)(hwidbtype *idb, paktype **frame)frame1421,57305
typedef void (*tx_comp_notify_t)tx_comp_notify_t1422,57371
typedef void (*tx_comp_notify_t)( hwidbtype *idb idb1422,57371
typedef int  (*rx_in_progress_t)rx_in_progress_t1423,57423
typedef int  (*rx_in_progress_t)(hwidbtype *idb idb1423,57423
typedef void (*restart_rx_t)restart_rx_t1424,57474
typedef void (*restart_rx_t)(hwidbtype hwidbtype1424,57474
typedef void (*disable_rx_t)disable_rx_t1425,57517
typedef void (*disable_rx_t)(hwidbtype hwidbtype1425,57517
typedef paktype* (*getsysbuf_t)getsysbuf_t1426,57561
typedef paktype* (*getsysbuf_t)(hwidbtype hwidbtype1426,57561
struct hwidbtype_ hwidbtype_1429,57631
struct idbtype_ idbtype_2462,102034
typedef enum RXTYPE RXTYPE3056,127292
} rxtype_t;rxtype_t3113,130951
#define FSF_DODIP	3122,131126
#define FSF_DECNET	3123,131162
#define FSF_XNS	3124,131200
#define FSF_CLNS	3125,131233
#define FSF_APPLE	3126,131267
#define FSF_NOVELL1	3127,131303
#define FSF_NOVELL2	3128,131343
#define FSF_APOLLO	3129,131383
#define FSF_SNAP_APPLE	3130,131421
#define FSF_VINES	3131,131467
#define FSF_SRB	3132,131503
#define FSF_LEX	3133,131536
#define NONSLOTTED_IDB_NAMES	3135,131571
#define SLOTTED_IDB_NAMES	3136,131603
#define EXTENDED_SLOTTED_IDB_NAMES	3137,131632
#define PA_VIRTUAL_SLOT_BOUNDARY	3139,131670
#define PA_VIRTUAL_SLOT_BITS	3140,131705
#define PA_BAY_0	3141,131737
#define PA_BAY_1	3142,131758
static inline boolean is_lapb 3162,132451
static inline boolean is_slapb_only 3172,132733
static inline boolean is_mlapb_only 3182,132976
static inline boolean is_x25 3193,133235
static inline boolean is_smds 3198,133344
static inline boolean is_isdn 3204,133488
static inline void pak_requeue_head(3215,133711
#define FOR_ALL_HWIDBS(FOR_ALL_HWIDBS3226,133921
#define FOR_ALL_SWIDBS(FOR_ALL_SWIDBS3233,134068
#define FOR_ALL_SWIDBS_ON_HW(FOR_ALL_SWIDBS_ON_HW3240,134249
static inline hwidbtype *  hwidb_or_null 3248,134477
static inline idbtype *firstsw_or_null 3256,134655
static inline hwidbtype *hwidb_or_null_from_index 3264,134834
#define SWIDBQ_HEAD 3279,135123
#define HWIDBQ_HEAD 3284,135212
enum VLAN_PROTOCOLS VLAN_PROTOCOLS3294,135463
    VLAN_ROUTE_IP,3296,135486
    VLAN_ROUTE_NOVELL,3297,135505
    VLAN_ROUTE_APPLE,3298,135528
    VLAN_TBRIDGE,3299,135550
    VLAN_ROUTE_DECNET,3300,135568
    VLAN_ROUTE_XNS,3301,135591
    VLAN_ROUTE_CLNS,3302,135611
    VLAN_ROUTE_APOLLO,3303,135632
    VLAN_ROUTE_VINES,3304,135655
    VLAN_PROTOCOLS_MAX3305,135677
static inline boolean is_subinterface 3312,135819
static inline boolean is_first_swidb 3322,136056
static inline boolean is_sde_capable 3336,136351
static inline boolean is_isl_vlan_capable 3356,136844
static inline boolean is_vlan_capable 3365,137079
static inline boolean is_p2p_hw_inline 3385,137557
static inline boolean is_p2p_inline 3403,138043
static inline boolean is_only_swidb 3413,138297
static inline boolean hwidb_use_short_tx_q_inline 3432,138972
static inline boolean is_ieee802 3448,139331
static inline boolean is_ethernet 3456,139517
static inline boolean is_tokenring 3464,139702
static inline boolean is_serial 3472,139886
static inline boolean is_hdlc 3477,140008
static inline boolean is_async 3482,140127
static inline boolean is_sync 3487,140247
static inline boolean is_ppp 3492,140365
static inline boolean is_slip 3497,140482
static inline boolean is_ppp_or_slip 3502,140601
static inline boolean is_cpp 3508,140771
static inline boolean is_fddi 3516,140944
static inline boolean is_fddi_br_encap 3522,141095
static inline boolean is_lan 3529,141290
static inline boolean is_frame_relay 3538,141507
static inline boolean is_ddr 3547,141755
static inline boolean if_is_ddr 3557,141969
static inline boolean is_multilink 3566,142141
static inline boolean if_is_nbma 3579,142382
static inline boolean is_sdlc 3604,143030
static inline boolean is_lex 3615,143328
static inline boolean is_cip 3620,143444
static inline boolean is_cip_lan 3625,143563
static inline boolean is_cip_virtual 3631,143723
static inline boolean is_srb_hw 3637,143894
static inline boolean is_bstun 3645,144092
static inline boolean is_g703 3651,144244
static inline boolean is_tunnel 3659,144463
static inline boolean is_vip 3664,144585
static inline boolean is_valid_vip 3669,144706
static inline boolean is_atmhw 3674,144816
static inline boolean is_atm 3679,144934
static inline boolean is_atm_dxi 3684,145051
static inline boolean is_wan 3689,145176
static inline boolean is_bvi 3696,145382
static inline boolean is_ospf_vl 3701,145498
idb_get_swsb_inline 3724,146174
idb_get_hwsb_inline 3730,146310
idb_use_swsb_inline 3736,146448
idb_use_hwsb_inline 3743,146608
idb_release_swsb_inline 3750,146771
idb_release_hwsb_inline 3757,146909
#define RXF_ROUTING 3771,147283
#define RXF_BRIDGING 3772,147312
#define SET_ROUTING_RXTYPE(SET_ROUTING_RXTYPE3774,147342
#define CLEAR_ROUTING_RXTYPE(CLEAR_ROUTING_RXTYPE3776,147429
#define ROUTING_RXTYPE(ROUTING_RXTYPE3778,147519
#define SET_BRIDGING_RXTYPE(SET_BRIDGING_RXTYPE3800,148327
#define CLEAR_BRIDGING_RXTYPE(CLEAR_BRIDGING_RXTYPE3802,148412
#define BRIDGING_RXTYPE(BRIDGING_RXTYPE3804,148500
static inline boolean interface_up_simple 3815,148930
static inline boolean interface_admin_down 3829,149276
static inline boolean holdq_enqueue_fast 3839,149538
static inline int hwidb_holdqueue_size_or_default(3866,150404
struct hwidbtype_ hwidbtype_3884,150939
