
ospf.h,9643
#define MAXOSPFPACKETSIZE 164,5857
#define MAX_OSPF_DATA 165,5888
#define OSPF_VERSION 166,5968
#define OSPF_RUN_TIME 167,5992
#define OSPF_SCAN_RUN_TIME 168,6043
#define OSPF_SCAN_INTERVAL 169,6097
#define INIT_OSPF_SUSPEND(INIT_OSPF_SUSPEND170,6136
#define OSPF_SHOULD_SUSPEND(OSPF_SHOULD_SUSPEND171,6203
#define LSA_PROCESSCOUNT 173,6304
#define OSPF_AUTH_NONE	175,6333
#define OSPF_AUTH_CT	176,6359
#define OSPF_AUTH_MD	177,6383
#define OSPF_MD5	179,6408
#define MAX_BAD_CKSUM_QUEUE_SIZE	181,6429
typedef struct l_link_ l_link_183,6466
} l_link;l_link186,6542
typedef struct l_queue_ l_queue_188,6553
} l_queue;l_queue192,6630
typedef struct ospf_bad_cksum_ ospf_bad_cksum_194,6642
} ospf_bad_cksum;ospf_bad_cksum200,6962
typedef struct ospf_md_key_ ospf_md_key_202,6985
} ospf_md_key;ospf_md_key210,7272
typedef struct lsdb_list_ lsdb_list_212,7288
} lsdb_list;lsdb_list222,7638
struct list_queue_ list_queue_224,7652
    OSPF_SUMUPDATE_TIMER,231,7753
    OSPF_EXUPDATE_TIMER,232,7779
    OSPF_EXLOCK_TIMER,233,7804
    OSPF_FADDR_TIMER,234,7827
    OSPF_AR_REBUILD_RTR_TIMER,235,7849
    OSPF_AR_EXLOCK_TIMER,236,7880
    OSPF_AR_SUMLOCK_TIMER,237,7906
    OSPF_AR_DELAY_TIMER,238,7933
    OSPF_AR_UPDATE_TIMER,239,7958
    OSPF_AR_AGE_TIMER,240,7984
    OSPF_IF_HELLO_TIMER,241,8007
    OSPF_IF_WAIT_TIMER,242,8032
    OSPF_IF_DELAY_TIMER,243,8056
    OSPF_IF_ACK_TIMER,244,8081
    OSPF_IF_FLUSH_NET_TIMER,245,8104
    OSPF_NBR_DEAD_TIMER,246,8133
    OSPF_NBR_DBD_RETRANS_TIMER,247,8158
    OSPF_NBR_DBD_HOLD_TIMER,248,8190
    OSPF_NBR_POLL_TIMER,249,8219
    OSPF_NBR_LSA_RETRANS_TIMER,250,8244
