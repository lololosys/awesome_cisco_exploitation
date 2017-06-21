
free.h,2350
#define __FREE_H__144,5589
#define MAX_FREE_POOLS 149,5669
typedef enum bad_code bad_code158,6076
} bad_code_t;bad_code_t178,6511
typedef char * (*kernel_validblock_diagnose_t)kernel_validblock_diagnose_t181,6527
					   bad_code_t)bad_code_t182,6599
typedef struct freeblocktype_ freeblocktype_190,6802
} freeblocktype;freeblocktype196,7117
#define freeptr(freeptr198,7135
struct blocktype_ blocktype_203,7221
#define BLOCKMAGIC 219,7903
typedef struct free_tree_ free_tree_238,8801
} free_tree;free_tree249,9412
typedef enum mempool_class_ mempool_class_257,9606
} mempool_class;mempool_class268,9905
struct mempool_ mempool_273,9946
#define FOR_ALL_MEMPOOLS(FOR_ALL_MEMPOOLS295,11243
#define MEMPOOL_ALTERNATE_IGNORE	308,11560
#define MEMPOOL_ALTERNATE_INCLUDE	309,11599
#define	REALLOC_TRIM 315,11743
#define BLOCKOVERHEADWORDS 321,11902
#define REDZONE 328,12126
#define REDZONEBYTES 329,12173
#define MEMORY_BLOCK_OVERHEAD 335,12337
#define MINDATABYTES 343,12590
#define MINDATAWORDS 344,12635
#define MINBLOCKWORDS 350,12811
#define MEMINUSE 356,12991
#define FREEDMASK 357,13019
#define LONGBOUND 363,13145
#define BAD_ADDRESS 368,13248
#define LARGEST_DEFAULT_FREELIST_SIZE 375,13459
#define MEMPOOL_LOW_THRESHOLD 380,13534
#define MEMPOOL_FRAGMENT_THRESHOLD 381,13577
# define BUFFERTHRESHOLD 388,13783
# define BOOTBUFFERTHRESHOLD 396,13942
#define MEMDEBUG	399,13984
#define FREESIZE(FREESIZE406,14183
struct memtrace memtrace408,14245
#define	TRACE_SIZE	413,14298
typedef boolean (*memory_pool_walk_rtn_t)memory_pool_walk_rtn_t492,17782
typedef boolean (*memory_pool_walk_rtn_t)(mempool*, blocktype blocktype492,17782
static inline void mtrace 504,18185
static inline blocktype *data_to_block(522,18535
static inline blocktype *freeblock_to_block(536,18896
static inline uchar *block_to_data(549,19204
static inline void inline_mem_lock 559,19480
static inline void inline_mem_unlock_or_free 568,19674
static inline ulong *rz_ptr(583,20003
static inline void write_red 589,20146
static inline ulong malloc_word_size 598,20309
typedef enum memory_dump_data_type memory_dump_data_type614,20625
} memory_dump_data_type_t;memory_dump_data_type_t618,20760
typedef void (*memory_dump_output_t)memory_dump_output_t620,20788
static inline void *malloc_block 642,21472
