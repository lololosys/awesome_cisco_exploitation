
if_as5200_csm.c,4980
#define CSM_SET_STATUS(CSM_SET_STATUS132,4860
#define CSM_CLEAR_STATUS(CSM_CLEAR_STATUS133,4940
#define MICROCOM_SLOWRING_FIRMWARE_VERSION 139,5164
#define CSM_EVENT_QUEUE_ENABLE 146,5371
#define CSM_USE_AT_COMMAND_CTRL 147,5407
#define TDM_WORKAROUND 148,5451
static inline csm_event_queue_t* write_next_csm_event_inline 173,6192
static inline csm_event_queue_t* read_next_csm_event_inline 199,6889
uint csm_get_modem_firmware_version 320,11950
static boolean csm_modem_is_ready_to_take_ic 346,12709
static boolean csm_is_newer_modem_firmware 375,13642
void csm_setup_capability_flags 405,14550
static const uchar* csm_get_csm_state 455,15845
static void csm_init_modem_pool 483,16535
static boolean csm_exists 566,19600
csm_modem_get_calling_number 598,20640
csm_modem_get_called_number 621,21209
static void csm_init 657,22070
void csm_clear_modem_info_counter 726,24387
static void csm_init_modem_info 764,25772
static void csm_add_digit_collector 845,28734
static void csm_init_interface 914,31176
static void csm_activate_periodical_background_check 964,32722
static void csm_ct1_rbs_adjust_idle_dsx0 986,33428
static process csm_periodical_background_check_process 1046,35742
static process csm_event_handler_process 1172,41105
boolean async_tty_transmit_hold(1203,41935
static void csm_ring_indication_proc 1229,42682
static void csm_print_modem_list 1358,47984
void csm_show_modem_info 1387,48788
static void csm_lock_modem_port 1556,55777
static void csm_unlock_modem_port 1583,56688
static boolean csm_set_csm_status 1617,58031
static boolean csm_clear_csm_status 1672,59830
boolean csm_set_busy_out_status 1725,61635
boolean csm_clear_busy_out_status 1791,63759
boolean csm_set_modem_back2back_test 1842,65320
boolean csm_clear_modem_back2back_test 1895,67035
boolean csm_set_download_file_status 1956,69115
boolean csm_clear_download_file_status 1977,69766
boolean csm_set_download_fail_status 1999,70480
boolean csm_clear_download_fail_status 2020,71128
boolean csm_set_bad_modem_status 2042,71865
boolean csm_clear_bad_modem_status 2063,72496
boolean csm_set_busy_status 2086,73254
boolean csm_clear_busy_status 2107,73845
boolean csm_set_modem_reset_status 2131,74650
boolean csm_clear_modem_reset_status 2175,76353
boolean csm_set_shutdown_status 2225,78060
boolean csm_clear_shutdown_status 2276,79509
static modem_info_t* csm_modem_allocate 2298,80226
boolean csm_do_pending_request 2458,87060
static void csm_modem_deallocate 2514,89142
static void csm_add_pri_signaling_interface 2573,91148
static void csm_add_ct1_signaling_interface 2633,92903
static void csm_delete_pri_signaling_interface 2716,95681
static void csm_delete_ct1_signaling_interface 2813,99233
static modem_info_t* find_modem_info_by_call_id 2895,102261
static void  csm_tty_platform_dependent_info_report 2929,103253
static void  csm_idb_platform_dependent_info_report 2979,104822
static ushort modem_report 3012,105858
void csm_modem_interrupt 3376,121113
void dsx1_mail_from_neat 3466,124147
static void csm_rx_ct1_event_from_neat 3562,127449
static void csm_disable_oh_interrupt 3823,137694
static void csm_enable_oh_interrupt 3848,138433
static void csm_stop_modem_reset 3875,139279
static void csm_start_modem_reset 3900,139988
static void csm_turn_ring_off 3929,140908
static void csm_turn_ring_on 3980,142811
static void csm_process_next_digit_collect 4029,144642
static void csm_process_next_pending_digit_collect 4086,146846
static dchan_info_t* csm_get_signaling_channel 4135,148480
static dchan_info_t* csm_get_1st_signaling_channel 4176,150071
static dchan_info_t* csm_get_next_signaling_channel 4215,151468
static void csm_start_digit_collection 4252,152759
static int csm_connect_pri_modem 4306,154754
static int csm_disconnect_pri_modem 4380,157159
static void csm_enter_idle_state 4436,158947
static void csm_enter_disconnecting_state 4479,160463
static void csm_enter_busy_tone_state 4511,161526
static void csm_send_neat_ct1_event 4546,162738
static ushort csm_proc_idle 4591,164476
static ushort csm_proc_ic4_oc8_disconnecting 4711,168979
static ushort csm_proc_ic1_ring 4774,170916
static ushort csm_proc_ic2_wait_for_carrier 4897,175584
static ushort csm_proc_ic3_oc6_connected 5009,179846
static ushort csm_proc_oc1_request_digit 5096,183181
static ushort csm_proc_oc2_collect_1st_digit 5191,186682
static ushort csm_proc_oc3_collect_all_digit 5296,190461
static ushort csm_proc_oc4_dialing 5420,195139
static ushort csm_proc_oc5_wait_for_carrier 5589,201936
static ushort csm_proc_oc7_busy_error 5735,207683
int csm_make_bit_field 5788,209348
static void csm_ct1_rbs_debug 5797,209511
void modem_mgmt_csm_exec_command 5824,210593
void modem_mgmt_exec_command 5923,213533
void modem_mgmt_csm_show_command 5935,213726
void modem_mgmt_show_command 5963,214527
#define CSM_MAJVERSION 5984,215056
#define CSM_MINVERSION 5985,215082
#define CSM_EDITVERSION 5986,215108
