
dlsw_csm.h,3576
#define __DLSW_CSM_H__106,3904
#define CSM_DEF_EXPLQ_DEPTH 123,4244
#define CSM_DEF_SNA_XCAST_FLAG 124,4297
#define CSM_DEF_NB_XCAST_FLAG 125,4359
#define CSM_DEF_SNA_EXPL_TO 130,4456
#define CSM_DEF_SNA_CACHE_TO 131,4506
#define CSM_DEF_SNA_VERIFY_INT 132,4555
#define CSM_DEF_SNA_RETRY_INT 133,4603
#define CSM_DEF_NB_EXPL_TO 134,4652
#define CSM_DEF_NB_CACHE_TO 135,4700
#define CSM_DEF_NB_VERIFY_INT 136,4749
#define CSM_DEF_NB_RETRY_INT 137,4797
#define CSM_DEF_NEG_CACHE_TO 138,4845
#define CSM_DEF_EXPL_WAIT_INT 139,4882
#define CSM_DEF_EXPL_DELAY 140,4919
#define CSM_DEF_POD_RETRY_TO 141,4956
#define CSM_DEF_POD_RETRY_COUNT 143,5006
#define CSM_DEF_TOUCH_SNA_CACHE_ACTIVE_TO 148,5084
#define CSM_DEF_TOUCH_SNA_CACHE_RESET_TO 149,5132
#define CSM_DEF_TOUCH_NB_CACHE_ACTIVE_TO 150,5180
#define CSM_DEF_TOUCH_NB_CACHE_RESET_TO 151,5228
#define CSM_TOUCH_CACHE_RESTART_DELTA 152,5276
#define MAX_REACH 157,5360
#define AVAILABLE 160,5467
#define USE_BIT 161,5495
#define INDELIBLE 162,5523
#define DLSW_LPORT_ID_COOKIE 167,5578
#define DLSW_XPORT_ID_COOKIE 168,5652
#define DLSW_SAP_COOKIE 169,5726
#define DLSW_AC_COOKIE 170,5764
#define DLSW_FC_COOKIE 171,5804
#define DIRECT_FRAME 172,5848
#define MAX_TESTDATA_SIZE 173,5922
#define MAX_UIDATA_SIZE 174,5996
#define NQ_FIND_NAME 177,6099
#define NQ_CALL_NAME 178,6133
#define CSM_INVALID_PARAMS 181,6186
#define CSM_NO_REACH_ENTRY 182,6221
#define CSM_NO_MTU_SIZE 183,6257
#define CSM_NO_RING_MATCH 184,6293
#define CSM_NO_EXCL_MATCH 185,6329
#define CSM_NONE_AVAILABLE 186,6365
#define CSM_FRAME_FILTERED 187,6401
    } e_xmission_type;e_xmission_type199,6912
    } e_cache_type;e_cache_type208,7074
    } e_cache_status;e_cache_status217,7218
    } e_message_pointer_type;e_message_pointer_type223,7299
struct lcache_ lcache_225,7330
typedef struct lcache_ lcache_t;lcache_t236,7642
    } u_group_id_t;u_group_id_t242,7749
struct group_cache_ group_cache_244,7770
typedef struct group_cache_ group_cache_t;group_cache_t249,7903
struct resp_pend_ resp_pend_251,7947
typedef struct resp_pend_ resp_pend_t;resp_pend_t270,8890
struct icr_pend_ icr_pend_272,8930
typedef struct icr_pend_ icr_pend_t;icr_pend_t288,9525
struct startdl_pend_ startdl_pend_290,9563
typedef struct startdl_pend_ startdl_pend_t;startdl_pend_t303,10024
struct nb_retry_info_ nb_retry_info_306,10071
typedef struct nb_retry_info_ nb_retry_info_t;nb_retry_info_t318,10444
struct nr_pend_ nr_pend_320,10492
typedef struct nr_pend_ nr_pend_t;nr_pend_t334,10973
struct dlxnr_pend_ dlxnr_pend_336,11009
typedef struct dlxnr_pend_ dlxnr_pend_t;dlxnr_pend_t346,11364
struct nbanq_info_ nbanq_info_348,11406
typedef struct nbanq_info_ nbanq_info_t;nbanq_info_t355,11591
    } wc_mac_cache_t;wc_mac_cache_t368,11968
    } mac_cache_key_t;mac_cache_key_t373,12080
} wc_name_cache_t;wc_name_cache_t382,12297
#define CSM_DEBUG 386,12336
#define CSMV_DEBUG 387,12424
#define CSME_DEBUG 388,12484
#define SNA_CSM_DEBUG 389,12598
#define SNA_CSMV_DEBUG 390,12738
#define SNA_CSME_DEBUG 391,12850
#define NETB_CSM_DEBUG 392,13016
#define NETB_CSMV_DEBUG 393,13157
#define NETB_CSME_DEBUG 394,13270
#define LCACHE_MAC 410,13896
#define LCACHE_NAME 411,13932
#define RCACHE_MAC 412,13969
#define RCACHE_NAME 413,14005
#define WCACHE_MAC 414,14042
#define WCACHE_NAME 415,14078
#define ICR_PEND 416,14115
#define RESP_PEND 417,14149
#define DLXNR_PEND 418,14184
#define NR_PEND 419,14220
#define NBANQ_FILE 420,14253
#define STARTDL_PEND 421,14289
