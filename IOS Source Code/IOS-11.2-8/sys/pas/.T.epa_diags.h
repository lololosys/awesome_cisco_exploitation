
epa_diags.h,7045
#define __EPA_DIAGS_H__20,586
#define MAX_FRAME_SIZE 25,831
#define MAX_FRAME_SIZE	27,865
#define MAX_OFFSET	29,946
#define MAXPACKET	30,972
#define SIZE_64K	31,1001
#define SIZE_32K	32,1024
#define SIZE_4K 33,1047
#define SIZE_1K 34,1072
#define MAX_ALLOC 35,1094
#define MAX_HMA_SIZE 36,1123
#define GHANDLE1_SIZE 37,1189
#define GHANDLE2_SIZE 38,1214
#define GHANDLE3_SIZE 39,1239
#define GHANDLE4_SIZE 40,1264
#define GHANDLE5_SIZE 41,1289
#define	ENCRYPT	43,1317
#define DECRYPT 44,1337
#define BITFLIP	45,1358
#define TESTMODE 46,1379
typedef struct statistic statistic50,1429
} epad_stat_t;epad_stat_t58,1621
typedef struct encrypt_cntl_blk_ encrypt_cntl_blk_63,1683
} encrypt_cntl_blk_t;encrypt_cntl_blk_t72,1885
#define EPA_TAMPER 75,1909
#define EPA_XTRACT 76,1940
#define EPA_TESTING 77,1971
#define EPA_PRIVMOD 78,2002
#define EPA_PASSWD 79,2033
#define EPA_RNG_SD 80,2064
#define EPA_DSSKEY 81,2095
#define EPA_1024FAIL 82,2126
#define EPA_1046FAIL 83,2161
#define EPA_BOOTFAIL 84,2196
#define EPA_LED_BOOT 85,2231
#define EPA_LED_NORMAL 86,2266
#define EPA_LED_ERROR 87,2301
#define EPA_LED_TAMPER 88,2336
#define SHA_HASH_SIZE	90,2372
#define R_SIZE	91,2397
#define S_SIZE	92,2416
#define Q_SIZE	93,2435
#define PUBLIC_SIZE	94,2454
typedef struct hMemAlloc_ hMemAlloc_96,2478
} hMemAlloc_t;hMemAlloc_t103,2615
typedef struct hMemFree_ hMemFree_105,2631
} hMemFree_t;hMemFree_t109,2712
typedef struct hMemWrite_ hMemWrite_111,2727
} hMemWrite_t;hMemWrite_t117,2850
typedef struct hMemRead_ hMemRead_119,2866
} hMemRead_t;hMemRead_t125,2986
typedef struct hMemSyndrome_ hMemSyndrome_127,3001
} hMemSyndrome_t;hMemSyndrome_t135,3161
typedef struct hMemCopy_ hMemCopy_137,3180
} hMemCopy_t;hMemCopy_t143,3303
typedef struct hReadRandom_ hReadRandom_145,3318
} hReadRandom_t;hReadRandom_t149,3400
typedef struct hGenRandom_ hGenRandom_151,3418
} hGenRandom_t;hGenRandom_t156,3517
typedef struct hSHA_ hSHA_158,3534
} hSHA_t;hSHA_t162,3609
} SHA_context;SHA_context169,3700
typedef struct hSHAinit_ hSHAinit_171,3716
} hSHAinit_t;hSHAinit_t176,3825
typedef struct hSHAupdate_ hSHAupdate_179,3879
} hSHAupdate_t;hSHAupdate_t184,3985
typedef struct hSHAfinal_ hSHAfinal_186,4002
} hSHAfinal_t;hSHAfinal_t191,4112
typedef struct hGenDSSparameters_ hGenDSSparameters_193,4128
} hGenDSSparameters_t;hGenDSSparameters_t197,4216
typedef struct hGetDSSpqg_ hGetDSSpqg_199,4240
} hGetDSSpqg_t;hGetDSSpqg_t203,4321
typedef struct hGenDSSnumber_ hGenDSSnumber_205,4338
} hGenDSSnumber_t;hGenDSSnumber_t210,4443
typedef struct hGenDSSkey_ hGenDSSkey_212,4463
} hGenDSSkey_t;hGenDSSkey_t218,4586
typedef struct hGenDSSsignature_ hGenDSSsignature_220,4603
} hGenDSSsignature_t;hGenDSSsignature_t227,4761
typedef struct hVerDSSsignature_ hVerDSSsignature_229,4784
} hVerDSSsignature_t;hVerDSSsignature_t236,4944
typedef struct hGenDHpair_ hGenDHpair_238,4967
} hGenDHpair_t;hGenDHpair_t244,5095
typedef struct hGetDHsharedNumber_ hGetDHsharedNumber_246,5112
} hGetDHsharedNumber_t;hGetDHsharedNumber_t252,5237
typedef struct hSetCipherkey_ hSetCipherkey_254,5262
} hSetCipherkey_t;hSetCipherkey_t262,5427
typedef struct hDHoneTimePad_ hDHoneTimePad_264,5447
} hDHoneTimePad_t;hDHoneTimePad_t270,5573
typedef struct hGetDHkey_ hGetDHkey_272,5593
} hGetDHkey_t;hGetDHkey_t281,5765
typedef struct epaGetStatus_ epaGetStatus_284,5782
} epaGetStatus_t;epaGetStatus_t293,5971
typedef struct epaSetStatus_ epaSetStatus_295,5990
} epaSetStatus_t;epaSetStatus_t301,6106
typedef struct epaSetPassword_{epaSetPassword_303,6125
} epaSetPassword_t;epaSetPassword_t307,6210
} epaZeroize_t;epaZeroize_t312,6283
typedef struct epaReKey_ epaReKey_314,6300
} epaReKey_t;epaReKey_t318,6379
typedef struct epaReadFlash_{epaReadFlash_320,6394
} epaReadFlash_t;epaReadFlash_t326,6517
typedef struct epaWriteFlash_ epaWriteFlash_328,6536
} epaWriteFlash_t;epaWriteFlash_t334,6661
typedef struct setRNGseed_{setRNGseed_336,6681
} setRNGseed_t;setRNGseed_t340,6761
typedef struct epaNewSession_ epaNewSession_342,6778
} epaNewSession_t;epaNewSession_t349,6912
typedef struct epaFreeSession_ epaFreeSession_351,6932
} epaFreeSession_t;epaFreeSession_t355,7018
typedef struct epaGetTime_{epaGetTime_357,7039
} epaGetTime_t;epaGetTime_t363,7165
typedef struct epaGetMethods_ epaGetMethods_365,7182
} epaGetMethods_t;epaGetMethods_t370,7292
typedef struct epa_ktest_ epa_ktest_372,7312
} epa_ktest_t;epa_ktest_t376,7401
typedef struct epa_diag_ epa_diag_378,7417
} cyl_epa_diag_t;cyl_epa_diag_t386,7655
#define SUCCESS	389,7675
#define ERR_INVALID_DATA	390,7695
#define ERR_MEM_ALLOC	391,7722
#define ERR_INPUT_LEN	392,7747
#define ERR_ERR_DSS_LEN	393,7772
#define ERR_DH_LEN	394,7799
#define	ERR_BLOCK_LEN	395,7821
#define ERR_HASH_LEN	396,7846
#define ERR_BAD_MODE	397,7870
#define ERR_TK_NUMBER	398,7894
#define	ERR_TK_POSITION	399,7920
#define ERR_TK_COUNT	400,7948
#define ERR_REQUEST_SIZE	401,7973
#define ERR_DSS_SIGNATURE	402,8001
#define ERR_TK_PRIME	403,8030
#define ERR_WEAK_KEY	404,8055
#define ERR_INPUT_VAL	405,8080
#define ERR_ACCESS	406,8106
#define ERR_CLAMP	407,8129
#define ERR_FLASH_FAIL	408,8151
#define ERR_DES_FAIL	409,8178
#define ERR_BAD_PASSWD	410,8203
#define ERR_BAD_STATUS	411,8230
#define ERR_ALGORITHM	412,8257
#define ERR_METHOD	413,8283
#define ERR_HMA_HANDLE	414,8306
#define ERR_HMA_SIZE	415,8333
#define ERR_HMA_CHKSUM	416,8358
#define ERR_HMA_ALLOC	417,8385
#define ERR_INVALID_SESSION	419,8412
#define ERR_OFFSET	420,8444
#define ERR_BAD_SIZE	421,8468
#define CRYPTO_HMA_ALLOC 426,8518
#define CRYPTO_HMA_FREE 427,8557
#define CRYPTO_HMA_WRITE 428,8596
#define CRYPTO_HMA_READ 429,8635
#define CRYPTO_MEM_SYNDROME 430,8674
#define CRYPTO_HMA_COPY 431,8713
#define CRYPTO_READ_RANDOM 432,8752
#define CRYPTO_GEN_RANDOM 433,8791
#define CRYPTO_SHA 434,8830
#define CRYPTO_SHA_INIT	435,8869
#define CRYPTO_SHA_UPDATE	436,8901
#define CRYPTO_SHA_FINAL	437,8934
#define CRYPTO_GEN_DSS_PARAMETERS 438,8966
#define CRYPTO_GET_DSS_PQG	439,9005
#define CRYPTO_GEN_DSS_NUMBER	440,9039
#define CRYPTO_GEN_DSS_KEY 441,9076
#define CRYPTO_GEN_DSS_SIGNATURE 442,9115
#define CRYPTO_VER_DSS_SIGNATURE 443,9154
#define CRYPTO_GEN_DH_PAIR 444,9193
#define CRYPTO_GET_DH_SHARED 445,9232
#define CRYPTO_DH_ONE_TIME_PAD	446,9271
#define CRYPTO_GET_DH_KEY	447,9309
#define CRYPTO_GET_EPA_STATUS 448,9342
#define CRYPTO_SET_EPA_STATUS 449,9381
#define CRYPTO_SET_EPA_PASSWD 450,9420
#define CRYPTO_ZEROIZE_EPA 451,9459
#define CRYPTO_READ_FLASH 452,9498
#define CRYPTO_WRITE_FLASH 453,9537
#define CRYPTO_SET_CIPHER_KEY 454,9576
#define CRYPTO_SET_RNG_SEED 455,9615
#define CRYPTO_NEW_SESSION 456,9654
#define CRYPTO_FREE_SESSION 457,9693
#define CRYPTO_GET_TIME 458,9732
#define CRYPTO_GET_METHODS	459,9771
#define CRYPTO_EPA_DIAG	460,9805
#define NUM_KEY_CMD	461,9837
#define MAX_KEY_TEST	462,9864
