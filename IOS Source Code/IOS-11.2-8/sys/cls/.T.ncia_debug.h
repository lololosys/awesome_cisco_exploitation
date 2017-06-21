
ncia_debug.h,1742
#define _NCIA_DEBUG_H_46,1465
enum ncia_show_ ncia_show_48,1489
    NCIA_SHOW_ALL,49,1507
    NCIA_SHOW_SERVER,50,1526
    NCIA_SHOW_CLIENT,51,1548
    NCIA_SHOW_CIRCUIT52,1570
enum ncia_debug_client_ ncia_debug_client_55,1596
    NCIA_DEBUG_CLIENT_ALL,56,1622
    NCIA_DEBUG_CLIENT_MSGS,57,1649
    NCIA_DEBUG_CLIENT_EVENT,58,1677
    NCIA_DEBUG_CLIENT_ERROR59,1706
enum ncia_circuit_debug_ ncia_circuit_debug_63,1739
    NCIA_DEBUG_CIRCUIT_ALL,64,1766
    NCIA_DEBUG_CIRCUIT_FLOW,65,1794
    NCIA_DEBUG_CIRCUIT_XID,66,1823
    NCIA_DEBUG_CIRCUIT_EVENT,67,1851
    NCIA_DEBUG_CIRCUIT_STATE,68,1881
    NCIA_DEBUG_CIRCUIT_ERROR69,1911
#define NCIA_CLEAR_CIRCUIT	76,1984
#define NCIA_CLEAR_ALL_CIRCUITS	77,2013
#define NCIA_CLEAR_CLIENT	78,2047
#define NCIA_CLEAR_ALL_CLIENTS	79,2075
#define NCIA_DEBUG_ALL_CLIENTS 85,2154
#define NCIA_DEBUG_ALL_CIRCUITS	86,2189
#define NCIA_DEBUG_ALL_SERVERS	87,2223
#define NCIA_SHOW_ALL_CLIENTS 89,2258
#define NCIA_SHOW_ALL_CIRCUITS	90,2292
#define NCIA_SHOW_ALL_SERVERS	91,2325
#define NCIA_TYPE_STRING_SIZE 93,2359
#define NciaCircuitDebugEvent	95,2393
#define NciaCircuitDebugFlow	97,2489
#define NciaCircuitDebugError	99,2583
#define NciaCircuitDebugState	101,2679
#define NciaClientDebugEvent(NciaClientDebugEvent104,2776
#define NciaClientDebugMsgs(NciaClientDebugMsgs106,2881
#define NciaClientDebugError(NciaClientDebugError108,2985
#define NciaSetClientDebug(NciaSetClientDebug111,3091
#define NciaUnsetClientDebug(NciaUnsetClientDebug112,3170
#define NciaClientDebugOn(NciaClientDebugOn114,3262
#define __DECLARE_DEBUG_NUMS__124,3592
#define NciaServerDebugAll	127,3661
#define NciaClientDebugAll	128,3718
#define NciaCircuitDebugAll	129,3775
