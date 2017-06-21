
lnx.c,3728
#define QLLC_MAJVERSION 381,13855
#define QLLC_MINVERSION 382,13881
#define QLLC_EDITVERSION 383,13907
void lnx_init 389,14082
static dlc_handle_t *lnx_get_dlc_handle(412,14819
void lnx_start 426,15279
lnxtype *lnx_findfree 494,16962
void lnx_freeup 512,17143
boolean lnx_x25_map_add(537,17721
static boolean snx_add_lnxtype(557,18114
static void lnx_unmap_qllc(571,18362
void lnx_x25_map_delete(589,18730
static lnxtype *lnxtype_init 610,19285
boolean lnx_busy(668,20815
static void lnx_pvc_setup 684,21075
lnx_get_startup_pak 738,22383
lnx_get_xid1_pak 751,22649
static void lnx_net_linkup 773,23111
static void lnx_dlc_linkup 856,25192
void lnx_dlc_ind_connect(972,28592
static inline boolean lnx_is_xid3(990,28856
static boolean is_xid1 1012,29262
inline void xid1_flip_role 1028,29582
static inline boolean is_xidnull(1038,29738
static inline void lnx_xid_spoof_abm(1052,30033
static inline void lnx_xid_glean_maxbtu(1074,30534
void lnx_startup_seq 1086,30908
void lnx_forward_held_xid 1340,39062
void lnx_qllc_notify(1363,39675
void lnx_contact_net(1501,43023
static inline void xid1_glean_maxbtu 1566,44656
static boolean is_xid1_primary 1575,44878
static inline void  xid1_set_null 1586,45096
static inline void  xid1_set_xid0T2 1597,45378
static void lnx_contacted(1613,45857
boolean lnx_local_ack_this(1677,47458
void lnx_net_disc_ack(1702,48028
void lnx_disconnect_net(1718,48339
void lnx_net_ind_disconnect 1758,49472
void lnx_net_ind_dm 1785,50052
void lnx_net_ind_connect 1801,50401
lnx_net_event_t lnx_map_net_event(1823,50939
void lnx_input 1896,53043
void lnx_quench(2361,66532
void lnx_initiate 2404,67777
void lnx_net_fwd_xid 2435,68669
paktype *lnx_to_llc_xid(2531,71397
void lnx_output 2599,73129
void lnx_vrforward_data 2642,74184
void lnx_cleanup_llc 2702,75756
void lnx_fix_llc 2790,78048
void lnx_fix_qllc 2837,79286
static inline void lnx_to_lak_addr(2874,79918
boolean lnx_create_lackie(2889,80364
static dlc_rtn_t lnx_lak2_open 2924,81007
static dlc_rtn_t lnx_lak2_close 2978,82418
static dlc_rtn_t lnx_lak2_busy 3001,82986
static dlc_rtn_t lnx_lak2_abort 3028,83663
void lnx_lak2_dlc_event(3059,84316
void lnx_setstate(3096,85175
static boolean virtualp_qllc 3119,85866
static boolean lnx_add_vring_interface 3141,86547
static boolean lnx_active 3184,87498
void lnx_vidb_input 3192,87595
hwidbtype *lnx_new_vidb 3289,90339
boolean rsrb_lnx_oqueue 3312,90835
static boolean lnx_remove_vring 3328,91153
lnxtype *lnx_by_macaddr 3410,93274
void lnx_enter_macaddr_hash 3441,93830
void lnx_remove_macaddr_hash 3462,94208
boolean lnx_validate 3497,94866
int lnx_hash_macaddr 3528,95357
void lnxif_command(3549,95809
static lnxtype *lnx_addr_lookup(3701,99811
static void lnx_config_srb 3745,101167
static boolean lnx_vr_other_lnx(3865,104721
static void lnx_config_partner 3897,105529
static void lnx_config_xid 3936,106456
static void lnx_config_qllclf(3970,107055
static void lnx_config_poll(4020,108040
static void lnx_config_sap 4038,108403
void lak_lnx_register 4082,109300
char *lnx_pr_event(4094,109429
char *lnx_prstate 4135,110167
static void snx_start 4161,111234
void snx_periodic 4184,111738
void snx_upstream(4207,112226
void snx_output(4287,113992
void snx_input(4337,114817
void snx_clean_sdlc(4368,115413
void snx_disconnect_sdlc(4416,116549
static void sdlc_clean_snx 4454,117347
void snx_config_pass 4500,118375
void lnx_upstream(4533,118962
void pr_xid(4572,119745
void show_lnx(4593,120283
static void show_lnxtype(4608,120546
static boolean QllcSend 4662,122059
static void QllcSendThruQ 4702,123060
forktype lnx_linkup_process 4710,123251
static process lnx_data_process 4743,124120
