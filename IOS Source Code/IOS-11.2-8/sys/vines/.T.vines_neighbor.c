
vines_neighbor.c,2451
void vines_neighbor_init 156,5607
static void vines_set_path_flags 178,6211
static void vines_set_neighbor_flags 206,6956
static void vines_compute_neighbor_metric 246,8094
static void vines_build_path_list 284,9298
void vines_fixup_neighbor 314,9986
boolean vines_check_path_list 333,10563
boolean vines_verify_neighbor_structure 369,11711
ulong vneigh_get_path_count 409,12836
ulong vneigh_get_neighbor_count 414,12901
static void vneigh_inc_type_counts 432,13304
static void vines_dec_type_counts 481,14211
void vines_insert_new_path 533,15366
static inline void vines_init_dqueue(563,16382
vinespathtype *vines_make_new_path 569,16529
static void vines_set_path_timestamp 595,17280
vinespathtype *vines_add_path 613,17855
static inline void vines_unlink_path 696,20103
vinesneighbortype *vines_delete_path 733,21201
vinesneighbortype *vines_flush_paths_by_flag 777,22361
void vines_flush_dynamic_paths 827,23600
static boolean vines_flush_one_entry 843,24078
void vines_flush_all_paths_by_xxx 887,25189
void vines_flush_static_paths 914,25886
static boolean vines_poison_one_entry 944,26783
void vines_poison_all_paths_by_idb 986,27922
vinesneighbortype *vines_add_neighbor 1010,28618
void vines_delete_neighbor_entry 1049,29474
void vines_delete_neighbor 1080,30230
static void vines_dump_neighbor 1094,30557
vinespathtype *vines_get_path_by_xxx 1127,31618
boolean vines_best_path_uses_idb 1173,32793
boolean vines_best_path_from_delta_or_snapshot 1198,33416
int vines_get_neighbor_metric 1224,34091
int vines_get_slowest_update_interval 1239,34532
boolean vines_neighbor_update 1265,35252
void vines_route_adjust 1417,40447
#define VNEIGH_FLAG_FLUSH 1452,41426
#define VNEIGH_FLAG_CHECK_REINIT 1453,41464
#define VNEIGH_FLAG_SEND_REINIT 1454,41502
static boolean vneigh_age_entry 1469,42100
void vines_neighbor_ager 1566,45023
void vneigh_ShowStats 1598,45982
static char vines_neighbortype 1622,46686
static void vines_display_paths 1647,47207
static boolean vneigh_display_entry 1736,49403
void vines_display_neighbors 1784,50606
static void vines_display_paths_on_idb 1809,51445
void vneigh_display_interface 1849,52450
int vines_get_enctype 1864,52995
uchar *vines_get_mac_address 1871,53113
ulong vines_get_net 1878,53244
ushort vines_get_host 1883,53333
void vines_new_neighbor 1895,53633
static boolean vines_start_aging_one_entry 1930,54662
void vines_start_aging_paths 1975,55846
