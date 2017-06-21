
novroute.h,2157
#define __NOVROUTE_H__66,2381
#define NOV_CONNECTED_DISTANCE 71,2465
#define NOV_STATIC_DISTANCE 72,2500
#define NOV_IGRP_DISTANCE 73,2535
#define NOV_RIP_DISTANCE 74,2570
#define NOV_NETMASK_ALL 75,2605
#define IPXRADIXDATAOFF 76,2647
#define IPXRADIXKEYLEN 77,2724
union novrdbunion novrdbunion79,2800
struct novrdbtype_ novrdbtype_95,3442
typedef struct novlsptype_ novlsptype_112,4289
} novlsptype;novlsptype115,4425
typedef struct rdb_list_ rdb_list_120,4500
} nov_rdb_list;nov_rdb_list123,4576
union novndbunion novndbunion130,4730
#define IPX_NDB_OPTION_STATIC_NO_NVGEN 140,5027
#define IPX_NDB_OPTION_VALID 141,5073
#define IPX_NDB_OPTION_DELETED 142,5109
#define IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP 143,5147
#define IPX_NDB_OPTION_INTERFACE_STATIC 144,5198
enum IPX_NDB_TYPE IPX_NDB_TYPE151,5337
    IPX_NDB_RESERVED 152,5357
    IPX_NDB_ACTIVE 153,5383
    IPX_NDB_PLACEHOLDER 154,5407
typedef struct novndbtype_ novndbtype_160,5591
} novndbtype;novndbtype207,7808
static inline int ipx_count_route_paths 209,7823
static inline int ipx_count_server_paths 217,8014
typedef novndbtype *nov_routetable[nov_routetable226,8225
typedef novndbtype *nov_statictable[nov_statictable227,8277
typedef struct nov_svc_type_ nov_svc_type_233,8461
} nov_svc_type;nov_svc_type252,8969
typedef nov_svc_type *nov_backup_saptable[nov_backup_saptable254,8986
typedef struct nov_urd_type_ nov_urd_type_260,9150
} nov_urd_type;nov_urd_type264,9358
typedef nov_urd_type *nov_urd_table_type[nov_urd_table_type266,9375
struct novpdbtype_ novpdbtype_268,9434
typedef struct novroutetype_ novroutetype_429,16784
} novroutetype;novroutetype441,17499
typedef novroutetype *nov_backup_routetable[nov_backup_routetable443,17516
typedef struct nov_spawntype_ nov_spawntype_450,17787
} nov_spawntype;nov_spawntype455,17914
typedef struct nov_sumroutetype_ nov_sumroutetype_457,17932
} nov_sumroutetype;nov_sumroutetype460,18071
typedef struct ipxradixkeytype_ ipxradixkeytype_462,18092
} ipxradixkeytype;ipxradixkeytype466,18179
ipxaddr2radixkey 479,18509
static inline boolean ipx_route_valid 491,18748
