
rpsp_memd.h,1422
#define ODD_BYTE_START 45,1407
#define VALID_PKT_LENGTH 46,1478
#define CCB_BASE 48,1540
#define QUE_BASE 49,1604
#define ICB_BASE 50,1670
#define EXT_ICB_BASE 51,1735
#define ICB_LENGTH 52,1801
#define ICB_INTFCS 53,1854
#define ICB_TXQHEAD 54,1922
#define FCI_BUFFERIN 55,1988
#define MAX_LOCK_SPIN 57,2059
#define MAX_ICB_PER_SLOT 59,2149
#define MEMD_QUE_RAW 61,2216
#define MEMD_QUE_RET 62,2246
#define RPSP_MEMD2VIRTUAL(RPSP_MEMD2VIRTUAL67,2294
#define RPSP_VIRTUAL2MEMD(RPSP_VIRTUAL2MEMD68,2361
#define RPSP_PP_ADDR(RPSP_PP_ADDR70,2434
#define RPSP_BH_CACHE_INDEX(RPSP_BH_CACHE_INDEX73,2521
#define VIP_RPSP_BF_TX_START_SHIFT 76,2607
#define VIP_RPSP_BF_TX_BYTES_MASK 77,2648
#define VIP_RPSP_BH_SHIFT	85,2926
#define VIP_RPSP_BHFIELD_SHIFT	87,2956
} vip_rpsp_que_t;vip_rpsp_que_t129,3925
#define q_rpsp_rawhead 131,3944
#define q_rpsp_rawtail 132,3982
#define q_rpsp_rethead 133,4020
#define q_rpsp_rettail 134,4058
#define q_rpsp_rawque 135,4096
#define q_rpsp_retque 136,4135
static inline uchar *vip_rpsp_lookup_bhcache 162,5022
static inline vip_bhdr_t *vip_rpsp_local_freeq_fetch 185,5626
static inline void vip_rpsp_local_raw_queue_add 230,6710
static inline void vip_rpsp_return_rx_bh 248,7175
static inline void vip_rpsp_local_returnq_add 258,7393
static inline void vip_rpsp_local_returnq_add_list 273,7769
static inline void vip_rpsp_local_rawq_add_list 289,8165
