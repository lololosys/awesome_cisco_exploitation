
flash_services.c,2799
void run_from_flash_msg 206,7293
boolean flash_copy_file_name 254,9179
static boolean flash_copy_get_names 277,9795
void flash_create_path 299,10509
boolean flash_copy_get_server 307,10737
boolean flash_locate_and_open_file 346,12000
static void flash_copy_server_addr_init 378,13081
static void reloc_image_transfer_cleanup 387,13252
static int reloc_image_transfer_state 408,13708
static void reloc_image_relocation_offset 494,16254
static void reloc_image_set_sections 517,17028
static boolean reloc_image_cksum_init 538,17703
static boolean reloc_image_transfer_init 606,20038
static boolean reloc_image_next_reloc_entry 689,22528
static boolean reloc_image_prev_reloc_entry 714,23347
boolean reloc_image_next_reloc_offset 742,24211
static void reloc_image_relocation_offset_init 795,26026
static boolean reloc_image_relocation_init 812,26647
static ulong reloc_image_txt_cksum_write 857,28233
static ulong reloc_image_relocate_buffer 894,29502
static void reloc_image_calculate_cksum 939,30776
static ulong reloc_image_transfer_process 949,31097
static boolean reloc_image_header_write 1091,35991
static boolean net2flash_write 1158,37949
#define FLASH_MAX_DUMMY 1233,40312
static boolean flash_dummy_write 1237,40429
static void flash_decode_file_read_status 1260,41110
static boolean invoke_net_to_flash 1297,42135
void invoke_tftp_from_flash 1364,44568
static boolean flash_download_start 1411,46094
static ipsocktype * flash_upload_start 1495,48869
static void flash_copy_end 1581,51533
static void flash_copy_abnormal_terminate 1603,52294
boolean copy_tftp_flash 1651,53917
boolean copy_mop_flash 1736,56373
boolean copy_flash_tftp 1868,60596
boolean flash_file_checksum_verify 1914,61848
uint flash_size 1960,63154
uint flash_free_space 1969,63288
int flash_file_count 1978,63428
boolean flash_file_match 1999,63997
void flash_file_parse 2038,65317
boolean copy_rcp_flash 2061,65911
typedef struct flash2rcp_context_ flash2rcp_context_2142,68337
} flash2rcp_context;flash2rcp_context2146,68426
static long flash2rcp_callback 2148,68448
boolean copy_flash_rcp 2166,68914
boolean flash2rcp_remote 2246,71393
boolean write_file_to_flash 2295,72785
boolean flash_block_server 2373,74834
boolean multi_part_get_file_name 2426,76364
int flash_device_count 2465,77567
boolean flash_device_is_valid(2477,77812
long flash_partition_free_space 2497,78415
long flash_partition_filecount 2505,78571
long flash_partition_chksum_algo 2513,78728
long flash_partition_upgrade_method 2521,78937
long flash_partition_need_erasure 2529,79141
long flash_partition_filename_length 2537,79331
static void flash_services_init 2549,79562
#define FLASH_MAJVERSION 2559,79733
#define FLASH_MINVERSION 2560,79760
#define FLASH_EDITVERSION 2561,79787
