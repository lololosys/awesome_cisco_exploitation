
vtp_dep.c,4098
#define LANE_TYPE_802_3 470,18316
#define LANE_TYPE_802_5 471,18371
#define VTP_DEFAULT_ELAN_NAME_LENGTH 492,18975
#define VTP_MAJVERSION 513,19767
#define VTP_MINVERSION 514,19794
#define VTP_EDITVERSION 515,19821
static void vtp_tftp_report_failure(518,19851
void vtp_tftp_main 533,20284
unsigned char vtp_load_database 615,22322
void vtp_swif_erase 638,22940
void vtp_swif_going_down 652,23181
void vtp_swif_coming_up 666,23494
void vtp_srt_port_state_change 680,23804
void vtp_srt_add_local_port 710,24593
void vtp_srt_remove_local_port 741,25508
boolean vtp_dep_is_pruned 771,26352
void vtp_sys_init 786,26615
void vtp_process_main 822,27707
void vtp_start_process 882,28970
void vtp_process_reschedule 901,29429
void vtp_process_teardown 913,29533
void* vtp_get_buffer 953,30417
void* vtp_buffer_duplicate 963,30557
void vtp_free_buffer 975,30729
void* vtp_buffer_payload 985,30860
boolean vtp_is_print 993,31006
void vtp_root_timer_init 1009,31222
vtp_timer *vtp_search_timer 1021,31473
void* vtp_create_timer 1043,31896
void vtp_delete_timer 1078,32915
void vtp_start_timer 1093,33203
void vtp_stop_timer 1103,33372
boolean vtp_timer_active 1113,33525
unsigned long vtp_timer_time_left 1118,33642
void vtp_process_timer 1125,33817
unsigned long vtp_get_system_uptime 1144,34287
void vtp_get_timestamp 1154,34438
boolean vtp_post_message 1181,35155
void vtp_post_configure_domain 1204,35592
void vtp_post_change_vlan_local_assoc(1222,36003
void vtp_post_change_domain_name 1240,36509
unsigned char vtp_intf_enable 1263,37253
unsigned char vtp_intf_disable 1294,38026
void vtp_post_configure_trunk 1315,38472
void vtp_post_change_trunk_link_state 1336,38928
vtp_post_change_trunk_vlan_stp_state 1355,39475
vtp_post_change_vlan_pruning_eligible 1376,40094
vtp_post_set_vlan_eligible_on_trunk 1391,40486
void vtp_post_configure_vlan 1410,40970
void vtp_post_download_config 1435,41625
void vtp_post_shutdown 1466,42332
void vtp_post_clear_statistics 1486,42655
void vtp_post_delete_database 1514,43220
void vtp_process_message 1539,43750
void vtp_enqueue 1652,47040
void vtp_process_packet_queue 1666,47249
boolean vtp_ether_macaddr 1728,48612
boolean vtp_fddi_macaddr 1746,49065
void vtp_macaddrs_update 1762,49383
void vtp_media_registry 1792,50326
int vtp_transmit_packet 1807,50898
void vtp_get_secret_value 1884,52822
void vtp_get_md5_digest 1923,53876
unsigned long vtp_get_random_number 1945,54323
Vtp_Notif_Err_t vtp_store_domain_info 1960,54590
void vtp_set_pruning_state_on_all_trunks 1997,55626
Vtp_Notif_Err_t vtp_domain_change_notification 2044,56989
static idbtype* vtp_configure_subif 2116,58807
static idbtype* vtp_get_subif 2250,62068
static idbtype* vtp_add_vlan_to_trunk 2285,62796
static idbtype* vtp_modify_vlan_in_trunk 2333,64341
static void vtp_delete_vlan_from_trunk 2416,66625
static int vtp_get_bridge_group 2431,67077
Vtp_Notif_Err_t vtp_vlan_change_notification 2454,67653
void vtp_error_notification 2531,69635
Vtp_Notif_Err_t vtp_trunk_change_notification 2540,69773
unsigned long vtp_format_4_bytes 2569,70457
unsigned short vtp_format_2_bytes 2584,70916
boolean vtp_cli_database_find 2605,71582
int vtp_cli_database_editing 2621,71980
int vtp_cli_database_init 2690,73981
void vtp_cli_database_abort 2735,75046
void vtp_cli_database_apply 2752,75321
void vtp_dep_add_trap_functions 2815,76956
void* vtp_dep_get_trunk_from_ifIndex 2839,77811
long vtp_dep_get_ifIndex_from_trunk 2859,78423
unsigned long vtp_dep_get_local_ipaddr 2882,79019
boolean vtp_dep_validate_managementDomainRowStatus 2901,79476
boolean vtp_dep_validate_vlanTrunkPortRowStatus 2916,79934
long vtp_dep_validate_vlanTrunkPortEncaps 2932,80464
long vtp_dep_get_notifications_enabled 2959,81247
void vtp_dep_notification_enabled 2970,81531
void vtp_no_request_response 2981,81731
vtp_dep_vlan_state_change_notification 2989,81794
boolean vtp_configure_isl_on_idb 3025,82747
void vtp_configure_sde_on_idb 3139,86539
idbtype *vtp_find_idb_for_isl 3243,89957
idbtype *vtp_find_sde_on_idb 3276,90635
