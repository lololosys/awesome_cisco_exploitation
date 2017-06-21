
ipc_server.c,2818
struct ipc_delayed_func_ ipc_delayed_func_164,5441
boolean ipc_register_delayed_func 179,5838
static void ipc_invoke_delayed_func 230,6832
void ipc_show_status 253,7183
static inline boolean ipc_output 315,9293
static inline ipc_message *ipc_yank_original_message(346,10113
static inline void ipc_send_ack_reply 386,11282
static void ipc_notify_sender 435,12630
static inline void ipc_process_ack_message 487,14020
static inline ipc_sequence_report ipc_check_sequence 550,16223
static inline void ipc_dispatch_inbound_message 594,17337
static void ipc_dispatch_rpc_reply(646,18877
static inline void ipc_fast_dispatch_inbound_message 678,19701
static void ipc_process_message 728,21121
void ipc_process_raw_pak 819,23673
void ipc_flush_retry_queue 865,24789
static inline void ipc_inc_output_sequence 892,25370
static inline void ipc_dec_output_sequence 920,26013
static void ipc_process_retry_queue 951,26900
static void ipc_server_periodic 1028,28959
static boolean ipc_serverBLOCK(1053,29555
static forktype ipc_server_process 1085,30236
static boolean ipc_realmBLOCK(1125,30914
static forktype ipc_realm_manager_process 1140,31195
static boolean ipc_zoneBLOCK(1166,31616
static forktype ipc_zone_manager_process 1181,31895
static boolean ipc_send_local 1224,32815
static boolean ipc_send_local 1244,33285
static boolean ipc_messageBLOCK(1265,33701
ipc_error_code ipc_send_message_noack 1272,33837
ipc_error_code ipc_send_message_async 1322,35349
ipc_error_code ipc_send_message(1398,37404
ipc_error_code ipc_send_pak_async(1427,37965
ipc_error_code ipc_send_opaque(1465,39026
ipc_error_code ipc_send_opaque_async(1493,39840
ipc_message *ipc_send_rpc(1517,40513
ipc_error_code ipc_send_rpc_reply_async(1554,41571
ipc_error_code ipc_send_rpc_reply(1576,42421
static void ipc_process_echo_message_callback 1590,42868
static void ipc_process_echo_message 1601,43184
static void ipc_process_register_name_request 1644,44341
static void ipc_process_lookup_name_request 1688,45425
static void ipc_process_zone_man_message 1722,46225
static void ipc_process_server_message 1729,46395
static boolean ipc_initialize_server_ports 1746,46780
static void get_seat_id(1768,47347
static void get_zone_id(1777,47562
static void register_seat_with_peers(1780,47596
static boolean connect_to_zone(1788,47751
static void ipc_init_global_entities(1793,47813
static boolean ipc_create_local_seat(1853,49348
static boolean ipc_initialize_globals 1876,49919
static void ipc_send_eoir_notice_callback 1911,50700
static void ipc_send_eoir_notice 1924,51042
void ipc_reset_seq_nums 1940,51525
void ipc_change_seat_info 1948,51745
static void ipc_subsys_init 1991,53063
#define IPC_MAJOR_VERSION 2051,55200
#define IPC_MINOR_VERSION 2052,55228
#define IPC_EDIT_VERSION 2053,55256
