
as_robbed_bit.c,2447
#define static 80,3327
#define MAX_CONTROLLER 85,3515
#define MAX_B_CHANNEL 86,3547
#define MAX_LOCK_WAITING_INDEX 87,3575
static robbed_bit_signaling_t get_rx_onhook_abcd_bits(289,18687
static inline robbed_bit_signaling_t get_tx_onhook_abcd_bits_inline(309,19123
static inline robbed_bit_signaling_t get_tx_offhook_abcd_bits_inline(314,19248
static inline robbed_bit_signaling_t get_tx_busy_abcd_bits_inline(319,19377
static void insert_free_channel 338,19853
static signal_info_t *delete_free_channel 357,20350
static void insert_state_change_list 377,20860
static signal_info_t * delete_state_change_list 396,21387
static void rbs_allocate_signal_channel 416,21900
void rbs_deallocate_signal_channel 446,22693
void rbs_enable_signal_channel 486,24150
void rbs_disable_signal_channel 499,24585
void rbs_init_signal_info 508,24860
void rbs_t1_framing_down 547,26397
void rbs_t1_framing_up 567,27013
static void rbs_t1_busyout 582,27394
static void rbs_t1_unbusyout 595,27725
static signal_info_t* recover_busyout_signal_channel 611,28266
static void insert_to_lock_waiting_list 633,28944
static inline void delete_from_lock_waiting_list 649,29403
static signal_info_t* find_signal_info_by_call_id 664,29858
static void send_reject_event_to_host 681,30299
static void send_event_to_host 707,31241
static void receive_event_from_host 734,32266
void rbs_rx_abcd_bit_signal 826,35770
void rbs_rx_ab_bit_signal 921,39360
static void tx_abcd_bit_signal 926,39535
static void postidle_handler 1017,42991
static void invalid_bit_change_handler 1059,44361
static void valid_bit_change_handler 1079,45102
static void guardtime_handler 1086,45345
static void delayed_dialout_handler 1111,46151
static const state_table_t *get_state_table 1126,46624
static boolean do_action_rx_mf 1149,47288
static boolean do_action_tx_mf 1159,47586
static boolean do_action_connect 1169,47884
static boolean do_action_idle 1182,48277
static boolean do_action_glare 1206,49378
static boolean do_action_ignore 1217,49693
static boolean do_action_fallback 1229,49993
static boolean do_action_init_chnl_lock 1254,51140
static boolean do_action_check_channel_lock 1268,51586
static void action_execution 1286,52189
static void call_idle_state 1351,54900
static void call_disconnect_state 1435,58500
static void call_signaling_state 1463,59556
static void process_signaling_state_change 1534,62390
void rbs_process 1574,63978
