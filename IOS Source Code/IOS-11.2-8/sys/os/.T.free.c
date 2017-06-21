
free.c,3785
typedef enum {HEAP, CKSUM} checkheaps_timer_type;checkheaps_timer_type258,10374
#define MAX_FREE_TREE_DEPTH 284,11222
void report_malloc_failure 292,11391
void errmsg_mtrace 318,12127
#define MT(MT320,12155
#define FT(FT321,12223
#define mt(mt322,12291
#define ft(ft323,12326
void errmsg_blkinfo 335,12642
mempool *mempool_find_by_class 360,13268
mempool *mempool_find_by_addr 372,13496
static boolean valid_freeblock 392,14001
static inline void delete_free_block 404,14244
typedef enum {FREE_HEAD, FREE_TAIL} freetype;freetype418,14571
static inline void add_free_block 420,14618
void init_block 455,15840
void init_new_block 488,16567
static inline boolean right_freelist_for_block 500,16865
static inline free_tree *freelist_find_by_size_inline 510,17116
free_tree *freelist_find_by_size 532,17643
static inline free_tree *freelist_find_by_block 542,17850
static inline free_tree *freelist_for_malloc 552,18105
static free_tree *build_subtree 578,18704
static free_tree *mempool_add_free_list_internal 610,19676
boolean mempool_add_free_list 773,24780
boolean mempool_add_free_list_manual 783,25032
static void free_tree_init 802,25371
static inline void *malloc_aligned_inline 834,26077
void *mempool_malloc 884,27294
void *mempool_malloc_aligned 890,27437
void *malloc 896,27604
void *malloc_aligned 902,27730
void *malloc_named 914,28049
void *malloc_named_aligned 934,28412
void *malloc_iomem 957,28790
void *malloc_named_iomem 977,29116
void *malloc_iomem_aligned 999,29519
void *malloc_named_iomem_aligned 1013,29800
void *malloc_pcimem 1035,30214
void *malloc_named_pcimem 1056,30547
void *malloc_fast 1078,30934
void *malloc_named_fast 1098,31266
static inline boolean check_red 1120,31715
static inline boolean check_red_for_scheduler 1133,31995
static inline boolean validblock_inline 1165,33073
boolean validblock 1298,36316
boolean validblock_gentle 1306,36468
blocktype *block_suspend 1319,36769
blocktype *get_aligned_block 1371,37956
blocktype *get_block 1647,46220
boolean memory_available 1804,51102
void name_memory 1883,53148
void memory_new_owner 1900,53668
void memory_specify_owner 1914,53964
void memory_specify_allocator 1928,54291
void poison 1940,54505
void free 1994,55707
void mem_lock 2128,59204
boolean mem_lockable 2153,59689
int mem_refcount 2169,60066
mempool *mempool_create 2186,60326
boolean mempool_add_region 2278,62571
void memory_init 2339,64037
void mempool_add_alternate_pool 2374,64771
void mempool_add_exception_pool 2391,65166
void mempool_add_exception_pool_init 2408,65576
void mempool_add_alias_pool 2421,65927
void mempool_delete_alias_pool 2436,66193
void mempool_set_low_threshold 2447,66428
void mempool_set_fragment_threshold 2467,66921
boolean validmem 2487,67318
boolean validmem_complete 2498,67559
boolean validmem_complete_interrupt 2524,68090
boolean validlink 2549,68508
static void validate_memory 2560,68703
static void validate_free_memory(2621,70295
boolean mempool_is_low 2719,72678
boolean mempool_is_empty 2738,73054
ulong mempool_get_free_bytes 2785,74202
ulong mempool_get_used_bytes 2807,74795
ulong mempool_get_total_bytes 2831,75464
boolean validpiece 2853,75991
boolean linkinblock 2877,76498
void *realloc 2900,77117
ulong get_block_size 2936,77952
#define TALLY_SUSPEND_INTERVAL 2953,78254
void tally_memory_usage(2955,78301
static void mempool_checkheap_for_scheduler 3054,81154
void checkheaps 3083,81865
void checkheaps_for_scheduler 3105,82325
void mempool_set_check_timer(3122,82739
static process checkheaps_process 3137,83126
void init_checkheaps 3175,84377
int get_available_buf_mem 3191,84894
ulong mempool_get_largest_block 3201,85129
void setstring 3264,86658
void setstring_named 3293,87498
