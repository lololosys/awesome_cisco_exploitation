
igrp2.c,5762
static boolean igrp2_pdebug 446,20613
void igrp2_set_debug_ddb 465,21038
static inline boolean igrp2_induce_error 477,21226
static inline boolean igrp2_induce_error 494,21638
static void igrp2_log_peer_change 508,21836
boolean igrp2_xmit_debug_ok 530,22483
static boolean igrp2_detail_debug 544,22851
static inline boolean igrp2_multicast_transmit_blocked 563,23556
boolean igrp2_peer_is_flow_ready 583,24376
boolean igrp2_interface_is_flow_ready 606,25259
static boolean igrp2_defer_peer_timer 625,25760
static xmit_queue_type igrp2_next_peer_queue 650,26505
static void igrp2_start_peer_timer 685,27512
static void igrp2_kick_interface 730,28808
static void igrp2_kick_peer 748,29308
static void igrp2_kick_pacing_timer 763,29650
static void igrp2_bind_pakdesc(780,30026
static void igrp2_free_qelm 797,30553
static void igrp2_cleanup_multipak 840,31555
static void igrp2_start_mcast_flow_timer 862,32145
static void igrp2_update_mcast_flow_state 906,33587
static void igrp2_flush_queue 995,36271
static void igrp2_dequeue_qelm 1012,36721
paktype *igrp2_getpak 1045,37562
igrp2type *igrp2_getbuf 1060,37780
dual_peertype *igrp2_handle_to_peer 1073,38023
#define PEER_TABLE_INCREMENT 1090,38471
static boolean igrp2_adjust_peer_array 1091,38535
static boolean igrp2_adjust_handle_array 1125,39480
static void igrp2_set_handle 1165,40556
void igrp2_clear_handle 1189,41222
boolean igrp2_test_handle 1217,41998
static ulong igrp2_calculate_pacing_interval 1235,42555
static void igrp2_set_pacing_intervals 1288,43997
static void igrp2_allochandle 1342,46211
void igrp2_freehandle 1408,47931
int igrp2_findhandle 1429,48421
ulong igrp2_cdelay 1458,49020
void igrp2_metric_decode 1476,49416
void igrp2_metric_encode 1496,49932
ulong igrp2_compute_metric 1546,51393
static void igrp2_get_sw_version 1626,53640
static uchar *igrp2_hello 1641,54057
char *igrp2_otype 1688,55520
int igrp2_dual2opc 1701,55751
void igrp2_bugpak 1718,56198
static igrp2type *igrp2_pak_sanity 1809,58755
static int igrp2_compare_seq 1873,60540
static void igrp2_send_ack 1896,61031
static inline void igrp2_cancel_cr_mode 1920,61573
    packet_accept_type;packet_accept_type1933,61959
static packet_accept_type igrp2_should_accept_seq_packet 1935,61984
static boolean igrp2_accept_sequenced_packet 2030,64400
static void igrp2_update_srtt 2085,65611
static void igrp2_calculate_rto 2112,66266
static void igrp2_post_ack 2149,67137
static void igrp2_process_ack 2174,67873
static void igrp2_neighbor_down 2222,69090
static void igrp2_reinit_peer 2252,69975
static void igrp2_find_sequence_tlv 2302,71663
static void igrp2_receive_hello 2367,73397
dual_peertype *igrp2_receive 2522,77350
dual_peertype *igrp2_find_peer 2733,82962
static ulong igrp2_next_seq_number 2764,83575
static void igrp2_set_seq_number 2783,83955
igrp2_pakdesc *igrp2_enqueue 2803,84555
static boolean igrp2_retransmit_packet 2869,86326
static igrp2type *igrp2_generate_packet 2916,87848
static ulong igrp2_pacing_value 2991,89646
static void igrp2_generate_checksum 3011,90211
static void igrp2_send_unicast 3027,90593
static igrp2_gentlvtype *igrp2_build_sequence_tlv 3291,97580
static void igrp2_send_hello_packet 3337,98704
static void igrp2_send_seq_hello 3371,99895
static boolean igrp2_enqueue_peer_qelms 3429,101564
static void igrp2_flow_block_interface 3532,104451
static void igrp2_send_multicast 3569,105537
static void igrp2_peer_send_timer_expiry 3735,110814
static void igrp2_interface_pacing_timer_expiry 3783,112609
static void igrp2_show_neighbor_q 3903,116173
static void igrp2_print_thread_serno 3941,117231
static void igrp2_print_anchored_serno 3956,117528
void igrp2_show_interface 3975,118012
static char *igrp2_display_one_timer 4049,120197
void igrp2_show_timers 4122,121796
void igrp2_show_neighbors 4143,122480
static void igrp2_print_send_flag 4209,124325
void igrp2_show_topology 4238,124867
igrp2_idbtype *igrp2_find_iidb 4500,131680
igrp2_idbtype *igrp2_create_iidb 4532,132364
static void igrp2_link_new_iidb 4591,134346
static igrp2_idbtype *igrp2_get_idb 4616,134853
igrp2_idbtype *igrp2_find_temp_idb 4652,135758
void igrp2_flush_iidb_xmit_queues 4674,136375
static void igrp2_destroy_iidb 4697,137023
void igrp2_process_starting 4755,138459
static void igrp2_process_dying 4806,139745
void igrp2_free_ddb 4835,140453
boolean igrp2_allocate_iidb 4932,142845
void igrp2_deallocate_iidb 4987,143979
void igrp2_passive_interface 5031,144945
void igrp2_take_all_nbrs_down 5073,145942
void igrp2_take_nbrs_down 5096,146622
void igrp2_take_peer_down 5125,147385
void igrp2_flush_peer_packets 5142,147856
void igrp2_destroy_peer 5204,149834
static void igrp2_send_hello 5274,151474
void igrp2_set_hello_interval 5309,152361
void igrp2_set_holdtime 5333,152763
void igrp2_set_holddown 5355,153109
void igrp2_set_split_horizon 5376,153439
void igrp2_set_bandwidth_percent 5399,153911
void igrp2_update_mib 5422,154346
void igrp2_show_traffic 5455,154923
void igrp2_for_each_peer 5489,156180
static void igrp2_flow_control_timer_expiry 5535,157267
#define TIMER_LIMITER 5554,157702
boolean igrp2_process_managed_timers 5555,157766
boolean igrp2_default_timers 5720,161851
static void igrp2_delay_changed 5738,162280
static void igrp2_bandwidth_changed 5766,162909
void igrp2_probe_peer 5808,163909
void igrp2_probe_interface 5825,164270
void igrp2_start_acl_timer 5846,164860
#define IGRP2_HELLO_PACKET_LIMIT 5866,165285
static process igrp2_hello_process 5867,165358
boolean igrp2_init_hello_process 6024,169061
static void igrp2_init 6043,169532
#define EIGRP_MAJVERSION 6056,169883
#define EIGRP_MINVERSION 6057,169910
#define EIGRP_EDITVERSION 6058,169937
