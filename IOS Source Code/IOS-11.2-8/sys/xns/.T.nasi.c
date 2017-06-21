
nasi.c,3411
#define NASI_PORT_IN_USE(NASI_PORT_IN_USE130,4377
static void nasi_server_init 255,9412
void nasi_server_enable 273,9880
void nasi_server_disable 297,10260
static void nasi_start 312,10524
static void nasi_ipx_internal_name_changed 335,11040
static void nasi_ipx_internal_net_changed 371,11892
static void nasi_setup_ncs_service 401,12545
static boolean nasi_setup_spx_socket 429,13123
static void nasi_reset_stats 450,13607
void nasi_vector_init 459,13708
static boolean start_nasivty 497,15128
static spxcontype *tty2spxcon 561,16914
static boolean nasi_outputblock 591,17521
static boolean nasi_startoutput 610,17843
static boolean nasi_set_new_tty 637,18295
static boolean nasi_alive 652,18559
static void nasi_close_connection 666,18820
static void nasicon_close 707,19743
static boolean nasi_newincoming_con 731,20362
static boolean nasi_close_con_allowed 755,20923
static boolean nasi_putblock 775,21298
static int nasi_unread 785,21487
static boolean nasiBLOCK 793,21586
static forktype nasi_main 825,22363
static boolean nasi_sendbyte 883,23884
static void nasi_send_server_ack 913,24776
static void nasi_rcv_client_data(946,25866
static void nasi_process_input 974,26591
static void nasi_rcvd_srvinfo_req 1204,32187
static void nasi_send_server_preq_response 1269,34190
static void nasi_rcvd_qname_ffirst 1309,35529
static void nasi_rcvd_qname_fnext 1400,38538
static nasi_port *nasi_get_port_info 1496,41847
static int nasi_next_port 1560,43243
static int nasi_read_portnum 1605,44083
static int nasi_port_name_to_num 1635,44676
static void nasicon_setbuffers 1677,45710
static boolean nasicon_puts 1714,46638
static void nasi_get_port_params 1749,47489
static inline uchar nasi_table_search 1771,48380
static inline uchar tty2nasi_speed 1790,48812
static inline uchar tty2nasi_databits 1798,49002
static inline uchar tty2nasi_stopbits 1806,49209
static inline uchar tty2nasi_parity 1814,49420
static inline uchar tty2nasi_flow 1822,49617
static int nasi_getc 1839,49946
static int nasi_getbyte(1903,51437
static boolean nasiputBLOCK 1927,51915
static void nasi_putc 1946,52372
static void nasi_putbyte 1973,53022
static int nasi_inputpending 2019,54132
static void nasi_clearinput 2039,54417
static int nasi_outputpending 2055,54687
static void nasi_clearoutput 2071,54974
static void nasi_setsflow 2088,55276
static void nasicon_output 2098,55428
static void nasicon_clearinput 2132,56442
static void nasicon_clearoutput 2153,56976
static void nasi_initport_rcvd 2175,57591
static boolean nasi_send_data_pkt 2290,60697
static void nasi_send_close_pkt 2325,61765
static void nasi_rcvd_close 2358,62647
static void nasi_rcvd_flush_rxbuf 2368,62832
static void nasi_rcvd_flush_txbuf 2388,63310
static void nasi_rcvd_set_service_name 2410,63928
static void nasi_rcvd_get_service_name 2429,64364
static void nasi_rcvd_get_general_name 2449,64814
static void nasi_rcvd_get_specific_name 2469,65266
static void nasi_send_setget_service_response 2488,65703
static void nasi_break_rcvd 2522,66784
static void nasi_sflow_rcvd 2548,67383
static void nasi_def_sflow_rcvd 2573,67997
static void nasi_modify_port 2593,68414
static boolean nasi_check_auth 2602,68576
static void nasi_decrypt_pw 2641,69550
#define NASI_SERVER_MAJVERSION 2667,70059
#define NASI_SERVER_MINVERSION 2668,70092
#define NASI_SERVER_EDITVERSION 2669,70125
