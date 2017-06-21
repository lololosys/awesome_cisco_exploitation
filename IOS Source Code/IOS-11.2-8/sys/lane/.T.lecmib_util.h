
lecmib_util.h,911
#define LECMIB_UTIL_H55,1975
#define NULL_LEC 61,2114
#define NULL_IDB 62,2152
#define NULL_FILTER_FN 63,2187
#define NOT_SET_FLAG 65,2233
#define SR_GET_ON_SET_FLAG 66,2269
#define ID_NOT_FOUND 67,2305
#define LECMIB_CACHE_INVALID 69,2342
#define LECMIB_CACHE_GET 70,2378
#define LECMIB_CACHE_GETNEXT 71,2414
#define NULL_CACHE 74,2458
#define get_mac_addr(get_mac_addr78,2571
#define verify_mac_addr(verify_mac_addr83,2727
#define verify_rd(verify_rd86,2842
#define verify_nsap_hwaddr(verify_nsap_hwaddr91,3016
#define verify_mac_hwaddr(verify_mac_hwaddr97,3243
typedef boolean (*lec_gnFilterFn_t)lec_gnFilterFn_t106,3601
typedef boolean (*lec_gnFilterFn_t) (lec_info_t *lec_info)lec_info106,3601
typedef struct lecmib_cache_t_ lecmib_cache_t_120,4188
} lecmib_cache_t;lecmib_cache_t129,4590
static inline long get_lec_index 137,4804
static inline long get_lec_ifIndex 150,5222
