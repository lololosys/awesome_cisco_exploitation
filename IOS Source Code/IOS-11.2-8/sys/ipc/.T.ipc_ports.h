
ipc_ports.h,1322
#define __IPC_PORTS_H__31,1008
typedef enum ipc_port_type ipc_port_type36,1108
} ipc_port_type;ipc_port_type39,1187
typedef enum ipc_recieve_method ipc_recieve_method41,1205
} ipc_receive_method;ipc_receive_method45,1296
#define IPC_PORT_NAME_MAX 48,1320
#define IPC_PORT_NAME_SEPARATOR 49,1382
struct ipc_port_data_ ipc_port_data_55,1499
typedef struct ipc_name_ ipc_name_74,1808
} ipc_name;ipc_name78,1899
#define IPC_CONTROL_PORT_NAME 84,1940
typedef struct ipc_register_name_request_ ipc_register_name_request_86,1981
} ipc_register_name_request;ipc_register_name_request90,2110
typedef struct ipc_register_name_response_ ipc_register_name_response_92,2140
} ipc_register_name_response;ipc_register_name_response95,2256
typedef struct ipc_deregister_name_request_ ipc_deregister_name_request_97,2287
} ipc_deregister_name_request;ipc_deregister_name_request100,2401
typedef struct ipc_lookup_name_request_ ipc_lookup_name_request_102,2433
} ipc_lookup_name_request;ipc_lookup_name_request105,2533
typedef struct ipc_lookup_name_response_ ipc_lookup_name_response_107,2561
} ipc_lookup_name_response;ipc_lookup_name_response109,2631
#define IPC_PORT_TABLE_SIZE 115,2682
#define IPC_NAME_TABLE_SIZE 116,2713
#define IPC_PORT_UNICAST 118,2745
#define IPC_PORT_MULTICAST 119,2772
