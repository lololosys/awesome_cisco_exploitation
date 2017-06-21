
ipc.h,2271
#define __IPC_H__61,2171
#define IPC_REALM_MANAGER_SEAT 63,2190
#define IPC_REALM_MANAGER_NAME 64,2232
#define IPC_MASTER_SEAT 65,2283
#define IPC_MASTER_NAME 66,2319
#define ipcheadstart(ipcheadstart73,2478
#define ipcdatastart(ipcdatastart74,2527
typedef struct ipc_message_header_ ipc_message_header_76,2609
} ipc_message_header;ipc_message_header84,2815
#define IPC_MESSAGE_ENCAPS_SIZE 86,2838
#define IPC_MESSAGE_MAX_MTU 87,2873
#define IPC_MAX_SEAT_NAME	88,2907
#define IPC_CLASS_TYPE_MASK	90,2937
#define IPC_CLASS_A_SEAT_MASK 91,2976
#define IPC_CLASS_A_PORT_MASK 92,3018
#define IPC_CLASS_B_SEAT_MASK 93,3056
#define IPC_CLASS_B_PORT_MASK 94,3098
#define IPC_SEAT_INCREMENT	95,3136
#define IPC_PERMANENT_PORT	97,3175
#define is_ipcclassA(is_ipcclassA99,3244
#define is_ipcclassB(is_ipcclassB100,3312
#define IPC_PORT_IS_LOCAL 102,3388
#define IPC_PORT_OOB	103,3449
#define IPC_PORT_IB	104,3503
#define IPC_FLAG_BOOTSTRAP 106,3553
#define IPC_FLAG_ACK 107,3625
#define IPC_FLAG_IS_RPC 108,3692
#define IPC_FLAG_IS_RPC_REPLY 109,3762
#define IPC_RX_NAME_0	111,3829
#define IPC_RX_NAME_1	112,3865
#define IPC_RX_NAME_2	113,3901
#define IPC_RX_NAME_3	114,3937
#define IPC_RX_NAME_4	115,3973
#define IPC_TX_NAME_0	117,4010
#define IPC_TX_NAME_1	118,4046
#define IPC_TX_NAME_2	119,4082
#define IPC_TX_NAME_3	120,4118
#define IPC_TX_NAME_4	121,4154
#define IPC_FLAG_IS_FRAGMENT 125,4233
#define IPC_FLAG_LAST_FRAGMENT 126,4271
#define IPC_FLAG_DO_NOT_ACK 127,4309
#define IPC_FRAG_PAGE_MASK 128,4377
struct ipc_message_ ipc_message_140,4853
#define IPC_FLAG_BLOCKED 163,5666
#define IPC_FLAG_GOT_RPC_ACK 164,5743
#define IPC_FLAG_IN_CACHE 166,5814
typedef void (*ipc_permanent_port_process_vector)ipc_permanent_port_process_vector173,6036
typedef void (*ipc_permanent_port_process_vector)(paktype *pak)pak173,6036
typedef void (*ipc_delayed_func_vector)ipc_delayed_func_vector178,6173
typedef void (*ipc_init_slaves_vector)ipc_init_slaves_vector179,6220
typedef void (*ipc_init_slaves_vector)(ulong)ulong179,6220
} ipc_level_t;ipc_level_t189,6472
static inline ipc_seat ipc_portid_to_seat 299,9725
static inline ipc_port ipc_portid_to_port 307,9964
static inline ipc_port_id ipc_make_portid 316,10213
