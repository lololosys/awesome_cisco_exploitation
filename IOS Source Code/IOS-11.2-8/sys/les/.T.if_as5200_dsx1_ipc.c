
if_as5200_dsx1_ipc.c,1710
typedef struct t1_msg_handle_ t1_msg_handle_82,2715
} t1_msg_handle;t1_msg_handle85,2814
typedef struct  proc_level_msg proc_level_msg92,2963
} proc_level_msg_t;proc_level_msg_t98,3208
typedef struct proc_msg_queue proc_msg_queue100,3229
} proc_msg_queue_t;proc_msg_queue_t103,3314
#define MAX_PROC_LEVEL_MSGS	105,3335
ushort dsx1_set_neat_page_reg 122,3830
static void local_msg_enq 149,4458
static proc_level_msg_t *local_msg_deq 169,4878
static proc_level_msg_t *localmsg_getbuf 195,5314
static void localmsg_freebuf 205,5496
static void localmsg_init 215,5665
static boolean neat_msgbuf_lock 243,6415
static void neat_msgbuf_release 283,7517
static boolean neat_msgbuf_locked 305,8072
ushort *t1_addr_n2c 344,8950
ulong t1_addr_c2n 369,9539
static void msg_enq 385,9925
static void msg_enq_common 408,10580
static neat_ipc_message *msg_deq 432,11341
static neat_ipc_message *msg_deq_common 455,11912
static void msg_que_merge_cp 480,12585
static void msg_que_merge_pc 507,13292
static  neat_ipc_message *msg_scan 534,14135
void neat_ipc_init 568,15679
process neat_ipc_reset 622,17420
static neat_ipc_message *msg_getbuf 680,19096
static void msg_putbuf 691,19294
static void msg_send 705,19701
void msg_queue_check 752,21538
static neat_ipc_message *msg_get 784,22417
static void msg_parser_high 819,23181
static void t1_null_function 863,24239
static void t1_errmsg_report 872,24394
static void t1_buginfo 883,24601
static void msg_parser_low 892,24770
static void msg_process 909,25097
static void msg_send_to_process 941,25712
void neat_ipc_message_register 991,27045
neat_ipc_msg_ret neat_ipc_message_send 1006,27474
void neat_msgrcv_process 1053,28572
