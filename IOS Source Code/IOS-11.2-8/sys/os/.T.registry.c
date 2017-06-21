
registry.c,1985
#define REGISTRY_FUNCTION_CHUNK_MAX 137,4612
static boolean registry_default_routine 155,4941
static void registry_increment_statistics 166,5142
static void registry_decrement_statistics 180,5424
void registry_init 192,5647
static inline regservicetype *service_lookup_inline 237,6711
static regservicetype *service_lookup 276,7660
regtype_t registry_create 293,8112
void service_create 348,9500
static void registry_function_enqueue 465,12780
static void registry_function_unqueue 498,13401
boolean registry_add_case 532,13958
boolean registry_add_list 597,15630
boolean registry_add_default 643,16677
boolean registry_add_stub 694,17825
boolean registry_add_value 745,18974
boolean registry_add_default_value 800,20370
void registry_delete_case 850,21525
void registry_delete_default 878,22333
void registry_delete_list 900,22885
void registry_delete_stub 936,23620
boolean registry_add_pid_list 968,24738
void registry_delete_pid_list 1011,25779
boolean registry_add_pid_case 1058,26948
void registry_delete_pid_case 1115,28511
typedef long (*reg_t0)reg_t01144,29590
typedef long (*reg_t1)reg_t11145,29620
typedef long (*reg_t2)reg_t21146,29652
typedef long (*reg_t3)reg_t31147,29691
typedef long (*reg_t4)reg_t41148,29737
typedef long (*reg_t5)reg_t51149,29790
typedef long (*reg_t6)reg_t61150,29850
typedef long (*reg_t7)reg_t71151,29917
typedef long (*reg_t8)reg_t81152,29991
typedef long (*reg_t9)reg_t91154,30082
typedef long (*reg_t10)reg_t101156,30180
static inline long function_execute_inline 1160,30282
static long function_execute 1272,33627
void registry_case 1288,34113
void registry_list 1338,35310
long registry_retval 1365,36074
ulong registry_value 1418,37434
boolean registry_case_used 1463,38449
boolean registry_loop 1501,39411
long registry_stub 1528,40078
void registry_pid_list 1561,41174
void registry_pid_case 1608,42421
boolean registry_allocated 1674,44069
boolean registry_populated 1686,44363
