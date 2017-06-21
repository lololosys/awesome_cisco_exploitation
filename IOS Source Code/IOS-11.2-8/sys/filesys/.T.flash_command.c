
flash_command.c,4582
#define	COPY_FILE_RCONFIG_SCONFIG	192,6571
#define	COPY_FILE_RCONFIG_ID	193,6607
#define	COPY_FILE_SCONFIG_RCONFIG	194,6639
#define	COPY_FILE_SCONFIG_ID	195,6675
#define	COPY_FILE_ID_RCONFIG	196,6707
#define	COPY_FILE_ID_SCONFIG	197,6739
#define	COPY_FILE_ID_ID	198,6771
#define	COPY_FILE_ID_TFTP	199,6799
#define	COPY_FILE_ID_RCP	200,6828
#define	COPY_FILE_ID_FLASH	201,6856
#define TFTP_DEV_NAME	203,6888
#define RCP_DEV_NAME	204,6917
#define NV_DEV_NAME	205,6944
#define FLASH_DEV_NAME	207,6991
#define NV_FILE_ID 214,7193
#define NV_PRIVATE_FILE_ID 215,7228
#define NV_MATCH_PRIVATE_FILE_ID(NV_MATCH_PRIVATE_FILE_ID216,7271
#define	COPY_FLASH_FILE_ID	222,7472
#define	COPY_TFTP_FILE_ID	223,7503
#define	COPY_RCP_FILE_ID	224,7533
#define	COPY_MOP_FILE_ID	225,7562
#define	STARTUP_CONFIG	227,7592
#define	RUNNING_CONFIG	228,7632
#define	MAX_CONFIG_FILE_ID	233,7711
typedef struct df_dev_rec_df_dev_rec_235,7743
} df_dev_rec_t;df_dev_rec_t240,7857
static boolean is_flash_device 273,9102
static boolean is_nv_device 282,9333
static int get_file_size 291,9558
static int get_file_type 304,9783
static int config_buffer_to_startup 318,10034
static const char *error_code2text 328,10234
static const char *get_device_names(336,10432
static boolean config_file_not_exist 348,10761
static const char * get_busy_string 380,11554
#define CONTROL_C 397,11928
void show_file_command 401,11990
void build_whole_file_id 496,14316
static boolean convert_key_to_file_id 541,15762
void copy_file_command 612,17522
static void copy_file_id_to_file_id 971,26551
void rsp_flash_copy_command 984,26886
void delete_command 1109,29694
static df_dev_rec_t *get_df_dev_entry 1155,30808
static void put_df_dev_entry 1169,31053
static boolean rem_df_dev_entry 1178,31216
const char *get_default_dev_id 1200,31590
void set_default_dev_id 1224,32207
static void df_dev_clean 1250,32735
void undelete_command 1259,32871
static void list_device_files 1286,33405
void dir_command 1413,36363
static boolean check_device_id_colon 1434,36833
void squeeze_command 1454,37190
void format_command 1483,37819
void cd_command 1550,39312
void pwd_command 1575,39750
void boot_config_file_command 1583,39881
void boot_helper_file_command 1680,42098
void show_boot_command 1749,43572
void verify_file_command 1787,44544
boolean rsp_flash_tftp_server 1827,45442
#define	DEVICE_LIST_LEN	1863,46315
static int list_all_devices 1868,46384
void show_flash_file_command 1904,47019
char *current_config_to_buf 1991,49272
char *startup_config_to_buf 2016,49711
char *startup_private_config_to_buf 2067,50781
char *get_config_file_id 2074,50917
static char *fetch_config_file_id 2096,51399
#define	MEM_ARRAY_CNT	2115,51742
#define	MEM_BLOCK_SIZE	2116,51767
char *read_file_to_buf 2122,51880
char *config_file_to_buf 2235,54300
static boolean is_startup_config_file 2302,55943
static boolean check_device_space 2320,56343
static char *IP_header_getword 2360,57216
static int IP_header_parse 2375,57410
static boolean is_IP_multi_segment_file 2391,57942
int copy_file_to_file 2433,58913
int buf_to_current_config 2829,68280
int buf_to_startup_config 2839,68521
int write_buffer_to_file 2928,70726
static boolean is_config_file 2999,72537
static boolean is_image_file 3024,73013
boolean file_protection_check 3043,73382
static char *find_first_file 3129,75445
boolean device_protection_check 3179,76529
boolean check_file_exist 3293,79517
char *trunc_image_id_from_list 3308,79791
boolean match_filename 3332,80196
#define	ALTERNATE	3367,80889
#define	ALTERNATE	3376,81062
#define	ALTERNATE	3385,81247
#define	ALTERNATE	3394,81425
#define	ALTERNATE	3403,81600
#define	ALTERNATE	3412,81772
#define	ALTERNATE	3421,81942
#define	ALTERNATE	3429,82108
#define	ALTERNATE	3438,82281
#define	ALTERNATE	3447,82469
#define	ALTERNATE	3456,82615
#define	ALTERNATE	3465,82773
#define ALTERNATE 3473,82946
#define	ALTERNATE	3481,83114
static void file_parser_init 3523,84608
int get_whole_file_id 3533,84830
int construct_whole_file_id 3571,85791
static boolean check_flash_file_name 3605,86700
static boolean rsp_flash_file_system 3616,86891
void rsp_flash_file_parse 3621,86957
char *rsp_flash_first_bootable_file 3647,87740
boolean rsp_file_boot_in_place 3690,88558
int rsp_flash_file_read 3737,89832
boolean rsp_flash_file_exists 3775,90622
boolean rsp_flash_block_server 3782,90786
void rsp_flash_file_source 3837,91795
int flash_file_length 3850,92022
int flash_read_file 3866,92288
void init_flash_command 3934,93561