struct ospf_idb_info_ ospf_idb_info_253,8280
struct ospf_pdb_info_ ospf_pdb_info_313,11405
struct ospf_worktype_ ospf_worktype_343,12860
#define OSPF_REDIST_ROUTE	353,13022
#define OSPF_TRANSLATE_TYPE7	354,13055
#define NON_INDICATION_LSA_COUNT(NON_INDICATION_LSA_COUNT359,13168
#define OSPF_ADV_INDICATION	365,13334
#define OSPF_REMOVE_INDICATION	366,13364
typedef struct listtype_ listtype_368,13398
} listype;listype373,13530
typedef struct faddrtype_ faddrtype_375,13542
} faddrtype;faddrtype378,13623
typedef struct ls_req_ ls_req_380,13637
} ls_req_list;ls_req_list390,13911
#define	REQ_INQUEUE	392,13927
#define REQ_SENT	393,13949
struct nbrtype_ nbrtype_395,13969
#define	NBR_DOWN	433,15456
#define	NBR_ATTEMPT	434,15475
#define	NBR_INIT	435,15497
#define	NBR_2WAY	436,15516
#define	NBR_EXSTART	437,15535
#define	NBR_EXCHANGE	438,15557
#define	NBR_LOADING	439,15580
#define	NBR_FULL	440,15602
#define	NBR_SCVIRTUAL	441,15621
#define NBR_STATES	442,15645
#define	NBR_EVENT_HELLO_RCV	447,15731
#define	NBR_EVENT_START	448,15761
#define	NBR_EVENT_2WAY	449,15788
#define	NBR_EVENT_ADJ_OK	450,15814
#define	NBR_EVENT_NEGO_DONE	451,15841
#define	NBR_EVENT_EXCH_DONE	452,15871
#define	NBR_EVENT_SEQ_MISMATCH	453,15901
#define	NBR_EVENT_BAD_LS_REQ	454,15934
#define	NBR_EVENT_LOAD_DONE	455,15965
#define	NBR_EVENT_1WAY	456,15995
#define	NBR_EVENT_RST_ADJ	457,16021
#define	NBR_EVENT_KILL_NBR	458,16050
#define	NBR_EVENT_INACT_TIMER	459,16080
#define	NBR_EVENT_LLDOWN	460,16113
#define NBR_EVENTS	461,16141
#define START_REDIST	463,16183
#define	NBR_ADJ_SLAVE	467,16231
#define	NBR_ADJ_MASTER	468,16256
#define	NBR_ADJ_SLAVE_HOLD	469,16282
static inline boolean ospf_no_req 471,16312
#define NO_REQ(NO_REQ481,16677
static inline boolean ospf_req_to_send 483,16713
#define REQ_TO_SEND(REQ_TO_SEND499,17386
#define ALL_UP_NBRS 504,17469
#define ALL_ELIG_NBRS 505,17500
#define ALL_EXCH_NBRS 506,17531
#define ALL_VIRTUAL_NBRS 507,17562
#define DR_BDR_NBRS 508,17593
#define NBRF_CONFIGURED 513,17645
#define NBRF_NEW	514,17676
typedef struct addr_range_ addr_range_520,17774
} addr_range;addr_range525,17912
struct areatype_ areatype_528,17930
#define	DELAY_RTR_LSA	592,21419
#define	DELAY_NET_LSA	593,21456
#define	DELAY_RTR_LSA_UPDATE 594,21493
#define	DELAY_NET_LSA_UPDATE	595,21532
#define	DELAY_INTRA_NET	597,21569
#define	DELAY_INTER_NET	598,21596
#define	DELAY_INTRA_ASB 599,21623
typedef struct vlink_ vlink_603,21691
} vlinktype;vlinktype609,21884
typedef struct ospf_rtr_path_ ospf_rtr_path_611,21898
} ospf_rtr_path;ospf_rtr_path624,22250
typedef struct rtr_route_ rtr_route_626,22268
} rtr_route;rtr_route631,22416
struct ospfbackuptype_ ospfbackuptype_638,22542
#define	DEFAULT_MAP_METRIC	646,22832
#define	DEFAULT_MAP_METRIC_CONNECTED	647,22863
#define	DTYPE_NET	650,22926
#define	DTYPE_ABR	651,22951
#define	DTYPE_ASBR	652,22976
#define REDIST_LOST	655,23019
#define OSPF_SOFT_RESET	656,23047
#define OSPF_MAXAGEQ	657,23079
#define OSPF_ID_CHANGING	658,23108
#define OSPF_CLEANING_UP	659,23140
#define OSPF_IF_JUMPSTART	660,23172
#define OSPF_SCHD_SPF	661,23205
#define OSPF_IMMEDIATE_SPF	662,23235
#define OSPF_FLUSH_DO_NOT_AGE	663,23269
#define OSPF_CHECK_INDICATION	664,23306
#define OSPF_CHECK_ABR	665,23343
#define MAXAGE	668,23395
#define MAXDIFFAGE	669,23436
#define BACKBONE_AREA_ID	670,23489
#define TOS0_IF_COST_ASYNC	671,23516
#define TOS0_IF_COST_X25	672,23549
#define TOS0_IF_COST_SERIAL	673,23579
#define TOS0_IF_COST_ETHER	674,23612
#define TOS0_IF_COST_TR	675,23642
#define TOS0_IF_COST_TR16	676,23670
#define TOS0_IF_COST_FDDI	677,23698
#define TOS0_IF_COST_ATM	678,23726
#define STUB_DEFAULT_COST	680,23815
#define STUB_HOST_COST	681,23843
#define HELLO_INTERVAL	683,23870
#define HELLO_INTERVAL_NBMA	684,23915
#define DEFAULT_PRIORITY	685,23964
#define DEAD_INTERVAL	687,23992
#define DEAD_INTERVAL_NBMA	688,24036
#define WAIT_INTERVAL	689,24089
#define POLL_INTERVAL	690,24133
#define RTR_INTERVAL	691,24182
#define TRANSDELAY	692,24206
#define MINLSINTERVAL	693,24228
#define FLUSH_NET_INTERVAL	694,24253
#define REBUILD_RTR_INTERVAL	695,24293
#define MAXAGEQ_SCAN_INTERVAL	696,24356
#define LS_REFRESH_TIME	697,24389
#define AGE_TIME 698,24441
#define ACK_PACKET_LIMIT_NUM	699,24487
#define DELAYED_ACK_INTERVAL	700,24549
#define SPF_HOLD_TIME	701,24595
#define MAX_EX_LSA_PAK	702,24645
#define MAX_EX_PAK_RATE	703,24690
#define MIN_ABR_AREA_COUNT 704,24742
#define SPF_SCHD_DELAY 705,24819
#define SPF_HOLD_INTERVAL 706,24894
#define FADDR_INTERVAL 708,24983
#define SCANNING_HOLD_TIME	710,25087
#define OSPF_MAX_QDEPTH	712,25175
#define OSPF_LSDB_LIST_CHUNKSIZE	718,25298
#define OSPF_NBR_LIST_CHUNKSIZE	719,25334
#define OSPF_WORK_CHUNKSIZE	720,25370
#define HOLD_TIMER_CHECK(HOLD_TIMER_CHECK726,25487
#define SPF_HOLD_EXPIRED(SPF_HOLD_EXPIRED733,25678
#define OSPF_DATA_PTR(OSPF_DATA_PTR736,25802
#define VALID_IPADDRESS(VALID_IPADDRESS739,25905
#define ospf_buginf	741,25963
#define ospf_adj_buginf	742,26014
#define ospf_flood_buginf	743,26087
#define ospf_flood_acl_buginf	744,26142
#define ospf_flood_adj_buginf	745,26204
#define ospf_spec_buginf	746,26281
#define ospf_adj_spec_buginf	747,26341
#define ospf_lsa_gen_buginf	749,26433
#define ospf_packet_buginf	750,26492
#define ospf_retrans_buginf	751,26549
#define ospf_spf_intra_buginf	752,26608
#define ospf_spf_inter_buginf	753,26671
#define ospf_spf_ext_buginf	754,26734
#define ospf_spf_buginf	755,26793
#define ospf_spf_intra_acl_buginf	758,26917
#define ospf_spf_inter_acl_buginf	759,26987
#define ospf_spf_ext_acl_buginf	760,27057
#define ospf_spf_acl_buginf	761,27124
#define EXLSA_COUNT 778,27577
#define EXLSA_CHKSUMSUM 779,27604
#define ORNLSA_COUNT 780,27631
#define RCNLSA_COUNT 781,27658
#define OSPF_PDB_MIB_COUNT 783,27686
#define MIB_TABLE(MIB_TABLE785,27716
#define PDB_INC_MIB_TABLE(PDB_INC_MIB_TABLE787,27757
#define PDB_DEC_MIB_TABLE(PDB_DEC_MIB_TABLE789,27892
#define PDB_ADD_MIB_TABLE(PDB_ADD_MIB_TABLE791,28027
#define PDB_SUB_MIB_TABLE(PDB_SUB_MIB_TABLE793,28175
#define SPF_TIME_MAX 798,28362
#define SPF_PROC_TIME_MAX 799,28386
#define ADD_EX_TIME	800,28416
#define PROCESS_ASE_TIME 801,28439
#define PROCESS_SUM_TIME	802,28467
typedef struct _spf_run_time _spf_run_time804,28496
} spfruntimetype;spfruntimetype808,28631
typedef struct _process_time _process_time810,28650
} processtimetype;processtimetype812,28701
typedef struct _write_time _write_time814,28721
} spfstattimetype;spfstattimetype818,28835
} ospf_event_code;ospf_event_code876,29784
typedef struct ospf_event_type_ ospf_event_type_878,29804
} ospf_event_type;ospf_event_type883,30032
#define OSPF_IDB_UP 888,30106
#define OSPF_IDB_DOWN 889,30128
#define	IF_BROADCAST	894,30190
#define	IF_NONBROADCAST	895,30214
#define	IF_POINT_TO_POINT 896,30241
#define	IF_POINT_TO_M_POINT 897,30271
#define	IF_VIRTUAL_LINK 898,30302
#define	IF_LOOPBACK 899,30332
#define	IFS_DOWN	904,30408
#define	IFS_LOOPBACK	905,30428
#define	IFS_WAITING 906,30452
#define	IFS_POINT_TO_M_POINT 907,30482
#define	IFS_POINT_TO_POINT 908,30514
#define	IFS_DR 909,30544
#define	IFS_BACKUP 910,30574
#define	IFS_OTHER 911,30604
#define IF_STATES 912,30634
#define	IFE_UP 918,30715
#define	IFE_WAIT_TIMER 919,30744
#define	IFE_BACKUP_SEEN 920,30773
#define	IFE_NBRCHANGE 921,30802
#define	IFE_LOOP_IND 922,30831
#define	IFE_UNLOOP_IND 923,30860
#define	IFE_DOWN 924,30889
#define IF_EVENTS 925,30918
#define OSPF_DEF_REFERENCE_BW	933,31156
#define OSPF_MAX_REFERENCE_BW	934,31220
#define OSPF_METRIC_SCALE	935,31289
typedef struct ospf_traffic_t_ ospf_traffic_t_939,31385
} ospf_traffic_t;ospf_traffic_t948,31589
typedef boolean (*ospf_flush_function)ospf_flush_function951,31644
typedef boolean (*ospf_flush_function)(pdbtype *, areatype *, lsdbtype lsdbtype951,31644
