
vsvc_st.c,4197
#define VST_MIN_VERSION 143,4874
vst_GroupKey 224,7299
vst_GroupCast 253,7943
vst_UnprotectGroup 260,8096
vst_ProtectGroup 270,8265
vst_GroupProtected 280,8452
vst_GetNextGroup 289,8641
vst_GetFirstGroup 308,9010
vst_GetBestGroup 319,9212
vst_FindGroup 336,9768
vst_InsertGroup 376,10732
vst_ReinitGroup 407,11491
vst_CreateGroup 417,11648
vst_DeleteGroup 446,12361
vst_WalkGroupTree 461,12684
vst_WalkPartialGroupTree 472,12921
boolean vines_is_st_welcome 495,13414
static inline void vst_set_master_timer 516,13848
ushort vsvc_break_name 532,14482
static inline void vst_start_search 590,15867
static inline void vst_stop_search 605,16263
static inline void vst_bump_reinforce 627,16841
vst_is_new_reinforce 645,17333
vst_compare_ids 680,18533
static char *vst_freshness_tag 725,19600
static inline boolean vst_new_info 745,20140
static boolean vst_flush_old_group 758,20593
static ushort vst_priority2misc 779,21016
static ushort vst_misc2priority 792,21299
char *vst_find_server_by_address 815,21947
ulong vst_find_server_by_name 836,22453
static inline void vst_set_handle_timer 869,23210
static vst_handle *vst_find_handle 881,23492
static vst_handle *vst_find_valid_handle 903,24094
static void *vst_setup_reply 933,25029
void vst_set_gen_dest 1001,26806
vst_clear_gen_dest 1062,28210
static void vst_propagate 1085,28799
static void vst_send_delayed_msgs 1116,29657
static void vst_gen_delayed_msg 1154,30759
ushort vst_lookup_rcv_call 1179,31310
static void vst_lookup_finished 1215,32205
ushort vst_logout_rcv_call 1255,33202
ushort vst_getports_rcv_call 1282,33829
static void vst_getports_finished 1318,34772
ushort vst_begin_rcv_call 1362,35951
void vst_user_validated 1407,37306
void vst_cold_dump_rcv_reply 1452,38487
static boolean vst_cold_dump_insert_group 1566,41944
ushort vst_cold_dump_rcv_call 1622,43823
void vst_cold_dump_send_call 1659,44934
static boolean vst_rcv_ni_summary 1708,46310
void vst_send_ni_summary 1798,49066
static void vst_detail_extract_group 1846,50233
static boolean vst_rcv_ni_detail 1898,51616
static boolean vst_detail_insert_group 1983,54111
void vst_send_ni_detail 2021,55190
static boolean vst_rcv_ni_need 2083,56987
static boolean vst_need_insert_server 2166,59336
void vst_send_ni_need 2208,60504
static boolean vst_welcome_insert_server 2264,61787
void vst_send_ni_welcome 2313,63334
ushort vst_ni_rcv_call 2356,64401
ushort vst_rtndetail_rcv_call 2423,66280
void vst_rtndetail_rcv_reply 2511,68995
static void vst_rtndetail_send_call 2527,69376
ushort vst_getdetail_rcv_call 2592,71224
static inline void vst_set_operation_timer 2646,72642
static ushort vst_op_do_lookup 2657,72922
static ushort vst_op_do_enumerate 2712,74669
static ushort vst_op_do_membership 2729,75158
static ushort vst_op_do_nicename 2744,75412
ushort vst_operate_rcv_call 2762,75924
static ushort vst_operate_send_call 2817,77546
static void vst_operate_done 2917,80642
void vst_operate_rcv_reply 2973,81841
void vst_operate_rcv_abort 2985,82057
void vst_rcv_search 3003,82434
void vst_send_search 3019,82687
void vst_rcv_returnaddress 3034,83072
void vst_create_local_group 3067,83853
void vst_delete_local_group 3131,85656
static boolean vst_delete_one_group 3165,86327
static void vst_init_serverentry 3184,86890
static void vst_insert_self 3231,88369
static boolean vst_invalidate_entry 3243,88695
static void vst_delete_self 3264,89220
static inline void vst_display_forward_queue 3296,90131
static void vst_display_handles 3318,90759
static void vst_display_operations 3342,91446
static void vst_display_timer 3366,92058
static void vst_display_time_dest 3381,92432
static boolean vst_print_server_node 3405,93044
static boolean vst_print_group_node 3432,93790
void vst_display_group_tree 3455,94514
void vst_new_neighbor 3476,94990
void vst_hostname_changed 3496,95454
static boolean vst_age_one_group 3518,96011
static void vst_age_groups 3539,96520
void vst_periodic 3549,96757
boolean vst_age_service_info 3642,99465
void vst_command 3666,100020
void vst_init 3706,100791
void vst_start 3740,101638
void vst_stop 3777,102583
void vst_display 3833,104039
