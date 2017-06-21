
crypto_engine_vip.h,2429
#define CRYPTO_ENGINE_VIP_H_103,3357
#define CRYPTO_HWEPA_NORMAL 105,3387
#define CRYPTO_SWEPA_PRESENT	106,3422
#define CRYPTO_HWEPA_NOT_PRESENT 107,3459
#define CRYPTO_ENGINE_SELF 109,3500
#define CRYPTO_ENGINE_VIP_IPC_PORT_NAME 111,3531
#define CRYPTO_CONN_UNSET 113,3601
#define CRYPTO_CONN_SET 114,3633
#define CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE 116,3666
#define CRYPTO_VIP_KEY_REQ_BLK_BUFFER_SIZE 117,3714
#define CRYPTO_REG_KEY_REQ 119,3763
#define CRYPTO_NON_REG_KEY_REQ 120,3793
#define CRYPTO_VIP_KEY_REQ_ENTRY_SET 122,3827
#define CRYPTO_VIP_KEY_REQ_ENTRY_UNSET 123,3867
#define CRYPTO_EPA_KEY_RING 125,3909
#define CRYPTO_EPA_ENCRYPT_RING 126,3939
#define CRYPTO_ENGINE_REGIST_KEY_REQ 128,3974
#define CRYPTO_ENGINE_NON_REGIST_KEY_REQ 129,4013
#define CRYPTO_EPA_ENCRYPT_KEY_UNSET 131,4057
#define CRYPTO_EPA_IV_KEY_UNSET 132,4099
#define CRYPTO_EPA_ENCRYPT_KEY_SET 133,4141
#define CRYPTO_EPA_IV_KEY_SET 134,4183
#define CRYPTO_EPA_SESSION_SET 135,4225
#define CRYPTO_EPA_SESSION_UNSET 136,4267
#define CRYPTO_ENGINE_VIP_EPA_PASSWD 138,4310
#define CRYPTO_CONN_UNSET 140,4360
#define CRYPTO_CONN_SET 141,4392
#define CRYPTO_VIP_SIGNATURE_BUFFER 143,4425
} crypto_encrypt_control_blk;crypto_encrypt_control_blk158,4763
} crypto_callback_info;crypto_callback_info169,4963
} crypto_init_callback_info;crypto_init_callback_info179,5269
} crypto_vip_pak_info;crypto_vip_pak_info191,5536
typedef void (*encrypt_t)encrypt_t193,5560
typedef void (*encrypt_t)(short, crypto_vip_pak_info*, ushort ushort193,5560
typedef void (*decrypt_t)decrypt_t194,5626
typedef void (*decrypt_t)(short, crypto_vip_pak_info*, ushort*ushort194,5626
} crypto_engine_vip_conf;crypto_engine_vip_conf210,6032
#define CRYPTO_EPA_ENCRYPT	218,6180
#define CRYPTO_EPA_DECRYPT 219,6212
#define CRYPTO_EPA_TEST2 220,6250
#define CRYPTO_EPA_DECRYPT_DONE 223,6336
#define CRYPTO_EPA_INIT_DONE 226,6430
#define CRYPTO_EPA_ENCRYPTION_ERROR 229,6520
#define CRYPTO_ENGINE_VIP_KEY_INPUT_MAX_SIZE 231,6564
} crypto_engine_key_comm_buffer_t;crypto_engine_key_comm_buffer_t237,6731
} crypto_vip_conn_entry;crypto_vip_conn_entry252,7101
} crypto_vip_key_req_context;crypto_vip_key_req_context262,7289
} crypto_engine_vip_dh_entry;crypto_engine_vip_dh_entry268,7410
} crypto_vip_fast_hdr;crypto_vip_fast_hdr274,7528
}epa_passwd_t;epa_passwd_t279,7635
static inline uint get_vip_serial_number 281,7651
