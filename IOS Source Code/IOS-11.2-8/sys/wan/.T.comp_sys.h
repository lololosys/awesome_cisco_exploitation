
comp_sys.h,1314
#define __COMP_SYS_H__16,461
#define COMP_PRED_ENCAP_OFFSET 50,2007
#define COMP_STAC_ENCAP_OFFSET 51,2083
#define COMP_RATIO_F 53,2134
#define COMP_RATIO_M 54,2198
#define HASH_MASK 56,2263
#define GUESS_TABLE_SIZE 57,2288
#define RAND_HASH(RAND_HASH59,2354
#define RAND_HASH(RAND_HASH61,2437
#define SCRATCH_LEN	65,2570
#define INLINE_RAND_COMP(INLINE_RAND_COMP67,2596
#define INLINE_RAND_DECOMP(INLINE_RAND_DECOMP77,2847
#define INLINE_RAND_DECOMP_END(INLINE_RAND_DECOMP_END86,3068
#define COMP_ANTI_EXPANSION 100,3477
#define COMP_NO_ANTI_EXPANSION 101,3511
#define COMP_UNCOMPRESSED 102,3546
#define COMP_VERSION_MASK 103,3616
#define COMP_SYNC_MASK 111,3878
} comp_hdr_t;comp_hdr_t112,3907
#define COMPHEADERBYTES 114,3922
#define COMP_STAT_PERIODS_MAX 116,3974
#define COMP_ONEMIN 117,4006
#define COMP_5_ONEMIN 118,4028
#define COMP_10_ONEMIN 119,4052
typedef struct comp_encaps_info_ comp_encaps_info_125,4297
} comp_encaps_info;comp_encaps_info138,4930
#define XMT 142,4970
#define RCV 143,4984
#define ALLOCD 144,4998
#define DONE 145,5016
typedef struct buf_trace_entry_ buf_trace_entry_147,5033
} buf_trace_entry;buf_trace_entry150,5108
typedef struct buf_trace_ buf_trace_152,5128
} buf_trace;buf_trace155,5203
struct compress_db_ compress_db_158,5239
