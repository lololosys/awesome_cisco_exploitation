
if_cip_vlan.c,7146
typedef struct vadapter_llc2_ vadapter_llc2_348,13109
} vadapter_llc2;vadapter_llc2361,14020
#define VLAN_MAX_VADAPTERS 367,14066
#define VLAN_MAX_VLANS_OF_TYPE 368,14128
#define VLAN_MAX_VLANS 369,14187
#define VADAPTER_NAME_LEN 371,14264
#define VADAPTER_NAME_ZLEN 372,14298
typedef struct vadapter_element_ vadapter_element_374,14355
} vadapter_element;vadapter_element384,15112
#define VADAP_STATE_UNUSED 386,15133
#define VADAP_STATE_INUSE 387,15213
#define VADAP_STATE_CFGING 388,15293
#define VADAP_STATE_CREATE 389,15373
#define VADAP_STATE_UPDATE 390,15453
#define VADAP_STATE_DELETE 391,15533
#define VADAP_STATE_PNDING 392,15613
#define VADAP_STATE_ACKED 393,15693
typedef struct vlan_element_ vlan_element_395,15774
} vlan_element;vlan_element408,16767
#define VLAN_ATTRB_SRB_OK 410,16784
#define VLAN_ATTRB_TB_OK 411,16864
#define VLAN_ATTRB_SRB 412,16944
#define VLAN_ATTRB_TB 413,17024
#define VLAN_STATE_UNUSED 415,17105
#define VLAN_STATE_INUSE 416,17185
#define VLAN_STATE_CFGING 417,17265
#define VLAN_STATE_CREATE 418,17345
#define VLAN_STATE_UPDATE 419,17426
#define VLAN_STATE_DELETE 420,17506
#define VLAN_STATE_PNDING 421,17586
#define VLAN_STATE_ACKED 422,17668
typedef struct cip_vlan_type_ cip_vlan_type_424,17749
} cip_vlan_type;cip_vlan_type427,17896
typedef struct csna_vlans_ csna_vlans_429,17914
} csna_vlans;csna_vlans432,18113
#define MAX_VADAPTER_VC 440,18307
#define MIN_VADAPTER_VC 445,18456
#define ASSIGN_VADAPTER_VC(ASSIGN_VADAPTER_VC448,18506
#define VADAPTER_FROM_VC(VADAPTER_FROM_VC451,18624
#define LANTYPE_FROM_VC(LANTYPE_FROM_VC454,18715
#define SHOWLLC2(SHOWLLC2457,18805
#define NVWRITE(NVWRITE461,18909
#define UDB_VLAN 497,20639
#define UDB_VADAP 498,20659
#define VADAP_PROMPT_BASE 500,20680
#define VLAN_PROMPT_BASE 501,20727
static void csna_llc2_defaults 541,21703
static void display_this_vadap_llc2 557,22509
static inline boolean csna_llc2_val_change 619,24069
void csna_llc2_command 640,24477
static boolean cip_find_vlan_type_id 790,28576
static void csna_reset_vadapter 806,28958
static void csna_reset_vlan 818,29330
static boolean csna_disallow_if_vadapters 834,29823
static boolean vadap_matching_macaddr 848,30219
void csna_vadap_name_command 882,31014
static void csna_vlan_print_configured_bridge_stmt 917,31883
static boolean csna_validate_bridge_command 936,32369
void csna_vlan_srb_command 987,33959
void csna_vlan_bridge_command 1100,37222
void vlan_vadapter_command 1198,39957
void csna_vlan_max_llc2_sess_command 1395,46304
void csna_vlan_command 1449,47576
static void vlan_exit 1553,50674
void vlan_exit_command 1567,50915
void *vlan_set_vars 1581,51289
void vlan_reset_vars 1593,51506
void csna_vlan_subcommand 1608,51776
static void vadapter_exit 1663,53113
void vadap_exit_command 1683,53579
void *vadap_set_vars 1698,53987
void vadap_reset_vars 1709,54217
void csna_vadap_subcommand 1724,54499
static void display_this_vlan 1783,55904
#define BLANKS 1807,56522
static void display_this_vadap_status 1808,56543
static void display_this_vadap 1828,57333
void show_ext_channel_vlan_command 1857,58105
static int incr_mac_addr 1955,60627
static boolean display_llc_connections 1967,60804
static boolean display_llc_sap 2122,68141
static boolean display_llc_port 2228,72748
void show_ext_channel_llc2_command 2303,75929
void show_ext_channel_csna_conn_map_command(2443,80948
void show_ext_channel_max_llc2_sess_command 2564,85640
ushort cip_get_csna_vlan_maxllc2_sessions 2615,87235
void *cip_get_vlan 2632,87619
void *cip_get_vlan_from_vadapt 2682,88836
uchar *get_vlan_lan_type_name 2711,89479
lan_types_t get_vlan_lan_type 2723,89752
ulong get_vlan_lan_id 2734,89963
bridge_types_t get_vlan_bridge_type 2745,90171
ulong get_vlan_srb_local_ring 2768,90792
ulong get_vlan_srb_bridge_num 2779,91012
ulong get_vlan_srb_target_ring 2790,91234
ulong get_vlan_tb_bridge_group 2801,91456
void *cip_get_vadapt_from_adapno 2815,91738
void *cip_get_vadapt_from_vc 2861,92916
ulong get_vadapt_vc 2911,94134
ulong get_vadapt_adapt_no 2920,94280
uchar *get_vadapt_mac_address 2929,94436
uchar *get_vadapt_adapt_name 2938,94597
ulong get_vadapt_llc2_T1_ack 2949,94809
ulong get_vadapt_llc2_T1_pf 2958,94978
ulong get_vadapt_llc2_T1_rej 2967,95141
ulong get_vadapt_llc2_T1_busy 2975,95305
ulong get_vadapt_llc2_Ti 2984,95472
ulong get_vadapt_llc2_Tw 2993,95629
ulong get_vadapt_llc2_Rw 3002,95786
ulong get_vadapt_llc2_N2 3011,95943
ulong get_vadapt_llc2_N1 3020,96100
ulong get_vadapt_llc2_T2 3029,96257
ulong get_vadapt_llc2_N3 3038,96414
ulong get_vadapt_llc2_Nw 3047,96571
void cip_vlan_create 3067,96962
#define OFFSET_CFG_CMDCODE 3137,99590
#define OFFSET_VADAPTER_VCNUM 3138,99630
#define OFFSET_VADAPTER_ADAPNO 3139,99671
#define	OFFSET_VADAPTER_MACADDR 3140,99711
#define	OFFSET_VADAPTER_LANTYPE 3141,99751
#define	OFFSET_VADAPTER_LANID 3142,99791
#define	OFFSET_VADAPTER_LLC2_T1_ACK 3143,99831
#define	OFFSET_VADAPTER_LLC2_T1_PF 3144,99871
#define	OFFSET_VADAPTER_LLC2_T1_REJ 3145,99911
#define	OFFSET_VADAPTER_LLC2_T1_BUSY 3146,99951
#define	OFFSET_VADAPTER_LLC2_TI 3147,99991
#define	OFFSET_VADAPTER_LLC2_TW 3148,100031
#define	OFFSET_VADAPTER_LLC2_RW 3149,100071
#define	OFFSET_VADAPTER_LLC2_N2 3150,100111
#define	OFFSET_VADAPTER_LLC2_N1 3151,100151
#define	OFFSET_VADAPTER_LLC2_T2 3152,100191
#define	OFFSET_VADAPTER_LLC2_N3 3153,100231
#define	OFFSET_VADAPTER_LLC2_NW 3154,100271
#define OFFSET_VADAPTER_CORRELATOR 3155,100311
#define CFG_CMDBLOCK_SIZE 3157,100377
static boolean csna_send_vadapter_cfg 3166,100608
#define OFFSET_MAX_LLC2_MAXSESSIONS 3267,104154
#define OFFSET_MAX_LLC2_CORRELATOR 3268,104195
#define CFG_CMDBLOCK_SIZE 3270,104261
static boolean csna_send_max_llc2_sess_command 3272,104302
void cip_vlan_reset_cfg 3328,105925
void cip_vlan_send_vlan_cfg 3382,107461
void cip_vlan_cfg_acknowledge 3442,109424
static void csna_bad_cfg_ack 3598,113962
static void csna_unexpected_cfg_ack 3615,114409
static void cip_vlan_remove_srb 3641,114918
static void cip_vlan_remove_tb 3669,115761
static void cip_remove_vadapter_from_ilan 3694,116410
static boolean cip_vlan_holdq_enqueue 3724,117398
static void cip_vlan_soutput 3786,119302
static void cip_ilan_show_interface 3805,119689
static void clear_ack_for_all_adapters_on_ilan 3843,120758
static void cip_ilan_cstate 3871,121617
void cip_vlan_reset_idb 3899,122454
static long cip_getlink 3919,122991
static void cip_vlan_free_idb 3930,123157
static hwidbtype *cip_vlan_get_idb 3961,124109
static ushort cip_ilan_vc_from_macaddr 4079,127400
hwidbtype *cip_ilan_hwidb_from_vc 4124,129130
static void cip_reset_llc2_parms 4152,129823
static void cip_setup_srb_init 4163,130131
static boolean cip_setup_srb_info 4171,130340
static void cip_setup_tb_info 4218,131920
static void cip_vlan_add_counters 4231,132200
static void cip_ilan_consolidate_counters 4345,137427
static void cip_vlan_init 4360,137732
#define CIP_VLAN_MAJVERSION 4373,138184
#define CIP_VLAN_MINVERSION 4374,138216
#define CIP_VLAN_EDITVERSION 4375,138248
