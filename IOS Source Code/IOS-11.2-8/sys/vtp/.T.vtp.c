
vtp.c,2577
#define VTP_FACTORY_DEFAULT_ETHERNET_VLAN_NAME_LENGTH 587,21611
#define VTP_FACTORY_DEFAULT_FDDI_VLAN_NAME_LENGTH 588,21671
#define VTP_FACTORY_DEFAULT_TR_VLAN_NAME_LENGTH 589,21731
#define VTP_FACTORY_DEFAULT_FDDINET_VLAN_NAME_LENGTH 590,21791
#define VTP_FACTORY_DEFAULT_TRNET_VLAN_NAME_LENGTH 591,21851
static void change_to_transparent 601,22077
void change_to_client 613,22467
vtp_boolean vtp_rev_less 627,22867
void vtp_init 640,23247
void vtp_add_default_vlans 678,24414
void vtp_flush_update_adv_info 799,28971
void vtp_flush_tx_subset_adv_queue 839,30285
void vtp_receive_packet 865,30723
void vtp_receive_summary_adv_packet 933,32665
void vtp_receive_subset_adv_packet 1119,39434
void vtp_receive_adv_request_packet 1275,44659
vtp_boolean vtp_validate_subset_adv 1369,47390
void vtp_allocate_update_memory 1425,48964
void vtp_record_subset_adv 1458,49976
void vtp_mark_database_old 1508,51590
unsigned char vtp_delete_old_vlans 1524,52009
void vtp_cancel_transition 1549,52586
void vtp_process_subset_adv 1582,53827
void vtp_transmit_adv_request_packet 1754,59726
void vtp_do_trunk_flooding 1826,61894
void vtp_transmit_summary_adv_packet 1891,63877
vtp_boolean vtp_add_vlan_to_subset 1957,66275
vtp_generate_one_subset 2062,69703
void vtp_cancel_subset_transmission 2109,71137
void vtp_generate_subset_adv_packet 2129,71917
void vtp_transmit_subset_adv_packet 2173,73208
void vtp_config_updating_timeout 2204,73983
void vtp_trunk_link_flood_timeout 2238,74923
void vtp_trunk_link_rqst_timeout 2259,75442
void vtp_report_load_failure 2290,76393
unsigned char vtp_check_domain_info 2309,76952
Vtp_Err_t vtp_set_update_id 2355,78252
void vtp_update_domain_info 2372,78673
Vtp_Err_t vtp_configure_domain 2426,80576
Vtp_Err_t vtp_change_domain_name 2635,86620
unsigned char vtp_precheck_vlan 2721,89079
static unsigned char vtp_verify_vlan_values 2811,91626
unsigned char vtp_test_vlan 2922,94978
void vtp_update_vlan_info 3007,97448
unsigned char vtp_configure_vlan_in_database 3045,99171
vtp_automate_translation_info 3110,101087
static void vtp_save_translation_values 3138,102025
static void vtp_restore_translation_values 3154,102487
Vtp_Err_t vtp_configure_vlan 3170,102866
void vtp_do_flooding 3289,106231
void vtp_update_revision_number(3313,106954
void vtp_create_new_revision 3336,107522
vtp_boolean vtp_validate_md5_digest 3373,108570
void vtp_generate_md5_digest 3438,110792
unsigned long vtp_get_random_jittered_timer_value 3590,115592
void vtp_relay_packet 3605,115875
void vtp_terminate 3633,116563
