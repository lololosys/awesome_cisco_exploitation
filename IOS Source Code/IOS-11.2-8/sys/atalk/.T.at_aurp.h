
at_aurp.h,3248
#define __AT_AURP_H__59,2013
#define AURP_VERSION 64,2076
#define AURP_SCHED_DELAY 66,2124
#define AURP_MAX_SEQUENCE 68,2192
#define AURP_MAX_CONNECTION_ID 70,2259
#define AURP_MIN_CONNECTION_ID 71,2296
typedef ushort connid_t;connid_t72,2330
#define AURPTUPLEVERSION 74,2356
#define OPTIMIZED_TUPLE 75,2424
#define OFFSET_MASK 76,2453
#define AURP_DEFAULT_TICKLETIME 78,2504
#define AURP_TICKLE_BEFORE_DATA_TIME 79,2579
#define AURP_PATHENTRY_INTERVAL 80,2627
#define AURP_PATHAGER_INTERVAL 81,2705
#define AURP_DEFAULT_UPDATE_RATE 83,2746
#define DEFAULT_SUI 86,2842
#define NA_SUI_MASK 87,2870
#define ND_SUI_MASK 88,2898
#define NRC_SUI_MASK 89,2926
#define NDC_SUI_MASK 90,2954
#define ZC_SUI_MASK 91,2982
#define SZI_MASK 94,3042
#define REMAPPING_MASK 95,3109
#define HOPREDUCE_MASK 96,3167
#define LASTFLAG_MASK 97,3225
} di_type;di_type107,3523
#define DI_IP_LENGTH 110,3562
typedef struct ip_di_ ip_di_112,3647
} ip_di_t;ip_di_t117,3778
#define DI_NULL_LENGTH 120,3819
typedef struct null_di_ null_di_122,3907
} null_di_t;null_di_t125,3988
} di_t;di_t136,4183
} Seq_status;Seq_status147,4427
#define IP_DOMAINHEADER_SIZE	150,4485
typedef struct ip_domainheader_ ip_domainheader_151,4556
} ip_domainheader_t;ip_domainheader_t157,4725
} aurp_pkttype;aurp_pkttype162,4828
} aurp_cmdtype;aurp_cmdtype176,5116
} ErrorCode;ErrorCode186,5333
#define AURP_HEADER_SIZE	189,5372
typedef struct aurp_ aurp_190,5401
} aurp_t;aurp_t195,5598
#define OPEN_REQ_SIZE 198,5638
typedef struct Open_Req_ Open_Req_199,5665
} Open_Req_t;Open_Req_t202,5751
#define OPEN_RSP_SIZE	205,5795
typedef struct Open_Rsp_ Open_Rsp_206,5821
} Open_Rsp_t;Open_Rsp_t209,5908
#define RD_SIZE 212,5946
typedef struct RD_ RD_213,5967
} RD_t;RD_t215,6013
    ZIRSP 219,6052
    XZIRSP 220,6067
    GZNRSP 221,6083
    GDZLRSP 222,6099
    ZIREQ 227,6150
    GZNREQ 228,6165
    GDZLREQ 229,6181
#define ZI_REQ_SIZE	233,6229
#define GDZL_REQ_SIZE 234,6253
} ZI_Req_t;ZI_Req_t238,6350
#define ZI_RSP_SIZE	241,6390
} ZI_Rsp_t;ZI_Rsp_t245,6489
} aurp_zipentry;aurp_zipentry256,6654
#define GDZL_RSP_SIZE	259,6701
} GDZL_Rsp_t;GDZL_Rsp_t263,6796
#define GZN_REQ_SIZE	266,6839
} GZN_Req_t;GZN_Req_t269,6908
#define GZN_RSP_SIZE	272,6950
} GZN_Rsp_t;GZN_Rsp_t275,7019
#define EVENT_TUPLE_SIZE	277,7033
} Event_tuple_t;Event_tuple_t282,7159
typedef struct at_aurp_info_ at_aurp_info_292,7487
} at_aurp_info;at_aurp_info328,8961
typedef struct _AURPOps _AURPOps337,9338
} aurpOps_t;aurpOps_t344,9638
#define GC_EVENTS_INTERVAL 349,9826
#define EXTENDED_TUPLE_MASK 350,9903
#define EXTENDED_EVENT_TUPLE_SIZE 351,9974
#define NONEXTENDED_EVENT_TUPLE_SIZE 352,10050
} EventCode;EventCode362,10261
typedef struct EventQueue_ EventQueue_365,10340
} EventQueue;EventQueue370,10459
typedef struct EventEntry_ EventEntry_373,10533
} EventEntry;EventEntry382,10794
} aurp_pdb_t;aurp_pdb_t402,11666
typedef struct aurp_pathentry_ aurp_pathentry_404,11681
} aurp_pathentry;aurp_pathentry410,11894
aurpheadstart(418,12136
aurphdrtype_start(424,12225
aurpdatastart(430,12327
aurpdatastart_aurph(436,12434
#define FOR_ALL_AURP_TUNNELS(FOR_ALL_AURP_TUNNELS445,12667
