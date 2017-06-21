
dialer.c,4009
dialer_holdq_type *create_dialer_holdq 700,27365
void dialer_holdq_timer_start 725,28049
void dialer_holdq_free 748,28568
dialerentrytype *dialer_name_lookup 767,28988
dialermaptype *dialer_addr_lookup 795,29582
dialerentrytype *dialer_string_lookup 833,30364
void dialer_goingdown 876,31272
static void dialer_comingup 904,32051
void dialer_shutdown 926,32515
void dialer_reset 982,34016
static dialergrouptype *dialer_find_group 999,34374
static boolean dialer_link_member 1108,37481
void dialer_unlink_member 1200,39904
void dialer_disconnect_group 1289,42390
boolean dialer_check_number 1318,43150
boolean dialer_calling_num 1338,43616
static boolean dialer_classify 1404,45545
boolean dialer_async_classify 1516,48147
void dialer_get_packet_header 1535,48482
static void dialer_packet_hexdump 1560,49265
boolean dialer_connected 1586,49917
dialerdbtype *dialer_pending 1640,51485
static void add_oddparity 1676,52366
static boolean dial_v25bis 1692,52680
static boolean dial_dtr 1742,54175
boolean dialer_connect_name 1766,54925
static char *find_dialer_string 1810,56010
static dialermaptype *find_dialer_addr 1844,56987
static dialermaptype *find_dialer_addr_name 1888,58076
void dialer_set_number_called 2028,62318
void dialer_cannot_dial 2098,64318
void dialer_set_call_data 2136,65308
boolean dial_if 2164,66111
void dialer_fail 2322,70491
static void dialer_failed 2365,71549
dialerdbtype *pick_best_dialer_idb 2383,71867
static dialerdbtype *find_best_output_idb 2466,74328
static dialerdbtype *connected_to 2504,75322
static inline boolean dialer_connected_to_inline 2525,75801
static boolean dialer_connected_to 2554,76490
inline static boolean in_transition 2568,76845
boolean dialer_is_free 2580,77108
get_free_dialer 2593,77341
void start_fast_idle 2729,80877
static boolean dialer_execute_save_vencap 2772,82159
static void dialer_holdq_output 2794,82624
void dialer_holdq_discard_queue 2885,84940
void dialer_enqueue_pak 2922,85726
static void dialer_protocol_up 2972,87076
boolean dialer_place_call 3069,89551
static dialerdbtype *dialer_process_packet 3101,90400
static void dialer_process_broadcast 3236,95014
static dialerdbtype *dialer_broadcast 3284,96416
void dialer_disconnect 3407,100375
static void dialer_set_subif_state 3466,101584
boolean dialer_keep_dtr_low 3485,102158
static boolean dtr_dialer_return 3512,102739
static boolean is_spoofing_int 3545,103501
static boolean is_spoofing_prot 3559,103715
boolean huntgroup_member 3573,103931
boolean huntgroup_leader 3587,104139
static boolean dialer_is_in_use 3602,104383
static void for_all_huntgroup_members 3617,104656
static char *get_dialer_string 3636,105040
static ulong dialer_compute_load 3651,105357
static void dialer_load_analyze 3723,107013
static void dialer_timers 3842,110904
static boolean dialer_change_pak_if 4010,116105
void dialer_raw_enqueue 4042,117201
void dialer_remote_name 4143,120405
addrtype *dialer_remote_addr 4210,122479
dialer_lookup_caller 4248,123386
void dialer_out_call_connected 4302,124582
void dialer_in_call_connected 4337,125478
void dialer_call_disconnected 4361,126165
static void dialer_statechange 4445,128653
boolean dialer_vencapsulate 4565,132227
static void dialer_sync_drop 4663,135112
void dialer_dtr_drop 4673,135286
void dialer_initiate_connection 4683,135493
static inline void dialer_reset_timers_inline 4716,136197
void dialer_reset_timers 4737,136704
static void dialer_return_connected_hwidbs_inline 4752,136951
void dialer_return_connected_hwidbs 4789,138117
static boolean dialer_serial_is_answer 4800,138428
static boolean dialer_bcast_default 4811,138766
static hwidbtype *dialer_pool_acl_idb 4824,139099
static hwidbtype *dialer_get_conn_member 4841,139471
static dialergrouptype *dialer_get_connected_group 4867,140114
static void dialer_init 4906,140931
#define DIALER_MAJVERSION 5012,146393
#define DIALER_MINVERSION 5013,146421
#define DIALER_EDITVERSION 5014,146449
