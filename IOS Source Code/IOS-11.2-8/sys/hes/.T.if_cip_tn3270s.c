
if_cip_tn3270s.c,18558
#define DEBUG_TN3270S 148,4788
typedef struct tn3270s_max_lu_ tn3270s_max_lu_156,5027
} tn3270s_max_lu_t;tn3270s_max_lu_t160,5129
typedef struct tn3270s_timing_mark_ tn3270s_timing_mark_162,5150
} tn3270s_timing_mark_t;tn3270s_timing_mark_t165,5231
typedef struct siftdown_ siftdown_167,5257
} siftdown_t;siftdown_t170,5338
typedef struct tn3270s_siftdown_ tn3270s_siftdown_172,5353
} tn3270s_siftdown_t;tn3270s_siftdown_t179,5550
typedef struct tn3270s_lu_dns_ tn3270s_lu_dns_181,5573
} tn3270s_lu_dns_t;tn3270s_lu_dns_t186,5741
typedef struct tn3270s_svr_t_ tn3270s_svr_t_188,5762
} tn3270s_svr_t;tn3270s_svr_t195,5993
typedef struct tn3270s_pu_dns_ tn3270s_pu_dns_197,6011
} tn3270s_pu_dns_t;tn3270s_pu_dns_t201,6131
    AVL_PU_ASCENDING	204,6159
    AVL_PU_DESCENDING	205,6185
    NUM_PU_AVL_THREADS206,6212
enum tn3270s_putype_t_ tn3270s_putype_t_209,6239
    TN3270S_PUTYPE_DIRECT,210,6264
    TN3270S_PUTYPE_DLUR211,6291
typedef enum tn3270s_putype_t_ tn3270s_putype_t;tn3270s_putype_t213,6318
struct tn3270s_pu_ tn3270s_pu_215,6368
struct tn3270s_direct_pu_t_ tn3270s_direct_pu_t_236,7002
struct tn3270s_dlur_pu_ tn3270s_dlur_pu_240,7067
typedef struct tn3270s_dlus_ tn3270s_dlus_244,7128
} tn3270s_dlus_t;tn3270s_dlus_t248,7246
typedef struct tn3270s_NNsvr_ tn3270s_NNsvr_250,7265
} tn3270s_NNsvr_t;tn3270s_NNsvr_t254,7384
typedef struct tn3270s_vrn_ tn3270s_vrn_256,7404
} tn3270s_vrn_t;tn3270s_vrn_t260,7521
    AVL_LINK_NAME_ASCENDING	263,7546
    AVL_LINK_SAPMAC_ASCENDING	264,7579
    NUM_LINK_AVL_THREADS265,7614
typedef struct tn3270s_link_ tn3270s_link_268,7643
} tn3270s_link_t;tn3270s_link_t279,8062
    AVL_SAP_ASCENDING	282,8088
    AVL_SAP_DESCENDING	283,8115
    NUM_SAP_AVL_THREADS284,8143
