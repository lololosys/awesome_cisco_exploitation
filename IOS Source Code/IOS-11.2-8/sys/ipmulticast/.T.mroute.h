
mroute.h,2304
#define __MROUTE_H__157,5520
#define MDB_PRUNE_FREQUENCY 167,5655
#define MDB_EXPIRATION_TIME 168,5699
#define IP_RPF_CHECK_FREQUENCY 169,5743
#define ASSERT_RPF_FACTOR 170,5787
#define MDB_JITTER_PCT 171,5865
#define MDB_BPS_AVG_DURATION 172,5898
#define IP_MPACKET_CACHE_SIZE 173,5932
#define IP_MPACKET_SIZE_MASK 174,5968
typedef struct midbtype_ midbtype_184,6175
} midbtype;midbtype201,7321
#define MIDB_NULL_STATE 206,7361
#define MIDB_FORWARD_STATE 207,7390
#define MIDB_PRUNE_STATE 208,7419
#define MIDB_MAX_STATES 209,7448
typedef uchar ipradixtype[ipradixtype214,7518
typedef struct mdbtype_ mdbtype_216,7570
} mdbtype;mdbtype252,10030
#define PRUNE_FLAG 257,10078
#define SPARSE_FLAG 258,10155
#define CONNECTED_FLAG 259,10224
#define LOCAL_FLAG 260,10295
#define SET_REG_FLAG 261,10357
#define RP_BIT_FLAG 262,10428
#define SPT_FLAG 263,10497
#define FAST_FLAG 264,10570
#define JOIN_FLAG 265,10649
#define RPF_UNICAST 270,10751
#define RPF_DVMRP 271,10773
#define RPF_STATIC 272,10795
typedef struct mdb_rptype_ mdb_rptype_279,10991
} mdb_rptype;mdb_rptype283,11223
#define RP_UP 288,11291
#define RP_DOWN 289,11370
#define RP_COMING_UP 290,11422
typedef struct gdbtype_ gdbtype_296,11592
} gdbtype;gdbtype304,12101
typedef struct mroutetype_ mroutetype_309,12154
} mroutetype;mroutetype318,12379
struct ratetype_ ratetype_323,12439
typedef struct ipmhelpertype_ ipmhelpertype_335,12650
} ipmhelpertype;ipmhelpertype341,12967
typedef struct ipmpackettype_ ipmpackettype_349,13234
} ipmpackettype;ipmpackettype354,13367
#define MROUTE_BUGINF(MROUTE_BUGINF359,13432
#define MCACHE_DEBUG(MCACHE_DEBUG364,13587
#define MCACHE_BUGINF(MCACHE_BUGINF367,13698
#define MDB_OUTPUTQ_NULL(MDB_OUTPUTQ_NULL369,13768
#define IPMBUG(IPMBUG375,13935
#define IPMBUG_MAC 378,14050
#define IPMBUG_IP 379,14071
#define IPMBUG_ALL 380,14092
#define IS_SPARSE_GROUP(IS_SPARSE_GROUP386,14213
#define FOR_ALL_IPMULTICAST_IDBS(FOR_ALL_IPMULTICAST_IDBS388,14274
static inline gdbtype *ip_get_gdb_inline 503,18928
static inline mdbtype *ip_get_best_mdb_inline 522,19326
static inline midbtype *ip_get_midb_inline 564,20368
static inline void ipmulticast_cache_header_inline 585,20910
static inline boolean ip_rate_limit_check 606,21573
