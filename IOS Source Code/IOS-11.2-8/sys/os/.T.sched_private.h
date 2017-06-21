
sched_private.h,1966
#define __SCHED_PRIVATE_H__137,5785
#define BACKWARD_COMPATIBLE	139,5814
#define RANDOMIZE_FREQUENCY	141,5879
#define MIN_SCHED_INTERVAL	143,5953
#define WATCHDOG_TIME_SHIFT	145,6018
#define WATCHDOG_FATAL_TIME 150,6228
#define	PROC_MAGIC	154,6402
#define LOAD_SCALAR 156,6433
#define MESSAGES_PER_CHUNK 158,6496
#define SCHED_THRASH_THRESHHOLD 169,6965
typedef enum sched_state_ sched_state_175,7127
} sched_state;sched_state190,7984
struct message_type_ message_type_203,8427
enum sched_timer_types_ sched_timer_types_281,11653
    STT_PROCESS_MANAGED,282,11679
    STT_PROCESS_SIMPLE,283,11746
    STT_PROCESS_TEMP,284,11812
    STT_FORCE	285,11879
#define ST_FORCE_TIME 297,12340
typedef struct watched_item_ watched_item_317,13129
} watched_item;watched_item324,13407
struct watched_boolean_ watched_boolean_334,13705
static inline boolean wb_value_inline 339,13775
#define WB_VALUE(WB_VALUE343,13862
struct watched_bitfield_ watched_bitfield_353,14124
struct watched_queue_ watched_queue_365,14329
static inline queuetype *wq_queue_inline 371,14413
static inline int wq_queuesize_inline 375,14498
static inline boolean wq_queuefull_inline 379,14585
static inline boolean wq_queuefull_reserve_inline 383,14719
static inline boolean wq_queueempty_inline 387,14886
#define WQ_QUEUE(WQ_QUEUE391,14986
#define WQ_QUEUESIZE(WQ_QUEUESIZE392,15028
#define	WQ_QUEUEFULL(WQ_QUEUEFULL393,15077
#define	WQ_QUEUEFULL_RESERVE(WQ_QUEUEFULL_RESERVE394,15126
#define WQ_QUEUEEMPTY(WQ_QUEUEEMPTY396,15215
struct watched_semaphore_ watched_semaphore_405,15417
typedef struct watcher_info_ watcher_info_417,15801
} watcher_info;watcher_info425,16257
typedef void (*new_exec_point_t)new_exec_point_t428,16275
struct sched_event_set_ sched_event_set_430,16316
#define SCHED_EVENT_MAGIC 441,16837
struct sprocess_ sprocess_447,17001
#define MAX_PROC_PER_ARRAY 534,21105
struct sprocess_list_ sprocess_list_536,21166