typedef struct tn3270s_dlur_sap_ tn3270s_dlur_sap_287,8171
} tn3270s_dlur_sap_t;tn3270s_dlur_sap_t295,8451
typedef struct tn3270s_dlur_ tn3270s_dlur_297,8474
} tn3270s_dlur_t;tn3270s_dlur_t307,8887
typedef struct tn3270s_ipvc_t_ tn3270s_ipvc_t_309,8906
} tn3270s_ipvc_t;tn3270s_ipvc_t315,9093
#define TN3270S_MIN_VC 322,9275
#define TN3270S_MAX_VC 323,9302
#define TN3270S_NUM_VC 324,9329
#define TN3270S_NO_VC 325,9390
typedef struct tn3270s_ tn3270s_326,9432
} tn3270s_t;tn3270s_t335,9773
#define TN3270S_CORRELATOR_HUH 343,9985
#define TN3270S_CORRELATOR_INIT 344,10019
#define TN3270S_SNMP_INDEX_HUH 346,10054
#define TN3270S_SNMP_INDEX_INIT 347,10088
#define TN3270S_STATE_UNUSED 352,10153
#define TN3270S_STATE_INUSE 353,10234
#define TN3270S_STATE_CFGING 354,10315
#define TN3270S_STATE_CREATE 355,10396
#define TN3270S_STATE_UPDATE 356,10477
#define TN3270S_STATE_DELETE 357,10558
#define TN3270S_STATE_PNDING 358,10639
#define TN3270S_STATE_ACKED 359,10720
#define TN3270S_STATE_VC_SR 360,10801
#define TN3270S_STATE_SHUT 361,10882
#define TN3270S_STATE_CIP_SHUT 362,10963
#define UDB_TN3270S_CONTEXT 369,11208
#define CSB_PTR_TN3270S 370,11275
#define UDB_SVR 372,11343
#define UDB_PU 373,11362
#define UDB_DLUR 374,11383
#define UDB_DLUR_PU 375,11404
#define UDB_DLUR_SAP 376,11427
#define TN3270S_MAX_SUBMODES 377,11450
#define CSB_PTR_SVR 380,11533
#define CSB_PTR_PU 381,11556
#define CSB_PTR_DLUR 382,11581
#define CSB_PTR_DLUR_PU 383,11606
#define CSB_PTR_DLUR_SAP 384,11633
#define TN3270S_SVR_PROMPT_BASE 386,11661
#define TN3270S_PU_PROMPT_BASE 387,11711
#define TN3270S_DLUR_PROMPT_BASE 388,11760
#define TN3270S_DLUR_PU_PROMPT_BASE 389,11811
#define TN3270S_DLUR_SAP_PROMPT_BASE 390,11865
struct prompt_t_ prompt_t_395,11977
typedef struct prompt_t_ prompt_t;prompt_t398,12037
#define MAX_MACADDR_STRING_SIZE 426,12885
typedef enum lu_detail lu_detail432,13114
}lu_detail;lu_detail436,13230
wavl_to_pu 466,14082
pu_to_wavl 472,14185
compare_puname_asc 483,14452
compare_puname_desc 501,14884
tn3270s_pu_tree_create 519,15306
tn3270s_pu_entry_create 529,15531
tn3270s_pu_entry_delete 547,15927
tn3270s_pu_tree_destroy 560,16151
tn3270s_pu_entry_get_first 574,16500
tn3270s_pu_entry_get_next 587,16767
tn3270s_pu_entry_get_last 598,17021
tn3270s_pu_entry_get_prev 608,17225
tn3270s_pu_entry_locate_by_name 624,17656
wavl_to_sap 643,18137
sap_to_wavl 649,18247
compare_sap_asc 664,18631
compare_sap_desc 687,19198
tn3270s_sap_tree_create 710,19755
tn3270s_sap_entry_create 724,20039
tn3270s_sap_entry_delete 751,20657
tn3270s_sap_tree_destroy 770,21024
tn3270s_sap_entry_get_first 788,21474
tn3270s_sap_entry_get_next 803,21820
tn3270s_sap_entry_get_last 819,22230
tn3270s_sap_entry_get_prev 834,22578
tn3270s_sap_entry_locate 855,23158
wavl_to_link 883,23846
link_to_wavl 889,23953
compare_link_name_asc 900,24230
compare_link_sapmac_asc 918,24691
tn3270s_link_tree_create 951,25510
tn3270s_link_entry_create 966,25810
tn3270s_link_entry_delete 1000,26646
tn3270s_link_tree_destroy 1019,27013
tn3270s_link_entry_get_first_by_name 1037,27472
tn3270s_link_entry_get_next_by_name 1052,27831
tn3270s_link_entry_get_first_by_sapmac 1069,28255
tn3270s_link_entry_get_next_by_sapmac 1084,28610
enum tn3270s_link_context_t_ tn3270s_link_context_t_1100,29016
    TN3270S_LINK_CONTEXT_ALL,1101,29047
    TN3270S_LINK_CONTEXT_THIS_SAP1102,29077
