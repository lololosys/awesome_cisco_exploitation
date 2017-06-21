
c4000_bsi.c,3369
#define BSI_DEBUG(BSI_DEBUG262,10317
#define DEBUG_COUNTERS(DEBUG_COUNTERS263,10338
#define DEBUG_PKT 265,10365
#define DEBUG_PKT_FAST 266,10392
#define fddibuginf 342,12514
#define bprintf 343,12578
void cam_interrupt 366,13493
static void les_fddi_cam_depopulate 454,15862
static void empty_function 469,16185
static int hammer 479,16347
static int regtestplayer 502,16828
static int regtestbmac 524,17539
static int regtestbsi 541,17978
static char *bsi_typestring 575,18751
static void bsi_dump_packet 592,19088
static boolean bsi_linestate 613,19566
static void bsi_stop 626,19784
static void bsi_restart 673,20910
static void bsi_write_pram 711,21739
static inline void bsi_write_lram_inline 767,23120
static void bsi_write_lram 824,24467
static inline void bsi_return_page_inline 849,25374
static inline paktype *bsi_get_page_inline 877,26376
static paktype *bsi_get_page 893,26710
static void bsi_disable 905,27016
static void bsi_enable 937,27699
static void bsi_queue_for_process 980,29089
static void bsi_proc_idud_errors 1108,32648
boolean bsi_proc_psp_error 1225,35752
static void bsi_proc_idud 1299,37878
static void bsi_disable_psp_int 1908,53218
static void bsi_enable_psp_int 1931,53706
static inline void bsi_fill_psp_inline 1959,54314
static void bsi_fill_psp 2048,56716
void bsi_interrupt 2058,57031
static void *bsi_buffer_create 2310,63834
static void bsi_buffer_destroy 2360,64830
static void bsi_check_disabled_psp 2372,65162
static void bsi_cache_threshold 2398,65878
static void bsi_build_ring 2420,66294
static void bsi_init_req_ring 2473,67633
void bsi_init_req_channel 2545,69179
static void bsi_init_ind_ring 2615,70944
static void bsi_init_ind_channel 2661,71954
static void bsi_proc_cnf_errors 2754,74588
static void bsi_proc_cnf 2860,77214
static int bsi_txqlength 2965,79732
static void bsi_debug_dump_packet 2982,80017
static void bsi_get_default_hw_address 3010,80527
static void bsi_reset_multicast_addresses 3022,80829
static void bsi_set_cam_mode 3039,81161
static void bsi_add_multicast_address 3108,83108
static void bsi_add_multicast_range 3162,84641
static void bmac_set_gla 3181,85070
static void bsi_bmac_initialize 3210,85724
static void bsi_setup_mac 3368,90498
boolean bsi_initialize 3388,90998
static void bsi_shutdown 3821,102610
static void bsi_output 3850,103311
static void bsi_fastsend 4148,111567
static void bsi_setup_bridging 4422,118735
static void bsi_periodic 4436,119018
static void bsi_show_Req_chan 4459,119505
static void bsi_show_Ind_chan 4477,120325
static void bsi_show_cam 4495,121195
static void bsi_show_bsi 4530,121913
static void bsi_show_bmac 4586,123524
static void bsi_show_player 4660,126710
static void bsi_show_players 4682,127674
static void bsi_show_counters 4699,127998
static char *modestring 4756,130724
static boolean bsi_show_controller 4768,130926
void bsi_repeat 4822,132809
static hwidbtype *init_bsi_idb 4988,138032
static void bsi_memory_notify 5452,152135
static void bsi_analyze_interface 5471,152536
static void bsi_if_final_init 5567,155637
static void bsi_bad_nvram_if 5586,156054
static void bsi_subsys_init 5602,156305
#define BSI_MAJVERSION 5661,157762
#define BSI_MINVERSION 5662,157789
#define BSI_EDITVERSION 5663,157816
void cmt_connect 5674,158032
void cmt_disconnect 5698,158663
