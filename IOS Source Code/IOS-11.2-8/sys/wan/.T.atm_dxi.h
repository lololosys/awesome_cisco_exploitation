
atm_dxi.h,2271
#define __ATM_DXI_H__54,1961
# define ATM_DXI_SNAPENCAPBYTES 62,2115
# define ATM_DXI_BRIDGE_ENCAPBYTES 63,2151
# define ATM_DXI_MUXENCAPBYTES 64,2190
# define ATM_DXI_ENCAPBYTES 65,2225
# define DXI_SNAP_NLPID 66,2257
# define DXI_SNAP_CONTROL 67,2301
# define DXI_NON_ISO_LLC_SNAP	68,2343
# define DXI_ISO_LLC	69,2381
# define ATM_DXI_ISOPDU_HDRBYTES 70,2411
# define ADDRLEN_DXI	71,2453
# define ATM_DXI_SNAP1PAD 72,2478
# define ATM_DXI_SNAP0PAD 73,2516
# define ATM_DXI_EA_BITS 74,2554
# define DFA_DELETED 75,2588
# define DFA_ACTIVE 76,2614
# define IS_DFA_ACTIVE(IS_DFA_ACTIVE77,2639
# define IS_DFA_DELETED(IS_DFA_DELETED78,2674
# define RELAYHASH 79,2711
# define MIN_DFA 80,2736
# define MAX_DFA 81,2758
# define MAXDFA_VALUES	82,2783
# define DXI_PVC_LOCAL	83,2817
# define DXI_PVC_STATIC	84,2844
# define DXI_1483_ETHER_BRIDGE 85,2872
# define DXI_IEEE_DEC_BPDU 86,2919
# define NLPID_DEF(NLPID_DEF88,2967
typedef struct atm_dxi_hdr_ atm_dxi_hdr_91,3045
} atm_dxi_hdr;atm_dxi_hdr96,3261
typedef struct atm_dxi_llc_header_ atm_dxi_llc_header_98,3277
} atm_dxi_llc_hdr;atm_dxi_llc_hdr109,3890
typedef struct atm_dxi_bridge_hdr_ atm_dxi_bridge_hdr_111,3910
} atm_dxi_bridge_hdr;atm_dxi_bridge_hdr116,4152
enum DXI_MAP_TYPE DXI_MAP_TYPE118,4175
    DXI_MAP_SNAP 119,4195
    DXI_MAP_NLPID,120,4217
    DXI_MAP_MUX 121,4236
typedef struct dxi_pvc_stats_type_ dxi_pvc_stats_type_124,4258
} dxi_pvc_stats_type;dxi_pvc_stats_type134,4898
typedef struct atm_dxi_map_ atm_dxi_map_136,4921
} dximaptype;dximaptype145,5312
struct dxi_pvc_info_type_ dxi_pvc_info_type_147,5327
struct dxi_idb_struct_type_ dxi_idb_struct_type_162,6002
#define NUM_TO_DFA(NUM_TO_DFA176,6484
#define DFA_TO_NUM(DFA_TO_NUM177,6550
#define BR_NUM_TO_DFA(BR_NUM_TO_DFA178,6614
#define BR_DFA_TO_NUM(BR_DFA_TO_NUM179,6685
#define HWIDB_TO_DXI_PVC(HWIDB_TO_DXI_PVC181,6756
static inline ulong dxi_pvc_input_pkts 182,6831
static inline ulong dxi_pvc_output_pkts 187,6967
static inline ulong dxi_pvc_input_bytes 192,7106
static inline ulong dxi_pvc_output_bytes 197,7241
static inline void clear_dxi_pvc_stats 202,7379
static inline void INC_DXI_PVC_FAST_INPUT 207,7496
static inline void INC_DXI_PVC_FAST_OUTPUT 214,7680
