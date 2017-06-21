
if_pas.h,2224
#define PAS_DEBUG(PAS_DEBUG170,5714
#define PAS_DEBUG_BOOL 171,5737
#define PAS_DEBUG_FLAG172,5766
#define PAS_DEBUG(PAS_DEBUG176,5824
#define PAS_DEBUG_BOOL 177,5845
#define INSTANCE(INSTANCE205,6386
typedef struct pas_management_t_ pas_management_t_210,6458
} pas_management_t;pas_management_t242,7067
typedef int pas_eeprom_type;pas_eeprom_type244,7088
typedef int pas_eeprom_cmd_t;pas_eeprom_cmd_t245,7143
#define PAS_EEPROM_X2444	250,7247
#define PAS_EEPROM_NMC93C46	251,7275
#define PAS_EEPROM_CMD_READ	256,7350
#define PAS_EEPROM_CMD_WRITE	257,7381
#define PAS_EEPROM_CMD_ERASE	258,7413
#define PAS_EEPROM_CMD_ZERO	259,7445
#define PAS_EEPROM_DELAY	264,7529
#define PAS_X2444_DELAY	265,7570
#define PAS_NMC93C46_WRITE_DELAY 266,7615
#define NMC93C46_CMD_CONTROL 270,7749
#define NMC93C46_CMD_WRDS 271,7794
#define NMC93C46_CMD_ERASE_ALL 272,7846
#define NMC93C46_CMD_WRITE_ALL 273,7898
#define NMC93C46_CMD_WREN 274,7950
#define NMC93C46_CMD_READ 275,8002
#define NMC93C46_CMD_WRITE 276,8047
#define NMC93C46_CMD_ERASE 277,8092
typedef void 		(*pas_coalesce_v)pas_coalesce_v287,8293
typedef void 		(*pas_coalesce_v)(hwidbtype *, paktype *, iqueue_t)iqueue_t287,8293
typedef void		(*pas_deallocate_t)pas_deallocate_t292,8407
typedef void		(*pas_deallocate_t)(hwidbtype hwidbtype292,8407
typedef boolean 	(*pak_to_host_t)pak_to_host_t294,8456
typedef boolean 	(*pak_to_host_t)(hwidbtype *, pas_cbtype *, paktype paktype294,8456
typedef boolean		(*pak_switch_t)pak_switch_t295,8529
					volatile ulong ulong297,8604
typedef boolean 	(*pak_dfs_t)pak_dfs_t298,8628
typedef boolean 	(*pak_dfs_t)(hwidbtype *, paktype paktype298,8628
struct pas_cbtype_ pas_cbtype_303,8720
typedef int (pa_wrapper_t)pa_wrapper_t346,10079
typedef void (pa_nmi_wrapper_t)pa_nmi_wrapper_t347,10113
typedef struct pa_nmi_handler_struc pa_nmi_handler_struc352,10199
} pa_nmi_handler_t;pa_nmi_handler_t357,10315
typedef struct pa_jumptable_t_ pa_jumptable_t_363,10482
} pa_jumptable_t;pa_jumptable_t370,10903
#define MAX_PCI_DEVICES	382,11117
#define IDB_FIRST_INIT	389,11216
#define IDB_FINAL_INIT	390,11283
#define pas_stuff_pak_for_holdq(pas_stuff_pak_for_holdq403,11587
