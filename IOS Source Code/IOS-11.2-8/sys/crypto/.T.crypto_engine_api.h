
crypto_engine_api.h,6320
#define EPA_API_H176,6283
#define EPA_CRYPTO_UNIT_NOT_DEFINED(EPA_CRYPTO_UNIT_NOT_DEFINED185,6477
#define EPA_TEST_DATA_LENGTH 186,6534
#define CRYPTO_ENGINE_ON_RP_RSP 187,6580
#define CRYPTO_ENGINE_NOT_PRESENT 188,6624
#define CRYPTO_ENGINE_ON_VIP 189,6668
#define CRYPTO_CONNTBL_SIZE 190,6710
#define CRYPTO_VALID_SIGNATURE 192,6790
#define CRYPTO_INVALID_SIGNATURE 193,6838
#define CRYPTO_FAST_SWITCH 195,6887
#define CRYPTO_PROCESS_SWITCH 196,6927
#define CRYPTO_RSP_FLOW_SWITCH 197,6967
#define CRYPTO_VIP_FAST_SWITCH 198,7007
#define CRYPTO_TUNNEL_FAST_SWITCH	199,7048
#define CRYPTO_PLATFORM_UNKNOWN 201,7091
#define CRYPTO_PLATFORM_RP 202,7128
#define CRYPTO_PLATFORM_RSP 203,7164
#define CRYPTO_PLATFORM_VIP 204,7200
#define CRYPTO_ENCRYPT_BLOCK_LEN 206,7237
#define DH_LENGTH_MIN 207,7273
#define CRYPTO_ENGINE_NAME_MAX_SIZE 209,7301
#define CRYPTO_ENGINE_NOT_INSTALLED 211,7344
#define CRYPTO_ENGINE_INSTALLED 212,7388
#define CRYPTO_ENGINE_DSS_KEY_GEN 213,7432
#define CRYPTO_VIP_SERIAL_SIZE 215,7477
#define CRYPTO_PAK_OPTION_SIZE 217,7520
#define CRYPTO_CHALLENGE_SIZE 219,7563
#define CRYPTO_DSS_SIG_SIZE 220,7605
#define    CRYPTO_TYPE_UNINIT 224,7695
#define    CRYPTO_TYPE_UNKNOWN 225,7735
#define    CRYPTO_SWEPA 226,7777
#define    CRYPTO_HWEPA 227,7817
#define CRYPTO_EPA_PASSWD_MAX_SIZE 229,7858
#define CRYPTO_TCP_IV_SIZE 234,7928
#define CRYPTO_UDP_IV_SIZE 235,7962
#define CRYPTO_IP_IV_SIZE 236,7996
#define CRYPTO_DEFAULT_PROT 237,8030
#define CRYPTO_TCP_PROT 238,8067
#define CRYPTO_UDP_PROT 239,8104
#define CRYPTO_DEFAULT_IV_OFFSET 240,8141
#define CRYPTO_MAX_IV_SIZE 241,8176
#define    CRYPTO_CFB8_SHIFT_COUNT 244,8234
typedef enum epa_op_status_ epa_op_status_246,8274
} epa_status;epa_status250,8359
#define EPA_TIMEOUT 252,8374
typedef enum epa_crypto_name_ epa_crypto_name_254,8402
} epa_crypto_command;epa_crypto_command284,9195
#define CRYPTO_ENGINE_OP_SUCCESS 289,9239
#define CRYPTO_ENGINE_OP_FAIL 290,9294
#define CRYPTO_ENGINE_GEN_DSS_KEY_FAIL 291,9349
#define CRYPTO_ENGINE_DSS_SIGN_FAIL 292,9404
#define CRYPTO_ENGINE_DSS_VER_FAIL 293,9459
#define CRYPTO_ENGINE_SHA_FAIL 294,9514
#define CRYPTO_ENGINE_GEN_DH_PAIR_FAIL 295,9569
#define CRYPTO_ENGINE_GEN_SESSION_KEY_FAIL 296,9624
#define CRYPTO_ENGINE_GEN_IV_KEY_FAIL 297,9679
#define CRYPTO_ENGINE_INVALID_ALG_TYPE 298,9734
#define CRYPTO_ENGINE_INVALID_INPUT 299,9789
#define CRYPTO_ENGINE_MEM_ALLOC_FAIL 300,9844
#define CRYPTO_ENGINE_KEY_REQ_TIMEOUT 301,9899
#define CRYPTO_ENGINE_IPC_FAIL 302,9954
#define CRYPTO_ENGINE_CLEAR_CONN_FAIL 303,10009
#define CRYPTO_ENGINE_ENCRYPT_QUEUE_FULL 304,10064
#define CRYPTO_ENGINE_ENCRYPT_DEQUEUE_FAIL 305,10119
#define ESA_DSS_SIGNATURE_ERROR 308,10204
#define ESA_INPUT_INVALID 309,10255
#define ESA_BAD_PASSWORD 310,10306
#define ESA_LATCH_SET 311,10357
#define CRYPTO_DSS_KEY_GEN_TIMEOUT 314,10446
#define CRYPTO_KEY_COMM_TIMEOUT 315,10500
#define CRYPTO_SIGN_TIMEOUT 316,10554
#define CRYPTO_DH_PUBLIC_NUMBER_SIZE 322,10673
#define CRYPTO_DSS_P_MAX_SIZE 323,10716
#define CRYPTO_DSS_Q_MAX_SIZE 324,10760
#define CRYPTO_DSS_G_MAX_SIZE 325,10804
typedef enum alg_types_ alg_types_327,10849
} alg_types;alg_types352,11538
struct DH_alg_phase1_ DH_alg_phase1_355,11589
struct DH_alg_phase2_ DH_alg_phase2_363,11775
struct DH_alg_ DH_alg_367,11853
struct DSS_KEY_GEN_alg_ DSS_KEY_GEN_alg_376,12050
struct RSA_KEY_GEN_alg_ RSA_KEY_GEN_alg_383,12170
struct random_number_alg_ random_number_alg_387,12244
} epa_alg_obj;epa_alg_obj400,12641
#define CRYPTO_DSS_PUBLIC_KEY_MAX_SIZE 406,12702
#define CRYPTO_DSS_PRIVATE_KEY_MAX_SIZE 407,12775
#define CRYPTO_RSA_PUBLIC_KEY_MAX_SIZE 408,12848
#define CRYPTO_RSA_PRIVATE_KEY_MAX_SIZE 409,12909
#define CRYPTO_RANDOM_MAX_SIZE 410,12970
#define CRYTPO_DSS_SIG_SIZE 411,13025
#define CRYPTO_ENCRYPTION_QUEUE_LEN 413,13086
typedef enum key_types_ key_types_415,13133
} key_types;key_types420,13237
struct DSS_PUBLIC_KEY_ DSS_PUBLIC_KEY_422,13251
struct RSA_PUBLIC_KEY_ RSA_PUBLIC_KEY_427,13323
} crypto_dss_keys;crypto_dss_keys437,13575
} crypto_rsa_keys;crypto_rsa_keys444,13775
} epa_key_obj;epa_key_obj454,14041
} epa_session_id;epa_session_id461,14164
typedef struct crypto_engine_config_t_ crypto_engine_config_t_464,14227
} crypto_engine_config_t;crypto_engine_config_t476,14689
typedef struct epa_pak_info_ epa_pak_info_483,14837
} epa_pak_info;epa_pak_info488,14975
}crypto_fs_pak_info;crypto_fs_pak_info502,15303
} crypto_ps_pak_info;crypto_ps_pak_info518,15662
} crypto_pak_info;crypto_pak_info530,15915
} epa_callback_context;epa_callback_context569,17423
typedef void (*epa_callback_handle)epa_callback_handle571,17448
                                     struct epa_pak_info_ *pak_info)pak_info576,17717
} epa_rtn_info;epa_rtn_info611,18717
} epa_rtn_DH_pub_number;epa_rtn_DH_pub_number615,18810
} epa_rtn_DSS_number;epa_rtn_DSS_number622,19007
} epa_rtn_alg_param;epa_rtn_alg_param630,19192
} epa_rtn_syndrome;epa_rtn_syndrome635,19279
} epa_rtn_random_number;epa_rtn_random_number640,19388
} epa_rtn_sig_key;epa_rtn_sig_key651,19629
} epa_rtn_get_status;epa_rtn_get_status656,19706
} epa_rtn_sig;epa_rtn_sig660,19785
#define CRYPTO_NUM_PROTOCOLS 666,19852
}   cylink_cyConfig[cylink_cyConfig672,20170
#define CYLINK_DES40_CFB8 675,20279
#define CYLINK_DES40_CFB64 676,20316
#define CYLINK_DES56_CFB8 677,20353
#define CYLINK_DES56_CFB64 678,20390
static inline ushort get_cylink_encrypt_method 684,20517
static inline char * xlate_cylink_encrypt_method 699,20864
#define EPA_TAMPERED(EPA_TAMPERED732,21624
#define EPA_XTRACTED(EPA_XTRACTED733,21667
#define EPA_PASSWD_SET(EPA_PASSWD_SET734,21710
#define EPA_DSS_SET(EPA_DSS_SET735,21755
#define EPA_BOOT_LED(EPA_BOOT_LED736,21798
#define EPA_NORMAL_LED(EPA_NORMAL_LED737,21842
#define EPA_ERROR_LED(EPA_ERROR_LED738,21888
#define EPA_TAMPER_LED(EPA_TAMPER_LED739,21933
#define CRYPTO_EGP_HDR_SIZE 746,22203
#define CRYPTO_OSPF_HDR_SIZE 747,22236
#define IS_ENCRYPTION(IS_ENCRYPTION916,28778
static inline ushort crypto_get_engine_id 918,28831
static inline void crypto_get_iv_seed 937,29252
static inline int crypto_count_sre 974,30371
static inline void crypto_set_encrypt_start 987,30672
