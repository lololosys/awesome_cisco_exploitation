
crypto_engine_rsp_ipc.h,1872
#define CRYPTO_ENGINE_VIP_SLOT0_NAME 80,2655
#define CRYPTO_ENGINE_VIP_SLOT1_NAME 81,2718
#define CRYPTO_ENGINE_VIP_SLOT2_NAME 82,2781
#define CRYPTO_ENGINE_VIP_SLOT3_NAME 83,2844
#define CRYPTO_ENGINE_VIP_SLOT4_NAME 84,2907
#define CRYPTO_ENGINE_VIP_SLOT5_NAME 85,2970
#define CRYPTO_ENGINE_VIP_SLOT6_NAME 86,3033
#define CRYPTO_ENGINE_VIP_SLOT7_NAME 87,3096
#define CRYPTO_ENGINE_VIP_SLOT8_NAME 88,3159
#define CRYPTO_ENGINE_VIP_SLOT9_NAME 89,3222
#define CRYPTO_ENGINE_VIP_SLOT10_NAME 90,3285
#define CRYPTO_ENGINE_RSP_CRNT_PORT 92,3351
#define CRYPTO_VIP_MSG_TYPE_SIZE 94,3418
#define CRYPTO_MAP_TAG_MAX_SIZE 96,3455
} crypto_engine_vip_ipc_msg;crypto_engine_vip_ipc_msg106,3637
} crypto_engine_vip_map_msg;crypto_engine_vip_map_msg111,3744
#define CRYPTO_VIP_ADD_MAP_HEAD 113,3774
#define CRYPTO_VIP_ADD_MAP 114,3815
#define CRYPTO_VIP_DEL_MAP_HEAD 115,3856
#define CRYPTO_VIP_DEL_MAP 116,3897
#define CRYPTO_VIP_ASSIGN_MAP 117,3938
#define CRYPTO_VIP_DEASSIGN_MAP 118,3979
#define CRYPTO_VIP_MAP_SET_PEUPE 119,4020
#define CRYPTO_VIP_UPD_CONN_ID 120,4061
#define CRYPTO_VIP_REMOVE_MAP 121,4102
#define CRYPTO_VIP_GET_CONN_TBL 122,4143
#define CRYPTO_VIP_MAP_SET_MODE 123,4184
}crypto_vip_upd_map_msg;crypto_vip_upd_map_msg140,4659
} crypto_vip_map_tag_msg;crypto_vip_map_tag_msg146,4814
}crypto_vip_assign_map_to_int_msg;crypto_vip_assign_map_to_int_msg155,4988
} crypto_vip_map_upd_conn_msg;crypto_vip_map_upd_conn_msg164,5194
} crypto_engine_vip_upd_idb_msg;crypto_engine_vip_upd_idb_msg171,5326
} crypto_vip_get_conn_tbl_msg;crypto_vip_get_conn_tbl_msg176,5446
#define CRYPTO_VIP_MAP_HEAD_MAX 179,5479
#define CRYPTO_VIP_MAP_MAX 180,5516
#define CRYPTO_VIP_MAP_TAG_MAX 181,5554
#define CRYPTO_VIP_KEY_BLK_MAX 182,5592
#define CRYPTO_VIP_VECTOR_MAX 183,5630
static inline char* crypto_generate_ipc_port_name 191,5844
