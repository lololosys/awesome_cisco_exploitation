
buffers.c,2213
#define DEBUGCMD(DEBUGCMD234,9047
#define DEBUGCMD(DEBUGCMD236,9075
void report_getbuffer_failure 303,11254
void pak_pool_init 332,12040
pooltype *pak_pool_create 396,13715
static void *pak_pool_item_create 467,15006
static void pak_pool_item_destroy 539,16327
static void *pak_pool_item_get 589,17296
static void pak_pool_item_ret 646,18456
static void pak_pool_item_status 691,19378
boolean pak_pool_create_cache 711,19805
static void *pak_pool_cache_item_get 727,20097
static void pak_pool_cache_item_ret 739,20279
void pak_pool_periodic 751,20452
static inline paktype *getbuffer_inline 778,20867
paktype *getbuffer 871,23100
paktype *pool_getbuffer 902,23629
paktype *input_getbuffer 919,24129
static inline void retbuffer_inline 978,25550
static inline void kill_fragment_chain 1038,26933
void retbuffer 1055,27278
void datagram_done 1076,27633
void discard_buffer 1139,29153
static inline void pak_copy_data_inline 1170,29642
static inline int pak_net_offset 1206,30534
static inline uchar *pak_adjust_pointer 1228,30948
static void pak_get_particle_offset 1240,31150
static inline void pak_get_paktype_offset 1271,31729
static inline void pak_get_offset_table_inline 1287,32129
static void pak_set_particle_offset 1346,34171
static void pak_set_paktype_offset 1378,34766
static inline void pak_set_offset_table_inline 1395,35190
static void pak_copy_header 1457,37427
static void pak_copy_contiguous_to_contiguous 1516,39400
void pak_copy_scatter_to_contiguous 1622,42790
void pak_copy 1742,46169
void pak_copy_and_recenter 1763,46552
paktype *pak_duplicate 1809,47614
boolean pak_realign 1849,48753
paktype *pak_clip 1910,50302
paktype *pak_grow 1938,50947
paktype *pak_dup_grow_realign 1968,51582
paktype *pak_grow_realign 2011,52643
paktype *pak_coalesce 2051,53587
paktype *pak_clone 2100,54922
int pak_get_pool_size 2146,55999
int pak_pool_maximum_size 2165,56332
pooltype *pak_pool_find_by_size 2181,56587
pooltype *pak_pool_find_by_name 2203,56959
void clear_if_input 2223,57357
void set_if_input 2235,57570
boolean change_if_input 2248,57884
#define MAX_APPN_POOL_SIZE 2282,58795
void appn_init_pool 2283,58827
boolean pak_encap 2302,59206
