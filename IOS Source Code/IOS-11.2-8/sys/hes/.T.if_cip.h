
if_cip.h,6960
#define	__IF_CIP_H__123,4321
#define CIP_DEFAULT_MTU 125,4343
#define CIP_VIRTUAL_MTU 126,4376
#define CIP_MAX_DAUGHTERS 131,4511
#define CIP_VIRTUAL_PORT 136,4595
#define CIP_NONE 142,4760
#define CIP_PCA_DAUGHTER 143,4794
#define CIP_ECA_DAUGHTER 144,4828
#define CIP_VIRTUAL_INTERFACE 145,4862
#define CIP_NUM_INTERFACE_TYPE 147,4897
#define VISIBLE_CIP_PCA_BANDWIDTH_3 152,5005
#define VISIBLE_CIP_PCA_BANDWIDTH_4_5 153,5070
#define VISIBLE_CIP_ECA_BANDWIDTH 154,5135
#define CIP_PCA_DELAY_3	159,5229
#define CIP_PCA_DELAY_4_5	160,5272
#define CIP_ECA_DELAY	161,5316
#define CIP_NUM_PCA_RATES 173,5723
#define CIP_PCA_RATE_3 174,5758
#define CIP_PCA_RATE_4_5 175,5829
#define CIP_PCA_DEFAULT_RATE 176,5864
#define CIP_DEFAULT_STATS_INTERVAL 183,6083
#define CIP_MIN_STATS_INTERVAL 184,6160
#define FCI_CMD_CIP_DAUGHTER_TYPE 189,6271
#define FCI_CMD_CIP_SET_PCA_BW 190,6336
#define FCI_CMD_CIP_ADDR_SELECT 191,6402
#define FCI_CMD_CIP_MEM_WRITE 192,6468
#define FCI_CMD_CIP_MEM_READ 193,6527
#define FCI_CMD_CIP_CFGBLOCK 194,6585
#define FCI_CMD_CIP_CFGPARSE 195,6658
#define FCI_CMD_CIP_DIAGNOSTIC 196,6734
#define FCI_CMD_CIP_STATS_INTERVAL 197,6798
#define FCI_CMD_CIP_ADPTR_INFO 199,6866
#  define FCI_ADPTR_TYPE 200,6943
#  define FCI_ADPTR_HWVER 201,7020
#  define FCI_ADPTR_UCODE 202,7097
#define FCI_CMD_CIP_CARD_INFO 203,7174
#  define FCI_SRAM_AVAIL 204,7251
#  define FCI_SRAM_TOTAL 205,7328
#  define FCI_DRAM_AVAIL 206,7405
#  define FCI_DRAM_TOTAL 207,7482
#define CIP_NAME_LEN 209,7560
#define CIP_MAX_PATH 210,7590
#define CIP_MAX_DEVICE 211,7625
#define CIP_VC_INUSE 216,7702
#define CIP_ADJ_DEV 217,7731
#define CIP_VC_BRDCST 218,7816
#define CIP_VC_SROUTE 219,7884
#define CIP_CFG_PNDING 224,7998
#define CIP_CFG_CREATE 225,8074
#define CIP_CFG_UPDATE 226,8128
#define CIP_CFG_DELETE 227,8182
#define CIP_CHG_PNDING 232,8292
#define CIP_CHG_UP 233,8352
#define CIP_CHG_DOWN 234,8411
#define CIP_CFG_ACK_CIP 239,8550
#define CIP_CFG_ACK_LOCAL 240,8615
#define PATH_ESTABLISHED 245,8722
#define CIP_PER_INT_MAX_VC 247,8755
#define CIP_TEARDOWN_VC 248,8787
#define CIP_NEW_VC 249,8844
#define CIP_INVALID_VC 250,8900
#define CIP_NODEID_SIZE 252,8955
#define CIP_CFG_SIZE 253,9015
#define CIP_DISABLE_TIME 257,9122
#define SEARCH_NEXT 262,9264
#define SEARCH_MATCH 263,9288
#define NEXT_BEST 264,9312
#define ND_TYPE_LEN 270,9433
#define ND_MODEL_LEN 271,9460
#define ND_MANUF_LEN 272,9487
#define ND_PLANT_LEN 273,9514
#define ND_SERIAL_LEN 274,9541
struct cip_nodeid cip_nodeid284,9862
typedef struct cip_nodeid cip_nodeidtype;cip_nodeidtype291,10217
#define LIR_IMPL_INCIDENT 297,10325
#define LIR_BER_EXCEEDED 298,10380
#define LIR_LINK_FAILED 299,10441
#define LIR_NOS 300,10490
#define LIR_SEQ_TO 301,10563
#define LIR_INVALID_SEQ 302,10632
#define MAX_LIR_TYPES 303,10701
#define LIR_ND_SIZE 304,10764
#define DB_VALID 310,10862
#define DB_ESCON 311,10937
#define DB_LOADED 312,11015
#define DB_END_LOAD 313,11090
#define DB_OFFLINE 314,11164
#define DB_GO_OFFLINE 315,11234
#define DB_RQC_PENDING 316,11320
#define DB_RES1_1 317,11389
#define DB_RES1_2 318,11437
#define DB_RES1_3 319,11485
#define DB_LOAD_PENDING 320,11533
#define DB_FORCD_LOGOUT 321,11613
#define DB_RES2_1 322,11688
#define DB_RES2_2 323,11736
#define DB_RES2_3 324,11785
#define DB_RES2_4 325,11833
#define PA_VALID 330,11939
#define PA_ENABLED 331,12014
#define PA_SIGNAL 332,12096
#define PA_SEND_LOVE 333,12151
#define PA_PASS_DISABLE 334,12215
#define PA_PASS_ENA_PEND 335,12277
} rate_type;rate_type344,12550
typedef struct data_stats_ data_stats_349,12628
} data_stats;data_stats362,13463
#define CLAW_MAX_LINKS 367,13543
typedef struct statistics_ statistics_368,13572
} statistics;statistics381,14338
typedef struct cip_metrics_ cip_metrics_386,14386
} cip_metrics;cip_metrics390,14476
typedef cip_metrics *cip_metrics_p;cip_metrics_p391,14491
enum CFGACK_CODE CFGACK_CODE400,14636
     CIP_CFG_NACK 401,14655
     CIP_CFG_ACK 402,14726
     CIP_CFG_SR_ADD 403,14797
     CIP_CFG_SR_DEL 404,14868