typedef enum tn3270s_link_context_t_ tn3270s_link_context_t;tn3270s_link_context_t1104,29114
tn3270s_link_entry_get_first 1107,29200
tn3270s_link_entry_get_next 1135,29952
tn3270s_link_entry_locate_by_name 1165,30714
tn3270s_link_entry_locate_by_rmac 1183,31164
tn3270s_ipvc_entry_reset 1212,31929
tn3270s_ipvc_create 1221,32095
tn3270s_ipvc_destroy 1234,32350
tn3270s_ipvc_entry_create 1239,32426
tn3270s_ipvc_entry_locate_by_ipaddr 1257,32773
tn3270s_ipvc_entry_locate_by_vc 1269,33043
tn3270s_ipvc_entry_add_vc_user 1281,33281
tn3270s_ipvc_entry_del_vc_user 1294,33510
tn3270s_ipvc_entry_add_sr_user 1310,33807
tn3270s_ipvc_entry_del_sr_user 1335,34462
tn3270s_pu_add_sr 1354,34876
tn3270s_pu_del_sr 1368,35190
tn3270s_ipvc_assign_vc 1383,35565
tn3270s_pu_entry_locate_by_xsap_xmac 1418,36520
tn3270s_pu_entry_locate_by_xsap 1450,37389
tn3270s_pu_entry_locate_by_snmp_index 1475,38049
tn3270s_sap_entry_locate_by_xsap_xmac 1502,38793
tn3270s_svr_create 1562,40459
tn3270s_svr_delete 1577,40825
tn3270s_svr_init 1591,41114
tn3270s_pu_init 1631,42416
tn3270s_dlur_create 1663,43469
tn3270s_dlur_delete 1682,43874
tn3270s_dlur_init 1699,44200
static void  tn3270s_dlur_sap_init 1721,44768
tn3270s_link_init 1735,45188
tn3270s_query_shut 1749,45623
tn3270s_query_shut_svr 1757,45768
tn3270s_query_cipshut_svr 1763,45945
tn3270s_query_shut_dlur 1769,46104
tn3270s_query_cipshut_dlur 1775,46283
tn3270s_query_shut_pu 1781,46444
tn3270s_query_cipshut_pu 1787,46609
tn3270s_query_shut_dlur_sap 1793,46756
tn3270s_query_cipshut_dlur_sap 1799,46935
enum tn3270s_cfg_t_ tn3270s_cfg_t_1812,47296
    TN3270S_CFGTYPE_CREATE,1813,47318
    TN3270S_CFGTYPE_UPDATE,1814,47346
    TN3270S_CFGTYPE_NOSHUT,1815,47374
    TN3270S_CFGTYPE_SHUT,1816,47402
    TN3270S_CFGTYPE_DELETE1817,47428
