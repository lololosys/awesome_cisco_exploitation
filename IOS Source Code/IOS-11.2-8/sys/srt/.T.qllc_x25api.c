
qllc_x25api.c,2604
#define QLLC_VC_EVENT_BUF_SIZE 80,2422
#define QLLC_PID_LEN 81,2457
struct qllcx25_cfg_and_address_ qllcx25_cfg_and_address_91,2743
static void qllcx25_data_rx 175,5238
boolean qllcx25_svc_announce 231,6536
static void qllcx25_enter_d1 263,7374
static void qllcx25_rr_rx 277,7735
static void qllcx25_rnr_rx 289,7985
static void qllcx25_reset_rx 302,8265
static void qllcx25_clear_rx 324,8804
static void qllcx25_deleted 339,9177
static void qllcx25_invalid 355,9544
static void qllcx25_call_coll_rx 372,9998
static void qllcx25_pvc_inactive 388,10412
static void qllcx25_interrupt_rx 404,10896
static void qllcx25_enter_p4 441,11859
static void qllcx25_clear_tx 459,12294
static void qllcx25_reset_tx 477,12735
static void qllcx25_idle 492,13152
static void qllcx25_interrupt_conf_rx 506,13444
static enum TX_DQ_RESULT qllcx25_xmitter 524,13977
boolean qllcx25_send_data 629,17858
void qllcx25_setup_client_ctxt(721,20584
x25_vc * qllcx25_place_call 735,20872
boolean qllcx25_send_call_confirm 835,23686
boolean qllcx25_send_rr 896,25142
boolean qllcx25_send_rnr 927,25992
boolean qllcx25_send_clear 958,26880
boolean qllcx25_send_clear_confirm 1009,28038
boolean qllcx25_send_reset 1046,28826
boolean qllcx25_send_reset_confirm 1095,29958
void qllcx25_quench 1131,30736
void qllcx25_config_created 1194,32297
void qllcx25_config_deleted 1220,32828
void qllcx25_client_config_map 1243,33417
void qllcx25_client_config_map_nvgen 1287,34609
static inline int qllcx25_hash 1331,35932
static void qllcx25_map_add 1340,36207
qllcx25_map * qllcx25_map_lookup 1429,38668
static qllcx25_map_addr * qllcx25_remove_hash 1461,39487
static void qllcx25_map_delete 1490,40332
void qllcx25_lock_vc 1553,41826
void qllcx25_unlock_vc 1568,42087
void qllcx25_bind_vc 1579,42260
void qllcx25_unbind_vc 1611,42955
void qllcx25_get_dst_address 1637,43388
void qllcx25_get_src_address 1651,43749
void qllcx25_get_if_x25_address 1665,44129
ushort qllcx25_get_maxdataout 1681,44532
ushort qllcx25_get_lcn 1692,44701
idbtype *qllcx25_get_swidb_from_config(1702,44856
idbtype * qllcx25_get_swidb_from_vc(1715,45084
x25_config * qllcx25_get_config_from_vc 1726,45287
x25_config * qllcx25_get_config_from_hwidb 1737,45469
boolean qllcx25_search_config_maps 1755,45867
x25_config * qllcx25_get_config_from_addr 1775,46354
qllctype * qllcx25_get_qllc_from_vc 1786,46670
boolean qllcx25_vc_is_pvc(1794,46804
boolean qllcx25_x25service_up 1801,46890
static void qllcx25_pvc_setup 1816,47228
x25_vc * qllcx25_vc_lookup_next(1826,47499
x25_vc * qllcx25_vc_lookup(1831,47593
