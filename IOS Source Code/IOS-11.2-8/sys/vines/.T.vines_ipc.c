
vines_ipc.c,2777
void vines_init_ipc 169,5445
void vipc_traffic_init 187,5977
void vipc_traffic_checkpoint 206,6349
static inline sys_timestamp vipc_bump_retrans_timer 224,6825
static inline sys_timestamp vipc_set_retrans_timer 236,7197
static inline void vipc_clear_retrans_timer 247,7492
static inline void vipc_set_idle_timer 260,7882
static inline void vipc_clear_idle_timer 270,8090
static inline void vipc_set_ack_timer 283,8502
static inline void vipc_clear_ack_timer 293,8707
static ushort vipc_allocate_port_number 314,9347
vipc_port_info *vipc_find_port 344,9995
vipc_port_info *vipc_create_port 362,10443
void vipc_destroy_port 410,11548
void vipc_destroy_port_number 428,11928
static boolean vipc_connection_in_use 442,12236
static ushort vipc_allocate_connection 458,12531
vinesipcsocket *vipc_create_conn 484,13343
vinesipcsocket *vipc_get_conn 585,16201
vinesipcsocket *vipc_get_conn_by_address 619,16992
void vipc_kill_conn 640,17424
void vipc_user_clear_conn 667,18085
void vipc_network_error 687,18508
static vinesidbtype *vipc_send_socket 716,19264
static inline vinesidbtype *vipc_send_nosocket_common 763,20562
static vinesidbtype *vipc_send_nosocket 794,21331
static vinesidbtype *vipc_send_nosocket_alt_src 805,21564
static void vipc_send_broadcast 826,22096
static void vipc_purge_acked 871,23363
static vinesidbtype *vipc_send_ack 905,24279
static void vipc_rcv_ack 936,24996
static vinesidbtype *vipc_send_error 980,26185
static void vipc_rcv_error 1022,27642
vinesidbtype *vipc_send_probe 1073,29219
static void vipc_rcv_probe 1109,30284
static vinesidbtype *vipc_send_disc_nosocket 1148,31356
static vinesidbtype *vipc_send_disc 1179,32113
static void vipc_rcv_disc 1209,32834
vinesidbtype *vipc_send_data 1235,33591
static void vipc_process_data 1328,36207
static void vipc_rcv_data 1390,37971
vinesidbtype *vipc_send_datagram_common 1514,41222
void vipc_send_broadcast_datagram 1554,42220
static boolean vipc_rcv_datagram 1589,43071
static boolean vipc_demux_message 1689,46206
void vines_ipc_receive 1794,49132
void vipc_set_metric 1879,51991
static void vipc_resend_packets 1920,53221
static void vipc_check_metric 1977,54472
void vipc_ack_timer_expired 2000,55164
void vipc_idle_timer_expired 2016,55528
void vipc_retrans_timer_expired 2039,56137
void vines_process_connections 2055,56516
char *vipc_portname 2070,56850
void vipc_display 2089,57299
void vipc_display_timer 2152,59511
void vipc_display_traffic 2165,59785
uchar *display_vinespak_ipc 2205,60930
uchar *display_vinespak_spp 2238,61799
boolean vines_forward_nearest 2268,62660
vinesproxytype *vines_create_proxy 2345,64836
void vines_flush_proxy_queue 2365,65376
boolean vipc_proxy_packet 2388,66088
void vines_proxy_reply 2468,68285
