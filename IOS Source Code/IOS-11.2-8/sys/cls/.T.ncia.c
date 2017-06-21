
ncia.c,1687
#define NCIA_MAJVERSION 210,7296
#define NCIA_MINVERSION 211,7322
#define NCIA_EDITVERSION 212,7348
void ncia_init 219,7530
void ncia_start 255,8562
void ncia_peer_background_teardown 295,9523
process ncia_peer_background 311,9859
void ncia_command 379,12233
ncia_rsrb_t *ncia_get_rsrb_by_index 461,14976
ncia_servertype *ncia_get_server_by_index 479,15450
void ncia_config_server 498,15874
void ncia_unconfig_server 610,19336
void ncia_config_client 660,20598
void ncia_unconfig_client 808,25737
void ncia_do_unconfig_client 834,26373
ushort ncia_cls_get_rsrb_ring 878,27963
uchar ncia_cls_get_rsrb_bridge 898,28420
void ncia_config_rsrb 926,29119
void ncia_unconfig_rsrb 1060,33517
void ncia_do_unconfig_rsrb 1107,35099
nciapeertype *ncia_find_client_by_mac 1143,35953
NciaCepT *ncia_find_circuit 1159,36333
NciaCepT *ncia_circuit_create 1182,37010
boolean ncia_check_client_sap 1221,38161
boolean ncia_cep_find_opened 1248,38704
NciaCepT *ncia_cep_search_by_ucepid 1261,38962
ncia_servertype *ncia_idb_to_server 1281,39475
TCLSIMsg *ncia_get_act_ring_msg 1299,39821
void ncia_set_enable_req 1325,40406
void rsrb_set_enable_req 1346,41030
paktype *ncia_get_ndlc_buffer 1381,41922
void ncia_fill_ndlc 1425,43024
nciapeertype *ncia_fake_msg(1526,46684
void ncia_send_ndlc 1668,52761
void ncia_send_ndlc_no_ckt 1753,55510
static NciaCepT *ncia_get_cep_for_halt(1845,58442
NciaCepT *ncia_get_cep_from_ndlc 1879,59524
void ncia_drain_client_outputq 1942,61439
void ncia_drain_circuit_outQ 1973,62352
boolean ncia_verify_circuit 2011,63377
nciapeertype *ncia_verify_client 2027,63797
void ncia_insert_cep 2044,64300
void ncia_remove_cep 2070,64820
