
crypto_hwepa.h,4362
#define CRYPTO_HWEPA_H_40,1340
#define EPA_HMA_ALLOC 46,1394
#define EPA_HMA_FREE 47,1430
#define EPA_HMA_WRITE 48,1466
#define EPA_HMA_READ 49,1502
#define EPA_MEM_SYNDROME 50,1538
#define EPA_HMA_COPY 51,1577
#define EPA_READ_RANDOM 52,1613
#define EPA_GEN_RANDOM 53,1649
#define EPA_SHA 54,1685
#define EPA_GEN_DSS_KEY 55,1721
#define EPA_GEN_DSS_SIGNATURE 56,1757
#define EPA_VER_DSS_SIGNATURE 57,1793
#define EPA_GEN_DH_PAIR 58,1829
#define EPA_GET_DH_SHARED 59,1865
#define EPA_SET_DESKAPPA_KEY 60,1901
#define EPA_SET_CIPHER_KEY 61,1937
#define EPA_GET_EPA_STATUS 62,1973
#define EPA_SET_EPA_STATUS 63,2009
#define EPA_SET_EPA_PASSWD 64,2045
#define EPA_ZEROIZE_EPA 65,2081
#define EPA_READ_FLASH 66,2117
#define EPA_WRITE_FLASH 67,2153
#define EPA_SET_RN_SEED 68,2189
#define EPA_SET_SESSION 69,2225
#define EPA_FREE_SESSION 70,2261
#define EPA_GET_TIME 71,2297
#define EPA_SET_TIME 72,2333
#define EPA_TRANS_END 73,2370
#define EPA_SET_ENCRYPT_KEY 76,2433
#define EPA_SET_IV_KEY 77,2466
#define EPA_CHECK_STATUS 78,2499
#define EPA_FREE_ENCRYPT_KEY 79,2542
#define EPA_FREE_IV_KEY 80,2584
#define EPA_FREE_DH_PRIVATE 81,2626
#define EPA_FREE_DH_SHARED 82,2668
#define CRYPTO_SHA_HASH_SIZE 87,2742
#define CRYPTO_SIG_R_SIZE 88,2779
#define CRYPTO_SIG_S_SIZE 89,2816
#define CRYPTO_DH_PUBLIC_SIZE 90,2853
#define CRYPTO_DSS_PRIVATE_KEY_SIZE 91,2890
#define EPA_ENCRYPT_METHOD 93,2930
#define EPA_ENCRYPT_ALG 94,2969
#define CRYPTO_ESA_DSS_PRIVATE_KEY_HANDLE 96,3009
#define EPA_TAMPER 101,3090
#define EPA_XTRACT 102,3129
#define EPA_TESTING 103,3168
#define EPA_PRIVMOD 104,3207
#define EPA_PASSWD 105,3246
#define EPA_RNG_SD 106,3285
#define EPA_DSSKEY 107,3324
#define EPA_1024FAIL 108,3363
#define EPA_1046FAIL 109,3402
#define EPA_BOOTFAIL 110,3441
#define EPA_LED_BOOT 111,3480
#define EPA_LED_NORMAL 112,3519
#define EPA_LED_ERROR 113,3558
#define EPA_LED_TAMPER 114,3597
#define EPA_HMA_BLK_PERMANENT 120,3669
#define EPA_HMA_CHECKSUM_ON 121,3704
#define EPA_HMA_READ_ALLOWED 122,3739
#define EPA_HMA_WRITE_ALLOWED 123,3774
#define EPA_DSS_PRIVATE_KEY_HMA_ATTRI 125,3810
#define EPA_SESSION_KEY_HMA_ATTRI 126,3854
#define EPA_IV_KEY_HMA_ATTRI 127,3897
#define EPA_DH_SHARED_HMA_ATTRI 128,3940
#define EPA_DH_PRIVATE_HMA_ATTRI 129,3983
#define EPA_RANDOM_SIZE 130,4026
#define EPA_RANDOM_HMA_ATTRI 131,4069
#define EPA_SESSION_KEY_SIZE 136,4136
#define EPA_IV_KEY_SIZE 137,4176
#define EPA_DSS_PRIVATE_KEY_SIZE 138,4216
#define EPA_DH_PRIVATE_SIZE 139,4257
#define EPA_DH_SHARED_SIZE 140,4291
#define EPA_DES56_CFB8 145,4358
#define EPA_DES56_CFB64 146,4401
#define EPA_DES56_CBC64 147,4444
#define EPA_DES56_ECB 148,4487
#define EPA_DES40_CFB8 149,4530
#define EPA_DES40_CFB64 150,4573
#define EPA_DES40_CBC64 151,4616
#define EPA_DES40_ECB 152,4659
} crypto_hma_alloc_t;crypto_hma_alloc_t168,5046
} crypto_hma_free_t;crypto_hma_free_t174,5153
} crypto_hma_write_t;crypto_hma_write_t182,5312
} crypto_hma_read_t;crypto_hma_read_t190,5466
} crypto_mem_syndrome_t;crypto_mem_syndrome_t199,5637
} crypto_hma_copy_t;crypto_hma_copy_t207,5800
} crypto_read_random_t;crypto_read_random_t213,5910
} crypto_gen_random_t;crypto_gen_random_t220,6044
} crypto_sha_t;crypto_sha_t226,6151
} crypto_gen_dss_key_t;crypto_gen_dss_key_t235,6342
} crypto_gen_dss_signature_t;crypto_gen_dss_signature_t244,6555
} crypto_sig_t;crypto_sig_t249,6681
}crypto_verify_dss_signature_t;crypto_verify_dss_signature_t258,6895
} crypto_gen_dh_pair_t;crypto_gen_dh_pair_t267,7117
} crypto_get_dh_shared_t;crypto_get_dh_shared_t277,7447
} crypto_set_deskappa_key_t;crypto_set_deskappa_key_t285,7624
}crypto_set_new_session_t;crypto_set_new_session_t294,7852
} crypto_free_session_t;crypto_free_session_t300,7963
} crypto_get_epa_status_t;crypto_get_epa_status_t311,8239
} crypto_set_epa_status_t;crypto_set_epa_status_t319,8379
} crypto_set_epa_passwd_t;crypto_set_epa_passwd_t325,8485
} crypto_set_rn_seed_t;crypto_set_rn_seed_t331,8589
} crypto_zeroize_t;crypto_zeroize_t336,8668
} crypto_gen_syndrome_t;crypto_gen_syndrome_t346,8856
} crypto_set_cipher_key_t;crypto_set_cipher_key_t356,9074
static inline ushort epa_encrypt_method 358,9102
#define CRYPTO_DSS_PH_1 438,12317
#define CRYPTO_DSS_PH_2 439,12349
#define EPA_TEST_ONLY 441,12382
