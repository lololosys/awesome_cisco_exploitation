
ipigrp2.c,6948
#define	ALTERNATE	367,13737
#define	ALTERNATE	376,13910
#define	ALTERNATE	385,14082
#define	ALTERNATE	393,14254
#define	ALTERNATE	401,14429
#define	ALTERNATE	409,14613
#define	ALTERNATE	417,14812
#define	ALTERNATE	425,15009
#define ALTERNATE 433,15214
static ulong ipigrp2_gettag 457,16160
static void ipigrp2_set_routerid 475,16553
static void ipigrp2_qevent 489,16882
static inline boolean ipigrp2_callback_sanity 501,17140
static pdbtype *ipigrp2_find_pdb 521,17492
static ipsocktype *find_igrp2socket 539,17820
static void ipigrp2_process 554,18147
static ulong ipigrp2_mmetric_fudge 583,18788
static int ipigrp2_addrextract 595,19126
static inline boolean ipigrp2_exteriors_out 620,19815
static inline boolean ipigrp2_exteriors_in 645,20476
static char *ipigrp2_printnet 668,20987
static boolean ipigrp2_process_routes 681,21384
static void ipigrp2_receive 817,25399
static void ipigrp2_write 911,27616
static void ipigrp2_cleanup 951,28766
static void ipigrp2_receive_probe 972,29276
void ipigrp2_timers_command 1000,30024
static boolean ipigrp2_ager 1039,30878
static void ipigrp2_rdbupdate 1051,31120
static rdbtype *ipigrp2_proto_table_add 1074,31771
static summary_revise_result ipigrp2_summary_revise 1121,33009
static void ipigrp2_process_summary 1186,35091
static void ipigrp2_summary_depend 1228,36138
static boolean ipigrp2_rtupdate 1282,37413
static boolean ipigrp2_addrmatch 1361,39208
static char *ipigrp2_printaddr 1371,39409
static boolean ipigrp2_netmatch 1381,39624
static ulong ipigrp2_addrcopy 1393,39988
static boolean ipigrp2_localaddr 1406,40276
static void ipigrp2_rtdelete 1417,40532
static boolean ipigrp2_holddown 1467,41873
static ulong ipigrp2_rtgetmetric 1501,43128
static int ipigrp2_peerbucket 1515,43480
static int ipigrp2_ndbbucket 1524,43647
static igrp2type *ipigrp2_headerptr 1534,43848
static ulong ipigrp2_paklen 1558,44366
static void ipigrp2_conn_rtchange 1572,44591
static boolean ipigrp2_is_exterior 1600,45340
void ipigrp2_show_neighbors 1635,46151
static char *ipigrp2_protid_string 1672,46978
static void ipigrp2_show_topology_route 1722,47714
void ipigrp2_show_topology 1840,51975
static boolean ipigrp2_auto_summary_needed 1885,52973
static sum_idbtype *ipigrp2_find_sum_idb 1937,54163
static void ipigrp2_delete_summary_interface 1955,54575
static void ipigrp2_configure_summary 1981,55403
static void ipigrp2_build_auto_summaries 2068,57507
static boolean ipigrp2_interface_up 2118,58971
static void ipigrp2_adjust_connected 2128,59182
static void ipigrp2_ifdown 2161,60031
static void ipigrp2_setup_multicast 2193,60750
static void ipigrp2_set_mtu 2239,61926
static void ipigrp2_mtu_change 2253,62174
static void ipigrp2_onoff_idb 2286,63041
static void ipigrp2_enqueue_redist 2315,63719
static void ipigrp2_redist_conn_state 2346,64549
static void ipigrp2_conn_summary_depend 2382,65528
static void ipigrp2_redistribute 2413,66406
static void ipigrp2_network_address 2454,67443
static void ipigrp2_network_command 2478,68265
static void ipigrp2_passive_interface_command 2573,70547
static void ipigrp2_ip_address_command 2604,71474
static void ipigrp2_process_reload_iptable 2687,74060
static void ipigrp2_reload_iptable 2702,74369
static void ipigrp2_process_exterior_change 2739,75296
static void ipigrp2_exterior_change 2801,77028
static int ipigrp2_get_prot_id 2828,77716
static ipigrp2_extdatatype *ipigrp2_build_external_info 2876,78638
static idbtype *ipigrp2_find_ndb_idb 2921,79810
static boolean ipigrp2_pick_redist_metric 2966,80893
static void ipigrp2_rtchange 3060,84141
static void ipigrp2_process_redistribute 3235,89286
static inline boolean ipigrp2_redist_conn_sanity 3291,91185
static void ipigrp2_redist_callback 3317,91825
static void ipigrp2_candidate_update 3388,93888
static void ipigrp2_process_redist 3419,94683
static void ipigrp2_process_backup 3471,96198
static void ipigrp2_process_lost 3491,96644
static boolean ipigrp2_get_summary_metric 3512,97156
static void ipigrp2_process_ifdown 3604,99512
static void ipigrp2_process_summary_entry 3639,100265
static void ipigrp2_process_passive_intf 3746,102965
static void ipigrp2_process_conn_change 3788,104020
#define WORKQ_LIMITER 3867,105962
static boolean ipigrp2_process_workq 3868,106010
static void ipigrp2_defaultmetric_command 3934,107687
static void ipigrp2_set_distflags 3977,108701
void ipigrp2_commands 3999,109158
static void ipigrp2_summary_address_command 4083,110926
void ipigrp2_if_commands 4132,112173
static void ipigrp2_callbackup_routes 4263,115563
static void ipigrp2_lostroute 4292,116305
void ipigrp2_show_event 4318,116848
void ipigrp2_show_traffic 4344,117379
void ipigrp2_show_timers 4376,117996
static void ipigrp2_metric_command 4405,118486
static boolean ipigrp2_accept_peer 4426,118949
static advertise_request_type ipigrp2_advertise_request 4450,119588
static void ipigrp2_show_summary_entry 4541,121939
static void ipigrp2_show_auto_summary 4581,122908
static void ipigrp2_show_summary 4599,123311
static void ipigrp2_auto_summary_command 4624,123948
static void ipigrp2_encap_fixup 4665,124932
void ipigrp2_clear_event_logging 4692,125534
void ipigrp2_clear_log 4721,126089
void ipigrp2_clear_neighbors 4750,126640
static ddbtype *ipigrp2_find_ddb 4815,127987
void ipigrp2_test_command 4839,128564
void ipigrp2_debug_command 4873,129231
static void ipigrp2_debug_all 4938,130937
static int ipigrp2_distance 4961,131443
static enum DISTTYPE ipigrp2_distcmp 4986,132149
static inline uchar ipigrp2_exterior_bit 5022,133184
static uchar ipigrp2_exterior_flag 5039,133599
static boolean ipigrp2_exteriors_differ 5062,134214
static void ipigrp2_exterior_check 5096,134974
static boolean ipigrp2_compare_external 5116,135538
static void ipigrp2_transport_ready 5140,136189
static igrp2type *ipigrp2_build_packet 5192,137807
static ulong ipigrp2_item_size 5225,138616
static ulong ipigrp2_add_item 5247,139254
void ipigrp2_error_cmd 5357,142513
void ipigrp2_probe_command 5411,143688
static void ipigrp2_enable_process_wait 5469,144865
static paktype *ipigrp2_input_packet 5485,145191
#define PACKET_LIMITER 5512,145712
static process ipigrp2_router 5513,145766
static void ipigrp2_pdm_timer_expiry 5656,149298
static void ipigrp2_route_adjust 5670,149603
static void ipigrp2_interface_change 5709,150523
static boolean ipigrp2_ddbinit 5723,150770
static void ipigrp2_showipproto 5822,153636
static void ipigrp2_show_global_traffic 5853,154747
static void ipigrp2_start_acl_timer 5868,155240
static void ipigrp2_acl_changed 5884,155488
static void ipigrp2_access_list_callback 5902,156008
static void ipigrp2_hopcount_changed 5949,157091
static pdbtype *ipigrp2_pdb_init 5963,157376
static void ipigrp2_init 6052,160294
#define IPIGRP2_MAJVERSION 6084,161516
#define IPIGRP2_MINVERSION 6085,161545
#define IPIGRP2_EDITVERSION 6086,161574
