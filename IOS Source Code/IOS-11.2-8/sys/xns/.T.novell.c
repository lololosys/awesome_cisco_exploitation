
novell.c,14438
#define NOV_LINKDOWN_POISONS_NEEDED(NOV_LINKDOWN_POISONS_NEEDED1381,57264
dump_nidb 1387,57452
dump_novidbQ 1401,57724
static void nov_free_nidb 1418,57993
static nidbtype *nov_get_nidb 1428,58191
static char *novell_name_auto_mode 1448,58658
void novell_start_nidb_rip_auto 1459,58952
void novell_stop_nidb_rip_auto 1466,59145
void novell_set_nidb_rip_mode 1471,59268
void novell_start_nidb_sap_auto 1489,59701
void novell_stop_nidb_sap_auto 1496,59894
void novell_set_nidb_sap_mode 1501,60017
static void nov_get_rip_metrics(1526,60702
boolean nov_multicast_addr 1549,61460
boolean novell_auto_sw_active 1572,62048
novell_on_idb 1593,62412
boolean novell_on_hwidb 1606,62666
static boolean rcache_cbus_sse_on_this_hwidb 1625,62989
static boolean rcache_cbus_sse_on_any_hwidb 1642,63327
boolean novell_sse_sw_active 1663,63729
static novell_sapqueuetype *novell_findsapqueue 1685,64209
void nov_nlsp_router_command 1743,65993
void novell_router_command 1789,66969
static void nov_get_timestamp 1990,72599
nidbtype *novell_nidb_new 2001,72806
static void novell_algorithms_command 2198,79473
void novell_internal_net_command 2205,79627
static void novell_acctthreshold_command 2314,82819
static void novell_accttransits_command 2321,83029
static void novell_sap_uses_routing_command 2331,83294
static void novell_potential_pseudonode_command 2346,83653
static void novell_nsqrr_command 2357,83928
static void novell_perhost_command 2364,84078
static void novell_type20_in_command 2372,84315
static void novell_type20_out_command 2379,84474
static void novell_netbios_in_command 2386,84635
static void novell_rip_default_route_command 2393,84796
void novell_sap_check_backup_queue 2438,86229
static void nov_insert_sap_placeholder 2461,86775
    *nov_delete_sap_placeholder 2489,87386
static novell_sapentrytype *nov_get_first_sap 2551,88541
static novell_sapentrytype *nov_get_next_sap 2577,88960
static void nov_common_sap_entry_init 2606,89544
static novell_sapentrytype *nov_allocate_sap_entry 2617,89778
static novell_sapentrytype *nov_sap_malloc_placeholder 2633,90089
static void nov_mark_sap_newest 2653,90501
static void mark_sap_changed 2687,91471
static void novell_mark_sap_changed 2712,91853
void novell_mark_srdb_changed 2739,92495
void novell_posit_backup_servers_needed 2803,94903
void novell_paths_command 2812,95104
static void novell_sapqueuemax_command 2912,98199
static void nov_spx_session_command 2923,98428
static void nov_spx_clear_command 2933,98691
static void novell_gns_response_delay_command 2944,98958
static void novell_rcache_inactive_age_rate_command 2965,99592
static void novell_rcache_max_size_command 2978,99938
static void novell_rcache_max_inactive_age_command 2991,100248
static void novell_rcache_update_age_rate_command 3004,100592
static void novell_rcache_max_update_age_command 3017,100930
static void novell_backup_server_interval_command 3030,101288
novell_ping_default_command 3043,101634
nov_route_command 3062,101943
static boolean nov_sap_distance_compare 3249,107375
int nov_sap_hash 3356,110363
static novell_sapentrytype *nov_find_sap_name_entry 3373,110779
static void nov_sap_delete_net_hash 3394,111272
static void nov_sap_free 3433,112363
static void nov_sap_insert_net_hash 3498,114210
static void nov_sap_set_new_dnet 3518,114874
static void novell_place_in_sap_list 3532,115226
nov_sap_command 3585,116872
static void novell_enable_full_rip_updates 3824,123566
static void novell_enable_full_sap_updates 3867,124622
static void novell_disable_full_rip_updates 3927,126382
static void novell_disable_full_sap_updates 3956,126942
static boolean novell_posit_full_rip_update 3985,127505
static boolean novell_posit_full_sap_update 4007,127921
static boolean novell_posit_partial_rip_update 4031,128368
static boolean novell_posit_partial_sap_update 4054,128806
static boolean novell_posit_reverse_rip_update 4078,129267
static boolean novell_posit_reverse_sap_update 4097,129678
static boolean novell_posit_linkdown_rip_update 4118,130115
static boolean novell_posit_linkdown_sap_update 4137,130528
void novell_nidb_bringup 4161,131119
static void nov_kill_connected_route 4268,134748
void novell_nidb_bringdown 4300,135802
static char *novell_nidb_print_state 4331,136724
static void novell_nidb_free 4368,137608
static void fiddle_node_address 4435,139576
void novell_address_command 4489,141810
void nov_hwaddr_notify(4673,146687
static void change_node_address(4696,147393
static boolean ok_to_change_node_address(4727,148078
static void nov_mac_address_change_swidb(4760,149230
void nov_mac_address_change(4809,150352
void ipx_configure_for_lane 4839,151279
void novell_command 4850,151534
static void nov_default_sap_delay_command 5361,165698
static void nov_default_rip_delay_command 5381,166314
static void nov_default_trig_rip_delay_command 5401,166930
static void nov_default_trig_sap_delay_command 5421,167543
void novell_nidb_newstate 5448,168407
void novell_delete_network 5539,171184
void novell_nidb_set_network_number 5568,172287
static boolean novell_non_default_encapsulation 5583,172540
int novell_get_default_encapsulation 5602,173046
static boolean novell_valid_encapsulation 5629,173645
static void novell_network_ifcommand 5672,174827
static void novell_advertise_default_route_only_ifcommand 6024,185094
static void novell_down_ifcommand 6053,185867
static void novell_delay_ifcommand 6102,187250
void novell_sapint_ifcommand 6127,187875
void novell_updatetime_ifcommand 6161,188644
static void novell_sap_delay_ifcommand 6188,189264
static void novell_rip_delay_ifcommand 6199,189537
static void novell_gns_response_delay_ifcommand 6210,189810
void novell_gns_reply_disable_ifcommand 6220,190112
void novell_rip_multiplier_ifcommand 6232,190399
void novell_sap_multiplier_ifcommand 6243,190670
void novell_rip_max_packetsize_ifcommand 6254,190941
void novell_sap_max_packetsize_ifcommand 6265,191218
static void novell_throughput_ifcommand 6275,191494
static void novell_link_delay_ifcommand 6296,192144
static void novell_spx_idle_time_ifcommand 6315,192681
void nov_update_global_acl_flags 6339,193329
static void update_novell_inaccesslist 6381,194400
static void update_novell_accesslist 6394,194758
static void update_novell_helperlist 6405,194976
static void novell_routecache_ifcommand 6415,195160
static void novell_enctype_ifcommand 6608,200665
static void novell_hlpaddr_ifcommand 6703,203189
void novellif_command 6774,205009
void clear_novell_command 7513,226108
void novell_enqueue 7605,228680
void novell_start 7656,230351
void novell_sap_poison 7734,232726
static boolean novell_sap_advertised_everywhere 7764,233564
static void novell_sap_delete_if_advertised 7804,234398
static boolean novell_sapentry_poisoned 7858,236081
novell_sap_adjust 7903,237291
static void novspx_ager 7993,239878
static void novell_sap_ager 8037,241163
boolean novellBLOCK 8144,244608
forktype novell_fork 8171,245471
boolean novell_sap_split_horizon 8389,252520
static void novell_possibly_send_snapshot_null_updates 8437,254112
static boolean nov_any_idb_up 8481,255374
static boolean nov_all_idb_linkdown_poisons 8492,255643
static boolean novell_rip_network_enable 8510,256101
boolean ipx_distribute_sap_list_check 8532,256521
static boolean nov_sap_out_filter 8564,257464
static boolean nov_sap_name_check 8617,258993
static boolean nov_advertising_this_server 8705,262023
static boolean nov_valid_choice_network 8864,267182
static boolean nov_advertising_this_network 9065,273225
static nov_usv **nov_get_nidb_usv_addr 9124,274820
static void debug_display_update_type 9176,275931
static int compute_gap 9214,276555
static boolean nov_reset_usv 9284,278371
static void nov_deallocate_usv 9340,279790
static nov_usv *nov_allocate_and_init_usv 9415,281454
static boolean nov_schedule_update 9573,285669
static boolean nov_allocate_and_init_rip_packet 9707,288894
static boolean nov_allocate_and_init_sap_packet 9749,290057
static void nov_set_outbound_rip_metric 9802,291387
static novndbtype *nov_fill_rip_packet 9874,293915
static novell_sapentrytype *nov_fill_sap_packet 9982,297011
static boolean nov_send_rip_update_packet 10079,299829
static boolean nov_send_sap_update_packet 10182,302393
static void nov_process_atlr 10310,305864
static void nov_super_timer_service 10334,306396
static forktype novell_supervisor 10460,310365
static void nov_process_next_sap_update 10521,311673
static forktype novell_sap_update_daemon 10647,315235
static void nov_process_next_rip_update 10684,316035
static forktype novell_rip_update_daemon 10820,319607
static void nov_net_down_handler_linkdown 10857,320407
static void nov_net_down_handler_completion 11050,325647
static forktype novell_net_down_handler 11105,327359
boolean novell_posit_sap_update 11147,328296
boolean novell_posit_sap_query_response 11216,330127
boolean novell_posit_rip_update 11293,332202
boolean nov_good_query_packet 11386,335164
void novell_rcv_query 11424,336376
void novell_rcv_name_query 11451,336963
inline static boolean novell_sap_compare_lt 11494,338103
inline static boolean novell_sap_compare_eq 11527,339019
inline static boolean novell_sort_sap_mustmove 11541,339400
inline static int novell_sort_sap_select 11556,339745
inline static void novell_sort_sap_enheap 11566,339954
static void novell_sort_sap_list 11579,340250
static int novell_sort_all_saps(11643,341767
static boolean novell_nsokay 11660,342068
static void novell_nearnamequery 11722,343823
novell_rcv_nearquery 11902,349773
novell_prune_nidb_sapq 12102,355836
novell_send_response 12123,356254
void novell_accept_sap_data 12193,358700
void novell_rcv_response 12230,359769
static boolean nov_same_eigrp_nidb_and_shost 12242,360113
void novell_update_sap_route_info 12267,360813
static void ipx_insert_server_backup_info 12373,364223
static nov_urd_type *nov_find_urd 12396,364783
static nov_urd_type *nov_get_urd 12429,365403
void nov_insert_urd 12455,365831
static void ipx_delete_backup_info 12489,366447
void nov_clean_urd_table 12512,366921
static void nov_delete_urd_table 12559,367938
void nov_update_urd_list 12607,369157
static void nov_backup_to_urd_list 12626,369534
static void nov_check_urd_backup_queue 12668,370585
static void display_urd_backup_queue 12700,371444
void nov_display_urd_table 12750,372458
static void nov_up_route 12852,374563
static void nov_modif_route 12892,375870
static void nov_down_route 12933,377426
void novell_addserver 13017,380044
void nov_echo 13739,404446
void novell_printrippak 13806,406434
void novell_printpak 13828,406984
void novell_printnamepak 13855,407845
forktype novell_forwarder 13881,408606
boolean novell_routeBLOCK 13974,411270
forktype novell_router 14004,412170
novell_riprcvupd 14117,415806
novell_rcvriprequest 14237,419622
novell_show_int_brief 14402,424851
void show_novell 14472,426691
static forktype novell_capacity_fork 14549,428488
static void test_novell_capacity 14589,429722
test_novell 14613,430328
static void test_novell_gns 14671,431335
static void test_novell_name 14771,433890
static void test_novell_watchdog 14894,437288
static test_novell_netbios 14941,438557
static void test_novell_ripreq(15077,442405
static void test_novell_debug 15126,443611
uchar *novdbg_proctype2string 15153,444406
uchar *novdbg_event2string 15188,444858
static void test_novell_debug_switch 15216,445458
display_novell 15343,449301
static void free_srdb_list 15537,456641
#define SLEN 15550,456966
print_server 15552,457049
static int cmp_sap_names 15671,460312
static int cmp_sap_nets 15685,460632
static int cmp_sap_types 15699,460927
static void xsort 15722,461534
#define MAX_QSORT_DEPTH 15759,462262
#define MIN_QSORT_PARTITION 15760,462289
void nov_quicksort 15762,462320
static void sort_by_name 15820,463420
static void sort_by_net 15860,464460
static void sort_by_type 15917,465879
void show_novell_servers 15958,466857
void show_novellroute 16109,470622
void show_novelltraffic 16122,470874
void show_novell_spx_spoof 16216,475510
boolean parse_novelladdress 16251,476695
novell_forward 16290,477519
nov_flood 16407,480780
nov_sapcheck 16488,482793
nov_saproutercheck 16556,484502
boolean novell_netbios_acfcheck 16618,485956
static void novrip_redist_callback 16669,487349
static void novsap_redist_callback 16707,488279
static void novmisc_pdb_restart 16729,488672
void novmisc_pdbinit 16742,488872
void novell_common_restart 16802,490928
void novell_common_cleanup 16880,493095
static void novrip_restart 16958,494892
static void novrip_cleanup 16989,495463
void novrip_pdbinit 17007,495957
void novell_send_gen_sap_query 17068,497998
void novell_send_gen_rip_query 17135,500082
void novell_set_timer 17195,501854
void novell_start_aging 17214,502303
void novell_swif_erase 17273,503913
void novell_set_max_hops 17288,504220
novndbtype *nov_get_each_route 17313,504994
novndbtype *nov_get_each_route_per_index 17366,506234
novell_sapentrytype *nov_get_each_service 17415,507378
    *nov_get_each_service_per_index 17468,508758
void ipx_insert_backup_info 17512,509731
void ipx_ndb_self_backup 17634,512967
void ipx_sap_self_backup 17659,513501
void ipx_delete_backup_list 17684,514070
nov_srdb_same_idb 17702,514361
void novrdb_free 17719,514644
void novsrdb_free 17751,515383
void novell_return_static_counts 17778,515996
boolean novell_client_add(17795,516455
boolean novell_client_delete(17819,517112
void novell_pppclient_configure(17849,517975
idbtype *novell_get_owner_idb(17899,519025
void nov_network_finish 17912,519230
boolean allocate_novpdbindex 18002,521732
int nv_write_patricia(18024,522191
boolean ipx_validate_contiguous_mask(18052,522975
int patricia_paths_command(18075,523387
int patricia_start_aging 18113,524370
void nov_set_ndb_flags 18151,525346
void nov_network_redist 18167,525669
static void nov_invalidate_redist 18213,527134
static void nov_table_prune_pdb 18239,527729
void nov_router_erase 18301,529293
void nov_service_redist 18416,532610
void novell_redist_command 18452,533695
void novell_distributesaplist_command 18692,541148
void novell_distributelist_command 18810,544496
forktype nov_router_init 18923,547790
boolean novigrp_net_in_pdb 18942,548248
void nov_network_command 18959,548593
