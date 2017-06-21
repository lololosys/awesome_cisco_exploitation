
as_ipc_section.h,3833
#define IPC_SECTIONH68,2491
#define SRAM_PAGESIZE	70,2513
#define SRAM_PAGEMASK	71,2575
#define MAX_DIGITS 73,2621
#define MAX_T1	74,2648
#define MAX_DSX1	75,2666
#define FW_VERSION_EXT 76,2685
#define NUM_STACK_FRAMES 78,2715
#define H_BUSERR 79,2742
typedef volatile uchar vuchar;vuchar81,2772
typedef volatile ushort vushort;vushort82,2803
typedef ushort LineStatus;LineStatus83,2836
typedef struct stack_trace_info stack_trace_info85,2864
} stack_trace_info_t;stack_trace_info_t88,2987
typedef struct excpt_stack_frame excpt_stack_frame90,3010
} excpt_stack_frame;excpt_stack_frame154,6845
typedef struct cpu_context_t cpu_context_t156,6867
} cpu_context_t;cpu_context_t160,7011
typedef struct neat2mpu_mail_type neat2mpu_mail_type166,7092
} neat2mpu_mail_type;neat2mpu_mail_type176,7386
typedef struct carrier_mail_type carrier_mail_type178,7409
} carrier_mail_type;carrier_mail_type182,7535
typedef struct dtmfmail_from_mpu dtmfmail_from_mpu188,7645
} dtmfmail_from_mpu;dtmfmail_from_mpu193,7948
typedef struct dtmfmail_to_mpu dtmfmail_to_mpu201,8198
} dtmfmail_to_mpu;dtmfmail_to_mpu206,8486
#define MSG_DATA_SIZE 208,8506
typedef struct _neat_ipc_message _neat_ipc_message214,8706
} neat_ipc_message;neat_ipc_message222,9199
#define IPCMSGFLAG_DATA_EXT 226,9261
 MSG_NEAT_NULL 233,9434
 MSG_NEAT_MIB,234,9477
 MSG_NEAT_BUGINFO,235,9530
 MSG_NEAT_ERROR_REPORT,236,9581
 MSG_NEAT_RX_RBS_DATA,237,9636
 MSG_NEAT_TX_RBS_DATA,238,9704
 MSG_NEAT_RBS2CSM_HI,239,9772
 MSG_NEAT_RBS2CSM_LO,240,9851
 MSG_NEAT_MAX 241,9930
 MSG_MPU_NULL	246,10045
 MSG_MPU_SET_CT1_CHANNELS,247,10090
 MSG_MPU_SHOW_RBS,248,10154
 MSG_MPU_CSM2RBS,249,10213
 MSG_MPU_MAX	250,10278
#define MAX_DSX1_CHANNELS	254,10312
} msg_ct1_channels;msg_ct1_channels260,10473
} msg_show_rbs_resp;msg_show_rbs_resp266,10579
typedef struct _ipc_msg_queue _ipc_msg_queue268,10601
} ipc_msg_queue;ipc_msg_queue278,11083
#define IPCQ_OWNER_NONE	280,11101
#define IPCQ_OWNER_NEAT	281,11155
#define IPCQ_OWNER_CORE	282,11220
#define WHO_NEAT	284,11276
#define WHO_CORE	285,11301
#define WHO_INIT	286,11326
#define WHO_CHECK	287,11351
#define WHO_MSG_SEND	288,11377
#define WHO_GET	289,11406
#define IPC_MSG_SIZE 291,11432
#define TOTAL_MSG_BUFFERS 297,11631
typedef struct _ipc_section _ipc_section300,11720
} ipc_section;ipc_section318,12808
#define DIALTONE_ADDR	323,12868
#define BUSYTONE_ADDR	324,12909
#define IPC_MSG_BUFF_ADDR	325,12950
#define CARRIER_TO_MPU(CARRIER_TO_MPU326,12995
#define MAIL_FROM_MPU(MAIL_FROM_MPU327,13067
#define MSG_FROM_MPU	328,13135
#define MSG_TO_MPU	329,13180
#define FIRMWARE_VER_ADDR	330,13221
#define MPU_RELOAD_DONE_ADDR	331,13274
#define FW_VERSION 333,13325
#define MPU_RELOAD_DONE 334,13372
#define MAIL_TO_MPU(MAIL_TO_MPU336,13417
#define DTMF_TO_MPU(DTMF_TO_MPU337,13476
#define DTMF_FROM_MPU(DTMF_FROM_MPU338,13538
#define IPC_AREA 344,13680
#define IPC_MSG_AREA 346,13800
#define DYNAMIC_MEM_AREA 349,13929
} neat_ipc_msg_priority;neat_ipc_msg_priority355,14079
} neat_ipc_msg_ret;neat_ipc_msg_ret362,14213
typedef void (*neat_ipc_msg_function_t)neat_ipc_msg_function_t365,14267
typedef struct csm_message_t_ csm_message_t_367,14317
} csm_message_t;csm_message_t384,15135
#define TRACE(TRACE394,15332
#define TRACE(TRACE396,15416
#define MAX_IPC_TRACE_SIZE	399,15463
#define T_BEGIN 401,15522
#define T_END	402,15542
#define IPC_MSGRCV 404,15564
#define MSG_QUEUE_CHECK 405,15607
#define IPC_SEND_TO_PROC 406,15641
#define IPC_MSG_PROC 407,15675
#define IPC_MSG_GET 408,15709
#define IPC_QUE_MRG 409,15743
#define IPC_MSG_DEQ 410,15777
#define IPC_MSG_ENQ 411,15811
#define LCL_MSG_DEQ 412,15845
#define LCL_MSG_ENQ 413,15879
#define IPC_MSG_SEND 414,15916
#define IPC_MESSAGE_SEND 415,15953
