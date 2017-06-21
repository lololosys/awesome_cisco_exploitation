
crypto_engine.h,3511
#define EPA_ENGINE_H131,4263
#define    CRYPTO_CONNTBL_SIZE 133,4341
#define    CRYPTO_ENGINE_MAX_NUM 134,4420
#define    CRYPTO_ENGINE_REQPOOL_SIZE 135,4494
#define    CRYPTO_ENGINE_REQ_ENTRY_UNUSED 136,4573
#define    CRYPTO_REQ_DATA_MAX_SIZE 137,4654
#define    CRYPTO_DH_PAIR_SET 138,4729
#define    CRYPTO_DH_PAIR_UNSET 139,4771
#define    CRYPTO_REQ_SEQ_END 142,4867
#define    CRYPTO_ENGINE_VIP_SEND_TO_RSP 143,4902
#define    CRYPTO_REQ_TIMEOUT_SET 146,5006
#define    CRYPTO_REQ_TIMEOUT_UNSET 147,5046
#define    CRYPTO_REQ_TIMER_UPDATE_INTERVAL 148,5086
#define CRYPTO_ENGINE_RSP_BUFFER_LENGTH 151,5194
#define DH_LENGTH_MIN 153,5240
#define DSS_LENGTH_MIN 154,5265
#define CRYPTO_ENGINE_IPC_MSG_SIZE 156,5292
#define CRYPTO_ENGINE_MAX 157,5334
#define CRYPTO_NO_CANCEL 158,5371
#define CRYPTO_ENGINE_REGISTER 160,5407
#define CRYPTO_ENGINE_DEREGISTER 161,5445
#define CRYPTO_RELEASE_CNT_HIGH	167,5583
#define CRYPTO_RELEASE_CNT_MID	168,5619
#define CRYPTO_RELEASE_CNT_LOW	169,5654
#define CRYPTO_RELEASE_CNT_DEFAULT 170,5689
#define CRYPTO_KEY_COMM_TIMER 172,5748
} crypto_encrypt_header;crypto_encrypt_header187,6186
typedef struct epa_engine_req_ epa_engine_req_192,6283
} epa_engine_req;epa_engine_req199,6547
typedef struct epa_crypto_blk_header_ epa_crypto_blk_header_204,6634
} epa_crypto_blk_header;epa_crypto_blk_header211,6814
typedef struct epa_crypto_blk_ epa_crypto_blk_213,6840
} epa_crypto_blk;epa_crypto_blk217,6992
} epa_crypto_session_key_data_blk;epa_crypto_session_key_data_blk228,7280
} crypto_gen_dh_param_data_blk;crypto_gen_dh_param_data_blk233,7412
} crypto_dh_pair;crypto_dh_pair240,7596
} crypto_sign_data_blk;crypto_sign_data_blk246,7703
} crypto_vip_sign_data_blk;crypto_vip_sign_data_blk251,7796
} crypto_verify_data_blk;crypto_verify_data_blk260,8024
} crypto_vip_verify_data_blk;crypto_vip_verify_data_blk267,8191
} crypto_clear_conn_entry_data_blk;crypto_clear_conn_entry_data_blk271,8262
}crypto_config_epa_data_blk;crypto_config_epa_data_blk276,8386
}crypto_gen_dss_key_data_blk;crypto_gen_dss_key_data_blk281,8503
} crypto_gen_syndrome_data_blk;crypto_gen_syndrome_data_blk288,8694
}crypto_clear_dh_number_data_blk;crypto_clear_dh_number_data_blk292,8773
typedef void (* crypto_swepa_output_t)crypto_swepa_output_t309,9232
typedef void (* crypto_swepa_output_t) (ushort,ushort309,9232
typedef boolean (* crypto_vip_encrypt_t)crypto_vip_encrypt_t310,9289
typedef boolean (* crypto_vip_encrypt_t) (void*, ushort ushort310,9289
typedef boolean (* crypto_vip_decrypt_t)crypto_vip_decrypt_t311,9349
typedef boolean (* crypto_vip_decrypt_t) (void*, ushort ushort311,9349
typedef void (* crypto_engine_encrypt_proc)crypto_engine_encrypt_proc312,9409
typedef void (* crypto_engine_encrypt_proc) (ushort, crypto_encrypt_header crypto_encrypt_header312,9409
typedef void (* crypto_engine_decrypt_proc)crypto_engine_decrypt_proc313,9488
typedef void (* crypto_engine_decrypt_proc) (ushort, crypto_encrypt_header crypto_encrypt_header313,9488
typedef paktype * (* crypto_engine_reassemble_proc)crypto_engine_reassemble_proc314,9567
typedef paktype * (* crypto_engine_reassemble_proc) (paktype paktype314,9567
} crypto_engine_idb_t;crypto_engine_idb_t327,10281
typedef struct crypto_drop_pak_ crypto_drop_pak_329,10305
}crypto_drop_pak_t;crypto_drop_pak_t333,10415
typedef boolean (* ip_les_fast_t)ip_les_fast_t386,12406
typedef boolean (* ip_les_fast_t)(paktype paktype386,12406
