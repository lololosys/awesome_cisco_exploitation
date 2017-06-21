
vines_neighbor.h,1780
typedef struct vneigh_fn_data_ vneigh_fn_data_68,2492
} vneigh_fn_data;vneigh_fn_data75,2637
#define V_N_SELF	86,2854
#define V_N_STATIC	87,2878
#define V_N_DYNAMIC	88,2904
#define V_N_IGRP	89,2931
#define V_N_TEST	90,2955
#define V_N_REDIRECT	91,2979
#define V_N_PLACEHOLDER	92,3007
#define V_N_USE_ROUTE	93,3038
#define V_N_ME	95,3068
struct vinespathtype_ vinespathtype_102,3305
struct vinesneighbortype_ vinesneighbortype_154,5214
vneigh_EntryKey 187,5967
vneighCast 199,6209
vneigh_UnprotectNeighbor 206,6371
vneigh_ProtectNeighbor 216,6558
vneigh_NeighborProtected 226,6763
vneigh_GetNextNeighbor 235,6984
vneigh_GetFirstNeighbor 254,7391
vneigh_GetBestNeighbor 264,7599
vneigh_InsertEntry 274,7785
vneigh_DeleteEntry 286,8055
vneigh_ReinitNeighbor 297,8298
vneigh_CreateEntry 307,8477
vneigh_FindEntry 338,9210
vneigh_WalkTree 356,9576
vneigh_WalkPartialTree 367,9829
vneigh_FindSelf 377,10136
static inline boolean vines_servers_present 394,10542
static inline boolean vines_routers_present 399,10659
static inline boolean vines_rtp_present 407,10871
static inline boolean vines_srtp_present 414,11053
static inline boolean vines_anyone_present 420,11217
static inline boolean vines_any_servers_present 425,11348
static inline boolean vines_any_rtp_present 430,11458
static inline boolean vines_any_srtp_present 435,11542
#define FOR_ALL_PATHS(FOR_ALL_PATHS443,11686
#define FOR_ALL_PATHS_W_PREV(FOR_ALL_PATHS_W_PREV446,11805
static inline vinespathtype *vines_get_path 456,12123
static inline vinespathtype *vines_get_path_by_idb 474,12586
static inline vinespathtype *vines_get_path_by_flags 481,12768
static inline void vines_flush_all_paths_by_flags 488,12951
static inline void vines_flush_all_paths_by_idb 493,13063
