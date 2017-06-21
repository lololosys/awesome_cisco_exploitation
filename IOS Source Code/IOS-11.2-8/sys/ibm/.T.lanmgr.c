
lanmgr.c,4576
#define NLNM_BAD_CLSI_OP 468,15711
#define NLNM_DATAREQ_CLSI_OP 469,15779
#define NLNM_CONTEXTREQ_CLSI_OP 470,15856
#define NLNM_INIT_CLSI_OP 471,15936
#define NLNM_CS_CLSI_OP 472,16006
#define NLNM_CS2LNM_CLSI_OP 473,16072
static inline void start_rem_timer 481,16334
static inline void bump_rem_timer 487,16460
static inline void stop_rem_timer 492,16554
static inline subvector_t *assign_mac_ring 504,16832
static inline subvector_t *assign_mac_location 514,17089
static inline subvector_t *assign_mac_softerr_timer 531,17485
static inline subvector_t *assign_mac_correlator 541,17736
static inline subvector_t *assign_mac_short 551,17975
static inline subvector_t *assign_mac_long 562,18210
static inline subvector_t *assign_mac_bytes 573,18440
static inline subvector_t *assign_mac_address 585,18729
static void finish_mvid 593,18911
hwidbtype *ring_to_idb 618,19387
ring_info *idb_to_ring 629,19551
hwidbtype *address_to_idb 634,19637
ring_info *address_to_ring 646,19847
ring_info *ringno_to_ring 658,20075
void lanmgr_enqueue 672,20280
station_info *find_station 684,20428
station_info *find_or_build_station 703,20866
station_info *newrold_station 742,21773
static inline boolean valid_mvid 819,25716
static inline boolean valid_svid 825,25807
char *mvid_to_str 831,25898
static inline char *svid_to_string 836,26002
static inline char *sv_beacon_to_str 842,26128
static inline char *sv_errcode_to_str 848,26262
boolean parse_mac_subvectors 857,26392
static paktype *setup_macpacket2 1142,35588
paktype *setup_macpacket 1170,36287
paktype *setup_macreply 1175,36428
static void rs_init 1229,38317
void watch_ring_poll 1250,39102
void lanmgr_ringstat 1273,39664
void ringstation_input 1370,42087
static void vring_mac_adapter 1466,44665
void free_rps_pak 1662,50606
static boolean send_rps_pak 1672,50828
void resend_rps_pak 1711,51863
void rps_input 1755,53149
void rps_startup 1871,56541
void rps_init 1948,58834
void free_crs_pak 1971,59778
static boolean send_crs_pak 1981,60000
void resend_crs_pak 2016,60865
boolean send_crs_general 2068,62298
boolean send_crs_remove 2099,63066
boolean send_crs_set_sta 2120,63532
void crs_input 2157,64683
void crs_init 2283,68199
static boolean rem_report_test 2298,68675
static void rem_process_softerr 2318,69459
void rem_purge_softerr 2469,74546
void rem_input 2510,75390
void rem_init 2569,76915
boolean rem_background 2588,77469
boolean lanmgr_BLOCK 2713,81166
boolean lanmgr_background 2735,81652
void lanmgr_input 2829,83713
forktype lanmgr_process 2919,85857
void lanmgr_destroy_ring 3001,87761
ring_info *lanmgr_create_ring 3027,88176
void lanmgr_init 3068,89014
static void lnm_enqueue 3119,90400
void lanmgr_start 3252,94510
void lnm_set_srb_params(3270,94967
void lnm_enable_port(3284,95269
void lnm_disable_port(3342,97143
lnm_clsi_t *lnm_get_clsi(3388,98659
lnm_clsi_t *lnm_get_clsi_swidb(3403,99086
lnm_clsi_t *lnm_get_clsi_vdlc(3415,99370
void lnm_clsi_cleanup(3427,99681
lnm_t *lnm_pre_proc_clsi(3476,101060
lnm_t *lnm_proc_clsi(3491,101435
static lnm_t *id2lnm(3719,108564
lnm_t *ucepid2lnm(3745,109022
lnm_clsi_t *lnm_usapid2dgra(3755,109209
void lnm_get_rif_from_clsi 3772,109623
void lnm_proc_disc(3890,113375
void lnm_proc_openreq_cnf(3902,113631
static uint calc_lnm_hash 3917,113901
lnm_t *lnm_add_entry 3935,114343
void lnm_init_fields(3987,116006
void lnm_delete_entry 4003,116499
char *lnm_id 4038,117187
boolean lnm_is_bad_clsi(4060,117677
boolean lnm_is_context_valid(4128,119347
void lnm_send_info_to_disp(4162,120153
void lnm_send_udata_to_disp(4173,120481
void lnm_send_req_open_stn 4199,121418
void lnm_send_close_stn 4218,122147
void lnm_send_connect_req 4233,122501
void lnm_send_signalstn_req 4243,122734
boolean	lnm_is_init_clsi_op(4253,122929
void lnm_init_clsi_op_proc(4264,123121
static void lnm_send_activate_sap(4352,125721
void ldisp_to_lfsm(4429,128354
void ldisp_to_init(4484,129781
void lnm_proc_info(4489,129831
void lanmgr_command 4495,129905
void lanmgrif_command 4573,131792
static int display_one_station 4861,139426
static void display_station_by_idb 4921,141648
static void display_station_by_address 4964,142780
static void display_all_stations 4982,143225
void display_ring_info 5029,144479
void show_lanmgr 5113,146984
void lnm_get_key 5220,149375
void lnm_fill_key 5233,149568
void lanmgr_terminate 5248,149759
void lanmgr_restart 5318,151368
#define LNM_MAJVERSION 5350,151991
#define LNM_MINVERSION 5351,152016
#define LNM_EDITVERSION 5352,152041