struct deed_t_ deed_t_407,14943
typedef struct deed_t_ deed_t;deed_t411,15144
enum dispose_t_ dispose_t_413,15176
    DISPOSE_TO_CHUNK,414,15194
    DISPOSE_TO_PREQ 415,15251
typedef enum dispose_t_ dispose_t;dispose_t417,15310
enum plevent_loc_t_ plevent_loc_t_419,15346
    PLEVEL_CHNK,420,15368
    PLEVEL_PREQ,421,15403
    PLEVEL_RDYQ 422,15441
typedef enum plevent_loc_t_ plevent_loc_t;plevent_loc_t424,15478
enum PLEVENT PLEVENT427,15555
     PLEVENT_CFGACK,428,15570
     PLEVENT_ASYREQ,429,15637
     PLEVENT_ENABLE 430,15704
#define PLEVENT_DSIZE 438,16027
struct plevel_t_ plevel_t_440,16080
typedef struct plevel_t_ plevel_t;plevel_t446,16323
#define CIP_PLEVENT_ELEMENT_SIZE 448,16359
#define CIP_PLEVENT_COUNT 449,16411
#define CIP_PLEVENT_CHUNK_NAME 450,16496
#define CIP_PLEVEL_PREQ_MIN 452,16559
typedef void (*cip_encap_t)cip_encap_t455,16697
typedef void (*cip_encap_t)(hwidbtype *, void *, ushort)ushort455,16697
struct cip_idb_info cip_idb_info460,16808
typedef struct cip_idb_info cip_idb_infotype;cip_idb_infotype507,18425
struct cip_vc cip_vc512,18530
typedef struct cip_vc cip_vctype;cip_vctype540,19706
#define CIP_NUM_CLAW_NAMES 542,19741
#define CIP_NUM_OFFLOAD_NAMES 543,19773
#define LL_TIMESTAMP_SIZE 549,19878
#define LL_LEN_SIZE 550,19950
#define LL_TYPE_SIZE 551,20022
#define LL_HEADER_SIZE 552,20094
#define CIP_MAX_MIB_DATA 554,20152
} cip_ll;cip_ll560,20382
#define CIP_MIB_CARD 562,20393
#define CIP_MIB_DB 563,20445
#define CIP_MIB_DEV 564,20497
#define CIP_MIB_PA 565,20549
} cip_ll_card;cip_ll_card579,20919
} cip_ll_db;cip_ll_db594,21411
} cip_ll_dev;cip_ll_dev606,21688
} cip_ll_pa;cip_ll_pa612,21935
} nd;nd632,22750
#define ND_VALID 633,22756
#define ND_VAL_NOT_CURRENT 634,22805
#define ND_NOT_VALID 635,22870
#define CLASS_DIRECT 636,22923
#define CLASS_CTCA 637,22955
#define CLASS_SWITCH 638,23007
} cip_totals;cip_totals650,23282
#define TOT_DEV 651,23296
#define TOT_PATH 652,23344
#define TOT_DB 653,23392
#define NUM_TOTALS 654,23448
struct cta_tt_cmd cta_tt_cmd660,23540
typedef struct cta_tt_cmd cta_tt_cmdtype;cta_tt_cmdtype669,23739
#define FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO675,23849
#define FOR_ALL_VCS_ON_CIP_IDB_INFO(FOR_ALL_VCS_ON_CIP_IDB_INFO682,24081
#define CHANNEL_ENCAPS_LOVE 690,24295
#define CHANNEL_ENCAPS_CFGACK 691,24374
#define CHANNEL_ENCAPS_IPC 692,24457
#define CHANNEL_ENCAPS_VLAN 693,24532
#define SEGMENT_NAME_MAX_LENGTH 697,24616
#define COMPILE_INFO_MAX_LENGTH 698,24651
typedef struct cip_appl_version_ cip_appl_version_703,24716
}cip_appl_version_t;cip_appl_version_t707,24857
#define CIP_APPL_VERSION_SIZE 709,24879
typedef struct cip_version_ cip_version_715,24975
} cip_version_t;cip_version_t720,25109
