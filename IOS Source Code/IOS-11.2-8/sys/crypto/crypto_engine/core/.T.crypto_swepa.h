
crypto_swepa.h,1297
#define SW_EPA_CRYPTO_H94,3022
#define DH_LENGTH_MIN 97,3071
#define  EPA_SWCRYPTO_PROCESS_RUN	101,3149
#define  EPA_SWCRYPTO_PROCESS_DEAD 102,3185
#define  EPA_SWCRYPTO_PROCESS_FAKE_PID 103,3225
#define  EPA_DES_ENCDEC_BLOCK_LENGTH 106,3311
#define  EPA_SWCRYPTO_EXP_YIELD_TIME 109,3391
#define  EPA_SWCRYPTO_EXP_RUN_TIME 110,3437
#define  EPA_ENCRYPT_QUE_SIZE	113,3532
#define  EPA_SWCRYPTO_CONN_UNSET 116,3599
#define  EPA_SWCRYPTO_CONN_SET 117,3641
#define  EPA_SWCRYPTO_CONN_CLEAR 118,3683
#define  DH_NUMBER_PAIR_ASSIGNED 121,3753
#define  DH_NUMBER_PAIR_NOT_ASSIGNED 122,3795
#define  CRYPTO_RANDOM_SEED_SIZE 125,3869
#define  CRYPTO_RANDOM_NUM_SIZE 128,3970
#define  CRYPTO_DES_KEY_VALUE_SIZE 131,4046
#define CRYPTO_DROPPED_PACKETS_CHUNK_MAX 134,4133
typedef void (* crypto_encrypt_decrypt_proc)crypto_encrypt_decrypt_proc136,4178
                                             ushort)ushort137,4255
typedef struct crypto_conn_idb_t_ crypto_conn_idb_t_139,4310
}crypto_conn_idb_t;crypto_conn_idb_t142,4405
} epa_swcrypto_connection;epa_swcrypto_connection161,5068
} epa_vip_connection_tbl;epa_vip_connection_tbl175,5515
} epa_swcrypto_DH_pair;epa_swcrypto_DH_pair183,5685
static inline void crypto_exp_iv 185,5710
static inline void crypto_iv_gen 211,6544
