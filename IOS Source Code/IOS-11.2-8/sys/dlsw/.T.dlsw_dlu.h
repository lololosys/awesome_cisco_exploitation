
dlsw_dlu.h,3788
#define _DLSW_DLU_H_76,2679
#define FAIL 80,2733
#define PASS 81,2748
#define NUM_OF_RINGS 83,2764
#define NUM_OF_SLOTS 84,2788
#define NUM_OF_TYPES 85,2812
#define DLSW_DEF_EXPLQ_DEPTH 87,2837
struct strbufstrbuf93,3026
struct str_core_rcvctl str_core_rcvctl99,3128
struct str_core_sndctl str_core_sndctl110,3260
#define  DLSW_INIT_LAN 119,3391
#define  DLSW_CONFIG_LAN 120,3426
#define  DLSW_FSM_LAN 121,3461
#define  DLSW_CSTM_LAN 122,3496
#define INIT_START 127,3625
#define INIT_STOP 128,3648
#define CONFIG_FILT_1 131,3701
#define CONFIG_FILT_2 132,3726
#define CONFIG_ADDR_1 133,3751
#define CONFIG_ADDR_2 134,3776
#define FSM_1 137,3831
#define FSM_2 138,3859
#define FSM_3 139,3887
#define CSTM_NETBIOS_WILD_FILT 142,3945
#define CSTM_NETBIOS_FILT 143,3978
#define CSTM_SNA_FILT 144,4011
#define  DLSW_LAN_INIT 147,4086
#define  DLSW_LAN_CONFIG 148,4122
#define  DLSW_LAN_FSM 149,4158
#define  DLSW_LAN_CSTM 150,4194
#define  D_ENABLE_REQ 158,4321
#define  D_DISABLE_REQ 159,4359
#define  D_ACTIVATESAP_REQ 160,4397
#define  D_ACTIVATERING_REQ 161,4435
#define  D_DEACTIVATESAP_REQ 164,4496
#define  D_DEACTIVATERING_REQ 165,4534
#define  D_REQ_OPNSTN_REQ 166,4572
#define  D_REQ_OPN_REQ 169,4633
#define  D_CLOSESTN_REQ 170,4671
#define  D_ID_REQ 171,4709
#define  D_ID_RSP 172,4747
#define  D_TEST_REQ 173,4786
#define  D_TEST_RSP 174,4825
#define  D_CONNECT_REQ 175,4864
#define  D_CONNECT_RSP 176,4903
#define  D_SIGNALSTN_REQ 177,4942
#define  D_DATA_REQ 178,4981
#define  D_UDATA_REQ 179,5020
#define  D_FLOW_REQ 180,5059
#define  D_DISCONNECT_REQ 181,5098
#define  D_DISCONNECT_RSP 182,5137
#define  D_OPENSTN_REQ 183,5176
#define  D_IDSTN_REQ 184,5215
#define  D_IDSTN_RSP 185,5254
#define  D_TESTSTN_REQ 186,5293
#define  D_TESTSTN_RSP 187,5332
#define  D_UDATASTN_REQ 188,5371
#define  D_MODIFYSAP_REQ 191,5422
#define  D_CONFIG_1 194,5463
#define  D_CONFIG_2 195,5502
#define  D_CONFIG_3 196,5541
#define  D_CONFIG_4 197,5580
#define  D_INIT_1 199,5620
#define  D_INIT_2 200,5659
#define  D_INIT_3 201,5698
#define  D_INIT_4 202,5737
#define  D_INIT_5 203,5776
 struct D_enable_reqD_enable_req206,5817
 struct D_disable_reqD_disable_req214,5953
 struct D_activatesap_reqD_activatesap_req221,6062
 struct D_modifysap_reqD_modifysap_req231,6247
 struct D_activatering_reqD_activatering_req240,6411
 struct D_deactivatesap_reqD_deactivatesap_req251,6615
 struct D_deactivatering_reqD_deactivatering_req260,6778
 struct D_reqopenstn_reqD_reqopenstn_req267,6878
 struct D_closestn_reqD_closestn_req287,7293
 struct D_id_reqD_id_req294,7450
 struct D_id_rspD_id_rsp303,7555
 struct D_test_reqD_test_req312,7663
 struct D_test_rspD_test_rsp321,7775
 struct D_generic_stn_msgD_generic_stn_msg330,7887
 struct D_connect_reqD_connect_req346,8158
 struct D_connect_rspD_connect_rsp352,8227
 struct D_signalstn_reqD_signalstn_req360,8312
 struct D_data_reqD_data_req367,8384
 struct D_udata_reqD_udata_req377,8512
 struct D_flow_reqD_flow_req386,8626
 struct D_disconnect_reqD_disconnect_req394,8710
 struct D_disconnect_rspD_disconnect_rsp401,8783
 struct D_openstn_reqD_openstn_req408,8856
 struct D_config_1D_config_1415,8926
 struct D_config_2D_config_2421,8976
 struct D_init_1D_init_1427,9026
 struct D_init_2D_init_2433,9074
 union D_primitivesD_primitives467,10427
#define  D_DLSW 506,11630
#define  D_DLSW_AND_RSRB 507,11660
#define  D_RSRB 508,11690
#define  D_LNM 509,11720
#define  D_APPN 510,11750
#define  D_DSPU 511,11780
#define  D_DLSW2 512,11810
#define  DLSW_TEST 517,11870
#define  DLSW_XID 518,11894
#define  DLSW_SABME 519,11918
#define  DLSW_UDATA 520,11942
#define  DLSW_OTHER 521,11966
#define  DLSW_ERROR 522,11990
