
c7100_flash.c,1364
#define FLASH_CARD0_NAME_STR 123,3916
#define FLASH_CARD1_NAME_STR 124,3978
#define FLASH_SIMM_NAME_STR 125,4040
#define DEFAULT_DEV_ID	130,4127
typedef struct flash_chip_rec_flash_chip_rec_132,4159
} flash_chip_rec_t;flash_chip_rec_t141,4469
#define	CIS_ACTUAL_SIZE	156,4835
#define	CIS_MFG_ID_OFFSET	157,4895
#define	CIS_MFG_ID_LEN	158,4960
typedef struct format_dev_rec_format_dev_rec_160,5015
} format_dev_rec_t;format_dev_rec_t168,5315
boolean get_flash_device_info 199,5997
static void flash_print_hardware 227,6606
ushort add_to_cksum 256,7326
ushort update_cksum 272,7786
static boolean flash_alias_match 287,8105
static int read_dev_cis 299,8312
static flash_chip_rec_t *find_flash_chip 343,9249
static int set_flash_read_mode 357,9537
static int read_flash_mfg_dev_id 396,10509
static int identify_flash_chip 464,12246
static int analyze_dev_cis 535,13882
static int flash_simm_size 611,15679
int analyze_flash_device 651,16611
#define	ALTERNATE	695,17777
#define	ALTERNATE	703,17945
#define	ALTERNATE	711,18135
#define	ALTERNATE	716,18257
static void c7100_flash_parser_init 736,18826
static char *default_flash_device 756,19402
static void flash_init 761,19475
int rsp_internal_flash_show_info_fn 792,20374
#define FLASH_MAJVERSION	910,23505
#define FLASH_MINVERSION	911,23532
#define FLASH_EDITVERSION	912,23559
