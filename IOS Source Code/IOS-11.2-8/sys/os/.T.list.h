
list.h,2576
#define __LIST_H__40,1275
struct list_element_ list_element_51,1379
typedef int (*list_insert_func_t)list_insert_func_t58,1506
typedef int (*list_lookup_func_t)list_lookup_func_t59,1557
typedef boolean (*list_enqueue_t)list_enqueue_t61,1609
				  list_element list_element62,1658
typedef void * (*list_dequeue_t)list_dequeue_t64,1695
typedef void * (*list_dequeue_t)(list_header list_header64,1695
typedef boolean (*list_requeue_t)list_requeue_t65,1744
                                  list_element list_element66,1793
typedef boolean (*list_insert_t)list_insert_t68,1886
				 list_insert_func_t func)func71,1968
typedef void * (*list_remove_t)list_remove_t72,2000
				list_element list_element73,2047
typedef boolean (*list_info_t)list_info_t75,2081
			       list_element list_element76,2127
typedef void * (*list_lookup_t)list_lookup_t78,2173
				 list_lookup_func_t func)func81,2254
typedef struct list_action_ list_action_83,2287
} list_action_t;list_action_t90,2506
struct list_header_ list_header_92,2524
#define LIST_MAX_INFO 116,3003
#define LIST_FLAGS_VALID 121,3064
#define LIST_FLAGS_MALLOC 122,3105
#define LIST_FLAGS_AUTOMATIC 123,3146
#define LIST_FLAGS_INTERRUPT_SAFE 124,3187
#define LIST_FLAGS_USER_MASK 126,3229
#define LIST_VALID(LIST_VALID132,3351
#define LIST_SIZE(LIST_SIZE133,3411
#define	LIST_FULL(LIST_FULL134,3452
#define LIST_EMPTY(LIST_EMPTY136,3568
#define LIST_NEXT_ELEMENT(LIST_NEXT_ELEMENT141,3660
#define LIST_PREV_ELEMENT(LIST_PREV_ELEMENT142,3732
#define LIST_HEAD_ELEMENT(LIST_HEAD_ELEMENT143,3804
#define LIST_TAIL_ELEMENT(LIST_TAIL_ELEMENT144,3852
#define LIST_ELEMENT_QUEUED(LIST_ELEMENT_QUEUED149,3933
#define FOR_ALL_ELEMENTS_IN_LIST(FOR_ALL_ELEMENTS_IN_LIST151,3997
#define LIST_GET_DATA(LIST_GET_DATA159,4250
#define LIST_SET_DATA(LIST_SET_DATA160,4318
#define LIST_NEXT_DATA(LIST_NEXT_DATA162,4366
#define LIST_HEAD_DATA(LIST_HEAD_DATA164,4480
#define LIST_TAIL_DATA(LIST_TAIL_DATA166,4585
#define ELEMENT_GET_LIST(ELEMENT_GET_LIST169,4691
#define FOR_ALL_DATA_IN_LIST(FOR_ALL_DATA_IN_LIST171,4744
#define FOR_ALL_DATA_IN_LIST_W_NEXT(FOR_ALL_DATA_IN_LIST_W_NEXT178,5077
#define FOR_ALL_DATA_IN_LIST_LOCKED(FOR_ALL_DATA_IN_LIST_LOCKED187,5540
static inline boolean list_enqueue 198,5953
static inline void *list_dequeue 205,6140
static inline boolean list_requeue 210,6241
static inline boolean list_insert 217,6427
static inline void *list_remove 225,6647
static inline boolean list_info 232,6820
static inline void *list_lookup 241,7031
