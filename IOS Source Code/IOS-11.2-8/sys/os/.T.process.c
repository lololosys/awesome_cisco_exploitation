
process.c,4139
#define STATIC 268,10484
#define INLINE 269,10506
char *event_type2text 286,10977
static char *event_type2name 300,11370
void process_run_add 318,11854
/* inline */ sprocess *process_pid2ptr 338,12396
boolean process_exists 362,12946
boolean process_is_ok 378,13213
set_wi_minor_inline 404,13867
void process_set_bitfield_minor 409,13952
void process_set_boolean_minor 414,14072
void process_set_semaphore_minor 419,14190
void process_set_queue_minor 424,14312
set_wi_first_only_inline 443,14839
void process_set_bitfield_first_only 448,14946
void process_set_boolean_first_only 453,15088
void process_set_semaphore_first_only 458,15228
void process_set_queue_first_only 463,15372
STATIC INLINE void process_suspend_internal 484,16033
void process_suspend 508,16819
STATIC INLINE boolean process_would_suspend_inline 536,17386
boolean process_would_suspend 555,17813
STATIC INLINE boolean process_may_suspend_inline 560,17899
boolean process_may_suspend 574,18187
STATIC INLINE boolean process_setup_sleep_inline 591,18580
STATIC INLINE void process_do_sleep_inline 615,19088
void process_sleep_until 630,19551
void process_sleep_for 656,20402
void process_sleep_periodic 683,21218
void process_sleep_on_timer 715,22341
typedef enum process_ev_result_ process_ev_result_738,22934
} process_ev_result;process_ev_result743,23059
static inline process_ev_result process_check_thrashing 745,23081
static process_ev_result process_events_waiting_p 763,23732
STATIC INLINE void process_idle_internal 883,27573
void process_wait_for_event 966,30125
void process_wait_for_event_timed 991,30929
void process_hang 1010,31327
void process_wakeup_internal 1037,31911
void process_wakeup_w_reason 1141,35408
void process_wakeup 1170,36187
static boolean process_add_wakeup 1189,36664
static void process_remove_wakeup 1251,38882
STATIC INLINE void process_watch_common 1285,39916
void watcher_initialize_common 1301,40236
static void *watcher_create_common 1327,41100
static boolean watcher_delete_common 1351,41672
static void process_notify_event 1392,42852
boolean process_caller_has_events 1445,44361
sched_event_set *process_malloc_event_set 1474,45038
boolean process_push_event_list 1492,45523
boolean process_pop_event_list 1548,47149
watched_queue *create_watched_queue 1620,49256
void delete_watched_queue 1639,49829
void process_watch_queue 1678,50775
boolean process_enqueue 1697,51222
boolean process_requeue 1725,51960
void *process_dequeue 1754,52720
boolean process_is_queue_empty 1770,53094
boolean process_is_queue_full 1784,53399
void process_resize_queue 1800,53830
void *process_queue_peek 1843,54873
int process_queue_size 1856,55158
int process_queue_max_size 1869,55449
void process_watch_mgd_timer 1892,56121
void process_watch_timer 1923,56919
watched_boolean *create_watched_boolean 1960,57784
void delete_watched_boolean 1975,58129
void process_watch_boolean 2005,58820
boolean process_get_boolean 2023,59275
void process_set_boolean 2034,59512
void process_wait_on_system_init 2054,59997
void process_wait_on_system_config 2089,60946
watched_bitfield *create_watched_bitfield 2131,62115
void delete_watched_bitfield 2146,62468
void process_watch_bitfield 2176,63173
ulong process_get_bitfield 2194,63623
void process_set_bitfield 2205,63892
void process_clear_bitfield 2225,64355
void process_keep_bitfield 2245,64821
watched_semaphore *create_watched_semaphore 2272,65486
void delete_watched_semaphore 2289,65890
void process_watch_semaphore 2319,66587
static boolean lock_watched_semaphore 2338,67110
boolean process_lock_semaphore 2358,67604
void process_unlock_semaphore 2408,68830
boolean process_get_message 2458,70419
boolean process_send_message 2481,71134
void process_initialize_messages 2514,71872
static sprocess *process_getset_lookup 2536,72394
boolean process_get_info 2577,73339
boolean process_set_info 2722,76658
void process_set_all_profiles 2833,79380
boolean process_get_wakeup 2865,80308
void process_set_debug 2923,82015
process critical_background 2942,82346
void critical_background_wake 2967,82972
