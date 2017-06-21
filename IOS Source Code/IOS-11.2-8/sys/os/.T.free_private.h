
free_private.h,1049
typedef struct malloc_failures_ malloc_failures_105,3690
} malloc_failures;malloc_failures111,3981
#define MALLOC_FAILURE_RECORD_COUNT 113,4001
#define ALLOC_PID_INIT 138,4742
#define ALLOC_PID_SCHED 139,4807
#define MIN_ALLOC_PID 140,4868
#define HEAPCHECK_IVL_DEFAULT 146,4950
#define CKSUMCHECK_IVL_DEFAULT 147,5024
#define CHECKHEAPS_TIME 148,5102
static inline blocktype *prev_block(155,5338
#define IS_LAST_FREE_LIST(IS_LAST_FREE_LIST162,5559
#define PREV_FREE_BLOCK(PREV_FREE_BLOCK168,5787
#define FREEMAGIC 170,5918
#define MAXBLOCKSIZE 171,5947
#define WAY_TOO_MANY_TRIPS 173,5980
static inline ulong block_size 180,6168
static inline blocktype *block_end 192,6466
#define CRASH_AND_BURN(CRASH_AND_BURN208,6971
#define CRASH_AND_BURN_FOR_SCHEDULER(CRASH_AND_BURN_FOR_SCHEDULER221,7377
static inline boolean valid_freeblock_inline 247,8221
static inline mempool *mempool_find_by_class_inline 294,9226
static inline regiontype *mempool_find_region_by_addr 307,9488
static inline boolean mempool_is_alias 329,9965
