
rsp_flash.c,1354
#define FLASH_CARD0_NAME_STR 165,5937
#define FLASH_CARD1_NAME_STR 166,5999
#define FLASH_SIMM_NAME_STR 167,6061
#define DEFAULT_DEV_ID	172,6148
typedef struct flash_chip_rec_flash_chip_rec_174,6180
} flash_chip_rec_t;flash_chip_rec_t183,6490
#define	CIS_ACTUAL_SIZE	198,6856
#define	CIS_MFG_ID_OFFSET	199,6916
#define	CIS_MFG_ID_LEN	200,6981
typedef struct format_dev_rec_format_dev_rec_202,7036
} format_dev_rec_t;format_dev_rec_t210,7336
boolean get_flash_device_info 241,8037
flash_print_hardware 267,8626
ushort add_to_cksum 305,9519
ushort update_cksum 322,9980
static boolean flash_alias_match 337,10299
static int read_dev_cis 349,10506
static flash_chip_rec_t *find_flash_chip 393,11442
static int set_flash_read_mode 407,11730
static int read_flash_mfg_dev_id 446,12702
static int identify_flash_chip 514,14437
static int analyze_dev_cis 585,16061
static int flash_simm_size 660,17836
int analyze_flash_device 705,18753
int rsp_internal_flash_show_info_fn 758,20277
#define	ALTERNATE	885,23757
#define	ALTERNATE	893,23925
#define	ALTERNATE	901,24104
#define	ALTERNATE	906,24226
static void rsp_flash_parser_init 926,24789
static char *default_flash_device 946,25363
static void flash_init 951,25433
#define FLASH_MAJVERSION	972,25958
#define FLASH_MINVERSION	973,25985
#define FLASH_EDITVERSION	974,26012
