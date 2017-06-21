
snmp_interface.h,2551
#define __SNMP_INTERFACE_H__105,3574
enum SNMP_IF_DATA_TYPE SNMP_IF_DATA_TYPE120,3896
    SNMP_IF_NONE 121,3921
    SNMP_IF_NONE   = 0x00,121,3921
    SNMP_IF_HWIDB 122,3980
    SNMP_IF_HWIDB  = 0x01,122,3980
    SNMP_IF_SWIDB 123,4036
    SNMP_IF_SWIDB  = 0x02,123,4036
    SNMP_IF_TTY 124,4097
    SNMP_IF_TTY    = 0x04,124,4097
    SNMP_IF_CDB 125,4144
    SNMP_IF_CDB    = 0x08,125,4144
    SNMP_IF_IAB 126,4191
    SNMP_IF_IAB    = 0x10,126,4191
    SNMP_IF_SUBIAB 127,4257
    SNMP_IF_SUBIAB = 0x20	127,4257
struct snmpifptrtype_ snmpifptrtype_130,4327
struct snmpidbtype_ snmpidbtype_141,4696
typedef boolean (*iab_get_stats_t)iab_get_stats_t200,7379
typedef boolean (*iab_get_stats_t)(iabtype iabtype200,7379
typedef long (*iab_iftype_get_t)iab_iftype_get_t201,7426
typedef long (*iab_iftype_get_t)(iabtype iabtype201,7426
typedef boolean (*iab_hw_if_up_t)iab_hw_if_up_t202,7471
typedef boolean (*iab_hw_if_up_t)(iabtype iabtype202,7471
typedef ulong (*iab_rx_cumbytes_t)iab_rx_cumbytes_t203,7517
typedef ulong (*iab_rx_cumbytes_t)(iabtype iabtype203,7517
typedef ulong (*iab_inputs_t)iab_inputs_t204,7564
typedef ulong (*iab_inputs_t)(iabtype iabtype204,7564
typedef ulong (*iab_tx_cumbytes_t)iab_tx_cumbytes_t205,7606
typedef ulong (*iab_tx_cumbytes_t)(iabtype iabtype205,7606
typedef ulong (*iab_outputs_t)iab_outputs_t206,7653
typedef ulong (*iab_outputs_t)(iabtype iabtype206,7653
typedef ulong (*iab_oqueue_len_t)iab_oqueue_len_t207,7696
typedef ulong (*iab_oqueue_len_t)(iabtype iabtype207,7696
typedef struct iab_ifalias_ iab_ifalias_208,7742
} iab_ifalias_t;iab_ifalias_t211,7815
typedef boolean (*iab_update_ifalias_t)iab_update_ifalias_t212,7832
typedef boolean (*iab_update_ifalias_t)(iabtype *, iab_ifalias_t *, boolean)boolean212,7832
struct iabtype_ iabtype_214,7911
typedef void (*subiab_cleaner_t)subiab_cleaner_t301,11913
typedef void (*subiab_cleaner_t)(subiabtype subiabtype301,11913
struct subiabtype_ subiabtype_303,11962
#define MAX_SNMP_INTERFACES 335,13056
#define MAX_SNMPIDB_TYPES 337,13148
#define FOR_ALL_SNMPIDBS(FOR_ALL_SNMPIDBS342,13250
typedef boolean (*ProcTest)ProcTest349,13452
typedef boolean (*ProcTest)(hwidbtype* hwidb, long* indexP)indexP349,13452
typedef boolean (*ProcTestSwidb)ProcTestSwidb350,13513
typedef boolean (*ProcTestSwidb)(idbtype* swidb, long* indexP)indexP350,13513
typedef boolean (*ProcTestSnmp)ProcTestSnmp351,13577
typedef boolean (*ProcTestSnmp)(snmpidbtype *snmpidb, long* indexP)indexP351,13577
