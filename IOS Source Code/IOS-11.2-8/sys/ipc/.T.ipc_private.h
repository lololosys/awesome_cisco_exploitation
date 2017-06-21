
ipc_private.h,1859
#define __IPC_PRIVATE_H__68,2482
typedef struct ipc_global_info_ ipc_global_info_74,2549
} ipc_global_info;ipc_global_info143,5777
#define IPC_MAX_SEQUENCE 152,5890
#define IPC_GRACE_SEQUENCE 153,5922
typedef enum ipc_sequence_report_ ipc_sequence_report_159,6004
} ipc_sequence_report;ipc_sequence_report163,6098
#define IPC_RETRY_MAX 169,6146
#define IPC_RETRY_PERIOD 170,6170
#define IPC_MAINTENANCE_PERIOD 171,6206
#define IPC_RPC_DEFAULT_PERIOD 172,6249
#define IPC_TYPE_SERVER_ECHO 179,6383
#define IPC_TYPE_REGISTER_NAME_REQUEST 180,6414
#define IPC_TYPE_REGISTER_NAME_RESPONSE 181,6455
#define IPC_TYPE_DEREGISTER_NAME_REQUEST 182,6497
#define IPC_TYPE_LOOKUP_NAME_REQUEST 183,6540
#define IPC_TYPE_IP_CACHE 189,6651
#define IPC_TYPE_IP_STATS 190,6719
#define IPC_TYPE_IP_DFSCFG 191,6785
#define IPC_TYPE_IP_ACLCFG 192,6859
#define IPC_TYPE_CRYPTO 193,6933
#define IPC_BOOTSTRAP_REGISTER_REQUEST_V1 200,7044
#define IPC_BOOTSTRAP_REGISTER_RESPONSE_V1 201,7088
typedef struct ipc_boot_request_v1 ipc_boot_request_v1203,7134
} ipc_boot_request_v1;ipc_boot_request_v1208,7294
typedef struct ipc_boot_response_v1 ipc_boot_response_v1210,7318
} ipc_boot_response_v1;ipc_boot_response_v1214,7452
typedef struct ipc_callback_arg_ ipc_callback_arg_220,7548
} ipc_callback_arg_t;ipc_callback_arg_t224,7655
typedef enum ipc_private_msg_type_ ipc_private_msg_type_230,7754
} ipc_private_msg_type;ipc_private_msg_type232,7815
typedef struct ipc_private_msg_ ipc_private_msg_234,7840
} ipc_private_msg;ipc_private_msg237,7927
typedef struct ipc_slot_init_msg_ ipc_slot_init_msg_239,7947
} ipc_slot_init_msg;ipc_slot_init_msg242,8031
#define IPC_SLOT_STR 247,8092
#define IPC_SLOT_STR_SIZE 248,8128
#define IPC_STARTUP_MESSAGE 249,8184
#define IPC_TERMINATOR_MESSAGE 250,8230
#define IPC_OIR_MESSAGE 251,8278
