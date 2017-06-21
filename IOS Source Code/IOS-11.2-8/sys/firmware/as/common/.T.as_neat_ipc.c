
as_neat_ipc.c,1223
typedef struct neat_ipc_msg_handle_ neat_ipc_msg_handle_49,1481
} neat_ipc_msg_handle;neat_ipc_msg_handle52,1587
} trace_type;trace_type64,1805
void trace_log(70,1951
typedef struct  proc_level_msg proc_level_msg87,2317
} proc_level_msg_t;proc_level_msg_t92,2527
typedef struct proc_msg_queue proc_msg_queue94,2548
} proc_msg_queue_t;proc_msg_queue_t97,2633
#define MAX_PROC_LEVEL_MSGS	99,2654
static void local_msg_enq 110,2952
static proc_level_msg_t *local_msg_deq 129,3425
static proc_level_msg_t *localmsg_getbuf 154,3901
static void localmsg_freebuf 164,4083
static void localmsg_init 174,4252
static void msg_enq(191,4687
static neat_ipc_message *msg_deq(215,5209
static void msg_que_merge 243,5799
static  neat_ipc_message *msg_scan 269,6446
void neat_ipc_msg_mgmt_init 296,7774
neat_ipc_message *msg_getbuf 333,8995
void msg_putbuf 343,9150
void msg_send 355,9501
void msg_queue_check 415,11685
neat_ipc_message *msg_get 458,12988
static void neat_ipc_null_function 490,13769
void neat_ipc_msg_process 500,13929
static void neat_ipc_send_to_proc 520,14352
void neat_ipc_message_register 566,15622
neat_ipc_msg_ret neat_ipc_message_send 587,16208
void neat_ipc_msgrcv 632,17324
