
sched.h,3458
#define __SCHED_H__143,5606
#define	HOTSWAP_SCHED_INTERVAL	145,5627
#define SCHED_VOLUNTARY_TASK_TIME 152,5849
#define TEXTBASE_STRING 160,6100
typedef enum ENABLESTATE_ ENABLESTATE_162,6176
} ENABLESTATE;ENABLESTATE165,6236
typedef enum ONESHOT_ ONESHOT_167,6252
} ONESHOTTYPE;ONESHOTTYPE170,6312
typedef enum SYSTEM_MSGS_ SYSTEM_MSGS_172,6328
} SYSTEM_MSGS;SYSTEM_MSGS177,6452
typedef enum major_event_type_ major_event_type_194,7161
} major_event_type;major_event_type205,7531
typedef enum minor_event_type_ minor_event_type_207,7552
} minor_event_type;minor_event_type213,7693
enum process_info_types process_info_types228,8115
    PI_ANALYZE,229,8141
    PI_ARG,230,8157
    PI_CRASHBLOCK,231,8169
    PI_NAME,232,8188
    PI_PID,233,8201
    PI_PRIORITY,234,8213
    PI_PROFILE,235,8230
    PI_RUNTIME,236,8246
    PI_STACKSIZE,237,8262
    PI_STARTTIME,238,8280
    PI_TIMING_INFO,239,8298
    PI_TTYSOC,240,8318
    PI_TTYNUM,241,8333
    PI_WAKEUP_REASONS242,8348
#define THIS_PROCESS	248,8400
#define NO_PROCESS	249,8459
typedef enum process_priority_t_ process_priority_t_254,8548
} process_priority_t;process_priority_t260,8862
#define PROC_DEBUG_EXECUTION	265,8923
#define PROC_DEBUG_WAKEUP	266,8957
#define PROC_DEBUG_WAKEUP_DELAY	267,8988
#define PROC_DEBUG_WATCH	268,9025
typedef void (*kernel_sched_loop_hook_t)kernel_sched_loop_hook_t283,9419
typedef void (*kernel_sched_loop_hook_t)(boolean run_norm)run_norm283,9419
} reload_type_t;reload_type_t319,10590
#define SUPPORT_OLD_SCHEDULER400,14929
static inline void process_safe_suspend 423,15554
static inline void process_safe_wait_for_event 430,15755
static inline void process_safe_sleep_for 437,15905
static inline void process_safe_sleep_until 444,16057
static inline void process_safe_sleep_on_timer 451,16220
static inline void process_safe_sleep_periodic 458,16387
static inline boolean process_get_analyze 477,16939
static inline boolean process_get_arg_num 482,17066
static inline boolean process_get_arg_ptr 487,17183
static inline boolean process_get_crashblock 492,17300
static inline boolean process_get_name 497,17430
static inline boolean process_get_pid 502,17555
static inline boolean process_get_priority 507,17668
static inline boolean process_get_profile 512,17795
static inline boolean process_get_runtime 517,17922
static inline boolean process_get_stacksize 522,18047
static inline boolean process_get_starttime 527,18186
static inline boolean process_get_ttynum 532,18323
static inline boolean process_get_ttysoc 537,18452
static inline boolean process_get_wakeup_reasons 542,18587
static inline boolean process_is_high_priority 547,18726
static inline boolean process_set_analyze 559,18929
static inline boolean process_set_arg_num 564,19067
static inline boolean process_set_arg_ptr 569,19185
static inline boolean process_set_crashblock 574,19303
static inline boolean process_set_name 581,19472
static inline boolean process_set_priority 586,19588
static inline boolean process_set_profile 591,19718
static inline boolean process_set_timing 596,19850
static inline boolean process_set_ttynum 601,20003
static inline boolean process_set_ttysoc 606,20127
static inline boolean process_set_wakeup_reasons 611,20249
static inline boolean debugger_active(629,20643
static inline boolean process_time_exceeded 644,21045
static inline boolean process_suspend_if_req 657,21331
