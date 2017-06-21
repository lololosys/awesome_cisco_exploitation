
v120.c,2126
#define ALTERNATE	143,4813
#define ALTERNATE	148,4927
static void v120_iqueue 171,5676
static iqueue_t v120_parse_packet 181,5889
static boolean v120_vencapsulate 201,6393
static boolean v120_line_state 216,6752
void v120_command 234,7178
static enum v120_packet_type v120_classify_packet 261,8067
static void v120_dump_bytes 341,10152
static void v120_display_packet 367,10633
static void v120_display_input_packet 416,12026
static void v120_display_output_packet 423,12210
static void v120_start_idle_timer 437,12540
static void v120_create_unnumbered_packet 450,12949
static void v120_create_frmr_packet 483,13707
static void v120_create_rr 521,14799
static void v120_create_rej 554,15494
static void v120_dump_buffers 584,16267
static void v120_change_to_tei_assigned 593,16420
static void v120_change_to_await_establish 602,16643
static void v120_change_to_establish 614,16991
static void v120_change_to_timer_recovery 629,17362
void v120_change_to_await_release 644,17853
boolean v120_send_data 671,18889
static void v120_retransmit_adjust 766,21489
static void v120_retransmit 776,21744
static void v120_tei_assigned_state 830,23230
static void v120_awaiting_establishment_state 871,24279
static boolean v120_check_nr 926,25784
static void v120_established_state 974,26953
static void v120_timer_recovery_state 1156,31785
static void v120_awaiting_release_state 1267,34439
static void v120_input 1314,35590
static inline ulong v120_tei_assigned_timer 1413,38352
static ulong v120_awaiting_establishment_timer 1419,38474
static ulong v120_established_timer 1439,38944
static ulong v120_timer_recovery_timer 1470,39798
static ulong v120_awaiting_release_timer 1490,40307
static void v120_timers 1509,40775
static boolean v120_start_interface 1560,42136
static void v120_show 1702,46171
static void v120_stop_interface 1717,46522
static boolean v120_autodetect_test 1787,48128
static boolean v120_autodetect_start_interface 1832,49202
static void v120_init 1853,49599
#define V120_MAJVERSION 1870,50186
#define V120_MINVERSION 1871,50212
#define V120_EDITVERSION 1872,50238
