
ospf_pak.h,4378
typedef struct link_state_header link_state_header49,1402
} lsatype;lsatype59,1878
#define LINK_STATE_HEADER_SZ 61,1890
#define LSA_TYPE_STUB	66,1986
#define LSA_TYPE_RTR	67,2014
#define LSA_TYPE_NET	68,2041
#define LSA_TYPE_SUM_NET	69,2065
#define LSA_TYPE_SUM_ASB	70,2092
#define LSA_TYPE_ASE	71,2119
#define LSA_TYPE_IGNORE	72,2143
#define LSA_TYPE_7_ASE	73,2170
#define RTR_LS_MAX_COST	75,2197
#define SUM_LS_INFINITY	76,2229
#define INTRA_PATH_COST_MAX	77,2263
#define SPF_SCHD_RTR	79,2314
#define SPF_SCHD_NET	80,2356
#define SPF_SCHD_SNET	81,2398
#define SPF_SCHD_ASB	82,2445
#define SPF_SCHD_ASE	83,2491
#define SPF_SCHD_7_ASE	84,2533
#define SPF_SCHD_BIT(SPF_SCHD_BIT86,2580
#define SPF_SCHD_INTRA	88,2622
#define SPF_SCHD_INTER	89,2674
#define SPF_SCHD_EX	90,2727
typedef struct hello_header hello_header95,2823
} hellotype;hellotype104,3312
#define HELLO_HDR_SIZE	106,3327
#define NBR_ID_SZ	107,3354
#define OPTION_T_BIT 108,3375
#define OPTION_E_BIT 109,3403
#define OPTION_MC_BIT 110,3431
#define OPTION_N_P_BIT 111,3459
#define OPTION_DC_BIT 112,3487
#define GET_E_BIT(GET_E_BIT115,3562
#define GET_T_BIT(GET_T_BIT116,3616
#define GET_TE_BITS(GET_TE_BITS117,3670
#define GET_MC_BIT(GET_MC_BIT118,3743
#define GET_N_P_BIT(GET_N_P_BIT119,3797
#define GET_DC_BIT(GET_DC_BIT120,3855
#define OSPF_DC_IF_TYPE(OSPF_DC_IF_TYPE125,3968
#define HELLO_OPTION(HELLO_OPTION135,4315
#define HELLO_PRIORITY(HELLO_PRIORITY136,4381
#define HELLO_NBR_START_PTR(HELLO_NBR_START_PTR137,4444
typedef struct dbdtype dbdtype143,4554
} dbdtype;dbdtype149,4882
#define DBD_HDR_SIZE 151,4894
#define I_BIT 153,4934
#define M_BIT 154,4954
#define MS_BIT 155,4974
#define IS_MS_BIT_SET(IS_MS_BIT_SET157,4996
#define IS_M_BIT_SET(IS_M_BIT_SET158,5040
#define IS_I_BIT_SET(IS_I_BIT_SET159,5083
#define IS_I_M_MS_SET(IS_I_M_MS_SET160,5126
#define IS_I_MS_OFF(IS_I_MS_OFF161,5190
typedef struct ls_req ls_req166,5299
} ls_req;ls_req170,5454
#define LS_REQ_SZ 172,5465
typedef struct ls_upd_ ls_upd_178,5547
} ls_upd;ls_upd181,5688
#define LS_UPD_SZ 183,5699
typedef struct tos_metric tos_metric185,5735
} tos_metric;tos_metric188,5874
#define	LINK_TOS_MASK 190,5889
#define LINK_METRIC_SIZE 191,5919
typedef struct link_info link_info193,5966
} link_info;link_info200,6297
#define	LINK_IF_RTR	206,6397
#define	LINK_IF_TRANS_NET	207,6420
#define	LINK_IF_STUB_NET	208,6448
#define	LINK_IF_VIRTUAL_LINK	209,6475
#define LINK_INFO_SIZE 211,6507
#define HOST_NET_MASK	213,6551
typedef struct rtr_la_hdr rtr_la_hdr216,6588
} rtr_lsatype;rtr_lsatype222,6779
#define	AREA_BORDER_B_BIT	224,6795
#define	AS_BORDER_E_BIT	225,6849
#define AS_AREA_BORDER_MASK 227,6905
#define RTR_LSA_HDR_SIZE	229,6977
typedef struct net_lsatype{net_lsatype231,7031
} net_lsatype;net_lsatype235,7160
#define NET_LSA_HDR_SIZE	237,7176
typedef struct sum_lsatype{sum_lsatype239,7229
} sum_lsatype;sum_lsatype243,7365
#define SUM_LSA_HDR_SIZE	245,7381
#define SUM_METRIC(SUM_METRIC246,7433
#define SUM_TOS(SUM_TOS247,7492
typedef struct as_metric{as_metric249,7555
} as_metric;as_metric253,7658
#define AS_METRIC_SZ	255,7672
#define	TAG_AUTOMATIC	257,7697
#define	TAG_COMPLETE	258,7730
#define	TAG_PATHLEN_RSV	260,7763
#define	TAG_PATHLEN_GE1	261,7798
#define	TAG_PATHLEN_1	262,7833
#define	TAG_PATHLEN_0	263,7866
#define	TAG_PATHLENPART 265,7900
#define	TAG_ARBPART	266,7935
#define TAG_ASPART	267,7966
#define	TAG_ARBSHIFT	269,7997
typedef struct ex_lsatype{ex_lsatype271,8022
} ex_lsatype;ex_lsatype275,8111
#define ASE_LSA_HDR_SIZE	277,8126
#define	ASE_TYPE2_METRIC_BIT	278,8178
#define ASE_METRIC(ASE_METRIC280,8220
#define  ASE_TOS(ASE_TOS281,8279
#define  ASE_METRIC_TYPE(ASE_METRIC_TYPE282,8342
static inline ushort exlsa_to_route_type_inline 284,8409
#define EXLSA_TO_ROUTE_TYPE(EXLSA_TO_ROUTE_TYPE294,8744
#define EXLSA_TO_METRIC_TYPE(EXLSA_TO_METRIC_TYPE296,8816
#define OSPF_AUTH_LEN 300,8960
typedef struct ospf_auth_type2_ ospf_auth_type2_305,9067
} ospf_auth_type2;ospf_auth_type2310,9185
typedef struct ospf_header ospf_header312,9205
} ospftype;ospftype325,9845
#define OSPF_HDR_SIZE 327,9858
#define	OSPF_HELLO	332,9941
#define	OSPF_DB_DES	333,9996
#define	OSPF_LS_REQ	334,10063
#define	OSPF_LS_UPD	335,10110
#define	OSPF_LS_ACK	336,10156
