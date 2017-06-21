
if_les_sync_cd2430.c,3201
#define IER_SYNC_BASIC 156,5718
#define MAX_LOOP_CNT 162,5884
#define SMALLEST_FRAME_SIZE 163,5910
cd2430_process_hi_priority_pak 237,8777
cd2430_extract_tx_pak 268,9853
inline static void cd2430_tx_pak_accounting 319,11478
cd2430_release_tx_pak 345,12268
alloc_rx_buffers 388,13391
arm_tx_dma 443,14930
cd2430_receive_packet 467,15726
cd2430_transmit_packet 541,18025
cd2430_flush_restart_receive 585,19255
cd2430_disable_receiver 603,19617
cd2430_get_sysbuffer 619,19934
int cd2430_syn_detected 629,20116
enable_modem_interrupts 652,20639
disable_modem_interrupts 661,20795
monitor_interface_flap 670,20952
start_timer1 691,21424
clear_timer1 702,21692
start_timer2 711,21858
clear_timer2 722,22127
txqueue_nonempty 733,22306
deassert_modem_signals 750,22599
static void reset_interface(769,22985
cd2430_clear_nested_txint 790,23731
cd2430_clear_nested_rxint 825,24831
cd2430_clear_nested_modint 860,25928
static void log_half_duplex_data 892,26849
static void log_half_duplex_figure 905,27408
inline static boolean valid_dte_tx_fsm_state_event 939,28846
static void cd2430_sync_hdx_dte_tx_fsm 969,30195
inline static boolean valid_dte_rx_fsm_state_event 1328,44136
static void cd2430_sync_hdx_dte_rx_fsm 1348,44892
inline static boolean valid_dce_tx_fsm_state_event 1477,49435
static void cd2430_sync_hdx_dce_tx_fsm 1499,50301
inline static boolean valid_dce_rx_fsm_state_event 1780,61275
static void cd2430_sync_hdx_dce_rx_fsm 1804,62232
static void cd2430_sync_line_state 2060,71737
static void cd2430_sync_rxint 2183,76343
static void cd2430_sync_hdx_rxint 2465,87005
static void cd2430_sync_txint 2577,90647
static void cd2430_sync_hdx_txint 2671,93132
static void cd2430_sync_modint 2765,95606
static void cd2430_sync_hdx_modint 2837,97620
static void cd2430_sync_rxgoodint 2923,100268
void cd2430_sync_txstart 3013,103224
void cd2430_set_transmit_delay 3055,104419
static const cd2430_clocktype_t *get_clock 3073,104831
void cd2430_clockrate_command 3089,105159
static boolean cd2430_set_maxdgram 3151,106582
void cd2430_set_rxoffset 3168,107028
static void cd2430_sync_shutdown 3176,107162
static void cd2430_periodic 3190,107627
static void cd2430_show_serial_state 3312,111650
boolean cd2430_sync_show_controller 3356,112805
static void cd2430_set_channel_option_regs 3442,115014
void cd2430_set_clock_option_regs 3564,119476
static void cd2430_set_timer_params 3683,124865
static void cd2430_buffer_pools_init 3717,125765
static void cd2430_sync_setup_isr_vectors 3787,127836
static void cd2430_reset_rx_buffers 3840,129556
static void cd2430_reset_tx_buffers 3859,129918
static void cd2430_sync_show_hdx_log 3883,130429
static void cd2430_sync_show_errors 3942,132226
static void cd2430_enter_transparent_mode 4000,133883
static void cd2430_reset_channel 4118,137575
cd2430_clear_receiver 4152,138472
static void cd2430_driver_context_init 4176,139057
void cd2430_sync_mode_init 4341,144237
static void cd2430_sync_reset 4462,147104
static void cd2430_fastsend 4501,148446
static boolean cd2430_hw_set_config 4659,153713
static boolean cd2430_hwidb_state_config 4883,161764
void cd2430_init_sync_idb 4918,162567
