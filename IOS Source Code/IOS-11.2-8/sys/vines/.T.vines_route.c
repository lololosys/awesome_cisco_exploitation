
vines_route.c,4087
#define VINT_TIME_ROUTE_START	186,6628
#define VINT_TIME_ROUTE_END	187,6661
#define VINT_TIME_NBR_START	188,6692
#define VINT_TIME_NBR_END	189,6723
#define VINT_TIME_CACHE_START	190,6752
#define VINT_TIME_CACHE_END	191,6785
#define VINT_TIME_PROXY_START	192,6816
#define VINT_TIME_PROXY_END	193,6849
#define VINT_TIME_RTP_START	194,6880
#define VINT_TIME_RTP_END	195,6911
#define VINT_TIME_MAX	196,6940
#define VINT_COUNT_RTP	199,7013
#define VINT_COUNT_MAX	200,7038
extern inline void vint_record_time 203,7105
extern inline void vint_record_value 210,7243
void vines_route_init 229,7669
void vines_unlock_nodes 277,9000
void vines_lock_nodes 294,9468
void vines_compute_server_metric 311,9924
void vines_build_route_list 367,11564
void vines_fixup_server 398,12377
boolean vines_fixup_one_server 416,12907
void vines_fixup_servers_by_neighbor 436,13369
boolean vines_check_route_list 454,13835
boolean vines_verify_server_structure 488,14930
ulong vroute_get_route_count 527,16034
ulong vroute_get_lookup_success 531,16100
ulong vroute_get_lookup_failed 535,16178
void vines_insert_new_route 555,16788
vinesroutetype *vines_make_new_route 586,17802
vinesroutetype *vines_add_route 608,18439
static inline vinesnettype *vines_unlink_route 660,19755
vinesnettype *vines_delete_this_route 690,20400
vinesnettype *vines_delete_route_by_xxx 723,21153
static inline vinesnettype *vines_delete_routes_by_flags 756,21932
static inline vinesnettype *vines_delete_routes_by_neighbor 763,22117
static inline vinesnettype *vines_delete_routes 772,22367
void vines_flush_dynamic_routes 784,22693
boolean vines_flush_one_route 813,23410
void vines_flush_routes_by_neighbor 827,23768
boolean vines_poison_one_route 850,24393
void vines_poison_routes_by_neighbor 886,25443
static boolean vines_start_aging_one_route 903,25834
void vines_start_aging_routes_by_neighbor 929,26577
vinesnettype *vines_add_server 946,27053
void vines_delete_server_entry 977,27847
void vines_delete_server 1009,28664
void vroute_update_flags 1025,29111
static void vines_dump_server 1046,29587
vinesroutetype *vines_get_route_by_xxx 1078,30606
boolean vines_server_uses_idb 1126,32010
int vines_get_server_metric 1150,32546
boolean vines_first_hop 1170,33287
boolean vines_first_hop_test 1244,35356
boolean vroute_age_one_net 1290,36452
void vroute_age_all_nets 1386,39178
void vroute_interface_came_up_work 1401,39554
void vroute_interface_came_up 1414,39985
void vroute_interface_went_down_work 1427,40362
void vroute_interface_went_down 1453,41147
void vroute_vc_added_work 1464,41479
void vroute_vc_added 1485,42114
void vroute_vc_removed_work 1509,42697
void vroute_vc_removed 1516,42796
static void vines_delete_if_work 1525,43025
void vines_delete_if 1535,43265
boolean vines_reload_entry 1548,43681
void vines_reload_imminent 1560,44039
char vroute_nettype 1600,45227
void vines_display_route_list 1623,45692
boolean vroute_display_one_net 1720,48051
void vroute_display_all_nets 1750,48885
void vroute_display_nets_by_metric 1783,49873
void vines_display_times 1807,50548
void vroute_ShowStats 1849,51891
uchar *vroute_decode_packet 1862,52159
boolean vines_from_reverse_path 1895,53105
vinesidbtype *vines_get_reverse_idb 1938,54414
boolean vines_nearest_server 1968,55438
ulong vines_nearest_server_net 2015,56581
void vines_new_server 2035,56995
void vroute_all_use_neighbor 2047,57343
void vines_ager 2081,58302
static void vroute_process_queues 2115,59318
static void vroute_query_timer_expired 2219,62058
static void vroute_wakeup_timer_expired 2234,62421
static void vroute_sync_timer_expired 2250,62866
static void vines_periodic_if_check 2264,63269
static void vroute_do_timers 2285,63725
void vroute_process_messages 2363,65693
static void vines_router_teardown 2434,67363
process vines_router 2454,67960
void vroute_send_full_updates 2508,69239
void vroute_send_change_updates 2539,69956
void vroute_send_requests 2570,70675
void vroute_send_redirects 2585,70964
void vines_set_route_timestamp 2605,71427