typedef enum tn3270s_cfg_t_ tn3270s_cfg_t;tn3270s_cfg_t1819,47458
#define OFFSET_CFG_CMDCODE 1821,47502
tn3270s_cfgcmd 1823,47543
#define OFFSET_CFG_CMDCODE 1991,52326
#define OFFSET_CFG_SVR_CFG_CORRELATOR 1992,52370
#define OFFSET_CFG_SVR_RESET_CORRELATOR 1993,52414
#define CFG_CMDBLOCK_SIZE 1995,52484
tn3270s_cfgcmd_svr 1997,52540
#define OFFSET_CFG_CMDCODE 2045,53962
#define OFFSET_CFG_SVR_MAX_LU_CFG_MAX_LU 2046,54013
#define OFFSET_CFG_SVR_MAX_LU_CFG_CORRELATOR 2048,54065
#define OFFSET_CFG_SVR_MAX_LU_RESET_CORRELATOR 2049,54116
#define CFG_CMDBLOCK_SIZE 2051,54193
tn3270s_cfgcmd_svr_max_lu(2054,54257
#define OFFSET_CFG_CMDCODE 2092,55529
#define OFFSET_CFG_SVR_TIMING_MARK_CFG_TIMING_MARK 2093,55580
#define OFFSET_CFG_SVR_TIMING_MARK_CFG_CORRELATOR 2095,55642
#define OFFSET_CFG_SVR_TIMING_MARK_RESET_CORRELATOR 2096,55698
#define CFG_CMDBLOCK_SIZE 2098,55780
tn3270s_cfgcmd_svr_timing_mark(2101,55844
#define OFFSET_CFG_CMDCODE 2143,57143
#define OFFSET_CFG_SVR_LU_DNS_FQDN 2144,57194
#define OFFSET_CFG_SVR_LU_REV_DNS_FQDN 2145,57245
#define OFFSET_CFG_SVR_LU_DNS_CFG_CORRELATOR 2146,57296
#define OFFSET_CFG_SVR_LU_DNS_RESET_CORRELATOR 2147,57347
#define CFG_CMDBLOCK_SIZE 2149,57424
tn3270s_cfgcmd_svr_lu_dns 2152,57488
#define OFFSET_CFG_CMDCODE 2205,59159
#define OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT_FLAG 2206,59211
#define OFFSET_CFG_SVR_SIFTDOWN_TCP_PORT 2207,59263
#define OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME_FLAG 2208,59315
#define OFFSET_CFG_SVR_SIFTDOWN_IDLE_TIME 2209,59367
#define OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE_FLAG 2210,59419
#define OFFSET_CFG_SVR_SIFTDOWN_KEEPALIVE 2211,59471
#define OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT_FLAG 2212,59524
#define OFFSET_CFG_SVR_SIFTDOWN_UNBIND_ACT 2213,59577
#define OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL_FLAG 2214,59630
#define OFFSET_CFG_SVR_SIFTDOWN_GENERIC_POOL 2215,59683
#define OFFSET_CFG_SVR_SIFTDOWN_CFG_CORRELATOR 2216,59736
#define OFFSET_CFG_SVR_SIFTDOWN_RESET_CORRELATOR 2217,59789
#define CFG_CMDBLOCK_SIZE 2219,59868
#define SIFTDOWN_NO_CHANGE 2221,59922
#define SIFTDOWN_USE_VALUE 2222,59955
#define SIFTDOWN_APPLY_DEFAULT 2223,59988
tn3270s_cfgcmd_svr_siftdown(2226,60034
#define OFFSET_CFG_CMDCODE 2311,62942
#define OFFSET_CFG_PU_NEW_PUNAME 2312,62989
#define OFFSET_CFG_PU_NEW_INDEX 2313,63036
#define OFFSET_CFG_PU_NEW_XID 2314,63083
#define OFFSET_CFG_PU_NEW_IPADDR 2315,63130
#define OFFSET_CFG_PU_NEW_VC 2316,63177
#define OFFSET_CFG_PU_NEW_LANTYPE 2319,63261
#define OFFSET_CFG_PU_NEW_ADAPNO 2320,63309
#define OFFSET_CFG_PU_NEW_LSAP 2321,63357
#define OFFSET_CFG_PU_NEW_RMAC 2322,63405
#define OFFSET_CFG_PU_NEW_RSAP 2323,63453
#define OFFSET_CFG_PU_NEW_LUSEED 2324,63500
#define OFFSET_CFG_PU_SIFTDOWN_DIRECT 2326,63548
#define OFFSET_CFG_PU_SIFTDOWN_DLUR 2327,63608
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT_FLAG 2329,63669
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_TCP_PORT 2330,63729
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME_FLAG 2331,63789
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_IDLE_TIME 2332,63849
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE_FLAG 2333,63909
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_KEEPALIVE 2334,63969
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT_FLAG 2335,64029
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_UNBIND_ACT 2336,64089
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL_FLAG 2337,64149
#define REL_OFFSET_CFG_PU_NEW_SIFTDOWN_GENERIC_POOL 2338,64209
#define REL_OFFSET_CFG_PU_NEW_DNS_FQDN 2340,64270
#define REL_OFFSET_CFG_PU_NEW_CFG_CORRELATOR 2342,64323
#define REL_OFFSET_CFG_PU_NEW_RESET_CORRELATOR 2343,64375
#define CFG_CMDBLOCK_SIZE 2345,64453
tn3270s_cfgcmd_pu(2348,64551
#define OFFSET_CFG_CMDCODE 2479,68824
#define OFFSET_CFG_DLUR_CPNAME 2480,68871
#define OFFSET_CFG_DLUR_DLUSNAME 2481,68918
#define OFFSET_CFG_DLUR_CORRELATOR_FIELD 2483,68966
#define OFFSET_CFG_NO_DLUR_CORRELATOR_FIELD 2484,69013
#define REL_OFFSET_CFG_DLUR_CFG_CORRELATOR 2486,69061
#define REL_OFFSET_CFG_DLUR_RESET_CORRELATOR 2487,69111
#define CFG_CMDBLOCK_SIZE 2489,69187
tn3270s_cfgcmd_dlur(2492,69247
#define OFFSET_CFG_CMDCODE 2555,71090
#define OFFSET_CFG_DLUR_DLUS_NAME 2556,71140
#define OFFSET_CFG_DLUR_DLUS_CFG_CORRELATOR 2558,71192
#define OFFSET_CFG_DLUR_DLUS_RESET_CORRELATOR 2559,71242
#define CFG_CMDBLOCK_SIZE 2561,71318
tn3270s_cfgcmd_dlur_dlus(2564,71381
#define OFFSET_CFG_CMDCODE 2606,72671
#define OFFSET_CFG_DLUR_NN_SVR_NAME 2607,72723
#define OFFSET_CFG_DLUR_NN_SVR_CFG_CORRELATOR 2609,72777
#define OFFSET_CFG_DLUR_NN_SVR_RESET_CORRELATOR 2610,72829
#define CFG_CMDBLOCK_SIZE 2612,72907
tn3270s_cfgcmd_dlur_nn_svr(2615,72972
#define OFFSET_CFG_CMDCODE 2663,74436
#define OFFSET_CFG_DLUR_SAP_LANTYPE 2664,74485
#define OFFSET_CFG_DLUR_SAP_ADAPNO 2665,74535
#define OFFSET_CFG_DLUR_SAP_LSAP 2666,74585
#define OFFSET_CFG_DLUR_SAP_CFG_CORRELATOR 2668,74636
#define OFFSET_CFG_DLUR_SAP_RESET_CORRELATOR 2669,74686
#define CFG_CMDBLOCK_SIZE 2671,74762
tn3270s_cfgcmd_dlur_sap(2674,74824
#define OFFSET_CFG_CMDCODE 2731,76591
#define OFFSET_CFG_SAP_VRN_LANTYPE 2732,76640
#define OFFSET_CFG_SAP_VRN_ADAPNO 2733,76689
#define OFFSET_CFG_SAP_VRN_LSAP 2734,76738
#define OFFSET_CFG_SAP_VRN_NAME 2735,76787
#define OFFSET_CFG_SAP_VRN_CFG_CORRELATOR 2737,76836
#define OFFSET_CFG_SAP_VRN_RESET_CORRELATOR 2738,76885
#define CFG_CMDBLOCK_SIZE 2740,76959
tn3270s_cfgcmd_dlur_sap_vrn(2743,77020
#define OFFSET_CFG_CMDCODE 2798,78805
#define OFFSET_CFG_SAP_LINK_LANTYPE 2799,78854
#define OFFSET_CFG_SAP_LINK_ADAPNO 2800,78904
#define OFFSET_CFG_SAP_LINK_LSAP 2801,78954
#define OFFSET_CFG_SAP_LINK_NAME 2802,79004
#define OFFSET_CFG_SAP_LINK_RMAC 2803,79053
#define OFFSET_CFG_SAP_LINK_RSAP 2804,79102
#define OFFSET_CFG_SAP_LINK_CFG_CORRELATOR 2806,79152
#define OFFSET_CFG_SAP_LINK_RESET_CORRELATOR 2807,79202
#define CFG_CMDBLOCK_SIZE 2809,79278
tn3270s_cfgcmd_dlur_sap_link(2812,79341
tn3270s_is_loopback_mac 2881,81475
tn3270s_loopback_mac 2887,81579
tn3270s_rmac_string 2893,81672
tn3270s_lan_type_string 2909,82158
tn3270s_port_type_string 2929,82705
tn3270s_name_from_fqname 2961,83404
#define BLANKS 3008,84755
typedef void (*output_t)output_t3009,84776
tn3270s_print_state_flags 3012,84834
tn3270s_buginf_svr_state_flags 3032,85579
tn3270s_buginf_pu_state_flags 3042,85792
tn3270s_buginf_dlur_state_flags 3053,86012
tn3270s_buginf_sap_show 3065,86297
tn3270s_reset_this_link_cfg 3083,86638
tn3270s_reset_all_link_cfg 3103,87195
tn3270s_reset_this_sap_cfg 3127,87901
tn3270s_reset_all_sap_cfg 3149,88575
tn3270s_reset_this_pu_cfg 3171,89158
tn3270s_reset_all_pu_cfg 3200,90025
tn3270s_reset_all_dlur_cfg 3224,90666
tn3270s_reset_all_svr_cfg 3259,91680
tn3270s_send_this_link_cfg 3278,92302
tn3270s_send_all_link_cfg 3294,92750
tn3270s_send_this_sap_cfg 3315,93405
tn3270s_send_all_sap_cfg 3337,94072
tn3270s_send_this_pu_cfg 3355,94546
tn3270s_send_all_pu_cfg 3373,95098
tn3270s_send_all_dlur_cfg 3395,95688
tn3270s_send_all_svr_cfg 3434,96782
tn3270s_delete_this_link_cfg 3479,98228
tn3270s_delete_all_link_cfg 3493,98588
tn3270s_delete_this_sap_cfg 3517,99306
tn3270s_delete_all_sap_cfg 3533,99746
tn3270s_delete_this_pu_cfg 3554,100336
tn3270s_delete_all_pu_cfg 3570,100863
tn3270s_delete_all_dlur_cfg 3594,101508
tn3270s_delete_all_svr_cfg 3618,102132
tn3270s_cfg_identify 3646,103057
tn3270s_cfg_correlator 3714,105209
tn3270s_cfg_accepted 3776,106991
tn3270s_nack_report 3847,108922
tn3270s_cfg_rejected 3884,109831
tn3270s_del_accepted 3983,113047
tn3270s_del_rejected 4067,115384
tn3270s_unexpected_cfg_ack 4095,116088
tn3270s_cfg_acknowledge 4130,117267
tn3270s_default 4273,121148
tn3270s_shut_state_change 4304,121835
#define STRING_PARAM_UPPERCASE 4338,122895
#define STRING_PARAM_VERIFY_LEN 4339,122934
tn3270s_string_param 4341,122988
tn3270s_svr_reconfig 4365,123553
tn3270s_svr 4400,124575
tn3270s_shut 4462,126486
tn3270s_svr_max_lu 4583,129777
tn3270s_svr_timing_mark 4630,131287
tn3270s_svr_lu_dns 4657,132093
tn3270s_siftdown 4724,133952
tn3270s_svr_pu_reconfig 4850,137746
tn3270s_svr_pu 4930,140232
tn3270s_pu_dns 5106,144880
tn3270s_dlur_reconfig 5155,146136
tn3270s_dlur 5204,147458
tn3270s_dlur_dlus 5282,149662
tn3270s_dlur_nn_svr 5328,150846
tn3270s_dlur_pu_reconfig 5373,151981
tn3270s_dlur_pu 5416,153163
tn3270s_dlur_sap_reconfig 5524,156056
tn3270s_dlur_sap 5584,158069
tn3270s_dlur_sap_vrn 5696,161139
tn3270s_dlur_sap_link 5761,163302
tn3270s_nvgen 5870,166232
tn3270s_command 6118,173540
tn3270s_subcommand 6236,176853
tn3270s_exit_subcommand 6333,179544
tn3270s_set_vars 6425,181909
tn3270s_reset_vars 6487,183414
tn3270s_query_next_link 6530,184409
tn3270s_query_this_link 6569,185367
tn3270s_query_this_sap 6613,186456
tn3270s_query_next_client_lu 6657,187603
tn3270s_query_pu 6685,188208
tn3270s_query_this_pu 6711,188742
tn3270s_query_this_dlur 6765,190746
tn3270s_query_this_svr 6809,192029
tn3270s_state_string 6851,193736
tn3270s_dlur_status_string 6878,194316
tn3270s_linksap_status_string 6903,194985
Dlur_status_string 6926,195657
tn3270s_pu_status_string 6949,196182
tn3270s_show_this_link_cfg 7002,197790
tn3270s_show_all_link_cfg 7063,199065
tn3270s_show_this_sap_cfg 7164,202024
tn3270s_show_all_sap_cfg 7223,203764
static const char* GetLuStatus 7240,204191
enum show_state_t_ show_state_t_7276,204993
    DISC_CLIENT 7277,205014
    DISC_CLIENT = TN3270S_SHOW_CLIENT_THIS_DISCONNECTED,7277,205014
    PEND_CLIENT 7278,205072
    PEND_CLIENT = TN3270S_SHOW_CLIENT_THIS_PENDING,7278,205072
    INSESS_CLIENT 7279,205125
    INSESS_CLIENT = TN3270S_SHOW_CLIENT_THIS_INSESSION,7279,205125
    ALL_CLIENT 7280,205181
    ALL_CLIENT = TN3270S_SHOW_CLIENT_THIS_ALL 7280,205181
typedef enum show_state_t_ show_state_t;show_state_t7282,205232
category 7285,205294
tn3270s_show_all_client_lu 7303,205839
static const char* GetEvent 7381,208374
tn3270s_show_lu 7420,209402
tn3270s_show_all_pu_lu 7538,213046
apply_siftdown(7570,213817
tn3270s_show_pu 7625,215689
tn3270s_show_all_pu_cfg 7716,218429
tn3270s_show_all_server_stats 7734,218867
tn3270s_show_all_dlur_cfg 7809,221865
tn3270s_show_all_svr_cfg 7925,225813
tn3270s_show_command 8113,231991
tn3270s_show_this_ipvc 8293,237138
tn3270s_show_all_ipvc 8313,237606
tn3270s_display_vc_list 8357,238496
tn3270s_add_idb_to_list 8383,239147
tn3270s_hwidb_from_pu_index 8416,240060
tn3270s_pu_t* tn3270s_pu_from_index 8463,241231
boolean IsPuShut(8495,241935
boolean UpdateLocalPuConfig(8501,242059
cip_tn3270s_create 8578,245234
cip_tn3270s_reset_cfg 8624,246509
cip_tn3270s_send_cfg 8677,247889
cip_tn3270s_cfg_str 8701,248416
cip_tn3270s_cfg_acknowledge 8720,248852
cip_tn3270s_display_vc_list 8735,249253
tn3270s_cfg_command 8741,249344
tn3270s_exit_command 8747,249417
tn3270s_show_ext_channel_command 8753,249499
tn3270s_svr_set_vars 8759,249594
tn3270s_svr_reset_vars 8765,249695
tn3270s_pu_set_vars 8772,249818
tn3270s_pu_reset_vars 8778,249917
tn3270s_dlur_set_vars 8785,250044
tn3270s_dlur_reset_vars 8791,250147
tn3270s_dlur_pu_set_vars 8798,250273
tn3270s_dlur_pu_reset_vars 8804,250382
tn3270s_dlur_sap_set_vars 8811,250517
tn3270s_dlur_sap_reset_vars 8817,250628
tn3270s_svr_subcommand 8824,250764
tn3270s_pu_subcommand 8832,250909
tn3270s_dlur_subcommand 8840,251064
tn3270s_dlur_pu_subcommand 8848,251213
tn3270s_dlur_sap_subcommand 8856,251374
ulong GetTn3270sUpTime 8863,251534
