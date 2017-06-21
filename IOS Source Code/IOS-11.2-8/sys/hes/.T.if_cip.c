
if_cip.c,8501
typedef struct cip_adptr_info_ cip_adptr_info_436,16118
} cip_adptr_info;cip_adptr_info440,16358
typedef struct cip_mem_info_ cip_mem_info_442,16377
} cip_mem_info;cip_mem_info447,16496
#define CIP_METRIC_NOTAVAIL 449,16513
typedef boolean (vc_check_proc)vc_check_proc454,16617
typedef boolean (vc_check_proc)(cip_vctype cip_vctype454,16617
#define CHANNEL_DEBUG_DEED 506,18932
#define CIP_DEFAULT_WRITE_MEM_ATTEMPTS 595,21465
static void cip_init 601,21530
cip_get_love_letter_timestamp 678,23817
cip_set_love_letter_timestamp 693,24108
char *cip_cfg_name_str 708,24374
static inline char *cip_mode_to_str_inline 719,24610
static char *cip_daughter_string 730,24791
static int cip_daughter_type 747,25082
static boolean cip_is_tn3270s_cmd 763,25419
static inline boolean cip_is_vinterface_cmd 792,26182
void cip_copy_idb_id_info 813,26617
void cip_copy_idb_info 831,27086
boolean cip_interface_is_virtual 890,28855
boolean cip_cmd_valid_on_interface 919,29397
static boolean cip_ipaddr_duplicate_chk 958,30573
boolean cip_validate_ipaddr 1014,32135
boolean cip_len_exceeded 1048,32995
static ulong cip_get_cfg_block 1061,33205
static int cip_write_mem 1079,33519
static void cip_cfg_block_dump 1100,33956
static boolean cip_send_cfg_cmd 1173,35449
boolean cip_send_or_ack 1243,37241
static int cip_send_nodeid 1270,37909
cip_init_metrics 1300,38872
cip_init_load_metric_values 1308,39068
static cip_idb_infotype *cip_create_info 1318,39281
static cip_idb_infotype *cip_get_or_create_info 1379,40911
static inline boolean cip_adjacent_devices 1391,41151
static cip_vctype *cip_find_vc 1408,41426
static cip_vctype *cip_find_vc_with_vcnum 1432,41862
static cip_vctype *cip_find_vc_with_ipaddr 1455,42301
static boolean cip_can_make_vc 1475,42624
static int cip_create_vc 1488,42858
static cip_vctype *cip_add_vc 1504,43068
static void cip_default_pca_rate 1600,45317
static void cip_add_static_route 1609,45578
static void cip_delete_static_route 1641,46428
static boolean cip_teardown_vc 1659,46876
static boolean cip_valid_path_device 1697,47649
static boolean len_exceeded 1772,49189
static boolean cip_validate_parameters 1783,49424
#define OFFSET_CFG_CMD 1909,53580
#define OFFSET_CFG_VCNUM 1910,53605
static int cip_vc2buf 1912,53633
static int cip_send_subchannel 1988,55779
static int cip_send_cta_tt_command 2032,56925
static boolean cip_send_rate 2070,58010
static boolean cip_send_stats_interval 2096,58493
static boolean cip_vc_pending 2113,58838
static boolean update_csna_block_option 2129,59175
void channel_interface_csna_command 2168,60297
void channel_interface_command 2355,65006
static cta_tt_cmdtype *cip_find_cta_tt 2534,69330
static void cip_cta_tt_remove 2549,69673
static void cip_cta_tt_add 2568,70133
void channel_interface_cta_tt_cmd 2583,70414
void channel_rate_command 2670,72505
static void cip_send_cfg 2746,74232
static void cip_clear_counters 2806,75839
boolean get_cip_appl_version 2828,76350
static boolean get_from_cip_appl_version 2889,77823
cip_ipc_capable 2923,78915
cip_print_loader_segments 2942,79271
cip_print_pld_rom_ver 2964,79819
adapter_info_desc 2990,80375
cip_query_adapter_info 2996,80512
cip_print_adapter_info 3038,81537
cip_query_memory_info 3067,82411
cip_print_memory_stats 3113,83503
cip_percentage_str 3150,84553
cip_print_metrics 3162,84769
cip_print_channel_metrics 3178,85156
cip_most_recent_loveletter 3201,85686
void cip_show_cbus 3235,86443
static void cip_show_interface 3276,87612
static void show_last_love_time 3311,88374
static void cip_display_vc 3336,88973
#define BLANKS 3392,90387
static void cip_display_vc_list 3393,90411
static void cip_display_cta_test_tool 3438,91819
static void cip_display_nd 3459,92390
static cip_vctype *find_next_path 3507,93414
static cip_vctype *find_next_path_device 3545,94083
static void cip_display_subchannel 3577,94754
static char *print_long_count 3670,97404
static inline void cip_zero_totals 3689,97877
static void cip_add_totals 3697,98005
static void cip_print_totals 3711,98436
static int cip_num_mode_links 3727,98917
static boolean cip_nonzero_data_stats 3753,99337
#define si_val(si_val3776,99809
#define si_dval(si_dval3777,99881
static void cip_print_data_stats 3783,99986
static void cip_display_stats 3844,101947
static void cip_clear_si_stats 4000,106938
void show_extended_channel_commands 4015,107215
static void cip_display_tcp 4075,108448
#define MAX_TCPCONN_STATES 4130,109947
static void cip_display_tcpconn 4132,110011
static void cip_display_ip 4167,111252
static void cip_display_udp 4205,112681
static void cip_display_udp_listeners 4219,113068
static void cip_display_icmp 4228,113351
void show_extended_channel_tcpconns_command 4251,114353
static int show_ext_channel_tcp 4444,121338
static int show_ext_channel_ip 4465,121965
static int show_ext_channel_udp_listeners 4486,122588
static int show_ext_channel_udp 4520,123740
static int show_ext_channel_icmp 4542,124360
void show_extended_channel_tcpip_command 4563,125009
void show_ext_channel_csna_command 4651,127422
static long link2channel 4876,136148
static long channel_getlink 4893,136449
static void channel_pkt_dump 4906,136719
static boolean channel_et_srb 4928,137246
static boolean channel_encapsulate 4973,138549
#define CIP_BROADCAST 5030,140198
static boolean channel_broadcast 5032,140264
boolean channel_vencap 5079,141339
static void channel_input 5150,143211
static iqueue_t channel_parse_packet 5176,143996
void cip_reset 5270,147139
static void cip_eoir_reset 5392,150232
static void cip_reset_queues_and_buffs 5420,150953
static boolean cip_disable 5446,151626
static boolean cip_enable 5471,152303
static void cip_reset_cfg 5513,153706
static void cip_send_configuration 5570,155302
#define OFFSET_ENABLE_CORR 5600,156250
static void cip_defer_enable 5602,156319
static void cip_deferred_enable 5640,157515
static void cip_plevel_init 5687,158763
static void cip_plevel_adjust_preQ_size 5710,159354
plevel_t *cip_plevel_element_get 5750,160341
static boolean cip_plevel_element_put 5792,161609
static boolean cip_plevel_chunk_to_preQ 5836,162830
static void cip_plevel_preQ_to_chunk 5855,163352
static void cip_plevel_element_dispose 5874,163875
static void cip_purge_queued_events 5907,165130
#define OFFSET_CODE 5949,166352
#define OFFSET_CFG_CODE 5950,166414
#define OFFSET_ASYNC_CORR 5951,166482
#define OFFSET_ASYNC_VC 5952,166529
void cip_plevel_enqueue 5954,166569
static void cip_cfg_state_change 6046,169553
void cip_process_queued_events 6140,172694
static void channel_onesec 6192,174399
static void cip_info_reset 6206,174620
ushort return_NO_CHANNEL_VC 6238,175500
void channel_idb_init 6247,175608
static void channel_love_raw_pak 6388,179889
static void cip_parse_love_letter 6402,180204
static void cip_cfg_acknowledge 6564,185323
static void cip_unexpected_cfg_ack 6779,190696
static void cip_nack_report 6790,190976
static void cip_vc_del_accepted 6855,192533
static void cip_vc_del_rejected 6888,193454
static void cip_vc_cfg_accepted 6911,194123
static void cip_vc_cfg_rejected 6936,194747
data * cip_somethingGet 6965,195458
cip_idb_infotype * cip_cardInfoGet 6999,196311
cip_idb_infotype * cip_interfaceInfoGet 7101,199200
static long cip_find_next_nonzero_link 7157,200917
static cip_vctype * cip_findNextPathDevice(7180,201333
static cip_vctype * cip_getSubchannelInfo 7238,203335
cip_vctype * cip_getSubchannel 7383,207721
static boolean claw_vc_check 7393,208041
cip_vctype * cip_getSubchannelClaw 7397,208175
static boolean claw_config_vc_check 7410,208553
cip_vctype * cip_getSubchannelClawConfig 7415,208725
static boolean offload_config_vc_check 7423,209019
cip_vctype *cip_getSubchannelOffloadConfig 7428,209195
static boolean csna_config_vc_check 7435,209538
cip_vctype *cip_getSubchannelCsnaConfig 7439,209639
data_stats * cip_getDataStats 7446,209990
ulong cip_subchannelWriteBlocksDropped 7461,210313
ulong cip_timeSinceLastReset 7477,210650
boolean cip_daughterBoardStatus 7487,210931
boolean cip_daughterBoardSignal 7494,211173
boolean cip_daughterBoardOffline 7500,211393
get_cip_version 7508,211661
static uchar link_incident_cause_convert 7521,212056
static boolean setup_link_event 7543,212860
static void chan_netmgt_event_handler 7558,213613
#define CIP_MAJVERSION 7583,214346
#define CIP_MINVERSION 7584,214373
#define CIP_EDITVERSION 7585,214400
