
if_pas_ibm2692.h,13461
#define IBMTR_VENDOR_DEVICE_ID 217,7787
#define PCI_4R_NUM_INTERFACES	219,7830
#define HERMON_MPC_RESET_DELAY 221,7863
#define HERMON_MPC_ENABLE_DELAY	222,7898
#define REJECT_TBL_SIZE 226,7991
#define REJECT_INDEX_MASK 227,8025
#define MAX_TR_PKT_SIZE 229,8079
#define MIN_TR_PKT_SIZE 230,8108
#define IBM2692_CRC_SIZE	232,8136
#define	FBB_ENABLE	236,8227
#define SERR_ENABLE	237,8288
#define PARITY_ENABLE	238,8357
#define MWI_ENABLE	239,8416
#define BUS_MASTER	240,8483
#define MEM_SPACE	241,8542
#define IO_SPACE	242,8602
#define	LAP_REG_WE	246,8698
#define LAP_REG_RE	247,8757
#define LAP_RSV_WE	248,8815
#define LAP_RSV_RE	249,8880
#define LAP_ROM_WE	250,8944
#define LAP_ROM_RE	251,9004
#define LAP_RAM_WE	252,9063
#define	SYS_INT_ENA	256,9182
#define	SCB_STS	257,9239
#define	SCB_CTL	258,9302
#define SCB_INT_MASK	259,9366
#define	TMR_INT_MASK	260,9419
#define LAP_PERR_MASK	261,9474
#define LAP_ACC_ERR_M	262,9545
#define PCI_PERR_MASK	263,9621
#define ADP_CHK	265,9689
#define SRB_RESP	266,9748
#define ASB_FREE	267,9806
#define ARB_CMD	268,9860
#define TRB_RESP	269,9917
#define	LOCAL_INT_ENA	277,10153
#define	LOCAL_INT_MASK	278,10211
#define	L_SCB_INT_MASK	279,10268
#define ICDB_PERR_MASK	280,10329
#define	SIF_SRB_CMD	282,10399
#define	SIF_ASB_RSP	283,10458
#define SIF_ASB_FREE	284,10514
#define SIF_ARB_FREE	285,10579
#define SIF_TRB_CMD	286,10636
#define	MI_IND	290,10757
#define SCB_STAT_INT	291,10818
#define SCB_CNTL_INT	292,10882
#define SCB_SIG_INT	293,10947
#define TMR_INT	294,11010
#define LAP_PERR_INT	295,11064
#define LAP_ACC_INT	296,11130
#define PCI_PERR_INT	297,11200
#define UNUSED_INT	299,11262
#define TX2_EOC_INT	304,11423
#define TX2_NOST_INT	305,11492
#define TX2_HALT_INT	306,11566
#define TX2_EOF_INT	307,11634
#define TX2_INT_MASK	308,11709
#define TX1_EOC_INT 310,11778
#define TX1_NOST_INT 311,11852
#define TX1_HALT_INT 312,11930
#define TX1_EOF_INT 313,12002
#define TX1_INT_MASK	314,12082
#define RX_NBA_INT	316,12151
#define RX_EOB_INT	317,12213
#define RX_NOSTA_INT	318,12278
#define RX_HALT_INT	319,12349
#define RX_EOF_INT	320,12407
#define RX_INT_MASK	321,12478
#define PCI_FIFO_SIZE 323,12537
#define TX1_DISABLE_REQ	327,12676
#define TX1_DISABLED	328,12752
#define TX1_ENABLED	329,12814
#define TX1_DISABLE_PENDING	330,12874
#define TX2_DISABLE_REQ	332,12951
#define TX2_DISABLED	333,13027
#define TX2_ENABLED	334,13089
#define TX2_DISABLE_PENDING	335,13149
#define RX_DISABLE_REQ	337,13226
#define RX_DISABLED	338,13298
#define RX_ENABLED	339,13356
#define RX_DISABLE_PENDING	340,13412
#define RX_BM_ACTIVE_MASK	341,13484
#define HERMON_SOFT_RESET 345,13613
#define MAX_TX_BFR_SIZE	347,13647
#define SIGNAL_LOSS	351,13731
#define HARD_ERROR	352,13759
#define SOFT_ERROR	353,13786
#define TRANSMIT_BEACON	354,13813
#define LOBE_WIRE_FAULT	355,13845
#define AUTO_REMOVAL	356,13877
#define REMOVE_RECEIVED	357,13906
#define COUNTER_OVERFLOW	358,13938
#define SINGLE_STATION	359,13970
#define RING_RECOVERY	360,14001
#define SR_COUNTER_OVERFLOW	361,14031
#define	CLOSE_ADAPTER	365,14098
#define CONFIG_BRIDGE 366,14164
#define	CFG_HP_CHANNEL	367,14236
#define	MODIFY_BG_PARM	368,14272
#define MODIFY_OPEN_OPTION 369,14308
#define MODIFY_RCV_OPTION 370,14345
#define MODIFY_TB_OPTION 371,14382
#define NO_OP	372,14419
#define OPEN_ADAPTER	373,14447
#define READ_LOG	374,14481
#define	READ_SR_COUNT	375,14511
#define RESET_GROUP_ADDR 376,14546
#define RESET_TARGET_SEG 377,14583
#define SAVE_CFG	378,14620
#define SET_BRDGE_PARM	379,14650
#define SET_FUNC_ADDR	380,14686
#define SET_GROUP_ADDR	381,14721
#define SET_TARGET_SEG	382,14757
#define TB_REJET_ADDR	383,14793
#define INIT_RESP 384,14828
#define SIZE_OF_INIT_RESP	386,14903
#define BRIDGE_ACCESS_PRIORITY	387,14967
#define MPC_INITIAL_STAT	388,15023
#define MPC_OK_STAT	389,15053
#define PRODUCT_ID_LEN	390,15078
#define ENA_COPY_ALL_LLC	394,15159
#define ENA_COPY_ALL_MAC	395,15189
#define ENA_TB_COPY	396,15219
#define ENA_COPY_ALL_SRB	397,15245
#define MPC_PWD1	401,15350
#define MPC_PWD2	402,15375
#define MPC_PWD3	403,15400
#define MPC_PWD4	404,15425
#define	RECEIVE_DATA	408,15481
#define	LAN_STATUS_CHANGE	409,15508
#define ARB_SIZE	411,15540
#define MAX_BRIDGE_PROTOS 413,15562
typedef struct receive_data_arb_t receive_data_arb_t415,15592
} receive_data_arb;receive_data_arb422,15751
typedef struct lan_status_change_arb_t lan_status_change_arb_t424,15772
} lan_status_change_arb;lan_status_change_arb428,15871
typedef union arb_ arb_430,15897
} arb_t;arb_t434,16012
typedef struct tx_bfr_ tx_bfr_436,16022
} tx_bfr_t;tx_bfr_t440,16219
typedef struct tx_bfr_desc_ tx_bfr_desc_442,16232
} tx_bfr_desc_t;tx_bfr_desc_t453,16816
typedef struct rx_bfr_desc_ rx_bfr_desc_455,16834
} rx_bfr_desc_t;rx_bfr_desc_t461,17182
typedef struct rx_desc_shadow rx_desc_shadow463,17200
} rx_desc_shadow_t;rx_desc_shadow_t467,17449
typedef struct tx_desc_shadow tx_desc_shadow469,17470
} tx_desc_shadow_t;tx_desc_shadow_t475,17778
typedef struct init_buffer_ init_buffer_477,17799
} ibm2692_init_buffer_t;ibm2692_init_buffer_t479,17850
typedef struct hermon_reg_map_t hermon_reg_map_t483,17916
} hermon_reg_map;hermon_reg_map554,22002
#define   TX_PCPE	558,22064
#define   TX_IPRE	559,22114
#define   TX_IWBE	560,22176
#define   TX_IABE	561,22239
#define   TX_FDP	562,22304
#define   TX_WER	563,22368
#define   TX_EOC	564,22424
#define   TX_TFUR	565,22474
#define   TX_TPE	566,22526
#define   TX_ERROR_MASK	568,22591
#define   CANCEL_FRAME	570,22647
#define   RX_EOF	574,22744
#define   RX_PCPE	575,22791
#define   RX_IPRE	576,22851
#define   RX_IWBE	577,22913
#define   RX_IABE	578,22976
#define   RX_FPE	579,23041
#define   RX_NRBDA	580,23097
#define   RX_BDP	581,23153
#define   RX_WER	582,23218
#define   RX_GMAT	583,23272
#define   RX_IMAT	584,23319
#define   RX_FMAT	585,23379
#define   RX_RIBC	586,23439
#define   RX_NRIBC	587,23487
#define   RX_RFOR	588,23540
#define   RX_RPE	589,23591
#define   RX_IMP_ABORTED_FRAME 591,23642
#define   RX_EXP_ABORTED_FRAME 592,23725
#define   RX_ABORT_MASK 593,23808
#define   RX_MPC_ERROR 595,23870
#define   RX_ERROR_MASK	597,23931
#define TIMER_ENABLE 601,24011
#define MS_500 602,24040
#define MS_20 603,24075
#define DEST_ADDR_MATCH	607,24172
#define ROUTE_ADDR_MATCH	608,24234
#define	TRANB_ADDR_MATCH	609,24297
#define ADDR_MATCH_MASK	610,24366
#define RXSTAT_MAC	611,24428
#define FUNCTIONAL_ADDR	613,24486
#define FRAME_TYPE_MASK	614,24546
#define RI_INDICATION	615,24608
#define BRIDGE_FUNCTIONAL	616,24667
#define STATION_TYPE	620,24777
#define MULTICAST_TYPE	621,24804
#define BRIDGING_TYPE	622,24833
#define MAC_TYPE	623,24861
#define DOD_IP_TYPE	625,24885
#define DECNET_TYPE	626,24912
#define XNS_TYPE	627,24939
#define CLNS_TYPE	628,24963
#define CLASSIFY_MASK 630,24989
#define TYPE_IP10MB	632,25021
#define TYPE_RFC826_ARP	633,25048
#define TYPE_DECNET	634,25079
#define TYPE_XNS	635,25106
#define EXPLICIT_CONFIG_PARM	639,25177
#define RJ45	640,25211
#define RS16	641,25231
#define EXTERNAL_WRAP	643,25252
#define NO_HARD_ERROR	644,25282
#define NO_SOFT_ERROR	645,25312
#define APAPTER_MAC	646,25342
#define ATTENTION_MAC	647,25370
#define SR_COUNT_4	648,25400
#define CONTENDER	649,25427
#define BEACON_MAC	650,25453
#define RPL	651,25480
#define ETR	652,25501
#define INTERNAL_WRAP	653,25522
#define NO_TX_LLC_COPY	655,25553
#define NO_TX_RI_COPY	656,25582
#define NO_DA_LLC_COPY	657,25610
#define DA_MAC_COPY	658,25639
#define ALL_NODE_MAC_COPY	659,25665
#define DISABLE_ALL_RCV_OPTION	661,25697
#define ENABLE_LLC_COPY 662,25733
#define DISABLE_ASA_LLC_COPY 663,25770
#define   MPC_RAM	667,25882
#define   MPC_ROM	668,25928
#define   MPC_REG	669,25974
#define INIT_RESP_SIZE	672,26027
struct init_resp init_resp674,26054
typedef union init_resp_ init_resp_688,26353
} init_resp_t;init_resp_t691,26437
#define ADDR_TABLE_SIZE 693,26453
struct addr_table addr_table695,26481
typedef union adr_tbl_ adr_tbl_701,26579
} adr_tbl_t;adr_tbl_t704,26656
#define UAA_ADDR_SIZE	706,26671
struct uaa_addr uaa_addr708,26696
typedef union ua_adr_ ua_adr_714,26782
} ua_adr_t;ua_adr_t717,26867
#define PARMS_TABLE_SIZE	720,26881
struct parms_table parms_table722,26910
typedef union parms_tbl_ parms_tbl_747,27503
} parms_tbl_t;parms_tbl_t750,27584
#define UCODE_LEVEL_SIZE	752,27600
struct ucode_level ucode_level754,27629
typedef union ucode_level_ ucode_level_758,27682
} ucode_level_t;ucode_level_t761,27767
#define CLOSE_ADAPTER_SIZE	767,27868
typedef struct close_adapter_ close_adapter_769,27898
} close_adapter_t;close_adapter_t773,27986
#define CONFIGURE_BRIDGE_SIZE	777,28049
typedef struct configure_bridge_ configure_bridge_779,28082
} configure_bridge_t;configure_bridge_t785,28219
#define CONFIGURE_HP_CHANNEL_SIZE	789,28309
typedef struct configure_hp_channel_ configure_hp_channel_791,28346
} configure_hp_channel_t;configure_hp_channel_t797,28487
#define MODIFY_BRIDGE_PARMS_SIZE	801,28565
typedef struct modify_bridge_parms_ modify_bridge_parms_803,28602
} modify_bridge_parms_t;modify_bridge_parms_t813,28834
#define MODIFY_OPEN_OPTIONS_SIZE	817,28905
typedef struct modify_open_options_ modify_open_options_819,28941
} modify_open_options_t;modify_open_options_t825,29081
#define MODIFY_RECEIVE_OPTIONS_SIZE	829,29156
typedef struct modify_receive_options_ modify_receive_options_831,29196
} modify_receive_options_t;modify_receive_options_t839,29391
#define MODIFY_TB_OPTIONS_SIZE	843,29480
typedef struct modify_tb_options_ modify_tb_options_845,29514
} modify_tb_options_t;modify_tb_options_t851,29651
#define	NO_OPERATION_SIZE	855,29714
typedef struct no_operation_ no_operation_857,29743
} no_operation_t;no_operation_t861,29831
#define OPEN_ADAPTER_SIZE	865,29890
typedef struct open_adapter_ open_adapter_867,29920
} open_adapter_t;open_adapter_t880,30259
#define OPEN_RESP_SIZE	884,30334
typedef struct open_resp_ open_resp_886,30361
} open_resp_t;open_resp_t898,30625
#define	READ_LOG_SIZE	902,30683
typedef struct read_log_ read_log_904,30709
} read_log_t;read_log_t920,31078
#define READ_SR_COUNTERS_SIZE	924,31162
typedef struct read_sr_counters_ read_sr_counters_926,31196
} read_sr_counters_t;read_sr_counters_t935,31387
#define RESET_GROUP_ADDRESS_SIZE	939,31457
typedef struct reset_group_address_ reset_group_address_941,31494
} reset_group_address_t;reset_group_address_t949,31681
#define RESET_TARGET_SEGMENT_SIZE	953,31753
typedef struct reset_target_segment_ reset_target_segment_955,31790
} reset_target_segment_t;reset_target_segment_t961,31930
#define SAVE_CONFIGURATION_SIZE	965,32002
typedef struct save_configuration_ save_configuration_967,32037
} save_configuration_t;save_configuration_t975,32214
#define SET_BRIDGE_PARMS_SIZE	979,32288
#define ENA_LIMITED_BROADCAST	980,32321
typedef struct set_bridge_parms_ set_bridge_parms_982,32354
} set_bridge_parms_t;set_bridge_parms_t994,32650
#define SET_FUNCTIONAL_ADDRESS_SIZE	998,32723
typedef struct set_functional_address_ set_functional_address_1000,32763
} set_functional_address_t;set_functional_address_t1006,32915
#define SET_GROUP_ADDRESS_SIZE	1010,32989
typedef struct set_group_address_ set_group_address_1012,33024
} set_group_address_t;set_group_address_t1023,33275
#define SET_TARGET_SEGMENT_SIZE	1027,33344
typedef struct set_target_segment_ set_target_segment_1029,33379
} set_target_segment_t;set_target_segment_t1037,33568
#define TB_REJECT_ADDRESS_SIZE	1041,33654
#define SET_ONE	1043,33689
#define RESET_ONE	1044,33710
#define RESET_ALL	1045,33733
typedef struct tb_reject_address_ tb_reject_address_1047,33757
} tb_reject_address_t;tb_reject_address_t1056,33964
typedef struct trb_hdr_ trb_hdr_1058,33988
} trb_hdr_t;trb_hdr_t1064,34113
typedef union trb_ trb_1066,34128
} trb_t;trb_t1069,34188
#define TRB_HDR_LEN	1071,34199
typedef struct rp_srb_ rp_srb_1075,34288
} rp_srb_t;rp_srb_t1077,34334
typedef union srb_command_block_ srb_command_block_1079,34347
} srb_cmd_block_t;srb_cmd_block_t1101,35255
#define ASB_AVAILABLE	1103,35275
#define ILLEGAL_CMD	1104,35302
#define ILLEGAL_ADDR	1105,35327
#define MPC_ASB_SIZE	1107,35354
#define ASB_QUEUE_SIZE	1108,35377
#define ASB_INDEX_MASK	1109,35402
#define MAX_SRB_QUEUE_SIZE 1111,35445
#define MAX_SRB_QUEUE_MASK 1112,35477
typedef struct mpc_asb mpc_asb1114,35528
} mpc_asb_t;mpc_asb_t1120,35735
typedef struct mac_ mac_1122,35749
} mac_hdr_t;mac_hdr_t1126,35830
#define ASB_MAC_HDR_SIZE 1128,35844
typedef struct mac_addr mac_addr1130,35879
} mac_addr_t;mac_addr_t1132,35924
typedef struct ibm2692_instance ibm2692_instance1136,35983
} ibm2692_instance_t;ibm2692_instance_t1271,39359
static inline boolean in_reject_list 1280,39598
static inline void add_reject_addr 1296,39945
static inline void ibm2692_queue_tx_desc 1311,40269
static inline void ibm2692_flush_pak 1340,41010
static inline void ibm2692_cleanup_pak 1352,41263
static inline void ibm2692_output_enqueue 1361,41412
static inline void ibm2692_return_buffer 1392,42113
static inline void ibm2692_rx_error_log 1408,42487
static inline particletype *ibm2692_get_particle_inline 1442,43331
#define IBM2692_STATIC_INLINES_ONLY1540,46789
