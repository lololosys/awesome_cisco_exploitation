
ipfast.h,1432
#define __IPFAST_PREFIX_H__124,4348
typedef int (*ipencaps_t)ipencaps_t128,4406
typedef int (*ipencaps_t)(void *, iphdrtype *, ipcache_rntype *, ulong)ulong128,4406
typedef void (*crypto_encap_t)crypto_encap_t129,4479
                              ulong,ulong130,4550
struct ipcache_rntype_ ipcache_rntype_141,4896
#define	IPCACHE_FLAG_RECURSIVE	173,6020
#define	IPCACHE_FLAG_VALID	174,6092
rn2entry 189,6604
typedef struct shortlong_ shortlong_198,6823
} shortlong;shortlong203,6904
#define MAXMACSTRING	205,6918
#define IPCACHE_MEM_STARVE_LIMIT 207,6982
#define IPCACHE_MEMORY_LIMIT	208,7055
#define	IPCACHE_AGER_DEF_INTERVAL 212,7157
#define	IPCACHE_AGER_DEF_AGE_FAST	213,7225
#define	IPCACHE_AGER_DEF_AGE_SLOW	214,7292
#define IPCACHE_MIN_INVAL_DEFAULT 218,7410
#define IPCACHE_MAX_INVAL_DEFAULT 219,7479
#define IPCACHE_QUIET_IVL_DEFAULT 220,7548
#define IPCACHE_QUIET_THRESH_DEFAULT 221,7615
#define IP_FAST_ACCESSLISTS	228,7891
#define IP_FAST_ACCOUNTING	229,7927
#define IP_FAST_INPUTACCESSLISTS	230,7962
#define IP_FAST_POLICY	231,8034
#define IP_FAST_RSP_DDR	232,8092
#define IP_FAST_NAT	233,8152
#define IP_FAST_unused_bit	234,8210
#define IP_FAST_CRYPTO_MAP	235,8264
#define IP_FAST_INPUT_CRYPTO_MAP	236,8330
#define IP_FAST_INPUT_NACL	237,8400
ipcache_lookup_fast 266,9289
static inline void cache_copy_even_inline 310,10494
static inline void ipcache_adjust_refcount 332,11029
