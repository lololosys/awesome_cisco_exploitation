
dlsw_core.c,2316
boolean dlsw_is_init_clsi_op 473,17097
boolean dlsw_is_cstm_clsi_msg 485,17298
boolean dlsw_is_cstm_ssp_op 498,17560
char *dlsw_sspop2str 514,17891
boolean dlsw_is_ssp_ctl_mac_hdr(547,19263
static dlsw_t *remdlc2dlsw 565,19715
static dlsw_t *clsi2dlsw 577,19943
dlsw_ring_t *usapid2dgra 592,20336
dlsw_ring_t *dlsw_get_ring_frpeer(605,20637
dlsw_bgroup_t *dlsw_get_bgroup 621,20975
dlsw_ring_t *dlsw_get_ring(636,21276
dlsw_sap_t *dlsw_get_sap(669,22197
static void dlsw_enable_port 682,22519
static void dlsw_disable_port 723,24098
void dlsw_set_bgroup_params 765,25431
static void dlsw_set_srb_params 787,25961
static void dlsw_parse_if_cmds 834,27227
static void dlsw_parse_srb_if 881,28619
void dlsw_ports_disable(906,29407
static ushort dlsw_get_hash_bucket 925,29979
static dlsw_t *dlsw_find_ckt 941,30294
static void dlsw_init_fields 972,31462
static dlsw_t *dlsw_create_ckt 1042,33797
void dlsw_init 1120,36312
static ClsUSapIdT dlsw_default_get_sap 1136,36545
void dlsw_start 1155,37068
boolean dlsw_msgBLOCK 1235,39483
static inline void dlsw_process_delayed_events 1250,39748
forktype dlsw_msg_proc 1272,40289
forktype dlsw_background 1303,40920
dlsw_delayed_event 1370,42776
static void fill_ssp_hdrs(1395,43511
static paktype *dlsw_get_ssp_info_pak 1465,46015
static paktype *dlsw_get_ssp_cntl_pak 1496,46939
boolean dlsw_cantput 1524,47759
static void insert_fca 1568,49104
static void insert_fci 1586,49586
static void send_ssp_ifcack 1645,51838
static void send_ssp_ifcind 1693,53451
static void do_wan_inbound_fc 1736,54793
static void do_wan_outbound_fc 1875,60045
void dlsw_send_ssp 1905,60943
void dlsw2clsi_flow_action 2004,64308
static void dlsw_mark_peerckts 2052,65823
static csm_rc_t new_ckt_from_clsi 2095,67498
static void new_ckt_from_wan 2173,70073
csm_rc_t csm_to_core 2269,73989
void disp_to_fsm(2302,75118
static void dlsw_pre_proc_clsi 2374,77135
static void dlsw_core_proc_clsi 2442,78793
static void fsm_destroy_peer 2482,79978
static void dlsw_pre_proc_dlx 2503,80583
static void dlsw_pre_proc_ssp 2525,81150
peer2core_ret_t peer_to_core(2622,84169
static void dlsw_send_activate_ring 2657,85363
static void dlsw_init_clsi_op_proc 2830,91374
int dlsw_get_circuit_creates 2932,95135
int dlsw_get_active_circuits 2937,95205
