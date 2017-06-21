
mlpvt_mq_protocol.c,3835
#define ADD_TO_MQ_MSG(ADD_TO_MQ_MSG504,19534
#define MLPVT_PEER_ADDR(MLPVT_PEER_ADDR508,19690
#define MLPVT_OUR_ADDR509,19756
#define MLPVT_PEER_ID(MLPVT_PEER_ID510,19779
#define MLPVT_OUR_ID 511,19847
#define MLPVT_FIRST_ACTIVE_PEERID 512,19904
#define MLPVT_NEXT_ACTIVE_PEERID(MLPVT_NEXT_ACTIVE_PEERID513,19974
#define MLPVT_PEER_COUNT 515,20090
#define MLPVT_SEED_BID	516,20160
#define MLPVT_BIDTYPE 517,20211
#define VPNTYPE	521,20285
#define MLP_SEARCH_BUNDLE(MLP_SEARCH_BUNDLE522,20316
#define VPN_SET_HANDLE(VPN_SET_HANDLE523,20395
#define VPNTYPE	533,20640
#define MLP_SEARCH_BUNDLE(MLP_SEARCH_BUNDLE534,20666
#define VPN_SET_HANDLE(VPN_SET_HANDLE535,20751
static inline void mlpvt_mq_process_wakeup 542,20904
static ushort mlpvt_query_next_ref 550,21078
static void mlpvt_master_print 555,21170
static char *mlpvt_msgtype_print 565,21415
static char *mlpvt_mqstate_print 592,21794
static char *mlpvt_pbstate_print 616,22193
static char *mlpvt_pbsubstate_print 652,22814
static void mlpvt_show_query_MQ_state 673,23153
static void mlpvt_show_query_PB_state 685,23529
void mlpvt_show_query_states 693,23792
static void mlpvt_show_debug_query_states(723,24316
static int mlpvt_mq_handle_find 751,24907
static boolean mlpvt_mq_handle_add 768,25277
static boolean mlpvt_mq_handle_delete 789,25784
static void mlpvt_mq_all_handles_dispose 817,26435
static inline boolean mlpvt_pb_states_none 848,27213
static PB_state_t *mlpvt_pb_state_find 856,27385
static boolean mlpvt_pb_state_delete 877,27737
static boolean mlpvt_pb_state_add 905,28233
static void mlpvt_pb_state_reactivate 926,28612
static PB_state_t *mlpvt_pb_state_create 941,28893
static MQ_state_t *mlpvt_query_state_find 976,29722
static MQ_state_t *mlpvt_query_state_create 995,30078
static void mlpvt_query_state_delete 1041,31155
static boolean mlpvt_query_validate 1064,31655
static boolean mlpvt_query_lookup_duplicate 1112,32892
static boolean mlpvt_query_lookup_new 1171,34202
static boolean mlpvt_query_lookup 1245,36224
static void mlpvt_query_cancel 1271,36855
static void mlpvt_mq_make_us_master 1332,38308
static void mlpvt_mq_make_peer_master 1363,39246
static void mlpvt_mq_peer_vanish 1379,39636
static void mlpvt_mq_peer_connect 1440,41028
static void mlpvt_mq_peer_event_listener 1488,42119
static char *mlpvt_mq_msg_field_find 1529,43115
static char *mlpvt_mq_msg_field_get 1555,43570
static boolean mlpvt_mq_peer_bid_process 1603,44680
static void mlpvt_mq_p2p_msg_MQO_process 1642,45385
static void mlpvt_mq_p2p_msg_MB_process 1705,46947
static void mlpvt_mq_p2p_msg_BA_process 1736,47770
static void mlpvt_mq_p2p_msg_MQC_process 1773,48704
static void mlpvt_mq_p2p_msg_MQA_process 1849,50325
void mlpvt_mq_peer_msg_listener 1907,51737
static void mlpvt_mq_p2p_msg_send 2113,57081
static void mlpvt_mq_p2p_msg_MQO_send 2232,59987
static void mlpvt_mq_p2p_msg_MQO_send_resend 2239,60161
static void mlpvt_mq_p2p_msg_MB_send 2255,60560
static void mlpvt_mq_p2p_msg_MB_send_resend 2263,60766
static void mlpvt_mq_peer_bid_accept 2276,61108
static void mlpvt_mq_process_query_close 2299,61779
static void mlpvt_mq_select_master 2411,64742
static void mlpvt_mq_process_query_to_peers 2540,67649
static void mlpvt_mq_process_query_from_peers 2644,70276
static void mlpvt_MQ_process_work 2686,71076
static process mlpvt_MQ_process 2720,71901
void mlpvt_mq_shutdown 2762,73015
#define MAX_BUNDLES	2837,74671
void vpn_put_ipaddress_test 2854,74909
int mlp_search_bundle_test 2860,75045
void mlp_appoint_bundle_test 2883,75606
static process mlpvt_MQ_sim_process 2887,75655
static BIDTYPE mlpvt_calibrate_seed_bid(2966,78019
#define MLPVT_SAMPLE_ITERATION 3001,79059
#define EMPTY 3002,79094
static BIDTYPE mlpvt_sample_seed_bid(3004,79112
void mlpvt_mq_init 3109,81523
