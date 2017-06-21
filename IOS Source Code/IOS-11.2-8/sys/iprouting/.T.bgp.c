
bgp.c,9554
#define	BGP_PRIVATE 535,21213
#define	BGP_PUBLIC 536,21240
#define	WALK_CONTINUE	659,25908
#define	WALK_ABORT	660,25934
#define	PREFIX2BYTES(PREFIX2BYTES694,26820
#define	PREFIX2MASK(PREFIX2MASK695,26874
bgp_writeprefix 719,27610
bgp_readprefix 738,27933
bgp_printmsg 765,28413
bgp_getbuffer 791,29009
bgp_free_writeQ(822,29804
bgp_reset_cache 839,30062
bgp_reset_rcache 859,30401
bgp_reset_fcache 867,30552
bgp_find_cond_adv_map 873,30698
bgp_link_camap 891,31173
bgp_delink_camap 923,31778
bgp_statestring 964,32580
bgp_statechange 984,33061
bgp_close 1017,34095
bgp_send_open 1078,35889
bgp_english_notification 1138,37584
bgp_notification_data 1200,39126
bgp_send_notification 1224,39534
bgp_rcv_notification 1273,40849
bgp_send_keepalive 1325,42254
bgp_rcv_open 1345,42653
bgp_netread 1571,49550
bgp_open_active 1657,51970
bgp_open_check 1821,56405
bgp_guess_md5_key 1888,58166
bgp_open_passive_guts 1920,58947
bgp_accepter 1984,60574
bgp_open_passive 2007,61041
bgp_lock_msg 2049,61999
bgp_free_msg 2062,62184
bgpioBLOCK 2075,62387
bgp_io 2129,63631
bgp_bump_net_version 2260,67018
rn2bnettype(2271,67271
bgp_netlookup 2282,67439
bgp_nextnode 2302,67952
bgp_subtreelookup 2344,68999
#define	net	2346,69057
bgp_fwdtable_lookup 2395,70278
bgp_advertised_mask 2412,70707
bgp_set_advertised_bit 2427,71002
bgp_clear_advertised_bit 2441,71300
bgp_advertised 2455,71613
bgp_suppress_unreachable 2472,72108
bgp_reset_bit_walker 2490,72418
bgp_create_net 2505,72716
bgp_attr2neighboras 2536,73453
bgp_get_pathindex 2579,74710
bgp_link_path 2652,76901
bgp_delink_path 2678,77558
bgp_enlarge_net 2702,78058
bgp_add_path 2800,81150
bgp_relocate_path 2875,83186
bgp_replace_path 2912,84297
bgp_dinfo_free 2950,85365
bgp_dinfo_lock 2972,85748
bgp_free_dampinfo 2987,86004
bgp_rechoose_best_MED_path 3014,86808
bgp_delete_path 3070,88409
bgp_find_usedpath 3144,90472
bgp_find_path 3167,90961
bgp_add_rcvdpath 3181,91219
bgp_ospftag_check(3288,94232
bgp_redist_check 3309,94772
bgp_path2neighbor 3377,96413
bgp_pathlength 3395,96742
bgp_pathvalid 3424,97252
bgp_path2string 3458,97893
bgp_path2originas 3527,99573
bgp_path_union 3580,100759
bgp_route_map_match_routetype 3649,102760
route_map_match_community 3691,103648
bgp_filter_list_check 3723,104454
bgp_route_map_check 3757,105452
bgp_com_union 3914,109489
bgp_special_community 3944,110306
bgp_get_routemap_setmetric 3972,111011
bgp_route_map_rcv_set 3998,111500
bgp_nonmap_path_policy 4107,114691
bgp_path_policy 4167,116155
bgp_best_metric_inline 4220,117551
bgp_best_metric 4248,118134
bgp_reset_gwcache 4264,118430
bgp_compare_med 4285,118753
bgp_bestpath 4323,119822
bgp_nosource 4674,129518
bgp_unsync 4698,129981
bgp_find_attr 4724,130559
bgp_calculate_origin 4832,134055
bgp_update_origin 5017,139299
bgp_setorigin 5073,141083
bgp_edit 5106,142266
bgp_free_parent_alist 5209,145088
bgp_extract_community 5229,145487
bgp_find_set_attr 5271,146693
bgp3_rcv_update 5310,147518
bgp_dinfo_malloc 5839,161173
bgp_decay_penalty 5856,161378
bgp_get_reuseindex 5895,162391
bgp_get_reuseticks_inline 5913,162653
bgp_get_reuseticks 5971,164277
bgp_suppress 5985,164585
bgp_check_routemap_dampening 6035,166040
bgp_charge_penalty 6068,166836
bgp_damp_update 6166,169531
bnettype *bgp_route_map_net_accesslist 6200,170525
bnettype *bgp_route_map_find_net 6264,172157
bgp4_rcv_update 6311,173303
bgp3_find_update 7431,203840
bgp4_find_update 7464,204696
bgp3_format_unreachable 7520,206114
bgp4_format_unreachable 7614,208361
bgp3_format_update 7662,209435
bgp4_format_update 7933,216445
ipaddrtype bgp_next_hop 8443,229829
bgp_update_policy 8588,233868
bgp_send_update 8663,236089
bgp_pgrp_enqueue_updates 8918,243680
bgp_enqueue_updates 9038,246931
bgp_synchronize_pgrp_members 9085,248241
bgp_clear_peergroup_bit 9118,248980
bgp_update_walker 9159,250103
bgp_updates 9219,252304
bgp_clean_dampinfo 9327,255455
bgp_free_dampinfo_walker 9358,256102
bgp_free_dampctrlblk 9372,256430
bgp_create_decay_array 9395,256819
bgp_create_reuse_helper 9452,258813
bgp_create_reuse_array 9510,260385
bgp_start_dampening 9586,262717
bgp_deconfig_dampening 9646,264223
bgp_config_dampening(9679,265169
bgp_redist_check_subnets 9766,267526
bgp_scan_walker 9789,268069
bgp_network_check 9970,273203
bgp_nettable_scan 10022,274471
bgp_scanner 10235,280724
bgp_rdb_update 10262,281382
bgp_revise_route 10285,281863
bgp_update_nettable_walker 10362,283879
bgp_update_nettable 10553,289473
bgp_docommand 10572,289876
bgp_nettable_add_req 10663,292189
bgp_service_add_req 10689,292814
bgp_nettable_query 10849,297464
bgp_network_command 10860,297669
bgp_redist_callback 10990,300832
bgp_service_callback 11038,301915
bgp_rtr_id 11188,305570
bgp_reset_walker 11224,306567
bgp_reset 11285,308188
bgp_service_reset 11409,311387
bgp_free_neighbor 11439,312077
bgp_delete_neighbor 11464,312691
bgp_delete_group 11533,314126
bgp_backdoor 11576,315123
bgp_free_aggregate 11594,315447
bgp_suppress_children 11609,315817
bgp_calculate_aggregate 11669,317442
bgp_update_aggregate 11834,322382
bgp_setaggregate 11884,323797
bgp_morespecific 11912,324515
bgp_aggregate 11950,325467
bgp_unsuppress 12063,328686
bgp_default_originate 12101,329655
bgprouterBLOCK 12173,331442
bgp_upd_candidate 12197,331987
bgp_config_multipath_walker 12260,333717
bgp_config_multipath(12287,334298
bgp_accept_path 12303,334618
bgp_deny_path 12399,337490
bgp_reconfig_path 12443,338560
bgp_soft_reconfig_walker 12552,341366
bgp_soft_reconfig 12624,343077
bgp_enlarge_net_walker 12718,345493
bgp_enlarge_neighbor_bitfield 12735,345815
bgp_set_neighbor_index 12792,347330
bgp_neighbor_indexing 12822,348144
bgp_router 12895,350066
bgp_async_reset 13265,359848
bgp_route_adjust 13285,360227
clear_ip_bgp_dampinfo 13306,360803
clear_ip_bgp_pgrp 13334,361426
bgp_clear_fstat_flist_walker 13360,361941
bgp_clear_fstat_regexp_walker 13395,362751
bgp_clear_fstat_neighbor_walker 13429,363520
bgp_clear_fstat 13455,364063
bgp_clear_fstat_walker 13474,364396
clear_ip_bgp_soft 13491,364704
clear_ip_bgp 13594,367181
bgp_distance 13709,369750
bgp_originstring 13726,370133
bgp_originchar 13742,370496
bgp_print_entry 13759,370833
bgp_process_onceflag 13819,372521
bgp_process_fstat_onceflag 13828,372724
bgp_print_fstat_entry 13842,373015
bgp_show_walker 13899,374564
bgp_show_inconsistent_walker 13947,375744
bgp_show_nets_walker 14028,377820
bgp_show_filter_list_walker 14096,379447
bgp_show_regexp_walker 14160,381000
bgp_calc_memory_walker 14226,382672
bgp_show_network_detail 14264,383670
bgp_show_paths 14430,388624
bgp_show_nets 14497,390207
bgp_show_community_walker 14520,390810
bgp_show_communitylist_walker 14588,392488
bgp_show_dampinfo 14644,393747
bgp_show_dampened_routes 14709,395335
bgp_show_fstat 14764,396727
bgp_show_fstat_walker 14799,397612
bgp_show 14820,398018
bgp_show_neighbor_paths_walker 14997,402814
bgp_show_neighbor_paths 15091,405271
bgp_display_summary_header 15160,406829
bgp_display_summary_entry 15249,409816
bgp_show_pgrp_summary 15277,410712
bgp_show_summary 15319,411576
bgp_show_single_neighbor 15349,412220
bgp_show_pgrp 15483,417403
bgp_show_neighbors 15529,418366
bgp_show_neighbor_route_walker 15583,419479
bgp_show_neighbor_advertised_routes_walker 15680,422312
bgp_show_neighbor_routes 15726,423494
bgp4_pathattr_walker 15821,426031
bgp4_find_pathattr 15919,429026
bgp_as_parse 15943,429583
bgp_filter_parse 15990,430535
bgp_init_tcp_listener 16047,431650
bgp_schedule_neighbor_indexing 16112,433151
bgp_neighbor_init 16168,434660
bgp_route_map_config 16218,436076
bgp_pgrp_member 16243,436603
bgp_neighbor_parse 16384,440779
bgp_nv_neighbor 16833,452056
bgp_neighbor_valid_command 16973,457460
bgp_neighbor 17093,460729
bgp_aggregate_command 17231,464106
bgp_distance_command 17346,467070
bgp_dampening_command 17381,467844
bgp_command 17472,470490
bgp_confed_peers_command 17576,473184
bgp_timers_command 17674,475401
route_map_free_community 17707,476074
BGP_PUBLIC void route_map_match_aspath_command 17728,476506
BGP_PUBLIC void route_map_set_as_path_tag_command 17743,476788
BGP_PUBLIC void route_map_set_as_path_prepend_command 17759,477100
BGP_PUBLIC void route_map_set_as_path_prepend_fresh_command 17798,478162
valid_match_community_command(17815,478620
BGP_PUBLIC void route_map_match_community_command 17835,479082
route_map_process_community_item(17913,480969
void route_map_set_community_command 17948,481717
void route_map_set_weight_command 17984,482589
void route_map_set_dampening_command 18001,482921
void route_map_set_destpref_command 18045,484026
void route_map_set_origin_command 18062,484363
void route_map_set_localpref_command 18081,484767
BGP_PRIVATE void bgp_route_map_match_nvgen 18100,485135
BGP_PRIVATE void bgp_route_map_set_nvgen 18127,485816
BGP_PRIVATE void bgp_show_route_map_match 18188,487729
BGP_PRIVATE void bgp_show_route_map_set 18217,488396
bgp_show_ip_proto 18279,490209
bgp_ip_path 18400,493723
bgp_ip_originas 18426,494309
bgp_community_list_callback 18451,494894
bgp_access_list_callback 18466,495229
bgp_aspath_access_list_callback 18480,495535
bgp_set_route_map_head 18489,495673
bgp_route_map_inform 18505,496034
bgp_cleanup 18576,498445
bgp_pdb_init 18667,501154
bgp_init 18848,507530
#define BGP_MAJVERSION 18903,509293
#define BGP_MINVERSION 18904,509318
#define BGP_EDITVERSION 18905,509343
