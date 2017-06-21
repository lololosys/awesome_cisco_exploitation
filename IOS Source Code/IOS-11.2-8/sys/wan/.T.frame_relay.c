
frame_relay.c,5912
static void fr_pvc_enqueue 866,32616
static void fr_pvc_unqueue 882,32959
fr_set_p2pvc 895,33270
fr_clear_p2pvc 914,33731
static fr_pvc_info_type *find_p2p_pvc_from_swidb 929,33959
static void fr_idblist_add 953,34448
static void fr_idblist_remove 977,34950
static void fr_resync_pvc_count 992,35338
void fr_dec_pvc_count 1017,35929
boolean frame_relay_lmi_debug(1026,36084
boolean frame_relay_packet_debug(1039,36280
static void debug_fr_pak 1067,36925
static boolean fr_pvc_stay_down 1175,40188
void fr_set_pvc_status 1196,40730
void fr_add_mc_dlci 1250,42222
void fr_mc_delete_dlci 1289,43236
static void fr_listdebug 1311,43802
static fr_listtype *fr_pkt_classify 1340,44513
boolean fr_vencapsulate 1466,47274
boolean fr_encaps 1481,47570
static boolean fr_pbp_comp_decide 1770,57309
static paktype *fr_pbp_compress 1786,57741
paktype *fr_tcpip_hdr_compress 1872,60954
boolean fr_doencaps 1910,61905
static boolean is_fr_osi_nlpid 2200,71615
static iqueue_t frame_relay_parse_packet 2229,72425
static void fr_raw_enqueue 2458,79426
static boolean fr_translate_1490_to_ether 2514,80774
void frame_relay_input 2568,82079
boolean fr_sort_packet 2783,88746
void fr_update_ctrl_bits 2915,93204
long frame_relay_getlink 2941,93920
static boolean fr_can_mcast 2996,95454
frame_relay_bcast 3012,95788
static boolean frame_relay_check_idb(3033,96235
static void fr_add_bridge_flood 3050,96582
static void fr_delete_bridge_flood 3112,98189
static void fr_bridge_add_p2p_flood 3150,99092
static void fr_bridge_clear_p2p_flood 3155,99204
void fr_pri_delete_bridge_flood 3176,99747
void fr_pri_delete_add_bridge_flood 3208,100726
void reset_fr_pvc_from_priority_list 3226,101354
int get_pri_from_priority_list 3257,102355
void set_fr_pvc_from_priority_list 3281,102948
static void fr_set_idbmap 3304,103760
static void fr_unset_idbmap 3380,105839
int frame_relay_parse_map 3461,107849
static frmaptype *findMapEntry 3628,113495
static boolean fr_payload_compression_command 3645,113803
static void frame_relay_bridge_address 3695,115201
static uchar fr_find_multistate 3736,116204
static boolean fr_subif_stay_down 3776,117010
void fr_set_subif_state 3809,117742
static void fr_de_parse(3860,118853
static void nv_fr_listwrite 4040,122734
static void fr_dlci_group_clear 4087,123943
void frame_relay_global_command 4115,124622
void fr_dlci_exit_command 4157,125435
void frame_relay_command 4176,125717
fr_pvc_info_type *fr_pvc_init 5054,151933
void fr_pvc_clear 5115,153843
fr_pvc_info_type *get_pri_fr_pvc_bridging 5170,155378
frmaptype *fr_addr_enter 5204,156333
frmaptype *frame_relay_addr_lookup 5337,160204
void frame_relay_map_delete 5388,161476
static void frame_relay_deencapsulate 5482,164217
frmaptype *fr_verify_compression 5554,166268
uint fr_inherit_compr_attributes 5583,166953
static boolean frame_relay_linestate 5619,167954
static boolean frame_relay_extract_addr 5633,168277
ushort fr_copy_snpa 5663,168953
static boolean frame_relay_setup 5674,169172
static void frame_relay_set_dialer 5841,174311
void frame_relay_periodic 5870,174978
void fr_interface 5905,175825
void print_dlci_status 5980,178015
void frame_relay_hash_init 6010,178572
static ushort fr_get_subint_ref_num_from_pak 6018,178686
static void fr_clear_subif_dynamic_entries 6030,179006
static void fr_clear_subif_pvc 6064,179783
static void fr_subif_pvcchange 6088,180375
static void fr_subif_dte_statechange(6129,181541
static void fr_subif_dce_statechange(6169,182553
static void fr_subif_statechange 6198,183271
static void fr_list_init 6235,184029
static boolean frame_relay_srt_learn_source 6245,184178
static void fr_if_statechange_complete 6266,184780
static short fr_fair_queue_flow_id 6299,185578
static void fr_print_network_header 6327,186416
static void fr_reset_pvc_stats 6350,186967
static void fr_init_idb_stats 6367,187323
static void fr_clear_counters 6380,187582
void fr_bridging_pri_list_changed 6397,188041
void frame_relay_priority_list_changed 6473,190862
static void frame_relay_init 6511,191903
boolean fr_encaps_check(6616,196458
uchar fr_extract_l3pid_value(6665,197877
boolean fr_enter_static_map 6681,198357
void fr_bootp_setup 6695,198692
int fr_bring_line_up 6730,199796
void fr_cleanup_after_ai(6765,200731
void fr_bootp_complete 6798,201473
addrtype *fr_addr_search 6836,202421
void show_frame_relay 6868,203013
void show_frame_relay_maplist 6909,203702
void show_frame_relay_compression 7054,208303
void show_fr_traffic 7153,210777
void show_frame_relay_map 7167,211112
void frame_relay_display_map 7232,212416
void frame_relay_display_int_dlci 7326,215426
void fr_svc_broadcast_handler(7377,217092
frmaptype *fr_broadcast 7450,219845
frmaptype *fr_broadcast_on_all_eligible_pvcs 7491,221062
boolean fr_broadcast_cdp 7566,223116
void hexdump_packet 7630,224833
void fr_clear_map 7668,225631
void print_pvc_ie 7692,226010
void print_mc_ie 7717,226624
void print_ka_ie 7728,226897
void print_report_ie 7738,227106
ushort l3pid(7749,227336
ushort tb1490link2serial(7776,227826
void fr_cstate 7788,228026
void fr_clear_dynamic_entries 7948,233270
boolean fr_parse_p2p_dlci 8018,234711
static void fr_clear_dlci_map(8149,238590
void fr_parse_multi_dlci 8178,239160
void fr_parse_no_if_dlci 8232,240773
boolean fr_switching_idb 8297,242339
void fr_encaps_nvram_write 8308,242594
void fr_encaps_show_protocol_attr 8317,242819
void fr_tcpip_header_compression_command 8331,243117
boolean fr_header_compression_set 8359,243793
void fr_init_encaps_idb(8471,246930
void fr_wait(8503,247939
void fr_debug_pak 8507,248010
ushort fr_pr_set_dlci 8535,248721
void fr_pr_init_dlci_group 8558,249256
void fr_mapclass_nvgen_func(8600,250319
void  fr_debug_command 8611,250536
#define FR_MAJVERSION 8662,251657
#define FR_MINVERSION 8663,251681
#define FR_EDITVERSION 8664,251705
