
sysmgt_snallc.h,3249
#define _SYSMGT_SNADLCLLC_H_34,1145
#define MAC_ADDR_LEN 44,1468
#define MAC_ADDR_ALIGNED_LEN 45,1491
#define PORT_NAME_LEN 46,1522
#define FRMR_INFO_LEN 47,1546
#define FRMR_INFO_ALIGNED_LEN 48,1570
typedef enum sysmgt_snadlcllc_table_ sysmgt_snadlcllc_table_50,1603
} sysmgt_snadlcllc_table_t;sysmgt_snadlcllc_table_t61,1897
typedef enum sysmgt_snadlcllc_states_ sysmgt_snadlcllc_states_63,1926
} sysmgt_snadlcllc_states_t;sysmgt_snadlcllc_states_t78,2238
typedef enum sysmgt_snadlcllc_ww_causes_ sysmgt_snadlcllc_ww_causes_80,2268
} sysmgt_snadlcllc_ww_causes_t;sysmgt_snadlcllc_ww_causes_t85,2413
typedef enum sysmgt_snadlcllc_fail_causes_ sysmgt_snadlcllc_fail_causes_87,2446
} sysmgt_snadlcllc_fail_causes_t;sysmgt_snadlcllc_fail_causes_t95,2660
typedef struct llc2_sessions_ llc2_sessions_97,2695
} llc2_sessions_t;llc2_sessions_t101,2887
typedef struct llc_port_index_ llc_port_index_106,2948
} llc_port_index_t;llc_port_index_t108,3010
typedef struct llc_port_admin_ llc_port_admin_110,3031
} llc_port_admin_t;llc_port_admin_t127,3833
typedef struct llc_port_oper_ llc_port_oper_132,3894
} llc_port_oper_t;llc_port_oper_t138,4173
typedef struct llc_port_stats_ llc_port_stats_143,4234
} llc_port_stats_t;llc_port_stats_t153,4685
typedef struct llc_sap_index_ llc_sap_index_158,4746
} llc_sap_index_t;llc_sap_index_t161,4842
typedef struct llc_sap_admin_ llc_sap_admin_163,4862
} llc_sap_admin_t;llc_sap_admin_t177,5504
typedef struct _llc_sap_oper_ _llc_sap_oper_182,5563
} llc_sap_oper_t;llc_sap_oper_t189,5831
typedef struct _llc_sap_stats_ _llc_sap_stats_194,5890
} llc_sap_stats_t;llc_sap_stats_t214,6847
typedef struct llc_cc_index_ llc_cc_index_219,6906
} llc_cc_index_t;llc_cc_index_t224,7089
typedef struct llc_cc_admin_ llc_cc_admin_226,7108
} llc_cc_admin_t;llc_cc_admin_t241,7779
typedef struct llc_cc_oper_ llc_cc_oper_246,7836
} llc_cc_oper_t;llc_cc_oper_t269,8894
typedef struct llc_cc_stats_ llc_cc_stats_274,8951
} llc_cc_stats_t;llc_cc_stats_t288,9559
typedef enum llc_802_event_code_ llc_802_event_code_294,9610
} llc_802_event_code;llc_802_event_code304,10200
#define CC_NOT_BUSY 310,10251
#define REM_CC_BUSY 311,10279
#define LOCAL_CC_BUSY 312,10307
typedef struct llc_sap_event_ llc_sap_event_315,10337
} llc_sap_event_t;llc_sap_event_t321,10706
typedef struct llc_cc_event_ llc_cc_event_323,10726
} llc_cc_event_t;llc_cc_event_t339,11755
#define LLC2_SESSIONS_SIZE 342,11775
#define LLC_SAP_EVENT_SIZE 343,11831
#define LLC_CC_EVENT_SIZE 344,11887
#define LLC_PORT_INDEX_SIZE 345,11942
#define LLC_SAP_INDEX_SIZE 346,11999
#define LLC_CC_INDEX_SIZE 347,12055
#define LLC_PORT_ADMIN_SIZE 348,12110
#define LLC_PORT_OPER_SIZE 349,12167
#define LLC_PORT_STATS_SIZE 350,12223
#define LLC_SAP_ADMIN_SIZE 351,12280
#define LLC_SAP_OPER_SIZE 352,12336
#define LLC_SAP_STATS_SIZE 353,12391
#define LLC_CC_ADMIN_SIZE 354,12447
#define LLC_CC_OPER_SIZE 355,12502
#define LLC_CC_STATS_SIZE 356,12556
typedef enum sysmgt_snadlcllc_type_ sysmgt_snadlcllc_type_360,12614
} sysmgt_snadlcllc_type_t;sysmgt_snadlcllc_type_t364,12711
typedef struct sna_dlc_llc_ sna_dlc_llc_366,12739
} sna_dlc_llc_t;sna_dlc_llc_t375,13118
