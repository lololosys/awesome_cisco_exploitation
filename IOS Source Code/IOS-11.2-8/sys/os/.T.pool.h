
pool.h,2644
#define __POOL_H__85,3052
typedef enum pool_item_type_ pool_item_type_93,3143
} pool_item_type;pool_item_type97,3261
typedef struct pool_item_status_ pool_item_status_99,3280
} pool_item_status;pool_item_status102,3364
typedef  void * (*pool_item_create_t)pool_item_create_t104,3385
typedef  void * (*pool_item_create_t)(pooltype *, pool_item_type type)type104,3385
typedef  void   (*pool_item_destroy_t)pool_item_destroy_t105,3457
typedef  void   (*pool_item_destroy_t)(pooltype pooltype105,3457
typedef  void   (*pool_item_status_t)pool_item_status_t106,3517
				      pool_item_status *status)status108,3585
typedef  void * (*pool_item_get_t)pool_item_get_t109,3622
typedef  void * (*pool_item_get_t)(pooltype pooltype109,3622
typedef  void   (*pool_item_ret_t)pool_item_ret_t110,3670
typedef  void   (*pool_item_ret_t)(pooltype pooltype110,3670
typedef boolean (*pool_item_validate_t)pool_item_validate_t111,3726
typedef boolean (*pool_item_validate_t)(pooltype pooltype111,3726
typedef  void * (*pool_cache_get_t)pool_cache_get_t113,3788
typedef  void * (*pool_cache_get_t)(pooltype pooltype113,3788
typedef  void   (*pool_cache_ret_t)pool_cache_ret_t114,3837
typedef	 void   (*pool_cache_threshold_t)pool_cache_threshold_t115,3882
typedef	 void   (*pool_cache_threshold_t)(pooltype *pool)pool115,3882
typedef struct pool_item_vectors_ pool_item_vectors_117,3942
} pool_item_vectors;pool_item_vectors124,4173
typedef struct pool_cache_vectors_ pool_cache_vectors_126,4195
} pool_cache_vectors;pool_cache_vectors130,4334
struct pooltype_ pooltype_132,4357
#define POOLMAGIC 182,7322
#define POOLVALID(POOLVALID183,7351
#define POOL_DYNAMIC	208,8425
#define POOL_SANITY	209,8459
#define POOL_CACHE	210,8492
#define POOL_PUBLIC_DUPLICATE	211,8524
#define POOL_INVISIBLE	212,8566
#define POOL_DEFAULT_FLAGS 214,8610
#define POOL_SERVICE_NONE 220,8699
#define POOL_SERVICE_GROWTH 221,8743
#define POOL_SERVICE_DISCARD 222,8787
#define POOL_GROUP_MAX 229,8864
#define POOL_GROUP_PUBLIC 230,8897
#define POOL_GROUP_INVALID 231,8928
#define POOL_USER_MAX	237,9017
#define POOL_USER_PERM	238,9075
#define POOL_USER_MIN	239,9135
#define POOL_PRUNE_TIME 244,9235
static inline void *pool_dequeue_item 257,9393
static inline void pool_unqueue_item 276,9646
static inline void pool_enqueue_item 289,9858
static inline int pool_cache_available_items 310,10286
static inline void *pool_dequeue_cache 322,10454
static inline boolean pool_enqueue_cache 343,10778
static inline boolean pool_low_watermarks 363,11158
static inline pooltype *pool_find_by_name_internal 375,11369
